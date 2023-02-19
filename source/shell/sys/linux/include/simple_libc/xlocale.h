#ifndef __SIMPLE_LIBC_XLOCALE_H__
#define __SIMPLE_LIBC_XLOCALE_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#else
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct __locale_struct {
    // TODO:
    int dummy;
};
typedef struct __locale_struct *                locale_t;

#define scanf_l(__loc, ...)                     scanf(__VA_ARGS__)
#define fscanf_l(__file, __loc, ...)            fscanf(__file, __VA_ARGS__)
#define sscanf_l(__str, __loc, ...)             sscanf(__str, __VA_ARGS__)
#define vscanf_l(__loc, ...)                    vscanf(__VA_ARGS__)
#define vfscanf_l(__file, __loc, ...)           vfscanf(__file, __VA_ARGS__)
#define vsscanf_l(__str, __loc, ...)            vsscanf(__str, __VA_ARGS__)

#define printf_l(__loc, ...)                    printf(__VA_ARGS__)
#define fprintf_l(__file, __loc, ...)           fprintf(__file, __VA_ARGS__)
#define sprintf_l(__str, __loc, ...)            sprintf(__str, __VA_ARGS__)
#define snprintf_l(__str, __size, __loc, ...)   snprintf(__str, __size, __VA_ARGS__)
#define asprintf_l(__ret, __loc, ...)           asprintf(__ret, __VA_ARGS__)
#define vprintf_l(__loc, ...)                   vprintf(__VA_ARGS__)
#define vfprintf_l(__file, __loc, ...)          vfprintf(__file, __VA_ARGS__)
#define vsprintf_l(__str, __loc, ...)           vsprintf(__str, __VA_ARGS__)
#define vsnprintf_l(__str, __size, __loc, ...)  vsnprintf_l(__str, __size, __VA_ARGS__)
#define vasprintf_l(__ret, __loc, ...)          vasprintf(__ret, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
