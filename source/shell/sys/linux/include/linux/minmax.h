#ifndef __VSF_LINUX_MINMAX_H__
#define __VSF_LINUX_MINMAX_H__

// for vsf_min/vsf_max
#include "utilities/vsf_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

#define min(x, y)                       vsf_min(x, y)
#define max(x, y)                       vsf_max(x, y)
#define min_t(type, x, y)               vsf_min((type)x, (type)y)
#define max_t(type, x, y)               vsf_max((type)x, (type)y)
#define clamp(val, lo, hi)              min((typeof(val))max(val, lo), hi)

#ifdef __cplusplus
}
#endif

#endif
