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

#ifndef __VSF_INPUT_KEYBOARD_H__
#define __VSF_INPUT_KEYBOARD_H__

/*============================ INCLUDES ======================================*/
#include "../vsf_input_cfg.h"

#include "../vsf_input_get_type.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_input_keyboard_set(__EVENT, __KEYCODE,  __IS_DOWN)                  \
            do {                                                                \
                (__EVENT)->id = ((__KEYCODE) | ((__IS_DOWN) << 16));            \
            } while (0)

#define vsf_input_keyboard_get_keycode(__EVENT)                                 \
            ((uint16_t)(((__EVENT)->id >> 0) & 0xFFFF))
#define vsf_input_keyboard_is_down(__EVENT)                                     \
            (((__EVENT)->id >> 16) & 0x01)

/*============================ TYPES =========================================*/

// TODO: add keycode

enum {
    VSF_INPUT_TYPE_KEYBOARD = VSF_INPUT_USER_TYPE,
};

struct vk_keyboard_evt_t {
    implement(vk_input_evt_t)
};
typedef struct vk_keyboard_evt_t vk_keyboard_evt_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */
