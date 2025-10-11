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

#if defined(__VSF_HEADER_ONLY_SHOW_ARCH_INFO__)

/*\note __VSF_HEADER_ONLY_SHOW_ARCH_INFO__ is defined to include arch information only.
 *      Some arch provide chip-vendor with some vendor-specified options, define these options here.
 */

//! arch info

#   define VSF_ARCH_LIMIT_NO_SET_STACK

#elif defined(__VSF_HAL_SHOW_VENDOR_INFO__)

/*\note __VSF_HAL_SHOW_VENDOR_INFO__ is defined to include vendor information only.
 *      Vendor information means the registers/structures/macros from vendor SDK.
 *      Usually these information are not visible from user side to avoid name-space pollution.
 */

#   define __VSF_HEADER_ONLY_SHOW_VENDOR_INFO__

#else

/*\note __VSF_HEADER_ONLY_SHOW_ARCH_INFO__ and __VSF_HAL_SHOW_VENDOR_INFO__ are not defined.
 *      Define device information here.
 */

#ifndef __VSF_HAL_DEVICE_EMULATION_DISTBUS_H__
#define __VSF_HAL_DEVICE_EMULATION_DISTBUS_H__

/*============================ MACROS ========================================*/

/*\note For specified peripheral, VSF_HW_PERIPHERAL_COUNT MUST be defined as number of peripheral instance.
 *      If peripheral instances start from 0, and are in sequence order(eg, 3 I2Cs: I2C0, I2C1, I2C2), VSF_HW_PERIPHERAL_MASK is not needed.
 *      Otherwise, define VSF_HW_PERIPHERAL_MASK to indicate which peripheral instances to implmenent.
 *      eg: 3 I2Cs: I2C0, I2C2, I2C4, define VSF_HW_I2C_MASK to 0x15(BIT(0) | BIT(2)) | BIT(4)).
 *
 *      Other configurations are vendor specified, drivers will use these information to generate peripheral instances.
 *      Usually need irqn, irqhandler, peripheral clock enable bits, peripheral reset bites, etc.
 */

// GPIO

#ifndef VSF_HW_GPIO_PORT_COUNT
#   define VSF_HW_GPIO_PORT_COUNT                   1
#endif
#ifndef VSF_HW_GPIO_PIN_COUNT
#   define VSF_HW_GPIO_PIN_COUNT                    16
#endif

// USART UART

#ifndef VSF_HW_USART_COUNT
#   define VSF_HW_USART_COUNT                       1
#endif

// SPI

#ifndef VSF_HW_SPI_COUNT
#   define VSF_HW_SPI_COUNT                         1
#endif

// I2C

#ifndef VSF_HW_I2C_COUNT
#   define VSF_HW_I2C_COUNT                         1
#endif

// SDIO

#ifndef VSF_HW_SDIO_COUNT
#   define VSF_HW_SDIO_COUNT                        1
#endif

/*============================ INCLUDES ======================================*/

// Include common irq and af headers after peripherals are defined, so that
//  irq and af can be adjusted according to the dedicated device configuration.

#include "./device_irq.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __VSF_HAL_DEVICE_EMULATION_DISTBUS_H__
#endif
/* EOF */
