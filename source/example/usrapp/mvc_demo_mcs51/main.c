/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless requir by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct usrapp_t {
#if VSF_USE_UI == ENABLED && VSF_USE_TINY_GUI == ENABLED
    struct {
        vk_disp_t disp;
        //vsf_touchscreen_evt_t ts_evt;
        //vsf_tgui_color_t color[VSF_TGUI_VER_MAX][VSF_TGUI_HOR_MAX];
        vsf_tgui_color_t color;
    } ui;
#endif
    uint8_t xxx;
};
typedef struct usrapp_t usrapp_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static usrapp_t usrapp = {
#if VSF_USE_UI == ENABLED && VSF_USE_TINY_GUI == ENABLED 
    .ui.disp = {
        .param                  = {
            .height             = VSF_TGUI_VER_MAX,
            .width              = VSF_TGUI_HOR_MAX,
            //.drv                = &vsf_disp_drv_sdl2,
            .color              = VSF_DISP_COLOR_ARGB8888,
        },
        //.amplifier              = 1,
    },
#endif
    .xxx = 9,
};

/*============================ PROTOTYPES ====================================*/
extern void refresh_my_stopwatch(void);
/*============================ IMPLEMENTATION ================================*/

#if VSF_USE_UI == ENABLED
#if 0
extern void vsf_tgui_on_touchscreen_evt(vsf_touchscreen_evt_t* ts_evt);
void vsf_input_on_touchscreen(vsf_touchscreen_evt_t *ts_evt)
{
    if (ts_evt->dev == &usrapp.ui.disp) {
        vsf_tgui_on_touchscreen_evt(ts_evt);
    }
}
#endif
#endif

int main(void)
{
    vsf_trace_init(NULL);

#if VSF_USE_UI == ENABLED && VSF_USE_TINY_GUI == ENABLED

    extern vsf_err_t tgui_demo_init(void);
    tgui_demo_init();

	extern void vsf_tgui_bind(vk_disp_t * disp, void* ui_data);
	vsf_tgui_bind(&(usrapp.ui.disp), &usrapp.ui.color);
    
    while(1) {
        refresh_my_stopwatch();
    }
#endif
    return 0;
}

/* EOF */
