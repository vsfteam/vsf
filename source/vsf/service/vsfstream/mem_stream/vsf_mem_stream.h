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

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED

#include "utilities/communicate.h"

#if     defined(VSFSTREAM_MEM_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#   undef VSFSTREAM_MEM_CLASS_IMPLEMENT
#elif   defined(VSFSTREAM_MEM_CLASS_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#   undef VSFSTREAM_MEM_CLASS_INHERIT
#endif   

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#ifndef VSF_MEM_STREAM_CFG_SUPPORT_BLOCK
#   define VSF_MEM_STREAM_CFG_SUPPORT_BLOCK      ENABLED
#endif

#ifndef VSF_MEM_STREAM_CFG_SUPPORT_STREAM
#   define VSF_MEM_STREAM_CFG_SUPPORT_STREAM    ENABLED
#endif

#if VSF_MEM_STREAM_CFG_SUPPORT_BLOCK != ENABLED && VSF_MEM_STREAM_CFG_SUPPORT_STREAM != ENABLED
#   error why do people need me?
#endif

#define __describe_mem_stream_ex(__NAME, __BUFFER, __SIZE)                      \
            vsf_mem_stream_t __NAME = {                                         \
                .op                 = &vsf_mem_stream_op,                       \
                .pchBuffer          = (__BUFFER),                               \
                .nSize              = (__SIZE),                                 \
            };

#define __describe_mem_stream(__NAME, __SIZE)                                   \
            uint8_t __##__NAME##_buffer[(__SIZE)];                              \
            __describe_mem_stream_ex(__NAME, __##__NAME##_buffer, (__SIZE))

#define __declare_mem_stream(__name)                                            \
            extern vsf_mem_stream_t __name;

#define declare_mem_stream(__name)                                              \
            __declare_mem_stream(__name)
#define describe_mem_stream_ex(__NAME, __BUFFER, __SIZE)                        \
            __describe_mem_stream_ex(__NAME, (__BUFFER), (__SIZE))
#define describe_mem_stream(__NAME, __SIZE)                                     \
            __describe_mem_stream(__NAME, (__SIZE))

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vsf_mem_stream_t)
def_simple_class(vsf_mem_stream_t) {
    public_member(
        implement(vsf_stream_t)
        implement(vsf_mem_t)

        union {
#if VSF_MEM_STREAM_CFG_SUPPORT_BLOCK == ENABLED
            uint16_t block_size;
#endif
#if VSF_MEM_STREAM_CFG_SUPPORT_STREAM == ENABLED
            uint16_t align;
#endif
        };
#if VSF_MEM_STREAM_CFG_SUPPORT_BLOCK == ENABLED && VSF_MEM_STREAM_CFG_SUPPORT_STREAM == ENABLED
        bool is_block;
#endif
    )
    private_member(
        union {
            uint32_t state;
#if VSF_MEM_STREAM_CFG_SUPPORT_BLOCK == ENABLED
            struct {
                uint8_t rblock_idx;
                uint8_t rblock_num;
                uint8_t wblock_idx;
                uint32_t *block_size_arr;
            };
#endif
#if VSF_MEM_STREAM_CFG_SUPPORT_STREAM == ENABLED
            struct {
                bool is_writing;
                uint32_t data_size;
                uint32_t rpos;
                uint32_t wpos;
            };
#endif
        };
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_stream_op_t vsf_mem_stream_op;

/*============================ PROTOTYPES ====================================*/

extern uint_fast32_t vsf_mem_stream_get_buffer_len(vsf_mem_stream_t *stream, uint8_t *buf);
extern void vsf_mem_stream_set_buffer_len(vsf_mem_stream_t *stream, uint8_t *buf, uint_fast32_t len);

#endif      // VSF_USE_SERVICE_VSFSTREAM
#endif      // __VSF_MEM_STREAM_H__
