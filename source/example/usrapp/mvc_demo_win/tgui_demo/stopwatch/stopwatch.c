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
extern void refresh_all(void);
/*============================ GLOBAL VARIABLES ==============================*/

#if 0
implement_tgui_panel(stopwatch_t, __my_stopwatch,

    tgui_region(
        tgui_location(100, 100),
        tgui_size(300, 300),
    ),
    tgui_text(tTitle, "My Stopwatch"),

    tgui_label(time, &__my_stopwatch, 0, false,
        tgui_region(16, 64, 228, 32),
        tgui_text(tLabel, __my_stopwatch.chTimeBuffer),
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
#endif

/*============================ IMPLEMENTATION ================================*/

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

            tgui_label(time, ptPanel, 0, false,
                tgui_region(0, 48, 228, 32),
                tgui_text(tLabel, ptPanel->chTimeBuffer),
                tgui_background(&ic_settings_phone_RGBA, VSF_TGUI_ALIGN_LEFT),
            ),

            tgui_button(start_stop, ptPanel, sizeof(vsf_tgui_label_t), false,
                tgui_region(0, 94, 100, 32),
                tgui_text(tLabel, "START", VSF_TGUI_ALIGN_CENTER),
            ),

            tgui_button(lap, ptPanel, sizeof(vsf_tgui_button_t), false,
                tgui_region(300 - 64 - 16 - 16, 94, 64, 32),
                tgui_text(tLabel, "LAP"),
            ),

            tgui_button(Setting, ptPanel, sizeof(vsf_tgui_button_t), false,
                tgui_region(300 - 16 - 32 - 16, 48, 32, 32),
                tgui_background(&ic_build_black_18dp_RGBA, VSF_TGUI_ALIGN_CENTER),
            ),

            tgui_container(vcontainer, ptPanel, sizeof(vsf_tgui_button_t), true,

                tgui_location(0, 150),
                tgui_attribute(bIsAutoSize, true),
                tgui_container_type(VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL),
                tgui_padding(8, 8, 8, 8),

                tgui_label(history, &(ptPanel->vcontainer), 0, false,
                    tgui_text(tLabel, "history\n\nabdcdef\nABCDEF"),
                    tgui_size(300, 128),
                    tgui_attribute(bIsUseRawView, true),
                    tgui_margin(0, 2, 0, 2),
                ),

                tgui_button(button1, &(ptPanel->vcontainer), sizeof(vsf_tgui_label_t), false,
                    tgui_size(300, 32),
                    tgui_text(tLabel, "button1"),
                    tgui_margin(0, 2, 0, 2),
                ),

                tgui_button(button2, &(ptPanel->vcontainer), sizeof(vsf_tgui_button_t), false,
                    tgui_size(300, 32),
                    tgui_text(tLabel, "button2"),
                    tgui_margin(0, 2, 0, 2),
                ),

                tgui_container(hcontainer, &(ptPanel->vcontainer), sizeof(vsf_tgui_button_t), true,

                    tgui_attribute(bIsAutoSize, true),
                    tgui_container_type(VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_HORIZONTAL),
                    tgui_padding(8, 8, 8, 8),
                    tgui_margin(0, 2, 0, 2),

                    tgui_button(buttonA, &(ptPanel->vcontainer.hcontainer), 0, false,
                        tgui_size(80, 32),
                        tgui_text(tLabel, "A"),
                        tgui_margin(2, 0, 2, 0),
                    ),

                    tgui_button(buttonB, &(ptPanel->vcontainer.hcontainer), sizeof(vsf_tgui_button_t), false,
                        tgui_size(80, 32),
                        tgui_text(tLabel, "B"),
                        tgui_margin(2, 0, 2, 0),
                    ),

                    tgui_button(buttonC, &(ptPanel->vcontainer.hcontainer), sizeof(vsf_tgui_button_t), true,
                        tgui_size(80, 32),
                        tgui_text(tLabel, "C"),
                        tgui_margin(2, 0, 2, 0),
                    ),
                )
            ),
            
        );


        ptPanel->use_as__vsf_tgui_panel_t.ptGUI = ptGUI;

        //! following code show the right initialisation sequence
        do {
            vsf_tgui_label_v_init(&(ptPanel->time));
            vsf_tgui_button_v_init(&(ptPanel->start_stop));
            vsf_tgui_button_v_init(&(ptPanel->lap));
            vsf_tgui_button_v_init(&(ptPanel->Setting));

            do {
                vsf_tgui_label_v_init(&(ptPanel->vcontainer.history));
                vsf_tgui_button_v_init(&(ptPanel->vcontainer.button1));
                vsf_tgui_button_v_init(&(ptPanel->vcontainer.button2));

                do {
                    vsf_tgui_button_v_init(&(ptPanel->vcontainer.hcontainer.buttonA));
                    vsf_tgui_button_v_init(&(ptPanel->vcontainer.hcontainer.buttonB));
                    vsf_tgui_button_v_init(&(ptPanel->vcontainer.hcontainer.buttonC));
                    vsf_tgui_container_v_init(&(ptPanel->vcontainer.hcontainer.use_as__vsf_tgui_container_t));
                } while(0);

                vsf_tgui_container_v_init(&(ptPanel->vcontainer.use_as__vsf_tgui_container_t));        
            } while(0);

            vsf_tgui_panel_v_init(      &(ptPanel->use_as__vsf_tgui_panel_t));
        } while(0);
        ptPanel->task.param.bWaitforRefresh = true;
        init_vsf_pt(tgui_demo_t, &(ptPanel->task), vsf_prio_0);
    } while (0);
    return ptPanel;
}

static vsf_tgui_region_t __refresh_region = {
                            tgui_location(104, 100),
                            tgui_size(300, 80),
                        };

implement_vsf_pt(tgui_demo_t)
{
    stopwatch_t *ptBase = (stopwatch_t*)container_of(ptThis, stopwatch_t, task.param);
    vsf_pt_begin();

    //vk_tgui_refresh_ex(ptBase->use_as__vsf_tgui_panel_t.ptGUI, NULL, &__refresh_region);
    vk_tgui_refresh(ptBase->use_as__vsf_tgui_panel_t.ptGUI);
    while(1) {
            
        do {
            bool bReadyToRefresh = false;
            vsf_sched_safe() {
                bReadyToRefresh = vsf_tgui_port_is_ready_to_refresh();
                if (!bReadyToRefresh) {
                    this.bWaitforRefresh = true;
                } 
            }
            if (!bReadyToRefresh) {
                vsf_pt_wait_for_evt(VSF_EVT_USER);
                this.bWaitforRefresh = false;
            } else {
                this.bWaitforRefresh = false;
                break;
            }
        } while(true);
        
        //! refresh timer
        do {
            base.cnt++;
            sprintf(base.chTimeBuffer, "%02d:%02d", base.cnt / 99 % 99, base.cnt % 99);
            vk_tgui_refresh_ex(base.use_as__vsf_tgui_panel_t.ptGUI, 
                                (vsf_tgui_control_t *)&(base.time), NULL);
                                //&__refresh_region);
        } while(0);
        vsf_pt_wait_until(vsf_delay_ms(1000));
    }

    vsf_pt_end();
}

#endif


/* EOF */
