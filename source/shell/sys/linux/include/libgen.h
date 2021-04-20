#ifndef __LIBGEN_H__
#define __LIBGEN_H__

#ifdef __cplusplus
extern "C" {
#endif

#define basename            __vsf_linux_basename
#define dirname             __vsf_linux_dirname

char * basename(char *);
char * dirname(char *);

#ifdef __cplusplus
}
#endif

#endif
