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

#ifndef __USRAPP_UI_COMMON_H__
#define __USRAPP_UI_COMMON_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"

#if     VSF_USE_UI == ENABLED                                                   \
    &&  (VSF_USE_DISP_SDL2 == ENABLED || VSF_USE_TINY_GUI == ENABLED || VSF_USE_UI_AWTK == ENABLED || VSF_USE_UI_LVGL == ENABLED)

#if VSF_USE_UI_LVGL == ENABLED
#   include "lvgl/lvgl.h"
#   include "lv_conf.h"
#   include "component/3rd-party/littlevgl/6.0/port/vsf_lvgl_port.h"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct usrapp_ui_common_t {
#if VSF_USE_DISP_SDL2 == ENABLED
    vk_disp_sdl2_t disp;
#endif

#if VSF_USE_TINY_GUI == ENABLED
    struct {
        vk_input_notifier_t notifier;
    } tgui;
#endif

#if VSF_USE_UI_AWTK == ENABLED
    struct {
        vk_input_notifier_t notifier;
    } awtk;
#endif

#if VSF_USE_UI_LVGL == ENABLED
    struct {
        vk_input_notifier_t notifier;
        vk_touchscreen_evt_t ts_evt;
        lv_disp_buf_t disp_buf;
        lv_color_t color[LV_VER_RES_MAX][LV_HOR_RES_MAX];
    } lvgl;
#endif
};
typedef struct usrapp_ui_common_t usrapp_ui_common_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern usrapp_ui_common_t usrapp_ui_common;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // VSF_USE_UI
#endif      // __USRAPP_UI_COMMON_H__
/* EOF */