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


//! \note Top Level Application Configuration

#ifndef __TOP_TGUI_USER_CFG_H__
#define __TOP_TGUI_USER_CFG_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define VSF_TGUI_CFG_USE_MINIMAL_CFG        DISABLED

#if VSF_TGUI_CFG_USE_MINIMAL_CFG == ENABLED

#define VSF_USE_UI                                          ENABLED
#   define VSF_USE_LVGL                                     DISABLED
#   define VSF_DISP_USE_SDL2                                ENABLED
#       define VSF_DISP_SDL2_CFG_MOUSE_AS_TOUCHSCREEN       ENABLED

#   define VSF_USE_TINY_GUI                                 ENABLED
#       define VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL          VSF_TGUI_V_TEMPLATE_SIMPLE_VIEW
#       define VSF_TGUI_CFG_COLOR_MODE                      VSF_TGUI_COLOR_ARGB_8888
#       define VSF_TGUI_CFG_SUPPORT_NAME_STRING             ENABLED        /* Enabled for debug */
#       define VSF_TGUI_CFG_REFRESH_SCHEME                  VSF_TGUI_REFRESH_SCHEME_Z_ORDER

#       define VSF_TGUI_CFG_SUPPORT_MOUSE                   DISABLED
#       define VSF_TGUI_CFG_SUPPORT_DESTRUCTOR_SCHEME       DISABLED
#       define VSF_TGUI_CFG_SUPPORT_TIMER                   DISABLED
#       define VSF_TGUI_CFG_SUPPORT_SLIDER                  DISABLED

#       define VSF_TGUI_CFG_SUPPORT_TEXT_LIST               DISABLED
#           define VSF_TGUI_CFG_TEXT_LIST_SUPPORT_SLIDE     DISABLED

#       define VSF_TGUI_CFG_SUPPORT_LIST                    DISABLED
#           define VSF_TGUI_CFG_LIST_SUPPORT_SCROOLBAR      DISABLED
#           define VSF_TGUI_CFG_LIST_SUPPORT_SLIDE          DISABLED

#       define VSF_TGUI_CFG_SUPPORT_STREAM_CONTAINER        DISABLED
#       define VSF_TGUI_CFG_SUPPORT_LINE_STREAM_CONTAINER   DISABLED
#       define VSF_TGUI_CFG_TEXT_SIZE_INFO_CACHING          DISABLED


#       define VSF_TGUI_CFG_USER_FONTS                      ENABLED

#       define VSF_TGUI_CFG_SV_BUTTON_ADDITIONAL_TILES      DISABLED
#       define VSF_TGUI_CFG_SV_BUTTON_BACKGROUND_COLOR      VSF_TGUI_COLOR_DEF(0xB4, 0xC7, 0xE7)
#       define VSF_TGUI_CFG_SV_CONTAINER_ADDITIONAL_TILES   DISABLED
#       define VSF_TGUI_CFG_SV_CONTAINER_BACKGROUND_COLOR   VSF_TGUI_COLOR_DEF(0x44, 0x72, 0xC4)
#       define VSF_TGUI_CFG_SV_LABEL_ADDITIONAL_TILES       DISABLED
#       define VSF_TGUI_CFG_SV_LABEL_BACKGROUND_COLOR       VSF_TGUI_COLOR_DEF(0xB4, 0xC7, 0xE7)
#       define VSF_TGUI_CFG_SV_TEXT_LIST_BACKGROUND_COLOR   VSF_TGUI_COLOR_DEF(0xB4, 0xC7, 0xE7)
#       define VSF_TGUI_CFG_SV_TEXT_LIST_INDICATOR_COLOR    VSF_TGUI_COLOR_DEF(0x44, 0x72, 0xC4)

#       define VSF_TGUI_LOG                                 vsf_trace
#           define VSF_TGUI_CFG_SHOW_REFRESH_EVT_LOG        DISABLED
#           define VSF_TGUI_CFG_SHOW_USER_INPUT_LOG         ENABLED
#               define  VSF_TGUI_CFG_SHOW_POINTER_EVT_LOG   ENABLED
#               define  VSF_TGUI_CFG_SHOW_KEY_EVT_LOG       DISABLED
#               define  VSF_TGUI_CFG_SHOW_GESTURE_EVT_LOG   DISABLED

#           define VSF_TGUI_CFG_SV_PORT_LOG                 DISABLED
#           define VSF_TGUI_CFG_SV_DRAW_LOG                 DISABLED
#           define VSF_TGUI_CFG_SV_RENDERING_LOG            DISABLED
#           define VSF_TGUI_CFG_SV_REFRESH_RATE             DISABLED


#   define VSF_USE_MSG_TREE                                 ENABLED
#       define VSF_MSG_TREE_CFG_SUPPORT_NAME_STRING         ENABLED        /* Enabled for debug */
#       define VSF_MSGT_NODE_OFFSET_TYPE                    int16_t

/*----------------------------------------------------------------------------*
 * Regarget Weak interface                                                    *
 *----------------------------------------------------------------------------*/
#define WEAK_VSF_INPUT_ON_TOUCHSCREEN
#define WEAK_VSF_INPUT_ON_KEYBOARD
#define WEAK_VSF_TGUI_LOW_LEVEL_ON_READY_TO_REFRESH
#define WEAK_VSF_TGUI_IDX_ROOT_TILE_GET_SIZE
//#define WEAK_VSF_INPUT_ON_MOUSE

#endif

#if VSF_TGUI_CFG_USE_MINIMAL_CFG != ENABLED

#define VSF_USE_UI                                          ENABLED
#   define VSF_USE_LVGL                                     DISABLED
#   define VSF_DISP_USE_SDL2                                ENABLED


#   define VSF_USE_TINY_GUI                                 ENABLED
#       define VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL          VSF_TGUI_V_TEMPLATE_SIMPLE_VIEW
#       define VSF_TGUI_CFG_COLOR_MODE                      VSF_TGUI_COLOR_ARGB_8888
#       define VSF_TGUI_CFG_SUPPORT_NAME_STRING             ENABLED        /* Enabled for debug */
#       define VSF_TGUI_CFG_REFRESH_SCHEME                  VSF_TGUI_REFRESH_SCHEME_Z_ORDER
#       define VSF_TGUI_CFG_SUPPORT_MOUSE                   ENABLED


#       define VSF_TGUI_CFG_USER_FONTS                      ENABLED

#       define VSF_TGUI_CFG_SV_BUTTON_ADDITIONAL_TILES      ENABLED
#       define VSF_TGUI_CFG_SV_BUTTON_BACKGROUND_COLOR      VSF_TGUI_COLOR_DEF(0xB4, 0xC7, 0xE7)
#       define VSF_TGUI_CFG_SV_CONTAINER_ADDITIONAL_TILES   ENABLED
#       define VSF_TGUI_CFG_SV_CONTAINER_BACKGROUND_COLOR   VSF_TGUI_COLOR_DEF(0x44, 0x72, 0xC4)
#       define VSF_TGUI_CFG_SV_LABEL_ADDITIONAL_TILES       ENABLED
#       define VSF_TGUI_CFG_SV_LABEL_BACKGROUND_COLOR       VSF_TGUI_COLOR_DEF(0xB4, 0xC7, 0xE7)
#       define VSF_TGUI_CFG_SV_TEXT_LIST_BACKGROUND_COLOR   VSF_TGUI_COLOR_DEF(0xB4, 0xC7, 0xE7)
#       define VSF_TGUI_CFG_SV_TEXT_LIST_INDICATOR_COLOR    VSF_TGUI_COLOR_DEF(0x44, 0x72, 0xC4)

#       define VSF_TGUI_LOG                                 vsf_trace
#           define VSF_TGUI_CFG_SHOW_REFRESH_EVT_LOG        DISABLED

#           define VSF_TGUI_CFG_SV_PORT_LOG                 DISABLED
#           define VSF_TGUI_CFG_SV_DRAW_LOG                 DISABLED
#           define VSF_TGUI_CFG_SV_RENDERING_LOG            DISABLED
#           define VSF_TGUI_CFG_SV_REFRESH_RATE             DISABLED


#   define VSF_USE_MSG_TREE                                 ENABLED
#       define VSF_MSG_TREE_CFG_SUPPORT_NAME_STRING         ENABLED        /* Enabled for debug */
#       define VSF_MSGT_NODE_OFFSET_TYPE                    int16_t

/*----------------------------------------------------------------------------*
 * Regarget Weak interface                                                    *
 *----------------------------------------------------------------------------*/
#define WEAK_VSF_INPUT_ON_TOUCHSCREEN
#define WEAK_VSF_INPUT_ON_KEYBOARD
#define WEAK_VSF_TGUI_LOW_LEVEL_ON_READY_TO_REFRESH
#define WEAK_VSF_TGUI_IDX_ROOT_TILE_GET_SIZE

#if VSF_TGUI_CFG_SUPPORT_MOUSE == ENABLED
#   define WEAK_VSF_INPUT_ON_MOUSE
#   define VSF_DISP_SDL2_CFG_MOUSE_AS_TOUCHSCREEN       DISABLED
#else
#   define VSF_DISP_SDL2_CFG_MOUSE_AS_TOUCHSCREEN       ENABLED
#endif

#endif

/*============================ TYPES =========================================*/

/*! \note Users should assign values to those enum symbols according to the
 *!       target platform.
 *!
 *! \note Platform: PC
 */
enum {

    VSF_TGUI_KEY_BACKSPACE      = 0x08,
    VSF_TGUI_KEY_OK             = 0x0D,
    VSF_TGUI_KEY_CANCEL         = 0x1B,

    VSF_TGUI_KEY_INSERT         = 0x49,
    VSF_TGUI_KEY_HOME           = 0x4A,
    VSF_TGUI_KEY_PAGE_UP        = 0x4B,
    VSF_TGUI_KEY_END            = 0x4D,
    VSF_TGUI_KEY_PAGE_DOWN      = 0x4E,
    VSF_TGUI_KEY_RIGHT          = 0x4F,
    VSF_TGUI_KEY_LEFT           = 0x50,
    VSF_TGUI_KEY_DOWN           = 0x51,
    VSF_TGUI_KEY_UP             = 0x52,


    VSF_TGUI_KEY_MUTE           = 0x106,
    VSF_TGUI_KEY_VOLUME_DOWN    = 0x81,
    VSF_TGUI_KEY_VOLUME_UP      = 0x80,


    VSF_TGUI_KEY_SYSTEM_CALL    = 0xE3,
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */