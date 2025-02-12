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

/*\note first define basic info for arch. */
//! arch info
#   define VSF_ARCH_PRI_NUM                         64
#   define VSF_ARCH_PRI_BIT                         6

// aic8800 rtoa_al need this
#   define VSF_KERNEL_CFG_QUEUE_HAS_RX_NOTIFIED     ENABLED

// aic8800 sdk defined dma_addr_t to uint32_t
#   define VSF_ARCH_DMA_ADDR_T                      uint32_t
#elif defined(__VSF_HAL_SHOW_VENDOR_INFO__)

#   define __VSF_HEADER_ONLY_SHOW_VENDOR_INFO__
#   include "chip.h"

#else

#ifndef __HAL_DEVICE_AIC_AIC8800_H__
#define __HAL_DEVICE_AIC_AIC8800_H__

/*============================ INCLUDES ======================================*/

#include "common.h"

/*============================ MACROS ========================================*/

#define VSF_HW_FLASH_CFG_BASE_ADDRESS               0x08000000
#define VSF_HW_FLASH_CFG_ERASE_SECTORE_SIZE         0x1000
#define VSF_HW_FLASH_CFG_WRITE_SECTORE_SIZE         0x100
#define __ROM_APITBL_BASE                           ((unsigned int *)0x00000180UL)
#define __ROM_FlashChipSizeGet                                                  \
    ((unsigned int (*)(void))__ROM_APITBL_BASE[2])
#define __ROM_FlashErase                                                        \
    ((int (*)(unsigned int a4k, unsigned int len))__ROM_APITBL_BASE[4])
#define __ROM_FlashWrite                                                        \
    ((int (*)(unsigned int adr, unsigned int len, unsigned int buf))__ROM_APITBL_BASE[5])
#define __ROM_FlashRead                                                         \
    ((int (*)(unsigned int adr, unsigned int len, unsigned int buf))__ROM_APITBL_BASE[6])
#define __ROM_FlashCacheInvalidRange                                            \
    ((void (*)(unsigned int adr, unsigned int len))__ROM_APITBL_BASE[8])


#ifndef VSF_AIC8800_USB_CFG_SPEED
#   define VSF_AIC8800_USB_CFG_SPEED                USB_SPEED_HIGH
// AIC8800 has problem with usbh disconnect detecting level in high speed mode,
//  use port disable as a replacement
#   define VSF_DWCOTG_HCD_WORKAROUND_PORT_DISABLE_AS_DISCONNECT    ENABLED
#endif

#define USB_OTG_COUNT               1
// required by dwcotg, define the max ep number of dwcotg include ep0
#define USB_DWCOTG_MAX_EP_NUM       16

#define USB_OTG0_IRQHandler         USBDMA_IRQHandler
#define USB_OTG0_CONFIG                                                         \
            .dc_ep_num              = 4 << 1,                                   \
            .hc_ep_num              = 5,                                        \
            .reg                    = (void *)AIC_USB_BASE,                     \
            /* vk_dwcotg_hw_info_t */                                           \
                .buffer_word_size   = 948,                                      \
                .speed              = VSF_AIC8800_USB_CFG_SPEED,                \
                .dma_en             = true,                                     \
                .ulpi_en            = true,                                     \
                .utmi_en            = false,                                    \
                .vbus_en            = false,

                                                        //GPIOB2 ~ GPIOB15 default function is GPIO

#define VSF_HW_GPIO_PORT_COUNT          2
#define VSF_HW_GPIO_PIN_COUNT           16
#define VSF_HW_GPIO_EXTI_PORT_MASK      0x01    // only PORTA support external interrupt

#define VSF_HW_GPIO_FUNCTION_MAX        10 // All Function 10 ~ Function 15 is invalid
#define VSF_HW_GPIO0_BASE_ADDRESS       (0x40504000UL)
#define VSF_HW_GPIO0_IOMUX_REG_BASE     (0x40503000)
// GPIOA0 : SWCLK
// GPIOA1 : SWD
// GPIOA8 : UART0_RX
// GPIOA9 : UART0_TX
#define VSF_HW_GPIO0_MAP                {1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0}
#define VSF_HW_GPIO0_GPIO_PIN_MASK      (0x00000000)    // PORTA is not pmic port, only used to compile, ignore it's value
#define VSF_HW_GPIO0_IRQ_IDX            GPIO_IRQn
#define VSF_HW_GPIO0_IRQ                GPIO_IRQHandler
#define VSF_HW_GPIO0_IS_PMIC            false
#define VSF_HW_GPIO1_BASE_ADDRESS       (0x50011000UL)
#define VSF_HW_GPIO1_IOMUX_REG_BASE     (0x50012000)
// GPIOB0: PSI_SCL
// GPIOB1: PSI_SDA
#define VSF_HW_GPIO1_MAP                {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
#define VSF_HW_GPIO1_GPIO_PIN_MASK      (0x0000FFFC)    // GPIOB0 and GPIOB1 default function is not GPIO
#define VSF_HW_GPIO1_IRQ_IDX            0                   // TODO
#define VSF_HW_GPIO1_IRQ                GPIOB_IRQHandler
#define VSF_HW_GPIO1_IS_PMIC            true

#define VSF_HW_RNG_COUNT            1
#define VSF_HW_RNG_BITLEN           32
#define VSF_HW_RNG0_IRQHandler      TRNG_IRQHandler
#define VSF_HW_RNG0_CONFIG                                                      \
            .reg                    = (void *)AIC_TRNG_BASE,                    \
            .pclk                   = CSC_PCLKME_TRNG_EN_BIT,

// uart0 is already used in the library(include interrupt) and debug_uart
#define VSF_HW_USART_COUNT          2
#define VSF_HW_USART_MASK           ((1 << 1) | (1 << 2))
#define VSF_HW_USART1_REG           AIC_UART1_BASE
#define VSF_HW_USART2_REG           AIC_UART2_BASE

#define VSF_HW_I2C_COUNT            1
#define VSF_HW_I2C0_IRQ_IDX         I2CM_IRQn
#define VSF_HW_I2C0_IRQ             I2CM_IRQHandler
#define VSF_HW_I2C0_REG             (AIC_I2CM_TypeDef *)AIC_I2CM0_BASE
#define VSF_HW_I2C0_PCLKME_EN_BIT   CSC_PCLKME_I2CM_EN_BIT

#define VSF_HW_SPI_COUNT            1
#define VSF_HW_SPI0_IRQ_IDX         SPI_IRQn
#define VSF_HW_SPI0_IRQ             SPI0_IRQHandler
#define VSF_HW_SPI0_RXDMA_IRQ_IDX   DMA08_IRQn
#define VSF_HW_SPI0_TXDMA_IRQ_IDX   DMA09_IRQn
#define VSF_HW_SPI0_RXDMA_IRQ       DMA08_IRQHandler
#define VSF_HW_SPI0_TXDMA_IRQ       DMA09_IRQHandler
#define VSF_HW_SPI0_RXDMA_CH_IDX    DMA_CHANNEL_SPI0_RX
#define VSF_HW_SPI0_TXDMA_CH_IDX    DMA_CHANNEL_SPI0_TX
#define VSF_HW_SPI0_RXDMA_CID       REQ_CID_SPI_RX
#define VSF_HW_SPI0_TXDMA_CID       REQ_CID_SPI_TX
#define VSF_HW_SPI0_PCLKME_EN_BIT   CSC_PCLKME_SPI_EN_BIT
#define VSF_HW_SPI0_OCLKME_EN_BIT   CSC_OCLKME_SPI_EN_BIT
#define VSF_HW_SPI0_HCLKME_EN_BIT   CSC_HCLKME_DMA_EN_BIT

#define VSF_HW_SDIO_COUNT           1
#define VSF_HW_SDIO0_IRQ_IDX        SDMMC_IRQn
#define VSF_HW_SDIO0_IRQ            SDMMC_IRQHandler
#define VSF_HW_SDIO0_REG            (AIC_SDMMC_TypeDef *)AIC_SDMMC_BASE

#define VSF_HW_I2S_COUNT            2
#define VSF_HW_I2S0_REG             (HWP_AUD_PROC_T *)REG_AUD_PROC_BASE
#define VSF_HW_I2S0_IDX             0
#define VSF_HW_I2S0_OCLK            (CSC_OCLKME_AUDIO_PROC_EN_BIT | CSC_OCLKME_BCK0_EN_BIT)
#ifndef VSF_HW_I2S0_RXPATH
#   define VSF_HW_I2S0_RXPATH       AUD_PATH_RX01
#endif
#ifndef VSF_HW_I2S0_RXDMA_CH
#   define VSF_HW_I2S0_RXDMA_CH     DMA_CH_1
#   define VSF_HW_I2S0_RXDMA_IRQN   DMA01_IRQn
#endif
#ifndef VSF_HW_I2S0_TXPATH
#   define VSF_HW_I2S0_TXPATH       AUD_PATH_TX01
#endif
#ifndef VSF_HW_I2S0_TXDMA_CH
#   define VSF_HW_I2S0_TXDMA_CH     DMA_CH_4
#   define VSF_HW_I2S0_TXDMA_IRQN   DMA04_IRQn
#endif
#define VSF_HW_I2S1_REG             (HWP_AUD_PROC_T *)REG_AUD_PROC_BASE
#define VSF_HW_I2S1_IDX             1
#define VSF_HW_I2S1_OCLK            (CSC_OCLKME_AUDIO_PROC_EN_BIT | CSC_OCLKME_BCK1_EN_BIT)
#ifndef VSF_HW_I2S1_RXPATH
#   define VSF_HW_I2S1_RXPATH       AUD_PATH_RX01
#endif
#ifndef VSF_HW_I2S1_RXDMA_CH
#   define VSF_HW_I2S1_RXDMA_CH     DMA_CH_3
#   define VSF_HW_I2S1_RXDMA_IRQN   DMA03_IRQn
#endif
#ifndef VSF_HW_I2S1_TXPATH
#   define VSF_HW_I2S1_TXPATH       AUD_PATH_TX01
#endif
#ifndef VSF_HW_I2S1_TXDMA_CH
#   define VSF_HW_I2S1_TXDMA_CH     DMA_CH_2
#   define VSF_HW_I2S1_TXDMA_IRQN   DMA02_IRQn
#endif

#define VSF_HW_ADC_COUNT            1

#define VSF_HW_PWM_COUNT            1
#define VSF_HW_PWM0_BASE_ADDRESS    0x50014000
#define VSF_HW_PWM0_CHANNEL_COUNT   6
#define VSF_HW_PWM0_TIMER_COUNT     3

#define VSF_AIC_PWM_COUNT           3
#define VSF_AIC_PWM0_CHANNEL_COUNT  1
#define VSF_AIC_PWM0_CHANNEL_COUNT  1
#define VSF_AIC_PWM1_CHANNEL_COUNT  1
#define VSF_AIC_PWM2_CHANNEL_COUNT  1

#define VSF_HW_FLASH_COUNT          1
#define VSF_HW_RTC_COUNT            1

#define VSF_HW_TIMER_COUNT          6
#define VSF_HW_TIMER0_BASE_ADDRESS  (AIC_TIM0_BASE + 0x20 * 0)
#define VSF_HW_TIMER0_IRQN          TIMER00_IRQn
#define VSF_HW_TIMER0_OCLK_EN_BIT   CSC_OCLKME_TIMER00_EN_BIT
#define VSF_HW_TIMER0_PCLK_EN_BIT   CSC_PCLKME_TIMER0_EN_BIT
#define VSF_HW_TIMER0_IRQHandler    TIMER00_IRQHandler

#define VSF_HW_TIMER1_BASE_ADDRESS  (AIC_TIM0_BASE + 0x20 * 1)
#define VSF_HW_TIMER1_IRQN          TIMER01_IRQn
#define VSF_HW_TIMER1_OCLK_EN_BIT   CSC_OCLKME_TIMER01_EN_BIT
#define VSF_HW_TIMER1_PCLK_EN_BIT   CSC_PCLKME_TIMER0_EN_BIT
#define VSF_HW_TIMER1_IRQHandler    TIMER01_IRQHandler

#define VSF_HW_TIMER2_BASE_ADDRESS  (AIC_TIM0_BASE + 0x20 * 2)
#define VSF_HW_TIMER2_IRQN          TIMER02_IRQn
#define VSF_HW_TIMER2_OCLK_EN_BIT   CSC_OCLKME_TIMER02_EN_BIT
#define VSF_HW_TIMER2_PCLK_EN_BIT   CSC_PCLKME_TIMER0_EN_BIT
#define VSF_HW_TIMER2_IRQHandler    TIMER02_IRQHandler

#define VSF_HW_TIMER3_BASE_ADDRESS  (AIC_TIM1_BASE + 0x20 * 0)
#define VSF_HW_TIMER3_IRQN          TIMER10_IRQn
#define VSF_HW_TIMER3_OCLK_EN_BIT   CSC_OCLKME_TIMER10_EN_BIT
#define VSF_HW_TIMER3_PCLK_EN_BIT   CSC_PCLKME_TIMER1_EN_BIT
#define VSF_HW_TIMER3_IRQHandler    TIMER10_IRQHandler

#define VSF_HW_TIMER4_BASE_ADDRESS  (AIC_TIM1_BASE + 0x20 * 1)
#define VSF_HW_TIMER4_IRQN          TIMER11_IRQn
#define VSF_HW_TIMER4_OCLK_EN_BIT   CSC_OCLKME_TIMER11_EN_BIT
#define VSF_HW_TIMER4_PCLK_EN_BIT   CSC_PCLKME_TIMER1_EN_BIT
#define VSF_HW_TIMER4_IRQHandler    TIMER11_IRQHandler

#define VSF_HW_TIMER5_BASE_ADDRESS  (AIC_TIM1_BASE + 0x20 * 2)
#define VSF_HW_TIMER5_IRQN          TIMER12_IRQn
#define VSF_HW_TIMER5_OCLK_EN_BIT   CSC_OCLKME_TIMER12_EN_BIT
#define VSF_HW_TIMER5_PCLK_EN_BIT   CSC_PCLKME_TIMER1_EN_BIT
#define VSF_HW_TIMER5_IRQHandler    TIMER12_IRQHandler

#define VSF_HW_WDT_COUNT            1
#define VSF_HW_WDT0_BASE_ADDRESS    AIC_WDT0_BASE
#define VSF_HW_WDT0_IRQN            WDT0_IRQn
#define VSF_HW_WDT0_OCLK_EN_BIT     (CSC_OCLKME_RTC_WDG0_EN_BIT | CSC_OCLKMD_RTC_ALWAYS_DIS_BIT)
#define VSF_HW_WDT0_PCLK_EN_BIT     CSC_PCLKME_WDG0_EN_BIT
#define VSF_HW_WDT0_IRQHandler      WDT0_IRQHandler

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __HAL_DEVICE_AIC_AIC8800_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
