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

#ifndef __VSF_TYPE_H_INCLUDED__
#define __VSF_TYPE_H_INCLUDED__



/*============================ INCLUDES ======================================*/

#if (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L) && !defined(__cplusplus)
typedef unsigned char       uint8_t;
typedef signed char         int8_t;
typedef unsigned char       uint_fast8_t;
typedef signed char         int_fast8_t;

typedef unsigned short      uint16_t;
typedef signed short        int16_t;
typedef unsigned short      uint_fast16_t;
typedef signed short        int_fast16_t;

typedef unsigned long int   uint32_t;
typedef signed long int     int32_t;
typedef unsigned long int   uint_fast32_t;
typedef signed long int     int_fast32_t;

typedef uint32_t            uintptr_t;
typedef int32_t             intptr_t;
typedef uint32_t            uintmax_t;
typedef int32_t             intmax_t;

typedef enum {
    false = 0,
    true = !false,
} bool;
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#if !__IS_COMPILER_GCC__
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

#if     defined(__CPU_X86__) || defined(__CPU_WEBASSEMBLY__)
typedef uint32_t            uintalu_t;
typedef int32_t             intalu_t;
#elif   defined(__CPU_X64__)
typedef uint64_t            uintalu_t;
typedef int64_t             intalu_t;
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif // __APP_TYPE_H_INCLUDED__

/*============================ Multiple-Entry ================================*/
#include "../__common/__type.h"

/*============================ Library Patch  ================================*/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __WIN__
#   if !(VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED && VSF_LINUX_USE_SIMPLE_STDIO == ENABLED)
// __WIN__ has no 64-bit stdio APIs, if simple_stdio in simple_libc is not used,
//  implement 64-but stdio APIs here

#       ifndef __VSF_COMPILER_STDIO_H__
#       define __VSF_COMPILER_STDIO_H__
typedef long                off_t;
typedef long long           off64_t;
#       endif

#       define ftello64     ftell
#       define fseeko64     fseek
#       define ftello       ftell
#       define fseeko       fseek
#   endif

// win has no such constants in math.h
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

// __WIN__ uses stricmp instead of strcasecmp in strings.h
#   define strcasecmp       stricmp
#   define strncasecmp      strnicmp
// __WIN__ uses _alloca instead of alloca in alloca.h
#   define alloca _alloca
extern void * _alloca(size_t);
extern int stricmp(const char *s1, const char *s2);
extern int strnicmp(const char *s1, const char *s2, size_t n);

extern char * strsep(char **stringp, const char *delim);
extern size_t strlcpy(char *dst, const char *src, size_t dsize);
extern char * strcasestr(const char *str, const char *substr);
extern char * strtok_r(char *str, const char *delim, char **saveptr);

extern void srandom(unsigned int seed);
extern long int random(void);

#include <time.h>
#   if !(VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED && VSF_LINUX_USE_SIMPLE_TIME == ENABLED)
#       ifndef __VSF_COMPILER_TIME_H__
#       define __VSF_COMPILER_TIME_H__
typedef enum {
    CLOCK_MONOTONIC,
    CLOCK_REALTIME,
} clockid_t;
extern int clock_gettime(clockid_t clk_id, struct timespec *tp);
extern int nanosleep(const struct timespec *requested_time, struct timespec *remaining);
#       endif
#   endif
#endif

#ifdef __cplusplus
}
#endif
