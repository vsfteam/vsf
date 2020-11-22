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
#else
#   include <unistd.h>
#   include <time.h>
#   include <pthread.h>
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

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
    vsf_eda_mutex_init(mutex);
    return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    vsf_eda_sync_cancel(&mutex->use_as__vsf_sync_t);
    return 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    if (vsf_eda_mutex_enter(mutex)) {
        vsf_thread_wfe(VSF_EVT_SYNC);
    }
    return 0;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    vsf_eda_mutex_leave(mutex);
    return 0;
}




int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr)
{
    vsf_slist_init(cond);
    return 0;
}

int pthread_cond_destroy(pthread_cond_t *cond)
{
    return 0;
}

int pthread_cond_signal(pthread_cond_t *cond)
{
    vsf_eda_t *eda;
    vsf_protect_t orig = vsf_protect_sched();
        vsf_slist_remove_from_head(vsf_eda_t, pending_node, cond, eda);
    vsf_unprotect_sched(orig);
    if (eda != NULL) {
        vsf_eda_post_evt(eda, VSF_EVT_USER);
    }
    return 0;
}

int pthread_cond_broadcast(pthread_cond_t *cond)
{
    vsf_eda_t *eda;
    vsf_protect_t orig;
    while (1) {
        orig = vsf_protect_sched();
            vsf_slist_remove_from_head(vsf_eda_t, pending_node, cond, eda);
        vsf_unprotect_sched(orig);
        if (NULL == eda) {
            break;
        }
        vsf_eda_post_evt(eda, VSF_EVT_USER);
    }
    return 0;
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();

    vsf_protect_t orig = vsf_protect_sched();
        vsf_slist_add_to_head(vsf_eda_t, pending_node, cond, &thread->use_as__vsf_eda_t);
    vsf_unprotect_sched(orig);

    pthread_mutex_unlock(mutex);
    vsf_thread_wfe(VSF_EVT_USER);
    return 0;
}

int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
        const struct timespec *abstime)
{
    return pthread_cond_wait(cond, mutex);
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

#endif      // VSF_USE_LINUX
