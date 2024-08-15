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
#define VSF_HW_GPIO_PORT0_EN            VSF_HW_EN_GPIOA
#define VSF_HW_GPIO_PORT0_RST           VSF_HW_RST_GPIOA
#define VSF_HW_GPIO_PORT1_REG_BASE      GPIOB
#define VSF_HW_GPIO_PORT1_EN            VSF_HW_EN_GPIOB
#define VSF_HW_GPIO_PORT1_RST           VSF_HW_RST_GPIOB
#define VSF_HW_GPIO_PORT2_REG_BASE      GPIOC
#define VSF_HW_GPIO_PORT2_EN            VSF_HW_EN_GPIOC
#define VSF_HW_GPIO_PORT2_RST           VSF_HW_RST_GPIOC
#define VSF_HW_GPIO_PORT3_REG_BASE      GPIOD
#define VSF_HW_GPIO_PORT3_EN            VSF_HW_EN_GPIOD
#define VSF_HW_GPIO_PORT3_RST           VSF_HW_RST_GPIOD
#define VSF_HW_GPIO_PORT4_REG_BASE      GPIOE
#define VSF_HW_GPIO_PORT4_EN            VSF_HW_EN_GPIOE
#define VSF_HW_GPIO_PORT4_RST           VSF_HW_RST_GPIOE
#define VSF_HW_GPIO_PORT5_REG_BASE      GPIOF
#define VSF_HW_GPIO_PORT5_EN            VSF_HW_EN_GPIOF
#define VSF_HW_GPIO_PORT5_RST           VSF_HW_RST_GPIOF
#define VSF_HW_GPIO_PORT6_REG_BASE      GPIOG
#define VSF_HW_GPIO_PORT6_EN            VSF_HW_EN_GPIOG
#define VSF_HW_GPIO_PORT6_RST           VSF_HW_RST_GPIOG
#define VSF_HW_GPIO_PORT7_REG_BASE      GPIOH
#define VSF_HW_GPIO_PORT7_EN            VSF_HW_EN_GPIOH
#define VSF_HW_GPIO_PORT7_RST           VSF_HW_RST_GPIOH
#define VSF_HW_GPIO_PORT9_REG_BASE      GPIOJ
#define VSF_HW_GPIO_PORT9_EN            VSF_HW_EN_GPIOJ
#define VSF_HW_GPIO_PORT9_RST           VSF_HW_RST_GPIOJ
#define VSF_HW_GPIO_PORT10_REG_BASE     GPIOK
#define VSF_HW_GPIO_PORT10_EN           VSF_HW_EN_GPIOK
#define VSF_HW_GPIO_PORT10_RST          VSF_HW_RST_GPIOK

#define VSF_HW_USART_COUNT              8
#define VSF_HW_USART0_REG_BASE          USART0
#define VSF_HW_USART0_CLK               VSF_HW_CLK_USART0
#define VSF_HW_USART0_EN                VSF_HW_EN_USART0
#define VSF_HW_USART0_RST               VSF_HW_RST_USART0
#define VSF_HW_USART0_SYNC              true
#define VSF_HW_USART0_IRQN              USART0_IRQn
#define VSF_HW_USART0_IRQHandler        USART0_IRQHandler
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
#define VSF_HW_USART3_REG_BASE          UART3
#define VSF_HW_USART3_CLK               VSF_HW_CLK_UART3
#define VSF_HW_USART3_EN                VSF_HW_EN_UART3
#define VSF_HW_USART3_RST               VSF_HW_RST_UART3
#define VSF_HW_USART3_SYNC              false
#define VSF_HW_USART3_IRQN              UART3_IRQn
#define VSF_HW_USART3_IRQHandler        UART3_IRQHandler
#define VSF_HW_USART4_REG_BASE          UART4
#define VSF_HW_USART4_CLK               VSF_HW_CLK_UART4
#define VSF_HW_USART4_EN                VSF_HW_EN_UART4
#define VSF_HW_USART4_RST               VSF_HW_RST_UART4
#define VSF_HW_USART4_SYNC              false
#define VSF_HW_USART4_IRQN              UART4_IRQn
#define VSF_HW_USART4_IRQHandler        UART4_IRQHandler
#define VSF_HW_USART5_REG_BASE          USART5
#define VSF_HW_USART5_CLK               VSF_HW_CLK_USART5
#define VSF_HW_USART5_EN                VSF_HW_EN_USART5
#define VSF_HW_USART5_RST               VSF_HW_RST_USART5
#define VSF_HW_USART5_SYNC              true
#define VSF_HW_USART5_IRQN              USART5_IRQn
#define VSF_HW_USART5_IRQHandler        USART5_IRQHandler
#define VSF_HW_USART6_REG_BASE          UART6
#define VSF_HW_USART6_CLK               VSF_HW_CLK_UART6
#define VSF_HW_USART6_EN                VSF_HW_EN_UART6
#define VSF_HW_USART6_RST               VSF_HW_RST_UART6
#define VSF_HW_USART6_SYNC              false
#define VSF_HW_USART6_IRQN              UART6_IRQn
#define VSF_HW_USART6_IRQHandler        UART6_IRQHandler
#define VSF_HW_USART7_REG_BASE          UART7
#define VSF_HW_USART7_CLK               VSF_HW_CLK_UART7
#define VSF_HW_USART7_EN                VSF_HW_EN_UART7
#define VSF_HW_USART7_RST               VSF_HW_RST_UART7
#define VSF_HW_USART7_SYNC              false
#define VSF_HW_USART7_IRQN              UART7_IRQn
#define VSF_HW_USART7_IRQHandler        UART7_IRQHandler

#define VSF_HW_FLASH_COUNT              1
#define VSF_HW_FLASH0_ADDR              0x08000000
#define VSF_HW_FLASH0_REG_BASE          FMC
#define VSF_HW_FLASH0_IRQN              FMC_IRQn
#define VSF_HW_FLASH0_IRQHandler        FMC_IRQHandler

#define VSF_HW_SDIO_COUNT               2
#define VSF_HW_SDIO0_REG_BASE           SDIO0
#define VSF_HW_SDIO0_CLK                VSF_HW_CLK_SDIO0
#define VSF_HW_SDIO0_EN                 VSF_HW_EN_SDIO0
#define VSF_HW_SDIO0_RST                VSF_HW_RST_SDIO0
#define VSF_HW_SDIO0_IRQN               SDIO0_IRQn
#define VSF_HW_SDIO0_IRQHandler         SDIO0_IRQHandler
#define VSF_HW_SDIO1_REG_BASE           SDIO1
#define VSF_HW_SDIO1_CLK                VSF_HW_CLK_SDIO1
#define VSF_HW_SDIO1_EN                 VSF_HW_EN_SDIO1
#define VSF_HW_SDIO1_RST                VSF_HW_RST_SDIO1
#define VSF_HW_SDIO1_IRQN               SDIO1_IRQn
#define VSF_HW_SDIO1_IRQHandler         SDIO1_IRQHandler

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __HAL_DEVICE_GIGADEVICE_GD32H7XX_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
