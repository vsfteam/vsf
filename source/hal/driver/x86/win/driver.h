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

#ifndef __HAL_DRIVER_X86_WIN_H__
#define __HAL_DRIVER_X86_WIN_H__

/*============================ INCLUDES ======================================*/

#include "service/vsf_service.h"

#include "./device.h"
#include "./uart/uart.h"
#include "./rtc/rtc.h"
#include "./rng/rng.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/

#if VSF_HAL_USE_USART == ENABLED
#   include "hal/driver/common/template/vsf_template_usart.h"

#   define VSF_USART_CFG_DEC_PREFIX                     vsf_hostos
#   define VSF_USART_CFG_DEC_UPCASE_PREFIX              VSF_HOSTOS
#   include "hal/driver/common/usart/usart_template.h"

#    ifndef VSF_HOSTOS_USART_PREFIX
#       define VSF_HOSTOS_USART_PREFIX                  vsf_hw
#       define VSF_HOSTOS_USART_UPCASE_PREFIX           VSF_HW
#       ifndef VSF_HW_USART_COUNT
#           define VSF_HW_USART_COUNT                   VSF_HOSTOS_USART_COUNT
#       endif
#    endif

#   define VSF_USART_CFG_DEC_PREFIX                     VSF_HOSTOS_USART_PREFIX
#   define VSF_USART_CFG_DEC_UPCASE_PREFIX              VSF_HOSTOS_USART_UPCASE_PREFIX
#   define VSF_USART_CFG_DEC_REMAP                      ENABLED
#   define VSF_USART_CFG_DEC_REMAP_PREFIX               vsf_fifo2req
#   include "hal/driver/common/usart/usart_template.h"
#endif

#if VSF_HAL_USE_RTC == ENABLED
#   include "hal/driver/common/template/vsf_template_rtc.h"

#   define VSF_RTC_CFG_DEC_PREFIX                       vsf_hostos
#   define VSF_RTC_CFG_DEC_UPCASE_PREFIX                VSF_HOSTOS
#   include "hal/driver/common/rtc/rtc_template.h"

#    ifndef VSF_HOSTOS_RTC_PREFIX
#       define VSF_HOSTOS_RTC_PREFIX                    vsf_hw
#       define VSF_HOSTOS_RTC_UPCASE_PREFIX             VSF_HW
#       ifndef VSF_HW_RTC_COUNT
#           define VSF_HW_RTC_COUNT                     VSF_HOSTOS_RTC_COUNT
#       endif
#    endif

#   define VSF_RTC_CFG_DEC_PREFIX                       VSF_HOSTOS_RTC_PREFIX
#   define VSF_RTC_CFG_DEC_UPCASE_PREFIX                VSF_HOSTOS_RTC_UPCASE_PREFIX
#   define VSF_RTC_CFG_DEC_REMAP                        ENABLED
#   define VSF_RTC_CFG_DEC_REMAP_PREFIX                 vsf_remapped
#   include "hal/driver/common/rtc/rtc_template.h"
#endif

#if VSF_HAL_USE_RNG == ENABLED
#   include "hal/driver/common/template/vsf_template_rng.h"

#   define VSF_RNG_CFG_DEC_PREFIX                       vsf_hostos
#   define VSF_RNG_CFG_DEC_UPCASE_PREFIX                VSF_HOSTOS
#   include "hal/driver/common/rng/rng_template.h"

#    ifndef VSF_HOSTOS_RNG_PREFIX
#       define VSF_HOSTOS_RNG_PREFIX                    vsf_hw
#       define VSF_HOSTOS_RNG_UPCASE_PREFIX             VSF_HW
#       ifndef VSF_HW_RNG_COUNT
#           define VSF_HW_RNG_COUNT                     VSF_HOSTOS_RNG_COUNT
#       endif
#       ifndef VSF_HW_RNG_BITLEN
#           define VSF_HW_RNG_BITLEN                    VSF_HOSTOS_RNG_BITLEN
#       endif
#    endif

#   define VSF_RNG_CFG_DEC_PREFIX                       VSF_HOSTOS_RNG_PREFIX
#   define VSF_RNG_CFG_DEC_UPCASE_PREFIX                VSF_HOSTOS_RNG_UPCASE_PREFIX
#   define VSF_RNG_CFG_DEC_REMAP                        ENABLED
#   define VSF_RNG_CFG_DEC_REMAP_PREFIX                 vsf_remapped
#   include "hal/driver/common/rng/rng_template.h"
#endif

#if VSF_HAL_USE_USBD == ENABLED || VSF_HAL_USE_USBH == ENABLED
#   include "hal/driver/common/template/vsf_template_usb.h"
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_USE_SIMPLE_STREAM == ENABLED
extern vsf_stream_t VSF_DEBUG_STREAM_TX;
extern vsf_mem_stream_t VSF_DEBUG_STREAM_RX;
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern bool vsf_hostos_driver_init(void);

#ifdef __cplusplus
}
#endif

#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
#endif
/* EOF */
