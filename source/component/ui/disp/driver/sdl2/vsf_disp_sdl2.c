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

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_DISP_USE_SDL2 == ENABLED

#define __VSF_DISP_CLASS_INHERIT__
#define __VSF_DISP_SDL2_CLASS_IMPLEMENT

#include "../../vsf_disp.h"
#include "./vsf_disp_sdl2.h"

#include "component/input/vsf_input.h"

#ifdef VSF_DISP_SDL2_CFG_INCLUDE
#   include VSF_DISP_SDL2_CFG_INCLUDE
#else
#   include "SDL.h"
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_DISP_SDL2_CFG_HW_PRIORITY
#   define VSF_DISP_SDL2_CFG_HW_PRIORITY                vsf_arch_prio_0
#endif

#ifndef VSF_DISP_SDL2_USE_CONTROLLER
#   define VSF_DISP_SDL2_USE_CONTROLLER                 ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_disp_sdl2_t {
    vsf_arch_irq_thread_t init_thread;
    bool is_init_called;
    vk_disp_sdl2_t *disp;
} vsf_disp_sdl2_t;

/*============================ LOCAL VARIABLES ===============================*/

static vsf_disp_sdl2_t __vk_disp_sdl2 = {
    .is_init_called = false,
};

/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vk_disp_sdl2_init(vk_disp_t *pthis);
static vsf_err_t __vk_disp_sdl2_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff);
static void __vk_disp_sdl2_event_thread(void *arg);

#if VSF_USE_INPUT == ENABLED
extern void vsf_input_on_mouse(vk_mouse_evt_t *mouse_evt);
extern void vsf_input_on_touchscreen(vk_touchscreen_evt_t *ts_evt);
extern void vsf_input_on_gamepad(vk_gamepad_evt_t *gamepad_evt);
extern void vsf_input_on_keyboard(vk_keyboard_evt_t *keyboard_evt);
#endif

/*============================ GLOBAL VARIABLES ==============================*/

const vk_disp_drv_t vk_disp_drv_sdl2 = {
    .init       = __vk_disp_sdl2_init,
    .refresh    = __vk_disp_sdl2_refresh,
};

/*============================ IMPLEMENTATION ================================*/

static void __vk_disp_sdl2_common_init(void)
{
    SDL_Init(SDL_INIT_VIDEO
#if VSF_DISP_SDL2_USE_CONTROLLER == ENABLED
        | SDL_INIT_GAMECONTROLLER
#endif
    );
}

static void __vk_disp_sdl2_common_fini(void)
{
    SDL_Quit();
}

static Uint32 __vk_disp_sdl2_get_format(vk_disp_sdl2_t *disp_sdl2)
{
    switch (disp_sdl2->param.color) {
    default:
    case VSF_DISP_COLOR_RGB332:     return SDL_PIXELFORMAT_RGB332;
    case VSF_DISP_COLOR_RGB565:     return SDL_PIXELFORMAT_RGB565;
    case VSF_DISP_COLOR_ARGB8888:   return SDL_PIXELFORMAT_ARGB8888;
    }
}

static void __vk_disp_sdl2_screen_init(vk_disp_sdl2_t *disp_sdl2)
{
    disp_sdl2->window = SDL_CreateWindow(disp_sdl2->title,
                            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            disp_sdl2->param.width * disp_sdl2->amplifier,
                            disp_sdl2->param.height * disp_sdl2->amplifier,
                            0);
    disp_sdl2->renderer = SDL_CreateRenderer(disp_sdl2->window, -1, 0);
    disp_sdl2->texture = SDL_CreateTexture(disp_sdl2->renderer,
                            __vk_disp_sdl2_get_format(disp_sdl2),
                            SDL_TEXTUREACCESS_STATIC,
                            disp_sdl2->param.width, disp_sdl2->param.height);
    SDL_SetTextureBlendMode(disp_sdl2->texture, SDL_BLENDMODE_BLEND);
}

static void __vk_disp_sdl2_screen_update(vk_disp_sdl2_t *disp_sdl2)
{
    SDL_Rect rect = {
       .x = disp_sdl2->area.pos.x,
       .y = disp_sdl2->area.pos.y,
       .w = disp_sdl2->area.size.x,
       .h = disp_sdl2->area.size.y,
    };
    SDL_UpdateTexture(disp_sdl2->texture, &rect, disp_sdl2->disp_buff,
        disp_sdl2->area.size.x * vsf_disp_get_pixel_bytesize(disp_sdl2));
    SDL_RenderClear(disp_sdl2->renderer);
    SDL_RenderCopy(disp_sdl2->renderer, disp_sdl2->texture, NULL, NULL);
    SDL_RenderPresent(disp_sdl2->renderer);
}

static void __vk_disp_sdl2_init_thread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = arg;
    vk_disp_sdl2_t *disp_sdl2 = __vk_disp_sdl2.disp;

    __vsf_arch_irq_set_background(irq_thread);
        __vk_disp_sdl2_common_init();

        __vsf_arch_irq_start(irq_thread);
            vk_disp_on_ready(&disp_sdl2->use_as__vk_disp_t);
        __vsf_arch_irq_end(irq_thread, false);

        __vsf_arch_irq_init(&disp_sdl2->event_thread, "disp_sdl2_event", __vk_disp_sdl2_event_thread, VSF_DISP_SDL2_CFG_HW_PRIORITY);

    __vsf_arch_irq_fini(irq_thread);
}

static void __vk_disp_sdl2_flush_thread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = arg;
    vk_disp_sdl2_t *disp_sdl2 = container_of(irq_thread, vk_disp_sdl2_t, flush_thread);

    __vsf_arch_irq_set_background(irq_thread);

    while (true) {
        __vsf_arch_irq_request_pend(&disp_sdl2->flush_request);
        __vk_disp_sdl2_screen_update(disp_sdl2);

        if (disp_sdl2->flush_delay_ms > 0) {
            __vsf_arch_irq_sleep(disp_sdl2->flush_delay_ms);
        }

        __vsf_arch_irq_start(irq_thread);
            vk_disp_on_ready(&disp_sdl2->use_as__vk_disp_t);
        __vsf_arch_irq_end(irq_thread, false);
    }
}

static uint_fast16_t __vk_disp_sdl2_keycode_remap(SDL_Scancode scancode)
{
    SDL_Keymod mod = SDL_GetModState();
    bool is_upper = false;

    if (    (mod & VSF_KM_CAPSLOCK)
        &&  ((scancode >= SDL_SCANCODE_A) && (scancode <= SDL_SCANCODE_Z))) {
        is_upper = !is_upper;
     }
    if (    (mod & KMOD_CAPS)
        &&  (   ((scancode >= SDL_SCANCODE_1) && (scancode <= SDL_SCANCODE_0))
            ||  ((scancode >= SDL_SCANCODE_MINUS) && (scancode <= VSF_KB_SLASH))
            ||  ((scancode >= SDL_SCANCODE_A) && (scancode <= SDL_SCANCODE_Z)))) {
        is_upper = !is_upper;
    }

    return scancode | (is_upper ? VSF_KB_EXT : 0);
}

static uint_fast16_t __vk_disp_sdl2_keymod_remap(uint16_t keymod)
{
    uint_fast16_t keymod_remap = 0;

    if (keymod & KMOD_LSHIFT) {
        keymod_remap |= VSF_KM_LEFT_SHIFT;
    }
    if (keymod & KMOD_RSHIFT) {
        keymod_remap |= VSF_KM_RIGHT_SHIFT;
    }
    if (keymod & KMOD_LCTRL) {
        keymod_remap |= VSF_KM_LEFT_CTRL;
    }
    if (keymod & KMOD_RCTRL) {
        keymod_remap |= VSF_KM_RIGHT_CTRL;
    }
    if (keymod & KMOD_LALT) {
        keymod_remap |= VSF_KM_LEFT_ALT;
    }
    if (keymod & KMOD_RALT) {
        keymod_remap |= VSF_KM_RIGHT_ALT;
    }
    if (keymod & KMOD_LGUI) {
        keymod_remap |= VSF_KM_LEFT_GUI;
    }
    if (keymod & KMOD_RGUI) {
        keymod_remap |= VSF_KM_RIGHT_GUI;
    }
    if (keymod & KMOD_NUM) {
        keymod_remap |= VSF_KM_NUMLOCK;
    }
    if (keymod & KMOD_CAPS) {
        keymod_remap |= VSF_KM_CAPSLOCK;
    }
    return keymod_remap;
}

static void __vk_disp_sdl2_event_thread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = arg;
    vk_disp_sdl2_t *disp_sdl2 = container_of(irq_thread, vk_disp_sdl2_t, event_thread);

    SDL_Event event;
#if VSF_USE_INPUT == ENABLED
    union {
        implement(vk_input_evt_t)
        vk_touchscreen_evt_t    ts_evt;
        vk_gamepad_evt_t        gamepad_evt;
        vk_keyboard_evt_t       keyboard_evt;
        vk_mouse_evt_t          mouse_evt;
    } evt;
    vk_input_type_t evt_type;

    uint_fast16_t x = 0, y = 0;
    uint_fast8_t pressure = 0;
    bool is_down = false, is_to_update = false;
    int_fast16_t wheel_x = 0, wheel_y = 0;
    int mouse_evt = 0, mouse_button = 0;

    evt.dev = disp_sdl2;
#endif

    __vsf_arch_irq_set_background(irq_thread);
        __vk_disp_sdl2_screen_init(disp_sdl2);

    __vsf_arch_irq_init(&disp_sdl2->flush_thread, "disp_sdl2_flush", __vk_disp_sdl2_flush_thread, VSF_DISP_SDL2_CFG_HW_PRIORITY);

    while (1) {
        if (SDL_WaitEvent(&event)) {
#if VSF_USE_INPUT == ENABLED
            switch (event.type) {
#   if VSF_DISP_SDL2_CFG_MOUSE_AS_TOUCHSCREEN == ENABLED
            case SDL_MOUSEBUTTONUP:
                is_down = false;
                is_to_update = true;
                evt_type = VSF_INPUT_TYPE_TOUCHSCREEN;
                break;
            case SDL_MOUSEBUTTONDOWN:
                is_down = true;
                x = event.motion.x / disp_sdl2->amplifier;
                y = event.motion.y / disp_sdl2->amplifier;
                is_to_update = true;
                evt_type = VSF_INPUT_TYPE_TOUCHSCREEN;
                break;
            case SDL_MOUSEMOTION:
                x = event.motion.x / disp_sdl2->amplifier;
                y = event.motion.y / disp_sdl2->amplifier;
                is_to_update = is_down;
                evt_type = VSF_INPUT_TYPE_TOUCHSCREEN;
                break;
#   else
            case SDL_MOUSEBUTTONUP:
                is_down = false;
                x = event.button.x / disp_sdl2->amplifier;
                y = event.button.y / disp_sdl2->amplifier;
                switch (event.button.button) {
                case SDL_BUTTON_LEFT:   mouse_button = VSF_INPUT_MOUSE_BUTTON_LEFT;     break;
                case SDL_BUTTON_MIDDLE: mouse_button = VSF_INPUT_MOUSE_BUTTON_MIDDLE;   break;
                case SDL_BUTTON_RIGHT:  mouse_button = VSF_INPUT_MOUSE_BUTTON_RIGHT;    break;
                default:                VSF_UI_ASSERT(false);
                }
                mouse_evt = VSF_INPUT_MOUSE_EVT_BUTTON;
                is_to_update = true;
                evt_type = VSF_INPUT_TYPE_MOUSE;
                break;
            case SDL_MOUSEBUTTONDOWN:
                is_down = true;
                x = event.button.x / disp_sdl2->amplifier;
                y = event.button.y / disp_sdl2->amplifier;
                switch (event.button.button) {
                case SDL_BUTTON_LEFT:   mouse_button = VSF_INPUT_MOUSE_BUTTON_LEFT;     break;
                case SDL_BUTTON_MIDDLE: mouse_button = VSF_INPUT_MOUSE_BUTTON_MIDDLE;   break;
                case SDL_BUTTON_RIGHT:  mouse_button = VSF_INPUT_MOUSE_BUTTON_RIGHT;    break;
                default:                VSF_UI_ASSERT(false);
                }
                mouse_evt = VSF_INPUT_MOUSE_EVT_BUTTON;
                is_to_update = true;
                evt_type = VSF_INPUT_TYPE_MOUSE;
                break;
            case SDL_MOUSEMOTION:
                x = event.motion.x / disp_sdl2->amplifier;
                y = event.motion.y / disp_sdl2->amplifier;
                mouse_evt = VSF_INPUT_MOUSE_EVT_MOVE;
                is_to_update = true;
                evt_type = VSF_INPUT_TYPE_MOUSE;
                break;
            case SDL_MOUSEWHEEL:
                if (SDL_MOUSEWHEEL_FLIPPED == event.wheel.direction) {
                    wheel_x = -event.wheel.x;
                    wheel_y = -event.wheel.y;
                } else {
                    wheel_x = event.wheel.x;
                    wheel_y = event.wheel.y;
                }
                mouse_evt = VSF_INPUT_MOUSE_EVT_WHEEL;
                is_to_update = true;
                evt_type = VSF_INPUT_TYPE_MOUSE;
                break;
#   endif

            case SDL_KEYDOWN:
            case SDL_KEYUP:
                if (!event.key.repeat && (event.key.keysym.sym != SDLK_CAPSLOCK)) {
                    is_to_update = true;
                    evt_type = VSF_INPUT_TYPE_KEYBOARD;
                    vsf_input_keyboard_set(&evt.keyboard_evt,
                        __vk_disp_sdl2_keycode_remap(event.key.keysym.scancode),
                        SDL_KEYDOWN == event.type,
                        __vk_disp_sdl2_keymod_remap(event.key.keysym.mod));
                }
                break;
#if VSF_DISP_SDL2_USE_CONTROLLER == ENABLED
            case SDL_CONTROLLERDEVICEADDED:
                SDL_GameControllerOpen(event.cdevice.which);
                break;
            case SDL_CONTROLLERDEVICEREMOVED:
                break;
            case SDL_CONTROLLERAXISMOTION:
                is_to_update = true;
                evt_type = VSF_INPUT_TYPE_GAMEPAD;

                evt.gamepad_evt.cur.val16 = event.caxis.value;

                switch (event.caxis.axis) {
                case SDL_CONTROLLER_AXIS_LEFTX:
                case SDL_CONTROLLER_AXIS_LEFTY:
                case SDL_CONTROLLER_AXIS_RIGHTX:
                case SDL_CONTROLLER_AXIS_RIGHTY:
                    evt.gamepad_evt.bitlen = 16;
                    evt.gamepad_evt.is_signed = true;
                    break;
                case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
                case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
                    evt.gamepad_evt.bitlen = 15;
                    evt.gamepad_evt.is_signed = false;
                    break;
                default:                                    is_to_update = false;                       break;
                }

                switch (event.caxis.axis) {
                case SDL_CONTROLLER_AXIS_LEFTX:             evt.gamepad_evt.id = GAMEPAD_ID_LX;         break;
                case SDL_CONTROLLER_AXIS_LEFTY:             evt.gamepad_evt.id = GAMEPAD_ID_LY;         break;
                case SDL_CONTROLLER_AXIS_RIGHTX:            evt.gamepad_evt.id = GAMEPAD_ID_RX;         break;
                case SDL_CONTROLLER_AXIS_RIGHTY:            evt.gamepad_evt.id = GAMEPAD_ID_RY;         break;
                case SDL_CONTROLLER_AXIS_TRIGGERLEFT:       evt.gamepad_evt.id = GAMEPAD_ID_LT;         break;
                case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:      evt.gamepad_evt.id = GAMEPAD_ID_RT;         break;
                default:                                    is_to_update = false;                       break;
                }
                break;
            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP:
                is_to_update = true;
                evt_type = VSF_INPUT_TYPE_GAMEPAD;

                evt.gamepad_evt.is_signed = false;
                evt.gamepad_evt.bitlen = 1;
                if (SDL_CONTROLLERBUTTONDOWN == event.type) {
                    evt.gamepad_evt.cur.bit = 1;
                    evt.gamepad_evt.pre.bit = 0;
                } else {
                    evt.gamepad_evt.cur.bit = 0;
                    evt.gamepad_evt.pre.bit = 1;
                }

                switch (event.cbutton.button) {
                case SDL_CONTROLLER_BUTTON_A:               evt.gamepad_evt.id = GAMEPAD_ID_R_DOWN;     break;
                case SDL_CONTROLLER_BUTTON_B:               evt.gamepad_evt.id = GAMEPAD_ID_R_RIGHT;    break;
                case SDL_CONTROLLER_BUTTON_X:               evt.gamepad_evt.id = GAMEPAD_ID_R_LEFT;     break;
                case SDL_CONTROLLER_BUTTON_Y:               evt.gamepad_evt.id = GAMEPAD_ID_R_UP;       break;
                case SDL_CONTROLLER_BUTTON_BACK:            evt.gamepad_evt.id = GAMEPAD_ID_MENU_LEFT;  break;
                case SDL_CONTROLLER_BUTTON_GUIDE:           evt.gamepad_evt.id = GAMEPAD_ID_MENU_MAIN;  break;
                case SDL_CONTROLLER_BUTTON_START:           evt.gamepad_evt.id = GAMEPAD_ID_MENU_RIGHT; break;
                case SDL_CONTROLLER_BUTTON_LEFTSTICK:       evt.gamepad_evt.id = GAMEPAD_ID_LS;         break;
                case SDL_CONTROLLER_BUTTON_RIGHTSTICK:      evt.gamepad_evt.id = GAMEPAD_ID_RS;         break;
                case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:    evt.gamepad_evt.id = GAMEPAD_ID_LB;         break;
                case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:   evt.gamepad_evt.id = GAMEPAD_ID_RB;         break;
                case SDL_CONTROLLER_BUTTON_DPAD_UP:         evt.gamepad_evt.id = GAMEPAD_ID_L_UP;       break;
                case SDL_CONTROLLER_BUTTON_DPAD_DOWN:       evt.gamepad_evt.id = GAMEPAD_ID_L_DOWN;     break;
                case SDL_CONTROLLER_BUTTON_DPAD_LEFT:       evt.gamepad_evt.id = GAMEPAD_ID_L_LEFT;     break;
                case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:      evt.gamepad_evt.id = GAMEPAD_ID_L_RIGHT;    break;
                default:                                    is_to_update = false;                       break;
                }
                break;
#endif
            }

            if (is_to_update) {
                is_to_update = false;

                __vsf_arch_irq_start(irq_thread);
                    switch (evt_type) {
                    case VSF_INPUT_TYPE_TOUCHSCREEN:
                        evt.ts_evt.info.width = disp_sdl2->param.width;
                        evt.ts_evt.info.height = disp_sdl2->param.height;
                        vsf_input_touchscreen_set(&evt.ts_evt, 0, is_down, pressure, x, y);
                        vsf_input_on_touchscreen(&evt.ts_evt);
                        break;
                    case VSF_INPUT_TYPE_GAMEPAD:
                        vsf_input_on_gamepad(&evt.gamepad_evt);
                        break;
                    case VSF_INPUT_TYPE_KEYBOARD:
                        vsf_input_on_keyboard(&evt.keyboard_evt);
                        break;
                    case VSF_INPUT_TYPE_MOUSE:
                        switch (mouse_evt) {
                        case VSF_INPUT_MOUSE_EVT_BUTTON:
                            vk_input_mouse_evt_button_set(&evt.mouse_evt, mouse_button, is_down, x, y);
                            break;
                        case VSF_INPUT_MOUSE_EVT_MOVE:
                            vk_input_mouse_evt_move_set(&evt.mouse_evt, x, y);
                            break;
                        case VSF_INPUT_MOUSE_EVT_WHEEL:
                            vk_input_mouse_evt_wheel_set(&evt.mouse_evt, wheel_x, wheel_y);
                            break;
                        }
                        vsf_input_on_mouse(&evt.mouse_evt);
                        break;
                    }
                __vsf_arch_irq_end(irq_thread, false);
            }
#endif
        }
    }
}






static vsf_err_t __vk_disp_sdl2_init(vk_disp_t *pthis)
{
    vk_disp_sdl2_t *disp_sdl2 = (vk_disp_sdl2_t *)pthis;
    VSF_UI_ASSERT(disp_sdl2 != NULL);

    if (!__vk_disp_sdl2.is_init_called) {
        __vk_disp_sdl2.is_init_called = true;
        __vk_disp_sdl2.disp = (vk_disp_sdl2_t *)pthis;
        __vsf_arch_irq_request_init(&disp_sdl2->flush_request);
        __vsf_arch_irq_init(&__vk_disp_sdl2.init_thread, "disp_sdl2_init", __vk_disp_sdl2_init_thread, VSF_DISP_SDL2_CFG_HW_PRIORITY);
    } else {
        vk_disp_on_ready(&disp_sdl2->use_as__vk_disp_t);
    }
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_disp_sdl2_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff)
{
    vk_disp_sdl2_t *disp_sdl2 = (vk_disp_sdl2_t *)pthis;
    VSF_UI_ASSERT(disp_sdl2 != NULL);

    disp_sdl2->area = *area;
    disp_sdl2->disp_buff = disp_buff;
    __vsf_arch_irq_request_send(&disp_sdl2->flush_request);
    return VSF_ERR_NONE;
}

#endif

/* EOF */
