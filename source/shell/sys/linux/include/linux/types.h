#ifndef __VSF_LINUX_TYPES_H__
#define __VSF_LINUX_TYPES_H__

// for endian APIs
#include "hal/arch/vsf_arch.h"
#include <sys/types.h>
#include <linux/err.h>
#include <linux/minmax.h>
#include <linux/spinlock.h>
#include <linux/gfp.h>
#include <linux/atomic.h>
#include <linux/printk.h>
#include <linux/bits.h>
#include <linux/limits.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __packed
#   define __packed             VSF_CAL_PACKED
#endif
#ifndef __aligned
#   define __aligned(n)         VSF_CAL_ALIGN(n)
#endif
#ifndef __user
#   define __user
#endif
#ifndef __bitwise
#   define __bitwise
#endif

#define EXPORT_SYMBOL_GPL(__SYMBOL)

#define MSEC_PER_SEC            1000L
#define USEC_PER_MSEC           1000L
#define NSEC_PER_USEC           1000L
#define NSEC_PER_MSEC           1000000L
#define USEC_PER_SEC            1000000L
#define NSEC_PER_SEC            1000000000L

#define __struct_group(TAG, NAME, ATTRS, MEMBERS...)                            \
    union {                                                                     \
        struct { MEMBERS } ATTRS;                                               \
        struct TAG { MEMBERS } ATTRS NAME;                                      \
    }
#define struct_group(NAME, MEMBERS...)                                          \
    __struct_group(/* no tag */, NAME, /* no attrs */, MEMBERS)

typedef int8_t __s8;
typedef uint8_t __u8;
typedef int16_t __s16;
typedef uint16_t __u16;
typedef int32_t __s32;
typedef uint32_t __u32;
typedef int64_t __s64;
typedef uint64_t __u64;

typedef __u16 __le16;
typedef __u16 __be16;
typedef __u32 __le32;
typedef __u32 __be32;
typedef __u64 __le64;
typedef __u64 __be64;

typedef __u16 __sum16;
typedef __u32 __wsum;

typedef __u8 u8;
typedef __s8 s8;
typedef __u16 u16;
typedef __s16 s16;
typedef __u32 u32;
typedef __s32 s32;
typedef __u64 u64;
typedef __s64 s64;

typedef off_t __kernel_off_t;
typedef pid_t __kernel_pid_t;

typedef unsigned int slab_flags_t;
typedef unsigned int fmode_t;

#ifndef VSF_ARCH_PHYS_ADDR_T
typedef uintalu_t phys_addr_t;
#else
typedef VSF_ARCH_PHYS_ADDR_T phys_addr_t;
#endif
#ifndef VSF_ARCH_DMA_ADDR_T
typedef uintalu_t dma_addr_t;
#else
typedef VSF_ARCH_DMA_ADDR_T dma_addr_t;
#endif

struct hlist_head {
    struct hlist_node *first;
};

struct hlist_node {
    struct hlist_node *next, **pprev;
};

#define ARRAY_SIZE(__ARR)       dimof(__ARR)

// some compiler does not support typeof
#define roundup(__x, __y) ({                                                    \
    (((__x) + ((__y) - 1)) / (__y)) * (__y);                                    \
})

#define rounddown(__x, __y) ({                                                  \
    (__x) - ((__x) % (__y));                                                    \
})

#define round_up(__x, __y)  ((((__x) - 1) | ((__y) - 1)) + 1)

#ifdef __cplusplus
}
#endif

#endif
