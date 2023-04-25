// for VSF_ASSERT
#include "utilities/compiler/compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

#undef assert

#ifdef NDEBUG
#   define assert(...)          ((void)0)
#else
#   define assert               VSF_ASSERT
#endif

#ifdef __cplusplus
}
#endif
