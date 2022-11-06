#ifndef __VSF_LINUX_IDR_H__
#define __VSF_LINUX_IDR_H__

#ifdef __cplusplus
extern "C" {
#endif

struct ida {
    int dummy;
};
#define IDA_INIT(__NAME)                        { 0 }
#define DEFINE_IDA(__NAME)                      struct ida __NAME = IDA_INIT(__NAME)

extern int ida_alloc_range(struct ida *, unsigned int min, unsigned int max, gfp_t);
extern void ida_free(struct ida *, unsigned int id);
extern void ida_destroy(struct ida *ida);

static inline int ida_alloc(struct ida *ida, gfp_t gfp)
{
    return ida_alloc_range(ida, 0, ~0, gfp);
}
static inline int ida_alloc_min(struct ida *ida, unsigned int min, gfp_t gfp)
{
    return ida_alloc_range(ida, min, ~0, gfp);
}
static inline int ida_alloc_max(struct ida *ida, unsigned int max, gfp_t gfp)
{
    return ida_alloc_range(ida, 0, max, gfp);
}

static inline bool ida_is_empty(const struct ida *ida)
{
    // TODO:
	return false;
}

#define ida_simple_get(ida, start, end, gfp)    ida_alloc_range(ida, start, (end) - 1, gfp)
#define ida_simple_remove(ida, id)              ida_free(ida, id)

#ifdef __cplusplus
}
#endif

#endif
