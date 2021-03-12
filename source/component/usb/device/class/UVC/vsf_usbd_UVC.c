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

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_UVC == ENABLED

#define __VSF_USBD_CLASS_INHERIT__
#define __VSF_USBD_UVC_CLASS_IMPLEMENT

#include "../../vsf_usbd.h"
#include "./vsf_usbd_UVC.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_usbd_uvc_stop_stream(vk_usbd_uvc_t *uvc, uint_fast8_t ifs);
extern void vsf_usbd_uvc_start_stream(vk_usbd_uvc_t *uvc, uint_fast8_t ifs);

static vsf_err_t __vk_usbd_uvc_vc_class_init(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
static vsf_err_t __vk_usbd_uvc_vs_class_init(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
static vsf_err_t __vk_usbd_uvc_request_prepare(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
static vsf_err_t __vk_usbd_uvc_request_process(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbd_class_op_t vk_usbd_uvc_control_class = {
    .request_prepare =      __vk_usbd_uvc_request_prepare,
    .request_process =      __vk_usbd_uvc_request_process,
    .init =                 __vk_usbd_uvc_vc_class_init,
};

const vk_usbd_class_op_t vk_usbd_uvc_stream_class = {
    .request_prepare =      __vk_usbd_uvc_request_prepare,
    .request_process =      __vk_usbd_uvc_request_process,
    .init =                 __vk_usbd_uvc_vs_class_init,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_USBD_UVC_CFG_TRACE_EN == ENABLED
static char * __vk_usbd_uvc_trace_get_request(uint_fast8_t request)
{
    switch (request) {
    case USB_UVC_REQ_CUR:   return "CUR";
    case USB_UVC_REQ_MIN:   return "MIN";
    case USB_UVC_REQ_MAX:   return "MAX";
    case USB_UVC_REQ_RES:   return "RES";
    case USB_UVC_REQ_LEN:   return "LEN";
    case USB_UVC_REQ_INFO:  return "INFO";
    case USB_UVC_REQ_DEF:   return "DEF";
    default:                return "UNKNOWN";
    }
}

static void __vk_usbd_uvc_trace_request_prepare(vk_usbd_ctrl_handler_t *ctrl_handler)
{
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    uint_fast8_t req = request->bRequest;
    uint_fast8_t ifs_ep = (request->wIndex >> 0) & 0xFF;
    uint_fast8_t entity = (request->wIndex >> 8) & 0xFF;
    uint_fast8_t cs = (request->wValue >> 8) & 0xFF;
    bool is_get = req & USB_UVC_REQ_GET;

    if (    (USB_RECIP_INTERFACE == (request->bRequestType & USB_RECIP_MASK))
        &&  (USB_TYPE_CLASS == (request->bRequestType & USB_TYPE_MASK))) {

        vsf_trace_debug("uvc: %s%s ifs/ep=%d, entity=%d, cs=%d" VSF_TRACE_CFG_LINEEND,
                    is_get ? "GET_" : "SET_",
                    __vk_usbd_uvc_trace_get_request(req & ~USB_UVC_REQ_GET),
                    ifs_ep, entity, cs);
        if (is_get) {
            vsf_trace_buffer(VSF_TRACE_NONE, ctrl_handler->trans.buffer,
                    ctrl_handler->trans.size, VSF_TRACE_DF_DEFAULT);
        }
    }
}

static void __vk_usbd_uvc_trace_request_process(vk_usbd_ctrl_handler_t *ctrl_handler)
{
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    uint_fast8_t req = request->bRequest;
    bool is_get = req & USB_UVC_REQ_GET;

    if (    (USB_RECIP_INTERFACE == (request->bRequestType & USB_RECIP_MASK))
        &&  (USB_TYPE_CLASS == (request->bRequestType & USB_TYPE_MASK))) {

        if (!is_get) {
            vsf_trace_buffer(VSF_TRACE_NONE, ctrl_handler->trans.buffer,
                    ctrl_handler->trans.size, VSF_TRACE_DF_DEFAULT);
        }
    }
}
#endif

#ifndef WEAK_VSF_USBD_UVC_STOP_STREAM
WEAK(vsf_usbd_uvc_stop_stream)
void vsf_usbd_uvc_stop_stream(vk_usbd_uvc_t *uvc, uint_fast8_t ifs)
{
}
#endif

#ifndef WEAK_VSF_USBD_UVC_START_STREAM
WEAK(vsf_usbd_uvc_start_stream)
void vsf_usbd_uvc_start_stream(vk_usbd_uvc_t *uvc, uint_fast8_t ifs)
{
}
#endif

vsf_err_t vk_usbd_uvc_send_packet(vk_usbd_uvc_t *uvc, uint8_t *buffer, uint_fast32_t size)
{
    vk_usbd_trans_t *trans = &uvc->trans_in;

    trans->ep = uvc->ep_in;
    trans->feature = USB_DC_FEATURE_TRANSFER;
    trans->use_as__vsf_mem_t.buffer = buffer;
    trans->use_as__vsf_mem_t.size = size;
    trans->zlp = false;
    trans->eda = vsf_eda_get_cur();
    trans->notify_eda = true;
    return vk_usbd_ep_send(uvc->dev, trans);
}

static vk_usbd_uvc_control_t *__vk_usbd_uvc_get_control(
        vk_usbd_uvc_t *uvc, uint_fast8_t id, uint_fast8_t selector)
{
    vk_usbd_uvc_entity_t *entity = uvc->entity;
    vk_usbd_uvc_control_t *control;

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

static vk_av_control_value_t *__vk_usbd_uvc_get_value(
        vk_usbd_uvc_control_t *control, uint_fast8_t request)
{
    vk_av_control_value_t *value = NULL;
    request = request & ~USB_UVC_REQ_GET;
    switch (request) {
    case USB_UVC_REQ_CUR:   value = &control->cur; break;
    case USB_UVC_REQ_MIN:   value = (vk_av_control_value_t *)&control->info->min; break;
    case USB_UVC_REQ_MAX:   value = (vk_av_control_value_t *)&control->info->max; break;
    case USB_UVC_REQ_RES:
    case USB_UVC_REQ_LEN:
    case USB_UVC_REQ_INFO:  break;
    case USB_UVC_REQ_DEF:   value = (vk_av_control_value_t *)&control->info->def; break;
    }
    return value;
}

static vsf_err_t __vk_usbd_uvc_request_prepare(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_usbd_uvc_t *uvc = ifs->class_param;
    vk_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    const vk_usbd_uvc_control_info_t *cinfo;
    vk_usbd_uvc_control_t *control;
    vk_av_control_value_t *value;
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
#if VSF_USBD_UVC_CFG_TRACE_EN == ENABLED
                    vsf_trace_debug("uvc: stop stream." VSF_TRACE_CFG_LINEEND);
#endif
                    vsf_usbd_uvc_stop_stream(uvc, request->wValue);
                } else {
#if VSF_USBD_UVC_CFG_TRACE_EN == ENABLED
                    vsf_trace_debug("uvc: start stream %d." VSF_TRACE_CFG_LINEEND,
                                request->wValue);
#endif
                    vsf_usbd_uvc_start_stream(uvc, request->wValue);
                }
                break;
            }
            break;
        case USB_TYPE_CLASS: {
//            uint_fast8_t ifs_ep = (request->wIndex >> 0) & 0xFF;
            uint_fast8_t entity = (request->wIndex >> 8) & 0xFF;
            uint_fast8_t cs = (request->wValue >> 8) & 0xFF;

            control = __vk_usbd_uvc_get_control(uvc, entity, cs);
            if (!control) { return VSF_ERR_FAIL; }

            cinfo = control->info;
            value = __vk_usbd_uvc_get_value(control, request->bRequest);
            if (request->bRequest & USB_UVC_REQ_GET) {
                if (!value) {
                    switch (request->bRequest & ~USB_UVC_REQ_GET) {
                    case USB_UVC_REQ_LEN:
                        size = sizeof(control->info->size);
                        // TODO: code below only support little-endian
                        buffer = (uint8_t *)&cinfo->size;
                        break;
                    case USB_UVC_REQ_RES:
                    case USB_UVC_REQ_INFO:
                        return VSF_ERR_FAIL;
                    }
                } else {
                    size = cinfo->size;
                    buffer = size < 4 ? &value->uval32 : value->buffer;
                }
            } else {
                if (request->bRequest != (USB_UVC_REQ_SET | USB_UVC_REQ_CUR)) {
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
    ctrl_handler->trans.use_as__vsf_mem_t.buffer = buffer;
    ctrl_handler->trans.use_as__vsf_mem_t.size = size;
#if VSF_USBD_UVC_CFG_TRACE_EN == ENABLED
    uvc->cur_size = size;
    __vk_usbd_uvc_trace_request_prepare(ctrl_handler);
#endif
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_usbd_uvc_request_process(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_usbd_uvc_t *uvc = ifs->class_param;
    vk_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    vk_usbd_uvc_control_t *control;

#if VSF_USBD_UVC_CFG_TRACE_EN == ENABLED
    ctrl_handler->trans.size = uvc->cur_size;
    __vk_usbd_uvc_trace_request_process(ctrl_handler);
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
        case USB_TYPE_CLASS:
            {
//                uint_fast8_t ifs_ep = (request->wIndex >> 0) & 0xFF;
                uint_fast8_t entity = (request->wIndex >> 8) & 0xFF;
                uint_fast8_t cs = (request->wValue >> 8) & 0xFF;

                control = __vk_usbd_uvc_get_control(uvc, entity, cs);
                if (!control) { return VSF_ERR_FAIL; }

                if (    (request->bRequest == (USB_UVC_REQ_SET | USB_UVC_REQ_CUR))
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

static vsf_err_t __vk_usbd_uvc_vc_class_init(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_usbd_uvc_t *uvc = ifs->class_param;

    uvc->ifs = ifs;
    uvc->dev = dev;
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_usbd_uvc_vs_class_init(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
//    vk_usbd_uvc_t *uvc = ifs->class_param;

//    TODO
    return VSF_ERR_NONE;
}

#endif      // VSF_USE_USB_DEVICE && VSF_USBD_USE_UVC
