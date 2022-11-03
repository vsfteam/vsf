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
    struct list_head entry;
    work_func_t func;
};

struct delayed_work {
    struct work_struct work;
//    struct timer_list timer;
};

extern struct workqueue_struct *system_wq;

#ifdef __cplusplus
}
#endif

#endif
