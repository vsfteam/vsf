#ifndef _COMPILER_H_
#define _COMPILER_H_

// for cmsis-core APIs
#include "chip.h"

#define __STATIC            static

#undef __INLINE
#define __INLINE            static inline

#ifndef __STATIC_INLINE
#   define __STATIC_INLINE  static inline
#endif

#define __MODULE__          __BASE_FILE__

#define __ALIGN4            ALIGN(4)

#define __PRIVATE_HOST_N(m,n)

#define PRIVATE_HOST_ARRAY_DECLARE(module, type, count, name)                   \
    type backup_ ## name[count] __PRIVATE_HOST_N(module, name)

#define __SHAREDRAM         SECTION("SHAREDRAM")

#endif
