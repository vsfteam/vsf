#ifndef __VSF_LINUX_BITOPS_H__
#define __VSF_LINUX_BITOPS_H__

#ifdef __cplusplus
extern "C" {
#endif

extern void set_bit(int nr, volatile unsigned long *addr);
extern void clear_bit(int nr, volatile unsigned long *addr);

// if test_bit is defined as a macro, do not implement test_bit here
#ifndef test_bit
static inline int test_bit(int nr, const unsigned long *addr)
{
    addr = &addr[nr / (sizeof(unsigned long) * 8)];
    return !!(*addr & (1ULL << (nr & (sizeof(unsigned long) * 8 - 1))));
}
#endif

#ifdef __cplusplus
}
#endif

#endif
