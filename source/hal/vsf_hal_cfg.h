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

//! \note do not move this pre-processor statement to other places
#include "vsf_cfg.h"

#ifndef __VSF_HAL_CFG_H__
#define __VSF_HAL_CFG_H__

//! all hal modules use this configuation file

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#ifndef VSF_HAL_ASSERT
#   define VSF_HAL_ASSERT(__CON)        VSF_ASSERT(__CON)
#endif
#else
#ifndef VSF_HAL_ASSERT
#   define VSF_HAL_ASSERT(...)          VSF_ASSERT(__VA_ARGS__)
#endif
#endif

#ifndef VSF_HAL_USE_IO
#   define VSF_HAL_USE_IO               ENABLED
#endif

#ifndef VSF_HAL_USE_GPIO
#   define VSF_HAL_USE_GPIO             ENABLED
#endif

#ifndef VSF_HAL_USE_PM
#   define VSF_HAL_USE_PM               ENABLED
#endif

#ifndef VSF_HAL_USE_USART
#   define VSF_HAL_USE_USART            ENABLED
#endif

#ifndef VSF_HAL_USE_SPI
#   define VSF_HAL_USE_SPI              ENABLED
#endif

#ifndef VSF_HAL_USE_I2C
#   define VSF_HAL_USE_I2C              ENABLED
#endif

#ifndef VSF_HAL_USE_I2C_GPIO
#   define VSF_HAL_USE_I2C_GPIO         ENABLED
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

#ifndef VSF_HAL_USE_FLASH
#   define VSF_HAL_USE_FLASH            ENABLED
#endif

#ifndef VSF_HAL_USE_PWM
#   define VSF_HAL_USE_PWM              ENABLED
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