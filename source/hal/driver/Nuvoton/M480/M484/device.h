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
//! arch info
#   define VSF_ARCH_PRI_NUM         16
#   define VSF_ARCH_PRI_BIT         4

// software interrupt provided by a dedicated device
#define VSF_DEV_SWI_NUM             9

#else

#ifndef __HAL_DEVICE_NUVOTON_M484_H__
#define __HAL_DEVICE_NUVOTON_M484_H__

// software interrupt provided by a dedicated device
#define VSF_DEV_SWI_LIST            5, 45, 50, 69, 81, 83, 91, 94, 95

/*============================ INCLUDES ======================================*/

/*\note this is should be the only place where __common.h is included.*/
#include "../common/__common.h"

/*============================ MACROS ========================================*/

// user configurations with default value
#ifndef M480_PLL_FREQ_HZ
#   define M480_PLL_FREQ_HZ         (384 * 1000 * 1000)
#endif

#ifndef M480_HCLK_FREQ_HZ
#   define M480_HCLK_FREQ_HZ        (192 * 1000 * 1000)
#endif

#ifndef M480_HXT_FREQ_HZ
#   define M480_HXT_FREQ_HZ         (12 * 1000 * 1000)
#endif

#ifndef M480_PCLK0_FREQ_HZ
#   define M480_PCLK0_FREQ_HZ       (96 * 1000 * 1000)
#endif

#ifndef M480_PCLK1_FREQ_HZ
#   define M480_PCLK1_FREQ_HZ       (96 * 1000 * 1000)
#endif

// bit0 - bit7  : __bit_offset  (0 .. 255)
// bit8 - bit12 : __bit_len     (0 .. 31)
// bit13        : __is_wprotect
#define M480_BIT_FIELD(__name, __bit_offset, __bit_len, __is_wprotect)          \
        __name = (__bit_offset) | ((__bit_len) << 8) | ((__is_wprotect) << 13)

#define M480_BIT_FIELD_GET_BITLEN(__bf) (((__bf) >> 8) & 0x1F)

#define VSF_HW_PM_COUNT             1

#define VSF_HAL_USE_GPIO            ENABLED
#define VSF_HW_GPIO_COUNT           4
#define VSF_HW_GPIO0_ADDRESS        GPIOA_BASE
#define VSF_HW_GPIO0_PIN_NUM        16

#define VSF_HW_GPIO1_ADDRESS        GPIOB_BASE
#define VSF_HW_GPIO1_PIN_NUM        16

#define VSF_HW_GPIO2_ADDRESS        GPIOC_BASE
#define VSF_HW_GPIO2_PIN_NUM        16

#define VSF_HW_GPIO3_ADDRESS        GPIOD_BASE
#define VSF_HW_GPIO3_PIN_NUM        16

/*
#define PIO_PORTE
#define PIO_PORTF
#define PIO_PORTG
#define PIO_PORTH
*/

#define VSF_HW_SPI_COUNT            0x02
#define VSF_HW_SPI0_DMA_TX_CHANNEL  0
#define VSF_HW_SPI0_DMA_RX_CHANNEL  1
#define VSF_HW_SPI1_DMA_TX_CHANNEL  2
#define VSF_HW_SPI1_DMA_RX_CHANNEL  3
#define VSF_HW_SPI2_DMA_TX_CHANNEL  4
#define VSF_HW_SPI2_DMA_RX_CHANNEL  5

#define VSF_HW_USB_HC_COUNT         1
#define VSF_HW_USB_HC_OHCI_COUNT    1

#define VSF_HW_USB_HC0_TYPE                ohci
#define VSF_HW_USB_HC0_IRQHandler          OHCI_IRQHandler
#define VSF_HW_USB_HC0_CONFIG                                                          \
    .reg                            = USBH,                                     \
    .sclk                           = SCLK_USBH_IDX,                            \
    .pclk                           = PCLK_USB_IDX,                             \
    .phy                            = M480_USBPHY_FS,                           \
    .irq                            = USBH_IRQn,                                \
                                                                                \
    .dp.pin_index                   = PA14,                                     \
    .dp.function                    = 14,                                       \
    .dm.pin_index                   = PA13,                                     \
    .dm.function                    = 14,

#define VSF_HW_USB_DC_COUNT         1
#define VSF_HW_USB_DC_HS_COUNT      1
#define VSF_HW_USB_DC_FS_COUNT      0


#define VSF_HW_USB_DC0_TYPE                usbd_hs
#define VSF_HW_USB_DC0_IRQHandler          USBD20_IRQHandler
#define VSF_HW_USB_DC0_EP_NUM              14
#define VSF_HW_USB_DC0_CONFIG                                                          \
    .reg                            = HSUSBD,                                   \
    .sclk                           = SCLK_USBD_IDX,                            \
    .phy                            = M480_USBPHY_HS,                           \
    .irq                            = USBD20_IRQn,


#define VSF_HW_USART_COUNT          5
/* RST */
#define VSF_HW_USART0_RST           ((4ul << 24) | SYS_IPRST1_UART0RST_Pos)
#define VSF_HW_USART1_RST           ((4ul << 24) | SYS_IPRST1_UART1RST_Pos)
#define VSF_HW_USART2_RST           ((4ul << 24) | SYS_IPRST1_UART2RST_Pos)
#define VSF_HW_USART3_RST           ((4ul << 24) | SYS_IPRST1_UART3RST_Pos)
#define VSF_HW_USART4_RST           ((4ul << 24) | SYS_IPRST1_UART4RST_Pos)
#define VSF_HW_USART5_RST           ((4ul << 24) | SYS_IPRST1_UART5RST_Pos)
/* Module */
#define VSF_HW_USART0_MODULE        ((1ul << 30) | (1ul << 28) | (0x3ul << 25) | (24ul << 20) | (0ul << 18) | (0xFul << 10) | (8ul << 5) |(16ul << 0))
#define VSF_HW_USART1_MODULE        ((1ul << 30) | (1ul << 28) | (0x3ul << 25) | (26ul << 20) | (0ul << 18) | (0xFul << 10) | (12ul << 5) |(17ul << 0))
#define VSF_HW_USART2_MODULE        ((1ul << 30) | (3ul << 28) | (0x3ul << 25) | (24ul << 20) | (3ul << 18) | (0xFul << 10) | (0ul << 5) |(18ul << 0))
#define VSF_HW_USART3_MODULE        ((1ul << 30) | (3ul << 28) | (0x3ul << 25) | (26ul << 20) | (3ul << 18) | (0xFul << 10) | (4ul << 5) |(19ul << 0))
#define VSF_HW_USART4_MODULE        ((1ul << 30) | (3ul << 28) | (0x3ul << 25) | (28ul << 20) | (3ul << 18) | (0xFul << 10) | (8ul << 5) |(20ul << 0))
#define VSF_HW_USART5_MODULE        ((1ul << 30) | (3ul << 28) | (0x3ul << 25) | (30ul << 20) | (3ul << 18) | (0xFul << 10) | (12ul << 5) |(21ul << 0))
/* SEL */
#define VSF_HW_USART0SEL_HXT        (0x0UL << CLK_CLKSEL1_UART0SEL_Pos)
#define VSF_HW_USART0SEL_LXT        (0x2UL << CLK_CLKSEL1_UART0SEL_Pos)
#define VSF_HW_USART0SEL_PLL        (0x1UL << CLK_CLKSEL1_UART0SEL_Pos)
#define VSF_HW_USART0SEL_HIRC       (0x3UL << CLK_CLKSEL1_UART0SEL_Pos)
#define VSF_HW_USART1SEL_HXT        (0x0UL << CLK_CLKSEL1_UART1SEL_Pos)
#define VSF_HW_USART1SEL_LXT        (0x2UL << CLK_CLKSEL1_UART1SEL_Pos)
#define VSF_HW_USART1SEL_PLL        (0x1UL << CLK_CLKSEL1_UART1SEL_Pos)
#define VSF_HW_USART1SEL_HIRC       (0x3UL << CLK_CLKSEL1_UART1SEL_Pos)
#define VSF_HW_USART2SEL_HXT        (0x0UL << CLK_CLKSEL3_UART2SEL_Pos)
#define VSF_HW_USART2SEL_LXT        (0x2UL << CLK_CLKSEL3_UART2SEL_Pos)
#define VSF_HW_USART2SEL_PLL        (0x1UL << CLK_CLKSEL3_UART2SEL_Pos)
#define VSF_HW_USART2SEL_HIRC       (0x3UL << CLK_CLKSEL3_UART2SEL_Pos)
#define VSF_HW_USART3SEL_HXT        (0x0UL << CLK_CLKSEL3_UART3SEL_Pos)
#define VSF_HW_USART3SEL_LXT        (0x2UL << CLK_CLKSEL3_UART3SEL_Pos)
#define VSF_HW_USART3SEL_PLL        (0x1UL << CLK_CLKSEL3_UART3SEL_Pos)
#define VSF_HW_USART3SEL_HIRC       (0x3UL << CLK_CLKSEL3_UART3SEL_Pos)
#define VSF_HW_USART4SEL_HXT        (0x0UL << CLK_CLKSEL3_UART4SEL_Pos)
#define VSF_HW_USART4SEL_LXT        (0x2UL << CLK_CLKSEL3_UART4SEL_Pos)
#define VSF_HW_USART4SEL_PLL        (0x1UL << CLK_CLKSEL3_UART4SEL_Pos)
#define VSF_HW_USART4SEL_HIRC       (0x3UL << CLK_CLKSEL3_UART4SEL_Pos)
#define VSF_HW_USART5SEL_HXT        (0x0UL << CLK_CLKSEL3_UART5SEL_Pos)
#define VSF_HW_USART5SEL_LXT        (0x2UL << CLK_CLKSEL3_UART5SEL_Pos)
#define VSF_HW_USART5SEL_PLL        (0x1UL << CLK_CLKSEL3_UART5SEL_Pos)
#define VSF_HW_USART5SEL_HIRC       (0x3UL << CLK_CLKSEL3_UART5SEL_Pos)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif      // __HAL_DEVICE_NUVOTON_M484_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
