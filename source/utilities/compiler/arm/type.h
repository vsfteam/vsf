/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

#ifndef __APP_TYPE_H_INCLUDED__
#define __APP_TYPE_H_INCLUDED__

/*============================ INCLUDES ======================================*/

#if (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L) && !defined(__cplusplus)

typedef unsigned char       uint8_t;
typedef signed char         int8_t;
typedef unsigned int        uint_fast8_t;
typedef signed int          int_fast8_t;

typedef unsigned short      uint16_t;
typedef signed short        int16_t;
typedef unsigned int        uint_fast16_t;
typedef signed int          int_fast16_t;

typedef unsigned int        uint32_t;
typedef signed int          int32_t;
typedef unsigned int        uint_fast32_t;
typedef signed int          int_fast32_t;

typedef unsigned long long  uint64_t;
typedef signed long long    int64_t;
typedef unsigned long long  uint_fast64_t;
typedef signed long long    int_fast64_t;

#ifndef UINT64_MAX
#   define UINT64_MAX       ((uint64_t)(-1))
#endif

typedef uint32_t            uintptr_t;
typedef int32_t             intptr_t;
typedef uint64_t            uintmax_t;
typedef int64_t             intmax_t;

#if !defined(bool)
typedef enum {
    false = 0,
    true = !false,
} bool;
#endif

#else
#include <stdint.h>
#include <stdbool.h>
#endif

#if !__IS_COMPILER_GCC__ && !__IS_COMPILER_ARM_COMPILER_5__
#   include <uchar.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define __optimal_bit_sz    (sizeof(uintalu_t) * 8)
#define __optimal_bit_msk   (__optimal_bit_sz - 1)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef uint_fast8_t        uintalu_t;
typedef int_fast8_t         intalu_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif // __APP_TYPE_H_INCLUDED__

/*============================ Multiple-Entry ================================*/

#include "../__common/__type.h"

#if __IS_COMPILER_IAR__

// iar has no such constants in math.h
#   define M_E              2.71828182845904523536   // e
#   define M_LOG2E          1.44269504088896340736   // log2(e)
#   define M_LOG10E         0.434294481903251827651  // log10(e)
#   define M_LN2            0.693147180559945309417  // ln(2)
#   define M_LN10           2.30258509299404568402   // ln(10)
#   define M_PI             3.14159265358979323846   // pi
#   define M_PI_2           1.57079632679489661923   // pi/2
#   define M_PI_4           0.785398163397448309616  // pi/4
#   define M_1_PI           0.318309886183790671538  // 1/pi
#   define M_2_PI           0.636619772367581343076  // 2/pi
#   define M_2_SQRTPI       1.12837916709551257390   // 2/sqrt(pi)
#   define M_SQRT2          1.41421356237309504880   // sqrt(2)
#   define M_SQRT1_2        0.707106781186547524401  // 1/sqrt(2)

// iar has no strlcpy and strcasestr
#ifdef __cplusplus
extern "C" {
#endif
extern size_t strlcpy(char *dst, const char *src, size_t dsize);
extern char * strcasestr(const char *str, const char *substr);
#ifdef __cplusplus
}
#endif

#if !(VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED && VSF_LINUX_USE_SIMPLE_STDIO == ENABLED)
#   include <stdio.h>
#   ifdef __cplusplus
extern "C" {
#   endif
typedef long                off_t;
typedef int64_t             off64_t;
extern int fseeko(FILE *f, off_t offset, int whence);
extern off_t ftello(FILE *f);
extern int fseeko64(FILE *f, off64_t offset, int whence);
extern off64_t ftello64(FILE *f);
#   ifdef __cplusplus
}
#   endif
#endif      // !(VSF_USE_LINUX && VSF_LINUX_USE_SIMPLE_LIBC && VSF_LINUX_USE_SIMPLE_STDIO)

#if !(VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED && VSF_LINUX_USE_SIMPLE_TIME == ENABLED)
#   include <time.h>
// iar has no clockid_t and useconds_t
#   ifdef __cplusplus
extern "C" {
#   endif
#   ifndef __IAR_TYPE_CLOCKID_T__
#   define __IAR_TYPE_CLOCKID_T__
typedef enum {
    CLOCK_MONOTONIC,
    CLOCK_REALTIME,
} clockid_t;
typedef unsigned long       useconds_t;
extern int clock_gettime(clockid_t clk_id, struct timespec *tp);
#   ifdef __cplusplus
}
#   endif
#   endif
#endif      // !(VSF_USE_LINUX && VSF_LINUX_USE_SIMPLE_LIBC && VSF_LINUX_USE_SIMPLE_TIME)

#elif __IS_COMPILER_GCC__

#if !(VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED && VSF_LINUX_USE_SIMPLE_STDIO == ENABLED)
// arm-none-eabi-gcc has no 64-bit stdio APIs, if simple_stdio in simple_libc is not used,
//  implement 64-bit stdio APIs here, currently simply redirect to 32-bit version
#   define off64_t          off_t
#   define ftello64         ftell
#   define fseeko64         fseek
#endif      // !(VSF_USE_LINUX && VSF_LINUX_USE_SIMPLE_LIBC && VSF_LINUX_USE_SIMPLE_STDIO)

// gcc has no strcasestr
#ifdef __cplusplus
extern "C" {
#endif
extern char * strcasestr(const char *str, const char *substr);
#ifdef __cplusplus
}
#endif

#endif      // __IS_COMPILER_XXX__
