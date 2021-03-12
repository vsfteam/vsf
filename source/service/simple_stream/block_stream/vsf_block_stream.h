/***************************************************************************
 *   Copyright (C) 2009 - 2010 by Simon Qian <SimonQian@SimonQian.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef __VSF_BLOCK_STREAM_H__
#define __VSF_BLOCK_STREAM_H__

/*============================ INCLUDES ======================================*/

#include "service/vsf_service_cfg.h"

#if VSF_USE_SIMPLE_STREAM == ENABLED

#include "../../fifo/vsf_fifo.h"
#include "utilities/vsf_utilities.h"

#if     defined(__VSF_BLOCK_STREAM_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#   undef __VSF_BLOCK_STREAM_CLASS_IMPLEMENT
#elif   defined(__VSF_BLOCK_STREAM_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#   undef __VSF_BLOCK_STREAM_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_USE_FIFO != ENABLED
#   error block stream need VSF_USE_FIFO
#endif

#define vsf_block_stream_size_t             uint32_t

#define __VSF_BLOCK_STREAM_INIT(__BLOCK_NUM, __BLOCK_SIZE)                      \
            .op                 = &vsf_block_stream_op,                         \
            .block_size         = (vsf_fifo_item_size_t)(__BLOCK_SIZE),         \
            .block_num          = (vsf_fifo_index_t)(__BLOCK_NUM),
#define VSF_BLOCK_STREAM_INIT(__BLOCK_NUM, __BLOCK_SIZE)                        \
            __VSF_BLOCK_STREAM_INIT((__BLOCK_NUM), (__BLOCK_SIZE))

#define __vsf_block_stream_type(__name)     __name##_block_stream_t
#define vsf_block_stream_type(__name)       __vsf_block_stream_type(__name)

#define __declare_block_stream(__name)                                          \
            dcl_simple_class(vsf_block_stream_type(__name))                     \
            dcl_vsf_fifo(__name)

#define __define_block_stream(__name, __block_num, __block_size)                \
            typedef struct __name##_block_stream_item_t {                       \
                vsf_block_stream_size_t size;                                   \
                uint32_t buffer[(__block_size + 3) >> 2];                       \
            } __name##_block_stream_item_t;                                     \
            def_vsf_fifo(__name, __name##_block_stream_item_t, (__block_num))   \
            def_simple_class(vsf_block_stream_type(__name)) {                   \
                public_member(                                                  \
                    implement(__vsf_block_stream_t)                             \
                )                                                               \
                private_member(                                                 \
                    vsf_fifo(__name) fifo;                                      \
                )                                                               \
            };

#define declare_block_stream(__name)                                            \
            __declare_block_stream(__name)
#define dcl_block_stream(__name)                                                \
            declare_block_stream(__name)
#define define_block_stream(__name, __block_num, __block_size)                  \
            __define_block_stream(__name, (__block_num), (__block_size))
#define def_block_stream(__name, __block_num, __block_size)                     \
            define_block_stream(__name, (__block_num), (__block_size))

#define __describe_block_stream(__name, __block_num, __block_size)              \
            declare_block_stream(__name)                                        \
            define_block_stream(__name, (__block_num), (__block_size))          \
            vsf_block_stream_type(__name) __name = {                            \
                VSF_BLOCK_STREAM_INIT(__block_num, __block_size)                \
            };

#define describe_block_stream(__name, __block_num, __block_size)                \
            __describe_block_stream(__name, (__block_num), (__block_size))

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_simple_class(__vsf_block_stream_t)
dcl_simple_class(__vsf_block_stream_base_t)

def_simple_class(__vsf_block_stream_base_t) {
    public_member(
        implement(vsf_stream_t)
        vsf_fifo_item_size_t block_size;
        vsf_fifo_index_t block_num;
    )
};

def_simple_class(__vsf_block_stream_t) {
    public_member(
        implement(__vsf_block_stream_base_t)
        // implementation of virtual class vsf_fifo_t
        vsf_fifo_base_t __fifo[0];
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_stream_op_t vsf_block_stream_op;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_SIMPLE_STREAM
#endif      // __VSF_BLOCK_STREAM_H__
