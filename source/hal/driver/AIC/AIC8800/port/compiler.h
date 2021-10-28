#ifndef _COMPILER_H_
#define _COMPILER_H_

// for cmsis-core APIs
#include "chip.h"
#include "utilities/vsf_utilities.h"

#define __STATIC                static

#undef __INLINE
#define __INLINE                static inline

#ifndef __STATIC_INLINE
#   define __STATIC_INLINE      static inline
#endif

#define __MODULE__              __BASE_FILE__

#define __ALIGN4                ALIGN(4)

#define __PRIVATE_HOST_N(m,n)

#define PRIVATE_HOST_ARRAY_DECLARE(module, type, count, name)                   \
    type backup_ ## name[count] __PRIVATE_HOST_N(module, name)

#define __SHAREDRAM             SECTION("SHAREDRAM")

#define MCAT(a,b)               a##b
#define VAR_WITH_VERx(name,v)   VSF_MCONNECT3(name, _U0, v)
#define VAR_WITH_VER(name)      VAR_WITH_VERx(name, CFG_ROM_VER)

#endif
