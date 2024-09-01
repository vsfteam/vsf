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

#include <stdint.h>

#if !defined(__USE_LOCAL_STDBOOL__)
#   if (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L) && !defined(__cplusplus)
#       if !defined(bool)
typedef enum {
    false = 0,
    true = !false,
} bool;
#       endif
#   else
#       include <stdbool.h>
#   endif
#else
#   undef __USE_LOCAL_STDINT_STDBOOL__
#endif

#if !__IS_COMPILER_GCC__ && !__IS_COMPILER_LLVM__
#   include <uchar.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define __optimal_bit_sz        (sizeof(uintalu_t) * 8)
#define __optimal_bit_msk       (__optimal_bit_sz - 1)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if     defined(__VSF64__)
typedef uint64_t                uintalu_t;
typedef int64_t                 intalu_t;
#elif   defined(__VSF16__)
typedef uint16_t                uintalu_t;
typedef int16_t                 intalu_t;
#elif   defined(__VSF8__)
typedef uint8_t                 uintalu_t;
typedef int8_t                  intalu_t;
#else
typedef uint32_t                uintalu_t;
typedef int32_t                 intalu_t;
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
