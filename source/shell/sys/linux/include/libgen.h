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

char * basename(char *);
char * dirname(char *);

#ifdef __cplusplus
}
#endif

#endif
