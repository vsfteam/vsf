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

#ifndef __VSF_FIFO_STREAM_H__
#define __VSF_FIFO_STREAM_H__

/*============================ INCLUDES ======================================*/

#include "service/vsf_service_cfg.h"

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED

#if     defined(VSFSTREAM_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#   undef VSFSTREAM_CLASS_IMPLEMENT
#elif   defined(VSFSTREAM_CLASS_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#   undef VSFSTREAM_CLASS_INHERIT
#endif   

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vsf_fifo_t)
declare_simple_class(vsf_fifo_stream_t)

def_simple_class(vsf_fifo_t) {
	uint8_t *buffer;
    uint32_t size;

	uint32_t head;
	uint32_t tail;
};

def_simple_class(vsf_fifo_stream_t) {
    which(
        implement(vsf_stream_t)
        implement(vsf_fifo_t)
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_stream_op_t vsf_fifo_stream_op;

/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_fifo_init(struct vsf_fifo_t *fifo);
extern uint_fast32_t vsf_fifo_write(struct vsf_fifo_t *fifo, uint8_t *data, uint_fast32_t size);
extern uint_fast32_t vsf_fifo_read(struct vsf_fifo_t *fifo, uint8_t *data, uint_fast32_t size);
extern uint_fast32_t vsf_fifo_peek(struct vsf_fifo_t *fifo, uint8_t *data, uint_fast32_t size);
extern uint_fast32_t vsf_fifo_get_wbuf(struct vsf_fifo_t *fifo, uint8_t **data);
extern uint_fast32_t vsf_fifo_get_rbuf(struct vsf_fifo_t *fifo, uint8_t **data);
extern uint_fast32_t vsf_fifo_get_data_length(struct vsf_fifo_t *fifo);
extern uint_fast32_t vsf_fifo_get_avail_length(struct vsf_fifo_t *fifo);

#endif      // VSF_USE_SERVICE_VSFSTREAM
#endif      // __VSF_FIFO_STREAM_H__
