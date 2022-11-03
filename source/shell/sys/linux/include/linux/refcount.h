#ifndef __VSF_LINUX_REFCOUNT_H__
#define __VSF_LINUX_REFCOUNT_H__

#include <linux/types.h>
#include <linux/atomic.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct refcount_struct {
    atomic_t refs;
} refcount_t;

#define REFCOUNT_INIT(__VALUE)          { .refs = ATOMIC_INIT(__VALUE) }

static inline unsigned int refcount_read(refcount_t *r)
{
    return atomic_read(&r->refs);
}

static inline void refcount_set(refcount_t *r, int value)
{
    atomic_set(&r->refs, value);
}

static inline void refcount_inc(refcount_t *r)
{
    atomic_inc(&r->refs);
}

static inline bool refcount_dec_and_test(refcount_t *r)
{
    return atomic_dec_and_test(&r->refs);
}

#ifdef __cplusplus
}
#endif

#endif
