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

vsf_tgui_color_t vsf_tgui_color_mix(vsf_tgui_color_t tColor0, vsf_tgui_color_t tColor1, uint_fast8_t chMix)
{
    vsf_tgui_color_t result;

#if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_ARGB_8888
    result.tChannel.chR = ((uint32_t)tColor0.tChannel.chR * chMix + (uint32_t)tColor1.tChannel.chR * (255 - chMix)) / 255;
    result.tChannel.chG = ((uint32_t)tColor0.tChannel.chG * chMix + (uint32_t)tColor1.tChannel.chG * (255 - chMix)) / 255;
    result.tChannel.chB = ((uint32_t)tColor0.tChannel.chB * chMix + (uint32_t)tColor1.tChannel.chB * (255 - chMix)) / 255;
    result.tChannel.chA = 0xFF;
#elif VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_RGB16_565
    result.tChannel.u5R = ((uint32_t)tColor0.tChannel.u5R * chMix + (uint32_t)tColor1.tChannel.u5R * (255 - chMix)) / 255;
    result.tChannel.u6G = ((uint32_t)tColor0.tChannel.u6G * chMix + (uint32_t)tColor1.tChannel.u6G * (255 - chMix)) / 255;
    result.tChannel.u5B = ((uint32_t)tColor0.tChannel.u5B * chMix + (uint32_t)tColor1.tChannel.u5B * (255 - chMix)) / 255;
#else
#	error "TODO: add more color support"
#endif

    return result;
}

bool vsf_tgui_sv_color_is_opaque(vsf_tgui_sv_color_t tColor)
{
#if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_ARGB_8888
    return tColor.tColor.tChannel.chA == 0xFF;
#elif VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_RGB16_565
#   if VSF_TGUI_CFG_SV_SUPPORT_TRANS_RATE_ALWAY == ENABLED
    return tColor.tTransparencyRate == 0xFF;
#   else
    return true;
#   endif
#else
#	error "TODO: add more color support"
#endif
}

vsf_tgui_color_t vsf_tgui_sv_color_get_color(vsf_tgui_sv_color_t tColor)
{
    return tColor.tColor;
}

uint_fast8_t vsf_tgui_sv_color_get_trans_rate(vsf_tgui_sv_color_t tColor)
{
#if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_ARGB_8888
    return tColor.tColor.tChannel.chA;
#elif VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_RGB16_565
#   if VSF_TGUI_CFG_SV_SUPPORT_TRANS_RATE_ALWAY == ENABLED
    return tColor.tTransparencyRate;
#   else
    return 0xFF;
#   endif
#else
#	error "TODO: add more color support"
#endif
}

void vsf_tgui_sv_color_set_trans_rate(vsf_tgui_sv_color_t *ptColor, uint_fast8_t chRate)
{
#if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_ARGB_8888
    ptColor->tColor.tChannel.chA = chRate;
#elif VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_RGB16_565
#   if VSF_TGUI_CFG_SV_SUPPORT_TRANS_RATE_ALWAY == ENABLED
    ptColor->tTransparencyRate = chRate;
#   endif
#else
#	error "TODO: add more color support"
#endif
}


vsf_tgui_sv_color_t vsf_tgui_sv_argb8888_to_color(vsf_tgui_sv_argb8888_color_t tARGBColor)
{
    vsf_tgui_sv_color_t tColor;
#if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_ARGB_8888
    tColor.tColor.tChannel.chA = tARGBColor.tChannel.chA;
    tColor.tColor.tChannel.chR = tARGBColor.tChannel.chR;
    tColor.tColor.tChannel.chG = tARGBColor.tChannel.chG;
    tColor.tColor.tChannel.chB = tARGBColor.tChannel.chB;
#elif VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_RGB16_565
    tColor.tColor.tChannel.u5R = tARGBColor.tChannel.chR >> 3;
    tColor.tColor.tChannel.u6G = tARGBColor.tChannel.chG >> 2;
    tColor.tColor.tChannel.u5B = tARGBColor.tChannel.chB >> 3;
#if VSF_TGUI_CFG_SV_SUPPORT_TRANS_RATE_ALWAY == ENABLED
    tColor.tTransparencyRate = tARGBColor.tChannel.chA;
#endif
#else
#	error "TODO: add more color support"
#endif
    return tColor;
}

#endif

/* EOF */
