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
