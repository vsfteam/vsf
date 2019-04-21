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

#ifndef __VSFSTREAM_H__
#define __VSFSTREAM_H__

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

#define VSF_STREAM_INIT(__stream)                       vsf_stream_init((vsf_stream_t *)(__stream))
#define VSF_STREAM_FINI(__stream)                       vsf_stream_fini((vsf_stream_t *)(__stream))
#define VSF_STREAM_WRITE(__stream, __buf, __size)       vsf_stream_write((vsf_stream_t *)(__stream), (__buf), (__size))
#define VSF_STREAM_READ(__stream, __buf, __size)        vsf_stream_read((vsf_stream_t *)(__stream), (__buf), (__size))
#define VSF_STREAM_GET_DATA_SIZE(__stream)              vsf_stream_get_data_size((vsf_stream_t *)(__stream))
#define VSF_STREAM_GET_FREE_SIZE(__stream)              vsf_stream_get_free_size((vsf_stream_t *)(__stream))
#define VSF_STREAM_GET_WBUF(__stream, p)                vsf_stream_get_wbuf((vsf_stream_t *)(__stream), (p))
#define VSF_STREAM_GET_RBUF(__stream, p)                vsf_stream_get_rbuf((vsf_stream_t *)(__stream), (p))
#define VSF_STREAM_CONNECT_RX(__stream)                 vsf_stream_connect_rx((vsf_stream_t *)(__stream))
#define VSF_STREAM_CONNECT_TX(__stream)                 vsf_stream_connect_tx((vsf_stream_t *)(__stream))
#define VSF_STREAM_DISCONNECT_RX(__stream)              vsf_stream_disconnect_rx((vsf_stream_t *)(__stream))
#define VSF_STREAM_DISCONNECT_TX(__stream)              vsf_stream_disconnect_tx((vsf_stream_t *)(__stream))
#define VSF_STREAM_IS_RX_CONNECTED(__stream)            vsf_stream_is_rx_connected((vsf_stream_t *)(__stream))
#define VSF_STREAM_IS_TX_CONNECTED(__stream)            vsf_stream_is_tx_connected((vsf_stream_t *)(__stream))
#define VSF_STREAM_SET_RX_THRESHOLD(__stream, __thres)  vsf_stream_set_rx_threshold((vsf_stream_t *)(__stream), (__thres))
#define VSF_STREAM_SET_TX_THRESHOLD(__stream, __thres)  vsf_stream_set_tx_threshold((vsf_stream_t *)(__stream), (__thres))

/*============================ TYPES =========================================*/

declare_simple_class(vsf_stream_t)
declare_simple_class(vsf_stream_terminal_t)

enum vsf_stream_evt_t {
    VSF_STREAM_ON_CONNECT,
    VSF_STREAM_ON_DISCONNECT,
    VSF_STREAM_ON_IN,
    VSF_STREAM_ON_RX = VSF_STREAM_ON_IN,
    VSF_STREAM_ON_OUT,
    VSF_STREAM_ON_TX = VSF_STREAM_ON_OUT,
};
typedef enum vsf_stream_evt_t vsf_stream_evt_t;

struct vsf_stream_op_t {
    void (*init)(vsf_stream_t *stream);
    void (*fini)(vsf_stream_t *stream);
    // for read/write, if buffer->buffer is NULL,
    //         then do dummy read/write of buffer->size
    uint_fast32_t (*write)(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
    uint_fast32_t (*read)(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
    uint_fast32_t (*get_data_length)(vsf_stream_t *stream);
    uint_fast32_t (*get_avail_length)(vsf_stream_t *stream);
    // get consequent buffer for read/write
    uint_fast32_t (*get_wbuf)(vsf_stream_t *stream, uint8_t **ptr);
    uint_fast32_t (*get_rbuf)(vsf_stream_t *stream, uint8_t **ptr);
};
typedef struct vsf_stream_op_t vsf_stream_op_t;

def_simple_class(vsf_stream_terminal_t) {

    public_member(
        void *param;
        void (*evthandler)(void *param, vsf_stream_evt_t evt);
    )

    private_member(
        uint32_t threshold;
        bool ready;
        bool data_notified;
    )
};

def_simple_class(vsf_stream_t) {

    public_member(
        vsf_stream_op_t const *op;
    )

    protected_member(
        union {
            struct {
                vsf_stream_terminal_t tx;
                vsf_stream_terminal_t rx;
            };
            vsf_stream_terminal_t terminal[2];
        };
    )
};

/*============================ INCLUDES ======================================*/

#include "./fifo_stream/vsf_fifo_stream.h"

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_stream_init(vsf_stream_t *stream);
extern vsf_err_t vsf_stream_fini(vsf_stream_t *stream);
extern uint_fast32_t vsf_stream_write(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
extern uint_fast32_t vsf_stream_read(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
extern void vsf_stream_set_tx_threshold(vsf_stream_t *stream, uint_fast32_t threshold);
extern void vsf_stream_set_rx_threshold(vsf_stream_t *stream, uint_fast32_t threshold);
extern uint_fast32_t vsf_stream_get_data_size(vsf_stream_t *stream);
extern uint_fast32_t vsf_stream_get_free_size(vsf_stream_t *stream);
extern uint_fast32_t vsf_stream_get_wbuf(vsf_stream_t *stream, uint8_t **ptr);
extern uint_fast32_t vsf_stream_get_rbuf(vsf_stream_t *stream, uint8_t **ptr);
extern void vsf_stream_connect_rx(vsf_stream_t *stream);
extern void vsf_stream_connect_tx(vsf_stream_t *stream);
extern void vsf_stream_disconnect_rx(vsf_stream_t *stream);
extern void vsf_stream_disconnect_tx(vsf_stream_t *stream);
extern bool vsf_stream_is_rx_connected(vsf_stream_t *stream);
extern bool vsf_stream_is_tx_connected(vsf_stream_t *stream);

#endif      // VSF_USE_SERVICE_VSFSTREAM
#endif      // __VSFSTREAM_H__
