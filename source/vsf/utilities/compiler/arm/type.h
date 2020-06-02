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

#   ifdef __cplusplus
extern "C" {
#   endif
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
#   define UINT64_MAX          ((uint64_t)(-1))
#endif

typedef uint32_t            uintptr_t;
typedef int32_t             intptr_t;
typedef uint64_t            uintmax_t;
typedef int64_t             intmax_t;

#ifndef bool
typedef enum {
    false = 0,
    true = !false,
} bool;
#endif

#   ifdef __cplusplus
}
#   endif

#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include <stddef.h>
#include <assert.h>

#include "../__common/__type.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
#define __optimal_bit_sz        (sizeof(uintalu_t) * 8)
#define __optimal_bit_msk       (__optimal_bit_sz - 1)

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
