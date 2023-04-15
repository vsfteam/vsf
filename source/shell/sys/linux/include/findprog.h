#ifndef __VSF_LINUX_FINDPROG_H__
#define __VSF_LINUX_FINDPROG_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define find_in_path        VSF_LINUX_WRAPPER(find_in_path)
#define find_in_given_path  VSF_LINUX_WRAPPER(find_in_given_path)
#endif

#if VSF_LINUX_APPLET_USE_FINDPROG == ENABLED
typedef struct vsf_linux_findprog_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(find_in_path);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(find_in_given_path);
} vsf_linux_findprog_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_findprog_vplt_t vsf_linux_findprog_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_FINDPROG == ENABLED

#ifndef VSF_LINUX_APPLET_FINDPROG_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_FINDPROG_VPLT                                   \
            ((vsf_linux_findprog_vplt_t *)(VSF_LINUX_APPLET_VPLT->findprog_vplt))
#   else
#       define VSF_LINUX_APPLET_FINDPROG_VPLT                                   \
            ((vsf_linux_findprog_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_FINDPROG_ENTRY(__NAME)                                 \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_FINDPROG_VPLT, __NAME)
#define VSF_LINUX_APPLET_FINDPROG_IMP(...)                                      \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_FINDPROG_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_FINDPROG_IMP(find_in_path, const char *, const char *progname) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_FINDPROG_ENTRY(find_in_path)(progname);
}
VSF_LINUX_APPLET_FINDPROG_IMP(find_in_given_path, const char *, const char *progname, const char *path, const char *directory, bool optimize_for_exec) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_FINDPROG_ENTRY(find_in_given_path)(progname, path, directory, optimize_for_exec);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_FINDPROG

const char * find_in_path(const char *progname);
const char * find_in_given_path(const char *progname, const char *path, const char *directory, bool optimize_for_exec);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_FINDPROG

#ifdef __cplusplus
}
#endif

#endif
