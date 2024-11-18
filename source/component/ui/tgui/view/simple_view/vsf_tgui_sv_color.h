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

#ifndef __VSF_TINY_GUI_COLOR_H__
#define __VSF_TINY_GUI_COLOR_H__

/*============================ INCLUDES ======================================*/
#include "../../vsf_tgui_cfg.h"

#if VSF_USE_TINY_GUI == ENABLED
/*============================ MACROS ========================================*/

#ifndef VSF_TGUI_SV_CFG_COLOR_HAS_ALPHA
#   define VSF_TGUI_SV_CFG_COLOR_HAS_ALPHA              ENABLED
#endif

#if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_ARGB_8888
#   undef VSF_TGUI_SV_CFG_COLOR_HAS_ALPHA
#   define VSF_TGUI_SV_CFG_COLOR_HAS_ALPHA              ENABLED
#endif

#if VSF_TGUI_SV_CFG_COLOR_HAS_ALPHA == ENABLED
#   define __VSF_TGUI_COLOR_RGBA(__R, __G, __B, __A)    ((vsf_tgui_sv_color_t){.red = __R, .green = __G, .blue = __B, .alpha = __A})
#else
#   define __VSF_TGUI_COLOR_RGBA(__R, __G, __B, __A)    ((vsf_tgui_sv_color_t){.red = __R, .green = __G, .blue = __B,})
#endif

#if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_ARGB_8888
#   define VSF_TGUI_COLOR_RGBA(__R, __G, __B, __A)      __VSF_TGUI_COLOR_RGBA(__R, __G, __B, __A)
#elif VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_RGB16_565
#   define VSF_TGUI_COLOR_RGBA(__R, __G, __B, __A)      __VSF_TGUI_COLOR_RGBA(__R >> 3, __G >> 2, __B >> 3, __A)
#else
#   error "TODO: add more color support"
#endif
#define VSF_TGUI_COLOR_RGB(__R, __G, __B)               VSF_TGUI_COLOR_RGBA(__R, __G, __B, 0xFF)

#if VSF_TGUI_SV_CFG_COLOR_HAS_ALPHA == ENABLED
#   define VSF_TGUI_COLOR_TRANSPARENT                   VSF_TGUI_COLOR_RGBA(0, 0, 0, 0)
#endif
#define VSF_TGUI_COLOR_WHITE                            VSF_TGUI_COLOR_RGB(0xFF, 0xFF, 0xFF)    //  Red:100%  Green:100%  Blue:100%
#define VSF_TGUI_COLOR_SILVER                           VSF_TGUI_COLOR_RGB(0xC0, 0xC0, 0xC0)    //  Red: 75%  Green: 75%  Blue: 75%
#define VSF_TGUI_COLOR_GRAY                             VSF_TGUI_COLOR_RGB(0x80, 0x80, 0x80)    //  Red: 50%  Green: 50%  Blue: 50%
#define VSF_TGUI_COLOR_BLACK                            VSF_TGUI_COLOR_RGB(0x00, 0x00, 0x00)    //  Red:  0%  Green: 0%   Blue:  0%
#define VSF_TGUI_COLOR_RED                              VSF_TGUI_COLOR_RGB(0xFF, 0x00, 0x00)    //  Red:100%  Green: 0%   Blue:  0%
#define VSF_TGUI_COLOR_MAROON                           VSF_TGUI_COLOR_RGB(0x80, 0x00, 0x00)    //  Red: 50%  Green: 0%   Blue:  0%
#define VSF_TGUI_COLOR_YELLOW                           VSF_TGUI_COLOR_RGB(0xFF, 0xFF, 0x00)    //  Red:100%  Green:100%  Blue:  0%
#define VSF_TGUI_COLOR_OLIVE                            VSF_TGUI_COLOR_RGB(0x80, 0x80, 0x00)    //  Red: 50%  Green: 50%  Blue:  0%
#define VSF_TGUI_COLOR_LIME                             VSF_TGUI_COLOR_RGB(0x00, 0xFF, 0x00)    //  Red:  0%  Green:100%  Blue:  0%
#define VSF_TGUI_COLOR_GREEN                            VSF_TGUI_COLOR_RGB(0x00, 0x80, 0x00)    //  Red:  0%  Green: 50%  Blue:  0%
#define VSF_TGUI_COLOR_AQUA                             VSF_TGUI_COLOR_RGB(0x00, 0xFF, 0xFF)    //  Red:  0%  Green:100%  Blue:100%
#define VSF_TGUI_COLOR_TEAL                             VSF_TGUI_COLOR_RGB(0x00, 0x80, 0x80)    //  Red:  0%  Green: 50%  Blue: 50%
#define VSF_TGUI_COLOR_BLUE                             VSF_TGUI_COLOR_RGB(0x00, 0x00, 0xFF)    //  Red:  0%  Green: 0%   Blue:100%
#define VSF_TGUI_COLOR_NAVY                             VSF_TGUI_COLOR_RGB(0x00, 0x00, 0x80)    //  Red:  0%  Green: 0%   Blue: 50%
#define VSF_TGUI_COLOR_FUCHSIA                          VSF_TGUI_COLOR_RGB(0xFF, 0x00, 0xFF)    //  Red:100%  Green: 0%   Blue:100%
#define VSF_TGUI_COLOR_PURPLE                           VSF_TGUI_COLOR_RGB(0x80, 0x00, 0x80)    //  Red: 50%  Green: 0%   Blue: 50%

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef struct vsf_tgui_sv_color_rgb565_t {
    union {
        uint16_t     value;
        struct {
            uint16_t blue  : 5;
            uint16_t green : 6;
            uint16_t red   : 5;
        };
    };
#if VSF_TGUI_SV_CFG_COLOR_HAS_ALPHA == ENABLED
    uint8_t  alpha;
#endif
} vsf_tgui_sv_color_rgb565_t;

typedef struct vsf_tgui_sv_color_bgr565_t {
    union {
        uint16_t     value;
        struct {
            uint16_t red   : 5;
            uint16_t green : 6;
            uint16_t blue  : 5;
        };
    };
#if VSF_TGUI_SV_CFG_COLOR_HAS_ALPHA == ENABLED
    uint8_t  alpha;
#endif
} vsf_tgui_sv_color_bgr565_t;

typedef union vsf_tgui_sv_color_argb8888_t {
    uint32_t     value;
    struct {
        uint8_t  blue;
        uint8_t  green;
        uint8_t  red;
        uint8_t  alpha;
    };
} vsf_tgui_sv_color_argb8888_t;

#if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_BGR_565
typedef vsf_tgui_sv_color_bgr565_t vsf_tgui_sv_color_t;
#elif VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_RGB_565
typedef vsf_tgui_sv_color_rgb565_t vsf_tgui_sv_color_t;
#elif VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_ARGB_8888
typedef vsf_tgui_sv_color_argb8888_t vsf_tgui_sv_color_t;
#else
typedef VSF_TGUI_CFG_COLOR_TYPE vsf_tgui_sv_color_t;
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_TGUI_SV_CFG_MIX_AS_FUNCTION == ENABLED
extern vsf_tgui_sv_color_t vsf_tgui_sv_color_mix(vsf_tgui_sv_color_t color_0, vsf_tgui_sv_color_t color_1, uint_fast8_t mix);
#else
#   if VSF_TGUI_SV_CFG_COLOR_HAS_ALPHA == ENABLED
#       define __vsf_tgui_sv_color_mix_alpha()                                  \
            __vsf_tgui_sv_color_mix_result__.alpha = 0xFF;
#   else
#       define __vsf_tgui_sv_color_mix_alpha()
#   endif

#   if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_ARGB_8888
#       define vsf_tgui_sv_color_mix(__color_0, __color_1, __mix) ({            \
        uint_fast8_t __vsf_tgui_sv_color_mix__ = (__mix);                       \
        vsf_tgui_sv_color_t __vsf_tgui_sv_color_mix_result__;                   \
        __vsf_tgui_sv_color_mix_result__.value =                                \
                (((((__color_0).value >> 8) & 0x00FF00FF) * __vsf_tgui_sv_color_mix__) & 0xFF00FF00)\
            +   (((((__color_0).value & 0x00FF00FF) * __vsf_tgui_sv_color_mix__) >> 8) & 0x00FF00FF);\
        __vsf_tgui_sv_color_mix__ = __vsf_tgui_sv_color_mix__ ^ 0xFF;           \
        __vsf_tgui_sv_color_mix_result__.value +=                               \
                (((((__color_1).value >> 8) & 0x00FF00FF) * __vsf_tgui_sv_color_mix__) & 0xFF00FF00)    \
            +   (((((__color_1).value & 0x00FF00FF) * __vsf_tgui_sv_color_mix__) >> 8) & 0x00FF00FF);   \
        __vsf_tgui_sv_color_mix_alpha();                                        \
        __vsf_tgui_sv_color_mix_result__;                                       \
    })
#   else
#       define vsf_tgui_sv_color_mix(__color_0, __color_1, __mix) ({            \
        vsf_tgui_sv_color_t __vsf_tgui_sv_color_mix_result__;                   \
        uint_fast8_t __vsf_tgui_sv_color_rmix__ = (__mix) ^ 0xFF;               \
        __vsf_tgui_sv_color_mix_result__.red     = ((uint32_t)(__color_0).red   * (__mix) + (uint32_t)(__color_1).red   * __vsf_tgui_sv_color_rmix__) / 255;\
        __vsf_tgui_sv_color_mix_result__.green   = ((uint32_t)(__color_0).green * (__mix) + (uint32_t)(__color_1).green * __vsf_tgui_sv_color_rmix__) / 255;\
        __vsf_tgui_sv_color_mix_result__.blue    = ((uint32_t)(__color_0).blue  * (__mix) + (uint32_t)(__color_1).blue  * __vsf_tgui_sv_color_rmix__) / 255;\
        __vsf_tgui_sv_color_mix_alpha();                                        \
        __vsf_tgui_sv_color_mix_result__;                                       \
    })
#   endif
#endif

#if VSF_TGUI_SV_CFG_COLOR_HAS_ALPHA == ENABLED
#   define vsf_tgui_sv_color_get_trans_rate(__color)    ((__color).alpha)
#   define vsf_tgui_sv_color_set_trans_rate(__color_ptr, __alpha)   do { (__color_ptr)->alpha = (__alpha); } while (0)
#   define vsf_tgui_sv_color_is_opaque(__color)         ((__color).alpha == 0xFF)
#else
#   define vsf_tgui_sv_color_get_trans_rate(__color)    0xFF
#   define vsf_tgui_sv_color_set_trans_rate(__color_ptr, __alpha)
#   define vsf_tgui_sv_color_is_opaque(__color)         true
#endif

#if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_ARGB_8888
#   define vsf_tgui_sv_argb8888_to_color(__color)       (__color)
#else
extern vsf_tgui_sv_color_t vsf_tgui_sv_argb8888_to_color(vsf_tgui_sv_color_argb8888_t argb8888_color);
#endif
#if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_RGB_565
#   define vsf_tgui_sv_rgb565_to_color(__color)         (__color)
#else
extern vsf_tgui_sv_color_t vsf_tgui_sv_rgb565_to_color(vsf_tgui_sv_color_rgb565_t rgb565_color);
#endif
#if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_BGR_565
#   define vsf_tgui_sv_bgr565_to_color(__color)         (__color)
#else
extern vsf_tgui_sv_color_t vsf_tgui_sv_bgr565_to_color(vsf_tgui_sv_color_bgr565_t bgr565_color);
#endif

#if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_ARGB_8888
#   define vsf_tgui_sv_color_to_argb8888(__color)       (__color)
#else
extern vsf_tgui_sv_color_argb8888_t vsf_tgui_sv_color_to_argb8888(vsf_tgui_sv_color_t color);
#endif
#if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_RGB_565
#   define vsf_tgui_sv_color_to_rgb565(__color)         (__color)
#else
extern vsf_tgui_sv_color_rgb565_t   vsf_tgui_sv_color_to_rgb565(vsf_tgui_sv_color_t color);
#endif
#if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_BGR_565
#   define vsf_tgui_sv_color_to_bgr565(__color)         (__color)
#else
extern vsf_tgui_sv_color_bgr565_t   vsf_tgui_sv_color_to_bgr565(vsf_tgui_sv_color_t color);
#endif

#endif

#endif
/* EOF */
