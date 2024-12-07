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

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_CDCNCM == ENABLED && VSF_USBD_CFG_RAW_MODE != ENABLED

#define __VSF_USBD_CLASS_INHERIT__
#define __VSF_USBD_CDC_CLASS_INHERIT__
#define __VSF_USBD_CDCNCM_CLASS_IMPLEMENT
#define __VSF_NETDRV_CLASS_INHERIT_NETLINK__

#include "kernel/vsf_kernel.h"
#include "../../vsf_usbd.h"
#include "./vsf_usbd_CDCNCM.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vk_usbd_cdcncm_data_init(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
static vsf_err_t __vk_usbd_cdcncm_control_init(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
static vsf_err_t __vk_usbd_cdcncm_request_prepare(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
static vsf_err_t __vk_usbd_cdcncm_request_process(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);

static vsf_err_t __vk_usbd_cdcncm_netlink_init(vk_netdrv_t *netdrv);
static vsf_err_t __vk_usbd_cdcncm_netlink_fini(vk_netdrv_t *netdrv);
static void * __vk_usbd_cdcncm_netlink_can_output(vk_netdrv_t *netdrv);
static vsf_err_t __vk_usbd_cdcncm_netlink_output(vk_netdrv_t *netdrv, void *slot, void *netbuf);

static void __vk_usbd_cdcncm_netdrv_thread(void *param);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbd_class_op_t vk_usbd_cdcncm_control =
{
    .init = __vk_usbd_cdcncm_control_init,
    .request_prepare = __vk_usbd_cdcncm_request_prepare,
    .request_process = __vk_usbd_cdcncm_request_process,
};

const vk_usbd_class_op_t vk_usbd_cdcncm_data =
{
    .init = __vk_usbd_cdcncm_data_init,
    .request_prepare = __vk_usbd_cdcncm_request_prepare,
    .request_process = __vk_usbd_cdcncm_request_process,
};

/*============================ LOCAL VARIABLES ===============================*/

static const usb_cdcncm_ntb_param_t __vsf_usbd_cdcncm_ntb_param = {
    .wLength = __constant_cpu_to_le16(sizeof(usb_cdcncm_ntb_param_t)),
#if VSF_USBD_CDCNCM_SUPPORT_NTB32 == ENABLED
    .bmNtbFormatsSupported = __constant_cpu_to_le16(3),
#else
    .bmNtbFormatsSupported = __constant_cpu_to_le16(1),
#endif
    .dwNtbInMaxSize = __constant_cpu_to_le32(VSF_USBD_CDCNCM_CFG_MAX_NTB_IN_SIZE),
    .wNdpInDivisor = __constant_cpu_to_le16(4),
    .wNdpInPayloadRemainder = __constant_cpu_to_le16(0),
    .wNdpInAlignment = __constant_cpu_to_le16(4),
    .dwNtbOutMaxSize = __constant_cpu_to_le32(VSF_USBD_CDCNCM_CFG_MAX_NTB_OUT_SIZE),
    .wNdpOutDivisor = __constant_cpu_to_le16(4),
    .wNdpOutPayloadRemainder = __constant_cpu_to_le16(0),
    .wNdpOutAlignment = __constant_cpu_to_le16(4),
    .wNtbOutMaxDatagrams = __constant_cpu_to_le16(0),
};

static const struct vk_netlink_op_t __vk_usbd_cdcncm_netlink_op =
{
    .init       = __vk_usbd_cdcncm_netlink_init,
    .fini       = __vk_usbd_cdcncm_netlink_fini,
    .can_output = __vk_usbd_cdcncm_netlink_can_output,
    .output     = __vk_usbd_cdcncm_netlink_output,
};

/*============================ IMPLEMENTATION ================================*/

static vsf_err_t __vk_usbd_cdcncm_data_init(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    return vk_usbd_cdc_data.init(dev, ifs);
}

static vsf_err_t __vk_usbd_cdcncm_control_init(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_usbd_cdcncm_t *ncm = ifs->class_param;
    ncm->dev = dev;
    ncm->ntb_format = __vsf_usbd_cdcncm_ntb_param.bmNtbFormatsSupported;
    ncm->ntb_input_size.dwNtbInMaxSize = __vsf_usbd_cdcncm_ntb_param.dwNtbInMaxSize;
    ncm->ntb_input_size.wNtbInMaxDataframes = 0;
    ncm->ntb_input_size.reserved = 0;

    for (uint_fast8_t i = 0; i < TCPIP_ETH_ADDRLEN; i++) {
        ncm->netdrv.macaddr.addr_buf[i] =
                (vsf_usb_hex_to_bin(ncm->str_mac[(i << 1) + 0]) << 4)
            |   (vsf_usb_hex_to_bin(ncm->str_mac[(i << 1) + 1]) << 0);
    }
    ncm->netdrv.macaddr.size = TCPIP_ETH_ADDRLEN;
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_usbd_cdcncm_request_prepare(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_usbd_cdcncm_t *ncm = ifs->class_param;
    vk_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    uint8_t *buffer = NULL;
    uint_fast32_t size = 0;

    switch (request->bRequest) {
    case USB_CDCNCM_REQ_GET_NTB_PARAMETERS:
        buffer = (uint8_t *)&__vsf_usbd_cdcncm_ntb_param;
        size = sizeof(__vsf_usbd_cdcncm_ntb_param);
        break;
    case USB_CDCNCM_REQ_SET_NTB_FORMAT:
    case USB_CDCNCM_REQ_GET_NTB_FORMAT:
        buffer = (uint8_t *)&ncm->ntb_format;
        size = sizeof(ncm->ntb_format);
        break;
    case USB_CDCNCM_REQ_GET_NTB_INPUT_SIZE:
    case USB_CDCNCM_REQ_SET_NTB_INPUT_SIZE:
        buffer = (uint8_t *)&ncm->ntb_input_size;
        size = sizeof(ncm->ntb_input_size);
        break;
    case USB_CDCNCM_REQ_GET_CRC_MODE:
    case USB_CDCNCM_REQ_SET_CRC_MODE:
        buffer = (uint8_t *)&ncm->crc_mode;
        size = sizeof(ncm->crc_mode);
        break;
    case USB_CDCNCM_REQ_SET_ETHERNET_PACKET_FILTER:
        buffer = NULL;
        size = 0;
        break;
    case USB_CDCNCM_REQ_GET_MAX_DATAGRAM_SIZE:
    case USB_CDCNCM_REQ_SET_MAX_DATAGRAM_SIZE:
        break;
    default:
        return vk_usbd_cdc_control.request_prepare(dev, ifs);
    }

    ctrl_handler->trans.use_as__vsf_mem_t.buffer = buffer;
    ctrl_handler->trans.use_as__vsf_mem_t.size = size;
    return VSF_ERR_NONE;
}

static void __vk_usbd_cdcncm_on_bulkout_transfer_finished(void *param)
{
    vk_usbd_cdcncm_t *ncm = (vk_usbd_cdcncm_t *)param;
    vk_usbd_trans_t *trans = &ncm->transact_out;

    ncm->cur_rx_size = sizeof(ncm->ntb_out_buffer) - trans->size;
    vsf_eda_sem_post(&ncm->sem);
}

static void __vk_usbd_cdcncm_on_bulkin_transfer_finished(void *param)
{
    vk_usbd_cdcncm_t *ncm = (vk_usbd_cdcncm_t *)param;
    ncm->is_tx_busy = false;
    vk_netdrv_on_netlink_outputted(&ncm->netdrv, VSF_ERR_NONE);
}

static void __vk_usbd_cdcncm_on_notify_transfer_finished(void *param)
{
    vk_usbd_ifs_t *ifs = (vk_usbd_ifs_t *)param;
    vk_usbd_cdcncm_t *ncm = (vk_usbd_cdcncm_t *)ifs->class_param;

    if (0 == ncm->connect_state) {
        ncm->connect_state++;

        usb_cdcecm_notification_network_connection_t *notifier =
            (usb_cdcecm_notification_network_connection_t *)ncm->ntb_in_buffer;
        notifier->bRequestType = USB_DIR_IN | USB_TYPE_CLASS |USB_RECIP_INTERFACE;
        notifier->bRequest = USB_CDCECM_NOTIFICATION_NETWORK_CONNECTION;
        notifier->wValue = cpu_to_le16(1);
        notifier->wIndex = cpu_to_le16(vk_usbd_get_ifs_no(ncm->dev, ifs) - 1);
        notifier->wLength = cpu_to_le16(0);

        vk_usbd_trans_t *trans = &ncm->transact_in;
        trans->buffer = (uint8_t *)notifier;
        trans->size = sizeof(*notifier);
        trans->on_finish = __vk_usbd_cdcncm_on_notify_transfer_finished;
        vk_usbd_ep_send(ncm->dev, trans);
    } else if (1 == ncm->connect_state) {
        ncm->connect_state++;

        // connected
        vsf_trace_info("ncm_event: NETWORK_CONNECTION Connected" VSF_TRACE_CFG_LINEEND);

        vk_netdrv_t *netdrv = &ncm->netdrv;
        vk_netdrv_prepare(netdrv);
        vsf_eda_sem_init(&ncm->sem);
        ncm->thread = vk_netdrv_thread(netdrv, __vk_usbd_cdcncm_netdrv_thread, ncm);
        VSF_USB_ASSERT(ncm->thread != NULL);
    }
}

static vsf_err_t __vk_usbd_cdcncm_request_process(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_usbd_cdcncm_t *ncm = ifs->class_param;
    vk_netdrv_t *netdrv = &ncm->netdrv;
    vk_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;

    uint_fast8_t type = request->bRequestType & USB_TYPE_MASK;
    if (USB_TYPE_STANDARD == type) {
        uint8_t recip = request->bRequestType & USB_RECIP_MASK;
        if (    (recip == USB_RECIP_INTERFACE)
            &&  (USB_REQ_SET_INTERFACE == request->bRequest)) {

            bool is_connected = vk_netdrv_is_connected(netdrv);
            if (is_connected != !!request->wValue) {
                if (!is_connected) {
                    // start connect by issuing ConnectionSpeedChange and NetworkConnection notifications
                    ncm->connect_state = 0;

                    usb_cdcecm_notification_speed_change_t *notifier =
                        (usb_cdcecm_notification_speed_change_t *)ncm->ntb_in_buffer;
                    notifier->bRequestType = USB_DIR_IN | USB_TYPE_CLASS |USB_RECIP_INTERFACE;
                    notifier->bRequest = USB_CDCECM_NOTIFICATION_CONNECTION_SPEED_CHANGE;
                    notifier->wValue = cpu_to_le16(0);
                    notifier->wIndex = cpu_to_le16(vk_usbd_get_ifs_no(dev, ifs) - 1);
                    notifier->wLength = cpu_to_le16(8);
                    notifier->down = notifier->up = 100 * 1000 * 1000;

                    vk_usbd_trans_t *trans = &ncm->transact_in;
                    trans->ep = ncm->ep.notify;
                    trans->buffer = (uint8_t *)notifier;
                    trans->size = sizeof(*notifier);
                    trans->zlp = false;
                    trans->on_finish = __vk_usbd_cdcncm_on_notify_transfer_finished;
                    trans->param = ifs;
                    trans->notify_eda = false;
                    vk_usbd_ep_send(dev, trans);
                } else {
                    vsf_eda_sem_post(&ncm->sem);
                }
            }
        }
        return VSF_ERR_NONE;
    }

    switch (request->bRequest) {
    case USB_CDCNCM_REQ_GET_NTB_PARAMETERS:
        ncm->netdrv.netlink.op = &__vk_usbd_cdcncm_netlink_op;
        vsf_pnp_on_netdrv_new(&ncm->netdrv);
        break;
    case USB_CDCNCM_REQ_GET_NTB_FORMAT:
    case USB_CDCNCM_REQ_SET_NTB_FORMAT:
    case USB_CDCNCM_REQ_GET_NTB_INPUT_SIZE:
    case USB_CDCNCM_REQ_SET_NTB_INPUT_SIZE:
    case USB_CDCNCM_REQ_GET_CRC_MODE:
    case USB_CDCNCM_REQ_SET_CRC_MODE:
    case USB_CDCNCM_REQ_SET_ETHERNET_PACKET_FILTER:
        break;
    default:
        return vk_usbd_cdc_control.request_process(dev, ifs);
    }
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_usbd_cdcncm_netlink_init(vk_netdrv_t *netdrv)
{
    vk_usbd_cdcncm_t *ncm = vsf_container_of(netdrv, vk_usbd_cdcncm_t, netdrv);

    netdrv->mtu = ncm->max_datagram_size - TCPIP_ETH_HEADSIZE;
    netdrv->mac_header_size = TCPIP_ETH_HEADSIZE;
    netdrv->hwtype = TCPIP_ETH_HWTYPE;
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_usbd_cdcncm_netlink_fini(vk_netdrv_t *netdrv)
{
    vsf_pnp_on_netdrv_del(netdrv);
    return VSF_ERR_NONE;
}

static void * __vk_usbd_cdcncm_netlink_can_output(vk_netdrv_t *netdrv)
{
    vk_usbd_cdcncm_t *ncm = vsf_container_of(netdrv, vk_usbd_cdcncm_t, netdrv);
    return (void *)(uintptr_t)!ncm->is_tx_busy;
}

static vsf_err_t __vk_usbd_cdcncm_netlink_output(vk_netdrv_t *netdrv, void *slot, void *netbuf)
{
    vk_usbd_cdcncm_t *ncm = vsf_container_of(netdrv, vk_usbd_cdcncm_t, netdrv);
    vk_usbd_trans_t *trans = &ncm->transact_in;
    uint16_t datagram_offset, datagram_size = 0;

    usb_cdcncm_nth_t *nth = (usb_cdcncm_nth_t *)ncm->ntb_in_buffer;
    nth->nth16.dwSignature = __constant_le32_to_cpu(USB_CDCNCM_NTH16_SIG);
    nth->nth16.wHeaderLength = __constant_le16_to_cpu(12);
    datagram_offset = ncm->seq++;
    nth->nth16.wSequence = le16_to_cpu(datagram_offset);
    nth->nth16.wNdpIndex = le16_to_cpu(12);

    usb_cdcncm_ndp_t *ndp = (usb_cdcncm_ndp_t *)((uintptr_t)ncm->ntb_in_buffer + 12);
    ndp->ndp16.dwSignature = __constant_le32_to_cpu(USB_CDCNCM_NDP16_SIG_NOCRC);
    ndp->ndp16.wLength = __constant_le16_to_cpu(16);
    ndp->ndp16.dwNextNdpIndex = 0;
    // segments(offset, size): NTB(0, 12) NDP(12, 16) padding(28, 2) ETHERNET_HEADER(30, 14) + IP(44)
    // IP MUST be aligned to 4-byte
    datagram_offset = 12 + 16 + 2;
    ndp->ndp16.indexes[0].wDatagramIndex = le16_to_cpu(datagram_offset);
    ndp->ndp16.indexes[1].wDatagramIndex = 0;
    ndp->ndp16.indexes[1].wDatagramLength = 0;

    vsf_mem_t mem;
    uint8_t *ptr = (uint8_t *)ncm->ntb_in_buffer + datagram_offset;
    void *netbuf_tmp = netbuf;
    while (netbuf_tmp != NULL) {
        netbuf_tmp = vk_netdrv_read_buf(netdrv, netbuf_tmp, &mem);
        memcpy(ptr, mem.buffer, mem.size);
        ptr += mem.size;
        datagram_size += mem.size;
    }

    datagram_offset += datagram_size;
    nth->nth16.wBlockLength = le16_to_cpu(datagram_offset);
    ndp->ndp16.indexes[0].wDatagramLength = le16_to_cpu(datagram_size);

    trans->size = datagram_offset;
    vk_usbd_ep_send(ncm->dev, trans);

    vk_netdrv_on_netbuf_outputted(netdrv, netbuf);
    return VSF_ERR_NONE;
}

static void __vk_usbd_cdcncm_netdrv_thread(void *param)
{
    vk_usbd_cdcncm_t *ncm = param;
    vk_netdrv_t *netdrv = &ncm->netdrv;
    vsf_sync_reason_t reason;
    vsf_protect_t orig;
    vk_usbd_trans_t *trans;

    trans = &ncm->transact_in;
    trans->buffer = (uint8_t *)ncm->ntb_in_buffer;
    trans->ep = ncm->ep.in;
    trans->zlp = true;
    trans->param = ncm;
    trans->on_finish = __vk_usbd_cdcncm_on_bulkin_transfer_finished;

    trans = &ncm->transact_out;
    trans->buffer = (uint8_t *)ncm->ntb_out_buffer;
    trans->ep = ncm->ep.out;
    trans->zlp = true;
    trans->on_finish = __vk_usbd_cdcncm_on_bulkout_transfer_finished;
    trans->param = ncm;
    trans->notify_eda = false;

    vk_netdrv_connect(netdrv);

    usb_cdcncm_nth_t *nth;
    usb_cdcncm_ndp_t *ndp;
    uint32_t cur_rx_size, offset, size, cur_size;
    uint8_t *cur_ptr;
    bool is_32bit;
    union {
        uint16_t *ptr16;
        uint32_t *ptr32;
    } datagrams;
    void *netbuf, *netbuf_cur;
    vsf_mem_t mem;

    while (true) {
        if (vk_netdrv_is_connected(netdrv)) {
            trans->size = sizeof(ncm->ntb_out_buffer);
            vk_usbd_ep_recv(ncm->dev, trans);
        }

        reason = vsf_thread_sem_pend(&ncm->sem, -1);
        VSF_USB_ASSERT(VSF_SYNC_GET == reason);

        cur_rx_size = ncm->cur_rx_size;
        ncm->cur_rx_size = 0;
        if (!cur_rx_size) {
            vk_netdrv_disconnect(netdrv);
            ncm->thread = NULL;
            break;
        }

        nth = (usb_cdcncm_nth_t *)ncm->ntb_out_buffer;
        if (nth->dwSignature == __constant_le32_to_cpu(USB_CDCNCM_NTH16_SIG)) {
            offset = le16_to_cpu(nth->nth16.wNdpIndex);
            ndp = (usb_cdcncm_ndp_t *)((uint8_t *)ncm->ntb_out_buffer + offset);
            if (    (offset < 12) || (offset & 3)
                ||  (le16_to_cpu(nth->nth16.wBlockLength) != cur_rx_size)) {
                continue;
            }
            is_32bit = false;
        } else {
            offset = le32_to_cpu(nth->nth32.dwNdpIndex);
            ndp = (usb_cdcncm_ndp_t *)((uint8_t *)ncm->ntb_out_buffer + offset);
            if (    (offset < 16) || (offset & 3)
                ||  (le32_to_cpu(nth->nth32.dwBlockLength) != cur_rx_size)) {
                continue;
            }
            is_32bit = true;
        }
        if (is_32bit) {
            datagrams.ptr32 = (uint32_t *)ndp->ndp32.indexes;
            while ((offset = *datagrams.ptr32++) != 0) {
                offset = le32_to_cpu(offset);
                cur_ptr = (uint8_t *)ncm->ntb_out_buffer + offset;
                size = *datagrams.ptr32;
                size = le32_to_cpu(size);

                netbuf_cur = netbuf = vk_netdrv_alloc_buf(netdrv);
                if (NULL == netbuf) {
                    vsf_trace_error("ncm: fail to allocate netbuf" VSF_TRACE_CFG_LINEEND);
                    break;
                }

                do {
                    netbuf_cur = vk_netdrv_read_buf(netdrv, netbuf_cur, &mem);
                    cur_size = vsf_min(mem.size, size);
                    memcpy(mem.buffer, cur_ptr, cur_size);
                    size -= cur_size;
                    cur_ptr += cur_size;
                } while ((netbuf_cur != NULL) && (size > 0));

                size = *datagrams.ptr32++;
                vk_netdrv_on_inputted(netdrv, netbuf, size);
            }
        } else {
            datagrams.ptr16 = (uint16_t *)ndp->ndp16.indexes;
            while ((offset = *datagrams.ptr16++) != 0) {
                offset = le32_to_cpu(offset);
                cur_ptr = (uint8_t *)ncm->ntb_out_buffer + offset;
                size = *datagrams.ptr16;
                size = le16_to_cpu(size);

                netbuf_cur = netbuf = vk_netdrv_alloc_buf(netdrv);
                if (NULL == netbuf) {
                    vsf_trace_error("ncm: fail to allocate netbuf" VSF_TRACE_CFG_LINEEND);
                    break;
                }

                do {
                    netbuf_cur = vk_netdrv_read_buf(netdrv, netbuf_cur, &mem);
                    cur_size = vsf_min(mem.size, size);
                    memcpy(mem.buffer, cur_ptr, cur_size);
                    size -= cur_size;
                    cur_ptr += cur_size;
                } while ((netbuf_cur != NULL) && (size > 0));

                size = *datagrams.ptr16++;
                vk_netdrv_on_inputted(netdrv, netbuf, size);
            }
        }
    }
}

#endif  // VSF_USE_USB_DEVICE && VSF_USBD_USE_CDCNCM
