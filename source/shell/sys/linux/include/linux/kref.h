#ifndef __VSF_LINUX_KREF_H__
#define __VSF_LINUX_KREF_H__

#include <linux/refcount.h>

#ifdef __cplusplus
extern "C" {
#endif

struct kref {
    refcount_t refcount;
};

#define KREF_INIT(__N)              { .refcount = REFCOUNT_INIT(__N) }

static inline void kref_init(struct kref *kref)
{
    refcount_set(&kref->refcount, 1);
}

static inline unsigned int kref_read(const struct kref *kref)
{
    return refcount_read(&kref->refcount);
}

static inline void kref_get(struct kref *kref)
{
    refcount_inc(&kref->refcount);
}

static inline int kref_put(struct kref *kref, void (*release)(struct kref *kref))
{
    if (refcount_dec_and_test(&kref->refcount)) {
        release(kref);
        return 1;
    }
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif
