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

#   define VSF_ARCH_PRI_NUM                         16
#   define VSF_ARCH_PRI_BIT                         4

// software interrupt provided by a dedicated device
#   ifndef VSF_DEV_SWI_NUM
#       if  defined(CORE_CM4)
#           define VSF_DEV_SWI_NUM                  8
#       else
#           define VSF_DEV_SWI_NUM                  11
#       endif
#   endif
#   if VSF_DEV_SWI_NUM > VSF_ARCH_PRI_NUM
#       warning too many VSF_DEV_SWI_NUM, max is VSF_ARCH_PRI_NUM
#       undef VSF_DEV_SWI_NUM
#       define VSF_DEV_SWI_NUM                      VSF_ARCH_PRI_NUM
#   endif

#elif defined(__VSF_HAL_SHOW_VENDOR_INFO__)

/*\note __VSF_HAL_SHOW_VENDOR_INFO__ is defined to include vendor information only.
 *      Vendor information means the registers/structures/macros from vendor SDK.
 *      Usually these information are not visible from user side to avoid name-space pollution.
 */

#   if !defined(CORE_CM4) && !defined(CORE_CM7)
#       define CORE_CM7
#   endif

#   define __VSF_HEADER_ONLY_SHOW_VENDOR_INFO__
#   include "../common/vendor/n32h76x_78x.h"

#else

/*\note __VSF_HEADER_ONLY_SHOW_ARCH_INFO__ and __VSF_HAL_SHOW_VENDOR_INFO__ are not defined.
 *      Define device information here.
 */

#ifndef __VSF_HAL_DEVICE_NATIONS_N32H765IIB7EC_H__
#define __VSF_HAL_DEVICE_NATIONS_N32H765IIB7EC_H__

// software interrupt provided by a dedicated device
#define __VSF_DEF_SWI_LIST_DEF(__N, __PARAM)                                    \
                                    VSF_MCONNECT(SWI, __N, _IRQn),
#define VSF_DEV_SWI_LIST            VSF_MREPEAT(VSF_DEV_SWI_NUM, __VSF_DEF_SWI_LIST_DEF, NULL)

/*============================ INCLUDES ======================================*/

/*\note this is should be the only place where __common.h is included if implemented.
        The path of common.h is up to developer to decide.
*/

#if !defined(CORE_CM4) && !defined(CORE_CM7)
#   define CORE_CM7
#endif
#include "../common/common.h"

/*============================ MACROS ========================================*/

/*\note For specified peripheral, VSF_HW_PERIPHERAL_COUNT MUST be defined as number of peripheral instance.
 *      If peripheral instances start from 0, and are in sequence order(eg, 3 I2Cs: I2C0, I2C1, I2C2), VSF_HW_PERIPHERAL_MASK is not needed.
 *      Otherwise, define VSF_HW_PERIPHERAL_MASK to indicate which peripheral instances to implmenent.
 *      eg: 3 I2Cs: I2C0, I2C2, I2C4, define VSF_HW_I2C_MASK to 0x15(BIT(0) | BIT(2)) | BIT(4)).
 *
 *      Other configurations are vendor specified, drivers will use these information to generate peripheral instances.
 *      Usually need irqn, irqhandler, peripheral clock enable bits, peripheral reset bites, etc.
 */

// Interrupts
//  Defined in common/device_irq.h

// RAM

#define VSF_HW_RAM_COUNT                            1
#define VSF_HW_RAM0_ADDR                            0x24000000
#define VSF_HW_RAM0_SIZE                            (1152 * 1024)

// FLASH

#define VSF_HW_FLASH_COUNT                          1
#define VSF_HW_FLASH0_ADDR                          0x15000000
#define VSF_HW_FLASH0_SIZE                          ((2048 - 128) * 1024)

// GPIO, TODO: update VSF_HW_GPIO_PORTX_MASK

#define VSF_HW_GPIO_PORT_COUNT                      11
#define VSF_HW_GPIO_PIN_COUNT                       16
#define VSF_HW_GPIO_FUNCTION_MAX                    16
#define VSF_HW_GPIO_PORT0_REG                       GPIOA
#define VSF_HW_GPIO_PORT0_MASK                      0xFFFF
#define VSF_HW_GPIO_PORT0_EN                        VSF_HW_EN_GPIOA
#define VSF_HW_GPIO_PORT0_RST                       VSF_HW_RST_GPIOA
#define VSF_HW_GPIO_PORT1_REG                       GPIOB
#define VSF_HW_GPIO_PORT1_MASK                      0xFFFF
#define VSF_HW_GPIO_PORT1_EN                        VSF_HW_EN_GPIOB
#define VSF_HW_GPIO_PORT1_RST                       VSF_HW_RST_GPIOB
#define VSF_HW_GPIO_PORT2_REG                       GPIOC
#define VSF_HW_GPIO_PORT2_MASK                      0xFFFF
#define VSF_HW_GPIO_PORT2_EN                        VSF_HW_EN_GPIOC
#define VSF_HW_GPIO_PORT2_RST                       VSF_HW_RST_GPIOC
#define VSF_HW_GPIO_PORT3_REG                       GPIOD
#define VSF_HW_GPIO_PORT3_MASK                      0xFFFF
#define VSF_HW_GPIO_PORT3_EN                        VSF_HW_EN_GPIOD
#define VSF_HW_GPIO_PORT3_RST                       VSF_HW_RST_GPIOD
#define VSF_HW_GPIO_PORT4_REG                       GPIOE
#define VSF_HW_GPIO_PORT4_MASK                      0xFFFF
#define VSF_HW_GPIO_PORT4_EN                        VSF_HW_EN_GPIOE
#define VSF_HW_GPIO_PORT4_RST                       VSF_HW_RST_GPIOE
#define VSF_HW_GPIO_PORT5_REG                       GPIOF
#define VSF_HW_GPIO_PORT5_MASK                      0xFFFF
#define VSF_HW_GPIO_PORT5_EN                        VSF_HW_EN_GPIOF
#define VSF_HW_GPIO_PORT5_RST                       VSF_HW_RST_GPIOF
#define VSF_HW_GPIO_PORT6_REG                       GPIOG
#define VSF_HW_GPIO_PORT6_MASK                      0xFFFF
#define VSF_HW_GPIO_PORT6_EN                        VSF_HW_EN_GPIOG
#define VSF_HW_GPIO_PORT6_RST                       VSF_HW_RST_GPIOG
#define VSF_HW_GPIO_PORT7_REG                       GPIOH
#define VSF_HW_GPIO_PORT7_MASK                      0xFFFF
#define VSF_HW_GPIO_PORT7_EN                        VSF_HW_EN_GPIOH
#define VSF_HW_GPIO_PORT7_RST                       VSF_HW_RST_GPIOH
#define VSF_HW_GPIO_PORT8_REG                       GPIOI
#define VSF_HW_GPIO_PORT8_MASK                      0xFFFF
#define VSF_HW_GPIO_PORT8_EN                        VSF_HW_EN_GPIOI
#define VSF_HW_GPIO_PORT8_RST                       VSF_HW_RST_GPIOI
#define VSF_HW_GPIO_PORT9_REG                       GPIOJ
#define VSF_HW_GPIO_PORT9_MASK                      0xFFFF
#define VSF_HW_GPIO_PORT9_EN                        VSF_HW_EN_GPIOJ
#define VSF_HW_GPIO_PORT9_RST                       VSF_HW_RST_GPIOJ
#define VSF_HW_GPIO_PORT10_REG                      GPIOK
#define VSF_HW_GPIO_PORT10_MASK                     0xFFFF
#define VSF_HW_GPIO_PORT10_EN                       VSF_HW_EN_GPIOK
#define VSF_HW_GPIO_PORT10_RST                      VSF_HW_RST_GPIOK

// USART UART

#define VSF_HW_USART_COUNT                          15
#ifndef VSF_HW_USART_MASK
#   define VSF_HW_USART_MASK                        0xFFFE
#elif VSF_HW_USART_MASK & ~0xFFFE
#   error VSF_HW_USART_MASK defines a usart not supported
#endif
#define VSF_HW_USART1_REG                           USART1
#define VSF_HW_USART1_CLK                           VSF_HW_CLK_USART1
#define VSF_HW_USART1_EN                            VSF_HW_EN_USART1
#define VSF_HW_USART1_RST                           VSF_HW_RST_USART1
#define VSF_HW_USART1_SYNC                          true
#define VSF_HW_USART1_IRQN                          USART1_IRQn
#define VSF_HW_USART1_IRQHandler                    USART1_IRQHandler
#define VSF_HW_USART2_REG                           USART2
#define VSF_HW_USART2_CLK                           VSF_HW_CLK_USART2
#define VSF_HW_USART2_EN                            VSF_HW_EN_USART2
#define VSF_HW_USART2_RST                           VSF_HW_RST_USART2
#define VSF_HW_USART2_SYNC                          true
#define VSF_HW_USART2_IRQN                          USART2_IRQn
#define VSF_HW_USART2_IRQHandler                    USART2_IRQHandler
#define VSF_HW_USART3_REG                           USART3
#define VSF_HW_USART3_CLK                           VSF_HW_CLK_USART3
#define VSF_HW_USART3_EN                            VSF_HW_EN_USART3
#define VSF_HW_USART3_RST                           VSF_HW_RST_USART3
#define VSF_HW_USART3_SYNC                          true
#define VSF_HW_USART3_IRQN                          USART3_IRQn
#define VSF_HW_USART3_IRQHandler                    USART3_IRQHandler
#define VSF_HW_USART4_REG                           USART4
#define VSF_HW_USART4_CLK                           VSF_HW_CLK_USART4
#define VSF_HW_USART4_EN                            VSF_HW_EN_USART4
#define VSF_HW_USART4_RST                           VSF_HW_RST_USART4
#define VSF_HW_USART4_SYNC                          true
#define VSF_HW_USART4_IRQN                          USART4_IRQn
#define VSF_HW_USART4_IRQHandler                    USRT4_IRQHandler
#define VSF_HW_USART5_REG                           USART5
#define VSF_HW_USART5_CLK                           VSF_HW_CLK_USART5
#define VSF_HW_USART5_EN                            VSF_HW_EN_USART5
#define VSF_HW_USART5_RST                           VSF_HW_RST_USART5
#define VSF_HW_USART5_SYNC                          true
#define VSF_HW_USART5_IRQN                          USART5_IRQn
#define VSF_HW_USART5_IRQHandler                    USART5_IRQHandler
#define VSF_HW_USART6_REG                           USART6
#define VSF_HW_USART6_CLK                           VSF_HW_CLK_USART6
#define VSF_HW_USART6_EN                            VSF_HW_EN_USART6
#define VSF_HW_USART6_RST                           VSF_HW_RST_USART6
#define VSF_HW_USART6_SYNC                          true
#define VSF_HW_USART6_IRQN                          USART6_IRQn
#define VSF_HW_USART6_IRQHandler                    USART6_IRQHandler
#define VSF_HW_USART7_REG                           USART7
#define VSF_HW_USART7_CLK                           VSF_HW_CLK_USART7
#define VSF_HW_USART7_EN                            VSF_HW_EN_USART7
#define VSF_HW_USART7_RST                           VSF_HW_RST_USART7
#define VSF_HW_USART7_SYNC                          true
#define VSF_HW_USART7_IRQN                          USART7_IRQn
#define VSF_HW_USART7_IRQHandler                    USART7_IRQHandler
#define VSF_HW_USART8_REG                           USART8
#define VSF_HW_USART8_CLK                           VSF_HW_CLK_USART8
#define VSF_HW_USART8_EN                            VSF_HW_EN_USART8
#define VSF_HW_USART8_RST                           VSF_HW_RST_USART8
#define VSF_HW_USART8_SYNC                          true
#define VSF_HW_USART8_IRQN                          USART8_IRQn
#define VSF_HW_USART8_IRQHandler                    USART8_IRQHandler
#define VSF_HW_USART9_REG                           UART9
#define VSF_HW_USART9_CLK                           VSF_HW_CLK_UART9
#define VSF_HW_USART9_EN                            VSF_HW_EN_UART9
#define VSF_HW_USART9_RST                           VSF_HW_RST_UART9
#define VSF_HW_USART9_SYNC                          false
#define VSF_HW_USART9_IRQN                          UART9_IRQn
#define VSF_HW_USART9_IRQHandler                    UART9_IRQHandler
#define VSF_HW_USART10_REG                          UART10
#define VSF_HW_USART10_CLK                          VSF_HW_CLK_UART10
#define VSF_HW_USART10_EN                           VSF_HW_EN_UART10
#define VSF_HW_USART10_RST                          VSF_HW_RST_UART10
#define VSF_HW_USART10_SYNC                         false
#define VSF_HW_USART10_IRQN                         UART10_IRQn
#define VSF_HW_USART10_IRQHandler                   UART10_IRQHandler
#define VSF_HW_USART11_REG                          UART11
#define VSF_HW_USART11_CLK                          VSF_HW_CLK_UART11
#define VSF_HW_USART11_EN                           VSF_HW_EN_UART11
#define VSF_HW_USART11_RST                          VSF_HW_RST_UART11
#define VSF_HW_USART11_SYNC                         false
#define VSF_HW_USART11_IRQN                         UART11_IRQn
#define VSF_HW_USART11_IRQHandler                   UART11_IRQHandler
#define VSF_HW_USART12_REG                          UART12
#define VSF_HW_USART12_CLK                          VSF_HW_CLK_UART12
#define VSF_HW_USART12_EN                           VSF_HW_EN_UART12
#define VSF_HW_USART12_RST                          VSF_HW_RST_UART12
#define VSF_HW_USART12_SYNC                         false
#define VSF_HW_USART12_IRQN                         UART12_IRQn
#define VSF_HW_USART12_IRQHandler                   UART12_IRQHandler
#define VSF_HW_USART13_REG                          UART13
#define VSF_HW_USART13_CLK                          VSF_HW_CLK_UART13
#define VSF_HW_USART13_EN                           VSF_HW_EN_UART13
#define VSF_HW_USART13_RST                          VSF_HW_RST_UART13
#define VSF_HW_USART13_SYNC                         false
#define VSF_HW_USART13_IRQN                         UART13_IRQn
#define VSF_HW_USART13_IRQHandler                   UART13_IRQHandler
#define VSF_HW_USART14_REG                          UART14
#define VSF_HW_USART14_CLK                          VSF_HW_CLK_UART14
#define VSF_HW_USART14_EN                           VSF_HW_EN_UART14
#define VSF_HW_USART14_RST                          VSF_HW_RST_UART14
#define VSF_HW_USART14_SYNC                         false
#define VSF_HW_USART14_IRQN                         UART14_IRQn
#define VSF_HW_USART14_IRQHandler                   UART14_IRQHandler
#define VSF_HW_USART15_REG                          UART15
#define VSF_HW_USART15_CLK                          VSF_HW_CLK_UART15
#define VSF_HW_USART15_EN                           VSF_HW_EN_UART15
#define VSF_HW_USART15_RST                          VSF_HW_RST_UART15
#define VSF_HW_USART15_SYNC                         false
#define VSF_HW_USART15_IRQN                         UART15_IRQn
#define VSF_HW_USART15_IRQHandler                   UART15_IRQHandler

// SPI

#define VSF_HW_SPI_COUNT                            7
#define VSF_HW_SPI_MASK                             0x00FE
#define VSF_HW_SPI1_REG                             SPI1
#define VSF_HW_SPI1_CLK                             VSF_HW_CLK_SPI1
#define VSF_HW_SPI1_EN                              VSF_HW_EN_SPI1
#define VSF_HW_SPI1_RST                             VSF_HW_RST_SPI1
#define VSF_HW_SPI1_IRQN                            SPI1_IRQn
#define VSF_HW_SPI1_IRQ                             SPI1_IRQHandler
#define VSF_HW_SPI2_REG                             SPI2
#define VSF_HW_SPI2_CLK                             VSF_HW_CLK_SPI2
#define VSF_HW_SPI2_EN                              VSF_HW_EN_SPI2
#define VSF_HW_SPI2_RST                             VSF_HW_RST_SPI2
#define VSF_HW_SPI2_IRQN                            SPI2_IRQn
#define VSF_HW_SPI2_IRQ                             SPI2_IRQHandler
#define VSF_HW_SPI3_REG                             SPI3
#define VSF_HW_SPI3_CLK                             VSF_HW_CLK_SPI3
#define VSF_HW_SPI3_EN                              VSF_HW_EN_SPI3
#define VSF_HW_SPI3_RST                             VSF_HW_RST_SPI3
#define VSF_HW_SPI3_IRQN                            SPI3_IRQn
#define VSF_HW_SPI3_IRQ                             SPI3_IRQHandler
#define VSF_HW_SPI4_REG                             SPI4
#define VSF_HW_SPI4_CLK                             VSF_HW_CLK_SPI4
#define VSF_HW_SPI4_EN                              VSF_HW_EN_SPI4
#define VSF_HW_SPI4_RST                             VSF_HW_RST_SPI4
#define VSF_HW_SPI4_IRQN                            SPI4_IRQn
#define VSF_HW_SPI4_IRQ                             SPI4_IRQHandler
#define VSF_HW_SPI5_REG                             SPI5
#define VSF_HW_SPI5_CLK                             VSF_HW_CLK_SPI5
#define VSF_HW_SPI5_EN                              VSF_HW_EN_SPI5
#define VSF_HW_SPI5_RST                             VSF_HW_RST_SPI5
#define VSF_HW_SPI5_IRQN                            SPI5_IRQn
#define VSF_HW_SPI5_IRQ                             SPI5_IRQHandler
#define VSF_HW_SPI6_REG                             SPI6
#define VSF_HW_SPI6_CLK                             VSF_HW_CLK_SPI6
#define VSF_HW_SPI6_EN                              VSF_HW_EN_SPI6
#define VSF_HW_SPI6_RST                             VSF_HW_RST_SPI6
#define VSF_HW_SPI6_IRQN                            SPI6_IRQn
#define VSF_HW_SPI6_IRQ                             SPI6_IRQHandler
#define VSF_HW_SPI7_REG                             SPI7
#define VSF_HW_SPI7_CLK                             VSF_HW_CLK_SPI7
#define VSF_HW_SPI7_EN                              VSF_HW_EN_SPI7
#define VSF_HW_SPI7_RST                             VSF_HW_RST_SPI7
#define VSF_HW_SPI7_IRQN                            SPI7_IRQn
#define VSF_HW_SPI7_IRQ                             SPI7_IRQHandler

// I2C

#define VSF_HW_I2C_COUNT                            10
#define VSF_HW_I2C_MASK                             0x07FE
#define VSF_HW_I2C1_REG                             I2C1
#define VSF_HW_I2C1_CLK                             VSF_HW_CLK_I2C1
#define VSF_HW_I2C1_EN                              VSF_HW_EN_I2C1
#define VSF_HW_I2C1_RST                             VSF_HW_RST_I2C1
#define VSF_HW_I2C1_IRQN                            I2C1_IRQn
#define VSF_HW_I2C1_IRQ                             I2C1_IRQHandler
#define VSF_HW_I2C2_REG                             I2C2
#define VSF_HW_I2C2_CLK                             VSF_HW_CLK_I2C2
#define VSF_HW_I2C2_EN                              VSF_HW_EN_I2C2
#define VSF_HW_I2C2_RST                             VSF_HW_RST_I2C2
#define VSF_HW_I2C2_IRQN                            I2C2_IRQn
#define VSF_HW_I2C2_IRQ                             I2C2_IRQHandler
#define VSF_HW_I2C3_REG                             I2C3
#define VSF_HW_I2C3_CLK                             VSF_HW_CLK_I2C3
#define VSF_HW_I2C3_EN                              VSF_HW_EN_I2C3
#define VSF_HW_I2C3_RST                             VSF_HW_RST_I2C3
#define VSF_HW_I2C3_IRQN                            I2C3_IRQn
#define VSF_HW_I2C3_IRQ                             I2C3_IRQHandler
#define VSF_HW_I2C4_REG                             I2C4
#define VSF_HW_I2C4_CLK                             VSF_HW_CLK_I2C4
#define VSF_HW_I2C4_EN                              VSF_HW_EN_I2C4
#define VSF_HW_I2C4_RST                             VSF_HW_RST_I2C4
#define VSF_HW_I2C4_IRQN                            I2C4_IRQn
#define VSF_HW_I2C4_IRQ                             I2C4_IRQHandler
#define VSF_HW_I2C5_REG                             I2C5
#define VSF_HW_I2C5_CLK                             VSF_HW_CLK_I2C5
#define VSF_HW_I2C5_EN                              VSF_HW_EN_I2C5
#define VSF_HW_I2C5_RST                             VSF_HW_RST_I2C5
#define VSF_HW_I2C5_IRQN                            I2C5_IRQn
#define VSF_HW_I2C5_IRQ                             I2C5_IRQHandler
#define VSF_HW_I2C6_REG                             I2C6
#define VSF_HW_I2C6_CLK                             VSF_HW_CLK_I2C6
#define VSF_HW_I2C6_EN                              VSF_HW_EN_I2C6
#define VSF_HW_I2C6_RST                             VSF_HW_RST_I2C6
#define VSF_HW_I2C6_IRQN                            I2C6_IRQn
#define VSF_HW_I2C6_IRQ                             I2C6_IRQHandler
#define VSF_HW_I2C7_REG                             I2C7
#define VSF_HW_I2C7_CLK                             VSF_HW_CLK_I2C7
#define VSF_HW_I2C7_EN                              VSF_HW_EN_I2C7
#define VSF_HW_I2C7_RST                             VSF_HW_RST_I2C7
#define VSF_HW_I2C7_IRQN                            I2C7_IRQn
#define VSF_HW_I2C7_IRQ                             I2C7_IRQHandler
#define VSF_HW_I2C8_REG                             I2C8
#define VSF_HW_I2C8_CLK                             VSF_HW_CLK_I2C8
#define VSF_HW_I2C8_EN                              VSF_HW_EN_I2C8
#define VSF_HW_I2C8_RST                             VSF_HW_RST_I2C8
#define VSF_HW_I2C8_IRQN                            I2C8_IRQn
#define VSF_HW_I2C8_IRQ                             I2C8_IRQHandler
#define VSF_HW_I2C9_REG                             I2C9
#define VSF_HW_I2C9_CLK                             VSF_HW_CLK_I2C9
#define VSF_HW_I2C9_EN                              VSF_HW_EN_I2C9
#define VSF_HW_I2C9_RST                             VSF_HW_RST_I2C9
#define VSF_HW_I2C9_IRQN                            I2C9_IRQn
#define VSF_HW_I2C9_IRQ                             I2C9_IRQHandler
#define VSF_HW_I2C10_REG                            I2C10
#define VSF_HW_I2C10_CLK                            VSF_HW_CLK_I2C10
#define VSF_HW_I2C10_EN                             VSF_HW_EN_I2C10
#define VSF_HW_I2C10_RST                            VSF_HW_RST_I2C10
#define VSF_HW_I2C10_IRQN                           I2C10_IRQn
#define VSF_HW_I2C10_IRQ                            I2C10_IRQHandler

// SDIO

#define VSF_HW_SDIO_COUNT                           2
#define VSF_HW_SDIO_MASK                            0x06
#define VSF_HW_SDIO1_REG                            SDMMC1
#define VSF_HW_SDIO1_PWR                            VSF_HW_PWR_SDMMC1
#define VSF_HW_SDIO1_CLK                            VSF_HW_CLK_SDMMC1
#define VSF_HW_SDIO1_EN                             VSF_HW_EN_SDMMC1
#define VSF_HW_SDIO1_RST                            VSF_HW_RST_SDMMC1
#define VSF_HW_SDIO1_IRQN                           SDMMC1_IRQn
#define VSF_HW_SDIO1_IRQ                            SDMMC1_IRQHandler
#define VSF_HW_SDIO2_REG                            SDMMC2
#define VSF_HW_SDIO2_PWR                            VSF_HW_PWR_SDMMC2
#define VSF_HW_SDIO2_CLK                            VSF_HW_CLK_SDMMC2
#define VSF_HW_SDIO2_EN                             VSF_HW_EN_SDMMC2
#define VSF_HW_SDIO2_RST                            VSF_HW_RST_SDMMC2
#define VSF_HW_SDIO2_IRQN                           SDMMC2_IRQn
#define VSF_HW_SDIO2_IRQ                            SDMMC2_IRQHandler

// USB OTG

#define VSF_HW_USB_OTG_COUNT                        2
// required by dwcotg, define the max ep number of dwcotg include ep0
#define USB_DWCOTG_MAX_EP_NUM                       16

#define VSF_HW_USB_OTG0_IRQHandler                  USB1_HS_IRQHandler
#define VSF_HW_USB_OTG0_CONFIG                                                  \
            .dc_ep_num                              = 9 << 1,                   \
            .hc_ep_num                              = 16,                       \
            .reg                                    = (void *)0x40100000,       \
            .wrap_reg                               = (void *)0x40140000,       \
            .irq                                    = USB1_HS_IRQn,             \
            .en                                     = VSF_HW_EN_USB1,           \
            .pwr                                    = &VSF_HW_PWR_USB1,         \
            /* vk_dwcotg_hw_info_t */                                           \
                .buffer_word_size                   = 1024,                     \
                .speed                              = USB_SPEED_HIGH,           \
                .dma_en                             = true,                     \
                .ulpi_en                            = false,                    \
                .utmi_en                            = false,                    \
                .vbus_en                            = false,

#define VSF_HW_USB_OTG1_IRQHandler                  USB2_HS_IRQHandler
#define VSF_HW_USB_OTG1_CONFIG                                                  \
            .dc_ep_num                              = 9 << 1,                   \
            .hc_ep_num                              = 16,                       \
            .reg                                    = (void *)0x40060000,       \
            .wrap_reg                               = (void *)0x400A0000,       \
            .irq                                    = USB1_HS_IRQn,             \
            .en                                     = VSF_HW_EN_USB2,           \
            .pwr                                    = &VSF_HW_PWR_USB2,         \
            /* vk_dwcotg_hw_info_t */                                           \
                .buffer_word_size                   = 1024,                     \
                .speed                              = USB_SPEED_HIGH,           \
                .dma_en                             = true,                     \
                .ulpi_en                            = false,                    \
                .utmi_en                            = false,                    \
                .vbus_en                            = false,

/*============================ INCLUDES ======================================*/

// Include common irq and af headers after peripherals are defined, so that
//  irq and af can be adjusted according to the dedicated device configuration.

#include "../common/device_irq.h"
#include "../common/device_af.h"

/*============================ MACROS ========================================*/

// MACROs here is only used to over-write MACROs defined in device_irq

// SWI, use unexisted DSI and ETHERCAT as SWI

#if     defined(CORE_CM4)

#   define SWI3_IRQn                                166
#   undef VSF_HW_INTERRUPT166
#   define VSF_HW_INTERRUPT166                      SWI3_IRQHandler

#   define SWI4_IRQn                                187
#   undef VSF_HW_INTERRUPT187
#   define VSF_HW_INTERRUPT187                      SWI4_IRQHandler

#   define SWI5_IRQn                                188
#   undef VSF_HW_INTERRUPT188
#   define VSF_HW_INTERRUPT188                      SWI5_IRQHandler

#   define SWI6_IRQn                                189
#   undef VSF_HW_INTERRUPT189
#   define VSF_HW_INTERRUPT189                      SWI6_IRQHandler

#   define SWI7_IRQn                                190
#   undef VSF_HW_INTERRUPT190
#   define VSF_HW_INTERRUPT190                      SWI7_IRQHandler

#else

#   define SWI6_IRQn                                166
#   undef VSF_HW_INTERRUPT166
#   define VSF_HW_INTERRUPT166                      SWI6_IRQHandler

#   define SWI7_IRQn                                187
#   undef VSF_HW_INTERRUPT187
#   define VSF_HW_INTERRUPT187                      SWI7_IRQHandler

#   define SWI8_IRQn                                188
#   undef VSF_HW_INTERRUPT188
#   define VSF_HW_INTERRUPT188                      SWI8_IRQHandler

#   define SWI9_IRQn                                189
#   undef VSF_HW_INTERRUPT189
#   define VSF_HW_INTERRUPT189                      SWI9_IRQHandler

#   define SWI10_IRQn                               190
#   undef VSF_HW_INTERRUPT190
#   define VSF_HW_INTERRUPT190                      SWI10_IRQHandler

#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#define DECLARE_VSF_HW_INTERRUPT(__N)                                           \
            extern void VSF_MCONNECT(VSF_HW_INTERRUPT, __N)(void);
VSF_MREPEAT(VSF_HW_INTERRUPTS_NUM, DECLARE_VSF_HW_INTERRUPT)

#endif      // __VSF_HAL_DEVICE_NATIONS_N32H765IIB7EC_H__
#endif
/* EOF */
