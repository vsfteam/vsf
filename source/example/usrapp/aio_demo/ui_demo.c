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
    uint32_t frame_rate;
    uint32_t idletick_orig;
    volatile uint32_t idletick;
    vsf_callback_timer_t poll_timer;

    struct {
        volatile bool started;
        bool last;

        lv_disp_buf_t disp_buf;
        lv_disp_drv_t disp_drv;

        ui_demo_buffer_t buffer[2];
        ui_demo_buffer_t *cur_buffer;

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
    //.ui.eda.evthandler          = ui_demo_disp_evthandler,
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

// lvgl porting

#if USE_LV_LOG
static void lvgl_printf(lv_log_level_t level, const char *file, uint32_t line,  const char *dsc)
{
    static const char * lvl_prefix[] = {"Trace", "Info", "Warn", "Error"};
    vsf_trace(VSF_TRACE_DEBUG, "%s: %s \t(%s #%d)\r\n", lvl_prefix[level], dsc,  file, line);
}
#endif

static void lvgl_disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    uint_fast32_t pixel_number = LV_HOR_RES;

    if ((0 == area->x1) && ((LV_HOR_RES - 1) == area->x2) && (area->y1 == area->y2)) {
        ASSERT(NULL == ui_demo.ui.cur_buffer);
        ui_demo.ui.last = (area->y1 == 0);
        ui_demo.ui.cur_buffer = container_of(color_p, ui_demo_buffer_t, color);
        vsf_eda_post_evt(&ui_demo.ui.eda, VSF_EVT_USER);
    } else {
        vsf_trace(0, "non-line disp area [%d,%d], [%d,%d]\r\n",
                    area->x1, area->y1, area->x2, area->y2);
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
    ui_demo_buffer_t *buffer = ui_demo.ui.cur_buffer;

    switch (evt) {
    case VSF_EVT_INIT:
        ui_demo.ui.buffer[0].header[0] = ui_demo.ui.buffer[1].header[0] = 2;
        ui_demo.ui.buffer[0].header[1] = ui_demo.ui.buffer[1].header[1] = 0;
        break;
    case VSF_EVT_USER:
        ASSERT(buffer != NULL);
        usbd_demo_uvc_trans_disp_line((uint8_t *)buffer, sizeof(*buffer));
        break;
    case VSF_EVT_MESSAGE:
        ASSERT(buffer != NULL);
        lv_disp_flush_ready(&ui_demo.ui.disp_drv);

        if (ui_demo.ui.last) {
            ui_demo.ui.buffer[0].header[1] = ui_demo.ui.buffer[1].header[1] = buffer->header[1] ^ 1;
            ui_demo.frame_rate++;
        }
        ui_demo.ui.cur_buffer = NULL;
        break;
    }
}

void usbd_demo_uvc_on_ready(void)
{
    ui_demo.ui.started = true;
    vsf_eda_init(&ui_demo.ui.eda, vsf_prio_0, false);
}

void ui_demo_start(void)
{
    vsf_eda_set_evthandler(&ui_demo.ui.eda, ui_demo_disp_evthandler);

    vsf_callback_timer_add_ms(&ui_demo.poll_timer, 1000);

#if USE_LV_LOG
    lv_log_register_print(lvgl_printf);
#endif
    lv_init();

    lv_disp_buf_init(   &ui_demo.ui.disp_buf,
                        &ui_demo.ui.buffer[0].color,
                        &ui_demo.ui.buffer[1].color,
                        LV_HOR_RES_MAX * 1);
    lv_disp_drv_init(&ui_demo.ui.disp_drv);

    ui_demo.ui.disp_drv.hor_res = LV_HOR_RES_MAX;
    ui_demo.ui.disp_drv.ver_res = LV_VER_RES_MAX;
    ui_demo.ui.disp_drv.flush_cb = lvgl_disp_flush;
    ui_demo.ui.disp_drv.buffer = &ui_demo.ui.disp_buf;
    lv_disp_drv_register(&ui_demo.ui.disp_drv);

    lvgl_create_demo();

    while (1) {
        ui_demo.idletick++;
        if (ui_demo.ui.started) {
            lv_task_handler();
        }
    }
}

/* EOF */
