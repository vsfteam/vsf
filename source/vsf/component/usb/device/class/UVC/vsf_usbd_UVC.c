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
#define VSF_USBD_UVC_IMPLEMENT
#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_USBD_UVC_TRACE_EN == ENABLED
static char * vsf_usbd_UVC_trace_get_request(uint_fast8_t request)
{
    switch (request) {
    case USB_UVCREQ_CUR:    return "CUR";
    case USB_UVCREQ_MIN:    return "MIN";
    case USB_UVCREQ_MAX:    return "MAX";
    case USB_UVCREQ_RES:    return "RES";
	case USB_UVCREQ_LEN:    return "LEN";
	case USB_UVCREQ_INFO:   return "INFO";
    case USB_UVCREQ_DEF:    return "DEF";
    default:                return "UNKNOWN";
    }
}

static void vsf_usbd_UVC_trace_request_prepare(vsf_usbd_ctrl_handler_t *ctrl_handler)
{
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    uint_fast8_t req = request->bRequest;
    uint_fast8_t ifs_ep = (request->wIndex >> 0) & 0xFF;
    uint_fast8_t entity = (request->wIndex >> 8) & 0xFF;
    uint_fast8_t cs = (request->wValue >> 8) & 0xFF;
    bool is_get = req & USB_UVCREQ_GET;

    if (    (USB_RECIP_INTERFACE == (request->bRequestType & USB_RECIP_MASK))
        &&  (USB_TYPE_CLASS == (request->bRequestType & USB_TYPE_MASK))) {

        vsf_trace(VSF_TRACE_DEBUG, "uvc: %s%s ifs/ep=%d, entity=%d, cs=%d" VSF_TRACE_CFG_LINEEND,
                    is_get ? "GET_" : "SET_",
                    vsf_usbd_UVC_trace_get_request(req & ~USB_UVCREQ_GET),
                    ifs_ep, entity, cs);
        if (is_get) {
            vsf_trace_buffer(VSF_TRACE_NONE, ctrl_handler->trans.pchBuffer,
                    ctrl_handler->trans.nSize, VSF_TRACE_DF_DEFAULT);
        }
    }
}

static void vsf_usbd_UVC_trace_request_process(vsf_usbd_ctrl_handler_t *ctrl_handler)
{
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    uint_fast8_t req = request->bRequest;
    bool is_get = req & USB_UVCREQ_GET;

    if (    (USB_RECIP_INTERFACE == (request->bRequestType & USB_RECIP_MASK))
        &&  (USB_TYPE_CLASS == (request->bRequestType & USB_TYPE_MASK))) {

        if (!is_get) {
            vsf_trace_buffer(VSF_TRACE_NONE, ctrl_handler->trans.pchBuffer,
                    ctrl_handler->trans.nSize, VSF_TRACE_DF_DEFAULT);
        }
    }
}
#endif

WEAK void vsf_usbd_UVC_stop_stream(vsf_usbd_UVC_t *uvc, uint_fast8_t ifs)
{
}

WEAK void vsf_usbd_UVC_start_stream(vsf_usbd_UVC_t *uvc, uint_fast8_t ifs)
{
}

vsf_err_t vsf_usbd_UVC_send_packet(vsf_usbd_UVC_t *uvc, uint8_t *buffer, uint_fast32_t size)
{
    vsf_usbd_trans_t *trans = &uvc->trans_in;

    trans->ep = uvc->ep_in;
    trans->use_as__vsf_mem_t.pchBuffer = buffer;
    trans->use_as__vsf_mem_t.nSize = size;
    trans->zlp = false;
    trans->eda = vsf_eda_get_cur();
    trans->notify_eda = true;
    return vsf_usbd_ep_send(uvc->dev, trans);
}

static vsf_usbd_UVC_control_t *vsf_usbd_UVC_get_control(
        vsf_usbd_UVC_t *uvc, uint_fast8_t id, uint_fast8_t selector)
{
    vsf_usbd_UVC_entity_t *entity = uvc->entity;
    vsf_usbd_UVC_control_t *control;

    for (int i = 0; i < uvc->entity_num; i++, entity++) {
        if (entity->id == id) {
            control = entity->control;
            // TODO: if control_num is 1, cs from wValue maybe invalid
            for (int j = 0; j < entity->control_num; j++, control++) {
                if (control->info->selector == selector) {
                    return control;
                }
            }
            break;
        }
    }
    return NULL;
}

static vsfav_control_value_t *vsf_usbd_UVC_get_value(
        vsf_usbd_UVC_control_t *control, uint_fast8_t request)
{
    vsfav_control_value_t *value = NULL;
    request = request & ~USB_UVCREQ_GET;
    switch (request) {
    case USB_UVCREQ_CUR:    value = &control->cur; break;
    case USB_UVCREQ_MIN:    value = (vsfav_control_value_t *)&control->info->min; break;
    case USB_UVCREQ_MAX:    value = (vsfav_control_value_t *)&control->info->max; break;
    case USB_UVCREQ_RES:
	case USB_UVCREQ_LEN:
	case USB_UVCREQ_INFO:   break;
    case USB_UVCREQ_DEF:    value = (vsfav_control_value_t *)&control->info->def; break;
    }
    return value;
}

static vsf_err_t vsf_usbd_UVC_request_prepare(vsf_usbd_dev_t *dev, vsf_usbd_ifs_t *ifs)
{
    vsf_usbd_UVC_t *uvc = ifs->class_param;
    vsf_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    const vsf_usbd_UVC_control_info_t *cinfo;
    vsf_usbd_UVC_control_t *control;
    vsfav_control_value_t *value;
    uint8_t *buffer = NULL;
    uint_fast32_t size = 0;

    switch (request->bRequestType & USB_RECIP_MASK) {
    case USB_RECIP_INTERFACE:
        switch (request->bRequestType & USB_TYPE_MASK) {
        case USB_TYPE_STANDARD:
            switch (request->bRequest) {
            case USB_REQ_SET_INTERFACE:
                if (0 == request->wValue) {
                    // 0-bandwidth
#if VSF_USBD_UVC_TRACE_EN == ENABLED
                    vsf_trace(VSF_TRACE_DEBUG, "uvc: stop stream." VSF_TRACE_CFG_LINEEND);
#endif
                    vsf_usbd_UVC_stop_stream(uvc, request->wValue);
                } else {
#if VSF_USBD_UVC_TRACE_EN == ENABLED
                    vsf_trace(VSF_TRACE_DEBUG, "uvc: start stream %d." VSF_TRACE_CFG_LINEEND,
                                request->wValue);
#endif
                    vsf_usbd_UVC_start_stream(uvc, request->wValue);
                }
                break;
            }
            break;
        case USB_TYPE_CLASS: {
//            uint_fast8_t ifs_ep = (request->wIndex >> 0) & 0xFF;
            uint_fast8_t entity = (request->wIndex >> 8) & 0xFF;
            uint_fast8_t cs = (request->wValue >> 8) & 0xFF;

            control = vsf_usbd_UVC_get_control(uvc, entity, cs);
            if (!control) { return VSF_ERR_FAIL; }

            cinfo = control->info;
            value = vsf_usbd_UVC_get_value(control, request->bRequest);
            if (request->bRequest & USB_UVCREQ_GET) {
                if (!value) {
                    switch (request->bRequest & ~USB_UVCREQ_GET) {
                    case USB_UVCREQ_LEN:
                        size = sizeof(control->info->size);
                        // TODO: code below only support little-endian
                        buffer = (uint8_t *)&cinfo->size;
                        break;
                    case USB_UVCREQ_RES:
                    case USB_UVCREQ_INFO:
                        return VSF_ERR_FAIL;
                    }
                } else {
                    size = cinfo->size;
                    buffer = size < 4 ? &value->uval32 : value->buffer;
                }
            } else {
                if (request->bRequest != (USB_UVCREQ_SET | USB_UVCREQ_CUR)) {
                    return VSF_ERR_FAIL;
                }

                size = cinfo->size;
                buffer = size < 4 ? &control->cur.uval8 : control->cur.buffer;
            }
            break;
        }
        }
        break;
    case USB_RECIP_ENDPOINT:
        break;
    }
    ctrl_handler->trans.pchBuffer = buffer;
    ctrl_handler->trans.nSize = size;
#if VSF_USBD_UVC_TRACE_EN == ENABLED
    uvc->cur_size = size;
    vsf_usbd_UVC_trace_request_prepare(ctrl_handler);
#endif
    return VSF_ERR_NONE;
}

static vsf_err_t vsf_usbd_UVC_request_process(vsf_usbd_dev_t *dev, vsf_usbd_ifs_t *ifs)
{
    vsf_usbd_UVC_t *uvc = ifs->class_param;
    vsf_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    vsf_usbd_UVC_control_t *control;

#if VSF_USBD_UVC_TRACE_EN == ENABLED
    ctrl_handler->trans.nSize = uvc->cur_size;
    vsf_usbd_UVC_trace_request_process(ctrl_handler);
#endif
    switch (request->bRequestType & USB_RECIP_MASK) {
    case USB_RECIP_INTERFACE:
        switch (request->bRequestType & USB_TYPE_MASK) {
        case USB_TYPE_STANDARD:
            switch (request->bRequest) {
            case USB_REQ_SET_INTERFACE:
                break;
            }
            break;
        case USB_TYPE_CLASS: {
//            uint_fast8_t ifs_ep = (request->wIndex >> 0) & 0xFF;
            uint_fast8_t entity = (request->wIndex >> 8) & 0xFF;
            uint_fast8_t cs = (request->wValue >> 8) & 0xFF;

            control = vsf_usbd_UVC_get_control(uvc, entity, cs);
            if (!control) { return VSF_ERR_FAIL; }

            if (    (request->bRequest == (USB_UVCREQ_SET | USB_UVCREQ_CUR))
                &&  (control->info->on_set != NULL)) {
                control->info->on_set(control);
            }
            break;
        }
        }
        break;
    case USB_RECIP_ENDPOINT:
        break;
    }
    return VSF_ERR_NONE;
}

static vsf_err_t vsf_usbd_UVCVC_class_init(vsf_usbd_dev_t *dev, vsf_usbd_ifs_t *ifs)
{
    vsf_usbd_UVC_t *uvc = ifs->class_param;

    uvc->ifs = ifs;
    uvc->dev = dev;
    return VSF_ERR_NONE;
}

static vsf_err_t vsf_usbd_UVCVS_class_init(vsf_usbd_dev_t *dev, vsf_usbd_ifs_t *ifs)
{
//    vsf_usbd_UVC_t *uvc = ifs->class_param;

//    TODO
    return VSF_ERR_NONE;
}

const vsf_usbd_class_op_t vsf_usbd_UVC_control_class = {
    .request_prepare =  vsf_usbd_UVC_request_prepare,
    .request_process =  vsf_usbd_UVC_request_process,
    .init =             vsf_usbd_UVCVC_class_init,
};

const vsf_usbd_class_op_t vsf_usbd_UVC_stream_class = {
    .request_prepare =  vsf_usbd_UVC_request_prepare,
    .request_process =  vsf_usbd_UVC_request_process,
    .init =             vsf_usbd_UVCVS_class_init,
};

#endif      // VSF_USE_USB_DEVICE
