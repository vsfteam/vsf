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

#if VSF_USE_TINY_GUI == ENABLED
#include <stdio.h>
#include "./images/demo_images.h"
#include "./stopwatch/stopwatch.h"
/*============================ MACROS ========================================*/
#define DEMO_OFFSET            0

#define DEMO_BACKGROUND_ALIGN   (VSF_TGUI_ALIGN_TOP | VSF_TGUI_ALIGN_LEFT)
//#define DEMO_BACKGROUND_ALIGN   (VSF_TGUI_ALIGN_TOP | VSF_TGUI_ALIGN_RIGHT)
//#define DEMO_BACKGROUND_ALIGN   (VSF_TGUI_ALIGN_BOTTOM | VSF_TGUI_ALIGN_LEFT)
//#define DEMO_BACKGROUND_ALIGN   (VSF_TGUI_ALIGN_BOTTOM | VSF_TGUI_ALIGN_RIGHT)
//#define DEMO_BACKGROUND_ALIGN   (VSF_TGUI_ALIGN_TOP)
//#define DEMO_BACKGROUND_ALIGN   (VSF_TGUI_ALIGN_BOTTOM)
//#define DEMO_BACKGROUND_ALIGN   (VSF_TGUI_ALIGN_LEFT)
//#define DEMO_BACKGROUND_ALIGN   (VSF_TGUI_ALIGN_RIGHT)
//#define DEMO_BACKGROUND_ALIGN   (VSF_TGUI_ALIGN_CENTER)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
static NO_INIT vsf_tgui_t s_tTGUIDemo;
static NO_INIT stopwatch_t s_tMyStopwatch;

/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

static void vsf_tgui_region_init_with_size(vsf_tgui_region_t* region_ptr, vsf_tgui_size_t* ptSize)
{
    region_ptr->tLocation.iX = 0;
    region_ptr->tLocation.iY = 0;
    region_ptr->tSize = *ptSize;
}


void refresh_my_stopwatch(void)
{
    //vk_tgui_refresh(&s_tTGUIDemo);
}


static fsm_rt_t my_stopwatch_start_stop_on_click(vsf_tgui_button_t* node_ptr, vsf_msgt_msg_t* ptMSG)
{
    return fsm_rt_cpl;
}



vsf_err_t tgui_demo_init(void)
{
    NO_INIT static vsf_tgui_evt_t __evt_queue_buffer[16];
#if VSF_TGUI_CFG_REFRESH_SCHEME == VSF_TGUI_REFRESH_SCHEME_BREADTH_FIRST_TRAVERSAL
    NO_INIT static uint16_t __bfs_buffer[32];
#endif
    
    const vsf_tgui_cfg_t cfg = {
        .evt_queue = {
            .obj_ptr = __evt_queue_buffer, 
            .s32_size = sizeof(__evt_queue_buffer)
        },
#if VSF_TGUI_CFG_REFRESH_SCHEME == VSF_TGUI_REFRESH_SCHEME_BREADTH_FIRST_TRAVERSAL
        .bfs_queue = {
            .obj_ptr = __bfs_buffer,
            .s32_size = sizeof(__bfs_buffer),
        },
#endif
        .root_node_ptr = (const vsf_tgui_root_container_t *)&s_tMyStopwatch,
    };
    vsf_err_t err = vk_tgui_init(&s_tTGUIDemo, &cfg);
    
    my_stopwatch_init(&s_tMyStopwatch, &s_tTGUIDemo);

    return err;
}
//
//void vsf_tgui_on_keyboard_evt(vk_keyboard_evt_t* evt)
//{
///*
//    vsf_input_keyboard_get_keycode(evt)
//    vsf_input_keyboard_is_down(evt)
//*/
///*
//    vsf_trace(VSF_TRACE_INFO, "\r\n Key %08x ", vsf_input_keyboard_get_keycode(evt));
//    if (vsf_input_keyboard_is_down(evt)) {
//        vsf_trace(VSF_TRACE_INFO, "is pressed");
//    } else {
//        vsf_trace(VSF_TRACE_INFO, "is released");
//    }
//*/
////! this block of code is used for test purpose only
//    vsf_tgui_evt_t event = {
//        .tKeyEvt = {
//            .msg = vsf_input_keyboard_is_down(evt)
//                                ? VSF_TGUI_EVT_KEY_DOWN
//                                : VSF_TGUI_EVT_KEY_UP,
//            .hwKeyValue = vsf_input_keyboard_get_keycode(evt),
//        },
//    };
//
//    vk_tgui_send_message(&s_tTGUIDemo, event);
//
//    if (!vsf_input_keyboard_is_down(evt)) {
//        event.tKeyEvt.msg = VSF_TGUI_EVT_KEY_PRESSED;
//        vk_tgui_send_message(&s_tTGUIDemo, event);
//    }
//}


//void vsf_tgui_on_touchscreen_evt(vk_touchscreen_evt_t* ts_evt)
//{
///*
//    vsf_trace_debug("touchscreen(%d): %s x=%d, y=%d" VSF_TRACE_CFG_LINEEND,
//        vsf_input_touchscreen_get_id(ts_evt),
//        vsf_input_touchscreen_is_down(ts_evt) ? "down" : "up",
//        vsf_input_touchscreen_get_x(ts_evt),
//        vsf_input_touchscreen_get_y(ts_evt));
// */
//    //! this block of code is used for test purpose only
//
//    vsf_tgui_evt_t event = {
//        .PointerEvt = {
//            .msg = vsf_input_touchscreen_is_down(ts_evt) 
//                                ?   VSF_TGUI_EVT_POINTER_DOWN 
//                                :   VSF_TGUI_EVT_POINTER_UP,
//            
//            .iX = vsf_input_touchscreen_get_x(ts_evt),
//            .iY = vsf_input_touchscreen_get_y(ts_evt),
//        },
//    };
//
//    vk_tgui_send_message(&s_tTGUIDemo, event);
//
//    
//    if (!vsf_input_touchscreen_is_down(ts_evt)) {
//        event.PointerEvt.msg = VSF_TGUI_EVT_POINTER_CLICK;
//        vk_tgui_send_message(&s_tTGUIDemo, event);
//    }
//}

void vsf_tgui_demo_on_ready(void)
{
    bool bRequirePostEvent = false;
    vsf_sched_safe() {
        if (s_tMyStopwatch.tTask.param.bWaitforRefresh) {
            s_tMyStopwatch.tTask.param.bWaitforRefresh = false;
            bRequirePostEvent = true;
        }
    }

    if (bRequirePostEvent) {
        vsf_eda_post_evt(&(s_tMyStopwatch.tTask.use_as__vsf_eda_t), VSF_EVT_USER);
    }
}

#endif


/* EOF */
