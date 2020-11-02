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

#define VSF_EDA_CLASS_INHERIT
#include "vsf.h"

#include "lvgl/lvgl.h"
#include "lv_conf.h"

/*============================ MACROS ========================================*/

//#define USRAPP_FLUSH_TEST

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct usrapp_ui_buffer_t {
    struct {
        uint8_t header[2];
    } PACKED;
    lv_color_t color[LV_HOR_RES_MAX];
} PACKED;
typedef struct usrapp_ui_buffer_t usrapp_ui_buffer_t;

struct usrapp_t {
    uint32_t frame_rate;
    uint32_t idletick_orig;
    volatile uint32_t idletick;
    vsf_callback_timer_t poll_timer;

    struct {
        volatile bool started;
        bool last;
#ifdef USRAPP_FLUSH_TEST
        bool inited;
        uint16_t cur_line;
#endif

        lv_disp_buf_t disp_buf;
        lv_disp_drv_t disp_drv;

        usrapp_ui_buffer_t buffer[2];
        usrapp_ui_buffer_t *cur_buffer;

        vsf_eda_t eda;
    } ui;
};
typedef struct usrapp_t usrapp_t;

/*============================ PROTOTYPES ====================================*/

static void usrapp_on_timer(vsf_callback_timer_t *timer);
static void usrapp_ui_disp_evthandler(vsf_eda_t *eda, vsf_evt_t evt);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static usrapp_t usrapp = {
    .poll_timer.on_timer        = usrapp_on_timer,
#ifdef __OOC_DEBUG__
    .ui.eda.evthandler          = usrapp_ui_disp_evthandler,
#endif
};

/*============================ PROTOTYPES ====================================*/

extern void lvgl_create_demo(void);

/*============================ IMPLEMENTATION ================================*/

WEAK(usrapp_trans_init)
void usrapp_trans_init(void) {}
WEAK(usrapp_trans_disp_line)
void usrapp_trans_disp_line(uint8_t *buffer, uint_fast32_t size) {}

#if USE_LV_LOG
static void lvgl_printf(lv_log_level_t level, const char *file, uint32_t line,  const char *dsc)
{
    static const char * lvl_prefix[] = {"Trace", "Info", "Warn", "Error"};
    vsf_trace_debug("%s: %s \t(%s #%d)\r\n", lvl_prefix[level], dsc,  file, line);
}
#endif

static void lvgl_disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    uint_fast32_t pixel_number = LV_HOR_RES;

    if ((0 == area->x1) && ((LV_HOR_RES - 1) == area->x2) && (area->y1 == area->y2)) {
        ASSERT(NULL == usrapp.ui.cur_buffer);
        usrapp.ui.last = (area->y1 == 0);
        usrapp.ui.cur_buffer = container_of(color_p, usrapp_ui_buffer_t, color);
        vsf_eda_post_evt(&usrapp.ui.eda, VSF_EVT_USER);
    } else {
        vsf_trace(0, "non-line disp area [%d,%d], [%d,%d]\r\n",
                    area->x1, area->y1, area->x2, area->y2);
        ASSERT(false);
    }
}

void usrapp_on_ready(void)
{
    usrapp.ui.started = true;
#ifndef __OOC_DEBUG__
    vsf_eda_set_evthandler(&usrapp.ui.eda, usrapp_ui_disp_evthandler);
#endif
    vsf_eda_init(&usrapp.ui.eda, vsf_prio_0, false);
}

static void usrapp_ui_disp_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    usrapp_ui_buffer_t *buffer = usrapp.ui.cur_buffer;

    switch (evt) {
    case VSF_EVT_INIT:
        usrapp.ui.buffer[0].header[0] = usrapp.ui.buffer[1].header[0] = 2;
        usrapp.ui.buffer[0].header[1] = usrapp.ui.buffer[1].header[1] = 0;
        break;
    case VSF_EVT_USER:
        ASSERT(buffer != NULL);
        usrapp_trans_disp_line((uint8_t *)buffer, sizeof(*buffer));
        break;
    case VSF_EVT_MESSAGE:
#ifdef USRAPP_FLUSH_TEST
        usrapp.ui.cur_line++;
        if (usrapp.ui.cur_line >= LV_VER_RES_MAX) {
            usrapp.ui.last = true;
            usrapp.ui.cur_line = 0;
        } else {
            usrapp.ui.last = false;
        }
#else
        lv_disp_flush_ready(&usrapp.ui.disp_drv);
#endif

        if (usrapp.ui.last) {
            usrapp.ui.buffer[0].header[1] = usrapp.ui.buffer[1].header[1] = buffer->header[1] ^ 1;
            usrapp.frame_rate++;
        }
#ifdef USRAPP_FLUSH_TEST
        vsf_eda_post_evt(&usrapp.ui.eda, VSF_EVT_USER);
#else
        usrapp.ui.cur_buffer = NULL;
#endif
        break;
    }
}

static void usrapp_on_timer(vsf_callback_timer_t *timer)
{
    if (usrapp.ui.started) {
        vsf_trace(VSF_TRACE_INFO, "frame_rate: %d" VSF_TRACE_CFG_LINEEND, usrapp.frame_rate);
    }
    uint32_t idletick = usrapp.idletick - usrapp.idletick_orig;
    vsf_trace(VSF_TRACE_INFO, "idletick: %d" VSF_TRACE_CFG_LINEEND, idletick);
    usrapp.idletick_orig = usrapp.idletick;

    usrapp.frame_rate = 0;
    vsf_callback_timer_add_ms(timer, 1000);
}

int main(void)
{
    vsf_start_trace();
    usrapp_trans_init();
    vsf_callback_timer_add_ms(&usrapp.poll_timer, 1000);


#if USE_LV_LOG
    lv_log_register_print(lvgl_printf);
#endif
    lv_init();

    lv_disp_buf_init(   &usrapp.ui.disp_buf,
                        &usrapp.ui.buffer[0].color,
                        &usrapp.ui.buffer[1].color,
                        LV_HOR_RES_MAX * 1);
    lv_disp_drv_init(&usrapp.ui.disp_drv);

    usrapp.ui.disp_drv.hor_res = LV_HOR_RES_MAX;
    usrapp.ui.disp_drv.ver_res = LV_VER_RES_MAX;
    usrapp.ui.disp_drv.flush_cb = lvgl_disp_flush;
    usrapp.ui.disp_drv.buffer = &usrapp.ui.disp_buf;
    lv_disp_drv_register(&usrapp.ui.disp_drv);

    lvgl_create_demo();

    while (1) {
        usrapp.idletick++;
        if (usrapp.ui.started) {
#ifdef USRAPP_FLUSH_TEST
            if (!usrapp.ui.inited) {
                usrapp.ui.inited = true;
                usrapp.ui.cur_line = 0;
                memset(usrapp.ui.buffer[0].color, 0xAA, sizeof(usrapp.ui.buffer[0].color));
                usrapp.ui.cur_buffer = &usrapp.ui.buffer[0];
                vsf_eda_post_evt(&usrapp.ui.eda, VSF_EVT_USER);
            }
#else
            lv_task_handler();
#endif
        }
    }
    return 0;
}

/* EOF */
