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

#ifndef __HAL_DRIVER_ARTERY_AT32F402_405_COMMON_H__
#define __HAL_DRIVER_ARTERY_AT32F402_405_COMMON_H__

/* \note __common.h should only be included by device.h */

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"
#include "hal/arch/vsf_arch.h"

// CMSIS headers which will not be included in core_xxx.h
//  and arm_math.h CAN ONLY be included after core_xxx.h
//#include "arm_math.h"

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

#define VSF_SYSTIMER_FREQ               vsf_hw_clk_get_freq_hz(&VSF_HW_CLK_CPU)

/*============================ TYPES =========================================*/

typedef enum vsf_hw_peripheral_rst_t {
    // CRM_AHBRST1
    VSF_HW_RST_OTGHS                    = VSF_HW_CLKRST_REGION(0x4,  29, 1),// OTGHSRST
    VSF_HW_RST_DMA2                     = VSF_HW_CLKRST_REGION(0x4,  24, 1),// DMA2RST
    VSF_HW_RST_DMA1                     = VSF_HW_CLKRST_REGION(0x4,  22, 1),// DMA1RST
    VSF_HW_RST_CRC                      = VSF_HW_CLKRST_REGION(0x4,  12, 1),// CRCRST
    VSF_HW_RST_GPIO5                    = VSF_HW_CLKRST_REGION(0x4,  5,  1),// GPIOFRST
    VSF_HW_RST_GPIO3                    = VSF_HW_CLKRST_REGION(0x4,  3,  1),// GPIODRST
    VSF_HW_RST_GPIO2                    = VSF_HW_CLKRST_REGION(0x4,  2,  1),// GPIOCRST
    VSF_HW_RST_GPIO1                    = VSF_HW_CLKRST_REGION(0x4,  1,  1),// GPIOBRST
    VSF_HW_RST_GPIO0                    = VSF_HW_CLKRST_REGION(0x4,  0,  1),// GPIOARST

    VSF_HW_RST_GPIOA                    = VSF_HW_RST_GPIO0,
    VSF_HW_RST_GPIOB                    = VSF_HW_RST_GPIO1,
    VSF_HW_RST_GPIOC                    = VSF_HW_RST_GPIO2,
    VSF_HW_RST_GPIOD                    = VSF_HW_RST_GPIO3,
    VSF_HW_RST_GPIOF                    = VSF_HW_RST_GPIO5,

    // CRM_AHBRST2
    VSF_HW_RST_OTGFS1                   = VSF_HW_CLKRST_REGION(0x5,  7,  1),// OTGFS1RST

    // CRM_AHBRST3
    VSF_HW_RST_QSPI1                    = VSF_HW_CLKRST_REGION(0x6,  1,  1),// QSPI1RST

    // CRM_APB1RST
    VSF_HW_RST_UART8                    = VSF_HW_CLKRST_REGION(0x7,  31,  1),// UART8RST
    VSF_HW_RST_UART7                    = VSF_HW_CLKRST_REGION(0x7,  30,  1),// UART7RST
    VSF_HW_RST_PWC                      = VSF_HW_CLKRST_REGION(0x7,  28,  1),// PWCRST
    VSF_HW_RST_CAN1                     = VSF_HW_CLKRST_REGION(0x7,  25,  1),// CAN1RST
    VSF_HW_RST_I2C3                     = VSF_HW_CLKRST_REGION(0x7,  23,  1),// I2C3RST
    VSF_HW_RST_I2C2                     = VSF_HW_CLKRST_REGION(0x7,  22,  1),// I2C2RST
    VSF_HW_RST_I2C1                     = VSF_HW_CLKRST_REGION(0x7,  21,  1),// I2C1RST
    VSF_HW_RST_USART5                   = VSF_HW_CLKRST_REGION(0x7,  20,  1),// USART5RST
    VSF_HW_RST_USART4                   = VSF_HW_CLKRST_REGION(0x7,  19,  1),// USART4RST
    VSF_HW_RST_USART3                   = VSF_HW_CLKRST_REGION(0x7,  18,  1),// USART3RST
    VSF_HW_RST_USART2                   = VSF_HW_CLKRST_REGION(0x7,  17,  1),// USART2RST
    VSF_HW_RST_SPI3                     = VSF_HW_CLKRST_REGION(0x7,  15,  1),// SPI3RST
    VSF_HW_RST_SPI2                     = VSF_HW_CLKRST_REGION(0x7,  14,  1),// SPI2RST
    VSF_HW_RST_WWDT                     = VSF_HW_CLKRST_REGION(0x7,  11,  1),// WWDTRST
    VSF_HW_RST_TMR14                    = VSF_HW_CLKRST_REGION(0x7,   8,  1),// TMR14RST
    VSF_HW_RST_TMR13                    = VSF_HW_CLKRST_REGION(0x7,   7,  1),// TMR13RST
    VSF_HW_RST_TMR7                     = VSF_HW_CLKRST_REGION(0x7,   5,  1),// TMR7RST
    VSF_HW_RST_TMR6                     = VSF_HW_CLKRST_REGION(0x7,   4,  1),// TMR6RST
    VSF_HW_RST_TMR4                     = VSF_HW_CLKRST_REGION(0x7,   2,  1),// TMR4RST
    VSF_HW_RST_TMR3                     = VSF_HW_CLKRST_REGION(0x7,   1,  1),// TMR3RST
    VSF_HW_RST_TMR2                     = VSF_HW_CLKRST_REGION(0x7,   0,  1),// TMR2RST

    // CRM_APB2RST
    VSF_HW_RST_ACC                      = VSF_HW_CLKRST_REGION(0x8,  29,  1),// ACCRST
    VSF_HW_RST_I2SF5                    = VSF_HW_CLKRST_REGION(0x8,  20,  1),// I2SF5RST
    VSF_HW_RST_TMR11                    = VSF_HW_CLKRST_REGION(0x8,  18,  1),// TMR11RST
    VSF_HW_RST_TMR10                    = VSF_HW_CLKRST_REGION(0x8,  17,  1),// TMR10RST
    VSF_HW_RST_TMR9                     = VSF_HW_CLKRST_REGION(0x8,  16,  1),// TMR9RST
    VSF_HW_RST_SCFG                     = VSF_HW_CLKRST_REGION(0x8,  14,  1),// SCFGRST
    VSF_HW_RST_SPI1                     = VSF_HW_CLKRST_REGION(0x8,  12,  1),// SPI1RST
    VSF_HW_RST_ADC                      = VSF_HW_CLKRST_REGION(0x8,   8,  1),// ADCRST
    VSF_HW_RST_USART6                   = VSF_HW_CLKRST_REGION(0x8,   5,  1),// USART6RST
    VSF_HW_RST_USART1                   = VSF_HW_CLKRST_REGION(0x8,   4,  1),// USART1RST
    VSF_HW_RST_TMR1                     = VSF_HW_CLKRST_REGION(0x8,   0,  1),// TMR1RST
} vsf_hw_peripheral_rst_t;

typedef enum vsf_hw_peripheral_en_t {
    // CRM_AHBEN1
    VSF_HW_EN_OTGHS                     = VSF_HW_CLKRST_REGION(0xC,  29, 1),// OTGHSEN
    VSF_HW_EN_DMA2                      = VSF_HW_CLKRST_REGION(0xC,  24, 1),// DMA2EN
    VSF_HW_EN_DMA1                      = VSF_HW_CLKRST_REGION(0xC,  22, 1),// DMA1EN
    VSF_HW_EN_CRC                       = VSF_HW_CLKRST_REGION(0xC,  12, 1),// CRCEN
    VSF_HW_EN_GPIO5                     = VSF_HW_CLKRST_REGION(0xC,  5,  1),// GPIOFEN
    VSF_HW_EN_GPIO3                     = VSF_HW_CLKRST_REGION(0xC,  3,  1),// GPIODEN
    VSF_HW_EN_GPIO2                     = VSF_HW_CLKRST_REGION(0xC,  2,  1),// GPIOCEN
    VSF_HW_EN_GPIO1                     = VSF_HW_CLKRST_REGION(0xC,  1,  1),// GPIOBEN
    VSF_HW_EN_GPIO0                     = VSF_HW_CLKRST_REGION(0xC,  0,  1),// GPIOAEN

    VSF_HW_EN_GPIOA                     = VSF_HW_EN_GPIO0,
    VSF_HW_EN_GPIOB                     = VSF_HW_EN_GPIO1,
    VSF_HW_EN_GPIOC                     = VSF_HW_EN_GPIO2,
    VSF_HW_EN_GPIOD                     = VSF_HW_EN_GPIO3,
    VSF_HW_EN_GPIOF                     = VSF_HW_EN_GPIO5,

    // CRM_AHBEN2
    VSF_HW_EN_OTGFS1                    = VSF_HW_CLKRST_REGION(0xD,  7,  1),// OTGFS1EN

    // CRM_AHBEN3
    VSF_HW_EN_QSPI1                     = VSF_HW_CLKRST_REGION(0xE,  1,  1),// QSPI1EN

    // CRM_APB1EN
    VSF_HW_EN_UART8                     = VSF_HW_CLKRST_REGION(0x10, 31,  1),// UART8EN
    VSF_HW_EN_UART7                     = VSF_HW_CLKRST_REGION(0x10, 30,  1),// UART7EN
    VSF_HW_EN_PWC                       = VSF_HW_CLKRST_REGION(0x10, 28,  1),// PWCEN
    VSF_HW_EN_CAN1                      = VSF_HW_CLKRST_REGION(0x10, 25,  1),// CAN1EN
    VSF_HW_EN_I2C3                      = VSF_HW_CLKRST_REGION(0x10, 23,  1),// I2C3EN
    VSF_HW_EN_I2C2                      = VSF_HW_CLKRST_REGION(0x10, 22,  1),// I2C2EN
    VSF_HW_EN_I2C1                      = VSF_HW_CLKRST_REGION(0x10, 21,  1),// I2C1EN
    VSF_HW_EN_USART5                    = VSF_HW_CLKRST_REGION(0x10, 20,  1),// USART5EN
    VSF_HW_EN_USART4                    = VSF_HW_CLKRST_REGION(0x10, 19,  1),// USART4EN
    VSF_HW_EN_USART3                    = VSF_HW_CLKRST_REGION(0x10, 18,  1),// USART3EN
    VSF_HW_EN_USART2                    = VSF_HW_CLKRST_REGION(0x10, 17,  1),// USART2EN
    VSF_HW_EN_SPI3                      = VSF_HW_CLKRST_REGION(0x10, 15,  1),// SPI3EN
    VSF_HW_EN_SPI2                      = VSF_HW_CLKRST_REGION(0x10, 14,  1),// SPI2EN
    VSF_HW_EN_WWDT                      = VSF_HW_CLKRST_REGION(0x10, 11,  1),// WWDTEN
    VSF_HW_EN_TMR14                     = VSF_HW_CLKRST_REGION(0x10,  8,  1),// TMR14EN
    VSF_HW_EN_TMR13                     = VSF_HW_CLKRST_REGION(0x10,  7,  1),// TMR13EN
    VSF_HW_EN_TMR7                      = VSF_HW_CLKRST_REGION(0x10,  5,  1),// TMR7EN
    VSF_HW_EN_TMR6                      = VSF_HW_CLKRST_REGION(0x10,  4,  1),// TMR6EN
    VSF_HW_EN_TMR4                      = VSF_HW_CLKRST_REGION(0x10,  2,  1),// TMR4EN
    VSF_HW_EN_TMR3                      = VSF_HW_CLKRST_REGION(0x10,  1,  1),// TMR3EN
    VSF_HW_EN_TMR2                      = VSF_HW_CLKRST_REGION(0x10,  0,  1),// TMR2EN

    // CRM_APB2EN
    VSF_HW_EN_ACC                       = VSF_HW_CLKRST_REGION(0x11, 29,  1),// ACCEN
    VSF_HW_EN_I2SF5                     = VSF_HW_CLKRST_REGION(0x11, 20,  1),// I2SF5EN
    VSF_HW_EN_TMR11                     = VSF_HW_CLKRST_REGION(0x11, 18,  1),// TMR11EN
    VSF_HW_EN_TMR10                     = VSF_HW_CLKRST_REGION(0x11, 17,  1),// TMR10EN
    VSF_HW_EN_TMR9                      = VSF_HW_CLKRST_REGION(0x11, 16,  1),// TMR9EN
    VSF_HW_EN_SCFG                      = VSF_HW_CLKRST_REGION(0x11, 14,  1),// SCFGEN
    VSF_HW_EN_SPI1                      = VSF_HW_CLKRST_REGION(0x11, 12,  1),// SPI1EN
    VSF_HW_EN_ADC                       = VSF_HW_CLKRST_REGION(0x11,  8,  1),// ADCEN
    VSF_HW_EN_USART6                    = VSF_HW_CLKRST_REGION(0x11,  5,  1),// USART6EN
    VSF_HW_EN_USART1                    = VSF_HW_CLKRST_REGION(0x11,  4,  1),// USART1EN
    VSF_HW_EN_TMR1                      = VSF_HW_CLKRST_REGION(0x11,  0,  1),// TMR1EN

    // CRM_AHBLPEN1
    VSF_HW_EN_OTGHSLP                   = VSF_HW_CLKRST_REGION(0x14, 29, 1),// OTGHSLPEN
    VSF_HW_EN_DMA2LP                    = VSF_HW_CLKRST_REGION(0x14, 24, 1),// DMA2LPEN
    VSF_HW_EN_DMA1LP                    = VSF_HW_CLKRST_REGION(0x14, 22, 1),// DMA1LPEN
    VSF_HW_EN_CRCLP                     = VSF_HW_CLKRST_REGION(0x14, 12, 1),// CRCLPEN
    VSF_HW_EN_GPIO5LP                   = VSF_HW_CLKRST_REGION(0x14, 5,  1),// GPIOFLPEN
    VSF_HW_EN_GPIO3LP                   = VSF_HW_CLKRST_REGION(0x14, 3,  1),// GPIODLPEN
    VSF_HW_EN_GPIO2LP                   = VSF_HW_CLKRST_REGION(0x14, 2,  1),// GPIOCLPEN
    VSF_HW_EN_GPIO1LP                   = VSF_HW_CLKRST_REGION(0x14, 1,  1),// GPIOBLPEN
    VSF_HW_EN_GPIO0LP                   = VSF_HW_CLKRST_REGION(0x14, 0,  1),// GPIOALPEN

    VSF_HW_EN_GPIOALP                   = VSF_HW_EN_GPIO0LP,
    VSF_HW_EN_GPIOBLP                   = VSF_HW_EN_GPIO1LP,
    VSF_HW_EN_GPIOCLP                   = VSF_HW_EN_GPIO2LP,
    VSF_HW_EN_GPIODLP                   = VSF_HW_EN_GPIO3LP,
    VSF_HW_EN_GPIOFLP                   = VSF_HW_EN_GPIO5LP,

    // CRM_AHBLPEN2
    VSF_HW_EN_OTGFS1LP                  = VSF_HW_CLKRST_REGION(0x15, 7,  1),// OTGFS1LPEN

    // CRM_AHBLPEN3
    VSF_HW_EN_QSPI1LP                   = VSF_HW_CLKRST_REGION(0x16, 1,  1),// QSPI1LPEN

    // CRM_APB1LPEN
    VSF_HW_EN_UART8LP                   = VSF_HW_CLKRST_REGION(0x18, 31,  1),// UART8LPEN
    VSF_HW_EN_UART7LP                   = VSF_HW_CLKRST_REGION(0x18, 30,  1),// UART7LPEN
    VSF_HW_EN_PWCLP                     = VSF_HW_CLKRST_REGION(0x18, 28,  1),// PWCLPEN
    VSF_HW_EN_CAN1LP                    = VSF_HW_CLKRST_REGION(0x18, 25,  1),// CAN1LPEN
    VSF_HW_EN_I2C3LP                    = VSF_HW_CLKRST_REGION(0x18, 23,  1),// I2C3LPEN
    VSF_HW_EN_I2C2LP                    = VSF_HW_CLKRST_REGION(0x18, 22,  1),// I2C2LPEN
    VSF_HW_EN_I2C1LP                    = VSF_HW_CLKRST_REGION(0x18, 21,  1),// I2C1LPEN
    VSF_HW_EN_USART5LP                  = VSF_HW_CLKRST_REGION(0x18, 20,  1),// USART5LPEN
    VSF_HW_EN_USART4LP                  = VSF_HW_CLKRST_REGION(0x18, 19,  1),// USART4LPEN
    VSF_HW_EN_USART3LP                  = VSF_HW_CLKRST_REGION(0x18, 18,  1),// USART3LPEN
    VSF_HW_EN_USART2LP                  = VSF_HW_CLKRST_REGION(0x18, 17,  1),// USART2LPEN
    VSF_HW_EN_SPI3LP                    = VSF_HW_CLKRST_REGION(0x18, 15,  1),// SPI3LPEN
    VSF_HW_EN_SPI2LP                    = VSF_HW_CLKRST_REGION(0x18, 14,  1),// SPI2LPEN
    VSF_HW_EN_WWDTLP                    = VSF_HW_CLKRST_REGION(0x18, 11,  1),// WWDTLPEN
    VSF_HW_EN_TMR14LP                   = VSF_HW_CLKRST_REGION(0x18,  8,  1),// TMR14LPEN
    VSF_HW_EN_TMR13LP                   = VSF_HW_CLKRST_REGION(0x18,  7,  1),// TMR13LPEN
    VSF_HW_EN_TMR7LP                    = VSF_HW_CLKRST_REGION(0x18,  5,  1),// TMR7LPEN
    VSF_HW_EN_TMR6LP                    = VSF_HW_CLKRST_REGION(0x18,  4,  1),// TMR6LPEN
    VSF_HW_EN_TMR4LP                    = VSF_HW_CLKRST_REGION(0x18,  2,  1),// TMR4LPEN
    VSF_HW_EN_TMR3LP                    = VSF_HW_CLKRST_REGION(0x18,  1,  1),// TMR3LPEN
    VSF_HW_EN_TMR2LP                    = VSF_HW_CLKRST_REGION(0x18,  0,  1),// TMR2LPEN

    // CRM_APB2LPEN
    VSF_HW_EN_ACCLP                     = VSF_HW_CLKRST_REGION(0x19, 29,  1),// ACCLPEN
    VSF_HW_EN_I2SF5LP                   = VSF_HW_CLKRST_REGION(0x19, 20,  1),// I2SF5LPEN
    VSF_HW_EN_TMR11LP                   = VSF_HW_CLKRST_REGION(0x19, 18,  1),// TMR11LPEN
    VSF_HW_EN_TMR10LP                   = VSF_HW_CLKRST_REGION(0x19, 17,  1),// TMR10LPEN
    VSF_HW_EN_TMR9LP                    = VSF_HW_CLKRST_REGION(0x19, 16,  1),// TMR9LPEN
    VSF_HW_EN_SCFGLP                    = VSF_HW_CLKRST_REGION(0x19, 14,  1),// SCFGLPEN
    VSF_HW_EN_SPI1LP                    = VSF_HW_CLKRST_REGION(0x19, 12,  1),// SPI1LPEN
    VSF_HW_EN_ADCLP                     = VSF_HW_CLKRST_REGION(0x19,  8,  1),// ADCLPEN
    VSF_HW_EN_USART6LP                  = VSF_HW_CLKRST_REGION(0x19,  5,  1),// USART6LPEN
    VSF_HW_EN_USART1LP                  = VSF_HW_CLKRST_REGION(0x19,  4,  1),// USART1LPEN
    VSF_HW_EN_TMR1LP                    = VSF_HW_CLKRST_REGION(0x19,  0,  1),// TMR1LPEN
} vsf_hw_peripheral_en_t;

typedef struct vsf_hw_clk_t vsf_hw_clk_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_hw_clk_t VSF_HW_CLK_HSE;
#define VSF_HW_CLK_HEXT                 VSF_HW_CLK_HSE
extern const vsf_hw_clk_t VSF_HW_CLK_HSI48;
extern const vsf_hw_clk_t VSF_HW_CLK_HSI8;
extern const vsf_hw_clk_t VSF_HW_CLK_HSI;
#define VSF_HW_CLK_HICK                 VSF_HW_CLK_HSI
extern const vsf_hw_clk_t VSF_HW_CLK_LSE;
#define VSF_HW_CLK_LEXT                 VSF_HW_CLK_LSE
extern const vsf_hw_clk_t VSF_HW_CLK_LSI;
#define VSF_HW_CLK_LICK                 VSF_HW_CLK_LSI

extern const vsf_hw_clk_t VSF_HW_CLK_PLL;
extern const vsf_hw_clk_t VSF_HW_CLK_PLLP;
extern const vsf_hw_clk_t VSF_HW_CLK_PLLU;

extern const vsf_hw_clk_t VSF_HW_CLK_SYS;
#define VSF_HW_CLK_SCLK                 VSF_HW_CLK_SYS
extern const vsf_hw_clk_t VSF_HW_CLK_AHB;
#define VSF_HW_CLK_HCLK                 VSF_HW_CLK_AHB
#define VSF_HW_CLK_CPU                  VSF_HW_CLK_AHB

extern const vsf_hw_clk_t VSF_HW_CLK_SYSTICK_EXT;
extern const vsf_hw_clk_t VSF_HW_CLK_APB1;
extern const vsf_hw_clk_t VSF_HW_CLK_APB2;

#define VSF_HW_CLK_I2S1                 VSF_HW_CLK_SYS
#define VSF_HW_CLK_I2S2                 VSF_HW_CLK_SYS
#define VSF_HW_CLK_I2S3                 VSF_HW_CLK_SYS

/*============================ LOCAL VARIABLES ===============================*/
/*============================ INCLUDES ======================================*/
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

extern vsf_err_t vsf_hw_pll_config(const vsf_hw_clk_t *clk, const vsf_hw_clk_t *clksrc, uint16_t mul, uint16_t div, uint32_t out_freq_hz);

#endif
/* EOF */
