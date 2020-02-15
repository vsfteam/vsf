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

/*============================ MACROS ========================================*/
#ifndef VSF_UVC_HELPER_CFG_SUPPORT_LINE_BUFFER
#   define VSF_UVC_HELPER_CFG_SUPPORT_LINE_BUFFER           DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
#if VSF_UVC_HELPER_CFG_SUPPORT_LINE_BUFFER == ENABLED
typedef struct line_buffer_t line_buffer_t;
struct line_buffer_t {
    implement(vsf_slist_node_t)
    uint16_t line[256];
    bool    last;
};
#endif

struct vk_usbd_uvc_helper_t {
    volatile struct {
        uint8_t started     : 1;
        uint8_t flushing    : 1;
        uint8_t last        : 1;
    }flag;
    uint8_t frame_rate;
    uint8_t idletick_orig;
    volatile uint8_t idletick;
#if VSF_UVC_HELPER_CFG_SUPPORT_LINE_BUFFER == ENABLED
    line_buffer_t *current_line;
    vsf_slist_queue_t line_queue;
#endif

    uint8_t buffer[2 + (240 * 16 / 8)];
    vsf_eda_t eda;
    vsf_callback_timer_t poll_timer;
};
typedef struct vk_usbd_uvc_helper_t vk_usbd_uvc_helper_t;

#if VSF_UVC_HELPER_CFG_SUPPORT_LINE_BUFFER == ENABLED
declare_vsf_pool(line_pool);
def_vsf_pool(line_pool, line_buffer_t);
#endif
/*============================ PROTOTYPES ====================================*/

static void __uvc_helper_on_statistic_timer(vsf_callback_timer_t *timer);
static void __uvc_helper_refresh_evthandler(vsf_eda_t *eda, vsf_evt_t evt);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static vk_usbd_uvc_helper_t __uvc_helper = {
    .poll_timer.on_timer    = __uvc_helper_on_statistic_timer,
    //.eda.evthandler         = __uvc_helper_refresh_evthandler,
};

/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/
#if VSF_UVC_HELPER_CFG_SUPPORT_LINE_BUFFER == ENABLED
implement_vsf_pool(line_pool, line_buffer_t)

static NO_INIT vsf_pool(line_pool) __line_pool;
#endif

WEAK(ui_demo_trans_init)
void ui_demo_trans_init(void) {}
WEAK(ui_demo_trans_disp_line)
void ui_demo_trans_disp_line(uint8_t *buffer, uint_fast32_t size) {}
WEAK(uvc_app_init)
void uvc_app_init(void){}
WEAK(uvc_app_task)
void uvc_app_task(void){}
WEAK(uvc_app_on_fill_line_cpl)
void uvc_app_on_fill_line_cpl(bool frame_cpl){}

void ui_demo_on_ready(void)
{
    __uvc_helper.flag.started = true;
    vsf_eda_init(&__uvc_helper.eda, vsf_prio_0, false);
}

void uvc_app_fill_line(void *line_buf, uint_fast16_t size, bool last_line)
{
#if VSF_UVC_HELPER_CFG_SUPPORT_LINE_BUFFER == ENABLED
    line_buffer_t *line = NULL;
    do {
        line = VSF_POOL_ALLOC(line_pool, &__line_pool);
    } while(NULL == line);
    
    line->last = last_line;
    memcpy(line->line, line_buf, size);

    vsf_slist_queue_enqueue(line_buffer_t, 
                            use_as__vsf_slist_node_t, 
                            &(__uvc_helper.line_queue),
                            line);
#else
    while(__uvc_helper.flag.flushing);
    __uvc_helper.flag.last = last_line;
    memcpy(&__uvc_helper.buffer[2], line_buf, size);
    __uvc_helper.flag.flushing = true;
#endif
    
    vsf_eda_post_evt(&__uvc_helper.eda, VSF_EVT_USER);
}

#if VSF_UVC_HELPER_CFG_SUPPORT_LINE_BUFFER == ENABLED
static bool __request_transfer(vk_usbd_uvc_helper_t *pthis)
{
    bool bResult = false;
    vsf_sched_safe() {
        do {
            if (NULL != pthis->current_line) {
                //! busy
                break;
            }
            //! dequeue
            vsf_slist_queue_dequeue(line_buffer_t, 
                                    use_as__vsf_slist_node_t, 
                                    &(__uvc_helper.line_queue),
                                    (pthis->current_line));
            if (NULL == pthis->current_line) {
                //! no content
                break;
            }
            memcpy(&pthis->buffer[2], pthis->current_line, sizeof(pthis->current_line->line));
            pthis->flag.last = (pthis->current_line->last) != 0;
            pthis->flag.flushing = true;
            ui_demo_trans_disp_line(pthis->buffer, sizeof(pthis->buffer));
            bResult = true;
        } while(0);
    }

    return bResult;
}
#endif

static void __uvc_helper_refresh_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    uint8_t *buffer = __uvc_helper.buffer;

    switch (evt) {
    case VSF_EVT_INIT:
        buffer[0] = 2;
        buffer[1] = 0;
        break;
    case VSF_EVT_USER:
    #if VSF_UVC_HELPER_CFG_SUPPORT_LINE_BUFFER == ENABLED
        __request_transfer(&__uvc_helper);
    #else
        ui_demo_trans_disp_line(buffer, sizeof(__uvc_helper.buffer));
    #endif
        break;
    case VSF_EVT_MESSAGE:
        __uvc_helper.flag.flushing = false;
        if (__uvc_helper.flag.last) {
            buffer[1] ^= 1;
            __uvc_helper.frame_rate++;
        }

    #if VSF_UVC_HELPER_CFG_SUPPORT_LINE_BUFFER == ENABLED
        vsf_sched_safe() {
            VSF_POOL_FREE(line_pool, &__line_pool, __uvc_helper.current_line);
            __uvc_helper.current_line = NULL;
        }
        __request_transfer(&__uvc_helper);
    #endif
        uvc_app_on_fill_line_cpl(__uvc_helper.flag.last);
        break;
    }
}

static void __uvc_helper_on_statistic_timer(vsf_callback_timer_t *timer)
{
    if (__uvc_helper.flag.started) {
        vsf_trace(VSF_TRACE_INFO, "frame_rate: %d" VSF_TRACE_CFG_LINEEND, __uvc_helper.frame_rate);
    }
    uint32_t idletick = __uvc_helper.idletick - __uvc_helper.idletick_orig;
    vsf_trace(VSF_TRACE_INFO, "idletick: %d" VSF_TRACE_CFG_LINEEND, idletick);
    __uvc_helper.idletick_orig = __uvc_helper.idletick;

    __uvc_helper.frame_rate = 0;
    vsf_callback_timer_add_ms(timer, 1000);
}

vsf_err_t vk_usbd_uvc_helper_init(void)
{
    if (VSF_ERR_NONE != vsf_eda_set_evthandler(&__uvc_helper.eda, 
                                                __uvc_helper_refresh_evthandler)) {
        ASSERT(false);
    }

#if VSF_UVC_HELPER_CFG_SUPPORT_LINE_BUFFER == ENABLED
    VSF_POOL_PREPARE(line_pool, &__line_pool);
    vsf_slist_queue_init(&(__uvc_helper.line_queue));
#endif

    ui_demo_trans_init();
    vsf_callback_timer_add_ms(&__uvc_helper.poll_timer, 1000);

    uvc_app_init();
    return VSF_ERR_NONE;
}

void vk_usbd_uvc_helper_task(void)
{
    __uvc_helper.idletick++;
    if (__uvc_helper.flag.started) {
        uvc_app_task();
    }
}

static void demo(void)
{
#define MFUNC_IN_U8_DEC_VALUE          0x7E

#include "utilities\preprocessor\mf_u8_dec2str.h"

    vsf_trace(VSF_TRACE_INFO, "0x7F is converted to " STR(MFUNC_OUT_DEC_STR) );
}

int main(void)
{
    vsf_trace_init((vsf_stream_t *)&VSF_DEBUG_STREAM_TX);

    demo();

    vk_usbd_uvc_helper_init();

    while (1) {
        vk_usbd_uvc_helper_task();
    }
    return 0;
}



/* EOF */
