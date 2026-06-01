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

#include "./uart.h"

#if VSF_HAL_USE_USART == ENABLED

#define __VSF_HAL_PL011_UART_CLASS_INHERIT__

#include "hal/vsf_hal.h"

#include "hal/driver/vendor_driver.h"
#include "hal/driver/IPCore/ARM/PL011/vsf_pl011_uart_reg.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_USART_CFG_MULTI_CLASS
#   define VSF_HW_USART_CFG_MULTI_CLASS         VSF_USART_CFG_MULTI_CLASS
#endif

#define VSF_USART_CFG_IMP_PREFIX                vsf_hw
#define VSF_USART_CFG_IMP_UPCASE_PREFIX         VSF_HW

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) {
    implement(vsf_pl011_usart_t)
    IRQn_Type               irqn;
    uint32_t                rst_bit;
    /* Cached copy of the cfg passed to init(); returned verbatim by
     * get_configuration. PL011 itself stores config only in registers,
     * but reading them back requires undoing the integer / fractional
     * divider math, so a cached copy is simpler. See PRD
     * hal-get-configuration-reads-hardware. */
    vsf_usart_cfg_t         cached_cfg;
    /* DMA support for request_tx / request_rx.  Channels are acquired
     * dynamically via vsf_hw_dma_channel_acquire_from_all() so the driver
     * does not depend on a specific DMA instance. */
    vsf_dma_t              *tx_dma;
    vsf_dma_t              *rx_dma;
    int8_t                  tx_dma_ch;
    int8_t                  rx_dma_ch;
    uint32_t                tx_count;
    uint32_t                rx_count;
    uint8_t                 tx_dreq;
    uint8_t                 rx_dreq;
} VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t);

/*============================ LOCAL FUNCTIONS ===============================*/

/*============================ LOCAL FUNCTIONS ===============================*/

static void __vsf_hw_usart_dma_tx_isr(void *target, vsf_dma_t *dma,
                                      int8_t ch, vsf_dma_irq_mask_t mask)
{
    VSF_UNUSED_PARAM(dma);
    VSF_UNUSED_PARAM(ch);
    VSF_UNUSED_PARAM(mask);
    vsf_hw_usart_t *usart = (vsf_hw_usart_t *)target;
    vsf_pl011_usart_txdma_config(&usart->use_as__vsf_pl011_usart_t, false);
    if (usart->tx_dma_ch >= 0) {
        usart->tx_count = vsf_dma_channel_get_transferred_count(
            usart->tx_dma, (uint8_t)usart->tx_dma_ch);
        vsf_dma_channel_release(usart->tx_dma, (uint8_t)usart->tx_dma_ch);
        usart->tx_dma_ch = -1;
        usart->tx_dma    = NULL;
    }
    vsf_usart_isr_t *isr = &usart->cached_cfg.isr;
    if (isr->handler_fn != NULL) {
        isr->handler_fn(isr->target_ptr, (vsf_usart_t *)usart,
                        VSF_USART_IRQ_MASK_TX_CPL);
    }
}

static void __vsf_hw_usart_dma_rx_isr(void *target, vsf_dma_t *dma,
                                      int8_t ch, vsf_dma_irq_mask_t mask)
{
    VSF_UNUSED_PARAM(dma);
    VSF_UNUSED_PARAM(ch);
    VSF_UNUSED_PARAM(mask);
    vsf_hw_usart_t *usart = (vsf_hw_usart_t *)target;
    vsf_pl011_usart_rxdma_config(&usart->use_as__vsf_pl011_usart_t, false);
    if (usart->rx_dma_ch >= 0) {
        usart->rx_count = vsf_dma_channel_get_transferred_count(
            usart->rx_dma, (uint8_t)usart->rx_dma_ch);
        vsf_dma_channel_release(usart->rx_dma, (uint8_t)usart->rx_dma_ch);
        usart->rx_dma_ch = -1;
        usart->rx_dma    = NULL;
    }
    vsf_usart_isr_t *isr = &usart->cached_cfg.isr;
    if (isr->handler_fn != NULL) {
        isr->handler_fn(isr->target_ptr, (vsf_usart_t *)usart,
                        VSF_USART_IRQ_MASK_RX_CPL);
    }
}

/*============================ IMPLEMENTATION ================================*/

vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_init)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    /* Clean up any DMA channels left active by a prior scenario that aborted
     * (e.g. VSF_TEST_ASSERT timeout without reaching fini). The UART reset
     * below clears DMACR, which stops DREQ generation, but the DMA channel
     * itself keeps running unless we explicitly cancel it. */
    if (usart_ptr->tx_dma != NULL && usart_ptr->tx_dma_ch >= 0) {
        vsf_dma_channel_cancel(usart_ptr->tx_dma, (uint8_t)usart_ptr->tx_dma_ch);
        vsf_dma_channel_release(usart_ptr->tx_dma, (uint8_t)usart_ptr->tx_dma_ch);
        usart_ptr->tx_dma_ch = -1;
        usart_ptr->tx_dma    = NULL;
    }
    if (usart_ptr->rx_dma != NULL && usart_ptr->rx_dma_ch >= 0) {
        vsf_dma_channel_cancel(usart_ptr->rx_dma, (uint8_t)usart_ptr->rx_dma_ch);
        vsf_dma_channel_release(usart_ptr->rx_dma, (uint8_t)usart_ptr->rx_dma_ch);
        usart_ptr->rx_dma_ch = -1;
        usart_ptr->rx_dma    = NULL;
    }

    uint32_t rst_bit = usart_ptr->rst_bit;
    /* Cycle the peripheral through reset so re-init from a previously-active
     * state (e.g. a prior scenario that left UART1 enabled) starts clean.
     * The earlier "only de-assert" pattern silently no-op'd on re-init. */
    resets_hw->reset = resets_hw->reset | rst_bit;
    // spin-wait: wait for reset to assert
    while (resets_hw->reset_done & rst_bit);
    resets_hw->reset = resets_hw->reset & ~rst_bit;
    // spin-wait: wait for reset to de-assert
    while (!(resets_hw->reset_done & rst_bit));

    vsf_err_t err = vsf_pl011_usart_init(
        &usart_ptr->use_as__vsf_pl011_usart_t, cfg_ptr, clock_get_hz(clk_peri));

    if (err == VSF_ERR_NONE) {
        usart_ptr->cached_cfg = *cfg_ptr;
    }

    vsf_usart_isr_t *isr_ptr = &cfg_ptr->isr;
    if (isr_ptr->handler_fn != NULL) {
        NVIC_SetPriority(usart_ptr->irqn, (uint32_t)isr_ptr->prio);
        NVIC_EnableIRQ(usart_ptr->irqn);
    } else {
        NVIC_DisableIRQ(usart_ptr->irqn);
    }

    return err;
}

void VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_fini)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);

    /* Cancel any outstanding DMA transfers so the next scenario starts
     * with clean channel state. Without this, a timed-out scenario leaves
     * tx_dma_ch / rx_dma_ch >= 0 and the next request_rx / request_tx
     * returns VSF_ERR_NOT_AVAILABLE. */
    if (usart_ptr->tx_dma != NULL && usart_ptr->tx_dma_ch >= 0) {
        vsf_pl011_usart_txdma_config(&usart_ptr->use_as__vsf_pl011_usart_t, false);
        vsf_dma_channel_cancel(usart_ptr->tx_dma, (uint8_t)usart_ptr->tx_dma_ch);
        vsf_dma_channel_release(usart_ptr->tx_dma, (uint8_t)usart_ptr->tx_dma_ch);
        usart_ptr->tx_dma_ch = -1;
        usart_ptr->tx_dma    = NULL;
    }
    if (usart_ptr->rx_dma != NULL && usart_ptr->rx_dma_ch >= 0) {
        vsf_pl011_usart_rxdma_config(&usart_ptr->use_as__vsf_pl011_usart_t, false);
        vsf_dma_channel_cancel(usart_ptr->rx_dma, (uint8_t)usart_ptr->rx_dma_ch);
        vsf_dma_channel_release(usart_ptr->rx_dma, (uint8_t)usart_ptr->rx_dma_ch);
        usart_ptr->rx_dma_ch = -1;
        usart_ptr->rx_dma    = NULL;
    }

    vsf_pl011_usart_fini(&usart_ptr->use_as__vsf_pl011_usart_t);

    /* Hold the UART peripheral in reset so it stays inert until the next
     * vsf_hw_usart_init() brings it back out. Symmetric with the
     * set-then-clear cycle init does, and protects scenarios that don't
     * fini explicitly. */
    NVIC_DisableIRQ(usart_ptr->irqn);
    resets_hw->reset = resets_hw->reset | usart_ptr->rst_bit;
}

fsm_rt_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_enable)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return vsf_pl011_usart_enable(&usart_ptr->use_as__vsf_pl011_usart_t);
}

fsm_rt_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_disable)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return vsf_pl011_usart_disable(&usart_ptr->use_as__vsf_pl011_usart_t);
}

void VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_irq_enable)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    vsf_pl011_usart_irq_enable(&usart_ptr->use_as__vsf_pl011_usart_t, irq_mask);
}

void VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_irq_disable)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    vsf_pl011_usart_irq_disable(&usart_ptr->use_as__vsf_pl011_usart_t, irq_mask);
}

vsf_usart_irq_mask_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_irq_clear)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    vsf_pl011_usart_reg_t *reg = (vsf_pl011_usart_reg_t *)usart_ptr->reg;
    reg->UARTICR.VALUE = irq_mask & PL011_USART_IRQ_MASK;
    return irq_mask & PL011_USART_IRQ_MASK;
}

vsf_usart_status_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_status)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return vsf_pl011_usart_status(&usart_ptr->use_as__vsf_pl011_usart_t);
}

uint_fast32_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_rxfifo_get_data_count)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return vsf_pl011_usart_rxfifo_get_data_count(
        &usart_ptr->use_as__vsf_pl011_usart_t);
}

uint_fast32_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_rxfifo_read)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    void *buffer_ptr,
    uint_fast32_t count
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(NULL != buffer_ptr);
    return vsf_pl011_usart_rxfifo_read(
        &usart_ptr->use_as__vsf_pl011_usart_t, buffer_ptr, (uint_fast16_t)count);
}

uint_fast32_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_txfifo_get_free_count)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return vsf_pl011_usart_txfifo_get_free_count(
        &usart_ptr->use_as__vsf_pl011_usart_t);
}

uint_fast32_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_txfifo_write)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    void *buffer_ptr,
    uint_fast32_t count
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(NULL != buffer_ptr);
    return vsf_pl011_usart_txfifo_write(
        &usart_ptr->use_as__vsf_pl011_usart_t, buffer_ptr, (uint_fast16_t)count);
}

vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_request_rx)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    void *buffer_ptr,
    uint_fast32_t count
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(NULL != buffer_ptr);
    if (usart_ptr->rx_dma_ch >= 0) {
        return VSF_ERR_NOT_AVAILABLE;
    }
    vsf_dma_channel_hint_t hint = { .channel = -1 };
    vsf_dma_t *dma = vsf_hw_dma_channel_acquire_from_all(&hint);
    if (dma == NULL) { return VSF_ERR_NOT_AVAILABLE; }
    usart_ptr->rx_dma    = dma;
    usart_ptr->rx_dma_ch = hint.channel;
    vsf_err_t err = vsf_dma_channel_config(dma, (uint8_t)hint.channel,
        &(vsf_dma_channel_cfg_t){
            .mode = VSF_DMA_PERIPHERAL_TO_MEMORY
                  | VSF_DMA_SRC_ADDR_NO_CHANGE
                  | VSF_DMA_DST_ADDR_INCREMENT,
            .isr = {
                .handler_fn = __vsf_hw_usart_dma_rx_isr,
                .target_ptr = usart_ptr,
            },
            .irq_mask = VSF_DMA_IRQ_MASK_CPL,
            .prio = vsf_arch_prio_0,
            .src_request_idx = usart_ptr->rx_dreq,
        });
    if (err != VSF_ERR_NONE) {
        vsf_dma_channel_release(dma, (uint8_t)hint.channel);
        usart_ptr->rx_dma_ch = -1;
        usart_ptr->rx_dma    = NULL;
        return err;
    }
    err = vsf_dma_channel_start(dma, (uint8_t)hint.channel,
                                (vsf_dma_addr_t)usart_ptr->reg + 0x00,
                                (vsf_dma_addr_t)buffer_ptr, count);
    if (err != VSF_ERR_NONE) {
        vsf_dma_channel_release(dma, (uint8_t)hint.channel);
        usart_ptr->rx_dma_ch = -1;
        usart_ptr->rx_dma    = NULL;
        return err;
    }
    vsf_pl011_usart_rxdma_config(&usart_ptr->use_as__vsf_pl011_usart_t, true);
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_request_tx)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    void *buffer_ptr,
    uint_fast32_t count
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(NULL != buffer_ptr);
    if (usart_ptr->tx_dma_ch >= 0) {
        return VSF_ERR_NOT_AVAILABLE;
    }
    vsf_dma_channel_hint_t hint = { .channel = -1 };
    vsf_dma_t *dma = vsf_hw_dma_channel_acquire_from_all(&hint);
    if (dma == NULL) { return VSF_ERR_NOT_AVAILABLE; }
    usart_ptr->tx_dma    = dma;
    usart_ptr->tx_dma_ch = hint.channel;
    vsf_err_t err = vsf_dma_channel_config(dma, (uint8_t)hint.channel,
        &(vsf_dma_channel_cfg_t){
            .mode = VSF_DMA_MEMORY_TO_PERIPHERAL
                  | VSF_DMA_SRC_ADDR_INCREMENT
                  | VSF_DMA_DST_ADDR_NO_CHANGE,
            .isr = {
                .handler_fn = __vsf_hw_usart_dma_tx_isr,
                .target_ptr = usart_ptr,
            },
            .irq_mask = VSF_DMA_IRQ_MASK_CPL,
            .prio = vsf_arch_prio_0,
            .dst_request_idx = usart_ptr->tx_dreq,
        });
    if (err != VSF_ERR_NONE) {
        vsf_dma_channel_release(dma, (uint8_t)hint.channel);
        usart_ptr->tx_dma_ch = -1;
        usart_ptr->tx_dma    = NULL;
        return err;
    }
    err = vsf_dma_channel_start(dma, (uint8_t)hint.channel,
                                (vsf_dma_addr_t)buffer_ptr,
                                (vsf_dma_addr_t)usart_ptr->reg + 0x00,
                                count);
    if (err != VSF_ERR_NONE) {
        vsf_dma_channel_release(dma, (uint8_t)hint.channel);
        usart_ptr->tx_dma_ch = -1;
        usart_ptr->tx_dma    = NULL;
        return err;
    }
    vsf_pl011_usart_txdma_config(&usart_ptr->use_as__vsf_pl011_usart_t, true);
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_cancel_rx)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    int8_t ch = usart_ptr->rx_dma_ch;
    vsf_dma_t *dma = usart_ptr->rx_dma;
    if (ch >= 0 && dma != NULL) {
        vsf_pl011_usart_rxdma_config(&usart_ptr->use_as__vsf_pl011_usart_t, false);
        vsf_dma_channel_cancel(dma, (uint8_t)ch);
        vsf_dma_channel_release(dma, (uint8_t)ch);
        usart_ptr->rx_dma_ch = -1;
        usart_ptr->rx_dma    = NULL;
    }
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_cancel_tx)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    int8_t ch = usart_ptr->tx_dma_ch;
    vsf_dma_t *dma = usart_ptr->tx_dma;
    if (ch >= 0 && dma != NULL) {
        vsf_pl011_usart_txdma_config(&usart_ptr->use_as__vsf_pl011_usart_t, false);
        vsf_dma_channel_cancel(dma, (uint8_t)ch);
        vsf_dma_channel_release(dma, (uint8_t)ch);
        usart_ptr->tx_dma_ch = -1;
        usart_ptr->tx_dma    = NULL;
    }
    return VSF_ERR_NONE;
}

int_fast32_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_get_rx_count)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    if (usart_ptr->rx_dma_ch >= 0 && usart_ptr->rx_dma != NULL) {
        return (int_fast32_t)vsf_dma_channel_get_transferred_count(
            usart_ptr->rx_dma, (uint8_t)usart_ptr->rx_dma_ch);
    }
    return (int_fast32_t)usart_ptr->rx_count;
}

int_fast32_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_get_tx_count)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    if (usart_ptr->tx_dma_ch >= 0 && usart_ptr->tx_dma != NULL) {
        return (int_fast32_t)vsf_dma_channel_get_transferred_count(
            usart_ptr->tx_dma, (uint8_t)usart_ptr->tx_dma_ch);
    }
    return (int_fast32_t)usart_ptr->tx_count;
}

vsf_usart_capability_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_capability)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    vsf_usart_capability_t cap = vsf_pl011_usart_capability(
        &usart_ptr->use_as__vsf_pl011_usart_t, clock_get_hz(clk_peri));
    cap.irq_mask |= VSF_USART_IRQ_MASK_TX_CPL | VSF_USART_IRQ_MASK_RX_CPL;
    return cap;
}

vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_ctrl)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_ctrl_t ctrl,
    void *param
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    vsf_pl011_usart_reg_t *reg = (vsf_pl011_usart_reg_t *)usart_ptr->reg;
    switch (ctrl) {
    case VSF_USART_CTRL_SET_BREAK:
        reg->UARTLCR_H.BRK = 1;
        return VSF_ERR_NONE;
    case VSF_USART_CTRL_CLEAR_BREAK:
        reg->UARTLCR_H.BRK = 0;
        return VSF_ERR_NONE;
    case VSF_USART_CTRL_SEND_BREAK:
        /* PL011 only supports manual break via the BRK bit (SET_BREAK /
         * CLEAR_BREAK). There is no hardware auto-timed break. The HAL
         * is a thin wrapper that exposes hardware capability as-is — it
         * does not emulate SEND_BREAK in software. Callers that need a
         * timed break should use SET_BREAK + timer + CLEAR_BREAK. */
        VSF_HAL_ASSERT(0);
        return VSF_ERR_NOT_SUPPORT;
    default:
        return VSF_ERR_NOT_SUPPORT;
    }
}

vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_get_configuration)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    /* Return the cached cfg captured at the last init(). See struct
     * comment and PRD hal-get-configuration-reads-hardware for why we
     * cache instead of reading PL011 registers. */
    *cfg_ptr = usart_ptr->cached_cfg;
    return VSF_ERR_NONE;
}

static void VSF_MCONNECT(__, VSF_USART_CFG_IMP_PREFIX, _usart_irqhandler)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    vsf_pl011_usart_irqhandler(&usart_ptr->use_as__vsf_pl011_usart_t);
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_USART_CFG_MODE_CHECK_UNIQUE                 VSF_HAL_CHECK_MODE_LOOSE
#define VSF_USART_CFG_IRQ_MASK_CHECK_UNIQUE             VSF_HAL_CHECK_MODE_LOOSE
#define VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY        ENABLED
#define VSF_USART_CFG_REIMPLEMENT_API_REQUEST           ENABLED
#define VSF_USART_CFG_REIMPLEMENT_API_CTRL              ENABLED
#define VSF_USART_CFG_REIMPLEMENT_API_GET_CONFIGURATION ENABLED
#define VSF_USART_CFG_REIMPLEMENT_API_IRQ_CLEAR         ENABLED

#define VSF_USART_CFG_IMP_LV0(__IDX, __HAL_OP)                                  \
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t)                            \
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart, __IDX) = {                   \
        .reg = (void *)VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX, _USART,    \
                                    __IDX, _REG),                               \
        .irqn = VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX, _USART, __IDX,    \
                             _IRQN),                                            \
        .rst_bit = VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX, _USART,        \
                                __IDX, _RST_BIT),                               \
        .tx_dma = NULL,                                                         \
        .rx_dma = NULL,                                                         \
        .tx_dma_ch = -1,                                                        \
        .rx_dma_ch = -1,                                                        \
        .tx_dreq = VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX, _USART,        \
                                __IDX, _TX_DREQ),                               \
        .rx_dreq = VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX, _USART,        \
                                __IDX, _RX_DREQ),                               \
        __HAL_OP};                                                              \
    void VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX, _USART, __IDX,           \
                      _IRQHandler)(void) {                                      \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_USART_CFG_IMP_PREFIX, _usart_irqhandler)(          \
            &VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart, __IDX));            \
        vsf_hal_irq_leave(ctx);                                                 \
    }

// PL011 aliases RX_IDLE to RX_TIMEOUT (same value).
// Suppress RX_IDLE during uniqueness check so the alias doesn't trigger a duplicate-value assertion.
#undef VSF_USART_IRQ_MASK_RX_IDLE
#include "hal/driver/common/usart/usart_template.inc"
#define VSF_USART_IRQ_MASK_RX_IDLE      VSF_USART_IRQ_MASK_RX_IDLE

#endif      // VSF_HAL_USE_USART
/* EOF */
