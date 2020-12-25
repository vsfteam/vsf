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

static fsm_rt_t __on_top_panel_load(vsf_tgui_control_t* node_ptr,
                                    vsf_msgt_msg_t* ptMSG);

static fsm_rt_t __on_top_panel_depose(vsf_tgui_control_t* node_ptr,
                                    vsf_msgt_msg_t* ptMSG);

#if VSF_TGUI_CFG_SUPPORT_TIMER == ENABLED
static fsm_rt_t __on_top_panel_time(vsf_tgui_control_t* node_ptr,
                                    vsf_msgt_msg_t* ptMSG);
#endif

static fsm_rt_t __on_button_start_stop_click(   vsf_tgui_control_t* node_ptr,
                                                vsf_msgt_msg_t* ptMSG);
static fsm_rt_t __on_button_start_stop_ok(  vsf_tgui_control_t* node_ptr,
                                            vsf_msgt_msg_t* ptMSG);

static fsm_rt_t __on_button_lap_all_pointer_evt(vsf_tgui_control_t* node_ptr,
                                                vsf_msgt_msg_t* ptMSG);
#if VSF_TGUI_CFG_SUPPORT_TEXT_LIST == ENABLED
static fsm_rt_t __on_text_list_post_refresh(vsf_tgui_control_t* node_ptr,
                                                vsf_msgt_msg_t* ptMSG);
#endif
#if VSF_TGUI_CFG_SUPPORT_LIST == ENABLED
static fsm_rt_t __on_list_post_refresh( vsf_tgui_list_t* ptList,
                                        vsf_tgui_refresh_evt_t* event_ptr);

static fsm_rt_t __on_list_sliding_started(  vsf_tgui_list_t* ptList,
                                            vsf_tgui_refresh_evt_t* event_ptr);

static fsm_rt_t __on_list_sliding_stopped( vsf_tgui_list_t* ptList,
                                            vsf_tgui_refresh_evt_t* event_ptr);
#endif
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

describe_tgui_msgmap(tStartStopMSGMap,
    tgui_msg_handler(VSF_TGUI_EVT_POINTER_CLICK,        __on_button_start_stop_click),
    tgui_msg_handler(VSF_TGUI_EVT_KEY_PRESSED,          __on_button_start_stop_ok),
)

#if VSF_TGUI_CFG_SUPPORT_TEXT_LIST == ENABLED
describe_tgui_msgmap(tTextListMGSMap,
    tgui_msg_handler(VSF_TGUI_EVT_POST_REFRESH,         __on_text_list_post_refresh),
)
#endif

#if VSF_TGUI_CFG_SUPPORT_LIST == ENABLED
describe_tgui_msgmap(tListMGSMap,
    tgui_msg_handler(VSF_TGUI_EVT_POST_REFRESH,         __on_list_post_refresh),
    tgui_msg_handler(VSF_TGUI_EVT_LIST_SLIDING_STARTED, __on_list_sliding_started),
    tgui_msg_handler(VSF_TGUI_EVT_LIST_SLIDING_STOPPED, __on_list_sliding_stopped),
)
#endif

describe_tgui_msgmap(tStopWatchMSGMap,
    tgui_msg_handler(VSF_TGUI_EVT_ON_LOAD,              __on_top_panel_load),
    tgui_msg_handler(VSF_TGUI_EVT_ON_DEPOSE,            __on_top_panel_depose),
#if VSF_TGUI_CFG_SUPPORT_TIMER == ENABLED
    tgui_msg_handler(VSF_TGUI_EVT_ON_TIME,              __on_top_panel_time),
#endif
)

describe_tgui_msgmap(tLapMSGMap,
    tgui_msg_mux(VSF_TGUI_MSG_POINTER_EVT, __on_button_lap_all_pointer_evt, VSF_TGUI_MSG_MSK),
)

stopwatch_t* my_stopwatch_init(stopwatch_t* ptPanel, vsf_tgui_t *gui_ptr)
{
    do {
        if (NULL == ptPanel && NULL != gui_ptr) {
            break;
        }

        describ_tgui_panel(stopwatch_t, *ptPanel,
            tgui_region(
                tgui_location(100, 100),
            ),
            tgui_text(tTitle, "My Stopwatch", false),
            tgui_padding(16,16,16,16),

            tgui_msgmap(tStopWatchMSGMap),

        #if VSF_TGUI_CFG_SUPPORT_TIMER == ENABLED
            tgui_timer(tTimer, 97, false),
        #endif

            tgui_label(tTime, ptPanel, tTime, tStartStop,
                tgui_region(0, 48, 228, 32),
                tgui_text(tLabel, ptPanel->chTimeBuffer, false),
                tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S20),
                tgui_background(&ic_settings_phone_RGBA, VSF_TGUI_ALIGN_LEFT),
            ),

            tgui_button(tStartStop, ptPanel, tTime, tLap,
                tgui_location(0, 94),
                tgui_text(tLabel, "START", true),
                tgui_attribute(bIsCheckButton, true),

                tgui_msgmap(tStartStopMSGMap),
            ),

            tgui_button(tLap, ptPanel, tStartStop, tSetting,
                tgui_region(300 - 64 - 16 - 16, 94, 64, 32),
                tgui_text(tLabel, "LAP", false),
                tgui_msgmap(tLapMSGMap),
            ),

            tgui_button(tSetting, ptPanel, tLap, tContainerA,
                tgui_region(300 - 16 - 32 - 16, 48, 32, 32),
                tgui_background(&ic_build_black_18dp_RGBA, VSF_TGUI_ALIGN_CENTER),
            ),

            tgui_container(tContainerA, ptPanel, tSetting, tContainerA,

                tgui_location(0, 150),
#if VSF_TGUI_CFG_SUPPORT_TEXT_LIST == ENABLED && VSF_TGUI_CFG_SUPPORT_LIST == ENABLED
                tgui_container_type(VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_HORIZONTAL),
#else
                tgui_container_type(VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL),
#endif

                tgui_contains(

#if VSF_TGUI_CFG_SUPPORT_TEXT_LIST == ENABLED
                    tgui_text_list(tNumberList, &(ptPanel->tContainerA), tNumberList, tVContainer,
                        tgui_size(100, 100),
                        tgui_margin(8, 0, 8, 0),
                    #if VSF_TGUI_CFG_TEXT_LIST_SUPPORT_SLIDE == ENABELD
                        //tgui_attribute(tSlider, 400),
                    #endif
                        tgui_msgmap(tTextListMGSMap),

                        tgui_text_list_content(

                            tgui_size(100, 0),

                            /*! \note as inside text_list, the autosize param will be overrided,
                             *        so the true or false here doesn't really affect the result.
                             */
                            tgui_text(  tLabel,
                                        "0\n1\n2\n3\n4\n5\n6\n7\n8\n9",
                                        true),
                            tgui_line_space(tLabel, 8),
                            tgui_attribute(tFontColor, VSF_TGUI_COLOR_GRAY),
                        )
                    ),

#   if VSF_TGUI_CFG_SUPPORT_LIST == ENABLED
                    tgui_list(tVContainer, &(ptPanel->tContainerA), tNumberList, tVContainer,
#   endif
#else
#   if VSF_TGUI_CFG_SUPPORT_LIST == ENABLED
                    tgui_list(tVContainer, &(ptPanel->tContainerA), tVContainer, tVContainer,
#   endif
#endif
#if VSF_TGUI_CFG_SUPPORT_LIST == ENABLED
                        tgui_size(160, 130),
                        //tgui_attribute(u2WorkMode, VSF_TGUI_LIST_MODE_ITEM_SELECTION),
                        tgui_padding(0,0,10,0),
                        tgui_msgmap(tListMGSMap),

                        tgui_list_items(
                            tgui_container_type(VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL),

                            tgui_button(tButton1, &(ptPanel->tContainerA.tVContainer.list), tButton1, tButton2,
                                tgui_size(150, 32),
                                tgui_text(tLabel, "tButton1", false),
                                   tgui_attribute(tFontColor, VSF_TGUI_COLOR_RGBA(0x80, 0x80, 0x00, 0x30)),
                                   tgui_margin(0, 2, 0, 2),
                            ),

                            tgui_button(tButton2, &(ptPanel->tContainerA.tVContainer.list), tButton1, tHContainer,
                                tgui_size(150, 32),
                                tgui_text(tLabel, "tButton2", false),
                                tgui_margin(0, 2, 0, 2),
                            ),

                            tgui_list(tHContainer, &(ptPanel->tContainerA.tVContainer.list), tButton2, tHistory,
                                tgui_size(150, 32),
                                tgui_margin(0, 2, 0, 2),
                            #if VSF_TGUI_CFG_LIST_SUPPORT_SLIDE == ENABELD
                                //tgui_attribute(tSlider, 500),
                            #endif
                                tgui_attribute(u2WorkMode, VSF_TGUI_LIST_MODE_ITEM_SELECTION),

                                tgui_list_items(
                                    tgui_container_type(VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_HORIZONTAL),

                                    tgui_button(tButtonA, &(ptPanel->tContainerA.tVContainer.list.tHContainer.list), tButtonA, tButtonB,
                                        tgui_size(80, 32),
                                        tgui_text(tLabel, "A", false),
                                        tgui_margin(2, 0, 2, 0),
                                    ),
                                    tgui_button(tButtonB, &(ptPanel->tContainerA.tVContainer.list.tHContainer.list), tButtonA, tButtonC,
                                        tgui_size(80, 32),
                                        tgui_text(tLabel, "B", false),
                                        tgui_margin(2, 0, 2, 0),
                                    ),
                                    tgui_button(tButtonC, &(ptPanel->tContainerA.tVContainer.list.tHContainer.list), tButtonB, tButtonC,
                                        tgui_size(80, 32),
                                        tgui_text(tLabel, "C", false),
                                        tgui_margin(2, 0, 2, 0),
                                    ),
                                )
                            ),

                            tgui_label(tHistory, &(ptPanel->tContainerA.tVContainer.list), tHContainer, tHistory,
                                tgui_text(tLabel, "tHistory\n1234\nABCDEF", false),
                                tgui_size(150, 128),
                                tgui_attribute(bIsUseRawView, true),
                                tgui_attribute(tFontColor, VSF_TGUI_COLOR_GRAY),
                                tgui_margin(0, 2, 0, 2),
                            ),
                        )
                    ),
#else
                    tgui_button(tButton1, &(ptPanel->tContainerA), tButton1, tButton2,
                        tgui_size(150, 32),
                        tgui_text(tLabel, "tButton1", false),
                        tgui_attribute(tFontColor, VSF_TGUI_COLOR_RGBA(0x80, 0x80, 0x00, 0x30)),
                        tgui_margin(0, 2, 0, 2),
                    ),

                    tgui_button(tButton2, &(ptPanel->tContainerA), tButton1, tHistory,

                    #if VSF_TGUI_CFG_SUPPORT_LINE_STREAM_CONTAINER != ENABLED
                        tgui_location(50, 42),
                    #endif
                        tgui_size(150, 32),
                        tgui_text(tLabel, "tButton2", false),
                        tgui_margin(0, 2, 0, 2),
                    ),

                    tgui_label(tHistory, &(ptPanel->tContainerA), tButton2, tHistory,
                    #if VSF_TGUI_CFG_SUPPORT_LINE_STREAM_CONTAINER != ENABLED
                        tgui_location(100, 84),
                    #endif

                        tgui_text(tLabel, "tHistory\n1234\nABCDEF", true),
                        tgui_size(150, 128),
                        tgui_attribute(bIsUseRawView, true),
                        tgui_attribute(tFontColor, VSF_TGUI_COLOR_GRAY),
                        tgui_margin(0, 2, 0, 2),
                    ),
#endif
                )
            ),
        );
    } while (0);
    return ptPanel;
}

static fsm_rt_t __on_top_panel_load(vsf_tgui_control_t* node_ptr,
                                    vsf_msgt_msg_t* ptMSG)
{
    stopwatch_t *ptPanel = (stopwatch_t *)node_ptr;

    UNUSED_PARAM(ptPanel);
#if VSF_TGUI_CFG_SUPPORT_TIMER == ENABLED
    vsf_tgui_timer_init(&ptPanel->tTimer,
                        (const vsf_tgui_control_t *)ptPanel);
#endif
    //vsf_tgui_text_list_select_set(&(ptPanel->tContainerA.tNumberList), 6);

    //init_vsf_pt(tgui_demo_t, &(ptPanel->tTask), vsf_prio_0);
    vsf_tgui_control_refresh(node_ptr, NULL);
    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}

#if VSF_TGUI_CFG_SUPPORT_TIMER == ENABLED
static fsm_rt_t __on_top_panel_time(vsf_tgui_control_t* node_ptr,
                                    vsf_msgt_msg_t* ptMSG)
{
    stopwatch_t *ptPanel = (stopwatch_t *)node_ptr;

    vsf_systimer_cnt_t tTimeElapsedInMS = vsf_systimer_tick_to_ms( vsf_systimer_get_tick());
    uint8_t ch10Ms = (tTimeElapsedInMS / 10) % 100;
    uint8_t chSecond = (tTimeElapsedInMS / 1000) % 60;
    uint8_t chMinute = (tTimeElapsedInMS / 60000) % 60;
    sprintf(ptPanel->chTimeBuffer, "%02d:%02d:%02d",  chMinute, chSecond, ch10Ms);

    //! update existing text content
    vsf_tgui_text_set(&(ptPanel->tTime.tLabel), &(ptPanel->tTime.tLabel.tString));

    vk_tgui_refresh_ex(ptPanel->use_as__vsf_tgui_panel_t.gui_ptr,
                        (vsf_tgui_control_t *)&(ptPanel->tTime), NULL);

    vsf_tgui_timer_enable(&ptPanel->tTimer);

    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}
#endif

static fsm_rt_t __on_top_panel_depose(vsf_tgui_control_t* node_ptr,
                                    vsf_msgt_msg_t* ptMSG)
{
    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}

static fsm_rt_t __on_button_start_stop_click(   vsf_tgui_control_t* node_ptr,
                                                vsf_msgt_msg_t* ptMSG)
{
    VSF_TGUI_LOG(VSF_TRACE_WARNING, "\tCall User Handler\r\n");

    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}

static fsm_rt_t __on_button_start_stop_ok(  vsf_tgui_control_t* node_ptr,
                                            vsf_msgt_msg_t* ptMSG)
{
    VSF_TGUI_LOG(VSF_TRACE_WARNING, "\tCall User Handler\r\n");
    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}

static fsm_rt_t __on_button_lap_all_pointer_evt(vsf_tgui_control_t* node_ptr,
                                                vsf_msgt_msg_t* ptMSG)
{
    VSF_TGUI_LOG(VSF_TRACE_WARNING, "\tCall User Handler\r\n");
    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}


#if VSF_TGUI_CFG_SUPPORT_TEXT_LIST == ENABLED
static fsm_rt_t __on_text_list_post_refresh(vsf_tgui_control_t* control_ptr,
                                            vsf_msgt_msg_t* ptMSG)
{
    vsf_tgui_refresh_evt_t *event_ptr = (vsf_tgui_refresh_evt_t *)ptMSG;
    const vsf_tgui_region_t *ptDirtyRegion = (const vsf_tgui_region_t *)(event_ptr->region_ptr);

    do {
        vsf_tgui_sv_color_t tColor = VSF_TGUI_CFG_SV_TEXT_LIST_INDICATOR_COLOR;
        vsf_tgui_region_t tRegion = {0};

        tRegion.tSize = *vsf_tgui_control_get_size(control_ptr);

        tRegion.tLocation.iY = tRegion.tSize.iHeight / 2 - 1;
        tRegion.tLocation.iX = 4;
        tRegion.tSize.iHeight = 2;
        tRegion.tSize.iWidth -= 8;

        vsf_tgui_control_v_draw_rect(   control_ptr,
                                        ptDirtyRegion,
                                        &tRegion,
                                        tColor);
    } while(0);

    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}
#endif

#if VSF_TGUI_CFG_SUPPORT_LIST == ENABLED
static volatile bool s_bShowProgressbar = false;

static fsm_rt_t __on_list_sliding_started(  vsf_tgui_list_t* ptList,
                                            vsf_tgui_refresh_evt_t* event_ptr)
{
    s_bShowProgressbar = true;
    return fsm_rt_cpl;
}

static fsm_rt_t __on_list_sliding_stopped(  vsf_tgui_list_t* ptList,
                                            vsf_tgui_refresh_evt_t* event_ptr)
{
    s_bShowProgressbar = false;
    return fsm_rt_cpl;
}

static fsm_rt_t __on_list_post_refresh( vsf_tgui_list_t* ptList,
                                        vsf_tgui_refresh_evt_t* event_ptr)
{

#if VSF_TGUI_CFG_LIST_SUPPORT_SCROOLBAR == ENABLED

#define VSF_TGUI_SCROLLBAR_TRACK_PIECE_COLOR_WHITE      VSF_TGUI_COLOR_RGBA(0xff, 0xff, 0xff, 0x24)
#define VSF_TGUI_SCROLLBAR_COLOR_BLUE                   VSF_TGUI_COLOR_RGBA(0x4c, 0xa1, 0xff, 0x80)

    vsf_tgui_list_scrollbar_region_t tScrollbarRegion;
    vsf_tgui_region_t *ptDirtyRegion = event_ptr->region_ptr;

    if (!s_bShowProgressbar) {
        return fsm_rt_cpl;
    }

    if (NULL == vsf_tgui_list_scrollbar_regions_generate(   (const vsf_tgui_control_t *)ptList,
                                                            ptList,
                                                            2,
                                                            &tScrollbarRegion)) {
        return fsm_rt_cpl;
    }

    vsf_tgui_control_v_draw_rect(   (const vsf_tgui_control_t *)ptList,
                                    ptDirtyRegion,
                                    &tScrollbarRegion.tTrack,
                                    VSF_TGUI_SCROLLBAR_TRACK_PIECE_COLOR_WHITE);

    vsf_tgui_control_v_draw_rect(   (const vsf_tgui_control_t *)ptList,
                                    ptDirtyRegion,
                                    &tScrollbarRegion.tBar,
                                    VSF_TGUI_SCROLLBAR_COLOR_BLUE);
#endif

    return fsm_rt_cpl;
}

#endif

#if 0
static vsf_tgui_region_t s_tRefreshRegion = {
                            tgui_location(104, 100),
                            tgui_size(300, 80),
                        };


implement_vsf_pt(tgui_demo_t)
{
    stopwatch_t *ptBase = (stopwatch_t*)container_of(this_ptr, stopwatch_t, tTask.param);
    vsf_pt_begin();

    UNUSED_PARAM(s_tRefreshRegion);

    //vk_tgui_refresh_ex(ptBase->use_as__vsf_tgui_panel_t.gui_ptr, NULL, &s_tRefreshRegion);
    vk_tgui_refresh(ptBase->use_as__vsf_tgui_panel_t.gui_ptr);
    while(1) {

        //! refresh timer
        do {
            vsf_systimer_cnt_t tTimeElapsedInMS = vsf_systimer_tick_to_ms( vsf_systimer_get_tick());
            uint8_t ch10Ms = (tTimeElapsedInMS / 10) % 100;
            uint8_t chSecond = (tTimeElapsedInMS / 1000) % 60;
            uint8_t chMinute = (tTimeElapsedInMS / 60000) % 60;
            sprintf(base.chTimeBuffer, "%02d:%02d:%02d",  chMinute, chSecond, ch10Ms);

            //! update existing text content
            vsf_tgui_text_set(&(base.tTime.tLabel), &(base.tTime.tLabel.tString));

            vk_tgui_refresh_ex(base.use_as__vsf_tgui_panel_t.gui_ptr,
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

