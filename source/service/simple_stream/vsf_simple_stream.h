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

#ifndef __VSF_SIMPLE_STREAM_H__
#define __VSF_SIMPLE_STREAM_H__

/*============================ INCLUDES ======================================*/

#include "service/vsf_service_cfg.h"
#include "utilities/vsf_utilities.h"

#if VSF_USE_SIMPLE_STREAM == ENABLED

#if     defined(__VSF_SIMPLE_STREAM_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#   undef __VSF_SIMPLE_STREAM_CLASS_IMPLEMENT
#elif   defined(__VSF_SIMPLE_STREAM_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#   undef __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_STREAM_CFG_TICKTOCK
#   define VSF_STREAM_CFG_TICKTOCK                      ENABLED
#endif

#ifndef VSF_STREAM_CFG_THRESHOLD
#   define VSF_STREAM_CFG_THRESHOLD                     ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

// VSF_STREAM_XXX in upper case is compatible with all stream classes
#define VSF_STREAM_INIT(__stream)                       vsf_stream_init((vsf_stream_t *)(__stream))
#define VSF_STREAM_FINI(__stream)                       vsf_stream_fini((vsf_stream_t *)(__stream))
#define VSF_STREAM_WRITE(__stream, __buf, __size)       vsf_stream_write((vsf_stream_t *)(__stream), (__buf), (__size))
#define VSF_STREAM_READ(__stream, __buf, __size)        vsf_stream_read((vsf_stream_t *)(__stream), (__buf), (__size))
#define VSF_STREAM_GET_BUFF_SIZE(__stream)              vsf_stream_get_buff_size((vsf_stream_t *)(__stream))
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
#if VSF_STREAM_CFG_THRESHOLD == ENABLED
#define VSF_STREAM_SET_RX_THRESHOLD(__stream, __thres)  vsf_stream_set_rx_threshold((vsf_stream_t *)(__stream), (__thres))
#define VSF_STREAM_SET_TX_THRESHOLD(__stream, __thres)  vsf_stream_set_tx_threshold((vsf_stream_t *)(__stream), (__thres))
#endif

/*============================ TYPES =========================================*/

dcl_simple_class(vsf_stream_t)
dcl_simple_class(vsf_stream_terminal_t)
dcl_simple_class(vsf_stream_op_t)

typedef enum vsf_stream_evt_t {
    VSF_STREAM_ON_CONNECT,
    VSF_STREAM_ON_DISCONNECT,
    VSF_STREAM_ON_IN,
    VSF_STREAM_ON_RX = VSF_STREAM_ON_IN,
    VSF_STREAM_ON_OUT,
    VSF_STREAM_ON_TX = VSF_STREAM_ON_OUT,
} vsf_stream_evt_t;

def_simple_class(vsf_stream_op_t) {
    protected_member(
        void (*init)(vsf_stream_t *stream);
        void (*fini)(vsf_stream_t *stream);
        // for read/write, if buffer->buffer is NULL,
        //         then do dummy read/write of buffer->size
        uint_fast32_t (*write)(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
        uint_fast32_t (*read)(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
        uint_fast32_t (*get_buff_length)(vsf_stream_t *stream);
        uint_fast32_t (*get_data_length)(vsf_stream_t *stream);
        uint_fast32_t (*get_avail_length)(vsf_stream_t *stream);
        // get consequent buffer for read/write
        uint_fast32_t (*get_wbuf)(vsf_stream_t *stream, uint8_t **ptr);
        uint_fast32_t (*get_rbuf)(vsf_stream_t *stream, uint8_t **ptr);
    )
};

def_simple_class(vsf_stream_terminal_t) {

    public_member(
        void *param;
        void (*evthandler)(void *param, vsf_stream_evt_t evt);
    )

    private_member(
#if VSF_STREAM_CFG_THRESHOLD == ENABLED
        uint32_t threshold;
#endif
        bool ready;
        bool data_notified;
    )
};

def_simple_class(vsf_stream_t) {

    public_member(
        vsf_stream_op_t const *op;
#if VSF_STREAM_CFG_TICKTOCK == ENABLED
        bool is_ticktock_read;
        bool is_ticktock_write;
#endif
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

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_stream_init(vsf_stream_t *stream);
extern vsf_err_t vsf_stream_fini(vsf_stream_t *stream);
extern uint_fast32_t vsf_stream_write(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
extern uint_fast32_t vsf_stream_read(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
#if VSF_STREAM_CFG_THRESHOLD == ENABLED
extern void vsf_stream_set_tx_threshold(vsf_stream_t *stream, uint_fast32_t threshold);
extern void vsf_stream_set_rx_threshold(vsf_stream_t *stream, uint_fast32_t threshold);
#endif
extern uint_fast32_t vsf_stream_get_buff_size(vsf_stream_t *stream);
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

#ifdef __cplusplus
}
#endif

/*============================ INCLUDES ======================================*/

#include "./fifo_stream/vsf_fifo_stream.h"
#include "./mem_stream/vsf_mem_stream.h"
#include "./block_stream/vsf_block_stream.h"

#endif      // VSF_USE_SIMPLE_STREAM
#endif      // __VSF_SIMPLE_STREAM_H__
