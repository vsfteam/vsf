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

#include "component/3rd-party/littlevgl/5.3/raw/lvgl/lvgl.h"
#include "lv_conf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct ui_demo_t {
    uint32_t frame_rate;
    uint32_t idletick_orig;
    volatile uint32_t idletick;
    vsf_callback_timer_t poll_timer;

    struct {
        volatile bool started;
        bool flushing;
        bool last;
        lv_disp_drv_t disp_drv;
        uint8_t buffer[2 + (LV_HOR_RES * LV_COLOR_DEPTH / 8)];
        vsf_eda_t eda;
    } ui;
};
typedef struct ui_demo_t ui_demo_t;

/*============================ PROTOTYPES ====================================*/

static void ui_demo_on_timer(vsf_callback_timer_t *timer);
static void ui_demo_disp_evthandler(vsf_eda_t *eda, vsf_evt_t evt);

extern void usbd_demo_uvc_trans_disp_line(uint8_t *buffer, uint_fast32_t size);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static ui_demo_t ui_demo = {
    .poll_timer.on_timer        = ui_demo_on_timer,
    .ui.eda.evthandler          = ui_demo_disp_evthandler,
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void lv_refr_task(void * param)
{
    static lv_coord_t y;
    lv_obj_t *label1 = (lv_obj_t *)param;

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

// lvgl porting

#if USE_LV_LOG
static void lvgl_printf(lv_log_level_t level, const char *file, uint32_t line,  const char *dsc)
{
    static const char * lvl_prefix[] = {"Trace", "Info", "Warn", "Error"};
    vsf_trace(VSF_TRACE_DEBUG, "%s: %s \t(%s #%d)\r\n", lvl_prefix[level], dsc,  file, line);
}
#endif

static void lvgl_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color)
{
    uint_fast32_t pixel_number = LV_HOR_RES;

    if ((0 == x1) && ((LV_HOR_RES - 1) == x2) && (y1 == y2)) {
        ui_demo.ui.last = (y1 == (LV_VER_RES - 1));
        memcpy(&ui_demo.ui.buffer[2], color, pixel_number * LV_COLOR_DEPTH / 8);
        ui_demo.ui.flushing = true;
        vsf_eda_post_evt(&ui_demo.ui.eda, VSF_EVT_USER);
    } else {
        vsf_trace(0, "non-line disp area [%d,%d], [%d,%d]\r\n", x1, y1, x2, y2);
        ASSERT(false);
    }
}

static void ui_demo_on_timer(vsf_callback_timer_t *timer)
{
    if (ui_demo.ui.started) {
        vsf_trace(VSF_TRACE_INFO, "frame_rate: %d" VSF_TRACE_CFG_LINEEND, ui_demo.frame_rate);
    }
    uint32_t idletick = ui_demo.idletick - ui_demo.idletick_orig;
    vsf_trace(VSF_TRACE_INFO, "idletick: %d" VSF_TRACE_CFG_LINEEND, idletick);
    ui_demo.idletick_orig = ui_demo.idletick;

    ui_demo.frame_rate = 0;
    vsf_callback_timer_add_ms(timer, 1000);
}

static void ui_demo_disp_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    uint8_t *buffer = ui_demo.ui.buffer;

    switch (evt) {
    case VSF_EVT_INIT:
        buffer[0] = 2;
        buffer[1] = 0;
        break;
    case VSF_EVT_USER:
        usbd_demo_uvc_trans_disp_line(buffer, sizeof(ui_demo.ui.buffer));
        break;
    case VSF_EVT_MESSAGE:
        ui_demo.ui.flushing = false;
        lv_flush_ready();

        if (ui_demo.ui.last) {
            buffer[1] ^= 1;
            ui_demo.frame_rate++;
        }
        break;
    }
}

void usbd_demo_uvc_on_ready(void)
{
    ui_demo.ui.started = true;
    vsf_eda_init(&ui_demo.ui.eda, vsf_priority_0, false);
}

void ui_demo_start(void)
{
    vsf_callback_timer_add_ms(&ui_demo.poll_timer, 1000);

#if USE_LV_LOG
    lv_log_register_print(lvgl_printf);
#endif
    lv_init();

    lv_disp_drv_init(&ui_demo.ui.disp_drv);
    ui_demo.ui.disp_drv.disp_flush = lvgl_disp_flush;
    lv_disp_drv_register(&ui_demo.ui.disp_drv);

    lvgl_create_demo();

    while (1) {
        ui_demo.idletick++;
        if (ui_demo.ui.started && !ui_demo.ui.flushing) {
            lv_task_handler();
        }
    }
}

/* EOF */
