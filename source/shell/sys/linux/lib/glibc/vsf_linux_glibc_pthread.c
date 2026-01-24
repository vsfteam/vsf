/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "../../vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_LINUX_CLASS_INHERIT__
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../include/unistd.h"
#   include "../../include/simple_libc/time.h"
#   include "../../include/pthread.h"
#   include "../../include/signal.h"
#   include "../../include/errno.h"
#   include "../../include/sys/wait.h"
#else
#   include <unistd.h>
#   include <time.h>
#   include <pthread.h>
#   include <signal.h>
#   include <errno.h>
#   include <sys/wait.h>
#endif

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_linux_pthread_cleanup_handler_t {
    vsf_slist_node_t node;
    void (*routine)(void *);
    void *arg;
} vsf_linux_pthread_cleanup_handler_t;

typedef struct vsf_linux_pthread_priv_t {
    void *param;
    void * (*entry)(void *param);
    vsf_slist_t cleanup_handler_list;
    int cancel_state;
    int cancel_type;
    bool is_cancelled;
} vsf_linux_pthread_priv_t;

/*============================ PROTOTYPES ====================================*/

static void __vsf_linux_pthread_on_run(vsf_thread_cb_t *cb);
static void __vsf_linux_pthread_on_terminate(vsf_linux_thread_t *thread);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_sync")
void __vsf_eda_sync_pend(vsf_sync_t *sync, vsf_eda_t *eda, vsf_timeout_tick_t timeout);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const vsf_linux_thread_op_t __vsf_linux_pthread_op = {
    .priv_size          = sizeof(vsf_linux_pthread_priv_t),
    .on_run             = __vsf_linux_pthread_on_run,
    .on_terminate       = __vsf_linux_pthread_on_terminate,
};

/*============================ IMPLEMENTATION ================================*/

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#   pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#endif

static vsf_timeout_tick_t __vsf_linux_abstimespec_to_timeout(const struct timespec *abstime)
{
    vsf_timeout_tick_t timeout = -1;
    if (abstime != NULL) {
        struct timespec now, due = *abstime;
        clock_gettime(CLOCK_MONOTONIC, &now);
        vsf_systimer_tick_t now_tick =  vsf_systimer_ms_to_tick(now.tv_sec * 1000)
                                    +   vsf_systimer_us_to_tick(now.tv_nsec / 1000);
        vsf_systimer_tick_t due_tick =  vsf_systimer_ms_to_tick(due.tv_sec * 1000)
                                    +   vsf_systimer_us_to_tick(due.tv_nsec / 1000);
        if (now_tick >= due_tick) {
            return ETIMEDOUT;
        }
        timeout = due_tick - now_tick;
    }
    return timeout;
}

static void __vsf_linux_pthread_on_run(vsf_thread_cb_t *cb)
{
    vsf_linux_thread_t *thread = vsf_container_of(cb, vsf_linux_thread_t, use_as__vsf_thread_cb_t);
    vsf_linux_pthread_priv_t *priv = vsf_linux_thread_get_priv(thread);
    thread->retval = (int)(uintptr_t)priv->entry(priv->param);
}

static void __vsf_linux_pthread_on_terminate(vsf_linux_thread_t *thread)
{
    vsf_linux_pthread_priv_t *priv = vsf_linux_thread_get_priv(thread);
    vsf_linux_pthread_cleanup_handler_t *cleanup_handler;
    vsf_protect_t orig;

    while (true) {
        orig = vsf_protect_sched();
            vsf_slist_remove_from_head(vsf_linux_pthread_cleanup_handler_t, node, &priv->cleanup_handler_list, cleanup_handler);
        vsf_unprotect_sched(orig);
        if (NULL == cleanup_handler) {
            break;
        }

        free(cleanup_handler);
    }

    vsf_linux_thread_on_terminate(thread);
}

bool __vsf_linux_is_pthread_ctx(vsf_linux_thread_t *linux_thread)
{
    return linux_thread->on_terminate == (vsf_eda_on_terminate_t)__vsf_linux_pthread_on_terminate;
}

int pthread_detach(pthread_t tid)
{
    vsf_linux_thread_t *thread = vsf_linux_get_thread(-1, tid);
    if (thread != NULL) {
        vsf_linux_detach_thread(thread);
    }
    return 0;
}

int pthread_join(pthread_t tid, void **retval)
{
    int retval_int;
    int ret = vsf_linux_wait_thread(tid, &retval_int);
    if (retval != NULL) {
        *retval = (void *)(uintptr_t)retval_int;
    }
    return ret;
}

void pthread_exit(void *retval)
{
    extern const vsf_linux_thread_op_t __vsf_linux_main_op;
    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    if (thread != NULL) {
        if (thread->op == &__vsf_linux_pthread_op) {
            thread->retval = (int)(uintptr_t)retval;
            vsf_thread_exit();
        } else if (thread->op == &__vsf_linux_main_op) {
            vsf_linux_process_t *process = vsf_linux_get_cur_process();
            vsf_linux_thread_t *child_thread;
            pid_t tid;
            vsf_protect_t orig;

            while (true) {
                orig = vsf_protect_sched();
                vsf_dlist_peek_head(vsf_linux_thread_t, thread_node, &process->thread_list, child_thread);
                tid = child_thread != NULL ? child_thread->tid : -1;
                vsf_unprotect_sched(orig);
                if (tid >= 0) {
                    vsf_linux_wait_thread(tid, NULL);
                }
            }
            exit(0);
        }
    }
    VSF_LINUX_ASSERT(false);
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

int pthread_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void))
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

pthread_t pthread_self(void)
{
    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    return (NULL == thread) ? 0 : thread->tid;
}

int pthread_equal(pthread_t t1, pthread_t t2)
{
    return t1 == t2;
}

int pthread_create(pthread_t *tidp, const pthread_attr_t *attr, void * (*start_rtn)(void *), void *arg)
{
    if (NULL == attr) {
        static const pthread_attr_t __default_attr = {
            .inheritsched   = 1,
        };
        attr = &__default_attr;
    }

    vsf_linux_pthread_priv_t *priv;
    vsf_linux_thread_t *thread = vsf_linux_create_thread(NULL, &__vsf_linux_pthread_op, attr->stacksize, attr->stackaddr);
    if (!thread) { return -1; }

    priv = vsf_linux_thread_get_priv(thread);
    priv->entry = start_rtn;
    priv->param = arg;

    if (tidp != NULL) {
        *tidp = thread->tid;
    }

    if (attr->detachstate == PTHREAD_CREATE_DETACHED) {
        pthread_detach(thread->tid);
    }

    vsf_prio_t priority = attr->inheritsched ? vsf_prio_inherit : attr->schedparam.sched_priority;
    vsf_linux_start_thread(thread, priority);
    return 0;
}

int pthread_kill(pthread_t thread, int sig)
{
    VSF_LINUX_ASSERT(false);
    // TODO:
    return 0;
}

int pthread_once(pthread_once_t *once_control, void (*init_routine)(void))
{
    pthread_mutex_lock(&once_control->mutex);
    if (!once_control->is_inited) {
        once_control->is_inited = true;
        pthread_mutex_unlock(&(once_control->mutex));

        init_routine();
    } else {
        pthread_mutex_unlock(&(once_control->mutex));
    }
    return 0;
}

int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param)
{
    return 0;
}

int pthread_getschedparam(pthread_t thread, int *policy, struct sched_param *param)
{
    return -1;
}

void pthread_cleanup_push(void (*routine)(void *), void *arg)
{
    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    vsf_linux_pthread_priv_t *priv = vsf_linux_thread_get_priv(thread);

    vsf_linux_pthread_cleanup_handler_t *cleanup_handler = malloc(sizeof(vsf_linux_pthread_cleanup_handler_t));
    VSF_LINUX_ASSERT(cleanup_handler != NULL);

    vsf_slist_init_node(vsf_linux_pthread_cleanup_handler_t, node, cleanup_handler);
    cleanup_handler->arg = arg;
    cleanup_handler->routine = routine;

    vsf_protect_t orig = vsf_protect_sched();
        vsf_slist_add_to_head(vsf_linux_pthread_cleanup_handler_t, node, &priv->cleanup_handler_list, cleanup_handler);
    vsf_unprotect_sched(orig);
}

void pthread_cleanup_pop(int execute)
{
    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    vsf_linux_pthread_priv_t *priv = vsf_linux_thread_get_priv(thread);

    vsf_linux_pthread_cleanup_handler_t *cleanup_handler;
    vsf_protect_t orig = vsf_protect_sched();
        vsf_slist_remove_from_head(vsf_linux_pthread_cleanup_handler_t, node, &priv->cleanup_handler_list, cleanup_handler);
    vsf_unprotect_sched(orig);

    if (cleanup_handler != NULL) {
        if (execute) {
            cleanup_handler->routine(cleanup_handler->arg);
        }
        free(cleanup_handler);
    }
}

int pthread_attr_init(pthread_attr_t *attr)
{
    if (attr != NULL) {
        memset(attr, 0, sizeof(*attr));
        attr->schedparam.sched_priority = vsf_prio_inherit;
        return 0;
    }
    return EINVAL;
}

int pthread_attr_destroy(pthread_attr_t *attr)
{
    return (attr != NULL) ? 0 : EINVAL;
}

int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize)
{
    if (attr != NULL) {
        attr->stackaddr = stackaddr;
        attr->stacksize = stacksize;
        return 0;
    }
    return EINVAL;
}

int pthread_attr_getstack(const pthread_attr_t *attr, void **stackaddr, size_t *stacksize)
{
    if (attr != NULL) {
        if (stackaddr != NULL) {
            *stackaddr = attr->stackaddr;
        }
        if (stacksize != NULL) {
            *stacksize = attr->stacksize;
        }
        return 0;
    }
    return EINVAL;
}

int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr)
{
    if (attr != NULL) {
        attr->stackaddr = stackaddr;
        return 0;
    }
    return EINVAL;
}

int pthread_attr_getstackaddr(const pthread_attr_t *attr, void **stackaddr)
{
    if (attr != NULL) {
        if (stackaddr != NULL) {
            *stackaddr = attr->stackaddr;
        }
        return 0;
    }
    return EINVAL;
}

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize)
{
    if (attr != NULL) {
        attr->stacksize = stacksize;
        return 0;
    }
    return EINVAL;
}

int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize)
{
    if (attr != NULL) {
        if (stacksize != NULL) {
            *stacksize = attr->stacksize;
        }
        return 0;
    }
    return EINVAL;
}

int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize)
{
    if (attr != NULL) {
        attr->guardsize = guardsize;
        return 0;
    }
    return EINVAL;
}

int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize)
{
    if (attr != NULL) {
        if (guardsize != NULL) {
            *guardsize = attr->guardsize;
        }
        return 0;
    }
    return EINVAL;
}

int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate)
{
    if (attr != NULL) {
        attr->detachstate = detachstate;
        return 0;
    }
    return EINVAL;
}
int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate)
{
    if (attr != NULL) {
        if (detachstate != NULL) {
            *detachstate = attr->detachstate;
        }
        return 0;
    }
    return EINVAL;
}

int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched)
{
    if (attr != NULL) {
        attr->inheritsched = inheritsched;
        return 0;
    }
    return EINVAL;
}

int pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inheritsched)
{
    if (attr != NULL) {
        if (inheritsched != NULL) {
            *inheritsched = attr->inheritsched;
        }
        return 0;
    }
    return EINVAL;
}

int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param)
{
    if ((attr != NULL) && (param != NULL)) {
        attr->schedparam = *param;
        return 0;
    }
    return EINVAL;
}

int pthread_attr_getschedparam(pthread_attr_t *attr, struct sched_param *param)
{
    if (attr != NULL) {
        if (param != NULL) {
            *param = attr->schedparam;
        }
        return 0;
    }
    return EINVAL;
}

int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy)
{
    if (attr != NULL) {
        attr->schedpolicy = policy;
        return 0;
    }
    return EINVAL;
}

int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy)
{
    if (attr != NULL) {
        if (policy != NULL) {
            *policy = attr->schedpolicy;
        }
        return 0;
    }
    return EINVAL;
}

int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope)
{
    if (attr != NULL) {
        attr->scope = contentionscope;
        return 0;
    }
    return EINVAL;
}

int pthread_attr_getscope(const pthread_attr_t *attr, int *contentionscope)
{
    if (attr != NULL) {
        if (contentionscope != NULL) {
            *contentionscope = attr->scope;
        }
        return 0;
    }
    return EINVAL;
}

// pthread_mutex
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mattr)
{
    mutex->attr = (mattr != NULL) ? mattr->attr : 0;
    vsf_eda_mutex_init(&mutex->use_as__vsf_mutex_t);
    return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    VSF_LINUX_ASSERT(NULL == mutex->eda_owner);
    vsf_eda_sync_cancel(&mutex->use_as__vsf_sync_t);
    return 0;
}

int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abstime)
{
    vsf_eda_t *eda = vsf_eda_get_cur();
    VSF_LINUX_ASSERT(eda != NULL);
    if ((mutex->attr & PTHREAD_MUTEX_RECURSIVE) && (eda == mutex->eda_owner)) {
        mutex->recursive_cnt++;
        return 0;
    }

    vsf_timeout_tick_t timeout = __vsf_linux_abstimespec_to_timeout(abstime);
    if (vsf_eda_mutex_enter(&mutex->use_as__vsf_mutex_t, timeout)) {
        vsf_sync_reason_t reason = vsf_eda_sync_get_reason(&mutex->use_as__vsf_sync_t, vsf_thread_wait());
        VSF_UNUSED_PARAM(reason);
        VSF_LINUX_ASSERT(VSF_SYNC_GET == reason);
    }

    mutex->recursive_cnt = 1;
    return 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    return pthread_mutex_timedlock(mutex, NULL);
}

int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
    vsf_eda_t *eda = vsf_eda_get_cur();
    VSF_LINUX_ASSERT(eda != NULL);
    if ((mutex->attr & PTHREAD_MUTEX_RECURSIVE) && (eda == mutex->eda_owner)) {
        mutex->recursive_cnt++;
        return 0;
    }

    if (vsf_eda_mutex_enter(&mutex->use_as__vsf_mutex_t, 0)) {
        return EBUSY;
    }
    mutex->recursive_cnt = 1;
    return 0;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    if (!--mutex->recursive_cnt) {
        vsf_eda_mutex_leave(&mutex->use_as__vsf_mutex_t);
    }
    return 0;
}

int pthread_mutexattr_init(pthread_mutexattr_t *mattr)
{
    if (mattr != NULL) {
        mattr->attr = 0;
        return 0;
    }
    return EINVAL;
}

int pthread_mutexattr_destroy(pthread_mutexattr_t *mattr)
{
    return (mattr != NULL) ? 0 : EINVAL;
}

int pthread_mutexattr_setpshared(pthread_mutexattr_t *mattr, int pshared)
{
    if (mattr != NULL) {
        mattr->attr |= pshared;
        return 0;
    }
    return EINVAL;
}

int pthread_mutexattr_getpshared(pthread_mutexattr_t *mattr, int *pshared)
{
    if (mattr != NULL) {
        if (pshared != NULL) {
            *pshared = mattr->attr & (PTHREAD_PROCESS_PRIVATE | PTHREAD_PROCESS_SHARED);
        }
        return 0;
    }
    return EINVAL;
}

int pthread_mutexattr_settype(pthread_mutexattr_t *mattr, int type)
{
    if (mattr != NULL) {
        mattr->attr |= type;
        return 0;
    }
    return EINVAL;
}

int pthread_mutexattr_gettype(pthread_mutexattr_t *mattr, int *type)
{
    if (mattr != NULL) {
        if (type != NULL) {
            *type = mattr->attr & (PTHREAD_MUTEX_RECURSIVE | PTHREAD_MUTEX_NORMAL);
        }
        return 0;
    }
    return EINVAL;
}




// pthread_cond
int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr)
{
    vsf_eda_trig_init(cond, false, true);
    return 0;
}

int pthread_cond_destroy(pthread_cond_t *cond)
{
    return 0;
}

int pthread_cond_signal(pthread_cond_t *cond)
{
    // TODO: is it secure to reset signal without sched protected?
    vsf_eda_trig_set(cond);
    vsf_eda_trig_reset(cond);
    return 0;
}

int pthread_cond_broadcast(pthread_cond_t *cond)
{
    vsf_eda_trig_set(cond, true);
    vsf_eda_trig_reset(cond);
    return 0;
}

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    return pthread_cond_timedwait(cond, mutex, NULL);
}

int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
        const struct timespec *abstime)
{
    if ((abstime != NULL) && (abstime->tv_nsec >= 1000000000UL)) {
        return EINVAL;
    }

    vsf_timeout_tick_t timeout = __vsf_linux_abstimespec_to_timeout(abstime);
    __vsf_eda_sync_pend(cond, NULL, timeout);
    pthread_mutex_unlock(mutex);

    vsf_sync_reason_t reason;
    int ret = -1;
    do {
        reason = vsf_eda_sync_get_reason(cond, vsf_thread_wait());
    } while (reason == VSF_SYNC_PENDING);
    switch (reason) {
    case VSF_SYNC_TIMEOUT:  ret = ETIMEDOUT;    break;
    case VSF_SYNC_GET:      ret = 0;            break;
    }

    pthread_mutex_lock(mutex);
    return ret;
}
#endif

int pthread_condattr_init(pthread_condattr_t *cattr)
{
    if (cattr != NULL) {
        cattr->attr = 0;
        return 0;
    }
    return EINVAL;
}

int pthread_condattr_destroy(pthread_condattr_t *cattr)
{
    return (cattr != NULL) ? 0 : EINVAL;
}

int pthread_condattr_setpshared(pthread_condattr_t *cattr, int pshared)
{
    if (cattr != NULL) {
        cattr->attr |= pshared;
        return 0;
    }
    return EINVAL;
}

int pthread_condattr_getpshared(pthread_condattr_t *cattr, int *pshared)
{
    if (cattr != NULL) {
        if (pshared != NULL) {
            *pshared = cattr->attr & (PTHREAD_PROCESS_PRIVATE | PTHREAD_PROCESS_SHARED);
        }
        return 0;
    }
    return EINVAL;
}

int pthread_condattr_getclock(const pthread_condattr_t *cattr, clockid_t *clock_id)
{
    if (cattr != NULL) {
        if (clock_id != NULL) {
            *clock_id = cattr->clockid;
        }
        return 0;
    }
    return EINVAL;
}

int pthread_condattr_setclock(pthread_condattr_t *cattr, clockid_t clock_id)
{
    if (cattr != NULL) {
        cattr->clockid = clock_id;
        return 0;
    }
    return EINVAL;
}




#if VSF_LINUX_CFG_TLS_NUM > 0
int pthread_key_create(pthread_key_t *key, void (*destructor)(void*))
{
    VSF_LINUX_ASSERT(key != NULL);
    *key = vsf_linux_tls_alloc(destructor);
    return *key >= 0 ? 0 : -1;
}

int pthread_key_delete(pthread_key_t key)
{
    vsf_linux_tls_free(key);
    return 0;
}

int pthread_setspecific(pthread_key_t key, const void *value)
{
    vsf_linux_localstorage_t *tls = vsf_linux_tls_get(key);
    if (NULL == tls) { return -1; }

    tls->data = (void *)value;
    return 0;
}

void * pthread_getspecific(pthread_key_t key)
{
    vsf_linux_localstorage_t *tls = vsf_linux_tls_get(key);
    if (NULL == tls) { return NULL; }

    return tls->data;
}
#endif

int pthread_sigmask(int how, const sigset_t *set, sigset_t *oldset)
{
    return 0;
}

static void __vsf_linux_pthread_do_cancel(vsf_linux_thread_t *thread)
{
    vsf_linux_pthread_priv_t *priv = vsf_linux_thread_get_priv(thread);

    vsf_linux_pthread_cleanup_handler_t *cleanup_handler;
    vsf_protect_t orig;
    while (true) {
        orig = vsf_protect_sched();
            vsf_slist_remove_from_head(vsf_linux_pthread_cleanup_handler_t, node, &priv->cleanup_handler_list, cleanup_handler);
        vsf_unprotect_sched(orig);
        if (NULL == cleanup_handler) {
            break;
        }

        cleanup_handler->routine(cleanup_handler->arg);
        free(cleanup_handler);
    }

    pthread_exit(NULL);
}

int pthread_cancel(pthread_t tid)
{
    vsf_linux_thread_t *thread = vsf_linux_get_thread(-1, tid);
    vsf_linux_pthread_priv_t *priv = vsf_linux_thread_get_priv(thread);
    priv->is_cancelled = true;
    if (    (PTHREAD_CANCEL_ENABLE == priv->cancel_state)
        &&  (PTHREAD_CANCEL_ASYNCHRONOUS == priv->cancel_type)) {
#if VSF_LINUX_CFG_SUPPORT_SIG == ENABLED
        if (thread->trigger_pending != NULL) {
            vsf_linux_trigger_signal(thread->trigger_pending, SIGABRT);
        } else
#endif
        {
            VSF_LINUX_ASSERT(false);
        }
    }
    return 0;
}

void pthread_testcancel(void)
{
    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    vsf_linux_pthread_priv_t *priv = vsf_linux_thread_get_priv(thread);

    if (priv->is_cancelled) {
        __vsf_linux_pthread_do_cancel(thread);
    }
}

int pthread_setcancelstate(int state, int *oldstate)
{
    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    vsf_linux_pthread_priv_t *priv = vsf_linux_thread_get_priv(thread);

    if (oldstate != NULL) {
        *oldstate = priv->cancel_state;
    }
    priv->cancel_state = state;
    return 0;
}

int pthread_setcanceltype(int type, int *oldtype)
{
    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    vsf_linux_pthread_priv_t *priv = vsf_linux_thread_get_priv(thread);

    if (oldtype != NULL) {
        *oldtype = priv->cancel_type;
    }
    priv->cancel_type = type;
    return 0;
}



// pthread_rwlock
int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr)
{
    vsf_dlist_init(&rwlock->rdlist);
    vsf_dlist_init(&rwlock->wrlist);
    rwlock->rdref = rwlock->wrref = rwlock->rdpend = rwlock->wrpend = 0;
    vsf_eda_sync_init(&rwlock->rdsync, 0, VSF_SYNC_AUTO_RST);
    vsf_eda_sync_init(&rwlock->wrsync, 0, VSF_SYNC_AUTO_RST);
    return 0;
}

int pthread_rwlock_destroy(pthread_rwlock_t *rwlock)
{
    return 0;
}

static vsf_sync_reason_t __pthread_rwlock_pend(vsf_sync_t *sync)
{
    vsf_sync_reason_t reason;
    vsf_evt_t evt;

    while (true) {
        evt = vsf_thread_wait();
        reason = vsf_eda_sync_get_reason(sync, evt);
        if (reason != VSF_SYNC_PENDING) {
            break;
        }
    }

    return reason;
}

static int __pthread_rwlock_rdlock(pthread_rwlock_t *rwlock, vsf_timeout_tick_t timeout)
{
    vsf_eda_t *eda = vsf_eda_get_cur();
    VSF_LINUX_ASSERT(eda != NULL);
    bool need_pend;

    vsf_protect_t orig = vsf_protect_sched();
        if (rwlock->wrref || rwlock->rdpend || rwlock->wrpend) {
            __vsf_eda_sync_pend(&rwlock->rdsync, eda, timeout);
            rwlock->rdpend++;
            need_pend = true;
        } else {
            vsf_dlist_queue_enqueue(
                vsf_eda_t, pending_node,
                &rwlock->rdlist, eda);
            rwlock->rdref++;
            need_pend = false;
        }
    vsf_unprotect_sched(orig);

    if (need_pend) {
        vsf_sync_reason_t reason = __pthread_rwlock_pend(&rwlock->rdsync);
        return (reason == VSF_SYNC_GET) ? 0 : -1;
    }
    return 0;
}

int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock)
{
    return __pthread_rwlock_rdlock(rwlock, -1);
}

int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock)
{
    return __pthread_rwlock_rdlock(rwlock, 0);
}

int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock, const struct timespec *abstime)
{
    if ((abstime != NULL) && (abstime->tv_nsec >= 1000000000UL)) {
        return EINVAL;
    }

    vsf_timeout_tick_t timeout = __vsf_linux_abstimespec_to_timeout(abstime);
    return __pthread_rwlock_rdlock(rwlock, timeout);
}

static int __pthread_rwlock_wrlock(pthread_rwlock_t *rwlock, vsf_timeout_tick_t timeout)
{
    vsf_eda_t *eda = vsf_eda_get_cur();
    VSF_LINUX_ASSERT(eda != NULL);
    bool need_pend;

    vsf_protect_t orig = vsf_protect_sched();
        if (rwlock->wrref || rwlock->rdref || rwlock->wrpend) {
            __vsf_eda_sync_pend(&rwlock->wrsync, eda, timeout);
            rwlock->wrpend++;
            need_pend = true;
        } else {
            vsf_dlist_queue_enqueue(
                vsf_eda_t, pending_node,
                &rwlock->wrlist, eda);
            rwlock->wrref++;
            need_pend = false;
        }
    vsf_unprotect_sched(orig);

    if (need_pend) {
        vsf_sync_reason_t reason = __pthread_rwlock_pend(&rwlock->wrsync);
        return (reason == VSF_SYNC_GET) ? 0 : -1;
    }
    return 0;
}

int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock)
{
    return __pthread_rwlock_wrlock(rwlock, -1);
}

int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock)
{
    return __pthread_rwlock_wrlock(rwlock, 0);
}

int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock, const struct timespec *abstime)
{
    if ((abstime != NULL) && (abstime->tv_nsec >= 1000000000UL)) {
        return EINVAL;
    }

    vsf_timeout_tick_t timeout = __vsf_linux_abstimespec_to_timeout(abstime);
    return __pthread_rwlock_wrlock(rwlock, timeout);
}

int pthread_rwlock_unlock(pthread_rwlock_t *rwlock)
{
    vsf_sync_t *sync = NULL;
    vsf_eda_t *eda = vsf_eda_get_cur();
    VSF_LINUX_ASSERT(eda != NULL);

    vsf_protect_t orig = vsf_protect_sched();
        if (vsf_dlist_is_in(vsf_eda_t, pending_node, &rwlock->rdlist, eda)) {
            vsf_dlist_remove(vsf_eda_t, pending_node, &rwlock->rdlist, eda);
            rwlock->rdref--;
        } else if (vsf_dlist_is_in(vsf_eda_t, pending_node, &rwlock->wrlist, eda)) {
            vsf_dlist_remove(vsf_eda_t, pending_node, &rwlock->wrlist, eda);
            rwlock->wrref--;
        } else {
            VSF_LINUX_ASSERT(false);
            vsf_unprotect_sched(orig);
            return -1;
        }

        if (rwlock->wrpend) {
            if (!rwlock->rdref && !rwlock->wrref) {
                vsf_dlist_queue_enqueue(
                    vsf_eda_t, pending_node,
                    &rwlock->wrlist, eda);
                rwlock->wrref++;
                rwlock->wrpend--;
                sync = &rwlock->wrsync;
            }
        } else if (rwlock->rdpend) {
            if (!rwlock->wrref) {
                vsf_dlist_queue_enqueue(
                    vsf_eda_t, pending_node,
                    &rwlock->rdlist, eda);
                rwlock->rdref++;
                rwlock->rdpend--;
                sync = &rwlock->rdsync;
            }
        }
    vsf_unprotect_sched(orig);

    if (sync != NULL) {
        vsf_eda_sync_increase(sync);
    }
    return 0;
}

int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count)
{
    int rc;

    barrier->in = barrier->out = 0;
    barrier->threshold = count;

    if ((rc = pthread_mutex_init(&barrier->mutex, NULL)) != 0) {
        goto __exit;
    }
    if ((rc = pthread_cond_init(&barrier->cond, NULL)) != 0) {
        goto __free_mutex_and_exit;
    }

    return 0;

__free_mutex_and_exit:
    pthread_mutex_destroy(&barrier->mutex);
__exit:
    return rc;
}

int pthread_barrier_destroy(pthread_barrier_t *barrier)
{
    int rc;

    if (NULL == barrier) {
        return EINVAL;
    }

    if ((rc = pthread_mutex_lock(&barrier->mutex)) != 0) {
        return rc;
    }

    if ((barrier->in > 0) || (barrier->out > 0)) {
        rc = EBUSY;
    }

    pthread_mutex_unlock(&barrier->mutex);
    if (rc) { return rc; }

    pthread_cond_destroy(&barrier->cond);
    pthread_mutex_destroy(&barrier->mutex);
    return 0;
}

int pthread_barrier_wait(pthread_barrier_t *barrier)
{
    int rc;

    if (NULL == barrier) {
        return EINVAL;
    }

    if ((rc = pthread_mutex_lock(&barrier->mutex)) != 0) {
        return rc;
    }

    if (++barrier->in == barrier->threshold) {
        barrier->in = 0;
        barrier->out = barrier->threshold - 1;
        VSF_LINUX_ASSERT(pthread_cond_signal(&barrier->cond) == 0);

        pthread_mutex_unlock(&barrier->mutex);
        return PTHREAD_BARRIER_SERIAL_THREAD;
    }

    do {
        if ((rc = pthread_cond_wait(&barrier->cond, &barrier->mutex)) != 0) {
            break;
        }
    } while (barrier->in != 0);

    barrier->out--;
    pthread_cond_signal(&barrier->cond);
    pthread_mutex_unlock(&barrier->mutex);
    return rc;
}

int pthread_barrierattr_init(pthread_barrierattr_t *battr)
{
    if (battr != NULL) {
        battr->attr = 0;
        return 0;
    }
    return EINVAL;
}

int pthread_barrierattr_destroy(pthread_barrierattr_t *battr)
{
    return 0;
}

int pthread_barrierattr_getpshared(const pthread_barrierattr_t *battr, int *pshared)
{
    if (battr != NULL) {
        if (pshared != NULL) {
            *pshared = battr->attr & (PTHREAD_PROCESS_PRIVATE | PTHREAD_PROCESS_SHARED);
        }
        return 0;
    }
    return EINVAL;
}

int pthread_barrierattr_setpshared(pthread_barrierattr_t *battr, int pshared)
{
    if (battr != NULL) {
        battr->attr |= pshared;
        return 0;
    }
    return EINVAL;
}

int pthread_setname_np(pthread_t tid, const char *name)
{
    vsf_linux_thread_t *thread = vsf_linux_get_thread(-1, tid);
    if (NULL == thread) { return -1; }
    strncpy(thread->name, name, sizeof(thread->name) - 1);
    thread->name[sizeof(thread->name) - 1] = '\0';
    return 0;
}

int pthread_getname_np(pthread_t tid, char *name, size_t size)
{
    vsf_linux_thread_t *thread = vsf_linux_get_thread(-1, tid);
    if (NULL == thread) { return -1; }
    strncpy(name, (const char *)thread->name, size);
    return 0;
}

int pthread_getcpuclockid(pthread_t thread, clockid_t* clockid)
{
    *clockid = CLOCK_REALTIME;
    return 0;
}

#if VSF_LINUX_APPLET_USE_PTHREAD == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_pthread_vplt_t vsf_linux_pthread_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_pthread_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_key_create),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_key_delete),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_setspecific),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_getspecific),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_mutex_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_mutex_destroy),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_mutex_lock),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_mutex_timedlock),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_mutex_trylock),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_mutex_unlock),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_mutexattr_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_mutexattr_destroy),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_mutexattr_setpshared),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_mutexattr_getpshared),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_mutexattr_settype),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_mutexattr_gettype),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_cond_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_cond_destroy),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_cond_signal),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_cond_broadcast),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_cond_wait),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_cond_timedwait),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_condattr_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_condattr_destroy),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_condattr_setpshared),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_condattr_getpshared),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_condattr_getclock),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_condattr_setclock),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_rwlock_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_rwlock_destroy),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_rwlock_rdlock),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_rwlock_tryrdlock),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_rwlock_timedrdlock),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_rwlock_wrlock),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_rwlock_trywrlock),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_rwlock_timedwrlock),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_rwlock_unlock),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_atfork),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_self),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_equal),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_create),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_join),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_detach),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_exit),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_cancel),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_testcancel),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_kill),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_once),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_setcancelstate),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_setcanceltype),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_setschedparam),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_getschedparam),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_cleanup_push),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_cleanup_pop),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_attr_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_attr_destroy),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_attr_setstack),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_attr_getstack),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_attr_setstackaddr),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_attr_getstackaddr),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_attr_setstacksize),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_attr_getstacksize),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_attr_setguardsize),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_attr_getguardsize),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_attr_setdetachstate),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_attr_getdetachstate),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_attr_setinheritsched),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_attr_getinheritsched),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_attr_setschedparam),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_attr_getschedparam),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_attr_setschedpolicy),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_attr_getschedpolicy),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_attr_setscope),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_attr_getscope),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_barrier_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_barrier_destroy),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_barrier_wait),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_barrierattr_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_barrierattr_destroy),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_barrierattr_getpshared),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_barrierattr_setpshared),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_setname_np),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_getname_np),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_getcpuclockid),
};
#endif

#endif      // VSF_USE_LINUX
