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

#include "component/tcpip/vsf_tcpip_cfg.h"

#if VSF_USE_TCPIP == ENABLED && VSF_USE_LWIP == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#include "kernel/vsf_kernel.h"

#include "./cc.h"
#include "./sys_arch.h"
#include "lwip/err.h"
#include "lwip/sys.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE != ENABLED
#   error "VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE is required"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*
struct vsf_rtos_thread_t {
    implement(vsf_thread_t)

    void *arg;
    lwip_thread_fn fn;
};
typedef struct vsf_rtos_thread_t vsf_rtos_thread_t;
*/

dcl_vsf_thread_ex(vsf_rtos_thread_t)
def_vsf_thread_ex(vsf_rtos_thread_t,
    def_params(
        void *arg;
        lwip_thread_fn lwip_thread;
    ))


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if SYS_LIGHTWEIGHT_PROT
sys_prot_t sys_arch_protect(void)
{
    return vsf_protect_sched();
}

void sys_arch_unprotect(sys_prot_t pval)
{
    vsf_unprotect_sched(pval);
}
#endif

#if NO_SYS
u32_t sys_now(void)
{
    return vsf_systimer_get_ms();
}
#else

// thread
static void __vsf_rtos_thread_on_terminate(vsf_eda_t *eda)
{
    vsf_heap_free(eda);
}

implement_vsf_thread_ex(vsf_rtos_thread_t)
{
    this.lwip_thread(this.arg);
}
/*
static void vsf_rtos_thread_entry(vsf_thread_t *thread)
{
    sys_thread_t sys_thread = (sys_thread_t)thread;
    sys_thread->fn(sys_thread->arg);
}
*/
sys_thread_t sys_thread_new(const char *name,
                            lwip_thread_fn fn,
                            void *arg,
                            int stacksize,
                            int prio)
{
    sys_thread_t thread;
    uint_fast32_t thread_size = sizeof(*thread);
    uint64_t *stack;

    thread_size += (1 << VSF_KERNEL_CFG_THREAD_STACK_ALIGN_BIT) - 1;
    thread_size &= ~((1 << VSF_KERNEL_CFG_THREAD_STACK_ALIGN_BIT) - 1);
    stacksize   += (1 << VSF_KERNEL_CFG_THREAD_STACK_ALIGN_BIT) - 1;
    stacksize   &= ~((1 << VSF_KERNEL_CFG_THREAD_STACK_ALIGN_BIT) - 1);

    thread = vsf_heap_malloc_aligned(thread_size + stacksize,
                        1 << VSF_KERNEL_CFG_THREAD_STACK_ALIGN_BIT);
    if (NULL == thread) {
        return NULL;
    }

    thread->on_terminate = __vsf_rtos_thread_on_terminate;

    thread->arg = arg;
    thread->lwip_thread = fn;

    stack = (uint64_t *)((uintptr_t)thread + thread_size);
    init_vsf_thread_ex( vsf_rtos_thread_t,
                        thread,
                        prio,
                        stack,
                        stacksize);
    /*
    thread->stack = (uint64_t *)((uintptr_t)thread + thread_size);
    thread->stack_size = stacksize;

    vsf_thread_start(&(thread->use_as__vsf_thread_t), prio);
    */
    return thread;
}

// sem
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
    vsf_eda_sem_init(sem, count);
    return ERR_OK;
}

void sys_sem_free(sys_sem_t *sem)
{

}

int sys_sem_valid(sys_sem_t *sem)
{
    return sem->max_union.max_value != 0 ? 1 : 0;
}

void sys_sem_set_invalid(sys_sem_t *sem)
{
    sem->max_union.bits.max = 0;
}

void sys_sem_signal(sys_sem_t *sem)
{
    vsf_eda_sem_post(sem);
}

u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
    vsf_timer_tick_t pre = vsf_systimer_get_tick();
    vsf_sync_reason_t reason = vsf_thread_sem_pend(sem, vsf_systimer_ms_to_tick(timeout));
    if (VSF_SYNC_GET == reason) {
        pre = vsf_systimer_get_tick() - pre;
        return vsf_systimer_tick_to_ms(pre);
    } else {
        return SYS_ARCH_TIMEOUT;
    }
}

// mutex
err_t sys_mutex_new(sys_mutex_t *mutex)
{
    vsf_eda_mutex_init(mutex);
    return ERR_OK;
}

void sys_mutex_free(sys_mutex_t *mutex)
{

}

void sys_mutex_lock(sys_mutex_t *mutex)
{
    vsf_thread_mutex_enter(mutex, -1);
}

void sys_mutex_unlock(sys_mutex_t *mutex)
{
    vsf_thread_mutex_leave(mutex);
}

int sys_mutex_valid(sys_mutex_t *mutex)
{
    return mutex->cur_union.bits.has_owner ? 1 : 0;
}

void sys_mutex_set_invalid(sys_mutex_t *mutex)
{
    mutex->cur_union.bits.has_owner = 0;
}

// mbox
static void __sys_mbox_next(sys_mbox_t *mbox, uint16_t *pos)
{
    if (++(*pos) >= mbox->max_union.max_value) {
        *pos = 0;
    }
}

static bool __sys_mbox_queue_enqueue(vsf_eda_queue_t *pthis, void *node)
{
    sys_mbox_t *mbox = (sys_mbox_t *)pthis;
    mbox->queue[mbox->tail] = node;
    __sys_mbox_next(mbox, &mbox->tail);
    return true;
}

static bool __sys_mbox_queue_dequeue(vsf_eda_queue_t *pthis, void **node)
{
    sys_mbox_t *mbox = (sys_mbox_t *)pthis;
    *node = mbox->queue[mbox->head];
    __sys_mbox_next(mbox, &mbox->head);
    return true;
}

static const vsf_eda_queue_op_t __sys_mbox_queue_op = {
    .enqueue = __sys_mbox_queue_enqueue,
    .dequeue = __sys_mbox_queue_dequeue,
};

err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
    ASSERT(size <= VSF_SYNC_MAX);

    mbox->queue = vsf_heap_malloc(sizeof(void *) * size);
    if (NULL == mbox->queue) {
        return ERR_MEM;
    }
    mbox->head = mbox->tail = 0;

    mbox->use_as__vsf_eda_queue_t.op = __sys_mbox_queue_op;
    vsf_eda_queue_init(&mbox->use_as__vsf_eda_queue_t, size);
    return ERR_OK;
}

void sys_mbox_free(sys_mbox_t *mbox)
{
//    vsf_eda_queue_fini(&mbox->use_as__vsf_eda_queue_t);
    if (mbox->queue != NULL) {
        vsf_heap_free(mbox->queue);
    }
}

int sys_mbox_valid(sys_mbox_t *mbox)
{
    return mbox->queue != NULL ? 1 : 0;
}

void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
    mbox->queue = NULL;
}

err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
    if (VSF_ERR_NONE == vsf_eda_queue_send(&mbox->use_as__vsf_eda_queue_t, msg, 0)) {
        return ERR_OK;
    }
    return ERR_MEM;
}

void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
    if (VSF_ERR_NONE != vsf_eda_queue_send(&mbox->use_as__vsf_eda_queue_t, msg, -1)) {
        vsf_sync_reason_t reason;

        do {
            reason = vsf_eda_queue_send_get_reason(&mbox->use_as__vsf_eda_queue_t, vsf_thread_wait(), msg);
        } while (reason == VSF_SYNC_PENDING);
    }
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
    if (VSF_ERR_NONE == vsf_eda_queue_recv(&mbox->use_as__vsf_eda_queue_t, msg, 0)) {
        return 0;
    }
    return SYS_MBOX_EMPTY;
}

u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
    vsf_timer_tick_t start = vsf_systimer_get_tick();
    u32_t duration;
    vsf_sync_reason_t reason;

    if (VSF_ERR_NONE != vsf_eda_queue_recv(&mbox->use_as__vsf_eda_queue_t, msg, vsf_systimer_ms_to_tick(timeout))) {
        do {
            reason = vsf_eda_queue_recv_get_reason(&mbox->use_as__vsf_eda_queue_t, vsf_thread_wait(), msg);
        } while (reason == VSF_SYNC_PENDING);
    } else {
        reason = VSF_SYNC_GET;
    }

    if (reason != VSF_SYNC_GET) {
        duration = SYS_ARCH_TIMEOUT;
    } else {
        duration = vsf_systimer_tick_to_ms(vsf_systimer_get_tick() - start);
    }
    return duration;
}

void sys_init(void)
{
}
#endif      // NO_SYS

#endif      // VSF_USE_TCPIP && VSF_USE_LWIP
