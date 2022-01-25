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

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_UAC == ENABLED

#define __VSF_USBD_CLASS_INHERIT__
#define __VSF_USBD_UAC_CLASS_IMPLEMENT
#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__

// for stream
#include "service/vsf_service.h"
#include "../../vsf_usbd.h"
#include "./vsf_usbd_UAC.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_usbd_uac_stop_stream(vk_usbd_uac_as_t *uac_as, uint_fast8_t ifs);
extern void vsf_usbd_uac_start_stream(vk_usbd_uac_as_t *uac_as, uint_fast8_t ifs);

static vsf_err_t __vk_usbd_uac_ac_class_init(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
static vsf_err_t __vk_usbd_uac_ac_request_prepare(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
static vsf_err_t __vk_usbd_uac_ac_request_process(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);

static vsf_err_t __vk_usbd_uac_as_class_init(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
static vsf_err_t __vk_usbd_uac_as_request_prepare(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
static vsf_err_t __vk_usbd_uac_as_request_process(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbd_class_op_t vk_usbd_uac_control_class = {
    .request_prepare =      __vk_usbd_uac_ac_request_prepare,
    .request_process =      __vk_usbd_uac_ac_request_process,
    .init =                 __vk_usbd_uac_ac_class_init,
};

const vk_usbd_class_op_t vk_usbd_uac_stream_class = {
    .request_prepare =      __vk_usbd_uac_as_request_prepare,
    .request_process =      __vk_usbd_uac_as_request_process,
    .init =                 __vk_usbd_uac_as_class_init,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static bool __vk_usbd_uac_is_get(vk_usbd_uac_ac_t *uac_ac, vk_usbd_ctrl_handler_t *ctrl_handler)
{
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
#if VSF_USBD_UAC_CFG_UAC1_EN == ENABLED
    if (uac_ac->version == VK_USB_UAC1) {
        return request->bRequest & USB_UAC_REQ_GET;
    } else
#endif
#if VSF_USBD_UAC_CFG_UAC2_EN == ENABLED
    if (uac_ac->version == VK_USB_UAC2) {
        return (request->bRequestType & USB_DIR_MASK) == USB_DIR_IN;
    }
#endif
    else {
        VSF_USB_ASSERT(0);
        return false;
    }
}

#if VSF_USBD_UAC_CFG_TRACE_EN == ENABLED
static char * __vk_usbd_uac_trace_get_request(vk_usbd_uac_ac_t *uac_ac, uint_fast8_t request)
{
#if VSF_USBD_UAC_CFG_UAC1_EN == ENABLED
    if (uac_ac->version == VK_USB_UAC1) {
        switch (request & ~USB_UAC_REQ_GET) {
        case USB_UAC_REQ_CUR:   return "CUR";
        case USB_UAC_REQ_MIN:   return "MIN";
        case USB_UAC_REQ_MAX:   return "MAX";
        case USB_UAC_REQ_RES:   return "RES";
        }
    } else
#endif
#if VSF_USBD_UAC_CFG_UAC2_EN == ENABLED
    if (uac_ac->version == VK_USB_UAC2) {
        switch (request) {
        case USB_UAC2_REQ_CUR:      return "CUR";
        case USB_UAC2_REQ_RANGE:    return "RANGE";
        case USB_UAC2_REQ_MEM:      return "MEM";
        }
    }
#endif
    else {
        VSF_USB_ASSERT(0);
    }

    return "UNKNOWN";
}

static void __vk_usbd_uac_trace_request_prepare(vk_usbd_uac_ac_t *uac_ac, vk_usbd_ctrl_handler_t *ctrl_handler)
{
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    uint_fast8_t ifs_ep = (request->wIndex >> 0) & 0xFF;
    uint_fast8_t entity = (request->wIndex >> 8) & 0xFF;
    uint_fast8_t cs = (request->wValue >> 8) & 0xFF;
    uint_fast8_t channel = (request->wValue >> 0) & 0xFF;
	bool is_get = __vk_usbd_uac_is_get(uac_ac, ctrl_handler);

    if (    (USB_RECIP_INTERFACE == (request->bRequestType & USB_RECIP_MASK))
        &&  (USB_TYPE_CLASS == (request->bRequestType & USB_TYPE_MASK))) {

        vsf_trace_debug("uac: %s%s ifs/ep=%d, entity=%d, cs=%d, channel=%d" VSF_TRACE_CFG_LINEEND,
                    is_get ? "GET_" : "SET_",
                    __vk_usbd_uac_trace_get_request(uac_ac, request->bRequest),
                    ifs_ep, entity, cs, channel);
        if (is_get) {
            vsf_trace_buffer(VSF_TRACE_NONE, ctrl_handler->trans.buffer,
                    ctrl_handler->trans.size, VSF_TRACE_DF_DEFAULT);
        }
    }
}

static void __vk_usbd_uac_trace_request_process(vk_usbd_uac_ac_t *uac_ac, vk_usbd_ctrl_handler_t *ctrl_handler)
{
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    bool is_get = __vk_usbd_uac_is_get(uac_ac, ctrl_handler);

    if (    (USB_RECIP_INTERFACE == (request->bRequestType & USB_RECIP_MASK))
        &&  (USB_TYPE_CLASS == (request->bRequestType & USB_TYPE_MASK))) {

        if (!is_get) {
            vsf_trace_buffer(VSF_TRACE_NONE, ctrl_handler->trans.buffer,
                    ctrl_handler->trans.size, VSF_TRACE_DF_DEFAULT);
        }
    }
}
#endif

#ifndef WEAK_VSF_USBD_UAC_STOP_STREAM
WEAK(vsf_usbd_uac_stop_stream)
void vsf_usbd_uac_stop_stream(vk_usbd_uac_as_t *uac_as, uint_fast8_t ifs)
{
}
#endif

#ifndef WEAK_VSF_USBD_UAC_START_STREAM
WEAK(vsf_usbd_uac_start_stream)
void vsf_usbd_uac_start_stream(vk_usbd_uac_as_t *uac_as, uint_fast8_t ifs)
{
}
#endif

static vk_usbd_uac_control_t *__vk_usbd_uac_get_control(vk_usbd_uac_ac_t *uac_ac,
        uint_fast8_t id, uint_fast8_t selector, uint_fast8_t channel)
{
    vk_usbd_uac_entity_t *entity = uac_ac->entity;
    vk_usbd_uac_control_t *control;

    for (int i = 0; i < uac_ac->entity_num; i++, entity++) {
        if (entity->id == id) {
            control = entity->control;
            // TODO: if control_num is 1, cs from wValue maybe invalid
            for (int j = 0; j < entity->control_num; j++, control++) {
                if (    (control->info->selector == selector)
                    &&  (control->info->channel == channel)) {
                    return control;
                }
            }
            break;
        }
    }
    return NULL;
}

static vk_av_control_value_t *__vk_usbd_uac_get_value(vk_usbd_uac_ac_t *uac_ac,
        vk_usbd_uac_control_t *control, uint_fast8_t request)
{
    vk_av_control_value_t *value = NULL;
    request = request & ~USB_UAC_REQ_GET;
#if VSF_USBD_UAC_CFG_UAC1_EN == ENABLED
    if (uac_ac->version == VK_USB_UAC1) {
        switch (request) {
        case USB_UAC_REQ_CUR:   value = &control->cur; break;
        case USB_UAC_REQ_MIN:   value = (vk_av_control_value_t *)&control->info->min; break;
        case USB_UAC_REQ_MAX:   value = (vk_av_control_value_t *)&control->info->max; break;
        case USB_UAC_REQ_RES:   value = (vk_av_control_value_t *)&control->info->res; break;
        }
    } else
#endif
#if VSF_USBD_UAC_CFG_UAC2_EN == ENABLED
    if (uac_ac->version == VK_USB_UAC2) {
        switch (request) {
        case USB_UAC2_REQ_CUR:   value = &control->cur; break;
        case USB_UAC2_REQ_RANGE: value = (vk_av_control_value_t *)&control->info->res; break;
        case USB_UAC2_REQ_MEM:   VSF_USB_ASSERT(0); break;  // TODO: support req memory
        }
    }
#endif
    else {
        VSF_USB_ASSERT(0);
    }
    return value;
}

static vsf_err_t __vk_usbd_uac_ac_request_prepare(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_usbd_uac_ac_t *uac_ac = ifs->class_param;
    vk_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    vk_usbd_uac_control_t *control;
    vk_av_control_value_t *value;
    uint8_t *buffer = NULL;
    uint_fast32_t size = 0;

    switch (request->bRequestType & USB_RECIP_MASK) {
    case USB_RECIP_INTERFACE:
        switch (request->bRequestType & USB_TYPE_MASK) {
        case USB_TYPE_CLASS: {
//                uint_fast8_t ifs_ep = (request->wIndex >> 0) & 0xFF;
                uint_fast8_t entity = (request->wIndex >> 8) & 0xFF;
                uint_fast8_t cs = (request->wValue >> 8) & 0xFF;
                uint_fast8_t channel = (request->wValue >> 0) & 0xFF;

                control = __vk_usbd_uac_get_control(uac_ac, entity, cs, channel);
                if (!control) { return VSF_ERR_FAIL; }

#if VSF_USBD_UAC_CFG_UAC1_EN == ENABLED
                if (uac_ac->version == VK_USB_UAC1) {
                    size = control->info->size;
                    value = __vk_usbd_uac_get_value(uac_ac, control, request->bRequest);
                    buffer = size > sizeof(*value) ? value->buffer : (uint8_t *)value;
                    // TODO: code below only support little-endian

                    if (request->bRequest & USB_UAC_REQ_GET) {
                        VSF_USB_ASSERT(value != NULL);
                    } else {
                        if (request->bRequest != (USB_UAC_REQ_SET | USB_UAC_REQ_CUR)) {
                            return VSF_ERR_FAIL;
                        }

#if VSF_USBD_UAC_WORKAROUND_CONTROL_OVERFLOW == ENABLED
                        if (request->wLength > size) {
                            VSF_USB_ASSERT(request->wLength <= sizeof(uac_ac->control_value));
                            size = request->wLength;
                            buffer = (uint8_t *)&uac_ac->control_value;
                        }
#endif
                    }
                } else
#endif
#if VSF_USBD_UAC_CFG_UAC2_EN == ENABLED
                if (uac_ac->version == VK_USB_UAC2) {
                    const vk_usbd_uac2_control_info_t *info2 = control->info2;
                    const vk_usbd_uac2_range_t * range = info2->range;
                    VSF_USB_ASSERT(range != NULL);
                    size = info2->size;

                    switch (request->bRequest) {
                    case USB_UAC2_REQ_CUR:
                        buffer = (uint8_t *)&control->cur;
                        break;
                    case USB_UAC2_REQ_RANGE:
                        // Audio20 final.pdf 5.2.3 Offset 0: wNumSubRanges
                        buffer = (uint8_t *)&range->number;
                        // 3: MIN(n), MAX(n), RES(n)
                        size = sizeof(info2->range->number) + size * info2->range->number * 3;
                        break;
                    case USB_UAC2_REQ_MEM:	//TODO: support request memory
                    default:
                        VSF_USB_ASSERT(0);
                        return VSF_ERR_FAIL;
                    }
                }
#endif
                else {
                    VSF_USB_ASSERT(0);
                    return VSF_ERR_FAIL;
                }
            }
            break;
        }
        break;
    }
    ctrl_handler->trans.buffer = buffer;
    ctrl_handler->trans.size = size;
#if VSF_USBD_UAC_CFG_TRACE_EN == ENABLED
    uac_ac->cur_size = size;
    __vk_usbd_uac_trace_request_prepare(uac_ac, ctrl_handler);
#endif
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_usbd_uac_ac_request_process(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_usbd_uac_ac_t *uac_ac = ifs->class_param;
    vk_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    vk_usbd_uac_control_t *control;

#if VSF_USBD_UAC_CFG_TRACE_EN == ENABLED
    ctrl_handler->trans.size = uac_ac->cur_size;
    __vk_usbd_uac_trace_request_process(uac_ac, ctrl_handler);
#endif
    switch (request->bRequestType & USB_RECIP_MASK) {
    case USB_RECIP_INTERFACE:
        switch (request->bRequestType & USB_TYPE_MASK) {
        case USB_TYPE_CLASS: {
//                uint_fast8_t ifs_ep = (request->wIndex >> 0) & 0xFF;
                uint_fast8_t entity = (request->wIndex >> 8) & 0xFF;
                uint_fast8_t cs = (request->wValue >> 8) & 0xFF;
                uint_fast8_t channel = (request->wValue >> 0) & 0xFF;

                control = __vk_usbd_uac_get_control(uac_ac, entity, cs, channel);
                if (!control) { return VSF_ERR_FAIL; }

                if (uac_ac->version == VK_USB_UAC1) {
                    if (request->bRequest == (USB_UAC_REQ_SET | USB_UAC_REQ_CUR)) {
    #if VSF_USBD_UAC_WORKAROUND_CONTROL_OVERFLOW == ENABLED
                        vk_av_control_value_t *value = __vk_usbd_uac_get_value(uac_ac, control, request->bRequest);
                        uint_fast32_t size = control->info->size;
                        if (request->wLength > size) {
                            uint8_t *cbuffer = size > sizeof(*value) ? value->buffer : (uint8_t *)value;
                            memcpy(cbuffer, &uac_ac->control_value, size);
                        }
    #endif
                        if (control->info->on_set != NULL) {
                            control->info->on_set(control);
                        }
                    }
                } else if (uac_ac->version == VK_USB_UAC2) {
                    if (   __vk_usbd_uac_is_get(uac_ac, ctrl_handler)
                        && (request->bRequest == USB_UAC2_REQ_CUR)) {
                        if (control->info2->on_set != NULL) {
                            control->info2->on_set(control);
                        }
                    }
                } else {
                    VSF_USB_ASSERT(0);
                    return VSF_ERR_FAIL;
                }
                break;
            }
        }
        break;
    }
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_usbd_uac_ac_class_init(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_usbd_uac_ac_t *uac_ac = ifs->class_param;

    uac_ac->ifs = ifs;
    uac_ac->dev = dev;

#if (VSF_USBD_UAC_CFG_UAC1_EN == ENABLED) && (VSF_USBD_UAC_CFG_UAC2_EN == ENABLED)
    vk_usbd_desc_t *desc = vk_usbd_get_descriptor(dev->desc, dev->num_of_desc, USB_DT_DEVICE, 0, 0);
    VSF_USB_ASSERT(desc != NULL);
    struct usb_device_desc_t *desc_dev = (struct usb_device_desc_t *)desc->buffer;
    if (        (desc_dev->bDeviceClass    == 0xEF)
             && (desc_dev->bDeviceSubClass == 0x02)
             && (desc_dev->bDeviceProtocol == 0x01)) {      // Audio20 final.pdf 4.2 Device Descriptor
        uac_ac->version = VK_USB_UAC2;
    } else if ( (desc_dev->bDeviceClass    == 0x00)
             && (desc_dev->bDeviceSubClass == 0x00)
             && (desc_dev->bDeviceProtocol == 0x00)) {      // audio10.pdf 4.1 Device Descriptor
        uac_ac->version = VK_USB_UAC1;
    } else {
        return VSF_ERR_INVALID_PARAMETER;
    }
#endif

    return VSF_ERR_NONE;
}

static void __vk_usbd_uac_as_evthandler(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt)
{
    vk_usbd_uac_as_t *uac_as = param;
    vk_usbd_trans_t *trans = &uac_as->trans;
    bool is_in = (USB_DIR_IN == (uac_as->ep & USB_DIR_MASK));
    uint_fast32_t size;
    vsf_protect_t orig;

    switch (evt) {
    case VSF_STREAM_ON_CONNECT:
        if (is_in) {
            goto check_rx;
        } else {
            goto check_tx;
        }
        break;
    case VSF_STREAM_ON_RX:
        VSF_USB_ASSERT(is_in);
    check_rx:
        orig = vsf_protect_int();
            if (!uac_as->is_submitted) {
                size = vsf_stream_get_rbuf(stream, &trans->buffer);
                if (size >= uac_as->packet_size) {
                    uac_as->is_submitted = true;
                    vsf_unprotect_int(orig);

                    trans->size = uac_as->packet_size;
                    vk_usbd_ep_send(uac_as->uac_ac->dev, trans);
                    return;
                }
            }
        vsf_unprotect_int(orig);
        break;
    case VSF_STREAM_ON_TX:
        VSF_USB_ASSERT(!is_in);
    check_tx:
        orig = vsf_protect_int();
            if (!uac_as->is_submitted) {
                size = vsf_stream_get_wbuf(stream, &trans->buffer);
                if (size >= uac_as->packet_size) {
                    uac_as->is_submitted = true;
                    vsf_unprotect_int(orig);

                    trans->size = uac_as->packet_size;
                    vk_usbd_ep_recv(uac_as->uac_ac->dev, trans);
                    return;
                }
            }
        vsf_unprotect_int(orig);
        break;
    }
}

static void __vk_usbd_uac_as_on_finish(void *param)
{
    vk_usbd_uac_as_t *uac_as = param;
    vsf_stream_t *stream = uac_as->stream;
    bool is_in = (USB_DIR_IN == (uac_as->ep & USB_DIR_MASK));
    vsf_protect_t orig;

    if (is_in) {
        if (!uac_as->trans.size) {
            vsf_stream_read(stream, NULL, uac_as->packet_size);
        }
        orig = vsf_protect_int();
            uac_as->is_submitted = false;
        vsf_unprotect_int(orig);
        __vk_usbd_uac_as_evthandler(uac_as->stream, uac_as, VSF_STREAM_ON_RX);
    } else {
        if (!uac_as->trans.size) {
            vsf_stream_write(stream, NULL, uac_as->packet_size);
        }
        orig = vsf_protect_int();
            uac_as->is_submitted = false;
        vsf_unprotect_int(orig);
        __vk_usbd_uac_as_evthandler(uac_as->stream, uac_as, VSF_STREAM_ON_TX);
    }
}

static vsf_err_t __vk_usbd_uac_as_request_prepare(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_usbd_uac_as_t *uac_as = ifs->class_param;
    vk_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
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
#if VSF_USBD_UAC_CFG_TRACE_EN == ENABLED
                    vsf_trace_debug("uac: stop stream." VSF_TRACE_CFG_LINEEND);
#endif
                    vsf_usbd_uac_stop_stream(uac_as, request->wValue);
                } else {
#if VSF_USBD_UAC_CFG_TRACE_EN == ENABLED
                    vsf_trace_debug("uac: start stream %d." VSF_TRACE_CFG_LINEEND,
                                request->wValue);
#endif
                    vsf_usbd_uac_start_stream(uac_as, request->wValue);
                }
                break;
            }
            break;
        }
    case USB_RECIP_ENDPOINT:
        break;
    }
    ctrl_handler->trans.buffer = buffer;
    ctrl_handler->trans.size = size;
#if VSF_USBD_UAC_CFG_TRACE_EN == ENABLED
    uac_as->cur_size = size;
    __vk_usbd_uac_trace_request_prepare(uac_as->uac_ac, ctrl_handler);
#endif
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_usbd_uac_as_request_process(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;

#if VSF_USBD_UAC_CFG_TRACE_EN == ENABLED
    vk_usbd_uac_as_t *uac_as = ifs->class_param;
    ctrl_handler->trans.size = uac_as->cur_size;
    __vk_usbd_uac_trace_request_process(uac_as->uac_ac, ctrl_handler);
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
        }
        break;
    case USB_RECIP_ENDPOINT:
        break;
    }
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_usbd_uac_as_class_init(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_usbd_uac_as_t *uac_as = ifs->class_param;
    vk_usbd_trans_t *trans = &uac_as->trans;

    trans->ep = uac_as->ep;
    trans->feature = USB_DC_FEATURE_TRANSFER;
    trans->zlp = false;
    trans->notify_eda = false;
    trans->param = uac_as;
    trans->on_finish = __vk_usbd_uac_as_on_finish;

    uac_as->is_submitted = false;
    if (uac_as->stream != NULL) {
        vk_usbd_uac_connect_stream(uac_as, uac_as->stream);
    }
    return VSF_ERR_NONE;
}

void vk_usbd_uac_connect_stream(vk_usbd_uac_as_t *uac_as, vsf_stream_t *stream)
{
    VSF_USB_ASSERT(stream != NULL);
    uac_as->stream = stream;
    if (USB_DIR_IN == (uac_as->ep & USB_DIR_MASK)) {
        uac_as->stream->rx.param = uac_as;
        uac_as->stream->rx.evthandler = __vk_usbd_uac_as_evthandler;
        vsf_stream_connect_rx(uac_as->stream);
    } else {
        uac_as->stream->tx.param = uac_as;
        uac_as->stream->tx.evthandler = __vk_usbd_uac_as_evthandler;
        vsf_stream_connect_tx(uac_as->stream);
    }
}

void vk_usbd_uac_disconnect_stream(vk_usbd_uac_as_t *uac_as)
{
    if (USB_DIR_IN == (uac_as->ep & USB_DIR_MASK)) {
        vsf_stream_disconnect_rx(uac_as->stream);
    } else {
        vsf_stream_disconnect_tx(uac_as->stream);
    }
    uac_as->stream = NULL;
}

#endif      // VSF_USE_USB_DEVICE && VSF_USBD_USE_UAC
