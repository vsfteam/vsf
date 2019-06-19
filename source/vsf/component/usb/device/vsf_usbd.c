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

#define VSF_USBD_IMPLEMENT
#define VSFSTREAM_CLASS_IMPLEMENT
#define __VSF_STREAM_BASE_CLASS_INHERIT

#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static void vsf_usbd_hal_evt_handler(void *, usb_evt_t, uint_fast32_t);

/*============================ IMPLEMENTATION ================================*/

vsf_usbd_desc_t * vsf_usbd_get_descriptor(vsf_usbd_desc_t *desc,
        uint_fast8_t desc_num, uint_fast8_t type,
        uint_fast8_t index, uint_fast16_t lanid)
{
    ASSERT(desc != NULL);
    for (uint_fast8_t i = 0; i < desc_num; i++) {
        if (    (desc->type == type)
            &&  (desc->index == index)
            &&  (desc->lanid == lanid)) {
            return desc;
        }
        desc++;
    }
    return 0;
}

static vsf_usbd_trans_t * vsf_usbd_get_trans(vsf_usbd_dev_t *dev, uint_fast8_t ep)
{
    vsf_usbd_trans_t *trans;

    vsf_slist_peek_next(vsf_usbd_trans_t, node, &dev->trans_list, trans);
    while (trans != NULL) {
        if (trans->ep == ep) {
            break;
        }
        vsf_slist_peek_next(vsf_usbd_trans_t, node, &trans->node, trans);
    }
    return trans;
}

static void vsf_usbd_trans_finish(vsf_usbd_dev_t *dev, vsf_usbd_trans_t *trans)
{
    vsf_slist_remove(vsf_usbd_trans_t, node, &dev->trans_list, trans);
    if (trans->notify_eda) {
        vsf_eda_post_msg(trans->eda, trans);
    } else if (trans->on_finish != NULL) {
        trans->on_finish(trans->param);
    }
}

vsf_err_t vsf_usbd_ep_recv(vsf_usbd_dev_t *dev, vsf_usbd_trans_t *trans)
{
    ASSERT( (dev != NULL) && (trans != NULL));
    VSF_USBD_DRV_PREPARE(dev);
    vsf_err_t err;

    trans->ep &= ~USB_DIR_MASK;
    if (vsf_usbd_drv_ep_is_dma) {
        uint_fast32_t size = trans->nSize;
        trans->nSize = 0;
        err = vsf_usbd_drv_ep_read_buffer(trans->ep, trans->pchBuffer, size);
    } else {
        trans->cur = trans->pchBuffer;
        err = vsf_usbd_drv_ep_enable_out(trans->ep);
    }
    if (VSF_ERR_NONE == err) {
        vsf_slist_init_node(vsf_usbd_trans_t, node, trans);
        vsf_slist_add_to_head(vsf_usbd_trans_t, node, &dev->trans_list, trans);
    }
    return err;
}

static vsf_err_t vsf_usbd_ep_send_imp(vsf_usbd_dev_t *dev, vsf_usbd_trans_t *trans)
{
    uint_fast8_t ep = trans->ep;
    VSF_USBD_DRV_PREPARE(dev);
    uint_fast16_t ep_size = vsf_usbd_drv_ep_get_size(ep);
    uint_fast16_t pkg_size = min(ep_size, trans->nSize);

    vsf_usbd_drv_ep_write_buffer(ep, trans->cur, pkg_size);
    trans->cur += pkg_size;
    trans->nSize -= pkg_size;
    if (!trans->nSize && (pkg_size < ep_size)) {
        trans->zlp = false;
    }

    return vsf_usbd_drv_ep_set_data_size(ep, pkg_size);
}

vsf_err_t vsf_usbd_ep_send(vsf_usbd_dev_t *dev, vsf_usbd_trans_t *trans)
{
    ASSERT((dev != NULL) && (trans != NULL));
    VSF_USBD_DRV_PREPARE(dev);
    vsf_err_t err;

    trans->ep |= USB_DIR_MASK;
    if (vsf_usbd_drv_ep_is_dma) {
        uint_fast32_t size = trans->nSize;
        trans->nSize = 0;
        err = vsf_usbd_drv_ep_write_buffer(trans->ep, trans->pchBuffer, size);
    } else {
        trans->cur = trans->pchBuffer;
        err = vsf_usbd_ep_send_imp(dev, trans);
    }
    if (VSF_ERR_NONE == err) {
        vsf_slist_init_node(vsf_usbd_trans_t, node, trans);
        vsf_slist_add_to_head(vsf_usbd_trans_t, node, &dev->trans_list, trans);
    }
    return err;
}

void vsf_usbd_cfg_fini(vsf_usbd_dev_t *dev)
{
    vsf_usbd_cfg_t *config = vsf_usbd_get_cur_cfg(dev);

    vsf_usbd_ifs_t *ifs = config->ifs;
    for (uint_fast8_t i = 0; i < config->num_of_ifs; i++, ifs++) {
        if (ifs->is_inited) {
            ifs->is_inited = false;
            if (ifs->class_op->fini != NULL) {
                ifs->class_op->fini(dev, ifs);
            }
        }
    }
}

// standard request handlers
static int16_t vsf_usbd_get_config(vsf_usbd_dev_t *dev, uint_fast8_t value)
{
    for (uint_fast8_t i = 0; i < dev->num_of_config; i++) {
        if (value == dev->config[i].configuration_value) {
            return i;
        }
    }
    return -1;
}

#if VSF_USBD_CFG_AUTOSETUP == ENABLED
static vsf_err_t vsf_usbd_auto_init(vsf_usbd_dev_t *dev)
{
    ASSERT(dev != NULL);
    VSF_USBD_DRV_PREPARE(dev);
    vsf_usbd_cfg_t *config;
    vsf_usbd_desc_t *desc;
    struct usb_config_descriptor_t *desc_config;

    uint_fast16_t pos, cur_ifs;
    uint_fast8_t attr, feature;

    config = &dev->config[dev->configuration];

    // config other eps according to descriptors
    desc = vsf_usbd_get_descriptor(dev->desc, dev->num_of_desc, USB_DT_CONFIG, dev->configuration, 0);
    ASSERT(desc != NULL);
    desc_config = (struct usb_config_descriptor_t *)desc->buffer;
    ASSERT(     (desc->size == desc_config->wTotalLength)
           &&   (desc_config->bLength == USB_DT_CONFIG_SIZE)
           &&   (desc_config->bDescriptorType == USB_DT_CONFIG)
           &&   (desc_config->bNumInterfaces == config->num_of_ifs));

    // initialize device feature according to
    // bmAttributes field in configuration descriptor
    attr = desc_config->bmAttributes;
    feature = 0;
    if (attr & USB_CONFIG_ATT_SELFPOWER) {
        feature |= 1 << USB_DEVICE_SELF_POWERED;
    }
    if (attr & USB_CONFIG_ATT_WAKEUP) {
        feature |= 1 << USB_DEVICE_REMOTE_WAKEUP;
    }

    cur_ifs = -1;
    pos = USB_DT_CONFIG_SIZE;
    while (desc->size > pos) {
        struct usb_endpoint_descriptor_t *desc_header =
            (struct usb_endpoint_descriptor_t *)((uint8_t *)desc_config + pos);
        ASSERT((desc_header->bLength > 2) && (desc->size >= (pos + desc_header->bLength)));

        switch (desc_header->bDescriptorType) {
        case USB_DT_INTERFACE:
            {
                struct usb_interface_descriptor_t *desc_ifs =
                        (struct usb_interface_descriptor_t *)desc_header;
                cur_ifs = desc_ifs->bInterfaceNumber;
                break;
            }
        case USB_DT_ENDPOINT:
            {
                struct usb_endpoint_descriptor_t *desc_ep =
                        (struct usb_endpoint_descriptor_t *)desc_header;
                uint_fast8_t ep_addr = desc_ep->bEndpointAddress;
                uint_fast8_t ep_attr = desc_ep->bmAttributes;
                uint_fast16_t ep_size = desc_ep->wMaxPacketSize;

                ASSERT((ep_attr & 0x03) <= 3);
                if (VSF_ERR_NONE != vsf_usbd_drv_ep_add(ep_addr, ep_attr & 0x03, ep_size)) {
                    return VSF_ERR_FAIL;
                }

                ep_addr = (ep_addr & 0x0F) | ((ep_addr & 0x80) >> 3);
                config->ep_ifs_map[ep_addr] = cur_ifs;
                break;
            }
        }
        pos += desc_header->bLength;
    }
    return VSF_ERR_NONE;
}
#endif    // VSF_USBD_CFG_AUTOSETUP

static vsf_err_t vsf_usbd_stdctrl_prepare(vsf_usbd_dev_t *dev)
{
    VSF_USBD_DRV_PREPARE(dev);
    vsf_usbd_cfg_t *config = &dev->config[dev->configuration];
    vsf_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    uint8_t *buffer = ctrl_handler->reply_buffer;
    uint_fast8_t recip = request->bRequestType & USB_RECIP_MASK;
    uint_fast16_t size = 0;

    if (USB_RECIP_DEVICE == recip) {
        switch (request->bRequest) {
        case USB_REQ_GET_STATUS:
            if ((request->wValue != 0) || (request->wIndex != 0)) {
                return VSF_ERR_FAIL;
            }
            buffer[0] = dev->feature;
            buffer[1] = 0;
            size = 2;
            break;
        case USB_REQ_CLEAR_FEATURE:
            if (    (request->wIndex != 0)
                ||  (request->wValue != USB_DEVICE_REMOTE_WAKEUP)) {
                return VSF_ERR_FAIL;
            }
            dev->feature &= ~USB_CONFIG_ATT_WAKEUP;
            break;
        case USB_REQ_SET_FEATURE:
            if (    (request->wIndex != 0)
                ||  (request->wValue != USB_DEVICE_REMOTE_WAKEUP)) {
                return VSF_ERR_FAIL;
            }
            dev->feature |= USB_CONFIG_ATT_WAKEUP;
            break;
        case USB_REQ_SET_ADDRESS:
            if (    (request->wValue > 127)
                ||  (request->wIndex != 0)
                ||  (dev->configuration != 0)) {
                return VSF_ERR_FAIL;
            }
            break;
        case USB_REQ_GET_DESCRIPTOR:
            {
                uint_fast8_t type = (request->wValue >> 8) & 0xFF;
                uint_fast8_t index = request->wValue & 0xFF;
                uint_fast16_t lanid = request->wIndex;
                vsf_usbd_desc_t *desc = vsf_usbd_get_descriptor(dev->desc,
                        dev->num_of_desc, type, index, lanid);

                if (NULL == desc) {
                    return VSF_ERR_FAIL;
                }
                buffer = desc->buffer;
                size = desc->size;
            }
            break;
        case USB_REQ_GET_CONFIGURATION:
            if ((request->wValue != 0) || (request->wIndex != 0)) {
                return VSF_ERR_FAIL;
            }
            buffer[0] = config->configuration_value;
            size = 1;
            break;
        case USB_REQ_SET_CONFIGURATION:
            if ((request->wIndex != 0) || (vsf_usbd_get_config(dev, request->wValue) < 0)) {
                return VSF_ERR_FAIL;
            }
            dev->configured = false;
            break;
        default:
            return VSF_ERR_FAIL;
        }
    } else if (USB_RECIP_INTERFACE == recip) {
        uint_fast8_t ifs_idx = request->wIndex;
        vsf_usbd_ifs_t *ifs = &config->ifs[ifs_idx];
        const vsf_usbd_class_op_t *class_op = ifs->class_op;

        if (ifs_idx >= config->num_of_ifs) {
            return VSF_ERR_FAIL;
        }

        switch (request->bRequest) {
        case USB_REQ_GET_STATUS:
            if ((request->wValue != 0) || (request->wIndex >= config->num_of_ifs)) {
                return VSF_ERR_FAIL;
            }
            buffer[0] = 0;
            buffer[1] = 0;
            size = 2;
            break;
        case USB_REQ_CLEAR_FEATURE:
            break;
        case USB_REQ_SET_FEATURE:
            break;
        case USB_REQ_GET_DESCRIPTOR:
            {
                uint_fast8_t type = (request->wValue >> 8) & 0xFF;
                uint_fast8_t index = request->wValue & 0xFF;
                vsf_usbd_desc_t *desc = NULL;

                if ((class_op != NULL) && (class_op->get_desc != NULL)) {
                    desc = class_op->get_desc(dev, type, index, 0);
                }
                if (NULL == desc) {
                    return VSF_ERR_FAIL;
                }
                buffer = desc->buffer;
                size = desc->size;
            }
            break;
        case USB_REQ_GET_INTERFACE:
            if (request->wValue != 0) {
                return VSF_ERR_FAIL;
            }
            buffer[0] = ifs->alternate_setting;
            size = 1;
            break;
        case USB_REQ_SET_INTERFACE:
            ifs->alternate_setting = request->wValue;
            if (class_op->request_prepare != NULL) {
                class_op->request_prepare(dev, ifs);
            }
            break;
        default:
            return VSF_ERR_FAIL;
        }
    } else if (USB_RECIP_ENDPOINT == recip) {
        uint_fast8_t ep = request->wIndex & 0xFF;
        uint_fast8_t ep_num = ep & 0x7F;

        if (    ((request->bRequestType & USB_DIR_MASK) == USB_DIR_IN)
            ||  (ep_num >= vsf_usbd_drv_ep_number)){
            return VSF_ERR_FAIL;
        }

        switch (request->bRequest) {
        case USB_REQ_GET_STATUS:
            if (request->wValue != 0) {
                return VSF_ERR_FAIL;
            }
            if (vsf_usbd_drv_ep_is_stalled(ep)) {
                buffer[0] = 1;
            } else {
                buffer[0] = 0;
            }
            buffer[1] = 0;
            size = 2;
            break;
        case USB_REQ_CLEAR_FEATURE:
            if (request->wValue != USB_ENDPOINT_HALT) {
                return VSF_ERR_FAIL;
            }
            vsf_usbd_drv_ep_clear_stall(ep);
            if ((ep & USB_DIR_MASK) == USB_DIR_OUT) {
                vsf_usbd_drv_ep_enable_out(ep);
            }
            break;
        case USB_REQ_SET_FEATURE:
        default:
            return VSF_ERR_FAIL;
        }
    } else {
        return VSF_ERR_FAIL;
    }

    if (!size) {
        buffer = NULL;
    }
    ctrl_handler->trans.pchBuffer = buffer;
    ctrl_handler->trans.nSize = size;
    return VSF_ERR_NONE;
}

static vsf_err_t vsf_usbd_stdctrl_process(vsf_usbd_dev_t *dev)
{
    struct usb_ctrlrequest_t *request = &dev->ctrl_handler.request;
    uint8_t recip = request->bRequestType & USB_RECIP_MASK;
    vsf_usbd_cfg_t *config;
    vsf_usbd_ifs_t *ifs;
    VSF_USBD_DRV_PREPARE(dev);

    if (USB_RECIP_DEVICE == recip) {
        switch (request->bRequest) {
        case USB_REQ_SET_ADDRESS:
            dev->address = (uint8_t)request->wValue;
            vsf_usbd_drv_set_address(dev->address);
            break;
        case USB_REQ_SET_CONFIGURATION:
            {
                int_fast16_t config_idx;

                config_idx = vsf_usbd_get_config(dev, request->wValue);
                if (config_idx < 0) {
                    return VSF_ERR_FAIL;
                }
                vsf_usbd_cfg_fini(dev);
                dev->configuration = (uint8_t)config_idx;
                config = &dev->config[dev->configuration];

#if VSF_USBD_CFG_AUTOSETUP == ENABLED
                if (VSF_ERR_NONE != vsf_usbd_auto_init(dev)) {
                    return VSF_ERR_FAIL;
                }
#endif

                // call user initialization
                if ((config->init != NULL) && (VSF_ERR_NONE != config->init(dev))) {
                    return VSF_ERR_FAIL;
                }

                ifs = config->ifs;
                for (uint_fast8_t i = 0; i < config->num_of_ifs; i++, ifs++) {
                    ifs->alternate_setting = 0;

                    if (    (ifs->class_op != NULL)
                        &&  (ifs->class_op->init != NULL)
                        &&  (VSF_ERR_NONE != ifs->class_op->init(dev, ifs))) {
                        return VSF_ERR_FAIL;
                    }
                    ifs->is_inited = true;
                }

                dev->configured = true;
            }
            break;
        }
    } else if (USB_RECIP_INTERFACE == recip) {
        uint_fast8_t ifs_idx = request->wIndex;
        config = &dev->config[dev->configuration];
        ifs = &config->ifs[ifs_idx];
        const vsf_usbd_class_op_t *class_op = ifs->class_op;

        if (ifs_idx >= config->num_of_ifs) {
            return VSF_ERR_FAIL;
        }

        switch (request->bRequest) {
        case USB_REQ_SET_INTERFACE:
            if (class_op->request_process != NULL) {
                class_op->request_process(dev, ifs);
            }
            break;
        }
    }
    return VSF_ERR_NONE;
}

static vsf_usbd_ifs_t * vsf_usbd_get_ifs_byep(vsf_usbd_cfg_t *config, uint_fast8_t ep)
{
    int_fast8_t ifs;

    ep = (ep & 0x0F) | ((ep & 0x80) >> 3);
    ifs = config->ep_ifs_map[ep];
    return ifs >= 0 ? &config->ifs[ifs] : NULL;
}

static vsf_err_t vsf_usbd_ctrl_prepare(vsf_usbd_dev_t *dev)
{
    vsf_usbd_cfg_t *config = &dev->config[dev->configuration];
    vsf_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    uint_fast8_t type = request->bRequestType & USB_TYPE_MASK;
    vsf_err_t err = VSF_ERR_FAIL;

    if (USB_TYPE_STANDARD == type) {
        err = vsf_usbd_stdctrl_prepare(dev);
    } else if ((USB_TYPE_CLASS == type) || (USB_TYPE_VENDOR == type)) {
        uint_fast8_t tmp = request->wIndex & 0xFF;
        vsf_usbd_ifs_t *ifs = NULL;

        switch (request->bRequestType & USB_RECIP_MASK) {
        case USB_RECIP_DEVICE:
            tmp = dev->device_class_ifs;
        case USB_RECIP_INTERFACE:
            if (tmp < config->num_of_ifs)
                ifs = &config->ifs[tmp];
            break;
        case USB_RECIP_ENDPOINT:
            ifs = vsf_usbd_get_ifs_byep(config, tmp);
            break;
        }
        if (ifs && ifs->class_op->request_prepare != NULL) {
            err = ifs->class_op->request_prepare(dev, ifs);
        }
    }

    return err;
}

static void vsf_usbd_ctrl_process(vsf_usbd_dev_t *dev)
{
    vsf_usbd_cfg_t *config = &dev->config[dev->configuration];
    vsf_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    uint_fast8_t type = request->bRequestType & USB_TYPE_MASK;

    if (USB_TYPE_STANDARD == type) {
        vsf_usbd_stdctrl_process(dev);
    } else if (USB_TYPE_CLASS == type) {
        uint_fast8_t tmp = request->wIndex & 0xFF;
        vsf_usbd_ifs_t *ifs = NULL;

        switch (request->bRequestType & USB_RECIP_MASK) {
        case USB_RECIP_DEVICE:
            tmp = dev->device_class_ifs;
        case USB_RECIP_INTERFACE:
            if (tmp < config->num_of_ifs) {
                ifs = &config->ifs[tmp];
            }
            break;
        case USB_RECIP_ENDPOINT:
            ifs = vsf_usbd_get_ifs_byep(config, tmp);
            break;
        }
        if (ifs && ifs->class_op->request_process != NULL) {
            ifs->class_op->request_process(dev, ifs);
        }
    }
}

static void vsf_usbd_setup_status_callback(void *param)
{
    vsf_usbd_dev_t *dev = (vsf_usbd_dev_t *)param;
    VSF_USBD_DRV_PREPARE(dev);
    vsf_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    bool out = (request->bRequestType & USB_DIR_MASK) == USB_DIR_OUT;

    vsf_usbd_drv_status_stage(out);
}

WEAK void vsf_usbd_notify_user(vsf_usbd_dev_t *dev, usb_evt_t evt, void *param)
{
    
}

static void vsf_usbd_hw_init(vsf_usbd_dev_t *dev)
{
    ASSERT(dev != NULL);
    VSF_USBD_DRV_PREPARE(dev);
    usb_dc_cfg_t cfg = {
        .speed          = dev->speed,
        .priority       = dev->priority_int,
        .evt_handler    = vsf_usbd_hal_evt_handler,
        .param          = dev,
    };
    vsf_usbd_drv_init(&cfg);
}

// state machines
static void vsf_usbd_hal_evt_handler(void *p, usb_evt_t evt, uint_fast8_t value)
#if VSF_USBD_CFG_USE_EDA == ENABLED
{
    vsf_usbd_dev_t *dev = p;
    vsf_eda_post_evt(&dev->eda, (vsf_evt_t)(VSF_EVT_USER + (evt | (value << 8))));
}

static void vsf_usbd_evt_handler(vsf_eda_t *eda, vsf_evt_t evt_eda)
{
    vsf_usbd_dev_t *dev;
    uint_fast8_t value;
    usb_evt_t evt;

    if (evt_eda < VSF_EVT_USER) {
        return;
    }
    evt_eda -= VSF_EVT_USER;

    dev = container_of(eda, vsf_usbd_dev_t, eda);
    value = evt_eda >> 8;
    evt = evt_eda & 0xFF;
#else
{
    vsf_usbd_dev_t *dev = p;
#endif
    VSF_USBD_DRV_PREPARE(dev);

    switch (evt) {
    case USB_ON_ATTACH:
    case USB_ON_DETACH:
    case USB_ON_SUSPEND:
    case USB_ON_RESUME:
    case USB_ON_UNDERFLOW:
    case USB_ON_OVERFLOW:
    case USB_ON_ERROR:
    case USB_ON_SOF:
    case USB_ON_NAK:
        vsf_usbd_notify_user(dev, evt, (void *)value);
        break;
    case USB_ON_RESET:
        {
            vsf_usbd_cfg_t *config = vsf_usbd_get_cur_cfg(dev);

            // fini inited ifs
            vsf_usbd_ifs_t *ifs = config->ifs;
            for (uint_fast8_t i = 0; i < config->num_of_ifs; i++, ifs++) {
                if (ifs->is_inited) {
                    ifs->is_inited = false;
                    if (ifs->class_op->fini != NULL) {
                        ifs->class_op->fini(dev, ifs);
                    }
                }
            }

            vsf_usbd_cfg_fini(dev);
            dev->configured = false;
            dev->configuration = 0;
            dev->feature = 0;

            config = dev->config;
            for (uint_fast8_t i = 0; i < dev->num_of_config; i++, config++) {
                memset(config->ep_ifs_map, -1, sizeof(config->ep_ifs_map));
            }

            // reset usb hw
            vsf_usbd_drv_reset();
            vsf_usbd_hw_init(dev);

#if VSF_USBD_CFG_AUTOSETUP == ENABLED
            uint_fast16_t ep_size;
            struct usb_device_descriptor_t *desc_dev;
            vsf_usbd_desc_t *desc = vsf_usbd_get_descriptor(dev->desc, dev->num_of_desc, USB_DT_DEVICE, 0, 0);
            ASSERT(desc != NULL);
            desc_dev = (struct usb_device_descriptor_t *)desc->buffer;
            ASSERT(     (desc->size == USB_DT_DEVICE_SIZE)
                   &&   (desc_dev->bLength == USB_DT_DEVICE_SIZE)
                   &&   (desc_dev->bDescriptorType == USB_DT_DEVICE)
                   &&   (desc_dev->bNumConfigurations == dev->num_of_config));

            // config ep0
            ep_size = desc_dev->bMaxPacketSize0;
            if (    vsf_usbd_drv_ep_add(0 | USB_DIR_OUT, USB_EP_TYPE_CONTROL, ep_size)
                ||  vsf_usbd_drv_ep_add(0 | USB_DIR_IN, USB_EP_TYPE_CONTROL, ep_size)) {
                // TODO:
                return;
            }
#endif

            vsf_usbd_notify_user(dev, evt, NULL);
            vsf_usbd_drv_set_address(0);
            break;
        }
    case USB_ON_SETUP:
        {
            vsf_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
            struct usb_ctrlrequest_t *request = &ctrl_handler->request;
            vsf_usbd_trans_t *trans = &ctrl_handler->trans;

            vsf_usbd_notify_user(dev, evt, request);
            vsf_usbd_drv_get_setup(request);
            if (VSF_ERR_NONE != vsf_usbd_ctrl_prepare(dev)) {
                // fail to get setup request data
                vsf_usbd_drv_ep_set_stall(0 | USB_DIR_OUT);
                vsf_usbd_drv_ep_set_stall(0 | USB_DIR_IN);
            }

            if (ctrl_handler->trans.nSize > request->wLength) {
                ctrl_handler->trans.nSize = request->wLength;
            }

            if ((request->bRequestType & USB_DIR_MASK) == USB_DIR_OUT) {
                if (0 == request->wLength) {
                    vsf_usbd_setup_status_callback((void *)dev);
                } else {
                    trans->on_finish = vsf_usbd_setup_status_callback;
                    vsf_usbd_ep_recv(dev, trans);
                }
            } else {
                trans->on_finish = vsf_usbd_setup_status_callback;
                trans->zlp = ctrl_handler->trans.nSize < request->wLength;
                vsf_usbd_ep_send(dev, trans);
            }
            break;
        }
    case USB_ON_STATUS:
        vsf_usbd_ctrl_process(dev);
        break;
    case USB_ON_IN:
        {
            uint_fast8_t ep = value | USB_DIR_IN;
            vsf_usbd_trans_t *trans = vsf_usbd_get_trans(dev, ep);
            ASSERT(trans != NULL);

            if (trans->nSize) {
                vsf_usbd_ep_send_imp(dev, trans);
            } else if (trans->zlp) {
                trans->zlp = false;
                vsf_usbd_drv_ep_set_data_size(ep, 0);
            } else {
                vsf_usbd_trans_finish(dev, trans);
            }
            break;
        }
    case USB_ON_OUT:
        {
            uint_fast8_t ep = value | USB_DIR_OUT;
            vsf_usbd_trans_t *trans = vsf_usbd_get_trans(dev, ep);
            ASSERT(trans != NULL);

            if (!trans->pchBuffer) {
                vsf_usbd_trans_finish(dev, trans);
            } else {
                uint_fast16_t pkg_size = vsf_usbd_drv_ep_get_data_size(ep);
                uint_fast16_t ep_size = vsf_usbd_drv_ep_get_size(ep);

                // ignore the over-run data
                pkg_size = min(pkg_size, trans->nSize);
                vsf_usbd_drv_ep_read_buffer(ep, trans->cur, pkg_size);
                trans->cur += pkg_size;
                trans->nSize -= pkg_size;

                // TODO: check trans->zlp
                if ((trans->nSize > 0) && (pkg_size == ep_size)) {
                    vsf_usbd_drv_ep_enable_out(ep);
                } else {
                    vsf_usbd_trans_finish(dev, trans);
                }
            }
            break;
        }
    default:
        break;
    }
}

void vsf_usbd_connect(vsf_usbd_dev_t *dev)
{
    ASSERT(dev != NULL);
    VSF_USBD_DRV_PREPARE(dev);
    vsf_usbd_drv_connect();
}

void vsf_usbd_disconnect(vsf_usbd_dev_t *dev)
{
    ASSERT(dev != NULL);
    VSF_USBD_DRV_PREPARE(dev);
    vsf_usbd_drv_disconnect();
}

void vsf_usbd_wakeup(vsf_usbd_dev_t *dev)
{
    ASSERT(dev != NULL);
    VSF_USBD_DRV_PREPARE(dev);
    vsf_usbd_drv_wakeup();
}

vsf_usbd_cfg_t * vsf_usbd_get_cur_cfg(vsf_usbd_dev_t *dev)
{
    return &dev->config[dev->configuration];
}

vsf_usbd_ifs_t * vsf_usbd_get_ifs(vsf_usbd_dev_t *dev, uint_fast8_t ifs_no)
{
    vsf_usbd_cfg_t *config = vsf_usbd_get_cur_cfg(dev);
    if (ifs_no < config->num_of_ifs) {
        return &config->ifs[ifs_no];
    }
    return NULL;
}

void vsf_usbd_init(vsf_usbd_dev_t *dev)
{
    ASSERT((dev != NULL) && (dev->config != NULL));

    dev->configured = false;
    dev->configuration = 0;
    dev->feature = 0;
    dev->ctrl_handler.trans.ep = 0;
    dev->ctrl_handler.trans.param = dev;

    vsf_usbd_desc_t *desc;
    vsf_usbd_cfg_t *config = dev->config;
    vsf_usbd_ifs_t *ifs;
    struct usb_config_descriptor_t *desc_config;

    for (uint_fast8_t i = 0; i < dev->num_of_config; i++, config++) {
#if VSF_USBD_CFG_AUTOSETUP == ENABLED
        desc = vsf_usbd_get_descriptor(dev->desc, dev->num_of_desc, USB_DT_CONFIG, i, 0);
        ASSERT(desc != NULL);
        desc_config = (struct usb_config_descriptor_t *)desc->buffer;
        ASSERT(     (desc->size == le16_to_cpu(desc_config->wTotalLength))
               &&   (desc_config->bLength == USB_DT_CONFIG_SIZE)
               &&   (desc_config->bDescriptorType == USB_DT_CONFIG)
               &&   (desc_config->bNumInterfaces == config->num_of_ifs));
        config->configuration_value = desc_config->bConfigurationValue;
#endif

        ifs = config->ifs;
        for (uint_fast8_t i = 0; i < config->num_of_ifs; i++, ifs++) {
            ifs->is_inited = false;
        }
    }

    vsf_usbd_hw_init(dev);
    vsf_usbd_notify_user(dev, USB_ON_INIT, NULL);

#if VSF_USBD_CFG_USE_EDA == ENABLED
    dev->eda.evthandler = vsf_usbd_evt_handler;
    vsf_eda_init(&dev->eda, dev->priority_eda, false);
#endif
}

// TODO: check fini and re-init
void vsf_usbd_fini(vsf_usbd_dev_t *dev)
{
    ASSERT(dev != NULL);
    VSF_USBD_DRV_PREPARE(dev);

    vsf_usbd_drv_disconnect();
    vsf_usbd_drv_fini();
    vsf_usbd_notify_user(dev, USB_ON_FINI, NULL);
}

#if VSF_USBD_CFG_STREAM_EN == ENABLED
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED

static void vsf_usbd_stream_tx_on_trans_finish(void *param)
{
    vsf_usbd_ep_stream_t *stream_ep = (vsf_usbd_ep_stream_t *)param;
    vsf_stream_t *stream = stream_ep->stream;
    VSF_USBD_DRV_PREPARE(stream_ep->dev);
    uint_fast8_t ep = stream_ep->use_as__vsf_usbd_trans_t.ep;
    uint_fast16_t pkg_size = vsf_usbd_drv_ep_get_data_size(ep);
    uint8_t *buffer;
    uint_fast32_t size;

    for (uint_fast16_t size_read = pkg_size; size_read > 0;) {
        size = vsf_stream_get_wbuf(stream, &buffer);
        size = min(size, size_read);
        vsf_usbd_drv_ep_read_buffer(ep, buffer, size);
        size_read -= size;
    }

    vsf_stream_write(stream, NULL, pkg_size);

    if (vsf_stream_get_free_size(stream) > vsf_usbd_drv_ep_get_size(ep)) {
        vsf_usbd_ep_recv(stream_ep->dev, &stream_ep->use_as__vsf_usbd_trans_t);
    } else {
        stream_ep->on_finish = NULL;
    }
}

static void vsf_usbd_stream_tx_evthandler(void *param, vsf_stream_evt_t evt)
{
    vsf_usbd_ep_stream_t *stream_ep = (vsf_usbd_ep_stream_t *)param;
    vsf_usbd_dev_t *dev = stream_ep->dev;
    vsf_stream_t *stream = stream_ep->stream;
    VSF_USBD_DRV_PREPARE(dev);

    switch (evt) {
    case VSF_STREAM_ON_CONNECT:
    case VSF_STREAM_ON_OUT:
        if (NULL == stream_ep->on_finish) {
            if (vsf_stream_get_free_size(stream) > vsf_usbd_drv_ep_get_size(stream_ep->ep)) {
                stream_ep->on_finish = vsf_usbd_stream_tx_on_trans_finish;
                vsf_usbd_ep_recv(dev, &stream_ep->use_as__vsf_usbd_trans_t);
            }
        }
        break;
    }
}

vsf_err_t vsf_usbd_ep_recv_stream(vsf_usbd_ep_stream_t *stream_ep)
{
    vsf_usbd_trans_t *trans = &stream_ep->use_as__vsf_usbd_trans_t;
    vsf_stream_t *stream = stream_ep->stream;

    stream->tx.param = stream_ep;
    stream->tx.evthandler = vsf_usbd_stream_tx_evthandler;
    trans->param = stream_ep;
    trans->pchBuffer = NULL;
    vsf_stream_connect_tx(stream);

    vsf_usbd_stream_tx_evthandler(stream_ep, VSF_STREAM_ON_OUT);
    return VSF_ERR_NONE;
}

static void vsf_usbd_stream_rx_on_trans_finish(void *param)
{
    vsf_usbd_ep_stream_t *stream_ep = (vsf_usbd_ep_stream_t *)param;
    vsf_stream_t *stream = stream_ep->stream;

    if (stream_ep->cur_size > 0) {
        vsf_stream_read(stream_ep->stream, NULL, stream_ep->cur_size);
        stream_ep->cur_size = 0;
    }

    stream_ep->nSize = vsf_stream_get_rbuf(stream, &stream_ep->pchBuffer);
    if (stream_ep->nSize > 0) {
        stream_ep->cur_size = stream_ep->nSize;
        vsf_usbd_ep_send(stream_ep->dev, &stream_ep->use_as__vsf_usbd_trans_t);
    } else {
        stream_ep->on_finish = NULL;
    }
}

static void vsf_usbd_stream_rx_evthandler(void *param, vsf_stream_evt_t evt)
{
    vsf_usbd_ep_stream_t *stream_ep = (vsf_usbd_ep_stream_t *)param;
    vsf_usbd_dev_t *dev = stream_ep->dev;
    vsf_stream_t *stream = stream_ep->stream;

    switch (evt) {
    case VSF_STREAM_ON_RX:
        if (NULL == stream_ep->on_finish) {
            // TODO: current implementation will send short packet
            stream_ep->nSize = vsf_stream_get_rbuf(stream, &stream_ep->pchBuffer);
            if (stream_ep->nSize > 0) {
                stream_ep->cur_size = stream_ep->nSize;
                stream_ep->on_finish = vsf_usbd_stream_rx_on_trans_finish;
                vsf_usbd_ep_send(dev, &stream_ep->use_as__vsf_usbd_trans_t);
            }
        }
        break;
    }
}

vsf_err_t vsf_usbd_ep_send_stream(vsf_usbd_ep_stream_t *stream_ep)
{
    vsf_usbd_trans_t *trans = &stream_ep->use_as__vsf_usbd_trans_t;
    vsf_stream_t *stream = stream_ep->stream;

    stream->rx.param = stream_ep;
    stream->rx.evthandler = vsf_usbd_stream_rx_evthandler;
    trans->param = stream_ep;
    vsf_stream_connect_rx(stream);

    vsf_usbd_stream_rx_evthandler(stream_ep, VSF_STREAM_ON_IN);
    return VSF_ERR_NONE;
}

#elif VSF_USE_SERVICE_STREAM == ENABLED

static vsf_err_t __vsf_usbd_ep_rcv_pbuf(vsf_usbd_ep_stream_t *pthis)
{
    vsf_err_t result = VSF_ERR_NOT_READY;
    VSF_USBD_DRV_PREPARE(pthis->dev);

    do {
        if (NULL == pthis->dev) {
            break ;
        }

        pthis->rx_current = vsf_stream_src_new_pbuf(  
            &pthis->use_as__vsf_stream_src_t, 
            //!//! require a big enough pbuf
            vsf_usbd_drv_ep_get_size(pthis->rx_trans.ep),        
            -1);

        if (NULL == pthis->rx_current) {
            result = VSF_ERR_NOT_ENOUGH_RESOURCES;
            break;
        }

        vsf_usbd_trans_t *trans = &pthis->rx_trans;
        trans->param = pthis;
        
        trans->pchSrc = vsf_pbuf_buffer_get(pthis->rx_current);
        trans->nSize = vsf_pbuf_size_get(pthis->rx_current);
        vsf_usbd_ep_recv(pthis->dev, trans);
        result = VSF_ERR_NONE;
    } while(0);
    return result;
}



static vsf_err_t vsf_usbd_on_ep_rcv(vsf_usbd_ep_stream_t *pthis)
{
    vsf_err_t result;// = VSF_ERR_NOT_READY;
    vsf_pbuf_t *pbuf;
    uint_fast16_t ep_left_size;
    VSF_USBD_DRV_PREPARE(pthis->dev);

    __SAFE_ATOM_CODE(                   //! this protection might not be necessary
        pbuf = pthis->rx_current;
        pthis->rx_current = NULL;
        if (pbuf != NULL) {
            ep_left_size = pthis->rx_trans.nSize;
        }
        result = __vsf_usbd_ep_rcv_pbuf(pthis);    //! start next rcv
    )
    if (NULL != pbuf) {
        vsf_pbuf_size_set(  pbuf, 
                            vsf_usbd_drv_ep_get_size(pthis->rx_trans.ep) - ep_left_size);
        vsf_stream_src_send_pbuf(&pthis->use_as__vsf_stream_src_t, pbuf);
    }

    return result;
}

vsf_err_t vsf_usbd_ep_recv_stream(vsf_usbd_ep_stream_t *pthis)
{
    vsf_err_t result = VSF_ERR_NOT_READY;
    __SAFE_ATOM_CODE(
        if (NULL == pthis->rx_current) {
            result = vsf_usbd_on_ep_rcv(pthis);
        }
    )

    return result;
}

static void vsf_usbd_ep_on_stream_rx_finish(void *param)
{
    vsf_usbd_ep_stream_t *pthis = (vsf_usbd_ep_stream_t *)param;

    vsf_usbd_on_ep_rcv(pthis);
}

static vsf_err_t __vsf_usbd_ep_send_pbuf(vsf_usbd_ep_stream_t *pthis)
{
    vsf_err_t result = VSF_ERR_NOT_READY;
    do {
        if (NULL == pthis->dev) {
            break ;
        }
        pthis->tx_current = vsf_stream_usr_fetch_pbuf(&pthis->use_as__vsf_stream_usr_t);
        if (NULL == pthis->tx_current) {
            result = VSF_ERR_NOT_ENOUGH_RESOURCES;
            break;
        }

        vsf_usbd_trans_t *trans = &pthis->tx_trans;
        trans->param = pthis;
        
        trans->pchSrc = vsf_pbuf_buffer_get(pthis->tx_current);
        trans->nSize = vsf_pbuf_size_get(pthis->tx_current);

        vsf_usbd_ep_send(pthis->dev, trans);
        result = VSF_ERR_NONE;
    } while(0);
    return result;
}

static void vsf_usbd_ep_on_stream_tx_finish(void *param)
{
    vsf_usbd_ep_stream_t *pthis = (vsf_usbd_ep_stream_t *)param;

    vsf_pbuf_t *ptBuff;

    __SAFE_ATOM_CODE(
        ptBuff = pthis->tx_current;
        pthis->tx_current = NULL;
        __vsf_usbd_ep_send_pbuf(pthis);
    )

    vsf_pbuf_free(ptBuff);       //! free old pbuf
}



vsf_err_t vsf_usbd_ep_send_stream(vsf_usbd_ep_stream_t *pthis)
{
    vsf_err_t result = VSF_ERR_NOT_READY;
    ASSERT(NULL != pthis);

    __SAFE_ATOM_CODE(
        if (NULL == pthis->tx_current) {
            result = __vsf_usbd_ep_send_pbuf(pthis);
        }
    )
    return result;
}

static void __vsf_usbd_on_data_ready_event( void *pTarget, 
                                            vsf_stream_rx_t *ptRX, 
                                            vsf_stream_status_t tStatus)
{
    vsf_usbd_ep_stream_t *pthis = (vsf_usbd_ep_stream_t *)pTarget;
    vsf_usbd_ep_send_stream(pthis);
}

void vsf_usbd_ep_stream_init(   vsf_usbd_ep_stream_t *pthis, 
                                vsf_usbd_ep_stream_cfg_t *cfg)
{
    ASSERT(NULL != pthis);
    pthis->tx_trans.on_finish = vsf_usbd_ep_on_stream_tx_finish;
    pthis->rx_trans.on_finish = vsf_usbd_ep_on_stream_rx_finish;
    pthis->tx_current = NULL;
    pthis->rx_current = NULL;

    pthis->rx_trans.ep = cfg->rx_ep;
    pthis->rx_trans.zlp = false;
    pthis->tx_trans.ep = cfg->tx_ep;
    pthis->tx_trans.zlp = true;
    pthis->dev = NULL;

    //! access protected member of vsf_stream_usr_t
    with_protected(vsf_stream_usr_t, &pthis->use_as__vsf_stream_usr_t, {
        if (NULL != _->ptRX) {
            //! register data rdy event handler
            _->ptRX->piMethod->DataReadyEvent.Register(
                _->ptRX,
                (vsf_stream_dat_rdy_evt_t){__vsf_usbd_on_data_ready_event, pthis});
        }
    });

    
#if VSF_STREAM_CFG_SUPPORT_OPEN_CLOSE == ENABLED
    vsf_stream_usr_open(&(pthis->use_as__vsf_stream_usr_t));
#endif
}

void vsf_usbd_ep_stream_connect_dev(vsf_usbd_ep_stream_t *pthis, vsf_usbd_dev_t *dev)
{
    ASSERT(NULL != pthis);
    pthis->dev = dev;
    vsf_usbd_ep_send_stream(pthis);
}

#endif      // VSF_USE_SERVICE_STREAM || VSF_USE_SERVICE_VSFSTREAM
#endif      // VSF_USBD_CFG_STREAM_EN
#endif      // VSF_USE_USB_DEVICE
