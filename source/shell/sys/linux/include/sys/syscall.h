#ifndef __VSF_LINUX_SYSCALL_H__
#define __VSF_LINUX_SYSCALL_H__

#include "shell/sys/linux/vsf_linux_cfg.h"
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/random.h"
#else
#   include <sys/random.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define SYS_getrandom               getrandom

#define syscall(__func, ...)        __func(__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
