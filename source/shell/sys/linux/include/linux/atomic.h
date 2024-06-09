#ifndef __VSF_LINUX_ATOMIC_H__
#define __VSF_LINUX_ATOMIC_H__

// for uintalu_t
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

#if     defined(__VSF64__)
#   define __VSF_LINUX_ATOM_BITLEN__    64
#elif   defined(__VSF16__)
#   define __VSF_LINUX_ATOM_BITLEN__    16
#elif   defined(__VSF8__)
#   define __VSF_LINUX_ATOM_BITLEN__    8
#elif   defined(__VSF32__)
#   define __VSF_LINUX_ATOM_BITLEN__    32
#else
#   define __VSF_LINUX_ATOM_BITLEN__    32
#endif

typedef struct {
    volatile intalu_t counter;
} atomic_t;
#define ATOMIC_INIT(__VALUE)            { (__VALUE) }

#define atomic_set(__v, __i)            ((__v)->counter = (__i))
#define atomic_read(__v)                ((__v)->counter)

#define atomic_add(__i, __v)            vsf_atom_add(__VSF_LINUX_ATOM_BITLEN__, &(__v)->counter, (__i))
#define atomic_sub(__i, __v)            atomic_add(- (__i), (__v))
#define atomic_inc(__v)                 atomic_add(1, __v)
#define atomic_dec(__v)                 atomic_sub(1, __v)

#define atomic_add_return(__i, __v)     ({                                          \
    int VSF_MACRO_SAFE_NAME(value) = (__i);                                         \
    vsf_atom_add(__VSF_LINUX_ATOM_BITLEN__, &(__v)->counter, VSF_MACRO_SAFE_NAME(value)) + VSF_MACRO_SAFE_NAME(value);\
})
#define atomic_sub_return(__i, __v)     atomic_add_return(- (__i), (__v))
#define atomic_inc_return(__v)          atomic_add_return(1, (__v))
#define atomic_dec_return(__v)          atomic_sub_return(1, (__v))

#define atomic_inc_and_test(__v)        (!atomic_inc_return(__v))
#define atomic_dec_and_test(__v)        (!atomic_dec_return(__v))
#define atomic_sub_and_test(__i, __v)   (!atomic_sub_return((__i), (__v)))

#define atomic_add_negative(__i, __v)   (atomic_sub_return((__i), (__v)) < 0)

#define atomic_xchg(__v, __new)         vsf_atom_xchg(__VSF_LINUX_ATOM_BITLEN__, &(__v)->counter, (__new))

#ifdef __cplusplus
}
#endif

#endif
