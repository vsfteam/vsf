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

#if VSF_USE_UI == ENABLED && VSF_USE_TINY_GUI == ENABLED
/*============================ MACROS ========================================*/
#ifndef VSF_TGUI_CFG_SV_PORT_COLOR_BUFFER_SIZE
#   define VSF_TGUI_CFG_SV_PORT_COLOR_BUFFER_SIZE \
              (VSF_TGUI_HOR_MAX * VSF_TGUI_VER_MAX / 10)
#endif
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct usrapp_t {
    struct {
        vk_disp_sdl2_t disp;
        vsf_tgui_color_t color[VSF_TGUI_CFG_SV_PORT_COLOR_BUFFER_SIZE];
    } ui;
};
typedef struct usrapp_t usrapp_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static usrapp_t __usrapp = {
#if VSF_USE_UI == ENABLED && VSF_USE_TINY_GUI == ENABLED
    .ui.disp                    = {
        .param                  = {
            .height             = VSF_TGUI_VER_MAX,
            .width              = VSF_TGUI_HOR_MAX,
            .drv                = &vk_disp_drv_sdl2,

        #if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_ARGB_8888
            .color              = VSF_DISP_COLOR_ARGB8888,
        #elif VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_RGB16_565
            .color              = VSF_DISP_COLOR_RGB565,
        #endif
        },
        .amplifier              = 1,
    },
#endif
};

/*============================ PROTOTYPES ====================================*/
extern void vsf_tgui_bind_disp(vk_disp_t* disp, void* bitmap_data, size_t bitmap_size);
extern vsf_err_t tgui_demo_init(void);
/*============================ IMPLEMENTATION ================================*/

extern void vsf_tgui_on_touchscreen_evt(vk_touchscreen_evt_t* ts_evt);
void vsf_input_on_touchscreen(vk_touchscreen_evt_t *ts_evt)
{
    if (ts_evt->dev == &__usrapp.ui.disp) {
        vsf_tgui_on_touchscreen_evt(ts_evt);
    }
}

extern void vsf_tgui_on_keyboard_evt(vk_keyboard_evt_t* keyboard_evt);
void vsf_input_on_keyboard(vk_keyboard_evt_t* keyboard_evt)
{
    if (keyboard_evt->dev == &__usrapp.ui.disp) {
        vsf_tgui_on_keyboard_evt(keyboard_evt);
    }
}

#if VSF_TGUI_CFG_SUPPORT_MOUSE == ENABLED
extern void vsf_tgui_on_mouse_evt(vk_mouse_evt_t *mouse_evt);
void vsf_input_on_mouse(vk_mouse_evt_t *mouse_evt)
{
    if (mouse_evt->dev == &__usrapp.ui.disp) {
        vsf_tgui_on_mouse_evt(mouse_evt);
    }

}
#endif


// TODO: SDL require that main need argc and argv
int main(int argc, char *argv[])
{
    vsf_trace_init((vsf_stream_t *)&VSF_DEBUG_STREAM_TX);
    vsf_stdio_init();

    vsf_tgui_bind_disp(&(__usrapp.ui.disp.use_as__vk_disp_t), &__usrapp.ui.color, dimof(__usrapp.ui.color));

    tgui_demo_init();
    return 0;
}
#endif

/* EOF */
