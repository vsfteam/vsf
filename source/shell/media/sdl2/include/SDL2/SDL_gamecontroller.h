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

#ifndef __VSF_SDL2_GAMECONTROLLER_H__
#define __VSF_SDL2_GAMECONTROLLER_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_sdl2_cfg.h"

#if VSF_USE_SDL2 == ENABLED

#include "SDL_stdinc.h"
#include "SDL_keycode.h"

#include "component/input/vsf_input.h"

#include "begin_code.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum SDL_GameControllerType {
    SDL_CONTROLLER_TYPE_UNKNOWN                 = 0,
} SDL_GameControllerType;

typedef enum SDL_GameControllerBindType {
    SDL_CONTROLLER_BINDTYPE_NONE                = 0,
    SDL_CONTROLLER_BINDTYPE_BUTTON,
    SDL_CONTROLLER_BINDTYPE_AXIS,
    SDL_CONTROLLER_BINDTYPE_HAT,
} SDL_GameControllerBindType;

typedef enum SDL_GameControllerAxis {
    SDL_CONTROLLER_AXIS_INVALID                 = -1,
    SDL_CONTROLLER_AXIS_LEFTX,
    SDL_CONTROLLER_AXIS_LEFTY,
    SDL_CONTROLLER_AXIS_RIGHTX,
    SDL_CONTROLLER_AXIS_RIGHTY,
    SDL_CONTROLLER_AXIS_TRIGGERLEFT,
    SDL_CONTROLLER_AXIS_TRIGGERRIGHT,
    SDL_CONTROLLER_AXIS_MAX
} SDL_GameControllerAxis;

typedef enum SDL_GameControllerButton {
    SDL_CONTROLLER_BUTTON_INVALID               = -1,
    SDL_CONTROLLER_BUTTON_A                     = GAMEPAD_ID_R_DOWN,
    SDL_CONTROLLER_BUTTON_B                     = GAMEPAD_ID_R_RIGHT,
    SDL_CONTROLLER_BUTTON_X                     = GAMEPAD_ID_R_LEFT,
    SDL_CONTROLLER_BUTTON_Y                     = GAMEPAD_ID_R_UP,
    SDL_CONTROLLER_BUTTON_BACK                  = GAMEPAD_ID_MENU_RIGHT,
    SDL_CONTROLLER_BUTTON_GUIDE                 = GAMEPAD_ID_MENU_MAIN,
    SDL_CONTROLLER_BUTTON_START                 = GAMEPAD_ID_MENU_LEFT,
    SDL_CONTROLLER_BUTTON_LEFTSTICK             = GAMEPAD_ID_LS,
    SDL_CONTROLLER_BUTTON_RIGHTSTICK            = GAMEPAD_ID_RS,
    SDL_CONTROLLER_BUTTON_LEFTSHOULDER          = GAMEPAD_ID_LB,
    SDL_CONTROLLER_BUTTON_RIGHTSHOULDER         = GAMEPAD_ID_RB,
    SDL_CONTROLLER_BUTTON_DPAD_UP               = GAMEPAD_ID_L_UP,
    SDL_CONTROLLER_BUTTON_DPAD_DOWN             = GAMEPAD_ID_L_DOWN,
    SDL_CONTROLLER_BUTTON_DPAD_LEFT             = GAMEPAD_ID_L_LEFT,
    SDL_CONTROLLER_BUTTON_DPAD_RIGHT            = GAMEPAD_ID_L_RIGHT,
    SDL_CONTROLLER_BUTTON_PADDLE1               = GAMEPAD_ID_PADDLE1,
    SDL_CONTROLLER_BUTTON_PADDLE2               = GAMEPAD_ID_PADDLE2,
    SDL_CONTROLLER_BUTTON_PADDLE3               = GAMEPAD_ID_PADDLE3,
    SDL_CONTROLLER_BUTTON_PADDLE4               = GAMEPAD_ID_PADDLE4,
    SDL_CONTROLLER_BUTTON_TOUCHPAD              = GAMEPAD_ID_SPECIAL,
    SDL_CONTROLLER_BUTTON_MAX
} SDL_GameControllerButton;

typedef struct SDL_GameController SDL_GameController;
struct SDL_GameController {
    const char *name;
    SDL_GameController *next;
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif      // VSF_USE_SDL2
#endif      // __VSF_SDL2_GAMECONTROLLER_H__
/* EOF */
