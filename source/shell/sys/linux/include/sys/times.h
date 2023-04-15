#ifndef __VSF_LINUX_SYS_TIMES_H__
#define __VSF_LINUX_SYS_TIMES_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./types.h"
#else
#   include <sys/types.h>
#endif
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define times               VSF_LINUX_WRAPPER(times)
#endif

struct tms {
    clock_t         tms_utime;
    clock_t         tms_stime;
    clock_t         tms_cutime;
    clock_t         tms_cstime;
};

#if VSF_LINUX_APPLET_USE_SYS_TIMES == ENABLED
typedef struct vsf_linux_sys_times_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(times);
} vsf_linux_sys_times_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_times_vplt_t vsf_linux_sys_times_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_SYS_TIMES == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_TIMES_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_TIMES_VPLT                                  \
            ((vsf_linux_sys_times_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_times_vplt))
#   else
#       define VSF_LINUX_APPLET_SYS_TIMES_VPLT                                  \
            ((vsf_linux_sys_times_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_SYS_TIMES_ENTRY(__NAME)                                \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_SYS_TIMES_VPLT, __NAME)
#define VSF_LINUX_APPLET_SYS_TIMES_IMP(...)                                     \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_SYS_TIMES_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_SYS_TIMES_IMP(times, clock_t, struct tms *buf) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_TIMES_ENTRY(times)(buf);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_TIMES

clock_t times(struct tms *buf);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_TIMES

#ifdef __cplusplus
}
#endif

#endif
