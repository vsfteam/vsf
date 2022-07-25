#ifndef __VSF_LINUX_INPUT_H__
#define __VSF_LINUX_INPUT_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./sys/types.h"
#   include "./sys/time.h"
#   include "./sys/ioctl.h"
#else
#   include <sys/types.h>
#   include <sys/time.h>
#   include <sys/ioctl.h>
#endif
#include <stdint.h>
#include "component/input/vsf_input.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EV_VERSION                  0x010001

// event types
#define EV_SYNC                     VSF_INPUT_TYPE_SYNC
#define EV_KEY                      VSF_INPUT_TYPE_KEYBOARD
#define EV_MAX                      0x1F
#define EV_CNT                      (EV_MAX + 1)

// event codes
// VSF_INPUT_TYPE_KEYBOARD
#define KEY_RESERVED                VSF_KB_UNKNOWN
#define KEY_ESC                     VSF_KB_ESCAPE
#define KEY_1                       VSF_KB_1
#define KEY_2                       VSF_KB_2
#define KEY_3                       VSF_KB_3
#define KEY_4                       VSF_KB_4
#define KEY_5                       VSF_KB_5
#define KEY_6                       VSF_KB_6
#define KEY_7                       VSF_KB_7
#define KEY_8                       VSF_KB_8
#define KEY_9                       VSF_KB_9
#define KEY_0                       VSF_KB_0
#define KEY_A                       VSF_KB_a
#define KEY_B                       VSF_KB_b
#define KEY_C                       VSF_KB_c
#define KEY_D                       VSF_KB_d
#define KEY_E                       VSF_KB_e
#define KEY_F                       VSF_KB_f
#define KEY_G                       VSF_KB_g
#define KEY_H                       VSF_KB_h
#define KEY_I                       VSF_KB_i
#define KEY_J                       VSF_KB_j
#define KEY_K                       VSF_KB_k
#define KEY_L                       VSF_KB_l
#define KEY_M                       VSF_KB_m
#define KEY_N                       VSF_KB_n
#define KEY_O                       VSF_KB_o
#define KEY_P                       VSF_KB_p
#define KEY_Q                       VSF_KB_q
#define KEY_R                       VSF_KB_r
#define KEY_S                       VSF_KB_s
#define KEY_T                       VSF_KB_t
#define KEY_U                       VSF_KB_u
#define KEY_V                       VSF_KB_v
#define KEY_W                       VSF_KB_w
#define KEY_X                       VSF_KB_x
#define KEY_Y                       VSF_KB_y
#define KEY_Z                       VSF_KB_z
#define KEY_MINUS                   VSF_KB_MINUS
#define KEY_EQUAL                   VSF_KB_EQUAL
#define KEY_BACKSPACE               VSF_KB_BACKSPACE
#define KEY_TAB                     VSF_KB_TAB
#define KEY_LEFTBRACE               VSF_KB_LEFT_BRACE
#define KEY_RIGHTBRACE              VSF_KB_RIGHT_BRACE
#define KEY_ENTER                   VSF_KB_ENTER
#define KEY_SEMICOLON               VSF_KB_SEMICOLON
#define KEY_APOSTROPHE              VSF_KB_SINGLE_QUOTE
#define KEY_GRAVE                   VSF_KB_GRAVE
#define KEY_BACKSLASH               VSF_KB_BACKSLASH
#define KEY_COMMA                   VSF_KB_COMMA
#define KEY_DOT                     VSF_KB_DOT
#define KEY_SLASH                   VSF_KB_SLASH
#define KEY_SPACE                   VSF_KB_SPACE
#define KEY_CAPSLOCK                VSF_KB_CAPSLOCK
#define KEY_NUMLOCK                 VSF_KP_NUMLOCK
#define KEY_F1                      VSF_KB_F1
#define KEY_F2                      VSF_KB_F2
#define KEY_F3                      VSF_KB_F3
#define KEY_F4                      VSF_KB_F4
#define KEY_F5                      VSF_KB_F5
#define KEY_F6                      VSF_KB_F6
#define KEY_F7                      VSF_KB_F7
#define KEY_F8                      VSF_KB_F8
#define KEY_F9                      VSF_KB_F9
#define KEY_F10                     VSF_KB_F10
#define KEY_F11                     VSF_KB_F11
#define KEY_F12                     VSF_KB_F12
#define KEY_F13                     VSF_KB_F13
#define KEY_F14                     VSF_KB_F14
#define KEY_F15                     VSF_KB_F15
#define KEY_F16                     VSF_KB_F16
#define KEY_F17                     VSF_KB_F17
#define KEY_F18                     VSF_KB_F18
#define KEY_F19                     VSF_KB_F19
#define KEY_F20                     VSF_KB_F20
#define KEY_F21                     VSF_KB_F21
#define KEY_F22                     VSF_KB_F22
#define KEY_F23                     VSF_KB_F23
#define KEY_F24                     VSF_KB_F24
#define KEY_LEFTCTRL                VSF_KB_LCTRL
#define KEY_LEFTSHIFT               VSF_KB_LSHIFT
#define KEY_LEFTALT                 VSF_KB_LALT
#define KEY_LEFTMETA                VSF_KB_LGUI
#define KEY_RIGHTCTRL               VSF_KB_RCTRL
#define KEY_RIGHTSHIFT              VSF_KB_RSHIFT
#define KEY_RIGHTALT                VSF_KB_RALT
#define KEY_RIGHTMETA               VSF_KB_RGUI
#define KEY_HOME                    VSF_KB_HOME
#define KEY_UP                      VSF_KB_UP
#define KEY_DOWN                    VSF_KB_DOWN
#define KEY_LEFT                    VSF_KB_LEFT
#define KEY_RIGHT                   VSF_KB_RIGHT
#define KEY_PAGEUP                  VSF_KB_PAGE_UP
#define KEY_PAGEDOWN                VSF_KB_PAGE_DOWN
#define KEY_INSERT                  VSF_KB_INSERT
#define KEY_DELETE                  VSF_KB_DELETE
#define KEY_END                     VSF_KB_END
#define KEY_MUTE                    VSF_KB_MUTE
#define KEY_VOLUMEUP                VSF_KB_VOLUMEUP
#define KEY_VOLUMEDOWN              VSF_KB_VOLUMEDOWN
#define KEY_POWER                   VSF_KB_POWER
#define KEY_PAUSE                   VSF_KB_PAUSE
#define KEY_STOP                    VSF_KB_STOP
#define KEY_AGAIN                   VSF_KB_AGAIN
#define KEY_UNDO                    VSF_KB_UNDO
#define KEY_COPY                    VSF_KB_COPY
#define KEY_PASTE                   VSF_KB_PASTE
#define KEY_CUT                     VSF_KB_CUT
#define KEY_FIND                    VSF_KB_FIND
#define KEY_HELP                    VSF_KB_HELP
#define KEY_MENU                    VSF_KB_MENU
#define KEY_SLEEP                   VSF_KB_SLEEP

#define KEY_KP1                     VSF_KP_1
#define KEY_KP2                     VSF_KP_2
#define KEY_KP3                     VSF_KP_3
#define KEY_KP4                     VSF_KP_4
#define KEY_KP5                     VSF_KP_5
#define KEY_KP6                     VSF_KP_6
#define KEY_KP7                     VSF_KP_7
#define KEY_KP8                     VSF_KP_8
#define KEY_KP9                     VSF_KP_9
#define KEY_KP0                     VSF_KP_0
#define KEY_KPASTERISK              VSF_KP_ASTERISK
#define KEY_KPMINUS                 VSF_KP_MINUS
#define KEY_KPPLUS                  VSF_KP_PLUS
#define KEY_KPDOT                   VSF_KP_DOT
#define KEY_KPEQUAL                 VSF_KP_EQUAL
#define KEY_KPCOMMA                 VSF_KP_COMMA

struct input_event {
    struct timeval time;
#define input_event_sec             time.tv_sec
#define input_event_usec            time.tv_usec

    uint16_t type;
    uint16_t code;
    int32_t value;
};

#ifdef __cplusplus
}
#endif

#endif
