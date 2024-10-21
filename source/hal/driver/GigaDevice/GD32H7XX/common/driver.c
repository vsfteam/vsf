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
    VSF_HW_CLK_PRESCALER_NONE           = 0,
    VSF_HW_CLK_PRESCALER_CONST,
    VSF_HW_CLK_PRESCALER_DIV,
    VSF_HW_CLK_PRESCALER_ADD1_DIV,
    VSF_HW_CLK_PRESCALER_SFT,
    VSF_HW_CLK_PRESCALER_ADD1_SFT,
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
        uint32_t prescaler;
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
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 3,
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
const vsf_hw_clk_t VSF_HW_CLK_PLL0_VCO = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x01, 0, 6), // PLLxPSC in RCU.PLLX
    .clksel_region              = VSF_HW_CLKRST_REGION(0x26, 0, 3), // PLLxRNG/PLLxVCOSEL in RCU.PLLALL
    .clken_region               = VSF_HW_CLKRST_REGION(0x00, 24, 1),// PLLxEN in RCU.CTL
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x00, 25, 1),// PLLxSTB in RCU.CTL

    .clksrc                     = &__VSF_HW_CLK_PLL_SRC,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .getclk                     = __vsf_hw_pll_get_vco,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_FUNC,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 63,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL1_VCO = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x21, 0, 6), // PLLxPSC in RCU.PLLX
    .clksel_region              = VSF_HW_CLKRST_REGION(0x26, 4, 3), // PLLxRNG/PLLxVCOSEL in RCU.PLLALL
    .clken_region               = VSF_HW_CLKRST_REGION(0x00, 26, 1),// PLLxEN in RCU.CTL
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x00, 27, 1),// PLLxSTB in RCU.CTL

    .clksrc                     = &__VSF_HW_CLK_PLL_SRC,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .getclk                     = __vsf_hw_pll_get_vco,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_FUNC,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 63,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL2_VCO = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x22, 0, 6), // PLLxPSC in RCU.PLLX
    .clksel_region              = VSF_HW_CLKRST_REGION(0x26, 8, 3), // PLLxRNG/PLLxVCOSEL in RCU.PLLALL
    .clken_region               = VSF_HW_CLKRST_REGION(0x00, 28, 1),// PLLxEN in RCU.CTL
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x00, 29, 1),// PLLxSTB in RCU.CTL

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

    .clksrc                     = &VSF_HW_CLK_PLL0_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL0Q = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x20, 0, 7), // PLLxQ in RCU.PLLADDCTL
    .clken_region               = VSF_HW_CLKRST_REGION(0x20, 23, 1),// PLLxQEN in RCU.PLLADDCTL

    .clksrc                     = &VSF_HW_CLK_PLL0_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL0R = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x01, 24, 7),// PLLxR in RCU.PLLX
    .clken_region               = VSF_HW_CLKRST_REGION(0x20, 24, 1),// PLLxREN in RCU.PLLADDCTL

    .clksrc                     = &VSF_HW_CLK_PLL0_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL1P = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x21, 16, 7),// PLLxP in RCU.PLLX
    .clken_region               = VSF_HW_CLKRST_REGION(0x20, 28, 1),// PLLxPEN in RCU.PLLADDCTL

    .clksrc                     = &VSF_HW_CLK_PLL1_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL1Q = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x20, 8, 7), // PLLxQ in RCU.PLLADDCTL
    .clken_region               = VSF_HW_CLKRST_REGION(0x20, 26, 1),// PLLxPEN in RCU.PLLADDCTL

    .clksrc                     = &VSF_HW_CLK_PLL1_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL1R = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x21, 24, 7),// PLLxR in RCU.PLLX
    .clken_region               = VSF_HW_CLKRST_REGION(0x20, 27, 1),// PLLxREN in RCU.PLLADDCTL

    .clksrc                     = &VSF_HW_CLK_PLL1_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL2P = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x22, 16, 7),// PLLxP in RCU.PLLX
    .clken_region               = VSF_HW_CLKRST_REGION(0x20, 31, 1),// PLLxPEN in RCU.PLLADDCTL

    .clksrc                     = &VSF_HW_CLK_PLL2_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL2Q = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x20, 16, 7),// PLLxQ in RCU.PLLADDCTL
    .clken_region               = VSF_HW_CLKRST_REGION(0x20, 29, 1),// PLLxQEN in RCU.PLLADDCTL

    .clksrc                     = &VSF_HW_CLK_PLL2_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL2R = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x22, 24, 7),// PLLxR in RCU.PLLX
    .clken_region               = VSF_HW_CLKRST_REGION(0x20, 30, 1),// PLLxREN in RCU.PLLADDCTL

    .clksrc                     = &VSF_HW_CLK_PLL2_VCO,
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
    .clkprescaler_max           = 15,
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

// TLI

const vsf_hw_clk_t VSF_HW_CLK_TLI = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x23, 16, 2),// PLL2RDIV in RCU.CFG1

    .clksrc                     = &VSF_HW_CLK_PLL2R,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_SFT,

    .clkprescaler_min           = 0,
    .clkprescaler_max           = 3,
};

// EXMC

static const vsf_hw_clk_t * __VSF_HW_CLK_EXMC_CLKSEL_MAPPER[4] = {
    &VSF_HW_CLK_AHB, &VSF_HW_CLK_PLL0Q, &VSF_HW_CLK_PLL1R, &VSF_HW_CLK_PER,
};
const vsf_hw_clk_t VSF_HW_CLK_EXMC = {
    .clksel_region              = VSF_HW_CLKSEL_EXMC,
    .clksel_mapper              = __VSF_HW_CLK_EXMC_CLKSEL_MAPPER,
    .clktype                    = VSF_HW_CLK_TYPE_SEL,
};

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

static uint32_t __vsf_hw_clk_get_src_freq_hz(const vsf_hw_clk_t *clk)
{
    VSF_HAL_ASSERT(clk != NULL);
    switch (clk->clktype) {
    case VSF_HW_CLK_TYPE_CONST:
        return clk->clk_freq_hz;
    case VSF_HW_CLK_TYPE_CLK:
        VSF_HAL_ASSERT(clk->clksrc != NULL);
        return vsf_hw_clk_get_freq_hz(clk->clksrc);
    case VSF_HW_CLK_TYPE_SEL: {
            const vsf_hw_clk_t *clksrc = clk->clksel_mapper[vsf_hw_clkrst_region_get(clk->clksel_region)];
            if (NULL == clksrc) {
                return 0;
            } else {
                return vsf_hw_clk_get_freq_hz(clksrc);
            }
        }
        break;
    default:
        VSF_HAL_ASSERT(false);
        return 0;
    }
}

uint32_t vsf_hw_clk_get_freq_hz(const vsf_hw_clk_t *clk)
{
    VSF_HAL_ASSERT(clk != NULL);

    if (    (clk->clken_region != 0)
        &&  !vsf_hw_clkrst_region_get_bit(clk->clken_region)) {
        return 0;
    }

    uint32_t clk_freq_hz = __vsf_hw_clk_get_src_freq_hz(clk);

    if (VSF_HW_CLK_PRESCALER_CONST == clk->clkprescaler_type) {
        VSF_HAL_ASSERT(clk->prescaler > 0);
        clk_freq_hz /= clk->prescaler;
    } else if (clk->clkprescaler_type != VSF_HW_CLK_PRESCALER_NONE) {
        VSF_HAL_ASSERT(clk->clkprescaler_region != 0);
        bool isfunc = clk->clkprescaler_type == VSF_HW_CLK_PRESCALER_FUNC;
        uint32_t prescaler =  vsf_hw_clkrst_region_get(clk->clkprescaler_region);
        if ((clk->clkprescaler_mapper != NULL) && !isfunc) {
            prescaler = clk->clkprescaler_mapper[prescaler];
        }
        if ((prescaler < clk->clkprescaler_min) || (prescaler > clk->clkprescaler_max)) {
            clk_freq_hz = 0;
        } else {
            switch (clk->clkprescaler_type) {
            case VSF_HW_CLK_PRESCALER_ADD1_DIV:
                prescaler++;
                // fall through
            case VSF_HW_CLK_PRESCALER_DIV:
                VSF_HAL_ASSERT(prescaler > 0);
                clk_freq_hz /= prescaler;
                break;
            case VSF_HW_CLK_PRESCALER_ADD1_SFT:
                prescaler++;
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

void vsf_hw_clk_enable(const vsf_hw_clk_t *clk)
{
    VSF_HAL_ASSERT(clk != NULL);
    if (clk->clken_region != 0) {
        vsf_hw_clkrst_region_set_bit(clk->clken_region);
    }
    if (clk->clkrdy_region != 0) {
        while (!vsf_hw_clkrst_region_get_bit(clk->clkrdy_region));
    }
}

void vsf_hw_clk_disable(const vsf_hw_clk_t *clk)
{
    VSF_HAL_ASSERT(clk != NULL);
    if (clk->clken_region != 0) {
        vsf_hw_clkrst_region_clear_bit(clk->clken_region);
    }
}

vsf_err_t vsf_hw_clk_config(const vsf_hw_clk_t *clk, const vsf_hw_clk_t *clksrc, uint16_t prescaler, uint32_t freq_hz)
{
    VSF_HAL_ASSERT(clk != NULL);
    if (clksrc != NULL) {
        VSF_HAL_ASSERT(clk->clksel_region != 0);
        if (clk->clktype != VSF_HW_CLK_TYPE_SEL) {
            VSF_HAL_ASSERT(false);
            return VSF_ERR_INVALID_PARAMETER;
        }

        uint8_t clksel_num = 1 << ((clk->clksel_region >> 8) & 0xFF);
        uint8_t clksel_idx = 0;
        for (; clksel_idx < clksel_num; clksel_idx++) {
            if (clk->clksel_mapper[clksel_idx] == clksrc) {
                break;
            }
        }
        if (clksel_idx >= clksel_num) {
            VSF_HAL_ASSERT(false);
            return VSF_ERR_INVALID_PARAMETER;
        }

        vsf_hw_clkrst_region_set(clk->clksel_region, clksel_idx);
    }

    if (prescaler != 0) {
        VSF_HAL_ASSERT(clk->clkprescaler_region != 0);

        switch (clk->clkprescaler_type) {
        case VSF_HW_CLK_PRESCALER_CONST:
            VSF_HAL_ASSERT(false);
            break;
        case VSF_HW_CLK_PRESCALER_DIV:
            break;
        case VSF_HW_CLK_PRESCALER_ADD1_SFT:
            VSF_HAL_ASSERT(!(prescaler & (prescaler - 1)));
            prescaler = vsf_msb32(prescaler);
            // fall through
        case VSF_HW_CLK_PRESCALER_ADD1_DIV:
            prescaler--;
            break;
        case VSF_HW_CLK_PRESCALER_SFT:
            VSF_HAL_ASSERT(!(prescaler & (prescaler - 1)));
            prescaler = vsf_msb32(prescaler);
            break;
        case VSF_HW_CLK_PRESCALER_FUNC:
            VSF_HAL_ASSERT(false);
            return VSF_ERR_INVALID_PARAMETER;
        }

        if (clk->clkprescaler_mapper != NULL) {
            uint8_t prescaler_num = 1 << ((clk->clkprescaler_region >> 8) & 0xFF);
            for (uint8_t i = 0; i < prescaler_num; i++) {
                if (clk->clkprescaler_mapper[i] == prescaler) {
                    prescaler = i;
                    break;
                }
            }
        }

        VSF_HAL_ASSERT((prescaler >= clk->clkprescaler_min) && (prescaler <= clk->clkprescaler_max));
        vsf_hw_clkrst_region_set(clk->clkprescaler_region, prescaler);
    } else if (freq_hz != 0) {
        uint32_t clk_freq_hz = __vsf_hw_clk_get_src_freq_hz(clk);
        VSF_HAL_ASSERT(clk_freq_hz >= freq_hz);
        return vsf_hw_clk_config(clk, NULL, clk_freq_hz / freq_hz, 0);
    }
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_pll_vco_config(const vsf_hw_clk_t *clk, uint_fast8_t src_prescaler, uint32_t vco_freq_hz)
{
    VSF_HAL_ASSERT((clk == &VSF_HW_CLK_PLL0_VCO) || (clk == &VSF_HW_CLK_PLL1_VCO) || (clk == &VSF_HW_CLK_PLL2_VCO));
    VSF_HAL_ASSERT((src_prescaler > 0) && (src_prescaler < 64));
    VSF_HAL_ASSERT((vco_freq_hz >= 150000000) && (vco_freq_hz <= 836000000));

    uint32_t *pll = &(((uint32_t *)RCU)[clk->clkprescaler_region >> 16]);
    uint32_t *pllfrac = &(((uint32_t *)RCU)[(clk->clksel_region >> 16) + 1 + ((clk->clksel_region & 0x1F) >> 2)]);

    uint32_t clksrc_freq_hz = vsf_hw_clk_get_freq_hz(clk->clksrc) / src_prescaler;
    VSF_HAL_ASSERT((clksrc_freq_hz >= 1000000) && (clksrc_freq_hz <= 16000000));

    uint8_t clk_range;
    if (clksrc_freq_hz < 2000000) {
        clk_range = 0;
    } else if (clksrc_freq_hz < 4000000) {
        clk_range = 1;
    } else if (clksrc_freq_hz < 8000000) {
        clk_range = 2;
    } else {
        clk_range = 3;
    }

    if (vco_freq_hz > 420000000) {
        VSF_HAL_ASSERT(vco_freq_hz >= 192000000);
    } else if (vco_freq_hz < 192000000) {
        clk_range |= 1 << 2;
    }
    vsf_hw_clkrst_region_set(clk->clksel_region, clk_range);

    uint32_t clk_div = vco_freq_hz / clksrc_freq_hz;
    uint32_t clkfrac_div = ((uint64_t)(vco_freq_hz % clksrc_freq_hz)) * (1 << 13) / clksrc_freq_hz;
    VSF_HAL_ASSERT((clk_div >= 9) && (clk_div <= 512));

    *pll = (*pll & ~0x7FFF) | src_prescaler | ((clk_div - 1) << 6);
    *pllfrac = clkfrac_div == 0 ? 0 : clkfrac_div | (1 << 15);
    return VSF_ERR_NONE;
}



// this function should be in gd32h7xx_misc.c, but this file is not included.
//  So implement this function here. Use weak in case user include this file.
VSF_CAL_WEAK(nvic_vector_table_set)
void nvic_vector_table_set(uint32_t nvic_vict_tab, uint32_t offset)
{
    SCB->VTOR = nvic_vict_tab | (offset & NVIC_VECTTAB_OFFSET_MASK);
    __DSB();
}





#if VSF_ARCH_CFG_CALLSTACK_TRACE == ENABLED

#include "service/trace/vsf_trace.h"

// re-write HardFault_Handler to dump stack
void HardFault_Handler(void)
{
    uintptr_t stack = vsf_arch_get_stack();
    uintptr_t callstack[16] = { 0 };
    uint_fast16_t num = vsf_arch_get_callstack(stack, callstack, dimof(callstack));

    vsf_trace_error("Hardfault:" VSF_TRACE_CFG_LINEEND);

    uint32_t cfsr = SCB->CFSR;
    vsf_trace_error("CFSR: 0x%08X" VSF_TRACE_CFG_LINEEND, cfsr);

    if (cfsr & 0xFF) {
        if (cfsr & SCB_CFSR_IACCVIOL_Msk) {
            vsf_trace_error("Instruction access violation");
        }
        if (cfsr & SCB_CFSR_DACCVIOL_Msk) {
            vsf_trace_error("Data access violation");
        }
        if (cfsr & SCB_CFSR_MUNSTKERR_Msk) {
            vsf_trace_error("MemManage fault on unstacking for a return from exception");
        }
        if (cfsr & SCB_CFSR_MSTKERR_Msk) {
            vsf_trace_error("MemManage fault on stacking for exception entry");
        }
        if (cfsr & SCB_CFSR_MLSPERR_Msk) {
            vsf_trace_error("MemManage fault during floating-point lazy state preservation");
        }
        if (cfsr & SCB_CFSR_MMARVALID_Msk) {
            vsf_trace_error(" at 0x%08X" VSF_TRACE_CFG_LINEEND, SCB->MMFAR);
        } else {
            vsf_trace_error(VSF_TRACE_CFG_LINEEND);
        }
    }
    if (cfsr & 0xFF00) {
        if (cfsr & SCB_CFSR_IBUSERR_Msk) {
            vsf_trace_error("Instruction bus error");
        }
        if (cfsr & SCB_CFSR_PRECISERR_Msk) {
            vsf_trace_error("Precise data bus error");
        }
        if (cfsr & SCB_CFSR_IMPRECISERR_Msk) {
            vsf_trace_error("Imprecise data bus error");
        }
        if (cfsr & SCB_CFSR_UNSTKERR_Msk) {
            vsf_trace_error("BusFault on unstacking for a return from exception");
        }
        if (cfsr & SCB_CFSR_STKERR_Msk) {
            vsf_trace_error("BusFault on stacking for exception entry");
        }
        if (cfsr & SCB_CFSR_LSPERR_Msk) {
            vsf_trace_error("BusFault during floating-point lazy state preservation");
        }
        if (cfsr & SCB_CFSR_BFARVALID_Msk) {
            vsf_trace_error(" at 0x%08X" VSF_TRACE_CFG_LINEEND, SCB->BFAR);
        } else {
            vsf_trace_error(VSF_TRACE_CFG_LINEEND);
        }
    }
    if (cfsr & 0xFFFF0000) {
        if (cfsr & SCB_CFSR_UNDEFINSTR_Msk) {
            vsf_trace_error("Undefined instruction" VSF_TRACE_CFG_LINEEND);
        }
        if (cfsr & SCB_CFSR_INVSTATE_Msk) {
            vsf_trace_error("Invalid state" VSF_TRACE_CFG_LINEEND);
        }
        if (cfsr & SCB_CFSR_INVPC_Msk) {
            vsf_trace_error("Invalid PC" VSF_TRACE_CFG_LINEEND);
        }
        if (cfsr & SCB_CFSR_NOCP_Msk) {
            vsf_trace_error("No coprocessor" VSF_TRACE_CFG_LINEEND);
        }
        if (cfsr & SCB_CFSR_UNALIGNED_Msk) {
            vsf_trace_error("Unaligned access" VSF_TRACE_CFG_LINEEND);
        }
        if (cfsr & SCB_CFSR_DIVBYZERO_Msk) {
            vsf_trace_error("Divide by zero" VSF_TRACE_CFG_LINEEND);
        }
    }

    vsf_trace_dump_stack();
    while (1);
}

void MemManage_Handler(void)
{
    HardFault_Handler();
}

void BusFault_Handler(void)
{
    HardFault_Handler();
}

#endif





// the region added later will have higher priority
void vsf_hw_mpu_add_basic_resgions(void)
{
    vsf_arch_mpu_add_region(0x24000000, 1 * 1024 * 1024,
                            VSF_ARCH_MPU_NON_SHARABLE       |
                            VSF_ARCH_MPU_EXECUTABLE         |
                            VSF_ARCH_MPU_ACCESS_FULL        |
                            VSF_ARCH_MPU_CACHABLE_WRITE_THROUGH_NOALLOC);

    vsf_arch_mpu_add_region(0x30000000, 32 * 1024,
                            VSF_ARCH_MPU_NON_SHARABLE       |
                            VSF_ARCH_MPU_EXECUTABLE         |
                            VSF_ARCH_MPU_ACCESS_FULL        |
                            VSF_ARCH_MPU_CACHABLE_WRITE_BACK_NOALLOC);

    vsf_arch_mpu_add_region(0x08000000, 4 * 1024 * 1024,
                            VSF_ARCH_MPU_NON_SHARABLE       |
                            VSF_ARCH_MPU_EXECUTABLE         |
                            VSF_ARCH_MPU_ACCESS_FULL        |
                            VSF_ARCH_MPU_CACHABLE_WRITE_THROUGH_NOALLOC);
}





/*! \note initialize device driver
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
VSF_CAL_WEAK(vsf_driver_init)
bool vsf_driver_init(void)
{
    vsf_hw_mpu_add_basic_resgions();
    return true;
}

/* EOF */
