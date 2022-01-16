#ifndef __VSF_LINUX_WAIT_H__
#define __VSF_LINUX_WAIT_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#else
#   include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define waitpid                 VSF_LINUX_WRAPPER(waitpid)
#endif

#define WIFEXITED(__STATUS)     !((__STATUS) & 0xFF)
#define WEXITSTATUS(__STATUS)   ((__STATUS) >> 8)
// TODO: implement WIFSIGNALED
#define WIFSIGNALED(__STATUS)   0
#define WTERMSIG(__STATUS)      ((__STATUS) & 0x7F)

pid_t waitpid(pid_t pid, int *status, int options);

#ifdef __cplusplus
}
#endif

#endif
