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

#ifndef __HAL_DRIVER_USART_REQUEST_H__
#define __HAL_DRIVER_USART_REQUEST_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// TODO: add usart rx idle or timeout support
// TODO: support 9 bit mode

#if VSF_HAL_USE_USART == ENABLED

typedef struct vsf_usart_request_item_t {
    void           * buffer;
    uint32_t         max_count;
    uint32_t         count;
} vsf_usart_request_item_t;

typedef struct vsf_usart_request_t {
#if VSF_USART_CFG_MULTI_INSTANCES == ENABLED
    vsf_usart_t vsf_usart;
#endif

    vsf_usart_t *real_usart_ptr;

    vsf_usart_request_item_t rx;
    vsf_usart_request_item_t tx;

    vsf_usart_isr_t isr;

    em_usart_irq_mask_t irq_mask;

    //uint8_t data_length;
} vsf_usart_request_t;

#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif
/* EOF */