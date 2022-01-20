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

#ifndef __VSF_QUEUE_STREAM_H__
#define __VSF_QUEUE_STREAM_H__

/*============================ INCLUDES ======================================*/

#include "service/vsf_service_cfg.h"

#if VSF_USE_SIMPLE_STREAM == ENABLED && VSF_USE_HEAP == ENABLED

#include "utilities/vsf_utilities.h"

#if     defined(__VSF_QUEUE_STREAM_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#   undef __VSF_QUEUE_STREAM_CLASS_IMPLEMENT
#elif   defined(__VSF_QUEUE_STREAM_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#   undef __VSF_QUEUE_STREAM_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define __VSF_QUEUE_STREAM_INIT(__MAX)                                          \
            .op = &vsf_queue_stream_op,                                         \
            .max_buffer_size = (__MAX),
#define VSF_QUEUE_STREAM_INIT(__MAX)            __VSF_QUEUE_STREAM_INIT(__MAX)

#define __describe_queue_stream(__name, __max)                                  \
            vsf_queue_stream_t __name = {                                       \
                VSF_QUEUE_STREAM_INIT(__max)                                    \
            };

#define describe_queue_stream(__name, __max)    __describe_queue_stream(__name)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_class(vsf_queue_stream_t) {
    public_member(
        implement(vsf_stream_t)
        // max_buffer_size < 0 means no limitation, otherwise means the buffer limit in bytes
        int32_t max_buffer_size;
        // each write will add a entry in queue
        // max_entry_num < 0 means no limitation, otherwise means the entry limit
        int32_t max_entry_num;
    )
    private_member(
        vsf_slist_queue_t buffer_queue;
        uint32_t size;
        uint32_t entry_num;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_stream_op_t vsf_queue_stream_op;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_SIMPLE_STREAM
#endif      // __VSF_QUEUE_STREAM_H__
