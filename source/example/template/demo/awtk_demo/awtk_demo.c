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

#include "vsf.h"

#if VSF_USE_UI == ENABLED && VSF_USE_AWTK == ENABLED && APP_USE_AWTK_DEMO == ENABLED

#include "../common/usrapp_common.h"

#include "awtk.h"
#include "awtk_vsf_port.h"
#include "component/3rd-party/awtk/port/platforms/vsf/main_loop_vsf.h"
#include "component/3rd-party/awtk/port/vsf_awtk_port.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

lcd_t *platform_create_lcd(wh_t w, wh_t h)
{
    return vsf_awtk_create_lcd_mem_fragment(&usrapp_ui_common.disp.use_as__vk_disp_t, w, h);
}

static void __vsf_awtk_on_evt(vk_input_type_t tpye, vk_touchscreen_evt_t *ts_evt)
{
    if (0 == vsf_input_touchscreen_get_id(ts_evt)) {
        main_loop_post_pointer_event(main_loop(),
                vsf_input_touchscreen_is_down(ts_evt),
                vsf_input_touchscreen_get_x(ts_evt),
                vsf_input_touchscreen_get_y(ts_evt));
    }
}

#if APP_USE_LINUX_DEMO == ENABLED
int awtk_main(int argc, char *argv[])
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
    extern void assets_init(void);
    extern void application_init(void);

    usrapp_ui_common.awtk.notifier.mask = 1 << VSF_INPUT_TYPE_TOUCHSCREEN,
    usrapp_ui_common.awtk.notifier.on_evt = (vk_input_on_evt_t)__vsf_awtk_on_evt,
    vk_input_notifier_register(&usrapp_ui_common.awtk.notifier);

    vsf_awtk_init(usrapp_ui_common.disp.param.width, usrapp_ui_common.disp.param.height, APP_MOBILE, NULL, NULL);

    assets_init();
    application_init();

    tk_run();
    return 0;
}

#endif
