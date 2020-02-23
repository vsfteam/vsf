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
#include <stdio.h>
#include "./stopwatch.h"

#if VSF_USE_TINY_GUI == ENABLED
#include <stdio.h>
#include "../images/demo_images.h"
#include "../tgui_custom.h"

/*============================ MACROS ========================================*/
#undef base
#define base        (*ptBase)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static fsm_rt_t __on_top_panel_load(vsf_tgui_control_t* ptNode,
                                    vsf_msgt_msg_t* ptMSG);

static fsm_rt_t __on_top_panel_depose(vsf_tgui_control_t* ptNode,
                                    vsf_msgt_msg_t* ptMSG);

static fsm_rt_t __on_top_panel_time(vsf_tgui_control_t* ptNode,
                                    vsf_msgt_msg_t* ptMSG);

static fsm_rt_t __on_button_start_stop_click(   vsf_tgui_control_t* ptNode, 
                                                vsf_msgt_msg_t* ptMSG);
static fsm_rt_t __on_button_start_stop_ok(  vsf_tgui_control_t* ptNode,
                                            vsf_msgt_msg_t* ptMSG);

static fsm_rt_t __on_button_lap_all_pointer_evt(vsf_tgui_control_t* ptNode,
                                                vsf_msgt_msg_t* ptMSG);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

describe_tgui_msgmap(tStartStopMSGMap,
    tgui_msg_handler(VSF_TGUI_EVT_POINTER_CLICK,    __on_button_start_stop_click),
    tgui_msg_handler(VSF_TGUI_EVT_KEY_PRESSED,      __on_button_start_stop_ok),
)

describe_tgui_msgmap(tStopWatchMSGMap,
    tgui_msg_handler(VSF_TGUI_EVT_ON_LOAD,          __on_top_panel_load),
    tgui_msg_handler(VSF_TGUI_EVT_ON_DEPOSE,        __on_top_panel_depose),
    tgui_msg_handler(VSF_TGUI_EVT_ON_TIME,          __on_top_panel_time),
)

describe_tgui_msgmap(tLapMSGMap,
    tgui_msg_mux(VSF_TGUI_MSG_POINTER_EVT, __on_button_lap_all_pointer_evt, VSF_TGUI_MSG_MSK),
)

stopwatch_t* my_stopwatch_init(stopwatch_t* ptPanel, vsf_tgui_t *ptGUI)
{
    do {
        if (NULL == ptPanel && NULL != ptGUI) {
            break;
        }

        describ_tgui_panel(stopwatch_t, *ptPanel,
            tgui_region(
                tgui_location(100, 100),
            ),
            tgui_text(tTitle, "My Stopwatch"),
            tgui_attribute(bIsAutoSize, true),
            //tgui_attribute(tTitle.bIsAutoSize, true),
            tgui_padding(16,16,16,16),

            tgui_msgmap(tStopWatchMSGMap),

            tgui_timer(tTimer, 300, false),

            tgui_label(tTime, ptPanel, tTime, tStartStop,
                tgui_region(0, 48, 228, 32),
                tgui_text(tLabel, ptPanel->chTimeBuffer),
                tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S20),
                tgui_background(&ic_settings_phone_RGBA, VSF_TGUI_ALIGN_LEFT),
            ),

            tgui_button(tStartStop, ptPanel, tTime, tLap,
                tgui_location(0, 94),
                tgui_text(tLabel, "START"),
                tgui_attribute(bIsCheckButton, true),
                tgui_attribute(tLabel.bIsAutoSize, true),

                tgui_msgmap(tStartStopMSGMap),
            ),

            tgui_button(tLap, ptPanel, tStartStop, tSetting,
                tgui_region(300 - 64 - 16 - 16, 94, 64, 32),
                tgui_text(tLabel, "LAP"),
                tgui_attribute(tLabel.bIsAutoSize, true),
                tgui_msgmap(tLapMSGMap),
            ),

            tgui_button(tSetting, ptPanel, tLap, tContainerA,
                tgui_region(300 - 16 - 32 - 16, 48, 32, 32),
                tgui_background(&ic_build_black_18dp_RGBA, VSF_TGUI_ALIGN_CENTER),
            ),

            tgui_container(tContainerA, ptPanel, tSetting, tContainerA,

                tgui_location(0, 150),
                tgui_container_type(VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_HORIZONTAL),
                tgui_attribute(bIsAutoSize, true),

                tgui_contains(
                    tgui_list(tVContainer, &(ptPanel->tContainerA), tVContainer, tNumberList,

                        tgui_size(150, 130),

                        tgui_list_items(
                            tgui_container_type(VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL),

                            tgui_button(tButton1, &(ptPanel->tContainerA.tVContainer.list), tButton1, tButton2,
                                tgui_size(150, 32),
                                tgui_text(tLabel, "tButton1"),
                                   tgui_attribute(tFontColor, VSF_TGUI_COLOR_OLIVE),
                                   tgui_margin(0, 2, 0, 2),
                            ),

                            tgui_button(tButton2, &(ptPanel->tContainerA.tVContainer.list), tButton1, tHContainer,
                                tgui_size(150, 32),
                                tgui_text(tLabel, "tButton2"),
                                tgui_margin(0, 2, 0, 2),
                            ),

                            tgui_list(tHContainer, &(ptPanel->tContainerA.tVContainer.list), tButton2, tHistory,
                                tgui_size(150, 32),
                                tgui_margin(0, 2, 0, 2),

                                tgui_list_items(
                                    tgui_container_type(VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_HORIZONTAL),

                                    tgui_button(tButtonA, &(ptPanel->tContainerA.tVContainer.list.tHContainer.list), tButtonA, tButtonB,
                                        tgui_size(80, 32),
                                        tgui_text(tLabel, "A"),
                                        tgui_margin(2, 0, 2, 0),
                                    ),
                                    tgui_button(tButtonB, &(ptPanel->tContainerA.tVContainer.list.tHContainer.list), tButtonA, tButtonC,
                                        tgui_size(80, 32),
                                        tgui_text(tLabel, "B"),
                                        tgui_margin(2, 0, 2, 0),
                                    ),
                                    tgui_button(tButtonC, &(ptPanel->tContainerA.tVContainer.list.tHContainer.list), tButtonB, tButtonC,
                                        tgui_size(80, 32),
                                        tgui_text(tLabel, "C"),
                                        tgui_margin(2, 0, 2, 0),
                                    ),
                                )
                            ),

                            tgui_label(tHistory, &(ptPanel->tContainerA.tVContainer.list), tHContainer, tHistory,
                                tgui_text(tLabel, "tHistory\n1234\nABCDEF"),
                                tgui_size(150, 128),
                                tgui_attribute(bIsUseRawView, true),
                                tgui_attribute(tFontColor, VSF_TGUI_COLOR_GRAY),
                                tgui_margin(0, 2, 0, 2),
                            ),
                        )
                    ),

                    tgui_text_list(tNumberList, &(ptPanel->tContainerA), tVContainer, tNumberList,
                        tgui_size(100, 100),
                        tgui_margin(8, 0, 8, 0),                                            

                        tgui_text_list_content(

                            tgui_size(100, 0),
                            tgui_text(tLabel, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n"),
                            tgui_line_space(tLabel, 8),
                            tgui_attribute(tFontColor, VSF_TGUI_COLOR_GRAY),
                        )
                    ),
                )
            ),
        );
    } while (0);
    return ptPanel;
}

static fsm_rt_t __on_top_panel_load(vsf_tgui_control_t* ptNode,
                                    vsf_msgt_msg_t* ptMSG)
{
    stopwatch_t *ptPanel = (stopwatch_t *)ptNode;

    vsf_tgui_timer_init(&ptPanel->tTimer, 
                        (const vsf_tgui_control_t *)ptPanel);

    //vsf_tgui_text_list_select_set(&(ptPanel->tContainerA.tNumberList), 6);

    //init_vsf_pt(tgui_demo_t, &(ptPanel->tTask), vsf_prio_0);
    vsf_tgui_control_refresh(ptNode, NULL);
    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}

static fsm_rt_t __on_top_panel_time(vsf_tgui_control_t* ptNode,
                                    vsf_msgt_msg_t* ptMSG)
{
    stopwatch_t *ptPanel = (stopwatch_t *)ptNode;

    vsf_systimer_cnt_t tTimeElapsedInMS = vsf_systimer_tick_to_ms( vsf_systimer_get());
    uint8_t ch10Ms = (tTimeElapsedInMS / 10) % 100;
    uint8_t chSecond = (tTimeElapsedInMS / 1000) % 60;
    uint8_t chMinute = (tTimeElapsedInMS / 60000) % 60;
    sprintf(ptPanel->chTimeBuffer, "%02d:%02d:%02d",  chMinute, chSecond, ch10Ms);

    //! update existing text content
    vsf_tgui_text_set(&(ptPanel->tTime.tLabel), &(ptPanel->tTime.tLabel.tString));

    vk_tgui_refresh_ex(ptPanel->use_as__vsf_tgui_panel_t.ptGUI, 
                        (vsf_tgui_control_t *)&(ptPanel->tTime), NULL);

    vsf_tgui_timer_enable(&ptPanel->tTimer);

    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}

static fsm_rt_t __on_top_panel_depose(vsf_tgui_control_t* ptNode,
                                    vsf_msgt_msg_t* ptMSG)
{
    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}

static fsm_rt_t __on_button_start_stop_click(   vsf_tgui_control_t* ptNode, 
                                                vsf_msgt_msg_t* ptMSG)
{
    VSF_TGUI_LOG(VSF_TRACE_WARNING, "\tCall User Handler");

    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}

static fsm_rt_t __on_button_start_stop_ok(  vsf_tgui_control_t* ptNode,
                                            vsf_msgt_msg_t* ptMSG)
{
    VSF_TGUI_LOG(VSF_TRACE_WARNING, "\tCall User Handler");
    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}

static fsm_rt_t __on_button_lap_all_pointer_evt(vsf_tgui_control_t* ptNode,
                                                vsf_msgt_msg_t* ptMSG)
{
    VSF_TGUI_LOG(VSF_TRACE_WARNING, "\tCall User Handler");
    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}

static vsf_tgui_region_t s_tRefreshRegion = {
                            tgui_location(104, 100),
                            tgui_size(300, 80),
                        };

#if 0
implement_vsf_pt(tgui_demo_t)
{
    stopwatch_t *ptBase = (stopwatch_t*)container_of(ptThis, stopwatch_t, tTask.param);
    vsf_pt_begin();

    UNUSED_PARAM(s_tRefreshRegion);

    //vk_tgui_refresh_ex(ptBase->use_as__vsf_tgui_panel_t.ptGUI, NULL, &s_tRefreshRegion);
    vk_tgui_refresh(ptBase->use_as__vsf_tgui_panel_t.ptGUI);
    while(1) {

        //! refresh timer
        do {
            vsf_systimer_cnt_t tTimeElapsedInMS = vsf_systimer_tick_to_ms( vsf_systimer_get());
            uint8_t ch10Ms = (tTimeElapsedInMS / 10) % 100;
            uint8_t chSecond = (tTimeElapsedInMS / 1000) % 60;
            uint8_t chMinute = (tTimeElapsedInMS / 60000) % 60;
            sprintf(base.chTimeBuffer, "%02d:%02d:%02d",  chMinute, chSecond, ch10Ms);

            //! update existing text content
            vsf_tgui_text_set(&(base.tTime.tLabel), &(base.tTime.tLabel.tString));

            vk_tgui_refresh_ex(base.use_as__vsf_tgui_panel_t.ptGUI, 
                                (vsf_tgui_control_t *)&(base.tTime), NULL);
                                //&s_tRefreshRegion);
        } while(0);

        vsf_pt_wait_until(vsf_delay_ms(50));
    }

    vsf_pt_end();
}
#endif

#endif


/* EOF */
