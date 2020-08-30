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
#include "./stopwatch.h"

#if VSF_USE_TINY_GUI == ENABLED
#include <stdio.h>
#include "../images/demo_images.h"

/*============================ MACROS ========================================*/
#undef base
#define base        (*ptBase)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
extern bool vsf_tgui_port_is_ready_to_refresh(void);

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

stopwatch_t* my_stopwatch_init(stopwatch_t* ptPanel, vsf_tgui_t *gui_ptr)
{
    do {
        if (NULL == ptPanel && NULL != gui_ptr) {
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
                tgui_background(&ic_settings_phone_RGBA, VSF_TGUI_ALIGN_LEFT),
            ),

            tgui_button(tStartStop, ptPanel, tTime, tLap,
                tgui_region(0, 94, 100, 32),
                tgui_text(tLabel, "START", VSF_TGUI_ALIGN_CENTER),
                tgui_attribute(bIsCheckButton, true),
            ),

            tgui_button(tLap, ptPanel, tStartStop, tSetting,
                tgui_region(300 - 64 - 16 - 16, 94, 64, 32),
                tgui_text(tLabel, "LAP"),
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
                        tgui_margin(0, 2, 0, 2),
                    ),
                )
            ),
            
        );


        ptPanel->use_as__vsf_tgui_panel_t.gui_ptr = gui_ptr;
#if 0
        //! following code show the right initialisation sequence
        do {
            vsf_tgui_label_v_init(&(ptPanel->tTime));
            vsf_tgui_button_v_init(&(ptPanel->tStartStop));
            vsf_tgui_button_v_init(&(ptPanel->tLap));
            vsf_tgui_button_v_init(&(ptPanel->tSetting));

            do {
                vsf_tgui_label_v_init(&(ptPanel->tVContainer.list.tHistory));
                vsf_tgui_button_v_init(&(ptPanel->tVContainer.list.tButton1));
                vsf_tgui_button_v_init(&(ptPanel->tVContainer.list.tButton2));

                do {
                    vsf_tgui_button_v_init(&(ptPanel->tVContainer.list.tHContainer.list.tButtonA));
                    vsf_tgui_button_v_init(&(ptPanel->tVContainer.list.tHContainer.list.tButtonB));
                    vsf_tgui_button_v_init(&(ptPanel->tVContainer.list.tHContainer.list.tButtonC));
                    vsf_tgui_list_v_init(&(ptPanel->tVContainer.list.tHContainer.use_as__vsf_tgui_list_t));
                } while(0);

                vsf_tgui_list_v_init(&(ptPanel->tVContainer.use_as__vsf_tgui_list_t));
            } while(0);

            vsf_tgui_panel_v_init(&(ptPanel->use_as__vsf_tgui_panel_t));
        } while(0);
#endif

        ptPanel->tTask.param.bWaitforRefresh = true;
        init_vsf_pt(tgui_demo_t, &(ptPanel->tTask), vsf_prio_0);
    } while (0);
    return ptPanel;
}

static vsf_tgui_region_t s_tRefreshRegion = {
                            tgui_location(104, 100),
                            tgui_size(300, 80),
                        };

implement_vsf_pt(tgui_demo_t)
{
    stopwatch_t *ptBase = (stopwatch_t*)container_of(this_ptr, stopwatch_t, tTask.param);
    vsf_pt_begin();

    //vk_tgui_refresh_ex(ptBase->use_as__vsf_tgui_panel_t.gui_ptr, NULL, &s_tRefreshRegion);
    vk_tgui_refresh(ptBase->use_as__vsf_tgui_panel_t.gui_ptr);
    while(1) {
        //! refresh timer
        do {
            base.tHistory++;
            sprintf(base.chTimeBuffer, "%02d:%02d", base.tHistory / 99 % 99, base.tHistory % 99);
            vk_tgui_refresh_ex(base.use_as__vsf_tgui_panel_t.gui_ptr, 
                                (vsf_tgui_control_t *)&(base.tTime), NULL);
                                //&s_tRefreshRegion);
        } while(0);
        vsf_pt_wait_until(vsf_delay_ms(1000));
    }

    vsf_pt_end();
}

#endif


/* EOF */
