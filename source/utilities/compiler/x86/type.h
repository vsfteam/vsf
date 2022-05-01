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
#   ifdef __cplusplus
extern "C" {
#   endif
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
#   ifdef __cplusplus
}
#   endif
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

#ifndef __VSF_HEADER_ONLY_SHOW_COMPILER_INFO__

/*============================ Multiple-Entry ================================*/

#   include "../__common/__type.h"

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

// __WIN__ uses stricmp instead of strcasecmp in strings.h
#   define strcasecmp       stricmp
#   define strncasecmp      strnicmp

extern char * strsep(char **stringp, const char *delim);
extern size_t strlcpy(char *dst, const char *src, size_t dsize);

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

#endif      // __VSF_HEADER_ONLY_SHOW_COMPILER_INFO__
