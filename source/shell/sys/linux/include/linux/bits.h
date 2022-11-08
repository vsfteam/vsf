#ifndef __VSF_LINUX_BITS_H__
#define __VSF_LINUX_BITS_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BIT
#   define BIT(__NR)            (1UL << (__NR))
#endif
#ifndef BIT_ULL
#   define BIT_ULL(__NR)        (1ULL << (__NR))
#endif

#define BITS_PER_LONG           (sizeof(long) << 3)
#define GENMASK(__H, __L)       (((~0UL) << (__L)) & (~0UL >> (BITS_PER_LONG - 1 - (__H))))

#ifdef __cplusplus
}
#endif

#endif
