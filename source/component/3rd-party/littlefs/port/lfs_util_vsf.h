#ifndef LFS_UTIL_VSF_H
#define LFS_UTIL_VSF_H

// for trace, heap, etc
#include "service/vsf_service.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef PRId8
#   define PRId8            "d"
#endif
#ifndef PRId16
#   define PRId16           "d"
#endif
#ifndef PRId32
#   define PRId32           "d"
#endif
#ifndef PRIu8
#   define PRIu8            "u"
#endif
#ifndef PRIu16
#   define PRIu16           "u"
#endif
#ifndef PRIu32
#   define PRIu32           "u"
#endif
#ifndef PRIx8
#   define PRIx8            "x"
#endif
#ifndef PRIx16
#   define PRIx16           "x"
#endif
#ifndef PRIx32
#   define PRIx32           "x"
#endif

#ifndef LFS_TRACE
#ifdef LFS_YES_TRACE
#define LFS_TRACE_(fmt, ...) \
    vsf_trace_info("%s:%d:trace: " fmt "%s\n", __FILE__, __LINE__, __VA_ARGS__)
#define LFS_TRACE(...)      LFS_TRACE_(__VA_ARGS__, "")
#else
#define LFS_TRACE(...)
#endif
#endif

#ifndef LFS_DEBUG
#ifndef LFS_NO_DEBUG
#define LFS_DEBUG_(fmt, ...) \
    vsf_trace_debug("%s:%d:debug: " fmt "%s\n", __FILE__, __LINE__, __VA_ARGS__)
#define LFS_DEBUG(...)      LFS_DEBUG_(__VA_ARGS__, "")
#else
#define LFS_DEBUG(...)
#endif
#endif

#ifndef LFS_WARN
#ifndef LFS_NO_WARN
#define LFS_WARN_(fmt, ...) \
    vsf_trace_warning("%s:%d:warn: " fmt "%s\n", __FILE__, __LINE__, __VA_ARGS__)
#define LFS_WARN(...)       LFS_WARN_(__VA_ARGS__, "")
#else
#define LFS_WARN(...)
#endif
#endif

#ifndef LFS_ERROR
#ifndef LFS_NO_ERROR
#define LFS_ERROR_(fmt, ...) \
    vsf_trace_error("%s:%d:error: " fmt "%s\n", __FILE__, __LINE__, __VA_ARGS__)
#define LFS_ERROR(...)      LFS_ERROR_(__VA_ARGS__, "")
#else
#define LFS_ERROR(...)
#endif
#endif

// Runtime assertions
#ifndef LFS_ASSERT
#ifndef LFS_NO_ASSERT
#define LFS_ASSERT(test)    VSF_ASSERT(test)
#else
#define LFS_ASSERT(test)
#endif
#endif

#define lfs_max             vsf_max
#define lfs_min             vsf_min
#define lfs_npw2(__a)       (32 - vsf_clz32(__a))
#define lfs_ctz(__a)        (lfs_npw2(((__a) & -(__a)) + 1) - 1)
#define lfs_fromle32        le32_to_cpu
#define lfs_tole32          cpu_to_le32
#define lfs_frombe32        be32_to_cpu
#define lfs_tobe32          cpu_to_be32
#define lfs_malloc          vsf_heap_malloc
#define lfs_free            vsf_heap_free

static inline uint32_t lfs_aligndown(uint32_t a, uint32_t alignment) {
    return a - (a % alignment);
}

static inline uint32_t lfs_alignup(uint32_t a, uint32_t alignment) {
    return lfs_aligndown(a + alignment-1, alignment);
}

static inline uint32_t lfs_popc(uint32_t a) {
    a = a - ((a >> 1) & 0x55555555);
    a = (a & 0x33333333) + ((a >> 2) & 0x33333333);
    return (((a + (a >> 4)) & 0xf0f0f0f) * 0x1010101) >> 24;
}

static inline int lfs_scmp(uint32_t a, uint32_t b) {
    return (int)(unsigned)(a - b);
}

// Calculate CRC-32 with polynomial = 0x04c11db7
uint32_t lfs_crc(uint32_t crc, const void *buffer, size_t size);

#ifdef __cplusplus
}
#endif

#endif      // LFS_UTIL_VSF_H
