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

#include "hal/vsf_hal.h"

#ifdef FALSE
#   undef FALSE
#endif
#ifdef TRUE
#   undef TRUE
#endif
#include "air105.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_USART_CFG_MULTI_CLASS
#   define VSF_HW_USART_CFG_MULTI_CLASS             VSF_USART_CFG_MULTI_CLASS
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_hw_usart_const_t {
    IRQn_Type irqn;
    UART_TypeDef *reg;
    uint8_t periph_idx;
} vsf_hw_usart_const_t;

typedef struct vsf_hw_usart_t {
#if VSF_HW_USART_CFG_MULTI_CLASS == ENABLED
    vsf_usart_t vsf_usart;
#endif
    const vsf_hw_usart_const_t *usart_const;

    vsf_usart_isr_t isr;
} vsf_hw_usart_t;

/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_hw_usart_init(vsf_hw_usart_t *hw_usart_ptr, vsf_usart_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    const vsf_hw_usart_const_t *usart_const = hw_usart_ptr->usart_const;
    VSF_HAL_ASSERT(NULL != usart_const);
    UART_TypeDef *reg = usart_const->reg;
    VSF_HAL_ASSERT(NULL != reg);

    SYSCTRL->SOFT_RST1 = 1 << usart_const->periph_idx;
    while (SYSCTRL->SOFT_RST1 & (1 << usart_const->periph_idx));

    {
        extern uint32_t SystemCoreClock;
        uint32_t tmp_baud_div = (SystemCoreClock >> 6) / cfg_ptr->baudrate;
        reg->LCR |= UART_LCR_DLAB;
        reg->OFFSET_0.DLL = (tmp_baud_div >> 0) & 0x00FF;
        reg->OFFSET_4.DLH = (tmp_baud_div >> 8) & 0x00FF;
        reg->LCR &= ~UART_LCR_DLAB;
    }

    reg->LCR = cfg_ptr->mode & AIR105_USART_MODE_ALL_BITS_MASK;

    reg->SFE = 0;
    reg->SRT = 2;
    reg->STET = 0;
    reg->SFE |= UART_SFE_SFE;

    vsf_usart_isr_t *isr_ptr = &cfg_ptr->isr;
    hw_usart_ptr->isr = *isr_ptr;

    if (isr_ptr->handler_fn != NULL) {
        NVIC_SetPriority(usart_const->irqn, (uint32_t)isr_ptr->prio);
        NVIC_EnableIRQ(usart_const->irqn);
    } else {
        NVIC_DisableIRQ(usart_const->irqn);
    }

    return VSF_ERR_NONE;
}

void vsf_hw_usart_fini(vsf_hw_usart_t *hw_usart_ptr)
{
    VSF_HAL_ASSERT(hw_usart_ptr != NULL);
}

vsf_usart_capability_t vsf_hw_usart_capability(vsf_hw_usart_t *hw_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    const vsf_hw_usart_const_t *usart_const = hw_usart_ptr->usart_const;
    VSF_HAL_ASSERT(NULL != usart_const);

    extern uint32_t SystemCoreClock;
    vsf_usart_capability_t usart_capability = {
        .irq_mask = AIR105_USART_MODE_ALL_BITS_MASK,
        .max_baudrate = (SystemCoreClock >> 6) / (16 * 1),
        .min_baudrate = (SystemCoreClock >> 6) / (16 * 65535),
        .min_data_bits = 5,
        .max_data_bits = 8,
        .max_tx_fifo_counter = 16,
        .max_rx_fifo_counter = 16,
        .support_rx_timeout = 0,
    };

    return usart_capability;
}

fsm_rt_t vsf_hw_usart_enable(vsf_hw_usart_t *hw_usart_ptr)
{
    return fsm_rt_cpl;
}

fsm_rt_t vsf_hw_usart_disable(vsf_hw_usart_t *hw_usart_ptr)
{
    return fsm_rt_cpl;
}

void vsf_hw_usart_irq_enable(vsf_hw_usart_t *hw_usart_ptr, vsf_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    const vsf_hw_usart_const_t *usart_const = hw_usart_ptr->usart_const;
    VSF_HAL_ASSERT(NULL != usart_const);
    VSF_HAL_ASSERT(NULL != usart_const->reg);
    VSF_HAL_ASSERT((irq_mask & ~AIR105_USART_IRQ_MASK) == 0);

    usart_const->reg->OFFSET_4.IER |= irq_mask;
}

void vsf_hw_usart_irq_disable(vsf_hw_usart_t *hw_usart_ptr, vsf_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    const vsf_hw_usart_const_t *usart_const = hw_usart_ptr->usart_const;
    VSF_HAL_ASSERT(NULL != usart_const);
    VSF_HAL_ASSERT(NULL != usart_const->reg);
    VSF_HAL_ASSERT((irq_mask & ~AIR105_USART_IRQ_MASK) == 0);

    usart_const->reg->OFFSET_4.IER &= ~irq_mask;
}

static bool __hw_usart_read_fifo_is_empty(vsf_hw_usart_t *hw_usart_ptr)
{
    return !(hw_usart_ptr->usart_const->reg->USR & UART_USR_RFNE);
}

static bool __hw_usart_write_fifo_is_full(vsf_hw_usart_t *hw_usart_ptr)
{
    return !(hw_usart_ptr->usart_const->reg->USR & UART_USR_TFNF);
}

vsf_usart_status_t vsf_hw_usart_status(vsf_hw_usart_t *hw_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);

    vsf_usart_status_t status = {
        .is_busy = !!(hw_usart_ptr->usart_const->reg->USR & UART_USR_BUSY),
    };

    return status;
}

uint_fast16_t vsf_hw_usart_rxfifo_get_data_count(vsf_hw_usart_t *hw_usart_ptr)
{
    return hw_usart_ptr->usart_const->reg->RFL & UART_RFL_RFL;
}

uint_fast16_t vsf_hw_usart_rxfifo_read(vsf_hw_usart_t *hw_usart_ptr, void *buffer_ptr, uint_fast16_t count)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    const vsf_hw_usart_const_t *usart_const = hw_usart_ptr->usart_const;
    VSF_HAL_ASSERT(NULL != usart_const);
    UART_TypeDef *reg = usart_const->reg;
    VSF_HAL_ASSERT(NULL != reg);

    uint8_t *buf_ptr = (uint8_t *)buffer_ptr;
    VSF_HAL_ASSERT(NULL != buf_ptr);

    uint_fast16_t i = 0;
    while (i < count) {
        if (__hw_usart_read_fifo_is_empty(hw_usart_ptr)) {
            break;
        }
        buf_ptr[i++] = reg->OFFSET_0.RBR;
    }
    return i;
}

uint_fast16_t vsf_hw_usart_txfifo_get_free_count(vsf_hw_usart_t *hw_usart_ptr)
{
    return 16 - (hw_usart_ptr->usart_const->reg->TFL & UART_TFL_TFL);
}

uint_fast16_t vsf_hw_usart_txfifo_write(vsf_hw_usart_t *hw_usart_ptr, void *buffer_ptr, uint_fast16_t count)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    const vsf_hw_usart_const_t *usart_const = hw_usart_ptr->usart_const;
    VSF_HAL_ASSERT(NULL != usart_const);
    UART_TypeDef *reg = usart_const->reg;
    VSF_HAL_ASSERT(NULL != reg);

    uint8_t *buf_ptr = (uint8_t *) buffer_ptr;
    VSF_HAL_ASSERT(NULL != buf_ptr);

    uint_fast16_t i = 0;
    while (i < count) {
        if (__hw_usart_write_fifo_is_full(hw_usart_ptr)) {
            break;
        }
        reg->OFFSET_0.THR = buf_ptr[i++];
    }
    return i;
}

static vsf_usart_irq_mask_t __get_uart_irq_mask(vsf_hw_usart_t *hw_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    const vsf_hw_usart_const_t *usart_const = hw_usart_ptr->usart_const;
    VSF_HAL_ASSERT(NULL != usart_const);
    UART_TypeDef *reg = usart_const->reg;
    VSF_HAL_ASSERT(NULL != reg);

    uint32_t value;
    uint32_t irq_type = reg->OFFSET_8.IIR & UART_IIR_IID;

    switch (irq_type) {
    case 0x0:       // Modem Status Interrupt
        value = reg->MSR;
        (void)value;
        return 0;

    case 0x1:       // No interrupt
        return 0;

    case 0x2:       // Tx FIFO Empty
        return VSF_USART_IRQ_MASK_TX;

    case 0x4:       // Rx FIFO valid
        return VSF_USART_IRQ_MASK_RX;

    case 0x6:       // Line status interrupt
        value = reg->LSR;
        (void)value;
        return 0;

    case 0xC:       // Rx timeout interrupt
        value = reg->OFFSET_0.RBR;
        (void)value;
        return VSF_USART_IRQ_MASK_RX_TIMEOUT;

    default:
        VSF_HAL_ASSERT(0);
        return (vsf_usart_irq_mask_t)0;
    }
}

static void __vsf_hw_usart_irq_handler(vsf_hw_usart_t *hw_usart_ptr)
{
    vsf_usart_irq_mask_t irq_mask = __get_uart_irq_mask(hw_usart_ptr);
    if (irq_mask & VSF_USART_IRQ_ALL_BITS_MASK) {
        if (NULL != hw_usart_ptr->isr.handler_fn) {
            hw_usart_ptr->isr.handler_fn(hw_usart_ptr->isr.target_ptr, (vsf_usart_t *)hw_usart_ptr, irq_mask);
        }
    }
}

/*============================ INCLUDES ======================================*/

#define VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY    ENABLED
#define VSF_USART_CFG_IMP_PREFIX                    __vsf_hw
#define VSF_USART_CFG_IMP_UPCASE_PREFIX             __VSF_HW
#define VSF_USART_CFG_IMP_COUNT_MASK_PREFIX         VSF_HW
#define VSF_USART_CFG_IMP_LV0(__IDX, __HAL_OP)                                  \
    static const vsf_hw_usart_const_t __vsf_hw_usart ## __IDX ## _clock = {     \
        .reg        = UART ## __IDX,                                            \
        .irqn       = UART ## __IDX ## _IRQn,                                   \
        .periph_idx = __IDX,                                                    \
    };                                                                          \
    vsf_hw_usart_t __vsf_hw_usart ## __IDX = {                                  \
        .usart_const  = &__vsf_hw_usart ## __IDX ## _clock,                     \
        __HAL_OP                                                                \
    };                                                                          \
    void UART ## __IDX ## _Handler(void)                                        \
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        __vsf_hw_usart_irq_handler(&__vsf_hw_usart ## __IDX);                   \
        vsf_hal_irq_leave(ctx);                                                 \
    }
#include "hal/driver/common/usart/usart_template.inc"

#define VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY    ENABLED
#define VSF_USART_CFG_REIMPLEMENT_API_REQUEST       ENABLED
#define VSF_USART_CFG_IMP_PREFIX                    vsf_hw
#define VSF_USART_CFG_IMP_UPCASE_PREFIX             vsf_hw
#define VSF_USART_CFG_IMP_REMAP_PREFIX              vsf_fifo2req
#define VSF_USART_CFG_IMP_LV0(__COUNT, __HAL_OP)                                \
    describe_fifo2req_usart(VSF_USART_CFG_IMP_PREFIX,                           \
                            vsf_hw_usart ## __COUNT, __vsf_hw_usart ## __COUNT)
#include "hal/driver/common/usart/usart_template.inc"

#endif      // VSF_HAL_USE_USART
