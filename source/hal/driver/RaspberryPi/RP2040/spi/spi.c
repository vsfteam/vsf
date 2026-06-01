/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_SPI == ENABLED

#include "hal/vsf_hal.h"

/* for IRQn_Type, NVIC_EnableIRQ, SPI0_IRQ_IRQn via vendor_driver.h -> device.h -> RP2040.h */
#include "hal/driver/vendor_driver.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_SPI_CFG_MULTI_CLASS
#   define VSF_HW_SPI_CFG_MULTI_CLASS              VSF_SPI_CFG_MULTI_CLASS
#endif

#define VSF_SPI_CFG_IMP_PREFIX                     vsf_hw
#define VSF_SPI_CFG_IMP_UPCASE_PREFIX              VSF_HW

/* PL022 register helpers */
#define __PL022_CR0_DSS(__BITS)                    (((__BITS) - 1) & 0x0F)
#define __PL022_CR0_SPO                            (1u << 6)
#define __PL022_CR0_SPH                            (1u << 7)
#define __PL022_CR1_SSE                            (1u << 1)
#define __PL022_CR1_MS                             (1u << 2)
#define __PL022_SR_BSY                             (1u << 4)
#define __PL022_SR_RNE                             (1u << 2)
#define __PL022_SR_TNF                             (1u << 1)

/*============================ TYPES =========================================*/

typedef struct VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) {
#if VSF_HW_SPI_CFG_MULTI_CLASS == ENABLED
    vsf_spi_t               vsf_spi;
#endif
    spi_hw_t                *reg;
    uint32_t                rst_bit;
    IRQn_Type               irqn;
    vsf_spi_isr_t           isr;
    vsf_spi_cfg_t           cfg;
    uint32_t                transferred;
    struct {
        uint32_t            is_enabled : 1;
        uint32_t            is_busy    : 1;
        uint32_t                       : 30;
    } status;
    /* Async transfer state */
    uint8_t                 *tx_buf;
    uint8_t                 *rx_buf;
    uint_fast32_t           count;
    uint_fast32_t           tx_offset;
    uint_fast32_t           rx_offset;
} VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __rp2040_spi_reset(uint32_t rst_bit)
{
    resets_hw->reset |= rst_bit;
    resets_hw->reset &= ~rst_bit;
    // spin-wait: wait for reset to de-assert
    while (!(resets_hw->reset_done & rst_bit));
}

static uint32_t __rp2040_spi_compute_prescale(uint32_t clock_hz, uint32_t *cpsr, uint32_t *scr)
{
    uint32_t sys_clk = clock_get_hz(clk_sys);
    if (clock_hz == 0 || clock_hz > sys_clk) {
        return 0;
    }

    /* Find smallest even CPSR [2,254] and SCR [0,255] such that
     * clock = sys_clk / (CPSR * (1 + SCR))
     */
    for (uint32_t p = 2; p <= 254; p += 2) {
        uint32_t div = sys_clk / (p * clock_hz);
        if (div == 0) {
            div = 1;
        }
        uint32_t s = div - 1;
        if (s > 255) {
            continue;
        }
        uint32_t actual = sys_clk / (p * (1 + s));
        if (actual <= clock_hz) {
            *cpsr = p;
            *scr = s;
            return actual;
        }
    }
    return 0;
}

vsf_err_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_init)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    vsf_spi_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((NULL != spi_ptr) && (NULL != cfg_ptr));

    spi_hw_t *reg = spi_ptr->reg;
    __rp2040_spi_reset(spi_ptr->rst_bit);

    spi_ptr->cfg = *cfg_ptr;
    spi_ptr->isr = cfg_ptr->isr;
    spi_ptr->status.is_enabled = false;
    spi_ptr->status.is_busy    = false;
    spi_ptr->transferred       = 0;

    /* Disable SPI during config */
    reg->cr1 = 0;

    bool is_slave = (cfg_ptr->mode & VSF_SPI_SLAVE) != 0;

    uint32_t cr0 = 0;

    /* Data size: support 4-16 bits. Default 8 if not specified. */
    uint8_t data_bits = vsf_spi_mode_to_data_bits(cfg_ptr->mode);
    if (data_bits < 4 || data_bits > 16) {
        return VSF_ERR_NOT_SUPPORT;
    }
    cr0 |= __PL022_CR0_DSS(data_bits);

    /* Motorola frame format */
    /* FRF bits [5:4] = 00 for Motorola */

    /* CPOL/CPHA — mode bits directly encode PL022 CR0 SPO (bit 6) / SPH (bit 7) */
    cr0 |= (cfg_ptr->mode & (__PL022_CR0_SPO | __PL022_CR0_SPH));

    if (is_slave) {
        /* Slave mode: external clock, no prescale needed */
        reg->cpsr = 2;
        reg->cr0  = cr0;
        reg->cr1  = __PL022_CR1_MS;  /* Slave mode */
    } else {
        uint32_t cpsr = 2, scr = 0;
        uint32_t actual_clock = __rp2040_spi_compute_prescale(cfg_ptr->clock_hz, &cpsr, &scr);
        if (actual_clock == 0) {
            return VSF_ERR_NOT_SUPPORT;
        }

        /* SCR */
        cr0 |= (scr << 8);

        reg->cpsr = cpsr;
        reg->cr0  = cr0;
        /* Master mode */
        reg->cr1 = 0;
    }

    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_fini)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);

    spi_hw_t *reg = spi_ptr->reg;
    reg->cr1 = 0;
    reg->imsc = 0;
    spi_ptr->status.is_enabled = false;
    spi_ptr->status.is_busy    = false;
}

fsm_rt_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_enable)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);

    spi_hw_t *reg = spi_ptr->reg;
    reg->cr1 |= __PL022_CR1_SSE;
    spi_ptr->status.is_enabled = true;

    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_disable)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);

    spi_hw_t *reg = spi_ptr->reg;
    reg->cr1 &= ~__PL022_CR1_SSE;
    spi_ptr->status.is_enabled = false;

    return fsm_rt_cpl;
}

vsf_err_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_cs_active)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    uint_fast8_t index)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_UNUSED_PARAM(index);
    /* Software CS only — caller manages GPIO */
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_cs_inactive)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    uint_fast8_t index)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_UNUSED_PARAM(index);
    /* Software CS only — caller manages GPIO */
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_fifo_transfer)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    void *out_buffer_ptr, uint_fast32_t *out_offset_ptr,
    void *in_buffer_ptr, uint_fast32_t *in_offset_ptr,
    uint_fast32_t cnt)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);

    spi_hw_t *reg = spi_ptr->reg;
    uint_fast32_t out_offset = out_offset_ptr ? *out_offset_ptr : 0;
    uint_fast32_t in_offset  = in_offset_ptr  ? *in_offset_ptr  : 0;
    uint_fast32_t transferred = 0;

    while (transferred < cnt) {
        /* Wait for TX FIFO not full */
        while (!(reg->sr & __PL022_SR_TNF));

        uint16_t tx_data = 0;
        if (out_buffer_ptr != NULL) {
            tx_data = ((uint8_t *)out_buffer_ptr)[out_offset++];
        }
        reg->dr = tx_data;

        /* Wait for RX FIFO not empty */
        while (!(reg->sr & __PL022_SR_RNE));

        uint16_t rx_data = (uint16_t)reg->dr;
        if (in_buffer_ptr != NULL) {
            ((uint8_t *)in_buffer_ptr)[in_offset++] = (uint8_t)rx_data;
        }

        transferred++;
    }

    if (out_offset_ptr != NULL) {
        *out_offset_ptr = out_offset;
    }
    if (in_offset_ptr != NULL) {
        *in_offset_ptr = in_offset;
    }
}

vsf_err_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_request_transfer)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    void *out_buffer_ptr, void *in_buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);

    if (count == 0) {
        return VSF_ERR_INVALID_PARAMETER;
    }
    if (spi_ptr->status.is_busy) {
        return VSF_ERR_NOT_READY;
    }

    spi_ptr->tx_buf    = out_buffer_ptr;
    spi_ptr->rx_buf    = in_buffer_ptr;
    spi_ptr->count     = count;
    spi_ptr->tx_offset = 0;
    spi_ptr->rx_offset = 0;
    spi_ptr->transferred = 0;
    spi_ptr->status.is_busy = true;

    spi_hw_t *reg = spi_ptr->reg;

    /* Pre-fill TX FIFO (up to 8 entries) */
    while ((reg->sr & __PL022_SR_TNF) && (spi_ptr->tx_offset < spi_ptr->count)) {
        uint16_t tx_data = 0;
        if (spi_ptr->tx_buf != NULL) {
            tx_data = spi_ptr->tx_buf[spi_ptr->tx_offset];
        }
        reg->dr = tx_data;
        spi_ptr->tx_offset++;
    }

    /* Enable TXIM + RXIM interrupts and NVIC */
    reg->imsc |= SPI_SSPIMSC_TXIM_BITS | SPI_SSPIMSC_RXIM_BITS;
    NVIC_EnableIRQ(spi_ptr->irqn);

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_cancel_transfer)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);

    spi_hw_t *reg = spi_ptr->reg;

    /* Disable interrupts */
    reg->imsc &= ~(SPI_SSPIMSC_TXIM_BITS | SPI_SSPIMSC_RXIM_BITS);

    /* Drain RX FIFO to clear any pending data */
    while (reg->sr & __PL022_SR_RNE) {
        (void)reg->dr;
    }

    /* Reset async state */
    spi_ptr->status.is_busy = false;
    spi_ptr->tx_buf = NULL;
    spi_ptr->rx_buf = NULL;
    spi_ptr->count = 0;
    spi_ptr->tx_offset = 0;
    spi_ptr->rx_offset = 0;
    spi_ptr->transferred = 0;

    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_get_transferred_count)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    uint_fast32_t *tx_count, uint_fast32_t *rx_count)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    if (tx_count != NULL) {
        *tx_count = spi_ptr->transferred;
    }
    if (rx_count != NULL) {
        *rx_count = spi_ptr->transferred;
    }
}

void VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_irq_enable)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    vsf_spi_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);

    spi_hw_t *reg = spi_ptr->reg;
    if (irq_mask & VSF_SPI_IRQ_MASK_TX) {
        reg->imsc |= SPI_SSPIMSC_TXIM_BITS;
    }
    if (irq_mask & VSF_SPI_IRQ_MASK_RX) {
        reg->imsc |= SPI_SSPIMSC_RXIM_BITS;
    }
}

void VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_irq_disable)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    vsf_spi_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);

    spi_hw_t *reg = spi_ptr->reg;
    if (irq_mask & VSF_SPI_IRQ_MASK_TX) {
        reg->imsc &= ~SPI_SSPIMSC_TXIM_BITS;
    }
    if (irq_mask & VSF_SPI_IRQ_MASK_RX) {
        reg->imsc &= ~SPI_SSPIMSC_RXIM_BITS;
    }
}

vsf_spi_irq_mask_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_irq_clear)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    vsf_spi_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);

    spi_hw_t *reg = spi_ptr->reg;
    vsf_spi_irq_mask_t cleared = 0;

    if (irq_mask & VSF_SPI_IRQ_MASK_TX) {
        reg->icr = SPI_SSPICR_RTIC_BITS;
        cleared |= VSF_SPI_IRQ_MASK_TX;
    }
    if (irq_mask & VSF_SPI_IRQ_MASK_RX) {
        reg->icr = SPI_SSPICR_RORIC_BITS;
        cleared |= VSF_SPI_IRQ_MASK_RX;
    }

    return cleared;
}

vsf_spi_status_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_status)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);

    vsf_spi_status_t status = {
        .is_busy = spi_ptr->status.is_busy,
    };

    return status;
}

vsf_spi_capability_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_capability)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);

    return (vsf_spi_capability_t) {
        .irq_mask           = VSF_SPI_IRQ_MASK_TX | VSF_SPI_IRQ_MASK_RX,
        .support_hardware_cs = 0,
        .support_software_cs = 1,
        .cs_count           = 1,
        .max_clock_hz       = clock_get_hz(clk_sys),
        .min_clock_hz       = 1,
    };
}

vsf_err_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_get_configuration)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    vsf_spi_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((NULL != spi_ptr) && (NULL != cfg_ptr));

    *cfg_ptr = spi_ptr->cfg;
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_ctrl)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    vsf_spi_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_UNUSED_PARAM(ctrl);
    VSF_UNUSED_PARAM(param);
    VSF_HAL_ASSERT(0);
    return VSF_ERR_NOT_SUPPORT;
}

static void VSF_MCONNECT(__, VSF_SPI_CFG_IMP_PREFIX, _spi_irqhandler)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);

    spi_hw_t *reg = spi_ptr->reg;
    uint32_t mis = reg->mis;

    if (spi_ptr->status.is_busy) {
        /* Async mode: manage FIFO and complete transfer */

        /* Drain RX FIFO */
        while ((reg->sr & __PL022_SR_RNE) && (spi_ptr->rx_offset < spi_ptr->count)) {
            uint16_t rx_data = (uint16_t)reg->dr;
            if (spi_ptr->rx_buf != NULL) {
                spi_ptr->rx_buf[spi_ptr->rx_offset] = (uint8_t)rx_data;
            }
            spi_ptr->rx_offset++;
        }

        /* Refill TX FIFO */
        while ((reg->sr & __PL022_SR_TNF) && (spi_ptr->tx_offset < spi_ptr->count)) {
            uint16_t tx_data = 0;
            if (spi_ptr->tx_buf != NULL) {
                tx_data = spi_ptr->tx_buf[spi_ptr->tx_offset];
            }
            reg->dr = tx_data;
            spi_ptr->tx_offset++;
        }

        /* Safety: if TX is done but RX still pending, poll-read remaining */
        if (spi_ptr->tx_offset >= spi_ptr->count) {
            while ((reg->sr & __PL022_SR_RNE) && (spi_ptr->rx_offset < spi_ptr->count)) {
                uint16_t rx_data = (uint16_t)reg->dr;
                if (spi_ptr->rx_buf != NULL) {
                    spi_ptr->rx_buf[spi_ptr->rx_offset] = (uint8_t)rx_data;
                }
                spi_ptr->rx_offset++;
            }
        }

        if ((spi_ptr->tx_offset >= spi_ptr->count) &&
            (spi_ptr->rx_offset >= spi_ptr->count)) {
            /* Transfer complete */
            reg->imsc &= ~(SPI_SSPIMSC_TXIM_BITS | SPI_SSPIMSC_RXIM_BITS);
            spi_ptr->status.is_busy = false;
            spi_ptr->transferred = spi_ptr->count;
            if (spi_ptr->isr.handler_fn != NULL) {
                spi_ptr->isr.handler_fn(spi_ptr->isr.target_ptr,
                                        (vsf_spi_t *)spi_ptr,
                                        VSF_SPI_IRQ_MASK_TX_CPL | VSF_SPI_IRQ_MASK_RX_CPL);
            }
            return;
        }

        /* Still in progress — clear interrupt flags and return */
        reg->icr = SPI_SSPICR_RTIC_BITS | SPI_SSPICR_RORIC_BITS;
        return;
    }

    /* Non-async mode: dispatch threshold interrupts */
    vsf_spi_irq_mask_t irq_mask = 0;
    if (mis & SPI_SSPMIS_TXMIS_BITS) {
        irq_mask |= VSF_SPI_IRQ_MASK_TX;
    }
    if (mis & SPI_SSPMIS_RXMIS_BITS) {
        irq_mask |= VSF_SPI_IRQ_MASK_RX;
    }

    reg->icr = SPI_SSPICR_RTIC_BITS | SPI_SSPICR_RORIC_BITS;

    if ((irq_mask != 0) && (spi_ptr->isr.handler_fn != NULL)) {
        spi_ptr->isr.handler_fn(spi_ptr->isr.target_ptr,
                                (vsf_spi_t *)spi_ptr,
                                irq_mask);
    }
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_SPI_CFG_MODE_CHECK_UNIQUE                          VSF_HAL_CHECK_MODE_LOOSE
#define VSF_SPI_CFG_IRQ_MASK_CHECK_UNIQUE                      VSF_HAL_CHECK_MODE_STRICT
#define VSF_SPI_CFG_REIMPLEMENT_API_CAPABILITY                 ENABLED
#define VSF_SPI_CFG_REIMPLEMENT_API_GET_CONFIGURATION          ENABLED
#define VSF_SPI_CFG_REIMPLEMENT_API_IRQ_CLEAR                  ENABLED
#define VSF_SPI_CFG_REIMPLEMENT_API_CTRL                       ENABLED

#define VSF_SPI_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t)                                \
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi, __IDX) = {                       \
        .reg = (spi_hw_t *)VSF_MCONNECT(VSF_SPI_CFG_IMP_UPCASE_PREFIX, _SPI,    \
                                        __IDX, _REG),                           \
        .rst_bit = VSF_MCONNECT(VSF_SPI_CFG_IMP_UPCASE_PREFIX, _SPI, __IDX,     \
                                _RST_BIT),                                      \
        .irqn =                                                                 \
            VSF_MCONNECT(VSF_SPI_CFG_IMP_UPCASE_PREFIX, _SPI, __IDX, _IRQN),    \
        __HAL_OP};                                                              \
    VSF_CAL_ROOT void VSF_MCONNECT(VSF_SPI_CFG_IMP_UPCASE_PREFIX, _SPI, __IDX,  \
                                   _IRQHandler)(void) {                         \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_SPI_CFG_IMP_PREFIX, _spi_irqhandler)(              \
            &VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi, __IDX));                \
        vsf_hal_irq_leave(ctx);                                                 \
    }

#include "hal/driver/common/spi/spi_template.inc"

#endif      /* VSF_HAL_USE_SPI */
/* EOF */
