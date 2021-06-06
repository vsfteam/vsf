#ifndef __VSF_LINUX_PTHREAD_H__
#define __VSF_LINUX_PTHREAD_H__

#include "shell/sys/linux/vsf_linux_cfg.h"
#include "vsf.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_TIME == ENABLED
#   include "./simple_libc/time.h"
#   include "./sched.h"
#else
#   include <time.h>
#   include <sched.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_FAKE_API == ENABLED
#define pthread_self                    __vsf_linux_pthread_self
#define pthread_create                  __vsf_linux_pthread_create
#define pthread_join                    __vsf_linux_pthread_join
#define pthread_exit                    __vsf_linux_pthread_exit
#define pthread_cancel                  __vsf_linux_pthread_cancel
#define pthread_kill                    __vsf_linux_pthread_kill
#define pthread_once                    __vsf_linux_pthread_once
#define pthread_attr_init               __vsf_linux_pthread_attr_init
#define pthread_attr_destroy            __vsf_linux_pthread_attr_destroy
#define pthread_attr_setstack           __vsf_linux_pthread_attr_setstack
#define pthread_attr_getstack           __vsf_linux_pthread_attr_getstack
#define pthread_attr_setstackaddr       __vsf_linux_pthread_attr_setstackaddr
#define pthread_attr_getstackaddr       __vsf_linux_pthread_attr_getstackaddr
#define pthread_attr_setstacksize       __vsf_linux_pthread_attr_setstacksize
#define pthread_attr_getstacksize       __vsf_linux_pthread_attr_getstacksize
#define pthread_attr_setguardsize       __vsf_linux_pthread_attr_setguardsize
#define pthread_attr_getguardsize       __vsf_linux_pthread_attr_getguardsize
#define pthread_attr_setdetachstate     __vsf_linux_pthread_attr_setdetachstate
#define pthread_attr_getdetachstate     __vsf_linux_pthread_attr_getdetachstate
#define pthread_attr_setinheritsched    __vsf_linux_pthread_attr_setinheritsched
#define pthread_attr_getinheritsched    __vsf_linux_pthread_attr_getinheritsched
#define pthread_attr_setschedparam      __vsf_linux_pthread_attr_setschedparam
#define pthread_attr_getschedparam      __vsf_linux_pthread_attr_getschedparam
#define pthread_attr_setschedpolicy     __vsf_linux_pthread_attr_setschedpolicy
#define pthread_attr_getschedpolicy     __vsf_linux_pthread_attr_getschedpolicy
#define pthread_attr_setscope           __vsf_linux_pthread_attr_setscope
#define pthread_attr_getscope           __vsf_linux_pthread_attr_getscope

#define pthread_key_create              __vsf_linux_pthread_key_create
#define pthread_setspecific             __vsf_linux_pthread_setspecific
#define pthread_getspecific             __vsf_linux_pthread_getspecific

#define pthread_mutex_init              __vsf_linux_pthread_mutex_init
#define pthread_mutex_destroy           __vsf_linux_pthread_mutex_destroy
#define pthread_mutex_lock              __vsf_linux_pthread_mutex_lock
#define pthread_mutex_trylock           __vsf_linux_pthread_mutex_trylock
#define pthread_mutex_unlock            __vsf_linux_pthread_mutex_unlock
#define pthread_mutexattr_init          __vsf_linux_pthread_mutexattr_init
#define pthread_mutexattr_destroy       __vsf_linux_pthread_mutexattr_destroy
#define pthread_mutexattr_setpshared    __vsf_linux_pthread_mutexattr_setpshared
#define pthread_mutexattr_getpshared    __vsf_linux_pthread_mutexattr_getpshared
#define pthread_mutexattr_settype       __vsf_linux_pthread_mutexattr_settype
#define pthread_mutexattr_gettype       __vsf_linux_pthread_mutexattr_gettype

#define pthread_cond_init               __vsf_linux_pthread_cond_init
#define pthread_cond_destroy            __vsf_linux_pthread_cond_destroy
#define pthread_cond_signal             __vsf_linux_pthread_cond_signal
#define pthread_cond_broadcast          __vsf_linux_pthread_cond_broadcast
#define pthread_cond_wait               __vsf_linux_pthread_cond_wait
#define pthread_cond_timedwait          __vsf_linux_pthread_cond_timedwait
#define pthread_condattr_init           __vsf_linux_pthread_condattr_init
#define pthread_condattr_destroy        __vsf_linux_pthread_condattr_destroy
#define pthread_condattr_setpshared     __vsf_linux_pthread_condattr_setpshared
#define pthread_condattr_getpshared     __vsf_linux_pthread_condattr_getpshared
#define pthread_condattr_setclock       __vsf_linux_pthread_condattr_setclock
#define pthread_condattr_getclock       __vsf_linux_pthread_condattr_getclock
#endif

// to use PTHREAD_MUTEX_INITIALIZER, __VSF_EDA_CLASS_INHERIT__ is needed or ooc is disabled
#define PTHREAD_MUTEX_INITIALIZER       { .use_as__vsf_mutex_t.use_as__vsf_sync_t.max_union.max_value = 1 }
#define PTHREAD_COND_INITIALIZER        { .use_as__vsf_mutex_t.use_as__vsf_sync_t.max_union.max_value = 1 | VSF_SYNC_AUTO_RST }



typedef int pthread_key_t;

int pthread_key_create(pthread_key_t *key, void (*destructor)(void*));
int pthread_setspecific(pthread_key_t key, const void *value);
void *pthread_getspecific(pthread_key_t key);



enum {
    // pshared
    PTHREAD_PROCESS_SHARED              = 0,
    PTHREAD_PROCESS_PRIVATE             = 1 << 0,

    // mutex
    PTHREAD_MUTEX_ERRORCHECK            = 1 << 1,
    PTHREAD_MUTEX_RECURSIVE             = 1 << 2,
    PTHREAD_MUTEX_NORMAL                = 0,
    PTHREAD_MUTEX_DEFAULT               = 0,

    // cond
};
typedef struct {
    int                                 attr;
} pthread_mutexattr_t;
typedef struct pthread_mutex_t {
    implement(vsf_mutex_t)
    int                                 attr;
    int                                 recursive_cnt;
} pthread_mutex_t;

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mattr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutexattr_init(pthread_mutexattr_t *mattr);
int pthread_mutexattr_destroy(pthread_mutexattr_t *mattr);
int pthread_mutexattr_setpshared(pthread_mutexattr_t *mattr, int pshared);
int pthread_mutexattr_getpshared(pthread_mutexattr_t *mattr, int *pshared);
int pthread_mutexattr_settype(pthread_mutexattr_t *mattr , int type);
int pthread_mutexattr_gettype(pthread_mutexattr_t *mattr , int *type);



typedef vsf_trig_t pthread_cond_t;
typedef struct {
    int                                 attr;
    clockid_t                           clockid;
} pthread_condattr_t;

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
int pthread_cond_destroy(pthread_cond_t *cond);
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
        const struct timespec *abstime);
int pthread_condattr_init(pthread_condattr_t *cattr);
int pthread_condattr_destroy(pthread_condattr_t *cattr);
int pthread_condattr_setpshared(pthread_condattr_t *cattr, int pshared);
int pthread_condattr_getpshared(pthread_condattr_t *cattr, int *pshared);
int pthread_condattr_getclock(const pthread_condattr_t *cattr, clockid_t *clock_id);
int pthread_condattr_setclock(pthread_condattr_t *cattr, clockid_t clock_id);



typedef int pthread_t;
typedef struct pthread_once_t {
    pthread_mutex_t                     mutex;
    bool                                is_inited;
} pthread_once_t;
typedef struct {
    int                                 detachstate;
    int                                 schedpolicy;
    struct sched_param                  schedparam;
    int                                 inheritsched;
    int                                 scope;
    size_t                              guardsize;
    void                               *stackaddr;
    size_t                              stacksize;
} pthread_attr_t;

pthread_t pthread_self(void);
int pthread_create(pthread_t *tidp, const pthread_attr_t *attr, void * (*start_rtn)(void *), void *arg);
int pthread_join(pthread_t tid, void **retval);
void pthread_exit(void *retval);
int pthread_cancel(pthread_t thread);
int pthread_kill(pthread_t thread, int sig);
int pthread_once(pthread_once_t *once_control, void (*init_routine)(void));
int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_destroy(pthread_attr_t *attr);
int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize);
int pthread_attr_getstack(const pthread_attr_t *attr, void **stackaddr, size_t *stacksize);
int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr);
int pthread_attr_getstackaddr(const pthread_attr_t *attr, void **stackaddr);
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);
int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize);
int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize);
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);
int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched);
int pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inheritsched);
int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param);
int pthread_attr_getschedparam(pthread_attr_t *attr, struct sched_param *param);
int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);
int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy);
int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope);
int pthread_attr_getscope(const pthread_attr_t *attr, int *contentionscope);


#ifdef __cplusplus
}
#endif

#endif
