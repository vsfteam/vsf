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
#   define HSE_VALUE            ((uint32_t)24000000)
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

typedef enum vsf_hw_peripheral_clksel_t {
    VSF_HW_CLKSEL_USART1                = VSF_HW_CLKRST_REGION(0x15, 0,  3),// USART1SEL IN RCC.CCIPR3
    VSF_HW_CLKSEL_USART234578           = VSF_HW_CLKRST_REGION(0x14, 0,  3),// UART234578SEL IN RCC.CIPR2
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

const vsf_hw_clk_t VSF_HW_CLK_HSI = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x00, 3,  2),// HSIDIV IN RCC.CR
    .clken_region               = VSF_HW_CLKRST_REGION(0x00, 0,  1),// HSION IN RCC.CR
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x00, 2,  1),// HSIRDY IN RCC.CR

    .clk_freq_hz                = HSI_VALUE,

    .clktype                    = VSF_HW_CLK_TYPE_CONST,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_SFT,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 3,
};
const vsf_hw_clk_t VSF_HW_CLK_HSI_KER = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x00, 3,  2),// HSIDIV IN RCC.CR
    .clken_region               = VSF_HW_CLKRST_REGION(0x00, 1,  1),// HSIKERON IN RCC.CR

    .clk_freq_hz                = HSI_VALUE,
    .clktype                    = VSF_HW_CLK_TYPE_CONST,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_SFT,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 3,
};
const vsf_hw_clk_t VSF_HW_CLK_HSI_CAL = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x00, 1,  1),// HSIKERON IN RCC.CR
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x00, 2,  1),// HSIRDY IN RCC.CR

    .clk_freq_hz                = HSI_VALUE,
    .clktype                    = VSF_HW_CLK_TYPE_CONST,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_CONST,
    .prescaler                  = 1024,
};
const vsf_hw_clk_t VSF_HW_CLK_UCPD_KER = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x00, 1,  1),// HSIKERON IN RCC.CR
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x00, 2,  1),// HSIRDY IN RCC.CR

    .clk_freq_hz                = HSI_VALUE,
    .clktype                    = VSF_HW_CLK_TYPE_CONST,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_CONST,
    .prescaler                  = 4,
};

const vsf_hw_clk_t VSF_HW_CLK_HSI48_KER = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x00, 12,  1),// HSI48ON IN RCC.CR
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x00, 13,  1),// HSI48RDY IN RCC.CR

    .clk_freq_hz                = 48000000,
    .clktype                    = VSF_HW_CLK_TYPE_CONST,
};

const vsf_hw_clk_t VSF_HW_CLK_LSI = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x1D, 0,  1),// LSION IN RCC.CSR
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x1D, 1,  1),// LSIRDY IN RCC.CSR

    .clk_freq_hz                = 32000,
    .clktype                    = VSF_HW_CLK_TYPE_CONST,
};

const vsf_hw_clk_t VSF_HW_CLK_CSI = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x00, 7,  1),// CSION IN RCC.CR
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x00, 8,  1),// CSIRDY IN RCC.CR

    .clk_freq_hz                = CSI_VALUE,
    .clktype                    = VSF_HW_CLK_TYPE_CONST,
};
const vsf_hw_clk_t VSF_HW_CLK_CSI_KER = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x00, 9,  1),// CSIKERON IN RCC.CR

    .clk_freq_hz                = CSI_VALUE,
    .clktype                    = VSF_HW_CLK_TYPE_CONST,
};
const vsf_hw_clk_t VSF_HW_CLK_CSI_CAL = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x00, 9,  1),// CSIKERON IN RCC.CR

    .clk_freq_hz                = CSI_VALUE,
    .clktype                    = VSF_HW_CLK_TYPE_CONST,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_CONST,
    .prescaler                  = 128,
};

const vsf_hw_clk_t VSF_HW_CLK_HSE = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x00, 16, 1),// HSEON IN RCC.CR
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x00, 17, 1),// HSERDY IN RCC.CR

    .clk_freq_hz                = HSE_VALUE,
    .clktype                    = VSF_HW_CLK_TYPE_CONST,
};

const vsf_hw_clk_t VSF_HW_CLK_LSE = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x1C, 0,  1),// LSEON IN RCC.BDCR
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x1C, 1,  1),// LSERDY IN RCC.BDCR

    .clk_freq_hz                = LSE_VALUE,
    .clktype                    = VSF_HW_CLK_TYPE_CONST,
};

static const vsf_hw_clk_t * __VSF_HW_CLK_PLL_CLKSEL_MAPPER[4] = {
    &VSF_HW_CLK_HSI, &VSF_HW_CLK_CSI, &VSF_HW_CLK_HSE, NULL,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL_SRC = {
    .clksel_region              = VSF_HW_CLKRST_REGION(0x0A, 0,  2),// PLLSRC IN RCC.PLLCKSELR

    .clksel_mapper              = __VSF_HW_CLK_PLL_CLKSEL_MAPPER,
    .clktype                    = VSF_HW_CLK_TYPE_SEL,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL1_VCO = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x0A, 4,  6),// DIVM1 IN RCC.PLLCKSELR
    .clksel_region              = VSF_HW_CLKRST_REGION(0x0B, 0,  5),// PLLxFRACEN/PLLxVCOSEL/PLLxRGE IN RCC.PLLCFGR

    .clksrc                     = &VSF_HW_CLK_PLL_SRC,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .getclk                     = __vsf_hw_pll_get_vco,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_FUNC,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 63,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL2_VCO = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x0A, 12, 6),// DIVM2 IN RCC.PLLCKSELR
    .clksel_region              = VSF_HW_CLKRST_REGION(0x0B, 11, 5),// PLLxFRACEN/PLLxVCOSEL/PLLxRGE IN RCC.PLLCFGR

    .clksrc                     = &VSF_HW_CLK_PLL_SRC,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .getclk                     = __vsf_hw_pll_get_vco,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_FUNC,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 63,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL3_VCO = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x0A, 20, 6),// DIVM3 IN RCC.PLLCKSELR
    .clksel_region              = VSF_HW_CLKRST_REGION(0x0B, 22, 5),// PLLxFRACEN/PLLxVCOSEL/PLLxRGE IN RCC.PLLCFGR

    .clksrc                     = &VSF_HW_CLK_PLL_SRC,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .getclk                     = __vsf_hw_pll_get_vco,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_FUNC,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 63,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL1_P = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x0C, 9,  7),// DIVP IN RCC.PLL1DIVR1
    .clken_region               = VSF_HW_CLKRST_REGION(0x0B, 5,  1),// PLL1PEN IN RCC.PLLCFGR

    .clksrc                     = &VSF_HW_CLK_PLL1_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL1_Q = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x0C, 16, 7),// DIVQ IN RCC.PLL1DIVR1
    .clken_region               = VSF_HW_CLKRST_REGION(0x0B, 6,  1),// PLL1QEN IN RCC.PLLCFGR

    .clksrc                     = &VSF_HW_CLK_PLL1_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL1_S = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x30, 0,  3),// DIVS IN RCC.PLL1DIVR2
    .clken_region               = VSF_HW_CLKRST_REGION(0x0B, 8,  1),// PLL1SEN IN RCC.PLLCFGR

    .clksrc                     = &VSF_HW_CLK_PLL1_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 7,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL2_P = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x0E, 9,  7),// DIVP IN RCC.PLL2DIVR1
    .clken_region               = VSF_HW_CLKRST_REGION(0x0B, 16, 1),// PLL2PEN IN RCC.PLLCFGR

    .clksrc                     = &VSF_HW_CLK_PLL2_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL2_Q = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x0E, 16, 7),// DIVQ IN RCC.PLL2DIVR1
    .clken_region               = VSF_HW_CLKRST_REGION(0x0B, 17, 1),// PLL2QEN IN RCC.PLLCFGR

    .clksrc                     = &VSF_HW_CLK_PLL2_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL2_R = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x0E, 24, 7),// DIVR IN RCC.PLL2DIVR1
    .clken_region               = VSF_HW_CLKRST_REGION(0x0B, 18, 1),// PLL2REN IN RCC.PLLCFGR

    .clksrc                     = &VSF_HW_CLK_PLL2_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL2_S = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x31, 0,  3),// DIVS IN RCC.PLL2DIVR
    .clken_region               = VSF_HW_CLKRST_REGION(0x0B, 19, 1),// PLL2SEN IN RCC.PLLCFGR

    .clksrc                     = &VSF_HW_CLK_PLL2_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 7,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL2_T = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x31, 8,  3),// DIVT IN RCC.PLL2DIVR
    .clken_region               = VSF_HW_CLKRST_REGION(0x0B, 20, 1),// PLL2TEN IN RCC.PLLCFGR

    .clksrc                     = &VSF_HW_CLK_PLL2_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 7,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL3_P = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x10, 9, 7), // DIVP IN RCC.PLL3DIVR1
    .clken_region               = VSF_HW_CLKRST_REGION(0x0B, 27, 1),// PLL3PEN IN RCC.PLLCFGR

    .clksrc                     = &VSF_HW_CLK_PLL3_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL3_Q = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x10, 16, 7),// DIVQ IN RCC.PLL3DIVR1
    .clken_region               = VSF_HW_CLKRST_REGION(0x0B, 28, 1),// PLL3QEN IN RCC.PLLCFGR

    .clksrc                     = &VSF_HW_CLK_PLL3_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL3_R = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x10, 24, 7),// DIVR IN RCC.PLL3DIVR1
    .clken_region               = VSF_HW_CLKRST_REGION(0x0B, 29, 1),// PLL3REN IN RCC.PLLCFGR

    .clksrc                     = &VSF_HW_CLK_PLL3_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 127,
};
const vsf_hw_clk_t VSF_HW_CLK_PLL3_S = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x32, 0,  3),// DIVS IN RCC.PLL3DIVR2
    .clken_region               = VSF_HW_CLKRST_REGION(0x0B, 30, 1),// PLL3SEN IN RCC.PLLCFGR

    .clksrc                     = &VSF_HW_CLK_PLL3_VCO,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_ADD1_DIV,
    .clkprescaler_min           = 0,
    .clkprescaler_max           = 7,
};

static const vsf_hw_clk_t * __VSF_HW_CLK_SYS_CLKSEL_MAPPER[8] = {
    &VSF_HW_CLK_HSI, &VSF_HW_CLK_CSI, &VSF_HW_CLK_HSE, &VSF_HW_CLK_PLL1_P,
    NULL, NULL, NULL, NULL,
};
const vsf_hw_clk_t VSF_HW_CLK_SYS = {
    .clksel_region              = VSF_HW_CLKRST_REGION(0x04, 0,  3),// SW IN RCC.CFGR

    .clksel_mapper              = __VSF_HW_CLK_SYS_CLKSEL_MAPPER,
    .clktype                    = VSF_HW_CLK_TYPE_SEL,
};
static const uint8_t __VSF_HW_CLK_SYSCPU_PRESCALER[16] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9,
};
const vsf_hw_clk_t VSF_HW_CLK_SYS_CPU = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x06, 0,  4),// CPRE IN RCC.CDCFGR

    .clksrc                     = &VSF_HW_CLK_SYS,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_SYSCPU_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_SFT,

    .clkprescaler_min           = 0,
    .clkprescaler_max           = 9,
};
const vsf_hw_clk_t VSF_HW_CLK_SYS_BUS = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x07, 0,  4),// BMPRE IN RCC.BMCFGR

    .clksrc                     = &VSF_HW_CLK_SYS_CPU,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_SYSCPU_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_SFT,

    .clkprescaler_min           = 0,
    .clkprescaler_max           = 9,
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
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x08, 0,  3),// PPRE1 IN RCC.APBCFGR

    .clksrc                     = &VSF_HW_CLK_HCLK,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_PCLK_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_SFT,

    .clkprescaler_min           = 0,
    .clkprescaler_max           = 7,
};
const vsf_hw_clk_t VSF_HW_CLK_PCLK2 = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x08, 4,  3),// PPRE2 IN RCC.APBCFGR

    .clksrc                     = &VSF_HW_CLK_HCLK,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_PCLK_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_SFT,

    .clkprescaler_min           = 0,
    .clkprescaler_max           = 7,
};
const vsf_hw_clk_t VSF_HW_CLK_PCLK4 = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x08, 8,  3),// PPRE4 IN RCC.APBCFGR

    .clksrc                     = &VSF_HW_CLK_HCLK,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_PCLK_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_SFT,

    .clkprescaler_min           = 0,
    .clkprescaler_max           = 7,
};
const vsf_hw_clk_t VSF_HW_CLK_PCLK5 = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x08, 12, 3),// PPRE5 IN RCC.APBCFGR

    .clksrc                     = &VSF_HW_CLK_HCLK,
    .clktype                    = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_PCLK_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_SFT,

    .clkprescaler_min           = 0,
    .clkprescaler_max           = 7,
};

// USARTs

#if VSF_HAL_USE_USART == ENABLED
static const vsf_hw_clk_t * __VSF_HW_CLK_USART_APB1_CLKSEL_MAPPER[8] = {
    &VSF_HW_CLK_PCLK1, &VSF_HW_CLK_PLL2_Q, &VSF_HW_CLK_PLL3_Q,
    &VSF_HW_CLK_HSI_KER, &VSF_HW_CLK_CSI_KER, &VSF_HW_CLK_LSE, NULL, NULL
};
static const vsf_hw_clk_t * __VSF_HW_CLK_USART_APB2_CLKSEL_MAPPER[8] = {
    &VSF_HW_CLK_PCLK2, &VSF_HW_CLK_PLL2_Q, &VSF_HW_CLK_PLL3_Q,
    &VSF_HW_CLK_HSI_KER, &VSF_HW_CLK_CSI_KER, &VSF_HW_CLK_LSE, NULL, NULL
};
const vsf_hw_clk_t VSF_HW_CLK_USART1 = {
    .clksel_region              = VSF_HW_CLKSEL_USART1,
    .clksel_mapper              = __VSF_HW_CLK_USART_APB2_CLKSEL_MAPPER,
    .clktype                    = VSF_HW_CLK_TYPE_SEL,
};
const vsf_hw_clk_t VSF_HW_CLK_USART234578 = {
    .clksel_region              = VSF_HW_CLKSEL_USART234578,
    .clksel_mapper              = __VSF_HW_CLK_USART_APB1_CLKSEL_MAPPER,
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
    uint8_t pllidx = ((clk->clkprescaler_region & 0x1F) - 4) >> 3;
    VSF_HAL_ASSERT(pllidx < 3);

    uint32_t divn = pllcfgr[1 + (pllidx << 1)] & 0x1FF;
    uint32_t fracen = pllcfgr[0] & (1 << (pllidx << 2));
    uint32_t frac = ((pllcfgr[2 + (pllidx << 1)] >> 3) & 0x1FFF) * fracen;

    VSF_HAL_ASSERT(prescaler < 64);
    if (!prescaler) { prescaler = 1; }
    return (uint32_t)(((float)clksrc_freq_hz / prescaler) * ((float)divn + ((float)frac / 0x2000) + 1));
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
            VSF_HAL_ASSERT(prescaler & (prescaler - 1));
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

    uint8_t pllidx = ((clk->clkprescaler_region & 0x1F) - 4) >> 3;
    uint32_t *pllcfgr = &(((uint32_t *)RCC)[clk->clksel_region >> 16]);
    uint32_t *pll = &pllcfgr[1 + (pllidx << 1)];
    uint32_t *pllfrac = &pllcfgr[2 + (pllidx << 1)];

    uint32_t clksrc_freq_hz = vsf_hw_clk_get_freq_hz(clk->clksrc) / src_prescaler;
    VSF_HAL_ASSERT((clksrc_freq_hz >= 1000000) && (clksrc_freq_hz <= 16000000));
    uint32_t clk_div = vco_freq_hz / clksrc_freq_hz;
    VSF_HAL_ASSERT((clk_div >= 9) && (clk_div <= 420));
    uint32_t clkfrac_div = ((uint64_t)(vco_freq_hz % clksrc_freq_hz)) * (1 << 13) / clksrc_freq_hz;

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
    clk_range <<= 3;

    if (vco_freq_hz > 420000000) {
        VSF_HAL_ASSERT(clk_range >= (1 << 3));
    } else if (vco_freq_hz < 192000000) {
        VSF_HAL_ASSERT(clk_range == 0);
        clk_range |= 1 << 1;                        // PLLxVCOSEL
    }
    clk_range |= clkfrac_div == 0 ? 0 : (1 << 0);   // PLLxFRACEN
    vsf_hw_clkrst_region_set(clk->clksel_region, clk_range);
    vsf_hw_clkrst_region_set(clk->clkprescaler_region, src_prescaler);

    *pll = (*pll & ~0x1FF) | (clk_div - 1);
    *pllfrac = clkfrac_div == 0 ? 0 : clkfrac_div;
    return VSF_ERR_NONE;
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
