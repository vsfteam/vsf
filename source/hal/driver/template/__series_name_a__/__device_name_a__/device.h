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

#   define __VSF_HEADER_ONLY_SHOW_VENDOR_INFO__
#   include "vendor_header.h"

#else

/*\note __VSF_HEADER_ONLY_SHOW_ARCH_INFO__ and __VSF_HAL_SHOW_VENDOR_INFO__ are not defined.
 *      Define device information here.
 */

#ifndef __VSF_HAL_DEVICE_${VENDOR}_${DEVICE}_H__
#define __VSF_HAL_DEVICE_${VENDOR}_${DEVICE}_H__

/*============================ INCLUDES ======================================*/

/*\note this is should be the only place where __common.h is included if implemented.
        The path of __common.h is up to developer to decide.
*/
//#include "../common/__common.h"

/*============================ MACROS ========================================*/

// software interrupt provided by a dedicated device
#define VSF_DEV_SWI_NUM                             0

/*\note For specified peripheral, VSF_HW_PERIPHERAL_COUNT MUST be defined as number of peripheral instance.
 *      If peripheral instances start from 0, and are in sequence order(eg, 3 I2Cs: I2C0, I2C1, I2C2), VSF_HW_PERIPHERAL_MASK is not needed.
 *      Otherwise, define VSF_HW_PERIPHERAL_MASK to indicate which peripheral instances to implmenent.
 *      eg: 3 I2Cs: I2C0, I2C2, I2C4, define VSF_HW_I2C_MASK to 0x15(BIT(0) | BIT(2)) | BIT(4)).
 *
 *      Other configurations are vendor specified, drivers will use these information to generate peripheral instances.
 *      Usually need irqn, irqhandler, peripheral clock enable bits, peripheral reset bites, etc.
 */

// GPIO

#define VSF_HW_GPIO_PORT_COUNT                      1
#define VSF_HW_GPIO_PIN_COUNT                       32

// I2C0, I2C2

#define VSF_HW_I2C_COUNT                            2
#define VSF_HW_I2C_MASK                             0x05
#define VSF_HW_I2C0_IRQN                            I2C0_IRQn
#define VSF_HW_I2C0_IRQHandler                      I2C0_IRQHandler
#define VSF_HW_I2C0_REG                             I2C0_BASE
#define VSF_HW_I2C2_IRQN                            I2C2_IRQn
#define VSF_HW_I2C2_IRQHandler                      I2C2_IRQHandler
#define VSF_HW_I2C2_REG                             I2C2_BASE

// SPI0, SPI1

#define VSF_HW_SPI_COUNT                          2
#define VSF_HW_SPI0_IRQN                          SPI0_IRQn
#define VSF_HW_SPI0_IRQHandler                    SPI0_IRQHandler
#define VSF_HW_SPI0_REG                           SPI0_BASE
#define VSF_HW_SPI1_IRQN                          SPI1_IRQn
#define VSF_HW_SPI1_IRQHandler                    SPI1_IRQHandler
#define VSF_HW_SPI1_REG                           SPI1_BASE

// QSPI0
#define VSF_HW_QSPI_COUNT                          1
#define VSF_HW_QSPI0_IRQN                          QSPI0_IRQn
#define VSF_HW_QSPI0_IRQHandler                    QSPI0_IRQHandler
#define VSF_HW_QSPI0_REG                           QSPI0_BASE
#define VSF_HW_QSPI1_IRQN                          QSPI1_IRQn
#define VSF_HW_QSPI1_IRQHandler                    QSPI1_IRQHandler
#define VSF_HW_QSPI1_REG                           QSPI1_BASE

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __VSF_HAL_DEVICE_${VENDOR}_${DEVICE}_H__
#endif
/* EOF */
