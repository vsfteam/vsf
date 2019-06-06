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

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
#   include "../vsfstream/vsfstream.h"
#elif VSF_USE_SERVICE_STREAM == ENABLED
#   include "../stream/vsf_stream.h"
#endif

#if VSF_USE_TRACE == ENABLED
#   include <stdarg.h>
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_TRACE_CFG_LINEEND
#   define VSF_TRACE_CFG_LINEEND    "\r\n"
#endif

// display flag
#define VSF_TRACE_DF_DS(n)          (((n) & 0xFF) << 0) // data size
#define VSF_TRACE_DF_DPL(n)         (((n) & 0xFF) << 8) // data per line
#define VSF_TRACE_DF_ADDR           (1 << 16)           // display address
#define VSF_TRACE_DF_CHAR           (1 << 17)           // display character
#define VSF_TRACE_DF_NEWLINE        (1 << 18)           // append newline

#define VSF_TRACE_DF_U8_16          (VSF_TRACE_DF_DS(1) | VSF_TRACE_DF_DPL(16))
#define VSF_TRACE_DF_U8_16_A        (VSF_TRACE_DF_U8_16 | VSF_TRACE_DF_ADDR)
#define VSF_TRACE_DF_U8_16_AC       (VSF_TRACE_DF_U8_16_A | VSF_TRACE_DF_CHAR)
#define VSF_TRACE_DF_U8_16_ACN      (VSF_TRACE_DF_U8_16_AC | VSF_TRACE_DF_NEWLINE)
#define VSF_TRACE_DF_U8_16_AN       (VSF_TRACE_DF_U8_16_A | VSF_TRACE_DF_NEWLINE)
#define VSF_TRACE_DF_U8_16_N        (VSF_TRACE_DF_U8_16 | VSF_TRACE_DF_NEWLINE)

#ifndef VSF_TRACE_DF_DEFAULT
#   define VSF_TRACE_DF_DEFAULT     VSF_TRACE_DF_U8_16_ACN
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_trace_print_mem(...)    vsf_trace_buffer(__VA_ARGS__)

#if VSF_USE_TRACE == ENABLED
#   if VSF_USE_SERVICE_VSFSTREAM == ENABLED
#       define vsf_trace_init(__ADDR)  __vsf_trace_init((vsf_stream_t *)(__ADDR))
#   elif VSF_USE_SERVICE_STREAM == ENABLED
#       define vsf_trace_init(__ADDR)  __vsf_trace_init((vsf_stream_tx_t *)(__ADDR))
#   endif
#endif

/*============================ TYPES =========================================*/

enum vsf_trace_level_t {
    VSF_TRACE_NONE,
    VSF_TRACE_ERROR,
    VSF_TRACE_INFO,
    VSF_TRACE_WARNING,
    VSF_TRACE_DEBUG,
    VSF_TRACE_LEVEL_NUM,
};
typedef enum vsf_trace_level_t vsf_trace_level_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_USE_TRACE == ENABLED

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
extern void __vsf_trace_init(vsf_stream_t *stream);
#elif VSF_USE_SERVICE_STREAM == ENABLED
extern void __vsf_trace_init(vsf_stream_tx_t *ptTX);
#endif
extern void vsf_trace_fini(void);

SECTION(".text.vsf.trace.vsf_trace_buffer")
extern void vsf_trace_buffer(   vsf_trace_level_t level, 
                                void *buffer, 
                                uint_fast16_t len, 
                                uint_fast32_t flag);
extern void vsf_trace_string(vsf_trace_level_t level, const char *str);
extern void vsf_trace(vsf_trace_level_t level, const char *format, ...);

#else
#   define vsf_trace_init(...)
#   define vsf_trace_fini(...)
//#   define vsf_trace_printr(...)
//#   define vsf_trace_printf_arg(...)
//#   define vsf_trace_printf(...)
//#   define vsf_trace_prints(...)
//#   define vsf_trace_printb(...)
#   define vsf_trace(...)
#   define vsf_trace_buffer(...)
#   define vsf_trace_string(...)
#endif


#endif      // __VSF_TRACE_H__
