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
#include "../__device.h"

#include "./vendor/Include/gd32h7xx.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vsf_hw_peripheral_clksel_t {
    // RCU.CFG1
    VSF_HW_CLKSEL_HPDF                  = VSF_HW_CLKRST_REGION(0x23, 31, 1),// HPDFSEL in RCU.CFG1
    VSF_HW_CLKSEL_TIMER                 = VSF_HW_CLKRST_REGION(0x23, 24, 1),// TIMERSEL in RCU.CFG1
    VSF_HW_CLKSEL_USART5                = VSF_HW_CLKRST_REGION(0x23, 22, 2),// USART5SEL in RCU.CFG1
    VSF_HW_CLKSEL_USART2                = VSF_HW_CLKRST_REGION(0x23, 20, 2),// USART2SEL in RCU.CFG1
    VSF_HW_CLKSEL_USART1                = VSF_HW_CLKRST_REGION(0x23, 18, 2),// USART1SEL in RCU.CFG1
    VSF_HW_CLKSEL_PER                   = VSF_HW_CLKRST_REGION(0x23, 14, 2),// PERSEL in RCU.CFG1
    VSF_HW_CLKSEL_CAN2                  = VSF_HW_CLKRST_REGION(0x23, 12, 2),// CAN2SEL in RCU.CFG1
    VSF_HW_CLKSEL_CAN1                  = VSF_HW_CLKRST_REGION(0x23, 10, 2),// CAN1SEL in RCU.CFG1
    VSF_HW_CLKSEL_CAN0                  = VSF_HW_CLKRST_REGION(0x23, 8, 2), // CAN0SEL in RCU.CFG1
    VSF_HW_CLKSEL_RSPDIF                = VSF_HW_CLKRST_REGION(0x23, 4, 2), // RSPDIFSEL in RCU.CFG1
    VSF_HW_CLKSEL_USART0                = VSF_HW_CLKRST_REGION(0x23, 0, 2), // USART0SEL in RCU.CFG1

    // RCU.CFG2
    VSF_HW_CLKSEL_SAI2B1                = VSF_HW_CLKRST_REGION(0x24, 28, 3),// SAI2B1SEL in RCU.CFG2
    VSF_HW_CLKSEL_SAI2B0                = VSF_HW_CLKRST_REGION(0x24, 24, 3),// SAI2B0SEL in RCU.CFG2
    VSF_HW_CLKSEL_SAI1                  = VSF_HW_CLKRST_REGION(0x24, 20, 3),// SAI1SEL in RCU.CFG2
    VSF_HW_CLKSEL_SAI0                  = VSF_HW_CLKRST_REGION(0x24, 16, 3),// SAI0SEL in RCU.CFG2
    VSF_HW_CLKSEL_CKOUT1                = VSF_HW_CLKRST_REGION(0x24, 12, 3),// CKOUT1SEL in RCU.CFG2
    VSF_HW_CLKSEL_CKOUT0                = VSF_HW_CLKRST_REGION(0x24, 4, 3), // CKOUT0SEL in RCU.CFG2

    // RCU.CFG3
    VSF_HW_CLKSEL_ADC2                  = VSF_HW_CLKRST_REGION(0x25, 28, 2),// ADC2SEL in RCU.CFG3
    VSF_HW_CLKSEL_ADC1                  = VSF_HW_CLKRST_REGION(0x25, 26, 2),// ADC1SEL in RCU.CFG3
    VSF_HW_CLKSEL_DSPWUS                = VSF_HW_CLKRST_REGION(0x25, 24, 1),// DSPWUSSEL in RCU.CFG3
    VSF_HW_CLKSEL_SDIO1                 = VSF_HW_CLKRST_REGION(0x25, 12, 1),// SDIO1SEL in RCU.CFG3
    VSF_HW_CLKSEL_I2C3                  = VSF_HW_CLKRST_REGION(0x25, 4, 2), // I2C3SEL in RCU.CFG3
    VSF_HW_CLKSEL_I2C2                  = VSF_HW_CLKRST_REGION(0x25, 2, 2), // I2C2SEL in RCU.CFG3
    VSF_HW_CLKSEL_I2C1                  = VSF_HW_CLKRST_REGION(0x25, 0, 2), // I2C1SEL in RCU.CFG3

    // RCU.CFG4
    VSF_HW_CLKSEL_EXMC                  = VSF_HW_CLKRST_REGION(0x34, 8, 2), // EXMCSEL in RCU.CFG4
    VSF_HW_CLKSEL_SDIO0                 = VSF_HW_CLKRST_REGION(0x34, 0, 1), // SDIO0SEL in RCU.CFG4
} vsf_hw_peripheral_clksel_t;

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

/*============================ PROTOTYPES ====================================*/

static uint32_t __vsf_hw_pll_get_vco(const vsf_hw_clk_t *clk, uint32_t clksrc_freq_hz, uint32_t prescaler);

/*============================ GLOBAL VARIABLES ==============================*/

const vsf_hw_clk_t VSF_HW_CLK_HXTAL = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x00, 16, 1),// HXTALEN in RCU.CTL
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x00, 17, 1),// HXTALSTB in RCU.CTL

    .clk_freq_hz                = HXTAL_VALUE,
    .clktype                    = VSF_HW_CLK_TYPE_CONST,
};

const vsf_hw_clk_t VSF_HW_CLK_LXTAL = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x1C, 0, 1), // LXTALEN in RCU.BDCTL
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x1C, 1, 1), // LXTALSTB in RCU.BDCTL

    .clk_freq_hz                = LXTAL_VALUE,
    .clktype                    = VSF_HW_CLK_TYPE_CONST,
};

const vsf_hw_clk_t VSF_HW_CLK_IRC64MDIV = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x00, 30, 1),// IRC64MEN in RCU.CTL
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x00, 31, 1),// IRC64MSTB in RCU.CTL
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x31, 16, 2),// IRC64MDIV in RCU.ADDCTL1

    .clk_freq_hz                = IRC64M_VALUE,
    .clktype                    = VSF_HW_CLK_TYPE_CONST,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_SFT,
};

const vsf_hw_clk_t VSF_HW_CLK_LPIRC4M = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x31, 0, 1), // LPIRC4MEN in RCU.ADDCTL1
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x31, 1, 1), // LPIRC4MSTB in RCU.ADDCTL1

    .clk_freq_hz                = LPIRC4M_VALUE,
    .clktype                    = VSF_HW_CLK_TYPE_CONST,
};

const vsf_hw_clk_t VSF_HW_CLK_IRC32K = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x1D, 0, 1), // IRC32KEN in RCU.RSTSCK
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x1D, 1, 1), // IRC32KSTB in RCU.RSTSCK

    .clk_freq_hz                = IRC32K_VALUE,
    .clktype                    = VSF_HW_CLK_TYPE_CONST,
};

static const vsf_hw_clk_t * __VSF_HW_CLK_PLL_CLKSEL_MAPPER[4] = {
    &VSF_HW_CLK_IRC64MDIV, &VSF_HW_CLK_LPIRC4M, &VSF_HW_CLK_HXTAL, NULL,
};
static const vsf_hw_clk_t __VSF_HW_CLK_PLL_SRC = {
    .clksel_region              = VSF_HW_CLKRST_REGION(0x26, 16, 2), // PLLSEL in RCU.PLLALL

    .clksel_mapper              = __VSF_HW_CLK_PLL_CLKSEL_MAPPER,
    .clktype                    = VSF_HW_CLK_TYPE_SEL,
};
static const vsf_hw_clk_t __VSF_HW_CLK_PLL0_VCO = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x01, 0, 6), // PLLxPSC in RCU.PLLX
    .clksel_region              = VSF_HW_CLKRST_REGION(0x26, 0, 3), // PLLxRNG/PLLxVCOSEL in RCU.PLLALL

    .clksrc                     = &__VSF_HW_CLK_PLL_SRC,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .getclk                     = __vsf_hw_pll_get_vco,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_FUNC,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 63,
};
static const vsf_hw_clk_t __VSF_HW_CLK_PLL1_VCO = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x21, 0, 6), // PLLxPSC in RCU.PLLX
    .clksel_region              = VSF_HW_CLKRST_REGION(0x26, 4, 3), // PLLxRNG/PLLxVCOSEL in RCU.PLLALL

    .clksrc                     = &__VSF_HW_CLK_PLL_SRC,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .getclk                     = __vsf_hw_pll_get_vco,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_FUNC,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 63,
};
static const vsf_hw_clk_t __VSF_HW_CLK_PLL2_VCO = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x22, 0, 6), // PLLxPSC in RCU.PLLX
    .clksel_region              = VSF_HW_CLKRST_REGION(0x26, 8, 3), // PLLxRNG/PLLxVCOSEL in RCU.PLLALL

    .clksrc                     = &__VSF_HW_CLK_PLL_SRC,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .getclk                     = __vsf_hw_pll_get_vco,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_FUNC,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 63,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL0P = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x01, 16, 7),// PLLxP in RCU.PLLX
    .clken_region               = VSF_HW_CLKRST_REGION(0x20, 25, 1),// PLLxPEN in RCU.PLLADDCTL

    .clksrc                     = &__VSF_HW_CLK_PLL0_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL0Q = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x20, 0, 7), // PLLxQ in RCU.PLLADDCTL
    .clken_region               = VSF_HW_CLKRST_REGION(0x20, 23, 1),// PLLxQEN in RCU.PLLADDCTL

    .clksrc                     = &__VSF_HW_CLK_PLL0_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL0R = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x01, 24, 7),// PLLxR in RCU.PLLX
    .clken_region               = VSF_HW_CLKRST_REGION(0x20, 24, 1),// PLLxREN in RCU.PLLADDCTL

    .clksrc                     = &__VSF_HW_CLK_PLL0_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL1P = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x21, 16, 7),// PLLxP in RCU.PLLX
    .clken_region               = VSF_HW_CLKRST_REGION(0x20, 28, 1),// PLLxPEN in RCU.PLLADDCTL

    .clksrc                     = &__VSF_HW_CLK_PLL1_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL1Q = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x20, 8, 7), // PLLxQ in RCU.PLLADDCTL
    .clken_region               = VSF_HW_CLKRST_REGION(0x20, 26, 1),// PLLxPEN in RCU.PLLADDCTL

    .clksrc                     = &__VSF_HW_CLK_PLL1_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL1R = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x21, 24, 7),// PLLxR in RCU.PLLX
    .clken_region               = VSF_HW_CLKRST_REGION(0x20, 27, 1),// PLLxREN in RCU.PLLADDCTL

    .clksrc                     = &__VSF_HW_CLK_PLL1_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL2P = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x22, 16, 7),// PLLxP in RCU.PLLX
    .clken_region               = VSF_HW_CLKRST_REGION(0x20, 31, 1),// PLLxPEN in RCU.PLLADDCTL

    .clksrc                     = &__VSF_HW_CLK_PLL2_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL2Q = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x20, 16, 7),// PLLxQ in RCU.PLLADDCTL
    .clken_region               = VSF_HW_CLKRST_REGION(0x20, 29, 1),// PLLxQEN in RCU.PLLADDCTL

    .clksrc                     = &__VSF_HW_CLK_PLL2_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL2R = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x22, 24, 7),// PLLxR in RCU.PLLX
    .clken_region               = VSF_HW_CLKRST_REGION(0x20, 30, 1),// PLLxREN in RCU.PLLADDCTL

    .clksrc                     = &__VSF_HW_CLK_PLL2_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};

const vsf_hw_clk_t VSF_HW_CLK_RTCDIV = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x02, 16, 6),// RTCDIV in RCU.CFG0

    .clksrc                     = &VSF_HW_CLK_HXTAL,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_SFT,
    .clkprescaler_min           = 2,
    .clkprescaler_max           = 63,
};
static const vsf_hw_clk_t * __VSF_HW_CLK_RTC_CLKSEL_MAPPER[4] = {
    NULL, &VSF_HW_CLK_LXTAL, &VSF_HW_CLK_IRC32K, &VSF_HW_CLK_RTCDIV,
};
const vsf_hw_clk_t VSF_HW_CLK_RTC = {
    .clksel_region              = VSF_HW_CLKRST_REGION(0x1C, 8, 2), // RTCSRC in RCU.BDCTL

    .clksel_mapper              = __VSF_HW_CLK_RTC_CLKSEL_MAPPER,
    .clktype                    = VSF_HW_CLK_TYPE_SEL,
};

static const vsf_hw_clk_t * __VSF_HW_CLK_SYS_CLKSEL_MAPPER[4] = {
    &VSF_HW_CLK_IRC64MDIV, &VSF_HW_CLK_HXTAL, &VSF_HW_CLK_LPIRC4M, &VSF_HW_CLK_PLL0P,
};
const vsf_hw_clk_t VSF_HW_CLK_SYS = {
    .clksel_region              = VSF_HW_CLKRST_REGION(0x02, 0, 2), // SCS in RCU.CFG0

    .clksel_mapper              = __VSF_HW_CLK_SYS_CLKSEL_MAPPER,
    .clktype                    = VSF_HW_CLK_TYPE_SEL,
};

const vsf_hw_clk_t VSF_HW_CLK_PER = {
    .clksel_region              = VSF_HW_CLKRST_REGION(0x23, 14, 2),// PERSEL in RCU.CFG1

    .clksel_mapper              = __VSF_HW_CLK_PLL_CLKSEL_MAPPER,
    .clktype                    = VSF_HW_CLK_TYPE_SEL,
};

static const uint8_t __VSF_HW_CLK_AHB_PRESCALER[16] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9,
};
const vsf_hw_clk_t VSF_HW_CLK_AHB = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x02, 4, 4), // AHBPSC in RCU.CFG0

    .clksrc                     = &VSF_HW_CLK_SYS,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,
    .clkprescaler_mapper        = __VSF_HW_CLK_AHB_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_SFT,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 9,
};

static const uint8_t __VSF_HW_CLK_PCLK_PRESCALER[8] = {
    0, 0, 0, 0, 1, 2, 3, 4,
};
const vsf_hw_clk_t VSF_HW_CLK_PCLK1 = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x02, 10, 3),// APB1PSC in RCU.CFG0

    .clksrc                     = &VSF_HW_CLK_AHB,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_PCLK_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_SFT,

    .clkprescaler_min           = 0,
    .clkprescaler_max           = 4,
};
const vsf_hw_clk_t VSF_HW_CLK_PCLK2 = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x02, 13, 3),// APB2PSC in RCU.CFG0

    .clksrc                     = &VSF_HW_CLK_AHB,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_PCLK_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_SFT,

    .clkprescaler_min           = 0,
    .clkprescaler_max           = 4,
};
const vsf_hw_clk_t VSF_HW_CLK_PCLK3 = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x02, 27, 3),// APB3PSC in RCU.CFG0

    .clksrc                     = &VSF_HW_CLK_AHB,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_PCLK_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_SFT,

    .clkprescaler_min           = 0,
    .clkprescaler_max           = 4,
};
const vsf_hw_clk_t VSF_HW_CLK_PCLK4 = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x02, 24, 3),// APB2PSC in RCU.CFG0

    .clksrc                     = &VSF_HW_CLK_AHB,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_PCLK_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_SFT,

    .clkprescaler_min           = 0,
    .clkprescaler_max           = 4,
};

// USARTs

#if VSF_HAL_USE_USART == ENABLED
static const vsf_hw_clk_t * __VSF_HW_CLK_USART_APB1_CLKSEL_MAPPER[4] = {
    &VSF_HW_CLK_APB1, &VSF_HW_CLK_AHB, &VSF_HW_CLK_LXTAL, &VSF_HW_CLK_IRC64MDIV,
};
static const vsf_hw_clk_t * __VSF_HW_CLK_USART_APB2_CLKSEL_MAPPER[4] = {
    &VSF_HW_CLK_APB2, &VSF_HW_CLK_AHB, &VSF_HW_CLK_LXTAL, &VSF_HW_CLK_IRC64MDIV,
};
const vsf_hw_clk_t VSF_HW_CLK_USART0 = {
    .clksel_region              = VSF_HW_CLKSEL_USART0,
    .clksel_mapper              = __VSF_HW_CLK_USART_APB2_CLKSEL_MAPPER,
    .clktype                    = VSF_HW_CLK_TYPE_SEL,
};
const vsf_hw_clk_t VSF_HW_CLK_USART1 = {
    .clksel_region              = VSF_HW_CLKSEL_USART1,
    .clksel_mapper              = __VSF_HW_CLK_USART_APB1_CLKSEL_MAPPER,
    .clktype                    = VSF_HW_CLK_TYPE_SEL,
};
const vsf_hw_clk_t VSF_HW_CLK_USART2 = {
    .clksel_region              = VSF_HW_CLKSEL_USART2,
    .clksel_mapper              = __VSF_HW_CLK_USART_APB1_CLKSEL_MAPPER,
    .clktype                    = VSF_HW_CLK_TYPE_SEL,
};
const vsf_hw_clk_t VSF_HW_CLK_USART5 = {
    .clksel_region              = VSF_HW_CLKSEL_USART5,
    .clksel_mapper              = __VSF_HW_CLK_USART_APB2_CLKSEL_MAPPER,
    .clktype                    = VSF_HW_CLK_TYPE_SEL,
};
#endif

// SDIOs

#if VSF_HAL_USE_SDIO == ENABLED
static const vsf_hw_clk_t * __VSF_HW_CLK_SDIO_CLKSEL_MAPPER[2] = {
    &VSF_HW_CLK_PLL0Q, &VSF_HW_CLK_PLL1R,
};
const vsf_hw_clk_t VSF_HW_CLK_SDIO0 = {
    .clksel_region              = VSF_HW_CLKSEL_SDIO0,
    .clksel_mapper              = __VSF_HW_CLK_SDIO_CLKSEL_MAPPER,
    .clktype                    = VSF_HW_CLK_TYPE_SEL,
};
const vsf_hw_clk_t VSF_HW_CLK_SDIO1 = {
    .clksel_region              = VSF_HW_CLKSEL_SDIO1,
    .clksel_mapper              = __VSF_HW_CLK_SDIO_CLKSEL_MAPPER,
    .clktype                    = VSF_HW_CLK_TYPE_SEL,
};
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#include "hal/driver/common/swi/arm/vsf_swi_template.inc"

void vsf_hw_clkrst_region_set(uint32_t region, uint_fast8_t value)
{
    uint8_t bit_length = (region >> 8) & 0x07, bit_offset = (region >> 0) & 0x1F;
    VSF_HAL_ASSERT(!(value & ~((1 << bit_length) - 1)));
    uint8_t reg_word_offset = region >> 16;
    uint32_t mask = ((1 << bit_length) - 1) << bit_offset;

    vsf_atom32_op(
        &(((uint32_t *)RCU)[reg_word_offset]),
        (_ & ~mask) | (value << bit_offset)
    );
}

uint_fast8_t vsf_hw_clkrst_region_get(uint32_t region)
{
    uint8_t bit_length = (region >> 8) & 0x07, bit_offset = (region >> 0) & 0x1F;
    uint8_t reg_word_offset = region >> 16;

    return (((uint32_t *)RCU)[reg_word_offset] >> bit_offset) & ((1 << bit_length) - 1);
}

void vsf_hw_clkrst_region_set_bit(uint32_t region)
{
    VSF_HAL_ASSERT(1 == ((region >> 8) & 0x07));
    uint8_t bit_offset = (region >> 0) & 0x1F;
    uint8_t reg_word_offset = region >> 16;

    vsf_atom_or(32,
        &(((uint32_t *)RCU)[reg_word_offset]),
        (1 << bit_offset)
    );
}

void vsf_hw_clkrst_region_clear_bit(uint32_t region)
{
    VSF_HAL_ASSERT(1 == ((region >> 8) & 0x07));
    uint8_t bit_offset = (region >> 0) & 0x1F;
    uint8_t reg_word_offset = region >> 16;

    vsf_atom_and(32,
        &(((uint32_t *)RCU)[reg_word_offset]),
        ~(1 << bit_offset)
    );
}

uint_fast8_t vsf_hw_clkrst_region_get_bit(uint32_t region)
{
    VSF_HAL_ASSERT(1 == ((region >> 8) & 0x07));
    uint8_t bit_offset = (region >> 0) & 0x1F;
    uint8_t reg_word_offset = region >> 16;

    return (((uint32_t *)RCU)[reg_word_offset] >> bit_offset) & 1;
}



static uint32_t __vsf_hw_pll_get_vco(const vsf_hw_clk_t *clk, uint32_t clksrc_freq_hz, uint32_t prescaler)
{
    uint32_t *pllall = &(((uint32_t *)RCU)[clk->clksel_region >> 16]);
    uint32_t *pll = &(((uint32_t *)RCU)[clk->clkprescaler_region >> 16]);
    uint32_t n = (pll[0] >> 6) & 0x1FF;
    uint8_t pllidx = (clk->clksel_region & 0x1F) >> 2;
    VSF_HAL_ASSERT(pllidx < 3);
    uint32_t frac = pllall[1 + pllidx];
    frac = (frac & 0x1FFF) * ((frac & (1 << 15)) ? 1 : 0);

    VSF_HAL_ASSERT(prescaler < 64);
    if (!prescaler) { prescaler = 1; }
    return (uint32_t)(((float)clksrc_freq_hz / prescaler) * ((float)n + ((float)frac / 0x2000) + 1));
}

const vsf_hw_clk_t * vsf_hw_clk_get_src(const vsf_hw_clk_t *clk)
{
    VSF_HAL_ASSERT(clk != NULL);

    switch (clk->clktype) {
    case VSF_HW_CLK_TYPE_CLK:
        return clk->clksrc;
    case VSF_HW_CLK_TYPE_SEL:
        return clk->clksel_mapper[vsf_hw_clkrst_region_get(clk->clksel_region)];
    default:
        return NULL;
    }
}

uint32_t vsf_hw_clk_get_freq_hz(const vsf_hw_clk_t *clk)
{
    VSF_HAL_ASSERT(clk != NULL);

    if (    (clk->clken_region != 0)
        &&  !vsf_hw_clkrst_region_get_bit(clk->clken_region)) {
        return 0;
    }

    uint32_t clk_freq_hz;
    switch (clk->clktype) {
    case VSF_HW_CLK_TYPE_CONST:
        clk_freq_hz = clk->clk_freq_hz;
        break;
    case VSF_HW_CLK_TYPE_CLK:
        VSF_HAL_ASSERT(clk->clksrc != NULL);
        clk_freq_hz = vsf_hw_clk_get_freq_hz(clk->clksrc);
        break;
    case VSF_HW_CLK_TYPE_SEL: {
            const vsf_hw_clk_t *clksrc = clk->clksel_mapper[vsf_hw_clkrst_region_get(clk->clksel_region)];
            if (NULL == clksrc) {
                return 0;
            } else {
                clk_freq_hz = vsf_hw_clk_get_freq_hz(clksrc);
            }
        }
        break;
    default:
        VSF_HAL_ASSERT(false);
        return 0;
    }

    if (clk->clkprescaler_region != 0) {
        bool isfunc = clk->clkprescaler_type == VSF_HW_CLK_PRESCALER_FUNC;
        uint32_t prescaler =  vsf_hw_clkrst_region_get(clk->clkprescaler_region);
        if ((clk->clkprescaler_mapper != NULL) && !isfunc) {
            prescaler = clk->clkprescaler_mapper[prescaler];
        }
        if ((prescaler < clk->clkprescaler_min) || (prescaler > clk->clkprescaler_max)) {
            clk_freq_hz = 0;
        } else {
            switch (clk->clkprescaler_type) {
            case VSF_HW_CLK_PRESCALER_DIV:
                VSF_HAL_ASSERT(prescaler > 0);
                clk_freq_hz /= prescaler;
                break;
            case VSF_HW_CLK_PRESCALER_ADD1_DIV:
                clk_freq_hz /= prescaler + 1;
                break;
            case VSF_HW_CLK_PRESCALER_SFT:
                clk_freq_hz >>= prescaler;
                break;
            case VSF_HW_CLK_PRESCALER_FUNC:
                clk_freq_hz = clk->getclk(clk, clk_freq_hz, prescaler);
                break;
            }
        }
    }
    return clk_freq_hz;
}



// this function should be in gd32h7xx_misc.c, but this file is not included.
//  So implement this function here. Use weak in case user include this file.
VSF_CAL_WEAK(nvic_vector_table_set)
void nvic_vector_table_set(uint32_t nvic_vict_tab, uint32_t offset)
{
    SCB->VTOR = nvic_vict_tab | (offset & NVIC_VECTTAB_OFFSET_MASK);
    __DSB();
}




/*! \note initialize device driver
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
VSF_CAL_WEAK(vsf_driver_init)
bool vsf_driver_init(void)
{
    return true;
}

/* EOF */
