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

struct usrapp_t {
    uint32_t frame_rate;
    uint32_t idletick_orig;
    volatile uint32_t idletick;
    vsf_callback_timer_t poll_timer;
    struct {
        volatile bool started;
        bool flushing;
        bool last;
        //lv_disp_drv_t disp_drv;
        uint8_t buffer[2 + (LV_HOR_RES * LV_COLOR_DEPTH / 8)];
        vsf_eda_t eda;
    } ui;
};
typedef struct usrapp_t usrapp_t;

/*============================ PROTOTYPES ====================================*/

static void usrapp_on_timer(vsf_callback_timer_t *timer);
static void usrapp_ui_disp_evthandler(vsf_eda_t *eda, vsf_evt_t evt);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

usrapp_t usrapp = {
    .poll_timer.on_timer        = usrapp_on_timer,
    .ui.eda.evthandler          = usrapp_ui_disp_evthandler,
};

/*============================ PROTOTYPES ====================================*/

extern void lvgl_create_demo(void);

/*============================ IMPLEMENTATION ================================*/

WEAK void ui_demo_trans_init(void) {}
WEAK void ui_demo_trans_disp_line(uint8_t *buffer, uint_fast32_t size) {}
WEAK void uvc_app_init(void){}
WEAK void uvc_app_task(void){}

void ui_demo_on_ready(void)
{
    usrapp.ui.started = true;
    vsf_eda_init(&usrapp.ui.eda, vsf_priority_0, false);
}

void uvc_app_fill_line(void *line_buf, uint_fast16_t size, bool last_line)
{
    /*
    usrapp.ui.last = (y1 == (LV_VER_RES - 1));
    memcpy(&usrapp.ui.buffer[2], color, pixel_number * LV_COLOR_DEPTH / 8);
    usrapp.ui.flushing = true;
    vsf_eda_post_evt(&usrapp.ui.eda, VSF_EVT_USER);
    */
    usrapp.ui.last = last_line;
    usrapp.ui.flushing = true;
    memcpy(&usrapp.ui.buffer[2], line_buf, size);
    vsf_eda_post_evt(&usrapp.ui.eda, VSF_EVT_USER);
}

static void usrapp_ui_disp_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    uint8_t *buffer = usrapp.ui.buffer;

    switch (evt) {
    case VSF_EVT_INIT:
        buffer[0] = 2;
        buffer[1] = 0;
        break;
    case VSF_EVT_USER:
        ui_demo_trans_disp_line(buffer, sizeof(usrapp.ui.buffer));
        break;
    case VSF_EVT_MESSAGE:
        usrapp.ui.flushing = false;
        lv_flush_ready();

        if (usrapp.ui.last) {
            buffer[1] ^= 1;
            usrapp.frame_rate++;
        }
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
    vsf_trace_init(NULL);
    ui_demo_trans_init();
    vsf_callback_timer_add_ms(&usrapp.poll_timer, 1000);

    uvc_app_init();

    while (1) {
        usrapp.idletick++;
        if (usrapp.ui.started && !usrapp.ui.flushing) {
            uvc_app_task();
        }
    }
    return 0;
}

/* EOF */
