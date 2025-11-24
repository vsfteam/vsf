// __VSF_LINUX_SEMAPHORE_H__ is used in <semaphore.h>
#ifndef __VSF_LINUX_KERNEL_SEMAPHORE_H__
#define __VSF_LINUX_KERNEL_SEMAPHORE_H__

#include <kernel/vsf_kernel.h>

// for vsf_linux_trigger_t
#include <shell/sys/linux/vsf_linux.h>

struct semaphore {
    vsf_linux_trigger_t trigger;
};

static inline void sema_init(struct semaphore *sem, int val)
{
    VSF_LINUX_ASSERT(val <= 0xFFFF);
    vsf_eda_sync_init(&sem->trigger.use_as__vsf_trig_t, (uint_fast16_t)val, (uint_fast16_t)val);
#if VSF_LINUX_CFG_SUPPORT_SIG == ENABLED
    vsf_dlist_init_node(vsf_linux_trigger_t, node, &sem->trigger);
    sem->trigger.pending_process = NULL;
#endif
}

static inline int down_interruptible(struct semaphore *sem)
{
    extern int vsf_linux_trigger_pend(vsf_linux_trigger_t *trig, vsf_timeout_tick_t timeout);
    return vsf_linux_trigger_pend(&sem->trigger, -1);
}

static inline void down(struct semaphore *sem)
{
    __vsf_thread_wait_for_sync(&sem->trigger.use_as__vsf_trig_t, -1);
}

static inline int down_trylock(struct semaphore *sem)
{
    // VSF_ERR_NONE = 0, VSF_ERR_NOT_READY = 1, match return value of down_trylock
    return vsf_eda_sync_decrease(&sem->trigger.use_as__vsf_trig_t, 0);
}

static inline void up(struct semaphore *sem)
{
    vsf_eda_sync_increase(&sem->trigger.use_as__vsf_trig_t);
}

#endif
