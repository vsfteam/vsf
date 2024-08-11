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

/*============================ MACROS ========================================*/

#ifndef HSE_VALUE
#   define HSE_VALUE            ((uint32_t)25000000)
#endif

#ifndef LSE_VALUE
#   define LSE_VALUE            ((uint32_t)32000)
#endif

#ifndef CSI_VALUE
#   define CSI_VALUE            ((uint32_t)4000000)
#endif

#ifndef HSI_VALUE
#   define HSI_VALUE            ((uint32_t)64000000)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static uint32_t __vsf_hw_pll_get_vco(const vsf_hw_clk_t *clk, uint32_t clksrc_freq_hz, uint32_t prescaler);

/*============================ GLOBAL VARIABLES ==============================*/

const vsf_hw_clk_t VSF_HW_CLK_HSI = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x00, 3, 2), // HSIDIV IN RCC.CR
    .clken_region               = VSF_HW_CLKRST_REGION(0x00, 0, 1), // HSION IN RCC.CR
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x00, 2, 1), // HSIRDY IN RCC.CR

    .clk_freq_hz                = HSI_VALUE,

    .clktype                    = VSF_HW_CLK_TYPE_CONST,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_SFT,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 3,
};
const vsf_hw_clk_t VSF_HW_CLK_HSI_KER = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x00, 3, 2), // HSIDIV IN RCC.CR
    .clken_region               = VSF_HW_CLKRST_REGION(0x00, 1, 1), // HSIKERON IN RCC.CR
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x00, 2, 1), // HSIRDY IN RCC.CR

    .clk_freq_hz                = HSI_VALUE,
    .clktype                    = VSF_HW_CLK_TYPE_CONST,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_SFT,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 3,
};

const vsf_hw_clk_t VSF_HW_CLK_HSI48 = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x00, 12, 1),// HSI48ON IN RCC.CR
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x00, 13, 1),// HSI48RDY IN RCC.CR

    .clk_freq_hz                = 48000000,
    .clktype                    = VSF_HW_CLK_TYPE_CONST,
};

const vsf_hw_clk_t VSF_HW_CLK_LSI = {
    .clk_freq_hz                = 32000,
    .clktype                    = VSF_HW_CLK_TYPE_CONST,
};

const vsf_hw_clk_t VSF_HW_CLK_CSI = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x00, 7, 1), // CSION IN RCC.CR
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x00, 8, 1), // CSIRDY IN RCC.CR

    .clk_freq_hz                = CSI_VALUE,
    .clktype                    = VSF_HW_CLK_TYPE_CONST,
};
const vsf_hw_clk_t VSF_HW_CLK_CSI_KER = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x00, 9, 1), // CSIKERON IN RCC.CR
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x00, 8, 1), // CSIRDY IN RCC.CR

    .clk_freq_hz                = CSI_VALUE,
    .clktype                    = VSF_HW_CLK_TYPE_CONST,
};

const vsf_hw_clk_t VSF_HW_CLK_HSE = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x00, 16, 1),// HSEON IN RCC.CR
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x00, 17, 1),// HSERDY IN RCC.CR

    .clk_freq_hz                = HSE_VALUE,
    .clktype                    = VSF_HW_CLK_TYPE_CONST,
};

const vsf_hw_clk_t VSF_HW_CLK_LSE = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x1C, 0, 1), // LSEON IN RCC.BDCR
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x1c, 1, 1), // LSERDY IN RCC.BDCR

    .clk_freq_hz                = LSE_VALUE,
    .clktype                    = VSF_HW_CLK_TYPE_CONST,
};

static const vsf_hw_clk_t * __VSF_HW_CLK_PLL_CLKSEL_MAPPER[4] = {
    &VSF_HW_CLK_HSI, &VSF_HW_CLK_CSI, &VSF_HW_CLK_HSE, NULL,
};
static const vsf_hw_clk_t __VSF_HW_CLK_PLL_SRC = {
    .clksel_region              = VSF_HW_CLKRST_REGION(0x0A, 0, 3), // PLLSRC IN RCC.PLLCKSELR

    .clksel_mapper              = __VSF_HW_CLK_PLL_CLKSEL_MAPPER,
    .clktype                    = VSF_HW_CLK_TYPE_SEL,
};
static const vsf_hw_clk_t __VSF_HW_CLK_PLL1_VCO = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x0A, 4, 6), // DIVM1 IN RCC.PLLCKSELR
    .clksel_region              = VSF_HW_CLKRST_REGION(0x0B, 0, 4), // PLLxFRACEN/PLLxVCOSEL/PLLxRGE IN RCC.PLLCFGR

    .clksrc                     = &__VSF_HW_CLK_PLL_SRC,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .getclk                     = __vsf_hw_pll_get_vco,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_FUNC,

    .clkprescaler_min           = 0,
    .clkprescaler_max           = 63,
};
static const vsf_hw_clk_t __VSF_HW_CLK_PLL2_VCO = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x0A, 12, 6),// DIVM2 IN RCC.PLLCKSELR
    .clksel_region              = VSF_HW_CLKRST_REGION(0x0B, 4, 4), // PLLxFRACEN/PLLxVCOSEL/PLLxRGE IN RCC.PLLCFGR

    .clksrc                     = &__VSF_HW_CLK_PLL_SRC,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .getclk                     = __vsf_hw_pll_get_vco,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_FUNC,

    .clkprescaler_min           = 0,
    .clkprescaler_max           = 63,
};
static const vsf_hw_clk_t __VSF_HW_CLK_PLL3_VCO = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x0A, 20, 6),// DIVM3 IN RCC.PLLCKSELR
    .clksel_region              = VSF_HW_CLKRST_REGION(0x0B, 8, 4), // PLLxFRACEN/PLLxVCOSEL/PLLxRGE IN RCC.PLLCFGR

    .clksrc                     = &__VSF_HW_CLK_PLL_SRC,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .getclk                     = __vsf_hw_pll_get_vco,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_FUNC,

    .clkprescaler_min           = 0,
    .clkprescaler_max           = 63,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL1_P = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x0C, 9, 7), // DIVP1 IN RCC.PLL1DIVR
    .clken_region               = VSF_HW_CLKRST_REGION(0x0B, 16, 1),// DIVP1EN IN RCC.PLLCFGR

    .clksrc                     = &__VSF_HW_CLK_PLL1_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL1_Q = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x0C, 16, 7),// DIVQ1 IN RCC.PLL1DIVR
    .clken_region               = VSF_HW_CLKRST_REGION(0x0B, 17, 1),// DIVQ1EN IN RCC.PLLCFGR

    .clksrc                     = &__VSF_HW_CLK_PLL1_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL1_R = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x0C, 24, 7),// DIVR1 IN RCC.PLL1DIVR
    .clken_region               = VSF_HW_CLKRST_REGION(0x0B, 18, 1),// DIVR1EN IN RCC.PLLCFGR

    .clksrc                     = &__VSF_HW_CLK_PLL1_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL2_P = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x0E, 9, 7), // DIVP2 IN RCC.PLL2DIVR
    .clken_region               = VSF_HW_CLKRST_REGION(0x0B, 19, 1),// DIVP2EN IN RCC.PLLCFGR

    .clksrc                     = &__VSF_HW_CLK_PLL2_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL2_Q = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x0E, 16, 7),// DIVQ2 IN RCC.PLL2DIVR
    .clken_region               = VSF_HW_CLKRST_REGION(0x0B, 20, 1),// DIVQ2EN IN RCC.PLLCFGR

    .clksrc                     = &__VSF_HW_CLK_PLL2_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL2_R = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x0E, 24, 7),// DIVR2 IN RCC.PLL2DIVR
    .clken_region               = VSF_HW_CLKRST_REGION(0x0B, 21, 1),// DIVR2EN IN RCC.PLLCFGR

    .clksrc                     = &__VSF_HW_CLK_PLL2_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL3_P = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x10, 9, 7), // DIVP3 IN RCC.PLL3DIVR
    .clken_region               = VSF_HW_CLKRST_REGION(0x0B, 22, 1),// DIVP3EN IN RCC.PLLCFGR

    .clksrc                     = &__VSF_HW_CLK_PLL3_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL3_Q = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x10, 16, 7),// DIVQ3 IN RCC.PLL3DIVR
    .clken_region               = VSF_HW_CLKRST_REGION(0x0B, 23, 1),// DIVQ3EN IN RCC.PLLCFGR

    .clksrc                     = &__VSF_HW_CLK_PLL3_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL3_R = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x10, 24, 7),// DIVR3 IN RCC.PLL3DIVR
    .clken_region               = VSF_HW_CLKRST_REGION(0x0B, 24, 1),// DIVR3EN IN RCC.PLLCFGR

    .clksrc                     = &__VSF_HW_CLK_PLL3_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};

static const vsf_hw_clk_t * __VSF_HW_CLK_SYS_CLKSEL_MAPPER[8] = {
    &VSF_HW_CLK_HSI, &VSF_HW_CLK_CSI, &VSF_HW_CLK_HSE, &VSF_HW_CLK_PLL1_P,
    NULL, NULL, NULL, NULL,
};
const vsf_hw_clk_t VSF_HW_CLK_SYS = {
    .clksel_region              = VSF_HW_CLKRST_REGION(0x04, 0, 3), // SW IN RCC.CFGR

    .clksel_mapper              = __VSF_HW_CLK_SYS_CLKSEL_MAPPER,
    .clktype                    = VSF_HW_CLK_TYPE_SEL,
};
static const uint8_t __VSF_HW_CLK_SYSD1_PRESCALER[16] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9,
};
const vsf_hw_clk_t VSF_HW_CLK_SYSD1 = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x06, 8, 4), // D1CPRE IN RCC.D1CFGR

    .clksrc                     = &VSF_HW_CLK_SYS,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_SYSD1_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_SFT,

    .clkprescaler_min           = 0,
    .clkprescaler_max           = 15,
};
const vsf_hw_clk_t VSF_HW_CLK_HCLK = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x06, 0, 4), // HPRE IN RCC.D1CFGR

    .clksrc                     = &VSF_HW_CLK_SYSD1,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_SYSD1_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_SFT,

    .clkprescaler_min           = 0,
    .clkprescaler_max           = 15,
};

static const vsf_hw_clk_t * __VSF_HW_CLK_PER_CLKSEL_MAPPER[4] = {
    &VSF_HW_CLK_HSI_KER, &VSF_HW_CLK_CSI_KER, &VSF_HW_CLK_HSE, NULL,
};
const vsf_hw_clk_t VSF_HW_CLK_PER = {
    .clksel_region              = VSF_HW_CLKRST_REGION(0x13, 28, 2),// CKPERSEL IN RCC.D1CCIPR

    .clksel_mapper              = __VSF_HW_CLK_PER_CLKSEL_MAPPER,
    .clktype                    = VSF_HW_CLK_TYPE_SEL,
};

static const uint8_t __VSF_HW_CLK_PCLK_PRESCALER[8] = {
    0, 0, 0, 0, 1, 2, 3, 4,
};
const vsf_hw_clk_t VSF_HW_CLK_PCLK1 = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x07, 4, 3), // D2PPRE1 IN RCC.D2CFGR

    .clksrc                     = &VSF_HW_CLK_HCLK,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_PCLK_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_SFT,

    .clkprescaler_min           = 0,
    .clkprescaler_max           = 7,
};
const vsf_hw_clk_t VSF_HW_CLK_PCLK2 = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x07, 8, 3), // D2PPRE2 IN RCC.D2CFGR

    .clksrc                     = &VSF_HW_CLK_HCLK,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_PCLK_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_SFT,

    .clkprescaler_min           = 0,
    .clkprescaler_max           = 7,
};
const vsf_hw_clk_t VSF_HW_CLK_PCLK3 = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x06, 4, 3), // D1PPRE IN RCC.D1CFGR

    .clksrc                     = &VSF_HW_CLK_HCLK,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_PCLK_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_SFT,

    .clkprescaler_min           = 0,
    .clkprescaler_max           = 7,
};
const vsf_hw_clk_t VSF_HW_CLK_PCLK4 = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x08, 4, 3), // D3PPRE IN RCC.D3CFGR

    .clksrc                     = &VSF_HW_CLK_HCLK,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_PCLK_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_SFT,

    .clkprescaler_min           = 0,
    .clkprescaler_max           = 7,
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

static uint32_t __vsf_hw_pll_get_vco(const vsf_hw_clk_t *clk, uint32_t clksrc_freq_hz, uint32_t prescaler)
{
    uint32_t *pllcfgr = &(((uint32_t *)RCC)[clk->clksel_region >> 16]);
    uint8_t pllidx = (clk->clksel_region & 0x1F) >> 2;
    VSF_HAL_ASSERT(pllidx < 3);

    uint32_t divn = pllcfgr[1 + (pllidx << 1)] & 0x1FF;
    uint32_t fracen = pllcfgr[0] & (1 << (pllidx << 2));
    uint32_t frac = ((pllcfgr[2 + (pllidx << 1)] >> 3) & 0x1FFF) * fracen;

    VSF_HAL_ASSERT(prescaler < 64);
    if (!prescaler) { prescaler = 1; }
    return (uint32_t)(((float)clksrc_freq_hz / prescaler) * ((float)divn + ((float)frac / 0x2000) + 1));
}

uint32_t vsf_hw_clk_get(const vsf_hw_clk_t *clk)
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
        clk_freq_hz = vsf_hw_clk_get(clk->clksrc);
        break;
    case VSF_HW_CLK_TYPE_SEL: {
            const vsf_hw_clk_t *clksrc = clk->clksel_mapper[vsf_hw_clkrst_region_get(clk->clksel_region)];
            if (NULL == clksrc) {
                return 0;
            } else {
                clk_freq_hz = vsf_hw_clk_get(clksrc);
            }
        }
        break;
    default:
        VSF_HAL_ASSERT(false);
        return 0;
    }

    if (clk->clkprescaler_region != 0) {
        uint32_t prescaler =  vsf_hw_clkrst_region_get(clk->clkprescaler_region);
        if (    (clk->clkprescaler_mapper != NULL)
            &&  (clk->clkprescaler_type != VSF_HW_CLK_PRESCALER_FUNC)) {
            prescaler = clk->clkprescaler_mapper[prescaler];
        }
        switch (clk->clkprescaler_type) {
        case VSF_HW_CLK_PRESCALER_DIV:
            if (prescaler != 0) {
                clk_freq_hz /= prescaler;
            } else {
                clk_freq_hz = 0;
            }
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
    return clk_freq_hz;
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
