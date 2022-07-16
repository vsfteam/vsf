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

#ifndef __HAL_DRIVER_FIFO2REQ_USART_H__
#define __HAL_DRIVER_FIFO2REQ_USART_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_USART == ENABLED

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// TODO: add usart rx idle or timeout support
// TODO: support 9 bit mode

typedef uint_fast16_t vsf_usart_fifo2req_fifo_fn_t(vsf_usart_t *usart_ptr,
                                                   void *buffer_ptr,
                                                   uint_fast16_t count);

typedef void vsf_usart_fifo2req_irq_fn_t(vsf_usart_t *usart_ptr,
                                         em_usart_irq_mask_t irq_mask);

typedef vsf_err_t vsf_usart_fifo2req_init_fn_t(vsf_usart_t *usart_ptr, usart_cfg_t *cfg_ptr);


typedef struct vsf_usart_fifo2req_item_t {
    vsf_usart_fifo2req_fifo_fn_t  * fifo_fn;
    void                          * buffer;
    uint32_t                        max_count;
    uint32_t                        count;
} vsf_usart_fifo2req_item_t;

typedef struct vsf_usart_fifo2req_t {
    vsf_usart_fifo2req_item_t rx;
    vsf_usart_fifo2req_item_t tx;

    vsf_usart_isr_t isr;
    em_usart_irq_mask_t irq_mask;

    vsf_usart_fifo2req_irq_fn_t * enable_irq_fn;
    vsf_usart_fifo2req_irq_fn_t * disable_irq_fn;

} vsf_usart_fifo2req_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

vsf_err_t vsf_usart_fifo2req_init(vsf_usart_fifo2req_t *request_ptr,
                                  vsf_usart_fifo2req_init_fn_t  * init_fn,
                                  vsf_usart_fifo2req_irq_fn_t   * enable_irq_fn,
                                  vsf_usart_fifo2req_irq_fn_t   * disable_irq_fn,
                                  vsf_usart_fifo2req_fifo_fn_t  * rxfifo_fn,
                                  vsf_usart_fifo2req_fifo_fn_t  * txfifo_fn,
                                  vsf_usart_t *usart_ptr,
                                  usart_cfg_t *cfg_ptr);

vsf_err_t vsf_usart_fifo2req_request_rx(vsf_usart_fifo2req_t *request_ptr, vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count);

vsf_err_t vsf_usart_fifo2req_cancel_rx(vsf_usart_fifo2req_t *request_ptr, vsf_usart_t *usart_ptr);

int_fast32_t vsf_usart_fifo2req_get_rx_count(vsf_usart_fifo2req_t *request_ptr, vsf_usart_t *usart_ptr);

vsf_err_t vsf_usart_fifo2req_request_tx(vsf_usart_fifo2req_t *request_ptr,
                                        vsf_usart_t *usart_ptr,
                                        void *buffer_ptr, uint_fast32_t count);

vsf_err_t vsf_usart_fifo2req_cancel_tx(vsf_usart_fifo2req_t *request_ptr, vsf_usart_t *usart_ptr);

int_fast32_t vsf_usart_fifo2req_get_tx_count(vsf_usart_fifo2req_t *request_ptr, vsf_usart_t *usart_ptr);

void vsf_usart_fifo2req_irq_enable(vsf_usart_fifo2req_t *request_ptr,
                                   vsf_usart_t *usart_ptr,
                                   em_usart_irq_mask_t irq_mask);

void vsf_usart_fifo2req_irq_disable(vsf_usart_fifo2req_t *request_ptr,
                                    vsf_usart_t *usart_ptr,
                                    em_usart_irq_mask_t irq_mask);

/*============================ IMPLEMENTATION ================================*/

#endif

#endif
/* EOF */
