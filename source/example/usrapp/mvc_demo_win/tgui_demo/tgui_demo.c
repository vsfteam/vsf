/*************************__LAST****************************************************
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
declare_tgui_panel(my_stopwatch_t)

def_tgui_panel(my_stopwatch_t,
    tgui_contains(
        vsf_tgui_label_t     time;
        vsf_tgui_button_t    start_stop;
        vsf_tgui_button_t    lap;
        vsf_tgui_button_t    Setting;
        vsf_tgui_label_t     history;
	))

end_def_tgui_panel(my_stopwatch_t)

/*============================ LOCAL VARIABLES ===============================*/
static NO_INIT vsf_tgui_t __tgui_demo;

/*============================ PROTOTYPES ====================================*/
static fsm_rt_t my_stopwatch_start_stop_on_click(vsf_tgui_mc_button_t* ptNode, vsf_msgt_msg_t* ptMSG);

static char s_chTimeBuffer[] = "00:00";
/*============================ GLOBAL VARIABLES ==============================*/

implement_tgui_panel(my_stopwatch_t, __my_stopwatch,

    tgui_region(
        tgui_location(100, 100),
        tgui_size(300, 300),
    ),
    tgui_text(tTitle, "My Stopwatch"),

    tgui_label(time, &__my_stopwatch, 0, false,
        tgui_region(16, 64, 228, 32),
        tgui_text(tLabel, s_chTimeBuffer),
        tgui_background(&ic_settings_phone_RGBA, VSF_TGUI_ALIGN_LEFT),
    ),
    
    tgui_button(start_stop, &__my_stopwatch, sizeof(vsf_tgui_label_t), false,
        tgui_region(16, 110, 100, 32),
        tgui_text(tLabel, "START", VSF_TGUI_ALIGN_CENTER),
    ),

    tgui_button(lap, &__my_stopwatch, sizeof(vsf_tgui_button_t), false,
        tgui_region(300 - 64 - 16, 110, 64, 32),
        tgui_text(tLabel, "LAP"),
    ),

    tgui_button(Setting, &__my_stopwatch, sizeof(vsf_tgui_button_t), false,
        tgui_region(300-16-32, 64, 32, 32),
        tgui_background(&ic_build_black_18dp_RGBA, VSF_TGUI_ALIGN_CENTER),
    ),

    tgui_label(history, &__my_stopwatch, sizeof(vsf_tgui_button_t), true,
        tgui_text(tLabel, "\nabdcdef\nABCDEF"),
        tgui_region(16, 150, 300-32, 128),
        tgui_attribute(bIsUseRawView, true),
    ),
);


/*============================ IMPLEMENTATION ================================*/

void my_stopwatch_init(void)
{
    my_stopwatch_t* ptWatch = &__my_stopwatch;
    vsf_tgui_panel_v_init((vsf_tgui_panel_t *)&(ptWatch->use_as__vsf_tgui_mc_panel_t));
    vsf_tgui_label_v_init((vsf_tgui_label_t *)&(ptWatch->time));
    vsf_tgui_button_v_init((vsf_tgui_button_t *)&(ptWatch->start_stop));
    vsf_tgui_button_v_init((vsf_tgui_button_t *)&(ptWatch->lap));
    vsf_tgui_button_v_init((vsf_tgui_button_t *)&(ptWatch->Setting));
    vsf_tgui_label_v_init((vsf_tgui_label_t *)&(ptWatch->history));
}

static void vsf_tgui_region_init_with_size(vsf_tgui_region_t* ptRegion, vsf_tgui_size_t* ptSize)
{
    ptRegion->tLocation.nX = 0;
    ptRegion->tLocation.nY = 0;
    ptRegion->tSize = *ptSize;
}

static uint32_t __cnt = 0;

void test_only_refresh_time_label(void)
{
    my_stopwatch_t* ptWatch = &__my_stopwatch;

    vsf_tgui_region_t tRegion;
    vsf_tgui_region_init_with_size(&tRegion, &ptWatch->time.tRegion.tSize);
    vsf_tgui_label_v_rendering((vsf_tgui_label_t *)&ptWatch->time, &tRegion);

    
    __cnt++;
    sprintf(s_chTimeBuffer, "%02d:%02d", __cnt / 99 % 99, __cnt % 99);
}

void refresh_my_stopwatch(void)
{
#if 0
    my_stopwatch_t* ptWatch = &__my_stopwatch;

    vsf_tgui_region_t tRegion;

    vsf_tgui_region_init_with_size(&tRegion, &ptWatch->tRegion.tSize);
    vsf_tgui_panel_v_rendering((vsf_tgui_panel_t *)&ptWatch->use_as__vsf_tgui_mc_panel_t, &tRegion);

    vsf_tgui_region_init_with_size(&tRegion, &ptWatch->time.tRegion.tSize);
    vsf_tgui_label_v_rendering((vsf_tgui_label_t *)&ptWatch->time, &tRegion);

    vsf_tgui_region_init_with_size(&tRegion, &ptWatch->start_stop.tRegion.tSize);
    vsf_tgui_button_v_rendering((vsf_tgui_button_t *)&ptWatch->start_stop, &tRegion);

    vsf_tgui_region_init_with_size(&tRegion, &ptWatch->lap.tRegion.tSize);
    vsf_tgui_button_v_rendering((vsf_tgui_button_t *)&ptWatch->lap, &tRegion);

    vsf_tgui_region_init_with_size(&tRegion, &ptWatch->Setting.tRegion.tSize);
    vsf_tgui_button_v_rendering((vsf_tgui_button_t *)&ptWatch->Setting, &tRegion);

    vsf_tgui_region_init_with_size(&tRegion, &ptWatch->history.tRegion.tSize);
    vsf_tgui_label_v_rendering((vsf_tgui_label_t *)&ptWatch->history, &tRegion);

    test_only_refresh_time_label();
#else
    __cnt++;
    sprintf(s_chTimeBuffer, "%02d:%02d", __cnt / 99 % 99, __cnt % 99);

    vsf_tgui_refresh(&__tgui_demo);
#endif
    //

}


static fsm_rt_t my_stopwatch_start_stop_on_click(vsf_tgui_mc_button_t* ptNode, vsf_msgt_msg_t* ptMSG)
{
    return fsm_rt_cpl;
}



vsf_err_t tgui_demo_init(void)
{
    NO_INIT static vsf_tgui_evt_t __evt_queue_buffer[16];
    NO_INIT static uint16_t __bfs_buffer[32];
    const vsf_tgui_cfg_t cfg = {
        .tEVTQueue = {
            .pObj = __evt_queue_buffer, 
            .nSize = sizeof(__evt_queue_buffer)
        },
        .tBFSQueue = {
            .pObj = __bfs_buffer,
            .nSize = sizeof(__bfs_buffer),
        },
        .ptRootNode = (vsf_tgui_control_t *)&__my_stopwatch,
    };
    return vsf_tgui_init(&__tgui_demo, &cfg);
}


void vsf_tgui_on_touchscreen_evt(vsf_touchscreen_evt_t* ts_evt)
{
/*
    vsf_trace(VSF_TRACE_DEBUG, "touchscreen(%d): %s x=%d, y=%d" VSF_TRACE_CFG_LINEEND,
        VSF_INPUT_TOUCHSCREEN_GET_ID(ts_evt),
        VSF_INPUT_TOUCHSCREEN_IS_DOWN(ts_evt) ? "down" : "up",
        VSF_INPUT_TOUCHSCREEN_GET_X(ts_evt),
        VSF_INPUT_TOUCHSCREEN_GET_Y(ts_evt));
 */
    vsf_tgui_evt_t tEvent = {
        .tPointerEvt = {
            .tMSG = VSF_TGUI_MSG_POINTER_EVT,
            .chEvent = VSF_INPUT_TOUCHSCREEN_IS_DOWN(ts_evt) 
                                ?   VSF_TGUI_EVT_POINTER_DOWN 
                                :   VSF_TGUI_EVT_POINTER_UP,
            {
                VSF_INPUT_TOUCHSCREEN_GET_X(ts_evt),
                VSF_INPUT_TOUCHSCREEN_GET_Y(ts_evt),
            },
        },
    };

    vsf_tgui_send_message(&__tgui_demo, tEvent);
}

#endif


/* EOF */
