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

    clock_t (*times)(struct tms *buf);
} vsf_linux_sys_times_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_times_vplt_t vsf_linux_sys_times_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_SYS_TIMES == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_TIMES_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_TIMES_VPLT                                   \
            ((vsf_linux_sys_times_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_time))
#   else
#       define VSF_LINUX_APPLET_SYS_TIMES_VPLT                                   \
            ((vsf_linux_sys_times_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

static inline clock_t times(struct tms *buf) {
    return VSF_LINUX_APPLET_SYS_TIMES_VPLT->times(buf);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_TIMES

clock_t times(struct tms *buf);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_TIMES

#ifdef __cplusplus
}
#endif

#endif
