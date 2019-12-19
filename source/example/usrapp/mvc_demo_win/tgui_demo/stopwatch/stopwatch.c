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
                tgui_size(300, 80),
                ),
            tgui_text(tTitle, "My Stopwatch"),

            tgui_label(time, ptPanel, 0, false,
                tgui_region(16, 64, 228, 32),
                tgui_text(tLabel, ptPanel->chTimeBuffer),
                tgui_background(&ic_settings_phone_RGBA, VSF_TGUI_ALIGN_LEFT),
                ),

            tgui_button(start_stop, ptPanel, sizeof(vsf_tgui_label_t), false,
                tgui_region(16, 110, 100, 32),
                tgui_text(tLabel, "START", VSF_TGUI_ALIGN_CENTER),
                ),

            tgui_button(lap, ptPanel, sizeof(vsf_tgui_button_t), false,
                tgui_region(300 - 64 - 16, 110, 64, 32),
                tgui_text(tLabel, "LAP"),
                ),

            tgui_button(Setting, ptPanel, sizeof(vsf_tgui_button_t), false,
                tgui_region(300 - 16 - 32, 64, 32, 32),
                tgui_background(&ic_build_black_18dp_RGBA, VSF_TGUI_ALIGN_CENTER),
                ),

            tgui_label(history, ptPanel, sizeof(vsf_tgui_button_t), true,
                tgui_text(tLabel, "\nabdcdef\nABCDEF"),
                tgui_region(16, 150, 300 - 32, 128),
                tgui_attribute(bIsUseRawView, true),
                ),
        );


        ptPanel->use_as__vsf_tgui_panel_t.ptGUI = ptGUI;

        vsf_tgui_panel_v_init(  &(ptPanel->use_as__vsf_tgui_panel_t));
        vsf_tgui_label_v_init(  &(ptPanel->time));
        vsf_tgui_button_v_init( &(ptPanel->start_stop));
        vsf_tgui_button_v_init( &(ptPanel->lap));
        vsf_tgui_button_v_init( &(ptPanel->Setting));
        vsf_tgui_label_v_init(  &(ptPanel->history));
        
        ptPanel->task.param.bWaitforRefresh = true;
        init_vsf_pt(tgui_demo_t, &(ptPanel->task), vsf_prio_0);
    } while (0);
    return ptPanel;
}

implement_vsf_pt(tgui_demo_t)
{
    stopwatch_t *ptBase = (stopwatch_t*)container_of(ptThis, stopwatch_t, task.param);
    vsf_pt_begin();

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
                                (vsf_tgui_control_t *)&(base.time), 
                                NULL);
        } while(0);
        vsf_pt_wait_until(vsf_delay_ms(1000));
    }

    vsf_pt_end();
}

#endif


/* EOF */
