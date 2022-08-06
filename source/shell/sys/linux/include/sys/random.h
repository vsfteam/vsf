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

#if VSF_LINUX_APPLET_USE_SYS_RANDOM == ENABLED
typedef struct vsf_linux_sys_random_vplt_t {
    vsf_vplt_info_t info;

    ssize_t (*getrandom)(void *buf, size_t buflen, unsigned int flags);
} vsf_linux_sys_random_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_random_vplt_t vsf_linux_sys_random_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_SYS_RANDOM == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_RANDOM_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_RANDOM_VPLT                                 \
            ((vsf_linux_sys_random_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_random))
#   else
#       define VSF_LINUX_APPLET_SYS_RANDOM_VPLT                                 \
            ((vsf_linux_sys_random_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

ssize_t getrandom(void *buf, size_t buflen, unsigned int flags) {
    return VSF_LINUX_APPLET_SYS_RANDOM_VPLT->getrandom(buf, buflen, flags);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_RANDOM

ssize_t getrandom(void *buf, size_t buflen, unsigned int flags);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_RANDOM

#ifdef __cplusplus
}
#endif

#endif
