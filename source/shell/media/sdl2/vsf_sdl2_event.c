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

#include "./vsf_sdl2_cfg.h"

#if VSF_USE_SDL2 == ENABLED

#include "./include/SDL2/SDL.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_sdl2_event_t {
    uint32_t                    evtflags;
    vk_input_notifier_t         notifier;
} vsf_sdl2_event_t;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

static NO_INIT vsf_sdl2_event_t __vsf_sdl2_event;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

static void __vsf_sdl2_event_on_input(vk_input_type_t type, vk_input_evt_t *evt)
{
}

void __SDL_InitEvent(uint32_t flags)
{
    memset(&__vsf_sdl2_event, 0, sizeof(__vsf_sdl2_event));
    __vsf_sdl2_event.notifier.mask = 1 << VSF_INPUT_TYPE_TOUCHSCREEN;
    __vsf_sdl2_event.notifier.on_evt = (vk_input_on_evt_t)__vsf_sdl2_event_on_input;
    vk_input_notifier_register(&__vsf_sdl2_event.notifier);
}

void __SDL_FiniEvent(void)
{
    vk_input_notifier_unregister(&__vsf_sdl2_event.notifier);
}

// joysticks
int SDL_NumJoysticks(void)
{
    return 0;
}
SDL_Joystick * SDL_JoystickOpen(int device_index)
{
    return NULL;
}
int SDL_JoystickEventState(int state)
{
    return -1;
}
int SDL_JoystickNumButtons(SDL_Joystick *joystick)
{
    return 0;
}
int SDL_JoystickNumAxes(SDL_Joystick *joystick)
{
    return 0;
}
int SDL_JoystickNumBalls(SDL_Joystick *joystick)
{
    return 0;
}
int SDL_JoystickNumHats(SDL_Joystick *joystick)
{
    return 0;
}

int SDL_CaptureMouse(SDL_bool enabled)
{
    return 0;
}

uint32_t SDL_GetGlobalMouseState(int * x, int * y)
{
    // TODO: get mouse position and button state
    if (x != NULL) {
        *x = 0;
    }
    if (y != NULL) {
        *y = 0;
    }
    return 0;
}

int SDL_PollEvent(SDL_Event *event)
{
    VSF_SDL2_ASSERT(event != NULL);


    return 0;
}

int SDL_WaitEventTimeout(SDL_Event * event, int timeout)
{
    // todo:
    vsf_thread_wfe(VSF_EVT_RETURN);
    return 0;
}

void SDL_FlushEvent(uint32_t type)
{

}

uint8_t SDL_EventState(uint32_t type, int state)
{
    // TODO: do any thing man
    return SDL_DISABLE;
}

const char * SDL_GetKeyName(SDL_Keycode key)
{
    return "unknown";
}

#endif      // VSF_USE_SDL2
