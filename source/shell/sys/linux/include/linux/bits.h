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

#define GENMASK(__H, __L)       (1ULL << ((__H) + 1)) - (1ULL << ((__L) + 1))

#ifdef __cplusplus
}
#endif

#endif
