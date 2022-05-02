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

#ifndef __VSF_SDL2_KEYBOARD_H__
#define __VSF_SDL2_KEYBOARD_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_sdl2_cfg.h"

#if VSF_USE_SDL2 == ENABLED

#include "SDL_stdinc.h"
#include "SDL_keycode.h"

#include "begin_code.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_SDL_CFG_WRAPPER == ENABLED
#define SDL_GetKeyboardState            VSF_SDL_WRAPPER(SDL_GetKeyboardState)
#endif

#define SDL_StartTextInput()
#define SDL_StopTextInput()

/*============================ TYPES =========================================*/

typedef struct SDL_Keysym {
    SDL_Scancode scancode;
    SDL_Keycode sym;
    uint16_t mod;
    uint16_t unicode;
} SDL_Keysym;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern const uint8_t * SDL_GetKeyboardState(int *numkeys);

#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif      // VSF_USE_SDL2
#endif      // __VSF_SDL2_KEYBOARD_H__
/* EOF */
