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

/*============================ INCLUDES ======================================*/

#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#define __VSF_USART_STREAM_CLASS_IMPLEMENT
#include "./vsf_usart_stream.h"

#if VSF_HAL_USE_USART == ENABLED && VSF_USE_SIMPLE_STREAM == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __vsf_usart_stream_tx(vsf_usart_stream_t *usart_stream, vsf_protect_t orig)
{
    uint8_t *buf;

    usart_stream->tx.size = vsf_stream_get_rbuf(usart_stream->stream_tx, &buf);
    vsf_unprotect_int(orig);
    if (usart_stream->tx.size > 0) {
        vsf_usart_request_tx(usart_stream->usart, buf, usart_stream->tx.size);
    }
}

static void __vsf_usart_stream_evthandler(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt)
{
    vsf_usart_stream_t *usart_stream = param;
    vsf_protect_t orig;

    switch (evt) {
    case VSF_STREAM_ON_CONNECT:
    case VSF_STREAM_ON_IN:
        orig = vsf_protect_int();
        if (usart_stream->tx.size > 0) {
            vsf_unprotect_int(orig);
            break;
        }
        __vsf_usart_stream_tx(usart_stream, orig);
        break;
    default:
        VSF_HAL_ASSERT(false);
        break;
    }
}

static void vsf_usart_stream_isrhandler(void *target, vsf_usart_t *usart, vsf_usart_irq_mask_t irq_mask)
{
    vsf_usart_stream_t *usart_stream = target;

    if (irq_mask & VSF_USART_IRQ_MASK_RX) {
        uint32_t size, total_size, cur_size;
        uint8_t *buffer;
        uint32_t bufsize;

        while (true) {
            total_size = 0;
            bufsize = vsf_stream_get_wbuf(usart_stream->stream_rx, &buffer);
            while (bufsize > 0) {
                size = vsf_usart_rxfifo_get_data_count(usart_stream->usart);
                if (0 == size) {
                    break;
                }

                size = vsf_min(bufsize, size);
                cur_size = vsf_usart_rxfifo_read(usart_stream->usart, buffer, size);
                total_size += cur_size;
                buffer += cur_size;
                bufsize -= cur_size;
            }

            if (0 == total_size) {
                break;
            }
            vsf_stream_write(usart_stream->stream_rx, NULL, total_size);
        }
    } 
    if (irq_mask & VSF_USART_IRQ_MASK_TX_CPL) {
        VSF_HAL_ASSERT(usart_stream->tx.size > 0);
        vsf_stream_read(usart_stream->stream_tx, NULL, usart_stream->tx.size);
        __vsf_usart_stream_tx(usart_stream, vsf_protect_int());
    }
    if ((irq_mask & VSF_USART_IRQ_MASK_ERR) && (usart_stream->on_error)) {
        usart_stream->on_error(usart_stream, irq_mask);
    }
}

vsf_err_t vsf_usart_stream_init(vsf_usart_stream_t *usart_stream, vsf_usart_cfg_t *cfg)
{
    VSF_HAL_ASSERT((usart_stream != NULL) && (cfg != NULL));
    VSF_HAL_ASSERT((usart_stream->usart != NULL));
    VSF_HAL_ASSERT((usart_stream->stream_tx != NULL));
    VSF_HAL_ASSERT((usart_stream->stream_rx != NULL));

    usart_stream->tx.size = 0;

    cfg->isr.handler_fn = vsf_usart_stream_isrhandler;
    cfg->isr.target_ptr = usart_stream;
    vsf_usart_init(usart_stream->usart, cfg);
    vsf_usart_irq_enable(usart_stream->usart, VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_TX_CPL | VSF_USART_IRQ_MASK_ERR);
    vsf_usart_enable(usart_stream->usart);

    VSF_STREAM_CONNECT_TX(usart_stream->stream_rx);

    usart_stream->stream_tx->rx.param = usart_stream;
    usart_stream->stream_tx->rx.evthandler = __vsf_usart_stream_evthandler;
    VSF_STREAM_CONNECT_RX(usart_stream->stream_tx);
    return VSF_ERR_NONE;
}

#endif      // VSF_HAL_USE_USART && VSF_USE_SIMPLE_STREAM
