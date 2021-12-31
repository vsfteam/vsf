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

#ifndef WEAK_VSF_INPUT_ON_GAMEPAD
WEAK(vsf_input_on_gamepad)
void vsf_input_on_gamepad(vk_gamepad_evt_t *gamepad_evt)
{
    vsf_input_on_evt(VSF_INPUT_TYPE_GAMEPAD, &gamepad_evt->use_as__vk_input_evt_t);
}
#endif

void vk_gamepad_input_parse(void *dev, vk_input_item_info_t *item_info,
                        uint8_t *pre, uint8_t *cur)
{
    vk_gamepad_evt_t evt;
    vk_input_item_info_t *info;
    vk_input_parser_t parser;
    bool event_sent = false;

    evt.duration = 0;
    evt.dev = dev;

    parser.info = item_info;
    parser.num = GAMEPAD_ID_NUM;
    do {
        info = vk_input_parse(&parser, pre, cur);
        if (info != NULL) {
            evt.id = info->item;
            evt.info = *info;
            evt.pre = parser.pre;
            evt.cur = parser.cur;
            vsf_input_on_gamepad(&evt);
            event_sent = true;
        }
    } while (info != NULL);
    if (event_sent) {
        evt.id = GAMEPAD_ID_DUMMY;
        vsf_input_on_gamepad(&evt);
    }
}

#endif      // VSF_USE_INPUT
