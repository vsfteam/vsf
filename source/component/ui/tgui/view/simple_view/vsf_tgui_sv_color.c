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
#include "../../vsf_tgui_cfg.h"

#if     VSF_USE_TINY_GUI == ENABLED
declare_class(vsf_tgui_t)

#include "./vsf_tgui_sv_color.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_tgui_sv_color_t vsf_tgui_sv_color_mix(vsf_tgui_sv_color_t color_0, vsf_tgui_sv_color_t color_1, uint_fast8_t mix)
{
#if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_ARGB_8888
    color_0.value   = ((((color_0.value >> 8) & 0x00FF00FF) * mix) & 0xFF00FF00)
                    + ((((color_0.value & 0x00FF00FF) * mix) >> 8) & 0x00FF00FF);
    mix = mix ^ 0xFF;
    color_0.value  += ((((color_1.value >> 8) & 0x00FF00FF) * mix) & 0xFF00FF00)
                    + ((((color_1.value & 0x00FF00FF) * mix) >> 8) & 0x00FF00FF);
#else
    uint_fast8_t rmix = mix ^ 0xFF;
    color_0.red     = ((uint32_t)color_0.red   * mix + (uint32_t)color_1.red   * rmix) / 255;
    color_0.green   = ((uint32_t)color_0.green * mix + (uint32_t)color_1.green * rmix) / 255;
    color_0.blue    = ((uint32_t)color_0.blue  * mix + (uint32_t)color_1.blue  * rmix) / 255;
#endif

#if VSF_TGUI_SV_CFG_COLOR_HAS_ALPHA == ENABLED
    color_0.alpha = 0xFF;
#endif

    return color_0;
}

#if VSF_TGUI_CFG_COLOR_MODE != VSF_TGUI_COLOR_ARGB_8888
vsf_tgui_sv_color_t vsf_tgui_sv_argb8888_to_color(vsf_tgui_sv_color_argb8888_t rgba888_color)
{
    vsf_tgui_sv_color_t color;
#if (VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_BGR_565) ||  (VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_RGB_565)
    color.red   = rgba888_color.red   >> 3;
    color.green = rgba888_color.green >> 2;
    color.blue  = rgba888_color.blue  >> 3;
#if VSF_TGUI_SV_CFG_COLOR_HAS_ALPHA == ENABLED
    color.alpha = rgba888_color.alpha;
#endif
#else
#   error "TODO: add more color support"
#endif

    return color;
}

vsf_tgui_sv_color_argb8888_t vsf_tgui_sv_color_to_argb8888(vsf_tgui_sv_color_t color)
{
#if (VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_BGR_565) ||  (VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_RGB_565)
    vsf_tgui_sv_color_argb8888_t argb8888_color;
    argb8888_color.red   = color.red   << 3;
    argb8888_color.green = color.green << 2;
    argb8888_color.blue  = color.blue  << 3;
#if VSF_TGUI_SV_CFG_COLOR_HAS_ALPHA == ENABLED
    argb8888_color.alpha = color.alpha;
#else
    argb8888_color.alpha = 0xFF;
#endif
    return argb8888_color;
#else
#   error "TODO: add more color support"
#endif
}
#endif

#if VSF_TGUI_CFG_COLOR_MODE != VSF_TGUI_COLOR_RGB_565
vsf_tgui_sv_color_t vsf_tgui_sv_rgb565_to_color(vsf_tgui_sv_color_rgb565_t rgb565_color)
{
#if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_ARGB_8888
    vsf_tgui_sv_color_t color = {
        .red   = rgb565_color.red   << 3,
        .green = rgb565_color.green << 2,
        .blue  = rgb565_color.blue  << 3,
#if VSF_TGUI_SV_CFG_COLOR_HAS_ALPHA == ENABLED
        .alpha = rgb565_color.alpha,
#endif
    };
    return color;
#elif VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_BGR_565
    vsf_tgui_sv_color_t color = {
        .red   = rgb565_color.red   >> 3,
        .green = rgb565_color.green >> 2,
        .blue  = rgb565_color.blue  >> 3,
#if VSF_TGUI_SV_CFG_COLOR_HAS_ALPHA == ENABLED
        .alpha = rgb565_color.alpha,
#endif
    };
    return color;
#else
#   error "TODO: add more color support"
#endif
}

vsf_tgui_sv_color_rgb565_t vsf_tgui_sv_color_to_rgb565(vsf_tgui_sv_color_t color)
{
#if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_ARGB_8888
    vsf_tgui_sv_color_rgb565_t rgb565_color = {
        .red   = color.red   >> 3,
        .green = color.green >> 2,
        .blue  = color.blue  >> 3,
#if VSF_TGUI_SV_CFG_COLOR_HAS_ALPHA == ENABLED
        .alpha = color.alpha,
#endif
    };
    return rgb565_color;
#elif VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_BGR_565
    vsf_tgui_sv_color_rgb565_t rgb565_color = {
        .red   = color.red   >> 3,
        .green = color.green >> 2,
        .blue  = color.blue  >> 3,
#if VSF_TGUI_SV_CFG_COLOR_HAS_ALPHA == ENABLED
        .alpha = color.alpha,
#endif
    };
    return color;
#else
#   error "TODO: add more color support"
#endif
}
#endif

#if VSF_TGUI_CFG_COLOR_MODE != VSF_TGUI_COLOR_BGR_565
vsf_tgui_sv_color_t vsf_tgui_sv_bgr565_to_color(vsf_tgui_sv_color_bgr565_t bgr565_color)
{
#if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_ARGB_8888
    vsf_tgui_sv_color_t color = {
        .red   = bgr565_color.red   >> 3,
        .green = bgr565_color.green >> 2,
        .blue  = bgr565_color.blue  >> 3,
#if VSF_TGUI_SV_CFG_COLOR_HAS_ALPHA == ENABLED
        .alpha = bgr565_color.alpha,
#endif
    };
    return color;
#elif VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_RGB_565
    vsf_tgui_sv_color_t color = {
        .red   = bgr565_color.red   >> 3,
        .green = bgr565_color.green >> 2,
        .blue  = bgr565_color.blue  >> 3,
#if VSF_TGUI_SV_CFG_COLOR_HAS_ALPHA == ENABLED
        .alpha = bgr565_color.alpha,
#endif
    };
    return color;
#else
#   error "TODO: add more color support"
#endif
}

vsf_tgui_sv_color_bgr565_t vsf_tgui_sv_color_to_bgr565(vsf_tgui_sv_color_t color)
{
#if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_ARGB_8888
    vsf_tgui_sv_color_bgr565_t bgr565_color = {
        .red   = color.red   >> 3,
        .green = color.green >> 2,
        .blue  = color.blue  >> 3,
#if VSF_TGUI_SV_CFG_COLOR_HAS_ALPHA == ENABLED
        .alpha = color.alpha,
#endif
    };
    return bgr565_color;
#elif VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_RGB_565
    vsf_tgui_sv_color_bgr565_t bgr565_color = {
        .red   = color.red   >> 3,
        .green = color.green >> 2,
        .blue  = color.blue  >> 3,
#if VSF_TGUI_SV_CFG_COLOR_HAS_ALPHA == ENABLED
        .alpha = color.alpha,
#endif
    };
    return bgr565_color;
#else
#   error "TODO: add more color support"
#endif
}
#endif
#endif

/* EOF */
