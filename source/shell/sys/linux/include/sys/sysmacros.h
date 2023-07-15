#ifndef __VSF_LINUX_SYSMACROS_H__
#define __VSF_LINUX_SYSMACROS_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#else
#   include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_APPLET_USE_SYS_SYSMACROS == ENABLED
typedef struct vsf_linux_sys_sysmacros_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(makedev);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(major);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(minor);
} vsf_linux_sys_sysmacros_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_sysmacros_vplt_t vsf_linux_sys_sysmacros_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_SYS_SYSMACROS_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_SYS_SYSMACROS == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_SYSMACROS_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_SYSMACROS_VPLT                              \
            ((vsf_linux_sys_sysmacros_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_sysmacros_vplt))
#   else
#       define VSF_LINUX_APPLET_SYS_SYSMACROS_VPLT                              \
            ((vsf_linux_sys_sysmacros_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_SYS_SYSMACROS_ENTRY(__NAME)                            \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_SYS_SYSMACROS_VPLT, __NAME)
#define VSF_LINUX_APPLET_SYS_SYSMACROS_IMP(...)                                 \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_SYS_SYSMACROS_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_SYS_SYSMACROS_IMP(makedev, dev_t, unsigned int maj, unsigned int min) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_SYSMACROS_ENTRY(makedev)(maj, min);
}
VSF_LINUX_APPLET_SYS_SYSMACROS_IMP(major, unsigned int, dev_t dev) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_SYSMACROS_ENTRY(major)(dev);
}
VSF_LINUX_APPLET_SYS_SYSMACROS_IMP(minor, unsigned int, dev_t dev) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_SYSMACROS_ENTRY(minor)(dev);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_SYSMACROS

dev_t makedev(unsigned int maj, unsigned int min);
unsigned int major(dev_t dev);
unsigned int minor(dev_t dev);

#endif

#ifdef __cplusplus
}
#endif

#endif
