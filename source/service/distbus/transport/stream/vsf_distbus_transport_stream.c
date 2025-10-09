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

/*
 * distbus transport implemented by usbd cdcacm
 *
 * Dependency:
 */

/*============================ INCLUDES ======================================*/

#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#define __VSF_DISTBUS_TRANSPORT_STREAM_CLASS_IMPLEMENT
#include "./vsf_distbus_transport_stream.h"

#if VSF_USE_SIMPLE_STREAM == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static uint_fast32_t __vsf_distbus_transport_try_send(vsf_distbus_transport_stream_t *transport_stream)
{
    uint_fast32_t trans_size = 0, wsiz;
    uint8_t *wbuf;

    while (transport_stream->tx.size > 0) {
        wsiz = VSF_STREAM_GET_WBUF(transport_stream->stream_tx, &wbuf);
        if (wsiz <= 0) {
            break;
        }

        wsiz = vsf_min(wsiz, transport_stream->tx.size);
        memcpy(wbuf, transport_stream->tx.buffer, wsiz);
        trans_size += wsiz;
        transport_stream->tx.size -= wsiz;
        transport_stream->tx.buffer += wsiz;

        VSF_STREAM_WRITE(transport_stream->stream_tx, NULL, wsiz);
    }
    return trans_size;
}

static uint_fast32_t __vsf_distbus_transport_try_recv(vsf_distbus_transport_stream_t *transport_stream)
{
    uint_fast32_t trans_size = 0, rsiz;
    uint8_t *rbuf;

    while (transport_stream->rx.size > 0) {
        rsiz = VSF_STREAM_GET_RBUF(transport_stream->stream_rx, &rbuf);
        if (rsiz <= 0) {
            break;
        }

        rsiz = vsf_min(rsiz, transport_stream->rx.size);
        memcpy(transport_stream->rx.buffer, rbuf, rsiz);
        trans_size += rsiz;
        transport_stream->rx.size -= rsiz;
        transport_stream->rx.buffer += rsiz;

        VSF_STREAM_READ(transport_stream->stream_rx, NULL, rsiz);
    }
    return trans_size;
}

static void __vsf_distbus_transport_stream_evthandler(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt)
{
    vsf_distbus_transport_stream_t *transport_stream = param;
    uint32_t remain_size;

    switch (evt) {
    case VSF_STREAM_ON_CONNECT:
        if (stream == transport_stream->stream_rx) {
            if (!transport_stream->rx.is_connected) {
                transport_stream->rx.is_connected = true;
                if (transport_stream->tx.is_connected && transport_stream->callback.on_inited != NULL) {
                    transport_stream->callback.on_inited(transport_stream->callback.param);
                }
            }
            goto check_rx;
        } else {
            if (!transport_stream->tx.is_connected) {
                transport_stream->tx.is_connected = true;
                if (transport_stream->rx.is_connected && transport_stream->callback.on_inited != NULL) {
                    transport_stream->callback.on_inited(transport_stream->callback.param);
                }
            }
            goto check_tx;
        }
        break;
    case VSF_STREAM_ON_IN:
    check_rx:
        remain_size = transport_stream->rx.size;
        if (    (remain_size > 0)
            &&  (__vsf_distbus_transport_try_recv(transport_stream) == remain_size)
            &&  (transport_stream->rx.callback.on_recv != NULL)) {
            transport_stream->rx.callback.on_recv(transport_stream->rx.callback.param);
        }
        break;
    case VSF_STREAM_ON_OUT:
    check_tx:
        remain_size = transport_stream->tx.size;
        if (    (remain_size > 0)
            &&  (__vsf_distbus_transport_try_send(transport_stream) == remain_size)
            &&  (transport_stream->tx.callback.on_sent != NULL)) {
            transport_stream->tx.callback.on_sent(transport_stream->tx.callback.param);
        }
        break;
    }
}

bool vsf_distbus_transport_stream_init(void *transport, void *p, void (*on_inited)(void *p))
{
    vsf_distbus_transport_stream_t *transport_stream = transport;

    transport_stream->tx.size = 0;
    transport_stream->tx.is_connected = false;
    transport_stream->rx.size = 0;
    transport_stream->rx.is_connected = false;
    transport_stream->callback.param = p;
    transport_stream->callback.on_inited = on_inited;

    transport_stream->stream_rx->rx.param = transport_stream;
    transport_stream->stream_rx->rx.evthandler = __vsf_distbus_transport_stream_evthandler;
    VSF_STREAM_CONNECT_RX(transport_stream->stream_rx);

    transport_stream->stream_tx->tx.param = transport_stream;
    transport_stream->stream_tx->tx.evthandler = __vsf_distbus_transport_stream_evthandler;
    VSF_STREAM_CONNECT_TX(transport_stream->stream_tx);
    return false;
}

bool vsf_distbus_transport_stream_send(void *transport, uint8_t *buffer, uint_fast32_t size, void *p, void (*on_sent)(void *p))
{
    vsf_distbus_transport_stream_t *transport_stream = transport;
    transport_stream->tx.buffer = buffer;
    transport_stream->tx.size = size;
    transport_stream->tx.callback.param = p;
    transport_stream->tx.callback.on_sent = on_sent;
    return __vsf_distbus_transport_try_send(transport_stream) == size;
}

bool vsf_distbus_transport_stream_recv(void *transport, uint8_t *buffer, uint_fast32_t size, void *p, void (*on_recv)(void *p))
{
    vsf_distbus_transport_stream_t *transport_stream = transport;
    transport_stream->rx.buffer = buffer;
    transport_stream->rx.size = size;
    transport_stream->rx.callback.param = p;
    transport_stream->rx.callback.on_recv = on_recv;
    return __vsf_distbus_transport_try_recv(transport_stream) == size;
}

#endif      // VSF_DISTBUS_TRANSPORT_USE_STREAM
