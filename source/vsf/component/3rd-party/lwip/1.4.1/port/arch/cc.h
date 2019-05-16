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

#ifndef __LWIP_CC_H__
#define __LWIP_CC_H__

#include "vsf.h"
#include "lwipopts.h"

#if __BYTE_ORDER == __BIG_ENDIAN
#   define BYTE_ORDER  BIG_ENDIAN
#else
#   define BYTE_ORDER  LITTLE_ENDIAN
#endif

typedef uint8_t     u8_t;
typedef int8_t      s8_t;
typedef uint16_t    u16_t;
typedef int16_t     s16_t;
typedef uint32_t    u32_t;
typedef int32_t     s32_t;
typedef u32_t       mem_ptr_t;

#ifdef LWIP_DEBUG

#define U16_F       "hu"
#define S16_F       "hd"
#define X16_F       "hx"
#define U32_F       "lu"
#define S32_F       "ld"
#define X32_F       "lx"
#define SZT_F       "uz"

#define lwip_trace(format ...) vsf_trace(VSF_TRACE_DEBUG, format)

/* Plaform specific diagnostic output */
#define LWIP_PLATFORM_DIAG(...)                                                 \
        do {                                                                    \
            lwip_trace __VA_ARGS__;                                             \
        } while (0)

#endif      // LWIP_DEBUG

#define LWIP_PLATFORM_ASSERT(__str)                                             \
        do {                                                                    \
            vsf_trace(  VSF_TRACE_ERROR,                                        \
                        "Assertion\"%s\" failed at line %d in %s\n",            \
                        __str,                                                  \
                        __LINE__,                                               \
                        __FILE__);                                              \
            ASSERT(false);                                                      \
        } while (0)

#define PACK_STRUCT_FIELD(x) x PACKED
#define PACK_STRUCT_STRUCT PACKED
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

#endif      // __LWIP_CC_H__
