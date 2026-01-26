#ifndef __VSF_LINUX_DLFCN_H__
#define __VSF_LINUX_DLFCN_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   include "./simple_libc/stddef.h"
#else
#   include <stddef.h>
#endif
#include <stdio.h>

// for elf_loader in service.loader
#include "service/vsf_service.h"

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define dlopen              VSF_LINUX_WRAPPER(dlopen)
#define dlclose             VSF_LINUX_WRAPPER(dlclose)
#define dlsym               VSF_LINUX_WRAPPER(dlsym)
#define dlerror             VSF_LINUX_WRAPPER(dlerror)
#endif

#define RTLD_LAZY           0
#define RTLD_NOW            1
#define RTLD_GLOBAL         2
#define RTLD_LOCAL          3

#define RTLD_DEFAULT        ((void *)0)

#if VSF_LINUX_APPLET_USE_DLFCN == ENABLED
typedef struct vsf_linux_dlfcn_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(dlopen);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(dlclose);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(dlsym);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(dlerror);
} vsf_linux_dlfcn_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_dlfcn_vplt_t vsf_linux_dlfcn_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_DLFCN_LIB__))\
    &&  VSF_LINUX_APPLET_USE_DLFCN == ENABLED

#ifndef VSF_LINUX_APPLET_DLFCN_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_DLFCN_VPLT                                      \
            ((vsf_linux_dlfcn_vplt_t *)(VSF_LINUX_APPLET_VPLT->dlfcn_vplt))
#   else
#       define VSF_LINUX_APPLET_DLFCN_VPLT                                      \
            ((vsf_linux_dlfcn_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_DLFCN_ENTRY(__NAME)                                    \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_DLFCN_VPLT, __NAME)
#define VSF_LINUX_APPLET_DLFCN_IMP(...)                                         \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_DLFCN_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_DLFCN_IMP(dlopen, void *, const char *pathname, int mode) {
    return VSF_LINUX_APPLET_DLFCN_ENTRY(dlopen)(pathname, mode);
}
VSF_LINUX_APPLET_DLFCN_IMP(dlclose, int, void *handle) {
    return VSF_LINUX_APPLET_DLFCN_ENTRY(dlclose)(handle);
}
VSF_LINUX_APPLET_DLFCN_IMP(dlsym, void *, void *handle, const char *name) {
    return VSF_LINUX_APPLET_DLFCN_ENTRY(dlsym)(handle, name);
}
VSF_LINUX_APPLET_DLFCN_IMP(dlerror, char *, void) {
    return VSF_LINUX_APPLET_DLFCN_ENTRY(dlerror)();
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_DLFCN

#if VSF_USE_LOADER == ENABLED
typedef struct vsf_linux_dynloader_t {
    union {
#if VSF_LOADER_USE_ELF == ENABLED
        vsf_elfloader_t elfloader;
#endif
#if VSF_LOADER_USE_PE == ENABLED
        vsf_peloader_t peloader;
#endif
        vsf_loader_t generic;
    } loader;
    vsf_loader_target_t target;

    // vplt is usd for staitc library, and emulate dlopen/dlsym/dlclose
    void *vplt;
} vsf_linux_dynloader_t;
#endif

void * dlopen(const char *pathname, int mode);
int dlclose(void *handle);
void * dlsym(void *handle, const char *name);
char * dlerror(void);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_DLFCN

#ifdef __cplusplus
}
#endif

#endif
