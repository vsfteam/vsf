#ifndef __VSF_LINUX_INPUT_H__
#define __VSF_LINUX_INPUT_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_USE_INPUT == ENABLED

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./sys/types.h"
#   include "./sys/time.h"
#   include "./sys/ioctl.h"
#else
#   include <sys/types.h>
#   include <sys/time.h>
#   include <sys/ioctl.h>
#endif
#include <linux/types.h>
#include <linux/device.h>
#include "component/input/vsf_input.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EVIOCGMTSLOTS(len)          _IOC(_IOC_READ, 'E', 0x0a, len)
#define EVIOCGKEY(len)              _IOC(_IOC_READ, 'E', 0x18, len)
#define EVIOCGLED(len)              _IOC(_IOC_READ, 'E', 0x19, len)
#define EVIOCGSND(len)              _IOC(_IOC_READ, 'E', 0x1a, len)
#define EVIOCGSW(len)               _IOC(_IOC_READ, 'E', 0x1b, len)
#define EVIOCGBIT(ev, len)          _IOC(_IOC_READ, 'E', 0x20 + (ev), len)
#define EVIOCGABS(abs)              _IOR('E', 0x40 + (abs), struct input_absinfo)
#define EVIOCSABS(abs)              _IOW('E', 0xc0 + (abs), struct input_absinfo)
#define EVIOCSFF                    _IOC(_IOC_WRITE, 'E', 0x80, sizeof(struct ff_effect))
#define EVIOCRMFF                   _IOW('E', 0x81, int)
#define EVIOCGEFFECTS               _IOR('E', 0x84, int)
#define EVIOCGRAB                   _IOW('E', 0x90, int)
#define EVIOCREVOKE                 _IOW('E', 0x91, int)
#define EVIOCSCLOCKID               _IOW('E', 0xa0, int)
#define EVIOCGVERSION               _IOR('E', 0x01, int)
#define EVIOCGID                    _IOR('E', 0x02, struct input_id)
#define EVIOCGREP                   _IOR('E', 0x03, unsigned int[2])
#define EVIOCSREP                   _IOW('E', 0x03, unsigned int[2])
#define EVIOCGKEYCODE               _IOR('E', 0x04, unsigned int[2])
#define EVIOCGKEYCODE_V2            _IOR('E', 0x04, struct input_keymap_entry)
#define EVIOCSKEYCODE               _IOW('E', 0x04, unsigned int[2])
#define EVIOCSKEYCODE_V2            _IOW('E', 0x04, struct input_keymap_entry)
#define EVIOCGNAME(len)             _IOC(_IOC_READ, 'E', 0x06, len)
#define EVIOCGPHYS(len)             _IOC(_IOC_READ, 'E', 0x07, len)
#define EVIOCGUNIQ(len)             _IOC(_IOC_READ, 'E', 0x08, len)
#define EVIOCGPROP(len)             _IOC(_IOC_READ, 'E', 0x09, len)

#define EV_VERSION                  0x010001

#define BUS_PCI                     0x01
#define BUS_USB                     0x03
#define BUS_BLUETOOTH               0x05
#define BUS_VIRTUAL                 0x06

// event types

#define EV_SYN                      VSF_INPUT_TYPE_SYNC
#define EV_KEY                      VSF_INPUT_TYPE_KEYBOARD
#define EV_REL                      0x10
#define EV_ABS                      0x11
#define EV_MSC                      0x12
#define EV_SW                       0x13
#define EV_LED                      0x14
#define EV_SND                      0x15
#define EV_REP                      0x16
#define EV_FF                       0x17
#define EV_PWR                      0x18
#define EV_FF_STATUS                0x19
#define EV_MAX                      0x1F
#define EV_CNT                      (EV_MAX + 1)

#define SYN_REPORT                  0
#define SYN_CONFIG                  1
#define SYN_MT_REPORT               2
#define SYN_DROPPED                 3
#define SYN_MAX                     0xf
#define SYN_CNT                     (SYN_MAX + 1)

// event codes
// VSF_INPUT_TYPE_SYNC

#define SYN_REPORT                  0

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

#define KEY_RECORD                  (VSF_KB_USER + 1)
#define KEY_SETUP                   (VSF_KB_USER + 2)
#define KEY_REWIND                  (VSF_KB_USER + 3)
#define KEY_PLAYPAUSE               (VSF_KB_USER + 4)
#define KEY_PLAY                    (VSF_KB_USER + 5)
#define KEY_FASTFORWARD             (VSF_KB_USER + 6)
#define KEY_CANCEL                  (VSF_KB_USER + 7)
#define KEY_SELECT                  (VSF_KB_USER + 8)
#define KEY_CLEAR                   (VSF_KB_USER + 9)
#define KEY_EXIT                    (VSF_KB_USER + 10)
#define KEY_INFO                    (VSF_KB_USER + 11)
#define KEY_PROGRAM                 (VSF_KB_USER + 12)
#define KEY_CALENDAR                (VSF_KB_USER + 13)
#define KEY_RED                     (VSF_KB_USER + 14)
#define KEY_GREEN                   (VSF_KB_USER + 15)
#define KEY_YELLOW                  (VSF_KB_USER + 16)
#define KEY_BLUE                    (VSF_KB_USER + 17)
#define KEY_CHANNELUP               (VSF_KB_USER + 18)
#define KEY_CHANNELDOWN             (VSF_KB_USER + 19)

#define KEY_MAX                     0x3ff
#define KEY_CNT                     (KEY_MAX + 1)

// rel

#define REL_X                       0x00
#define REL_Y                       0x01
#define REL_Z                       0x02
#define REL_RX                      0x03
#define REL_RY                      0x04
#define REL_RZ                      0x05
#define REL_HWHEEL                  0x06
#define REL_DIAL                    0x07
#define REL_WHEEL                   0x08
#define REL_MISC                    0x09

#define REL_MAX                     0x0f
#define REL_CNT                     (REL_MAX + 1)

// btn

#define BTN_GAMEPAD                 0x400
#define BTN_SOUTH                   (BTN_GAMEPAD | GAMEPAD_ID_R_DOWN)
#define BTN_A                       BTN_SOUTH
#define BTN_EAST                    (BTN_GAMEPAD | GAMEPAD_ID_R_RIGHT)
#define BTN_B                       BTN_EAST
#define BTN_NORTH                   (BTN_GAMEPAD | GAMEPAD_ID_R_UP)
#define BTN_X                       BTN_NORTH
#define BTN_WEST                    (BTN_GAMEPAD | GAMEPAD_ID_R_LEFT)
#define BTN_Y                       BTN_WEST
#define BTN_TL                      (BTN_GAMEPAD | GAMEPAD_ID_LT)
#define BTN_TR                      (BTN_GAMEPAD | GAMEPAD_ID_RT)
#define BTN_SELECT                  (BTN_GAMEPAD | GAMEPAD_ID_MENU_LEFT)
#define BTN_MODE                    (BTN_GAMEPAD | GAMEPAD_ID_MENU_MAIN)
#define BTN_START                   (BTN_GAMEPAD | GAMEPAD_ID_MENU_RIGHT)
#define BTN_THUMBL                  (BTN_GAMEPAD | GAMEPAD_ID_L_STICK)
#define BTN_THUMBR                  (BTN_GAMEPAD | GAMEPAD_ID_R_STICK)
#define BTN_TRIGGER_HAPPY           (BTN_GAMEPAD | (GAMEPAD_ID_USER + 0))
#define BTN_TRIGGER_HAPPY1          (BTN_GAMEPAD | (GAMEPAD_ID_USER + 0))
#define BTN_TRIGGER_HAPPY2          (BTN_GAMEPAD | (GAMEPAD_ID_USER + 1))
#define BTN_TRIGGER_HAPPY3          (BTN_GAMEPAD | (GAMEPAD_ID_USER + 2))
#define BTN_TRIGGER_HAPPY4          (BTN_GAMEPAD | (GAMEPAD_ID_USER + 3))
#define BTN_TRIGGER_HAPPY5          (BTN_GAMEPAD | (GAMEPAD_ID_USER + 4))
#define BTN_TRIGGER_HAPPY6          (BTN_GAMEPAD | (GAMEPAD_ID_USER + 5))
#define BTN_TRIGGER_HAPPY7          (BTN_GAMEPAD | (GAMEPAD_ID_USER + 6))
#define BTN_TRIGGER_HAPPY8          (BTN_GAMEPAD | (GAMEPAD_ID_USER + 7))
#define BTN_TRIGGER_HAPPY9          (BTN_GAMEPAD | (GAMEPAD_ID_USER + 8))
#define BTN_TRIGGER_HAPPY10         (BTN_GAMEPAD | (GAMEPAD_ID_USER + 9))
#define BTN_GAMEPAD_MAX             BTN_TRIGGER_HAPPY10

#define BTN_MOUSE                   0x420
#define BTN_LEFT                    (BTN_MOUSE + VSF_INPUT_MOUSE_BUTTON_LEFT)
#define BTN_RIGHT                   (BTN_MOUSE + VSF_INPUT_MOUSE_BUTTON_RIGHT)
#define BTN_MIDDLE                  (BTN_MOUSE + VSF_INPUT_MOUSE_BUTTON_MIDDLE)
#define BTN_MOUSE_MAX               0x422

#define BTN_JOYSTICK                0x430
#define BTN_TRIGGER                 BTN_JOYSTICK

#define BTN_DIGI                    0x440
#define BTN_TOOL_PEN                0x440
#define BTN_TOOL_FINGER             0x445
#define BTN_TOUCH                   0x44a
#define BTN_STYLUS                  0x44b

// abs

#define ABS_X                       0x00
#define ABS_Y                       0x01
#define ABS_Z                       0x02
#define ABS_RX                      0x03
#define ABS_RY                      0x04
#define ABS_RZ                      0x05
#define ABS_THROTTLE                0x06
#define ABS_RUDDER                  0x07
#define ABS_WHEEL                   0x08
#define ABS_GAS                     0x09
#define ABS_BRAKE                   0x0a
#define ABS_HAT0X                   0x10
#define ABS_HAT0Y                   0x11
#define ABS_HAT1X                   0x12
#define ABS_HAT1Y                   0x13
#define ABS_HAT2X                   0x14
#define ABS_HAT2Y                   0x15
#define ABS_HAT3X                   0x16
#define ABS_HAT3Y                   0x17
#define ABS_PRESSURE                0x18
#define ABS_DISTANCE                0x19
#define ABS_TILT_X                  0x1a
#define ABS_TILT_Y                  0x1b
#define ABS_TOOL_WIDTH              0x1c
#define ABS_VOLUME                  0x20
#define ABS_MISC                    0x28

#define ABS_MT_POSITION_X           0x35
#define ABS_MT_POSITION_Y           0x36
#define ABS_MT_TRACKING_ID          0x39
#define ABS_MT_PRESSURE             0x3a

#define ABS_MAX                     0x3f
#define ABS_CNT                     (ABS_MAX + 1)

// LED

#define LED_NUML                    0x00
#define LED_CAPSL                   0x01
#define LED_SCROLLL                 0x02
#define LED_COMPOSE                 0x03
#define LED_KANA                    0x04
#define LED_SLEEP                   0x05
#define LED_SUSPEND                 0x06
#define LED_MUTE                    0x07
#define LED_MISC                    0x08
#define LED_MAIL                    0x09
#define LED_CHARGING                0x0a
#define LED_MAX                     0x0f
#define LED_CNT                     (LED_MAX + 1)

// msc

#define MSC_SERIAL                  0x00
#define MSC_PULSELED                0x01
#define MSC_GESTURE                 0x02
#define MSC_RAW                     0x03
#define MSC_SCAN                    0x04
#define MSC_TIMESTAMP               0x05
#define MSC_MAX                     0x07
#define MSC_CNT                     (MSC_MAX + 1)

struct input_absinfo {
    __s32                           value;
    __s32                           minimum;
    __s32                           maximum;
    __s32                           fuzz;
    __s32                           flat;
    __s32                           resolution;
};

struct input_event {
    struct timeval time;
#define input_event_sec             time.tv_sec
#define input_event_usec            time.tv_usec

    uint16_t type;
    uint16_t code;
    int32_t value;
};

struct input_id {
    __u16                           bustype;
    __u16                           vendor;
    __u16                           product;
    __u16                           version;
};

dcl_vsf_bitmap(input_dev_bitmap, 0x450)
struct input_dev {
    const char                      *name;
    const char                      *phys;
    const char                      *uniq;
    struct input_id                 id;

    struct device                   dev;
    vk_input_notifier_t             notifier;

    uint64_t                        abs_msk;
    int                             abs_value[ABS_CNT];
    uint64_t                        rel_msk;
    int                             rel_value[REL_CNT];

    vsf_bitmap(input_dev_bitmap)    key_bitmap;
};
#define to_input_dev(__dev)         vsf_container_of((__dev), struct input_dev, dev)

struct input_value {
    __u16                           type;
    __u16                           code;
    __s32                           value;
};

struct input_handle;
struct input_handler {
    void (*event)(struct input_handle *handle, unsigned int type, unsigned int code, int value);
    void (*events)(struct input_handle *handle, const struct input_value *vals, unsigned int count);
    bool (*filter)(struct input_handle *handle, unsigned int type, unsigned int code, int value);
    bool (*match)(struct input_handler *handler, struct input_dev *dev);
//    int (*connect)(struct input_handler *handler, struct input_dev *dev, const struct input_device_id *id);
    void (*disconnect)(struct input_handle *handle);
    void (*start)(struct input_handle *handle);

    const char                      *name;
};
struct input_handle {
    const char                      *name;
    struct input_dev                *dev;
    struct input_handler            *handler;
};

// force feedback
struct ff_replay {
    __u16                           length;
    __u16                           delay;
};
struct ff_trigger {
    __u16                           button;
    __u16                           interval;
};
struct ff_envelope {
    __u16                           attack_length;
    __u16                           attack_level;
    __u16                           fade_length;
    __u16                           fade_level;
};
struct ff_constant_effect {
    __s16                           level;
    struct ff_envelope              envelope;
};
struct ff_ramp_effect {
    __s16                           start_level;
    __s16                           end_level;
    struct ff_envelope              envelope;
};
struct ff_condition_effect {
    __u16                           right_saturation;
    __u16                           left_saturation;
    __s16                           right_coeff;
    __s16                           left_coeff;
    __u16                           deadband;
    __s16                           center;
};
struct ff_periodic_effect {
    __u16                           waveform;
    __u16                           period;
    __s16                           magnitude;
    __s16                           offset;
    __u16                           phase;
    struct ff_envelope              envelope;
    __u32                           custom_len;
    __s16                           *custom_data;
};
struct ff_rumble_effect {
    __u16                           strong_magnitude;
    __u16                           weak_magnitude;
};
struct ff_effect {
    __u16                           type;
    __s16                           id;
    __u16                           direction;
    struct ff_trigger               trigger;
    struct ff_replay                replay;

    union {
        struct ff_constant_effect constant;
        struct ff_ramp_effect ramp;
        struct ff_periodic_effect periodic;
        struct ff_condition_effect condition[2];    // x/y
        struct ff_rumble_effect rumble;
    } u;
};
#define FF_RUMBLE                   0x50
#define FF_PERIODIC                 0x51
#define FF_CONSTANT                 0x52
#define FF_SPRING                   0x53
#define FF_FRICTION                 0x54
#define FF_DAMPER                   0x55
#define FF_INERTIA                  0x56
#define FF_RAMP                     0x57
#define FF_EFFECT_MIN               FF_RUMBLE
#define FF_EFFECT_MAX               FF_RAMP

#define FF_SQUARE                   0x58
#define FF_TRIANGLE                 0x59
#define FF_SINE                     0x5a
#define FF_SAW_UP                   0x5b
#define FF_SAW_DOWN                 0x5c
#define FF_CUSTOM                   0x5d
#define FF_WAVEFORM_MIN             FF_SQUARE
#define FF_WAVEFORM_MAX             FF_CUSTOM

#define FF_GAIN                     0x60
#define FF_AUTOCENTER               0x61

#define FF_MAX_EFFECTS              FF_GAIN
#define FF_MAX                      0x7f
#define FF_CNT                      (FF_MAX + 1)

extern int input_ff_create(struct input_dev *dev, unsigned int max_effects);
extern void input_ff_destroy(struct input_dev *dev);
extern int input_ff_event(struct input_dev *dev, unsigned int type, unsigned int code, int value);
//extern int input_ff_upload(struct input_dev *dev, struct ff_effect *effect, struct file *file);
//extern int input_ff_erase(struct input_dev *dev, int effect_id, struct file *file);
//extern int input_ff_flush(struct input_dev *dev, struct file *file);
extern int input_ff_create_memless(struct input_dev *dev, void *data, int (*play_effect)(struct input_dev *, void *, struct ff_effect *));

extern struct input_dev * input_allocate_device(void);
extern struct input_dev * devm_input_allocate_device(struct device *);
extern void input_free_device(struct input_dev *dev);

static inline struct input_dev * input_get_device(struct input_dev *dev)
{
    return dev ? to_input_dev(get_device(&dev->dev)) : NULL;
}

static inline void input_put_device(struct input_dev *dev)
{
    if (dev) {
        put_device(&dev->dev);
    }
}

static inline void * input_get_drvdata(struct input_dev *dev)
{
    return dev_get_drvdata(&dev->dev);
}

static inline void input_set_drvdata(struct input_dev *dev, void *data)
{
    dev_set_drvdata(&dev->dev, data);
}

extern int input_register_device(struct input_dev *dev);
extern void input_unregister_device(struct input_dev *dev);

extern void input_event(struct input_dev *dev, unsigned int type, unsigned int code, int value);
extern void input_inject_event(struct input_handle *handle, unsigned int type, unsigned int code, int value);

static inline void input_report_key(struct input_dev *dev, unsigned int code, int value)
{
    input_event(dev, EV_KEY, code, !!value);
}

static inline void input_report_rel(struct input_dev *dev, unsigned int code, int value)
{
    input_event(dev, EV_REL, code, value);
}

static inline void input_report_abs(struct input_dev *dev, unsigned int code, int value)
{
    input_event(dev, EV_ABS, code, value);
}

static inline void input_report_ff_status(struct input_dev *dev, unsigned int code, int value)
{
    input_event(dev, EV_FF_STATUS, code, value);
}

static inline void input_report_switch(struct input_dev *dev, unsigned int code, int value)
{
    input_event(dev, EV_SW, code, !!value);
}

static inline void input_sync(struct input_dev *dev)
{
    input_event(dev, EV_SYN, SYN_REPORT, 0);
}

static inline void input_mt_sync(struct input_dev *dev)
{
    input_event(dev, EV_SYN, SYN_MT_REPORT, 0);
}

extern void input_set_capability(struct input_dev *dev, unsigned int type, unsigned int code);

extern void input_alloc_absinfo(struct input_dev *dev);
extern void input_set_abs_params(struct input_dev *dev, unsigned int axis, int min, int max, int fuzz, int flat);
extern void input_copy_abs(struct input_dev *dst, unsigned int dst_axis, const struct input_dev *src, unsigned int src_axis);

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_INPUT
#endif      // __VSF_LINUX_INPUT_H__
