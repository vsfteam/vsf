#ifndef __PTHREAD_H__
#define __PTHREAD_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_TIME == ENABLED
#   include "./simple_libc/time.h"
#else
#   include <time.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define pthread_self                __vsf_linux_pthread_self
#define pthread_create              __vsf_linux_pthread_create
#define pthread_join                __vsf_linux_pthread_join
#define pthread_exit                __vsf_linux_pthread_exit
#define pthread_cancel              __vsf_linux_pthread_cancel
#define pthread_kill                __vsf_linux_pthread_kill

#define pthread_key_create          __vsf_linux_pthread_key_create
#define pthread_setspecific         __vsf_linux_pthread_setspecific
#define pthread_getspecific         __vsf_linux_pthread_getspecific

#define pthread_mutex_init          __vsf_linux_pthread_mutex_init
#define pthread_mutex_destroy       __vsf_linux_pthread_mutex_destroy
#define pthread_mutex_lock          __vsf_linux_pthread_mutex_lock
#define pthread_mutex_unlock        __vsf_linux_pthread_mutex_unlock

#define pthread_cond_init           __vsf_linux_pthread_cond_init
#define pthread_cond_destroy        __vsf_linux_pthread_cond_destroy
#define pthread_cond_signal         __vsf_linux_pthread_cond_signal
#define pthread_cond_broadcast      __vsf_linux_pthread_cond_broadcast
#define pthread_cond_wait           __vsf_linux_pthread_cond_wait
#define pthread_cond_timedwait      __vsf_linux_pthread_cond_timedwait

// PTHREAD_MUTEX_INITIALIZER is not support, please use pthread_mutex_init
//#define PTHREAD_MUTEX_INITIALIZER       { 0 }

typedef int pthread_t;
typedef struct {
    void *stackaddr;
    size_t stacksize;
} pthread_attr_t;

pthread_t pthread_self(void);
int pthread_create(pthread_t *tidp, const pthread_attr_t *attr, void * (*start_rtn)(void *), void *arg);
int pthread_join(pthread_t tid, void **retval);
void pthread_exit(void *retval);
int pthread_cancel(pthread_t thread);
int pthread_kill(pthread_t thread, int sig);



typedef int pthread_key_t;

int pthread_key_create(pthread_key_t *key, void (*destructor)(void*));
int pthread_setspecific(pthread_key_t key, const void *value);
void *pthread_getspecific(pthread_key_t key);



typedef vsf_mutex_t pthread_mutex_t;
typedef struct {
    int dummy;
} pthread_mutexattr_t;

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);



typedef vsf_slist_t pthread_cond_t;
typedef struct {
    int dummy;
} pthread_condattr_t;

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
int pthread_cond_destroy(pthread_cond_t *cond);
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
		const struct timespec *abstime);

#ifdef __cplusplus
}
#endif

#endif
