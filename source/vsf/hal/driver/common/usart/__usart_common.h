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

#ifndef __HAL_DRIVER_COMMON_USART_H__
#define __HAL_DRIVER_COMMON_USART_H__

/*============================ INCLUDES ======================================*/
#if VSF_HAL_USART_IMP_REQUEST_BY_FIFO == ENABLED
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#define vsf_hal_usart_def_req_by_fifo()                                         \
            __vsf_usart_req_by_fifo_t   __req_by_fifo;

/*============================ TYPES =========================================*/

typedef struct __vsf_usart_req_t {
    uint32_t max_count;
    uint32_t count;
    void * buffer;
} __vsf_usart_req_t;

typedef struct __vsf_usart_req_by_fifo_t {
    __vsf_usart_req_t rx;
    __vsf_usart_req_t tx;
    vsf_usart_isr_t   isr;
} __vsf_usart_req_by_fifo_t;
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

#if USART_MAX_PORT >= 0 && VSF_HAL_USE_USART0 == ENABLED && (USART_PORT_MASK & (1 << 0))
extern const i_usart_t VSF_USART0;
extern vsf_usart_t vsf_usart0;
#endif
#if USART_MAX_PORT >= 1 && VSF_HAL_USE_USART1 == ENABLED && (USART_PORT_MASK & (1 << 1))
extern const i_usart_t VSF_USART1;
extern vsf_usart_t vsf_usart1;
#endif
#if USART_MAX_PORT >= 2 && VSF_HAL_USE_USART2 == ENABLED && (USART_PORT_MASK & (1 << 2))
extern const i_usart_t VSF_USART2;
extern vsf_usart_t vsf_usart2;
#endif
#if USART_MAX_PORT >= 3 && VSF_HAL_USE_USART3 == ENABLED && (USART_PORT_MASK & (1 << 3))
extern const i_usart_t VSF_USART3;
extern vsf_usart_t vsf_usart3;
#endif
#if USART_MAX_PORT >= 4 && VSF_HAL_USE_USART4 == ENABLED && (USART_PORT_MASK & (1 << 4))
extern const i_usart_t VSF_USART4;
extern vsf_usart_t vsf_usart4;
#endif
#if USART_MAX_PORT >= 5 && VSF_HAL_USE_USART5 == ENABLED && (USART_PORT_MASK & (1 << 5))
extern const i_usart_t VSF_USART5;
extern vsf_usart_t vsf_usart5;
#endif
#if USART_MAX_PORT >= 6 && VSF_HAL_USE_USART6 == ENABLED && (USART_PORT_MASK & (1 << 6))
extern const i_usart_t VSF_USART6;
extern vsf_usart_t vsf_usart6;
#endif
#if USART_MAX_PORT >= 7 && VSF_HAL_USE_USART7 == ENABLED && (USART_PORT_MASK & (1 << 7))
extern const i_usart_t VSF_USART7;
extern vsf_usart_t vsf_usart7;
#endif

#endif
