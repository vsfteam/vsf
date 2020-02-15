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


#define VSF_USE_UI                                          ENABLED
#   define VSF_USE_UI_LVGL                                  DISABLED
#   define VSF_USE_DISP_DRV_SDL2                            ENABLED
#       define VSF_DISP_DRV_SDL2_CFG_MOUSE_AS_TOUCHSCREEN   DISABLED

#   define VSF_USE_TINY_GUI                                 ENABLED
#       define VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL          VSF_TGUI_V_TEMPLATE_SIMPLE_VIEW
#       define VSF_TGUI_CFG_COLOR_MODE                      VSF_TGUI_COLOR_ARGB_8888
#       define VSF_TGUI_CFG_SUPPORT_NAME_STRING             ENABLED         /* Enabled for debug */

#       define VSF_TGUI_CFG_USER_FONTS                      ENABLED

#       define VSF_TGUI_CFG_SV_BUTTON_ADDITIONAL_TILES      ENABLED
#       define VSF_TGUI_CFG_SV_BUTTON_BACKGROUND_COLOR      VSF_TGUI_COLOR_DEF(0xB4, 0xC7, 0xE7)
#       define VSF_TGUI_SV_CFG_PANEL_ADDITIONAL_TILES       ENABLED
#       define VSF_TGUI_SV_CFG_PANEL_BACKGROUND_COLOR       VSF_TGUI_COLOR_DEF(0x44, 0x72, 0xC4)
#       define VSF_TGUI_CFG_SV_LABEL_ADDITIONAL_TILES       ENABLED
#       define VSF_TGUI_CFG_SV_LABEL_BACKGROUND_COLOR       VSF_TGUI_COLOR_DEF(0xB4, 0xC7, 0xE7)
#       define VSF_TGUI_CFG_SV_TEXT_LIST_BACKGROUND_COLOR   VSF_TGUI_COLOR_DEF(0xB4, 0xC7, 0xE7)
#       define VSF_TGUI_CFG_SV_TEXT_LIST_INDICATOR_COLOR    VSF_TGUI_COLOR_DEF(0x44, 0x72, 0xC4)

#       define VSF_TGUI_LOG                                 vsf_trace
#           define VSF_TGUI_SV_CFG_PORT_LOG                 DISABLED
#           define VSF_TGUI_SV_CFG_DRAW_LOG                 DISABLED

#   define VSF_USE_MSG_TREE                                 ENABLED
#       define VSF_MSG_TREE_CFG_SUPPORT_NAME_STRING         ENABLED         /* Enabled for debug */
#       define VSF_MSGT_NODE_OFFSET_TYPE                    int16_t

/*----------------------------------------------------------------------------*
 * Regarget Weak interface                                                    *
 *----------------------------------------------------------------------------*/
#define WEAK_VSF_TGUI_IDX_ROOT_TILE_GET_SIZE_EXTERN                             \
        extern vsf_tgui_size_t vsf_tgui_sdl_idx_root_tile_get_size(const vsf_tgui_tile_t* ptTile);
#define WEAK_VSF_TGUI_IDX_ROOT_TILE_GET_SIZE(__PTTILE)                          \
        vsf_tgui_sdl_idx_root_tile_get_size(__PTTILE)

#if VSF_USE_UI == ENABLED && VSF_USE_TINY_GUI == ENABLED
#   define WEAK_VSF_INPUT_ON_TOUCHSCREEN_EXTERN                                 \
        extern void vsf_input_on_touchscreen(vk_touchscreen_evt_t *ts_evt);
#   define WEAK_VSF_INPUT_ON_TOUCHSCREEN(__TS_EVT)                              \
        vsf_input_on_touchscreen((__TS_EVT))

#   define WEAK_VSF_INPUT_ON_KEYBOARD_EXTERN                                    \
        extern void vsf_input_on_keyboard(vk_keyboard_evt_t *evt);
#   define WEAK_VSF_INPUT_ON_KEYBOARD(__EVT)                                    \
        vsf_input_on_keyboard((__EVT))
#endif

#define WEAK_VSF_TGUI_LOW_LEVEL_ON_READY_TO_REFRESH_EXTERN                      \
        extern void vsf_tgui_low_level_on_ready_to_refresh(void);

#define WEAK_VSF_TGUI_LOW_LEVEL_ON_READY_TO_REFRESH()                           \
        vsf_tgui_low_level_on_ready_to_refresh()

#define WEAK_VSF_INPUT_ON_MOUSE

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