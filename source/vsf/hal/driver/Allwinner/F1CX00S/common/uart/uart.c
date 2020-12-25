/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

#include "utilities/vsf_utilities.h"
#include "./uart.h"
#include "../intc/intc.h"

/*============================ MACROS ========================================*/

#if VSF_HAL_USE_USART == ENABLED

#ifndef VSF_USART_CFG_PROTECT_LEVEL
#   define VSF_USART_CFG_PROTECT_LEVEL      interrupt
#endif

#define __vsf_usart_protect                 vsf_protect(VSF_USART_CFG_PROTECT_LEVEL)
#define __vsf_usart_unprotect               vsf_unprotect(VSF_USART_CFG_PROTECT_LEVEL)

#define IIR_IID_MASK                        0x0Ful
#define IIR_IID_MODEM_STATUS                0x00ul // 0b0000
#define IIR_IID_NO_INTERRUPT_PENDING        0x01ul // 0b0001
#define IIR_IID_THR_EMPTY                   0x02ul // 0b0010
#define IIR_IID_RECEIVED_DATA_AVAILABLE     0x04ul // 0b0100
#define IIR_IID_RECEIVED_LINE_STATUS        0x06ul // 0b0110
#define IIR_IID_BUSY_DELECT                 0x07ul // 0b0111
#define IIR_IID_CHARACTER_TIMEOUT           0x0Cul // 0b1100

#if VSF_HAL_USART_IMP_REQUEST_BY_FIFO == ENABLED
#   define vsf_usart_irq_enable  __vsf_usart_irq_enable
#   define vsf_usart_irq_disable __vsf_usart_irq_disable
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __vsf_hw_usart_imp_lv0(__N, __DONT_CARE)                                \
vsf_usart_t vsf_usart##__N = {                                                  \
    .reg = UART##__N##_BASE,                                                    \
    .UART_IRQn = UART##__N##_IRQn,                                              \
};                                                                              \
ROOT void UART##__N##_Handler(void)                                             \
{                                                                               \
    vsf_usart_irqhandler(&vsf_usart##__N);                                      \
}

/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static inline bool __vsf_usart_rx_is_no_empty(uart_reg_t* reg)
{
    return reg->USR & USR_RFNE;
}

static inline uint8_t __vsf_usart_rx_read(uart_reg_t* reg)
{
    return reg->RBR;
}

static inline bool __vsf_usart_tx_is_no_full(uart_reg_t* reg)
{
    return reg->USR & USR_TFNF;
}

static inline void __vsf_usart_tx_write(uart_reg_t* reg, uint8_t byte)
{
    reg->THR = byte;
}

static inline bool __vsf_usart_is_busy(uart_reg_t* reg)
{
    return reg->USR & USR_BUSY;
}

vsf_err_t vsf_usart_init(vsf_usart_t *usart_ptr, usart_cfg_t *cfg_ptr)
{
    uint_fast32_t pclk;
    uint_fast32_t udiv;
    uart_reg_t *reg;

    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    reg = usart_ptr->reg;
    VSF_HAL_ASSERT(reg != NULL);

    pclk = 100UL * 1000 * 1000;
    udiv = pclk / (16 * cfg_ptr->baudrate);

    reg->IER = 0x0;              // disable all interrupt
    reg->FCR = FCR_FIFOE      || // enable fifo
               FCR_RFIFOR     || // receive fifo reset
               FCR_XFIFOR     || // transmit fifo reset
               FCR_TFT_HALF   || // transmit fifo trigger level: 1/2 full
               FCR_RT_1;         // receive fifo trigger level: 1 character

    reg->LCR |= LCR_DLAB;
    reg->DLL = (udiv >> 0) & 0xff;
    reg->DLH = (udiv >> 8) & 0xff;
    reg->LCR &= ~LCR_DLAB;

    reg->LCR =  cfg_ptr->mode & 0x000000FF;
    reg->MCR = (cfg_ptr->mode & 0x0000FF00) >> 8;
    reg->FCR = (cfg_ptr->mode & 0x00FF0000) >> 16 | FCR_FIFOE | FCR_RFIFOR | FCR_XFIFOR;

    if (cfg_ptr->isr.handler_fn != NULL) {
        usart_ptr->isr = cfg_ptr->isr;
        intc_priority_set(usart_ptr->UART_IRQn, (uint32_t)cfg_ptr->isr.prio);
        intc_enable_irq(usart_ptr->UART_IRQn);
    } else {
        intc_disable_irq(usart_ptr->UART_IRQn);
    }

    return VSF_ERR_NONE;
}

fsm_rt_t vsf_usart_enable(vsf_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->reg != NULL);

    return fsm_rt_cpl;
}

fsm_rt_t vsf_usart_disable(vsf_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->reg != NULL);

    return fsm_rt_cpl;
}

usart_status_t vsf_usart_status(vsf_usart_t *usart_ptr)
{
    usart_status_t state;
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->reg != NULL);

    state.is_busy = __vsf_usart_is_busy(usart_ptr->reg);

    return state;
}

uint_fast16_t vsf_usart_fifo_read(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t size)
{
    uint_fast16_t i = 0;
    uint8_t *buf = (uint8_t *)buffer_ptr;
    
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(buffer_ptr != NULL);
    VSF_HAL_ASSERT(size != 0);
    VSF_HAL_ASSERT(usart_ptr->reg != NULL);

    while (__vsf_usart_rx_is_no_empty(usart_ptr->reg) && (i++ < size)) {
        *buf++ = __vsf_usart_rx_read(usart_ptr->reg);
    }

    return i;
}

uint_fast16_t vsf_usart_fifo_write(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t size)
{
    uint_fast16_t i = 0;
    uint8_t *buf = (uint8_t *)buffer_ptr;
    
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(buffer_ptr != NULL);
    VSF_HAL_ASSERT(size != 0);
    VSF_HAL_ASSERT(usart_ptr->reg != NULL);

    while (__vsf_usart_tx_is_no_full(usart_ptr->reg) && (i++ < size)) {
        __vsf_usart_tx_write(usart_ptr->reg, *buf++);
    }

    return i;
}

bool vsf_usart_fifo_flush(vsf_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->reg != NULL);

    while (__vsf_usart_rx_is_no_empty(usart_ptr->reg) || __vsf_usart_tx_is_no_full(usart_ptr->reg));
    return true;
}

void vsf_usart_irqhandler(vsf_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->reg != NULL);
    em_usart_irq_mask_t irq_mask;

    // usart rx
    reg32_t IID = usart_ptr->reg->IIR & IIR_IID_MASK;
    if (IID == IIR_IID_RECEIVED_DATA_AVAILABLE) {
        irq_mask = USART_IRQ_MASK_RX;
    } else if (IID == IIR_IID_THR_EMPTY) {
        irq_mask = USART_IRQ_MASK_TX;
    } else {
        return ;
    }
    
    if (usart_ptr->isr.handler_fn != NULL) {
        usart_ptr->isr.handler_fn(usart_ptr->isr.target_ptr, usart_ptr, irq_mask);
    }
}

void vsf_usart_irq_enable(vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->reg != NULL);
    VSF_HAL_ASSERT(usart_ptr->isr.handler_fn != NULL);
    VSF_HAL_ASSERT((irq_mask == USART_IRQ_MASK_RX) ||
                   (irq_mask == USART_IRQ_MASK_TX) ||
                   (irq_mask == (USART_IRQ_MASK_RX | USART_IRQ_MASK_TX)));
    
    usart_ptr->reg->IER |= irq_mask;
}

void vsf_usart_irq_disable(vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT((irq_mask == USART_IRQ_MASK_RX) ||
                   (irq_mask == USART_IRQ_MASK_TX) ||
                   (irq_mask == (USART_IRQ_MASK_RX | USART_IRQ_MASK_TX)));
    
    usart_ptr->reg->IER &= ~irq_mask;
}

/*============================ INCLUDES ======================================*/

#if VSF_HAL_USART_IMP_REQUEST_BY_FIFO == ENABLED
#   include "hal/driver/common/usart/__usart_common.inc"
#endif

#endif

