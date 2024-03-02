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
//  for ARM, number of interrupt priority is vendor-specified, so define here

#   define VSF_ARCH_PRI_NUM                         64
#   define VSF_ARCH_PRI_BIT                         6

#elif defined(__VSF_HAL_SHOW_VENDOR_INFO__)

/*\note __VSF_HAL_SHOW_VENDOR_INFO__ is defined to include vendor information only.
 *      Vendor information means the registers/structures/macros from vendor SDK.
 *      Usually these information are not visible from user side to avoid name-space pollution.
 */

#include "vendor_header.h"

#else

/*\note __VSF_HEADER_ONLY_SHOW_ARCH_INFO__ and __VSF_HAL_SHOW_VENDOR_INFO__ are not defined.
 *      Define device information here.
 */

#ifndef __VSF_HAL_DEVICE_&{VENDOR}_${DEVICE}_H__
#define __VSF_HAL_DEVICE_&{VENDOR}_${DEVICE}_H__

/*============================ INCLUDES ======================================*/

/*\note this is should be the only place where __common.h is included if implemented.
        You can also put __common.h in ../common directory, it's up to user to decide.
*/
//#include "../__common.h"

/*============================ MACROS ========================================*/

// software interrupt provided by a dedicated device
#define VSF_DEV_SWI_NUM                             0

/*\note For specified peripherial, VSF_HW_PERIPHERIAL_COUNT MUST be defined as number of peripherial instance.
 *      If peripherial instances start from 0, and are in sequence order(eg, 3 I2Cs: I2C0, I2C1, I2C2), VSF_HW_PERIPHERIAL_MASK is not needed.
 *      Otherwise, defind VSF_HW_PERIPHERIAL_MASK to indicate which peripherial instances to implmenent.
 *      eg: 3 I2Cs: I2C0, I2C2, I2C4, define VSF_HW_I2C_MASK to 0x15(BIT(0) | BIT(2)) | BIT(4)).
 *
 *      Other configurations are vendor specified, drivers will use these information to generate peripherial instances.
 *      Usually need irqn, irqhandler, peripherial clock enable bits, peripherial reset bites, etc.
 */

// I2C0, I2C2

#define VSF_HW_I2C_COUNT                            2
#define VSF_HW_I2C_MASK                             0x05
#define VSF_HW_I2C0_IRQN                            I2C0_IRQn
#define VSF_HW_I2C0_IRQHandler                      I2C0_IRQHandler
#define VSF_HW_I2C0_REG                             I2C0_BASE
#define VSF_HW_I2C1_IRQN                            I2C1_IRQn
#define VSF_HW_I2C1_IRQHandler                      I2C1_IRQHandler
#define VSF_HW_I2C1_REG                             I2C1_BASE

// SPI0, SPI1

#define VSF_HW_USART_COUNT                          2
#define VSF_HW_USART0_IRQN                          UART0_IRQn
#define VSF_HW_USART0_IRQHandler                    UART0_IRQHandler
#define VSF_HW_USART0_REG                           UART0_BASE
#define VSF_HW_USART1_IRQN                          UART1_IRQn
#define VSF_HW_USART1_IRQHandler                    UART1_IRQHandler
#define VSF_HW_USART1_REG                           UART1_BASE

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __VSF_HAL_DEVICE_&{VENDOR}_${DEVICE}_H__
#endif
/* EOF */
