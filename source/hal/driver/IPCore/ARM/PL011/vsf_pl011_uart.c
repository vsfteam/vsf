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

#if VSF_HAL_USE_USART == ENABLED

#define __VSF_HAL_PL011_UART_CLASS_IMPLEMENT
#include "hal/vsf_hal.h"

// if __HAL_DRIVER_PL011_UART_H__ is defined, means chip driver include "vsf_pl011_uart.h".
#ifdef __HAL_DRIVER_PL011_UART_H__

#include "vsf_pl011_uart_reg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_pl011_usart_init(vsf_pl011_usart_t *pl011_usart_ptr, vsf_usart_cfg_t *cfg_ptr, uint_fast32_t clk_hz)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    vsf_pl011_usart_reg_t *reg = pl011_usart_ptr->reg;
    pl011_usart_ptr->isr = cfg_ptr->isr;

    // bardrate
    uint32_t baud_rate_div = (8 * clk_hz / cfg_ptr->baudrate);
    uint32_t baud_ibrd = baud_rate_div >> 7;
    uint32_t baud_fbrd;
    if (baud_ibrd == 0) {
        baud_ibrd = 1;
        baud_fbrd = 0;
    } else if (baud_ibrd >= 65535) {
        baud_ibrd = 65535;
        baud_fbrd = 0;
    }  else {
        baud_fbrd = ((baud_rate_div & 0x7f) + 1) / 2;
    }
    reg->UARTIBRD.VALUE = baud_ibrd;
    reg->UARTFBRD.VALUE = baud_fbrd;

    // mode | FEN(Enable FIFOs, bit4)
    reg->UARTLCR_H.VALUE = (cfg_ptr->mode & PL011_USART_MODE_MASK) | (1 << 4);
    // enable
    reg->UARTCR.VALUE = (cfg_ptr->mode >> 8) & PL011_USART_ENABLE_MASK;

    return VSF_ERR_NONE;
}

void vsf_pl011_usart_fini(vsf_pl011_usart_t *pl011_usart_ptr)
{
    VSF_HAL_ASSERT(pl011_usart_ptr != NULL);
    vsf_pl011_usart_reg_t *reg = pl011_usart_ptr->reg;
    reg->UARTCR.UARTEN = 0;
}

vsf_usart_capability_t vsf_pl011_usart_capability(vsf_pl011_usart_t *pl011_usart_ptr, uint_fast32_t clk_hz)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    return (vsf_usart_capability_t){
        .irq_mask               = PL011_USART_IRQ_MASK,
        .max_baudrate           = clk_hz / 1,
        .min_baudrate           = clk_hz / 65535,
        .min_data_bits          = 5,
        .max_data_bits          = 8,
        .max_tx_fifo_counter    = 32,
        .max_rx_fifo_counter    = 32,
        .support_rx_timeout     = true,
    };
}

fsm_rt_t vsf_pl011_usart_enable(vsf_pl011_usart_t *pl011_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    vsf_pl011_usart_reg_t *reg = pl011_usart_ptr->reg;
    reg->UARTCR.UARTEN = 1;
    return fsm_rt_cpl;
}

fsm_rt_t vsf_pl011_usart_disable(vsf_pl011_usart_t *pl011_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    vsf_pl011_usart_reg_t *reg = pl011_usart_ptr->reg;
    reg->UARTCR.UARTEN = 0;
    return fsm_rt_cpl;
}

vsf_usart_irq_mask_t vsf_pl011_usart_irq_enable(vsf_pl011_usart_t *pl011_usart_ptr, vsf_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    vsf_pl011_usart_reg_t *reg = pl011_usart_ptr->reg;
    reg->UARTIMSC.VALUE |= irq_mask & PL011_USART_IRQ_MASK;
    return reg->UARTIMSC.VALUE;
}

vsf_usart_irq_mask_t vsf_pl011_usart_irq_disable(vsf_pl011_usart_t *pl011_usart_ptr, vsf_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    vsf_pl011_usart_reg_t *reg = pl011_usart_ptr->reg;
    reg->UARTIMSC.VALUE &= ~(irq_mask & PL011_USART_IRQ_MASK);
    return reg->UARTIMSC.VALUE;
}

vsf_usart_status_t vsf_pl011_usart_status(vsf_pl011_usart_t *pl011_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    vsf_pl011_usart_reg_t *reg = pl011_usart_ptr->reg;
    return (vsf_usart_status_t){
        .value                  = reg->UARTFR.VALUE,
    };
}

uint_fast16_t vsf_pl011_usart_rxfifo_get_data_count(vsf_pl011_usart_t *pl011_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    vsf_pl011_usart_reg_t *reg = pl011_usart_ptr->reg;
    return reg->UARTFR.RXFE ? 0 : 1;
}

uint_fast16_t vsf_pl011_usart_rxfifo_read(vsf_pl011_usart_t *pl011_usart_ptr, void *buffer_ptr, uint_fast16_t count)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    vsf_pl011_usart_reg_t *reg = pl011_usart_ptr->reg;
    uint_fast16_t realcnt = 0;
    uint8_t *buffer8_ptr = buffer_ptr;
    while ((count > realcnt) && !reg->UARTFR.RXFE) {
        *buffer8_ptr++ = reg->UARTDR.VALUE;
        realcnt++;
    }
    return realcnt;
}

uint_fast16_t vsf_pl011_usart_txfifo_get_free_count(vsf_pl011_usart_t *pl011_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    vsf_pl011_usart_reg_t *reg = pl011_usart_ptr->reg;
    return reg->UARTFR.TXFF ? 0 : 1;
}

uint_fast16_t vsf_pl011_usart_txfifo_write(vsf_pl011_usart_t *pl011_usart_ptr, void *buffer_ptr, uint_fast16_t count)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    vsf_pl011_usart_reg_t *reg = pl011_usart_ptr->reg;
    uint_fast16_t realcnt = 0;
    uint8_t *buffer8_ptr = buffer_ptr;
    while ((count > realcnt) && !reg->UARTFR.TXFF) {
        reg->UARTDR.VALUE = *buffer8_ptr++;
        realcnt++;
    }
    return realcnt;
}

uintptr_t vsf_pl011_usart_rxdma_config(vsf_pl011_usart_t *pl011_usart_ptr, bool enable)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    vsf_pl011_usart_reg_t *reg = pl011_usart_ptr->reg;
    reg->UARTDMACR.RXDMAE = enable;
    return (uintptr_t)&reg->UARTDR;
}

uintptr_t vsf_pl011_usart_txdma_config(vsf_pl011_usart_t *pl011_usart_ptr, bool enable)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    vsf_pl011_usart_reg_t *reg = pl011_usart_ptr->reg;
    reg->UARTDMACR.TXDMAE = enable;
    return (uintptr_t)&reg->UARTDR;
}

void vsf_pl011_usart_irqhandler(vsf_pl011_usart_t *pl011_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    vsf_pl011_usart_reg_t *reg = pl011_usart_ptr->reg;
    vsf_usart_irq_mask_t mask = reg->UARTMIS.VALUE;
    reg->UARTICR.VALUE = mask;
    if (mask && (pl011_usart_ptr->isr.handler_fn != NULL)) {
        pl011_usart_ptr->isr.handler_fn(pl011_usart_ptr, pl011_usart_ptr->isr.target_ptr, mask);
    }
}

#endif      // __HAL_DRIVER_PL011_UART_H__
#endif      // VSF_HAL_USE_USART
