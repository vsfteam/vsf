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
#   define VSF_DEV_SWI_NUM          32

#elif defined(__VSF_HAL_SHOW_VENDOR_INFO__)

#else

#ifndef __HAL_DEVICE_ATTERY_AT32F405KCU7_4_H__
#define __HAL_DEVICE_ATTERY_AT32F405KCU7_4_H__

// software interrupt provided by a dedicated device
#define VSF_DEV_SWI_LIST                                                        \
    43, 46, 47, 48, 49,  50,  61,  62,                                          \
    63, 64, 65, 66, 70,  78,  79,  80,                                          \
    84, 86, 87, 88, 89,  90,  91,  93,                                          \
    95, 96, 97, 98, 99, 100, 101, 102

/*============================ MACROS ========================================*/

// user configurations with default value

// HW definition

// RAM

#define VSF_HW_RAM_COUNT            1
#define VSF_HW_RAM0_ADDR            0x20000000
// user can define VSF_HW_RAM0_SIZE to 96KB if parity check is enabled
#ifndef VSF_HW_RAM0_SIZE
#   define VSF_HW_RAM0_SIZE         ((96 + 6) * 1024)
#endif

// FLASH

#define VSF_HW_FLASH_COUNT          1
#define VSF_HW_FLASH0_ADDR          0x08000000
#define VSF_HW_FLASH0_SIZE          (256 * 1024)

// GPIO

#define VSF_HW_GPIO_PORT_COUNT      2
#define VSF_HW_GPIO_PIN_COUNT       16
#define VSF_HW_GPIO_FUNCTION_MAX    16
#define VSF_HW_GPIO_PORT0_MASK      0xF8FF
#define VSF_HW_GPIO_PORT1_MASK      0x01FF

// USB OTG

#define VSF_HW_USB_OTG_COUNT        2

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif      // __HAL_DEVICE_ATTERY_AT32F405KCU7_4_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__

#include "../common/device.h"

/* EOF */
