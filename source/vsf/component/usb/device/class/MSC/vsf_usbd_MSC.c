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

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USE_USB_DEVICE_MSC == ENABLED

#define VSF_USBD_INHERIT
#define VSF_USBD_MSC_IMPLEMENT
#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static vsf_err_t vsf_usbd_msc_request_prepare(vsf_usbd_dev_t *dev, vsf_usbd_ifs_t *ifs);
static vsf_err_t vsf_usbd_msc_class_init(vsf_usbd_dev_t *dev, vsf_usbd_ifs_t *ifs);

/*============================ GLOBAL VARIABLES ==============================*/

const vsf_usbd_class_op_t vsf_usbd_msc_class = {
    .request_prepare = vsf_usbd_msc_request_prepare,
    .init = vsf_usbd_msc_class_init,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static void vsf_usbd_msc_send_csw(void *p);
static void vsf_usbd_msc_on_cbw(void *p);
static void vsf_usbd_msc_on_data_out(void *p);
static void vsf_usbd_msc_on_data_in(void *p);
static void vsf_usbd_msc_on_idle(void *p);

/*============================ IMPLEMENTATION ================================*/

static void vsf_usbd_msc_error(vsf_usbd_msc_t *msc, uint_fast8_t error)
{
    usb_msc_cbw_t *cbw = &msc->ctx.cbw;
    bool is_in = (cbw->bmCBWFlags & USB_DIR_MASK) == USB_DIR_IN;

    msc->ctx.csw.dCSWStatus = error;
    if (is_in) {
        if (cbw->dCBWDataTransferLength > 0) {
            vsf_usbd_trans_t *trans = &msc->trans;
            trans->ep = msc->ep_in;
            trans->pchBuffer = NULL;
            trans->nSize = 0;
            trans->on_finish = vsf_usbd_msc_send_csw;
            vsf_usbd_ep_send(msc->dev, trans);
            return;
        }
    } else {
        vsf_usbd_ep_stall(msc->dev, msc->ep_out);
    }
    vsf_usbd_msc_send_csw(msc);
}

static void vsf_usbd_msc_send_csw(void *p)
{
    vsf_usbd_msc_t *msc = p;
    usb_msc_csw_t *csw = &msc->ctx.csw;
    vsf_usbd_trans_t *trans = &msc->trans;

    csw->dCSWSignature = cpu_to_le32(USB_MSC_CSW_SIGNATURE);
    trans->ep = msc->ep_in;
    trans->pchBuffer = (uint8_t *)&msc->ctx.csw;
    trans->nSize = sizeof(msc->ctx.csw);
    trans->on_finish = vsf_usbd_msc_on_idle;
    vsf_usbd_ep_send(msc->dev, trans);
}

static void vsf_usbd_msc_on_data_out(void *p)
{
    vsf_usbd_msc_t *msc = p;
    usb_msc_cbw_t *cbw = &msc->ctx.cbw;
    vsf_usbd_trans_t *trans = &msc->trans;

    if (VSF_ERR_NONE != msc->op->write(msc, &msc->ctx, &trans->use_as__vsf_mem_t)) {
        vsf_usbd_msc_error(msc, USB_MSC_CSW_PHASE_ERROR);
    }
    cbw->dCBWDataTransferLength -= msc->ctx.cur_size - trans->nSize;
    if (0 == cbw->dCBWDataTransferLength) {
        msc->ctx.csw.dCSWStatus = USB_MSC_CSW_OK;
        vsf_usbd_msc_send_csw(msc);
    }
}

static void vsf_usbd_msc_on_data_in(void *p)
{
    vsf_usbd_msc_t *msc = p;
    vsf_usbd_trans_t *trans = &msc->trans;

    if (msc->reply_size > 0) {
        if (VSF_ERR_NONE != msc->op->read(msc, &msc->ctx, &trans->use_as__vsf_mem_t)) {
            vsf_usbd_msc_error(msc, USB_MSC_CSW_PHASE_ERROR);
        }
    } else {
        msc->ctx.csw.dCSWStatus = USB_MSC_CSW_OK;
        vsf_usbd_msc_send_csw(msc);
    }
}

void vsf_usbd_msc_send_data(vsf_usbd_msc_t *msc)
{
    vsf_usbd_trans_t *trans = &msc->trans;

    VSF_USB_ASSERT(msc->ctx.cbw.dCBWDataTransferLength >= trans->nSize);
    msc->ctx.cbw.dCBWDataTransferLength -= trans->nSize;
    msc->reply_size -= trans->nSize;
    trans->ep = msc->ep_in;
    trans->on_finish = vsf_usbd_msc_on_data_in;
    vsf_usbd_ep_send(msc->dev, trans);
}

void vsf_usbd_msc_recv_data(vsf_usbd_msc_t *msc)
{
    usb_msc_cbw_t *cbw = &msc->ctx.cbw;
    vsf_usbd_trans_t *trans = &msc->trans;

    if (cbw->dCBWDataTransferLength > 0) {
        msc->ctx.cur_size = trans->nSize;
        vsf_usbd_ep_recv(msc->dev, trans);
    }
}

static void vsf_usbd_msc_on_cbw(void *p)
{
    vsf_usbd_msc_t *msc = p;
    usb_msc_cbw_t *cbw = &msc->ctx.cbw;
    vsf_usbd_trans_t *trans = &msc->trans;

    if (    (trans->nSize > 0)
        ||  (cbw->dCBWSignature != cpu_to_le32(USB_MSC_CBW_SIGNATURE))
        ||  (cbw->bCBWCBLength < 1) || (cbw->bCBWCBLength > 16)) {
        vsf_usbd_msc_error(msc, USB_MSC_CSW_PHASE_ERROR);
        return;
    }

    if (    (cbw->bCBWLUN > msc->max_lun)
        ||  ((msc->reply_size = msc->op->execute(msc, &msc->ctx, &trans->use_as__vsf_mem_t)) < 0)) {
        vsf_usbd_msc_error(msc, USB_MSC_CSW_FAIL);
        return;
    }

    if (cbw->dCBWDataTransferLength > 0) {
        if ((cbw->bmCBWFlags & USB_DIR_MASK) == USB_DIR_IN) {
            if (trans->pchBuffer != NULL) {
                VSF_USB_ASSERT(msc->reply_size == trans->nSize);
                vsf_usbd_msc_send_data(msc);
            } else {
                vsf_usbd_msc_on_data_in(msc);
            }
        } else {
            VSF_USB_ASSERT((trans->pchBuffer != NULL) && (trans->nSize > 0));
            trans->on_finish = vsf_usbd_msc_on_data_out;
            trans->ep = msc->ep_out;
            vsf_usbd_msc_recv_data(msc);
        }
    } else {
        vsf_usbd_msc_send_csw(msc);
    }
}

static void vsf_usbd_msc_on_idle(void *p)
{
    vsf_usbd_msc_t *msc = p;
    vsf_usbd_trans_t *trans = &msc->trans;

    trans->ep = msc->ep_out;
    trans->pchBuffer = (uint8_t *)&msc->ctx.cbw;
    trans->nSize = sizeof(msc->ctx.cbw);
    trans->on_finish = vsf_usbd_msc_on_cbw;
    vsf_usbd_ep_recv(msc->dev, trans);
}

static vsf_err_t vsf_usbd_msc_class_init(vsf_usbd_dev_t *dev, vsf_usbd_ifs_t *ifs)
{
    vsf_usbd_msc_t *msc = ifs->class_param;

    msc->dev = dev;
    memset(&msc->trans, 0, sizeof(msc->trans));
    msc->trans.param = msc;
    vsf_usbd_msc_on_idle(msc);
    return VSF_ERR_NONE;
}

static vsf_err_t vsf_usbd_msc_request_prepare(vsf_usbd_dev_t *dev, vsf_usbd_ifs_t *ifs)
{
    vsf_usbd_msc_t *msc = ifs->class_param;
    vsf_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    uint8_t *buffer = NULL;
    uint_fast32_t size = 0;

    switch (request->bRequest) {
    case USB_MSC_REQ_GET_MAX_LUN:
        if ((request->wLength != 1) || (request->wValue != 0)) {
            return VSF_ERR_FAIL;
        }
        // TODO: process alignment
        buffer = (uint8_t *)&msc->max_lun;
        size = 1;
        break;
    case USB_MSC_REQ_RESET:
    default:
        return VSF_ERR_NOT_SUPPORT;
    }
    ctrl_handler->trans.pchBuffer = buffer;
    ctrl_handler->trans.nSize = size;
    return VSF_ERR_NONE;
}

#endif      // VSF_USE_USB_DEVICE && VSF_USE_USB_DEVICE_MSC
