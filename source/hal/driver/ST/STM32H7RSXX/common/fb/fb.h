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

#ifndef __HAL_DRIVER_ST_STM32H7RSXX_FB_H__
#define __HAL_DRIVER_ST_STM32H7RSXX_FB_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_FB == ENABLED

#include "../../__device.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_USE_UI != ENABLED
#   error frame buffer driver depends on color constants in ui module
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vsf_hw_fb_color_type_t {
    VSF_HW_FB_RGB,
//    VSF_HW_FB_8080,
} vsf_hw_fb_color_type_t;

typedef enum vsf_hw_fb_polarity_t {
    VSF_HW_FB_HSYNC_POL_HIGH    = 1 << 31,
    VSF_HW_FB_HSYNC_POL_LOW     = 0 << 31,

    VSF_HW_FB_VSYNC_POL_HIGH    = 1 << 30,
    VSF_HW_FB_VSYNC_POL_LOW     = 0 << 30,

    VSF_HW_FB_DE_POL_HIGH       = 1 << 29,
    VSF_HW_FB_DE_POL_LOW        = 0 << 29,

    VSF_HW_FB_PIXELCLK_INVERT   = 1 << 28,
    VSF_HW_FB_PIXELCLK_NORMAL   = 0 << 28,
} vsf_hw_fb_polarity_t;

typedef struct vsf_hw_fb_timing_rgb_t {
    union {
        struct {
            uint16_t            h_front_porch;
            uint16_t            h_back_porch;
            uint16_t            h_sync;
            uint16_t            v_front_porch;
            uint16_t            v_back_porch;
            uint16_t            v_sync;
        };
        struct {
            uint16_t            hfp;
            uint16_t            hbp;
            uint16_t            hsw;
            uint16_t            vfp;
            uint16_t            vbp;
            uint16_t            vsw;
        };
    };

    vsf_hw_fb_polarity_t        polarity;
    uint8_t                     fps;
    // for VGA signals, de and pixel_clk are not used
    bool                        use_de;
    bool                        use_pixel_clk;

    struct {
        uint8_t                 r;
        uint8_t                 g;
        uint8_t                 b;
    } bgcolor;
} vsf_hw_fb_timing_rgb_t;

typedef struct vsf_hw_fb_t {
    vsf_hw_fb_color_type_t      color_type;
    union {
        vsf_hw_fb_timing_rgb_t  rgb;
    } timing;
    uint16_t                    width;          // in pixels
    uint16_t                    height;         // in pixels
    uint32_t                    pixel_format;   // VSF_DISP_COLOR_XXX in disp module
} vsf_hw_fb_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

// vsf_hw_fb_timing_rgb_init returns num_of_layer(> 0) on success, <= 0 on failure
extern int_fast8_t vsf_hw_fb_init(vsf_hw_fb_t *fb);
extern vsf_err_t vsf_hw_fb_fini(vsf_hw_fb_t *fb);
extern vsf_err_t vsf_hw_fb_enable(vsf_hw_fb_t *fb);
extern vsf_err_t vsf_hw_fb_disable(vsf_hw_fb_t *fb);

extern vsf_err_t vsf_hw_fb_layer_config(vsf_hw_fb_t *fb, int layer,
        uint16_t x, uint16_t y, uint16_t w, uint16_t h,
        int color_format, uint_fast8_t alpha, uint32_t default_color,
        void *initial_pixel_buffer);
extern vsf_err_t vsf_hw_fb_layer_enable(vsf_hw_fb_t *fb, int layer);
extern vsf_err_t vsf_hw_fb_layer_disable(vsf_hw_fb_t *fb, int layer);
extern vsf_err_t vsf_hw_fb_layer_present(vsf_hw_fb_t *fb, int layer, void *pixel_buffer);

#ifdef __cplusplus
}
#endif

#endif      // VSF_HAL_USE_FB
#endif
/* EOF */
