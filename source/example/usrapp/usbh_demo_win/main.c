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
#include "../usrapp_common.h"

#if VSF_USE_UI == ENABLED && VSF_USE_LVGL == ENABLED
#   include "lvgl/lvgl.h"
#   include "lv_conf.h"
#   include "component/3rd-party/littlevgl/6.0/port/vsf_lvgl_port.h"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct usrapp_t {
#if VSF_USE_UI == ENABLED && VSF_USE_LVGL == ENABLED
    struct {
        vk_disp_sdl2_t disp;
        vk_touchscreen_evt_t ts_evt;
        lv_disp_buf_t disp_buf;
        lv_color_t color[LV_VER_RES_MAX][LV_HOR_RES_MAX];
    } ui;
#endif

#if VSF_USE_UI == ENABLED && VSF_USE_TINY_GUI == ENABLED
    struct {
        vk_disp_sdl2_t disp;
        vk_touchscreen_evt_t ts_evt;
        vsf_tgui_color_t color[VSF_TGUI_VER_MAX][VSF_TGUI_HOR_MAX];
    } ui;
#endif

};
typedef struct usrapp_t usrapp_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static usrapp_t usrapp = {
#if VSF_USE_UI == ENABLED && VSF_USE_LVGL == ENABLED
    .ui.disp                    = {
        .param                  = {
            .height             = LV_VER_RES_MAX,
            .width              = LV_HOR_RES_MAX,
            .drv                = &vk_disp_drv_sdl2,
            .color              = VSF_DISP_COLOR_RGB565,
        },
        .amplifier              = 2,
    },
#endif
#if VSF_USE_UI == ENABLED && VSF_USE_TINY_GUI == ENABLED
    .ui.disp                    = {
        .param                  = {
            .height             = VSF_TGUI_VER_MAX,
            .width              = VSF_TGUI_HOR_MAX,
            .drv                = &vk_disp_drv_sdl2,
            .color              = VSF_DISP_COLOR_ARGB8888,
        },
        .amplifier              = 1,
    },
#endif
};

/*============================ PROTOTYPES ====================================*/

#if VSF_USE_UI == ENABLED && VSF_USE_LVGL == ENABLED
extern void ui_demo_start(void);
#endif

/*============================ IMPLEMENTATION ================================*/

#if VSF_USE_UI == ENABLED && VSF_USE_LVGL == ENABLED
void vsf_input_on_touchscreen(vk_touchscreen_evt_t *ts_evt)
{
    if (0 == vsf_input_touchscreen_get_id(ts_evt)) {
        usrapp.ui.ts_evt = *ts_evt;
//        vsf_trace_debug("ts: (%d, %d) %s" VSF_TRACE_CFG_LINEEND,
//                vsf_input_touchscreen_get_x(ts_evt),
//                vsf_input_touchscreen_get_y(ts_evt),
//                vsf_input_touchscreen_is_down(ts_evt) ? "down" : "up");
    }
}

static bool usrapp_touchscreen_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    data->state = vsf_input_touchscreen_is_down(&usrapp.ui.ts_evt) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    data->point.x = vsf_input_touchscreen_get_x(&usrapp.ui.ts_evt);
    data->point.y = vsf_input_touchscreen_get_y(&usrapp.ui.ts_evt);
//    vsf_trace_debug("touchscreen: %s x=%d, y=%d" VSF_TRACE_CFG_LINEEND,
//        data->state == LV_INDEV_STATE_PR ? "press" : "release",
//        data->point.x, data->point.y);
    return false;
}
#endif

// TODO: SDL require that main need argc and argv
int main(int argc, char *argv[])
{
    __usrapp_common_init();

#if VSF_USE_UI == ENABLED && VSF_USE_LVGL == ENABLED
#   if USE_LV_LOG
    lv_log_register_print(vsf_lvgl_printf);
#   endif
    lv_init();

    lv_disp_drv_t disp_drv;
    lv_indev_drv_t indev_drv;
    lv_disp_t *disp;

    lv_disp_buf_init(   &usrapp.ui.disp_buf,
                        &usrapp.ui.color,
                        NULL,
                        LV_HOR_RES_MAX * LV_VER_RES_MAX);
    lv_disp_drv_init(&disp_drv);

    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    disp_drv.flush_cb = vsf_lvgl_disp_flush;
    disp_drv.buffer = &usrapp.ui.disp_buf;
    disp = lv_disp_drv_register(&disp_drv);
    vsf_lvgl_disp_bind(&usrapp.ui.disp.use_as__vk_disp_t, &disp->driver);

    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.disp = disp;
    indev_drv.read_cb = usrapp_touchscreen_read;
    lv_indev_drv_register(&indev_drv);

    ui_demo_start();
    while (1) {
        lv_task_handler();
    }
#endif

#if VSF_USE_UI == ENABLED && VSF_USE_TINY_GUI == ENABLED
	extern void vsf_tgui_bind(vk_disp_t * disp, void* ui_data);
	vsf_tgui_bind(&usrapp.ui.disp, &usrapp.ui.color);
#endif
    return 0;
}

/* EOF */
