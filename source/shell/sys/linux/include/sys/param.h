#ifndef __VSF_LINUX_SYS_PARAM_H__
#define __VSF_LINUX_SYS_PARAM_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../linux/limits.h"
#else
#   include <linux/limits.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define MIN(a, b)       (((a) < (b)) ? (a) : (b))
#define MAX(a, b)       (((a) > (b)) ? (a) : (b))

#ifdef __cplusplus
}
#endif

#endif      // __VSF_LINUX_EVENTFD_H__
