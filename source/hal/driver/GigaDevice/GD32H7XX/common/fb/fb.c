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

#include "./fb.h"

#if VSF_USE_UI == ENABLED

#include "hal/vsf_hal.h"

#include "../vendor/Include/gd32h7xx_tli.h"

// in order to use color format and vk_disp_fb_drv_t
#include "component/ui/disp/vsf_disp.h"

/*============================ MACROS ========================================*/

#define TLI_PIXFORMAT_ARGB8888          0x00
#define TLI_PIXFORMAT_RGB888            0x01
#define TLI_PIXFORMAT_RGB565            0x02
#define TLI_PIXFORMAT_ARGB1555          0x03
#define TLI_PIXFORMAT_ARGB4444          0x04
#define TLI_PIXFORMAT_L8                0x05
#define TLI_PIXFORMAT_AL44              0x06
#define TLI_PIXFORMAT_AL88              0x07

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

const vk_disp_fb_drv_t vsf_disp_hw_fb_drv = {
    .init           = (vsf_err_t (*)(void *))vsf_hw_fb_init,
    .fini           = (vsf_err_t (*)(void *))vsf_hw_fb_fini,
    .enable         = (vsf_err_t (*)(void *))vsf_hw_fb_enable,
    .disable        = (vsf_err_t (*)(void *))vsf_hw_fb_disable,

    .layer          = {
        .config     = (vsf_err_t (*)(void *, int, uint16_t, uint16_t, uint16_t, uint16_t, int, uint_fast8_t, uint32_t, void *))vsf_hw_fb_layer_config,
        .enable     = (vsf_err_t (*)(void *, int))vsf_hw_fb_layer_enable,
        .disable    = (vsf_err_t (*)(void *, int))vsf_hw_fb_layer_disable,
        .present    = (vsf_err_t (*)(void *, int, void *))vsf_hw_fb_layer_present,
    },
};

const vk_disp_fb_drv_t vsf_disp_hw_fb_layer_drv = {
    .layer          = {
        .config     = (vsf_err_t (*)(void *, int, uint16_t, uint16_t, uint16_t, uint16_t, int, uint_fast8_t, uint32_t, void *))vsf_hw_fb_layer_config,
        .enable     = (vsf_err_t (*)(void *, int))vsf_hw_fb_layer_enable,
        .disable    = (vsf_err_t (*)(void *, int))vsf_hw_fb_layer_disable,
        .present    = (vsf_err_t (*)(void *, int, void *))vsf_hw_fb_layer_present,
    },
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

int_fast8_t vsf_hw_fb_init(vsf_hw_fb_t *fb)
{
    vsf_hw_fb_timing_rgb_t *timing = &fb->timing.rgb;

    uint32_t pixel_clock_hz = (timing->hsw + timing->hbp + timing->hfp + fb->width)
                            * (timing->vsw + timing->vbp + timing->vfp + fb->height)
                            * timing->fps;
    const vsf_hw_clk_t *pll2_clksrc = vsf_hw_clk_get_src(&VSF_HW_CLK_PLL2_VCO);
    uint32_t pll2_input_clock_hz = vsf_hw_clk_get_freq_hz(pll2_clksrc), pll2_vco_clock_hz;
    uint8_t pll2_input_clock_div = pll2_input_clock_hz / 1000000;
    uint16_t pll2_n, pll2r, pll2r_div;
    bool done = false;

    for (pll2r = 1; pll2r <= 128; pll2r++) {
        for (pll2r_div = 2; pll2r_div <= 16; pll2r <<= 1) {
            pll2_vco_clock_hz = pixel_clock_hz * pll2r_div * pll2r;
            pll2_n = pll2_vco_clock_hz / pll2_input_clock_hz;
            if (    (pll2_vco_clock_hz >= 150000000) && (pll2_vco_clock_hz <= 836000000)
                &&  (pll2_n >= 9) && (pll2_n <= 128)) {
                done = true;
                break;
            }
        }
        if (done) {
            break;
        }
    }
    if (!done) {
        return -1;
    }

    vsf_hw_peripheral_enable(VSF_HW_EN_TLI);
    vsf_hw_clk_disable(&VSF_HW_CLK_PLL2_VCO);
    // clock source of PLL2_VCO is ready, so can use frequency
    vsf_hw_pll_vco_config(&VSF_HW_CLK_PLL2_VCO, pll2_input_clock_div, pll2_vco_clock_hz);
    // clock sources of PLL2R and TLI are not ready, can not use frequency
    vsf_hw_clk_config(&VSF_HW_CLK_PLL2R, NULL, pll2r, 0);
    vsf_hw_clk_config(&VSF_HW_CLK_TLI, NULL, pll2r_div, 0);
    vsf_hw_clk_enable(&VSF_HW_CLK_PLL2R);
    vsf_hw_clk_enable(&VSF_HW_CLK_PLL2_VCO);

    uint32_t htmp = timing->hsw - 1;
    uint32_t vtmp = timing->vsw - 1;
    TLI_SPSZ &= ~(TLI_SPSZ_VPSZ | TLI_SPSZ_HPSZ);
    TLI_SPSZ = vtmp | (htmp << 16U);

    htmp += timing->hbp;
    vtmp += timing->vbp;
    TLI_BPSZ &= ~(TLI_BPSZ_VBPSZ | TLI_BPSZ_HBPSZ);
    TLI_BPSZ = vtmp | (htmp << 16U);

    htmp += fb->width;
    vtmp += fb->height;
    TLI_ASZ &= ~(TLI_ASZ_VASZ | TLI_ASZ_HASZ);
    TLI_ASZ = vtmp | (htmp << 16U);

    htmp += timing->hfp;
    vtmp += timing->vfp;
    TLI_TSZ &= ~(TLI_TSZ_VTSZ | TLI_TSZ_HTSZ);
    TLI_TSZ = vtmp | (htmp << 16U);

    TLI_BGC = timing->bgcolor.b | (timing->bgcolor.g << 8) | (timing->bgcolor.r << 16);
    TLI_CTL &= ~(TLI_CTL_HPPS | TLI_CTL_VPPS | TLI_CTL_DEPS | TLI_CTL_CLKPS);
    TLI_CTL |= timing->polarity;
    return 2;
}

vsf_err_t vsf_hw_fb_fini(vsf_hw_fb_t *fb)
{
    vsf_hw_peripheral_rst_set(VSF_HW_RST_TLI);
    vsf_hw_peripheral_rst_clear(VSF_HW_RST_TLI);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_fb_enable(vsf_hw_fb_t *fb)
{
    TLI_CTL |= TLI_CTL_TLIEN;
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_fb_disable(vsf_hw_fb_t *fb)
{
    TLI_CTL &= ~TLI_CTL_TLIEN;
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_fb_layer_config(vsf_hw_fb_t *fb, int layer,
        uint16_t x, uint16_t y, uint16_t w, uint16_t h,
        int color_format, uint_fast8_t alpha, uint32_t default_color,
        void *initial_pixel_buffer)
{
    VSF_HAL_ASSERT((fb != NULL) && (layer < 2));
    layer = 0 == layer ? LAYER0 : LAYER1;
    vsf_hw_fb_timing_rgb_t *timing = &fb->timing.rgb;
    uint8_t pixel_byte_size = vsf_disp_get_pixel_format_bytesize(color_format);

    x += timing->hsw + timing->hbp;
    y += timing->vsw + timing->vbp;
    TLI_LXHPOS(layer) = x | ((x + w - 1) << 16U);
    TLI_LXVPOS(layer) = y | ((y + h - 1) << 16U);

    switch (color_format) {
    case VSF_DISP_COLOR_ARGB8888:
        TLI_LXPPF(layer) = TLI_PIXFORMAT_ARGB8888;
        break;
    case VSF_DISP_COLOR_RGB888_24:
        TLI_LXPPF(layer) = TLI_PIXFORMAT_RGB888;
        break;
    case VSF_DISP_COLOR_RGB565:
        TLI_LXPPF(layer) = TLI_PIXFORMAT_RGB565;
        break;
    default:
        VSF_ASSERT(false);
        break;
    }

    TLI_LXSA(layer) = alpha;
    TLI_LXDC(layer) = default_color;
    TLI_LXBLEND(layer) &= ~(TLI_LXBLEND_ACF2 | TLI_LXBLEND_ACF1);
    TLI_LXBLEND(layer) = LAYER_ACF2_PASA | LAYER_ACF1_PASA;
    // frame buffer MUST be 8-byte aligned
    VSF_HAL_ASSERT(!((uint32_t)initial_pixel_buffer & 7));
    TLI_LXFBADDR(layer) = (uint32_t)initial_pixel_buffer;
    TLI_LXFLLEN(layer) &= ~(TLI_LXFLLEN_FLL | TLI_LXFLLEN_STDOFF);

    uint32_t pitch = w * pixel_byte_size;
    TLI_LXFLLEN(layer) = (pitch + 3) | (pitch << 16U);
    TLI_LXFTLN(layer) = h;
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_fb_layer_enable(vsf_hw_fb_t *fb, int layer)
{
    VSF_HAL_ASSERT((fb != NULL) && (layer < 2));
    layer = 0 == layer ? LAYER0 : LAYER1;
    TLI_LXCTL(layer) |= TLI_LXCTL_LEN;
    TLI_RL |= TLI_RL_RQR;
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_fb_layer_disable(vsf_hw_fb_t *fb, int layer)
{
    VSF_HAL_ASSERT((fb != NULL) && (layer < 2));
    layer = 0 == layer ? LAYER0 : LAYER1;
    TLI_LXCTL(layer) &= ~TLI_LXCTL_LEN;
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_fb_layer_present(vsf_hw_fb_t *fb, int layer, void *pixel_buffer)
{
    VSF_HAL_ASSERT((fb != NULL) && (layer < 2));
    layer = 0 == layer ? LAYER0 : LAYER1;
    // frame buffer MUST be 8-byte aligned
    VSF_HAL_ASSERT(!((uint32_t)pixel_buffer & 7));
    TLI_LXFBADDR(layer) = (uint32_t)pixel_buffer;
    TLI_RL |= TLI_RL_FBR;
    return VSF_ERR_NONE;
}

#endif
