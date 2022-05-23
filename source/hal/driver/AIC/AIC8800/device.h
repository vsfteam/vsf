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
#else

#ifndef __HAL_DEVICE_AIC_AIC8800_H__
#define __HAL_DEVICE_AIC_AIC8800_H__

/*============================ INCLUDES ======================================*/

#include "common.h"

/*============================ MACROS ========================================*/

#ifndef VSF_AIC8800_USB_CFG_SPEED
#   define VSF_AIC8800_USB_CFG_SPEED                USB_SPEED_HIGH
#endif

#define USB_OTG_COUNT               1
// required by dwcotg, define the max ep number of dwcotg include ep0
#define USB_DWCOTG_MAX_EP_NUM       16

// AIC8800 has problem with usbh disconnect detecting level, use port disable as a replacement
#define VSF_DWCOTG_HCD_WORKAROUND_PORT_DISABLE_AS_DISCONNECT    ENABLED

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

#define VSF_HW_GPIO_COUNT           2
#define VSF_HW_GPIO_PIN_MAX         16
#define VSF_HW_GPIO0_IOMUX_REG_BASE (0x40503000)
#define VSF_HW_GPIO0_IS_PMIC        false
#define VSF_HW_GPIO0_PIN_SEL        {                                           \
        1, 1, 0, 0, 0, 0, 0, 0,                                                 \
        1, 1, 0, 0, 0, 0, 0, 0,                                                 \
    }
#define VSF_HW_GPIO1_IOMUX_REG_BASE (0x50012000)
#define VSF_HW_GPIO1_IS_PMIC        true
#define VSF_HW_GPIO1_PIN_SEL        {                                           \
        1, 1, 0, 0, 0, 0, 0, 0,                                                 \
        0, 0, 0, 0, 0, 0, 0, 0,                                                 \
    }

#define VSF_HW_RNG_COUNT                   1
#define VSF_HW_RNG_BITLEN                  32
#define VSF_HW_RNG0_IRQHandler             TRNG_IRQHandler
#define VSF_HW_RNG0_CONFIG                                                      \
            .reg                    = (void *)AIC_TRNG_BASE,                    \
            .pclk                   = CSC_PCLKME_TRNG_EN_BIT,

// uart0 is already used in the library(include interrupt) and debug_uart
#define VSF_HW_USART_COUNT          2
#define VSF_HW_USART_MASK           ((1 << 1) | (1 << 2))

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

#define VSF_HW_ADC_COUNT            1

#define VSF_HW_PWM_COUNT            1
#define VSF_HW_PWM0_BASE_ADDRESS    0x50014000
#define VSF_HW_PWM0_CHANNAL_COUNT   6
#define VSF_HW_PWM0_TIMER_COUNT     3

#define VSF_AIC_PWM_COUNT           3
#define VSF_AIC_PWM0_CHANNAL_COUNT  1
#define VSF_AIC_PWM0_CHANNAL_COUNT  1
#define VSF_AIC_PWM1_CHANNAL_COUNT  1
#define VSF_AIC_PWM2_CHANNAL_COUNT  1

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

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __HAL_DEVICE_AIC_AIC8800_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
