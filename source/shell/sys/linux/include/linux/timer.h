#ifndef __VSF_LINUX_TIMER_H__
#define __VSF_LINUX_TIMER_H__

#include <linux/types.h>
#include <linux/jiffies.h>

#ifdef __cplusplus
extern "C" {
#endif

struct timer_list {
    struct hlist_node               entry;
    unsigned long                   expires;
    void (*function)(struct timer_list *);
    u32                             flags;
};

#define from_timer(var, callback_timer, timer_fieldname)                        \
        container_of(callback_timer, typeof(*var), timer_fieldname)

#ifdef __cplusplus
}
#endif

#endif
