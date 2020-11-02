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

#ifndef __VSF_MEM_STREAM_H__
#define __VSF_MEM_STREAM_H__

/*============================ INCLUDES ======================================*/

#include "service/vsf_service_cfg.h"

#if VSF_USE_SIMPLE_STREAM == ENABLED

#include "utilities/vsf_utilities.h"

#if     defined(__VSF_MEM_STREAM_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#   undef __VSF_MEM_STREAM_CLASS_IMPLEMENT
#elif   defined(__VSF_MEM_STREAM_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
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
            dcl_simple_class(vsf_mem_stream_type(__name))

#define __define_mem_stream(__name, __size)                                     \
            def_simple_class(vsf_mem_stream_type(__name)) {                     \
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

dcl_simple_class(vsf_mem_stream_t)
def_simple_class(vsf_mem_stream_t) {
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
