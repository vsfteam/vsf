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

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_USE_DISP_DRV_SDL2 == ENABLED

#define __VSF_DISP_SDL2_CLASS_IMPLEMENT
#define __VSF_DISP_CLASS_INHERIT
// TODO: use dedicated include
#include "vsf.h"

/*============================ MACROS ========================================*/

#ifndef VSF_DISP_DRV_SDL2_CFG_HW_PRIORITY
#   define VSF_DISP_DRV_SDL2_CFG_HW_PRIORITY            vsf_arch_prio_0
#endif

#if VSF_USE_INPUT == ENABLED
#if     defined(WEAK_VSF_INPUT_ON_TOUCHSCREEN_EXTERN)                           \
    &&  defined(WEAK_VSF_INPUT_ON_TOUCHSCREEN)
WEAK_VSF_INPUT_ON_TOUCHSCREEN_EXTERN
#endif

#if     defined(WEAK_VSF_INPUT_ON_GAMEPAD_EXTERN)                               \
    &&  defined(WEAK_VSF_INPUT_ON_GAMEPAD)
WEAK_VSF_INPUT_ON_GAMEPAD_EXTERN
#endif

#if     defined(WEAK_VSF_INPUT_ON_KEYBOARD_EXTERN)                              \
    &&  defined(WEAK_VSF_INPUT_ON_KEYBOARD)
WEAK_VSF_INPUT_ON_KEYBOARD_EXTERN
#endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct __vk_disp_sdl2_t {
    vsf_arch_irq_thread_t init_thread;
    bool is_init_called;
    bool is_inited;
};
typedef struct __vk_disp_sdl2_t __vk_disp_sdl2_t;

/*============================ LOCAL VARIABLES ===============================*/

static __vk_disp_sdl2_t __vk_disp_sdl2 = {
    .is_init_called = false,
    .is_inited      = false,
};

/*============================ PROTOTYPES ====================================*/

static vsf_err_t vk_disp_sdl2_init(vk_disp_t *pthis);
static vsf_err_t vk_disp_sdl2_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff);

extern void vsf_input_on_mouse(vk_mouse_evt_t *mouse_evt);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_disp_drv_t vk_disp_drv_sdl2 = {
    .init       = vk_disp_sdl2_init,
    .refresh    = vk_disp_sdl2_refresh,
};

/*============================ IMPLEMENTATION ================================*/

static void __vk_disp_sdl2_common_init(void)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
}

static void __vk_disp_sdl2_common_fini(void)
{
    SDL_Quit();
}

static Uint32 __vk_disp_sdl2_get_format(vk_disp_sdl2_t *disp_sdl2)
{
    switch (disp_sdl2->param.color) {
    default:
    case VSF_DISP_COLOR_RGB565:     return SDL_PIXELFORMAT_RGB565;
    case VSF_DISP_COLOR_ARGB8888:   return SDL_PIXELFORMAT_ARGB8888;
    }
}

static uint_fast8_t __vk_disp_sdl2_get_pixel_size(vk_disp_sdl2_t *disp_sdl2)
{
    return VSF_DISP_GET_PIXEL_SIZE(disp_sdl2);
}

static void __vk_disp_sdl2_screen_init(vk_disp_sdl2_t *disp_sdl2)
{
    disp_sdl2->window = SDL_CreateWindow("Screen",
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
        disp_sdl2->area.size.x * __vk_disp_sdl2_get_pixel_size(disp_sdl2) / 8);
    SDL_RenderClear(disp_sdl2->renderer);
    SDL_RenderCopy(disp_sdl2->renderer, disp_sdl2->texture, NULL, NULL);
    SDL_RenderPresent(disp_sdl2->renderer);
}

static void __vk_disp_sdl2_init_thread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = arg;

    __vsf_arch_irq_set_background(irq_thread);
        __vk_disp_sdl2_common_init();
        __vk_disp_sdl2.is_inited = true;
    __vsf_arch_irq_fini(irq_thread);
}

static void __vk_disp_sdl2_flush_thread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = arg;
    vk_disp_sdl2_t *disp_sdl2 = container_of(irq_thread, vk_disp_sdl2_t, flush_thread);

    __vsf_arch_irq_set_background(irq_thread);

    while (1) {
        __vsf_arch_irq_request_pend(&disp_sdl2->flush_request);
        __vk_disp_sdl2_screen_update(disp_sdl2);

        if (disp_sdl2->flush_delay_ms > 0) {
            Sleep(disp_sdl2->flush_delay_ms);
        }

        __vsf_arch_irq_start(irq_thread);
            vk_disp_on_ready(&disp_sdl2->use_as__vk_disp_t);
        __vsf_arch_irq_end(irq_thread, false);
    }
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
    bool is_down = false, is_to_update = false;
    int_fast16_t wheel_x = 0, wheel_y = 0;
    int mouse_evt = 0, mouse_button = 0;
#endif

    while (!__vk_disp_sdl2.is_inited) {
        Sleep(100);
    }

    __vsf_arch_irq_set_background(irq_thread);
        __vk_disp_sdl2_screen_init(disp_sdl2);

    __vsf_arch_irq_init(&disp_sdl2->flush_thread, __vk_disp_sdl2_flush_thread, VSF_DISP_DRV_SDL2_CFG_HW_PRIORITY, true);

    while (1) {
        if (SDL_WaitEvent(&event)) {
#if VSF_USE_INPUT == ENABLED
            switch (event.type) {
#   if VSF_DISP_DRV_SDL2_CFG_MOUSE_AS_TOUCHSCREEN == ENABLED
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
#else
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
#endif

            case SDL_KEYDOWN:
            case SDL_KEYUP:
                if (!event.key.repeat) {
                    is_to_update = true;
                    evt_type = VSF_INPUT_TYPE_KEYBOARD;
                    // TODO: remap event.key.sym to keycode in VSF
                    vsf_input_keyboard_set(&evt.keyboard_evt, event.key.keysym.sym, SDL_KEYDOWN == event.type);
                }
                break;
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
            }

            if (is_to_update) {
                is_to_update = false;
                evt.dev = disp_sdl2;

                __vsf_arch_irq_start(irq_thread);
                    switch (evt_type) {
                    case VSF_INPUT_TYPE_TOUCHSCREEN:
                        evt.ts_evt.info.width = disp_sdl2->param.width;
                        evt.ts_evt.info.height = disp_sdl2->param.height;
                        vsf_input_touchscreen_set(&evt.ts_evt, 0, is_down, x, y);
#ifndef WEAK_VSF_INPUT_ON_TOUCHSCREEN
                        vsf_input_on_touchscreen(&evt.ts_evt);
#else
                        WEAK_VSF_INPUT_ON_TOUCHSCREEN(&evt.ts_evt);
#endif
                        break;
                    case VSF_INPUT_TYPE_GAMEPAD:
#ifndef WEAK_VSF_INPUT_ON_GAMEPAD
                        vsf_input_on_gamepad(&evt.gamepad_evt);
#else
                        WEAK_VSF_INPUT_ON_GAMEPAD(&evt.gamepad_evt);
#endif
                        break;
                    case VSF_INPUT_TYPE_KEYBOARD:
#ifndef WEAK_VSF_INPUT_ON_KEYBOARD
                        vsf_input_on_keyboard(&evt.keyboard_evt);
#else
                        WEAK_VSF_INPUT_ON_KEYBOARD(&evt.keyboard_evt);
#endif
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






static vsf_err_t vk_disp_sdl2_init(vk_disp_t *pthis)
{
    vk_disp_sdl2_t *disp_sdl2 = (vk_disp_sdl2_t *)pthis;
    VSF_UI_ASSERT(disp_sdl2 != NULL);

    if (!__vk_disp_sdl2.is_init_called) {
        __vk_disp_sdl2.is_init_called = true;
        __vk_disp_sdl2.init_thread.name = "disp_sdl2_init";
        __vsf_arch_irq_init(&__vk_disp_sdl2.init_thread, __vk_disp_sdl2_init_thread, VSF_DISP_DRV_SDL2_CFG_HW_PRIORITY, true);
    }

    disp_sdl2->event_thread.name = "disp_sdl2_event";
    __vsf_arch_irq_init(&disp_sdl2->event_thread, __vk_disp_sdl2_event_thread, VSF_DISP_DRV_SDL2_CFG_HW_PRIORITY, true);

    __vsf_arch_irq_request_init(&disp_sdl2->flush_request);
    disp_sdl2->flush_thread.name = "disp_sdl2_flush";
    return VSF_ERR_NONE;
}

static vsf_err_t vk_disp_sdl2_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff)
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
