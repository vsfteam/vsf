#ifndef __VSF_LINUX_MUTEX_H__
#define __VSF_LINUX_MUTEX_H__

#include "kernel/vsf_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mutex {
    vsf_mutex_t             mutex;
};

#ifdef __cplusplus
}
#endif

#endif
