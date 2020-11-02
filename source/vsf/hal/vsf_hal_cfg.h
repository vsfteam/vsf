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


//! \note do not move this pre-processor statement to other places
#include "vsf_cfg.h"

#ifndef __VSF_HAL_CFG_H__
#define __VSF_HAL_CFG_H__

//! all hal modules use this configuation file

/*============================ INCLUDES ======================================*/

#ifdef __cplusplus
extern "C" {
#endif
/*============================ MACROS ========================================*/

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#ifndef VSF_HAL_ASSERT
#   define VSF_HAL_ASSERT(__CON)        ASSERT(__CON)
#endif
#else
#ifndef VSF_HAL_ASSERT
#   define VSF_HAL_ASSERT(...)          ASSERT(__VA_ARGS__)
#endif
#endif

#ifndef VSF_SYSTIMER_RESOLUTION
#   define VSF_SYSTIMER_RESOLUTION      (1000000)    /*! using default 1us */
#endif

#ifndef VSF_HAL_USE_GPIO
#   define VSF_HAL_USE_GPIO             ENABLED
#endif
#ifndef VSF_HAL_USE_PM
#   define VSF_HAL_USE_PM               ENABLED
#endif

#ifndef VSF_HAL_USE_DMA
#   define VSF_HAL_USE_DMA              ENABLED
#endif

/*----------------------------------------------------------------------------*
 * USART Switch                                                               *
 *----------------------------------------------------------------------------*/
#ifndef VSF_HAL_USE_USART
#   define VSF_HAL_USE_USART            ENABLED
#endif
#if VSF_HAL_USE_USART != ENABLED
#   undef VSF_HAL_USE_USART0
#   undef VSF_HAL_USE_USART1
#   undef VSF_HAL_USE_USART2
#   undef VSF_HAL_USE_USART3
#   undef VSF_HAL_USE_USART4
#   undef VSF_HAL_USE_USART5
#   undef VSF_HAL_USE_USART6
#   undef VSF_HAL_USE_USART7
#else
#   ifndef VSF_HAL_USE_USART0
#       define VSF_HAL_USE_USART0       ENABLED
#   endif
#   ifndef VSF_HAL_USE_USART1
#       define VSF_HAL_USE_USART1       ENABLED
#   endif
#   ifndef VSF_HAL_USE_USART2
#       define VSF_HAL_USE_USART2       ENABLED
#   endif
#   ifndef VSF_HAL_USE_USART3
#       define VSF_HAL_USE_USART3       ENABLED
#   endif
#   ifndef VSF_HAL_USE_USART4
#       define VSF_HAL_USE_USART4       ENABLED
#   endif
#   ifndef VSF_HAL_USE_USART5
#       define VSF_HAL_USE_USART5       ENABLED
#   endif
#   ifndef VSF_HAL_USE_USART6
#       define VSF_HAL_USE_USART6       ENABLED
#   endif
#   ifndef VSF_HAL_USE_USART7
#       define VSF_HAL_USE_USART7       ENABLED
#   endif
#endif

#ifndef VSF_HAL_USE_SPI
#   define VSF_HAL_USE_SPI              ENABLED
#endif
#ifndef VSF_HAL_USE_IIC
#   define VSF_HAL_USE_IIC              ENABLED
#endif
#ifndef VSF_HAL_USE_TIMER
#   define VSF_HAL_USE_TIMER            ENABLED
#endif
#ifndef VSF_HAL_USE_ADC
#   define VSF_HAL_USE_ADC              ENABLED
#endif
#ifndef VSF_HAL_USE_DAC
#   define VSF_HAL_USE_DAC              ENABLED
#endif
#ifndef VSF_HAL_USE_RTC
#   define VSF_HAL_USE_RTC              ENABLED
#endif
#ifndef VSF_HAL_USE_USBD
#   define VSF_HAL_USE_USBD             VSF_USE_USB_DEVICE
#endif
#ifndef VSF_HAL_USE_USBH
#   define VSF_HAL_USE_USBH             VSF_USE_USB_HOST
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif
#endif
/* EOF */