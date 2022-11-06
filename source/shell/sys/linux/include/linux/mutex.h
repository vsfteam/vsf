#ifndef __VSF_LINUX_MUTEX_H__
#define __VSF_LINUX_MUTEX_H__

#include "kernel/vsf_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mutex {
    vsf_mutex_t             mutex;
};

static inline void mutex_init(struct mutex *lock)
{
    vsf_eda_mutex_init(&lock->mutex);
}

static inline void mutex_lock(struct mutex *lock)
{
    vsf_thread_mutex_enter(&lock->mutex, -1);
}

static inline int mutex_trylock(struct mutex *lock)
{
    return VSF_SYNC_GET == vsf_thread_mutex_enter(&lock->mutex, 0) ? 1 : 0;
}

static inline void mutex_unlock(struct mutex *lock)
{
    vsf_eda_mutex_leave(&lock->mutex);
}

#ifdef __cplusplus
}
#endif

#endif
