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

static fsm_rt_t __on_button_start_stop_click(   vsf_tgui_control_t* ptNode, 
                                                vsf_msgt_msg_t* ptMSG);
static fsm_rt_t __on_button_start_stop_ok(  vsf_tgui_control_t* ptNode,
                                            vsf_msgt_msg_t* ptMSG);

static fsm_rt_t __on_button_lap_all_pointer_evt(vsf_tgui_control_t* ptNode,
                                                vsf_msgt_msg_t* ptMSG);

/*============================ GLOBAL VARIABLES ==============================*/

#if 0
implement_tgui_panel(stopwatch_t, s_tMyStopwatch,

    tgui_region(
        tgui_location(100, 100),
        //tgui_size(300, 300),
        ),
    tgui_text(tTitle, "My Stopwatch"),
    tgui_attribute(bIsAutoSize, true),
    tgui_padding(16, 16, 16, 16),

    tgui_label(tTime, ptPanel, tTime, tStartStop,
        tgui_region(0, 48, 228, 32),
        tgui_text(tLabel, ptPanel->chTimeBuffer),
        tgui_background(&ic_settings_phone_RGBA, VSF_TGUI_ALIGN_LEFT),
        ),

    tgui_button(tStartStop, ptPanel, tTime, tLap,
        tgui_region(0, 94, 100, 32),
        tgui_text(tLabel, "START", VSF_TGUI_ALIGN_CENTER),
        ),

    tgui_button(tLap, ptPanel, tStartStop, tSetting,
        tgui_region(300 - 64 - 16 - 16, 94, 64, 32),
        tgui_text(tLabel, "LAP"),
        ),

    tgui_button(tSetting, ptPanel, tLap, vcontainer,
        tgui_region(300 - 16 - 32 - 16, 48, 32, 32),
        tgui_background(&ic_build_black_18dp_RGBA, VSF_TGUI_ALIGN_CENTER),
        ),

    tgui_list(vcontainer, ptPanel, tSetting, vcontainer,

        tgui_size(300, 130),
        tgui_location(0, 150),

        tgui_list_items(
            tgui_container_type(VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL),

            tgui_button(tButton1, &(ptPanel->vcontainer.list), tButton1, tButton2,
                tgui_size(300, 32),
                tgui_text(tLabel, "tButton1"),
                tgui_margin(0, 2, 0, 2),
                ),

            tgui_button(tButton2, &(ptPanel->vcontainer.list), tButton1, tHContainer,
                tgui_size(300, 32),
                tgui_text(tLabel, "tButton2"),
                tgui_margin(0, 2, 0, 2),
                ),

            tgui_list(tHContainer, &(ptPanel->vcontainer.list), tButton2, tHistory,
                tgui_size(150, 32),
                tgui_margin(0, 2, 0, 2),

                tgui_list_items(
                    tgui_container_type(VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_HORIZONTAL),

                    tgui_button(tButtonA, &(ptPanel->vcontainer.list.tHContainer.list), tButtonA, tButtonB,
                        tgui_size(80, 32),
                        tgui_text(tLabel, "A"),
                        tgui_margin(2, 0, 2, 0),
                        ),
                    tgui_button(tButtonB, &(ptPanel->vcontainer.list.tHContainer.list), tButtonA, tButtonC,
                        tgui_size(80, 32),
                        tgui_text(tLabel, "B"),
                        tgui_margin(2, 0, 2, 0),
                        ),
                    tgui_button(tButtonC, &(ptPanel->vcontainer.list.tHContainer.list), tButtonB, tButtonC,
                        tgui_size(80, 32),
                        tgui_text(tLabel, "C"),
                        tgui_margin(2, 0, 2, 0),
                        ),
                    )
            ),

            tgui_label(tHistory, &(ptPanel->vcontainer.list), tHContainer, tHistory,
                tgui_text(tLabel, "tHistory\n\nabdcdef\nABCDEF"),
                tgui_size(300, 128),
                tgui_attribute(bIsUseRawView, true),
                tgui_margin(0, 2, 0, 2),
                ),
            )
    ),
);
#endif

/*============================ IMPLEMENTATION ================================*/

describe_tgui_msgmap(tStartStopMSGMap,
    tgui_msg_handler(VSF_TGUI_EVT_POINTER_CLICK,    __on_button_start_stop_click),
    tgui_msg_handler(VSF_TGUI_EVT_KEY_PRESSED,      __on_button_start_stop_ok),
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
                //tgui_size(300, 300),
            ),
            tgui_text(tTitle, "My Stopwatch"),
            tgui_attribute(bIsAutoSize, true),
            tgui_padding(16,16,16,16),

            tgui_label(tTime, ptPanel, tTime, tStartStop,
                tgui_region(0, 48, 228, 32),
                tgui_text(tLabel, ptPanel->chTimeBuffer),
                tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S20),
                tgui_background(&ic_settings_phone_RGBA, VSF_TGUI_ALIGN_LEFT),
            ),

            tgui_button(tStartStop, ptPanel, tTime, tLap,
                tgui_region(0, 94, 100, 32),
                tgui_text(tLabel, "START", VSF_TGUI_ALIGN_CENTER),
                tgui_attribute(bIsCheckButton, true),

                tgui_msgmap(tStartStopMSGMap),

            ),

            tgui_button(tLap, ptPanel, tStartStop, tSetting,
                tgui_region(300 - 64 - 16 - 16, 94, 64, 32),
                tgui_text(tLabel, "LAP"),

                tgui_msgmap(tLapMSGMap),
            ),

            tgui_button(tSetting, ptPanel, tLap, tVContainer,
                tgui_region(300 - 16 - 32 - 16, 48, 32, 32),
                tgui_background(&ic_build_black_18dp_RGBA, VSF_TGUI_ALIGN_CENTER),
            ),

            tgui_list(tVContainer, ptPanel, tSetting, tVContainer,

                tgui_size(300, 130),
                tgui_location(0, 150),

                tgui_list_items(
                    tgui_container_type(VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL),

                    tgui_button(tButton1, &(ptPanel->tVContainer.list), tButton1, tButton2,
                        tgui_size(300, 32),
                        tgui_text(tLabel, "tButton1"),
                           tgui_attribute(tFontColor, VSF_TGUI_COLOR_OLIVE),
                           tgui_margin(0, 2, 0, 2),
                    ),

                    tgui_button(tButton2, &(ptPanel->tVContainer.list), tButton1, tHContainer,
                        tgui_size(300, 32),
                        tgui_text(tLabel, "tButton2"),
                        tgui_margin(0, 2, 0, 2),
                    ),

                    tgui_list(tHContainer, &(ptPanel->tVContainer.list), tButton2, tHistory,
                        tgui_size(150, 32),
                        tgui_margin(0, 2, 0, 2),

                        tgui_list_items(
                            tgui_container_type(VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_HORIZONTAL),

                            tgui_button(tButtonA, &(ptPanel->tVContainer.list.tHContainer.list), tButtonA, tButtonB,
                                tgui_size(80, 32),
                                tgui_text(tLabel, "A"),
                                tgui_margin(2, 0, 2, 0),
                            ),
                            tgui_button(tButtonB, &(ptPanel->tVContainer.list.tHContainer.list), tButtonA, tButtonC,
                                tgui_size(80, 32),
                                tgui_text(tLabel, "B"),
                                tgui_margin(2, 0, 2, 0),
                            ),
                            tgui_button(tButtonC, &(ptPanel->tVContainer.list.tHContainer.list), tButtonB, tButtonC,
                                tgui_size(80, 32),
                                tgui_text(tLabel, "C"),
                                tgui_margin(2, 0, 2, 0),
                            ),
                        )
                    ),

                    tgui_label(tHistory, &(ptPanel->tVContainer.list), tHContainer, tHistory,
                        tgui_text(tLabel, "tHistory\n\nabdcdef\nABCDEF"),
                        tgui_size(300, 128),
                        tgui_attribute(bIsUseRawView, true),
                        tgui_attribute(tFontColor, VSF_TGUI_COLOR_GRAY),
                        tgui_margin(0, 2, 0, 2),
                    ),
                )
            ),
            
        );

        ptPanel->use_as__vsf_tgui_panel_t.ptGUI = ptGUI;

        init_vsf_pt(tgui_demo_t, &(ptPanel->tTask), vsf_prio_0);
    } while (0);
    return ptPanel;
}

static fsm_rt_t __on_button_start_stop_click(vsf_tgui_control_t* ptNode, vsf_msgt_msg_t* ptMSG)
{
    vsf_trace(VSF_TRACE_WARNING, "\tCall User Handler");

    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}

static fsm_rt_t __on_button_start_stop_ok(  vsf_tgui_control_t* ptNode,
                                            vsf_msgt_msg_t* ptMSG)
{
    vsf_trace(VSF_TRACE_WARNING, "\tCall User Handler");
    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}

static fsm_rt_t __on_button_lap_all_pointer_evt(  vsf_tgui_control_t* ptNode,
                                            vsf_msgt_msg_t* ptMSG)
{
    vsf_trace(VSF_TRACE_WARNING, "\tCall User Handler");
    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}

static vsf_tgui_region_t s_tRefreshRegion = {
                            tgui_location(104, 100),
                            tgui_size(300, 80),
                        };

implement_vsf_pt(tgui_demo_t)
{
    stopwatch_t *ptBase = (stopwatch_t*)container_of(ptThis, stopwatch_t, tTask.param);
    vsf_pt_begin();

    //vk_tgui_refresh_ex(ptBase->use_as__vsf_tgui_panel_t.ptGUI, NULL, &s_tRefreshRegion);
    vk_tgui_refresh(ptBase->use_as__vsf_tgui_panel_t.ptGUI);
    while(1) {
#if 0
        //! refresh timer
        do {
            vsf_systimer_cnt_t tTimeElapsedInMS = vsf_systimer_tick_to_ms( vsf_systimer_get());
            uint8_t ch10Ms = (tTimeElapsedInMS / 10) % 100;
            uint8_t chSecond = (tTimeElapsedInMS / 1000) % 60;
            uint8_t chMinute = (tTimeElapsedInMS / 60000) % 60;
            sprintf(base.chTimeBuffer, "%02d:%02d:%02d",  chMinute, chSecond, ch10Ms);
            vk_tgui_refresh_ex(base.use_as__vsf_tgui_panel_t.ptGUI, 
                                (vsf_tgui_control_t *)&(base.tTime), NULL);
                                //&s_tRefreshRegion);
        } while(0);
#endif
        vsf_pt_wait_until(vsf_delay_ms(50));
    }

    vsf_pt_end();
}

#endif


/* EOF */
