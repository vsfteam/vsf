/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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

#if VSF_USE_UI == ENABLED && VSF_USE_TINY_GUI == ENABLED && APP_USE_TGUI_DEMO == ENABLED

#include "../common/usrapp_common.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_tgui_t * tgui_demo_init(void);

/*============================ IMPLEMENTATION ================================*/

#if APP_USE_LINUX_DEMO == ENABLED
int tgui_main(int argc, char *argv[])
{
#else
int VSF_USER_ENTRY(void)
{
#   if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#       if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#       endif
#   endif

    extern void freetype_demo_init(void);
    freetype_demo_init();
#endif

    vsf_tgui_fonts_init((vsf_tgui_font_t *)vsf_tgui_font_get(0), vsf_tgui_font_number(), "font/");

    vsf_tgui_t *tgui = tgui_demo_init();
    if (tgui != NULL) {
        vsf_tgui_sv_bind_disp(tgui, usrapp_ui_common.disp, &usrapp_ui_common.tgui.color, dimof(usrapp_ui_common.tgui.color));

        usrapp_ui_common.tgui.notifier.mask =
                    (1 << VSF_INPUT_TYPE_TOUCHSCREEN)
                |   (1 << VSF_INPUT_TYPE_KEYBOARD)
#if VSF_TGUI_CFG_SUPPORT_MOUSE_LIKE_EVENTS == ENABLED
                |   (1 << VSF_INPUT_TYPE_MOUSE)
#endif
        ;
        vsf_tgui_input_init(tgui, &usrapp_ui_common.tgui.notifier);
    }

    return 0;
}

#endif
/* EOF */
