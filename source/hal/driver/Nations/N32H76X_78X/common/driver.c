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

#include "hal/driver/vendor_driver.h"

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
    union {
        uint32_t clksel_region;
        uint32_t clkdly_region;         // for HSE/HSI/MSI/LISI/LSE/SECLSI constant clocks(clksrc_type is VSF_HW_CLK_TYPE_CONST)
    };
    union {
        uint32_t clkprescaler_region;
        uint32_t clkdly;                // for HSE/HSI/MSI/LISI/LSE/SECLSI constant clocks(clksrc_type is VSF_HW_CLK_TYPE_CONST)
    };
    uint32_t clken_region;
    uint32_t clkrdy_region;

    union {
        const vsf_hw_clk_t * const *clksel_mapper;
        uint32_t clk_freq_hz;
        const vsf_hw_clk_t *clksrc;
    };
    union {
        const uint16_t *clkprescaler_mapper;
        uint32_t prescaler;
        uint32_t (*getclk)(const vsf_hw_clk_t *clk, uint32_t clksrc_freq_hz, uint32_t prescaler);
        uint32_t clkdly_en_region;
    };

    uint8_t clksrc_type;
    uint8_t clkprescaler_type;
    uint8_t clkprescaler_min;
    uint8_t clkprescaler_max;
};

/*============================ PROTOTYPES ====================================*/

static uint32_t __vsf_hw_pll_getclk(const vsf_hw_clk_t *clk, uint32_t clksrc_freq_hz, uint32_t prescaler);

/*============================ GLOBAL VARIABLES ==============================*/

#ifdef HSE_VALUE
const vsf_hw_clk_t VSF_HW_CLK_HSE = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x0C, 2, 1), // RCC_SRCCTRL1.HSEEN
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x0C, 3, 1), // RCC_SRCCTRL1.HSERDF

    .clk_freq_hz                = HSE_VALUE,
    .clksrc_type                = VSF_HW_CLK_TYPE_CONST,
#   ifdef HSE_STARTUP_TIMEOUT
    .clkdly                     = HSE_STARTUP_TIMEOUT,
    .clkdly_en_region           = VSF_HW_CLKRST_REGION(0x0C, 8, 1), // RCC_SRCCTRL1.HSERDCNTEN
    .clkdly_region              = VSF_HW_CLKRST_REGION(0x68, 0, 32),// RCC_HSERDDL.DELAY
#   endif
};

// s_hse_rdy_cg_clk
const vsf_hw_clk_t VSF_HW_CLK_HSE_CG = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x5A, 13, 1),// RCC_CFG4.HSECGEN

    .clksrc                     = &VSF_HW_CLK_HSE,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,
};

// s_hse_rdy_ker_cg_clk
const vsf_hw_clk_t VSF_HW_CLK_HSE_KER_CG = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x5A, 12, 1),// RCC_CFG4.HSEKERCGEN

    .clksrc                     = &VSF_HW_CLK_HSE,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,
};
#endif

#ifdef LSE_VALUE
const vsf_hw_clk_t VSF_HW_CLK_LSE = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x49, 4, 1), // RCC_BDCTRL.LSEEN
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x49, 5, 1), // RCC_BDCTRL.LSERDF

    .clk_freq_hz                = LSE_VALUE,
    .clksrc_type                = VSF_HW_CLK_TYPE_CONST,
#   ifdef LSE_STARTUP_TIMEOUT
    .clkdly                     = LSE_STARTUP_TIMEOUT,
    .clkdly_region              = VSF_HW_CLKRST_REGION(0x66, 0, 32),// RCC_LSERDDL.DELAY
    .clkdly_en_region          = VSF_HW_CLKRST_REGION(0x49, 22, 1),// RCC_BDCTRL.LSERDCNTEN
#   endif
};
#endif

const vsf_hw_clk_t VSF_HW_CLK_HSI = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x0C, 0, 1), // RCC_SRCCTRL1.HSIEN
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x0C, 1, 1), // RCC_SRCCTRL1.HSIRDF
    // hsi has no clock delay register

    .clk_freq_hz                = HSI_VALUE,
    .clksrc_type                = VSF_HW_CLK_TYPE_CONST,
#   ifdef HSI_STARTUP_TIMEOUT
    .clkdly                     = HSI_STARTUP_TIMEOUT,
#   endif
};

static const uint16_t __VSF_HW_CLK_SYSBUS_PRESCALER[16] = {
    1, 2, 4, 0, 8, 0, 0, 16, 32, 64, 128, 256, 512, 0, 0, 0
};
static const vsf_hw_clk_t __VSF_HW_CLK_HSI_RDY = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x10, 20, 4),// RCC_SYSBUSDIV1.HSIDIV

    .clksrc                     = &VSF_HW_CLK_HSI,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,
    .clkprescaler_mapper        = __VSF_HW_CLK_SYSBUS_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = dimof(__VSF_HW_CLK_SYSBUS_PRESCALER) - 1,
};

const vsf_hw_clk_t VSF_HW_CLK_HSI_CG = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x5A, 15, 1),// RCC_CFG4.HSICGEN

    .clksrc                     = &__VSF_HW_CLK_HSI_RDY,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,
};

const vsf_hw_clk_t VSF_HW_CLK_HSI_KER_CG = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x5A, 14, 1),// RCC_CFG4.HSIKERCGEN

    .clksrc                     = &__VSF_HW_CLK_HSI_RDY,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,
};

const vsf_hw_clk_t VSF_HW_CLK_MSI = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x0C, 6, 1), // RCC_SRCCTRL1.MSIEN
    .clkdly_region              = VSF_HW_CLKRST_REGION(0x65, 0, 32),// RCC_MSIRDDL.DELAY

    .clk_freq_hz                = MSI_VALUE,
    .clksrc_type                = VSF_HW_CLK_TYPE_CONST,
#   ifdef MSI_STARTUP_TIMEOUT
    .clkdly                     = MSI_STARTUP_TIMEOUT,
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x0C, 7, 1), // RCC_SRCCTRL1.MSIRDF
    .clkdly_en_region           = VSF_HW_CLKRST_REGION(0x0C, 22, 1),// RCC_SRCCTRL1.MSIRDCNTEN
#   endif
};

static const vsf_hw_clk_t __VSF_HW_CLK_MSI_RDY = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x10, 4, 4), // RCC_SYSBUSDIV1.MSIDIV

    .clksrc                     = &VSF_HW_CLK_MSI,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,
    .clkprescaler_mapper        = __VSF_HW_CLK_SYSBUS_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = dimof(__VSF_HW_CLK_SYSBUS_PRESCALER) - 1,
};

const vsf_hw_clk_t VSF_HW_CLK_MSI_CG = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x5A, 11, 1),// RCC_CFG4.MSICGEN

    .clksrc                     = &__VSF_HW_CLK_MSI_RDY,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,
};

const vsf_hw_clk_t VSF_HW_CLK_MSI_KER_CG = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x5A, 10, 1),// RCC_CFG4.MSIKERCGEN

    .clksrc                     = &__VSF_HW_CLK_MSI_RDY,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,
};

const vsf_hw_clk_t VSF_HW_CLK_LSI = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x49, 0, 1), // RCC_BDCTRL.LSIEN

    .clk_freq_hz                = LSI_VALUE,
    .clksrc_type                = VSF_HW_CLK_TYPE_CONST,
#   ifdef LSI_STARTUP_TIMEOUT
    .clkdly                     = LSI_STARTUP_TIMEOUT,
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x49, 1, 1), // RCC_BDCTRL.LSIRDF
#   endif
};

static const vsf_hw_clk_t * const __VSF_HW_CLK_PLL_CLKSEL_MAPPER[4] = {
    &VSF_HW_CLK_HSI, NULL, &VSF_HW_CLK_MSI, &VSF_HW_CLK_HSE,
};

const vsf_hw_clk_t VSF_HW_CLK_PLL1 = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x00, 0, 32),// RCC_PLL1CTRL1

    .clksel_mapper              = __VSF_HW_CLK_PLL_CLKSEL_MAPPER,
    .clksrc_type                = VSF_HW_CLK_TYPE_SEL,

    .getclk                     = __vsf_hw_pll_getclk,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_FUNC,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL2 = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x04, 0, 32),// RCC_PLL2CTRL1

    .clksel_mapper              = __VSF_HW_CLK_PLL_CLKSEL_MAPPER,
    .clksrc_type                = VSF_HW_CLK_TYPE_SEL,

    .getclk                     = __vsf_hw_pll_getclk,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_FUNC,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL3 = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x08, 0, 32),// RCC_PLL3CTRL1

    .clksel_mapper              = __VSF_HW_CLK_PLL_CLKSEL_MAPPER,
    .clksrc_type                = VSF_HW_CLK_TYPE_SEL,

    .getclk                     = __vsf_hw_pll_getclk,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_FUNC,
};
const vsf_hw_clk_t VSF_HW_CLK_SHRPLL = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x63, 0, 32),// RCC_SHRPLLCTRL1

    .clksel_mapper              = __VSF_HW_CLK_PLL_CLKSEL_MAPPER,
    .clksrc_type                = VSF_HW_CLK_TYPE_SEL,

    .getclk                     = __vsf_hw_pll_getclk,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_FUNC,
};

const vsf_hw_clk_t VSF_HW_CLK_PLL1A = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x0D, 0, 6), // RCC_PLL1DIV.PLL1ADIV

    .clksrc                     = &VSF_HW_CLK_PLL1,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
    .clkprescaler_min           = 1,
    .clkprescaler_max           = 63,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL1B = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x0D, 8, 6), // RCC_PLL1DIV.PLL1BDIV

    .clksrc                     = &VSF_HW_CLK_PLL1,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
    .clkprescaler_min           = 1,
    .clkprescaler_max           = 63,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL1C = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x0D, 16, 6),// RCC_PLL1DIV.PLL1CDIV

    .clksrc                     = &VSF_HW_CLK_PLL1,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
    .clkprescaler_min           = 1,
    .clkprescaler_max           = 63,
};

const vsf_hw_clk_t VSF_HW_CLK_PLL2A = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x0E, 0, 6), // RCC_PLL2DIV.PLL2ADIV

    .clksrc                     = &VSF_HW_CLK_PLL1,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
    .clkprescaler_min           = 1,
    .clkprescaler_max           = 63,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL2B = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x0E, 8, 6), // RCC_PLL2DIV.PLL2BDIV

    .clksrc                     = &VSF_HW_CLK_PLL1,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
    .clkprescaler_min           = 1,
    .clkprescaler_max           = 63,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL2C = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x0E, 16, 6),// RCC_PLL2DIV.PLL2CDIV

    .clksrc                     = &VSF_HW_CLK_PLL1,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
    .clkprescaler_min           = 1,
    .clkprescaler_max           = 63,
};

const vsf_hw_clk_t VSF_HW_CLK_PLL3A = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x0F, 0, 6), // RCC_PLL3DIV.PLL3ADIV

    .clksrc                     = &VSF_HW_CLK_PLL1,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
    .clkprescaler_min           = 1,
    .clkprescaler_max           = 63,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL3B = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x0F, 8, 6), // RCC_PLL3DIV.PLL3BDIV

    .clksrc                     = &VSF_HW_CLK_PLL1,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
    .clkprescaler_min           = 1,
    .clkprescaler_max           = 63,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL3C = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x0F, 16, 6),// RCC_PLL3DIV.PLL3CDIV

    .clksrc                     = &VSF_HW_CLK_PLL1,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
    .clkprescaler_min           = 1,
    .clkprescaler_max           = 63,
};

static const vsf_hw_clk_t * const __VSF_HW_CLK_SYS_CLKSEL_MAPPER[4] = {
    &VSF_HW_CLK_HSI_CG, &VSF_HW_CLK_MSI_CG, &VSF_HW_CLK_HSE_CG, &VSF_HW_CLK_PLL1A,
};
const vsf_hw_clk_t VSF_HW_CLK_SYS = {
    .clksel_region              = VSF_HW_CLKRST_REGION(0x0C, 24, 2),// RCC_SRCCTRL1.SCLKSW
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x10, 0, 4), // RCC_SYSBUSDIV1.SCLKDIV

    .clksel_mapper              = __VSF_HW_CLK_SYS_CLKSEL_MAPPER,
    .clksrc_type                = VSF_HW_CLK_TYPE_SEL,

    .clkprescaler_mapper        = __VSF_HW_CLK_SYSBUS_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = dimof(__VSF_HW_CLK_SYSBUS_PRESCALER) - 1,
};

const vsf_hw_clk_t VSF_HW_CLK_SYSBUS = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x10, 8, 4), // RCC_SYSBUSDIV1.BUSDIV

    .clksrc                     = &VSF_HW_CLK_SYS,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_SYSBUS_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = dimof(__VSF_HW_CLK_SYSBUS_PRESCALER) - 1,
};

const vsf_hw_clk_t VSF_HW_CLK_AXISYS = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x10, 12, 4),// RCC_SYSBUSDIV1.AXIDIV

    .clksrc                     = &VSF_HW_CLK_SYS,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_SYSBUS_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = dimof(__VSF_HW_CLK_SYSBUS_PRESCALER) - 1,
};

const vsf_hw_clk_t VSF_HW_CLK_M7HYP = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x10, 16, 4),// RCC_SYSBUSDIV1.M7HYPDIV

    .clksrc                     = &VSF_HW_CLK_PLL2A,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_SYSBUS_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = dimof(__VSF_HW_CLK_SYSBUS_PRESCALER) - 1,
};

#if     defined(CORE_CM7)
static const vsf_hw_clk_t * const __VSF_HW_CLK_CPU_CLKSEL_MAPPER[2] = {
    &VSF_HW_CLK_SYS, &VSF_HW_CLK_M7HYP
};
const vsf_hw_clk_t VSF_HW_CLK_CPU = {
    .clksel_region              = VSF_HW_CLKRST_REGION(0x5B, 17, 1),// RCC_SRCCTRL2.M7HYPSEL

    .clksel_mapper              = __VSF_HW_CLK_CPU_CLKSEL_MAPPER,
    .clksrc_type                = VSF_HW_CLK_TYPE_SEL,
};
const vsf_hw_clk_t VSF_HW_CLK_SYSTICK = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x59, 4, 4), // RCC_CFG3.M7STCLKDIV

    .clksrc                     = &VSF_HW_CLK_CPU,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_SYSBUS_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = dimof(__VSF_HW_CLK_SYSBUS_PRESCALER) - 1,
};
#elif   defined(CORE_CM4)
const vsf_hw_clk_t VSF_HW_CLK_CPU = {
    .clksrc                     = &VSF_HW_CLK_SYSBUS,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,
};
const vsf_hw_clk_t VSF_HW_CLK_SYSTICK = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x59, 0, 4), // RCC_CFG3.M4STCLKDIV

    .clksrc                     = &VSF_HW_CLK_CPU,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_SYSBUS_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = dimof(__VSF_HW_CLK_SYSBUS_PRESCALER) - 1,
};
#endif

const vsf_hw_clk_t VSF_HW_CLK_AXIHYP = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x10, 24, 4),// RCC_SYSBUSDIV1.AXIHYPDIV

    .clksrc                     = &VSF_HW_CLK_M7HYP,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_SYSBUS_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = dimof(__VSF_HW_CLK_SYSBUS_PRESCALER) - 1,
};

static const vsf_hw_clk_t * const __VSF_HW_CLK_AXI_CLKSEL_MAPPER[4] = {
    &VSF_HW_CLK_AXISYS, &VSF_HW_CLK_AXIHYP,
};
const vsf_hw_clk_t VSF_HW_CLK_AXI = {
    .clksel_region              = VSF_HW_CLKRST_REGION(0x5B, 16, 1),// RCC_SRCCTRL2.AXIHYPSEL

    .clksel_mapper              = __VSF_HW_CLK_AXI_CLKSEL_MAPPER,
    .clksrc_type                = VSF_HW_CLK_TYPE_SEL,
};

static const uint16_t __VSF_HW_CLK_APB_PRESCALER[8] = {
    [0]                         = 1,
    [4]                         = 2,
    [5]                         = 4,
    [6]                         = 8,
    [7]                         = 16,
};
const vsf_hw_clk_t VSF_HW_CLK_APB1 = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x11, 0, 3), // RCC_SYSBUSDIV2.APB1DIV

    .clksrc                     = &VSF_HW_CLK_SYSBUS,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_APB_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = dimof(__VSF_HW_CLK_APB_PRESCALER) - 1,
};
const vsf_hw_clk_t VSF_HW_CLK_APB2 = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x11, 8, 3), // RCC_SYSBUSDIV2.APB2DIV

    .clksrc                     = &VSF_HW_CLK_SYSBUS,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_APB_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = dimof(__VSF_HW_CLK_APB_PRESCALER) - 1,
};
const vsf_hw_clk_t VSF_HW_CLK_APB5 = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x11, 16, 3),// RCC_SYSBUSDIV2.APB5DIV

    .clksrc                     = &VSF_HW_CLK_SYSBUS,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_APB_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = dimof(__VSF_HW_CLK_APB_PRESCALER) - 1,
};
const vsf_hw_clk_t VSF_HW_CLK_APB6 = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x11, 24, 3),// RCC_SYSBUSDIV2.APB6DIV

    .clksrc                     = &VSF_HW_CLK_AXI,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_APB_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = dimof(__VSF_HW_CLK_APB_PRESCALER) - 1,
};

static const vsf_hw_clk_t * const __VSF_HW_CLK_PERI_CLKSEL_MAPPER[4] = {
    &VSF_HW_CLK_HSI_KER_CG, NULL, &VSF_HW_CLK_MSI_KER_CG, &VSF_HW_CLK_HSE_KER_CG,
};
const vsf_hw_clk_t VSF_HW_CLK_PERI = {
    .clksel_region              = VSF_HW_CLKRST_REGION(0x59, 12, 2),// RCC_CFG3.PERSW

    .clksel_mapper              = __VSF_HW_CLK_PERI_CLKSEL_MAPPER,
    .clksrc_type                = VSF_HW_CLK_TYPE_SEL,
};

const vsf_hw_clk_t VSF_HW_CLK_SDRAM_AXI = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x5E, 8, 4), // RCC_AXIDIV2.SDRAMMEMDIV

    .clksrc                     = &VSF_HW_CLK_AXI,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_SYSBUS_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = dimof(__VSF_HW_CLK_SYSBUS_PRESCALER) - 1,
    
};
static const vsf_hw_clk_t * const __VSF_HW_CLK_SDRAM_CLKSEL_MAPPER[8] = {
    &VSF_HW_CLK_SDRAM_AXI, &VSF_HW_CLK_PERI, &VSF_HW_CLK_PLL2A, &VSF_HW_CLK_PLL3A, &VSF_HW_CLK_PLL1B,
};
const vsf_hw_clk_t VSF_HW_CLK_SDRAM = {
    .clksel_region              = VSF_HW_CLKRST_REGION(0x5F, 8, 3), // RCC_AXISEL2.SDRAMMEMSEL

    .clksel_mapper              = __VSF_HW_CLK_SDRAM_CLKSEL_MAPPER,
    .clksrc_type                = VSF_HW_CLK_TYPE_SEL,
};

#if VSF_HAL_USE_SDIO == ENABLED
const vsf_hw_clk_t VSF_HW_CLK_SDMMC1_AXI = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x4E, 20, 4),// RCC_AXIDIV1.ADMMC1AXIDIV

    .clksrc                     = &VSF_HW_CLK_AXI,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_SYSBUS_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = dimof(__VSF_HW_CLK_SYSBUS_PRESCALER) - 1,
    
};
static const vsf_hw_clk_t * const __VSF_HW_CLK_SDMMC1_CLKSEL_MAPPER[8] = {
    &VSF_HW_CLK_SDMMC1_AXI, &VSF_HW_CLK_PERI, &VSF_HW_CLK_PLL2A, &VSF_HW_CLK_PLL3A, &VSF_HW_CLK_PLL1B,
};
const vsf_hw_clk_t VSF_HW_CLK_SDMMC1 = {
    .clksel_region              = VSF_HW_CLKRST_REGION(0x4F, 20, 3),// RCC_AXISEL2.SDMMC1KERSEL

    .clksel_mapper              = __VSF_HW_CLK_SDMMC1_CLKSEL_MAPPER,
    .clksrc_type                = VSF_HW_CLK_TYPE_SEL,
};

// todo:
static const vsf_hw_clk_t * const __VSF_HW_CLK_SDMMC2_CLKSEL_MAPPER[8] = {
    /*&VSF_HW_CLK_SDMMC2_AXI*/NULL, &VSF_HW_CLK_PERI, &VSF_HW_CLK_PLL2A, &VSF_HW_CLK_PLL3A, &VSF_HW_CLK_PLL1B,
};
const vsf_hw_clk_t VSF_HW_CLK_SDMMC2 = {
    .clksel_region              = VSF_HW_CLKRST_REGION(0x14, 12, 3),// RCC_AHB1SEL1.SDMMC2KERSEL

    .clksel_mapper              = __VSF_HW_CLK_SDMMC2_CLKSEL_MAPPER,
    .clksrc_type                = VSF_HW_CLK_TYPE_SEL,
};
#endif

#if VSF_HAL_USE_I2C == ENABLED

#endif

#if VSF_HAL_USE_USART == ENABLED
const vsf_hw_clk_t VSF_HW_CLK_USART1_2 = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x1D, 28, 3),// RCC_APB1DIV1.APB1USARTDIV

    .clksrc                     = &VSF_HW_CLK_APB1,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_APB_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = dimof(__VSF_HW_CLK_APB_PRESCALER) - 1,
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
        &(((uint32_t *)RCC)[reg_word_offset]),
        (_ & ~mask) | (value << bit_offset)
    );
}

uint_fast8_t vsf_hw_clkrst_region_get(uint32_t region)
{
    uint8_t bit_length = (region >> 8) & 0x07, bit_offset = (region >> 0) & 0x1F;
    uint8_t reg_word_offset = region >> 16;

    return (((uint32_t *)RCC)[reg_word_offset] >> bit_offset) & ((1 << bit_length) - 1);
}

void vsf_hw_clkrst_region_set_bit(uint32_t region)
{
    VSF_HAL_ASSERT(1 == ((region >> 8) & 0x07));
    uint8_t bit_offset = (region >> 0) & 0x1F;
    uint8_t reg_word_offset = region >> 16;

    vsf_atom_or(32,
        &(((uint32_t *)RCC)[reg_word_offset]),
        (1 << bit_offset)
    );
}

void vsf_hw_clkrst_region_clear_bit(uint32_t region)
{
    VSF_HAL_ASSERT(1 == ((region >> 8) & 0x07));
    uint8_t bit_offset = (region >> 0) & 0x1F;
    uint8_t reg_word_offset = region >> 16;

    vsf_atom_and(32,
        &(((uint32_t *)RCC)[reg_word_offset]),
        ~(1 << bit_offset)
    );
}

uint_fast8_t vsf_hw_clkrst_region_get_bit(uint32_t region)
{
    VSF_HAL_ASSERT(1 == ((region >> 8) & 0x07));
    uint8_t bit_offset = (region >> 0) & 0x1F;
    uint8_t reg_word_offset = region >> 16;

    return (((uint32_t *)RCC)[reg_word_offset] >> bit_offset) & 1;
}



const vsf_hw_clk_t * vsf_hw_clk_get_src(const vsf_hw_clk_t *clk)
{
    VSF_HAL_ASSERT(clk != NULL);

    switch (clk->clksrc_type) {
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
    switch (clk->clksrc_type) {
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

bool vsf_hw_clk_is_enabled(const vsf_hw_clk_t *clk)
{
    if (clk->clken_region != 0) {
        return !!vsf_hw_clkrst_region_get_bit(clk->clken_region);
    }
    return true;
}

bool vsf_hw_clk_is_ready(const vsf_hw_clk_t *clk)
{
    if (clk->clkrdy_region != 0) {
        return !!vsf_hw_clkrst_region_get_bit(clk->clkrdy_region);
    }
    return true;
}

vsf_err_t vsf_hw_clk_config(const vsf_hw_clk_t *clk, const vsf_hw_clk_t *clksrc, uint16_t prescaler, uint32_t freq_hz)
{
    VSF_HAL_ASSERT(clk != NULL);
    if (clksrc != NULL) {
        VSF_HAL_ASSERT(clk->clksel_region != 0);
        if (clk->clksrc_type != VSF_HW_CLK_TYPE_SEL) {
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



static uint32_t __vsf_hw_pll_getclk(const vsf_hw_clk_t *clk, uint32_t clksrc_freq_hz, uint32_t prescaler)
{
    uint32_t *ctrl = &(((uint32_t *)RCC)[clk->clken_region >> 16]);
    uint32_t clkr = ctrl[1] >> 26;
    uint32_t clkf = ctrl[1] & ((1 << 26) - 1);
    return (uint32_t)((((uint64_t)clksrc_freq_hz * clkf) >> 14) / (clkr + 1));
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

#if VSF_USE_TRACE == ENABLED && VSF_ARCH_CFG_CALLSTACK_TRACE == ENABLED
    vsf_trace_dump_stack();
#endif
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
VSF_CAL_WEAK(vsf_hw_mpu_add_basic_resgions)
void vsf_hw_mpu_add_basic_resgions(void)
{
    vsf_arch_mpu_disable();

    // background, 4G from 0x00000000
    vsf_arch_mpu_add_region(0x00000000, 0,
                            VSF_ARCH_MPU_NON_SHARABLE       |
                            VSF_ARCH_MPU_NON_EXECUTABLE     |
                            VSF_ARCH_MPU_ACCESS_NO          |
                            VSF_ARCH_MPU_NON_CACHABLE);

    // AHBs & APBs, 512M from 0x40000000
    vsf_arch_mpu_add_region(0x40000000, 512 * 1024 * 1024,
                            VSF_ARCH_MPU_SHARABLE           |
                            VSF_ARCH_MPU_NON_EXECUTABLE     |
                            VSF_ARCH_MPU_ACCESS_FULL        |
                            VSF_ARCH_MPU_NON_CACHABLE);

    // AXI SRAM, 1M from 0x24000000
    vsf_arch_mpu_add_region(0x24000000, 1 * 1024 * 1024,
                            VSF_ARCH_MPU_NON_SHARABLE       |
                            VSF_ARCH_MPU_EXECUTABLE         |
                            VSF_ARCH_MPU_ACCESS_FULL        |
                            VSF_ARCH_MPU_CACHABLE_WRITE_THROUGH_NOALLOC);

    // AHB SRAM, 352K from 0x30000000
    vsf_arch_mpu_add_region(0x30000000, 512 * 1024,
                            VSF_ARCH_MPU_NON_SHARABLE       |
                            VSF_ARCH_MPU_EXECUTABLE         |
                            VSF_ARCH_MPU_ACCESS_FULL        |
                            VSF_ARCH_MPU_CACHABLE_WRITE_BACK_NOALLOC);

    // ROM, 32K from 0x1FFFF0000
    vsf_arch_mpu_add_region(0x1FFF0000, 32 * 1024,
                            VSF_ARCH_MPU_NON_SHARABLE       |
                            VSF_ARCH_MPU_EXECUTABLE         |
                            VSF_ARCH_MPU_ACCESS_READONLY    |
                            VSF_ARCH_MPU_CACHABLE_WRITE_THROUGH_NOALLOC);

    // User FLASH, 32M from 0x08000000 and 0x15000000
    vsf_arch_mpu_add_region(0x08000000, 32 * 1024 * 1024,
                            VSF_ARCH_MPU_NON_SHARABLE       |
                            VSF_ARCH_MPU_EXECUTABLE         |
                            VSF_ARCH_MPU_ACCESS_FULL        |
                            VSF_ARCH_MPU_CACHABLE_WRITE_THROUGH_NOALLOC);
    vsf_arch_mpu_add_region(0x15000000, 32 * 1024 * 1024,
                            VSF_ARCH_MPU_NON_SHARABLE       |
                            VSF_ARCH_MPU_EXECUTABLE         |
                            VSF_ARCH_MPU_ACCESS_FULL        |
                            VSF_ARCH_MPU_CACHABLE_WRITE_THROUGH_NOALLOC);

    vsf_arch_mpu_enable();
}





/*! \note initialize device driver
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
VSF_CAL_WEAK(vsf_driver_init)
bool vsf_driver_init(void)
{
    // refer to vsf_hal_pre_startup_init implementation in startup file
//    SCB_EnableICache();
//    SCB_EnableDCache();

    vsf_hw_mpu_add_basic_resgions();
    return true;
}





// workarounds

// no idea why after __WFE is called, SWD will fail
void vsf_arch_sleep(uint_fast32_t mode)
{
    switch (mode) {
    case 0:     SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;  break;
    case 1:     SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;  break;
    default:
        // arm core supports normalsleep(0) and deepsleep(1)
        // for other vendor specified sleep modes,
        //  chip vendor should rewrite vsf_arch_sleep in chip driver
        VSF_ARCH_ASSERT(false);
    }
    __DSB();
//    __WFE();
}

/* EOF */
