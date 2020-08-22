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
#include "component/3rd-party/littlevgl/6.1.2/port/vsf_lvgl_port.h"

#if APP_LVGL_DEMO_CFG_FREETYPE == ENABLED
#include "component/3rd-party/littlevgl/6.1.2/extension/lv_lib_freetype/raw/lv_freetype.h"
#endif

#if APP_CFG_USE_LINUX_DEMO == ENABLED
#   include <pthread.h>
#endif
#include <stdio.h>

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
        if (usrapp_ui_common.lvgl.eda_poll != NULL) {
            vsf_eda_post_evt(usrapp_ui_common.lvgl.eda_poll, VSF_EVT_USER);
        }
    }
}

static bool __lvgl_touchscreen_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    vk_touchscreen_evt_t *ts_evt = &usrapp_ui_common.lvgl.ts_evt;
    data->state = vsf_input_touchscreen_is_down(ts_evt) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;

#if APP_LVGL_DEMO_CFG_TOUCH_REMAP == ENABLED
    data->point.x = vsf_input_touchscreen_get_x(ts_evt) * LV_HOR_RES_MAX / ts_evt->info.width;
    data->point.y = vsf_input_touchscreen_get_y(ts_evt) * LV_VER_RES_MAX / ts_evt->info.height;
#else
    data->point.x = vsf_input_touchscreen_get_x(ts_evt);
    data->point.y = vsf_input_touchscreen_get_y(ts_evt);
#endif

//    vsf_trace(VSF_TRACE_DEBUG, "touchscreen: %s x=%d, y=%d" VSF_TRACE_CFG_LINEEND,
//        data->state == LV_INDEV_STATE_PR ? "press" : "release",
//        data->point.x, data->point.y);
    return false;
}

#if APP_CFG_USE_LINUX_DEMO == ENABLED

void * __lvgl_thread(void *arg)
{
    usrapp_ui_common.lvgl.eda_poll = vsf_eda_get_cur();
    usrapp_ui_common.lvgl.notifier.mask = 1 << VSF_INPUT_TYPE_TOUCHSCREEN;
    usrapp_ui_common.lvgl.notifier.on_evt = (vk_input_on_evt_t)__lvgl_on_touchscreen;
    vk_input_notifier_register(&usrapp_ui_common.lvgl.notifier);

    while (1) {
        lv_task_handler();
        vsf_thread_wfe(VSF_EVT_USER);
    }
}

int lvgl_main(int argc, char *argv[])
{
    uint_fast8_t gamepad_num = 1;
    if (argc > 2) {
        printf("format: %s [GAMEPAD_NUM<1..4>]", argv[1]);
        return -1;
    } else if (argc > 1) {
        gamepad_num = strtoul(argv[1], NULL, 0);
    }

    if ((gamepad_num <= 0) || (gamepad_num > 4)) {
        printf("invalid gamepad_num %d", gamepad_num);
        return -1;
    }
#else
int main(void)
{
    uint_fast8_t gamepad_num = 1;
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

#if APP_LVGL_DEMO_CFG_FREETYPE == ENABLED
    lv_freetype_init(APP_LVGL_DEMO_CFG_FREETYPE_MAX_FACES);
#endif

    extern void lvgl_application(uint_fast8_t);
    lvgl_application(gamepad_num);

#if APP_CFG_USE_LINUX_DEMO == ENABLED
    pthread_t thread;
    pthread_create(&thread, NULL, __lvgl_thread, NULL);
#else
    usrapp_ui_common.lvgl.notifier.mask = 1 << VSF_INPUT_TYPE_TOUCHSCREEN;
    usrapp_ui_common.lvgl.notifier.on_evt = (vk_input_on_evt_t)__lvgl_on_touchscreen;
    vk_input_notifier_register(&usrapp_ui_common.lvgl.notifier);

    while (1) {
        lv_task_handler();
    }
#endif
    return 0;
}

#endif
