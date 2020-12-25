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

#if VSF_USE_UI == ENABLED && VSF_USE_TINY_GUI == ENABLED && APP_USE_TGUI_DEMO == ENABLED

#include "../common/usrapp_common.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
extern void vsf_tgui_bind_disp(vk_disp_t* disp, void* bitmap_data, size_t bitmap_size);
extern void vsf_tgui_on_touchscreen_evt(vk_touchscreen_evt_t* ts_evt);
extern void vsf_tgui_on_keyboard_evt(vk_keyboard_evt_t* keyboard_evt);
extern vsf_err_t tgui_demo_init(void);

/*============================ IMPLEMENTATION ================================*/

static void __tgui_on_input_evt(vk_input_type_t type, vk_input_evt_t *evt)
{
//! this block of code is used for test purpose only
    if (VSF_INPUT_TYPE_KEYBOARD == type) {
        vsf_tgui_on_keyboard_evt((vk_keyboard_evt_t *)evt);
    } else if (VSF_INPUT_TYPE_TOUCHSCREEN == type) {
        vsf_tgui_on_touchscreen_evt((vk_touchscreen_evt_t *)evt);
    }
}

#if VSF_TGUI_CFG_SUPPORT_MOUSE == ENABLED
extern void vsf_tgui_on_mouse_evt(vk_mouse_evt_t *mouse_evt);
void vsf_input_on_mouse(vk_mouse_evt_t *mouse_evt)
{
    vsf_tgui_on_mouse_evt(mouse_evt);
}
#endif

#if APP_USE_LINUX_DEMO == ENABLED
int tgui_main(int argc, char *argv[])
{
#else
int VSF_USER_ENTRY(void)
{
#   if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#       if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#       endif
#   endif
#endif

    usrapp_ui_common.tgui.notifier.mask = (1 << VSF_INPUT_TYPE_TOUCHSCREEN) | (1 << VSF_INPUT_TYPE_KEYBOARD);
    usrapp_ui_common.tgui.notifier.on_evt = __tgui_on_input_evt;
    vk_input_notifier_register(&usrapp_ui_common.tgui.notifier);

    // insecure operation
    ((vk_disp_param_t *)&usrapp_ui_common.disp.param)->color = VSF_DISP_COLOR_ARGB8888;
	vsf_tgui_bind_disp(&(usrapp_ui_common.disp.use_as__vk_disp_t), &usrapp_ui_common.tgui.color, dimof(usrapp_ui_common.tgui.color));

    tgui_demo_init();

    return 0;
}

#endif
/* EOF */
