#ifndef __VSF_LINUX_PRCTL_H__
#define __VSF_LINUX_PRCTL_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define prctl               VSF_LINUX_WRAPPER(prctl)
#endif

#define PR_SET_NAME         15
#define PR_GET_NAME         16

#if VSF_LINUX_APPLET_USE_SYS_PRCTL == ENABLED
typedef struct vsf_linux_sys_prctl_vplt_t {
    vsf_vplt_info_t info;
} vsf_linux_sys_prctl_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_prctl_vplt_t vsf_linux_sys_prctl_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && defined(__VSF_APPLET_LIB__) && VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_SYS_PRCTL == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_PRCTL_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_PRCTL_VPLT                                  \
            ((vsf_linux_sys_prctl_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_prctl_vplt))
#   else
#       define VSF_LINUX_APPLET_SYS_PRCTL_VPLT                                  \
            ((vsf_linux_sys_prctl_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_SYS_PRCTL_ENTRY(__NAME)                                \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_SYS_PRCTL_VPLT, __NAME)
#define VSF_LINUX_APPLET_SYS_PRCTL_IMP(...)                                     \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_SYS_PRCTL_VPLT, __VA_ARGS__)

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_PRCTL

int prctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_PRCTL

#ifdef __cplusplus
}
#endif

#endif
