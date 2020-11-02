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
/*============================ MACROFIED FUNCTIONS ===========================*/
/*uart_irqhandler*/
#define ____USART_LOCAL(__N, __DONT_CARE)                                       \
vsf_usart_t vsf_usart##__N = {                                                  \
    .reg = UART##__N##_BASE,                                                    \
    .UART_IRQn = UART##__N##_IRQn,                                              \
};                                                                              \
ROOT void UART##__N##_Handler(void)                                             \
{                                                                               \
    vsf_usart_irqhandler(&vsf_usart##__N);                                      \
}
#define __USART_LOCAL(__N, __DONT_CARE)                                         \
            ____USART_LOCAL(__N, __DONT_CARE)
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static inline void __vsf_usart_rx_interrupt_enable(uart_reg_t* reg)
{
    reg->IER |= IER_ERBFI;
}

static inline void __vsf_usart_rx_interrupt_disable(uart_reg_t* reg)
{
    reg->IER &= ~IER_ERBFI;
}

static inline void __vsf_usart_tx_interrupt_enable(uart_reg_t* reg)
{
    reg->IER |= IER_ETBEI;
}

static inline void __vsf_usart_tx_interrupt_disable(uart_reg_t* reg)
{
    reg->IER &= ~IER_ETBEI;
}

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

static bool __vsf_usart_isr_run(vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT((irq_mask == USART_IRQ_MASK_RX) ||
                   (irq_mask == USART_IRQ_MASK_TX) ||
                   (irq_mask == USART_IRQ_MASK_RX_CPL) ||
                   (irq_mask == USART_IRQ_MASK_TX_CPL));

    if((usart_ptr->isr.handler_fn != NULL) && (usart_ptr->irq_mask & irq_mask)) {
        usart_ptr->isr.handler_fn(usart_ptr->isr.target_ptr, usart_ptr, irq_mask);
        return true;
    } else {
        return false;
    }
}


static void __vsf_usart_request_init(vsf_usart_request_t* request,
                                     uint8_t *buffer_ptr, uint_fast32_t max_count)
{
    request->is_busy      = true;
    request->buffer_ptr   = buffer_ptr;
    request->max_count    = max_count;
    request->count        = 0;
}

static inline void __vsf_usart_request_write(vsf_usart_request_t* request, uint8_t byte)
{
    request->buffer_ptr[request->count++] = byte;
}

static inline uint8_t __vsf_usart_request_read(vsf_usart_request_t* request)
{
    return request->buffer_ptr[request->count++];
}


static inline bool __vsf_usart_request_is_busy(vsf_usart_request_t* request)
{
    return request->is_busy;
}

static inline void __vsf_usart_request_set_busy(vsf_usart_request_t* request, bool is_busy)
{
    request->is_busy = is_busy;
}

static inline bool __vsf_usart_request_is_finish(vsf_usart_request_t* request)
{
    return request->count >= request->max_count;
}

static inline intalu_t __vsf_usart_request_get_count(vsf_usart_request_t* request)
{
    return request->count;
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

    // enable usart clock
    // should call
    //PM.Clock.SyncClock.Enable(usart_ptr->sync_clk_idx);
    // to replace the following
        // TODO

    // there should be no IO configuration here
        // TODO

    // should call
    //PM.Clock.Peripheral.Config(usart_ptr->pclk_idx, (pm_periph_asyn_clk_cfg_t) {...} );
    // to configure the PCLK for current usart, it is missing here
        // TODO

    // calculate the baud rate divider
    // should call
    //PM.Clock.Peripheral.GetClock(usart_ptr->pclk_idx);
    // to replace the PCLKn register reading
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

    reg->LCR = cfg_ptr->mode & 0xFF;
    reg->MCR = (cfg_ptr->mode >> 8) & 0xFF;

    // interrupt control configuration
    intc_enable_irq(usart_ptr->UART_IRQn);

    // should call
    //PM.Clock.SyncClock.Disable(usart_ptr->sync_clk_idx);
    // to disable SyncClock for current USART, user should enable this usart
    // before use it

    return VSF_ERR_NONE;
}

fsm_rt_t vsf_usart_enable(vsf_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->reg != NULL);
    //! don't forget to enable sync clock and peripheral_clk here
    // should call
    //PM.Clock.SyncClock.Enable(usart_ptr->sync_clk_idx);
    // to enable sync clock

    // should call
    //PM.Clock.Peripheral.Enable(usart_ptr->pclk_idx );
    // to enable PCLK for current usart

    return fsm_rt_cpl;
}

fsm_rt_t vsf_usart_disable(vsf_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->reg != NULL);

    // should call
    //PM.Clock.Peripheral.Disable(usart_ptr->pclk_idx );
    // to disable PCLK for current usart

    // should call
    //PM.Clock.SyncClock.Disable(usart_ptr->sync_clk_idx);
    // to disable sync clock

    //! don't forget to disable sync clock and peripheral_clk here

    return fsm_rt_cpl;
}

usart_status_t vsf_usart_status(vsf_usart_t *usart_ptr)
{
    usart_status_t state;
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->reg != NULL);

    if ( __vsf_usart_is_busy(usart_ptr->reg) ||
         __vsf_usart_request_is_busy(&usart_ptr->rx) ||
         __vsf_usart_request_is_busy(&usart_ptr->tx)) {
        state.is_busy = true;
    } else {
        state.is_busy = false;
    }

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

    while (__vsf_usart_rx_is_no_empty(usart_ptr->reg) && (i < size)) {
        *buf++ = __vsf_usart_rx_read(usart_ptr->reg);
        i++;
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

    while (__vsf_usart_tx_is_no_full(usart_ptr->reg) && (i < size)) {
        __vsf_usart_tx_write(usart_ptr->reg, *buf++);
        i++;
    }

    return i;
}

bool vsf_usart_fifo_flush(vsf_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->reg != NULL);
    // TODO

    return true;
}

vsf_err_t vsf_usart_request_rx(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t size)
{
    vsf_err_t err;
    vsf_protect_t state;

    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->reg != NULL);

    state = __vsf_usart_protect();

    if (__vsf_usart_request_is_busy(&usart_ptr->rx)) {
        err = VSF_ERR_REQ_ALREADY_REGISTERED;
    } else {
        __vsf_usart_request_init(&usart_ptr->rx, buffer_ptr, size);
        __vsf_usart_rx_interrupt_enable(usart_ptr->reg);
        err = VSF_ERR_NONE;
    }

    __vsf_usart_unprotect(state);

    return err;
}

vsf_err_t vsf_usart_request_tx(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t size)
{
    vsf_err_t err;
    vsf_protect_t state;

    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->reg != NULL);

    state = __vsf_usart_protect();

    if (__vsf_usart_request_is_busy(&usart_ptr->tx)) {
        err = VSF_ERR_REQ_ALREADY_REGISTERED;
    } else {
        __vsf_usart_request_init(&usart_ptr->tx, buffer_ptr, size);
        __vsf_usart_tx_interrupt_enable(usart_ptr->reg);
        err = VSF_ERR_NONE;
    }

    __vsf_usart_unprotect(state);

    return err;
}

vsf_err_t vsf_usart_cancel_rx(vsf_usart_t *usart_ptr)
{
    vsf_protect_t state;

    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->reg != NULL);

    state = __vsf_usart_protect();
    __vsf_usart_tx_interrupt_disable(usart_ptr->reg);
    __vsf_usart_unprotect(state);

    return VSF_ERR_NONE;
}

vsf_err_t vsf_usart_cancel_tx(vsf_usart_t *usart_ptr)
{
    vsf_protect_t state;
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->reg != NULL);

    state = __vsf_usart_protect();
    __vsf_usart_tx_interrupt_disable(usart_ptr->reg);
    __vsf_usart_unprotect(state);

    return VSF_ERR_NONE;
}

int_fast32_t vsf_usart_get_rx_count(vsf_usart_t *usart_ptr)
{
    int_fast32_t count;
    vsf_protect_t state;
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->reg != NULL);

    state = __vsf_usart_protect();
    count = __vsf_usart_request_get_count(&usart_ptr->rx);
    __vsf_usart_unprotect(state);

    return count;
}

int_fast32_t vsf_usart_get_tx_count(vsf_usart_t *usart_ptr)
{
    int_fast32_t count;
    vsf_protect_t state;
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->reg != NULL);

    state = __vsf_usart_protect();
    count = __vsf_usart_request_get_count(&usart_ptr->tx);
    __vsf_usart_unprotect(state);

    return count;
}

void vsf_usart_irqhandler(vsf_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);

    // usart rx
    if (__vsf_usart_rx_is_no_empty(usart_ptr->reg)) {
        // request to read or fifo read?
        if (__vsf_usart_request_is_busy(&usart_ptr->rx)) {
            do {
                __vsf_usart_request_write(&usart_ptr->rx, __vsf_usart_rx_read(usart_ptr->reg));
                if (__vsf_usart_request_is_finish(&usart_ptr->rx)) {
                    __vsf_usart_rx_interrupt_disable(usart_ptr->reg);

                    // rx send event and update request status
                    __vsf_usart_isr_run(usart_ptr, USART_IRQ_MASK_RX_CPL);
                    __vsf_usart_request_set_busy(&usart_ptr->rx, false);
                    break;
                }
            } while (__vsf_usart_rx_is_no_empty(usart_ptr->reg));
        } else {
            __vsf_usart_isr_run(usart_ptr, USART_IRQ_MASK_RX);
        }
    }

    // usart tx
    if (__vsf_usart_tx_is_no_full(usart_ptr->reg)) {
        if (__vsf_usart_request_is_busy(&usart_ptr->tx)) {
            do  {
                __vsf_usart_tx_write(usart_ptr->reg, __vsf_usart_request_read(&usart_ptr->tx));
                if (__vsf_usart_request_is_finish(&usart_ptr->tx)) {
                    __vsf_usart_tx_interrupt_disable(usart_ptr->reg);

                    // tx send event and update request status
                    __vsf_usart_isr_run(usart_ptr, USART_IRQ_MASK_TX_CPL);
                    __vsf_usart_request_set_busy(&usart_ptr->tx, false);
                    break;
                }
            } while (__vsf_usart_tx_is_no_full(usart_ptr->reg));
        } else {
            __vsf_usart_isr_run(usart_ptr, USART_IRQ_MASK_TX);
        }
    }
}

void vsf_usart_irq_enable(vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->isr.handler_fn != NULL);
    VSF_HAL_ASSERT((irq_mask == USART_IRQ_MASK_RX) ||
                   (irq_mask == USART_IRQ_MASK_TX) ||
                   (irq_mask == USART_IRQ_MASK_RX_CPL) ||
                   (irq_mask == USART_IRQ_MASK_TX_CPL));
    
    usart_ptr->irq_mask |= irq_mask;
}

void vsf_usart_irq_disable(vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT((irq_mask == USART_IRQ_MASK_RX) ||
                   (irq_mask == USART_IRQ_MASK_TX) ||
                   (irq_mask == USART_IRQ_MASK_RX_CPL) ||
                   (irq_mask == USART_IRQ_MASK_TX_CPL));

    usart_ptr->irq_mask &= ~irq_mask;
}

#if USART_COUNT > 0 && VSF_HAL_USE_USART0 == ENABLED

__VSF_USART_LV1_IMPL(0)
____USART_LOCAL(0, NULL)

#endif

#if USART_COUNT > 1 && VSF_HAL_USE_USART1 == ENABLED

__VSF_USART_LV1_IMPL(1)
____USART_LOCAL(1, NULL)

#endif

#if USART_COUNT > 2 && VSF_HAL_USE_USART2 == ENABLED

__VSF_USART_LV1_IMPL(2)
____USART_LOCAL(2, NULL)

#endif

#if USART_COUNT > 3 && VSF_HAL_USE_USART3 == ENABLED

__VSF_USART_LV1_IMPL(3)
____USART_LOCAL(3, NULL)

#endif

#if USART_COUNT > 4 && VSF_HAL_USE_USART4 == ENABLED

__VSF_USART_LV1_IMPL(4)
____USART_LOCAL(4, NULL)

#endif

#if USART_COUNT > 5 && VSF_HAL_USE_USART5 == ENABLED

__VSF_USART_LV1_IMPL(5)
____USART_LOCAL(5, NULL)

#endif

#if USART_COUNT > 6 && VSF_HAL_USE_USART6 == ENABLED

__VSF_USART_LV1_IMPL(6)
____USART_LOCAL(6, NULL)

#endif

#if USART_COUNT >= 7 && VSF_HAL_USE_USART7 == ENABLED

__VSF_USART_LV1_IMPL(7)
____USART_LOCAL(7, NULL)

#endif

#endif

