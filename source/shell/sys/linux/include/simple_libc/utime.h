#ifndef __SIMPLE_LIBC_UTIME_H__
#define __SIMPLE_LIBC_UTIME_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   include "./stddef.h"
#else
#   include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct utimbuf {
    time_t actime;
    time_t modtime;
};

//extern int utime(const char * file, const struct utimbuf * time);
#define utime(__file, __time)           0

#ifdef __cplusplus
}
#endif

#endif
