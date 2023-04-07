#ifndef __SIMPLE_LIBC_DLFCN_H__
#define __SIMPLE_LIBC_DLFCN_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   include "./stddef.h"
#else
#   include <stddef.h>
#endif
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_LIBC_CFG_WRAPPER == ENABLED
#define dlsym               VSF_LINUX_LIBC_WRAPPER(dlsym)
#endif

#define RTLD_DEFAULT        ((void *)0)

#if VSF_LINUX_APPLET_USE_LIBC_DLFCN == ENABLED
typedef struct vsf_linux_libc_dlfcn_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(dlsym);
} vsf_linux_libc_dlfcn_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_libc_dlfcn_vplt_t vsf_linux_libc_dlfcn_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_LIBC_DLFCN == ENABLED

#ifndef VSF_LINUX_APPLET_LIBC_DLFCN_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_LIBC_DLFCN_VPLT                                 \
            ((vsf_linux_libc_dlfcn_vplt_t *)(VSF_LINUX_APPLET_VPLT->libc_dlfcn))
#   else
#       define VSF_LINUX_APPLET_LIBC_DLFCN_VPLT                                 \
            ((vsf_linux_libc_dlfcn_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_LIBC_DLFCN_ENTRY(__NAME)                               \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_LIBC_DLFCN_VPLT, __NAME)
#define VSF_LINUX_APPLET_LIBC_DLFCN_IMP(...)                                    \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_LIBC_DLFCN_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_LIBC_DLFCN_IMP(dlsym, void *, void *handle, const char *name) {
    return VSF_LINUX_APPLET_LIBC_DLFCN_ENTRY(dlsym)(handle, name);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_LIBC_DLFCN

void * dlsym(void *handle, const char *name);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_LIBC_DLFCN

#ifdef __cplusplus
}
#endif

#endif
