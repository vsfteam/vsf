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
    ncm->ntb_input_size.wNtbInMaxDataframs = 0;
    ncm->ntb_input_size.reserved = 0;
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

static void __vk_usbd_cdcncm_on_notified(void *param)
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

        vk_usbd_trans_t *trans = &ncm->transact_int_in;
        trans->buffer = (uint8_t *)notifier;
        trans->size = sizeof(*notifier);
        trans->on_finish = __vk_usbd_cdcncm_on_notified;
        vk_usbd_ep_send(ncm->dev, trans);
    } else if (1 == ncm->connect_state) {
        ncm->connect_state++;

        // connected
        vk_netdrv_connect(&ncm->netdrv);
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
                if (request->wValue) {
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

                    vk_usbd_trans_t *trans = &ncm->transact_int_in;
                    trans->ep = ncm->ep.notify;
                    trans->buffer = (uint8_t *)notifier;
                    trans->size = sizeof(*notifier);
                    trans->zlp = false;
                    trans->on_finish = __vk_usbd_cdcncm_on_notified;
                    trans->param = ifs;
                    trans->notify_eda = false;
                    vk_usbd_ep_send(dev, trans);
                } else if (vk_netdrv_is_connected(netdrv)) {
                    vk_netdrv_disconnect(netdrv);
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
    return VSF_ERR_NONE;
}

#endif  // VSF_USE_USB_DEVICE && VSF_USBD_USE_CDCNCM
