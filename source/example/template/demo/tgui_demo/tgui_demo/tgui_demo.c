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

void vsf_tgui_low_level_on_ready_to_refresh(void)
{
    vsf_tgui_low_level_refresh_ready(&s_tTGUIDemo);
}

vsf_err_t tgui_demo_init(void)
{
    NO_INIT static vsf_tgui_evt_t s_tEvtQueueBuffer[16];
    NO_INIT static uint16_t s_tBFSBuffer[32];
    
    const vsf_tgui_cfg_t tCfg = {
        .tEVTQueue = {
            .pObj = s_tEvtQueueBuffer, 
            .nSize = sizeof(s_tEvtQueueBuffer)
        },
        .tBFSQueue = {
            .pObj = s_tBFSBuffer,
            .nSize = sizeof(s_tBFSBuffer),
        },
        //.ptRootNode = (const vsf_tgui_top_container_t *)&s_tMyStopwatch,
    };

    vsf_err_t err = vk_tgui_init(&s_tTGUIDemo, &tCfg);

    my_stopwatch_init(&s_tMyStopwatch, &s_tTGUIDemo);

    vk_tgui_set_top_container(&s_tTGUIDemo, (vsf_tgui_top_container_t *)&s_tMyStopwatch);

    return err;
}

void vsf_tgui_on_keyboard_evt(vk_keyboard_evt_t* evt)
{
//! this block of code is used for test purpose only
    vsf_tgui_evt_t tEvent = {
        .tKeyEvt = {
            .tMSG = vsf_input_keyboard_is_down(evt)
                                ? VSF_TGUI_EVT_KEY_DOWN
                                : VSF_TGUI_EVT_KEY_UP,
            .hwKeyValue = vsf_input_keyboard_get_keycode(evt),
        },
    };

    vsf_tgui_send_message(&s_tTGUIDemo, tEvent);

    if (!vsf_input_keyboard_is_down(evt)) {
        tEvent.tKeyEvt.tMSG = VSF_TGUI_EVT_KEY_PRESSED;
        vsf_tgui_send_message(&s_tTGUIDemo, tEvent);
    }
}


void vsf_tgui_on_touchscreen_evt(vk_touchscreen_evt_t* ts_evt)
{
    //! this block of code is used for test purpose only

    vsf_tgui_evt_t tEvent = {
        .tPointerEvt = {
            .tMSG = vsf_input_touchscreen_is_down(ts_evt) 
                                ?   VSF_TGUI_EVT_POINTER_DOWN 
                                :   VSF_TGUI_EVT_POINTER_UP,
            
            .iX = vsf_input_touchscreen_get_x(ts_evt),
            .iY = vsf_input_touchscreen_get_y(ts_evt),
        },
    };

    vsf_tgui_send_message(&s_tTGUIDemo, tEvent);

    
    if (!vsf_input_touchscreen_is_down(ts_evt)) {
        tEvent.tPointerEvt.tMSG = VSF_TGUI_EVT_POINTER_CLICK;
        vsf_tgui_send_message(&s_tTGUIDemo, tEvent);
    }
}

#endif


/* EOF */
