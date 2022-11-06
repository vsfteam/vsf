#ifndef __VSF_LINUX_WORKQUEUE_H__
#define __VSF_LINUX_WORKQUEUE_H__

#include <linux/list.h>

#ifdef __cplusplus
extern "C" {
#endif

struct workqueue_struct;
struct work_struct;
typedef void (*work_func_t)(struct work_struct *work);

struct work_struct {
    vsf_dlist_node_t entry;
    work_func_t func;
};

struct delayed_work {
    struct work_struct work;
    vsf_systimer_tick_t start_tick;
};

extern struct workqueue_struct *system_wq;

extern struct workqueue_struct * alloc_workqueue(const char *fmt, unsigned int flags, int max_active, ...);
extern void destroy_workqueue(struct workqueue_struct *wq);
extern bool queue_delayed_work(struct workqueue_struct *wq, struct delayed_work *dwork, unsigned long delay);
extern bool queue_work(struct workqueue_struct *wq, struct work_struct *work);
extern void flush_workqueue(struct workqueue_struct *wq);

#define INIT_WORK(__work, __func)                                               \
        do {                                                                    \
            (__work)->func = (__func);                                          \
            vsf_dlist_init_node(struct work_struct, entry, (__work));           \
        } while (0)
#define alloc_ordered_workqueue(__fmt, __flags, ...)                            \
        alloc_workqueue(__fmt, __flags, 1, ##__VA_ARGS__)
#define create_singlethread_workqueue(__name)                                   \
        alloc_ordered_workqueue("%s", 0, (__name))
#define create_workqueue(__name)                                                \
        alloc_workqueue("%s", 0, 1, (__name))

static inline bool schedule_work(struct work_struct *work)
{
    return queue_work(system_wq, work);
}

static inline bool schedule_delayed_work(struct delayed_work *dwork, unsigned long delay)
{
    return queue_delayed_work(system_wq, dwork, delay);
}

#ifdef __cplusplus
}
#endif

#endif
