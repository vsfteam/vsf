#ifndef __VSF_LINUX_WAIT_H__
#define __VSF_LINUX_WAIT_H__

#include "kernel/vsf_kernel.h"
#include <linux/spinlock.h>
#include <linux/list.h>

#ifdef __cplusplus
extern "C" {
#endif

struct wait_queue_entry;
typedef int (*wait_queue_func_t)(struct wait_queue_entry *wqe, unsigned mode, int flags, void *key);

#define WQ_FLAG_EXCLUSIVE   0x01
#define WQ_FLAG_WOKEN       0x02
#define WQ_FLAG_BOOKMARK    0x04
#define WQ_FLAG_CUSTOM      0x08
#define WQ_FLAG_DONE        0x10
#define WQ_FLAG_PRIORITY    0x20

typedef struct wait_queue_entry {
    unsigned int            flags;
    wait_queue_func_t       func;
    struct list_head        entry;

    vsf_trig_t              *__trig;
} wait_queue_entry_t;

typedef struct wait_queue_head {
    spinlock_t              lock;
    struct list_head        head;
} wait_queue_head_t;

static inline void init_waitqueue_head(struct wait_queue_head *wqh)
{
    spin_lock_init(&wqh->lock);
    INIT_LIST_HEAD(&wqh->head);
}

extern void init_wait_entry(struct wait_queue_entry *wqe, int flags);
extern void prepare_to_wait(struct wait_queue_head *wqh, struct wait_queue_entry *wqe, int state);
extern bool prepare_to_wait_exclusive(struct wait_queue_head *wqh, struct wait_queue_entry *wqe, int state);
extern long prepare_to_wait_event(struct wait_queue_head *wqh, struct wait_queue_entry *wqe, int state);
extern void finish_wait(struct wait_queue_head *wqh, struct wait_queue_entry *wqe);
extern long wait_woken(struct wait_queue_entry *wqe, unsigned mode, long timeout);
extern int woken_wake_function(struct wait_queue_entry *wqe, unsigned mode, int sync, void *key);
extern int autoremove_wake_function(struct wait_queue_entry *wqe, unsigned mode, int sync, void *key);

#define __wait_event_timeout(__wqh, __cond, __timeout) ({                       \
    struct wait_queue_entry __wqe;                                              \
    long local_timeout = (__timeout);                                           \
    vsf_trig_t trig;                                                            \
    vsf_eda_trig_init(&trig, false, true);                                      \
    __wqe.__trig = &trig;                                                       \
    init_wait_entry(&__wqe, 0);                                                 \
    while (1) {                                                                 \
        prepare_to_wait_event(&__wqh, &__wqe, 0);                               \
        if (__cond) {                                                           \
            break;                                                              \
        }                                                                       \
        vsf_thread_trig_pend(&trig, local_timeout);                             \
    }                                                                           \
    finish_wait(&__wqh, &__wqe);                                                \
    local_timeout;                                                              \
})

#define wait_event_timeout(__wqh, __cond, __timeout) ({                         \
    long local_timeout = (__timeout);                                           \
    bool local_cond = (__cond);                                                 \
    if (local_cond && !local_timeout) {                                         \
        local_timeout = 1;                                                      \
    }                                                                           \
    if (!(local_cond || !local_timeout)) {                                      \
        local_timeout = __wait_event_timeout(__wqh, __cond, __timeout);         \
    }                                                                           \
    local_timeout;                                                              \
})

#define wait_event(__wqh, __cond)       wait_event_timeout(__wqh, __cond, -1)
extern void wake_up_nr(struct wait_queue_head *wqh, int nr);
#define wake_up(__wqh)                  wake_up_nr((__wqh), 1)
#define wake_up_all(__wqh)              wake_up_nr((__wqh), -1)

#ifdef __cplusplus
}
#endif

#endif
