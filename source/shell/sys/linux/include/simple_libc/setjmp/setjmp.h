#ifndef __SIMPLE_LIBC_SETJMP_H__
#define __SIMPLE_LIBC_SETJMP_H__

// for VSF_ARCH_SETJMP and VSF_ARCH_LONGJMP
#include "hal/arch/vsf_arch.h"
// for ALIGN and stdint.h
#include "utilities/compiler/compiler.h"

#if     defined(__CPU_X64__)

typedef struct {
    uint64_t part[2];
} ALIGN(16) setjmp_float128;
typedef setjmp_float128 jmp_buf[16];

#else
#   error not supported, do not add to path, use setjmp from libc instead
#endif

#if defined(VSF_ARCH_SETJMP) && !defined(__VSF_APPLET__)
#   define setjmp               VSF_ARCH_SETJMP
#else
int setjmp(jmp_buf env);
#endif

#if defined(VSF_ARCH_LONGJMP) && !defined(__VSF_APPLET__)
#   define longjmp              VSF_ARCH_LONGJMP
#else
void longjmp(jmp_buf env, int val);
#endif

#endif
