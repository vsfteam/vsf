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

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_input_keyboard_set(__event, __keycode, __is_down, __keymod)         \
            do {                                                                \
                (__event)->id = (((__keycode) << 0) | ((__is_down) << 9) | ((__keymod) << 10));\
            } while (0)

#define vsf_input_keyboard_get_keymod(__event)                                  \
            ((uint16_t)(((__event)->id >> 10) & 0xFF))
#define vsf_input_keyboard_get_keycode(__event)                                 \
            ((uint16_t)(((__event)->id >> 0) & 0x1FF))
#define vsf_input_keyboard_is_down(__event)                                     \
            (((__event)->id >> 9) & 0x01)

/*============================ TYPES =========================================*/

enum {
    VSF_INPUT_TYPE_KEYBOARD = VSF_INPUT_USER_TYPE,
};

enum {
    VSF_KB_UNKNOWN          = 0,

    // keycodes from hut1.2 keyboard/keypad page
    VSF_KB_a                = 4,
    VSF_KB_b                = 5,
    VSF_KB_c                = 6,
    VSF_KB_d                = 7,
    VSF_KB_e                = 8,
    VSF_KB_f                = 9,
    VSF_KB_g                = 10,
    VSF_KB_h                = 11,
    VSF_KB_i                = 12,
    VSF_KB_j                = 13,
    VSF_KB_k                = 14,
    VSF_KB_l                = 15,
    VSF_KB_m                = 16,
    VSF_KB_n                = 17,
    VSF_KB_o                = 18,
    VSF_KB_p                = 19,
    VSF_KB_q                = 20,
    VSF_KB_r                = 21,
    VSF_KB_s                = 22,
    VSF_KB_t                = 23,
    VSF_KB_u                = 24,
    VSF_KB_v                = 25,
    VSF_KB_w                = 26,
    VSF_KB_x                = 27,
    VSF_KB_y                = 28,
    VSF_KB_z                = 29,
    VSF_KB_1                = 30,
    VSF_KB_2                = 31,
    VSF_KB_3                = 32,
    VSF_KB_4                = 33,
    VSF_KB_5                = 34,
    VSF_KB_6                = 35,
    VSF_KB_7                = 36,
    VSF_KB_8                = 37,
    VSF_KB_9                = 38,
    VSF_KB_0                = 39,
    VSF_KB_ENTER            = 40,
    VSF_KB_ESCAPE           = 41,
    VSF_KB_BACKSPACE        = 42,
    VSF_KB_TAB              = 43,
    VSF_KB_SPACE            = 44,
    VSF_KB_MINUS            = 45,
    VSF_KB_EQUAL            = 46,
    VSF_KB_LEFT_BRACKET     = 47,
    VSF_KB_RIGHT_BRACKET    = 48,
    VSF_KB_BACKSLASH        = 49,   // '\'
    // Non-US #
    VSF_KB_SEMICOLON        = 51,
    VSF_KB_SINGLE_QUOTE     = 52,
    VSF_KB_GRAVE            = 53,
    VSF_KB_COMMA            = 54,
    VSF_KB_DOT              = 55,
    VSF_KB_SLASH            = 56,   // '/'
    VSF_KB_CAPSLOCK         = 57,
    VSF_KB_F1               = 58,
    VSF_KB_F2               = 59,
    VSF_KB_F3               = 60,
    VSF_KB_F4               = 61,
    VSF_KB_F5               = 62,
    VSF_KB_F6               = 63,
    VSF_KB_F7               = 64,
    VSF_KB_F8               = 65,
    VSF_KB_F9               = 66,
    VSF_KB_F10              = 67,
    VSF_KB_F11              = 68,
    VSF_KB_F12              = 69,
    VSF_KB_PRINT_SCREEN     = 70,
    VSF_KB_SCROLL_LOCK      = 71,
    VSF_KB_PAUSE            = 72,
    VSF_KB_INSERT           = 73,
    VSF_KB_HOME             = 74,
    VSF_KB_PAGE_UP          = 75,
    VSF_KB_DELETE           = 76,
    VSF_KB_END              = 77,
    VSF_KB_PAGE_DOWN        = 78,
    VSF_KB_RIGHT            = 79,
    VSF_KB_LEFT             = 80,
    VSF_KB_DOWN             = 81,
    VSF_KB_UP               = 82,
    VSF_KP_NUMLOCK          = 83,
    VSF_KP_DIVIDE           = 84,
    VSF_KP_MULTIPLY         = 85,
    VSF_KP_MINUS            = 86,
    VSF_KP_PLUS             = 87,
    VSF_KP_ENTER            = 88,
    VSF_KP_1                = 89,
    VSF_KP_2                = 90,
    VSF_KP_3                = 91,
    VSF_KP_4                = 92,
    VSF_KP_5                = 93,
    VSF_KP_6                = 94,
    VSF_KP_7                = 95,
    VSF_KP_8                = 96,
    VSF_KP_9                = 97,
    VSF_KP_0                = 98,
    VSF_KP_DOT              = 99,

    VSF_KB_EXT              = 0x0100,
    // keycodes with SHIFT pressed
    VSF_KB_EXCLAM           = VSF_KB_1 | VSF_KB_EXT,
    VSF_KB_AT               = VSF_KB_2 | VSF_KB_EXT,
    VSF_KB_POUND            = VSF_KB_3 | VSF_KB_EXT,
    VSF_KB_DOLLAR           = VSF_KB_4 | VSF_KB_EXT,
    VSF_KB_PERCENT          = VSF_KB_5 | VSF_KB_EXT,
    VSF_KB_CARET            = VSF_KB_6 | VSF_KB_EXT,
    VSF_KB_AMPERSAND        = VSF_KB_7 | VSF_KB_EXT,
    VSF_KB_ASTERISK         = VSF_KB_8 | VSF_KB_EXT,
    VSF_KB_LEFT_PAREN       = VSF_KB_9 | VSF_KB_EXT,
    VSF_KB_RIGHT_PAREN      = VSF_KB_0 | VSF_KB_EXT,
    VSF_KB_UNDERSCORE       = VSF_KB_MINUS | VSF_KB_EXT,
    VSF_KB_PLUS             = VSF_KB_EQUAL | VSF_KB_EXT,
    VSF_KB_LEFT_BRACE       = VSF_KB_LEFT_BRACKET | VSF_KB_EXT,
    VSF_KB_RIGHT_BRACE      = VSF_KB_RIGHT_BRACKET | VSF_KB_EXT,
    VSF_KB_BAR              = VSF_KB_BACKSLASH | VSF_KB_EXT,
    VSF_KB_COLON            = VSF_KB_SEMICOLON | VSF_KB_EXT,
    VSF_KB_DOUBLE_QUOTE     = VSF_KB_SINGLE_QUOTE | VSF_KB_EXT,
    VSF_KB_TIDE             = VSF_KB_GRAVE | VSF_KB_EXT,
    VSF_KB_LESS             = VSF_KB_COMMA | VSF_KB_EXT,
    VSF_KB_GREATER          = VSF_KB_DOT | VSF_KB_EXT,
    VSF_KB_QUESTION         = VSF_KB_SLASH | VSF_KB_EXT,

    VSF_KB_A                = VSF_KB_a | VSF_KB_EXT,
    VSF_KB_B                = VSF_KB_b | VSF_KB_EXT,
    VSF_KB_C                = VSF_KB_c | VSF_KB_EXT,
    VSF_KB_D                = VSF_KB_d | VSF_KB_EXT,
    VSF_KB_E                = VSF_KB_e | VSF_KB_EXT,
    VSF_KB_F                = VSF_KB_f | VSF_KB_EXT,
    VSF_KB_G                = VSF_KB_g | VSF_KB_EXT,
    VSF_KB_H                = VSF_KB_h | VSF_KB_EXT,
    VSF_KB_I                = VSF_KB_i | VSF_KB_EXT,
    VSF_KB_J                = VSF_KB_j | VSF_KB_EXT,
    VSF_KB_K                = VSF_KB_k | VSF_KB_EXT,
    VSF_KB_L                = VSF_KB_l | VSF_KB_EXT,
    VSF_KB_M                = VSF_KB_m | VSF_KB_EXT,
    VSF_KB_N                = VSF_KB_n | VSF_KB_EXT,
    VSF_KB_O                = VSF_KB_o | VSF_KB_EXT,
    VSF_KB_P                = VSF_KB_p | VSF_KB_EXT,
    VSF_KB_Q                = VSF_KB_q | VSF_KB_EXT,
    VSF_KB_R                = VSF_KB_r | VSF_KB_EXT,
    VSF_KB_S                = VSF_KB_s | VSF_KB_EXT,
    VSF_KB_T                = VSF_KB_t | VSF_KB_EXT,
    VSF_KB_U                = VSF_KB_u | VSF_KB_EXT,
    VSF_KB_V                = VSF_KB_v | VSF_KB_EXT,
    VSF_KB_W                = VSF_KB_w | VSF_KB_EXT,
    VSF_KB_X                = VSF_KB_x | VSF_KB_EXT,
    VSF_KB_Y                = VSF_KB_y | VSF_KB_EXT,
    VSF_KB_Z                = VSF_KB_z | VSF_KB_EXT,
};

enum {
    VSF_KM_LEFT_SHIFT       = 0x01,
    VSF_KM_LEFT_CTRL        = 0x02,
    VSF_KM_LEFT_ALT         = 0x04,
    VSF_KM_LEFT_GUI         = 0x08,
    VSF_KM_RIGHT_SHIFT      = 0x10,
    VSF_KM_RIGHT_CTRL       = 0x20,
    VSF_KM_RIGHT_ALT        = 0x40,
    VSF_KM_RIGHT_GUI        = 0x80,
};

typedef struct vk_keyboard_evt_t {
    implement(vk_input_evt_t)
} vk_keyboard_evt_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
