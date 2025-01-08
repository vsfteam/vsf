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
    &&  VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL == VSF_TGUI_V_TEMPLATE_SCGUI_VIEW

#include "./vsf_tgui_scguiv_port.h"

/*============================ MACROS ========================================*/

// colors

#define VSF_TGUI_COLOR_WHITE                                    C_WHITE
#define VSF_TGUI_COLOR_SILVER                                   C_SILVER
#define VSF_TGUI_COLOR_GRAY                                     C_GRAY
#define VSF_TGUI_COLOR_BLACK                                    C_BLACK
#define VSF_TGUI_COLOR_RED                                      C_RED
#define VSF_TGUI_COLOR_MAROON                                   C_MAROON
#define VSF_TGUI_COLOR_YELLOW                                   C_YELLOW
#define VSF_TGUI_COLOR_LIME                                     C_LIME
#define VSF_TGUI_COLOR_GREEN                                    C_GREEN
#define VSF_TGUI_COLOR_AQUA                                     C_AQUA
#define VSF_TGUI_COLOR_TEAL                                     C_TEAL
#define VSF_TGUI_COLOR_BLUE                                     C_BLUE
#define VSF_TGUI_COLOR_NAVY                                     C_NAVY
#define VSF_TGUI_COLOR_PURPLE                                   C_PURPLE

#define VSF_TGUI_COLOR_OLIVE                                    C_OLIVER
#define VSF_TGUI_COLOR_FUCHSIA                                  C_FUCHSIA

#define VSF_TGUI_COLOR_RGBA(__R, __G, __B, __A)                 ((vsf_tgui_v_color_t) ((((__R) & 0xF1) << 8) | (((__G) & 0xF3) << 3) | (((__B) & 0xF1) >> 3)) )
#define VSF_TGUI_COLOR_RGB(__R, __G, __B)                       VSF_TGUI_COLOR_RGBA((__R), (__G), (__B), 0xFF)

// configurations

// control configurations

#ifndef VSF_TGUI_CFG_V_SUPPORT_FLUXIBLE_BACKGROUND_COLOR
#   define VSF_TGUI_CFG_V_SUPPORT_FLUXIBLE_BACKGROUND_COLOR     ENABLED
#endif

#ifndef VSF_TGUI_CFG_V_SUPPORT_BORDER
#   define VSF_TGUI_CFG_V_SUPPORT_BORDER                        ENABLED
#endif
#if VSF_TGUI_CFG_V_SUPPORT_BORDER == ENABLED
#   ifndef VSF_TGUI_CFG_V_SUPPORT_ROUND_BORDER
#       define VSF_TGUI_CFG_V_SUPPORT_ROUND_BORDER              ENABLED
#   endif
#endif
#ifndef VSF_TGUI_CFG_V_BORDER_COLOR
#   define VSF_TGUI_CFG_V_BORDER_COLOR                          VSF_TGUI_COLOR_WHITE
#endif

// container configurations

#ifndef VSF_TGUI_CFG_V_CONTAINER_BACKGROUND_COLOR
#   define VSF_TGUI_CFG_V_CONTAINER_BACKGROUND_COLOR            VSF_TGUI_COLOR_RGB(0x44, 0x72, 0xC4)
#endif

// text list configurations

#ifndef VSF_TGUI_CFG_V_TEXT_LIST_BACKGROUND_COLOR
#   define VSF_TGUI_CFG_V_TEXT_LIST_BACKGROUND_COLOR            VSF_TGUI_COLOR_RGB(0xB4, 0xC7, 0xE7)
#endif

// label configurations

#ifndef VSF_TGUI_CFG_V_LABEL_BACKGROUND_COLOR
#   define VSF_TGUI_CFG_V_LABEL_BACKGROUND_COLOR                VSF_TGUI_COLOR_RGB(0xB4, 0xC7, 0xE7)
#endif

#ifndef VSF_TGUI_CFG_V_LABLE_SUPPORT_TEXT_COLOR
#   define VSF_TGUI_CFG_V_LABLE_SUPPORT_TEXT_COLOR              ENABLED
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

// button configurations

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

// default init and override init

#define VSF_TGUI_V_CONTROL_STATIC_INIT_DEFAULT                                  \
            tgui_v_border_color(VSF_TGUI_CFG_V_BORDER_COLOR),
#define VSF_TGUI_V_LABEL_STATIC_INIT_DEFAULT                                    \
            tgui_v_border_radius(16),                                           \
            tgui_v_background_color(VSF_TGUI_CFG_V_LABEL_BACKGROUND_COLOR),     \
            tgui_v_text_color(VSF_TGUI_CFG_V_LABEL_TEXT_COLOR),
#define VSF_TGUI_V_BUTTON_STATIC_INIT_DEFAULT                                   \
            tgui_v_border_radius(16),                                           \
            tgui_v_background_color(VSF_TGUI_CFG_V_BUTTON_BACKGROUND_COLOR),    \
            tgui_v_text_color(VSF_TGUI_CFG_V_BUTTON_TEXT_COLOR),
#define VSF_TGUI_V_CONTAINER_STATIC_INIT_DEFAULT                                \
            tgui_v_background_color(VSF_TGUI_CFG_V_CONTAINER_BACKGROUND_COLOR),
#define VSF_TGUI_V_TEXT_LIST_STATIC_INIT_DEFAULT                                \
            tgui_v_background_color(VSF_TGUI_CFG_V_CONTAINER_BACKGROUND_COLOR),
#define VSF_TGUI_V_TEXT_LIST_CONTENT_INIT_DEFAULT
#define VSF_TGUI_V_LIST_STATIC_INIT_DEFAULT
#define VSF_TGUI_V_PANEL_STATIC_INIT_DEFAULT                                    \
            tgui_v_border_radius(16),

#define VSF_TGUI_V_PANEL_STATIC_INIT_OVERRIDE
#define VSF_TGUI_V_CONTROL_STATIC_INIT_OVERRIDE
#define VSF_TGUI_V_LABEL_STATIC_INIT_OVERRIDE
#define VSF_TGUI_V_BUTTON_STATIC_INIT_OVERRIDE
#define VSF_TGUI_V_CONTAINER_STATIC_INIT_OVERRIDE
#define VSF_TGUI_V_LIST_STATIC_INIT_OVERRIDE
#define VSF_TGUI_V_TEXT_LIST_CONTENT_INIT_OVERRIDE
#define VSF_TGUI_V_TEXT_LIST_STATIC_INIT_OVERRIDE

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_TGUI_CFG_V_SUPPORT_FLUXIBLE_BACKGROUND_COLOR == ENABLED
#   define tgui_v_background_color(...)                                         \
            __tgui_attribute(background_color,  __VA_ARGS__)
#else
#   define tgui_v_background_color(...)
#endif

#if VSF_TGUI_CFG_V_SUPPORT_BORDER == ENABLED
#   define tgui_v_border_width(...)                                             \
            __tgui_attribute(border_width,  __VA_ARGS__)
#else
#   define tgui_v_border_width(...)
#endif

#if VSF_TGUI_CFG_V_SUPPORT_BORDER == ENABLED
#   define tgui_v_border_color(...)                                            \
            __tgui_attribute(border_color,  __VA_ARGS__)
#else
#   define tgui_v_border_color(...)
#endif

#if VSF_TGUI_CFG_V_SUPPORT_ROUND_BORDER == ENABLED
#   define tgui_v_border_radius(...)                                            \
            __tgui_attribute(border_radius,  __VA_ARGS__)
#else
#   define tgui_v_border_radius(...)
#endif

#if VSF_TGUI_CFG_V_LABLE_SUPPORT_TEXT_COLOR == ENABLED
#   define tgui_v_text_color(...)                                               \
            __tgui_attribute(text_color,  __VA_ARGS__)
#else
#   define tgui_v_text_color(...)
#endif

/*============================ TYPES =========================================*/

def_structure(vsf_tgui_v_control_t)
#if VSF_TGUI_CFG_V_SUPPORT_FLUXIBLE_BACKGROUND_COLOR == ENABLED
    vsf_tgui_v_color_t background_color;
#endif
#if VSF_TGUI_CFG_V_SUPPORT_BORDER == ENABLED
    vsf_tgui_v_color_t border_color;
    uint8_t border_width;
#   if VSF_TGUI_CFG_V_SUPPORT_ROUND_BORDER == ENABLED
    uint8_t border_radius;
#   endif
#endif
end_def_structure(vsf_tgui_v_control_t)

def_structure(vsf_tgui_v_container_t)
#if __IS_COMPILER_IAR__ || (!defined( __STDC_VERSION__ ) || __STDC_VERSION__ < 199901L)
    //please remove this if your structure is not emplty
    uint8_t             : 8;
#endif
end_def_structure(vsf_tgui_v_container_t)

def_structure(vsf_tgui_v_label_t)
#if VSF_TGUI_CFG_V_LABLE_SUPPORT_TEXT_COLOR == ENABLED
    vsf_tgui_v_color_t text_color;
#endif
    uint8_t font_index;
end_def_structure(vsf_tgui_v_label_t)

def_structure(vsf_tgui_v_button_t)
#if __IS_COMPILER_IAR__ || (!defined( __STDC_VERSION__ ) || __STDC_VERSION__ < 199901L)
    //please remove this if your structure is not emplty
    uint8_t             : 8;
#endif
end_def_structure(vsf_tgui_v_button_t)

def_structure(vsf_tgui_v_panel_t)
#if __IS_COMPILER_IAR__ || (!defined( __STDC_VERSION__ ) || __STDC_VERSION__ < 199901L)
    //please remove this if your structure is not emplty
    uint8_t             : 8;
#endif
end_def_structure(vsf_tgui_v_panel_t)

def_structure(vsf_tgui_v_list_t)
#if __IS_COMPILER_IAR__ || (!defined( __STDC_VERSION__ ) || __STDC_VERSION__ < 199901L)
    //please remove this if your structure is not emplty
    uint8_t             : 8;
#endif
end_def_structure(vsf_tgui_v_list_t)

def_structure(vsf_tgui_v_text_list_t)
#if __IS_COMPILER_IAR__ || (!defined( __STDC_VERSION__ ) || __STDC_VERSION__ < 199901L)
    //please remove this if your structure is not emplty
    uint8_t             : 8;
#endif
end_def_structure(vsf_tgui_v_text_list_t)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif

#endif
/* EOF */
