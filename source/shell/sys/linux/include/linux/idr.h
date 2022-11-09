#ifndef __VSF_LINUX_IDR_H__
#define __VSF_LINUX_IDR_H__

// for vsf_protect
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef VSF_LINUX_CFG_IDA_MAX
#   define VSF_LINUX_CFG_IDA_MAX                1024
#endif

vsf_declare_bitmap(ida_bitmap, VSF_LINUX_CFG_IDA_MAX)

struct ida {
    vsf_bitmap(ida_bitmap)                      bitmap;
    unsigned int                                id_cnt;
};
#define IDA_INIT(__NAME)                        { 0 }
#define DEFINE_IDA(__NAME)                      struct ida __NAME = IDA_INIT(__NAME)

extern int ida_alloc_range(struct ida *ida, unsigned int __min_to_avoid_conflict, unsigned int __max_to_avoid_confilict, gfp_t);
static inline void ida_free(struct ida *ida, unsigned int id)
{
    vsf_protect_t orig = vsf_protect_int();
    vsf_bitmap_clear(&ida->bitmap, id);
    vsf_unprotect_int(orig);
}

static inline void ida_init(struct ida *ida)
{
    memset(ida, 0, sizeof(*ida));
}
static inline void ida_destroy(struct ida *ida)
{
    // nothing allocated, nothing to destroy
}
static inline int ida_alloc(struct ida *ida, gfp_t gfp)
{
    return ida_alloc_range(ida, 0, ~0, gfp);
}
static inline int ida_alloc_min(struct ida *ida, unsigned int __min_to_avoid_conflict, gfp_t gfp)
{
    return ida_alloc_range(ida, __min_to_avoid_conflict, ~0, gfp);
}
static inline int ida_alloc_max(struct ida *ida, unsigned int __max_to_avoid_confilict, gfp_t gfp)
{
    return ida_alloc_range(ida, 0, __max_to_avoid_confilict, gfp);
}

static inline bool ida_is_empty(const struct ida *ida)
{
    vsf_protect_t orig = vsf_protect_int();
    bool is_empty = ida->id_cnt == 0;
    vsf_unprotect_int(orig);
    return is_empty;
}

#define ida_simple_get(ida, start, end, gfp)    ida_alloc_range(ida, start, (end) - 1, gfp)
#define ida_simple_remove(ida, id)              ida_free(ida, id)

#ifdef __cplusplus
}
#endif

#endif
