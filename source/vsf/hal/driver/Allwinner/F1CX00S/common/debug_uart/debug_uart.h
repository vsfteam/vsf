/*****************************************************************************
 *   Copyright(C)2009-2020 by VSF Team                                       *
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

#ifndef __HAL_DRIVER_ALLWINNER_F1CX00S_DEBUG_UART_H__
#define __HAL_DRIVER_ALLWINNER_F1CX00S_DEBUG_UART_H__

/*============================ INCLUDES ======================================*/

#include "osa_hal/vsf_osa_hal_cfg.h"

#if VSF_HAL_USE_DEBUG_STREAM == ENABLED

#if VSF_USE_SIMPLE_STREAM == ENABLED 
#   include "service/vsf_service.h"
#elif VSF_USE_STREAM == ENABLED
#   include "service/stream/vsf_stream.h"
#endif

#include "../../__device.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if     VSF_USE_SIMPLE_STREAM == ENABLED
#   define VSF_DEBUG_STREAM_NEED_POLL
#elif   VSF_USE_STREAM == ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if     VSF_USE_SIMPLE_STREAM == ENABLED
extern vsf_mem_stream_t VSF_DEBUG_STREAM_TX;
extern vsf_mem_stream_t VSF_DEBUG_STREAM_RX;
#elif   VSF_USE_STREAM == ENABLED
#endif

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

#if     VSF_USE_SIMPLE_STREAM == ENABLED
extern void VSF_DEBUG_STREAM_POLL(void);
#elif   VSF_USE_STREAM == ENABLED
#endif

#ifdef __cplusplus
}
#endif

#endif
#endif
/* EOF */
