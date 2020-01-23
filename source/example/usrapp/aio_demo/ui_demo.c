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

#include "lvgl/lvgl.h"
#include "lv_conf.h"
#include "component/3rd-party/littlevgl/6.0/port/vsf_lvgl_port.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct ui_demo_buffer_t {
    struct {
        uint8_t header[2];
    } PACKED;
    lv_color_t color[LV_HOR_RES_MAX];
} PACKED;
typedef struct ui_demo_buffer_t ui_demo_buffer_t;

struct ui_demo_t {
    uint32_t idletick_orig;
    volatile uint32_t idletick;
    vsf_callback_timer_t poll_timer;

    vk_disp_usbd_uvc_t disp;
    struct {
        volatile bool started;
        lv_disp_buf_t disp_buf;
        ui_demo_buffer_t buffer[2];
    } ui;
};
typedef struct ui_demo_t ui_demo_t;

/*============================ PROTOTYPES ====================================*/

static void ui_demo_on_timer(vsf_callback_timer_t *timer);
extern vk_usbd_uvc_t * usbd_demo_get_uvc(void);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static ui_demo_t ui_demo = {
    .poll_timer.on_timer        = ui_demo_on_timer,
    //.ui.eda.evthandler          = ui_demo_disp_evthandler,

    .disp                       = {
        .param                  = {
            .height             = LV_VER_RES_MAX,
            .width              = LV_HOR_RES_MAX,
            .drv                = &vk_disp_drv_usbd_uvc,
            .color              = VSF_DISP_COLOR_RGB565,
        },
    },
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void lv_refr_task(struct _lv_task_t *task)
{
    static lv_coord_t y;
    lv_obj_t *label1 = (lv_obj_t *)task->user_data;

    y = (y + 10) % LV_VER_RES;
  
    lv_obj_set_y(label1, y);
}

void lvgl_create_demo(void)
{
    lv_obj_t * label1 =  lv_label_create(lv_scr_act(), NULL);
    /*Modify the Label's text*/
    lv_label_set_text(label1, "Hello world!");
    /* Align the Label to the center
    * NULL means align on parent (which is the screen now)
    * 0, 0 at the end means an x, y offset after alignment*/
    lv_obj_align(label1, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_task_create(lv_refr_task, 100, LV_TASK_PRIO_LOWEST, label1);
}

static void ui_demo_on_timer(vsf_callback_timer_t *timer)
{
    if (ui_demo.ui.started) {
        vsf_trace(VSF_TRACE_INFO, "frame_rate: %d" VSF_TRACE_CFG_LINEEND,
                    ui_demo.disp.frame_cnt);
    }
    uint32_t idletick = ui_demo.idletick - ui_demo.idletick_orig;
    vsf_trace(VSF_TRACE_INFO, "idletick: %d" VSF_TRACE_CFG_LINEEND, idletick);
    ui_demo.idletick_orig = ui_demo.idletick;

    ui_demo.disp.frame_cnt = 0;
    vsf_callback_timer_add_ms(timer, 1000);
}

void usbd_demo_uvc_on_ready(void)
{
    ui_demo.ui.started = true;
}

void ui_demo_start(void)
{
    lv_disp_drv_t disp_drv;
    lv_disp_t *disp;

    vsf_callback_timer_add_ms(&ui_demo.poll_timer, 1000);

#if USE_LV_LOG
    lv_log_register_print(vsf_lvgl_printf);
#endif
    lv_init();

    lv_disp_buf_init(   &ui_demo.ui.disp_buf,
                        &ui_demo.ui.buffer[0].color,
                        &ui_demo.ui.buffer[1].color,
                        LV_HOR_RES_MAX * 1);
    lv_disp_drv_init(&disp_drv);

    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    disp_drv.flush_cb = vsf_lvgl_disp_flush;
    disp_drv.buffer = &ui_demo.ui.disp_buf;
    ui_demo.disp.uvc = usbd_demo_get_uvc();
    disp = lv_disp_drv_register(&disp_drv);
    vsf_lvgl_disp_bind(&ui_demo.disp.use_as__vk_disp_t, &disp->driver);

    lvgl_create_demo();
    while (1) {
        ui_demo.idletick++;
        if (ui_demo.ui.started) {
            lv_task_handler();
        }
    }
}

/* EOF */
