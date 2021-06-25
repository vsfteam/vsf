#ifndef __VSF_LINUX_LIBGEN_H__
#define __VSF_LINUX_LIBGEN_H__

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_LIBGEN_CFG_WRAPPER == ENABLED
#define basename            __vsf_linux_basename
#define dirname             __vsf_linux_dirname
#endif

char * basename(char *);
char * dirname(char *);

#ifdef __cplusplus
}
#endif

#endif
