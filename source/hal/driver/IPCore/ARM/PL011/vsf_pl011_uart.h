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

#ifndef __HAL_DRIVER_PL011_UART_H__
#define __HAL_DRIVER_PL011_UART_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_USART == ENABLED

#if     defined(__VSF_HAL_PL011_UART_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_PL011_UART_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/

// IP cores which will export class(es), need to include template before definition
//  of the class. Because the class member(s) maybe depend on the template.
#include "hal/driver/common/template/vsf_template_usart.h"

/*============================ TYPES =========================================*/

vsf_class(vsf_pl011_usart_t) {
#if VSF_DW_APB_I2C_CFG_MULTI_CLASS == ENABLED
    public_member(
        vsf_usart_t             vsf_usart;
    )
#endif
    public_member(
        void                   *reg;
    )
};

/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_pl011_usart_init(vsf_pl011_usart_t *pl011_usart_ptr, vsf_usart_cfg_t *cfg_ptr, uint_fast32_t clk_hz);
extern void vsf_pl011_usart_fini(vsf_pl011_usart_t *pl011_usart_ptr);
extern vsf_usart_capability_t vsf_pl011_usart_capability(vsf_pl011_usart_t *pl011_usart_ptr);
extern fsm_rt_t vsf_pl011_usart_enable(vsf_pl011_usart_t *pl011_usart_ptr);
extern fsm_rt_t vsf_pl011_usart_disable(vsf_pl011_usart_t *pl011_usart_ptr);
extern void vsf_pl011_usart_irq_enable(vsf_pl011_usart_t *pl011_usart_ptr, vsf_usart_irq_mask_t irq_mask);
extern void vsf_pl011_usart_irq_disable(vsf_pl011_usart_t *pl011_usart_ptr, vsf_usart_irq_mask_t irq_mask);
extern vsf_usart_status_t vsf_pl011_usart_status(vsf_pl011_usart_t *pl011_usart_ptr);
extern uint_fast16_t vsf_pl011_usart_rxfifo_get_data_count(vsf_pl011_usart_t *pl011_usart_ptr);
extern uint_fast16_t vsf_pl011_usart_rxfifo_read(vsf_pl011_usart_t *pl011_usart_ptr, void *buffer_ptr, uint_fast16_t count);
extern uint_fast16_t vsf_pl011_usart_txfifo_get_free_count(vsf_pl011_usart_t *pl011_usart_ptr);
extern uint_fast16_t vsf_pl011_usart_txfifo_write(vsf_pl011_usart_t *pl011_usart_ptr, void *buffer_ptr, uint_fast16_t count);
extern vsf_err_t vsf_pl011_usart_request_rx(vsf_pl011_usart_t *pl011_usart_ptr, void *buffer_ptr, uint_fast32_t count);
extern vsf_err_t vsf_pl011_usart_request_tx(vsf_pl011_usart_t *pl011_usart_ptr, void *buffer_ptr, uint_fast32_t count);
extern vsf_err_t vsf_pl011_usart_cancel_rx(vsf_pl011_usart_t *pl011_usart_ptr);
extern vsf_err_t vsf_pl011_usart_cancel_tx(vsf_pl011_usart_t *pl011_usart_ptr);
extern int_fast32_t vsf_pl011_usart_get_rx_count(vsf_pl011_usart_t *pl011_usart_ptr);
extern int_fast32_t vsf_pl011_usart_get_tx_count(vsf_pl011_usart_t *pl011_usart_ptr);
extern void vsf_pl011_uart_irqhandler(vsf_pl011_usart_t *pl011_usart_ptr);

#ifdef __cplusplus
}
#endif

#endif
#endif
/* EOF */
