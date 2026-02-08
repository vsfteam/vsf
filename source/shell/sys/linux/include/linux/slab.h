#ifndef __VSF_LINUX_SLAB_H__
#define __VSF_LINUX_SLAB_H__

#include "service/heap/vsf_heap.h"
#include <linux/gfp.h>
// <linux/slab.h> should include <linux/minmax.h>
#include <linux/minmax.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void * kmalloc(size_t size, gfp_t flags)
{
    void * buff = vsf_heap_malloc(size);
    if (buff != NULL) {
        if (flags & __GFP_ZERO) {
            memset(buff, 0, size);
        }
    }
    return buff;
}

static inline void * krealloc(const void *buff, size_t new_size, gfp_t flags)
{
    return vsf_heap_realloc((void *)buff, new_size);
}

static inline void * kzalloc(size_t size, gfp_t flags)
{
    return kmalloc(size, flags | __GFP_ZERO);
}

static inline void kfree(const void *buff)
{
    vsf_heap_free((void *)buff);
}

static inline size_t ksize(const void *buff)
{
    return (size_t)vsf_heap_size((uint8_t *)buff);
}

#ifdef __cplusplus
}
#endif

#endif
