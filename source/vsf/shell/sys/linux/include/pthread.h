#ifndef __PTHREAD_H__
#define __PTHREAD_H__

#include "vsf.h"

#include <time.h>

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



#endif
