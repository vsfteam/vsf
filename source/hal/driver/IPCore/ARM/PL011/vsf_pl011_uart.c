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

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_pl011_usart_init(vsf_pl011_usart_t *pl011_usart_ptr, vsf_usart_cfg_t *cfg_ptr, uint_fast32_t clk_hz)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    return VSF_ERR_NONE;
}

void vsf_pl011_usart_fini(vsf_pl011_usart_t *pl011_usart_ptr)
{
    VSF_HAL_ASSERT(pl011_usart_ptr != NULL);
}

vsf_usart_capability_t vsf_pl011_usart_capability(vsf_pl011_usart_t *pl011_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    return (vsf_usart_capability_t){ 0 };
}

fsm_rt_t vsf_pl011_usart_enable(vsf_pl011_usart_t *pl011_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    return fsm_rt_cpl;
}

fsm_rt_t vsf_pl011_usart_disable(vsf_pl011_usart_t *pl011_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    return fsm_rt_cpl;
}

void vsf_pl011_usart_irq_enable(vsf_pl011_usart_t *pl011_usart_ptr, vsf_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
}

void vsf_pl011_usart_irq_disable(vsf_pl011_usart_t *pl011_usart_ptr, vsf_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
}

vsf_usart_status_t vsf_pl011_usart_status(vsf_pl011_usart_t *pl011_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    return (vsf_usart_status_t){ 0 };
}

uint_fast16_t vsf_pl011_usart_rxfifo_get_data_count(vsf_pl011_usart_t *pl011_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    return 0;
}

uint_fast16_t vsf_pl011_usart_rxfifo_read(vsf_pl011_usart_t *pl011_usart_ptr, void *buffer_ptr, uint_fast16_t count)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    return 0;
}

uint_fast16_t vsf_pl011_usart_txfifo_get_free_count(vsf_pl011_usart_t *pl011_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    return 0;
}

uint_fast16_t vsf_pl011_usart_txfifo_write(vsf_pl011_usart_t *pl011_usart_ptr, void *buffer_ptr, uint_fast16_t count)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    return 0;
}

vsf_err_t vsf_pl011_usart_request_rx(vsf_pl011_usart_t *pl011_usart_ptr, void *buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_pl011_usart_request_tx(vsf_pl011_usart_t *pl011_usart_ptr, void *buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_pl011_usart_cancel_rx(vsf_pl011_usart_t *pl011_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_pl011_usart_cancel_tx(vsf_pl011_usart_t *pl011_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    return VSF_ERR_NONE;
}

int_fast32_t vsf_pl011_usart_get_rx_count(vsf_pl011_usart_t *pl011_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    return 0;
}

int_fast32_t vsf_pl011_usart_get_tx_count(vsf_pl011_usart_t *pl011_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
    return 0;
}

void vsf_pl011_uart_irqhandler(vsf_pl011_usart_t *pl011_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != pl011_usart_ptr);
}

#endif      // __HAL_DRIVER_PL011_UART_H__
#endif      // VSF_HAL_USE_USART
