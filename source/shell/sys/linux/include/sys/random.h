#ifndef __VSF_LINUX_RANDOM_H__
#define __VSF_LINUX_RANDOM_H__

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
#define getrandom               VSF_LINUX_WRAPPER(getrandom)
#endif

ssize_t getrandom(void *buf, size_t buflen, unsigned int flags);

#ifdef __cplusplus
}
#endif

#endif
