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
    uint32_t                    usr_evttype;

    vsf_slist_queue_t           evt_list;
    vsf_eda_t                   *eda_pending;
} vsf_sdl2_event_t;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

static NO_INIT vsf_sdl2_event_t __vsf_sdl2_event;

static const SDL_Keycode SDL_default_keymap[SDL_NUM_SCANCODES] = {
    0, 0, 0, 0,
    'a',
    'b',
    'c',
    'd',
    'e',
    'f',
    'g',
    'h',
    'i',
    'j',
    'k',
    'l',
    'm',
    'n',
    'o',
    'p',
    'q',
    'r',
    's',
    't',
    'u',
    'v',
    'w',
    'x',
    'y',
    'z',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '0',
    SDLK_RETURN,
    SDLK_ESCAPE,
    SDLK_BACKSPACE,
    SDLK_TAB,
    SDLK_SPACE,
    '-',
    '=',
    '[',
    ']',
    '\\',
    '#',
    ';',
    '\'',
    '`',
    ',',
    '.',
    '/',
    SDLK_CAPSLOCK,
    SDLK_F1,
    SDLK_F2,
    SDLK_F3,
    SDLK_F4,
    SDLK_F5,
    SDLK_F6,
    SDLK_F7,
    SDLK_F8,
    SDLK_F9,
    SDLK_F10,
    SDLK_F11,
    SDLK_F12,
    SDLK_PRINTSCREEN,
    SDLK_SCROLLLOCK,
    SDLK_PAUSE,
    SDLK_INSERT,
    SDLK_HOME,
    SDLK_PAGEUP,
    SDLK_DELETE,
    SDLK_END,
    SDLK_PAGEDOWN,
    SDLK_RIGHT,
    SDLK_LEFT,
    SDLK_DOWN,
    SDLK_UP,
    SDLK_NUMLOCKCLEAR,
    SDLK_KP_DIVIDE,
    SDLK_KP_MULTIPLY,
    SDLK_KP_MINUS,
    SDLK_KP_PLUS,
    SDLK_KP_ENTER,
    SDLK_KP_1,
    SDLK_KP_2,
    SDLK_KP_3,
    SDLK_KP_4,
    SDLK_KP_5,
    SDLK_KP_6,
    SDLK_KP_7,
    SDLK_KP_8,
    SDLK_KP_9,
    SDLK_KP_0,
    SDLK_KP_PERIOD,
    0,
    SDLK_APPLICATION,
    SDLK_POWER,
    SDLK_KP_EQUALS,
    SDLK_F13,
    SDLK_F14,
    SDLK_F15,
    SDLK_F16,
    SDLK_F17,
    SDLK_F18,
    SDLK_F19,
    SDLK_F20,
    SDLK_F21,
    SDLK_F22,
    SDLK_F23,
    SDLK_F24,
    SDLK_EXECUTE,
    SDLK_HELP,
    SDLK_MENU,
    SDLK_SELECT,
    SDLK_STOP,
    SDLK_AGAIN,
    SDLK_UNDO,
    SDLK_CUT,
    SDLK_COPY,
    SDLK_PASTE,
    SDLK_FIND,
    SDLK_MUTE,
    SDLK_VOLUMEUP,
    SDLK_VOLUMEDOWN,
    0, 0, 0,
    SDLK_KP_COMMA,
    0,  // SDLK_KP_EQUALSAS400,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  // SDLK_ALTERASE,
    0,  // SDLK_SYSREQ,
    0,  // SDLK_CANCEL,
    0,  // SDLK_CLEAR,
    0,  // SDLK_PRIOR,
    0,  // SDLK_RETURN2,
    0,  // SDLK_SEPARATOR,
    0,  // SDLK_OUT,
    0,  // SDLK_OPER,
    0,  // SDLK_CLEARAGAIN,
    0,  // SDLK_CRSEL,
    0,  // SDLK_EXSEL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  // SDLK_KP_00,
    0,  // SDLK_KP_000,
    0,  // SDLK_THOUSANDSSEPARATOR,
    0,  // SDLK_DECIMALSEPARATOR,
    0,  // SDLK_CURRENCYUNIT,
    0,  // SDLK_CURRENCYSUBUNIT,
    0,  // SDLK_KP_LEFTPAREN,
    0,  // SDLK_KP_RIGHTPAREN,
    0,  // SDLK_KP_LEFTBRACE,
    0,  // SDLK_KP_RIGHTBRACE,
    0,  // SDLK_KP_TAB,
    0,  // SDLK_KP_BACKSPACE,
    0,  // SDLK_KP_A,
    0,  // SDLK_KP_B,
    0,  // SDLK_KP_C,
    0,  // SDLK_KP_D,
    0,  // SDLK_KP_E,
    0,  // SDLK_KP_F,
    0,  // SDLK_KP_XOR,
    0,  // SDLK_KP_POWER,
    0,  // SDLK_KP_PERCENT,
    0,  // SDLK_KP_LESS,
    0,  // SDLK_KP_GREATER,
    0,  // SDLK_KP_AMPERSAND,
    0,  // SDLK_KP_DBLAMPERSAND,
    0,  // SDLK_KP_VERTICALBAR,
    0,  // SDLK_KP_DBLVERTICALBAR,
    0,  // SDLK_KP_COLON,
    0,  // SDLK_KP_HASH,
    0,  // SDLK_KP_SPACE,
    0,  // SDLK_KP_AT,
    0,  // SDLK_KP_EXCLAM,
    0,  // SDLK_KP_MEMSTORE,
    0,  // SDLK_KP_MEMRECALL,
    0,  // SDLK_KP_MEMCLEAR,
    0,  // SDLK_KP_MEMADD,
    0,  // SDLK_KP_MEMSUBTRACT,
    0,  // SDLK_KP_MEMMULTIPLY,
    0,  // SDLK_KP_MEMDIVIDE,
    0,  // SDLK_KP_PLUSMINUS,
    0,  // SDLK_KP_CLEAR,
    0,  // SDLK_KP_CLEARENTRY,
    0,  // SDLK_KP_BINARY,
    0,  // SDLK_KP_OCTAL,
    0,  // SDLK_KP_DECIMAL,
    0,  // SDLK_KP_HEXADECIMAL,
    0, 0,
    SDLK_LCTRL,
    SDLK_LSHIFT,
    SDLK_LALT,
    SDLK_LGUI,
    SDLK_RCTRL,
    SDLK_RSHIFT,
    SDLK_RALT,
    SDLK_RGUI,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    SDLK_MODE,
    0,  // SDLK_AUDIONEXT,
    0,  // SDLK_AUDIOPREV,
    0,  // SDLK_AUDIOSTOP,
    0,  // SDLK_AUDIOPLAY,
    0,  // SDLK_AUDIOMUTE,
    0,  // SDLK_MEDIASELECT,
    0,  // SDLK_WWW,
    0,  // SDLK_MAIL,
    0,  // SDLK_CALCULATOR,
    0,  // SDLK_COMPUTER,
    0,  // SDLK_AC_SEARCH,
    0,  // SDLK_AC_HOME,
    0,  // SDLK_AC_BACK,
    0,  // SDLK_AC_FORWARD,
    0,  // SDLK_AC_STOP,
    0,  // SDLK_AC_REFRESH,
    0,  // SDLK_AC_BOOKMARKS,
    0,  // SDLK_BRIGHTNESSDOWN,
    0,  // SDLK_BRIGHTNESSUP,
    0,  // SDLK_DISPLAYSWITCH,
    0,  // SDLK_KBDILLUMTOGGLE,
    0,  // SDLK_KBDILLUMDOWN,
    0,  // SDLK_KBDILLUMUP,
    0,  // SDLK_EJECT,
    0,  // SDLK_SLEEP,
    0,  // SDLK_APP1,
    0,  // SDLK_APP2,
    0,  // SDLK_AUDIOREWIND,
    0,  // SDLK_AUDIOFASTFORWARD,
};

static const char * SDL_scancode_names[SDL_NUM_SCANCODES] = {
    NULL, NULL, NULL, NULL,
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    "R",
    "S",
    "T",
    "U",
    "V",
    "W",
    "X",
    "Y",
    "Z",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "0",
    "Return",
    "Escape",
    "Backspace",
    "Tab",
    "Space",
    "-",
    "=",
    "[",
    "]",
    "\\",
    "#",
    ";",
    "'",
    "`",
    ",",
    ".",
    "/",
    "CapsLock",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12",
    "PrintScreen",
    "ScrollLock",
    "Pause",
    "Insert",
    "Home",
    "PageUp",
    "Delete",
    "End",
    "PageDown",
    "Right",
    "Left",
    "Down",
    "Up",
    "Numlock",
    "Keypad /",
    "Keypad *",
    "Keypad -",
    "Keypad +",
    "Keypad Enter",
    "Keypad 1",
    "Keypad 2",
    "Keypad 3",
    "Keypad 4",
    "Keypad 5",
    "Keypad 6",
    "Keypad 7",
    "Keypad 8",
    "Keypad 9",
    "Keypad 0",
    "Keypad .",
    NULL,
    "Application",
    "Power",
    "Keypad =",
    "F13",
    "F14",
    "F15",
    "F16",
    "F17",
    "F18",
    "F19",
    "F20",
    "F21",
    "F22",
    "F23",
    "F24",
    "Execute",
    "Help",
    "Menu",
    "Select",
    "Stop",
    "Again",
    "Undo",
    "Cut",
    "Copy",
    "Paste",
    "Find",
    "Mute",
    "VolumeUp",
    "VolumeDown",
    NULL, NULL, NULL,
    "Keypad ,",
    "Keypad = (AS400)",
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL,
    "AltErase",
    "SysReq",
    "Cancel",
    "Clear",
    "Prior",
    "Return",
    "Separator",
    "Out",
    "Oper",
    "Clear / Again",
    "CrSel",
    "ExSel",
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    "Keypad 00",
    "Keypad 000",
    "ThousandsSeparator",
    "DecimalSeparator",
    "CurrencyUnit",
    "CurrencySubUnit",
    "Keypad (",
    "Keypad )",
    "Keypad {",
    "Keypad }",
    "Keypad Tab",
    "Keypad Backspace",
    "Keypad A",
    "Keypad B",
    "Keypad C",
    "Keypad D",
    "Keypad E",
    "Keypad F",
    "Keypad XOR",
    "Keypad ^",
    "Keypad %",
    "Keypad <",
    "Keypad >",
    "Keypad &",
    "Keypad &&",
    "Keypad |",
    "Keypad ||",
    "Keypad :",
    "Keypad #",
    "Keypad Space",
    "Keypad @",
    "Keypad !",
    "Keypad MemStore",
    "Keypad MemRecall",
    "Keypad MemClear",
    "Keypad MemAdd",
    "Keypad MemSubtract",
    "Keypad MemMultiply",
    "Keypad MemDivide",
    "Keypad +/-",
    "Keypad Clear",
    "Keypad ClearEntry",
    "Keypad Binary",
    "Keypad Octal",
    "Keypad Decimal",
    "Keypad Hexadecimal",
    NULL, NULL,
    "Left Ctrl",
    "Left Shift",
    "Left Alt",
    "Left GUI",
    "Right Ctrl",
    "Right Shift",
    "Right Alt",
    "Right GUI",
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL,
    "ModeSwitch",
    "AudioNext",
    "AudioPrev",
    "AudioStop",
    "AudioPlay",
    "AudioMute",
    "MediaSelect",
    "WWW",
    "Mail",
    "Calculator",
    "Computer",
    "AC Search",
    "AC Home",
    "AC Back",
    "AC Forward",
    "AC Stop",
    "AC Refresh",
    "AC Bookmarks",
    "BrightnessDown",
    "BrightnessUp",
    "DisplaySwitch",
    "KBDIllumToggle",
    "KBDIllumDown",
    "KBDIllumUp",
    "Eject",
    "Sleep",
    "App1",
    "App2",
    "AudioRewind",
    "AudioFastForward",
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

static uint16_t __vsf_sdl2_kb_parse_keymod(uint_fast16_t mod)
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
    if (mod & VSF_KM_NUMLOCK) {
        sdl_mod |= KMOD_NUM;
    }
    if (mod & VSF_KM_CAPSLOCK) {
        sdl_mod |= KMOD_CAPS;
    }
    return sdl_mod;
}

int SDL_PushEvent(SDL_Event *event)
{
    vsf_sdl2_event_node_t *node = vsf_heap_malloc(sizeof(*node));
    if (node != NULL) {
        vsf_slist_init_node(vsf_sdl2_event_node_t, evt_node, node);
        node->event = *event;

        vsf_eda_t *eda_pending;
        vsf_protect_t orig = vsf_protect_int();
            vsf_slist_queue_enqueue(vsf_sdl2_event_node_t, evt_node, &__vsf_sdl2_event.evt_list, node);
            eda_pending = __vsf_sdl2_event.eda_pending;
            __vsf_sdl2_event.eda_pending = NULL;
        vsf_unprotect_int(orig);

        if (eda_pending != NULL) {
            vsf_eda_post_evt(eda_pending, VSF_EVT_USER);
        }
        return 1;
    }
    return -1;
}

static void __vsf_sdl2_event_on_input(vk_input_notifier_t *notifier, vk_input_type_t type, vk_input_evt_t *evt)
{
    SDL_Event event = { 0 };
    char text_input = '\0';

    switch (type) {
    case VSF_INPUT_TYPE_KEYBOARD:
        if (vsf_input_keyboard_is_down(evt)) {
            event.type = SDL_KEYDOWN;
        } else {
            event.type = SDL_KEYUP;
        }

        uint16_t vsf_keycode = vsf_input_keyboard_get_keycode(evt);
        uint16_t mod = __vsf_sdl2_kb_parse_keymod(vsf_input_keyboard_get_keymod(evt));
        SDL_Keycode keycode;
        event.key.keysym.scancode = vsf_keycode & ~VSF_KB_EXT;
        event.key.keysym.sym = keycode = SDL_default_keymap[event.key.keysym.scancode];
        event.key.keysym.mod = mod;

        if (SDLK_UNKNOWN == keycode) {
            if (0 == mod) {
                return;
            }
        } else if ( (SDL_KEYUP == event.type)
                &&  !(mod & (KMOD_LCTRL | KMOD_LALT | KMOD_LGUI | KMOD_RCTRL | KMOD_RALT | KMOD_RGUI))
                &&  (keycode >= ' ') && (keycode <= '~')) {
            text_input = keycode;
            if (vsf_keycode & VSF_KB_EXT) {
                if ((text_input >= 'a') && (text_input <= 'z')) {
                    text_input -= 0x20;
                } else {
                    switch (text_input) {
                    case '`':   text_input = '~';   break;
                    case '1':   text_input = '!';   break;
                    case '2':   text_input = '@';   break;
                    case '3':   text_input = '#';   break;
                    case '4':   text_input = '$';   break;
                    case '5':   text_input = '%';   break;
                    case '6':   text_input = '^';   break;
                    case '7':   text_input = '&';   break;
                    case '8':   text_input = '*';   break;
                    case '9':   text_input = '(';   break;
                    case '0':   text_input = ')';   break;
                    case '-':   text_input = '_';   break;
                    case '=':   text_input = '+';   break;
                    case '[':   text_input = '{';   break;
                    case ']':   text_input = '}';   break;
                    case '\\':  text_input = '|';   break;
                    case ';':   text_input = ':';   break;
                    case '\'':  text_input = '\"';  break;
                    case ',':   text_input = '<';   break;
                    case '.':   text_input = '>';   break;
                    case '/':   text_input = '?';   break;
                    }
                }
            }
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

    SDL_PushEvent(&event);    

    if (text_input != '\0') {
        event.type = SDL_TEXTINPUT;
        event.text.text[0] = text_input;
        event.text.text[1] = '\0';
        text_input = '\0';
        SDL_PushEvent(&event);
    }
}

void __SDL_InitEvent(uint32_t flags)
{
    memset(&__vsf_sdl2_event, 0, sizeof(__vsf_sdl2_event));
    __vsf_sdl2_event.usr_evttype = SDL_USEREVENT;

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

int SDL_PeepEvents(SDL_Event *events, int numevents, SDL_eventaction action, uint32_t minType, uint32_t maxType)
{
    bool is_to_remove = true;
    int real_events = 0;
    VSF_SDL2_ASSERT(numevents >= 0);

    switch (action) {
    case SDL_ADDEVENT:
        for (real_events = 0; real_events < numevents; real_events++) {
            SDL_PushEvent(events++);
        }
        break;
    case SDL_PEEKEVENT:
        is_to_remove = false;
    case SDL_GETEVENT: {
            vsf_protect_t orig = vsf_protect_int();
            __vsf_slist_foreach_unsafe(vsf_sdl2_event_node_t, evt_node, (vsf_slist_t *)&__vsf_sdl2_event.evt_list.head) {
                if (numevents--) {
                    if ((_->event.type >= minType) && (_->event.type <= maxType)) {
                        real_events++;
                        *events++ = _->event;
                        if (is_to_remove) {
                            _->event.type = SDL_DUMMYEVENT;
                        }
                    }
                } else {
                    break;
                }
            }
            vsf_unprotect_int(orig);
        }
    }
    return real_events;
}

int SDL_PollEvent(SDL_Event *event)
{
    vsf_sdl2_event_node_t *node;
poll_next:
    {
        vsf_protect_t orig = vsf_protect_int();
            vsf_slist_queue_dequeue(vsf_sdl2_event_node_t, evt_node, &__vsf_sdl2_event.evt_list, node);
        vsf_unprotect_int(orig);
    }

    if (node != NULL) {
        if (event != NULL) {
            *event = node->event;
        }
        vsf_heap_free(node);
        if (SDL_DUMMYEVENT == node->event.type) {
            goto poll_next;
        }
        return 1;
    }
    return 0;
}

int SDL_WaitEventTimeout(SDL_Event * event, int timeout)
{
    vsf_eda_t *eda = vsf_eda_get_cur();
    bool is_empty;

    vsf_protect_t orig = vsf_protect_int();
        is_empty = vsf_slist_queue_is_empty(&__vsf_sdl2_event.evt_list);
        if (is_empty && timeout > 0) {
            SECTION(".text.vsf.kernel.vsf_sync")
            vsf_eda_t * __vsf_eda_set_timeout(vsf_eda_t *eda, vsf_timeout_tick_t timeout);

            __vsf_sdl2_event.eda_pending = vsf_eda_get_cur();
            __vsf_eda_set_timeout(eda, vsf_systimer_ms_to_tick(timeout));
        }
    vsf_unprotect_int(orig);

    if (is_empty && timeout > 0) {
        vsf_evt_t evt = vsf_thread_wait();
        VSF_UNUSED_PARAM(evt);

        SECTION(".text.vsf.kernel.__vsf_teda_cancel_timer")
        vsf_err_t __vsf_teda_cancel_timer(vsf_teda_t *this_ptr);
        __vsf_teda_cancel_timer((vsf_teda_t *)eda);

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

uint32_t SDL_RegisterEvents(int numevents)
{
    uint32_t evttype = __vsf_sdl2_event.usr_evttype;
    if ((evttype + numevents) > SDL_LASTEVENT) {
        return (uint32_t)-1;
    }
    __vsf_sdl2_event.usr_evttype += numevents;
    return evttype;
}

uint8_t SDL_EventState(uint32_t type, int state)
{
    // TODO: do any thing man
    return SDL_DISABLE;
}

void SDL_PumpEvents(void)
{
}

const char * SDL_GetScancodeName(SDL_Scancode scancode)
{
    if (scancode < SDL_SCANCODE_UNKNOWN || scancode >= SDL_NUM_SCANCODES) {
        return "";
    }

    const char *name = SDL_scancode_names[scancode];
    return name ? name : "";
}

char * SDL_UCS4ToUTF8(uint32_t ch, char *dst)
{
    uint8_t *p = (uint8_t *) dst;
    if (ch <= 0x7F) {
        *p = (uint8_t) ch;
        ++dst;
    } else if (ch <= 0x7FF) {
        p[0] = 0xC0 | (uint8_t) ((ch >> 6) & 0x1F);
        p[1] = 0x80 | (uint8_t) (ch & 0x3F);
        dst += 2;
    } else if (ch <= 0xFFFF) {
        p[0] = 0xE0 | (uint8_t) ((ch >> 12) & 0x0F);
        p[1] = 0x80 | (uint8_t) ((ch >> 6) & 0x3F);
        p[2] = 0x80 | (uint8_t) (ch & 0x3F);
        dst += 3;
    } else {
        p[0] = 0xF0 | (uint8_t) ((ch >> 18) & 0x07);
        p[1] = 0x80 | (uint8_t) ((ch >> 12) & 0x3F);
        p[2] = 0x80 | (uint8_t) ((ch >> 6) & 0x3F);
        p[3] = 0x80 | (uint8_t) (ch & 0x3F);
        dst += 4;
    }
    return dst;
}

const char * SDL_GetKeyName(SDL_Keycode key)
{
    static char name[8];
    char *end;

    if (key & SDLK_SCANCODE_MASK) {
        return SDL_GetScancodeName((SDL_Scancode)(key & ~SDLK_SCANCODE_MASK));
    }

    switch (key) {
    case SDLK_RETURN:       return SDL_GetScancodeName(SDL_SCANCODE_RETURN);
    case SDLK_ESCAPE:       return SDL_GetScancodeName(SDL_SCANCODE_ESCAPE);
    case SDLK_BACKSPACE:    return SDL_GetScancodeName(SDL_SCANCODE_BACKSPACE);
    case SDLK_TAB:          return SDL_GetScancodeName(SDL_SCANCODE_TAB);
    case SDLK_SPACE:        return SDL_GetScancodeName(SDL_SCANCODE_SPACE);
    default:
        if (key >= 'a' && key <= 'z') {
            key -= 32;
        }
        end = SDL_UCS4ToUTF8((uint32_t) key, name);
        *end = '\0';
        return name;
    }
}

#endif      // VSF_USE_SDL2
