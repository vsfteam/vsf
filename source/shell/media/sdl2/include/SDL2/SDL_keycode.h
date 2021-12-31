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

#ifndef __VSF_SDL2_KEYCODE_H__
#define __VSF_SDL2_KEYCODE_H__

/*============================ INCLUDES ======================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef int32_t SDL_Keycode;
enum {
    SDLK_UNKNOWN                = 0,

    SDLK_BACKSPACE              = '\b',
    SDLK_RETURN                 = '\r',
    SDLK_ESCAPE                 = '\033',
    SDLK_TAB                    = '\t',
    SDLK_SPACE                  = ' ',
    SDLK_EXCLAIM                = '!',
    SDLK_QUOTEDBL               = '"',
    SDLK_HASH                   = '#',
    SDLK_PERCENT                = '%',
    SDLK_DOLLAR                 = '$',
    SDLK_AMPERSAND              = '&',
    SDLK_QUOTE                  = '\'',
    SDLK_LEFTPAREN              = '(',
    SDLK_RIGHTPAREN             = ')',
    SDLK_ASTERISK               = '*',
    SDLK_PLUS                   = '+',
    SDLK_COMMA                  = ',',
    SDLK_MINUS                  = '-',
    SDLK_PERIOD                 = '.',
    SDLK_SLASH                  = '/',

    SDLK_0                      = '0',
    SDLK_1                      = '1',
    SDLK_2                      = '2',
    SDLK_3                      = '3',
    SDLK_4                      = '4',
    SDLK_5                      = '5',
    SDLK_6                      = '6',
    SDLK_7                      = '7',
    SDLK_8                      = '8',
    SDLK_9                      = '9',

    SDLK_COLON                  = ':',
    SDLK_SEMICOLON              = ';',
    SDLK_LESS                   = '<',
    SDLK_EQUALS                 = '=',
    SDLK_GREATER                = '>',
    SDLK_QUESTION               = '?',
    SDLK_AT                     = '@',
    SDLK_LEFTBRACKET            = '[',
    SDLK_BACKSLASH              = '\\',
    SDLK_RIGHTBRACKET           = ']',
    SDLK_CARET                  = '^',
    SDLK_UNDERSCORE             = '_',
    SDLK_BACKQUOTE              = '`',

    SDLK_a                      = 'a',
    SDLK_b                      = 'b',
    SDLK_c                      = 'c',
    SDLK_d                      = 'd',
    SDLK_e                      = 'e',
    SDLK_f                      = 'f',
    SDLK_g                      = 'g',
    SDLK_h                      = 'h',
    SDLK_i                      = 'i',
    SDLK_j                      = 'j',
    SDLK_k                      = 'k',
    SDLK_l                      = 'l',
    SDLK_m                      = 'm',
    SDLK_n                      = 'n',
    SDLK_o                      = 'o',
    SDLK_p                      = 'p',
    SDLK_q                      = 'q',
    SDLK_r                      = 'r',
    SDLK_s                      = 's',
    SDLK_t                      = 't',
    SDLK_u                      = 'u',
    SDLK_v                      = 'v',
    SDLK_w                      = 'w',
    SDLK_x                      = 'x',
    SDLK_y                      = 'y',
    SDLK_z                      = 'z',

    // 256 - 287
    SDLK_KP_0                   = 256,
    SDLK_KP_1                   = 257,
    SDLK_KP_2                   = 258,
    SDLK_KP_3                   = 259,
    SDLK_KP_4                   = 260,
    SDLK_KP_5                   = 261,
    SDLK_KP_6                   = 262,
    SDLK_KP_7                   = 263,
    SDLK_KP_8                   = 264,
    SDLK_KP_9                   = 265,
    SDLK_KP0                    = SDLK_KP_0,
    SDLK_KP1                    = SDLK_KP_1,
    SDLK_KP2                    = SDLK_KP_2,
    SDLK_KP3                    = SDLK_KP_3,
    SDLK_KP4                    = SDLK_KP_4,
    SDLK_KP5                    = SDLK_KP_5,
    SDLK_KP6                    = SDLK_KP_6,
    SDLK_KP7                    = SDLK_KP_7,
    SDLK_KP8                    = SDLK_KP_8,
    SDLK_KP9                    = SDLK_KP_9,
    SDLK_KP_PERIOD              = 266,
    SDLK_KP_DIVIDE              = 267,
    SDLK_KP_MULTIPLY            = 268,
    SDLK_KP_MINUS               = 269,
    SDLK_KP_PLUS                = 270,
    SDLK_KP_ENTER               = 271,
    SDLK_KP_EQUALS              = 272,
    SDLK_NUMLOCKCLEAR           = 273,

    // 288 - 310
    SDLK_UP                     = 288,
    SDLK_DOWN                   = 289,
    SDLK_RIGHT                  = 290,
    SDLK_LEFT                   = 291,
    SDLK_INSERT                 = 292,
    SDLK_HOME                   = 293,
    SDLK_END                    = 294,
    SDLK_PAGEUP                 = 295,
    SDLK_PAGEDOWN               = 296,
    SDLK_DELETE                 = 297,
    SDLK_PAUSE                  = 298,

    // 320 - 351
    SDLK_F1                     = 320,
    SDLK_F2                     = 321,
    SDLK_F3                     = 322,
    SDLK_F4                     = 323,
    SDLK_F5                     = 324,
    SDLK_F6                     = 325,
    SDLK_F7                     = 326,
    SDLK_F8                     = 327,
    SDLK_F9                     = 328,
    SDLK_F10                    = 329,
    SDLK_F11                    = 330,
    SDLK_F12                    = 331,
    SDLK_F13                    = 332,
    SDLK_F14                    = 333,
    SDLK_F15                    = 334,
    SDLK_F16                    = 335,
    SDLK_F17                    = 336,
    SDLK_F18                    = 337,
    SDLK_F19                    = 338,
    SDLK_F20                    = 339,
    SDLK_F21                    = 340,
    SDLK_F22                    = 341,
    SDLK_F23                    = 342,
    SDLK_F24                    = 343,
    SDLK_CAPSLOCK               = 344,

    // 352 - 383
    SDLK_LCTRL                  = 352,
    SDLK_LSHIFT                 = 353,
    SDLK_LALT                   = 354,
    SDLK_LGUI                   = 355,
    SDLK_RCTRL                  = 356,
    SDLK_RSHIFT                 = 357,
    SDLK_RALT                   = 358,
    SDLK_RGUI                   = 359,
};

typedef enum SDL_Keymod {
    KMOD_NONE                   = 0,
    KMOD_LSHIFT                 = 1 << 0,
    KMOD_RSHIFT                 = 1 << 1,
    KMOD_LCTRL                  = 1 << 2,
    KMOD_RCTRL                  = 1 << 3,
    KMOD_LALT                   = 1 << 4,
    KMOD_RALT                   = 1 << 5,
    KMOD_LGUI                   = 1 << 6,
    KMOD_RGUI                   = 1 << 7,
    KMOD_LMETA                  = 1 << 8,
    KMOD_RMETA                  = 1 << 9,
    KMOD_NUM                    = 1 << 12,
    KMOD_CAPS                   = 1 << 13,
    KMOD_MODE                   = 1 << 14,

    KMOD_CTRL                   = (KMOD_LCTRL | KMOD_RCTRL),
    KMOD_SHIFT                  = (KMOD_LSHIFT | KMOD_RSHIFT),
    KMOD_ALT                    = (KMOD_LALT | KMOD_RALT),
    KMOD_GUI                    = (KMOD_LGUI | KMOD_RGUI),
    KMOD_META                   = (KMOD_LMETA | KMOD_RMETA),

    KMOD_RESERVED               = 0x8000,
} SDL_Keymod;

#ifdef __cplusplus
}
#endif

#endif      // __VSF_SDL2_KEYCODE_H__
