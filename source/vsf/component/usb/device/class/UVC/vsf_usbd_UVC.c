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

static vsf_usbd_UVC_control_t *vsf_usbd_UVC_get_control(
        vsf_usbd_UVC_t *uvc, uint8_t entity, uint8_t selector)
{
    vsf_usbd_UVC_control_t *control = uvc->control;
    for (uint32_t i = 0; i < uvc->control_num; i++, control++) {
        if (    (control->info->entity == entity)
            &&  (control->info->selector == selector)) {
            return control;
        }
    }
    return NULL;
}

static vsfav_control_value_t *vsf_usbd_UVC_get_value(
        vsf_usbd_UVC_control_t *control, uint8_t request)
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
    uint8_t entity = (request->wIndex >> 8) & 0xFF;
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
                break;
            }
            break;
        case USB_TYPE_CLASS:
            control = vsf_usbd_UVC_get_control(uvc, entity, request->wValue);
            if (!control) return VSF_ERR_FAIL;

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
                    buffer = size < 4 ? &control->cur.uval8 : control->cur.pbuf;
                }
            } else {
                if (request->bRequest != (USB_UVCREQ_SET | USB_UVCREQ_CUR)) {
                    return VSF_ERR_FAIL;
                }

                size = cinfo->size;
                buffer = size < 4 ? &control->cur.uval8 : control->cur.pbuf;
            }
            break;
        }
        break;
    case USB_RECIP_ENDPOINT:
        break;
    }
    ctrl_handler->trans.pchBuffer = buffer;
    ctrl_handler->trans.nSize = size;
    return VSF_ERR_NONE;
}

static vsf_err_t vsf_usbd_UVC_request_process(vsf_usbd_dev_t *dev, vsf_usbd_ifs_t *ifs)
{
    vsf_usbd_UVC_t *uvc = ifs->class_param;
    vsf_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    uint8_t entity = (request->wIndex >> 8) & 0xFF;
    vsf_usbd_UVC_control_t *control;

    switch (request->bRequestType & USB_RECIP_MASK) {
    case USB_RECIP_INTERFACE:
        switch (request->bRequestType & USB_TYPE_MASK) {
        case USB_TYPE_STANDARD:
            switch (request->bRequest) {
            case USB_REQ_SET_INTERFACE:
                break;
            }
            break;
        case USB_TYPE_CLASS:
            control = vsf_usbd_UVC_get_control(uvc, entity, request->wValue);
            if (!control) { return VSF_ERR_FAIL; }

            if (    (request->bRequest == (USB_UVCREQ_SET | USB_UVCREQ_CUR))
                &&  (control->info->on_set != NULL)) {
                control->info->on_set(control);
            }
            break;
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
