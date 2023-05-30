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
#   define VSF_USART_CFG_REIMPLEMENT_TYPE_MODE          ENABLED
#   define VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK      ENABLED
#   include "hal/driver/common/template/vsf_template_usart.h"

#   define VSF_USART_CFG_DEC_PREFIX                    vsf_win
#   define VSF_USART_CFG_DEC_UPCASE_PREFIX             VSF_WIN
#   include "hal/driver/common/usart/usart_template.h"

#   define VSF_USART_CFG_DEC_INSTANCE_PREFIX           vsf_win_fifo2req
#   define VSF_FIFO2REQ_USART_COUNT                    VSF_WIN_USART_COUNT
#   include "hal/driver/common/usart/fifo2req_usart.h"

#   if VSF_WIN_USART_CFG_USE_AS_HW_USART == ENABLED
#       ifndef VSF_HW_USART_COUNT
#           define VSF_HW_USART_COUNT                   32
#       endif
#       define VSF_REMAPPED_USART_COUNT                 VSF_WIN_USART_COUNT
#       define VSF_USART_CFG_DEC_INSTANCE_PREFIX        vsf_hw
#       include "hal/utilities/remap/usart/vsf_remapped_usart.h"
#   endif
#endif

#if VSF_HAL_USE_RTC == ENABLED
#   include "hal/driver/common/template/vsf_template_rtc.h"

#   define VSF_RTC_CFG_DEC_PREFIX                       vsf_hw
#   define VSF_RTC_CFG_DEC_UPCASE_PREFIX                VSF_HW
#   include "hal/driver/common/rtc/rtc_template.h"
#endif

#if VSF_HAL_USE_RNG == ENABLED
#   include "hal/driver/common/template/vsf_template_rng.h"

#   define VSF_RNG_CFG_DEC_PREFIX                       vsf_hw
#   define VSF_RNG_CFG_DEC_UPCASE_PREFIX                VSF_HW
#   include "hal/driver/common/rng/rng_template.h"
#endif

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
