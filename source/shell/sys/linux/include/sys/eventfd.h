#ifndef __VSF_LINUX_EVENTFD_H__
#define __VSF_LINUX_EVENTFD_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#   include "../fcntl.h"
#else
#   include <sys/types.h>
#   include <fcntl.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define eventfd                 VSF_LINUX_WRAPPER(eventfd)
#endif

#define EFD_SEMAPHORE           (1 << 0)
#define EFD_CLOEXEC             O_CLOEXEC
#define EFD_NONBLOCK            O_NONBLOCK

int eventfd(int count, int flags);

#ifdef __cplusplus
}
#endif

#endif      // __VSF_LINUX_EVENTFD_H__
