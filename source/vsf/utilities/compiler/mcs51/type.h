/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#ifdef __cplusplus
extern "C" {
#endif

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

typedef unsigned short      uint_least16_t;
typedef signed short        int_least16_t

typedef unsigned long int   uint_least32_t;
typedef signed long int     int_least32_t

typedef uint32_t            uintptr_t;
typedef int32_t             intptr_t;
typedef uint32_t            uintmax_t;
typedef int32_t             intmax_t;

typedef enum {
    false = 0,
    true = !false,
} bool;

#define INT8_MIN         (-127i8 - 1)
#define INT16_MIN        (-32767i16 - 1)
#define INT32_MIN        (-2147483647i32 - 1)
#define INT8_MAX         127i8
#define INT16_MAX        32767i16
#define INT32_MAX        2147483647i32
#define UINT8_MAX        0xffui8
#define UINT16_MAX       0xffffui16
#define UINT32_MAX       0xffffffffui32

#define INT_LEAST8_MIN   INT8_MIN
#define INT_LEAST16_MIN  INT16_MIN
#define INT_LEAST32_MIN  INT32_MIN
#define INT_LEAST8_MAX   INT8_MAX
#define INT_LEAST16_MAX  INT16_MAX
#define INT_LEAST32_MAX  INT32_MAX
#define UINT_LEAST8_MAX  UINT8_MAX
#define UINT_LEAST16_MAX UINT16_MAX
#define UINT_LEAST32_MAX UINT32_MAX

#define INT_FAST8_MIN    INT8_MIN
#define INT_FAST16_MIN   INT32_MIN
#define INT_FAST32_MIN   INT32_MIN
#define INT_FAST8_MAX    INT8_MAX
#define INT_FAST16_MAX   INT32_MAX
#define INT_FAST32_MAX   INT32_MAX
#define UINT_FAST8_MAX   UINT8_MAX
#define UINT_FAST16_MAX  UINT32_MAX
#define UINT_FAST32_MAX  UINT32_MAX

#define INTPTR_MIN      INT32_MIN
#define INTPTR_MAX      INT32_MAX

#define INTMAX_MIN       INT32_MIN
#define INTMAX_MAX       INT32_MAX
#define UINTMAX_MAX      UINT32_MAX

#define PTRDIFF_MIN      INTPTR_MIN
#define PTRDIFF_MAX      INTPTR_MAX

#define WCHAR_MIN        0x0000
#define WCHAR_MAX        0xffff

#ifndef SIZE_MAX
    #define SIZE_MAX     UINTPTR_MAX
#endif

#ifdef __cplusplus
}
#endif

#else
#   include <stdint.h>
#   include <stdbool.h>
#endif

#if !__IS_COMPILER_GCC__
#   include <uchar.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
#define __optimal_bit_sz        (sizeof(uintalu_t) * 8)
#define __optimal_bit_msk       (__optimal_bit_sz - 1)

/*============================ TYPE DEFINE ===================================*/
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
