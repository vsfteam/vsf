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

#ifndef __VSF_FIFO_STREAM_H__
#define __VSF_FIFO_STREAM_H__

/*============================ INCLUDES ======================================*/

#include "service/vsf_service_cfg.h"

#if VSF_USE_SIMPLE_STREAM == ENABLED

#if     defined(__VSF_FIFO_STREAM_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#   undef __VSF_FIFO_STREAM_CLASS_IMPLEMENT
#elif   defined(__VSF_FIFO_STREAM_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#   undef __VSF_FIFO_STREAM_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define vsf_fifo_stream_size_t              uint32_t

#define __VSF_FIFO_STREAM_INIT(__BUFFER, __SIZE)                                \
            .op                 = &vsf_fifo_stream_op,                          \
            .buffer             = (__BUFFER),                                   \
            .size               = (__SIZE),
#define VSF_FIFO_STREAM_INIT(__BUFFER, __SIZE)                                  \
            __VSF_FIFO_STREAM_INIT((__BUFFER), (__SIZE))

#define __vsf_fifo_stream_type(__name)      __name##_fifo_stream_t
#define vsf_fifo_stream_type(__name)        __vsf_fifo_stream_type(__name)

#define __declare_fifo_stream(__name)                                           \
            vsf_dcl_class(vsf_fifo_stream_type(__name))

#define __define_fifo_stream(__name, __size)                                    \
            vsf_class(vsf_fifo_stream_type(__name)) {                           \
                public_member(                                                  \
                    implement(vsf_fifo_stream_t)                                \
                    uint8_t __buffer[__size];                                   \
                )                                                               \
            };

#define declare_fifo_stream(__name)                                             \
            __declare_fifo_stream(__name)
#define dcl_fifo_stream(__name)                                                 \
            declare_fifo_stream(__name)
#define define_fifo_stream(__name, __size)                                      \
            __define_fifo_stream(__name, (__size))
#define def_fifo_stream(__name, __size)                                         \
            define_fifo_stream(__name, (__size))

#define __describe_fifo_stream(__name, __size)                                  \
            declare_fifo_stream(__name)                                         \
            define_fifo_stream(__name, (__size))                                \
            vsf_fifo_stream_type(__name) __name = {                             \
                VSF_FIFO_STREAM_INIT(__name.__buffer, (__size))                 \
            };

#define describe_fifo_stream(__name, __size)                                    \
            __describe_fifo_stream(__name, (__size))

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_class(vsf_byte_fifo_t) {
    public_member(
	    uint8_t *buffer;
        vsf_fifo_stream_size_t size;
    )
    private_member(
	    vsf_fifo_stream_size_t head;
	    vsf_fifo_stream_size_t tail;
    )
};

vsf_class(vsf_fifo_stream_t) {
    public_member(
        implement(vsf_stream_t)
        implement(vsf_byte_fifo_t)
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_stream_op_t vsf_fifo_stream_op;

/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_byte_fifo_init(vsf_byte_fifo_t *fifo);
extern uint_fast32_t vsf_byte_fifo_write(vsf_byte_fifo_t *fifo, uint8_t *data, uint_fast32_t size);
extern uint_fast32_t vsf_byte_fifo_read(vsf_byte_fifo_t *fifo, uint8_t *data, uint_fast32_t size);
extern uint_fast32_t vsf_byte_fifo_peek(vsf_byte_fifo_t *fifo, uint8_t *data, uint_fast32_t size);
extern uint_fast32_t vsf_byte_fifo_get_wbuf(vsf_byte_fifo_t *fifo, uint8_t **data);
extern uint_fast32_t vsf_byte_fifo_get_rbuf(vsf_byte_fifo_t *fifo, uint8_t **data);
extern uint_fast32_t vsf_byte_fifo_get_buff_length(vsf_byte_fifo_t *fifo);
extern uint_fast32_t vsf_byte_fifo_get_data_length(vsf_byte_fifo_t *fifo);
extern uint_fast32_t vsf_byte_fifo_get_avail_length(vsf_byte_fifo_t *fifo);

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_SIMPLE_STREAM
#endif      // __VSF_FIFO_STREAM_H__
