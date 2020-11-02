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

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_HID == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_USBD_CLASS_INHERIT__
#define __VSF_USBD_HID_CLASS_IMPLEMENT

#include "kernel/vsf_kernel.h"
#include "../../vsf_usbd.h"
#include "./vsf_usbd_HID.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    VSF_USBD_HID_EVT_INREPORT = VSF_EVT_USER + 0,
};

/*============================ PROTOTYPES ====================================*/

static vk_usbd_desc_t * __vk_usbd_hid_get_desc(vk_usbd_dev_t *dev,
            uint_fast8_t type, uint_fast8_t index, uint_fast16_t lanid);
static vsf_err_t __vk_usbd_hid_request_prepare(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
static vsf_err_t __vk_usbd_hid_request_process(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
static vsf_err_t __vk_usbd_hid_init(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
static vsf_err_t __vk_usbh_hid_fini(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbd_class_op_t vk_usbd_hid = {
    .get_desc =         __vk_usbd_hid_get_desc,
    .request_prepare =  __vk_usbd_hid_request_prepare,
    .request_process =  __vk_usbd_hid_request_process,
    .init =             __vk_usbd_hid_init,
    .fini =             __vk_usbh_hid_fini,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static vk_usbd_hid_report_t * __vk_usbd_hid_find_report(
        vk_usbd_hid_t *hid, uint_fast8_t type, uint_fast8_t id)
{
    VSF_USB_ASSERT(hid != NULL);

    for (uint_fast8_t i = 0; i < hid->num_of_report; i++) {
        if ((hid->reports[i].type == type) && (hid->reports[i].id == id)) {
            return &hid->reports[i];
        }
    }
    return NULL;
}

static void __vk_usbd_hid_on_report(vk_usbd_hid_t *hid, vk_usbd_hid_report_t *report)
{
    if (hid->notify_eda) {
        vsf_eda_post_msg(hid->eda, report);
    } else if (hid->on_report) {
        hid->on_report(hid, report);
    }
}

bool vk_usbh_hid_in_report_can_update(vk_usbd_hid_report_t *report)
{
    return !report->changed;
}

void vk_usbd_hid_in_report_changed(vk_usbd_hid_t *hid, vk_usbd_hid_report_t *report)
{
    VSF_USB_ASSERT(!report->changed);
    report->changed = true;
    if (!hid->busy) {
        vsf_eda_post_evt(&hid->teda.use_as__vsf_eda_t, VSF_USBD_HID_EVT_INREPORT);
    }
}

void vk_usbh_hid_out_report_processed(vk_usbd_hid_t *hid, vk_usbd_hid_report_t *report)
{
    vk_usbd_trans_t *trans = &hid->transact_out;
    report->mem.buffer = NULL;
    trans->size = hid->rx_buffer.size;
    vk_usbd_ep_recv(hid->dev, trans);
}

static void __vk_usbd_hid_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbd_hid_t *hid = container_of(eda, vk_usbd_hid_t, teda);
    vk_usbd_dev_t *dev = hid->dev;
    vk_usbd_hid_report_t *report;
    vk_usbd_trans_t *trans;

    switch (evt) {
    case VSF_EVT_INIT:
        if (hid->ep_out != 0) {
            trans = &hid->transact_out;
            trans->eda = eda;
            trans->notify_eda = true;
            trans->ep = hid->ep_out;
            trans->use_as__vsf_mem_t = hid->rx_buffer;
            vk_usbd_ep_recv(dev, trans);
        }

        hid->busy = false;
        for (uint_fast8_t i = 0; i < hid->num_of_report; i++) {
            report = &hid->reports[i];
            report->idle_cnt = 0;
        }

        vsf_teda_set_timer_ms(4);
        break;
    case VSF_EVT_MESSAGE:
        trans = vsf_eda_get_cur_msg();
        if (trans == &hid->transact_in) {
            report = __vk_usbd_hid_find_report(hid, USB_HID_REPORT_INPUT, hid->cur_in_id);
            hid->busy = false;
            __vk_usbd_hid_on_report(hid, report);
            goto process_in_report;
        } else /* if (trans == &hid->transact_out) */ {
            uint_fast16_t size = hid->rx_buffer.size - trans->size;
            if (size > 0) {
                uint_fast8_t report_id = 0;
                if (hid->has_report_id) {
                    report_id = hid->rx_buffer.buffer[0];
                }
                report = __vk_usbd_hid_find_report(hid, USB_HID_REPORT_OUTPUT, report_id);
                if (report != NULL) {
                    VSF_USB_ASSERT(report->mem.size == size);
                    report->mem.buffer = hid->rx_buffer.buffer;
                    __vk_usbd_hid_on_report(hid, report);
                }
            }
        }
        break;
    case VSF_EVT_TIMER:
        vsf_teda_set_timer_ms(4);
        for (uint_fast8_t i = 0; i < hid->num_of_report; i++) {
            report = &hid->reports[i];
            if ((report->type == USB_HID_REPORT_INPUT) && (report->idle != 0)) {
                report->idle_cnt++;
            }
        }
        if (hid->busy) {
            break;
        }
        // fall through
    case VSF_USBD_HID_EVT_INREPORT:
    process_in_report:
        if (hid->busy) {
            break;
        }

        if (hid->cur_report >= hid->num_of_report) {
            hid->cur_report = 0;
        }

        trans = &hid->transact_in;
        for (; hid->cur_report < hid->num_of_report; hid->cur_report++) {
            report = &hid->reports[hid->cur_report];
            if (    (report->type == USB_HID_REPORT_INPUT)
                &&  (   report->changed
                    || (    (report->idle != 0)
                        &&  (report->idle_cnt >= report->idle)))) {
                hid->cur_in_id = report->id;

                trans->ep = hid->ep_in;
                trans->use_as__vsf_mem_t = report->mem;
                trans->zlp = false;
                trans->eda = eda;
                trans->notify_eda = true;
                vk_usbd_ep_send(dev, trans);

                report->changed = false;
                report->idle_cnt = 0;
                hid->cur_report++;
                hid->busy = true;
                break;
            }
        }
        break;
    }
}

static vsf_err_t __vk_usbd_hid_init(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_usbd_hid_t *hid = ifs->class_param;

    VSF_USB_ASSERT((hid->rx_buffer.buffer != NULL) && (hid->rx_buffer.size > 0));
    hid->ifs = ifs;
    hid->dev = dev;

    hid->teda.fn.evthandler = __vk_usbd_hid_evthandler;
#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    hid->teda.on_terminate = NULL;
#endif
    return vsf_teda_init(&hid->teda, VSF_USBD_CFG_EDA_PRIORITY, false);
}

static vsf_err_t __vk_usbh_hid_fini(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_usbd_hid_t *hid = ifs->class_param;
    return vsf_eda_fini(&hid->teda.use_as__vsf_eda_t);
}

static vk_usbd_desc_t * __vk_usbd_hid_get_desc(vk_usbd_dev_t *dev,
            uint_fast8_t type, uint_fast8_t index, uint_fast16_t lanid)
{
    struct usb_ctrlrequest_t *request = &dev->ctrl_handler.request;
    vk_usbd_ifs_t *ifs = vk_usbd_get_ifs(dev, request->wIndex & 0xFF);
    vk_usbd_hid_t *hid = ifs->class_param;

    if ((NULL == ifs) || (NULL == hid) || (NULL == hid->desc)) {
        return NULL;
    }

    return vk_usbd_get_descriptor(hid->desc, 1, type, index, lanid);
}

static vsf_err_t __vk_usbd_hid_request_prepare(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_usbd_hid_t *hid = ifs->class_param;
    vk_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    uint_fast8_t type = request->wValue >> 8, id = request->wValue;
    vk_usbd_hid_report_t *report = __vk_usbd_hid_find_report(hid, type, id);
    uint8_t *buffer = NULL;
    uint_fast32_t size = 0;

    switch (request->bRequest) {
    case USB_HID_REQ_GET_REPORT:
        if ((NULL == report) || (type != report->type) || (NULL == report->mem.buffer)) {
            return VSF_ERR_FAIL;
        }

        buffer = report->mem.buffer;
        size = report->mem.size;
        if (hid->has_report_id) {
            size--;
            buffer++;
        }
        break;
    case USB_HID_REQ_GET_IDLE:
        if ((NULL == report) || (request->wLength != 1)) {
            return VSF_ERR_FAIL;
        }

        size = 1;
        buffer = &report->idle;
        break;
    case USB_HID_REQ_GET_PROTOCOL:
        if ((request->wValue != 0) || (request->wLength != 1)) {
            return VSF_ERR_FAIL;
        }

        size = 1;
        buffer = &hid->protocol;
        break;
    case USB_HID_REQ_SET_REPORT:
        if ((NULL == report) || (type != report->type)) {
            return VSF_ERR_FAIL;
        }

        size = report->mem.size;
        buffer = report->mem.buffer;
        if (hid->has_report_id) {
            size--;
            buffer++;
        }
        break;
    case USB_HID_REQ_SET_IDLE:
        if (request->wLength != 0) {
            return VSF_ERR_FAIL;
        }

        for (uint_fast8_t i = 0; i < hid->num_of_report; i++) {
            if (    (hid->reports[i].type == USB_HID_REPORT_INPUT)
                &&  ((0 == id) || (hid->reports[i].id == id))) {
                hid->reports[i].idle = request->wValue >> 8;
            }
        }
        break;
    case USB_HID_REQ_SET_PROTOCOL:
        if (    (request->wLength != 1)
            ||  (   (request->wValue != USB_HID_PROTOCOL_BOOT)
                 && (request->wValue != USB_HID_PROTOCOL_REPORT))) {
            return VSF_ERR_FAIL;
        }
        hid->protocol = request->wValue;
        break;
    default:
        return VSF_ERR_FAIL;
    }
    ctrl_handler->trans.buffer = buffer;
    ctrl_handler->trans.size = size;
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_usbd_hid_request_process(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_usbd_hid_t *hid = ifs->class_param;
    vk_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;

    if (    (USB_HID_REQ_SET_REPORT == request->bRequest)
        ||  (USB_HID_REQ_GET_REPORT == request->bRequest)) {

        uint_fast8_t type = request->wValue >> 8, id = request->wValue;
        vk_usbd_hid_report_t *report = __vk_usbd_hid_find_report(hid, type, id);

        if ((NULL == report) || (type != report->type)) {
            return VSF_ERR_FAIL;
        }
        __vk_usbd_hid_on_report(hid, report);
    }
    return VSF_ERR_NONE;
}

#endif      // VSF_USE_USB_DEVICE && VSF_USBD_USE_HID
