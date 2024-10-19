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

/*\note first define basic info for arch. */
#if defined(__VSF_HEADER_ONLY_SHOW_ARCH_INFO__)
//! arch info
#   define VSF_ARCH_PRI_NUM             16
#   define VSF_ARCH_PRI_BIT             4

// software interrupt provided by a dedicated device
#ifndef VSF_DEV_SWI_NUM
#   define VSF_DEV_SWI_NUM              7
#endif
#if VSF_DEV_SWI_NUM > VSF_ARCH_PRI_NUM
#   warning too many VSF_DEV_SWI_NUM, max is VSF_ARCH_PRI_NUM
#   undef VSF_DEV_SWI_NUM
#   define VSF_DEV_SWI_NUM              VSF_ARCH_PRI_NUM
#endif

#elif defined(__VSF_HAL_SHOW_VENDOR_INFO__)

#define __VSF_HEADER_ONLY_SHOW_VENDOR_INFO__
#include "../common/vendor/cmsis_device_h7rs/Include/stm32h7r7xx.h"

#else

#ifndef __HAL_DEVICE_ST_STM32H7R7L8_H__
#define __HAL_DEVICE_ST_STM32H7R7L8_H__

// software interrupt provided by a dedicated device
#define __VSF_DEF_SWI_LIST_DEF(__N, __PARAM)                                    \
                                        VSF_MCONNECT(SWI, __N, _IRQn),
#define VSF_DEV_SWI_LIST                VSF_MREPEAT(VSF_DEV_SWI_NUM, __VSF_DEF_SWI_LIST_DEF, NULL)

/*============================ INCLUDES ======================================*/

/*\note this is should be the only place where common.h is included.*/
#include "../common/common.h"

/*============================ MACROS ========================================*/

// SWI

#define SWI0_IRQn                       1
#define SWI1_IRQn                       11
#define SWI2_IRQn                       12
#define SWI3_IRQn                       14
#define SWI4_IRQn                       15
#define SWI5_IRQn                       110
#define SWI6_IRQn                       111

// GPIO

#define VSF_HW_GPIO_PORT_COUNT          11
#define VSF_HW_GPIO_PORT_MASK           0xF07F
#define VSF_HW_GPIO_PIN_COUNT           16
#define VSF_HW_GPIO_FUNCTION_MAX        16
#define VSF_HW_GPIO_PORT0_REG_BASE      GPIOA
#define VSF_HW_GPIO_PORT0_CLK_GATING    VSF_HW_EN_GPIOA
#define VSF_HW_GPIO_PORT0_RST           VSF_HW_RST_GPIOA
#define VSF_HW_GPIO_PORT1_REG_BASE      GPIOB
#define VSF_HW_GPIO_PORT1_CLK_GATING    VSF_HW_EN_GPIOB
#define VSF_HW_GPIO_PORT1_RST           VSF_HW_RST_GPIOB
#define VSF_HW_GPIO_PORT2_REG_BASE      GPIOC
#define VSF_HW_GPIO_PORT2_CLK_GATING    VSF_HW_EN_GPIOC
#define VSF_HW_GPIO_PORT2_RST           VSF_HW_RST_GPIOC
#define VSF_HW_GPIO_PORT3_REG_BASE      GPIOD
#define VSF_HW_GPIO_PORT3_CLK_GATING    VSF_HW_EN_GPIOD
#define VSF_HW_GPIO_PORT3_RST           VSF_HW_RST_GPIOD
#define VSF_HW_GPIO_PORT4_REG_BASE      GPIOE
#define VSF_HW_GPIO_PORT4_CLK_GATING    VSF_HW_EN_GPIOE
#define VSF_HW_GPIO_PORT4_RST           VSF_HW_RST_GPIOE
#define VSF_HW_GPIO_PORT5_REG_BASE      GPIOF
#define VSF_HW_GPIO_PORT5_CLK_GATING    VSF_HW_EN_GPIOF
#define VSF_HW_GPIO_PORT5_RST           VSF_HW_RST_GPIOF
#define VSF_HW_GPIO_PORT6_REG_BASE      GPIOG
#define VSF_HW_GPIO_PORT6_CLK_GATING    VSF_HW_EN_GPIOG
#define VSF_HW_GPIO_PORT6_RST           VSF_HW_RST_GPIOG
#define VSF_HW_GPIO_PORT12_REG_BASE     GPIOM
#define VSF_HW_GPIO_PORT12_CLK_GATING   VSF_HW_EN_GPIOM
#define VSF_HW_GPIO_PORT12_RST          VSF_HW_RST_GPIOM
#define VSF_HW_GPIO_PORT13_REG_BASE     GPION
#define VSF_HW_GPIO_PORT13_CLK_GATING   VSF_HW_EN_GPION
#define VSF_HW_GPIO_PORT13_RST          VSF_HW_RST_GPION
#define VSF_HW_GPIO_PORT14_REG_BASE     GPIOO
#define VSF_HW_GPIO_PORT14_CLK_GATING   VSF_HW_EN_GPIOO
#define VSF_HW_GPIO_PORT14_RST          VSF_HW_RST_GPIOO
#define VSF_HW_GPIO_PORT15_REG_BASE     GPIOP
#define VSF_HW_GPIO_PORT15_CLK_GATING   VSF_HW_EN_GPIOP
#define VSF_HW_GPIO_PORT15_RST          VSF_HW_RST_GPIOP

// FLASH

#define VSF_HW_FLASH_COUNT              1
#define VSF_HW_FLASH_IRQN               FLASH_IRQn
#define VSF_HW_FLASH_IRQHandler         FLASH_IRQHandler
#define VSF_HW_FLASH_PROG_WORD_CNT      4
#define VSF_HW_FLASH0_ADDR              0x08000000
#define VSF_HW_FLASH0_REG_BASE          FLASH

// UART/USART

#define VSF_HW_USART_COUNT              7
#define VSF_HW_USART_MASK               0x01BE      // start from 1
#define VSF_HW_USART1_REG_BASE          USART1
#define VSF_HW_USART1_CLK               VSF_HW_CLK_USART1
#define VSF_HW_USART1_EN                VSF_HW_EN_USART1
#define VSF_HW_USART1_RST               VSF_HW_RST_USART1
#define VSF_HW_USART1_SYNC              true
#define VSF_HW_USART1_IRQN              USART1_IRQn
#define VSF_HW_USART1_IRQHandler        USART1_IRQHandler
#define VSF_HW_USART2_REG_BASE          USART2
#define VSF_HW_USART2_CLK               VSF_HW_CLK_USART2
#define VSF_HW_USART2_EN                VSF_HW_EN_USART2
#define VSF_HW_USART2_RST               VSF_HW_RST_USART2
#define VSF_HW_USART2_SYNC              true
#define VSF_HW_USART2_IRQN              USART2_IRQn
#define VSF_HW_USART2_IRQHandler        USART2_IRQHandler
#define VSF_HW_USART3_REG_BASE          USART3
#define VSF_HW_USART3_CLK               VSF_HW_CLK_USART3
#define VSF_HW_USART3_EN                VSF_HW_EN_USART3
#define VSF_HW_USART3_RST               VSF_HW_RST_USART3
#define VSF_HW_USART3_SYNC              true
#define VSF_HW_USART3_IRQN              USART3_IRQn
#define VSF_HW_USART3_IRQHandler        USART3_IRQHandler
#define VSF_HW_USART4_REG_BASE          UART4
#define VSF_HW_USART4_CLK               VSF_HW_CLK_UART4
#define VSF_HW_USART4_EN                VSF_HW_EN_UART4
#define VSF_HW_USART4_RST               VSF_HW_RST_UART4
#define VSF_HW_USART4_SYNC              false
#define VSF_HW_USART4_IRQN              UART4_IRQn
#define VSF_HW_USART4_IRQHandler        UART4_IRQHandler
#define VSF_HW_USART5_REG_BASE          UART5
#define VSF_HW_USART5_CLK               VSF_HW_CLK_UART5
#define VSF_HW_USART5_EN                VSF_HW_EN_UART5
#define VSF_HW_USART5_RST               VSF_HW_RST_UART5
#define VSF_HW_USART5_SYNC              false
#define VSF_HW_USART5_IRQN              UART5_IRQn
#define VSF_HW_USART5_IRQHandler        UART5_IRQHandler
#define VSF_HW_USART7_REG_BASE          UART7
#define VSF_HW_USART7_CLK               VSF_HW_CLK_UART7
#define VSF_HW_USART7_EN                VSF_HW_EN_UART7
#define VSF_HW_USART7_RST               VSF_HW_RST_UART7
#define VSF_HW_USART7_SYNC              false
#define VSF_HW_USART7_IRQN              UART7_IRQn
#define VSF_HW_USART7_IRQHandler        UART7_IRQHandler
#define VSF_HW_USART8_REG_BASE          UART8
#define VSF_HW_USART8_CLK               VSF_HW_CLK_UART8
#define VSF_HW_USART8_EN                VSF_HW_EN_UART8
#define VSF_HW_USART8_RST               VSF_HW_RST_UART8
#define VSF_HW_USART8_SYNC              false
#define VSF_HW_USART8_IRQN              UART8_IRQn
#define VSF_HW_USART8_IRQHandler        UART8_IRQHandler

// SDIO

#define VSF_HW_SDIO_COUNT               2
#define VSF_HW_SDIO_MASK                0x06        // start from 1
#define VSF_HW_SDIO1_REG_BASE           SDMMC1
#define VSF_HW_SDIO1_CLK                VSF_HW_CLK_SDMMC1
#define VSF_HW_SDIO1_EN                 VSF_HW_EN_SDMMC1
#define VSF_HW_SDIO1_RST                VSF_HW_RST_SDMMC1
#define VSF_HW_SDIO1_IRQN               SDMMC1_IRQn
#define VSF_HW_SDIO1_IRQHandler         SDMMC1_IRQHandler
#define VSF_HW_SDIO2_REG_BASE           SDMMC2
#define VSF_HW_SDIO2_CLK                VSF_HW_CLK_SDMMC2
#define VSF_HW_SDIO2_EN                 VSF_HW_EN_SDMMC2
#define VSF_HW_SDIO2_RST                VSF_HW_RST_SDMMC2
#define VSF_HW_SDIO2_IRQN               SDMMC2_IRQn
#define VSF_HW_SDIO2_IRQHandler         SDMMC2_IRQHandler

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __HAL_DEVICE_ST_STM32H7R7L8_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
