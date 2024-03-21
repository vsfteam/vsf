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

#include "component/vsf_component_cfg.h"

#if VSF_USE_BTSTACK == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__

#include "hal/vsf_hal.h"
#include "kernel/vsf_kernel.h"
#include "service/vsf_service.h"

// TODO: check dependency

#include "btstack_config.h"
#include "btstack_debug.h"
#include "hci.h"
#include "hci_transport.h"
#include "bluetooth.h"

/*============================ MACROS ========================================*/

#ifndef HCI_INCOMING_PRE_SIZE
#   define HCI_INCOMING_PRE_SIZE        0
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct hci_transport_h4_param_t {
    bool opened;
    void (*packet_handler)(uint8_t packet_type, uint8_t *packet, uint16_t size);

    struct {
        vsf_mem_stream_t stream;
        vsf_mem_t buffer;
        uint32_t type;        // 32-bit aligned 8-bit data
        bool sending;
    } tx;
    struct {
        vsf_mem_stream_t stream;
        uint8_t buffer[HCI_INCOMING_PRE_SIZE + HCI_INCOMING_PACKET_BUFFER_SIZE];
        enum {
            HCI_RX_IDLE = 0,
            HCI_RX_TYPE,
            HCI_RX_HEADER,
            HCI_RX_DATA,
            HCI_RX_RECEIVED,
        } state;
    } rx;

    vsf_usart_cfg_t usart_cfg;
    vsf_usart_stream_t *usart_stream;

    vsf_eda_t eda;
} hci_transport_h4_param_t;
static hci_transport_h4_param_t __hci_transport_h4_param;

#define HCI_EVT_IN                          (VSF_EVT_USER + 0)
#define HCI_EVT_OUT                         (VSF_EVT_USER + 1)

static void __hci_transport_h4_on_in(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt)
{
    hci_transport_h4_param_t *h4param = (hci_transport_h4_param_t *)param;
    uint8_t *ptr = &h4param->rx.buffer[HCI_INCOMING_PRE_SIZE];
    vsf_mem_t buffer = { 0 };

    if (evt != VSF_STREAM_ON_IN) {
        return;
    }

    if (h4param->opened) {
        if (VSF_STREAM_GET_FREE_SIZE(stream)) {
            return;
        }

        switch (h4param->rx.state) {
        case HCI_RX_IDLE:
        hci_transport_h4_idle:
            h4param->rx.state++;
            buffer.buffer = ptr;
            buffer.size = 1;
            break;
        case HCI_RX_TYPE:
            h4param->rx.state++;
            buffer.buffer = &ptr[1];
            switch (ptr[0])
            {
            case HCI_ACL_DATA_PACKET:
                buffer.size = HCI_ACL_HEADER_SIZE;
                break;
            case HCI_SCO_DATA_PACKET:
                buffer.size = HCI_SCO_HEADER_SIZE;
                break;
            case HCI_EVENT_PACKET:
                buffer.size = HCI_EVENT_HEADER_SIZE;
                break;
            default:
            hci_transport_h4_reset:
                h4param->rx.state = HCI_RX_IDLE;
                goto hci_transport_h4_idle;
            }
            break;
        case HCI_RX_HEADER:
            switch (ptr[0]) {
            case HCI_ACL_DATA_PACKET:
                buffer.size = get_unaligned_le16(&ptr[3]);
                buffer.buffer = &ptr[1 + HCI_ACL_HEADER_SIZE];
                if (HCI_ACL_HEADER_SIZE + buffer.size > HCI_INCOMING_PACKET_BUFFER_SIZE)
                    goto hci_transport_h4_reset;
                break;
            case HCI_SCO_DATA_PACKET:
                buffer.size = ptr[3];
                buffer.buffer = &ptr[1 + HCI_SCO_HEADER_SIZE];
                break;
            case HCI_EVENT_PACKET:
                buffer.size = ptr[2];
                buffer.buffer = &ptr[1 + HCI_EVENT_HEADER_SIZE];
                break;
            }

            h4param->rx.state++;
            if (buffer.size) {
                break;
            }
        case HCI_RX_DATA:
            h4param->rx.state++;
            vsf_eda_post_evt(&h4param->eda, HCI_EVT_IN);
        case HCI_RX_RECEIVED:
            return;
        }
        VSF_STREAM_READ(stream, buffer.buffer, buffer.size);
    }
}

static void __hci_transport_h4_on_out(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt)
{
    hci_transport_h4_param_t *h4param = (hci_transport_h4_param_t *)param;

    if (evt != VSF_STREAM_ON_OUT) {
        return;
    }

    if (h4param->opened) {
        if (VSF_STREAM_GET_DATA_SIZE(stream)) {
            return;
        }
        vsf_eda_post_evt(&h4param->eda, HCI_EVT_OUT);
    }
}

static void __hci_transport_h4_evt_handler(vsf_eda_t *eda, vsf_evt_t evt)
{
    hci_transport_h4_param_t *h4param = container_of(eda, hci_transport_h4_param_t, eda);
    vsf_mem_stream_t *stream;
    uint8_t *ptr;

    switch (evt) {
    case VSF_EVT_INIT: {
            vsf_usart_stream_t *usart_stream = h4param->usart_stream;
            vsf_stream_t *stream;

            stream = usart_stream->stream_tx = (vsf_stream_t *)&h4param->tx.stream;
            stream->op = &vsf_mem_stream_op;
            stream->tx.param = &__hci_transport_h4_param;
            stream->tx.evthandler = __hci_transport_h4_on_out;

            stream = usart_stream->stream_rx = (vsf_stream_t *)&h4param->rx.stream;
            stream->op = &vsf_mem_stream_op;
            stream->rx.param = h4param;
            stream->rx.evthandler = __hci_transport_h4_on_in;

            h4param->usart_cfg.baudrate = 115200;
            vsf_usart_stream_init(usart_stream, &h4param->usart_cfg);
            vsf_stream_connect_tx(usart_stream->stream_tx);
            vsf_stream_connect_rx(usart_stream->stream_rx);
        }
        break;
    case HCI_EVT_IN:
        if (h4param->rx.state == HCI_RX_RECEIVED) {
            stream = &h4param->rx.stream;
            ptr = &h4param->rx.buffer[HCI_INCOMING_PRE_SIZE];
            h4param->packet_handler(ptr[0], &ptr[1], (stream->buffer - ptr) + stream->size - 1);

            h4param->rx.state = HCI_RX_IDLE;
            __hci_transport_h4_on_in((vsf_stream_t *)stream, h4param, VSF_STREAM_ON_IN);
        }
        break;
    case HCI_EVT_OUT:
        stream = &h4param->tx.stream;
        if (stream->size == 1) {
            VSF_STREAM_WRITE(stream, h4param->tx.buffer.buffer, h4param->tx.buffer.size);
        } else {
            const uint8_t event[] = {HCI_EVENT_TRANSPORT_PACKET_SENT, 0};
            __hci_transport_h4_param.tx.sending = false;
            h4param->packet_handler(HCI_EVENT_PACKET, (uint8_t *)event, 2);
        }
        break;
    }
}

static int __hci_transport_h4_open(void)
{
    __hci_transport_h4_param.opened = true;
    __hci_transport_h4_on_in((vsf_stream_t *)&__hci_transport_h4_param.rx.stream,
                                &__hci_transport_h4_param, VSF_STREAM_ON_IN);
    return 0;
}

static int __hci_transport_h4_close(void)
{
    __hci_transport_h4_param.opened = false;
    return 0;
}

static void __hci_transport_h4_register_packet_handler(
        void (*handler)(uint8_t packet_type, uint8_t *packet, uint16_t size))
{
    __hci_transport_h4_param.packet_handler = handler;
}

static int __hci_transport_h4_can_send_packet_now(uint8_t packet_type)
{
    return !__hci_transport_h4_param.tx.sending;
}

static int __hci_transport_h4_send_packet(uint8_t packet_type, uint8_t *packet,
        int size)
{
    if (__hci_transport_h4_param.opened && !__hci_transport_h4_param.tx.sending) {
        vsf_mem_stream_t *stream = &__hci_transport_h4_param.tx.stream;
        vsf_mem_t buffer;

        __hci_transport_h4_param.tx.sending = true;
        __hci_transport_h4_param.tx.buffer.buffer = packet;
        __hci_transport_h4_param.tx.buffer.size = size;
        __hci_transport_h4_param.tx.type = packet_type;

        buffer.buffer = (uint8_t *)&__hci_transport_h4_param.tx.type;
        buffer.size = 1;
        VSF_STREAM_WRITE(stream, buffer.buffer, buffer.size);
        return 0;
    }
    return -1;
}

static int __hci_transport_h4_set_baudrate(uint32_t baudrate)
{
    __hci_transport_h4_param.usart_cfg.baudrate = baudrate;
    vsf_usart_stream_init(__hci_transport_h4_param.usart_stream, &__hci_transport_h4_param.usart_cfg);
    return 0;
}

static void __hci_transport_h4_init(const void *transport_config)
{
    __hci_transport_h4_param.usart_stream = (vsf_usart_stream_t *)transport_config;
#   if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    __hci_transport_h4_param.eda.on_terminate = NULL;
#   endif
    __hci_transport_h4_param.eda.fn.evthandler = __hci_transport_h4_evt_handler;
    vsf_eda_init(&__hci_transport_h4_param.eda);
#   if VSF_KERNEL_CFG_TRACE == ENABLED
    vsf_kernel_trace_eda_info(&__hci_transport_h4_param.eda, "bthci_h4", NULL, 0);
#   endif
}

static const hci_transport_t __hci_transport_h4 =
{
    .name = "H4_VSF",
    .init = __hci_transport_h4_init,
    .open = __hci_transport_h4_open,
    .close = __hci_transport_h4_close,
    .register_packet_handler = __hci_transport_h4_register_packet_handler,
    .can_send_packet_now = __hci_transport_h4_can_send_packet_now,
    .send_packet = __hci_transport_h4_send_packet,
    .set_baudrate = __hci_transport_h4_set_baudrate,
};

const hci_transport_t * hci_transport_h4_instance(
        const btstack_uart_block_t *uart_driver)
{
    return &__hci_transport_h4;
}

#endif      // VSF_USE_BTSTACK
