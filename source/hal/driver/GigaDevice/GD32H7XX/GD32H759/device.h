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

/*\note first define basic info for arch. */
#define VSF_ARCH_PRI_NUM            16
#define VSF_ARCH_PRI_BIT            4

// software interrupt provided by a dedicated device
#ifndef VSF_DEV_SWI_NUM
#   define VSF_DEV_SWI_NUM          5
#endif
#if VSF_DEV_SWI_NUM > VSF_ARCH_PRI_NUM
#   warning too many VSF_DEV_SWI_NUM, max is VSF_ARCH_PRI_NUM
#   undef VSF_DEV_SWI_NUM
#   define VSF_DEV_SWI_NUM          VSF_ARCH_PRI_NUM
#endif

#elif defined(__VSF_HAL_SHOW_VENDOR_INFO__)

#define __VSF_HEADER_ONLY_SHOW_VENDOR_INFO__
#include "../common/vendor/Include/gd32h7xx.h"

#else

#ifndef __HAL_DEVICE_GIGADEVICE_GD32H7XX_H__
#define __HAL_DEVICE_GIGADEVICE_GD32H7XX_H__

// software interrupt provided by a dedicated device
#define __VSF_DEF_SWI_LIST_DEF(__N, __PARAM)                                    \
                                    VSF_MCONNECT(SWI, __N, _IRQn),
#define VSF_DEV_SWI_LIST            VSF_MREPEAT(VSF_DEV_SWI_NUM, __VSF_DEF_SWI_LIST_DEF, NULL)

/*============================ INCLUDES ======================================*/

/*\note this is should be the only place where common.h is included.*/
#include "../common/common.h"

/*============================ MACROS ========================================*/

#define USB_OTG_COUNT               1
#define USB_OTG0_IRQHandler         USBFS_IRQHandler
// required by dwcotg, define the max ep number of dwcotg include ep0
#define USB_DWCOTG_MAX_EP_NUM       4

#define USB_OTG0_CONFIG                                                         \
    .ep_num = 8,                                                                \
    .irq = USBFS_IRQn,                                                          \
    .reg = (void *)USBFS_BASE,                                                  \
    .buffer_word_size = 0x500 >> 2,                                             \
    .speed = USB_SPEED_FULL,                                                    \
    .dma_en = false,                                                            \
    .ulpi_en = false,                                                           \
    .utmi_en = false,                                                           \
    .vbus_en = false,

#define VSF_HW_GPIO_PORT_COUNT          10
#define VSF_HW_GPIO_PORT_MASK           0x6FF       // no GPIOI
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
#define VSF_HW_GPIO_PORT9_REG_BASE      GPIOJ
#define VSF_HW_GPIO_PORT9_CLK_GATING    RCU_GPIOJ
#define VSF_HW_GPIO_PORT9_RST           RCU_GPIOJRST
#define VSF_HW_GPIO_PORT10_REG_BASE     GPIOK
#define VSF_HW_GPIO_PORT10_CLK_GATING   RCU_GPIOK
#define VSF_HW_GPIO_PORT10_RST          RCU_GPIOKRST

#define VSF_HW_USART_COUNT              8
#define VSF_HW_USART0_REG_BASE          USART0
#define VSF_HW_USART0_CLK               CK_USART0
#define VSF_HW_USART0_CLK_GATING        RCU_USART0
#define VSF_HW_USART0_RST               RCU_USART0RST
#define VSF_HW_USART0_SYNC              true
#define VSF_HW_USART0_IRQN              USART0_IRQn
#define VSF_HW_USART0_IRQHandler        USART0_IRQHandler
#define VSF_HW_USART1_REG_BASE          USART1
#define VSF_HW_USART1_CLK               CK_USART1
#define VSF_HW_USART1_CLK_GATING        RCU_USART1
#define VSF_HW_USART1_RST               RCU_USART1RST
#define VSF_HW_USART1_SYNC              true
#define VSF_HW_USART1_IRQN              USART1_IRQn
#define VSF_HW_USART1_IRQHandler        USART1_IRQHandler
#define VSF_HW_USART2_REG_BASE          USART2
#define VSF_HW_USART2_CLK               CK_USART2
#define VSF_HW_USART2_CLK_GATING        RCU_USART2
#define VSF_HW_USART2_RST               RCU_USART2RST
#define VSF_HW_USART2_SYNC              true
#define VSF_HW_USART2_IRQN              USART2_IRQn
#define VSF_HW_USART2_IRQHandler        USART2_IRQHandler
#define VSF_HW_USART3_REG_BASE          UART3
#define VSF_HW_USART3_CLK               CK_APB1
#define VSF_HW_USART3_CLK_GATING        RCU_UART3
#define VSF_HW_USART3_RST               RCU_UART3RST
#define VSF_HW_USART3_SYNC              false
#define VSF_HW_USART3_IRQN              UART3_IRQn
#define VSF_HW_USART3_IRQHandler        UART3_IRQHandler
#define VSF_HW_USART4_REG_BASE          UART4
#define VSF_HW_USART4_CLK               CK_APB1
#define VSF_HW_USART4_CLK_GATING        RCU_UART4
#define VSF_HW_USART4_RST               RCU_UART4RST
#define VSF_HW_USART4_SYNC              false
#define VSF_HW_USART4_IRQN              UART4_IRQn
#define VSF_HW_USART4_IRQHandler        UART4_IRQHandler
#define VSF_HW_USART5_REG_BASE          USART5
#define VSF_HW_USART5_CLK               CK_USART5
#define VSF_HW_USART5_CLK_GATING        RCU_USART5
#define VSF_HW_USART5_RST               RCU_USART5RST
#define VSF_HW_USART5_SYNC              true
#define VSF_HW_USART5_IRQN              USART5_IRQn
#define VSF_HW_USART5_IRQHandler        USART5_IRQHandler
#define VSF_HW_USART6_REG_BASE          UART6
#define VSF_HW_USART6_CLK               CK_APB1
#define VSF_HW_USART6_CLK_GATING        RCU_UART6
#define VSF_HW_USART6_RST               RCU_UART6RST
#define VSF_HW_USART6_SYNC              false
#define VSF_HW_USART6_IRQN              UART6_IRQn
#define VSF_HW_USART6_IRQHandler        UART6_IRQHandler
#define VSF_HW_USART7_REG_BASE          UART7
#define VSF_HW_USART7_CLK               CK_APB1
#define VSF_HW_USART7_CLK_GATING        RCU_UART7
#define VSF_HW_USART7_RST               RCU_UART7RST
#define VSF_HW_USART7_SYNC              false
#define VSF_HW_USART7_IRQN              UART7_IRQn
#define VSF_HW_USART7_IRQHandler        UART7_IRQHandler

#define VSF_HW_FLASH_COUNT              1
#define VSF_HW_FLASH0_ADDR              0x08000000
#define VSF_HW_FLASH0_REG_BASE          FMC
#define VSF_HW_FLASH0_IRQN              FMC_IRQn
#define VSF_HW_FLASH0_IRQHandler        FMC_IRQHandler

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __HAL_DEVICE_GIGADEVICE_GD32H7XX_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
