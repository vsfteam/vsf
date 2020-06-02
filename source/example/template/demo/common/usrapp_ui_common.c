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

#include "./usrapp_ui_common.h"

#if     VSF_USE_UI == ENABLED                                                   \
    &&  (VSF_USE_DISP_SDL2 == ENABLED || VSF_USE_TINY_GUI == ENABLED || VSF_USE_UI_AWTK == ENABLED || VSF_USE_UI_LVGL == ENABLED)

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

usrapp_ui_common_t usrapp_ui_common = {
#if VSF_USE_DISP_SDL2 == ENABLED
        .disp                       = {
            .param                  = {
                .height             = APP_DISP_SDL2_HEIGHT,
                .width              = APP_DISP_SDL2_WIDTH,
                .drv                = &vk_disp_drv_sdl2,
                .color              = APP_DISP_SDL2_COLOR,
            },
            .amplifier              = APP_DISP_SDL2_AMPLIFIER,
        },
#endif
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif      // VSF_USE_UI
/* EOF */