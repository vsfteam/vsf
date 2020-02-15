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

#if VSF_USE_UI == ENABLED && VSF_USE_UI_LVGL == ENABLED && APP_CFG_USE_LVGL_DEMO == ENABLED

#include "../common/usrapp_common.h"

#include "lvgl/lvgl.h"
#include "lv_conf.h"
#include "component/3rd-party/littlevgl/6.0/port/vsf_lvgl_port.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __lvgl_on_touchscreen(vk_input_type_t type, vk_touchscreen_evt_t *ts_evt)
{
    if (0 == vsf_input_touchscreen_get_id(ts_evt)) {
        usrapp_ui_common.lvgl.ts_evt = *ts_evt;
    }
}

static bool __lvgl_touchscreen_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    data->state = vsf_input_touchscreen_is_down(&usrapp_ui_common.lvgl.ts_evt) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    data->point.x = vsf_input_touchscreen_get_x(&usrapp_ui_common.lvgl.ts_evt);
    data->point.y = vsf_input_touchscreen_get_y(&usrapp_ui_common.lvgl.ts_evt);

//    vsf_trace(VSF_TRACE_DEBUG, "touchscreen: %s x=%d, y=%d" VSF_TRACE_CFG_LINEEND,
//        data->state == LV_INDEV_STATE_PR ? "press" : "release",
//        data->point.x, data->point.y);
    return false;
}

#if APP_CFG_USE_LINUX_DEMO == ENABLED
int lvgl_main(int argc, char *argv[])
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

#   if USE_LV_LOG
    lv_log_register_print(vsf_lvgl_printf);
#   endif

    usrapp_ui_common.lvgl.notifier.mask = 1 << VSF_INPUT_TYPE_TOUCHSCREEN;
    usrapp_ui_common.lvgl.notifier.on_evt = (vk_input_on_evt_t)__lvgl_on_touchscreen;
    vk_input_notifier_register(&usrapp_ui_common.lvgl.notifier);

    lv_init();

    lv_disp_drv_t disp_drv;
    lv_indev_drv_t indev_drv;
    lv_disp_t *disp;

    lv_disp_buf_init(   &usrapp_ui_common.lvgl.disp_buf,
                        &usrapp_ui_common.lvgl.color,
                        NULL,
                        LV_HOR_RES_MAX * LV_VER_RES_MAX);
    lv_disp_drv_init(&disp_drv);

    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    disp_drv.flush_cb = vsf_lvgl_disp_flush;
    disp_drv.buffer = &usrapp_ui_common.lvgl.disp_buf;
    disp = lv_disp_drv_register(&disp_drv);

    // insecure operation
    ((vk_disp_param_t *)&usrapp_ui_common.disp.param)->color = VSF_DISP_COLOR_RGB565;
    vsf_lvgl_disp_bind(&usrapp_ui_common.disp.use_as__vk_disp_t, &disp->driver);

    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.disp = disp;
    indev_drv.read_cb = __lvgl_touchscreen_read;
    lv_indev_drv_register(&indev_drv);

    extern void lvgl_application(void);
    lvgl_application();

    while (1) {
        lv_task_handler();
    }
    return 0;
}

#endif
