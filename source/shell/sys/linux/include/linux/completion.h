#ifndef __VSF_LINUX_CRYPTO_COMPLETION_H__
#define __VSF_LINUX_CRYPTO_COMPLETION_H__

#include "kernel/vsf_kernel.h"

struct completion {
    vsf_sem_t sem;
};

static inline void init_completion(struct completion *x)
{
    vsf_eda_sem_init(&x->sem);
}

static inline void reinit_completion(struct completion *x)
{
    vsf_eda_sem_init(&x->sem);
}

static inline void wait_for_completion(struct completion *x)
{
    __vsf_thread_wait_for_sync(&x->sem, -1);
}

void complete(struct completion *);

#endif
