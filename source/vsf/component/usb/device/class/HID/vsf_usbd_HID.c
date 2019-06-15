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

#if VSF_USE_USB_DEVICE == ENABLED

#define VSF_USBD_INHERIT
#define VSF_USBD_HID_IMPLEMENT
#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum vsf_usbd_HID_EVT_t {
    VSF_USBD_HID_EVT_INREPORT = VSF_EVT_USER + 0,
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

SECTION(".text.vsf.kernel.eda")
vsf_err_t __vsf_eda_fini(vsf_eda_t *pthis);

/*============================ IMPLEMENTATION ================================*/

static vsf_usbd_HID_report_t * vsf_usbd_HID_find_report(
        vsf_usbd_HID_t *hid, uint_fast8_t type, uint_fast8_t id)
{
    ASSERT(hid != NULL);

    for (uint_fast8_t i = 0; i < hid->num_of_report; i++) {
        if ((hid->reports[i].type == type) && (hid->reports[i].id == id)) {
            return &hid->reports[i];
        }
    }
    return NULL;
}

static void vsf_usbd_HID_on_report(vsf_usbd_HID_t *hid, vsf_usbd_HID_report_t *report)
{
    if (hid->notify_eda) {
        vsf_eda_post_msg(hid->eda, report);
    } else if (hid->on_report) {
        hid->on_report(hid, report);
    }
}

bool vsf_usbh_HID_IN_report_can_update(vsf_usbd_HID_report_t *report)
{
    return !report->changed;
}

vsf_err_t vsf_usbd_HID_IN_report_changed(vsf_usbd_HID_t *hid, vsf_usbd_HID_report_t *report)
{
    report->changed = true;
    if (!hid->busy) {
        vsf_eda_post_evt(&hid->teda.use_as__vsf_eda_t, VSF_USBD_HID_EVT_INREPORT);
    }
    return VSF_ERR_NONE;
}

static void vsf_usbd_HID_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vsf_usbd_HID_t *hid = container_of(eda, vsf_usbd_HID_t, teda);
    vsf_usbd_dev_t *dev = hid->dev;
    vsf_usbd_HID_report_t *report;
    vsf_usbd_trans_t *trans;

    switch (evt) {
    case VSF_EVT_INIT:
        if (hid->ep_out != 0) {
            trans = &hid->transact_out;
            trans->eda = eda;
            trans->notify_eda = true;
            trans->ep = hid->ep_out;
            trans->pchBuffer = NULL;
            vsf_usbd_ep_recv(dev, trans);
        }

        hid->pos_out = 0;
        hid->output_state = HID_OUTPUT_STATE_WAIT;
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
            report = vsf_usbd_HID_find_report(hid, USB_HID_REPORT_INPUT, hid->cur_IN_id);
            hid->busy = false;
            vsf_usbd_HID_on_report(hid, report);
            goto process_in_report;
        } else /* if (trans == &hid->transact_out) */ {
            VSF_USBD_DRV_PREPARE(dev);
            uint_fast8_t ep_out = hid->ep_out;
            uint_fast16_t ep_size = vsf_usbd_drv_ep_get_size(ep_out);
            uint_fast16_t pkg_size = vsf_usbd_drv_ep_get_data_size(ep_out);
            uint8_t report_id = 0;

            switch (hid->output_state) {
            case HID_OUTPUT_STATE_WAIT:
                if (hid->has_report_id) {
                    vsf_usbd_drv_ep_read_buffer(ep_out, &report_id, 1);
                }
                report = vsf_usbd_HID_find_report(hid, USB_HID_REPORT_OUTPUT, report_id);
                if ((NULL == report) || (pkg_size > report->mem.nSize)) {
                    return;
                }

                if (hid->has_report_id) {
                    report->mem.pchBuffer[0] = report_id;
                }

                do {
                    uint_fast8_t offset = hid->has_report_id ? 1 : 0;
                    vsf_usbd_drv_ep_read_buffer(ep_out, &report->mem.pchBuffer[offset], pkg_size - offset);
                } while (0);

                if (pkg_size < report->mem.nSize) {
                    hid->pos_out = pkg_size;
                    hid->output_state++;
                    hid->cur_OUT_id = report_id;
                    vsf_usbd_ep_recv(dev, trans);
                } else {
                notify_report_rx:
                    vsf_usbd_HID_on_report(hid, report);
                }
                break;
            case HID_OUTPUT_STATE_RECEIVING:
                report_id = hid->cur_OUT_id;
                report = vsf_usbd_HID_find_report(hid, USB_HID_REPORT_OUTPUT, report_id);
                if ((NULL == report) || ((pkg_size + hid->pos_out) > report->mem.nSize)) {
                    return;
                }

                vsf_usbd_drv_ep_read_buffer(ep_out, report->mem.pchBuffer + hid->pos_out, pkg_size);
                hid->pos_out += pkg_size;
                if (hid->pos_out >= report->mem.nSize) {
                    hid->pos_out = 0;
                    hid->output_state = HID_OUTPUT_STATE_WAIT;
                    goto notify_report_rx;
                }
                break;
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
                hid->cur_IN_id = report->id;

                trans->ep = hid->ep_in;
                trans->use_as__vsf_mem_t = report->mem;
                trans->zlp = false;
                trans->eda = eda;
                trans->notify_eda = true;
                vsf_usbd_ep_send(dev, trans);

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

static vsf_err_t vsf_usbd_HID_init(vsf_usbd_dev_t *dev, vsf_usbd_ifs_t *ifs)
{
    vsf_usbd_HID_t *hid = (vsf_usbd_HID_t *)ifs->class_param;

    hid->ifs = ifs;
    hid->dev = dev;
    hid->teda.evthandler = vsf_usbd_HID_evthandler;
    return vsf_teda_init(&hid->teda, vsf_usbd_class_priority(dev), false);
}

static vsf_err_t vsf_usbh_HID_fini(vsf_usbd_dev_t *dev, vsf_usbd_ifs_t *ifs)
{
    vsf_usbd_HID_t *hid = (vsf_usbd_HID_t *)ifs->class_param;
    return __vsf_eda_fini(&hid->teda.use_as__vsf_eda_t);
}

static vsf_usbd_desc_t * vsf_usbd_HID_get_desc(vsf_usbd_dev_t *dev,
            uint_fast8_t type, uint_fast8_t index, uint_fast16_t lanid)
{
    struct usb_ctrlrequest_t *request = &dev->ctrl_handler.request;
    vsf_usbd_ifs_t *ifs = vsf_usbd_get_ifs(dev, request->wIndex & 0xFF);
    vsf_usbd_HID_t *hid = (struct vsf_usbd_HID_t *)ifs->class_param;

    if ((NULL == ifs) || (NULL == hid) || (NULL == hid->desc)) {
        return NULL;
    }

    return vsf_usbd_get_descriptor(hid->desc, 1, type, index, lanid);
}

static vsf_err_t vsf_usbd_HID_request_prepare(vsf_usbd_dev_t *dev, vsf_usbd_ifs_t *ifs)
{
    vsf_usbd_HID_t *hid = (vsf_usbd_HID_t *)ifs->class_param;
    vsf_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    uint_fast8_t type = request->wValue >> 8, id = request->wValue;
    vsf_usbd_HID_report_t *report = vsf_usbd_HID_find_report(hid, type, id);
    uint8_t *buffer = NULL;
    uint_fast32_t size = 0;

    switch (request->bRequest) {
    case USB_HIDREQ_GET_REPORT:
        if ((NULL == report) || (type != report->type)) {
            return VSF_ERR_FAIL;
        }

        buffer = report->mem.pchBuffer;
        size = report->mem.nSize;
        if (hid->has_report_id) {
            size--;
            buffer++;
        }
        break;
    case USB_HIDREQ_GET_IDLE:
        if ((NULL == report) || (request->wLength != 1)) {
            return VSF_ERR_FAIL;
        }

        size = 1;
        buffer = &report->idle;
        break;
    case USB_HIDREQ_GET_PROTOCOL:
        if ((request->wValue != 0) || (request->wLength != 1)) {
            return VSF_ERR_FAIL;
        }

        size = 1;
        buffer = &hid->protocol;
        break;
    case USB_HIDREQ_SET_REPORT:
        if ((NULL == report) || (type != report->type)) {
            return VSF_ERR_FAIL;
        }

        size = report->mem.nSize;
        buffer = report->mem.pchBuffer;
        if (hid->has_report_id) {
            size--;
            buffer++;
        }
        break;
    case USB_HIDREQ_SET_IDLE:
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
    case USB_HIDREQ_SET_PROTOCOL:
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
    ctrl_handler->trans.pchBuffer = buffer;
    ctrl_handler->trans.nSize = size;
    return VSF_ERR_NONE;
}

static vsf_err_t vsf_usbd_HID_request_process(vsf_usbd_dev_t *dev, vsf_usbd_ifs_t *ifs)
{
    vsf_usbd_HID_t *hid = (vsf_usbd_HID_t *)ifs->class_param;
    vsf_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;

    if (    (USB_HIDREQ_SET_REPORT == request->bRequest)
        ||  (USB_HIDREQ_GET_REPORT == request->bRequest)) {

        uint_fast8_t type = request->wValue >> 8, id = request->wValue;
        vsf_usbd_HID_report_t *report = vsf_usbd_HID_find_report(hid, type, id);

        if ((NULL == report) || (type != report->type)) {
            return VSF_ERR_FAIL;
        }
        vsf_usbd_HID_on_report(hid, report);
    }
    return VSF_ERR_NONE;
}

const vsf_usbd_class_op_t vsf_usbd_HID = {
    .get_desc =         vsf_usbd_HID_get_desc,
    .request_prepare =  vsf_usbd_HID_request_prepare,
    .request_process =  vsf_usbd_HID_request_process,
    .init =             vsf_usbd_HID_init,
    .fini =             vsf_usbh_HID_fini,
};

#endif      // VSF_USE_USB_DEVICE
