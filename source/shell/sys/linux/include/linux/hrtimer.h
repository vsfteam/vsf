#include <linux/ktime.h>

#include <kernel/vsf_kernel.h>

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED && VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED

enum hrtimer_restart {
    HRTIMER_NORESTART,
    HRTIMER_RESTART,
};

enum hrtimer_mode {
    HRTIMER_MODE_REL            = 0x00,
    HRTIMER_MODE_ABS            = 0x01,
};

struct hrtimer {
    vsf_callback_timer_t        callback_timer;
    enum hrtimer_restart        (*function)(struct hrtimer *);
    struct hrtimer_clock_base   *base;
    ktime_t                     due;
    u8                          state;
};

void __hrtimer_on_timer(vsf_callback_timer_t* timer);

static inline void hrtimer_init(struct hrtimer *timer, clockid_t clock_id, enum hrtimer_mode mode)
{
    memset(timer, 0, sizeof(*timer));
    vsf_callback_timer_init(&timer->callback_timer);
    timer->callback_timer.on_timer = __hrtimer_on_timer;
}

static inline int hrtimer_cancel(struct hrtimer *timer)
{
    vsf_callback_timer_remove(&timer->callback_timer);
    return 0;
}

static inline int hrtimer_try_to_cancel(struct hrtimer *timer)
{
    return hrtimer_cancel(timer);
}

static inline ktime_t hrtimer_cb_get_time(const struct hrtimer *timer)
{
    return (ktime_t)vsf_systimer_get();
}

static inline u64 hrtimer_forward(struct hrtimer *timer, ktime_t now, ktime_t interval)
{
    timer->due = now + interval;
    return 0;
}

static inline u64 hrtimer_forward_now(struct hrtimer *timer, ktime_t interval)
{
    return hrtimer_forward(timer, hrtimer_cb_get_time(timer), interval);
}

static void hrtimer_start_range_ns(struct hrtimer *timer, ktime_t tim, u64 range_ns, const enum hrtimer_mode mode)
{
    vsf_systimer_tick_t due;
    if (mode == HRTIMER_MODE_REL) {
        due = vsf_systimer_get() + (vsf_systimer_tick_t)tim;
    } else {
        due = (vsf_systimer_tick_t)tim;
    }
    vsf_callback_timer_add_due(&timer->callback_timer, due);
}

static inline void hrtimer_start(struct hrtimer *timer, ktime_t tim, const enum hrtimer_mode mode)
{
    hrtimer_start_range_ns(timer, tim, 0, mode);
}

#endif
