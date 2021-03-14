#ifndef __SIMPLE_LIBC_STDDEF_H__
#define __SIMPLE_LIBC_STDDEF_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#else
#   include <sys/types.h>
#endif
#include "utilities/vsf_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#   ifdef __cplusplus
#       define NULL     (0)
#   else
#       define NULL     ((void *)0)
#   endif
#endif

#ifndef TRUE
#   define TRUE         1
#endif
#ifndef FALSE
#   define FALSE        0
#endif

#define offsetof        offset_of

#ifndef __WIN__

#   if defined(__IS_COMPILER_ARM_COMPILER_6__) && defined(__PTRDIFF_TYPE__)
typedef __PTRDIFF_TYPE__ ptrdiff_t;
#   else
typedef long int         ptrdiff_t;
#   endif

#endif

#ifdef __cplusplus
}
#endif

#if     !defined(__cplusplus)
// TODO: it seems that IAR does not support wchar_t even if it's defined here
#   if !__IS_COMPILER_IAR__
typedef unsigned short  wchar_t;
#   endif
typedef unsigned short  wint_t;
#elif   defined(__WIN__)
namespace std {
    typedef decltype(__nullptr) nullptr_t;
}

using ::std::nullptr_t;
#endif

#endif
