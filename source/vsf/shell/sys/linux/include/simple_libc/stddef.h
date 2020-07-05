#ifndef __SIMPLE_LIBC_STDDEF_H__
#define __SIMPLE_LIBC_STDDEF_H__

//! \note please do not modify this to avoid circular-including
#include "utilities/compiler/compiler.h"

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

enum {
    FALSE               = 0,
    TRUE                = 1,
};

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
