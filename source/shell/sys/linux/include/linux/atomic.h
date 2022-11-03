#ifndef __VSF_LINUX_ATOMIC_H__
#define __VSF_LINUX_ATOMIC_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    volatile int counter;
} atomic_t;
#define ATOMIC_INIT(__VALUE)            { (__VALUE) }

static inline int atomic_read(const atomic_t *a)
{
    return a->counter;
}

static inline void atomic_set(atomic_t *a, int v)
{
    a->counter = v;
}

//static inline void atomic_inc(atomic_t *a)
//{
//}

#ifdef __cplusplus
}
#endif

#endif
