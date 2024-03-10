#ifndef __SIMPLE_LIBC_STDDEF_H__
#define __SIMPLE_LIBC_STDDEF_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#else
#   include <sys/types.h>
#endif

// for uintptr_t
#include <stdint.h>

#define VSF_LINUX_LIBC_WRAPPER(__api)   VSF_SHELL_WRAPPER(vsf_linux_libc, __api)

// define wchar_t and wint_t before include vsf_utilities.h
// because vsf_utilities will include other c headers which will require wchar_t
#if     !defined(__cplusplus)
// wchar_t MSUT match the real wchar_t in the compiler, if not, fix here
//  eg: for GCC, wchar_t is int
#   if      defined(__WCHAR_TYPE__)
typedef __WCHAR_TYPE__              wchar_t;
#   elif    defined(__WIN__)
typedef unsigned short              wchar_t;
#   elif __IS_COMPILER_GCC__ || __IS_COMPILER_LLVM__
typedef int                         wchar_t;
#   else
typedef unsigned short              wchar_t;
#   endif
#endif
#ifdef __WINT_TYPE__
typedef __WINT_TYPE__               wint_t;
#else
typedef unsigned short              wint_t;
#endif

// include compiler detect only, do not include compiler.h
#include "utilities/compiler/compiler_detect.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#   ifdef __cplusplus
#       define NULL                 (0)
#   else
#       define NULL                 ((void *)0)
#   endif
#endif

#ifndef TRUE
#   define TRUE                     1
#endif
#ifndef FALSE
#   define FALSE                    0
#endif
// DO not define BOOL, it will conflict with system headers in some platform

#ifndef offsetof
// use offsetof from compiler if available for constexpr feature in cpp
#   if __IS_COMPILER_GCC__ || __IS_COMPILER_LLVM__
#       define offsetof(__type, __member)   __builtin_offsetof(__type, __member)
#   else
#       define offsetof(__type, __member)   (uintptr_t)(&(((__type *)0)->__member))
#   endif
#endif

// define max_align_t, can not depend on uintalu_t because of circular depoendency
#if     __IS_COMPILER_GCC__
typedef struct {
  long long __clang_max_align_nonce1
      __attribute__((__aligned__(__alignof__(long long))));
  long double __clang_max_align_nonce2
      __attribute__((__aligned__(__alignof__(long double))));
} max_align_t;
#elif   __IS_COMPILER_LLVM__
#   if      defined(__WIN__)
typedef double max_align_t;
#   elif    defined(__MACOS__)
typedef long double max_align_t;
#   else
typedef struct {
  long long __clang_max_align_nonce1
      __attribute__((__aligned__(__alignof__(long long))));
  long double __clang_max_align_nonce2
      __attribute__((__aligned__(__alignof__(long double))));
} max_align_t;
#   endif
#else
#endif

#if defined(__PTRDIFF_TYPE__)
typedef __PTRDIFF_TYPE__            ptrdiff_t;
#elif defined(__WIN__)
#   if defined(__CPU_X86__)
typedef unsigned int                size_t;
typedef int                         ptrdiff_t;
typedef int                         intptr_t;
typedef unsigned int                uintptr_t;
#   elif defined(__CPU_X64__)
typedef unsigned __int64            size_t;
typedef __int64                     ptrdiff_t;
typedef __int64                     intptr_t;
typedef unsigned __int64            uintptr_t;
#   endif
#else
typedef long int                    ptrdiff_t;
#endif

#ifdef __cplusplus
}

#   ifdef __WIN__
namespace std {
    typedef decltype(__nullptr)     nullptr_t;
}

using ::std::nullptr_t;
#   endif
#endif

#endif
