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

#ifndef __VSF_TRACE_H__
#define __VSF_TRACE_H__

/*============================ INCLUDES ======================================*/
#include "service/vsf_service_cfg.h"

#if VSF_USE_SIMPLE_STREAM == ENABLED
#   include "../simple_stream/vsf_simple_stream.h"
#elif VSF_USE_STREAM == ENABLED
#   include "../stream/vsf_stream.h"
#endif

#if VSF_USE_TRACE == ENABLED
#   include <stdarg.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_TRACE_CFG_LINEEND
#   define VSF_TRACE_CFG_LINEEND    "\r\n"
#endif

// display flag
#define VSF_TRACE_DF_DS(n)          (((n) & 0xFF) << 0) // data size
#define VSF_TRACE_DF_DPL(n)         (((n) & 0xFF) << 8) // data per line
#define VSF_TRACE_DF_ADDR           (1UL << 16)         // display address
#define VSF_TRACE_DF_CHAR           (1UL << 17)         // display character
#define VSF_TRACE_DF_NEWLINE        (1UL << 18)         // append newline

#define VSF_TRACE_DF_U8_16          (VSF_TRACE_DF_DS(1) | VSF_TRACE_DF_DPL(16))
#define VSF_TRACE_DF_U8_16_A        (VSF_TRACE_DF_U8_16 | VSF_TRACE_DF_ADDR)
#define VSF_TRACE_DF_U8_16_AC       (VSF_TRACE_DF_U8_16_A | VSF_TRACE_DF_CHAR)
#define VSF_TRACE_DF_U8_16_ACN      (VSF_TRACE_DF_U8_16_AC | VSF_TRACE_DF_NEWLINE)
#define VSF_TRACE_DF_U8_16_AN       (VSF_TRACE_DF_U8_16_A | VSF_TRACE_DF_NEWLINE)
#define VSF_TRACE_DF_U8_16_N        (VSF_TRACE_DF_U8_16 | VSF_TRACE_DF_NEWLINE)

#ifndef VSF_TRACE_DF_DEFAULT
#   define VSF_TRACE_DF_DEFAULT     VSF_TRACE_DF_U8_16_ACN
#endif

// TODO: fix VSF_TRACE_POINTER_HEX with arch configuration if %p is not supported
#define VSF_TRACE_POINTER_HEX       "%p"

/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_trace_print_mem         vsf_trace_buffer

#if VSF_USE_TRACE == ENABLED
#   if VSF_USE_SIMPLE_STREAM == ENABLED
#       define vsf_trace_init(__stream) __vsf_trace_init((vsf_stream_t *)(__stream))
#   elif VSF_USE_STREAM == ENABLED
#       define vsf_trace_init(__stream) __vsf_trace_init((vsf_stream_tx_t *)(__stream))
#   endif
#endif

#define vsf_trace_info(...)         vsf_trace(VSF_TRACE_INFO, __VA_ARGS__)
#define vsf_trace_warning(...)      vsf_trace(VSF_TRACE_WARNING, __VA_ARGS__)
#define vsf_trace_error(...)        vsf_trace(VSF_TRACE_ERROR, __VA_ARGS__)
#define vsf_trace_debug(...)        vsf_trace(VSF_TRACE_DEBUG, __VA_ARGS__)

/*============================ TYPES =========================================*/

typedef enum vsf_trace_level_t {
    VSF_TRACE_NONE,
    VSF_TRACE_ERROR,
    VSF_TRACE_INFO,
    VSF_TRACE_WARNING,
    VSF_TRACE_DEBUG,
    VSF_TRACE_LEVEL_NUM,
} vsf_trace_level_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_USE_TRACE == ENABLED

#   if VSF_USE_SIMPLE_STREAM == ENABLED
extern void __vsf_trace_init(vsf_stream_t *stream);
#   elif VSF_USE_STREAM == ENABLED
extern void __vsf_trace_init(vsf_stream_tx_t *ptTX);
#   endif
extern void vsf_trace_fini(void);

SECTION(".text.vsf.trace.__vsf_trace_buffer")
extern void __vsf_trace_buffer( vsf_trace_level_t level,
                                void *buffer,
                                uint_fast16_t len,
                                uint_fast32_t flag);

#define __vsf_trace_buffer3(__level, __buffer, __len)                           \
        __vsf_trace_buffer((__level), (__buffer), (__len), VSF_TRACE_DF_DEFAULT)
#define __vsf_trace_buffer4(__level, __buffer, __len, __flag)                   \
        __vsf_trace_buffer((__level), (__buffer), (__len), (__flag))
// prototype
//  vsf_trace_buffer(__level, __buffer, __len)          // use VSF_TRACE_DF_DEFAULT
//  vsf_trace_buffer(__level, __buffer, __len, __flag)  // use flag
#define vsf_trace_buffer(__level, __buffer, __len, ...)                         \
        __PLOOC_EVAL(__vsf_trace_buffer, (__level), (__buffer), (__len), ##__VA_ARGS__)\
            ((__level), (__buffer), (__len), ##__VA_ARGS__)

extern void vsf_trace_string(vsf_trace_level_t level, const char *str);
extern void vsf_trace_arg(vsf_trace_level_t level, const char *format, va_list *arg);
extern void vsf_trace(vsf_trace_level_t level, const char *format, ...);

#else
#   if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#       define vsf_trace_init(__arg)
#       define vsf_trace_fini(__arg)
#       define vsf_trace(__arg)
#       define vsf_trace_buffer(__arg)
#       define vsf_trace_string(__arg)
#   else
#       define vsf_trace_init(...)
#       define vsf_trace_fini(...)
#       define vsf_trace(...)
#       define vsf_trace_buffer(...)
#       define vsf_trace_string(...)
#   endif
#endif

#ifdef __cplusplus
}
#endif

#endif      // __VSF_TRACE_H__
