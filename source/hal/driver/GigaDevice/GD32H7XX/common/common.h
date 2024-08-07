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

#ifndef __HAL_DRIVER_GIGADEVICE_GD32H7XX_COMMON_H__
#define __HAL_DRIVER_GIGADEVICE_GD32H7XX_COMMON_H__

/* \note common.h should only be included by device.h */

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"
#include "hal/arch/vsf_arch.h"

/*============================ MACROS ========================================*/

// CLKRST REGION

#define VSF_HW_CLKRST_REGION(__WORD_OFFSET, __BIT_OFFSET, __BIT_LENGTH)         \
            (((__WORD_OFFSET) << 16) | ((__BIT_LENGTH) << 8) | ((__BIT_OFFSET) << 0))

/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_hw_peripheral_clk_set       vsf_hw_clkrst_region_set
#define vsf_hw_peripheral_clk_get       vsf_hw_clkrst_region_get

#define vsf_hw_peripheral_rst_set       vsf_hw_clkrst_region_set_bit
#define vsf_hw_peripheral_rst_clear     vsf_hw_clkrst_region_clear_bit
#define vsf_hw_peripheral_rst_get       vsf_hw_clkrst_region_get_bit

#define vsf_hw_peripheral_enable        vsf_hw_clkrst_region_set_bit
#define vsf_hw_peripheral_disable       vsf_hw_clkrst_region_clear_bit

/*============================ TYPES =========================================*/

typedef enum vsf_hw_peripheral_clk_t {
    // RCU.CFG1
    VSF_HW_CLK_HPDF                     = VSF_HW_CLKRST_REGION(0x23, 31, 1),// HPDFSEL in RCU.CFG1
    VSF_HW_CLK_TIMER                    = VSF_HW_CLKRST_REGION(0x23, 24, 1),// TIMERSEL in RCU.CFG1
    VSF_HW_CLK_USART5                   = VSF_HW_CLKRST_REGION(0x23, 22, 2),// USART5SEL in RCU.CFG1
    VSF_HW_CLK_USART2                   = VSF_HW_CLKRST_REGION(0x23, 20, 2),// USART2SEL in RCU.CFG1
    VSF_HW_CLK_USART1                   = VSF_HW_CLKRST_REGION(0x23, 18, 2),// USART1SEL in RCU.CFG1
    VSF_HW_CLK_PER                      = VSF_HW_CLKRST_REGION(0x23, 14, 2),// PERSEL in RCU.CFG1
    VSF_HW_CLK_CAN2                     = VSF_HW_CLKRST_REGION(0x23, 12, 2),// CAN2SEL in RCU.CFG1
    VSF_HW_CLK_CAN1                     = VSF_HW_CLKRST_REGION(0x23, 10, 2),// CAN1SEL in RCU.CFG1
    VSF_HW_CLK_CAN0                     = VSF_HW_CLKRST_REGION(0x23, 8, 2), // CAN0SEL in RCU.CFG1
    VSF_HW_CLK_RSPDIF                   = VSF_HW_CLKRST_REGION(0x23, 4, 2), // RSPDIFSEL in RCU.CFG1
    VSF_HW_CLK_USART0                   = VSF_HW_CLKRST_REGION(0x23, 0, 2), // USART0SEL in RCU.CFG1

    // RCU.CFG2
    VSF_HW_CLK_SAI2B1                   = VSF_HW_CLKRST_REGION(0x24, 28, 3),// SAI2B1SEL in RCU.CFG2
    VSF_HW_CLK_SAI2B0                   = VSF_HW_CLKRST_REGION(0x24, 24, 3),// SAI2B0SEL in RCU.CFG2
    VSF_HW_CLK_SAI1                     = VSF_HW_CLKRST_REGION(0x24, 20, 3),// SAI1SEL in RCU.CFG2
    VSF_HW_CLK_SAI0                     = VSF_HW_CLKRST_REGION(0x24, 16, 3),// SAI0SEL in RCU.CFG2
    VSF_HW_CLK_CKOUT1                   = VSF_HW_CLKRST_REGION(0x24, 12, 3),// CKOUT1SEL in RCU.CFG2
    VSF_HW_CLK_CKOUT0                   = VSF_HW_CLKRST_REGION(0x24, 4, 3), // CKOUT0SEL in RCU.CFG2

    // RCU.CFG3
    VSF_HW_CLK_ADC2                     = VSF_HW_CLKRST_REGION(0x25, 28, 2),// ADC2SEL in RCU.CFG3
    VSF_HW_CLK_ADC1                     = VSF_HW_CLKRST_REGION(0x25, 26, 2),// ADC1SEL in RCU.CFG3
    VSF_HW_CLK_DSPWUS                   = VSF_HW_CLKRST_REGION(0x25, 24, 1),// DSPWUSSEL in RCU.CFG3
    VSF_HW_CLK_SDIO1                    = VSF_HW_CLKRST_REGION(0x25, 12, 1),// SDIO1SEL in RCU.CFG3
    VSF_HW_CLK_I2C3                     = VSF_HW_CLKRST_REGION(0x25, 4, 2), // I2C3SEL in RCU.CFG3
    VSF_HW_CLK_I2C2                     = VSF_HW_CLKRST_REGION(0x25, 2, 2), // I2C2SEL in RCU.CFG3
    VSF_HW_CLK_I2C1                     = VSF_HW_CLKRST_REGION(0x25, 0, 2), // I2C1SEL in RCU.CFG3
} vsf_hw_peripheral_clk_t;

typedef enum vsf_hw_peripheral_rst_t {
    // RCU.AHB1RST
    VSF_HW_RST_USBHS1                   = VSF_HW_CLKRST_REGION(0x04, 29, 1),// USBHS1RST in RCU.AHB1RST
    VSF_HW_RST_ENET0                    = VSF_HW_CLKRST_REGION(0x04, 25, 1),// ENET0RST in RCU.AHB1RST
    VSF_HW_RST_DMAMUX                   = VSF_HW_CLKRST_REGION(0x04, 23, 1),// DMAMUXRST in RCU.AHB1RST
    VSF_HW_RST_DMA1                     = VSF_HW_CLKRST_REGION(0x04, 22, 1),// DMA1RST in RCU.AHB1RST
    VSF_HW_RST_DMA0                     = VSF_HW_CLKRST_REGION(0x04, 21, 1),// DMA0RST in RCU.AHB1RST
    VSF_HW_RST_USBHS0                   = VSF_HW_CLKRST_REGION(0x04, 14, 1),// USBHS0RST in RCU.AHB1RST
    VSF_HW_RST_ENET1                    = VSF_HW_CLKRST_REGION(0x04, 0, 1), // ENET1RST in RCU.AHB1RST

    // RCU.AHB2RST
    VSF_HW_RST_TMU                      = VSF_HW_CLKRST_REGION(0x05, 7, 1), // TMURST in RCU.AHB2RST
    VSF_HW_RST_TRNG                     = VSF_HW_CLKRST_REGION(0x05, 6, 1), // TRNGRST in RCU.AHB2RST
    VSF_HW_RST_HAU                      = VSF_HW_CLKRST_REGION(0x05, 4, 1), // HAURST in RCU.AHB2RST
    VSF_HW_RST_CAU                      = VSF_HW_CLKRST_REGION(0x05, 3, 1), // CAURST in RCU.AHB2RST
    VSF_HW_RST_SDIO1                    = VSF_HW_CLKRST_REGION(0x05, 2, 1), // SDIO1RST in RCU.AHB2RST
    VSF_HW_RST_FAC                      = VSF_HW_CLKRST_REGION(0x05, 1, 1), // FACRST in RCU.AHB2RST
    VSF_HW_RST_DCI                      = VSF_HW_CLKRST_REGION(0x05, 0, 1), // DCIRST in RCU.AHB2RST

    // RCU.AHB3RST
    VSF_HW_RST_RTDEC1                   = VSF_HW_CLKRST_REGION(0x06, 9, 1), // RTDEC1RST in RCU.AHB3RST
    VSF_HW_RST_RTDEC0                   = VSF_HW_CLKRST_REGION(0x06, 8, 1), // RTDEC0RST in RCU.AHB3RST
    VSF_HW_RST_OSPI1                    = VSF_HW_CLKRST_REGION(0x06, 6, 1), // OSPI1RST in RCU.AHB3RST
    VSF_HW_RST_OSPI0                    = VSF_HW_CLKRST_REGION(0x06, 5, 1), // OSPI0RST in RCU.AHB3RST
    VSF_HW_RST_OSPIM                    = VSF_HW_CLKRST_REGION(0x06, 4, 1), // OSPIMRST in RCU.AHB3RST
    VSF_HW_RST_MDMA                     = VSF_HW_CLKRST_REGION(0x06, 3, 1), // MDMARST in RCU.AHB3RST
    VSF_HW_RST_SDIO0                    = VSF_HW_CLKRST_REGION(0x06, 2, 1), // SDIO0RST in RCU.AHB3RST
    VSF_HW_RST_IPA                      = VSF_HW_CLKRST_REGION(0x06, 1, 1), // IPARST in RCU.AHB3RST
    VSF_HW_RST_EXMC                     = VSF_HW_CLKRST_REGION(0x06, 0, 1), // EXMCRST in RCU.AHB3RST

    // RCU.AHB4RST
    VSF_HW_RST_HWSEM                    = VSF_HW_CLKRST_REGION(0x07, 15, 1),// HWSEMRST in RCU.AHB4RST
    VSF_HW_RST_CRC                      = VSF_HW_CLKRST_REGION(0x07, 14, 1),// CRCRST in RCU.AHB4RST
    VSF_HW_RST_GPIOK                    = VSF_HW_CLKRST_REGION(0x07, 9, 1), // PKRST in RCU.AHB4RST
    VSF_HW_RST_GPIOJ                    = VSF_HW_CLKRST_REGION(0x07, 8, 1), // PJRST in RCU.AHB4RST
    VSF_HW_RST_GPIOH                    = VSF_HW_CLKRST_REGION(0x07, 7, 1), // PHRST in RCU.AHB4RST
    VSF_HW_RST_GPIOG                    = VSF_HW_CLKRST_REGION(0x07, 6, 1), // PGRST in RCU.AHB4RST
    VSF_HW_RST_GPIOF                    = VSF_HW_CLKRST_REGION(0x07, 5, 1), // PFRST in RCU.AHB4RST
    VSF_HW_RST_GPIOE                    = VSF_HW_CLKRST_REGION(0x07, 4, 1), // PERST in RCU.AHB4RST
    VSF_HW_RST_GPIOD                    = VSF_HW_CLKRST_REGION(0x07, 3, 1), // PDRST in RCU.AHB4RST
    VSF_HW_RST_GPIOC                    = VSF_HW_CLKRST_REGION(0x07, 2, 1), // PCRST in RCU.AHB4RST
    VSF_HW_RST_GPIOB                    = VSF_HW_CLKRST_REGION(0x07, 1, 1), // PBRST in RCU.AHB4RST
    VSF_HW_RST_GPIOA                    = VSF_HW_CLKRST_REGION(0x07, 0, 1), // PARST in RCU.AHB4RST

    // RCC.APB1RST
    VSF_HW_RST_UART7                    = VSF_HW_CLKRST_REGION(0x08, 31, 1),// UART7RST in RCU.APB1RST
    VSF_HW_RST_UART6                    = VSF_HW_CLKRST_REGION(0x08, 30, 1),// UART6RST in RCU.APB1RST
    VSF_HW_RST_DAC                      = VSF_HW_CLKRST_REGION(0x08, 29, 1),// DACRST in RCU.APB1RST
    VSF_HW_RST_DACHOLD                  = VSF_HW_CLKRST_REGION(0x08, 28, 1),// DACHOLDRST in RCU.APB1RST
    VSF_HW_RST_CTC                      = VSF_HW_CLKRST_REGION(0x08, 27, 1),// CTCRST in RCU.APB1RST
    VSF_HW_RST_I2C3                     = VSF_HW_CLKRST_REGION(0x08, 24, 1),// I2C3RST in RCU.APB1RST
    VSF_HW_RST_I2C2                     = VSF_HW_CLKRST_REGION(0x08, 23, 1),// I2C2RST in RCU.APB1RST
    VSF_HW_RST_I2C1                     = VSF_HW_CLKRST_REGION(0x08, 22, 1),// I2C1RST in RCU.APB1RST
    VSF_HW_RST_I2C0                     = VSF_HW_CLKRST_REGION(0x08, 21, 1),// I2C0RST in RCU.APB1RST
    VSF_HW_RST_UART4                    = VSF_HW_CLKRST_REGION(0x08, 20, 1),// UART4RST in RCU.APB1RST
    VSF_HW_RST_UART3                    = VSF_HW_CLKRST_REGION(0x08, 19, 1),// UART3RST in RCU.APB1RST
    VSF_HW_RST_USART2                   = VSF_HW_CLKRST_REGION(0x08, 18, 1),// USART2RST in RCU.APB1RST
    VSF_HW_RST_USART1                   = VSF_HW_CLKRST_REGION(0x08, 17, 1),// USART1RST in RCU.APB1RST
    VSF_HW_RST_MDIO                     = VSF_HW_CLKRST_REGION(0x08, 16, 1),// MDIORST in RCU.APB1RST
    VSF_HW_RST_SPI2                     = VSF_HW_CLKRST_REGION(0x08, 15, 1),// SPI2RST in RCU.APB1RST
    VSF_HW_RST_SPI1                     = VSF_HW_CLKRST_REGION(0x08, 14, 1),// SPI1RST in RCU.APB1RST
    VSF_HW_RST_RSPDIF                   = VSF_HW_CLKRST_REGION(0x08, 13, 1),// RSPDIFRST in RCU.APB1RST
    VSF_HW_RST_TIMER51                  = VSF_HW_CLKRST_REGION(0x08, 11, 1),// TIMER51RST in RCU.APB1RST
    VSF_HW_RST_TIMER50                  = VSF_HW_CLKRST_REGION(0x08, 10, 1),// TIMER50RST in RCU.APB1RST
    VSF_HW_RST_TIMER31                  = VSF_HW_CLKRST_REGION(0x08, 9, 1), // TIMER31RST in RCU.APB1RST
    VSF_HW_RST_TIMER30                  = VSF_HW_CLKRST_REGION(0x08, 8, 1), // TIMER30RST in RCU.APB1RST
    VSF_HW_RST_TIMER23                  = VSF_HW_CLKRST_REGION(0x08, 7, 1), // TIMER23RST in RCU.APB1RST
    VSF_HW_RST_TIMER22                  = VSF_HW_CLKRST_REGION(0x08, 6, 1), // TIMER22RST in RCU.APB1RST
    VSF_HW_RST_TIMER6                   = VSF_HW_CLKRST_REGION(0x08, 5, 1), // TIMER6RST in RCU.APB1RST
    VSF_HW_RST_TIMER5                   = VSF_HW_CLKRST_REGION(0x08, 4, 1), // TIMER5RST in RCU.APB1RST
    VSF_HW_RST_TIMER4                   = VSF_HW_CLKRST_REGION(0x08, 3, 1), // TIMER4RST in RCU.APB1RST
    VSF_HW_RST_TIMER3                   = VSF_HW_CLKRST_REGION(0x08, 2, 1), // TIMER3RST in RCU.APB1RST
    VSF_HW_RST_TIMER2                   = VSF_HW_CLKRST_REGION(0x08, 1, 1), // TIMER2RST in RCU.APB1RST
    VSF_HW_RST_TIMER1                   = VSF_HW_CLKRST_REGION(0x08, 0, 1), // TIMER1RST in RCU.APB1RST

    // RCC.APB2RST
    VSF_HW_RST_TRIGSEL                  = VSF_HW_CLKRST_REGION(0x09, 31, 1),// TRIGRST in RCU.APB2RST
    VSF_HW_RST_EDOUT                    = VSF_HW_CLKRST_REGION(0x09, 30, 1),// EDOUTRST in RCU.APB2RST
    VSF_HW_RST_TIMER44                  = VSF_HW_CLKRST_REGION(0x09, 29, 1),// TIMER44RST in RCU.APB2RST
    VSF_HW_RST_TIMER43                  = VSF_HW_CLKRST_REGION(0x09, 28, 1),// TIMER43RST in RCU.APB2RST
    VSF_HW_RST_TIMER42                  = VSF_HW_CLKRST_REGION(0x09, 27, 1),// TIMER42RST in RCU.APB2RST
    VSF_HW_RST_TIMER41                  = VSF_HW_CLKRST_REGION(0x09, 26, 1),// TIMER41RST in RCU.APB2RST
    VSF_HW_RST_TIMER40                  = VSF_HW_CLKRST_REGION(0x09, 25, 1),// TIMER40RST in RCU.APB2RST
    VSF_HW_RST_SAI2                     = VSF_HW_CLKRST_REGION(0x09, 24, 1),// SAI2RST in RCU.APB2RST
    VSF_HW_RST_SAI1                     = VSF_HW_CLKRST_REGION(0x09, 23, 1),// SAI1RST in RCU.APB2RST
    VSF_HW_RST_SAI0                     = VSF_HW_CLKRST_REGION(0x09, 22, 1),// SAI0RST in RCU.APB2RST
    VSF_HW_RST_SPI5                     = VSF_HW_CLKRST_REGION(0x09, 21, 1),// SPI5RST in RCU.APB2RST
    VSF_HW_RST_SPI4                     = VSF_HW_CLKRST_REGION(0x09, 20, 1),// SPI4RST in RCU.APB2RST
    VSF_HW_RST_HPDF                     = VSF_HW_CLKRST_REGION(0x09, 19, 1),// HPDFRST in RCU.APB2RST
    VSF_HW_RST_TIMER16                  = VSF_HW_CLKRST_REGION(0x09, 18, 1),// TIMER16RST in RCU.APB2RST
    VSF_HW_RST_TIMER15                  = VSF_HW_CLKRST_REGION(0x09, 17, 1),// TIMER15RST in RCU.APB2RST
    VSF_HW_RST_TIMER14                  = VSF_HW_CLKRST_REGION(0x09, 16, 1),// TIMER14RST in RCU.APB2RST
    VSF_HW_RST_SPI3                     = VSF_HW_CLKRST_REGION(0x09, 13, 1),// SPI3RST in RCU.APB2RST
    VSF_HW_RST_SPI0                     = VSF_HW_CLKRST_REGION(0x09, 12, 1),// SPI0RST in RCU.APB2RST
    VSF_HW_RST_ADC2                     = VSF_HW_CLKRST_REGION(0x09, 10, 1),// ADC2RST in RCU.APB2RST
    VSF_HW_RST_ADC1                     = VSF_HW_CLKRST_REGION(0x09, 9, 1), // ADC1RST in RCU.APB2RST
    VSF_HW_RST_ADC0                     = VSF_HW_CLKRST_REGION(0x09, 8, 1), // ADC0RST in RCU.APB2RST
    VSF_HW_RST_USART5                   = VSF_HW_CLKRST_REGION(0x09, 5, 1), // USART5RST in RCU.APB2RST
    VSF_HW_RST_USART0                   = VSF_HW_CLKRST_REGION(0x09, 4, 1), // USART0RST in RCU.APB2RST
    VSF_HW_RST_TIMER7                   = VSF_HW_CLKRST_REGION(0x09, 1, 1), // TIMER7RST in RCU.APB2RST
    VSF_HW_RST_TIMER0                   = VSF_HW_CLKRST_REGION(0x09, 0, 1), // TIMER0RST in RCU.APB2RST

    // RCC.APB3RST
    VSF_HW_RST_WWDGT                    = VSF_HW_CLKRST_REGION(0x0A, 1, 1), // WWDGTRST in RCU.APB3RST
    VSF_HW_RST_TLI                      = VSF_HW_CLKRST_REGION(0x0A, 0, 1), // TLIRST in RCU.APB3RST

    // RCC.APB4RST
    VSF_HW_RST_PMU                      = VSF_HW_CLKRST_REGION(0x0B, 4, 1), // PMURST in RCU.APB4RST
    VSF_HW_RST_LPDTS                    = VSF_HW_CLKRST_REGION(0x0B, 3, 1), // LPDTSRST in RCU.APB4RST
    VSF_HW_RST_VREF                     = VSF_HW_CLKRST_REGION(0x0B, 2, 1), // VREFRST in RCU.APB4RST
    VSF_HW_RST_CMP                      = VSF_HW_CLKRST_REGION(0x0B, 1, 1), // CMPRST in RCU.APB4RST
    VSF_HW_RST_SYSCFG                   = VSF_HW_CLKRST_REGION(0x0B, 0, 1), // SYSCFGRST in RCU.APB4RST
} vsf_hw_peripheral_rst_t;

typedef enum vsf_hw_peripheral_en_t {
    // RCU.AHB1EN
    VSF_HW_EN_USBHS1ULPI                = VSF_HW_CLKRST_REGION(0x0C, 30, 1),// USBHS1ULPIEN in RCU.AHB1EN
    VSF_HW_EN_USBHS1                    = VSF_HW_CLKRST_REGION(0x0C, 29, 1),// USBHS1EN in RCU.AHB1EN
    VSF_HW_EN_ENET0PTP                  = VSF_HW_CLKRST_REGION(0x0C, 28, 1),// ENET0PTPEN in RCU.AHB1EN
    VSF_HW_EN_ENET0RX                   = VSF_HW_CLKRST_REGION(0x0C, 27, 1),// ENET0RXEN in RCU.AHB1EN
    VSF_HW_EN_ENET0TX                   = VSF_HW_CLKRST_REGION(0x0C, 26, 1),// ENET0TXEN in RCU.AHB1EN
    VSF_HW_EN_ENET0                     = VSF_HW_CLKRST_REGION(0x0C, 25, 1),// ENET0EN in RCU.AHB1EN
    VSF_HW_EN_DMAMUX                    = VSF_HW_CLKRST_REGION(0x0C, 23, 1),// DMAMUXEN in RCU.AHB1EN
    VSF_HW_EN_DMA1                      = VSF_HW_CLKRST_REGION(0x0C, 22, 1),// DMA1EN in RCU.AHB1EN
    VSF_HW_EN_DMA0                      = VSF_HW_CLKRST_REGION(0x0C, 21, 1),// DMA0EN in RCU.AHB1EN
    VSF_HW_EN_USBHS0ULPI                = VSF_HW_CLKRST_REGION(0x0C, 15, 1),// USBHS0ULPIEN in RCU.AHB1EN
    VSF_HW_EN_USBHS0                    = VSF_HW_CLKRST_REGION(0x0C, 14, 1),// USBHS0EN in RCU.AHB1EN
    VSF_HW_EN_ENET1PTP                  = VSF_HW_CLKRST_REGION(0x0C, 3, 1), // ENET1PTPEN in RCU.AHB1EN
    VSF_HW_EN_ENET1RX                   = VSF_HW_CLKRST_REGION(0x0C, 2, 1), // ENET1RXEN in RCU.AHB1EN
    VSF_HW_EN_ENET1TX                   = VSF_HW_CLKRST_REGION(0x0C, 1, 1), // ENET1TXEN in RCU.AHB1EN
    VSF_HW_EN_ENET1                     = VSF_HW_CLKRST_REGION(0x0C, 0, 1), // ENET1EN in RCU.AHB1EN

    // RCU.AHB2EN
    VSF_HW_EN_RAMECCMU1                 = VSF_HW_CLKRST_REGION(0x0D, 8, 1), // RAMECCMU1EN in RCU.AHB2EN
    VSF_HW_EN_TMU                       = VSF_HW_CLKRST_REGION(0x0D, 7, 1), // TMUEN in RCU.AHB2EN
    VSF_HW_EN_TRNG                      = VSF_HW_CLKRST_REGION(0x0D, 6, 1), // TRNGEN in RCU.AHB2EN
    VSF_HW_EN_HAU                       = VSF_HW_CLKRST_REGION(0x0D, 4, 1), // HAUEN in RCU.AHB2EN
    VSF_HW_EN_CAU                       = VSF_HW_CLKRST_REGION(0x0D, 3, 1), // CAUEN in RCU.AHB2EN
    VSF_HW_EN_SDIO1                     = VSF_HW_CLKRST_REGION(0x0D, 2, 1), // SDIO1EN in RCU.AHB2EN
    VSF_HW_EN_FAC                       = VSF_HW_CLKRST_REGION(0x0D, 1, 1), // FACEN in RCU.AHB2EN
    VSF_HW_EN_DCI                       = VSF_HW_CLKRST_REGION(0x0D, 0, 1), // DCIEN in RCU.AHB2EN

    // RCU.AHB3EN
    VSF_HW_EN_CPU                       = VSF_HW_CLKRST_REGION(0x0E, 15, 1),// CPUEN in RCU.AHB3EN
    VSF_HW_EN_RAMECCMU0                 = VSF_HW_CLKRST_REGION(0x0E, 10, 1),// RAMECCMU0EN in RCU.AHB3EN
    VSF_HW_EN_RTDEC1                    = VSF_HW_CLKRST_REGION(0x0E, 9, 1), // RTDEC1EN in RCU.AHB3EN
    VSF_HW_EN_RTDEC0                    = VSF_HW_CLKRST_REGION(0x0E, 8, 1), // RTDEC0EN in RCU.AHB3EN
    VSF_HW_EN_OSPI1                     = VSF_HW_CLKRST_REGION(0x0E, 6, 1), // OSPI1EN in RCU.AHB3EN
    VSF_HW_EN_OSPI0                     = VSF_HW_CLKRST_REGION(0x0E, 5, 1), // OSPI0EN in RCU.AHB3EN
    VSF_HW_EN_OSPIM                     = VSF_HW_CLKRST_REGION(0x0E, 4, 1), // OSPIMEN in RCU.AHB3EN
    VSF_HW_EN_MDMA                      = VSF_HW_CLKRST_REGION(0x0E, 3, 1), // MDMAEN in RCU.AHB3EN
    VSF_HW_EN_SDIO0                     = VSF_HW_CLKRST_REGION(0x0E, 2, 1), // SDIO0EN in RCU.AHB3EN
    VSF_HW_EN_IPA                       = VSF_HW_CLKRST_REGION(0x0E, 1, 1), // IPAEN in RCU.AHB3EN
    VSF_HW_EN_EXMC                      = VSF_HW_CLKRST_REGION(0x0E, 0, 1), // EXMCEN in RCU.AHB3EN

    // RCU.AHB4EN
    VSF_HW_EN_HWSEM                     = VSF_HW_CLKRST_REGION(0x0F, 15, 1),// HWSEMEN in RCU.AHB4EN
    VSF_HW_EN_CRC                       = VSF_HW_CLKRST_REGION(0x0F, 14, 1),// CRCEN in RCU.AHB4EN
    VSF_HW_EN_BKPSRAM                   = VSF_HW_CLKRST_REGION(0x0F, 13, 1),// BKPSRAMEN in RCU.AHB4EN
    VSF_HW_EN_GPIOK                     = VSF_HW_CLKRST_REGION(0x0F, 9, 1), // PKEN in RCU.AHB4EN
    VSF_HW_EN_GPIOJ                     = VSF_HW_CLKRST_REGION(0x0F, 8, 1), // PJEN in RCU.AHB4EN
    VSF_HW_EN_GPIOH                     = VSF_HW_CLKRST_REGION(0x0F, 7, 1), // PHEN in RCU.AHB4EN
    VSF_HW_EN_GPIOG                     = VSF_HW_CLKRST_REGION(0x0F, 6, 1), // PGEN in RCU.AHB4EN
    VSF_HW_EN_GPIOF                     = VSF_HW_CLKRST_REGION(0x0F, 5, 1), // PFEN in RCU.AHB4EN
    VSF_HW_EN_GPIOE                     = VSF_HW_CLKRST_REGION(0x0F, 4, 1), // PEEN in RCU.AHB4EN
    VSF_HW_EN_GPIOD                     = VSF_HW_CLKRST_REGION(0x0F, 3, 1), // PDEN in RCU.AHB4EN
    VSF_HW_EN_GPIOC                     = VSF_HW_CLKRST_REGION(0x0F, 2, 1), // PCEN in RCU.AHB4EN
    VSF_HW_EN_GPIOB                     = VSF_HW_CLKRST_REGION(0x0F, 1, 1), // PBEN in RCU.AHB4EN
    VSF_HW_EN_GPIOA                     = VSF_HW_CLKRST_REGION(0x0F, 0, 1), // PAEN in RCU.AHB4EN

    // RCC.APB1EN
    VSF_HW_EN_UART7                     = VSF_HW_CLKRST_REGION(0x10, 31, 1),// UART7EN in RCU.APB1EN
    VSF_HW_EN_UART6                     = VSF_HW_CLKRST_REGION(0x10, 30, 1),// UART6EN in RCU.APB1EN
    VSF_HW_EN_DAC                       = VSF_HW_CLKRST_REGION(0x10, 29, 1),// DACEN in RCU.APB1EN
    VSF_HW_EN_DACHOLD                   = VSF_HW_CLKRST_REGION(0x10, 28, 1),// DACHOLDEN in RCU.APB1EN
    VSF_HW_EN_CTC                       = VSF_HW_CLKRST_REGION(0x10, 27, 1),// CTCEN in RCU.APB1EN
    VSF_HW_EN_I2C3                      = VSF_HW_CLKRST_REGION(0x10, 24, 1),// I2C3EN in RCU.APB1EN
    VSF_HW_EN_I2C2                      = VSF_HW_CLKRST_REGION(0x10, 23, 1),// I2C2EN in RCU.APB1EN
    VSF_HW_EN_I2C1                      = VSF_HW_CLKRST_REGION(0x10, 22, 1),// I2C1EN in RCU.APB1EN
    VSF_HW_EN_I2C0                      = VSF_HW_CLKRST_REGION(0x10, 21, 1),// I2C0EN in RCU.APB1EN
    VSF_HW_EN_UART4                     = VSF_HW_CLKRST_REGION(0x10, 20, 1),// UART4EN in RCU.APB1EN
    VSF_HW_EN_UART3                     = VSF_HW_CLKRST_REGION(0x10, 19, 1),// UART3EN in RCU.APB1EN
    VSF_HW_EN_USART2                    = VSF_HW_CLKRST_REGION(0x10, 18, 1),// USART2EN in RCU.APB1EN
    VSF_HW_EN_USART1                    = VSF_HW_CLKRST_REGION(0x10, 17, 1),// USART1EN in RCU.APB1EN
    VSF_HW_EN_MDIO                      = VSF_HW_CLKRST_REGION(0x10, 16, 1),// MDIOEN in RCU.APB1EN
    VSF_HW_EN_SPI2                      = VSF_HW_CLKRST_REGION(0x10, 15, 1),// SPI2EN in RCU.APB1EN
    VSF_HW_EN_SPI1                      = VSF_HW_CLKRST_REGION(0x10, 14, 1),// SPI1EN in RCU.APB1EN
    VSF_HW_EN_RSPDIF                    = VSF_HW_CLKRST_REGION(0x10, 13, 1),// RSPDIFEN in RCU.APB1EN
    VSF_HW_EN_TIMER51                   = VSF_HW_CLKRST_REGION(0x10, 11, 1),// TIMER51EN in RCU.APB1EN
    VSF_HW_EN_TIMER50                   = VSF_HW_CLKRST_REGION(0x10, 10, 1),// TIMER50EN in RCU.APB1EN
    VSF_HW_EN_TIMER31                   = VSF_HW_CLKRST_REGION(0x10, 9, 1), // TIMER31EN in RCU.APB1EN
    VSF_HW_EN_TIMER30                   = VSF_HW_CLKRST_REGION(0x10, 8, 1), // TIMER30EN in RCU.APB1EN
    VSF_HW_EN_TIMER23                   = VSF_HW_CLKRST_REGION(0x10, 7, 1), // TIMER23EN in RCU.APB1EN
    VSF_HW_EN_TIMER22                   = VSF_HW_CLKRST_REGION(0x10, 6, 1), // TIMER22EN in RCU.APB1EN
    VSF_HW_EN_TIMER6                    = VSF_HW_CLKRST_REGION(0x10, 5, 1), // TIMER6EN in RCU.APB1EN
    VSF_HW_EN_TIMER5                    = VSF_HW_CLKRST_REGION(0x10, 4, 1), // TIMER5EN in RCU.APB1EN
    VSF_HW_EN_TIMER4                    = VSF_HW_CLKRST_REGION(0x10, 3, 1), // TIMER4EN in RCU.APB1EN
    VSF_HW_EN_TIMER3                    = VSF_HW_CLKRST_REGION(0x10, 2, 1), // TIMER3EN in RCU.APB1EN
    VSF_HW_EN_TIMER2                    = VSF_HW_CLKRST_REGION(0x10, 1, 1), // TIMER2EN in RCU.APB1EN
    VSF_HW_EN_TIMER1                    = VSF_HW_CLKRST_REGION(0x10, 0, 1), // TIMER1EN in RCU.APB1EN

    // RCC.APB2EN
    VSF_HW_EN_TRIGSEL                   = VSF_HW_CLKRST_REGION(0x11, 31, 1),// TRIGEN in RCU.APB2EN
    VSF_HW_EN_EDOUT                     = VSF_HW_CLKRST_REGION(0x11, 30, 1),// EDOUTEN in RCU.APB2EN
    VSF_HW_EN_TIMER44                   = VSF_HW_CLKRST_REGION(0x11, 29, 1),// TIMER44EN in RCU.APB2EN
    VSF_HW_EN_TIMER43                   = VSF_HW_CLKRST_REGION(0x11, 28, 1),// TIMER43EN in RCU.APB2EN
    VSF_HW_EN_TIMER42                   = VSF_HW_CLKRST_REGION(0x11, 27, 1),// TIMER42EN in RCU.APB2EN
    VSF_HW_EN_TIMER41                   = VSF_HW_CLKRST_REGION(0x11, 26, 1),// TIMER41EN in RCU.APB2EN
    VSF_HW_EN_TIMER40                   = VSF_HW_CLKRST_REGION(0x11, 25, 1),// TIMER40EN in RCU.APB2EN
    VSF_HW_EN_SAI2                      = VSF_HW_CLKRST_REGION(0x11, 24, 1),// SAI2EN in RCU.APB2EN
    VSF_HW_EN_SAI1                      = VSF_HW_CLKRST_REGION(0x11, 23, 1),// SAI1EN in RCU.APB2EN
    VSF_HW_EN_SAI0                      = VSF_HW_CLKRST_REGION(0x11, 22, 1),// SAI0EN in RCU.APB2EN
    VSF_HW_EN_SPI5                      = VSF_HW_CLKRST_REGION(0x11, 21, 1),// SPI5EN in RCU.APB2EN
    VSF_HW_EN_SPI4                      = VSF_HW_CLKRST_REGION(0x11, 20, 1),// SPI4EN in RCU.APB2EN
    VSF_HW_EN_HPDF                      = VSF_HW_CLKRST_REGION(0x11, 19, 1),// HPDFEN in RCU.APB2EN
    VSF_HW_EN_TIMER16                   = VSF_HW_CLKRST_REGION(0x11, 18, 1),// TIMER16EN in RCU.APB2EN
    VSF_HW_EN_TIMER15                   = VSF_HW_CLKRST_REGION(0x11, 17, 1),// TIMER15EN in RCU.APB2EN
    VSF_HW_EN_TIMER14                   = VSF_HW_CLKRST_REGION(0x11, 16, 1),// TIMER14EN in RCU.APB2EN
    VSF_HW_EN_SPI3                      = VSF_HW_CLKRST_REGION(0x11, 13, 1),// SPI3EN in RCU.APB2EN
    VSF_HW_EN_SPI0                      = VSF_HW_CLKRST_REGION(0x11, 12, 1),// SPI0EN in RCU.APB2EN
    VSF_HW_EN_ADC2                      = VSF_HW_CLKRST_REGION(0x11, 10, 1),// ADC2EN in RCU.APB2EN
    VSF_HW_EN_ADC1                      = VSF_HW_CLKRST_REGION(0x11, 9, 1), // ADC1EN in RCU.APB2EN
    VSF_HW_EN_ADC0                      = VSF_HW_CLKRST_REGION(0x11, 8, 1), // ADC0EN in RCU.APB2EN
    VSF_HW_EN_USART5                    = VSF_HW_CLKRST_REGION(0x11, 5, 1), // USART5EN in RCU.APB2EN
    VSF_HW_EN_USART0                    = VSF_HW_CLKRST_REGION(0x11, 4, 1), // USART0EN in RCU.APB2EN
    VSF_HW_EN_TIMER7                    = VSF_HW_CLKRST_REGION(0x11, 1, 1), // TIMER7EN in RCU.APB2EN
    VSF_HW_EN_TIMER0                    = VSF_HW_CLKRST_REGION(0x11, 0, 1), // TIMER0EN in RCU.APB2EN

    // RCC.APB3EN
    VSF_HW_EN_WWDGT                     = VSF_HW_CLKRST_REGION(0x12, 1, 1), // WWDGTEN in RCU.APB3EN
    VSF_HW_EN_TLI                       = VSF_HW_CLKRST_REGION(0x12, 0, 1), // TLIEN in RCU.APB3EN

    // RCC.APB4EN
    VSF_HW_EN_PMU                       = VSF_HW_CLKRST_REGION(0x13, 4, 1), // PMUEN in RCU.APB4EN
    VSF_HW_EN_LPDTS                     = VSF_HW_CLKRST_REGION(0x13, 3, 1), // LPDTSEN in RCU.APB4EN
    VSF_HW_EN_VREF                      = VSF_HW_CLKRST_REGION(0x13, 2, 1), // VREFEN in RCU.APB4EN
    VSF_HW_EN_CMP                       = VSF_HW_CLKRST_REGION(0x13, 1, 1), // CMPEN in RCU.APB4EN
    VSF_HW_EN_SYSCFG                    = VSF_HW_CLKRST_REGION(0x13, 0, 1), // SYSCFGEN in RCU.APB4EN
} vsf_hw_peripheral_en_t;

enum {
    VSF_HW_CLK_PRESCALER_DIV,
    VSF_HW_CLK_PRESCALER_ADD1_DIV,
    VSF_HW_CLK_PRESCALER_SFT,
    VSF_HW_CLK_PRESCALER_FUNC,
};
enum {
    VSF_HW_CLK_TYPE_CONST,
    VSF_HW_CLK_TYPE_CLK,
    VSF_HW_CLK_TYPE_SEL,
};

typedef struct vsf_hw_clk_t vsf_hw_clk_t;
struct vsf_hw_clk_t {
    uint32_t clksel_region;
    uint32_t clkprescaler_region;
    uint32_t clken_region;
    uint32_t clkrdy_region;

    union {
        const vsf_hw_clk_t **clksel_mapper;
        uint32_t clk_freq_hz;
        const vsf_hw_clk_t *clksrc;
    };
    union {
        const uint8_t *clkprescaler_mapper;
        uint32_t (*getclk)(const vsf_hw_clk_t *clk, uint32_t clksrc_freq_hz, uint32_t prescaler);
    };

    uint8_t clktype;
    uint8_t clkprescaler_type;
    uint8_t clkprescaler_min;
    uint8_t clkprescaler_max;
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern uint32_t vsf_hw_clk_get(const vsf_hw_clk_t *clk);

extern void vsf_hw_clkrst_region_set(uint32_t region, uint_fast8_t value);
extern uint_fast8_t vsf_hw_clkrst_region_get(uint32_t region);

extern void vsf_hw_clkrst_region_set_bit(uint32_t region);
extern void vsf_hw_clkrst_region_clear_bit(uint32_t region);
extern uint_fast8_t vsf_hw_clkrst_region_get_bit(uint32_t region);

#endif
/* EOF */
