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

#include "component/vsf_component_cfg.h"

#if VSF_USE_BTSTACK == ENABLED

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

struct hci_transport_h4_param_t {
    bool opened;
    void (*packet_handler)(uint8_t packet_type, uint8_t *packet, uint16_t size);

    struct {
        struct vsf_bufstream_t stream;
        struct vsf_buffer_t buffer;
        uint32_t type;        // 32-bit aligned 8-bit data
        bool sending;
    } tx;
    struct
    {
        struct vsf_bufstream_t stream;
        uint8_t buffer[HCI_INCOMING_PRE_SIZE + HCI_INCOMING_PACKET_BUFFER_SIZE];
        enum
        {
            HCI_RX_IDLE = 0,
            HCI_RX_TYPE,
            HCI_RX_HEADER,
            HCI_RX_DATA,
            HCI_RX_RECEIVED,
        } state;
    } rx;
    struct vsf_usart_stream_t *usart_stream;
    struct vsfsm_t sm;
} static hci_transport_h4_param;

#define HCI_EVT_IN                        (VSFSM_EVT_USER + 0)
#define HCI_EVT_OUT                        (VSFSM_EVT_USER + 1)

static void hci_transport_h4_on_in(void *param)
{
    struct hci_transport_h4_param_t *h4param =
            (struct hci_transport_h4_param_t *)param;
    struct vsf_bufstream_t *bufstream = &h4param->rx.stream;
    uint8_t *ptr = &h4param->rx.buffer[HCI_INCOMING_PRE_SIZE];
    struct vsf_buffer_t buffer;

    if (h4param->opened)
    {
        if (VSF_STREAM_GET_FREE_SIZE(bufstream))
            return;

        switch (h4param->rx.state)
        {
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
            switch (ptr[0])
            {
            case HCI_ACL_DATA_PACKET:
                buffer.size = GET_LE_U16(&ptr[3]);
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
            if (buffer.size)
                break;
        case HCI_RX_DATA:
            h4param->rx.state++;
            vsfsm_post_evt_pending(&h4param->sm, HCI_EVT_IN);
        case HCI_RX_RECEIVED:
            return;
        }
        VSF_STREAM_READ(bufstream, &buffer);
    }
}

static void hci_transport_h4_on_out(void *param)
{
    struct hci_transport_h4_param_t *h4param =
            (struct hci_transport_h4_param_t *)param;
    struct vsf_bufstream_t *bufstream = &h4param->tx.stream;

    if (h4param->opened)
    {
        if (VSF_STREAM_GET_DATA_SIZE(bufstream))
            return;
        vsfsm_post_evt_pending(&h4param->sm, HCI_EVT_OUT);
    }
}

static struct vsfsm_state_t *hci_transport_h4_evt_handler(struct vsfsm_t *sm,
        vsfsm_evt_t evt)
{
    struct hci_transport_h4_param_t *h4param =
            (struct hci_transport_h4_param_t *)sm->user_data;
    struct vsf_bufstream_t *bufstream;
    uint8_t *ptr;

    switch (evt)
    {
    case VSFSM_EVT_INIT:
        {
            struct vsf_usart_stream_t *usart_stream = h4param->usart_stream;
            struct vsf_stream_t *stream;

            h4param->tx.stream.mem.read = true;
            stream = usart_stream->stream_tx = &h4param->tx.stream.stream;
            stream->op = &vsf_bufstream_op;
            stream->callback_tx.param = &hci_transport_h4_param;
            stream->callback_tx.on_inout = hci_transport_h4_on_out;

            h4param->rx.stream.mem.read = false;
            stream = usart_stream->stream_rx = &h4param->rx.stream.stream;
            stream->op = &vsf_bufstream_op;
            stream->callback_rx.param = h4param;
            stream->callback_rx.on_inout = hci_transport_h4_on_in;

            vsf_usart_stream_init(usart_stream);
            vsf_stream_connect_tx(usart_stream->stream_tx);
            vsf_stream_connect_rx(usart_stream->stream_rx);
        }
        break;
    case HCI_EVT_IN:
        if (h4param->rx.state == HCI_RX_RECEIVED)
        {
            bufstream = &h4param->rx.stream;
            ptr = &h4param->rx.buffer[HCI_INCOMING_PRE_SIZE];
            h4param->packet_handler(ptr[0], &ptr[1],
                (bufstream->mem.buffer.buffer - ptr) + bufstream->mem.buffer.size - 1);

            h4param->rx.state = HCI_RX_IDLE;
            hci_transport_h4_on_in(h4param);
        }
        break;
    case HCI_EVT_OUT:
        bufstream = &h4param->tx.stream;
        if (bufstream->mem.buffer.size == 1)
            VSF_STREAM_WRITE(bufstream, &h4param->tx.buffer);
        else
        {
            const uint8_t event[] = {HCI_EVENT_TRANSPORT_PACKET_SENT, 0};
            hci_transport_h4_param.tx.sending = false;
            h4param->packet_handler(HCI_EVENT_PACKET, (uint8_t *)event, 2);
        }
        break;
    }
    return NULL;
}

static int hci_transport_h4_open(void)
{
    hci_transport_h4_param.opened = true;
    hci_transport_h4_on_in(&hci_transport_h4_param);
    return 0;
}

static int hci_transport_h4_close(void)
{
    hci_transport_h4_param.opened = false;
    return 0;
}

static void hci_transport_h4_register_packet_handler(
        void (*handler)(uint8_t packet_type, uint8_t *packet, uint16_t size))
{
    hci_transport_h4_param.packet_handler = handler;
}

static int hci_transport_h4_can_send_packet_now(uint8_t packet_type)
{
    return !hci_transport_h4_param.tx.sending;
}

static int hci_transport_h4_send_packet(uint8_t packet_type, uint8_t *packet,
        int size)
{
    if (hci_transport_h4_param.opened && !hci_transport_h4_param.tx.sending)
    {
        struct vsf_bufstream_t *stream = &hci_transport_h4_param.tx.stream;
        struct vsf_buffer_t buffer;

        hci_transport_h4_param.tx.sending = true;
        hci_transport_h4_param.tx.buffer.buffer = packet;
        hci_transport_h4_param.tx.buffer.size = size;
        hci_transport_h4_param.tx.type = packet_type;

        buffer.buffer = (uint8_t *)&hci_transport_h4_param.tx.type;
        buffer.size = 1;
        VSF_STREAM_WRITE(stream, &buffer);
        return 0;
    }
    return -1;
}

static int hci_transport_h4_set_baudrate(uint32_t baudrate)
{
    hci_transport_h4_param.usart_stream->baudrate = baudrate;
    vsf_usart_stream_config(hci_transport_h4_param.usart_stream);
    return 0;
}

static void hci_transport_h4_init(const void *transport_config)
{
    hci_transport_h4_param.usart_stream =
            (struct vsf_usart_stream_t *)transport_config;
    hci_transport_h4_param.sm.user_data = &hci_transport_h4_param;
    hci_transport_h4_param.sm.init_state.evt_handler =
            hci_transport_h4_evt_handler;
    vsfsm_init(&hci_transport_h4_param.sm);
}

static const hci_transport_t hci_transport_h4 =
{
    .name = "H4_VSF",
    .init = hci_transport_h4_init,
    .open = hci_transport_h4_open,
    .close = hci_transport_h4_close,
    .register_packet_handler = hci_transport_h4_register_packet_handler,
    .can_send_packet_now = hci_transport_h4_can_send_packet_now,
    .send_packet = hci_transport_h4_send_packet,
    .set_baudrate = hci_transport_h4_set_baudrate,
};

const hci_transport_t * hci_transport_h4_instance(
        const btstack_uart_block_t *uart_driver)
{
    return &hci_transport_h4;
}

#endif      // VSF_USE_BTSTACK
