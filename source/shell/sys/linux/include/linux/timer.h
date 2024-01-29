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
        vsf_container_of(callback_timer, typeof(*var), timer_fieldname)

extern void add_timer(struct timer_list *timer);
extern int del_timer(struct timer_list *timer);
extern int del_timer_sync(struct timer_list *timer);
extern int mod_timer(struct timer_list *timer, unsigned long expires);
extern int timer_pending(const struct timer_list *timer);
extern void timer_setup(struct timer_list *timer, void (*func)(struct timer_list *), unsigned int flags);

#ifdef __cplusplus
}
#endif

#endif
