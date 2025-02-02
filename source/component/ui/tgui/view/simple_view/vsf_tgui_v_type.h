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

#ifndef __VSF_TINY_GUI_V_TMPLATE_TYPE_H__
#define __VSF_TINY_GUI_V_TMPLATE_TYPE_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_tgui_cfg.h"

#if     VSF_USE_TINY_GUI == ENABLED                                             \
    &&  VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL == VSF_TGUI_V_TEMPLATE_SIMPLE_VIEW

#include "./vsf_tgui_sv_color.h"

#include "../../port/view/vsf_tgui_sv_port.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_TGUI_CFG_V_SUPPORT_FLUXIBLE_BACKGROUND_COLOR
#   define VSF_TGUI_CFG_V_SUPPORT_FLUXIBLE_BACKGROUND_COLOR     ENABLED
#endif

#if VSF_TGUI_CFG_V_SUPPORT_FLUXIBLE_BACKGROUND_COLOR == DISLABED
#ifndef VSF_TGUI_CFG_V_SUPPORT_FIXED_BACKGROUND_COLOR
#   define VSF_TGUI_CFG_V_SUPPORT_FIXED_BACKGROUND_COLOR        ENABLED
#endif
#endif

#ifndef VSF_TGUI_CFG_V_SUPPORT_TILE_TRANSPARENCY
#   define VSF_TGUI_CFG_V_SUPPORT_TILE_TRANSPARENCY             ENABLED
#endif

#ifndef VSF_TGUI_CFG_V_SUPPORT_CORNER_TILE
#   define VSF_TGUI_CFG_V_SUPPORT_CORNER_TILE                   ENABLED
#endif

#ifndef VSF_TGUI_CFG_V_LABLE_SUPPORT_TEXT_COLOR
#   define VSF_TGUI_CFG_V_LABLE_SUPPORT_TEXT_COLOR              ENABLED
#endif

#ifndef VSF_TGUI_CFG_V_CONTROL_BACKGROUND_COLOR
#   define VSF_TGUI_CFG_V_CONTROL_BACKGROUND_COLOR              VSF_TGUI_COLOR_RGB(0xB4, 0xC7, 0xE7)
#endif

#ifndef VSF_TGUI_CFG_V_LABEL_BACKGROUND_COLOR
#   define VSF_TGUI_CFG_V_LABEL_BACKGROUND_COLOR                VSF_TGUI_COLOR_RGB(0xB4, 0xC7, 0xE7)
#endif

#ifndef VSF_TGUI_CFG_V_LABEL_TEXT_COLOR
#   define VSF_TGUI_CFG_V_LABEL_TEXT_COLOR                      VSF_TGUI_COLOR_BLACK
#endif

#ifndef VSF_TGUI_CFG_V_LABEL_DISABLED_TEXT_MIX_COLOR
#   define VSF_TGUI_CFG_V_LABEL_DISABLED_TEXT_MIX_COLOR         VSF_TGUI_COLOR_WHITE
#endif

#ifndef VSF_TGUI_CFG_V_LABEL_DISABLED_TEXT_MIX_VALUE
#   define VSF_TGUI_CFG_V_LABEL_DISABLED_TEXT_MIX_VALUE         128
#endif

#ifndef VSF_TGUI_CFG_V_BUTTON_BACKGROUND_COLOR
#   define VSF_TGUI_CFG_V_BUTTON_BACKGROUND_COLOR               VSF_TGUI_COLOR_RGB(0xB4, 0xC7, 0xE7)
#endif

#ifndef VSF_TGUI_CFG_V_BUTTON_CLICKED_BACKGROUND_COLOR
#   define VSF_TGUI_CFG_V_BUTTON_CLICKED_BACKGROUND_COLOR       VSF_TGUI_COLOR_BLUE
#endif

#ifndef VSF_TGUI_CFG_V_BUTTON_CLICKED_MIX_COLOR
#   define VSF_TGUI_CFG_V_BUTTON_CLICKED_MIX_COLOR              VSF_TGUI_COLOR_BLUE
#endif

#ifndef VSF_TGUI_CFG_V_BUTTON_CLICKED_MIX_VALUE
#   define VSF_TGUI_CFG_V_BUTTON_CLICKED_MIX_VALUE              128
#endif

#ifndef VSF_TGUI_CFG_V_BUTTON_TEXT_COLOR
#   define VSF_TGUI_CFG_V_BUTTON_TEXT_COLOR                     VSF_TGUI_COLOR_BLACK
#endif

#ifndef VSF_TGUI_CFG_V_CONTAINER_BACKGROUND_COLOR
#   define VSF_TGUI_CFG_V_CONTAINER_BACKGROUND_COLOR            VSF_TGUI_COLOR_RGB(0x44, 0x72, 0xC4)
#endif

#ifndef VSF_TGUI_CFG_V_PANEL_BACKGROUND_COLOR
#   define VSF_TGUI_CFG_V_PANEL_BACKGROUND_COLOR                VSF_TGUI_COLOR_RGB(0x44, 0x72, 0xC4)
#endif

#ifndef VSF_TGUI_CFG_V_LIST_BACKGROUND_COLOR
#   define VSF_TGUI_CFG_V_LIST_BACKGROUND_COLOR                 VSF_TGUI_COLOR_RGBA(0xB4, 0xC7, 0xE7, 0x00)
#endif

#ifndef VSF_TGUI_CFG_V_TEXT_LIST_BACKGROUND_COLOR
#   define VSF_TGUI_CFG_V_TEXT_LIST_BACKGROUND_COLOR            VSF_TGUI_COLOR_RGB(0xB4, 0xC7, 0xE7)
#endif

#ifndef VSF_TGUI_CFG_V_TEXT_LIST_INDICATOR_COLOR
#   define VSF_TGUI_CFG_V_TEXT_LIST_INDICATOR_COLOR             VSF_TGUI_COLOR_RGB(0x44, 0x72, 0xC4)
#endif

#if VSF_TGUI_CFG_V_SUPPORT_FLUXIBLE_BACKGROUND_COLOR == ENABLED
#   define __SV_BACKGROUND_COLOR(__COLOR)                       .background_color = __COLOR,
#else
#   define __SV_BACKGROUND_COLOR(__COLOR)
#endif

#if VSF_TGUI_CFG_V_SUPPORT_TILE_TRANSPARENCY == ENABLED
#   define __SV_TILE_TRANS_RATE(__COLOR)                       .tile_trans_rate = __COLOR,
#else
#   define __SV_TILE_TRANS_RATE(__COLOR)
#endif

#if VSF_TGUI_CFG_V_SUPPORT_CORNER_TILE == ENABLED
#   define __SV_SHOW_CORNER_TILE(__VALUE)                       .show_corner_tile  = __VALUE,
#else
#   define __SV_SHOW_CORNER_TILE(__VALUE)
#endif

#if VSF_TGUI_CFG_V_LABLE_SUPPORT_TEXT_COLOR == ENABLED
#   define __SV_TEXT_COLOR(__COLOR)                             .text_color = __COLOR,
#else
#   define __SV_TEXT_COLOR(__COLOR)
#endif

#define VSF_TGUI_V_CONTROL_STATIC_INIT_DEFAULT                                  \
            __SV_SHOW_CORNER_TILE(false)                                        \
            __SV_TILE_TRANS_RATE(0xFF)                                          \
            __SV_BACKGROUND_COLOR(VSF_TGUI_CFG_V_CONTROL_BACKGROUND_COLOR)

#define VSF_TGUI_V_LABEL_STATIC_INIT_DEFAULT                                    \
            __SV_SHOW_CORNER_TILE(true)                                         \
            __SV_TILE_TRANS_RATE(0xFF)                                          \
            __SV_BACKGROUND_COLOR(VSF_TGUI_CFG_V_LABEL_BACKGROUND_COLOR)        \
            __SV_TEXT_COLOR(VSF_TGUI_CFG_V_LABEL_TEXT_COLOR)

#define VSF_TGUI_V_BUTTON_STATIC_INIT_DEFAULT                                   \
            __SV_SHOW_CORNER_TILE(true)                                         \
            __SV_TILE_TRANS_RATE(0xFF)                                          \
            __SV_BACKGROUND_COLOR(VSF_TGUI_CFG_V_BUTTON_BACKGROUND_COLOR)       \
            __SV_TEXT_COLOR(VSF_TGUI_CFG_V_BUTTON_TEXT_COLOR)

#define VSF_TGUI_V_CONTAINER_STATIC_INIT_DEFAULT                                \
            __SV_SHOW_CORNER_TILE(true)                                         \
            __SV_TILE_TRANS_RATE(0xFF)                                          \
            __SV_BACKGROUND_COLOR(VSF_TGUI_CFG_V_CONTAINER_BACKGROUND_COLOR)

#define VSF_TGUI_V_TEXT_LIST_STATIC_INIT_DEFAULT                                \
            __SV_SHOW_CORNER_TILE(true)                                         \
            __SV_TILE_TRANS_RATE(0xFF)                                          \
            __SV_BACKGROUND_COLOR(VSF_TGUI_CFG_V_TEXT_LIST_BACKGROUND_COLOR)

#define VSF_TGUI_V_LIST_STATIC_INIT_DEFAULT                                     \
            __SV_SHOW_CORNER_TILE(true)                                         \
            __SV_TILE_TRANS_RATE(0xFF)

#define VSF_TGUI_V_PANEL_STATIC_INIT_DEFAULT                                    \
            __SV_SHOW_CORNER_TILE(true)                                         \
            __SV_TILE_TRANS_RATE(0xFF)

#define VSF_TGUI_V_CONTROL_STATIC_INIT_OVERRIDE
#define VSF_TGUI_V_LABEL_STATIC_INIT_OVERRIDE
#define VSF_TGUI_V_BUTTON_STATIC_INIT_OVERRIDE
#define VSF_TGUI_V_CONTAINER_STATIC_INIT_OVERRIDE
#define VSF_TGUI_V_LIST_STATIC_INIT_OVERRIDE

#if VSF_TGUI_CFG_V_SUPPORT_FLUXIBLE_BACKGROUND_COLOR == ENABLED
#define VSF_TGUI_V_TEXT_LIST_STATIC_INIT_BACKGROUND_CLOLOR_OVERRIDE             \
            .tList.background_color = VSF_TGUI_COLOR_RGBA(0x00, 0x00, 0x00, 0x00),
#else
#define VSF_TGUI_V_TEXT_LIST_STATIC_INIT_BACKGROUND_CLOLOR_OVERRIDE
#endif

#if VSF_TGUI_CFG_V_SUPPORT_CORNER_TILE == ENABLED
#define VSF_TGUI_V_TEXT_LIST_STATIC_INIT_TILE_OVERRIDE                          \
            .show_corner_tile = true,
#else
#define VSF_TGUI_V_TEXT_LIST_STATIC_INIT_TILE_OVERRIDE
#endif

#define VSF_TGUI_V_TEXT_LIST_STATIC_INIT_OVERRIDE                               \
            VSF_TGUI_V_TEXT_LIST_STATIC_INIT_BACKGROUND_CLOLOR_OVERRIDE         

#if VSF_TGUI_CFG_V_SUPPORT_FLUXIBLE_BACKGROUND_COLOR == ENABLED
#   define VSF_TGUI_V_TEXT_LIST_CONTENT_INIT_OVERRIDE                           \
            .background_color = VSF_TGUI_COLOR_RGBA(0x00, 0x00, 0x00, 0x00),    \
            VSF_TGUI_V_TEXT_LIST_STATIC_INIT_TILE_OVERRIDE
#else
#   define VSF_TGUI_V_TEXT_LIST_CONTENT_INIT_OVERRIDE 
#endif   

#define VSF_TGUI_V_TEXT_LIST_CONTENT_INIT_DEFAULT

#define VSF_TGUI_V_PANEL_STATIC_INIT_OVERRIDE

#define tgui_v_font(...)                                            \
            __tgui_attribute(font_index, __VA_ARGS__)

#if VSF_TGUI_CFG_V_SUPPORT_CORNER_TILE == ENABLED
#define tgui_v_show_corner_tile(...)                                \
            __tgui_attribute(show_corner_tile,  __VA_ARGS__)
#endif

#if VSF_TGUI_CFG_V_SUPPORT_TILE_TRANSPARENCY == ENABLED
#define tgui_v_tile_trans_rate(...)                                 \
            __tgui_attribute(tile_trans_rate,  __VA_ARGS__)
#endif

#if VSF_TGUI_CFG_V_SUPPORT_FLUXIBLE_BACKGROUND_COLOR == ENABLED
#define tgui_v_background_color(...)                                \
            __tgui_attribute(background_color,  __VA_ARGS__)
#endif

#if VSF_TGUI_CFG_V_LABLE_SUPPORT_TEXT_COLOR == ENABLED
#define tgui_v_text_color(...)                                      \
            __tgui_attribute(text_color,  __VA_ARGS__)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_structure(vsf_tgui_v_control_t)
def_structure(vsf_tgui_v_control_t)
    uint8_t font_index : 6;
    uint8_t __skip_frame : 1;
#if VSF_TGUI_CFG_V_SUPPORT_CORNER_TILE == ENABLED
    uint8_t show_corner_tile : 1;
#endif
#if VSF_TGUI_CFG_V_SUPPORT_TILE_TRANSPARENCY == ENABLED
    uint8_t tile_trans_rate;
#endif
#if VSF_TGUI_CFG_V_SUPPORT_FLUXIBLE_BACKGROUND_COLOR == ENABLED
    vsf_tgui_v_color_t background_color;
#endif
end_def_structure(vsf_tgui_v_control_t)

declare_structure(vsf_tgui_v_container_t)
def_structure(vsf_tgui_v_container_t)
#if __IS_COMPILER_IAR__ || (!defined( __STDC_VERSION__ ) || __STDC_VERSION__ < 199901L)
    //please remove this if your structure is not emplty
    REG_RSVD_U8
#endif
end_def_structure(vsf_tgui_v_container_t)

declare_structure(vsf_tgui_v_label_t)
def_structure(vsf_tgui_v_label_t)
#if VSF_TGUI_CFG_V_LABLE_SUPPORT_TEXT_COLOR == ENABLED
    vsf_tgui_v_color_t text_color;
#endif
end_def_structure(vsf_tgui_v_label_t)

declare_structure(vsf_tgui_v_button_t)
def_structure(vsf_tgui_v_button_t)
#if __IS_COMPILER_IAR__ || (!defined( __STDC_VERSION__ ) || __STDC_VERSION__ < 199901L)
    //please remove this if your structure is not emplty
    REG_RSVD_U8
#endif
end_def_structure(vsf_tgui_v_button_t)

declare_structure(vsf_tgui_v_panel_t)
def_structure(vsf_tgui_v_panel_t)
#if __IS_COMPILER_IAR__ || (!defined( __STDC_VERSION__ ) || __STDC_VERSION__ < 199901L)
    //please remove this if your structure is not emplty
    REG_RSVD_U8
#endif
end_def_structure(vsf_tgui_v_panel_t)

declare_structure(vsf_tgui_v_list_t)
def_structure(vsf_tgui_v_list_t)
#if __IS_COMPILER_IAR__ || (!defined( __STDC_VERSION__ ) || __STDC_VERSION__ < 199901L)
    //please remove this if your structure is not emplty
    REG_RSVD_U8
#endif
end_def_structure(vsf_tgui_v_list_t)

declare_structure(vsf_tgui_v_text_list_t)
def_structure(vsf_tgui_v_text_list_t)
#if __IS_COMPILER_IAR__ || (!defined( __STDC_VERSION__ ) || __STDC_VERSION__ < 199901L)
    //please remove this if your structure is not emplty
    REG_RSVD_U8
#endif
end_def_structure(vsf_tgui_v_text_list_t)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
#endif
/* EOF */
