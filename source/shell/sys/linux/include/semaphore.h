#ifndef __VSF_LINUX_SEMAPHORE_H__
#define __VSF_LINUX_SEMAPHORE_H__

#include "shell/sys/linux/vsf_linux_cfg.h"
#include "kernel/vsf_kernel.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_TIME == ENABLED
#   include "./simple_libc/time.h"
#else
#   include <time.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define sem_init            VSF_LINUX_WRAPPER(sem_init)
#define sem_destroy         VSF_LINUX_WRAPPER(sem_destroy)
#define sem_wait            VSF_LINUX_WRAPPER(sem_wait)
#define sem_trywait         VSF_LINUX_WRAPPER(sem_trywait)
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
#   define sem_timedwait    VSF_LINUX_WRAPPER(sem_timedwait)
#endif
#define sem_post            VSF_LINUX_WRAPPER(sem_post)
#define sem_getvalue        VSF_LINUX_WRAPPER(sem_getvalue)
#endif

typedef vsf_sem_t sem_t;

int sem_init(sem_t *sem, int pshared, unsigned int value);
int sem_destroy(sem_t *sem);
int sem_wait(sem_t *sem);
int sem_trywait(sem_t *sem);
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
#endif
int sem_post(sem_t *sem);
int sem_getvalue(sem_t *sem, int *value);

#ifdef __cplusplus
}
#endif

#endif
