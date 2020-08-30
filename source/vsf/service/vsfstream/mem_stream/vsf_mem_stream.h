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

#include "utilities/vsf_utilities.h"

#if     defined(__VSFSTREAM_MEM_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#   undef __VSFSTREAM_MEM_CLASS_IMPLEMENT
#elif   defined(__VSFSTREAM_MEM_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#   undef __VSFSTREAM_MEM_CLASS_INHERIT__
#endif   

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define __describe_mem_stream_ex(__name, __buffer, __size)                      \
            vsf_mem_stream_t __name = {                                         \
                .op                 = &vsf_mem_stream_op,                       \
                .buffer             = (__buffer),                               \
                .size               = (__size),                                 \
            };

#define __describe_mem_stream(__name, __size)                                   \
            uint8_t __##__name##_buffer[(__size)];                              \
            __describe_mem_stream_ex(__name, __##__name##_buffer, (__size))

#define __declare_mem_stream(__name)                                            \
            extern vsf_mem_stream_t __name;

#define declare_mem_stream(__name)                                              \
            __declare_mem_stream(__name)
#define describe_mem_stream_ex(__name, __buffer, __size)                        \
            __describe_mem_stream_ex(__name, (__buffer), (__size))
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

#endif      // VSF_USE_SERVICE_VSFSTREAM
#endif      // __VSF_MEM_STREAM_H__
