#ifndef __VSF_LINUX_SYS_CAPABILITY_H__
#define __VSF_LINUX_SYS_CAPABILITY_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./types.h"
#else
#   include <sys/types.h>
#endif
#include <linux/capability.h>

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define capget              VSF_LINUX_WRAPPER(capget)
#define capset              VSF_LINUX_WRAPPER(capset)
#endif

#if VSF_LINUX_APPLET_USE_SYS_CAPABILITY == ENABLED
typedef struct vsf_linux_sys_capability_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(capget);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(capset);
} vsf_linux_sys_capability_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_capability_vplt_t vsf_linux_sys_capability_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && defined(__VSF_APPLET_LIB__) && VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_SYS_CAPABILITY == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_CAPABILITY_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_CAPABILITY_VPLT                             \
            ((vsf_linux_sys_capability_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_capability_vplt))
#   else
#       define VSF_LINUX_APPLET_SYS_CAPABILITY_VPLT                             \
            ((vsf_linux_sys_capability_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_SYS_CAPABILITY_ENTRY(__NAME)                           \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_SYS_CAPABILITY_VPLT, __NAME)
#define VSF_LINUX_APPLET_SYS_CAPABILITY_IMP(...)                                \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_SYS_CAPABILITY_VPLT, __VA_ARGS__)

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_CAPABILITY

int capget(cap_user_header_t hdrp, cap_user_data_t datap);
int capset(cap_user_header_t hdrp, const cap_user_data_t datap);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_CAPABILITY

#ifdef __cplusplus
}
#endif

#endif      // __VSF_LINUX_CAPABILITY_H__
