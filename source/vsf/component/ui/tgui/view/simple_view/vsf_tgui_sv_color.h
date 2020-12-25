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

#ifndef __VSF_TINY_GUI_COLOR_H__
#define __VSF_TINY_GUI_COLOR_H__

/*============================ INCLUDES ======================================*/
#include "../../vsf_tgui_cfg.h"

#if VSF_USE_TINY_GUI == ENABLED
/*============================ MACROS ========================================*/

#ifndef VSF_TGUI_CFG_SV_SUPPORT_TRANS_RATE_ALWAY
#define VSF_TGUI_CFG_SV_SUPPORT_TRANS_RATE_ALWAY    ENABLED
#endif

#if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_ARGB_8888
#   if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#       define VSF_TGUI_COLOR_RGBA(_R, _G, _B, _A)	((vsf_tgui_sv_color_t){ .tColor = { .tChannel = {.chB = _B, .chG =_G, .chR = _R, .chA =   _A}}, .bIsColorTransparency = 1})
#       define VSF_TGUI_COLOR_RGB(_R, _G, _B)	    ((vsf_tgui_sv_color_t){ .tColor = { .tChannel = {.chB = _B, .chG =_G, .chR = _R, .chA = 0xFF}}, .bIsColorTransparency = 0})
#   else
#       define VSF_TGUI_COLOR_RGBA(_R, _G, _B, _A)	((vsf_tgui_sv_color_t){{_B, _G, _R, _A  }, 1})
#       define VSF_TGUI_COLOR_RGB(_R, _G, _B)	    ((vsf_tgui_sv_color_t){{_B, _G, _R, 0xFF}, 0})
#   endif
#elif VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_RGB16_565
#   if VSF_TGUI_CFG_SV_SUPPORT_TRANS_RATE_ALWAY == ENABLED
#       if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#           define VSF_TGUI_COLOR_RGBA(_R, _G, _B, _A)	((vsf_tgui_sv_color_t){ .tColor = { .tChannel = {.u5B = _B >> 3, .u6G =_G >> 2, .u5R = _R >> 3}}, .tTransparencyRate =  _A,  .bIsColorTransparency = 1})
#           define VSF_TGUI_COLOR_RGB(_R, _G, _B)	    ((vsf_tgui_sv_color_t){ .tColor = { .tChannel = {.u5B = _B >> 3, .u6G =_G >> 2, .u5R = _R >> 3}}, .tTransparencyRate = 0xFF, .bIsColorTransparency = 0})
#       else
#           define VSF_TGUI_COLOR_RGBA(_R, _G, _B, _A)	((vsf_tgui_sv_color_t){{{_B >> 3, _G >> 2, _R >> 3},   _A}, 1})
#           define VSF_TGUI_COLOR_RGB(_R, _G, _B)	    ((vsf_tgui_sv_color_t){{{_B >> 3, _G >> 2, _R >> 3}, 0xFF}, 0})
#       endif
#   else
#       if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#           define VSF_TGUI_COLOR_RGBA(_R, _G, _B, _A)	((vsf_tgui_sv_color_t){{ .tColor = { .tChannel = {.u5B = _B >> 3, .u6G =_G >> 2, .u5R = _R >> 3}}, .bIsColorTransparency = 0})
#       else
#           define VSF_TGUI_COLOR_RGBA(_R, _G, _B, _A)	((vsf_tgui_sv_color_t){{{_B >> 3, _G >> 2, _R >> 3}}, 0})
#       endif
#   endif
#else
#	error "TODO: add more color support"
#endif

#define VSF_TGUI_COLOR_DEF(_R, _G, _B)	    VSF_TGUI_COLOR_RGB(_R, _G, _B)

#define VSF_TGUI_COLOR_WHITE	VSF_TGUI_COLOR_RGB(0xFF, 0xFF, 0xFF)    //  Red:100%  Green:100%  Blue:100%
#define VSF_TGUI_COLOR_SILVER	VSF_TGUI_COLOR_RGB(0xC0, 0xC0, 0xC0)    //  Red: 75%  Green: 75%  Blue: 75%
#define VSF_TGUI_COLOR_GRAY		VSF_TGUI_COLOR_RGB(0x80, 0x80, 0x80)    //  Red: 50%  Green: 50%  Blue: 50%
#define VSF_TGUI_COLOR_BLACK	VSF_TGUI_COLOR_RGB(0x00, 0x00, 0x00)    //  Red:  0%  Green: 0%   Blue:  0%
#define VSF_TGUI_COLOR_RED      VSF_TGUI_COLOR_RGB(0xFF, 0x00, 0x00)    //  Red:100%  Green: 0%   Blue:  0%
#define VSF_TGUI_COLOR_MAROON	VSF_TGUI_COLOR_RGB(0x80, 0x00, 0x00)    //  Red: 50%  Green: 0%   Blue:  0%
#define VSF_TGUI_COLOR_YELLOW	VSF_TGUI_COLOR_RGB(0xFF, 0xFF, 0x00)    //  Red:100%  Green:100%  Blue:  0%
#define VSF_TGUI_COLOR_OLIVE	VSF_TGUI_COLOR_RGB(0x80, 0x80, 0x00)    //  Red: 50%  Green: 50%  Blue:  0%
#define VSF_TGUI_COLOR_LIME		VSF_TGUI_COLOR_RGB(0x00, 0xFF, 0x00)    //  Red:  0%  Green:100%  Blue:  0%
#define VSF_TGUI_COLOR_GREEN	VSF_TGUI_COLOR_RGB(0x00, 0x80, 0x00)    //  Red:  0%  Green: 50%  Blue:  0%
#define VSF_TGUI_COLOR_AQUA		VSF_TGUI_COLOR_RGB(0x00, 0xFF, 0xFF)    //  Red:  0%  Green:100%  Blue:100%
#define VSF_TGUI_COLOR_TEAL		VSF_TGUI_COLOR_RGB(0x00, 0x80, 0x80)    //  Red:  0%  Green: 50%  Blue: 50%
#define VSF_TGUI_COLOR_BLUE		VSF_TGUI_COLOR_RGB(0x00, 0x00, 0xFF)    //  Red:  0%  Green: 0%   Blue:100%
#define VSF_TGUI_COLOR_NAVY		VSF_TGUI_COLOR_RGB(0x00, 0x00, 0x80)    //  Red:  0%  Green: 0%   Blue: 50%
#define VSF_TGUI_COLOR_FUCHSIA	VSF_TGUI_COLOR_RGB(0xFF, 0x00, 0xFF)    //  Red:100%  Green: 0%   Blue:100%
#define VSF_TGUI_COLOR_PURPLE	VSF_TGUI_COLOR_RGB(0x80, 0x00, 0x80)    //  Red: 50%  Green: 0%   Blue: 50%

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/


/*----------------------------------------------------------------------------*
 *  Color                                                                     *
 *----------------------------------------------------------------------------*/

/*! \note vsf_tgui_color is for most used by view (rendering) part
 */
typedef union vsf_tgui_color_t vsf_tgui_color_t;

#if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_BGR_565
union vsf_tgui_color_t {
    implement_ex(
        struct {
            uint16_t     u5R    : 5;
            uint16_t     u6G    : 6;
            uint16_t     u5B    : 5;
        },
        tChannel
    )
    uint16_t        hwValue;
    uint16_t        Value;          //!< generic symbol name
};
#elif VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_RGB_565
union vsf_tgui_color_t {
    implement_ex(
        struct {
            uint16_t     u5B    : 5;
            uint16_t     u6G    : 6;
            uint16_t     u5R    : 5;
        },
        tChannel
    )
    uint16_t        hwValue;
    uint16_t        Value;          //!< generic symbol name
};
#elif VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_RGB8_USER_TEMPLATE
union vsf_tgui_color_t {
    uint8_t chColorID;
    uint8_t Value;                  //!< generic symbol name
};
#else /*VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_ARGB_8888 */
union vsf_tgui_color_t {
    implement_ex(
        struct {
            uint8_t     chB;
            uint8_t     chG;
            uint8_t     chR;
            uint8_t     chA;
        },
        tChannel
    )
    uint8_t         chValues[4];
    uint32_t        wValue;
    uint32_t        Value;          //!< generic symbol name
};
#endif

struct vsf_tgui_sv_color_t {
    vsf_tgui_color_t tColor;
#if (VSF_TGUI_CFG_SV_SUPPORT_TRANS_RATE_ALWAY == ENABLED) && !(__VSF_TGUI_IS_COLOR_SUPPORT_ALPHA__)
    uint8_t tTransparencyRate;
#endif

    //Noto: Temporary code, Will be removed when style is supported
    uint8_t bIsColorTransparency : 1;
};
typedef struct vsf_tgui_sv_color_t vsf_tgui_sv_color_t;

union vsf_tgui_sv_argb8888_color_t {
    implement_ex(
        struct {
                uint8_t     chB;
                uint8_t     chG;
                uint8_t     chR;
                uint8_t     chA;
        },
        tChannel
    )
    uint8_t         chValues[4];
    uint32_t        wValue;
    uint32_t        Value;          //!< generic symbol name
};
typedef union vsf_tgui_sv_argb8888_color_t vsf_tgui_sv_argb8888_color_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
extern vsf_tgui_color_t vsf_tgui_color_mix(vsf_tgui_color_t tColor0, vsf_tgui_color_t tColor1, uint_fast8_t chMix);
extern bool vsf_tgui_sv_color_is_opaque(vsf_tgui_sv_color_t tColor);
extern vsf_tgui_color_t vsf_tgui_sv_color_get_color(vsf_tgui_sv_color_t tColor);
extern uint_fast8_t vsf_tgui_sv_color_get_trans_rate(vsf_tgui_sv_color_t tColor);
extern vsf_tgui_sv_color_t vsf_tgui_sv_argb8888_to_color(vsf_tgui_sv_argb8888_color_t tARGBColor);
extern void vsf_tgui_sv_color_set_trans_rate(vsf_tgui_sv_color_t* ptColor, uint_fast8_t chRate);
#endif

#endif
/* EOF */
