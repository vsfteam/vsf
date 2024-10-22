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

#ifndef __HAL_DRIVER_ST_STM32H7RSXX_COMMON_H__
#define __HAL_DRIVER_ST_STM32H7RSXX_COMMON_H__

/* \note __common.h should only be included by device.h */

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"
#include "hal/arch/vsf_arch.h"

/*============================ MACROS ========================================*/

#define VSF_HW_REG_REGION(__WORD_OFFSET, __BIT_OFFSET, __BIT_LENGTH)            \
            (((__WORD_OFFSET) << 16) | ((__BIT_LENGTH) << 8) | ((__BIT_OFFSET) << 0))

/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_hw_peripheral_clk_set       vsf_hw_clkrst_region_set
#define vsf_hw_peripheral_clk_get       vsf_hw_clkrst_region_get

#define vsf_hw_peripheral_rst_set       vsf_hw_clkrst_region_set_bit
#define vsf_hw_peripheral_rst_clear     vsf_hw_clkrst_region_clear_bit
#define vsf_hw_peripheral_rst_get       vsf_hw_clkrst_region_get_bit

#define vsf_hw_peripheral_enable        vsf_hw_clkrst_region_set_bit
#define vsf_hw_peripheral_disable       vsf_hw_clkrst_region_clear_bit



#define vsf_hw_power_enable             vsf_hw_power_region_set_bit
#define vsf_hw_power_disable            vsf_hw_power_region_clear_bit

/*============================ TYPES =========================================*/

typedef enum vsf_hw_peripheral_rst_t {
    // RCC.AHB1RSTR
    VSF_HW_RST_ADF1                     = VSF_HW_REG_REGION(0x20, 31, 1),   // ADF1RST
    VSF_HW_RST_OTGFS                    = VSF_HW_REG_REGION(0x20, 27, 1),   // OTGFSRST
    VSF_HW_RST_USBPHYC                  = VSF_HW_REG_REGION(0x20, 26, 1),   // USBPHYCRST
    VSF_HW_RST_OTGHS                    = VSF_HW_REG_REGION(0x20, 25, 1),   // OTGHSRST
    VSF_HW_RST_ETH1                     = VSF_HW_REG_REGION(0x20, 15, 1),   // ETH1RST
    VSF_HW_RST_ADC12                    = VSF_HW_REG_REGION(0x20, 5,  1),   // ADC12RST
    VSF_HW_RST_GPDMA                    = VSF_HW_REG_REGION(0x20, 4,  1),   // GPDMARST

    // RCC.AHB2RSTR
    VSF_HW_RST_CORDIC                   = VSF_HW_REG_REGION(0x21, 14, 1),   // CORDICRST
    VSF_HW_RST_SDMMC2                   = VSF_HW_REG_REGION(0x21, 9,  1),   // SDMMC2RST
    VSF_HW_RST_PSSI                     = VSF_HW_REG_REGION(0x21, 1,  1),   // PSSIRST

    // RCC.AHB3RSTR
    VSF_HW_RST_PKA                      = VSF_HW_REG_REGION(0x29, 6,  1),   // PKARST
    VSF_HW_RST_SAES                     = VSF_HW_REG_REGION(0x29, 4,  1),   // SAESRST
    VSF_HW_RST_CRYP                     = VSF_HW_REG_REGION(0x29, 2,  1),   // CRYPRST
    VSF_HW_RST_HASH                     = VSF_HW_REG_REGION(0x29, 1,  1),   // HASHRST
    VSF_HW_RST_RNG                      = VSF_HW_REG_REGION(0x29, 0,  1),   // RNGRST

    // RCC.AHB4RSTR
    VSF_HW_RST_CRC                      = VSF_HW_REG_REGION(0x22, 19, 1),   // CRCRST
    VSF_HW_RST_GPIOP                    = VSF_HW_REG_REGION(0x22, 15, 1),   // GPIOPRST
    VSF_HW_RST_GPIOO                    = VSF_HW_REG_REGION(0x22, 14, 1),   // GPIOORST
    VSF_HW_RST_GPION                    = VSF_HW_REG_REGION(0x22, 13, 1),   // GPIONRST
    VSF_HW_RST_GPIOM                    = VSF_HW_REG_REGION(0x22, 12, 1),   // GPIOMRST
    VSF_HW_RST_GPIOH                    = VSF_HW_REG_REGION(0x22, 7,  1),   // GPIOHRST
    VSF_HW_RST_GPIOG                    = VSF_HW_REG_REGION(0x22, 6,  1),   // GPIOGRST
    VSF_HW_RST_GPIOF                    = VSF_HW_REG_REGION(0x22, 5,  1),   // GPIOFRST
    VSF_HW_RST_GPIOE                    = VSF_HW_REG_REGION(0x22, 4,  1),   // GPIOERST
    VSF_HW_RST_GPIOD                    = VSF_HW_REG_REGION(0x22, 3,  1),   // GPIODRST
    VSF_HW_RST_GPIOC                    = VSF_HW_REG_REGION(0x22, 2,  1),   // GPIOCRST
    VSF_HW_RST_GPIOB                    = VSF_HW_REG_REGION(0x22, 1,  1),   // GPIOBRST
    VSF_HW_RST_GPIOA                    = VSF_HW_REG_REGION(0x22, 0,  1),   // GPIOARST

    // RCC.AHB5RSTR
    VSF_HW_RST_GPU2D                    = VSF_HW_REG_REGION(0x1F, 20, 1),   // GPU2DRST
    VSF_HW_RST_GFXMMU                   = VSF_HW_REG_REGION(0x1F, 19, 1),   // GFXMMURST
    VSF_HW_RST_XSPIM                    = VSF_HW_REG_REGION(0x1F, 14, 1),   // XSPIMRST
    VSF_HW_RST_XSPI2                    = VSF_HW_REG_REGION(0x1F, 12, 1),   // XSPI2RST
    VSF_HW_RST_SDMMC1                   = VSF_HW_REG_REGION(0x1F, 8,  1),   // SDMMC1RST
    VSF_HW_RST_XSPI1                    = VSF_HW_REG_REGION(0x1F, 5,  1),   // XSPI1RST
    VSF_HW_RST_FMC                      = VSF_HW_REG_REGION(0x1F, 4,  1),   // FMCRST
    VSF_HW_RST_JPEG                     = VSF_HW_REG_REGION(0x1F, 3,  1),   // JPEGRST
    VSF_HW_RST_DMA2D                    = VSF_HW_REG_REGION(0x1F, 1,  1),   // DMA2DRST
    VSF_HW_RST_HPDMA1                   = VSF_HW_REG_REGION(0x1F, 0,  1),   // HPDMA1RST

    // RCC.APB1RSTR1
    VSF_HW_RST_UART8                    = VSF_HW_REG_REGION(0x24, 31, 1),   // UART8RST
    VSF_HW_RST_UART7                    = VSF_HW_REG_REGION(0x24, 30, 1),   // UART7RST
    VSF_HW_RST_CEC                      = VSF_HW_REG_REGION(0x24, 27, 1),   // CECRST
    VSF_HW_RST_I2C3                     = VSF_HW_REG_REGION(0x24, 23, 1),   // I2C3RST
    VSF_HW_RST_I2C2                     = VSF_HW_REG_REGION(0x24, 22, 1),   // I2C2RST
    VSF_HW_RST_I2C1                     = VSF_HW_REG_REGION(0x24, 21, 1),   // I2C1_I3C1RST
    VSF_HW_RST_I3C1                     = VSF_HW_RST_I2C1,
    VSF_HW_RST_UART5                    = VSF_HW_REG_REGION(0x24, 20, 1),   // UART5RST
    VSF_HW_RST_UART4                    = VSF_HW_REG_REGION(0x24, 19, 1),   // UART4RST
    VSF_HW_RST_USART3                   = VSF_HW_REG_REGION(0x24, 18, 1),   // USART3RST
    VSF_HW_RST_USART2                   = VSF_HW_REG_REGION(0x24, 17, 1),   // USART2RST
    VSF_HW_RST_SPDIFRX                  = VSF_HW_REG_REGION(0x24, 16, 1),   // SPDIFRXRST
    VSF_HW_RST_SPI3                     = VSF_HW_REG_REGION(0x24, 15, 1),   // SPI3RST
    VSF_HW_RST_SPI2                     = VSF_HW_REG_REGION(0x24, 14, 1),   // SPI2RST
    VSF_HW_RST_LPTIM1                   = VSF_HW_REG_REGION(0x24, 9,  1),   // LPTIM1RST
    VSF_HW_RST_TIM14                    = VSF_HW_REG_REGION(0x24, 8,  1),   // TIM14RST
    VSF_HW_RST_TIM13                    = VSF_HW_REG_REGION(0x24, 7,  1),   // TIM13RST
    VSF_HW_RST_TIM12                    = VSF_HW_REG_REGION(0x24, 6,  1),   // TIM12RST
    VSF_HW_RST_TIM7                     = VSF_HW_REG_REGION(0x24, 5,  1),   // TIM7RST
    VSF_HW_RST_TIM6                     = VSF_HW_REG_REGION(0x24, 4,  1),   // TIM6RST
    VSF_HW_RST_TIM5                     = VSF_HW_REG_REGION(0x24, 3,  1),   // TIM5RST
    VSF_HW_RST_TIM4                     = VSF_HW_REG_REGION(0x24, 2,  1),   // TIM4RST
    VSF_HW_RST_TIM3                     = VSF_HW_REG_REGION(0x24, 1,  1),   // TIM3RST
    VSF_HW_RST_TIM2                     = VSF_HW_REG_REGION(0x24, 0,  1),   // TIM2RST

    // RCC.APB1RSTR2
    VSF_HW_RST_UCPD1                    = VSF_HW_REG_REGION(0x25, 27, 1),   // UCPD1RST
    VSF_HW_RST_FDCAN                    = VSF_HW_REG_REGION(0x25, 8,  1),   // FDCANRST
    VSF_HW_RST_MDIOS                    = VSF_HW_REG_REGION(0x25, 5,  1),   // MDIOSRST
    VSF_HW_RST_CRS                      = VSF_HW_REG_REGION(0x25, 1,  1),   // CRSRST

    // RCC.APB2RSTR
    VSF_HW_RST_SAI2                     = VSF_HW_REG_REGION(0x26, 23, 1),   // SAI2RST
    VSF_HW_RST_SAI1                     = VSF_HW_REG_REGION(0x26, 22, 1),   // SAI1RST
    VSF_HW_RST_SPI5                     = VSF_HW_REG_REGION(0x26, 20, 1),   // SPI5RST
    VSF_HW_RST_TIM9                     = VSF_HW_REG_REGION(0x26, 19, 1),   // TIM9RST
    VSF_HW_RST_TIM17                    = VSF_HW_REG_REGION(0x26, 18, 1),   // TIM17RST
    VSF_HW_RST_TIM16                    = VSF_HW_REG_REGION(0x26, 17, 1),   // TIM16RST
    VSF_HW_RST_TIM15                    = VSF_HW_REG_REGION(0x26, 16, 1),   // TIM15RST
    VSF_HW_RST_SPI4                     = VSF_HW_REG_REGION(0x26, 13, 1),   // SPI4RST
    VSF_HW_RST_SPI1                     = VSF_HW_REG_REGION(0x26, 12, 1),   // SPI1RST
    VSF_HW_RST_USART1                   = VSF_HW_REG_REGION(0x26, 4,  1),   // USART1RST
    VSF_HW_RST_TIM1                     = VSF_HW_REG_REGION(0x26, 0,  1),   // TIM1RST

    // RCC.APB4RSTR
    VSF_HW_RST_DTS                      = VSF_HW_REG_REGION(0x27, 26, 1),   // DTSRST
    VSF_HW_RST_VREF                     = VSF_HW_REG_REGION(0x27, 15, 1),   // VREFRST
    VSF_HW_RST_LPTIM5                   = VSF_HW_REG_REGION(0x27, 12, 1),   // LPTIM5RST
    VSF_HW_RST_LPTIM4                   = VSF_HW_REG_REGION(0x27, 11, 1),   // LPTIM4RST
    VSF_HW_RST_LPTIM3                   = VSF_HW_REG_REGION(0x27, 10, 1),   // LPTIM3RST
    VSF_HW_RST_LPTIM2                   = VSF_HW_REG_REGION(0x27, 9,  1),   // LPTIM2RST
    VSF_HW_RST_SPI6                     = VSF_HW_REG_REGION(0x27, 5,  1),   // SPI6RST
    VSF_HW_RST_LPUART1                  = VSF_HW_REG_REGION(0x27, 3,  1),   // LPUART1RST
    VSF_HW_RST_SBS                      = VSF_HW_REG_REGION(0x27, 1,  1),   // SBSRST

    // RCC.APB5RSTR
    VSF_HW_RST_GFXTIM                   = VSF_HW_REG_REGION(0x23, 4,  1),   // GFXTIMRST
    VSF_HW_RST_DCMIPP                   = VSF_HW_REG_REGION(0x23, 2,  1),   // DCMIPPRST
    VSF_HW_RST_LTDC                     = VSF_HW_REG_REGION(0x23, 1,  1),   // LTDCRST
} vsf_hw_peripheral_rst_t;

typedef enum vsf_hw_peripheral_en_t {
    // RCC.PHB1ENR
    VSF_HW_EN_ADF1                      = VSF_HW_REG_REGION(0x4E, 31, 1),   // ADF1EN
    VSF_HW_EN_OTGFS                     = VSF_HW_REG_REGION(0x4E, 27, 1),   // OTGFSEN
    VSF_HW_EN_USBPHYC                   = VSF_HW_REG_REGION(0x4E, 26, 1),   // USBPHYCEN
    VSF_HW_EN_OTGHS                     = VSF_HW_REG_REGION(0x4E, 26, 1),   // OTGHSEN
    VSF_HW_EN_ETH1RX                    = VSF_HW_REG_REGION(0x4E, 17, 1),   // ETH1RXEN
    VSF_HW_EN_ETH1TX                    = VSF_HW_REG_REGION(0x4E, 16, 1),   // ETH1TXEN
    VSF_HW_EN_ETH1MAC                   = VSF_HW_REG_REGION(0x4E, 15, 1),   // ETH1MACEN
    VSF_HW_EN_ADC12                     = VSF_HW_REG_REGION(0x4E, 5,  1),   // ADC12EN
    VSF_HW_EN_GPDMA                     = VSF_HW_REG_REGION(0x4E, 4,  1),   // GPDMAEN

    // RCC.PHB2ENR
    VSF_HW_EN_SRAM2                     = VSF_HW_REG_REGION(0x4F, 30, 1),   // SRAM2EN
    VSF_HW_EN_SRAM1                     = VSF_HW_REG_REGION(0x4F, 29, 1),   // SRAM1EN
    VSF_HW_EN_CORDIC                    = VSF_HW_REG_REGION(0x4F, 14, 1),   // CORDICEN
    VSF_HW_EN_SDMMC2                    = VSF_HW_REG_REGION(0x4F, 9,  1),   // SDMMC2EN
    VSF_HW_EN_PSSI                      = VSF_HW_REG_REGION(0x4F, 1,  1),   // PSSIEN

    // RCC.PHB3ENR
    VSF_HW_EN_PKA                       = VSF_HW_REG_REGION(0x56, 6,  1),   // PKAEN
    VSF_HW_EN_SAES                      = VSF_HW_REG_REGION(0x56, 4,  1),   // SAESEN
    VSF_HW_EN_CRYP                      = VSF_HW_REG_REGION(0x56, 2,  1),   // CRYPEN
    VSF_HW_EN_HASH                      = VSF_HW_REG_REGION(0x56, 1,  1),   // HASHEN
    VSF_HW_EN_RNG                       = VSF_HW_REG_REGION(0x56, 0,  1),   // RNGEN

    // RCC.PHB4ENR
    VSF_HW_EN_BKPRAM                    = VSF_HW_REG_REGION(0x50, 28, 1),   // BKPRAMEN
    VSF_HW_EN_CRC                       = VSF_HW_REG_REGION(0x50, 19, 1),   // CRCEN
    VSF_HW_EN_GPIOP                     = VSF_HW_REG_REGION(0x50, 15, 1),   // GPIOPEN
    VSF_HW_EN_GPIOO                     = VSF_HW_REG_REGION(0x50, 14, 1),   // GPIOOEN
    VSF_HW_EN_GPION                     = VSF_HW_REG_REGION(0x50, 13, 1),   // GPIONEN
    VSF_HW_EN_GPIOM                     = VSF_HW_REG_REGION(0x50, 12, 1),   // GPIOMEN
    VSF_HW_EN_GPIOH                     = VSF_HW_REG_REGION(0x50, 7,  1),   // GPIOHEN
    VSF_HW_EN_GPIOG                     = VSF_HW_REG_REGION(0x50, 6,  1),   // GPIOGEN
    VSF_HW_EN_GPIOF                     = VSF_HW_REG_REGION(0x50, 5,  1),   // GPIOFEN
    VSF_HW_EN_GPIOE                     = VSF_HW_REG_REGION(0x50, 4,  1),   // GPIOEEN
    VSF_HW_EN_GPIOD                     = VSF_HW_REG_REGION(0x50, 3,  1),   // GPIODEN
    VSF_HW_EN_GPIOC                     = VSF_HW_REG_REGION(0x50, 2,  1),   // GPIOCEN
    VSF_HW_EN_GPIOB                     = VSF_HW_REG_REGION(0x50, 1,  1),   // GPIOBEN
    VSF_HW_EN_GPIOA                     = VSF_HW_REG_REGION(0x50, 0,  1),   // GPIOAEN

    // RCC.PHB5ENR
    VSF_HW_EN_GPU2D                     = VSF_HW_REG_REGION(0x4D, 20, 1),   // GPU2DEN
    VSF_HW_EN_GFXMMU                    = VSF_HW_REG_REGION(0x4D, 19, 1),   // FGXMMUEN
    VSF_HW_EN_XSPIM                     = VSF_HW_REG_REGION(0x4D, 14, 1),   // XSPIMEN
    VSF_HW_EN_XSPI2                     = VSF_HW_REG_REGION(0x4D, 12, 1),   // XSPI2EN
    VSF_HW_EN_SDMMC1                    = VSF_HW_REG_REGION(0x4D, 8,  1),   // SDMMC1EN
    VSF_HW_EN_XSPI1                     = VSF_HW_REG_REGION(0x4D, 5,  1),   // XSPI1EN
    VSF_HW_EN_FMC                       = VSF_HW_REG_REGION(0x4D, 4,  1),   // FMCEN
    VSF_HW_EN_JPEG                      = VSF_HW_REG_REGION(0x4D, 3,  1),   // JPEGEN
    VSF_HW_EN_DMA2D                     = VSF_HW_REG_REGION(0x4D, 1,  1),   // DMA2DEN
    VSF_HW_EN_HPDMA1                    = VSF_HW_REG_REGION(0x4D, 0,  1),   // HPDMA1EN

    // RCC.APB1ENR1
    VSF_HW_EN_UART8                     = VSF_HW_REG_REGION(0x52, 31, 1),   // UART8EN
    VSF_HW_EN_UART7                     = VSF_HW_REG_REGION(0x52, 30, 1),   // UART7EN
    VSF_HW_EN_CEC                       = VSF_HW_REG_REGION(0x52, 27, 1),   // CECEN
    VSF_HW_EN_I2C3                      = VSF_HW_REG_REGION(0x52, 23, 1),   // I2C3EN
    VSF_HW_EN_I2C2                      = VSF_HW_REG_REGION(0x52, 22, 1),   // I2C2EN
    VSF_HW_EN_I2C1                      = VSF_HW_REG_REGION(0x52, 21, 1),   // I2C1_I2C1EN
    VSF_HW_EN_I3C1                      = VSF_HW_EN_I2C1,
    VSF_HW_EN_UART5                     = VSF_HW_REG_REGION(0x52, 20, 1),   // UART5EN
    VSF_HW_EN_UART4                     = VSF_HW_REG_REGION(0x52, 19, 1),   // UART4EN
    VSF_HW_EN_USART3                    = VSF_HW_REG_REGION(0x52, 18, 1),   // USART3EN
    VSF_HW_EN_USART2                    = VSF_HW_REG_REGION(0x52, 17, 1),   // USART2EN
    VSF_HW_EN_SPDIFRX                   = VSF_HW_REG_REGION(0x52, 16, 1),   // SPDIFRXEN
    VSF_HW_EN_SPI3                      = VSF_HW_REG_REGION(0x52, 15, 1),   // SPI3EN
    VSF_HW_EN_SPI2                      = VSF_HW_REG_REGION(0x52, 14, 1),   // SPI2EN
    VSF_HW_EN_WWDG                      = VSF_HW_REG_REGION(0x52, 11, 1),   // WWDGEN
    VSF_HW_EN_LPTIM1                    = VSF_HW_REG_REGION(0x52, 9,  1),   // LPTIM1EN
    VSF_HW_EN_TIM14                     = VSF_HW_REG_REGION(0x52, 8,  1),   // TIM14EN
    VSF_HW_EN_TIM13                     = VSF_HW_REG_REGION(0x52, 7,  1),   // TIM13EN
    VSF_HW_EN_TIM12                     = VSF_HW_REG_REGION(0x52, 6,  1),   // TIM12EN
    VSF_HW_EN_TIM7                      = VSF_HW_REG_REGION(0x52, 5,  1),   // TIM7EN
    VSF_HW_EN_TIM6                      = VSF_HW_REG_REGION(0x52, 4,  1),   // TIM6EN
    VSF_HW_EN_TIM5                      = VSF_HW_REG_REGION(0x52, 3,  1),   // TIM5EN
    VSF_HW_EN_TIM4                      = VSF_HW_REG_REGION(0x52, 2,  1),   // TIM4EN
    VSF_HW_EN_TIM3                      = VSF_HW_REG_REGION(0x52, 1,  1),   // TIM3EN
    VSF_HW_EN_TIM2                      = VSF_HW_REG_REGION(0x52, 0,  1),   // TIM2EN

    // RCC.APB1ENR2
    VSF_HW_EN_UCPD1                     = VSF_HW_REG_REGION(0x53, 27, 1),   // UCPD1EN
    VSF_HW_EN_FDCAN                     = VSF_HW_REG_REGION(0x53, 8,  1),   // FDCANEN
    VSF_HW_EN_MDIOS                     = VSF_HW_REG_REGION(0x53, 5,  1),   // MDIOSEN
    VSF_HW_EN_CRS                       = VSF_HW_REG_REGION(0x53, 1,  1),   // CRSEN

    // RCC.APB2ENR
    VSF_HW_EN_SAI2                      = VSF_HW_REG_REGION(0x54, 23, 1),   // SDI2EN
    VSF_HW_EN_SAI1                      = VSF_HW_REG_REGION(0x54, 22, 1),   // SDI1EN
    VSF_HW_EN_SPI5                      = VSF_HW_REG_REGION(0x54, 20, 1),   // SPI5EN
    VSF_HW_EN_TIM9                      = VSF_HW_REG_REGION(0x54, 19, 1),   // TIM9EN
    VSF_HW_EN_TIM17                     = VSF_HW_REG_REGION(0x54, 18, 1),   // TIM17EN
    VSF_HW_EN_TIM16                     = VSF_HW_REG_REGION(0x54, 17, 1),   // TIM16EN
    VSF_HW_EN_TIM15                     = VSF_HW_REG_REGION(0x54, 16, 1),   // TIM15EN
    VSF_HW_EN_SPI4                      = VSF_HW_REG_REGION(0x54, 13, 1),   // SPI4EN
    VSF_HW_EN_SPI1                      = VSF_HW_REG_REGION(0x54, 12, 1),   // SPI1EN
    VSF_HW_EN_USART1                    = VSF_HW_REG_REGION(0x54, 4,  1),   // USART1EN
    VSF_HW_EN_TIM1                      = VSF_HW_REG_REGION(0x54, 0,  1),   // TIM1EN

    // RCC.APB4ENR
    VSF_HW_EN_DTS                       = VSF_HW_REG_REGION(0x55, 26, 1),   // DTSEN
    VSF_HW_EN_RTCAPB                    = VSF_HW_REG_REGION(0x55, 16, 1),   // RTCA[BEN
    VSF_HW_EN_VREF                      = VSF_HW_REG_REGION(0x55, 15, 1),   // VREFEN
    VSF_HW_EN_LPTIM5                    = VSF_HW_REG_REGION(0x55, 12, 1),   // LPTIM5EN
    VSF_HW_EN_LPTIM4                    = VSF_HW_REG_REGION(0x55, 11, 1),   // LPTIM4EN
    VSF_HW_EN_LPTIM3                    = VSF_HW_REG_REGION(0x55, 10, 1),   // LPTIM3EN
    VSF_HW_EN_LPTIM2                    = VSF_HW_REG_REGION(0x55, 9,  1),   // LPTIM2EN
    VSF_HW_EN_SPI6                      = VSF_HW_REG_REGION(0x55, 5,  1),   // SPI6EN
    VSF_HW_EN_LPUART1                   = VSF_HW_REG_REGION(0x55, 3,  1),   // LPUART1EN
    VSF_HW_EN_SBS                       = VSF_HW_REG_REGION(0x55, 1,  1),   // SBSEN

    // RCC.APB5ENR
    VSF_HW_EN_GFXTIM                    = VSF_HW_REG_REGION(0x51, 4,  1),   // GFXTIMEN
    VSF_HW_EN_DCMIPP                    = VSF_HW_REG_REGION(0x51, 2,  1),   // DCMIPPEN
    VSF_HW_EN_LTDC                      = VSF_HW_REG_REGION(0x51, 1,  1),   // LTDCEN
} vsf_hw_peripheral_en_t;

typedef enum vsf_hw_power_en_t {
    // PWR.CSR2
    VSF_HW_PWR_EN_XSPIM1                = VSF_HW_REG_REGION(0x03, 14, 1),   // EN_XSPIM1
    VSF_HW_PWR_EN_XSPIM2                = VSF_HW_REG_REGION(0x03, 15, 1),   // EN_XSPIM2
    VSF_HW_PWR_EN_USB33_DET             = VSF_HW_REG_REGION(0x03, 24, 1),   // USB33DEN
    VSF_HW_PWR_EN_GPIOM                 = VSF_HW_PWR_EN_USB33_DET,      // GPIOM depends on USB33_DET
} vsf_hw_power_en_t;

typedef enum vsf_hw_power_mode_t {
    //                                            LDOEN      SDEN       BYPASS     SDHILEVEL/SMPSEXTHP
    VSF_HW_POWER_MODE_LDO_SUPPLY                = (1 << 1),
    VSF_HW_POWER_MODE_SMPS_SUPPLY               =            (1 << 2),
    VSF_HW_POWER_MODE_EXT_SUPPLY                =                       (1 << 0),
    VSF_HW_POWER_MODE_SMPS_SUPPLY_LDO           = (1 << 1) | (1 << 2)            | (2 << 3),
    VSF_HW_POWER_MODE_SMPS_SUPPLY_EXT_AND_LDO   = (1 << 1) | (1 << 2)            | (3 << 3),
    VSF_HW_POWER_MODE_SMPS_SUPPLY_EXT           =            (1 << 2) | (1 << 0) | (3 << 3),
    VSF_HW_POWER_MODE_BYPASS                    =                       (1 << 0),
} vsf_hw_power_mode_t;

typedef enum vsf_hw_hslv_t {
    VSF_HW_HSLV_IO                      = (1 << 16),
    VSF_HW_HSLV_XSPI1                   = (1 << 17),
    VSF_HW_HSLV_XSPI2                   = (1 << 18),
} vsf_hw_hslv_t;

typedef struct vsf_hw_clk_t vsf_hw_clk_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_hw_clk_t VSF_HW_CLK_HSI;
extern const vsf_hw_clk_t VSF_HW_CLK_HSI_CAL;
extern const vsf_hw_clk_t VSF_HW_CLK_HSI_KER;
extern const vsf_hw_clk_t VSF_HW_CLK_UCPD_KER;
extern const vsf_hw_clk_t VSF_HW_CLK_HSI48_KER;
extern const vsf_hw_clk_t VSF_HW_CLK_LSI;
extern const vsf_hw_clk_t VSF_HW_CLK_CSI;
extern const vsf_hw_clk_t VSF_HW_CLK_CSI_KER;
extern const vsf_hw_clk_t VSF_HW_CLK_CSI_CAL;
extern const vsf_hw_clk_t VSF_HW_CLK_HSE;
#define VSF_HW_CLK_HSE_KER              VSF_HW_CLK_HSE
extern const vsf_hw_clk_t VSF_HW_CLK_LSE;

extern const vsf_hw_clk_t VSF_HW_CLK_PLL_SRC;
extern const vsf_hw_clk_t VSF_HW_CLK_PLL1_VCO;
extern const vsf_hw_clk_t VSF_HW_CLK_PLL2_VCO;
extern const vsf_hw_clk_t VSF_HW_CLK_PLL3_VCO;
extern const vsf_hw_clk_t VSF_HW_CLK_PLL1_P;
extern const vsf_hw_clk_t VSF_HW_CLK_PLL1_Q;
extern const vsf_hw_clk_t VSF_HW_CLK_PLL1_S;
extern const vsf_hw_clk_t VSF_HW_CLK_PLL2_P;
extern const vsf_hw_clk_t VSF_HW_CLK_PLL2_Q;
extern const vsf_hw_clk_t VSF_HW_CLK_PLL2_R;
extern const vsf_hw_clk_t VSF_HW_CLK_PLL2_S;
extern const vsf_hw_clk_t VSF_HW_CLK_PLL2_T;
extern const vsf_hw_clk_t VSF_HW_CLK_PLL3_P;
extern const vsf_hw_clk_t VSF_HW_CLK_PLL3_Q;
extern const vsf_hw_clk_t VSF_HW_CLK_PLL3_R;
extern const vsf_hw_clk_t VSF_HW_CLK_PLL3_S;

extern const vsf_hw_clk_t VSF_HW_CLK_SYS;
extern const vsf_hw_clk_t VSF_HW_CLK_SYS_CPU;
extern const vsf_hw_clk_t VSF_HW_CLK_SYS_BUS;
#define VSF_HW_CLK_AXI                  VSF_HW_CLK_SYS_BUS
#define VSF_HW_CLK_HCLK                 VSF_HW_CLK_SYS_BUS
#define VSF_HW_CLK_HCLK1                VSF_HW_CLK_HCLK
#define VSF_HW_CLK_HCLK2                VSF_HW_CLK_HCLK
#define VSF_HW_CLK_HCLK3                VSF_HW_CLK_HCLK
#define VSF_HW_CLK_HCLK4                VSF_HW_CLK_HCLK
#define VSF_HW_CLK_HCLK5                VSF_HW_CLK_HCLK
#define VSF_HW_CLK_AHB1                 VSF_HW_CLK_HCLK1
#define VSF_HW_CLK_AHB2                 VSF_HW_CLK_HCLK2
#define VSF_HW_CLK_AHB3                 VSF_HW_CLK_HCLK3
#define VSF_HW_CLK_AHB4                 VSF_HW_CLK_HCLK4
#define VSF_HW_CLK_AHB5                 VSF_HW_CLK_HCLK5
extern const vsf_hw_clk_t VSF_HW_CLK_PCLK1;
extern const vsf_hw_clk_t VSF_HW_CLK_PCLK2;
extern const vsf_hw_clk_t VSF_HW_CLK_PCLK4;
extern const vsf_hw_clk_t VSF_HW_CLK_PCLK5;
#define VSF_HW_CLK_APB1                 VSF_HW_CLK_PCLK1
#define VSF_HW_CLK_APB2                 VSF_HW_CLK_PCLK2
#define VSF_HW_CLK_APB4                 VSF_HW_CLK_PCLK4
#define VSF_HW_CLK_APB5                 VSF_HW_CLK_PCLK5
extern const vsf_hw_clk_t VSF_HW_CLK_PER;

extern const vsf_hw_clk_t VSF_HW_CLK_XSPI1;
extern const vsf_hw_clk_t VSF_HW_CLK_XSPI2;

#if VSF_HAL_USE_USART == ENABLED
extern const vsf_hw_clk_t VSF_HW_CLK_USART1;
extern const vsf_hw_clk_t VSF_HW_CLK_USART234578;
#define VSF_HW_CLK_USART1               VSF_HW_CLK_USART1
#define VSF_HW_CLK_USART2               VSF_HW_CLK_USART234578
#define VSF_HW_CLK_USART3               VSF_HW_CLK_USART234578
#define VSF_HW_CLK_UART4                VSF_HW_CLK_USART234578
#define VSF_HW_CLK_UART5                VSF_HW_CLK_USART234578
#define VSF_HW_CLK_UART7                VSF_HW_CLK_USART234578
#define VSF_HW_CLK_UART8                VSF_HW_CLK_USART234578
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern const vsf_hw_clk_t * vsf_hw_clk_get_src(const vsf_hw_clk_t *clk);
extern uint32_t vsf_hw_clk_get_freq_hz(const vsf_hw_clk_t *clk);

extern void vsf_hw_clkrst_region_set(uint32_t region, uint_fast8_t value);
extern uint_fast8_t vsf_hw_clkrst_region_get(uint32_t region);

extern void vsf_hw_clkrst_region_set_bit(uint32_t region);
extern void vsf_hw_clkrst_region_clear_bit(uint32_t region);
extern uint_fast8_t vsf_hw_clkrst_region_get_bit(uint32_t region);

extern void vsf_hw_clk_enable(const vsf_hw_clk_t *clk);
extern void vsf_hw_clk_disable(const vsf_hw_clk_t *clk);
extern vsf_err_t vsf_hw_clk_config(const vsf_hw_clk_t *clk, const vsf_hw_clk_t *clksrc, uint16_t prescaler, uint32_t freq_hz);
// vsf_hw_clk_config_secure will tweak flash latency
extern vsf_err_t vsf_hw_clk_config_secure(const vsf_hw_clk_t *clk, const vsf_hw_clk_t *clksrc, uint16_t prescaler, uint32_t freq_hz);

/**
 \~english
 @brief configure VCO clock pll output
 @param[in] clk: a pointer to PLL_VCO clock
                VSF_HW_CLK_PLL0_VCO
                VSF_HW_CLK_PLL1_VCO
                VSF_HW_CLK_PLL2_VCO
 @param[in] src_prescaler: prescaler of clock src, [1 .. 63]
 @param[in] vco_freq_hz: VCO frequency in Hz

 \~chinese
 @brief 配置 PLL 输出的 VCO 时钟
 @param[in] clk: ָ指向 PLL_VCO 的指针
                VSF_HW_CLK_PLL0_VCO
                VSF_HW_CLK_PLL1_VCO
                VSF_HW_CLK_PLL2_VCO
 @param[in] src_prescaler: PLL 输入时钟分频系数，范围： 1 到 63
 @param[in] vco_freq_hz: PLL 输出的 VCO 频率，单位： Hz
 */
extern vsf_err_t vsf_hw_pll_vco_config(const vsf_hw_clk_t *clk, uint_fast8_t src_prescaler, uint32_t vco_freq_hz);

extern void vsf_hw_power_region_set_bit(uint32_t region);
extern void vsf_hw_power_region_clear_bit(uint32_t region);
extern vsf_err_t vsf_hw_power_config_supply(vsf_hw_power_mode_t mode);

/**
 \~english
 @brief configure Vcore mode
 @param[in] mode: VCore mode
                0: low power mode
                1: performance mode

 \~chinese
 @brief 配置 Vcore 模式
 @param[in] mode: Vcore 模式
                0: 低功耗模式
                1: 性能模式
 */
extern void vsf_hw_power_vcore_config(uint_fast8_t mode);

/**
 \~english
 @brief Enable High Speed Low Voltage (HSLV) mode
 @param[in] hslv:

 \~chinese
 @brief 使能 高速低压 (HSLV) 模式
 @param[in] hslv:
 */
extern void vsf_hw_hslv_enable(vsf_hw_hslv_t hslv);

#endif
/* EOF */
