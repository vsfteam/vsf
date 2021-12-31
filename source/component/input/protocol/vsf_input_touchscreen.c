/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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

#include "../vsf_input_cfg.h"

#if VSF_USE_INPUT == ENABLED

#include "../vsf_input.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSF_INPUT_ON_TOUCHSCREEN
WEAK(vsf_input_on_touchscreen)
void vsf_input_on_touchscreen(vk_touchscreen_evt_t *ts_evt)
{
#if VSF_INPUT_TOUCH_SCREEN_CFG_TRACE == ENABLED
    if (vsf_input_touchscreen_is_down(ts_evt)) {
        vsf_trace_debug("touch_screen(%d): (%d, %d) %d" VSF_TRACE_CFG_LINEEND,
            vsf_input_touchscreen_get_id(ts_evt), vsf_input_touchscreen_get_x(ts_evt),
            vsf_input_touchscreen_get_y(ts_evt), vsf_input_touchscreen_get_pressure(ts_evt));
    } else {
        vsf_trace_debug("touch_screen(%d): (-1, -1) 0" VSF_TRACE_CFG_LINEEND,
            vsf_input_touchscreen_get_id(ts_evt));
    }
#endif
    vsf_input_on_evt(VSF_INPUT_TYPE_TOUCHSCREEN, &ts_evt->use_as__vk_input_evt_t);
}
#endif

#endif      // VSF_USE_INPUT
