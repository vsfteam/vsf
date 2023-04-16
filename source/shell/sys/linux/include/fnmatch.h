#ifndef __VSF_LINUX_FNMATCH_H__
#define __VSF_LINUX_FNMATCH_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define find_in_path        VSF_LINUX_WRAPPER(find_in_path)
#define find_in_given_path  VSF_LINUX_WRAPPER(find_in_given_path)
#endif

#define FNM_NOMATCH         1
#define FNM_NOSYS           2

#define FNM_NOESCAPE        0x01
#define FNM_PATHNAME        0x02
#define FNM_PERIOD          0x04
#define FNM_LEADING_DIR     0x08
#define FNM_CASEFOLD        0x10

#define FNM_IGNORECASE      FNM_CASEFOLD
#define FNM_FILE_NAME       FNM_PATHNAME

#if VSF_LINUX_APPLET_USE_FNMATCH == ENABLED
typedef struct vsf_linux_fnmatch_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fnmatch);
} vsf_linux_fnmatch_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_fnmatch_vplt_t vsf_linux_fnmatch_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_FNMATCH == ENABLED

#ifndef VSF_LINUX_APPLET_FNMATCH_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_FNMATCH_VPLT                                    \
            ((vsf_linux_fnmatch_vplt_t *)(VSF_LINUX_APPLET_VPLT->fnmatch_vplt))
#   else
#       define VSF_LINUX_APPLET_FNMATCH_VPLT                                    \
            ((vsf_linux_fnmatch_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_FNMATCH_ENTRY(__NAME)                                  \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_FNMATCH_VPLT, __NAME)
#define VSF_LINUX_APPLET_FNMATCH_IMP(...)                                       \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_FNMATCH_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_FNMATCH_IMP(fnmatch, int, const char *pattern, const char *string, int flags) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_FNMATCH_ENTRY(fnmatch)(pattern, string, flags);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_FNMATCH

int fnmatch(const char *pattern, const char *string, int flags);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_FNMATCH

#ifdef __cplusplus
}
#endif

#endif
