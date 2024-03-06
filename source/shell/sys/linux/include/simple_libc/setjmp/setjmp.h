#ifndef __SIMPLE_LIBC_SETJMP_H__
#define __SIMPLE_LIBC_SETJMP_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

// for VSF_ARCH_SETJMP and VSF_ARCH_LONGJMP
#include "hal/arch/vsf_arch.h"
// for ALIGN and stdint.h
#include "utilities/compiler/compiler.h"

#ifndef __SIMPLE_LIBC_SETJMP_VPLT_ONLY__
#   if      defined(__CPU_X64__)

typedef struct {
    uint64_t part[2];
} VSF_CAL_ALIGN(16) setjmp_float128;
typedef setjmp_float128 jmp_buf[16];

#   elif    defined(__CPU_ARM__)
typedef unsigned long long int jmp_buf[16];
#   else
#   error not supported, do not add to path, use setjmp from libc instead
#   endif
#endif

#if VSF_LINUX_APPLET_USE_LIBC_SETJMP == ENABLED
typedef struct vsf_linux_libc_setjmp_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(setjmp);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(longjmp);
} vsf_linux_libc_setjmp_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_libc_setjmp_vplt_t vsf_linux_libc_setjmp_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_LIBC_SETJMP_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_LIBC_SETJMP == ENABLED

#ifndef VSF_LINUX_APPLET_LIBC_SETJMP_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_LIBC_SETJMP_VPLT                                \
            ((vsf_linux_libc_setjmp_vplt_t *)(VSF_LINUX_APPLET_VPLT->libc_setjmp_vplt))
#   else
#       define VSF_LINUX_APPLET_LIBC_SETJMP_VPLT                                \
            ((vsf_linux_libc_setjmp_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_LIBC_SETJMP_ENTRY(__NAME)                              \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_LIBC_SETJMP_VPLT, __NAME)
#define VSF_LINUX_APPLET_LIBC_SETJMP_IMP(...)                                   \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_LIBC_SETJMP_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_LIBC_SETJMP_IMP(setjmp, int, jmp_buf env) {
    return VSF_LINUX_APPLET_LIBC_SETJMP_ENTRY(setjmp)(env);
}
VSF_LINUX_APPLET_LIBC_SETJMP_IMP(longjmp, void, jmp_buf env, int val) {
    VSF_LINUX_APPLET_LIBC_SETJMP_ENTRY(longjmp)(env, val);
}

#else

#if defined(VSF_ARCH_SETJMP) && !defined(__VSF_APPLET__)
#   undef setjmp
#   define setjmp               VSF_ARCH_SETJMP
#endif
int setjmp(jmp_buf env);

#if defined(VSF_ARCH_LONGJMP) && !defined(__VSF_APPLET__)
#   undef longjmp
#   define longjmp              VSF_ARCH_LONGJMP
#endif
void longjmp(jmp_buf env, int val);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_LIBC_SETJMP

#undef __SIMPLE_LIBC_SETJMP_VPLT_ONLY__

#endif
