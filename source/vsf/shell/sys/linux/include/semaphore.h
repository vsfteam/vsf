#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

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

#define sem_init            __vsf_linux_sem_init
#define sem_destory         __vsf_linux_sem_destory
#define sem_wait            __vsf_linux_sem_wait
#define sem_trywait         __vsf_linux_sem_trywait
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
#   define sem_timedwait    __vsf_linux_sem_timedwait
#endif
#define sem_post            __vsf_linux_sem_post

typedef vsf_sem_t sem_t;

int sem_init(sem_t *sem, int pshared, unsigned int value);
int sem_destory(sem_t *sem);
int sem_wait(sem_t *sem);
int sem_trywait(sem_t *sem);
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
#endif
int sem_post(sem_t *sem);

#ifdef __cplusplus
}
#endif

#endif
