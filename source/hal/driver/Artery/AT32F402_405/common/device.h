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
/*============================ MACROS ========================================*/

#ifdef __VSF_HEADER_ONLY_SHOW_ARCH_INFO__

/*\note first define basic info for arch. */
//! arch info
#   define VSF_ARCH_PRI_NUM         8
#   define VSF_ARCH_PRI_BIT         3

#   define VSF_DEV_COMMON_SWI_NUM   32

#elif defined(__VSF_HAL_SHOW_VENDOR_INFO__)

/*\note __VSF_HAL_SHOW_VENDOR_INFO__ is defined to include vendor information only.
 *      Vendor information means the registers/structures/macros from vendor SDK.
 *      Usually these information are not visible from user side to avoid name-space pollution.
 */

#   define __VSF_HEADER_ONLY_SHOW_VENDOR_INFO__
#   include "./vendor/libraries/cmsis/cm4/device_support/at32f402_405.h"

#else

#ifndef __HAL_DEVICE_COMMON_ARTERY_AT32F402_405_H__
#define __HAL_DEVICE_COMMON_ARTERY_AT32F402_405_H__

/*============================ MACROS ========================================*/

// user configurations with default value

// HW definition

// SWI

#define VSF_DEV_COMMON_SWI_LIST                                                 \
    43, 46, 47, 48, 49,  50,  61,  62,                                          \
    63, 64, 65, 66, 70,  78,  79,  80,                                          \
    84, 86, 87, 88, 89,  90,  91,  93,                                          \
    95, 96, 97, 98, 99, 100, 101, 102

// RAM

#ifndef VSF_HW_RAM_COUNT
#   define VSF_HW_RAM_COUNT             1
#endif
#if VSF_HW_RAM_COUNT >= 1 && !defined(VSF_HW_RAM0_ADDR)
#   define VSF_HW_RAM0_ADDR             0x20000000
#endif

// FLASH

#ifndef VSF_HW_FLASH_COUNT
#   define VSF_HW_FLASH_COUNT           1
#endif
#if VSF_HW_FLASH_COUNT >= 1
#   define VSF_HW_FLASH0_ADDR           0x08000000
#   define VSF_HW_FLASH0_REG            FLASH
#   define VSF_HW_FLASH0_IRQN           4       // FLASH_IRQn
#endif

// GPIO: PORT0..PORT3, PORT5

#ifndef VSF_HW_GPIO_PORT_MASK
#   ifdef VSF_HW_GPIO_PORT_COUNT
#       define VSF_HW_GPIO_PORT_MASK    ((1 << VSF_HW_GPIO_PORT_COUNT) - 1)
#   else
#       define VSF_HW_GPIO_PORT_MASK    0x2F
#   endif
#endif
#if VSF_HW_GPIO_PORT_MASK & ~0x2F
#   error invalid VSF_HW_GPIO_PORT_MASK
#endif

#ifndef VSF_HW_GPIO_PIN_COUNT
#   define VSF_HW_GPIO_PIN_COUNT        16
#endif
#ifndef VSF_HW_GPIO_FUNCTION_MAX
#   define VSF_HW_GPIO_FUNCTION_MAX     16
#endif

#if VSF_HW_GPIO_PORT_MASK & (1 << 0)
#   define VSF_HW_GPIO_PORT0_REG        GPIOA
#   ifndef VSF_HW_GPIO_PORT0_MASK
#       define VSF_HW_GPIO_PORT0_MASK   0xFFFF
#   elif VSF_HW_GPIO_PORT0_MASK & ~0xFFFF
#       error invalid VSF_HW_GPIO_PORT0_MASK
#   endif
#endif
#if VSF_HW_GPIO_PORT_MASK & (1 << 1)
#   define VSF_HW_GPIO_PORT1_REG        GPIOB
#   ifndef VSF_HW_GPIO_PORT1_MASK
#       define VSF_HW_GPIO_PORT1_MASK   0xFFFF
#   elif VSF_HW_GPIO_PORT1_MASK & ~0xFFFF
#       error invalid VSF_HW_GPIO_PORT1_MASK
#   endif
#endif
#if VSF_HW_GPIO_PORT_MASK & (1 << 2)
#   define VSF_HW_GPIO_PORT2_REG        GPIOC
#   ifndef VSF_HW_GPIO_PORT2_MASK
#       define VSF_HW_GPIO_PORT2_MASK   0xFFFF
#   elif VSF_HW_GPIO_PORT2_MASK & ~0xFFFF
#       error invalid VSF_HW_GPIO_PORT2_MASK
#   endif
#endif
#if VSF_HW_GPIO_PORT_MASK & (1 << 3)
#   define VSF_HW_GPIO_PORT3_REG        GPIOD
#   ifndef VSF_HW_GPIO_PORT3_MASK
#       define VSF_HW_GPIO_PORT3_MASK   0x0004
#   elif VSF_HW_GPIO_PORT3_MASK & ~0xFFFF
#       error invalid VSF_HW_GPIO_PORT3_MASK
#   endif
#endif
#if VSF_HW_GPIO_PORT_MASK & (1 << 5)
#   define VSF_HW_GPIO_PORT5_REG        GPIOF
#   ifndef VSF_HW_GPIO_PORT5_MASK
#       define VSF_HW_GPIO_PORT5_MASK   0x08F3
#   elif VSF_HW_GPIO_PORT5_MASK & ~0x08F3
#       error invalid VSF_HW_GPIO_PORT5_MASK
#   endif
#endif

// DMA

#define VSF_HW_DMA_COUNT                2
#define VSF_HW_DMA_MASK                 0x6
#define VSF_HW_DMA_CHANNEL_NUM          7
#define VSF_HW_DMA1_REG                 DMA1
#define VSF_HW_DMA1_EN                  VSF_HW_EN_DMA1
#define VSF_HW_DMA1_RST                 VSF_HW_RST_DMA1
#   define VSF_HW_DMA1_CHANNEL0_IRQN    11
#   define VSF_HW_DMA1_CHANNEL1_IRQN    12
#   define VSF_HW_DMA1_CHANNEL2_IRQN    13
#   define VSF_HW_DMA1_CHANNEL3_IRQN    14
#   define VSF_HW_DMA1_CHANNEL4_IRQN    15
#   define VSF_HW_DMA1_CHANNEL5_IRQN    16
#   define VSF_HW_DMA1_CHANNEL6_IRQN    17
#define VSF_HW_DMA2_REG                 DMA2
#define VSF_HW_DMA2_EN                  VSF_HW_EN_DMA2
#define VSF_HW_DMA2_RST                 VSF_HW_RST_DMA2
#   define VSF_HW_DMA2_CHANNEL0_IRQN    56
#   define VSF_HW_DMA2_CHANNEL1_IRQN    57
#   define VSF_HW_DMA2_CHANNEL2_IRQN    58
#   define VSF_HW_DMA2_CHANNEL3_IRQN    59
#   define VSF_HW_DMA2_CHANNEL4_IRQN    60
#   define VSF_HW_DMA2_CHANNEL5_IRQN    68
#   define VSF_HW_DMA2_CHANNEL6_IRQN    69

// EXTI

#define VSF_HW_EXTI_COUNT               1
#define VSF_HW_EXTI_MASK                1
#ifndef VSF_HW_EXTI_CHANNEL_COUNT
#   define VSF_HW_EXTI_CHANNEL_COUNT    22
#endif
#ifndef VSF_HW_EXTI_CHANNEL_MASK
#   define VSF_HW_EXTI_CHANNEL_MASK     0x77FFFF
#endif
#ifndef VSF_HW_EXTI_IRQ_NUM
#   define VSF_HW_EXTI_IRQ_NUM          11
#endif

#if VSF_HW_EXTI_MASK & (1 << 0)
#   define VSF_HW_EXTI0_REG             EXINT
#   define VSF_HW_EXTI0_IRQ_NUM         11
#       define VSF_HW_EXTI0_IRQ0_MASK   (1 << 0)
#       define VSF_HW_EXTI0_IRQ0_IRQN   6       // EXINT0_IRQn
#       define VSF_HW_EXTI0_IRQ1_MASK   (1 << 1)
#       define VSF_HW_EXTI0_IRQ1_IRQN   7       // EXINT1_IRQn
#       define VSF_HW_EXTI0_IRQ2_MASK   (1 << 2)
#       define VSF_HW_EXTI0_IRQ2_IRQN   8       // EXINT1_IRQn
#       define VSF_HW_EXTI0_IRQ3_MASK   (1 << 3)
#       define VSF_HW_EXTI0_IRQ3_IRQN   9       // EXINT1_IRQn
#       define VSF_HW_EXTI0_IRQ4_MASK   (1 << 4)
#       define VSF_HW_EXTI0_IRQ4_IRQN   10       // EXINT1_IRQn
#       define VSF_HW_EXTI0_IRQ5_MASK   (0x1F << 5)
#       define VSF_HW_EXTI0_IRQ5_IRQN   23       // EXINT9_5_IRQn
#       define VSF_HW_EXTI0_IRQ6_MASK   (0x3F << 9)
#       define VSF_HW_EXTI0_IRQ6_IRQN   40       // EXINT15_10_IRQn
#       define VSF_HW_EXTI0_IRQ7_MASK   (1 << 16)
#       define VSF_HW_EXTI0_IRQ7_IRQN   1        // PVM_IRQn
#       define VSF_HW_EXTI0_IRQ8_MASK   (1 << 17)
#       define VSF_HW_EXTI0_IRQ8_IRQN   41       // ERTCAlarm_IRQn
#       define VSF_HW_EXTI0_IRQ9_MASK   (1 << 18)
#       define VSF_HW_EXTI0_IRQ9_IRQN   42       // OTGFS1_WKUP_IRQn
#       define VSF_HW_EXTI0_IRQ10_MASK  (1 << 20)
#       define VSF_HW_EXTI0_IRQ10_IRQN  76       // OTGHS_WKUP_IRQn
#endif

// USART: USART1..USART8

#ifndef VSF_HW_USART_MASK
#   ifdef VSF_HW_USART_COUNT
#       define VSF_HW_USART_MASK        ((1 << VSF_HW_USART_COUNT) - 1)
#   else
#       define VSF_HW_USART_MASK        0x01FE
#   endif
#endif
#if VSF_HW_USART_MASK & ~0x01FE
#   error invalid VSF_HW_USART_MASK
#endif

#if VSF_HW_USART_MASK & (1 << 1)
#   define VSF_HW_USART1_REG            USART1
#   define VSF_HW_USART1_CLK            VSF_HW_CLK_USART1
#   define VSF_HW_USART1_EN             VSF_HW_EN_USART1
#   define VSF_HW_USART1_RST            VSF_HW_RST_USART1
#   define VSF_HW_USART1_SYNC           true
#   define VSF_HW_USART1_IRQN           37      // USART1_IRQn
#endif
#if VSF_HW_USART_MASK & (1 << 2)
#   define VSF_HW_USART2_REG            USART2
#   define VSF_HW_USART2_CLK            VSF_HW_CLK_USART2
#   define VSF_HW_USART2_EN             VSF_HW_EN_USART2
#   define VSF_HW_USART2_RST            VSF_HW_RST_USART2
#   define VSF_HW_USART2_SYNC           true
#   define VSF_HW_USART2_IRQN           38      // USART2_IRQn
#endif
#if VSF_HW_USART_MASK & (1 << 3)
#   define VSF_HW_USART3_REG            USART3
#   define VSF_HW_USART3_CLK            VSF_HW_CLK_USART3
#   define VSF_HW_USART3_EN             VSF_HW_EN_USART3
#   define VSF_HW_USART3_RST            VSF_HW_RST_USART3
#   define VSF_HW_USART3_SYNC           true
#   define VSF_HW_USART3_IRQN           39      // USART3_IRQn
#endif
#if VSF_HW_USART_MASK & (1 << 4)
#   define VSF_HW_USART4_REG            USART4
#   define VSF_HW_USART4_CLK            VSF_HW_CLK_USART4
#   define VSF_HW_USART4_EN             VSF_HW_EN_USART4
#   define VSF_HW_USART4_RST            VSF_HW_RST_USART4
#   define VSF_HW_USART4_SYNC           true
#   define VSF_HW_USART4_IRQN           52      // USART4_IRQn
#endif
#if VSF_HW_USART_MASK & (1 << 5)
#   define VSF_HW_USART5_REG            USART5
#   define VSF_HW_USART5_CLK            VSF_HW_CLK_USART5
#   define VSF_HW_USART5_EN             VSF_HW_EN_USART5
#   define VSF_HW_USART5_RST            VSF_HW_RST_USART5
#   define VSF_HW_USART5_SYNC           true
#   define VSF_HW_USART5_IRQN           53      // USART5_IRQn
#endif
#if VSF_HW_USART_MASK & (1 << 6)
#   define VSF_HW_USART6_REG            USART6
#   define VSF_HW_USART6_CLK            VSF_HW_CLK_USART6
#   define VSF_HW_USART6_EN             VSF_HW_EN_USART6
#   define VSF_HW_USART6_RST            VSF_HW_RST_USART6
#   define VSF_HW_USART6_SYNC           true
#   define VSF_HW_USART6_IRQN           71      // USART6_IRQn
#endif
#if VSF_HW_USART_MASK & (1 << 7)
#   define VSF_HW_USART7_REG            UART7
#   define VSF_HW_USART7_CLK            VSF_HW_CLK_UART7
#   define VSF_HW_USART7_EN             VSF_HW_EN_UART7
#   define VSF_HW_USART7_RST            VSF_HW_RST_UART7
#   define VSF_HW_USART7_SYNC           false
#   define VSF_HW_USART7_IRQN           82      // UART7_IRQn
#endif
#if VSF_HW_USART_MASK & (1 << 8)
#   define VSF_HW_USART8_REG            UART8
#   define VSF_HW_USART8_CLK            VSF_HW_CLK_UART8
#   define VSF_HW_USART8_EN             VSF_HW_EN_UART8
#   define VSF_HW_USART8_RST            VSF_HW_RST_UART8
#   define VSF_HW_USART8_SYNC           false
#   define VSF_HW_USART8_IRQN           83      // UART8_IRQn
#endif

// SPI: SPI1..SPI3

#ifndef VSF_HW_SPI_MASK
#   ifdef VSF_HW_SPI_COUNT
#       define VSF_HW_SPI_MASK          ((1 << VSF_HW_SPI_COUNT) - 1)
#   else
#       define VSF_HW_SPI_MASK          0x0E
#   endif
#endif
#if VSF_HW_SPI_MASK & ~0x0E
#   error invalid VSF_HW_SPI_MASK
#endif

#if VSF_HW_SPI_MASK & (1 << 1)
#   define VSF_HW_SPI1_REG              SPI1
#   define VSF_HW_SPI1_CLK              VSF_HW_CLK_SPI1
#   define VSF_HW_SPI1_EN               VSF_HW_EN_SPI1
#   define VSF_HW_SPI1_RST              VSF_HW_RST_SPI1
#   define VSF_HW_SPI1_IRQN             35      // SPI1_IRQn
#   define VSF_HW_SPI1_TX_DMA_REQUEST   0x0B    // DMAMUX_DMAREQ_ID_SPI1_TX
#   define VSF_HW_SPI1_RX_DMA_REQUEST   0x0A    // DMAMUX_DMAREQ_ID_SPI1_RX
#endif
#if VSF_HW_SPI_MASK & (1 << 2)
#   define VSF_HW_SPI2_REG              SPI2
#   define VSF_HW_SPI2_CLK              VSF_HW_CLK_SPI2
#   define VSF_HW_SPI2_EN               VSF_HW_EN_SPI2
#   define VSF_HW_SPI2_RST              VSF_HW_RST_SPI2
#   define VSF_HW_SPI2_IRQN             36      // SPI2_IRQn
#   define VSF_HW_SPI2_TX_DMA_REQUEST   0x0D    // DMAMUX_DMAREQ_ID_SPI2_TX
#   define VSF_HW_SPI2_RX_DMA_REQUEST   0x0C    // DMAMUX_DMAREQ_ID_SPI2_RX
#endif
#if VSF_HW_SPI_MASK & (1 << 3)
#   define VSF_HW_SPI3_REG              SPI3
#   define VSF_HW_SPI3_CLK              VSF_HW_CLK_SPI3
#   define VSF_HW_SPI3_EN               VSF_HW_EN_SPI3
#   define VSF_HW_SPI3_RST              VSF_HW_RST_SPI3
#   define VSF_HW_SPI3_IRQN             51      // SPI3_IRQn
#   define VSF_HW_SPI3_TX_DMA_REQUEST   0x0F    // DMAMUX_DMAREQ_ID_SPI3_TX
#   define VSF_HW_SPI3_RX_DMA_REQUEST   0x0E    // DMAMUX_DMAREQ_ID_SPI3_RX
#endif

// I2C: I2C1..I2C3

#ifndef VSF_HW_I2C_MASK
#   ifdef VSF_HW_I2C_COUNT
#       define VSF_HW_I2C_MASK          ((1 << VSF_HW_I2C_COUNT) - 1)
#   else
#       define VSF_HW_I2C_MASK          0x0E
#   endif
#endif
#if VSF_HW_I2C_MASK & ~0x0E
#   error invalid VSF_HW_I2C_MASK
#endif

#if VSF_HW_I2C_MASK & (1 << 1)
#   define VSF_HW_I2C1_REG              I2C1
#   define VSF_HW_I2C1_CLK              VSF_HW_CLK_I2C1
#   define VSF_HW_I2C1_EN               VSF_HW_EN_I2C1
#   define VSF_HW_I2C1_RST              VSF_HW_RST_I2C1
#   define VSF_HW_I2C1_EVENT_IRQN       31      // I2C1_EVT_IRQn
#   define VSF_HW_I2C1_ERROR_IRQN       32      // I2C1_ERR_IRQn
#endif
#if VSF_HW_I2C_MASK & (1 << 2)
#   define VSF_HW_I2C2_REG              I2C2
#   define VSF_HW_I2C2_CLK              VSF_HW_CLK_I2C2
#   define VSF_HW_I2C2_EN               VSF_HW_EN_I2C2
#   define VSF_HW_I2C2_RST              VSF_HW_RST_I2C2
#   define VSF_HW_I2C2_EVENT_IRQN       33      // I2C2_EVT_IRQn
#   define VSF_HW_I2C2_ERROR_IRQN       34      // I2C2_ERR_IRQn
#endif
#if VSF_HW_I2C_MASK & (1 << 3)
#   define VSF_HW_I2C3_REG              I2C3
#   define VSF_HW_I2C3_CLK              VSF_HW_CLK_I2C3
#   define VSF_HW_I2C3_EN               VSF_HW_EN_I2C3
#   define VSF_HW_I2C3_RST              VSF_HW_RST_I2C3
#   define VSF_HW_I2C3_EVENT_IRQN       72      // I2C3_EVT_IRQn
#   define VSF_HW_I2C3_ERROR_IRQN       73      // I2C3_ERR_IRQn
#endif

// WDT: Independent Watchdog

#ifndef VSF_HW_WDT_COUNT
#   define VSF_HW_WDT_COUNT             1
#endif
#ifndef VSF_HW_WDT_MASK
#   define VSF_HW_WDT_MASK              0x1
#endif

#if VSF_HW_WDT_MASK & (1 << 0)
#   define VSF_HW_WDT0_REG              WDT
#   define VSF_HW_WDT0_TYPE             0   // WDT (Independent Watchdog)
#   define VSF_HW_WDT0_CLK              VSF_HW_CLK_LICK  // WDT uses LICK (40kHz)
#   define VSF_HW_WDT0_CLK_FREQ_HZ      40000
#endif

// WWDT: Window Watchdog (as separate device with device prefix)

#ifndef VSF_HW_WWDT_COUNT
#   define VSF_HW_WWDT_COUNT            1
#endif
#ifndef VSF_HW_WWDT_MASK
#   define VSF_HW_WWDT_MASK             0x1
#endif

#if VSF_HW_WWDT_MASK & (1 << 0)
#   define VSF_HW_WWDT0_REG             WWDT
#   define VSF_HW_WWDT0_TYPE            1   // WWDT (Window Watchdog)
#   define VSF_HW_WWDT0_CLK             VSF_HW_CLK_APB1  // WWDT uses APB1 (PCLK1)
#   define VSF_HW_WWDT0_EN              VSF_HW_EN_WWDT
#   define VSF_HW_WWDT0_RST             VSF_HW_RST_WWDT
#   define VSF_HW_WWDT0_IRQN            0   // WWDT has no dedicated IRQ, uses early wakeup interrupt
#endif

// USB OTG

#ifndef VSF_HW_USB_OTG_MASK
#   ifdef VSF_HW_USB_OTG_COUNT
#       define VSF_HW_USB_OTG_MASK      ((1 << VSF_HW_USB_OTG_COUNT) - 1)
#   else
#       define VSF_HW_USB_OTG_MASK      0x03
#   endif
#endif

// required by dwcotg, define the max ep number of dwcotg include ep0
#ifndef USB_DWCOTG_MAX_EP_NUM
#   define USB_DWCOTG_MAX_EP_NUM        16
#endif

#if VSF_HW_USB_OTG_MASK & (1 << 0)
#   define VSF_HW_USB_OTG0_IRQN         67      // OTGFS1_IRQn
#   define VSF_HW_USB_OTG0_CONFIG                                               \
            .dc_ep_num                  = 8 << 1,                               \
            .hc_ep_num                  = 16,                                   \
            .reg                        = (void *)OTGFS1_BASE,                  \
            .irq                        = VSF_HW_USB_OTG0_IRQN,                 \
            .en                         = VSF_HW_EN_OTGFS1,                     \
            .phyclk                     = &VSF_HW_CLK_HICK,                     \
            .phyclk_freq_required       = 48 * 1000 * 1000,                     \
            /* vk_dwcotg_hw_info_t */                                           \
                .buffer_word_size       = 320,                                  \
                .speed                  = USB_SPEED_FULL,                       \
                .dma_en                 = false,                                \
                .ulpi_en                = false,                                \
                .utmi_en                = false,                                \
                .vbus_en                = false,
#endif

#if VSF_HW_USB_OTG_MASK & (1 << 1)
// TODO: 修复DMA模式BUG， 修复高速模式的BUG
#   define VSF_HW_USB_OTG1_IRQN         77      // OTGHS_IRQn
#   define VSF_HW_USB_OTG1_CONFIG                                               \
            .dc_ep_num                  = 8 << 1,                               \
            .hc_ep_num                  = 16,                                   \
            .reg                        = (void *)OTGHS_BASE,                   \
            .irq                        = VSF_HW_USB_OTG1_IRQN,                 \
            .en                         = VSF_HW_EN_OTGHS,                      \
            .phyclk                     = &VSF_HW_CLK_HEXT,                     \
            .phyclk_freq_required       = 12 * 1000 * 1000,                     \
            /* vk_dwcotg_hw_info_t */                                           \
                .buffer_word_size       = 1024,                                 \
                .speed                  = USB_SPEED_HIGH,                       \
                .dma_en                 = false,                                \
                .ulpi_en                = false,                                \
                .utmi_en                = false,                                \
                .vbus_en                = false,
#endif

/*============================ INCLUDES ======================================*/

// Include common irq and af headers after peripherals are defined, so that
//  irq and af can be adjusted according to the dedicated device configuration.

#include "./common.h"
#include "./device_irq.h"
#include "./device_af.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __HAL_DEVICE_COMMON_ARTERY_AT32F402_405_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__

/* EOF */
