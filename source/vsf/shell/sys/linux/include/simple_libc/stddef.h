#ifndef __SIMPLE_LIBC_STDDEF_H__
#define __SIMPLE_LIBC_STDDEF_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#else
#   include <sys/types.h>
#endif

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
typedef long int        ptrdiff_t;
#endif

typedef unsigned short  wchar_t;
typedef unsigned short  wint_t;

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
namespace std {
    typedef decltype(__nullptr) nullptr_t;
}

using ::std::nullptr_t;
#endif

#endif
