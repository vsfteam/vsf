#ifndef __SIMPLE_LIBC_STDATOMIC_H__
#define __SIMPLE_LIBC_STDATOMIC_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#else
#   include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef _Atomic(bool)               atomic_bool;
typedef _Atomic(char)               atomic_char;
typedef _Atomic(signed char)        atomic_schar;
typedef _Atomic(unsigned char)      atomic_uchar;
typedef _Atomic(short)              atomic_short;
typedef _Atomic(unsigned short)     atomic_ushort;
typedef _Atomic(int)                atomic_int;
typedef _Atomic(unsigned int)       atomic_uint;
typedef _Atomic(long)               atomic_long;
typedef _Atomic(unsigned long)      atomic_ulong;
typedef _Atomic(long long)          atomic_llong;
typedef _Atomic(unsigned long long) atomic_ullong;
typedef _Atomic(uint_least16_t)     atomic_char16_t;
typedef _Atomic(uint_least32_t)     atomic_char32_t;
typedef _Atomic(wchar_t)            atomic_wchar_t;
typedef _Atomic(int_least8_t)       atomic_int_least8_t;
typedef _Atomic(uint_least8_t)      atomic_uint_least8_t;
typedef _Atomic(int_least16_t)      atomic_int_least16_t;
typedef _Atomic(uint_least16_t)     atomic_uint_least16_t;
typedef _Atomic(int_least32_t)      atomic_int_least32_t;
typedef _Atomic(uint_least32_t)     atomic_uint_least32_t;
typedef _Atomic(int_least64_t)      atomic_int_least64_t;
typedef _Atomic(uint_least64_t)     atomic_uint_least64_t;
typedef _Atomic(int_fast8_t)        atomic_int_fast8_t;
typedef _Atomic(uint_fast8_t)       atomic_uint_fast8_t;
typedef _Atomic(int_fast16_t)       atomic_int_fast16_t;
typedef _Atomic(uint_fast16_t)      atomic_uint_fast16_t;
typedef _Atomic(int_fast32_t)       atomic_int_fast32_t;
typedef _Atomic(uint_fast32_t)      atomic_uint_fast32_t;
typedef _Atomic(int_fast64_t)       atomic_int_fast64_t;
typedef _Atomic(uint_fast64_t)      atomic_uint_fast64_t;
typedef _Atomic(intptr_t)           atomic_intptr_t;
typedef _Atomic(uintptr_t)          atomic_uintptr_t;
typedef _Atomic(size_t)             atomic_size_t;
typedef _Atomic(ptrdiff_t)          atomic_ptrdiff_t;
typedef _Atomic(intmax_t)           atomic_intmax_t;
typedef _Atomic(uintmax_t)          atomic_uintmax_t;

enum memory_order {
    memory_order_relaxed,
    memory_order_consume,
    memory_order_acquire,
    memory_order_release,
    memory_order_acq_rel,
    memory_order_seq_cst,
};

#define ATOMIC_VAR_INIT(__VALUE)            (__VALUE)

#define atomic_init(object, value)                                              \
            atomic_store_explicit(object, value, memory_order_relaxed)

#define atomic_store_explicit(object, desired, order)                           \
            ({                                                                  \
                vsf_protect_t VSF_MACRO_SAFE_NAME(protect) = vsf_protect_int(); \
                    *(object) = desired;                                        \
                vsf_unprotect_int(VSF_MACRO_SAFE_NAME(protect));                \
            })
#define atomic_store(object, desired)                                           \
            atomic_store_explicit(object, desired, memory_order_seq_cst)

#define atomic_load_explicit(object, order)                                     \
            ({                                                                  \
                typeof(*object) VSF_MACRO_SAFE_NAME(tmp);                       \
                vsf_protect_t VSF_MACRO_SAFE_NAME(protect) = vsf_protect_int(); \
                    VSF_MACRO_SAFE_NAME(tmp) = *(object);                       \
                vsf_unprotect_int(VSF_MACRO_SAFE_NAME(protect));                \
                VSF_MACRO_SAFE_NAME(tmp);                                       \
            })
#define atomic_load(object)                                                     \
            atomic_load_explicit(object, memory_order_seq_cst)

#define atomic_exchange_explicit(object, desired, order)                        \
            ({                                                                  \
                typeof(*object) VSF_MACRO_SAFE_NAME(tmp);                       \
                vsf_protect_t VSF_MACRO_SAFE_NAME(protect) = vsf_protect_int(); \
                    VSF_MACRO_SAFE_NAME(tmp) = *(object);                       \
                    *(object) = desired;                                        \
                vsf_unprotect_int(VSF_MACRO_SAFE_NAME(protect));                \
                VSF_MACRO_SAFE_NAME(tmp);                                       \
            })
#define atomic_exchange(object, desired)                                        \
            atomic_exchange_explicit(object, desired, memory_order_seq_cst)

#define atomic_compare_exchange_strong_explicit(object, expected, desired, order_success, order_fail)\
            ({                                                                  \
                vsf_protect_t VSF_MACRO_SAFE_NAME(protect) = vsf_protect_int(); \
                    if (*(object) == *(expected)) {                             \
                        *(object) = desired;                                    \
                    } else {                                                    \
                        *(expected) = *(object);                                \
                    }                                                           \
                vsf_unprotect_int(VSF_MACRO_SAFE_NAME(protect));                \
            })
#define atomic_compare_exchange_strong(object, expected, desired)               \
            atomic_compare_exchange_strong_explicit(object, expected, desired, memory_order_seq_cst, memory_order_seq_cst)

#define atomic_compare_exchange_weak_explicit(object, expected, desired, order_success, order_fail)\
            ({                                                                  \
                vsf_protect_t VSF_MACRO_SAFE_NAME(protect) = vsf_protect_int(); \
                    if (*(object) == *(expected)) {                             \
                        *(object) = desired;                                    \
                    } else {                                                    \
                        *(expected) = *(object);                                \
                    }                                                           \
                vsf_unprotect_int(VSF_MACRO_SAFE_NAME(protect));                \
            })
#define atomic_compare_exchange_weak(object, expected, desired)                 \
            atomic_compare_exchange_weak_explicit(object, expected, desired, memory_order_seq_cst, memory_order_seq_cst)

#define atomic_fetch_add_explicit(object, operand, order)                       \
            ({                                                                  \
                typeof(*object) VSF_MACRO_SAFE_NAME(__tmp);                     \
                vsf_protect_t VSF_MACRO_SAFE_NAME(protect) = vsf_protect_int(); \
                    VSF_MACRO_SAFE_NAME(__tmp) = *(object);                     \
                    *(object) += operand;                                       \
                vsf_unprotect_int(VSF_MACRO_SAFE_NAME(protect));                \
                VSF_MACRO_SAFE_NAME(__tmp);                                     \
            })
#define atomic_fetch_add(object, operand)                                       \
            atomic_fetch_add_explicit(object, operand, memory_order_seq_cst)

#define atomic_fetch_sub_explicit(object, operand, order)                       \
            ({                                                                  \
                typeof(*object) VSF_MACRO_SAFE_NAME(tmp);                       \
                vsf_protect_t VSF_MACRO_SAFE_NAME(protect) = vsf_protect_int(); \
                    *(object) -= operand;                                       \
                    VSF_MACRO_SAFE_NAME(tmp) = *(object);                       \
                vsf_unprotect_int(VSF_MACRO_SAFE_NAME(protect));                \
                VSF_MACRO_SAFE_NAME(tmp);                                       \
            })
#define atomic_fetch_sub(object, operand)                                       \
            atomic_fetch_sub_explicit(object, operand, memory_order_seq_cst)

#define atomic_fetch_or_explicit(object, operand, order)                        \
            ({                                                                  \
                typeof(*object) VSF_MACRO_SAFE_NAME(tmp);                       \
                vsf_protect_t VSF_MACRO_SAFE_NAME(protect) = vsf_protect_int(); \
                    *(object) |= operand;                                       \
                    VSF_MACRO_SAFE_NAME(tmp) = *(object);                       \
                vsf_unprotect_int(VSF_MACRO_SAFE_NAME(protect));                \
                VSF_MACRO_SAFE_NAME(tmp);                                       \
            })
#define atomic_fetch_or(object, operand)                                        \
            atomic_fetch_or_explicit(object, operand, memory_order_seq_cst)

#define atomic_fetch_xor_explicit(object, operand, order)                       \
            ({                                                                  \
                typeof(*object) VSF_MACRO_SAFE_NAME(tmp);                       \
                vsf_protect_t VSF_MACRO_SAFE_NAME(protect) = vsf_protect_int(); \
                    *(object) ^= operand;                                       \
                    VSF_MACRO_SAFE_NAME(tmp) = *(object);                       \
                vsf_unprotect_int(VSF_MACRO_SAFE_NAME(protect));                \
                VSF_MACRO_SAFE_NAME(tmp);                                       \
            })
#define atomic_fetch_xor(object, operand)                                       \
            atomic_fetch_xor_explicit(object, operand, memory_order_seq_cst)

#define atomic_fetch_and_explicit(object, operand, order)                       \
            ({                                                                  \
                typeof(*object) VSF_MACRO_SAFE_NAME(tmp);                       \
                vsf_protect_t VSF_MACRO_SAFE_NAME(protect) = vsf_protect_int(); \
                    *(object) &= operand;                                       \
                    VSF_MACRO_SAFE_NAME(tmp) = *(object);                       \
                vsf_unprotect_int(VSF_MACRO_SAFE_NAME(protect));                \
                VSF_MACRO_SAFE_NAME(tmp);                                       \
            })
#define atomic_fetch_and(object, operand)                                       \
            atomic_fetch_and_explicit(object, operand, memory_order_seq_cst)

#ifdef __cplusplus
}
#endif

#endif
