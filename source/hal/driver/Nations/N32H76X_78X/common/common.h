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

#ifndef __HAL_DRIVER_NATIONS_N32H76X_N32H78X_COMMON_H__
#define __HAL_DRIVER_NATIONS_N32H76X_N32H78X_COMMON_H__

/* \note common.h should only be included by device.h */

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"
#include "hal/arch/vsf_arch.h"

/*============================ MACROS ========================================*/

// CLK & RST REGION

#define VSF_HW_REG_REGION(__WORD_OFFSET, __BIT_OFFSET, __BIT_LENGTH)            \
            (((__WORD_OFFSET) << 16) | ((__BIT_LENGTH) << 8) | ((__BIT_OFFSET) << 0))
#define VSF_HW_CLKRST_REGION            VSF_HW_REG_REGION

/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_hw_peripheral_clk_set       vsf_hw_clkrst_region_set
#define vsf_hw_peripheral_clk_get       vsf_hw_clkrst_region_get

#define vsf_hw_peripheral_rst_set       vsf_hw_clkrst_region_set_bit
#define vsf_hw_peripheral_rst_clear     vsf_hw_clkrst_region_clear_bit
#define vsf_hw_peripheral_rst_get       vsf_hw_clkrst_region_get_bit

#define vsf_hw_peripheral_enable        vsf_hw_clkrst_region_set_bit
#define vsf_hw_peripheral_disable       vsf_hw_clkrst_region_clear_bit

#define VSF_SYSTIMER_FREQ               vsf_hw_clk_get_freq_hz(&VSF_HW_CLK_SYSTICK)

/*============================ TYPES =========================================*/

typedef enum vsf_hw_peripheral_rst_t {
    // AXI
    // RCC.AXIRST1
    VSF_HW_RST_JPEGD                    = VSF_HW_CLKRST_REGION(0x54, 28, 1),// JPEGDRST
    VSF_HW_RST_JPEGE                    = VSF_HW_CLKRST_REGION(0x54, 20, 1),// JPEGERST
    VSF_HW_RST_DMAMUX2                  = VSF_HW_CLKRST_REGION(0x54, 16, 1),// DMAMUX2RST
    VSF_HW_RST_MDMA                     = VSF_HW_CLKRST_REGION(0x54, 12, 1),// MDMARST
    VSF_HW_RST_SDMMC1                   = VSF_HW_CLKRST_REGION(0x54, 9,  1),// SDMMC1RST
    VSF_HW_RST_SDHOST1                  = VSF_HW_CLKRST_REGION(0x54, 8,  1),// SDHOST1RST
    VSF_HW_RST_ECCM1                    = VSF_HW_CLKRST_REGION(0x54, 4,  1),// ECCM1RST
    VSF_HW_RST_OTPC                     = VSF_HW_CLKRST_REGION(0x54, 0,  1),// OTPCRST
    // RCC.AXIRST2
    VSF_HW_RST_DSICFG                   = VSF_HW_CLKRST_REGION(0x55, 29, 1),// DSICFGRST
    VSF_HW_RST_DSI                      = VSF_HW_CLKRST_REGION(0x55, 28, 1),// DSIRST
    VSF_HW_RST_LCD                      = VSF_HW_CLKRST_REGION(0x55, 24, 1),// LCDRST
    VSF_HW_RST_DVP1                     = VSF_HW_CLKRST_REGION(0x55, 16, 1),// DVP1RST
    VSF_HW_RST_DVP2                     = VSF_HW_CLKRST_REGION(0x55, 8,  1),// DVP2RST
    VSF_HW_RST_WWDG1                    = VSF_HW_CLKRST_REGION(0x55, 0,  1),// WWDG1RST
    // RCC.AXIRST3
    VSF_HW_RST_GPU                      = VSF_HW_CLKRST_REGION(0x56, 0,  1),// GPURST
    // RCC.AXIRST4
    VSF_HW_RST_XSPI1                    = VSF_HW_CLKRST_REGION(0x57, 28, 1),// XSPI1RST
    VSF_HW_RST_XSPI2                    = VSF_HW_CLKRST_REGION(0x57, 24, 1),// XSPI2RST
    VSF_HW_RST_FEMCCFG                  = VSF_HW_CLKRST_REGION(0x57, 21, 1),// FEMCCFGRST
    VSF_HW_RST_FEMC                     = VSF_HW_CLKRST_REGION(0x57, 20, 1),// FEMCRST
    VSF_HW_RST_SDRAM                    = VSF_HW_CLKRST_REGION(0x57, 16, 1),// SDRAMRST

    // AHB1
    //RCC.AHB1RST1
    VSF_HW_RST_SDMMC2                   = VSF_HW_CLKRST_REGION(0x19, 29, 1),// SDMMC2RST
    VSF_HW_RST_SDHOST2                  = VSF_HW_CLKRST_REGION(0x19, 28, 1),// SDHOST2RST
    VSF_HW_RST_USB2WRAP                 = VSF_HW_CLKRST_REGION(0x19, 22, 1),// USB2WRAPRST
    VSF_HW_RST_USB2POR                  = VSF_HW_CLKRST_REGION(0x19, 21, 1),// USB2PORRST
    VSF_HW_RST_USB2                     = VSF_HW_CLKRST_REGION(0x19, 20, 1),// USB2RST
    VSF_HW_RST_DMAMUX1                  = VSF_HW_CLKRST_REGION(0x19, 16, 1),// DMAMUX1RST
    VSF_HW_RST_ADC1                     = VSF_HW_CLKRST_REGION(0x19, 0,  1),// ADC1RST
    //RCC.AHB1RST2
    VSF_HW_RST_ETH2                     = VSF_HW_CLKRST_REGION(0x1A, 0,  1),// ETH2RST
    //RCC.AHB1RST3
    VSF_HW_RST_ECCMAC                   = VSF_HW_CLKRST_REGION(0x1B, 24, 1),// ECCMACRST
    VSF_HW_RST_DMA1                     = VSF_HW_CLKRST_REGION(0x1B, 16, 1),// DMA1RST
    VSF_HW_RST_DMA2                     = VSF_HW_CLKRST_REGION(0x1B, 8,  1),// DMA2RST
    VSF_HW_RST_DMA3                     = VSF_HW_CLKRST_REGION(0x1B, 0,  1),// DMA3RST
    //RCC.AHB1RST4
    VSF_HW_RST_ADC2                     = VSF_HW_CLKRST_REGION(0x1C, 16, 1),// ADC2RST
    VSF_HW_RST_ADC3                     = VSF_HW_CLKRST_REGION(0x1C, 0,  1),// ADC3RST

    // AHB2
    // RCC.AHB2RST1
    VSF_HW_RST_DAC56                    = VSF_HW_CLKRST_REGION(0x2D, 25, 1),// DAC56RST
    VSF_HW_RST_DAC34                    = VSF_HW_CLKRST_REGION(0x2D, 24, 1),// DAC34RST
    VSF_HW_RST_USB1WRAP                 = VSF_HW_CLKRST_REGION(0x2D, 22, 1),// USB1WRAPRST
    VSF_HW_RST_USB1POR                  = VSF_HW_CLKRST_REGION(0x2D, 21, 1),// USB1PORRST
    VSF_HW_RST_USB1                     = VSF_HW_CLKRST_REGION(0x2D, 20, 1),// USB1RST
    VSF_HW_RST_ETH1                     = VSF_HW_CLKRST_REGION(0x2D, 16, 1),// ETH1RST
    VSF_HW_RST_ECCM2                    = VSF_HW_CLKRST_REGION(0x2D, 12, 1),// ECCM2RST
    VSF_HW_RST_CORDIC                   = VSF_HW_CLKRST_REGION(0x2D, 8,  1),// CORDICRST
    VSF_HW_RST_SDPU                     = VSF_HW_CLKRST_REGION(0x2D, 4,  1),// SDPURST
    VSF_HW_RST_FMAC                     = VSF_HW_CLKRST_REGION(0x2D, 0,  1),// FMACRST

    // AHB5
    // RCC.AHB5RST1
    VSF_HW_RST_GPIOA                    = VSF_HW_CLKRST_REGION(0x3B, 28, 1),// GPIOARST
    VSF_HW_RST_GPIOB                    = VSF_HW_CLKRST_REGION(0x3B, 24, 1),// GPIOBRST
    VSF_HW_RST_GPIOC                    = VSF_HW_CLKRST_REGION(0x3B, 20, 1),// GPIOCRST
    VSF_HW_RST_GPIOD                    = VSF_HW_CLKRST_REGION(0x3B, 16, 1),// GPIODRST
    VSF_HW_RST_GPIOE                    = VSF_HW_CLKRST_REGION(0x3B, 12, 1),// GPIOERST
    VSF_HW_RST_GPIOF                    = VSF_HW_CLKRST_REGION(0x3B, 8,  1),// GPIOFRST
    VSF_HW_RST_GPIOG                    = VSF_HW_CLKRST_REGION(0x3B, 4,  1),// GPIOGRST
    VSF_HW_RST_GPIOH                    = VSF_HW_CLKRST_REGION(0x3B, 0,  1),// GPIOHRST
    // RCC.AHB5RST2
    VSF_HW_RST_GPIOI                    = VSF_HW_CLKRST_REGION(0x3C, 28, 1),// GPIOIRST
    VSF_HW_RST_GPIOJ                    = VSF_HW_CLKRST_REGION(0x3C, 24, 1),// GPIOJRST
    VSF_HW_RST_GPIOK                    = VSF_HW_CLKRST_REGION(0x3C, 20, 1),// GPIOKRST
    VSF_HW_RST_ECCM3                    = VSF_HW_CLKRST_REGION(0x3C, 16, 1),// ECCM3RST
    VSF_HW_RST_PWR                      = VSF_HW_CLKRST_REGION(0x3C, 12, 1),// PWRRST
    VSF_HW_RST_CRC                      = VSF_HW_CLKRST_REGION(0x3C, 8,  1),// CRCRST
    VSF_HW_RST_SEMA4                    = VSF_HW_CLKRST_REGION(0x3C, 4,  1),// SEMA4RST
    VSF_HW_RST_AFIO                     = VSF_HW_CLKRST_REGION(0x3C, 0,  1),// AFIORST

    // AHB9
    // RCC.AHB9RST1
    VSF_HW_RST_ESC                      = VSF_HW_CLKRST_REGION(0x0C, 0,  1),// ESCRST

    // APB1
    // RCC.APB1RST1
    VSF_HW_RST_BTIM1                    = VSF_HW_CLKRST_REGION(0x25, 28, 1),// BTIM1RST
    VSF_HW_RST_BTIM2                    = VSF_HW_CLKRST_REGION(0x25, 24, 1),// BTIM2RST
    VSF_HW_RST_BTIM3                    = VSF_HW_CLKRST_REGION(0x25, 20, 1),// BTIM3RST
    VSF_HW_RST_BTIM4                    = VSF_HW_CLKRST_REGION(0x25, 16, 1),// BTIM4RST
    VSF_HW_RST_GTIMB1                   = VSF_HW_CLKRST_REGION(0x25, 12, 1),// GTIMB1RST
    VSF_HW_RST_GTIMB2                   = VSF_HW_CLKRST_REGION(0x25, 8,  1),// GTIMB2RST
    VSF_HW_RST_GTIMB3                   = VSF_HW_CLKRST_REGION(0x25, 4,  1),// GTIMB3RST
    VSF_HW_RST_GTIMA4                   = VSF_HW_CLKRST_REGION(0x25, 0,  1),// GTIMA4RST
    // RCC.APB1RST2
    VSF_HW_RST_GTIMA5                   = VSF_HW_CLKRST_REGION(0x26, 28, 1),// GTIMA5RST
    VSF_HW_RST_GTIMA6                   = VSF_HW_CLKRST_REGION(0x26, 24, 1),// GTIMA6RST
    VSF_HW_RST_GTIMA7                   = VSF_HW_CLKRST_REGION(0x26, 20, 1),// GTIMA7RST
    VSF_HW_RST_SPI3                     = VSF_HW_CLKRST_REGION(0x26, 16, 1),// SPI3RST
    VSF_HW_RST_DAC12                    = VSF_HW_CLKRST_REGION(0x26, 12, 1),// DAC12RST
    VSF_HW_RST_WWDG2                    = VSF_HW_CLKRST_REGION(0x26, 4,  1),// WWDG2RST
    // RCC.APB1RST3
    VSF_HW_RST_USART1                   = VSF_HW_CLKRST_REGION(0x27, 28, 1),// USART1RST
    VSF_HW_RST_USART2                   = VSF_HW_CLKRST_REGION(0x27, 24, 1),// USART2RST
    VSF_HW_RST_USART3                   = VSF_HW_CLKRST_REGION(0x27, 20, 1),// USART3RST
    VSF_HW_RST_USART4                   = VSF_HW_CLKRST_REGION(0x27, 16, 1),// USART4RST
    VSF_HW_RST_UART9                    = VSF_HW_CLKRST_REGION(0x27, 12, 1),// UART9RST
    VSF_HW_RST_UART10                   = VSF_HW_CLKRST_REGION(0x27, 8,  1),// UART10RST
    VSF_HW_RST_UART11                   = VSF_HW_CLKRST_REGION(0x27, 4,  1),// UART11RST
    VSF_HW_RST_UART12                   = VSF_HW_CLKRST_REGION(0x27, 0,  1),// UART12RST
    // RCC.APB1RST4
    VSF_HW_RST_I2S3                     = VSF_HW_CLKRST_REGION(0x28, 28, 1),// I2S3RST
    VSF_HW_RST_I2S4                     = VSF_HW_CLKRST_REGION(0x28, 24, 1),// I2S4RST
    VSF_HW_RST_I2C1                     = VSF_HW_CLKRST_REGION(0x28, 20, 1),// I2C1RST
    VSF_HW_RST_I2C2                     = VSF_HW_CLKRST_REGION(0x28, 16, 1),// I2C2RST
    VSF_HW_RST_I2C3                     = VSF_HW_CLKRST_REGION(0x28, 12, 1),// I2C3RST
    // RCC.APB1RST5
    VSF_HW_RST_FDCAN1                   = VSF_HW_CLKRST_REGION(0x29, 28, 1),// FDCAN1RST
    VSF_HW_RST_FDCAN2                   = VSF_HW_CLKRST_REGION(0x29, 24, 1),// FDCAN2RST
    VSF_HW_RST_FDCAN5                   = VSF_HW_CLKRST_REGION(0x29, 20, 1),// FDCAN5RST
    VSF_HW_RST_FDCAN6                   = VSF_HW_CLKRST_REGION(0x29, 16, 1),// FDCAN6RST
    VSF_HW_RST_CAHI                     = VSF_HW_CLKRST_REGION(0x29, 4,  1),// CAHIRST
    VSF_HW_RST_CAHD                     = VSF_HW_CLKRST_REGION(0x29, 0,  1),// CAHDRST

    // APB2
    // RCC.APB2RST1
    VSF_HW_RST_ATIM1                    = VSF_HW_CLKRST_REGION(0x35, 28, 1),// ATIM1RST
    VSF_HW_RST_ATIM2                    = VSF_HW_CLKRST_REGION(0x35, 24, 1),// ATIM2RST
    VSF_HW_RST_GTIMA1                   = VSF_HW_CLKRST_REGION(0x35, 20, 1),// GTIMA1RST
    VSF_HW_RST_GTIMA2                   = VSF_HW_CLKRST_REGION(0x35, 16, 1),// GTIMA2RST
    VSF_HW_RST_GTIMA3                   = VSF_HW_CLKRST_REGION(0x35, 12, 1),// GTIMA3RST
    VSF_HW_RST_SHRTIM1                  = VSF_HW_CLKRST_REGION(0x35, 8,  1),// SHRTIM1RST
    VSF_HW_RST_SHRTIM2                  = VSF_HW_CLKRST_REGION(0x35, 4,  1),// SHRTIM2RST
    // RCC.APB2RST2
    VSF_HW_RST_I2S1                     = VSF_HW_CLKRST_REGION(0x36, 28, 1),// I2S1RST
    VSF_HW_RST_I2S2                     = VSF_HW_CLKRST_REGION(0x36, 24, 1),// I2S2RST
    VSF_HW_RST_SPI1                     = VSF_HW_CLKRST_REGION(0x36, 20, 1),// SPI1RST
    VSF_HW_RST_SPI2                     = VSF_HW_CLKRST_REGION(0x36, 16, 1),// SPI2RST
    VSF_HW_RST_DSMU                     = VSF_HW_CLKRST_REGION(0x36, 12, 1),// DSMURST
    VSF_HW_RST_I2C4                     = VSF_HW_CLKRST_REGION(0x36, 8,  1),// I2C4RST
    VSF_HW_RST_I2C5                     = VSF_HW_CLKRST_REGION(0x36, 4,  1),// I2C5RST
    VSF_HW_RST_I2C6                     = VSF_HW_CLKRST_REGION(0x36, 0,  1),// I2C6RST
    // RCC.APB2RST3
    VSF_HW_RST_USART5                   = VSF_HW_CLKRST_REGION(0x37, 28, 1),// USART5RST
    VSF_HW_RST_USART6                   = VSF_HW_CLKRST_REGION(0x37, 24, 1),// USART6RST
    VSF_HW_RST_USART7                   = VSF_HW_CLKRST_REGION(0x37, 20, 1),// USART7RST
    VSF_HW_RST_USART8                   = VSF_HW_CLKRST_REGION(0x37, 16, 1),// USART8RST
    VSF_HW_RST_UART13                   = VSF_HW_CLKRST_REGION(0x37, 12, 1),// UART13RST
    VSF_HW_RST_UART14                   = VSF_HW_CLKRST_REGION(0x37, 8,  1),// UART14RST
    VSF_HW_RST_UART15                   = VSF_HW_CLKRST_REGION(0x37, 4,  1),// UART15RST
    // RCC.APB2RST4
    VSF_HW_RST_FDCAN3                   = VSF_HW_CLKRST_REGION(0x38, 28, 1),// FDCAN3RST
    VSF_HW_RST_FDCAN4                   = VSF_HW_CLKRST_REGION(0x38, 24, 1),// FDCAN4RST
    VSF_HW_RST_FDCAN7                   = VSF_HW_CLKRST_REGION(0x38, 20, 1),// FDCAN7RST
    VSF_HW_RST_FDCAN8                   = VSF_HW_CLKRST_REGION(0x38, 16, 1),// FDCAN8RST

    // APB5
    // RCC.APB5RST1
    VSF_HW_RST_ATIM3                    = VSF_HW_CLKRST_REGION(0x41, 28, 1),// ATIM3RST
    VSF_HW_RST_ATIM4                    = VSF_HW_CLKRST_REGION(0x41, 24, 1),// ATIM4RST
    VSF_HW_RST_SPI4                     = VSF_HW_CLKRST_REGION(0x41, 12, 1),// SPI4RST
    VSF_HW_RST_SPI5                     = VSF_HW_CLKRST_REGION(0x41, 8,  1),// SPI5RST
    VSF_HW_RST_SPI6                     = VSF_HW_CLKRST_REGION(0x41, 4,  1),// SPI6RST
    VSF_HW_RST_SPI7                     = VSF_HW_CLKRST_REGION(0x41, 0,  1),// SPI7RST
    // RCC.APB5RST2
    VSF_HW_RST_I2C7                     = VSF_HW_CLKRST_REGION(0x42, 28, 1),// I2C7RST
    VSF_HW_RST_I2C8                     = VSF_HW_CLKRST_REGION(0x42, 24, 1),// I2C8RST
    VSF_HW_RST_I2C9                     = VSF_HW_CLKRST_REGION(0x42, 20, 1),// I2C9RST
    VSF_HW_RST_I2C10                    = VSF_HW_CLKRST_REGION(0x42, 16, 1),// I2C10RST

    // Retention domain
    // RCC.RDRST1
    VSF_HW_RST_LPTIM1                   = VSF_HW_CLKRST_REGION(0x47, 28, 1),// LPTIM1RST
    VSF_HW_RST_LPTIM2                   = VSF_HW_CLKRST_REGION(0x47, 24, 1),// LPTIM2RST
    VSF_HW_RST_LPTIM3                   = VSF_HW_CLKRST_REGION(0x47, 20, 1),// LPTIM3RST
    VSF_HW_RST_LPTIM4                   = VSF_HW_CLKRST_REGION(0x47, 16, 1),// LPTIM4RST
    VSF_HW_RST_LPTIM5                   = VSF_HW_CLKRST_REGION(0x47, 12, 1),// LPTIM5RST
    VSF_HW_RST_LPUART1                  = VSF_HW_CLKRST_REGION(0x47, 8,  1),// LPUART1RST
    VSF_HW_RST_LPUART2                  = VSF_HW_CLKRST_REGION(0x47, 4,  1),// LPUART2RST
    // RCC.RDRST2
    VSF_HW_RST_COMP                     = VSF_HW_CLKRST_REGION(0x48, 28, 1),// COMPRST
} vsf_hw_peripheral_rst_t;

typedef enum vsf_hw_peripheral_en_t {
    // AXI
    // RCC.AXIEN1
#if     defined(CORE_CM4)
    VSF_HW_EN_JPEGD                     = VSF_HW_CLKRST_REGION(0x50, 30, 1),// M4JPEGDEN
    VSF_HW_EN_JPEGDLP                   = VSF_HW_CLKRST_REGION(0x50, 28, 1),// M4JPEGDLPEN
    VSF_HW_EN_JPEGE                     = VSF_HW_CLKRST_REGION(0x50, 22, 1),// M4JPEGEEN
    VSF_HW_EN_JPEGELP                   = VSF_HW_CLKRST_REGION(0x50, 20, 1),// M4JPEGELPEN
    VSF_HW_EN_DMAMUX2                   = VSF_HW_CLKRST_REGION(0x50, 18, 1),// M4DMAMUX2EN
    VSF_HW_EN_DMAMUX2LP                 = VSF_HW_CLKRST_REGION(0x50, 16, 1),// M4DMAMUX2LPEN
    VSF_HW_EN_MDMA                      = VSF_HW_CLKRST_REGION(0x50, 14, 1),// M4MDMAEN
    VSF_HW_EN_MDMALP                    = VSF_HW_CLKRST_REGION(0x50, 12, 1),// M4MDMALPEN
    VSF_HW_EN_SDMMC1                    = VSF_HW_CLKRST_REGION(0x50, 10, 1),// M4SDMMC1EN
    VSF_HW_EN_SDMMC1LP                  = VSF_HW_CLKRST_REGION(0x50, 8,  1),// M4SDMMC1LPEN
    VSF_HW_EN_ECCM1                     = VSF_HW_CLKRST_REGION(0x50, 6,  1),// M4ECCM1EN
    VSF_HW_EN_ECCM1LP                   = VSF_HW_CLKRST_REGION(0x50, 4,  1),// M4ECCM1LPEN
    VSF_HW_EN_OTPC                      = VSF_HW_CLKRST_REGION(0x50, 2,  1),// M4OPTCEN
    VSF_HW_EN_OTPCLP                    = VSF_HW_CLKRST_REGION(0x50, 0,  1),// M4OPTCLPEN
#elif   defined(CORE_CM7)
    VSF_HW_EN_JPEGD                     = VSF_HW_CLKRST_REGION(0x50, 31, 1),// M7JPEGDEN
    VSF_HW_EN_JPEGDLP                   = VSF_HW_CLKRST_REGION(0x50, 29, 1),// M7JPEGDLPEN
    VSF_HW_EN_JPEGE                     = VSF_HW_CLKRST_REGION(0x50, 23, 1),// M7JPEGEEN
    VSF_HW_EN_JPEGELP                   = VSF_HW_CLKRST_REGION(0x50, 21, 1),// M7JPEGELPEN
    VSF_HW_EN_DMAMUX2                   = VSF_HW_CLKRST_REGION(0x50, 19, 1),// M7DMAMUX2EN
    VSF_HW_EN_DMAMUX2LP                 = VSF_HW_CLKRST_REGION(0x50, 17, 1),// M7DMAMUX2LPEN
    VSF_HW_EN_MDMA                      = VSF_HW_CLKRST_REGION(0x50, 15, 1),// M7MDMAEN
    VSF_HW_EN_MDMALP                    = VSF_HW_CLKRST_REGION(0x50, 13, 1),// M7MDMALPEN
    VSF_HW_EN_SDMMC1                    = VSF_HW_CLKRST_REGION(0x50, 11, 1),// M7SDMMC1EN
    VSF_HW_EN_SDMMC1LP                  = VSF_HW_CLKRST_REGION(0x50, 9,  1),// M7SDMMC1LPEN
    VSF_HW_EN_ECCM1                     = VSF_HW_CLKRST_REGION(0x50, 7,  1),// M7ECCM1EN
    VSF_HW_EN_ECCM1LP                   = VSF_HW_CLKRST_REGION(0x50, 5,  1),// M7ECCM1LPEN
    VSF_HW_EN_OTPC                      = VSF_HW_CLKRST_REGION(0x50, 3,  1),// M7OPTCEN
    VSF_HW_EN_OTPCLP                    = VSF_HW_CLKRST_REGION(0x50, 1,  1),// M7OPTCLPEN
#endif
    // RCC.AXIEN2
#if     defined(CORE_CM4)
    VSF_HW_EN_DSI                       = VSF_HW_CLKRST_REGION(0x51, 30, 1),// M4DSIEN
    VSF_HW_EN_DSILP                     = VSF_HW_CLKRST_REGION(0x51, 28, 1),// M4DSILPEN
    VSF_HW_EN_LCD                       = VSF_HW_CLKRST_REGION(0x51, 26, 1),// M4LCDEN
    VSF_HW_EN_LCDLP                     = VSF_HW_CLKRST_REGION(0x51, 24, 1),// M4LCDLPEN
    VSF_HW_EN_LCDAPB                    = VSF_HW_CLKRST_REGION(0x51, 22, 1),// M4LCDAPBEN
    VSF_HW_EN_LCDAPBLP                  = VSF_HW_CLKRST_REGION(0x51, 20, 1),// M4LCDAPBLPEN
    VSF_HW_EN_DVP1                      = VSF_HW_CLKRST_REGION(0x51, 18, 1),// M4DVP1EN
    VSF_HW_EN_DVP1LP                    = VSF_HW_CLKRST_REGION(0x51, 16, 1),// M4DVP1LPEN
    VSF_HW_EN_DVP1APB                   = VSF_HW_CLKRST_REGION(0x51, 14, 1),// M4DVP1APBEN
    VSF_HW_EN_DVP1APBLP                 = VSF_HW_CLKRST_REGION(0x51, 12, 1),// M4DVP1APBLPEN
    VSF_HW_EN_DVP2                      = VSF_HW_CLKRST_REGION(0x51, 10, 1),// M4DVP2EN
    VSF_HW_EN_DVP2LP                    = VSF_HW_CLKRST_REGION(0x51, 8,  1),// M4DVP2LPEN
    VSF_HW_EN_DVP2APB                   = VSF_HW_CLKRST_REGION(0x51, 6,  1),// M4DVP2APBEN
    VSF_HW_EN_DVP2APBLP                 = VSF_HW_CLKRST_REGION(0x51, 4,  1),// M4DVP2APBLPEN
    VSF_HW_EN_WWDG1                     = VSF_HW_CLKRST_REGION(0x51, 2,  1),// M4WWDG1EN
    VSF_HW_EN_WWDG1LP                   = VSF_HW_CLKRST_REGION(0x51, 0,  1),// M4WWDG1LPEN
#elif   defined(CORE_CM7)
    VSF_HW_EN_DSI                       = VSF_HW_CLKRST_REGION(0x51, 31, 1),// M7DSIEN
    VSF_HW_EN_DSILP                     = VSF_HW_CLKRST_REGION(0x51, 29, 1),// M7DSILPEN
    VSF_HW_EN_LCD                       = VSF_HW_CLKRST_REGION(0x51, 27, 1),// M7LCDEN
    VSF_HW_EN_LCDLP                     = VSF_HW_CLKRST_REGION(0x51, 25, 1),// M7LCDLPEN
    VSF_HW_EN_LCDAPB                    = VSF_HW_CLKRST_REGION(0x51, 23, 1),// M7LCDAPBEN
    VSF_HW_EN_LCDAPBLP                  = VSF_HW_CLKRST_REGION(0x51, 21, 1),// M7LCDAPBLPEN
    VSF_HW_EN_DVP1                      = VSF_HW_CLKRST_REGION(0x51, 19, 1),// M7DVP1EN
    VSF_HW_EN_DVP1LP                    = VSF_HW_CLKRST_REGION(0x51, 17, 1),// M7DVP1LPEN
    VSF_HW_EN_DVP1APB                   = VSF_HW_CLKRST_REGION(0x51, 15, 1),// M7DVP1APBEN
    VSF_HW_EN_DVP1APBLP                 = VSF_HW_CLKRST_REGION(0x51, 13, 1),// M7DVP1APBLPEN
    VSF_HW_EN_DVP2                      = VSF_HW_CLKRST_REGION(0x51, 11, 1),// M7DVP2EN
    VSF_HW_EN_DVP2LP                    = VSF_HW_CLKRST_REGION(0x51, 9,  1),// M7DVP2LPEN
    VSF_HW_EN_DVP2APB                   = VSF_HW_CLKRST_REGION(0x51, 7,  1),// M7DVP2APBEN
    VSF_HW_EN_DVP2APBLP                 = VSF_HW_CLKRST_REGION(0x51, 5,  1),// M7DVP2APBLPEN
    VSF_HW_EN_WWDG1                     = VSF_HW_CLKRST_REGION(0x51, 3,  1),// M7WWDG1EN
    VSF_HW_EN_WWDG1LP                   = VSF_HW_CLKRST_REGION(0x51, 1,  1),// M7WWDG1LPEN
#endif
    // RCC.AXIEN3
#if     defined(CORE_CM4)
    VSF_HW_EN_TASRAM2                   = VSF_HW_CLKRST_REGION(0x52, 30, 1),// M4TASRAM2EN
    VSF_HW_EN_TASRAM2LP                 = VSF_HW_CLKRST_REGION(0x52, 28, 1),// M4TASRAM2LPEN
    VSF_HW_EN_TASRAM3                   = VSF_HW_CLKRST_REGION(0x52, 26, 1),// M4TASRAM3EN
    VSF_HW_EN_TASRAM3LP                 = VSF_HW_CLKRST_REGION(0x52, 24, 1),// M4TASRAM3LPEN
    VSF_HW_EN_TCM                       = VSF_HW_CLKRST_REGION(0x52, 22, 1),// M4TCMEN
    VSF_HW_EN_TCMLP                     = VSF_HW_CLKRST_REGION(0x52, 20, 1),// M4TCMLPEN
    VSF_HW_EN_TCMAXI                    = VSF_HW_CLKRST_REGION(0x52, 18, 1),// M4TCMAXIEN
    VSF_HW_EN_TCMAXILP                  = VSF_HW_CLKRST_REGION(0x52, 16, 1),// M4TCMAXILPEN
    VSF_HW_EN_TCMAPB                    = VSF_HW_CLKRST_REGION(0x52, 14, 1),// M4TCMAPBEN
    VSF_HW_EN_TCMAPBLP                  = VSF_HW_CLKRST_REGION(0x52, 12, 1),// M4TCMAPBLPEN
    VSF_HW_EN_ASRAM1                    = VSF_HW_CLKRST_REGION(0x52, 10, 1),// M4ASRAM1EN
    VSF_HW_EN_ASRAM1LP                  = VSF_HW_CLKRST_REGION(0x52, 8,  1),// M4ASRAM1LPEN
    VSF_HW_EN_AXIROM                    = VSF_HW_CLKRST_REGION(0x52, 6,  1),// M4AXIROMEN
    VSF_HW_EN_AXIROMLP                  = VSF_HW_CLKRST_REGION(0x52, 4,  1),// M4AXIROMLPEN
    VSF_HW_EN_GPU                       = VSF_HW_CLKRST_REGION(0x52, 2,  1),// M4GPUEN
    VSF_HW_EN_GPULP                     = VSF_HW_CLKRST_REGION(0x52, 0,  1),// M4GPULPEN
#elif   defined(CORE_CM7)
    VSF_HW_EN_TASRAM2                   = VSF_HW_CLKRST_REGION(0x52, 31, 1),// M7TASRAM2EN
    VSF_HW_EN_TASRAM2LP                 = VSF_HW_CLKRST_REGION(0x52, 29, 1),// M7TASRAM2LPEN
    VSF_HW_EN_TASRAM3                   = VSF_HW_CLKRST_REGION(0x52, 27, 1),// M7TASRAM3EN
    VSF_HW_EN_TASRAM3LP                 = VSF_HW_CLKRST_REGION(0x52, 25, 1),// M7TASRAM3LPEN
    VSF_HW_EN_TCM                       = VSF_HW_CLKRST_REGION(0x52, 23, 1),// M7TCMEN
    VSF_HW_EN_TCMLP                     = VSF_HW_CLKRST_REGION(0x52, 21, 1),// M7TCMLPEN
    VSF_HW_EN_TCMAXI                    = VSF_HW_CLKRST_REGION(0x52, 19, 1),// M7TCMAXIEN
    VSF_HW_EN_TCMAXILP                  = VSF_HW_CLKRST_REGION(0x52, 17, 1),// M7TCMAXILPEN
    VSF_HW_EN_TCMAPB                    = VSF_HW_CLKRST_REGION(0x52, 15, 1),// M7TCMAPBEN
    VSF_HW_EN_TCMAPBLP                  = VSF_HW_CLKRST_REGION(0x52, 13, 1),// M7TCMAPBLPEN
    VSF_HW_EN_ASRAM1                    = VSF_HW_CLKRST_REGION(0x52, 11, 1),// M7ASRAM1EN
    VSF_HW_EN_ASRAM1LP                  = VSF_HW_CLKRST_REGION(0x52, 9,  1),// M7ASRAM1LPEN
    VSF_HW_EN_AXIROM                    = VSF_HW_CLKRST_REGION(0x52, 7,  1),// M7AXIROMEN
    VSF_HW_EN_AXIROMLP                  = VSF_HW_CLKRST_REGION(0x52, 5,  1),// M7AXIROMLPEN
    VSF_HW_EN_GPU                       = VSF_HW_CLKRST_REGION(0x52, 3,  1),// M7GPUEN
    VSF_HW_EN_GPULP                     = VSF_HW_CLKRST_REGION(0x52, 1,  1),// M7GPULPEN
#endif
    // RCC.AXIEN4
#if     defined(CORE_CM4)
    VSF_HW_EN_XSPI1                     = VSF_HW_CLKRST_REGION(0x53, 30, 1),// M4XSPI1EN
    VSF_HW_EN_XSPI1LP                   = VSF_HW_CLKRST_REGION(0x53, 28, 1),// M4XSPI1LPEN
    VSF_HW_EN_XSPI2                     = VSF_HW_CLKRST_REGION(0x53, 26, 1),// M4XSPI2EN
    VSF_HW_EN_XSPI2LP                   = VSF_HW_CLKRST_REGION(0x53, 24, 1),// M4XSPI2LPEN    
    VSF_HW_EN_FEMC                      = VSF_HW_CLKRST_REGION(0x53, 22, 1),// M4FEMCEN
    VSF_HW_EN_FEMCLP                    = VSF_HW_CLKRST_REGION(0x53, 20, 1),// M4FEMCLPEN
    VSF_HW_EN_SDRAM                     = VSF_HW_CLKRST_REGION(0x53, 18, 1),// M4SDRAMEN
    VSF_HW_EN_SDRAMLP                   = VSF_HW_CLKRST_REGION(0x53, 16, 1),// M4SDRAMLPEN
    VSF_HW_EN_DSIULPS                   = VSF_HW_CLKRST_REGION(0x53, 2,  1),// M4DSIULPSEN
    VSF_HW_EN_DSIULPSLP                 = VSF_HW_CLKRST_REGION(0x53, 0,  1),// M4DSIULPSLPEN
#elif   defined(CORE_CM7)
#endif

    // AHB1
    // RCC.AHB1EN1
#if     defined(CORE_CM4)
    VSF_HW_EN_SDMMC2                    = VSF_HW_CLKRST_REGION(0x15, 30, 1),// M4SDMMC2EN
    VSF_HW_EN_SDMMC2LP                  = VSF_HW_CLKRST_REGION(0x15, 28, 1),// M4SDMMC2LPEN
    VSF_HW_EN_USB2                      = VSF_HW_CLKRST_REGION(0x15, 22, 1),// M4USB2EN
    VSF_HW_EN_USB2LP                    = VSF_HW_CLKRST_REGION(0x15, 20, 1),// M4USB2LPEN
    VSF_HW_EN_DMAMUX1                   = VSF_HW_CLKRST_REGION(0x15, 18, 1),// M4DMAMUX1EN
    VSF_HW_EN_DMAMUX1LP                 = VSF_HW_CLKRST_REGION(0x15, 16, 1),// M4DMAMUX1LPEN
    VSF_HW_EN_ADC1PLL                   = VSF_HW_CLKRST_REGION(0x15, 14, 1),// M4ADC1PLLEN
    VSF_HW_EN_ADC1PLLLP                 = VSF_HW_CLKRST_REGION(0x15, 12, 1),// M4ADC1PLLLPEN
    VSF_HW_EN_ADC1SYS                   = VSF_HW_CLKRST_REGION(0x15, 10, 1),// M4ADC1SYSEN
    VSF_HW_EN_ADC1SYSLP                 = VSF_HW_CLKRST_REGION(0x15, 8,  1),// M4ADC1SYSLPEN
    VSF_HW_EN_ADC1BUS                   = VSF_HW_CLKRST_REGION(0x15, 2,  1),// M4ADC1BUSEN
    VSF_HW_EN_ADC1BUSLP                 = VSF_HW_CLKRST_REGION(0x15, 0,  1),// M4ADC1BUSLPEN
#elif   defined(CORE_CM7)
    VSF_HW_EN_SDMMC2                    = VSF_HW_CLKRST_REGION(0x15, 31, 1),// M7SDMMC2EN
    VSF_HW_EN_SDMMC2LP                  = VSF_HW_CLKRST_REGION(0x15, 29, 1),// M7SDMMC2LPEN
    VSF_HW_EN_USB2                      = VSF_HW_CLKRST_REGION(0x15, 23, 1),// M7USB2EN
    VSF_HW_EN_USB2LP                    = VSF_HW_CLKRST_REGION(0x15, 21, 1),// M7USB2LPEN
    VSF_HW_EN_DMAMUX1                   = VSF_HW_CLKRST_REGION(0x15, 19, 1),// M7DMAMUX1EN
    VSF_HW_EN_DMAMUX1LP                 = VSF_HW_CLKRST_REGION(0x15, 17, 1),// M7DMAMUX1LPEN
    VSF_HW_EN_ADC1PLL                   = VSF_HW_CLKRST_REGION(0x15, 15, 1),// M7ADC1PLLEN
    VSF_HW_EN_ADC1PLLLP                 = VSF_HW_CLKRST_REGION(0x15, 13, 1),// M7ADC1PLLLPEN
    VSF_HW_EN_ADC1SYS                   = VSF_HW_CLKRST_REGION(0x15, 11, 1),// M7ADC1SYSEN
    VSF_HW_EN_ADC1SYSLP                 = VSF_HW_CLKRST_REGION(0x15, 9,  1),// M7ADC1SYSLPEN
    VSF_HW_EN_ADC1BUS                   = VSF_HW_CLKRST_REGION(0x15, 3,  1),// M7ADC1BUSEN
    VSF_HW_EN_ADC1BUSLP                 = VSF_HW_CLKRST_REGION(0x15, 1,  1),// M7ADC1BUSLPEN
#endif
    // RCC.AHB1EN2
#if     defined(CORE_CM4)
    VSF_HW_EN_ETH2TX                    = VSF_HW_CLKRST_REGION(0x16, 10, 1),// M4ETH2TXEN
    VSF_HW_EN_ETH2TXLP                  = VSF_HW_CLKRST_REGION(0x16, 8,  1),// M4ETH2TXLPEN
    VSF_HW_EN_ETH2RX                    = VSF_HW_CLKRST_REGION(0x16, 6,  1),// M4ETH2RXEN
    VSF_HW_EN_ETH2RXLP                  = VSF_HW_CLKRST_REGION(0x16, 4,  1),// M4ETH2RXLPEN
    VSF_HW_EN_ETH2MAC                   = VSF_HW_CLKRST_REGION(0x16, 2,  1),// M4ETH2MACEN
    VSF_HW_EN_ETH2MACLP                 = VSF_HW_CLKRST_REGION(0x16, 0,  1),// M4ETH2MACLPEN
#elif   defined(CORE_CM7)
    VSF_HW_EN_ETH2TX                    = VSF_HW_CLKRST_REGION(0x16, 11, 1),// M7ETH2TXEN
    VSF_HW_EN_ETH2TXLP                  = VSF_HW_CLKRST_REGION(0x16, 9,  1),// M7ETH2TXLPEN
    VSF_HW_EN_ETH2RX                    = VSF_HW_CLKRST_REGION(0x16, 7,  1),// M7ETH2RXEN
    VSF_HW_EN_ETH2RXLP                  = VSF_HW_CLKRST_REGION(0x16, 5,  1),// M7ETH2RXLPEN
    VSF_HW_EN_ETH2MAC                   = VSF_HW_CLKRST_REGION(0x16, 3,  1),// M7ETH2MACEN
    VSF_HW_EN_ETH2MACLP                 = VSF_HW_CLKRST_REGION(0x16, 1,  1),// M7ETH2MACLPEN
#endif
    // RCC.AHB1EN3
#if     defined(CORE_CM4)
    VSF_HW_EN_ECCMAC                    = VSF_HW_CLKRST_REGION(0x17, 26, 1),// M4ECCMACEN
    VSF_HW_EN_ECCMACLP                  = VSF_HW_CLKRST_REGION(0x17, 24, 1),// M4ECCMACLPEN
    VSF_HW_EN_DMA1                      = VSF_HW_CLKRST_REGION(0x17, 18, 1),// M4DMA1EN
    VSF_HW_EN_DMA1LP                    = VSF_HW_CLKRST_REGION(0x17, 16, 1),// M4DMA1LPEN
    VSF_HW_EN_DMA2                      = VSF_HW_CLKRST_REGION(0x17, 10, 1),// M4DMA2EN
    VSF_HW_EN_DMA2LP                    = VSF_HW_CLKRST_REGION(0x17, 8,  1),// M4DMA2LPEN
    VSF_HW_EN_DMA3                      = VSF_HW_CLKRST_REGION(0x17, 2,  1),// M4DMA3EN
    VSF_HW_EN_DMA3LP                    = VSF_HW_CLKRST_REGION(0x17, 0,  1),// M4DMA3LPEN
#elif   defined(CORE_CM7)
    VSF_HW_EN_ECCMAC                    = VSF_HW_CLKRST_REGION(0x17, 27, 1),// M7ECCMACEN
    VSF_HW_EN_ECCMACLP                  = VSF_HW_CLKRST_REGION(0x17, 25, 1),// M7ECCMACLPEN
    VSF_HW_EN_DMA1                      = VSF_HW_CLKRST_REGION(0x17, 19, 1),// M7DMA1EN
    VSF_HW_EN_DMA1LP                    = VSF_HW_CLKRST_REGION(0x17, 17, 1),// M7DMA1LPEN
    VSF_HW_EN_DMA2                      = VSF_HW_CLKRST_REGION(0x17, 11, 1),// M7DMA2EN
    VSF_HW_EN_DMA2LP                    = VSF_HW_CLKRST_REGION(0x17, 9,  1),// M7DMA2LPEN
    VSF_HW_EN_DMA3                      = VSF_HW_CLKRST_REGION(0x17, 3,  1),// M7DMA3EN
    VSF_HW_EN_DMA3LP                    = VSF_HW_CLKRST_REGION(0x17, 1,  1),// M7DMA3LPEN
#endif
    // RCC.AHB1EN4
#if     defined(CORE_CM4)
    VSF_HW_EN_ADC2PLL                   = VSF_HW_CLKRST_REGION(0x18, 30, 1),// M4ADC2PLLEN
    VSF_HW_EN_ADC2PLLLP                 = VSF_HW_CLKRST_REGION(0x18, 28, 1),// M4ADC2PLLLPEN
    VSF_HW_EN_ADC2SYS                   = VSF_HW_CLKRST_REGION(0x18, 26, 1),// M4ADC2SYSEN
    VSF_HW_EN_ADC2SYSLP                 = VSF_HW_CLKRST_REGION(0x18, 24, 1),// M4ADC2SYSLPEN
    VSF_HW_EN_ADC2BUS                   = VSF_HW_CLKRST_REGION(0x18, 18, 1),// M4ADC2BUSEN
    VSF_HW_EN_ADC2BUSLP                 = VSF_HW_CLKRST_REGION(0x18, 16, 1),// M4ADC2BUSLPEN
    VSF_HW_EN_ADC3PLL                   = VSF_HW_CLKRST_REGION(0x18, 14, 1),// M4ADC3PLLEN
    VSF_HW_EN_ADC3PLLLP                 = VSF_HW_CLKRST_REGION(0x18, 12, 1),// M4ADC3PLLLPEN
    VSF_HW_EN_ADC3SYS                   = VSF_HW_CLKRST_REGION(0x18, 10, 1),// M4ADC3SYSEN
    VSF_HW_EN_ADC3SYSLP                 = VSF_HW_CLKRST_REGION(0x18, 8,  1),// M4ADC3SYSLPEN
    VSF_HW_EN_ADC3BUS                   = VSF_HW_CLKRST_REGION(0x18, 2,  1),// M4ADC3BUSEN
    VSF_HW_EN_ADC3BUSLP                 = VSF_HW_CLKRST_REGION(0x18, 0,  1),// M4ADC3BUSLPEN
#elif   defined(CORE_CM7)
    VSF_HW_EN_ADC2PLL                   = VSF_HW_CLKRST_REGION(0x18, 31, 1),// M7ADC2PLLEN
    VSF_HW_EN_ADC2PLLLP                 = VSF_HW_CLKRST_REGION(0x18, 29, 1),// M7ADC2PLLLPEN
    VSF_HW_EN_ADC2SYS                   = VSF_HW_CLKRST_REGION(0x18, 27, 1),// M7ADC2SYSEN
    VSF_HW_EN_ADC2SYSLP                 = VSF_HW_CLKRST_REGION(0x18, 25, 1),// M7ADC2SYSLPEN
    VSF_HW_EN_ADC2BUS                   = VSF_HW_CLKRST_REGION(0x18, 19, 1),// M7ADC2BUSEN
    VSF_HW_EN_ADC2BUSLP                 = VSF_HW_CLKRST_REGION(0x18, 17, 1),// M7ADC2BUSLPEN
    VSF_HW_EN_ADC3PLL                   = VSF_HW_CLKRST_REGION(0x18, 15, 1),// M7ADC3PLLEN
    VSF_HW_EN_ADC3PLLLP                 = VSF_HW_CLKRST_REGION(0x18, 13, 1),// M7ADC3PLLLPEN
    VSF_HW_EN_ADC3SYS                   = VSF_HW_CLKRST_REGION(0x18, 11, 1),// M7ADC3SYSEN
    VSF_HW_EN_ADC3SYSLP                 = VSF_HW_CLKRST_REGION(0x18, 9,  1),// M7ADC3SYSLPEN
    VSF_HW_EN_ADC3BUS                   = VSF_HW_CLKRST_REGION(0x18, 3,  1),// M7ADC3BUSEN
    VSF_HW_EN_ADC3BUSLP                 = VSF_HW_CLKRST_REGION(0x18, 1,  1),// M7ADC3BUSLPEN
#endif

    // AHB2
    // RCC.AHB2EN1
#if     defined(CORE_CM4)
    VSF_HW_EN_USB1                      = VSF_HW_CLKRST_REGION(0x2C, 22, 1),// M4USB1EN
    VSF_HW_EN_USB1LP                    = VSF_HW_CLKRST_REGION(0x2C, 20, 1),// M4USB1LPEN
    VSF_HW_EN_ECCM2                     = VSF_HW_CLKRST_REGION(0x2C, 14, 1),// M4ECCM2EN
    VSF_HW_EN_ECCM2LP                   = VSF_HW_CLKRST_REGION(0x2C, 12, 1),// M4ECCM2LPEN
    VSF_HW_EN_CORDIC                    = VSF_HW_CLKRST_REGION(0x2C, 10, 1),// M4CORDICEN
    VSF_HW_EN_CORDICLP                  = VSF_HW_CLKRST_REGION(0x2C, 8,  1),// M4CORDICLPEN
    VSF_HW_EN_SDPU                      = VSF_HW_CLKRST_REGION(0x2C, 6,  1),// M4SDPUEN
    VSF_HW_EN_SDPULP                    = VSF_HW_CLKRST_REGION(0x2C, 4,  1),// M4SDPULPEN
    VSF_HW_EN_FMAC                      = VSF_HW_CLKRST_REGION(0x2C, 2,  1),// M4FMACEN
    VSF_HW_EN_FMACLP                    = VSF_HW_CLKRST_REGION(0x2C, 0,  1),// M4FMACLPEN
#elif   defined(CORE_CM7)
    VSF_HW_EN_USB1                      = VSF_HW_CLKRST_REGION(0x2C, 23, 1),// M7USB1EN
    VSF_HW_EN_USB1LP                    = VSF_HW_CLKRST_REGION(0x2C, 21, 1),// M7USB1LPEN
    VSF_HW_EN_ECCM2                     = VSF_HW_CLKRST_REGION(0x2C, 15, 1),// M7ECCM2EN
    VSF_HW_EN_ECCM2LP                   = VSF_HW_CLKRST_REGION(0x2C, 13, 1),// M7ECCM2LPEN
    VSF_HW_EN_CORDIC                    = VSF_HW_CLKRST_REGION(0x2C, 11, 1),// M7CORDICEN
    VSF_HW_EN_CORDICLP                  = VSF_HW_CLKRST_REGION(0x2C, 9,  1),// M7CORDICLPEN
    VSF_HW_EN_SDPU                      = VSF_HW_CLKRST_REGION(0x2C, 7,  1),// M7SDPUEN
    VSF_HW_EN_SDPULP                    = VSF_HW_CLKRST_REGION(0x2C, 5,  1),// M7SDPULPEN
    VSF_HW_EN_FMAC                      = VSF_HW_CLKRST_REGION(0x2C, 3,  1),// M7FMACEN
    VSF_HW_EN_FMACLP                    = VSF_HW_CLKRST_REGION(0x2C, 1,  1),// M7FMACLPEN
#endif
    // RCC.AHB2EN2
#if     defined(CORE_CM4)
    VSF_HW_EN_DAC56                     = VSF_HW_CLKRST_REGION(0x6D, 22, 1),// M4DAC56EN
    VSF_HW_EN_DAC56LP                   = VSF_HW_CLKRST_REGION(0x6D, 20, 1),// M4DAC56LPEN
    VSF_HW_EN_DAC34                     = VSF_HW_CLKRST_REGION(0x6D, 18, 1),// M4DAC34EN
    VSF_HW_EN_DAC34LP                   = VSF_HW_CLKRST_REGION(0x6D, 16, 1),// M4DAC34LPEN
    VSF_HW_EN_ETH1TX                    = VSF_HW_CLKRST_REGION(0x6D, 10, 1),// M4ETH1TXEN
    VSF_HW_EN_ETH1TXLP                  = VSF_HW_CLKRST_REGION(0x6D, 8,  1),// M4ETH1TXLPEN
    VSF_HW_EN_ETH1RX                    = VSF_HW_CLKRST_REGION(0x6D, 6,  1),// M4ETH1RXEN
    VSF_HW_EN_ETH1RXLP                  = VSF_HW_CLKRST_REGION(0x6D, 4,  1),// M4ETH1RXULPEN
    VSF_HW_EN_ETH1MAC                   = VSF_HW_CLKRST_REGION(0x6D, 2,  1),// M4ETH1MACEN
    VSF_HW_EN_ETH1MACLP                 = VSF_HW_CLKRST_REGION(0x6D, 0,  1),// M4ETH1MACLPEN
#elif   defined(CORE_CM7)
    VSF_HW_EN_DAC56                     = VSF_HW_CLKRST_REGION(0x6D, 23, 1),// M7DAC56EN
    VSF_HW_EN_DAC56LP                   = VSF_HW_CLKRST_REGION(0x6D, 21, 1),// M7DAC56LPEN
    VSF_HW_EN_DAC34                     = VSF_HW_CLKRST_REGION(0x6D, 19, 1),// M7DAC34EN
    VSF_HW_EN_DAC34LP                   = VSF_HW_CLKRST_REGION(0x6D, 17, 1),// M7DAC34LPEN
    VSF_HW_EN_ETH1TX                    = VSF_HW_CLKRST_REGION(0x6D, 11, 1),// M7ETH1TXEN
    VSF_HW_EN_ETH1TXLP                  = VSF_HW_CLKRST_REGION(0x6D, 9,  1),// M7ETH1TXLPEN
    VSF_HW_EN_ETH1RX                    = VSF_HW_CLKRST_REGION(0x6D, 7,  1),// M7ETH1RXEN
    VSF_HW_EN_ETH1RXLP                  = VSF_HW_CLKRST_REGION(0x6D, 5,  1),// M7ETH1RXULPEN
    VSF_HW_EN_ETH1MAC                   = VSF_HW_CLKRST_REGION(0x6D, 3,  1),// M7ETH1MACEN
    VSF_HW_EN_ETH1MACLP                 = VSF_HW_CLKRST_REGION(0x6D, 1,  1),// M7ETH1MACLPEN
#endif

    // AHB5
    // RCC.AHB5EN1
#if     defined(CORE_CM4)
    VSF_HW_EN_GPIOA                     = VSF_HW_CLKRST_REGION(0x39, 30, 1),// M4GPIOAEN
    VSF_HW_EN_GPIOALP                   = VSF_HW_CLKRST_REGION(0x39, 28, 1),// M4GPIOALPEN
    VSF_HW_EN_GPIOB                     = VSF_HW_CLKRST_REGION(0x39, 26, 1),// M4GPIOBEN
    VSF_HW_EN_GPIOBLP                   = VSF_HW_CLKRST_REGION(0x39, 24, 1),// M4GPIOBLPEN
    VSF_HW_EN_GPIOC                     = VSF_HW_CLKRST_REGION(0x39, 22, 1),// M4GPIOCEN
    VSF_HW_EN_GPIOCLP                   = VSF_HW_CLKRST_REGION(0x39, 20, 1),// M4GPIOCLPEN
    VSF_HW_EN_GPIOD                     = VSF_HW_CLKRST_REGION(0x39, 18, 1),// M4GPIODEN
    VSF_HW_EN_GPIODLP                   = VSF_HW_CLKRST_REGION(0x39, 16, 1),// M4GPIODLPEN
    VSF_HW_EN_GPIOE                     = VSF_HW_CLKRST_REGION(0x39, 14, 1),// M4GPIOEEN
    VSF_HW_EN_GPIOELP                   = VSF_HW_CLKRST_REGION(0x39, 12, 1),// M4GPIOELPEN
    VSF_HW_EN_GPIOF                     = VSF_HW_CLKRST_REGION(0x39, 10, 1),// M4GPIOFEN
    VSF_HW_EN_GPIOFLP                   = VSF_HW_CLKRST_REGION(0x39, 8,  1),// M4GPIOFLPEN
    VSF_HW_EN_GPIOG                     = VSF_HW_CLKRST_REGION(0x39, 6,  1),// M4GPIOGEN
    VSF_HW_EN_GPIOGLP                   = VSF_HW_CLKRST_REGION(0x39, 4,  1),// M4GPIOGLPEN
    VSF_HW_EN_GPIOH                     = VSF_HW_CLKRST_REGION(0x39, 2,  1),// M4GPIOHEN
    VSF_HW_EN_GPIOHLP                   = VSF_HW_CLKRST_REGION(0x39, 0,  1),// M4GPIOHLPEN
#elif   defined(CORE_CM7)
    VSF_HW_EN_GPIOA                     = VSF_HW_CLKRST_REGION(0x39, 31, 1),// M7GPIOAEN
    VSF_HW_EN_GPIOALP                   = VSF_HW_CLKRST_REGION(0x39, 29, 1),// M7GPIOALPEN
    VSF_HW_EN_GPIOB                     = VSF_HW_CLKRST_REGION(0x39, 27, 1),// M7GPIOBEN
    VSF_HW_EN_GPIOBLP                   = VSF_HW_CLKRST_REGION(0x39, 25, 1),// M7GPIOBLPEN
    VSF_HW_EN_GPIOC                     = VSF_HW_CLKRST_REGION(0x39, 23, 1),// M7GPIOCEN
    VSF_HW_EN_GPIOCLP                   = VSF_HW_CLKRST_REGION(0x39, 21, 1),// M7GPIOCLPEN
    VSF_HW_EN_GPIOD                     = VSF_HW_CLKRST_REGION(0x39, 19, 1),// M7GPIODEN
    VSF_HW_EN_GPIODLP                   = VSF_HW_CLKRST_REGION(0x39, 17, 1),// M7GPIODLPEN
    VSF_HW_EN_GPIOE                     = VSF_HW_CLKRST_REGION(0x39, 15, 1),// M7GPIOEEN
    VSF_HW_EN_GPIOELP                   = VSF_HW_CLKRST_REGION(0x39, 13, 1),// M7GPIOELPEN
    VSF_HW_EN_GPIOF                     = VSF_HW_CLKRST_REGION(0x39, 11, 1),// M7GPIOFEN
    VSF_HW_EN_GPIOFLP                   = VSF_HW_CLKRST_REGION(0x39, 9,  1),// M7GPIOFLPEN
    VSF_HW_EN_GPIOG                     = VSF_HW_CLKRST_REGION(0x39, 7,  1),// M7GPIOGEN
    VSF_HW_EN_GPIOGLP                   = VSF_HW_CLKRST_REGION(0x39, 5,  1),// M7GPIOGLPEN
    VSF_HW_EN_GPIOH                     = VSF_HW_CLKRST_REGION(0x39, 3,  1),// M7GPIOHEN
    VSF_HW_EN_GPIOHLP                   = VSF_HW_CLKRST_REGION(0x39, 1,  1),// M7GPIOHLPEN
#endif
    // RCC.AHB5EN2
#if     defined(CORE_CM4)
    VSF_HW_EN_GPIOI                     = VSF_HW_CLKRST_REGION(0x3A, 30, 1),// M4GPIOIEN
    VSF_HW_EN_GPIOILP                   = VSF_HW_CLKRST_REGION(0x3A, 28, 1),// M4GPIOILPEN
    VSF_HW_EN_GPIOJ                     = VSF_HW_CLKRST_REGION(0x3A, 26, 1),// M4GPIOJEN
    VSF_HW_EN_GPIOJLP                   = VSF_HW_CLKRST_REGION(0x3A, 24, 1),// M4GPIOJLPEN
    VSF_HW_EN_GPIOK                     = VSF_HW_CLKRST_REGION(0x3A, 22, 1),// M4GPIOKEN
    VSF_HW_EN_GPIOKLP                   = VSF_HW_CLKRST_REGION(0x3A, 20, 1),// M4GPIOKLPEN
    VSF_HW_EN_ECCM3                     = VSF_HW_CLKRST_REGION(0x3A, 18, 1),// M4ECCM3EN
    VSF_HW_EN_ECCM3LP                   = VSF_HW_CLKRST_REGION(0x3A, 16, 1),// M4ECCM3LPEN

    VSF_HW_EN_CRC                       = VSF_HW_CLKRST_REGION(0x3A, 10, 1),// M4CRCEN
    VSF_HW_EN_CRCLP                     = VSF_HW_CLKRST_REGION(0x3A, 8,  1),// M4CRCLPEN
    VSF_HW_EN_SEMA4                     = VSF_HW_CLKRST_REGION(0x3A, 6,  1),// M4SEMA4EN
    VSF_HW_EN_SEMA4LP                   = VSF_HW_CLKRST_REGION(0x3A, 4,  1),// M4SEMA4LPEN
    VSF_HW_EN_AFIO                      = VSF_HW_CLKRST_REGION(0x3A, 2,  1),// M4AFIOEN
    VSF_HW_EN_AFIOLP                    = VSF_HW_CLKRST_REGION(0x3A, 0,  1),// M4AFIOLPEN
#elif   defined(CORE_CM7)
    VSF_HW_EN_GPIOI                     = VSF_HW_CLKRST_REGION(0x3A, 31, 1),// M7GPIOIEN
    VSF_HW_EN_GPIOILP                   = VSF_HW_CLKRST_REGION(0x3A, 29, 1),// M7GPIOILPEN
    VSF_HW_EN_GPIOJ                     = VSF_HW_CLKRST_REGION(0x3A, 27, 1),// M7GPIOJEN
    VSF_HW_EN_GPIOJLP                   = VSF_HW_CLKRST_REGION(0x3A, 25, 1),// M7GPIOJLPEN
    VSF_HW_EN_GPIOK                     = VSF_HW_CLKRST_REGION(0x3A, 23, 1),// M7GPIOKEN
    VSF_HW_EN_GPIOKLP                   = VSF_HW_CLKRST_REGION(0x3A, 21, 1),// M7GPIOKLPEN
    VSF_HW_EN_ECCM3                     = VSF_HW_CLKRST_REGION(0x3A, 19, 1),// M7ECCM3EN
    VSF_HW_EN_ECCM3LP                   = VSF_HW_CLKRST_REGION(0x3A, 17, 1),// M7ECCM3LPEN

    VSF_HW_EN_PWR                       = VSF_HW_CLKRST_REGION(0x3A, 15, 1),// PWREN
    VSF_HW_EN_PWRLP                     = VSF_HW_CLKRST_REGION(0x3A, 13, 1),// PWRLPEN

    VSF_HW_EN_CRC                       = VSF_HW_CLKRST_REGION(0x3A, 11, 1),// M7CRCEN
    VSF_HW_EN_CRCLP                     = VSF_HW_CLKRST_REGION(0x3A, 9,  1),// M7CRCLPEN
    VSF_HW_EN_SEMA4                     = VSF_HW_CLKRST_REGION(0x3A, 7,  1),// M7SEMA4EN
    VSF_HW_EN_SEMA4LP                   = VSF_HW_CLKRST_REGION(0x3A, 5,  1),// M7SEMA4LPEN
    VSF_HW_EN_AFIO                      = VSF_HW_CLKRST_REGION(0x3A, 3,  1),// M7AFIOEN
    VSF_HW_EN_AFIOLP                    = VSF_HW_CLKRST_REGION(0x3A, 1,  1),// M7AFIOLPEN
#endif

    // AHB9
    // RCC.AHB9EN1
#if     defined(CORE_CM4)
    VSF_HW_EN_ESC                       = VSF_HW_CLKRST_REGION(0x70, 2,  1),// M4ESCEN
    VSF_HW_EN_ESCLP                     = VSF_HW_CLKRST_REGION(0x70, 0,  1),// M4ESCLPEN
#elif   defined(CORE_CM7)
    VSF_HW_EN_ESC                       = VSF_HW_CLKRST_REGION(0x70, 3,  1),// M7ESCEN
    VSF_HW_EN_ESCLP                     = VSF_HW_CLKRST_REGION(0x70, 1,  1),// M7ESCLPEN
#endif

    // APB1
    // RCC.APB1EN1
#if     defined(CORE_CM4)
    VSF_HW_EN_BTIM1                     = VSF_HW_CLKRST_REGION(0x20, 30, 1),// M4BTIM1EN
    VSF_HW_EN_BTIM1LP                   = VSF_HW_CLKRST_REGION(0x20, 28, 1),// M4BTIM1LPEN
    VSF_HW_EN_BTIM2                     = VSF_HW_CLKRST_REGION(0x20, 26, 1),// M4BTIM2EN
    VSF_HW_EN_BTIM2LP                   = VSF_HW_CLKRST_REGION(0x20, 24, 1),// M4BTIM2LPEN
    VSF_HW_EN_BTIM3                     = VSF_HW_CLKRST_REGION(0x20, 22, 1),// M4BTIM3EN
    VSF_HW_EN_BTIM3LP                   = VSF_HW_CLKRST_REGION(0x20, 20, 1),// M4BTIM3LPEN
    VSF_HW_EN_BTIM4                     = VSF_HW_CLKRST_REGION(0x20, 18, 1),// M4BTIM4EN
    VSF_HW_EN_BTIM4LP                   = VSF_HW_CLKRST_REGION(0x20, 16, 1),// M4BTIM4LPEN
    VSF_HW_EN_GTIMB1                    = VSF_HW_CLKRST_REGION(0x20, 14, 1),// M4GTIMB1EN
    VSF_HW_EN_GTIMB1LP                  = VSF_HW_CLKRST_REGION(0x20, 12, 1),// M4GTIMB1LPEN
    VSF_HW_EN_GTIMB2                    = VSF_HW_CLKRST_REGION(0x20, 10, 1),// M4GTIMB2EN
    VSF_HW_EN_GTIMB2LP                  = VSF_HW_CLKRST_REGION(0x20, 8,  1),// M4GTIMB2LPEN
    VSF_HW_EN_GTIMB3                    = VSF_HW_CLKRST_REGION(0x20, 6,  1),// M4GTIMB3EN
    VSF_HW_EN_GTIMB3LP                  = VSF_HW_CLKRST_REGION(0x20, 4,  1),// M4GTIMB3LPEN
    VSF_HW_EN_GTIMA4                    = VSF_HW_CLKRST_REGION(0x20, 2,  1),// M4GTIMA4EN
    VSF_HW_EN_GTIMA4LP                  = VSF_HW_CLKRST_REGION(0x20, 0,  1),// M4GTIMA4LPEN
#elif   defined(CORE_CM7)
    VSF_HW_EN_BTIM1                     = VSF_HW_CLKRST_REGION(0x20, 31, 1),// M7BTIM1EN
    VSF_HW_EN_BTIM1LP                   = VSF_HW_CLKRST_REGION(0x20, 29, 1),// M7BTIM1LPEN
    VSF_HW_EN_BTIM2                     = VSF_HW_CLKRST_REGION(0x20, 27, 1),// M7BTIM2EN
    VSF_HW_EN_BTIM2LP                   = VSF_HW_CLKRST_REGION(0x20, 25, 1),// M7BTIM2LPEN
    VSF_HW_EN_BTIM3                     = VSF_HW_CLKRST_REGION(0x20, 23, 1),// M7BTIM3EN
    VSF_HW_EN_BTIM3LP                   = VSF_HW_CLKRST_REGION(0x20, 21, 1),// M7BTIM3LPEN
    VSF_HW_EN_BTIM4                     = VSF_HW_CLKRST_REGION(0x20, 19, 1),// M7BTIM4EN
    VSF_HW_EN_BTIM4LP                   = VSF_HW_CLKRST_REGION(0x20, 17, 1),// M7BTIM4LPEN
    VSF_HW_EN_GTIMB1                    = VSF_HW_CLKRST_REGION(0x20, 15, 1),// M7GTIMB1EN
    VSF_HW_EN_GTIMB1LP                  = VSF_HW_CLKRST_REGION(0x20, 13, 1),// M7GTIMB1LPEN
    VSF_HW_EN_GTIMB2                    = VSF_HW_CLKRST_REGION(0x20, 11, 1),// M7GTIMB2EN
    VSF_HW_EN_GTIMB2LP                  = VSF_HW_CLKRST_REGION(0x20, 9,  1),// M7GTIMB2LPEN
    VSF_HW_EN_GTIMB3                    = VSF_HW_CLKRST_REGION(0x20, 7,  1),// M7GTIMB3EN
    VSF_HW_EN_GTIMB3LP                  = VSF_HW_CLKRST_REGION(0x20, 5,  1),// M7GTIMB3LPEN
    VSF_HW_EN_GTIMA4                    = VSF_HW_CLKRST_REGION(0x20, 3,  1),// M7GTIMA4EN
    VSF_HW_EN_GTIMA4LP                  = VSF_HW_CLKRST_REGION(0x20, 1,  1),// M7GTIMA4LPEN
#endif
    // RCC.APB1EN2
#if     defined(CORE_CM4)
    VSF_HW_EN_GTIMA5                    = VSF_HW_CLKRST_REGION(0x21, 30, 1),// M4GTIMA5EN
    VSF_HW_EN_GTIMA5LP                  = VSF_HW_CLKRST_REGION(0x21, 28, 1),// M4GTIMA5LPEN
    VSF_HW_EN_GTIMA6                    = VSF_HW_CLKRST_REGION(0x21, 26, 1),// M4GTIMA6EN
    VSF_HW_EN_GTIMA6LP                  = VSF_HW_CLKRST_REGION(0x21, 24, 1),// M4GTIMA6LPEN
    VSF_HW_EN_GTIMA7                    = VSF_HW_CLKRST_REGION(0x21, 22, 1),// M4GTIMA7EN
    VSF_HW_EN_GTIMA7LP                  = VSF_HW_CLKRST_REGION(0x21, 20, 1),// M4GTIMA7LPEN
    VSF_HW_EN_SPI3                      = VSF_HW_CLKRST_REGION(0x21, 18, 1),// M4SPI3EN
    VSF_HW_EN_SPI3LP                    = VSF_HW_CLKRST_REGION(0x21, 16, 1),// M4SPI3LPEN
    VSF_HW_EN_DAC12                     = VSF_HW_CLKRST_REGION(0x21, 14, 1),// M4DAC12EN
    VSF_HW_EN_DAC12LP                   = VSF_HW_CLKRST_REGION(0x21, 12, 1),// M4DAC12LPEN
    VSF_HW_EN_WWDG2                     = VSF_HW_CLKRST_REGION(0x21, 6,  1),// M4WWDG2EN
    VSF_HW_EN_WWDG2LP                   = VSF_HW_CLKRST_REGION(0x21, 4,  1),// M4WWDG2LPEN
#elif   defined(CORE_CM7)
    VSF_HW_EN_GTIMA5                    = VSF_HW_CLKRST_REGION(0x21, 31, 1),// M7GTIMA5EN
    VSF_HW_EN_GTIMA5LP                  = VSF_HW_CLKRST_REGION(0x21, 29, 1),// M7GTIMA5LPEN
    VSF_HW_EN_GTIMA6                    = VSF_HW_CLKRST_REGION(0x21, 27, 1),// M7GTIMA6EN
    VSF_HW_EN_GTIMA6LP                  = VSF_HW_CLKRST_REGION(0x21, 25, 1),// M7GTIMA6LPEN
    VSF_HW_EN_GTIMA7                    = VSF_HW_CLKRST_REGION(0x21, 23, 1),// M7GTIMA7EN
    VSF_HW_EN_GTIMA7LP                  = VSF_HW_CLKRST_REGION(0x21, 21, 1),// M7GTIMA7LPEN
    VSF_HW_EN_SPI3                      = VSF_HW_CLKRST_REGION(0x21, 19, 1),// M7SPI3EN
    VSF_HW_EN_SPI3LP                    = VSF_HW_CLKRST_REGION(0x21, 17, 1),// M7SPI3LPEN
    VSF_HW_EN_DAC12                     = VSF_HW_CLKRST_REGION(0x21, 15, 1),// M7DAC12EN
    VSF_HW_EN_DAC12LP                   = VSF_HW_CLKRST_REGION(0x21, 13, 1),// M7DAC12LPEN
    VSF_HW_EN_WWDG2                     = VSF_HW_CLKRST_REGION(0x21, 7,  1),// M7WWDG2EN
    VSF_HW_EN_WWDG2LP                   = VSF_HW_CLKRST_REGION(0x21, 5,  1),// M7WWDG2LPEN
#endif
    // RCC.APB1EN3
#if     defined(CORE_CM4)
    VSF_HW_EN_USART1                    = VSF_HW_CLKRST_REGION(0x22, 30, 1),// M4USART1EN
    VSF_HW_EN_USART1LP                  = VSF_HW_CLKRST_REGION(0x22, 28, 1),// M4USART1LPEN
    VSF_HW_EN_USART2                    = VSF_HW_CLKRST_REGION(0x22, 26, 1),// M4USART2EN
    VSF_HW_EN_USART2LP                  = VSF_HW_CLKRST_REGION(0x22, 24, 1),// M4USART2LPEN
    VSF_HW_EN_USART3                    = VSF_HW_CLKRST_REGION(0x22, 22, 1),// M4USART3EN
    VSF_HW_EN_USART3LP                  = VSF_HW_CLKRST_REGION(0x22, 20, 1),// M4USART3LPEN
    VSF_HW_EN_USART4                    = VSF_HW_CLKRST_REGION(0x22, 18, 1),// M4USART4EN
    VSF_HW_EN_USART4LP                  = VSF_HW_CLKRST_REGION(0x22, 16, 1),// M4USART4LPEN
    VSF_HW_EN_UART9                     = VSF_HW_CLKRST_REGION(0x22, 14, 1),// M4UART9EN
    VSF_HW_EN_UART9LP                   = VSF_HW_CLKRST_REGION(0x22, 12, 1),// M4UART9LPEN
    VSF_HW_EN_UART10                    = VSF_HW_CLKRST_REGION(0x22, 10, 1),// M4UART10EN
    VSF_HW_EN_UART10LP                  = VSF_HW_CLKRST_REGION(0x22, 8,  1),// M4UART10LPEN
    VSF_HW_EN_UART11                    = VSF_HW_CLKRST_REGION(0x22, 6,  1),// M4UART11EN
    VSF_HW_EN_UART11LP                  = VSF_HW_CLKRST_REGION(0x22, 4,  1),// M4UART11LPEN
    VSF_HW_EN_UART12                    = VSF_HW_CLKRST_REGION(0x22, 2,  1),// M4UART12EN
    VSF_HW_EN_UART12LP                  = VSF_HW_CLKRST_REGION(0x22, 0,  1),// M4UART12LPEN
#elif   defined(CORE_CM7)
    VSF_HW_EN_USART1                    = VSF_HW_CLKRST_REGION(0x22, 31, 1),// M7USART1EN
    VSF_HW_EN_USART1LP                  = VSF_HW_CLKRST_REGION(0x22, 29, 1),// M7USART1LPEN
    VSF_HW_EN_USART2                    = VSF_HW_CLKRST_REGION(0x22, 27, 1),// M7USART2EN
    VSF_HW_EN_USART2LP                  = VSF_HW_CLKRST_REGION(0x22, 25, 1),// M7USART2LPEN
    VSF_HW_EN_USART3                    = VSF_HW_CLKRST_REGION(0x22, 23, 1),// M7USART3EN
    VSF_HW_EN_USART3LP                  = VSF_HW_CLKRST_REGION(0x22, 21, 1),// M7USART3LPEN
    VSF_HW_EN_USART4                    = VSF_HW_CLKRST_REGION(0x22, 19, 1),// M7USART4EN
    VSF_HW_EN_USART4LP                  = VSF_HW_CLKRST_REGION(0x22, 17, 1),// M7USART4LPEN
    VSF_HW_EN_UART9                     = VSF_HW_CLKRST_REGION(0x22, 15, 1),// M7UART9EN
    VSF_HW_EN_UART9LP                   = VSF_HW_CLKRST_REGION(0x22, 13, 1),// M7UART9LPEN
    VSF_HW_EN_UART10                    = VSF_HW_CLKRST_REGION(0x22, 11, 1),// M7UART10EN
    VSF_HW_EN_UART10LP                  = VSF_HW_CLKRST_REGION(0x22, 9,  1),// M7UART10LPEN
    VSF_HW_EN_UART11                    = VSF_HW_CLKRST_REGION(0x22, 7,  1),// M7UART11EN
    VSF_HW_EN_UART11LP                  = VSF_HW_CLKRST_REGION(0x22, 5,  1),// M7UART11LPEN
    VSF_HW_EN_UART12                    = VSF_HW_CLKRST_REGION(0x22, 3,  1),// M7UART12EN
    VSF_HW_EN_UART12LP                  = VSF_HW_CLKRST_REGION(0x22, 1,  1),// M7UART12LPEN
#endif
    // RCC.APB1EN4
#if     defined(CORE_CM4)
    VSF_HW_EN_I2S3                      = VSF_HW_CLKRST_REGION(0x23, 30, 1),// M4I2S3EN
    VSF_HW_EN_I2S3LP                    = VSF_HW_CLKRST_REGION(0x23, 28, 1),// M4I2S3LPEN
    VSF_HW_EN_I2S4                      = VSF_HW_CLKRST_REGION(0x23, 26, 1),// M4I2S4EN
    VSF_HW_EN_I2S4LP                    = VSF_HW_CLKRST_REGION(0x23, 24, 1),// M4I2S4LPEN
    VSF_HW_EN_I2C1                      = VSF_HW_CLKRST_REGION(0x23, 22, 1),// M4I2C1EN
    VSF_HW_EN_I2C1LP                    = VSF_HW_CLKRST_REGION(0x23, 20, 1),// M4I2C1LPEN
    VSF_HW_EN_I2C2                      = VSF_HW_CLKRST_REGION(0x23, 18, 1),// M4I2C2EN
    VSF_HW_EN_I2C2LP                    = VSF_HW_CLKRST_REGION(0x23, 16, 1),// M4I2C2LPEN
    VSF_HW_EN_I2C3                      = VSF_HW_CLKRST_REGION(0x23, 14, 1),// M4I2C3EN
    VSF_HW_EN_I2C3LP                    = VSF_HW_CLKRST_REGION(0x23, 12, 1),// M4I2C3LPEN
#elif   defined(CORE_CM7)
    VSF_HW_EN_I2S3                      = VSF_HW_CLKRST_REGION(0x23, 31, 1),// M7I2S3EN
    VSF_HW_EN_I2S3LP                    = VSF_HW_CLKRST_REGION(0x23, 29, 1),// M7I2S3LPEN
    VSF_HW_EN_I2S4                      = VSF_HW_CLKRST_REGION(0x23, 27, 1),// M7I2S4EN
    VSF_HW_EN_I2S4LP                    = VSF_HW_CLKRST_REGION(0x23, 25, 1),// M7I2S4LPEN
    VSF_HW_EN_I2C1                      = VSF_HW_CLKRST_REGION(0x23, 23, 1),// M7I2C1EN
    VSF_HW_EN_I2C1LP                    = VSF_HW_CLKRST_REGION(0x23, 21, 1),// M7I2C1LPEN
    VSF_HW_EN_I2C2                      = VSF_HW_CLKRST_REGION(0x23, 19, 1),// M7I2C2EN
    VSF_HW_EN_I2C2LP                    = VSF_HW_CLKRST_REGION(0x23, 17, 1),// M7I2C2LPEN
    VSF_HW_EN_I2C3                      = VSF_HW_CLKRST_REGION(0x23, 15, 1),// M7I2C3EN
    VSF_HW_EN_I2C3LP                    = VSF_HW_CLKRST_REGION(0x23, 13, 1),// M7I2C3LPEN
#endif
    // RCC.APB1EN5
#if     defined(CORE_CM4)
    VSF_HW_EN_FDCAN1                    = VSF_HW_CLKRST_REGION(0x24, 30, 1),// M4FDCAN1EN
    VSF_HW_EN_FDCAN1LP                  = VSF_HW_CLKRST_REGION(0x24, 28, 1),// M4FDCAN1LPEN
    VSF_HW_EN_FDCAN2                    = VSF_HW_CLKRST_REGION(0x24, 26, 1),// M4FDCAN2EN
    VSF_HW_EN_FDCAN2LP                  = VSF_HW_CLKRST_REGION(0x24, 24, 1),// M4FDCAN2LPEN
    VSF_HW_EN_FDCAN5                    = VSF_HW_CLKRST_REGION(0x24, 22, 1),// M4FDCAN5EN
    VSF_HW_EN_FDCAN5LP                  = VSF_HW_CLKRST_REGION(0x24, 20, 1),// M4FDCAN5LPEN
    VSF_HW_EN_FDCAN6                    = VSF_HW_CLKRST_REGION(0x24, 18, 1),// M4FDCAN6EN
    VSF_HW_EN_FDCAN6LP                  = VSF_HW_CLKRST_REGION(0x24, 16, 1),// M4FDCAN6LPEN
#elif   defined(CORE_CM7)
    VSF_HW_EN_FDCAN1                    = VSF_HW_CLKRST_REGION(0x24, 31, 1),// M7FDCAN1EN
    VSF_HW_EN_FDCAN1LP                  = VSF_HW_CLKRST_REGION(0x24, 29, 1),// M7FDCAN1LPEN
    VSF_HW_EN_FDCAN2                    = VSF_HW_CLKRST_REGION(0x24, 27, 1),// M7FDCAN2EN
    VSF_HW_EN_FDCAN2LP                  = VSF_HW_CLKRST_REGION(0x24, 25, 1),// M7FDCAN2LPEN
    VSF_HW_EN_FDCAN5                    = VSF_HW_CLKRST_REGION(0x24, 23, 1),// M7FDCAN5EN
    VSF_HW_EN_FDCAN5LP                  = VSF_HW_CLKRST_REGION(0x24, 21, 1),// M7FDCAN5LPEN
    VSF_HW_EN_FDCAN6                    = VSF_HW_CLKRST_REGION(0x24, 19, 1),// M7FDCAN6EN
    VSF_HW_EN_FDCAN6LP                  = VSF_HW_CLKRST_REGION(0x24, 17, 1),// M7FDCAN6LPEN
#endif

    // APB2
    // RCC.APB2EN1
#if     defined(CORE_CM4)
    VSF_HW_EN_ATIM1                     = VSF_HW_CLKRST_REGION(0x31, 30, 1),// M4ATIM1EN
    VSF_HW_EN_ATIM1LP                   = VSF_HW_CLKRST_REGION(0x31, 28, 1),// M4ATIM1LPEN
    VSF_HW_EN_ATIM2                     = VSF_HW_CLKRST_REGION(0x31, 26, 1),// M4ATIM2EN
    VSF_HW_EN_ATIM2LP                   = VSF_HW_CLKRST_REGION(0x31, 24, 1),// M4ATIM2LPEN
    VSF_HW_EN_GTIMA1                    = VSF_HW_CLKRST_REGION(0x31, 22, 1),// M4GTIMA1EN
    VSF_HW_EN_GTIMA1LP                  = VSF_HW_CLKRST_REGION(0x31, 20, 1),// M4GTIMA1LPEN
    VSF_HW_EN_GTIMA2                    = VSF_HW_CLKRST_REGION(0x31, 18, 1),// M4GTIMA2EN
    VSF_HW_EN_GTIMA2LP                  = VSF_HW_CLKRST_REGION(0x31, 16, 1),// M4GTIMA2LPEN
    VSF_HW_EN_GTIMA3                    = VSF_HW_CLKRST_REGION(0x31, 14, 1),// M4GTIMA3EN
    VSF_HW_EN_GTIMA3LP                  = VSF_HW_CLKRST_REGION(0x31, 12, 1),// M4GTIMA3LPEN
    VSF_HW_EN_SHRTIM1                   = VSF_HW_CLKRST_REGION(0x31, 10, 1),// M4SHRTIM1EN
    VSF_HW_EN_SHRTIM1LP                 = VSF_HW_CLKRST_REGION(0x31, 8,  1),// M4SHRTIM1LPEN
    VSF_HW_EN_SHRTIM2                   = VSF_HW_CLKRST_REGION(0x31, 6,  1),// M4SHRTIM2EN
    VSF_HW_EN_SHRTIM2LP                 = VSF_HW_CLKRST_REGION(0x31, 4,  1),// M4SHRTIM2LPEN
#elif   defined(CORE_CM7)
    VSF_HW_EN_ATIM1                     = VSF_HW_CLKRST_REGION(0x31, 31, 1),// M7ATIM1EN
    VSF_HW_EN_ATIM1LP                   = VSF_HW_CLKRST_REGION(0x31, 29, 1),// M7ATIM1LPEN
    VSF_HW_EN_ATIM2                     = VSF_HW_CLKRST_REGION(0x31, 27, 1),// M7ATIM2EN
    VSF_HW_EN_ATIM2LP                   = VSF_HW_CLKRST_REGION(0x31, 25, 1),// M7ATIM2LPEN
    VSF_HW_EN_GTIMA1                    = VSF_HW_CLKRST_REGION(0x31, 23, 1),// M7GTIMA1EN
    VSF_HW_EN_GTIMA1LP                  = VSF_HW_CLKRST_REGION(0x31, 21, 1),// M7GTIMA1LPEN
    VSF_HW_EN_GTIMA2                    = VSF_HW_CLKRST_REGION(0x31, 19, 1),// M7GTIMA2EN
    VSF_HW_EN_GTIMA2LP                  = VSF_HW_CLKRST_REGION(0x31, 17, 1),// M7GTIMA2LPEN
    VSF_HW_EN_GTIMA3                    = VSF_HW_CLKRST_REGION(0x31, 15, 1),// M7GTIMA3EN
    VSF_HW_EN_GTIMA3LP                  = VSF_HW_CLKRST_REGION(0x31, 13, 1),// M7GTIMA3LPEN
    VSF_HW_EN_SHRTIM1                   = VSF_HW_CLKRST_REGION(0x31, 11, 1),// M7SHRTIM1EN
    VSF_HW_EN_SHRTIM1LP                 = VSF_HW_CLKRST_REGION(0x31, 9,  1),// M7SHRTIM1LPEN
    VSF_HW_EN_SHRTIM2                   = VSF_HW_CLKRST_REGION(0x31, 7,  1),// M7SHRTIM2EN
    VSF_HW_EN_SHRTIM2LP                 = VSF_HW_CLKRST_REGION(0x31, 5,  1),// M7SHRTIM2LPEN
#endif
    // RCC.APB2EN2
#if     defined(CORE_CM4)
    VSF_HW_EN_I2S1                      = VSF_HW_CLKRST_REGION(0x32, 30, 1),// M4I2S1EN
    VSF_HW_EN_I2S1LP                    = VSF_HW_CLKRST_REGION(0x32, 28, 1),// M4I2S1LPEN
    VSF_HW_EN_I2S2                      = VSF_HW_CLKRST_REGION(0x32, 26, 1),// M4I2S2EN
    VSF_HW_EN_I2S2LP                    = VSF_HW_CLKRST_REGION(0x32, 24, 1),// M4I2S2LPEN
    VSF_HW_EN_SPI1                      = VSF_HW_CLKRST_REGION(0x32, 22, 1),// M4SPI1EN
    VSF_HW_EN_SPI1LP                    = VSF_HW_CLKRST_REGION(0x32, 20, 1),// M4SPI1LPEN
    VSF_HW_EN_SPI2                      = VSF_HW_CLKRST_REGION(0x32, 18, 1),// M4SPI2EN
    VSF_HW_EN_SPI2LP                    = VSF_HW_CLKRST_REGION(0x32, 16, 1),// M4SPI2LPEN
    VSF_HW_EN_DSMU                      = VSF_HW_CLKRST_REGION(0x32, 14, 1),// M4DSMUEN
    VSF_HW_EN_DSMULP                    = VSF_HW_CLKRST_REGION(0x32, 12, 1),// M4DSMULPEN
    VSF_HW_EN_I2C4                      = VSF_HW_CLKRST_REGION(0x32, 10, 1),// M4I2C4EN
    VSF_HW_EN_I2C4LP                    = VSF_HW_CLKRST_REGION(0x32, 8,  1),// M4I2C4LPEN
    VSF_HW_EN_I2C5                      = VSF_HW_CLKRST_REGION(0x32, 6,  1),// M4I2C5EN
    VSF_HW_EN_I2C5LP                    = VSF_HW_CLKRST_REGION(0x32, 4,  1),// M4I2C5LPEN
    VSF_HW_EN_I2C6                      = VSF_HW_CLKRST_REGION(0x32, 2,  1),// M4I2C6EN
    VSF_HW_EN_I2C6LP                    = VSF_HW_CLKRST_REGION(0x32, 0,  1),// M4I2C6LPEN
#elif   defined(CORE_CM7)
    VSF_HW_EN_I2S1                      = VSF_HW_CLKRST_REGION(0x32, 31, 1),// M7I2S1EN
    VSF_HW_EN_I2S1LP                    = VSF_HW_CLKRST_REGION(0x32, 29, 1),// M7I2S1LPEN
    VSF_HW_EN_I2S2                      = VSF_HW_CLKRST_REGION(0x32, 27, 1),// M7I2S2EN
    VSF_HW_EN_I2S2LP                    = VSF_HW_CLKRST_REGION(0x32, 25, 1),// M7I2S2LPEN
    VSF_HW_EN_SPI1                      = VSF_HW_CLKRST_REGION(0x32, 23, 1),// M7SPI1EN
    VSF_HW_EN_SPI1LP                    = VSF_HW_CLKRST_REGION(0x32, 21, 1),// M7SPI1LPEN
    VSF_HW_EN_SPI2                      = VSF_HW_CLKRST_REGION(0x32, 19, 1),// M7SPI2EN
    VSF_HW_EN_SPI2LP                    = VSF_HW_CLKRST_REGION(0x32, 17, 1),// M7SPI2LPEN
    VSF_HW_EN_DSMU                      = VSF_HW_CLKRST_REGION(0x32, 15, 1),// M7DSMUEN
    VSF_HW_EN_DSMULP                    = VSF_HW_CLKRST_REGION(0x32, 13, 1),// M7DSMULPEN
    VSF_HW_EN_I2C4                      = VSF_HW_CLKRST_REGION(0x32, 11, 1),// M7I2C4EN
    VSF_HW_EN_I2C4LP                    = VSF_HW_CLKRST_REGION(0x32, 9,  1),// M7I2C4LPEN
    VSF_HW_EN_I2C5                      = VSF_HW_CLKRST_REGION(0x32, 7,  1),// M7I2C5EN
    VSF_HW_EN_I2C5LP                    = VSF_HW_CLKRST_REGION(0x32, 5,  1),// M7I2C5LPEN
    VSF_HW_EN_I2C6                      = VSF_HW_CLKRST_REGION(0x32, 3,  1),// M7I2C6EN
    VSF_HW_EN_I2C6LP                    = VSF_HW_CLKRST_REGION(0x32, 1,  1),// M7I2C6LPEN
#endif
    // RCC.APB2EN3
#if     defined(CORE_CM4)
    VSF_HW_EN_USART5                    = VSF_HW_CLKRST_REGION(0x33, 30, 1),// M4USART5EN
    VSF_HW_EN_USART5LP                  = VSF_HW_CLKRST_REGION(0x33, 28, 1),// M4USART5LPEN
    VSF_HW_EN_USART6                    = VSF_HW_CLKRST_REGION(0x33, 26, 1),// M4USART6EN
    VSF_HW_EN_USART6LP                  = VSF_HW_CLKRST_REGION(0x33, 24, 1),// M4USART6LPEN
    VSF_HW_EN_USART7                    = VSF_HW_CLKRST_REGION(0x33, 22, 1),// M4USART7EN
    VSF_HW_EN_USART7LP                  = VSF_HW_CLKRST_REGION(0x33, 20, 1),// M4USART7LPEN
    VSF_HW_EN_USART8                    = VSF_HW_CLKRST_REGION(0x33, 18, 1),// M4USART8EN
    VSF_HW_EN_USART8LP                  = VSF_HW_CLKRST_REGION(0x33, 16, 1),// M4USART8LPEN
    VSF_HW_EN_UART13                    = VSF_HW_CLKRST_REGION(0x33, 14, 1),// M4UART13EN
    VSF_HW_EN_UART13LP                  = VSF_HW_CLKRST_REGION(0x33, 12, 1),// M4UART13LPEN
    VSF_HW_EN_UART14                    = VSF_HW_CLKRST_REGION(0x33, 10, 1),// M4UART14EN
    VSF_HW_EN_UART14LP                  = VSF_HW_CLKRST_REGION(0x33, 8,  1),// M4UART14LPEN
    VSF_HW_EN_UART15                    = VSF_HW_CLKRST_REGION(0x33, 6,  1),// M4UART15EN
    VSF_HW_EN_UART15LP                  = VSF_HW_CLKRST_REGION(0x33, 4,  1),// M4UART15LPEN
#elif   defined(CORE_CM7)
    VSF_HW_EN_USART5                    = VSF_HW_CLKRST_REGION(0x33, 31, 1),// M7USART5EN
    VSF_HW_EN_USART5LP                  = VSF_HW_CLKRST_REGION(0x33, 29, 1),// M7USART5LPEN
    VSF_HW_EN_USART6                    = VSF_HW_CLKRST_REGION(0x33, 27, 1),// M7USART6EN
    VSF_HW_EN_USART6LP                  = VSF_HW_CLKRST_REGION(0x33, 25, 1),// M7USART6LPEN
    VSF_HW_EN_USART7                    = VSF_HW_CLKRST_REGION(0x33, 23, 1),// M7USART7EN
    VSF_HW_EN_USART7LP                  = VSF_HW_CLKRST_REGION(0x33, 21, 1),// M7USART7LPEN
    VSF_HW_EN_USART8                    = VSF_HW_CLKRST_REGION(0x33, 19, 1),// M7USART8EN
    VSF_HW_EN_USART8LP                  = VSF_HW_CLKRST_REGION(0x33, 17, 1),// M7USART8LPEN
    VSF_HW_EN_UART13                    = VSF_HW_CLKRST_REGION(0x33, 15, 1),// M7UART13EN
    VSF_HW_EN_UART13LP                  = VSF_HW_CLKRST_REGION(0x33, 13, 1),// M7UART13LPEN
    VSF_HW_EN_UART14                    = VSF_HW_CLKRST_REGION(0x33, 11, 1),// M7UART14EN
    VSF_HW_EN_UART14LP                  = VSF_HW_CLKRST_REGION(0x33, 9,  1),// M7UART14LPEN
    VSF_HW_EN_UART15                    = VSF_HW_CLKRST_REGION(0x33, 7,  1),// M7UART15EN
    VSF_HW_EN_UART15LP                  = VSF_HW_CLKRST_REGION(0x33, 5,  1),// M7UART15LPEN
#endif
    // RCC.APB2EN4
#if     defined(CORE_CM4)
    VSF_HW_EN_FDCAN3                    = VSF_HW_CLKRST_REGION(0x34, 30, 1),// M4FDCAN3EN
    VSF_HW_EN_FDCAN3LP                  = VSF_HW_CLKRST_REGION(0x34, 28, 1),// M4FDCAN3LPEN
    VSF_HW_EN_FDCAN4                    = VSF_HW_CLKRST_REGION(0x34, 26, 1),// M4FDCAN4EN
    VSF_HW_EN_FDCAN4LP                  = VSF_HW_CLKRST_REGION(0x34, 24, 1),// M4FDCAN4LPEN
    VSF_HW_EN_FDCAN7                    = VSF_HW_CLKRST_REGION(0x34, 22, 1),// M4FDCAN7EN
    VSF_HW_EN_FDCAN7LP                  = VSF_HW_CLKRST_REGION(0x34, 20, 1),// M4FDCAN7LPEN
    VSF_HW_EN_FDCAN8                    = VSF_HW_CLKRST_REGION(0x34, 18, 1),// M4FDCAN8EN
    VSF_HW_EN_FDCAN8LP                  = VSF_HW_CLKRST_REGION(0x34, 16, 1),// M4FDCAN8LPEN
#elif   defined(CORE_CM7)
    VSF_HW_EN_FDCAN3                    = VSF_HW_CLKRST_REGION(0x34, 31, 1),// M7FDCAN3EN
    VSF_HW_EN_FDCAN3LP                  = VSF_HW_CLKRST_REGION(0x34, 29, 1),// M7FDCAN3LPEN
    VSF_HW_EN_FDCAN4                    = VSF_HW_CLKRST_REGION(0x34, 27, 1),// M7FDCAN4EN
    VSF_HW_EN_FDCAN4LP                  = VSF_HW_CLKRST_REGION(0x34, 25, 1),// M7FDCAN4LPEN
    VSF_HW_EN_FDCAN7                    = VSF_HW_CLKRST_REGION(0x34, 23, 1),// M7FDCAN7EN
    VSF_HW_EN_FDCAN7LP                  = VSF_HW_CLKRST_REGION(0x34, 21, 1),// M7FDCAN7LPEN
    VSF_HW_EN_FDCAN8                    = VSF_HW_CLKRST_REGION(0x34, 19, 1),// M7FDCAN8EN
    VSF_HW_EN_FDCAN8LP                  = VSF_HW_CLKRST_REGION(0x34, 17, 1),// M7FDCAN8LPEN
#endif

    // APB5
    // RCC.APB5EN1
#if     defined(CORE_CM4)
    VSF_HW_EN_ATIM3                     = VSF_HW_CLKRST_REGION(0x3F, 30, 1),// M4ATIM3EN
    VSF_HW_EN_ATIM3LP                   = VSF_HW_CLKRST_REGION(0x3F, 28, 1),// M4ATIM3LPEN
    VSF_HW_EN_ATIM4                     = VSF_HW_CLKRST_REGION(0x3F, 26, 1),// M4ATIM4EN
    VSF_HW_EN_ATIM4LP                   = VSF_HW_CLKRST_REGION(0x3F, 24, 1),// M4ATIM4LPEN
    VSF_HW_EN_AFEC                      = VSF_HW_CLKRST_REGION(0x3F, 22, 1),// M4AFECEN
    VSF_HW_EN_AFECLP                    = VSF_HW_CLKRST_REGION(0x3F, 20, 1),// M4AFECLPEN
    VSF_HW_EN_SPI4                      = VSF_HW_CLKRST_REGION(0x3F, 14, 1),// M4SPI4EN
    VSF_HW_EN_SPI4LP                    = VSF_HW_CLKRST_REGION(0x3F, 12, 1),// M4SPI4LPEN
    VSF_HW_EN_SPI5                      = VSF_HW_CLKRST_REGION(0x3F, 10, 1),// M4SPI5EN
    VSF_HW_EN_SPI5LP                    = VSF_HW_CLKRST_REGION(0x3F, 8,  1),// M4SPI5LPEN
    VSF_HW_EN_SPI6                      = VSF_HW_CLKRST_REGION(0x3F, 6,  1),// M4SPI6EN
    VSF_HW_EN_SPI6LP                    = VSF_HW_CLKRST_REGION(0x3F, 4,  1),// M4SPI6LPEN
    VSF_HW_EN_SPI7                      = VSF_HW_CLKRST_REGION(0x3F, 2,  1),// M4SPI7EN
    VSF_HW_EN_SPI7LP                    = VSF_HW_CLKRST_REGION(0x3F, 0,  1),// M4SPI7LPEN
#elif   defined(CORE_CM7)
    VSF_HW_EN_ATIM3                     = VSF_HW_CLKRST_REGION(0x3F, 30, 1),// M7ATIM3EN
    VSF_HW_EN_ATIM3LP                   = VSF_HW_CLKRST_REGION(0x3F, 28, 1),// M7ATIM3LPEN
    VSF_HW_EN_ATIM4                     = VSF_HW_CLKRST_REGION(0x3F, 26, 1),// M7ATIM4EN
    VSF_HW_EN_ATIM4LP                   = VSF_HW_CLKRST_REGION(0x3F, 24, 1),// M7ATIM4LPEN
    VSF_HW_EN_AFEC                      = VSF_HW_CLKRST_REGION(0x3F, 22, 1),// M7AFECEN
    VSF_HW_EN_AFECLP                    = VSF_HW_CLKRST_REGION(0x3F, 20, 1),// M7AFECLPEN
    VSF_HW_EN_SPI4                      = VSF_HW_CLKRST_REGION(0x3F, 14, 1),// M7SPI4EN
    VSF_HW_EN_SPI4LP                    = VSF_HW_CLKRST_REGION(0x3F, 12, 1),// M7SPI4LPEN
    VSF_HW_EN_SPI5                      = VSF_HW_CLKRST_REGION(0x3F, 10, 1),// M7SPI5EN
    VSF_HW_EN_SPI5LP                    = VSF_HW_CLKRST_REGION(0x3F, 8,  1),// M7SPI5LPEN
    VSF_HW_EN_SPI6                      = VSF_HW_CLKRST_REGION(0x3F, 6,  1),// M7SPI6EN
    VSF_HW_EN_SPI6LP                    = VSF_HW_CLKRST_REGION(0x3F, 4,  1),// M7SPI6LPEN
    VSF_HW_EN_SPI7                      = VSF_HW_CLKRST_REGION(0x3F, 2,  1),// M7SPI7EN
    VSF_HW_EN_SPI7LP                    = VSF_HW_CLKRST_REGION(0x3F, 0,  1),// M7SPI7LPEN
#endif
    // RCC.APB5EN2
#if     defined(CORE_CM4)
    VSF_HW_EN_I2C7                      = VSF_HW_CLKRST_REGION(0x40, 30, 1),// M4I2C7EN
    VSF_HW_EN_I2C7LP                    = VSF_HW_CLKRST_REGION(0x40, 28, 1),// M4I2C7LPEN
    VSF_HW_EN_I2C8                      = VSF_HW_CLKRST_REGION(0x40, 26, 1),// M4I2C8EN
    VSF_HW_EN_I2C8LP                    = VSF_HW_CLKRST_REGION(0x40, 24, 1),// M4I2C8LPEN
    VSF_HW_EN_I2C9                      = VSF_HW_CLKRST_REGION(0x40, 22, 1),// M4I2C9EN
    VSF_HW_EN_I2C9LP                    = VSF_HW_CLKRST_REGION(0x40, 20, 1),// M4I2C9LPEN
    VSF_HW_EN_I2C10                     = VSF_HW_CLKRST_REGION(0x40, 18, 1),// M4I2C10EN
    VSF_HW_EN_I2C10LP                   = VSF_HW_CLKRST_REGION(0x40, 16, 1),// M4I2C10LPEN

    VSF_HW_EN_RTCPCLK                   = VSF_HW_CLKRST_REGION(0x40, 10, 1),// M4RTCPCLKEN
    VSF_HW_EN_RTCPCLKLP                 = VSF_HW_CLKRST_REGION(0x40, 8,  1),// M4RTCPCLKLPEN
#elif   defined(CORE_CM7)
    VSF_HW_EN_I2C7                      = VSF_HW_CLKRST_REGION(0x40, 31, 1),// M7I2C7EN
    VSF_HW_EN_I2C7LP                    = VSF_HW_CLKRST_REGION(0x40, 29, 1),// M7I2C7LPEN
    VSF_HW_EN_I2C8                      = VSF_HW_CLKRST_REGION(0x40, 27, 1),// M7I2C8EN
    VSF_HW_EN_I2C8LP                    = VSF_HW_CLKRST_REGION(0x40, 25, 1),// M7I2C8LPEN
    VSF_HW_EN_I2C9                      = VSF_HW_CLKRST_REGION(0x40, 23, 1),// M7I2C9EN
    VSF_HW_EN_I2C9LP                    = VSF_HW_CLKRST_REGION(0x40, 21, 1),// M7I2C9LPEN
    VSF_HW_EN_I2C10                     = VSF_HW_CLKRST_REGION(0x40, 19, 1),// M7I2C10EN
    VSF_HW_EN_I2C10LP                   = VSF_HW_CLKRST_REGION(0x40, 17, 1),// M7I2C10LPEN

    VSF_HW_EN_EXTI                      = VSF_HW_CLKRST_REGION(0x40, 15, 1),// EXTIEN
    VSF_HW_EN_EXTILP                    = VSF_HW_CLKRST_REGION(0x40, 13, 1),// EXTILPEN
    VSF_HW_EN_IWDG1PCLK                 = VSF_HW_CLKRST_REGION(0x40, 7,  1),// IWDG1PCLKEN
    VSF_HW_EN_IWDG1PCLKLP               = VSF_HW_CLKRST_REGION(0x40, 5,  1),// IWDG1PCLKLPEN
    VSF_HW_EN_IWDG2PCLK                 = VSF_HW_CLKRST_REGION(0x40, 3,  1),// IWDG2PCLKEN
    VSF_HW_EN_IWDG2PCLKLP               = VSF_HW_CLKRST_REGION(0x40, 1,  1),// IWDG2PCLKLPEN

    VSF_HW_EN_RTCPCLK                   = VSF_HW_CLKRST_REGION(0x40, 11, 1),// M7RTCPCLKEN
    VSF_HW_EN_RTCPCLKLP                 = VSF_HW_CLKRST_REGION(0x40, 9,  1),// M7RTCPCLKLPEN
#endif

    // Retention domain
    // RCC.RDEN1
#if     defined(CORE_CM4)
    VSF_HW_EN_LPTIM1                    = VSF_HW_CLKRST_REGION(0x45, 30, 1),// M4LPTIM1EN
    VSF_HW_EN_LPTIM1LP                  = VSF_HW_CLKRST_REGION(0x45, 28, 1),// M4LPTIM1LPEN
    VSF_HW_EN_LPTIM2                    = VSF_HW_CLKRST_REGION(0x45, 26, 1),// M4LPTIM2EN
    VSF_HW_EN_LPTIM2LP                  = VSF_HW_CLKRST_REGION(0x45, 24, 1),// M4LPTIM2LPEN
    VSF_HW_EN_LPTIM3                    = VSF_HW_CLKRST_REGION(0x45, 22, 1),// M4LPTIM3EN
    VSF_HW_EN_LPTIM3LP                  = VSF_HW_CLKRST_REGION(0x45, 20, 1),// M4LPTIM3LPEN
    VSF_HW_EN_LPTIM4                    = VSF_HW_CLKRST_REGION(0x45, 18, 1),// M4LPTIM4EN
    VSF_HW_EN_LPTIM4LP                  = VSF_HW_CLKRST_REGION(0x45, 16, 1),// M4LPTIM4LPEN
    VSF_HW_EN_LPTIM5                    = VSF_HW_CLKRST_REGION(0x45, 14, 1),// M4LPTIM5EN
    VSF_HW_EN_LPTIM5LP                  = VSF_HW_CLKRST_REGION(0x45, 12, 1),// M4LPTIM5LPEN
    VSF_HW_EN_LPUART1                   = VSF_HW_CLKRST_REGION(0x45, 10, 1),// M4LPUART1EN
    VSF_HW_EN_LPUART1LP                 = VSF_HW_CLKRST_REGION(0x45, 8,  1),// M4LPUART1LPEN
    VSF_HW_EN_LPUART2                   = VSF_HW_CLKRST_REGION(0x45, 6,  1),// M4LPUART2EN
    VSF_HW_EN_LPUART2LP                 = VSF_HW_CLKRST_REGION(0x45, 4,  1),// M4LPUART2LPEN
#elif   defined(CORE_CM7)
    VSF_HW_EN_LPTIM1                    = VSF_HW_CLKRST_REGION(0x45, 31, 1),// M7LPTIM1EN
    VSF_HW_EN_LPTIM1LP                  = VSF_HW_CLKRST_REGION(0x45, 29, 1),// M7LPTIM1LPEN
    VSF_HW_EN_LPTIM2                    = VSF_HW_CLKRST_REGION(0x45, 27, 1),// M7LPTIM2EN
    VSF_HW_EN_LPTIM2LP                  = VSF_HW_CLKRST_REGION(0x45, 25, 1),// M7LPTIM2LPEN
    VSF_HW_EN_LPTIM3                    = VSF_HW_CLKRST_REGION(0x45, 23, 1),// M7LPTIM3EN
    VSF_HW_EN_LPTIM3LP                  = VSF_HW_CLKRST_REGION(0x45, 21, 1),// M7LPTIM3LPEN
    VSF_HW_EN_LPTIM4                    = VSF_HW_CLKRST_REGION(0x45, 19, 1),// M7LPTIM4EN
    VSF_HW_EN_LPTIM4LP                  = VSF_HW_CLKRST_REGION(0x45, 17, 1),// M7LPTIM4LPEN
    VSF_HW_EN_LPTIM5                    = VSF_HW_CLKRST_REGION(0x45, 15, 1),// M7LPTIM5EN
    VSF_HW_EN_LPTIM5LP                  = VSF_HW_CLKRST_REGION(0x45, 13, 1),// M7LPTIM5LPEN
    VSF_HW_EN_LPUART1                   = VSF_HW_CLKRST_REGION(0x45, 11, 1),// M7LPUART1EN
    VSF_HW_EN_LPUART1LP                 = VSF_HW_CLKRST_REGION(0x45, 9,  1),// M7LPUART1LPEN
    VSF_HW_EN_LPUART2                   = VSF_HW_CLKRST_REGION(0x45, 7,  1),// M7LPUART2EN
    VSF_HW_EN_LPUART2LP                 = VSF_HW_CLKRST_REGION(0x45, 5,  1),// M7LPUART2LPEN
#endif
    // RCC.RDEN1
#if     defined(CORE_CM4)
    VSF_HW_EN_COMP                     = VSF_HW_CLKRST_REGION(0x46, 30, 1),// M4COMPEN
    VSF_HW_EN_COMPLP                   = VSF_HW_CLKRST_REGION(0x46, 28, 1),// M4COMPLPEN
#elif   defined(CORE_CM7)
    VSF_HW_EN_COMP                     = VSF_HW_CLKRST_REGION(0x46, 31, 1),// M7COMPEN
    VSF_HW_EN_COMPLP                   = VSF_HW_CLKRST_REGION(0x46, 29, 1),// M7COMPLPEN
#endif
} vsf_hw_peripheral_en_t;

typedef struct vsf_hw_clk_t vsf_hw_clk_t;
typedef struct vsf_hw_pwr_domain_t vsf_hw_pwr_domain_t;
typedef struct vsf_hw_pwr_t vsf_hw_pwr_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_hw_clk_t VSF_HW_CLK_HSE;
    extern const vsf_hw_clk_t VSF_HW_CLK_HSE_CG;
    extern const vsf_hw_clk_t VSF_HW_CLK_HSE_KER_CG;
extern const vsf_hw_clk_t VSF_HW_CLK_LSE;
extern const vsf_hw_clk_t VSF_HW_CLK_HSI;
    extern const vsf_hw_clk_t VSF_HW_CLK_HSI_CG;
    extern const vsf_hw_clk_t VSF_HW_CLK_HSI_KER_CG;
extern const vsf_hw_clk_t VSF_HW_CLK_MSI;
    extern const vsf_hw_clk_t VSF_HW_CLK_MSI_CG;
    extern const vsf_hw_clk_t VSF_HW_CLK_MSI_KER_CG;
extern const vsf_hw_clk_t VSF_HW_CLK_LSI;

extern const vsf_hw_clk_t VSF_HW_CLK_PLL1;
    extern const vsf_hw_clk_t VSF_HW_CLK_PLL1A;
    extern const vsf_hw_clk_t VSF_HW_CLK_PLL1B;
    extern const vsf_hw_clk_t VSF_HW_CLK_PLL1C;
extern const vsf_hw_clk_t VSF_HW_CLK_PLL2;
    extern const vsf_hw_clk_t VSF_HW_CLK_PLL2A;
    extern const vsf_hw_clk_t VSF_HW_CLK_PLL2B;
    extern const vsf_hw_clk_t VSF_HW_CLK_PLL2C;
extern const vsf_hw_clk_t VSF_HW_CLK_PLL3;
    extern const vsf_hw_clk_t VSF_HW_CLK_PLL3A;
    extern const vsf_hw_clk_t VSF_HW_CLK_PLL3B;
    extern const vsf_hw_clk_t VSF_HW_CLK_PLL3C;
extern const vsf_hw_clk_t VSF_HW_CLK_SHRPLL;

extern const vsf_hw_clk_t VSF_HW_CLK_AXISYS;
extern const vsf_hw_clk_t VSF_HW_CLK_AXIHYP;

extern const vsf_hw_clk_t VSF_HW_CLK_SYS;
extern const vsf_hw_clk_t VSF_HW_CLK_SYSBUS;
extern const vsf_hw_clk_t VSF_HW_CLK_CPU;
extern const vsf_hw_clk_t VSF_HW_CLK_SYSTICK;
extern const vsf_hw_clk_t VSF_HW_CLK_AXI;
#define VSF_HW_CLK_AHB1                 VSF_HW_CLK_SYSBUS
#define VSF_HW_CLK_AHB2                 VSF_HW_CLK_SYSBUS
#define VSF_HW_CLK_AHB5                 VSF_HW_CLK_SYSBUS
#define VSF_HW_CLK_AHB6                 VSF_HW_CLK_AXI
#define VSF_HW_CLK_AHB9                 VSF_HW_CLK_SYSBUS
extern const vsf_hw_clk_t VSF_HW_CLK_APB1;
extern const vsf_hw_clk_t VSF_HW_CLK_APB2;
extern const vsf_hw_clk_t VSF_HW_CLK_APB5;
extern const vsf_hw_clk_t VSF_HW_CLK_APB6;
extern const vsf_hw_clk_t VSF_HW_CLK_PERI;

extern const vsf_hw_clk_t VSF_HW_CLK_SDRAM;

#if VSF_HAL_USE_SDIO == ENABLED
#   define VSF_HW_CLK_SDMMC1_BUS        VSF_HW_CLK_AHB6
extern const vsf_hw_clk_t VSF_HW_CLK_SDMMC1;

#   define VSF_HW_CLK_SDMMC2_BUS        VSF_HW_CLK_AHB1
extern const vsf_hw_clk_t VSF_HW_CLK_SDMMC2;
#endif

#if VSF_HAL_USE_USART == ENABLED
extern const vsf_hw_clk_t VSF_HW_CLK_USART1_2;
#   define VSF_HW_CLK_USART3_4          VSF_HW_CLK_APB1
#   define VSF_HW_CLK_USART5_6_7_8      VSF_HW_CLK_APB2
#   define VSF_HW_CLK_UART9_10_11_12    VSF_HW_CLK_APB1
#   define VSF_HW_CLK_UART13_14_15      VSF_HW_CLK_APB2

#   define VSF_HW_CLK_USART1            VSF_HW_CLK_USART1_2
#   define VSF_HW_CLK_USART2            VSF_HW_CLK_USART1_2
#   define VSF_HW_CLK_USART3            VSF_HW_CLK_USART3_4
#   define VSF_HW_CLK_USART4            VSF_HW_CLK_USART3_4
#   define VSF_HW_CLK_USART5            VSF_HW_CLK_USART5_6_7_8
#   define VSF_HW_CLK_USART6            VSF_HW_CLK_USART5_6_7_8
#   define VSF_HW_CLK_USART7            VSF_HW_CLK_USART5_6_7_8
#   define VSF_HW_CLK_USART8            VSF_HW_CLK_USART5_6_7_8
#   define VSF_HW_CLK_UART9             VSF_HW_CLK_UART9_10_11_12
#   define VSF_HW_CLK_UART10            VSF_HW_CLK_UART9_10_11_12
#   define VSF_HW_CLK_UART11            VSF_HW_CLK_UART9_10_11_12
#   define VSF_HW_CLK_UART12            VSF_HW_CLK_UART9_10_11_12
#   define VSF_HW_CLK_UART13            VSF_HW_CLK_UART13_14_15
#   define VSF_HW_CLK_UART14            VSF_HW_CLK_UART13_14_15
#   define VSF_HW_CLK_UART15            VSF_HW_CLK_UART13_14_15
#endif

#if VSF_HAL_USE_SPI == ENABLED
#   define VSF_HW_CLK_SPI1_2            VSF_HW_CLK_APB2
#   define VSF_HW_CLK_SPI3              VSF_HW_CLK_APB1
#   define VSF_HW_CLK_SPI4_5_6_7        VSF_HW_CLK_APB5
#endif

#if VSF_HAL_USE_USBD == ENABLED || VSF_HAL_USE_USBH == ENABLED
extern const vsf_hw_clk_t VSF_HW_CLK_USBREF;
#endif

// power

extern const vsf_hw_pwr_domain_t VSF_HW_PWR_DOMAIN_HCS1;
extern const vsf_hw_pwr_domain_t VSF_HW_PWR_DOMAIN_HCS2;
extern const vsf_hw_pwr_domain_t VSF_HW_PWR_DOMAIN_GRC;
extern const vsf_hw_pwr_domain_t VSF_HW_PWR_DOMAIN_ESC;
extern const vsf_hw_pwr_domain_t VSF_HW_PWR_DOMAIN_MDMA;
extern const vsf_hw_pwr_domain_t VSF_HW_PWR_DOMAIN_SHRA;
extern const vsf_hw_pwr_domain_t VSF_HW_PWR_DOMAIN_SHR2;
extern const vsf_hw_pwr_domain_t VSF_HW_PWR_DOMAIN_SHR1;

extern const vsf_hw_pwr_t VSF_HW_PWR_GPU;
extern const vsf_hw_pwr_t VSF_HW_PWR_LCDC;
extern const vsf_hw_pwr_t VSF_HW_PWR_JPEG;
extern const vsf_hw_pwr_t VSF_HW_PWR_DSI;
extern const vsf_hw_pwr_t VSF_HW_PWR_DVP;
extern const vsf_hw_pwr_t VSF_HW_PWR_ETH2;
extern const vsf_hw_pwr_t VSF_HW_PWR_USB2;
extern const vsf_hw_pwr_t VSF_HW_PWR_SDMMC2;
extern const vsf_hw_pwr_t VSF_HW_PWR_ETH1;
extern const vsf_hw_pwr_t VSF_HW_PWR_USB1;
extern const vsf_hw_pwr_t VSF_HW_PWR_SDMMC1;
extern const vsf_hw_pwr_t VSF_HW_PWR_FMAC;
extern const vsf_hw_pwr_t VSF_HW_PWR_ESC;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_hw_clkrst_region_set(uint32_t region, uint_fast8_t value);
extern uint_fast8_t vsf_hw_clkrst_region_get(uint32_t region);

extern void vsf_hw_clkrst_region_set_bit(uint32_t region);
extern void vsf_hw_clkrst_region_clear_bit(uint32_t region);
extern uint_fast8_t vsf_hw_clkrst_region_get_bit(uint32_t region);

extern const vsf_hw_clk_t * vsf_hw_clk_get_src(const vsf_hw_clk_t *clk);
extern uint32_t vsf_hw_clk_get_freq_hz(const vsf_hw_clk_t *clk);
extern void vsf_hw_clk_enable(const vsf_hw_clk_t *clk);
extern void vsf_hw_clk_disable(const vsf_hw_clk_t *clk);
extern bool vsf_hw_clk_is_enabled(const vsf_hw_clk_t *clk);
extern bool vsf_hw_clk_is_ready(const vsf_hw_clk_t *clk);
extern vsf_err_t vsf_hw_clk_config(const vsf_hw_clk_t *clk, const vsf_hw_clk_t *clksrc, uint16_t prescaler, uint32_t freq_hz);

/**
 \~english
 @brief configure frequency range of pll input/output clocks
 @param[in] clk: a pointer to PLL clock
                VSF_HW_CLK_PLL1
                VSF_HW_CLK_PLL2
                VSF_HW_CLK_PLL3
 @param[in] out_freq_hz: PLL output frequency in Hz

 \~chinese
 @brief 配置 PLL 输入输出时钟的频率范围
 @param[in] clk: 指向 PLL 的时钟
                VSF_HW_CLK_PLL1
                VSF_HW_CLK_PLL2
                VSF_HW_CLK_PLL3
 @param[in] out_freq_hz: PLL 输出时钟频率，单位 Hz
 */
extern vsf_err_t vsf_hw_pll_config(const vsf_hw_clk_t *clk, uint32_t out_freq_hz);

// power

extern void vsf_hw_power_domain_enable(const vsf_hw_pwr_domain_t *domain);
extern void vsf_hw_power_domain_disable(const vsf_hw_pwr_domain_t *domain);
extern bool vsf_hw_power_domain_is_ready(const vsf_hw_pwr_domain_t *domain);

extern void vsf_hw_power_enable(const vsf_hw_pwr_t *pwr);
extern void vsf_hw_power_disable(const vsf_hw_pwr_t *pwr);

#endif
/* EOF */
