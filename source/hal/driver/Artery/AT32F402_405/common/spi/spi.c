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

// HW
// for vendor headers
#include "hal/driver/vendor_driver.h"
// HW end

// SPI DMA transfer requires DMA to be enabled
#if VSF_HAL_USE_DMA != ENABLED
#   error "VSF_HAL_USE_DMA must be ENABLED for SPI DMA transfer support"
#endif

/*============================ MACROS ========================================*/

/*\note VSF_HW_SPI_CFG_MULTI_CLASS is only for drivers for specified device(hw drivers).
 *      For other drivers, please define VSF_${SPI_IP}_SPI_CFG_MULTI_CLASS in header file.
 */

// HW
#ifndef VSF_HW_SPI_CFG_MULTI_CLASS
#   define VSF_HW_SPI_CFG_MULTI_CLASS           VSF_SPI_CFG_MULTI_CLASS
#endif
// HW end

// HW
#define VSF_SPI_CFG_IMP_PREFIX                  vsf_hw
#define VSF_SPI_CFG_IMP_UPCASE_PREFIX           VSF_HW

// When it's not a standard SPI device, you can rename the device prefix. At the same time, you need to update:
//  VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t)    -> VSF_MCONNECT(VSF_SPI_CFG_IMP_DEVICE_PREFIX, _t)
//  VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_init) -> VSF_MCONNECT(VSF_SPI_CFG_IMP_DEVICE_PREFIX, _init)
//  ....
//
//#define VSF_SPI_CFG_IMP_RENAME_DEVICE_PREFIX      ENABLED     // Enable renaming device prefix
//#define VSF_SPI_CFG_IMP_DEVICE_PREFIX             vsf_hw_qspi // Custom device prefix when renaming
//#define VSF_SPI_CFG_IMP_DEVICE_UPCASE_PREFIX      VSF_HW_QSPI // Uppercase version of custom device prefix

// HW end

/*============================ TYPES =========================================*/

// HW
typedef struct {
    vsf_dma_t               *dma;          // DMA instance
    int8_t                  channel;       // DMA channel (-1 if not acquired)
    void                    *buffer;        // Buffer pointer
    uint32_t                transferred;  // Transferred count
} vsf_hw_spi_dma_t;

typedef struct VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_const_t) {
    spi_type                *reg;
    const vsf_hw_clk_t      *clk;
    vsf_hw_peripheral_en_t  en;
    vsf_hw_peripheral_rst_t rst;
    uint8_t                 irqn;
    uint8_t                 tx_dma_request;
    uint8_t                 rx_dma_request;
} VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_const_t);

typedef struct VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) {
#if VSF_HW_SPI_CFG_MULTI_CLASS == ENABLED
    vsf_spi_t               vsf_spi;
#endif
    const VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_const_t) *const_param;
    struct {
        vsf_spi_isr_t           isr;
        vsf_i2c_irq_mask_t      irq_mask;
        vsf_hw_spi_dma_t        tx;            // TX DMA context
        vsf_hw_spi_dma_t        rx;            // RX DMA context
        uint32_t                transfer_count; // Total transfer count
        bool                    is_transferring; // Transfer in progress flag
        uint8_t                 dummy_tx_buffer[4]; // Internal buffer for RX-only mode
        uint8_t                 dummy_rx_buffer[4]; // Internal buffer for TX-only mode
    };  // Anonymous struct - fields are directly accessible
} VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t);
// HW end


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*\note Implementation below is for hw spi only, because there is no requirements
 *          on the APIs of IPCore drivers.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
vsf_err_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_init)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    vsf_spi_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT((NULL != spi_ptr) && (NULL != cfg_ptr));

    memset(&spi_ptr->isr, 0,
           sizeof(*spi_ptr) - offsetof(VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t), isr));

    spi_ptr->tx.channel = -1;
    spi_ptr->rx.channel = -1;

    vsf_hw_peripheral_enable(spi_ptr->const_param->en);

    spi_type *reg = spi_ptr->const_param->reg;
    reg->ctrl1_bit.spien = 0;

    reg->i2sctrl_bit.i2smsel = 0;

    uint32_t div = vsf_hw_clk_get_freq_hz(spi_ptr->const_param->clk) / cfg_ptr->clock_hz;
    div = vsf_max(div, 2);
    if (div > 1024) {
        VSF_HAL_ASSERT(false);
        return VSF_ERR_NOT_SUPPORT;
    } else if (3 == div) {
        reg->ctrl2_bit.mdiv3en = 1;
    } else {
        uint8_t msb = vsf_msb32(div) - 1;
        if (div & ((1 << msb) - 1)) {
            msb++;
        }
        reg->ctrl1_bit.mdiv_l = msb & 7;
        reg->ctrl2_bit.mdiv_h = msb >> 3;
    }

    reg->ctrl1 = (reg->ctrl1 & ~__VSF_HW_SPI_CTRL1_MASK) | ((cfg_ptr->mode >>  0) & 0xFFFF);
    reg->ctrl2 = (reg->ctrl2 & ~(__VSF_HW_SPI_CTRL2_MASK >> 16)) | ((cfg_ptr->mode >> 16) & 0xFFFF);

    vsf_spi_isr_t *isr_ptr = &cfg_ptr->isr;
    spi_ptr->isr = *isr_ptr;
    if (cfg_ptr->isr.handler_fn != NULL) {
        NVIC_SetPriority(spi_ptr->const_param->irqn, (uint32_t)isr_ptr->prio);
        NVIC_EnableIRQ(spi_ptr->const_param->irqn);
    } else {
        NVIC_DisableIRQ(spi_ptr->const_param->irqn);
    }
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_fini)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);
    NVIC_DisableIRQ(spi_ptr->const_param->irqn);
}

fsm_rt_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_enable)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);
    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_disable)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);
    return fsm_rt_cpl;
}

void VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_irq_enable)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    vsf_spi_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);

    spi_type *reg = spi_ptr->const_param->reg;
    spi_ptr->irq_mask |= irq_mask;
    if (irq_mask & VSF_SPI_IRQ_MASK_RX) {
        reg->ctrl2_bit.rdbfie = 1;
    }
    if (irq_mask & VSF_SPI_IRQ_MASK_TX) {
        reg->ctrl2_bit.tdbeie = 1;
    }
    if (spi_ptr->irq_mask & (VSF_SPI_IRQ_MASK_RX_OVERFLOW_ERR | VSF_SPI_IRQ_MASK_CRC_ERR)) {
        reg->ctrl2_bit.errie = 1;
    }
}

void VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_irq_disable)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    vsf_spi_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);

    spi_type *reg = spi_ptr->const_param->reg;
    spi_ptr->irq_mask &= ~irq_mask;
    if (irq_mask & VSF_SPI_IRQ_MASK_RX) {
        reg->ctrl2_bit.rdbfie = 0;
    }
    if (irq_mask & VSF_SPI_IRQ_MASK_TX) {
        reg->ctrl2_bit.tdbeie = 0;
    }
    if (!(spi_ptr->irq_mask & (VSF_SPI_IRQ_MASK_RX_OVERFLOW_ERR | VSF_SPI_IRQ_MASK_CRC_ERR))) {
        reg->ctrl2_bit.errie = 0;
    }
}

vsf_spi_status_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_status)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);

    return (vsf_spi_status_t) {
        .is_busy = !!spi_ptr->const_param->reg->sts_bit.bf,
    };
}

vsf_err_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_cs_active)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    uint_fast8_t index
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(index == 0);

    spi_type *reg = spi_ptr->const_param->reg;
    if (!reg->ctrl1_bit.spien) {
        if (reg->ctrl1_bit.swcsen) {
            reg->ctrl1_bit.swcsil = 1;
        }
        reg->ctrl1_bit.spien = 1;
    }
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_cs_inactive)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    uint_fast8_t index
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(index == 0);

    spi_type *reg = spi_ptr->const_param->reg;
    if (reg->ctrl1_bit.spien) {
        reg->ctrl1_bit.spien = 0;
        if (reg->ctrl1_bit.swcsen) {
            reg->ctrl1_bit.swcsil = 0;
        }
    }
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_fifo_transfer)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    void *out_buffer_ptr,
    uint_fast32_t* out_offset_ptr,
    void *in_buffer_ptr,
    uint_fast32_t* in_offset_ptr,
    uint_fast32_t cnt
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);

    spi_type *reg = spi_ptr->const_param->reg;
    bool is_16bit = reg->ctrl1_bit.fbn;

    if (!reg->ctrl1_bit.spien) {
        VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_cs_active)(spi_ptr, 0);
    }

    if (reg->sts_bit.rdbf) {
        uint32_t dt = reg->dt;
        if ((in_buffer_ptr != NULL) && (*in_offset_ptr < cnt)) {
            if (is_16bit) {
                ((uint16_t *)in_buffer_ptr)[(*in_offset_ptr)++] = dt;
            } else {
                ((uint8_t *)in_buffer_ptr)[(*in_offset_ptr)++] = dt;
            }
        }
    }
    if (!reg->sts_bit.bf) {
        if ((out_buffer_ptr != NULL)) {
            if (*out_offset_ptr < cnt) {
                if (is_16bit) {
                    reg->dt = ((uint16_t *)out_buffer_ptr)[(*out_offset_ptr)++];
                } else {
                    reg->dt = ((uint8_t *)out_buffer_ptr)[(*out_offset_ptr)++];
                }
            }
        } else /* if (in_buffer_ptr != NULL) */ {
            reg->dt = 0;
        }
    }

    if (    ((NULL == out_buffer_ptr) || (*out_offset_ptr == cnt))
        &&  ((NULL == in_buffer_ptr) || (*in_offset_ptr == cnt))) {
        VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_cs_inactive)(spi_ptr, 0);
    }
}

static vsf_err_t __vsf_hw_spi_dma_acquire_channel(
    vsf_hw_spi_dma_t *dma_ch_ptr,
    uint8_t dma_request,
    vsf_arch_prio_t prio,
    const char *name
) {
    VSF_HAL_ASSERT(dma_ch_ptr->channel < 0);  // Channel should not be acquired yet

    vsf_dma_channel_hint_t hint = VSF_DMA_CHANNEL_HINT_INIT(dma_ch_ptr->channel, dma_request, prio);

    vsf_dma_t *dma = vsf_hw_dma_channel_acquire_from_all(&hint);
    if (dma == NULL) {
        return VSF_ERR_NOT_AVAILABLE;
    }

    dma_ch_ptr->dma = dma;
    dma_ch_ptr->channel = hint.channel;
    return VSF_ERR_NONE;
}

static void __vsf_hw_spi_dma_release_channel(vsf_hw_spi_dma_t *dma_ch_ptr)
{
    if (dma_ch_ptr->channel >= 0) {
        vsf_dma_channel_cancel(dma_ch_ptr->dma, dma_ch_ptr->channel);
        vsf_dma_channel_release(dma_ch_ptr->dma, dma_ch_ptr->channel);
        dma_ch_ptr->channel = -1;
        dma_ch_ptr->dma = NULL;
    }
}

static void __vsf_hw_spi_dma_cleanup(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    void *out_buffer_ptr,
    void *in_buffer_ptr
) {
    if (out_buffer_ptr != NULL) {
        __vsf_hw_spi_dma_release_channel(&spi_ptr->tx);
    }
    if (in_buffer_ptr != NULL) {
        __vsf_hw_spi_dma_release_channel(&spi_ptr->rx);
    }
    spi_ptr->is_transferring = false;
}

static uint32_t __vsf_hw_spi_dma_cancel_channel(
    vsf_hw_spi_dma_t *dma_ch_ptr,
    uint32_t transfer_count,
    const char *name
) {
    if (dma_ch_ptr->channel < 0) {
        return 0;
    }

    uint32_t remaining = vsf_dma_channel_get_transferred_count(dma_ch_ptr->dma, dma_ch_ptr->channel);
    vsf_dma_channel_cancel(dma_ch_ptr->dma, dma_ch_ptr->channel);
    vsf_dma_channel_release(dma_ch_ptr->dma, dma_ch_ptr->channel);
    dma_ch_ptr->transferred = (transfer_count > remaining) ? (transfer_count - remaining) : 0;
    dma_ch_ptr->channel = -1;
    dma_ch_ptr->dma = NULL;

    return dma_ch_ptr->transferred;
}

static vsf_err_t __vsf_hw_spi_dma_config(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    vsf_hw_spi_dma_t *dma_ch_ptr,
    bool is_tx,
    bool use_dummy_buffer,
    vsf_dma_addr_t spi_reg_addr,
    uint8_t dma_request,
    vsf_dma_channel_mode_t width_mode,
    vsf_dma_isr_handler_t *isr_handler
) {
    if (dma_ch_ptr->channel < 0) {
        return VSF_ERR_NONE;
    }

    vsf_dma_channel_mode_t dma_mode;
    if (is_tx) {
        dma_mode = VSF_DMA_MEMORY_TO_PERIPHERAL
                | (use_dummy_buffer ? VSF_DMA_SRC_ADDR_NO_CHANGE : VSF_DMA_SRC_ADDR_INCREMENT)
                | VSF_DMA_DST_ADDR_NO_CHANGE
                | width_mode;
    } else {
        dma_mode = VSF_DMA_PERIPHERAL_TO_MEMORY
                | VSF_DMA_SRC_ADDR_NO_CHANGE
                | (use_dummy_buffer ? VSF_DMA_DST_ADDR_NO_CHANGE : VSF_DMA_DST_ADDR_INCREMENT)
                | width_mode;
    }

    vsf_dma_isr_t dma_isr = {
        .handler_fn = isr_handler,
        .target_ptr = spi_ptr,
    };

    vsf_dma_channel_cfg_t dma_cfg = VSF_DMA_CHANNEL_CFG_INIT(dma_mode, dma_isr, VSF_DMA_IRQ_MASK_CPL,
                                                              is_tx ? 0 : dma_request, is_tx ? dma_request : 0,
                                                              spi_ptr->isr.prio);

    return vsf_dma_channel_config(dma_ch_ptr->dma, dma_ch_ptr->channel, &dma_cfg);
}

static vsf_err_t __vsf_hw_spi_dma_start(
    vsf_hw_spi_dma_t *dma_ch_ptr,
    bool is_tx,
    bool use_dummy_buffer,
    void *buffer,
    vsf_dma_addr_t spi_reg_addr,
    uint32_t count,
    bool is_16bit
) {
    if (dma_ch_ptr->channel < 0) {
        return VSF_ERR_NONE;
    }

    if (use_dummy_buffer && buffer != NULL) {
        if (is_16bit) {
            ((uint16_t *)buffer)[0] = 0;
        } else {
            ((uint8_t *)buffer)[0] = 0;
        }
    }

    vsf_dma_addr_t src_addr, dst_addr;
    if (is_tx) {
        src_addr = (vsf_dma_addr_t)buffer;
        dst_addr = spi_reg_addr;
    } else {
        src_addr = spi_reg_addr;
        dst_addr = (vsf_dma_addr_t)buffer;
    }

    return vsf_dma_channel_start(dma_ch_ptr->dma, dma_ch_ptr->channel, src_addr, dst_addr, count);
}

static vsf_err_t __vsf_hw_spi_dma_setup_and_start(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    vsf_hw_spi_dma_t *dma_ch_ptr,
    bool is_tx,
    bool use_dummy_buffer,
    void *buffer,
    vsf_dma_addr_t spi_reg_addr,
    uint8_t dma_request,
    vsf_dma_channel_mode_t width_mode,
    vsf_dma_isr_handler_t *isr_handler,
    uint32_t count,
    bool is_16bit
) {
    vsf_err_t err = __vsf_hw_spi_dma_acquire_channel(dma_ch_ptr, dma_request, spi_ptr->isr.prio, is_tx ? "TX" : "RX");
    if (err != VSF_ERR_NONE) {
        return err;
    }

    err = __vsf_hw_spi_dma_config(spi_ptr, dma_ch_ptr, is_tx, use_dummy_buffer,
                                   spi_reg_addr, dma_request, width_mode, isr_handler);
    if (err != VSF_ERR_NONE) {
        return err;
    }

    return __vsf_hw_spi_dma_start(dma_ch_ptr, is_tx, use_dummy_buffer, buffer,
                                   spi_reg_addr, count, is_16bit);
}

static void __vsf_hw_spi_dma_tx_isr_handler(void *target_ptr, vsf_dma_t *dma_ptr, int8_t channel, vsf_dma_irq_mask_t irq_mask)
{
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr = (VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *)target_ptr;

    if (irq_mask & VSF_DMA_IRQ_MASK_CPL) {
        VSF_HAL_ASSERT(spi_ptr->is_transferring);

        uint32_t dma_transferred = spi_ptr->transfer_count;
        if (spi_ptr->tx.transferred > 0) {
            spi_ptr->tx.transferred += dma_transferred;
        } else {
            spi_ptr->tx.transferred = dma_transferred;
        }

        spi_type *reg = spi_ptr->const_param->reg;
        reg->ctrl2_bit.dmaten = 0;

        if (spi_ptr->isr.handler_fn != NULL) {
            spi_ptr->isr.handler_fn(spi_ptr->isr.target_ptr, (vsf_spi_t *)spi_ptr, VSF_SPI_IRQ_MASK_TX_CPL);
        }
    }
}

static void __vsf_hw_spi_dma_rx_isr_handler(void *target_ptr, vsf_dma_t *dma_ptr, int8_t channel, vsf_dma_irq_mask_t irq_mask)
{
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr = (VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *)target_ptr;

    if (irq_mask & VSF_DMA_IRQ_MASK_CPL) {
        VSF_HAL_ASSERT(spi_ptr->is_transferring);

        spi_ptr->rx.transferred = spi_ptr->transfer_count;

        spi_type *reg = spi_ptr->const_param->reg;
        reg->ctrl2_bit.dmaren = 0;

        if (spi_ptr->isr.handler_fn != NULL) {
            spi_ptr->isr.handler_fn(spi_ptr->isr.target_ptr, (vsf_spi_t *)spi_ptr, VSF_SPI_IRQ_MASK_RX_CPL);
        }
    }
}

vsf_err_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_request_transfer)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    void *out_buffer_ptr,
    void *in_buffer_ptr,
    uint_fast32_t count
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(count > 0);
    VSF_HAL_ASSERT((out_buffer_ptr != NULL) || (in_buffer_ptr != NULL));

    spi_type *reg = spi_ptr->const_param->reg;

    if (spi_ptr->is_transferring) {
        return VSF_ERR_BUSY;
    }

    spi_ptr->tx.buffer = out_buffer_ptr;
    spi_ptr->rx.buffer = in_buffer_ptr;
    spi_ptr->transfer_count = count;
    spi_ptr->tx.transferred = 0;
    spi_ptr->rx.transferred = 0;
    spi_ptr->is_transferring = true;

    bool is_tx_only = (out_buffer_ptr != NULL) && (in_buffer_ptr == NULL);
    bool is_rx_only = (in_buffer_ptr != NULL) && (out_buffer_ptr == NULL);

    bool is_16bit = reg->ctrl1_bit.fbn;
    vsf_dma_channel_mode_t width_mode = (is_16bit ? VSF_DMA_SRC_WIDTH_BYTES_2 : VSF_DMA_SRC_WIDTH_BYTE_1)
                                      | (is_16bit ? VSF_DMA_DST_WIDTH_BYTES_2 : VSF_DMA_DST_WIDTH_BYTE_1)
                                      | VSF_DMA_PRIORITY_HIGH;

    vsf_dma_addr_t spi_reg_addr = (vsf_dma_addr_t)&reg->dt;

    void *rx_buffer = is_tx_only ? spi_ptr->dummy_rx_buffer : in_buffer_ptr;
    vsf_err_t err = __vsf_hw_spi_dma_setup_and_start(spi_ptr, &spi_ptr->rx, false, is_tx_only,
                                                      rx_buffer, spi_reg_addr,
                                                      spi_ptr->const_param->rx_dma_request, width_mode,
                                                      __vsf_hw_spi_dma_rx_isr_handler, count, is_16bit);
    if (err != VSF_ERR_NONE) {
        spi_ptr->is_transferring = false;
        return err;
    }

    void *tx_buffer = is_rx_only ? spi_ptr->dummy_tx_buffer : out_buffer_ptr;
    err = __vsf_hw_spi_dma_setup_and_start(spi_ptr, &spi_ptr->tx, true, is_rx_only,
                                            tx_buffer, spi_reg_addr,
                                            spi_ptr->const_param->tx_dma_request, width_mode,
                                            __vsf_hw_spi_dma_tx_isr_handler, count, is_16bit);
    if (err != VSF_ERR_NONE) {
        __vsf_hw_spi_dma_cleanup(spi_ptr, out_buffer_ptr, in_buffer_ptr);
        return err;
    }

    reg->ctrl2_bit.dmaren = 1;
    reg->ctrl2_bit.dmaten = 1;

    if (!reg->ctrl1_bit.spien) {
        VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_cs_active)(spi_ptr, 0);
    }

    return VSF_ERR_NONE;
}


vsf_err_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_cancel_transfer)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);

    if (!spi_ptr->is_transferring) {
        return VSF_ERR_NONE;
    }

    spi_type *reg = spi_ptr->const_param->reg;

    reg->ctrl2_bit.dmaten = 0;
    reg->ctrl2_bit.dmaren = 0;

    __vsf_hw_spi_dma_cancel_channel(&spi_ptr->tx, spi_ptr->transfer_count, "TX");
    __vsf_hw_spi_dma_cancel_channel(&spi_ptr->rx, spi_ptr->transfer_count, "RX");

    spi_ptr->is_transferring = false;

    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_get_transferred_count)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    uint_fast32_t *out_count,
    uint_fast32_t *in_count
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);

    if (out_count != NULL) {
        if (spi_ptr->tx.channel >= 0 && spi_ptr->is_transferring) {
            uint32_t remaining = vsf_dma_channel_get_transferred_count(spi_ptr->tx.dma, spi_ptr->tx.channel);
            *out_count = (spi_ptr->transfer_count > remaining) ? (spi_ptr->transfer_count - remaining) : 0;
        } else {
            *out_count = spi_ptr->tx.transferred;
        }
    }

    if (in_count != NULL) {
        if (spi_ptr->rx.channel >= 0 && spi_ptr->is_transferring) {
            uint32_t remaining = vsf_dma_channel_get_transferred_count(spi_ptr->rx.dma, spi_ptr->rx.channel);
            *in_count = (spi_ptr->transfer_count > remaining) ? (spi_ptr->transfer_count - remaining) : 0;
        } else {
            *in_count = spi_ptr->rx.transferred;
        }
    }
}

static void VSF_MCONNECT(__, VSF_SPI_CFG_IMP_PREFIX, _spi_irqhandler)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr
) {
    VSF_HAL_ASSERT(NULL != spi_ptr);

    vsf_spi_isr_t *isr_ptr = &spi_ptr->isr;
    vsf_spi_irq_mask_t irq_mask = VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_irq_clear)(spi_ptr, VSF_SPI_IRQ_ALL_BITS_MASK);
    if ((irq_mask != 0) && (isr_ptr->handler_fn != NULL)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_spi_t *)spi_ptr, irq_mask);
    }
}

/*\note Implementation of APIs below is optional, because there is default implementation in spi_template.inc.
 *      VSF_SPI_CFG_REIMPLEMENT_API_XXXX can be defined to ENABLED to re-write the default implementation for better performance.
 *
 *      The list of APIs and configuration:
 *      VSF_SPI_CFG_REIMPLEMENT_API_CAPABILITY for spi_capability.
 *          Default implementation will return hardware capability structure.
 *      VSF_SPI_CFG_REIMPLEMENT_API_GET_CONFIGURATION for spi_get_configuration.
 *          Default implementation will trigger assertion and return VSF_ERR_NOT_SUPPORT.
 *      VSF_SPI_CFG_REIMPLEMENT_API_CTRL for spi_ctrl.
 *          Default implementation will trigger assertion and return VSF_ERR_NOT_SUPPORT.
 *      VSF_SPI_CFG_REIMPLEMENT_API_IRQ_CLEAR for spi_irq_clear.
 *          Default implementation will assert(false) to indicate the feature is not implemented.
 */

vsf_spi_capability_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_capability)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);

    uint32_t clk_hz = vsf_hw_clk_get_freq_hz(spi_ptr->const_param->clk);
    return (vsf_spi_capability_t) {
        .irq_mask               = VSF_SPI_IRQ_MASK_TX | VSF_SPI_IRQ_MASK_RX
                                | VSF_SPI_IRQ_MASK_TX_CPL | VSF_SPI_IRQ_MASK_RX_CPL
                                | VSF_SPI_IRQ_MASK_RX_OVERFLOW_ERR
                                | VSF_SPI_IRQ_MASK_CRC_ERR,
        .support_hardware_cs    = 1,
        .support_software_cs    = 1,
        .cs_count               = 1,
        .max_clock_hz           = clk_hz / 2,
        .min_clock_hz           = clk_hz / 1024,
    };
}

vsf_err_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_ctrl)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    vsf_spi_ctrl_t ctrl,
    void *param
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);

    return VSF_ERR_NOT_SUPPORT;
}

vsf_spi_irq_mask_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_irq_clear)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    vsf_spi_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != spi_ptr);

    spi_type *reg = spi_ptr->const_param->reg;
    vsf_spi_irq_mask_t cleared_mask = 0;

    if ((irq_mask & VSF_SPI_IRQ_MASK_RX) && reg->sts_bit.rdbf && reg->ctrl2_bit.rdbfie) {
        (void)reg->dt;
        cleared_mask |= VSF_SPI_IRQ_MASK_RX;
    }

    if ((irq_mask & VSF_SPI_IRQ_MASK_TX) && reg->sts_bit.tdbe && reg->ctrl2_bit.tdbeie) {
        cleared_mask |= VSF_SPI_IRQ_MASK_TX;
    }

    if ((irq_mask & VSF_SPI_IRQ_MASK_RX_CPL) && reg->sts_bit.rdbf && !reg->sts_bit.bf && reg->ctrl2_bit.rdbfie) {
        (void)reg->dt;
        cleared_mask |= VSF_SPI_IRQ_MASK_RX_CPL;
    }

    if ((irq_mask & VSF_SPI_IRQ_MASK_TX_CPL) && reg->sts_bit.tdbe && !reg->sts_bit.bf && reg->ctrl2_bit.tdbeie) {
        cleared_mask |= VSF_SPI_IRQ_MASK_TX_CPL;
    }

    if ((irq_mask & VSF_SPI_IRQ_MASK_RX_OVERFLOW_ERR) && reg->sts_bit.roerr && reg->ctrl2_bit.errie) {
        (void)reg->dt;
        (void)reg->sts;
        cleared_mask |= VSF_SPI_IRQ_MASK_RX_OVERFLOW_ERR;
    }

    if ((irq_mask & VSF_SPI_IRQ_MASK_CRC_ERR) && reg->sts_bit.ccerr && reg->ctrl2_bit.errie) {
        reg->sts = ~SPI_CCERR_FLAG;
        cleared_mask |= VSF_SPI_IRQ_MASK_CRC_ERR;
    }

    return cleared_mask;
}
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/

/*\note Instantiation below is for hw spi only.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
#define VSF_SPI_CFG_REIMPLEMENT_API_CAPABILITY          ENABLED
#define VSF_SPI_CFG_REIMPLEMENT_API_CTRL                ENABLED
#define VSF_SPI_CFG_REIMPLEMENT_API_IRQ_CLEAR           ENABLED

#define VSF_SPI_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    static const VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_const_t)             \
        VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi, __IDX, _const) = {           \
        .reg   = VSF_MCONNECT(VSF_SPI_CFG_IMP_UPCASE_PREFIX, _SPI, __IDX,_REG), \
        .clk   = &VSF_MCONNECT(VSF_SPI_CFG_IMP_UPCASE_PREFIX, _SPI, __IDX,_CLK),\
        .en    = VSF_MCONNECT(VSF_SPI_CFG_IMP_UPCASE_PREFIX, _SPI, __IDX,_EN),  \
        .rst   = VSF_MCONNECT(VSF_SPI_CFG_IMP_UPCASE_PREFIX, _SPI, __IDX,_RST), \
        .irqn  = VSF_MCONNECT(VSF_SPI_CFG_IMP_UPCASE_PREFIX, _SPI, __IDX,_IRQN),\
        .tx_dma_request = VSF_MCONNECT(VSF_SPI_CFG_IMP_UPCASE_PREFIX,           \
                                       _SPI, __IDX,_TX_DMA_REQUEST),            \
        .rx_dma_request = VSF_MCONNECT(VSF_SPI_CFG_IMP_UPCASE_PREFIX, _SPI,     \
                                       __IDX,_RX_DMA_REQUEST),                  \
    };                                                                          \
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t)                                \
        VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi, __IDX) = {                   \
        .const_param = &VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi,              \
                                     __IDX, _const),                            \
        __HAL_OP                                                                \
    };                                                                          \
    void VSF_MCONNECT(VSF_HW_INTERRUPT, VSF_MCONNECT(                           \
            VSF_SPI_CFG_IMP_UPCASE_PREFIX, _SPI, __IDX, _IRQN))(void)           \
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_SPI_CFG_IMP_PREFIX, _spi_irqhandler)(              \
            &VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi, __IDX)                  \
        );                                                                      \
        vsf_hal_irq_leave(ctx);                                                 \
    }
#include "hal/driver/common/spi/spi_template.inc"
// HW end

#endif /* VSF_HAL_USE_SPI */
