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

#include "hal/vsf_hal_cfg.h"

#ifdef __VSF_HEADER_ONLY_SHOW_ARCH_INFO__

#else

#ifndef __HAL_DRIVER_X86_WIN_H__
#define __HAL_DRIVER_X86_WIN_H__

/*============================ INCLUDES ======================================*/

#include "service/vsf_service.h"

#include "./device.h"
#include "./uart/uart.h"
#include "hal/interface/vsf_interface_usb.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_USE_SIMPLE_STREAM == ENABLED
extern vsf_stream_t VSF_DEBUG_STREAM_TX;
extern vsf_mem_stream_t VSF_DEBUG_STREAM_RX;
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
#endif
/* EOF */
