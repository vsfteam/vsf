#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include "kernel/vsf_kernel.h"

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

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
