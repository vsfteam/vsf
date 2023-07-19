#ifndef __VSF_LINUX_SYS_INFO_H__
#define __VSF_LINUX_SYS_INFO_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#else
#   include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct sysinfo {
    long uptime;
    unsigned long loads[3];
    unsigned long totalram;
    unsigned long freeram;
    unsigned long sharedram;
    unsigned long bufferram;
    unsigned long totalswap;
    unsigned long freeswap;
    unsigned short procs;
    char _f[22];
};

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define sysinfo             VSF_LINUX_WRAPPER(sysinfo)
#endif

#if VSF_LINUX_APPLET_USE_SYS_INFO == ENABLED
typedef struct vsf_linux_sys_info_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sysinfo);
} vsf_linux_sys_info_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_info_vplt_t vsf_linux_sys_info_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_SYS_INFO_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_SYS_INFO == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_INFO_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_INFO_VPLT                                    \
            ((vsf_linux_sys_info_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_info_vplt))
#   else
#       define VSF_LINUX_APPLET_SYS_INFO_VPLT                                    \
            ((vsf_linux_sys_info_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_SYS_INFO_ENTRY(__NAME)                                  \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_SYS_INFO_VPLT, __NAME)
#define VSF_LINUX_APPLET_SYS_INFO_IMP(...)                                       \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_SYS_INFO_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_SYS_INFO_IMP(sysinfo, int, struct sysinfo *info) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_INFO_ENTRY(sysinfo)(info);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_INFO

int sysinfo(struct sysinfo *info);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_INFO

#ifdef __cplusplus
}
#endif

#endif      // __VSF_LINUX_SYS_INFO_H__
