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

/*============================ MACROS ========================================*/

#ifdef __VSF_HEADER_ONLY_SHOW_ARCH_INFO__

// software interrupt provided by a dedicated device
#   define VSF_DEV_SWI_NUM          VSF_DEV_COMMON_SWI_NUM

#   include "../common/device.h"

#elif defined(__VSF_HAL_SHOW_VENDOR_INFO__)

#   include "../common/device.h"

#else

#ifndef __HAL_DEVICE_ATTERY_AT32F405KCU7_4_H__
#define __HAL_DEVICE_ATTERY_AT32F405KCU7_4_H__

/*============================ MACROS ========================================*/

// user configurations with default value

// HW definition

// software interrupt provided by a dedicated device
#define VSF_DEV_SWI_LIST            VSF_DEV_COMMON_SWI_LIST

// RAM

#if VSF_HW_CFG_RAM_PARITY_CHECK == ENABLED
#   define VSF_HW_RAM0_SIZE         ((96 + 0) * 1024)
#else
#   define VSF_HW_RAM0_SIZE         ((96 + 6) * 1024)
#endif

// FLASH

#define VSF_HW_FLASH0_SIZE          (256 * 1024)

// GPIO

// PORT0, PORT1
#define VSF_HW_GPIO_PORT_COUNT      2
// PORT0: PIN0..PIN7, PIN11..PIN15
#define VSF_HW_GPIO_PORT0_MASK      0xF8FF
// PORT1: PIN0..PIN8
#define VSF_HW_GPIO_PORT1_MASK      0x01FF

// USART

// USART1..USART7
#define VSF_HW_USART_MASK           0x00FE

// SPI: use full support provided by common/device.h

// I2C: use full support provided by common/device.h

// USB OTG

#define VSF_HW_USB_OTG_COUNT        2

/*============================ INCLUDES ======================================*/

// Include common device header after peripherals are defined, so that
//  common part can be adjusted according to the dedicated device configuration.

#include "../common/device.h"

/*============================ MACROS ========================================*/

// Add code here to overwrite configurations from common device.h
//  eg: add more swi by overwriting none-exist interrupt handler

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif      // __HAL_DEVICE_ATTERY_AT32F405KCU7_4_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__

/* EOF */
