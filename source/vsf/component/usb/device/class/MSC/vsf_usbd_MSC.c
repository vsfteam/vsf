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

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_MSC == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_USBD_CLASS_INHERIT__
#define __VSF_USBD_MSC_CLASS_IMPLEMENT

#include "kernel/vsf_kernel.h"
#include "../../vsf_usbd.h"
#include "./vsf_usbd_MSC.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    VSF_EVT_EXECUTE = VSF_EVT_USER + 0,
};

/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vk_usbd_msc_request_prepare(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
static vsf_err_t __vk_usbd_msc_init(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbd_class_op_t vk_usbd_msc = {
    .request_prepare = __vk_usbd_msc_request_prepare,
    .init = __vk_usbd_msc_init,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static void __vk_usbd_msc_send_csw(void *p);
static void __vk_usbd_msc_on_cbw(void *p);
static void __vk_usbd_msc_on_idle(void *p);

/*============================ IMPLEMENTATION ================================*/

static void __vk_usbd_msc_error(vk_usbd_msc_t *msc, uint_fast8_t error)
{
    usb_msc_cbw_t *cbw = &msc->ctx.cbw;
    bool is_in = (cbw->bmCBWFlags & USB_DIR_MASK) == USB_DIR_IN;

    msc->ctx.csw.dCSWStatus = error;
    if (is_in) {
        if (cbw->dCBWDataTransferLength > 0) {
            vk_usbd_trans_t *trans = &msc->ep_stream.use_as__vk_usbd_trans_t;
            trans->ep = msc->ep_in;
            trans->buffer = NULL;
            trans->size = 0;
            trans->on_finish = __vk_usbd_msc_send_csw;
            trans->param = msc;
            vk_usbd_ep_send(msc->dev, trans);
            return;
        }
    } else {
        vk_usbd_ep_stall(msc->dev, msc->ep_out);
    }
    __vk_usbd_msc_send_csw(msc);
}

static void __vk_usbd_msc_send_csw(void *p)
{
    vk_usbd_msc_t *msc = p;
    usb_msc_csw_t *csw = &msc->ctx.csw;
    vk_usbd_trans_t *trans = &msc->ep_stream.use_as__vk_usbd_trans_t;

    // TODO: fix csw->dCSWDataResidue
    csw->dCSWSignature = cpu_to_le32(USB_MSC_CSW_SIGNATURE);
    trans->ep = msc->ep_in;
    trans->buffer = (uint8_t *)&msc->ctx.csw;
    trans->size = sizeof(msc->ctx.csw);
    trans->on_finish = __vk_usbd_msc_on_idle;
    trans->param = msc;
    vk_usbd_ep_send(msc->dev, trans);
}

static void __vk_usbd_msc_on_data_out(void *p)
{
    vsf_eda_post_evt(&((vk_usbd_msc_t *)p)->eda, VSF_EVT_EXECUTE);
}

static void __vk_usbd_msc_on_data_in(void *p)
{
    vk_usbd_msc_t *msc = p;
    msc->ctx.csw.dCSWStatus = USB_MSC_CSW_OK;
    __vk_usbd_msc_send_csw(msc);
}

static void __vk_usbd_msc_on_cbw(void *p)
{
    vk_usbd_msc_t *msc = p;
    usb_msc_cbw_t *cbw = &msc->ctx.cbw;
    vk_usbd_trans_t *trans = &msc->ep_stream.use_as__vk_usbd_trans_t;

    if (    (trans->size > 0)
        ||  (cbw->dCBWSignature != cpu_to_le32(USB_MSC_CBW_SIGNATURE))
        ||  (cbw->bCBWCBLength < 1) || (cbw->bCBWCBLength > 16)) {
        __vk_usbd_msc_error(msc, USB_MSC_CSW_PHASE_ERROR);
        return;
    }
    if (cbw->bCBWLUN > msc->max_lun) {
        __vk_usbd_msc_error(msc, USB_MSC_CSW_FAIL);
        return;
    }

    if (    vk_scsi_prepare_buffer(msc->scsi, msc->ctx.cbw.CBWCB, &trans->use_as__vsf_mem_t)
        &&  ((cbw->bmCBWFlags & USB_DIR_MASK) == USB_DIR_OUT)
        &&  (cbw->dCBWDataTransferLength > 0)) {

        trans->ep = msc->ep_out;
        trans->on_finish = __vk_usbd_msc_on_data_out;
        trans->param = msc;
        vk_usbd_ep_recv(msc->dev, trans);
    } else {
        vsf_eda_post_evt(&msc->eda, VSF_EVT_EXECUTE);
    }
}

static void __vk_usbd_msc_on_idle(void *p)
{
    vk_usbd_msc_t *msc = p;
    vk_usbd_trans_t *trans = &msc->ep_stream.use_as__vk_usbd_trans_t;

    trans->ep = msc->ep_out;
    trans->buffer = (uint8_t *)&msc->ctx.cbw;
    trans->size = sizeof(msc->ctx.cbw);
    trans->on_finish = __vk_usbd_msc_on_cbw;
    trans->param = msc;
    vk_usbd_ep_recv(msc->dev, trans);
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

static void __vk_usbd_msc_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbd_msc_t *msc = container_of(eda, vk_usbd_msc_t, eda);
    usb_msc_cbw_t *cbw = &msc->ctx.cbw;
    bool is_in = (cbw->bmCBWFlags & USB_DIR_MASK) == USB_DIR_IN;
    vk_usbd_trans_t *trans = &msc->ep_stream.use_as__vk_usbd_trans_t;
    int_fast32_t reply_len;

    switch (evt) {
    case VSF_EVT_INIT:
        msc->is_inited = false;
        vk_scsi_init(msc->scsi);
        break;
    case VSF_EVT_RETURN:
        reply_len = vsf_eda_get_return_value();
        if (!msc->is_inited) {
            if (reply_len < 0) {
                // fail to initialize scsi
                VSF_USB_ASSERT(false);
                return;
            }
            msc->is_inited = true;
            __vk_usbd_msc_on_idle(msc);
        } else {
            if (reply_len < 0) {
                __vk_usbd_msc_error(msc, USB_MSC_CSW_FAIL);
                break;
            }

            if (is_in && (cbw->dCBWDataTransferLength > 0)) {
                if (!msc->is_stream) {
                    trans->size = reply_len;
                    trans->ep = msc->ep_in;
                    trans->on_finish = __vk_usbd_msc_on_data_in;
                    trans->param = msc;
                    vk_usbd_ep_send(msc->dev, trans);
                }
            } else {
                __vk_usbd_msc_send_csw(msc);
            }
        }
        break;
    case VSF_EVT_EXECUTE:
        if (cbw->dCBWDataTransferLength > 0) {
            if (trans->buffer != NULL) {
                msc->is_stream = false;
                vk_scsi_execute(msc->scsi, cbw->CBWCB, &trans->use_as__vsf_mem_t);
            } else if (msc->stream != NULL) {
                msc->is_stream = true;
                msc->ep_stream.stream = msc->stream;
                vsf_stream_init(msc->stream);
                if (is_in) {
                    msc->ep_stream.ep = msc->ep_in;
                    msc->ep_stream.callback.on_finish = __vk_usbd_msc_on_data_in;
                    vk_usbd_ep_send_stream(&msc->ep_stream, cbw->dCBWDataTransferLength);
                } else {
                    msc->ep_stream.ep = msc->ep_out;
                    msc->ep_stream.callback.on_finish = NULL;
                    vk_usbd_ep_recv_stream(&msc->ep_stream, cbw->dCBWDataTransferLength);
                }
                vk_scsi_execute_stream(msc->scsi, cbw->CBWCB, msc->stream);
            } else {
                // how to get the buffer?
                VSF_USB_ASSERT(false);
            }
        } else {
            vk_scsi_execute(msc->scsi, cbw->CBWCB, NULL);
        }
        break;
    }
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_warning=pe111
#endif

static vsf_err_t __vk_usbd_msc_init(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_usbd_msc_t *msc = ifs->class_param;

    msc->dev = dev;
    memset(&msc->ep_stream, 0, sizeof(msc->ep_stream));
    msc->ep_stream.dev = dev;
    msc->ep_stream.zlp = false;
    msc->ep_stream.callback.param = msc;

    msc->eda.fn.evthandler = __vk_usbd_msc_evthandler;
#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    msc->eda.on_terminate = NULL;
#endif
    return vsf_eda_init(&msc->eda, VSF_USBD_CFG_EDA_PRIORITY, false);
}

static vsf_err_t __vk_usbd_msc_request_prepare(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_usbd_msc_t *msc = ifs->class_param;
    vk_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
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
    ctrl_handler->trans.buffer = buffer;
    ctrl_handler->trans.size = size;
    return VSF_ERR_NONE;
}

#endif      // VSF_USE_USB_DEVICE && VSF_USBD_USE_MSC
