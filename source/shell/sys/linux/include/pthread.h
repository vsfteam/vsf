#ifndef __VSF_LINUX_PTHREAD_H__
#define __VSF_LINUX_PTHREAD_H__

#include "shell/sys/linux/vsf_linux_cfg.h"
#include "vsf.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_TIME == ENABLED
#   include "./simple_libc/time.h"
#else
#   include <time.h>
#endif
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./sched.h"
#else
#   include <sched.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define pthread_atfork                  VSF_LINUX_WRAPPER(pthread_atfork)
#define pthread_self                    VSF_LINUX_WRAPPER(pthread_self)
#define pthread_equal                   VSF_LINUX_WRAPPER(pthread_equal)
#define pthread_create                  VSF_LINUX_WRAPPER(pthread_create)
#define pthread_join                    VSF_LINUX_WRAPPER(pthread_join)
#define pthread_detach                  VSF_LINUX_WRAPPER(pthread_detach)
#define pthread_exit                    VSF_LINUX_WRAPPER(pthread_exit)
#define pthread_cancel                  VSF_LINUX_WRAPPER(pthread_cancel)
#define pthread_kill                    VSF_LINUX_WRAPPER(pthread_kill)
#define pthread_once                    VSF_LINUX_WRAPPER(pthread_once)
#define pthread_testcancel              VSF_LINUX_WRAPPER(pthread_testcancel)
#define pthread_setcancelstate          VSF_LINUX_WRAPPER(pthread_setcancelstate)
#define pthread_setcanceltype           VSF_LINUX_WRAPPER(pthread_setcanceltype)
#define pthread_setschedparam           VSF_LINUX_WRAPPER(pthread_setschedparam)
#define pthread_getschedparam           VSF_LINUX_WRAPPER(pthread_getschedparam)
#define pthread_cleanup_push            VSF_LINUX_WRAPPER(pthread_cleanup_push)
#define pthread_cleanup_pop             VSF_LINUX_WRAPPER(pthread_cleanup_pop)
#define pthread_attr_init               VSF_LINUX_WRAPPER(pthread_attr_init)
#define pthread_attr_destroy            VSF_LINUX_WRAPPER(pthread_attr_destroy)
#define pthread_attr_setstack           VSF_LINUX_WRAPPER(pthread_attr_setstack)
#define pthread_attr_getstack           VSF_LINUX_WRAPPER(pthread_attr_getstack)
#define pthread_attr_setstackaddr       VSF_LINUX_WRAPPER(pthread_attr_setstackaddr)
#define pthread_attr_getstackaddr       VSF_LINUX_WRAPPER(pthread_attr_getstackaddr)
#define pthread_attr_setstacksize       VSF_LINUX_WRAPPER(pthread_attr_setstacksize)
#define pthread_attr_getstacksize       VSF_LINUX_WRAPPER(pthread_attr_getstacksize)
#define pthread_attr_setguardsize       VSF_LINUX_WRAPPER(pthread_attr_setguardsize)
#define pthread_attr_getguardsize       VSF_LINUX_WRAPPER(pthread_attr_getguardsize)
#define pthread_attr_setdetachstate     VSF_LINUX_WRAPPER(pthread_attr_setdetachstate)
#define pthread_attr_getdetachstate     VSF_LINUX_WRAPPER(pthread_attr_getdetachstate)
#define pthread_attr_setinheritsched    VSF_LINUX_WRAPPER(pthread_attr_setinheritsched)
#define pthread_attr_getinheritsched    VSF_LINUX_WRAPPER(pthread_attr_getinheritsched)
#define pthread_attr_setschedparam      VSF_LINUX_WRAPPER(pthread_attr_setschedparam)
#define pthread_attr_getschedparam      VSF_LINUX_WRAPPER(pthread_attr_getschedparam)
#define pthread_attr_setschedpolicy     VSF_LINUX_WRAPPER(pthread_attr_setschedpolicy)
#define pthread_attr_getschedpolicy     VSF_LINUX_WRAPPER(pthread_attr_getschedpolicy)
#define pthread_attr_setscope           VSF_LINUX_WRAPPER(pthread_attr_setscope)
#define pthread_attr_getscope           VSF_LINUX_WRAPPER(pthread_attr_getscope)

#define pthread_key_create              VSF_LINUX_WRAPPER(pthread_key_create)
#define pthread_key_delete              VSF_LINUX_WRAPPER(pthread_key_delete)
#define pthread_setspecific             VSF_LINUX_WRAPPER(pthread_setspecific)
#define pthread_getspecific             VSF_LINUX_WRAPPER(pthread_getspecific)

#define pthread_mutex_init              VSF_LINUX_WRAPPER(pthread_mutex_init)
#define pthread_mutex_destroy           VSF_LINUX_WRAPPER(pthread_mutex_destroy)
#define pthread_mutex_lock              VSF_LINUX_WRAPPER(pthread_mutex_lock)
#define pthread_mutex_timedlock         VSF_LINUX_WRAPPER(pthread_mutex_timedlock)
#define pthread_mutex_trylock           VSF_LINUX_WRAPPER(pthread_mutex_trylock)
#define pthread_mutex_unlock            VSF_LINUX_WRAPPER(pthread_mutex_unlock)
#define pthread_mutexattr_init          VSF_LINUX_WRAPPER(pthread_mutexattr_init)
#define pthread_mutexattr_destroy       VSF_LINUX_WRAPPER(pthread_mutexattr_destroy)
#define pthread_mutexattr_setpshared    VSF_LINUX_WRAPPER(pthread_mutexattr_setpshared)
#define pthread_mutexattr_getpshared    VSF_LINUX_WRAPPER(pthread_mutexattr_getpshared)
#define pthread_mutexattr_settype       VSF_LINUX_WRAPPER(pthread_mutexattr_settype)
#define pthread_mutexattr_gettype       VSF_LINUX_WRAPPER(pthread_mutexattr_gettype)

#define pthread_cond_init               VSF_LINUX_WRAPPER(pthread_cond_init)
#define pthread_cond_destroy            VSF_LINUX_WRAPPER(pthread_cond_destroy)
#define pthread_cond_signal             VSF_LINUX_WRAPPER(pthread_cond_signal)
#define pthread_cond_broadcast          VSF_LINUX_WRAPPER(pthread_cond_broadcast)
#define pthread_cond_wait               VSF_LINUX_WRAPPER(pthread_cond_wait)
#define pthread_cond_timedwait          VSF_LINUX_WRAPPER(pthread_cond_timedwait)
#define pthread_condattr_init           VSF_LINUX_WRAPPER(pthread_condattr_init)
#define pthread_condattr_destroy        VSF_LINUX_WRAPPER(pthread_condattr_destroy)
#define pthread_condattr_setpshared     VSF_LINUX_WRAPPER(pthread_condattr_setpshared)
#define pthread_condattr_getpshared     VSF_LINUX_WRAPPER(pthread_condattr_getpshared)
#define pthread_condattr_setclock       VSF_LINUX_WRAPPER(pthread_condattr_setclock)
#define pthread_condattr_getclock       VSF_LINUX_WRAPPER(pthread_condattr_getclock)

#define pthread_rwlock_init             VSF_LINUX_WRAPPER(pthread_rwlock_init)
#define pthread_rwlock_destroy          VSF_LINUX_WRAPPER(pthread_rwlock_destroy)
#define pthread_rwlock_rdlock           VSF_LINUX_WRAPPER(pthread_rwlock_rdlock)
#define pthread_rwlock_tryrdlock        VSF_LINUX_WRAPPER(pthread_rwlock_tryrdlock)
#define pthread_rwlock_timedrdlock      VSF_LINUX_WRAPPER(pthread_rwlock_timedrdlock)
#define pthread_rwlock_wrlock           VSF_LINUX_WRAPPER(pthread_rwlock_wrlock)
#define pthread_rwlock_trywrlock        VSF_LINUX_WRAPPER(pthread_rwlock_trywrlock)
#define pthread_rwlock_timedwrlock      VSF_LINUX_WRAPPER(pthread_rwlock_timedwrlock)
#define pthread_rwlock_unlock           VSF_LINUX_WRAPPER(pthread_rwlock_unlock)

#define pthread_barrier_init            VSF_LINUX_WRAPPER(pthread_barrier_init)
#define pthread_barrier_destroy         VSF_LINUX_WRAPPER(pthread_barrier_destroy)
#define pthread_barrier_wait            VSF_LINUX_WRAPPER(pthread_barrier_wait)
#define pthread_barrierattr_init        VSF_LINUX_WRAPPER(pthread_barrierattr_init)
#define pthread_barrierattr_destroy     VSF_LINUX_WRAPPER(pthread_barrierattr_destroy)
#define pthread_barrierattr_getpshared  VSF_LINUX_WRAPPER(pthread_barrierattr_getpshared)
#define pthread_barrierattr_setpshared  VSF_LINUX_WRAPPER(pthread_barrierattr_setpshared)

#define pthread_setname_np              VSF_LINUX_WRAPPER(pthread_setname_np)
#define pthread_getname_np              VSF_LINUX_WRAPPER(pthread_getname_np)
#endif

// to use PTHREAD_MUTEX_INITIALIZER, __VSF_EDA_CLASS_INHERIT__ is needed or ooc is disabled
#if __IS_COMPILER_IAR__
#define PTHREAD_MUTEX_INITIALIZER       {                                       \
                                            .use_as__vsf_mutex_t.use_as__vsf_sync_t.max_union.max_value = 1 | VSF_SYNC_AUTO_RST,\
                                            .use_as__vsf_mutex_t.use_as__vsf_sync_t.cur_union.bits.cur = 1 | VSF_SYNC_HAS_OWNER,\
                                        }
#define PTHREAD_COND_INITIALIZER        {                                       \
                                             .max_union.max_value = 1 | VSF_SYNC_AUTO_RST,\
                                        }
#else
#define PTHREAD_MUTEX_INITIALIZER       (pthread_mutex_t) {                     \
                                            .use_as__vsf_mutex_t.use_as__vsf_sync_t.max_union.max_value = 1 | VSF_SYNC_AUTO_RST,\
                                            .use_as__vsf_mutex_t.use_as__vsf_sync_t.cur_union.bits.cur = 1 | VSF_SYNC_HAS_OWNER,\
                                        }
#define PTHREAD_COND_INITIALIZER        (pthread_cond_t) {                      \
                                             .max_union.max_value = 1 | VSF_SYNC_AUTO_RST,\
                                        }
#endif


typedef int pthread_key_t;

enum {
    // pshared
    PTHREAD_PROCESS_SHARED              = 0,
#define PTHREAD_PROCESS_SHARED          PTHREAD_PROCESS_SHARED
    PTHREAD_PROCESS_PRIVATE             = 1 << 0,
#define PTHREAD_PROCESS_PRIVATE         PTHREAD_PROCESS_PRIVATE

    PTHREAD_CREATE_JOINABLE             = 0,
#define PTHREAD_CREATE_JOINABLE         PTHREAD_CREATE_JOINABLE
    PTHREAD_CREATE_DETACHED             = 1,
#define PTHREAD_CREATE_DETACHED         PTHREAD_CREATE_DETACHED

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

typedef vsf_trig_t pthread_cond_t;
typedef struct {
    int                                 attr;
    clockid_t                           clockid;
} pthread_condattr_t;

#define PTHREAD_RWLOCK_INITIALIZER      { 0 }
typedef struct pthread_rwlock_t {
    uint16_t rdref;
    uint16_t wrref;
    uint16_t rdpend;
    uint16_t wrpend;
    vsf_dlist_t rdlist;
    vsf_dlist_t wrlist;
    vsf_sync_t rdsync;
    vsf_sync_t wrsync;
} pthread_rwlock_t;
typedef struct {
    int                                 attr;
} pthread_rwlockattr_t;

typedef struct {
    int                                 attr;
} pthread_barrierattr_t;
typedef struct {
    pthread_mutex_t                     mutex;
    pthread_cond_t                      cond;
    unsigned                            threshold;
    unsigned                            in;
    unsigned                            out;
} pthread_barrier_t;
#define PTHREAD_BARRIER_SERIAL_THREAD   (-2)

typedef int pthread_t;
typedef struct pthread_once_t {
    pthread_mutex_t                     mutex;
    bool                                is_inited;
} pthread_once_t;
#if __IS_COMPILER_IAR__
#define PTHREAD_ONCE_INIT               {                                       \
                                            .mutex.max_union.max_value = 1 | VSF_SYNC_AUTO_RST,\
                                            .mutex.cur_union.cur_value = 1 | VSF_SYNC_HAS_OWNER,\
                                            .is_inited = false,                 \
                                        }
#else
#define PTHREAD_ONCE_INIT               (pthread_once_t) {                      \
                                            .mutex.use_as__vsf_mutex_t.use_as__vsf_sync_t.max_union.max_value = 1 | VSF_SYNC_AUTO_RST,\
                                            .mutex.use_as__vsf_mutex_t.use_as__vsf_sync_t.cur_union.cur_value = 1 | VSF_SYNC_HAS_OWNER,\
                                            .is_inited = false,                 \
                                        }
#endif
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

enum {
    PTHREAD_CANCEL_ENABLE,              // default
    PTHREAD_CANCEL_DISABLE,
};
enum {
    PTHREAD_CANCEL_DEFERRED,            // default
    PTHREAD_CANCEL_ASYNCHRONOUS,
};

#ifndef PTHREAD_STACK_MIN
#   define PTHREAD_STACK_MIN            1024
#endif

#if VSF_LINUX_APPLET_USE_PTHREAD == ENABLED
typedef struct vsf_linux_pthread_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_key_create);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_key_delete);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_setspecific);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_getspecific);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_mutex_init);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_mutex_destroy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_mutex_lock);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_mutex_timedlock);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_mutex_trylock);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_mutex_unlock);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_mutexattr_init);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_mutexattr_destroy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_mutexattr_setpshared);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_mutexattr_getpshared);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_mutexattr_settype);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_mutexattr_gettype);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_cond_init);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_cond_destroy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_cond_signal);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_cond_broadcast);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_cond_wait);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_cond_timedwait);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_condattr_init);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_condattr_destroy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_condattr_setpshared);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_condattr_getpshared);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_condattr_getclock);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_condattr_setclock);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_rwlock_init);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_rwlock_destroy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_rwlock_rdlock);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_rwlock_tryrdlock);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_rwlock_timedrdlock);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_rwlock_wrlock);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_rwlock_trywrlock);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_rwlock_timedwrlock);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_rwlock_unlock);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_atfork);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_self);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_equal);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_create);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_join);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_detach);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_exit);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_cancel);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_kill);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_once);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_testcancel);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_setcancelstate);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_setcanceltype);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_setschedparam);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_getschedparam);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_cleanup_push);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_cleanup_pop);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_attr_init);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_attr_destroy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_attr_setstack);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_attr_getstack);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_attr_setstackaddr);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_attr_getstackaddr);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_attr_setstacksize);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_attr_getstacksize);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_attr_setguardsize);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_attr_getguardsize);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_attr_setdetachstate);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_attr_getdetachstate);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_attr_setinheritsched);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_attr_getinheritsched);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_attr_setschedparam);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_attr_getschedparam);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_attr_setschedpolicy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_attr_getschedpolicy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_attr_setscope);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_attr_getscope);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_barrier_init);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_barrier_destroy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_barrier_wait);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_barrierattr_init);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_barrierattr_destroy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_barrierattr_getpshared);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_barrierattr_setpshared);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_setname_np);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pthread_getname_np);
} vsf_linux_pthread_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_pthread_vplt_t vsf_linux_pthread_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_PTHREAD_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_PTHREAD == ENABLED

#ifndef VSF_LINUX_APPLET_PTHREAD_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_PTHREAD_VPLT                                    \
            ((vsf_linux_pthread_vplt_t *)(VSF_LINUX_APPLET_VPLT->pthread_vplt))
#   else
#       define VSF_LINUX_APPLET_PTHREAD_VPLT                                    \
            ((vsf_linux_pthread_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_PTHREAD_ENTRY(__NAME)                                  \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_PTHREAD_VPLT, __NAME)
#define VSF_LINUX_APPLET_PTHREAD_IMP(...)                                       \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_PTHREAD_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_PTHREAD_IMP(pthread_key_create, int, pthread_key_t *key, void (*destructor)(void*)) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_key_create)(key, destructor);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_key_delete, int, pthread_key_t key) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_key_delete)(key);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_setspecific, int, pthread_key_t key, const void *value) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_setspecific)(key, value);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_getspecific, void *, pthread_key_t key) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_getspecific)(key);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_mutex_init, int, pthread_mutex_t *mutex, const pthread_mutexattr_t *mattr) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_mutex_init)(mutex, mattr);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_mutex_destroy, int, pthread_mutex_t *mutex) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_mutex_destroy)(mutex);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_mutex_lock, int, pthread_mutex_t *mutex) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_mutex_lock)(mutex);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_mutex_timedlock, int, pthread_mutex_t *mutex, const struct timespec *abstime) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_mutex_timedlock)(mutex, abstime);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_mutex_trylock, int, pthread_mutex_t *mutex) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_mutex_trylock)(mutex);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_mutex_unlock, int, pthread_mutex_t *mutex) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_mutex_unlock)(mutex);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_mutexattr_init, int, pthread_mutexattr_t *mattr) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_mutexattr_init)(mattr);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_mutexattr_destroy, int, pthread_mutexattr_t *mattr) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_mutexattr_destroy)(mattr);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_mutexattr_setpshared, int, pthread_mutexattr_t *mattr, int pshared) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_mutexattr_setpshared)(mattr, pshared);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_mutexattr_getpshared, int, pthread_mutexattr_t *mattr, int *pshared) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_mutexattr_getpshared)(mattr, pshared);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_mutexattr_settype, int, pthread_mutexattr_t *mattr , int type) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_mutexattr_settype)(mattr, type);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_mutexattr_gettype, int, pthread_mutexattr_t *mattr , int *type) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_mutexattr_gettype)(mattr, type);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_cond_init, int, pthread_cond_t *cond, const pthread_condattr_t *cattr) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_cond_init)(cond, cattr);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_cond_destroy, int, pthread_cond_t *cond) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_cond_destroy)(cond);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_cond_signal, int, pthread_cond_t *cond) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_cond_signal)(cond);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_cond_broadcast, int, pthread_cond_t *cond) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_cond_broadcast)(cond);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_cond_wait, int, pthread_cond_t *cond, pthread_mutex_t *mutex) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_cond_wait)(cond, mutex);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_cond_timedwait, int, pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_cond_timedwait)(cond, mutex, abstime);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_condattr_init, int, pthread_condattr_t *cattr) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_condattr_init)(cattr);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_condattr_destroy, int, pthread_condattr_t *cattr) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_condattr_destroy)(cattr);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_condattr_setpshared, int, pthread_condattr_t *cattr, int pshared) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_condattr_setpshared)(cattr, pshared);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_condattr_getpshared, int, pthread_condattr_t *cattr, int *pshared) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_condattr_getpshared)(cattr, pshared);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_condattr_getclock, int, const pthread_condattr_t *cattr, clockid_t *clockid) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_condattr_getclock)(cattr, clockid);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_condattr_setclock, int, pthread_condattr_t *cattr, clockid_t clockid) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_condattr_setclock)(cattr, clockid);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_rwlock_init, int, pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_rwlock_init)(rwlock, attr);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_rwlock_destroy, int, pthread_rwlock_t *rwlock) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_rwlock_destroy)(rwlock);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_rwlock_rdlock, int, pthread_rwlock_t *rwlock) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_rwlock_rdlock)(rwlock);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_rwlock_tryrdlock, int, pthread_rwlock_t *rwlock) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_rwlock_tryrdlock)(rwlock);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_rwlock_timedrdlock, int, pthread_rwlock_t *rwlock, const struct timespec *abstime) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_rwlock_timedrdlock)(rwlock, abstime);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_rwlock_wrlock, int, pthread_rwlock_t *rwlock) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_rwlock_wrlock)(rwlock);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_rwlock_trywrlock, int, pthread_rwlock_t *rwlock) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_rwlock_trywrlock)(rwlock);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_rwlock_timedwrlock, int, pthread_rwlock_t *rwlock, const struct timespec *abstime) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_rwlock_timedwrlock)(rwlock, abstime);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_rwlock_unlock, int, pthread_rwlock_t *rwlock) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_rwlock_unlock)(rwlock);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_atfork, int, void (*prepare)(void), void (*parent)(void), void (*child)(void)) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_atfork)(prepare, parent, child);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_self, pthread_t, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_self)();
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_equal, int, pthread_t t1, pthread_t t2) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_equal)(t1, t2);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_create, int, pthread_t *tidp, const pthread_attr_t *attr, void * (*start_rtn)(void *), void *arg) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_create)(tidp, attr, start_rtn, arg);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_join, int, pthread_t tid, void **retval) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_join)(tid, retval);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_detach, int, pthread_t thread) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_detach)(thread);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_exit, void, void *retval) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_exit)(retval);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_cancel, int, pthread_t thread) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_cancel)(thread);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_kill, int, pthread_t thread, int sig) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_kill)(thread, sig);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_once, int, pthread_once_t *once_control, void (*init_routine)(void)) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_once)(once_control, init_routine);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_testcancel, void, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_testcancel)();
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_setcancelstate, int, int state, int *oldstate) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_setcancelstate)(state, oldstate);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_setcanceltype, int, int type, int *oldtype) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_setcanceltype)(type, oldtype);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_setschedparam, int, pthread_t thread, int policy, const struct sched_param *param) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_setschedparam)(thread, policy, param);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_getschedparam, int, pthread_t thread, int *policy, struct sched_param *param) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_getschedparam)(thread, policy, param);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_cleanup_push, void, void (*routine)(void *), void *arg) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_cleanup_push)(routine, arg);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_cleanup_pop, void, int execute) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_cleanup_pop)(execute);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_attr_init, int, pthread_attr_t *attr) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_attr_init)(attr);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_attr_destroy, int, pthread_attr_t *attr) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_attr_destroy)(attr);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_attr_setstack, int, pthread_attr_t *attr, void *stackaddr, size_t stacksize) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_attr_setstack)(attr, stackaddr, stacksize);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_attr_getstack, int, const pthread_attr_t *attr, void **stackaddr, size_t *stacksize) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_attr_getstack)(attr, stackaddr, stacksize);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_attr_setstackaddr, int, pthread_attr_t *attr, void *stackaddr) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_attr_setstackaddr)(attr, stackaddr);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_attr_getstackaddr, int, const pthread_attr_t *attr, void **stackaddr) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_attr_getstackaddr)(attr, stackaddr);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_attr_setstacksize, int, pthread_attr_t *attr, size_t stacksize) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_attr_setstacksize)(attr, stacksize);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_attr_getstacksize, int, const pthread_attr_t *attr, size_t *stacksize) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_attr_getstacksize)(attr, stacksize);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_attr_setguardsize, int, pthread_attr_t *attr, size_t guardsize) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_attr_setguardsize)(attr, guardsize);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_attr_getguardsize, int, const pthread_attr_t *attr, size_t *guardsize) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_attr_getguardsize)(attr, guardsize);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_attr_setdetachstate, int, pthread_attr_t *attr, int detachstate) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_attr_setdetachstate)(attr, detachstate);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_attr_getdetachstate, int, const pthread_attr_t *attr, int *detachstate) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_attr_getdetachstate)(attr, detachstate);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_attr_setinheritsched, int, pthread_attr_t *attr, int inheritsched) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_attr_setinheritsched)(attr, inheritsched);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_attr_getinheritsched, int, const pthread_attr_t *attr, int *inheritsched) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_attr_getinheritsched)(attr, inheritsched);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_attr_setschedparam, int, pthread_attr_t *attr, const struct sched_param *param) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_attr_setschedparam)(attr, param);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_attr_getschedparam, int, pthread_attr_t *attr, struct sched_param *param) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_attr_getschedparam)(attr, param);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_attr_setschedpolicy, int, pthread_attr_t *attr, int policy) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_attr_setschedpolicy)(attr, policy);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_attr_getschedpolicy, int, const pthread_attr_t *attr, int *policy) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_attr_getschedpolicy)(attr, policy);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_attr_setscope, int, pthread_attr_t *attr, int contentionscope) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_attr_setscope)(attr, contentionscope);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_attr_getscope, int, const pthread_attr_t *attr, int *contentionscope) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_attr_getscope)(attr, contentionscope);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_barrier_init, int, pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_barrier_init)(barrier, attr, count);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_barrier_destroy, int, pthread_barrier_t *barrier) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_barrier_destroy)(barrier);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_barrier_wait, int, pthread_barrier_t *barrier) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_barrier_wait)(barrier);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_barrierattr_init, int, pthread_barrierattr_t *attr) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_barrierattr_init)(attr);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_barrierattr_destroy, int, pthread_barrierattr_t *attr) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_barrierattr_destroy)(attr);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_barrierattr_getpshared, int, const pthread_barrierattr_t *attr, int *pshared) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_barrierattr_getpshared)(attr, pshared);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_barrierattr_setpshared, int, pthread_barrierattr_t *attr, int pshared) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_barrierattr_setpshared)(attr, pshared);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_setname_np, int, pthread_t thread, const char *name) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_setname_np)(thread, name);
}
VSF_LINUX_APPLET_PTHREAD_IMP(pthread_getname_np, int, pthread_t thread, char *name, size_t size) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTHREAD_ENTRY(pthread_getname_np)(thread, name, size);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_PTHREAD

#if VSF_LINUX_CFG_TLS_NUM > 0
int pthread_key_create(pthread_key_t *key, void (*destructor)(void*));
int pthread_key_delete(pthread_key_t key);
int pthread_setspecific(pthread_key_t key, const void *value);
void * pthread_getspecific(pthread_key_t key);
#endif

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mattr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abstime);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutexattr_init(pthread_mutexattr_t *mattr);
int pthread_mutexattr_destroy(pthread_mutexattr_t *mattr);
int pthread_mutexattr_setpshared(pthread_mutexattr_t *mattr, int pshared);
int pthread_mutexattr_getpshared(pthread_mutexattr_t *mattr, int *pshared);
int pthread_mutexattr_settype(pthread_mutexattr_t *mattr , int type);
int pthread_mutexattr_gettype(pthread_mutexattr_t *mattr , int *type);

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *cattr);
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
int pthread_condattr_getclock(const pthread_condattr_t *cattr, clockid_t *clockid);
int pthread_condattr_setclock(pthread_condattr_t *cattr, clockid_t clockid);

int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr);
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock, const struct timespec *abstime);
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock, const struct timespec *abstime);
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);

int pthread_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void));
pthread_t pthread_self(void);
int pthread_equal(pthread_t t1, pthread_t t2);
int pthread_create(pthread_t *tidp, const pthread_attr_t *attr, void * (*start_rtn)(void *), void *arg);
int pthread_join(pthread_t tid, void **retval);
int pthread_detach(pthread_t thread);
void pthread_exit(void *retval);
int pthread_cancel(pthread_t thread);
int pthread_kill(pthread_t thread, int sig);
int pthread_once(pthread_once_t *once_control, void (*init_routine)(void));
void pthread_testcancel(void);
int pthread_setcancelstate(int state, int *oldstate);
int pthread_setcanceltype(int type, int *oldtype);
int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param);
int pthread_getschedparam(pthread_t thread, int *policy, struct sched_param *param);
void pthread_cleanup_push(void (*routine)(void *), void *arg);
void pthread_cleanup_pop(int execute);

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

int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count);
int pthread_barrier_destroy(pthread_barrier_t *barrier);
int pthread_barrier_wait(pthread_barrier_t *barrier);
int pthread_barrierattr_init(pthread_barrierattr_t *battr);
int pthread_barrierattr_destroy(pthread_barrierattr_t *battr);
int pthread_barrierattr_getpshared(const pthread_barrierattr_t *battr, int *pshared);
int pthread_barrierattr_setpshared(pthread_barrierattr_t *battr, int pshared);

int pthread_setname_np(pthread_t thread, const char *name);
int pthread_getname_np(pthread_t thread, char *name, size_t size);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_PTHREAD

#ifdef __cplusplus
}
#endif

#endif
