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

#ifndef __VSF_INPUT_MOUSE_H__
#define __VSF_INPUT_MOUSE_H__

/*============================ INCLUDES ======================================*/
#include "../vsf_input_cfg.h"

#include "../vsf_input_get_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define vk_input_mouse_evt_move_set(__evt,__x, __y)                             \
            do {                                                                \
                (__evt)->id = VSF_INPUT_MOUSE_EVT_MOVE;                         \
                (__evt)->cur.valu32 = ((__x) | ((__y) << 16));                  \
            } while (0)

#define vk_input_mouse_evt_wheel_set(__evt, __x, __y)                           \
            do {                                                                \
                (__evt)->id = VSF_INPUT_MOUSE_EVT_WHEEL;                        \
                (__evt)->cur.valu32 = ((__x) | ((__y) << 16));                  \
            } while (0)

#define vk_input_mouse_evt_button_set(__evt, __button, __is_down, __x, __y)     \
            do {                                                                \
                (__evt)->id = VSF_INPUT_MOUSE_EVT_BUTTON | ((__button) << 8) | ((__is_down) << 12);\
                (__evt)->cur.valu32 = ((__x) | ((__y) << 16));                  \
            } while (0)

// result is VSF_INPUT_MOUSE_EVT_(MOVE/BUTTON/WHEEL)
#define vk_input_mouse_evt_get(__evt)                                           \
            ((__evt)->id & 0xFF)

// for button events
#define vk_input_mouse_evt_button_get(__evt)                                    \
            (int)(((__evt)->id >> 8) & 0x03)
#define vk_input_mouse_evt_button_is_down(__evt)                                \
            (!!(((__evt)->id >> 12) & 1))

// for move/button/wheel events
#define vk_input_mouse_evt_get_x(__evt)                                         \
            ((int16_t)(((__evt)->cur.valu32 >> 0) & 0xFFFF))
#define vk_input_mouse_evt_get_y(__evt)                                         \
            ((int16_t)(((__evt)->cur.valu32 >> 16) & 0xFFFF))

/*============================ TYPES =========================================*/

enum {
    VSF_INPUT_TYPE_MOUSE = VSF_INPUT_USER_TYPE,
};

enum {
    VSF_INPUT_MOUSE_EVT_MOVE,
    VSF_INPUT_MOUSE_EVT_BUTTON,
    VSF_INPUT_MOUSE_EVT_WHEEL,
};

enum {
    VSF_INPUT_MOUSE_BUTTON_LEFT,
    VSF_INPUT_MOUSE_BUTTON_MIDDLE,
    VSF_INPUT_MOUSE_BUTTON_RIGHT,
};

typedef struct vk_mouse_evt_t {
    implement(vk_input_evt_t)
} vk_mouse_evt_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
