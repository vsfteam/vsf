// for VSF_ASSERT and compiler info
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

#if !defined(__cplusplus)
// _Static_assert is keyword, not MACRO
//#   define _Static_assert(__expr, ...)  VSF_STATIC_ASSERT(__expr)
#   define static_assert                _Static_assert
#endif

#ifdef __cplusplus
}
#endif
