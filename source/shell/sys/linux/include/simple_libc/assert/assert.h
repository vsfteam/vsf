#ifndef __SIMPLE_LIBC_ASSERT_H__
#define __SIMPLE_LIBC_ASSERT_H__

// for VSF_ASSERT
#include "utilities/compiler/compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NDEBUG
#   define assert(...)          ((void)0)
#else
#   define assert               VSF_ASSERT
#endif

#ifdef __cplusplus
}
#endif

#endif
