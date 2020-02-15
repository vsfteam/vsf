/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless requir by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "vsf.h"

#if VSF_USE_UI == ENABLED && VSF_USE_TINY_GUI == ENABLED && APP_CFG_USE_TGUI_DEMO == ENABLED

#include "../common/usrapp_common.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct usrapp_t {
    struct {
        vsf_tgui_color_t color[VSF_TGUI_VER_MAX][VSF_TGUI_HOR_MAX];
    } ui;
};
typedef struct usrapp_t usrapp_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static usrapp_t __usrapp;

/*============================ PROTOTYPES ====================================*/

#if VSF_USE_UI == ENABLED
extern void vsf_tgui_on_touchscreen_evt(vk_touchscreen_evt_t* ts_evt);
extern void vsf_tgui_on_keyboard_evt(vk_keyboard_evt_t* keyboard_evt);
#endif

/*============================ IMPLEMENTATION ================================*/

#if VSF_USE_UI == ENABLED
static void __tgui_on_input_evt(vk_input_type_t type, vk_input_evt_t *evt)
{
//! this block of code is used for test purpose only
    if (VSF_INPUT_TYPE_KEYBOARD == type) {
        vsf_tgui_on_keyboard_evt((vk_keyboard_evt_t *)evt);
    } else if (VSF_INPUT_TYPE_TOUCHSCREEN == type) {
        vsf_tgui_on_touchscreen_evt((vk_touchscreen_evt_t *)evt);
    }
}
#endif

#if APP_CFG_USE_LINUX_DEMO == ENABLED
int tgui_main(int argc, char *argv[])
{
#else
int main(void)
{
#   if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#       if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#       endif
#   endif
#endif

#if VSF_USE_UI == ENABLED && VSF_USE_TINY_GUI == ENABLED

    usrapp_ui_common.tgui.notifier.mask = (1 << VSF_INPUT_TYPE_TOUCHSCREEN) | (1 << VSF_INPUT_TYPE_KEYBOARD);
    usrapp_ui_common.tgui.notifier.on_evt = __tgui_on_input_evt;
    vk_input_notifier_register(&usrapp_ui_common.tgui.notifier);

    // insecure operation
    ((vk_disp_param_t *)&usrapp_ui_common.disp.param)->color = VSF_DISP_COLOR_ARGB8888;
	extern void vsf_tgui_bind(vk_disp_t * disp, void* ui_data);
	vsf_tgui_bind(&usrapp_ui_common.disp.use_as__vk_disp_t, &__usrapp.ui.color);

    extern vsf_err_t tgui_demo_init(void);
    tgui_demo_init();
#endif
    return 0;
}

#endif
/* EOF */
