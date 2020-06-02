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

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED

#include "utilities/communicate.h"

#if     defined(VSFSTREAM_BLOCK_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#   undef VSFSTREAM_BLOCK_CLASS_IMPLEMENT
#elif   defined(VSFSTREAM_BLOCK_CLASS_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#   undef VSFSTREAM_BLOCK_CLASS_INHERIT
#endif   

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define __describe_block_stream_ex(__NAME, __BUFFER, __SIZE)                      \
            vsf_block_stream_t __NAME = {                                         \
                .op                 = &vsf_block_stream_op,                       \
                .pchBuffer          = (__BUFFER),                               \
                .nSize              = (__SIZE),                                 \
            };

#define __describe_block_stream(__NAME, __SIZE)                                   \
            uint8_t __##__NAME##_buffer[(__SIZE)];                              \
            __describe_block_stream_ex(__NAME, __##__NAME##_buffer, (__SIZE))

#define __declare_block_stream(__name)                                            \
            extern vsf_block_stream_t __name;

#define declare_block_stream(__name)                                              \
            __declare_block_stream(__name)
#define describe_block_stream_ex(__NAME, __BUFFER, __SIZE)                        \
            __describe_block_stream_ex(__NAME, (__BUFFER), (__SIZE))
#define describe_block_stream(__NAME, __SIZE)                                     \
            __describe_block_stream(__NAME, (__SIZE))

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vsf_block_stream_t)
def_simple_class(vsf_block_stream_t) {
    public_member(
        implement(vsf_stream_t)
        implement(vsf_mem_t)

        uint32_t block_size;
    )
    private_member(
        union {
            uint32_t state;
            struct {
                uint8_t rblock_idx;
                uint8_t rblock_num;
                uint8_t wblock_idx;
            };
        };
        uint32_t *block_size_arr;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_stream_op_t vsf_block_stream_op;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_SERVICE_VSFSTREAM
#endif      // __VSF_BLOCK_STREAM_H__
