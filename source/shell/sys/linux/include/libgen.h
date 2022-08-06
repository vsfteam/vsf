#ifndef __VSF_LINUX_LIBGEN_H__
#define __VSF_LINUX_LIBGEN_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VSF_LINUX_LIBGEN_WRAPPER(__api)         VSF_SHELL_WRAPPER(vsf_linux_libgen, __api)

#if VSF_LINUX_LIBGEN_CFG_WRAPPER == ENABLED
#define basename            VSF_LINUX_LIBGEN_WRAPPER(basename)
#define dirname             VSF_LINUX_LIBGEN_WRAPPER(dirname)
#endif

#if VSF_LINUX_APPLET_USE_LIBGEN == ENABLED
typedef struct vsf_linux_libgen_vplt_t {
    vsf_vplt_info_t info;

    char * (*basename)(char *);
    char * (*dirname)(char *);
} vsf_linux_libgen_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_libgen_vplt_t vsf_linux_libgen_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_LIBGEN == ENABLED

#ifndef VSF_LINUX_APPLET_LIBGEN_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_LIBGEN_VPLT                                     \
            ((vsf_linux_libgen_vplt_t *)(VSF_LINUX_APPLET_VPLT->libgen))
#   else
#       define VSF_LINUX_APPLET_LIBGEN_VPLT                                     \
            ((vsf_linux_libgen_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

static inline char * basename(char *path) {
    return VSF_LINUX_APPLET_LIBGEN_VPLT->basename(path);
}
static inline char * dirname(char *path) {
    return VSF_LINUX_APPLET_LIBGEN_VPLT->basename(path);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_LIBGEN

char * basename(char *);
char * dirname(char *);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_LIBGEN

#ifdef __cplusplus
}
#endif

#endif
