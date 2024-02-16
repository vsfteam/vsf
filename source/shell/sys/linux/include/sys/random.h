#ifndef __VSF_LINUX_SYS_RANDOM_H__
#define __VSF_LINUX_SYS_RANDOM_H__

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

#define GRND_NONBLOCK           0x0001
#define GRND_RANDOM             0x0002

#if VSF_LINUX_APPLET_USE_SYS_RANDOM == ENABLED
typedef struct vsf_linux_sys_random_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getrandom);
} vsf_linux_sys_random_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_random_vplt_t vsf_linux_sys_random_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_SYS_RANDOM_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_SYS_RANDOM == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_RANDOM_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_RANDOM_VPLT                                 \
            ((vsf_linux_sys_random_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_random_vplt))
#   else
#       define VSF_LINUX_APPLET_SYS_RANDOM_VPLT                                 \
            ((vsf_linux_sys_random_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_SYS_RANDOM_ENTRY(__NAME)                               \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_SYS_RANDOM_VPLT, __NAME)
#define VSF_LINUX_APPLET_SYS_RANDOM_IMP(...)                                    \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_SYS_RANDOM_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_SYS_RANDOM_IMP(getrandom, ssize_t, void *buf, size_t buflen, unsigned int flags) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_RANDOM_ENTRY(getrandom)(buf, buflen, flags);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_RANDOM

ssize_t getrandom(void *buf, size_t buflen, unsigned int flags);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_RANDOM

#ifdef __cplusplus
}
#endif

#endif
