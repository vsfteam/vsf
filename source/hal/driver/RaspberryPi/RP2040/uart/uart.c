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

#include "../driver.h"

#if VSF_HAL_USE_USART == ENABLED

#define __VSF_HAL_PL011_UART_CLASS_INHERIT__

#include "hal/vsf_hal.h"

// for UART IRQn
#include "RP2040.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_USART_CFG_MULTI_CLASS
#   define VSF_HW_USART_CFG_MULTI_CLASS             VSF_USART_CFG_MULTI_CLASS
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_hw_usart_t {
    implement(vsf_pl011_usart_t)
    IRQn_Type irqn;
} vsf_hw_usart_t;

/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_hw_usart_init(vsf_hw_usart_t *hw_usart_ptr, vsf_usart_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    vsf_err_t err = vsf_pl011_usart_init(&hw_usart_ptr->use_as__vsf_pl011_usart_t, cfg_ptr, clock_get_hz(clk_peri));

    vsf_usart_isr_t *isr_ptr = &cfg_ptr->isr;
    if (isr_ptr->handler_fn != NULL) {
        NVIC_SetPriority(hw_usart_ptr->irqn, (uint32_t)isr_ptr->prio);
        NVIC_EnableIRQ(hw_usart_ptr->irqn);
    } else {
        NVIC_DisableIRQ(hw_usart_ptr->irqn);
    }

    return err;
}

void vsf_hw_usart_fini(vsf_hw_usart_t *hw_usart_ptr)
{
    VSF_HAL_ASSERT(hw_usart_ptr != NULL);
    vsf_pl011_usart_fini(&hw_usart_ptr->use_as__vsf_pl011_usart_t);
}

vsf_usart_capability_t vsf_hw_usart_capability(vsf_hw_usart_t *hw_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    vsf_usart_capability_t cap = vsf_pl011_usart_capability(&hw_usart_ptr->use_as__vsf_pl011_usart_t, clock_get_hz(clk_peri));
    cap.irq_mask |= VSF_USART_IRQ_MASK_TX_CPL | VSF_USART_IRQ_MASK_RX_CPL;
    return cap;
}

fsm_rt_t vsf_hw_usart_enable(vsf_hw_usart_t *hw_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    return vsf_pl011_usart_enable(&hw_usart_ptr->use_as__vsf_pl011_usart_t);
}

fsm_rt_t vsf_hw_usart_disable(vsf_hw_usart_t *hw_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    return vsf_pl011_usart_disable(&hw_usart_ptr->use_as__vsf_pl011_usart_t);
}

void vsf_hw_usart_irq_enable(vsf_hw_usart_t *hw_usart_ptr, vsf_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    vsf_pl011_usart_irq_enable(&hw_usart_ptr->use_as__vsf_pl011_usart_t, irq_mask);
}

void vsf_hw_usart_irq_disable(vsf_hw_usart_t *hw_usart_ptr, vsf_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    vsf_pl011_usart_irq_disable(&hw_usart_ptr->use_as__vsf_pl011_usart_t, irq_mask);
}

vsf_usart_status_t vsf_hw_usart_status(vsf_hw_usart_t *hw_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    return vsf_pl011_usart_status(&hw_usart_ptr->use_as__vsf_pl011_usart_t);
}

uint_fast16_t vsf_hw_usart_rxfifo_get_data_count(vsf_hw_usart_t *hw_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    return vsf_pl011_usart_rxfifo_get_data_count(&hw_usart_ptr->use_as__vsf_pl011_usart_t);
}

uint_fast16_t vsf_hw_usart_rxfifo_read(vsf_hw_usart_t *hw_usart_ptr, void *buffer_ptr, uint_fast16_t count)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    return vsf_pl011_usart_rxfifo_read(&hw_usart_ptr->use_as__vsf_pl011_usart_t, buffer_ptr, count);
}

uint_fast16_t vsf_hw_usart_txfifo_get_free_count(vsf_hw_usart_t *hw_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    return vsf_pl011_usart_txfifo_get_free_count(&hw_usart_ptr->use_as__vsf_pl011_usart_t);
}

uint_fast16_t vsf_hw_usart_txfifo_write(vsf_hw_usart_t *hw_usart_ptr, void *buffer_ptr, uint_fast16_t count)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    return vsf_pl011_usart_txfifo_write(&hw_usart_ptr->use_as__vsf_pl011_usart_t, buffer_ptr, count);
}

vsf_err_t vsf_hw_usart_request_rx(vsf_hw_usart_t *hw_usart_ptr, void *buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    uintptr_t srcaddr = vsf_pl011_usart_rxdma_config(&hw_usart_ptr->use_as__vsf_pl011_usart_t, true);
    // TODO: setup DMA
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_usart_request_tx(vsf_hw_usart_t *hw_usart_ptr, void *buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    uintptr_t dstaddr = vsf_pl011_usart_txdma_config(&hw_usart_ptr->use_as__vsf_pl011_usart_t, true);
    // TODO: setup DMA
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_usart_cancel_rx(vsf_hw_usart_t *hw_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_usart_cancel_tx(vsf_hw_usart_t *hw_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    return VSF_ERR_NONE;
}

int_fast32_t vsf_hw_usart_get_rx_count(vsf_hw_usart_t *hw_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    return 0;
}

int_fast32_t vsf_hw_usart_get_tx_count(vsf_hw_usart_t *hw_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    return 0;
}

/*============================ INCLUDES ======================================*/

#define VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY    ENABLED
#define VSF_USART_CFG_REIMPLEMENT_API_REQUEST       ENABLED
#define VSF_USART_CFG_IMP_PREFIX                vsf_hw
#define VSF_USART_CFG_IMP_UPCASE_PREFIX         VSF_HW
#define VSF_USART_CFG_IMP_LV0(__IDX, __HAL_OP)                                  \
    vsf_hw_usart_t vsf_hw_usart ## __IDX = {                                    \
        .reg  = (void *)VSF_HW_USART ## __IDX ## _REG,                          \
        .irqn = VSF_HW_USART ## __IDX ## _IRQN,                                 \
        __HAL_OP                                                                \
    };                                                                          \
    void VSF_HW_USART ## __IDX ## _IRQHandler(void)                             \
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        vsf_pl011_usart_irqhandler(&vsf_hw_usart ## __IDX .use_as__vsf_pl011_usart_t);\
        vsf_hal_irq_leave(ctx);                                                 \
    }
#include "hal/driver/common/usart/usart_template.inc"

#endif      // VSF_HAL_USE_USART
