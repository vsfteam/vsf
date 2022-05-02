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

#ifndef __VSF_SDL2_STDINC_H__
#define __VSF_SDL2_STDINC_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_sdl2_cfg.h"

#if VSF_USE_SDL2 == ENABLED

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "SDL_error.h"
#include "begin_code.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define SDL_arraysize                       dimof
#define SDL_TABLESIZE                       SDL_arraysize

// stdlib
#define SDL_assert                          VSF_SDL2_ASSERT
#define SDL_free                            free
#define SDL_malloc                          malloc
#define SDL_realloc                         realloc
#define SDL_calloc                          calloc
#define SDL_memcmp                          memcmp
#define SDL_atoi                            atoi

#define SDL_stack_alloc(__type, __count)    (__type*)SDL_malloc(sizeof(__type) * (__count))
#define SDL_stack_free(__data)              SDL_free(__data)

#define SDL_min                             vsf_min
#define SDL_max                             vsf_max

// string
#define SDL_memset                          memset
#define SDL_memcpy                          memcpy
#define SDL_strlen                          strlen
#define SDL_strcmp                          strcmp
#define SDL_strcasecmp                      strcasecmp
#define SDL_strlcpy                         strlcpy
#define SDL_strrchr                         strrchr
#define SDL_strchr                          strchr
#define SDL_strdup                          strdup
#define SDL_vsnprintf                       vsnprintf
#define SDL_strtol                          strtol
#define SDL_strtoll                         strtoll
#define SDL_strtoul                         strtoul
#define SDL_strtoull                        strtoull
#define SDL_strtof                          strtof
#define SDL_strtod                          strtod
#define SDL_itoa                            itoa
#define SDL_atoi                            atoi
#define SDL_atof                            atof

// ctype
#define SDL_toupper                         toupper

// cmath
#define SDL_ceil                            ceil
#define SDL_ceilf                           ceilf
#define SDL_ceill                           ceill

#define SDL_getenv                          getenv

#define SDL_COMPILE_TIME_ASSERT(__N, __X)   VSF_STATIC_ASSERT(__X)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum SDL_bool {
    SDL_FALSE = 0,
    SDL_TRUE = 1
} SDL_bool;

#define SDL_MAX_SINT8                       INT8_MAX
#define SDL_MIN_SINT8                       INT8_MIN
typedef int8_t Sint8;

#define SDL_MAX_UINT8                       UINT8_MAX
#define SDL_MIN_UINT8                       0
typedef uint8_t Uint8;

#define SDL_MAX_SINT16                      INT16_MAX
#define SDL_MIN_SINT16                      INT16_MIN
typedef int16_t Sint16;

#define SDL_MAX_UINT16                      UINT16_MAX
#define SDL_MIN_UINT16                      0
typedef uint16_t Uint16;

#define SDL_MAX_SINT32                      INT32_MAX
#define SDL_MIN_SINT32                      INT32_MIN
typedef int32_t Sint32;

#define SDL_MAX_UINT32                      UINT32_MAX
#define SDL_MIN_UINT32                      0
typedef uint32_t Uint32;


#define SDL_MAX_SINT64                      INT64_MAX
#define SDL_MIN_SINT64                      INT64_MIN
typedef int64_t Sint64;

#define SDL_MAX_UINT64                      UINT64_MAX
#define SDL_MIN_UINT64                      0
typedef uint64_t Uint64;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

// extern APIs in math
double ceil(double x);
float ceilf(float x);
long double ceill(long double x);

#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif      // VSF_USE_SDL2
#endif      // __VSF_SDL2_STDINC_H__
/* EOF */
