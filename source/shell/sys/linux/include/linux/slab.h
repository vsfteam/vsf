#ifndef __VSF_LINUX_SLAB_H__
#define __VSF_LINUX_SLAB_H__

#include "shell/sys/linux/vsf_linux_cfg.h"
#include "service/heap/vsf_heap.h"
#include <linux/gfp.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void * kmalloc(size_t size, gfp_t flags)
{
#if VSF_LINUX_SIMPLE_LIBC_CFG_SKIP_MM != ENABLED
    void * buff = vsf_heap_malloc(size);
#else
    void * buff = malloc(size);
#endif
    if (buff != NULL) {
        if (flags & __GFP_ZERO) {
            memset(buff, 0, size);
        }
    }
    return buff;
}

static inline void * krealloc(const void *buff, size_t new_size, gfp_t flags)
{
#if VSF_LINUX_SIMPLE_LIBC_CFG_SKIP_MM != ENABLED
    return vsf_heap_realloc((void *)buff, new_size);
#else
    return realloc((void *)buff, new_size);
#endif
}

static inline void * kzalloc(size_t size, gfp_t flags)
{
    return kmalloc(size, flags | __GFP_ZERO);
}

static inline void kfree(const void *buff)
{
#if VSF_LINUX_SIMPLE_LIBC_CFG_SKIP_MM != ENABLED
    vsf_heap_free((void *)buff);
#else
    free((void *)buff);
#endif
}

static inline size_t ksize(const void *buff)
{
#if VSF_LINUX_SIMPLE_LIBC_CFG_SKIP_MM != ENABLED
    return (size_t)vsf_heap_size((uint8_t *)buff);
#else
    return malloc_size(buff);
#endif
}

#ifdef __cplusplus
}
#endif

#endif
