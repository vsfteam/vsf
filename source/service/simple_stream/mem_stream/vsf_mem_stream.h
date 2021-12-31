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

#ifndef __VSF_MEM_STREAM_H__
#define __VSF_MEM_STREAM_H__

/*============================ INCLUDES ======================================*/

#include "service/vsf_service_cfg.h"

#if VSF_USE_SIMPLE_STREAM == ENABLED

#include "utilities/vsf_utilities.h"

#if     defined(__VSF_MEM_STREAM_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#   undef __VSF_MEM_STREAM_CLASS_IMPLEMENT
#elif   defined(__VSF_MEM_STREAM_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#   undef __VSF_MEM_STREAM_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define __VSF_MEM_STREAM_INIT(__BUFFER, __SIZE)                                 \
            .op                 = &vsf_mem_stream_op,                           \
            .buffer             = (__BUFFER),                                   \
            .size               = (__SIZE),
#define VSF_MEM_STREAM_INIT(__BUFFER, __SIZE)                                   \
            __VSF_MEM_STREAM_INIT((__BUFFER), (__SIZE))

#define __vsf_mem_stream_type(__name)      __name##_mem_stream_t
#define vsf_mem_stream_type(__name)        __vsf_mem_stream_type(__name)

#define __declare_mem_stream(__name)                                            \
            vsf_dcl_class(vsf_mem_stream_type(__name))

#define __define_mem_stream(__name, __size)                                     \
            vsf_class(vsf_mem_stream_type(__name)) {                            \
                public_member(                                                  \
                    implement(vsf_mem_stream_t)                                 \
                    uint8_t __buffer[__size];                                   \
                )                                                               \
            };

#define declare_mem_stream(__name)                                              \
            __declare_mem_stream(__name)
#define dcl_mem_stream(__name)                                                  \
            declare_mem_stream(__name)
#define define_mem_stream(__name, __size)                                       \
            __define_mem_stream(__name, (__size))
#define def_mem_stream(__name, __size)                                          \
            define_mem_stream(__name, (__size))

#define __describe_mem_stream(__name, __size)                                   \
            declare_mem_stream(__name)                                          \
            define_mem_stream(__name, (__size))                                 \
            vsf_mem_stream_type(__name) __name = {                              \
                VSF_MEM_STREAM_INIT(__name.__buffer, (__size))                  \
            };

#define describe_mem_stream(__name, __size)                                     \
            __describe_mem_stream(__name, (__size))

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_class(vsf_mem_stream_t) {
    public_member(
        implement(vsf_stream_t)
        implement(vsf_mem_t)
        uint16_t align;
    )
    private_member(
        bool is_writing;
        uint32_t data_size;
        uint32_t rpos;
        uint32_t wpos;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_stream_op_t vsf_mem_stream_op;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_SIMPLE_STREAM
#endif      // __VSF_MEM_STREAM_H__
