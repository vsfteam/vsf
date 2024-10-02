// for VSF_ASSERT
#include "utilities/compiler/compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

#undef assert

#ifdef NDEBUG
#   define assert(...)                  ((void)0)
#else
#   define assert                       VSF_ASSERT
#endif

// Win SDK xkercheck.h will not be happy if static_assert is a MACRO in c++
#if !defined(__WIN__) || !defined(__cplusplus)
#   define _Static_assert(__expr, ...)  VSF_STATIC_ASSERT(__expr)
#   define static_assert                _Static_assert
#endif

#ifdef __cplusplus
}
#endif
