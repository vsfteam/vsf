/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

/*============================ INCLUDES ======================================*/
#define __VSF_STREAM_USART_CLASS_IMPLEMENT
#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#include "./vsf_stream_usart.h"

#if VSF_USE_STREAM_HAL == ENABLED && VSF_USE_STREAM_USART == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_STREAM_USART_CFG_TRACE_STREAM_FULL
#   define VSF_STREAM_USART_CFG_TRACE_STREAM_FULL       ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __vsf_stream_usart_stream_evthandler(void *param, vsf_stream_evt_t evt)
{
    vsf_stream_usart_t *stream_usart = param;
    vsf_protect_t orig;

    switch (evt) {
    case VSF_STREAM_ON_IN:
        orig = vsf_protect_int();
        if (!stream_usart->is_txing) {
            stream_usart->cur_tx_len = vsf_stream_get_rbuf(stream_usart->stream_tx, &stream_usart->cur_tx_buf);
            if (stream_usart->cur_tx_len > 0) {
                stream_usart->is_txing = true;
                vsf_unprotect_int(orig);

                vsf_usart_request_tx(stream_usart->usart, stream_usart->cur_tx_buf, stream_usart->cur_tx_len);
            } else {
                stream_usart->is_txing = false;
                vsf_unprotect_int(orig);
            }
        } else {
            vsf_unprotect_int(orig);
        }
        break;
    case VSF_STREAM_ON_OUT:
        orig = vsf_protect_int();
        if (!stream_usart->is_rxing) {
            if (vsf_stream_get_free_size(stream_usart->stream_rx) > 0) {
                stream_usart->is_rxing = true;
                vsf_unprotect_int(orig);

                vsf_usart_request_rx(stream_usart->usart, &stream_usart->rxbuf, 1);
            } else {
                stream_usart->is_rxing = false;
                vsf_unprotect_int(orig);

#if VSF_STREAM_USART_CFG_TRACE_STREAM_FULL == ENABLED
                vsf_trace_warning("usart rx stream full" VSF_TRACE_CFG_LINEEND);
#endif
            }
        } else {
            vsf_unprotect_int(orig);
        }
        break;
    }
}

void vsf_stream_usart_irq(void *param, vsf_usart_t *usart, em_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(param != NULL);
    vsf_stream_usart_t *stream_usart = param;

    if (irq_mask & USART_IRQ_MASK_RX_CPL) {
        vsf_stream_write(stream_usart->stream_rx, &stream_usart->rxbuf, 1);
        stream_usart->is_rxing = false;
        __vsf_stream_usart_stream_evthandler(stream_usart, VSF_STREAM_ON_OUT);
    }
    if (irq_mask & USART_IRQ_MASK_TX_CPL) {
        vsf_stream_read(stream_usart->stream_tx, NULL, stream_usart->cur_tx_len);
        stream_usart->is_txing = false;
        __vsf_stream_usart_stream_evthandler(stream_usart, VSF_STREAM_ON_IN);
    }
}

void vsf_stream_usart_init(vsf_stream_usart_t *stream_usart)
{
    vsf_stream_t *stream = stream_usart->stream_tx;

    stream_usart->is_txing = false;
    stream_usart->is_rxing = false;
    if (stream != NULL) {
        stream->rx.param = stream_usart;
        stream->rx.evthandler = __vsf_stream_usart_stream_evthandler;
        vsf_stream_connect_rx(stream);
        vsf_usart_irq_enable(stream_usart->usart, USART_IRQ_MASK_TX_CPL);
        __vsf_stream_usart_stream_evthandler(stream_usart, VSF_STREAM_ON_IN);
    }

    stream = stream_usart->stream_rx;
    if (stream != NULL) {
        stream->tx.param = stream_usart;
        stream->tx.evthandler = __vsf_stream_usart_stream_evthandler;
        vsf_stream_connect_tx(stream);
        vsf_usart_irq_enable(stream_usart->usart, USART_IRQ_MASK_RX_CPL);
        __vsf_stream_usart_stream_evthandler(stream_usart, VSF_STREAM_ON_OUT);
    }
}

void vsf_stream_usart_fini(vsf_stream_usart_t *stream_usart)
{
    vsf_stream_t *stream = stream_usart->stream_tx;
    vsf_err_t err;

    if (stream != NULL) {
        vsf_usart_irq_disable(stream_usart->usart, USART_IRQ_MASK_TX_CPL);
        err = vsf_usart_cancel_tx(stream_usart->usart);
        VSF_HAL_ASSERT(err == VSF_ERR_NONE);
        vsf_stream_disconnect_rx(stream);
    }

    stream = stream_usart->stream_rx;
    if (stream != NULL) {
        vsf_usart_irq_disable(stream_usart->usart, USART_IRQ_MASK_RX_CPL);
        err = vsf_usart_cancel_rx(stream_usart->usart);
        VSF_HAL_ASSERT(err == VSF_ERR_NONE);
        vsf_stream_disconnect_tx(stream);
    }
}

#endif
/* EOF */
