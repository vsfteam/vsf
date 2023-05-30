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

#ifdef __VSF_HEADER_ONLY_SHOW_ARCH_INFO__

#else

#ifndef __HAL_DRIVER_GENERIC_LINUX_H__
#define __HAL_DRIVER_GENERIC_LINUX_H__

/*============================ INCLUDES ======================================*/

#include "service/vsf_service.h"

#include "./device.h"
#include "./rtc/rtc.h"

#if VSF_HAL_USE_RTC == ENABLED
#include "hal/driver/common/template/vsf_template_rtc.h"

#   define VSF_RTC_CFG_DEC_PREFIX              vsf_hw
#   define VSF_RTC_CFG_DEC_UPCASE_PREFIX       VSF_HW
#include "hal/driver/common/rtc/rtc_template.h"
#endif

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

#endif      // __HAL_DRIVER_GENERIC_LINUX_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
