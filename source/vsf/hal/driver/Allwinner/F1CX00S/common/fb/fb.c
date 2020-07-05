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

#include "./fb.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

const i_fb_t VSF_FB = {
    .Init       = (vsf_err_t (*)(void *, void *))f1cx00s_fb_init,
    .Fini       = (vsf_err_t (*)(void *))f1cx00s_fb_fini,
    .Present    = (vsf_err_t (*)(void *, void *))f1cx00s_fb_present,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t f1cx00s_fb_init(f1cx00s_fb_t *fb, void *initial_pixel_buffer)
{
    VSF_HAL_ASSERT((fb != NULL) && (fb->param != NULL));

    const f1cx00s_fb_param_t *param = fb->param;
    uint_fast32_t reg_tmp;

    // initialize clock gating
    CCU_BASE->FE_CLK |= FE_CLK_SCLK_GATING;
    CCU_BASE->BE_CLK |= BE_CLK_SCLK_GATING;
    CCU_BASE->TCON_CLK |= TCON_CLK_SCLK_GATING;
    CCU_BASE->BUS_CLK_GATING1 |= BUS_CLK_GATING1_DEFE_GATING | BUS_CLK_GATING1_DEBE_GATING | BUS_CLK_GATING1_LCD_GATING;

    // deassert reset
    CCU_BASE->BUS_SOFT_RST1 |= BUS_SOFT_RST1_DEFE_RST | BUS_SOFT_RST1_DEBE_RST | BUS_SOFT_RST1_LCD_RST;

    // clear debe
    uint32_t * debe_reg = (uint32_t *)&DEBE_BASE->MODE_CTRL;
    for (uint_fast32_t i = 0; i < 0x800; i+= 4) {
        *debe_reg++ = 0;
    }

    // enable video pll
    // TODO: configure PLL_VIDEO according to pixel_clock_hz
    // PLL_VIDEO = (24M * N) / M = 200M
    const uint_fast8_t n = 25, m = 3;
    uint_fast32_t pll_video = ((24UL * 1000 * 1000) * n) / m;
    CCU_BASE->PLL_VIDEO_CTRL =      PLL_VIDEO_CTRL_PLL_ENABLE
                                |   PLL_VIDEO_CTRL_PLL_MODE_INTEGER
                                |   PLL_VIDEO_CTRL_PLL_FACTOR_N(n)
                                |   PLL_VIDEO_CTRL_PLL_PREDIV_M(m);
    __delay_loops(100);

    // initialize debe clock
    CCU_BASE->BE_CLK = (CCU_BASE->BE_CLK & ~(BE_CLK_CLK_SRC_SEL | BE_CLK_CLK_DIV_RATIO_M()))
                    | BE_CLK_CLK_SRC_SEL_PLL_VIDEO | BE_CLK_CLK_DIV_RATIO_M(1);
    // initialize defe clock
    CCU_BASE->FE_CLK = (CCU_BASE->FE_CLK & ~(FE_CLK_CLK_SRC_SEL | FE_CLK_CLK_DIV_RATIO_M()))
                    | FE_CLK_CLK_SRC_SEL_PLL_VIDEO | FE_CLK_CLK_DIV_RATIO_M(1);

    // init gpio: PORTD
    // function 2
    PIO_BASE->PORTD.CFG0 = 0x22222222;
    PIO_BASE->PORTD.CFG1 = 0x22222222;
    PIO_BASE->PORTD.CFG2 = 0x00222222;
    // pull none
    PIO_BASE->PORTD.PUL0 = 0;
    PIO_BASE->PORTD.PUL1 = 0;
    // drive level3
    PIO_BASE->PORTD.DRV0 = 0xFFFFFFFF;
    PIO_BASE->PORTD.DRV1 = 0x00000FFF;

    // disable tcon
    TCON_BASE->CTRL &= ~TCON_CTRL_MODULE_EN;
    TCON_BASE->INT_REG0 = 0;
    TCON_BASE->TCON0.CTRL &= ~TCON0_CTRL_EN;
    TCON_BASE->TCON0.CLK_CTRL &= ~TCON0_CLK_CTRL_LCKL_EN;
    TCON_BASE->TCON0.IO_CTRL1 = 0xFFFFFFFF;
    TCON_BASE->TCON1.CTRL &= ~TCON1_CTRL_EN;
    TCON_BASE->TCON1.IO_CTRL1 = 0xFFFFFFFF;

    // initialize debe mode
    DEBE_BASE->MODE_CTRL = DEBE_MODE_CTRL_DEBE_EN;
    DEBE_BASE->REGBUFF_CTRL = 0;
    reg_tmp = ((fb->height - 1) << 16) | ((fb->width - 1) << 0);
    DEBE_BASE->DISP_SIZE = reg_tmp;
    // initialize debe layer0
    DEBE_BASE->LAY_SIZE[0] = reg_tmp;
    if (2 == fb->pixel_byte_size) {
        DEBE_BASE->LAY_LINEWIDTH[0] = fb->width << 4;
    } else if (4 == fb->pixel_byte_size) {
        DEBE_BASE->LAY_LINEWIDTH[0] = fb->width << 5;
    } else {
        VSF_HAL_ASSERT(false);
    }
    // set frame buffer
    f1cx00s_fb_present(fb, initial_pixel_buffer);
    // TODO: fix layer attribute
    DEBE_BASE->LAY_ATT_CTRL0[0] = 0;                    // pipe0, none-alpha
    DEBE_BASE->LAY_ATT_CTRL1[0] = param->color_format;
    DEBE_BASE->MODE_CTRL |= DEBE_MODE_CTRL_LAYER_EN(0);
    DEBE_BASE->REGBUFF_CTRL = DEBE_REGBUFF_CTRL_RELOAD;
    DEBE_BASE->MODE_CTRL |= DEBE_MODE_CTRL_CHANNEL_START;

    // initialize tcon mode
    switch (param->lcd_type) {
    case F1C100S_LCD_TYPE_RGB: {
            const f1cx00s_fb_rgb_timing_t *timing = &param->timing.rgb;
            TCON_BASE->CTRL &= ~TCON_CTRL_IO_MAP_SEL;   // use TCON0
            TCON_BASE->TCON0.CTRL = TCON0_CTRL_EN | TCON0_CTRL_IF_HV
                    | TCON0_CTRL_STA_DLY((timing->v_front_porch + timing->v_back_porch + timing->v_sync) & 0x1F);

            uint_fast8_t div = pll_video / timing->pixel_clock_hz;
            uint_fast32_t remain = pll_video % timing->pixel_clock_hz;
            if (remain) {
                div++;
            }
            TCON_BASE->TCON0.CLK_CTRL = TCON0_CLK_CTRL_LCKL_EN | TCON0_CLK_CTRL_DCLKDIV(div);

            uint_fast32_t bp, total;
            // active
            TCON_BASE->TCON0.BASIC_TIMING0 = ((fb->width - 1) << 16) | ((fb->height - 1) << 0);
            // horizontal
            bp = timing->h_sync + timing->h_back_porch;
            total = fb->width + timing->h_front_porch + bp;
            TCON_BASE->TCON0.BASIC_TIMING1 = ((total - 1) << 16) | ((bp - 1) << 0);
            // vertical
            bp = timing->v_sync + timing->v_back_porch;
            total = fb->height + timing->v_front_porch + bp;
            TCON_BASE->TCON0.BASIC_TIMING2 = ((total * 2) << 16) | ((bp - 1) << 0);
            // sync
            TCON_BASE->TCON0.BASIC_TIMING3 = ((timing->h_sync - 1) << 16) | ((timing->v_sync - 1) << 0);
            TCON_BASE->TCON0.HV_TIMING = 0;
            TCON_BASE->TCON0.CPU_IF = 0;

            if ((18 == fb->pixel_bit_size) || (16 == fb->pixel_bit_size)) {
                TCON_BASE->FRM_SEED[0]  = 0x11111111;
                TCON_BASE->FRM_SEED[1]  = 0x11111111;
                TCON_BASE->FRM_SEED[2]  = 0x11111111;
                TCON_BASE->FRM_SEED[3]  = 0x11111111;
                TCON_BASE->FRM_SEED[4]  = 0x11111111;
                TCON_BASE->FRM_SEED[5]  = 0x11111111;
                TCON_BASE->FRM_TBL[0]   = 0x01010000;
                TCON_BASE->FRM_TBL[1]   = 0x15151111;
                TCON_BASE->FRM_TBL[2]   = 0x57575555;
                TCON_BASE->FRM_TBL[3]   = 0x7f7f7777;
                TCON_BASE->FRM_CTRL     = TCON_FRM_CTRL_TCON0_FRM_EN
                    | ((18 == fb->pixel_bit_size) ?
                            TCON_FRM_CTRL_TCON0_FRM_MODE_R6 | TCON_FRM_CTRL_TCON0_FRM_MODE_G6 | TCON_FRM_CTRL_TCON0_FRM_MODE_B6
                        :   TCON_FRM_CTRL_TCON0_FRM_MODE_R5 | TCON_FRM_CTRL_TCON0_FRM_MODE_G6 | TCON_FRM_CTRL_TCON0_FRM_MODE_B5);
            }

            TCON_BASE->TCON0.IO_CTRL0 = TCON0_IO_CTRL0_DCLK_SEL_DCLK1
                                    |   (timing->den_active     ? 0 : TCON0_IO_CTRL0_IO3_INV)
                                    |   (timing->clk_active     ? 0 : TCON0_IO_CTRL0_IO2_INV)
                                    |   (timing->h_sync_active  ? 0 : TCON0_IO_CTRL0_IO1_INV)
                                    |   (timing->v_sync_active  ? 0 : TCON0_IO_CTRL0_IO0_INV);
            TCON_BASE->TCON0.IO_CTRL1 = 0;
        }
        break;
    case F1C100S_LCD_TYPE_CPU:
        TCON_BASE->CTRL &= ~TCON_CTRL_IO_MAP_SEL;       // use TCON0
        return VSF_ERR_NOT_SUPPORT;
    case F1C100S_LCD_TYPE_TV_OUT:
        TCON_BASE->CTRL |= TCON_CTRL_IO_MAP_SEL;        // use TCON1
        return VSF_ERR_NOT_SUPPORT;
    default:
        return VSF_ERR_NOT_SUPPORT;
    }

    // enable tcon
    TCON_BASE->CTRL |= TCON_CTRL_MODULE_EN;
    return VSF_ERR_NONE;
}

vsf_err_t f1cx00s_fb_fini(f1cx00s_fb_t *fb)
{
    CCU_BASE->FE_CLK &= ~FE_CLK_SCLK_GATING;
    CCU_BASE->BE_CLK &= ~BE_CLK_SCLK_GATING;
    CCU_BASE->TCON_CLK &= ~TCON_CLK_SCLK_GATING;
    CCU_BASE->BUS_CLK_GATING1 &= ~(BUS_CLK_GATING1_DEFE_GATING | BUS_CLK_GATING1_DEBE_GATING | BUS_CLK_GATING1_LCD_GATING);
    return VSF_ERR_NONE;
}

vsf_err_t f1cx00s_fb_present(f1cx00s_fb_t *fb, void *pixel_buffer)
{
    vsf_cache_sync(pixel_buffer, fb->pixel_byte_size * fb->width * fb->height, false);
    DEBE_BASE->LAY_FB_ADDR0[0] = (uint32_t)pixel_buffer << 3;
    DEBE_BASE->LAY_FB_ADDR1[0] = (uint32_t)pixel_buffer >> 29;
    return VSF_ERR_NONE;
}
