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

#define _Static_assert(__expr, ...)     VSF_STATIC_ASSERT(__expr)
#define static_assert                   _Static_assert

#ifdef __cplusplus
}
#endif
