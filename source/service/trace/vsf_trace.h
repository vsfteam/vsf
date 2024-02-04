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

#ifndef __VSF_TRACE_H__
#define __VSF_TRACE_H__

/*============================ INCLUDES ======================================*/

#include "service/vsf_service_cfg.h"

#if     defined(__VSF_TRACE_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_TRACE_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

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

#ifndef VSF_TRACE_CFG_DEFAULT_LEVEL
#   define VSF_TRACE_CFG_DEFAULT_LEVEL  VSF_TRACE_INFO
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
typedef struct vsf_stream_t vsf_stream_t;
#       define vsf_trace_init(__stream) __vsf_trace_init((vsf_stream_t *)(__stream))
#   elif VSF_USE_STREAM == ENABLED
typedef struct vsf_stream_tx_t vsf_stream_tx_t;
#       define vsf_trace_init(__stream) __vsf_trace_init((vsf_stream_tx_t *)(__stream))
#   endif
#endif

#define vsf_trace_info(...)         vsf_trace(VSF_TRACE_INFO, __VA_ARGS__)
#define vsf_trace_warning(...)      vsf_trace(VSF_TRACE_WARNING, __VA_ARGS__)
#define vsf_trace_error(...)        vsf_trace(VSF_TRACE_ERROR, __VA_ARGS__)
#define vsf_trace_debug(...)        vsf_trace(VSF_TRACE_DEBUG, __VA_ARGS__)

// background trace

#define __vsf_bgtrace_type(__name)  __name##_bgtrace_t
#define vsf_bgtrace_type(__name)    __vsf_bgtrace_type(__name)
#define __vsf_bgtrace_etype(__name) __name##_bgtrace_ele_t
#define vsf_bgtrace_etype(__name)   __vsf_bgtrace_etype(__name)

#define __declare_bgtrace(__name)   vsf_dcl_class(vsf_bgtrace_type(__name))
#define __define_bgtrace(__name, __ele_num, ...)                                \
            typedef struct vsf_bgtrace_etype(__name) {                          \
                __VA_ARGS__                                                     \
            } vsf_bgtrace_etype(__name);                                        \
            vsf_class(vsf_bgtrace_type(__name)) {                               \
                public_member(                                                  \
                    implement(vsf_bgtrace_t)                                    \
                    vsf_bgtrace_etype(__name) __elements[__ele_num];            \
                )                                                               \
            };

#define declare_bgtrace(__name)     __declare_bgtrace(__name)
#define dcl_bgtrace(__name)         declare_bgtrace(__name)

#define define_bgtrace(__name, __ele_num, ...)                                  \
            __define_bgtrace(__name, (__ele_num), __VA_ARGS__)
#define def_bgtrace(__name, __ele_num, ...)                                     \
            define_bgtrace(__name, (__ele_num), __VA_ARGS__)

#define __describe_bgtrace(__name, __ele_num, __print_element, ...)             \
            declare_bgtrace(__name)                                             \
            define_bgtrace(__name, (__ele_num), __VA_ARGS__)                    \
            vsf_bgtrace_type(__name) __name = {                                 \
                .ele_num = (__ele_num),                                         \
                .ele_size = sizeof(vsf_bgtrace_etype(__name)),                  \
                .elements = (void *)__name.__elements,                          \
                .print_element = (void (*)(uint16_t pos, void *element))(__print_element),\
            };

#define describe_bgtrace(__name, __ele_num, __print_element, ...)               \
            __describe_bgtrace(__name, (__ele_num), (__print_element), __VA_ARGS__)

/*============================ TYPES =========================================*/

typedef enum vsf_trace_level_t {
    VSF_TRACE_NONE,
    VSF_TRACE_ERROR,
    VSF_TRACE_INFO,
    VSF_TRACE_WARNING,
    VSF_TRACE_DEBUG,
    VSF_TRACE_LEVEL_NUM,
} vsf_trace_level_t;

vsf_class(vsf_bgtrace_t) {
    public_member(
        uint16_t ele_num, ele_size;
        void *elements;
        void (*print_element)(uint16_t pos, void *element);
    )

    private_member(
        uint16_t pos, num;
        uint32_t total;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_USE_TRACE == ENABLED

#   if VSF_USE_SIMPLE_STREAM == ENABLED
extern void __vsf_trace_init(vsf_stream_t *stream);
#   elif VSF_USE_STREAM == ENABLED
extern void __vsf_trace_init(vsf_stream_tx_t *ptTX);
#   endif
extern void vsf_trace_fini(void);

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

#if VSF_APPLET_USE_TRACE == ENABLED
typedef struct vsf_trace_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__vsf_trace_buffer);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__vsf_trace_output);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(vsf_trace_string);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(vsf_trace_arg);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(vsf_trace_assert);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(vsf_trace);
} vsf_trace_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_trace_vplt_t vsf_trace_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_TRACE_LIB__))\
    && VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_APPLET_USE_TRACE == ENABLED

#   define VSF_APPLET_TRACE_VPLT                                                 \
            ((vsf_trace_vplt_t *)(VSF_SERVICE_APPLET_VPLT->trace_vplt))

#define VSF_APPLET_TRACE_ENTRY(__NAME)                                           \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_APPLET_TRACE_VPLT, __NAME)
#define VSF_APPLET_TRACE_IMP(...)                                                \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_APPLET_TRACE_VPLT, __VA_ARGS__)

VSF_APPLET_TRACE_IMP(__vsf_trace_buffer, void, vsf_trace_level_t level, void *buffer, uint_fast16_t len, uint_fast32_t flag) {
    VSF_APPLET_TRACE_ENTRY(__vsf_trace_buffer)(level, buffer, len, flag);
}
VSF_APPLET_TRACE_IMP(vsf_trace_string, void, vsf_trace_level_t level, const char *str) {
    VSF_APPLET_TRACE_ENTRY(vsf_trace_string)(level, str);
}
VSF_APPLET_TRACE_IMP(vsf_trace_arg, void, vsf_trace_level_t level, const char *format, va_list arg) {
    VSF_APPLET_TRACE_ENTRY(vsf_trace_arg)(level, format, arg);
}
VSF_APPLET_TRACE_IMP(__vsf_trace_output, uint_fast32_t, const char *buff, uint_fast32_t size) {
    return VSF_APPLET_TRACE_ENTRY(__vsf_trace_output)(buff, size);
}
VSF_APPLET_TRACE_IMP(vsf_trace_assert, void, const char *expr, const char *file, int line, const char *func) {
    VSF_APPLET_TRACE_ENTRY(vsf_trace_assert)(expr, file, line, func);
}

VSF_APPLET_VPLT_FUNC_DECORATOR(vsf_trace) void vsf_trace(vsf_trace_level_t level, const char *format, ...) {
    va_list ap;

    va_start(ap, format);
    vsf_trace_arg(level, format, ap);
    va_end(ap);
}

#else

SECTION(".text.vsf.trace.__vsf_trace_buffer")
extern void __vsf_trace_buffer( vsf_trace_level_t level,
                                void *buffer,
                                uint_fast16_t len,
                                uint_fast32_t flag);

extern void vsf_trace_string(vsf_trace_level_t level, const char *str);
extern void vsf_trace_arg(vsf_trace_level_t level, const char *format, va_list arg);
extern void vsf_trace(vsf_trace_level_t level, const char *format, ...);

extern uint_fast32_t __vsf_trace_output(const char *buff, uint_fast32_t size);
extern void vsf_trace_assert(const char *expr, const char *file, int line, const char *func);

#endif

// bgtrace

extern void vsf_bgtrace_clear(vsf_bgtrace_t *bgtrace);
extern uint32_t vsf_bgtrace_total(vsf_bgtrace_t *bgtrace);
extern void vsf_bgtrace_append(vsf_bgtrace_t *bgtrace, void *element);
extern void vsf_bgtrace_print(vsf_bgtrace_t *bgtrace, int cnt);

#else
#   if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#       define vsf_trace_init(__arg)
#       define vsf_trace_fini(__arg)
#       define vsf_trace(__arg)
#       define vsf_trace_buffer(__arg)
#       define vsf_trace_string(__arg)
#       define vsf_bgtrace_clear(__arg)
#       define vsf_bgtrace_total(__arg)
#       define vsf_bgtrace_append(__arg)
#       define vsf_bgtrace_print(__arg)
#   else
#       define vsf_trace_init(...)
#       define vsf_trace_fini(...)
#       define vsf_trace(...)
#       define vsf_trace_buffer(...)
#       define vsf_trace_string(...)
#       define vsf_bgtrace_clear(...)
#       define vsf_bgtrace_total(...)
#       define vsf_bgtrace_append(...)
#       define vsf_bgtrace_print(...)
#   endif
#endif

#ifdef __cplusplus
}
#endif

#endif      // __VSF_TRACE_H__
