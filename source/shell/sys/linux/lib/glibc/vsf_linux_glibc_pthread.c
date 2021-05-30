/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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
#else
#   include <unistd.h>
#   include <time.h>
#   include <pthread.h>
#   include <signal.h>
#   include <errno.h>
#endif

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_linux_pthread_priv_t {
    void *param;
    void * (*entry)(void *param);
} vsf_linux_pthread_priv_t;

/*============================ PROTOTYPES ====================================*/

static void __vsf_linux_pthread_on_run(vsf_thread_cb_t *cb);

SECTION(".text.vsf.kernel.vsf_sync")
void __vsf_eda_sync_pend(vsf_sync_t *sync, vsf_eda_t *eda, vsf_timeout_tick_t timeout);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const vsf_linux_thread_op_t __vsf_linux_pthread_op = {
    .priv_size          = sizeof(vsf_linux_pthread_priv_t),
    .on_run             = __vsf_linux_pthread_on_run,
    .on_terminate       = vsf_linux_thread_on_terminate,
};

/*============================ IMPLEMENTATION ================================*/

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#   pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#endif

static void __vsf_linux_pthread_on_run(vsf_thread_cb_t *cb)
{
    vsf_linux_thread_t *thread = container_of(cb, vsf_linux_thread_t, use_as__vsf_thread_cb_t);
    vsf_linux_pthread_priv_t *priv = vsf_linux_thread_get_priv(thread);
    thread->retval = (int)priv->entry(priv->param);
}

int pthread_join(pthread_t tid, void **retval)
{
    vsf_protect_t orig = vsf_protect_sched();
    vsf_linux_thread_t *thread = vsf_linux_get_thread(tid);
    if (NULL == thread) {
        vsf_unprotect_sched(orig);
        return -1;
    }

    thread->thread_pending = vsf_linux_get_cur_thread();
    vsf_unprotect_sched(orig);
    vsf_thread_wfe(VSF_EVT_USER);

    if (retval != NULL) {
        *retval = (void *)thread->retval;
    }
    return 0;
}

void pthread_exit(void *retval)
{
    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    if (thread != NULL) {
        thread->retval = (int)retval;
        vsf_thread_exit();
    }
    VSF_LINUX_ASSERT(false);
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

pthread_t pthread_self(void)
{
    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    return (NULL == thread) ? 0 : thread->tid;
}

int pthread_create(pthread_t *tidp, const pthread_attr_t *attr, void * (*start_rtn)(void *), void *arg)
{
    vsf_linux_pthread_priv_t *priv;
    vsf_linux_thread_t *thread = vsf_linux_create_thread(NULL, 0, &__vsf_linux_pthread_op);
    if (!thread) { return -1; }

    priv = vsf_linux_thread_get_priv(thread);
    priv->entry = start_rtn;
    priv->param = arg;

    if (tidp != NULL) {
        *tidp = thread->tid;
    }
    vsf_linux_start_thread(thread);
    return 0;
}

int pthread_cancel(pthread_t thread)
{
    VSF_LINUX_ASSERT(false);
    // TODO:
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
    if (!once_control->is_inited) {
        pthread_mutex_lock(&once_control->mutex);
        if (once_control->is_inited) {
            init_routine();
            once_control->is_inited = true;
        }
        pthread_mutex_unlock(&(once_control->mutex));
    }
    return 0;
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

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    if (mutex->attr & PTHREAD_MUTEX_RECURSIVE) {
        if (vsf_eda_get_cur() == mutex->eda_owner) {
            mutex->recursive_cnt++;
            return 0;
        }
    }

    if (vsf_eda_mutex_enter(&mutex->use_as__vsf_mutex_t)) {
        vsf_thread_wfe(VSF_EVT_SYNC);
    }

    mutex->recursive_cnt = 1;
    return 0;
}

int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
    if (vsf_eda_mutex_try_enter(&mutex->use_as__vsf_mutex_t, 0)) {
        return EBUSY;
    }
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

int pthread_mutexattr_settype(pthread_mutexattr_t *mattr , int type)
{
    if (mattr != NULL) {
        mattr->attr |= type;
        return 0;
    }
    return EINVAL;
}

int pthread_mutexattr_gettype(pthread_mutexattr_t *mattr , int *type)
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
    vsf_eda_trig_set(cond);
    return 0;
}

int pthread_cond_broadcast(pthread_cond_t *cond)
{
    vsf_eda_trig_set(cond, true);
    vsf_eda_trig_reset(cond);
    return 0;
}

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

    vsf_timeout_tick_t timeout = -1;
    if (abstime != NULL) {
        struct timespec now, due = *abstime;
        clock_gettime(CLOCK_MONOTONIC, &now);
        vsf_systimer_tick_t now_tick =  1000 * vsf_systimer_ms_to_tick(now.tv_sec)
                                    +   vsf_systimer_us_to_tick(now.tv_nsec / 1000);
        vsf_systimer_tick_t due_tick =  1000 * vsf_systimer_ms_to_tick(due.tv_sec)
                                    +   vsf_systimer_us_to_tick(due.tv_nsec / 1000);
        if (now_tick >= due_tick) {
            return ETIMEDOUT;
        }
    }
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




int pthread_key_create(pthread_key_t *key, void (*destructor)(void*))
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

int pthread_setspecific(pthread_key_t key, const void *value)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

void *pthread_getspecific(pthread_key_t key)
{
    VSF_LINUX_ASSERT(false);
    return NULL;
}

int pthread_sigmask(int how, const sigset_t *set, sigset_t *oldset)
{
    return 0;
}

#endif      // VSF_USE_LINUX
