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

#ifndef __STOPWATCH_H__
#define __STOPWATCH_H__

#if VSF_USE_TINY_GUI == ENABLED
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
#if 0
declare_vsf_pt(tgui_demo_t)
def_vsf_pt(tgui_demo_t,
    def_params(
    )
)
end_def_vsf_pt(tgui_demo_t)
#endif

declare_tgui_panel(stopwatch_t)

def_tgui_panel(stopwatch_t,
    tgui_contains(

        use_tgui_container(tLeftContainer,
            tgui_contains(
                vsf_tgui_label_t     tTime;
                vsf_tgui_button_t    tStartStop;
                vsf_tgui_button_t    tLap;
                vsf_tgui_button_t    tSetting;

                use_tgui_container(tContainerA,
                    tgui_contains(
                    #if VSF_TGUI_CFG_SUPPORT_TEXT_LIST == ENABLED
                        vsf_tgui_text_list_t  tNumberList;
                    #endif

                    #if VSF_TGUI_CFG_SUPPORT_LIST == ENABLED
                        use_tgui_list(tVContainer,
                            tgui_contains(

                                vsf_tgui_button_t    tButton1;
                                vsf_tgui_button_t    tButton2;

                                use_tgui_list(tHContainer,
                                    tgui_contains(
                                        vsf_tgui_button_t    tButtonA;
                                        vsf_tgui_button_t    tButtonB;
                                        vsf_tgui_button_t    tButtonC;
                                    )
                                )

                                vsf_tgui_label_t     tHistory;
                            )
                        )
                    #else
                        vsf_tgui_button_t    tButton1;
                        vsf_tgui_button_t    tButton2;
                        vsf_tgui_label_t     tHistory;
                    #endif
                    )
                )
        ))

        use_tgui_panel(tRightPanel,
            tgui_contains(
                vsf_tgui_button_t tKey[10];
            )
        )

        #if VSF_TGUI_CFG_SUPPORT_TIMER == ENABLED
            vsf_tgui_timer_t tTimer;
        #endif

	))

    char        chTimeBuffer[sizeof("00:00:00")];

    //tgui_demo_t tTask;

end_def_tgui_panel(stopwatch_t)

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

extern
stopwatch_t* my_stopwatch_init(stopwatch_t* ptPanel, vsf_tgui_t *gui_ptr);

#endif
#endif


/* EOF */
