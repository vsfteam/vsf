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

/*============================ MACROS ========================================*/

/*\note first define basic info for arch. */
#if defined(__VSF_HEADER_ONLY_SHOW_ARCH_INFO__)

#ifndef VSF_ARCH_SWI_NUM
#   define VSF_ARCH_SWI_NUM                     4
#endif

#ifndef VSF_ARCH_RTOS_CFG_STACK_DEPTH
#   define VSF_ARCH_RTOS_CFG_STACK_DEPTH        4096
#endif

#define VSF_ARCH_FREERTOS_CFG_IS_IN_ISR         xPortInIsrContext

// software interrupt provided by a dedicated device
#define VSF_DEV_SWI_NUM                         0

#ifndef __RTOS__
#   define __RTOS__
#endif
#ifndef __FREERTOS__
#   define __FREERTOS__
#endif

// SET_STACK will fail to pass stack check
//#include "xt_instr_macros.h"
//#define VSF_ARCH_RTOS_CFG_SET_STACK(__STACK)    SET_STACK(__STACK)

// 16-byte align, necessary if > 8
#define VSF_ARCH_CFG_STACK_ALIGN_BIT            4

#else

#ifndef __HAL_DEVICE_ESPRESSIF_ESP32_H__
#define __HAL_DEVICE_ESPRESSIF_ESP32_H__

/*============================ INCLUDES ======================================*/

#include "soc/periph_defs.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __HAL_DEVICE_ESPRESSIF_ESP32_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
