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

#include "./vsf_sdl2_cfg.h"

#if VSF_USE_SDL2 == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#include "./include/SDL2/SDL.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER != ENABLED
#   error please enable VSF_KERNEL_CFG_EDA_SUPPORT_TIMER
#endif

#if VSF_KERNEL_CFG_SUPPORT_SYNC != ENABLED
#   error please enable VSF_KERNEL_CFG_SUPPORT_SYNC
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_sdl2_event_node_t {
    vsf_slist_node_t            evt_node;
    SDL_Event                   event;
} vsf_sdl2_event_node_t;

typedef struct vsf_sdl2_event_t {
    uint32_t                    evtflags;
    vk_input_notifier_t         notifier;

    vsf_slist_queue_t           evt_list;
    vsf_eda_t                   *eda_pending;
} vsf_sdl2_event_t;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

static NO_INIT vsf_sdl2_event_t __vsf_sdl2_event;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

static SDL_Keycode __vsf_sdl2_kb_parse_keycode(uint_fast32_t code)
{
    bool is_ext = (code & VSF_KB_EXT) != 0;

    code &= ~VSF_KB_EXT;
    if ((code >= VSF_KB_a) && (code <= VSF_KB_z)) {
        return SDLK_a + (code - VSF_KB_a);
    } else if ((code >= VSF_KB_F1) && (code <= VSF_KB_F12)) {
        return SDLK_F1 + (code - VSF_KB_F1);
    } else if (code == VSF_KB_0) {
        return SDLK_0;
    } else if ((code >= VSF_KB_1) && (code <= VSF_KB_0)) {
        return SDLK_1 + (code - VSF_KB_1);
    } else if (code == VSF_KP_0) {
        return SDLK_KP_0;
    } else if ((code >= VSF_KP_1) && (code <= VSF_KP_0)) {
        return SDLK_1 + (code - VSF_KP_1);
    } else if (is_ext) {
        switch (code | VSF_KB_EXT) {
        case VSF_KB_EXCLAM:             return SDLK_EXCLAIM;
        case VSF_KB_AT:                 return SDLK_AT;
        case VSF_KB_POUND:              return SDLK_HASH;
        case VSF_KB_DOLLAR:             return SDLK_DOLLAR;
        case VSF_KB_PERCENT:            return SDLK_PERCENT;
        case VSF_KB_CARET:              return SDLK_CARET;
        case VSF_KB_AMPERSAND:          return SDLK_AMPERSAND;
        case VSF_KB_ASTERISK:           return SDLK_ASTERISK;
        case VSF_KB_LEFT_PAREN:         return SDLK_LEFTPAREN;
        case VSF_KB_RIGHT_PAREN:        return SDLK_RIGHTPAREN;
        case VSF_KB_UNDERSCORE:         return SDLK_UNDERSCORE;
        case VSF_KB_PLUS:               return SDLK_PLUS;
//        case VSF_KB_LEFT_BRACE:         return SDLK_LEFTBRACE;
//        case VSF_KB_RIGHT_BRACE:        return SDLK_RIGHTBRACE;
        case VSF_KB_COLON:              return SDLK_COLON;
        case VSF_KB_DOUBLE_QUOTE:       return SDLK_QUOTEDBL;
//        case VSF_KB_TIDE:               return SDLK_TIDE;
        case VSF_KB_LESS:               return SDLK_LESS;
        case VSF_KB_GREATER:            return SDLK_GREATER;
        case VSF_KB_QUESTION:           return SDLK_QUESTION;
        }
    } else {
        switch (code) {
        case VSF_KB_ENTER:              return SDLK_RETURN;
        case VSF_KB_ESCAPE:             return SDLK_ESCAPE;
        case VSF_KB_BACKSPACE:          return SDLK_BACKSPACE;
        case VSF_KB_TAB:                return SDLK_TAB;
        case VSF_KB_SPACE:              return SDLK_SPACE;
        case VSF_KB_MINUS:              return SDLK_MINUS;
        case VSF_KB_EQUAL:              return SDLK_EQUALS;
        case VSF_KB_LEFT_BRACKET:       return SDLK_LEFTBRACKET;
        case VSF_KB_RIGHT_BRACKET:      return SDLK_RIGHTBRACKET;
        case VSF_KB_BACKSLASH:          return SDLK_BACKSLASH;
        case VSF_KB_SEMICOLON:          return SDLK_SEMICOLON;
        case VSF_KB_SINGLE_QUOTE:       return SDLK_QUOTE;
        case VSF_KB_GRAVE:              return SDLK_BACKQUOTE;
        case VSF_KB_COMMA:              return SDLK_COMMA;
        case VSF_KB_DOT:                return SDLK_PERIOD;
        case VSF_KB_SLASH:              return SDLK_SLASH;
        case VSF_KB_CAPSLOCK:           return SDLK_CAPSLOCK;
//        case VSF_KB_PRINT_SCREEN:       return SDLK_PRINT_SCREEN;
//        case VSF_KB_SCROLL_LOCK:        return SDLK_SCROOL_LOCK;
        case VSF_KB_PAUSE:              return SDLK_PAUSE;
        case VSF_KB_INSERT:             return SDLK_INSERT;
        case VSF_KB_HOME:               return SDLK_HOME;
        case VSF_KB_PAGE_UP:            return SDLK_PAGEUP;
        case VSF_KB_DELETE:             return SDLK_DELETE;
        case VSF_KB_END:                return SDLK_END;
        case VSF_KB_PAGE_DOWN:          return SDLK_PAGEDOWN;
        case VSF_KB_RIGHT:              return SDLK_RIGHT;
        case VSF_KB_LEFT:               return SDLK_LEFT;
        case VSF_KB_DOWN:               return SDLK_DOWN;
        case VSF_KB_UP:                 return SDLK_UP;

//        case VSF_KP_NUMLOCK:            return SDLK_NUMLOCKCLEAR;
        case VSF_KP_DIVIDE:             return SDLK_KP_DIVIDE;
        case VSF_KP_MULTIPLY:           return SDLK_KP_MULTIPLY;
        case VSF_KP_MINUS:              return SDLK_KP_MINUS;
        case VSF_KP_PLUS:               return SDLK_KP_PLUS;
        case VSF_KP_ENTER:              return SDLK_KP_ENTER;
        case VSF_KP_DOT:                return SDLK_KP_PERIOD;
        case VSF_KP_EQUAL:              return SDLK_KP_EQUALS;
        }
    }
    return SDLK_UNKNOWN;
}

static uint16_t __vsf_sdl2_kb_parse_keymod(uint_fast32_t mod)
{
    uint16_t sdl_mod = KMOD_NONE;

    if (mod & VSF_KM_LEFT_SHIFT) {
        sdl_mod |= KMOD_LSHIFT;
    }
    if (mod & VSF_KM_LEFT_CTRL) {
        sdl_mod |= KMOD_LCTRL;
    }
    if (mod & VSF_KM_LEFT_ALT) {
        sdl_mod |= KMOD_LALT;
    }
    if (mod & VSF_KM_LEFT_GUI) {
        sdl_mod |= KMOD_LGUI;
    }
    if (mod & VSF_KM_RIGHT_SHIFT) {
        sdl_mod |= KMOD_RSHIFT;
    }
    if (mod & VSF_KM_RIGHT_CTRL) {
        sdl_mod |= KMOD_RCTRL;
    }
    if (mod & VSF_KM_RIGHT_ALT) {
        sdl_mod |= KMOD_RALT;
    }
    if (mod & VSF_KM_RIGHT_GUI) {
        sdl_mod |= KMOD_RGUI;
    }
    return sdl_mod;
}

static void __vsf_sdl2_event_on_input(vk_input_type_t type, vk_input_evt_t *evt)
{
    SDL_Event event = { 0 };

    switch (type) {
    case VSF_INPUT_TYPE_KEYBOARD:
        if (vsf_input_keyboard_is_down(evt)) {
            event.type = SDL_KEYDOWN;
        } else {
            event.type = SDL_KEYUP;
        }
        event.key.keysym.sym = __vsf_sdl2_kb_parse_keycode(vsf_input_keyboard_get_keycode(evt));
        event.key.keysym.mod = __vsf_sdl2_kb_parse_keymod(vsf_input_keyboard_get_keymod(evt));

        if (SDLK_UNKNOWN == event.key.keysym.sym) {
            return;
        }
        break;
    case VSF_INPUT_TYPE_MOUSE:
        switch (vk_input_mouse_evt_get(evt)) {
        case VSF_INPUT_MOUSE_EVT_MOVE:
            event.type = SDL_MOUSEMOTION;
            event.motion.x = vk_input_mouse_evt_get_x(evt);
            event.motion.y = vk_input_mouse_evt_get_y(evt);
            // TODO: fix relative motion values
            event.motion.xrel = event.motion.yrel = 0;
            break;
        case VSF_INPUT_MOUSE_EVT_BUTTON:
            if (vk_input_mouse_evt_button_is_down(evt)) {
                event.type = SDL_MOUSEBUTTONDOWN;
            } else {
                event.type = SDL_MOUSEBUTTONUP;
            }
            event.button.x = vk_input_mouse_evt_get_x(evt);
            event.button.y = vk_input_mouse_evt_get_y(evt);
            event.button.clicks = 1;
            break;
        case VSF_INPUT_MOUSE_EVT_WHEEL:
            event.type = SDL_MOUSEWHEEL;
            event.wheel.x = vk_input_mouse_evt_get_x(evt);
            event.wheel.y = vk_input_mouse_evt_get_y(evt);
            // TODO: initialize more members in wheel
            break;
        }
        break;
    default:
        return;
    }

    vsf_sdl2_event_node_t *node = vsf_heap_malloc(sizeof(*node));
    if (node != NULL) {
        vsf_slist_init_node(vsf_sdl2_event_node_t, evt_node, node);
        node->event = event;

        vsf_eda_t *eda_pending;
        vsf_protect_t orig = vsf_protect_int();
            vsf_slist_queue_enqueue(vsf_sdl2_event_node_t, evt_node, &__vsf_sdl2_event.evt_list, node);
            eda_pending = __vsf_sdl2_event.eda_pending;
            __vsf_sdl2_event.eda_pending = NULL;
        vsf_unprotect_int(orig);

        if (eda_pending != NULL) {
            vsf_eda_post_evt(eda_pending, VSF_EVT_USER);
        }
    }
}

void __SDL_InitEvent(uint32_t flags)
{
    memset(&__vsf_sdl2_event, 0, sizeof(__vsf_sdl2_event));

    __vsf_sdl2_event.evtflags = flags;
    vsf_slist_queue_init(&__vsf_sdl2_event.evt_list);

    __vsf_sdl2_event.notifier.mask =    (1 << VSF_INPUT_TYPE_TOUCHSCREEN)
                                    |   (1 << VSF_INPUT_TYPE_KEYBOARD)
                                    |   (1 << VSF_INPUT_TYPE_MOUSE);
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
    vsf_sdl2_event_node_t *node;
    vsf_protect_t orig = vsf_protect_int();
        vsf_slist_queue_dequeue(vsf_sdl2_event_node_t, evt_node, &__vsf_sdl2_event.evt_list, node);
    vsf_unprotect_int(orig);

    if (node != NULL) {
        if (event != NULL) {
            *event = node->event;
        }
        vsf_heap_free(node);
        return 1;
    }
    return 0;
}

int SDL_WaitEventTimeout(SDL_Event * event, int timeout)
{
    vsf_eda_t *eda = vsf_eda_get_cur();
    bool is_empty;

    vsf_protect_t orig = vsf_protect_int();
        is_empty == vsf_slist_queue_is_empty(&__vsf_sdl2_event.evt_list);
        if (is_empty) {
            __vsf_sdl2_event.eda_pending = vsf_eda_get_cur();
            if (timeout > 0) {
                SECTION(".text.vsf.kernel.vsf_sync")
                vsf_eda_t * __vsf_eda_set_timeout(vsf_eda_t *eda, vsf_timeout_tick_t timeout);

                __vsf_eda_set_timeout(eda, vsf_systimer_ms_to_tick(timeout));
            }
        }
    vsf_unprotect_int(orig);

    if (is_empty) {
        vsf_evt_t evt = vsf_thread_wait();

        SECTION(".text.vsf.kernel.__vsf_teda_cancel_timer")
        vsf_err_t __vsf_teda_cancel_timer(vsf_teda_t *this_ptr);
        __vsf_teda_cancel_timer(eda);

        orig = vsf_protect_int();
            __vsf_sdl2_event.eda_pending = NULL;
        vsf_unprotect_int(orig);
        eda->flag.state.is_limitted = false;
    }

    return event != NULL ? SDL_PollEvent(event) : 0;
}

int SDL_WaitEvent(SDL_Event * event)
{
    return SDL_WaitEventTimeout(event, -1);
}

void SDL_FlushEvent(uint32_t type)
{
    vsf_sdl2_event_node_t *node;
    vsf_protect_t orig;

    do {
        orig = vsf_protect_int();
            vsf_slist_queue_dequeue(vsf_sdl2_event_node_t, evt_node, &__vsf_sdl2_event.evt_list, node);
        vsf_unprotect_int(orig);

        if (node != NULL) {
            vsf_heap_free(node);
        }
    } while (node != NULL);
}

uint8_t SDL_EventState(uint32_t type, int state)
{
    // TODO: do any thing man
    return SDL_DISABLE;
}

void SDL_PumpEvents(void)
{
}

const char * SDL_GetKeyName(SDL_Keycode key)
{
    static char __keyname[2] = { 0 };
    if (key < 256) {
        __keyname[0] = key;
        return __keyname;
    }
    return "";
}

#endif      // VSF_USE_SDL2
