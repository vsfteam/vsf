#ifndef __XBOOT_STDDEF_H__
#define __XBOOT_STDDEF_H__

#include "shell/sys/linux/include/simple_libc/stddef.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef offsetof
#   define offsetof     offset_of
#endif

#define clamp(v, a, b)	min(max(a, v), b)

#define ifloor(x)		((x) > 0 ? (int)(x) : (int)((x) - 0.9999999999))
#define iround(x)		((x) > 0 ? (int)((x) + 0.5) : (int)((x) - 0.5))
#define iceil(x)		((x) > 0 ? (int)((x) + 0.9999999999) : (int)(x))
#define idiv255(x)		((((int)(x) + 1) * 257) >> 16)

#ifdef __cplusplus
}
#endif

#endif
