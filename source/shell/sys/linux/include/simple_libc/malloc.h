#ifndef __SIMPLE_LIBC_MALLOC_H__
#define __SIMPLE_LIBC_MALLOC_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   include "./stddef.h"
#else
#   include <stddef.h>
#endif
#include "./stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

#define aligned_malloc          aligned_alloc
#define aligned_free            free
#define memalign                aligned_alloc

#ifdef __WIN__
#   define _aligned_malloc      aligned_malloc
#   define _aligned_free        aligned_free
#endif

#ifdef __cplusplus
}
#endif

#endif
