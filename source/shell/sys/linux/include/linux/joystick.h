#ifndef __VSF_LINUX_JOYSTICK_H__
#define __VSF_LINUX_JOYSTICK_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_USE_INPUT == ENABLED

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./linux/types.h"
#   include "./sys/types.h"
#   include "./sys/ioctl.h"
#else
#   include <linux/types.h>
#   include <sys/types.h>
#   include <sys/ioctl.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define JS_EVENT_BUTTON     0x01
#define JS_EVENT_AXIS       0x02
#define JS_EVENT_INIT       0x80

struct js_event {
    __u32 time;
    __s16 value;
    __u8 type;
    __u8 number;
};

#define JSIOCGAXES          _IOR('j', 0x11, __u8)
#define JSIOCGBUTTONS       _IOR('j', 0x12, __u8)
#define JSIOCGNAME(len)     _IOC(_IOC_READ, 'j', 0x13, len)

#define JSIOCSAXMAP         _IOW('j', 0x31, __u8[ABS_CNT])
#define JSIOCGAXMAP         _IOR('j', 0x32, __u8[ABS_CNT])
#define JSIOCSBTNMAP        _IOW('j', 0x33, __u16[KEY_MAX - BTN_MISC + 1])
#define JSIOCGBTNMAP        _IOR('j', 0x34, __u16[KEY_MAX - BTN_MISC + 1])

#ifdef __cplusplus
}
#endif

#endif
#endif
