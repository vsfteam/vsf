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
#   define VSF_DEV_SWI_NUM              11
#endif
#if VSF_DEV_SWI_NUM > VSF_ARCH_PRI_NUM
#   warning too many VSF_DEV_SWI_NUM, max is VSF_ARCH_PRI_NUM
#   undef VSF_DEV_SWI_NUM
#   define VSF_DEV_SWI_NUM              VSF_ARCH_PRI_NUM
#endif

#elif defined(__VSF_HAL_SHOW_VENDOR_INFO__)

#define __VSF_HEADER_ONLY_SHOW_VENDOR_INFO__
#include "../common/vendor/Inc/stm32h7xx.h"

#else

#ifndef __HAL_DEVICE_ST_STM32H743VI_H__
#define __HAL_DEVICE_ST_STM32H743VI_H__

// software interrupt provided by a dedicated device
#define __VSF_DEF_SWI_LIST_DEF(__N, __PARAM)                                    \
                                        VSF_MCONNECT(SWI, __N, _IRQn),
#define VSF_DEV_SWI_LIST                VSF_MREPEAT(VSF_DEV_SWI_NUM, __VSF_DEF_SWI_LIST_DEF, NULL)

/*============================ INCLUDES ======================================*/

/*\note this is should be the only place where common.h is included.*/
#include "../common/common.h"

/*============================ MACROS ========================================*/

// SWI

#define SWI0_IRQn                       42
#define SWI1_IRQn                       66
#define SWI2_IRQn                       67
#define SWI3_IRQn                       147

#define SWI4_IRQn                       64
#define SWI4_IRQHandler                 CM7_SEV_IRQHandler
#define SWI5_IRQn                       65
#define SWI5_IRQHandler                 CM4_SEV_IRQHandler
#define SWI6_IRQn                       79
#define SWI6_IRQHandler                 CRYP_IRQHandler
#define SWI7_IRQn                       123
#define SWI7_IRQHandler                 DSI_IRQHandler
#define SWI8_IRQn                       126
#define SWI8_IRQHandler                 HSEM2_IRQHandler
#define SWI9_IRQn                       143
#define SWI9_IRQHandler                 WWDG_RST_IRQHandler
#define SWI10_IRQn                      148
#define SWI10_IRQHandler                HOLD_CORE_IRQHandler

// GPIO

#define VSF_HW_GPIO_PORT_COUNT          11
#define VSF_HW_GPIO_PIN_COUNT           16
#define VSF_HW_GPIO_FUNCTION_MAX        16
#define VSF_HW_GPIO_PORT0_REG_BASE      GPIOA
#define VSF_HW_GPIO_PORT0_CLK_GATING    RCU_GPIOA
#define VSF_HW_GPIO_PORT0_RST           RCU_GPIOARST
#define VSF_HW_GPIO_PORT1_REG_BASE      GPIOB
#define VSF_HW_GPIO_PORT1_CLK_GATING    RCU_GPIOB
#define VSF_HW_GPIO_PORT1_RST           RCU_GPIOBRST
#define VSF_HW_GPIO_PORT2_REG_BASE      GPIOC
#define VSF_HW_GPIO_PORT2_CLK_GATING    RCU_GPIOC
#define VSF_HW_GPIO_PORT2_RST           RCU_GPIOCRST
#define VSF_HW_GPIO_PORT3_REG_BASE      GPIOD
#define VSF_HW_GPIO_PORT3_CLK_GATING    RCU_GPIOD
#define VSF_HW_GPIO_PORT3_RST           RCU_GPIODRST
#define VSF_HW_GPIO_PORT4_REG_BASE      GPIOE
#define VSF_HW_GPIO_PORT4_CLK_GATING    RCU_GPIOE
#define VSF_HW_GPIO_PORT4_RST           RCU_GPIOERST
#define VSF_HW_GPIO_PORT5_REG_BASE      GPIOF
#define VSF_HW_GPIO_PORT5_CLK_GATING    RCU_GPIOF
#define VSF_HW_GPIO_PORT5_RST           RCU_GPIOFRST
#define VSF_HW_GPIO_PORT6_REG_BASE      GPIOG
#define VSF_HW_GPIO_PORT6_CLK_GATING    RCU_GPIOG
#define VSF_HW_GPIO_PORT6_RST           RCU_GPIOGRST
#define VSF_HW_GPIO_PORT7_REG_BASE      GPIOH
#define VSF_HW_GPIO_PORT7_CLK_GATING    RCU_GPIOH
#define VSF_HW_GPIO_PORT7_RST           RCU_GPIOHRST
#define VSF_HW_GPIO_PORT8_REG_BASE      GPIOI
#define VSF_HW_GPIO_PORT8_CLK_GATING    RCU_GPIOI
#define VSF_HW_GPIO_PORT8_RST           RCU_GPIOIRST
#define VSF_HW_GPIO_PORT9_REG_BASE      GPIOJ
#define VSF_HW_GPIO_PORT9_CLK_GATING    RCU_GPIOJ
#define VSF_HW_GPIO_PORT9_RST           RCU_GPIOJRST
#define VSF_HW_GPIO_PORT10_REG_BASE     GPIOK
#define VSF_HW_GPIO_PORT10_CLK_GATING   RCU_GPIOK
#define VSF_HW_GPIO_PORT10_RST          RCU_GPIOKRST

// FLASH

#define VSF_HW_FLASH_COUNT              2
#define VSF_HW_FLASH_IRQN               FLASH_IRQn
#define VSF_HW_FLASH_IRQHandler         FLASH_IRQHandler
#define VSF_HW_FLASH0_ADDR              0x08000000
#define VSF_HW_FLASH0_REG_BASE          (&FLASH->KEYR1)
#define VSF_HW_FLASH1_ADDR              0x08100000
#define VSF_HW_FLASH1_REG_BASE          (&FLASH->KEYR2)

// UART/USART

#define VSF_HW_USART_COUNT              8
#define VSF_HW_USART_MASK               0x01FE      // start from 1
#define VSF_HW_USART1_REG_BASE          USART1
#define VSF_HW_USART1_CLK               VSF_HW_CLK_USART16
#define VSF_HW_USART1_EN                VSF_HW_EN_USART1
#define VSF_HW_USART1_RST               VSF_HW_RST_USART1
#define VSF_HW_USART1_SYNC              true
#define VSF_HW_USART1_IRQN              USART1_IRQn
#define VSF_HW_USART1_IRQHandler        USART1_IRQHandler
#define VSF_HW_USART2_REG_BASE          USART2
#define VSF_HW_USART2_CLK               VSF_HW_CLK_USART234578
#define VSF_HW_USART2_EN                VSF_HW_EN_USART2
#define VSF_HW_USART2_RST               VSF_HW_RST_USART2
#define VSF_HW_USART2_SYNC              true
#define VSF_HW_USART2_IRQN              USART2_IRQn
#define VSF_HW_USART2_IRQHandler        USART2_IRQHandler
#define VSF_HW_USART3_REG_BASE          USART3
#define VSF_HW_USART3_CLK               VSF_HW_CLK_USART234578
#define VSF_HW_USART3_EN                VSF_HW_EN_USART3
#define VSF_HW_USART3_RST               VSF_HW_RST_USART3
#define VSF_HW_USART3_SYNC              true
#define VSF_HW_USART3_IRQN              USART3_IRQn
#define VSF_HW_USART3_IRQHandler        USART3_IRQHandler
#define VSF_HW_USART4_REG_BASE          UART4
#define VSF_HW_USART4_CLK               VSF_HW_CLK_USART234578
#define VSF_HW_USART4_EN                VSF_HW_EN_USART4
#define VSF_HW_USART4_RST               VSF_HW_RST_USART4
#define VSF_HW_USART4_SYNC              false
#define VSF_HW_USART4_IRQN              UART4_IRQn
#define VSF_HW_USART4_IRQHandler        UART4_IRQHandler
#define VSF_HW_USART5_REG_BASE          UART5
#define VSF_HW_USART5_CLK               VSF_HW_CLK_USART234578
#define VSF_HW_USART5_EN                VSF_HW_EN_USART5
#define VSF_HW_USART5_RST               VSF_HW_RST_USART5
#define VSF_HW_USART5_SYNC              false
#define VSF_HW_USART5_IRQN              UART5_IRQn
#define VSF_HW_USART5_IRQHandler        UART5_IRQHandler
#define VSF_HW_USART6_REG_BASE          USART6
#define VSF_HW_USART6_CLK               VSF_HW_CLK_USART16
#define VSF_HW_USART6_EN                VSF_HW_EN_USART6
#define VSF_HW_USART6_RST               VSF_HW_RST_USART6
#define VSF_HW_USART6_SYNC              true
#define VSF_HW_USART6_IRQN              USART6_IRQn
#define VSF_HW_USART6_IRQHandler        USART6_IRQHandler
#define VSF_HW_USART7_REG_BASE          UART7
#define VSF_HW_USART7_CLK               VSF_HW_CLK_USART234578
#define VSF_HW_USART7_EN                VSF_HW_EN_USART7
#define VSF_HW_USART7_RST               VSF_HW_RST_USART7
#define VSF_HW_USART7_SYNC              false
#define VSF_HW_USART7_IRQN              UART7_IRQn
#define VSF_HW_USART7_IRQHandler        UART7_IRQHandler
#define VSF_HW_USART8_REG_BASE          UART8
#define VSF_HW_USART8_CLK               VSF_HW_CLK_USART234578
#define VSF_HW_USART8_EN                VSF_HW_EN_USART8
#define VSF_HW_USART8_RST               VSF_HW_RST_USART8
#define VSF_HW_USART8_SYNC              false
#define VSF_HW_USART8_IRQN              UART8_IRQn
#define VSF_HW_USART8_IRQHandler        UART8_IRQHandler

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __HAL_DEVICE_ST_STM32H743VI_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
