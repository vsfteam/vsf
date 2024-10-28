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

#include "hal/driver/ST/STM32H7RSXX/common/vendor/cmsis_device_h7rs/Include/stm32h7rsxx.h"

// in order to use color format and vk_disp_fb_drv_t
#include "component/ui/disp/vsf_disp.h"

/*============================ MACROS ========================================*/

#define LTDC_PIXFORMAT_ARGB8888         0x00
#define LTDC_PIXFORMAT_RGB888           0x01
#define LTDC_PIXFORMAT_RGB565           0x02
#define LTDC_PIXFORMAT_ARGB1555         0x03
#define LTDC_PIXFORMAT_ARGB4444         0x04
#define LTDC_PIXFORMAT_L8               0x05
#define LTDC_PIXFORMAT_AL44             0x06
#define LTDC_PIXFORMAT_AL88             0x07

#define LTDC_LAYER(__LAYER)           ((LTDC_Layer_TypeDef *)((uint32_t)LTDC + 0x84U + (0x80U * (__LAYER))))

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
    uint32_t pll3_input_clock_hz = vsf_hw_clk_get_freq_hz(&VSF_HW_CLK_PLL3_VCO);
    uint16_t pll3r = (pll3_input_clock_hz + (pixel_clock_hz >> 1)) / pixel_clock_hz;

    if (!pll3r) {
        return -1;
    }

    vsf_hw_peripheral_rst_set(VSF_HW_EN_LTDC);
    vsf_hw_peripheral_rst_clear(VSF_HW_EN_LTDC);
    vsf_hw_peripheral_enable(VSF_HW_EN_LTDC);

    vsf_hw_clk_config(&VSF_HW_CLK_PLL3_R, NULL, pll3r, 0);
    vsf_hw_clk_enable(&VSF_HW_CLK_PLL3_R);

    uint32_t htmp = timing->hsw - 1;
    uint32_t vtmp = timing->vsw - 1;
    LTDC->SSCR &= ~(LTDC_SSCR_VSH | LTDC_SSCR_HSW);
    LTDC->SSCR = vtmp | (htmp << 16U);

    htmp += timing->hbp;
    vtmp += timing->vbp;
    LTDC->BPCR &= ~(LTDC_BPCR_AVBP | LTDC_BPCR_AHBP);
    LTDC->BPCR = vtmp | (htmp << 16U);

    htmp += fb->width;
    vtmp += fb->height;
    LTDC->AWCR &= ~(LTDC_AWCR_AAH | LTDC_AWCR_AAW);
    LTDC->AWCR = vtmp | (htmp << 16U);

    htmp += timing->hfp;
    vtmp += timing->vfp;
    LTDC->TWCR &= ~(LTDC_TWCR_TOTALH | LTDC_TWCR_TOTALW);
    LTDC->TWCR = vtmp | (htmp << 16U);

    LTDC->BCCR = timing->bgcolor.b | (timing->bgcolor.g << 8) | (timing->bgcolor.r << 16);
    LTDC->GCR &= ~(LTDC_GCR_HSPOL | LTDC_GCR_VSPOL | LTDC_GCR_DEPOL | LTDC_GCR_PCPOL);
    LTDC->GCR |= timing->polarity;
    return 2;
}

vsf_err_t vsf_hw_fb_fini(vsf_hw_fb_t *fb)
{
    vsf_hw_peripheral_rst_set(VSF_HW_RST_LTDC);
    vsf_hw_peripheral_rst_clear(VSF_HW_RST_LTDC);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_fb_enable(vsf_hw_fb_t *fb)
{
    LTDC->GCR |= LTDC_GCR_LTDCEN;
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_fb_disable(vsf_hw_fb_t *fb)
{
    LTDC->GCR &= ~LTDC_GCR_LTDCEN;
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_fb_layer_config(vsf_hw_fb_t *fb, int layer,
        uint16_t x, uint16_t y, uint16_t w, uint16_t h,
        int color_format, uint_fast8_t alpha, uint32_t default_color,
        void *initial_pixel_buffer)
{
    VSF_HAL_ASSERT((fb != NULL) && (layer < 2) && (layer >= 0));
    vsf_hw_fb_timing_rgb_t *timing = &fb->timing.rgb;
    uint8_t pixel_byte_size = vsf_disp_get_pixel_format_bytesize(color_format);

    x += timing->hsw + timing->hbp;
    y += timing->vsw + timing->vbp;
    LTDC_LAYER(layer)->WHPCR = x | ((x + w - 1) << 16U);
    LTDC_LAYER(layer)->WVPCR = y | ((y + h - 1) << 16U);

    switch (color_format) {
    case VSF_DISP_COLOR_ARGB8888:
        LTDC_LAYER(layer)->PFCR = LTDC_PIXFORMAT_ARGB8888;
        break;
    case VSF_DISP_COLOR_RGB888_24:
        LTDC_LAYER(layer)->PFCR = LTDC_PIXFORMAT_RGB888;
        break;
    case VSF_DISP_COLOR_RGB565:
        LTDC_LAYER(layer)->PFCR = LTDC_PIXFORMAT_RGB565;
        break;
    default:
        VSF_ASSERT(false);
        break;
    }

    LTDC_LAYER(layer)->CACR = alpha;
    LTDC_LAYER(layer)->DCCR = default_color;
    LTDC_LAYER(layer)->BFCR = 0x0607;
    // frame buffer MUST be 8-byte aligned
    VSF_HAL_ASSERT(!((uint32_t)initial_pixel_buffer & 7));
    LTDC_LAYER(layer)->CFBAR = (uint32_t)initial_pixel_buffer;
    LTDC_LAYER(layer)->CFBLR &= ~(LTDC_LxCFBLR_CFBLL | (LTDC_LxCFBLR_CFBP));

    uint32_t pitch = w * pixel_byte_size;
    LTDC_LAYER(layer)->CFBLR = (pitch + 3) | (pitch << 16U);
    LTDC_LAYER(layer)->CFBLNR = h;
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_fb_layer_enable(vsf_hw_fb_t *fb, int layer)
{
    VSF_HAL_ASSERT((fb != NULL) && (layer < 2));
    LTDC_LAYER(layer)->CR |= LTDC_LxCR_LEN;
    LTDC->SRCR |= LTDC_SRCR_IMR;
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_fb_layer_disable(vsf_hw_fb_t *fb, int layer)
{
    VSF_HAL_ASSERT((fb != NULL) && (layer < 2));
    LTDC_LAYER(layer)->CR &= ~LTDC_LxCR_LEN;
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_fb_layer_present(vsf_hw_fb_t *fb, int layer, void *pixel_buffer)
{
    VSF_HAL_ASSERT((fb != NULL) && (layer < 2));
    // frame buffer MUST be 8-byte aligned
    VSF_HAL_ASSERT(!((uint32_t)pixel_buffer & 7));
    LTDC_LAYER(layer)->CFBAR = (uint32_t)pixel_buffer;
    LTDC->SRCR |= LTDC_SRCR_VBR;
    return VSF_ERR_NONE;
}

#endif
