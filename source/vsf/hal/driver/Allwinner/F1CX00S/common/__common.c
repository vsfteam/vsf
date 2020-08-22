/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

#if __IS_COMPILER_IAR__
//! expression has no effect
#   pragma diag_suppress=pe174
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void __delay_loops(int loops)
{
    while (loops-- > 0) { NOP(); };
}

static void __f1cx00s_clock_set_pll_cpu(uint_fast32_t clk)
{
    uint_fast32_t n, k, m, p;
    uint_fast32_t rval = 0;
    uint_fast32_t div = 0;

    if (clk > 720000000) {
        clk = 720000000;
    }

    if ((clk % 24000000) == 0) {
        div = clk / 24000000;
        n = div;
        k = 1;
        m = 1;
        p = 1;
    } else if ((clk % 12000000) == 0) {
        m = 2;
        div = clk / 12000000;
        if((div % 3) == 0) {
            k = 3;
        } else if((div % 4) == 0) {
            k = 4;
        } else {
            k = 2;
        }
        n = div / k;
        p = 1;
    } else {
        div = clk / 24000000;
        n = div;
        k = 1;
        m = 1;
        p = 1;
    }

    rval = CCU_BASE->PLL_CPU_CTRL;
    rval &= ~(PLL_CPU_CTRL_PLL_OUT_EVT_DIV_P() | PLL_CPU_CTRL_PLL_FACTOR_N() | PLL_CPU_CTRL_PLL_FACTOR_K() | PLL_CPU_CTRL_PLL_FACTOR_M());
    rval |= PLL_CPU_CTRL_PLL_ENABLE | PLL_CPU_CTRL_PLL_OUT_EVT_DIV_P(p) | PLL_CPU_CTRL_PLL_FACTOR_N(n) | PLL_CPU_CTRL_PLL_FACTOR_K(k) | PLL_CPU_CTRL_PLL_FACTOR_M(m);
    CCU_BASE->PLL_CPU_CTRL = rval;

    while (!(CCU_BASE->PLL_CPU_CTRL & PLL_CPU_CTRL_LOCK));
}

void f1cx00s_clock_init(uint_fast32_t pll_cpu)
{
    CCU_BASE->PLL_STABLE_TIME0 = 0x1FF;
    CCU_BASE->PLL_STABLE_TIME1 = 0x1FF;

    // switch to OSC24M
    CCU_BASE->CPU_CLK_SRC = (CCU_BASE->CPU_CLK_SRC & ~CPU_CLK_SRC_SEL) | CPU_CLK_SRC_SEL_OSC24M;
    __delay_loops(100);

    // PLL_PERIPH = 24M * N * K = 600M
    CCU_BASE->PLL_PERIPH_CTRL =     PLL_PERIPH_CTRL_PLL_ENABLE
                                |   PLL_PERIPH_CTRL_PLL_24M_OUT_EN
                                |   PLL_PERIPH_CTRL_PLL_FACTOR_N(25)
                                |   PLL_PERIPH_CTRL_PLL_FACTOR_K(1);
    __delay_loops(100);

    // AHB_CLK = PLL_PERIPH / AHB_PRE_DIV = 200M
    // APB_CLK = AHB_CLK / APB_CLK_RATIO = 100M
    CCU_BASE->AHB_APB_HCLKC_CFG =   AHB_APB_HCLKC_CFG_HCLKC_DIV(1)
                                |   AHB_APB_HCLKC_CFG_AHB_CLK_SRC_SEL_PERIPH
                                |   AHB_APB_HCLKC_CFG_APB_CLK_RATIO(1)
                                |   AHB_APB_HCLKC_CFG_AHB_PRE_DIV(3)
                                |   AHB_APB_HCLKC_CFG_AHB_CLK_DIV_RATIO(0);
    __delay_loops(100);

    CCU_BASE->DRAM_GATING |= DRAM_GATING_BE_DCLK_GATING | DRAM_GATING_FE_DCLK_GATING;
    __delay_loops(100);

    __f1cx00s_clock_set_pll_cpu(pll_cpu);

    // switch to PLL_CPU
    CCU_BASE->CPU_CLK_SRC = (CCU_BASE->CPU_CLK_SRC & ~CPU_CLK_SRC_SEL) | CPU_CLK_SRC_SEL_PLL_CPU;
    __delay_loops(100);
}

#if __IS_COMPILER_IAR__
//! no need to recover original level, diag_suppress is only valid in current file
#endif
