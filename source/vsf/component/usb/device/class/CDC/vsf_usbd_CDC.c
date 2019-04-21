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

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED

#define VSF_USBD_INHERIT
#define VSF_USBD_CDC_IMPLEMENT
#include "vsf.h"


/*============================ MACROS ========================================*/
#undef this
#define this        (*obj)
/*============================ MACROFIED FUNCTIONS ===========================*/


/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static vsf_err_t vsf_usbd_CDC_data_init(vsf_usbd_dev_t *dev, vsf_usbd_ifs_t *ifs)
{
    vsf_usbd_CDC_t *obj = (vsf_usbd_CDC_t *)ifs->class_param;

    ASSERT(obj != NULL);
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
    this.stream.tx.dev  = dev;
    this.stream.tx.ep   = this.ep.in;
    this.stream.tx.zlp  = true;
    this.stream.rx.dev  = dev;
    this.stream.rx.ep   = this.ep.out;
    this.stream.rx.zlp  = false;
#elif VSF_USE_SERVICE_STREAM == ENABLED

    
    vsf_usbd_ep_stream_connect_dev(&this.stream.use_as__vsf_usbd_ep_stream_t, dev);
    /*
    this.stream.dev = dev;
    this.stream.rx_trans.ep = this.ep.out;
    this.stream.rx_trans.zlp = false;
    this.stream.tx_trans.ep = this.ep.in;
    this.stream.tx_trans.zlp = true;
    */
    
#endif
    return VSF_ERR_NONE;
}

WEAK //!< this weak for debug purpose only 
void vsf_usbd_CDC_data_connect(vsf_usbd_CDC_t *cdc)
{
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED

    if ((cdc->stream.tx.stream != NULL) && !VSF_STREAM_IS_RX_CONNECTED(cdc->stream.tx.stream)) {
        vsf_usbd_ep_send_stream(&cdc->stream.tx);
    }

    if ((cdc->stream.rx.stream != NULL) && !VSF_STREAM_IS_TX_CONNECTED(cdc->stream.rx.stream)) {
        vsf_usbd_ep_recv_stream(&cdc->stream.rx);
    }

#elif VSF_USE_SERVICE_STREAM == ENABLED
     vsf_usbd_ep_recv_stream(&cdc->stream.use_as__vsf_usbd_ep_stream_t);
#endif
}

static vsf_err_t vsf_usbd_CDC_control_request_prepare(vsf_usbd_dev_t *dev, vsf_usbd_ifs_t *ifs)
{
    vsf_usbd_CDC_t *cdc = (vsf_usbd_CDC_t *)ifs->class_param;
    vsf_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    uint8_t *buffer = NULL;
    uint_fast32_t size = 0;

    switch (request->bRequest) {
    case USB_CDCREQ_SEND_ENCAPSULATED_COMMAND:
        if (request->wLength > cdc->cmd.size) {
            return VSF_ERR_FAIL;
        }
        buffer = cdc->cmd.buffer;
        size = request->wLength;
        break;
    case USB_CDCREQ_GET_ENCAPSULATED_RESPONSE:
        if (request->wLength > cdc->resp.size) {
            request->wLength = cdc->resp.size;
        }
        buffer = cdc->resp.buffer;
        size = request->wLength;
        break;
    default:
        return VSF_ERR_FAIL;
    }

    ctrl_handler->trans.pchBuffer = buffer;
    ctrl_handler->trans.nSize = size;
    return VSF_ERR_NONE;
}

static vsf_err_t vsf_usbd_CDC_control_request_process(vsf_usbd_dev_t *dev, vsf_usbd_ifs_t *ifs)
{
    vsf_usbd_CDC_t *cdc = (vsf_usbd_CDC_t *)ifs->class_param;
    vsf_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    vsf_usbd_encapsulate_t *pkg;

    switch (request->bRequest) {
    case USB_CDCREQ_SEND_ENCAPSULATED_COMMAND:
        pkg = &cdc->cmd;
        if (pkg->on_cmd != NULL) {
            return pkg->on_cmd(pkg);
        }
        break;
    case USB_CDCREQ_GET_ENCAPSULATED_RESPONSE:
        pkg = &cdc->resp;
        if (pkg->on_resp != NULL) {
            return pkg->on_resp(pkg);
        }
        break;
    default:
        return VSF_ERR_FAIL;
    }
    return VSF_ERR_NONE;
}

const vsf_usbd_class_op_t vsf_usbd_CDC_control = {
    .request_prepare = vsf_usbd_CDC_control_request_prepare,
    .request_process = vsf_usbd_CDC_control_request_process,
};

const vsf_usbd_class_op_t vsf_usbd_CDC_data = {
    .init = vsf_usbd_CDC_data_init,
};

#endif  // VSF_USE_USB_DEVICE
