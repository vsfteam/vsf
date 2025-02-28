#ifndef __VSF_LINUX_SYS_TIMEB_H__
#define __VSF_LINUX_SYS_TIMEB_H__

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
#define ftime               VSF_LINUX_WRAPPER(ftime)
#endif

struct timeb {
    time_t time;
    unsigned short millitm;
    short timezone;
    short dstflag;
};

#if VSF_LINUX_APPLET_USE_SYS_TIMEB == ENABLED
typedef struct vsf_linux_sys_timeb_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ftime);
} vsf_linux_sys_timeb_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_timeb_vplt_t vsf_linux_sys_timeb_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_SYS_TIMEB_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_SYS_TIMEB == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_TIMEB_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_TIMEB_VPLT                                  \
            ((vsf_linux_sys_timeb_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_timeb_vplt))
#   else
#       define VSF_LINUX_APPLET_SYS_TIMEB_VPLT                                  \
            ((vsf_linux_sys_timeb_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_SYS_TIMEB_ENTRY(__NAME)                                \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_SYS_TIMEB_VPLT, __NAME)
#define VSF_LINUX_APPLET_SYS_TIMEB_IMP(...)                                     \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_SYS_TIMEB_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_SYS_TIMEB_IMP(ftime, int, struct timeb *timebuf) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_TIMEB_ENTRY(ftime)(timebuf);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_TIMEB

int ftime(struct timeb *timebuf);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_TIMEB

#ifdef __cplusplus
}
#endif

#endif
