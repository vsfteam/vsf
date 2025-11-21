#include <linux/types.h>
#include <linux/jiffies.h>

static inline ktime_t ktime_set(const s64 secs, const unsigned long nsecs)
{
    return secs * NSEC_PER_SEC + (s64)nsecs;
}

#define ktime_sub(lhs, rhs)         ((lhs) - (rhs))
#define ktime_add(lhs, rhs)         ((lhs) + (rhs))
#define ktime_add_unsafe(lhs, rhs)  ((u64) (lhs) + (rhs))
#define ktime_add_ns(kt, nsval)     ((kt) + (nsval))
#define ktime_sub_ns(kt, nsval)     ((kt) - (nsval))

static inline int ktime_compare(const ktime_t cmp1, const ktime_t cmp2)
{
    if (cmp1 < cmp2) {
        return -1;
    } else if (cmp1 > cmp2) {
        return 1;
    } else {
        return 0;
    }
}

static inline bool ktime_after(const ktime_t cmp1, const ktime_t cmp2)
{
    return ktime_compare(cmp1, cmp2) > 0;
}

static inline bool ktime_before(const ktime_t cmp1, const ktime_t cmp2)
{
    return ktime_compare(cmp1, cmp2) < 0;
}

static inline s64 ktime_to_ns(const ktime_t kt)
{
    return vsf_systimer_tick_to_us(kt) * 1000;
}

static inline s64 ktime_to_us(const ktime_t kt)
{
    return vsf_systimer_tick_to_us(kt);
}

static inline s64 ktime_to_ms(const ktime_t kt)
{
    return vsf_systimer_tick_to_ms(kt);
}

static inline s64 ktime_us_delta(const ktime_t later, const ktime_t earlier)
{
    return ktime_to_us(ktime_sub(later, earlier));
}

static inline s64 ktime_ms_delta(const ktime_t later, const ktime_t earlier)
{
    return ktime_to_ms(ktime_sub(later, earlier));
}

static inline ktime_t ktime_add_us(const ktime_t kt, const u64 usec)
{
    return ktime_add_ns(kt, usec * NSEC_PER_USEC);
}

static inline ktime_t ktime_add_ms(const ktime_t kt, const u64 msec)
{
    return ktime_add_ns(kt, msec * NSEC_PER_MSEC);
}

static inline ktime_t ktime_sub_us(const ktime_t kt, const u64 usec)
{
    return ktime_sub_ns(kt, usec * NSEC_PER_USEC);
}

static inline ktime_t ktime_sub_ms(const ktime_t kt, const u64 msec)
{
    return ktime_sub_ns(kt, msec * NSEC_PER_MSEC);
}

static inline ktime_t us_to_ktime(u64 us)
{
    return vsf_systimer_us_to_tick(us);
}

static inline ktime_t ms_to_ktime(u64 ms)
{
    return vsf_systimer_ms_to_tick(ms);
}

static inline ktime_t ns_to_ktime(u64 ns)
{
    u64 us = ns / 1000;
    if (!us) us++;
    return us_to_ktime(us);
}
