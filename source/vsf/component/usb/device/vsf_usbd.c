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

#define __VSF_EDA_CLASS_INHERIT__
#if VSF_USBD_CFG_STREAM_EN == ENABLED
#   if VSF_USE_SIMPLE_STREAM == ENABLED
#       define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#   elif VSF_USE_STREAM == ENABLED
#       define __VSF_STREAM_BASE_CLASS_INHERIT__
#   endif
#endif
#define __VSF_USBD_CLASS_IMPLEMENT

#include "kernel/vsf_kernel.h"
#include "./vsf_usbd.h"
#include "./vsf_usbd_drv_ifs.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static void __vk_usbd_hal_evthandler(void *, usb_evt_t, uint_fast8_t);

extern vsf_err_t vsf_usbd_vendor_prepare(vk_usbd_dev_t *dev);
extern void vsf_usbd_vendor_process(vk_usbd_dev_t *dev);

extern vsf_err_t vsf_usbd_notify_user(vk_usbd_dev_t *dev, usb_evt_t evt, void *param);

/*============================ IMPLEMENTATION ================================*/

vk_usbd_desc_t * vk_usbd_get_descriptor(vk_usbd_desc_t *desc,
        uint_fast8_t desc_num, uint_fast8_t type,
        uint_fast8_t index, uint_fast16_t langid)
{
    VSF_USB_ASSERT(desc != NULL);
    for (uint_fast8_t i = 0; i < desc_num; i++) {
        if (    (desc->type == type)
            &&  (desc->index == index)
            &&  (desc->langid == langid)) {
            return desc;
        }
        desc++;
    }
    return NULL;
}

#if VSF_USBD_CFG_RAW_MODE != ENABLED
static vk_usbd_ifs_t * __vk_usbd_get_ifs_byep(vk_usbd_cfg_t *config, uint_fast8_t ep)
{
    int_fast8_t ifs;

    ep = (ep & 0x0F) | ((ep & 0x80) >> 3);
    ifs = config->ep_ifs_map[ep];
    return ifs >= 0 ? &config->ifs[ifs] : NULL;
}

static void __vk_usbd_cfg_fini(vk_usbd_dev_t *dev)
{
    vk_usbd_cfg_t *config = vk_usbd_get_cur_cfg(dev);

    vk_usbd_ifs_t *ifs = config->ifs;
    for (uint_fast8_t i = 0; i < config->num_of_ifs; i++, ifs++) {
        if (ifs->is_inited) {
            ifs->is_inited = false;
            if (ifs->class_op->fini != NULL) {
                ifs->class_op->fini(dev, ifs);
            }
        }
    }
}

#endif

static vk_usbd_trans_t * __vk_usbd_get_trans(vk_usbd_dev_t *dev, uint_fast8_t ep)
{
    vk_usbd_trans_t *trans;

    vsf_slist_peek_next(vk_usbd_trans_t, node, &dev->trans_list, trans);
    while (trans != NULL) {
        if (trans->ep == ep) {
            break;
        }
        vsf_slist_peek_next(vk_usbd_trans_t, node, &trans->node, trans);
    }
    return trans;
}

static void __vk_usbd_trans_finish(vk_usbd_dev_t *dev, vk_usbd_trans_t *trans)
{
    vsf_slist_remove(vk_usbd_trans_t, node, &dev->trans_list, trans);
#if VSF_USE_KERNEL == ENABLED
    if (trans->notify_eda) {
        vsf_eda_post_msg(trans->eda, trans);
    } else
#endif
    if (trans->on_finish != NULL) {
        trans->on_finish(trans->param);
    }
}

vsf_err_t vk_usbd_ep_stall(vk_usbd_dev_t *dev, uint_fast8_t ep)
{
    VSF_USB_ASSERT(dev != NULL);
    VSF_USBD_DRV_PREPARE(dev);

    return vk_usbd_drv_ep_set_stall(ep);
}

vsf_err_t vk_usbd_ep_recv(vk_usbd_dev_t *dev, vk_usbd_trans_t *trans)
{
    VSF_USB_ASSERT((dev != NULL) && (trans != NULL));
    VSF_USBD_DRV_PREPARE(dev);
    vsf_err_t err;

    trans->ep &= ~USB_DIR_MASK;
    vsf_slist_init_node(vk_usbd_trans_t, node, trans);
    vsf_slist_add_to_head(vk_usbd_trans_t, node, &dev->trans_list, trans);

    if (vk_usbd_drv_ep_get_feature(trans->ep, trans->feature) & USB_DC_FEATURE_TRANSFER) {
        err = vk_usbd_drv_ep_transfer_recv(trans->ep, trans->use_as__vsf_mem_t.buffer,
                trans->use_as__vsf_mem_t.size);
    } else {
        trans->cur = trans->use_as__vsf_mem_t.buffer;
        err = vk_usbd_drv_ep_transaction_enable_out(trans->ep);
    }
    if (VSF_ERR_NONE != err) {
        vsf_slist_remove(vk_usbd_trans_t, node, &dev->trans_list, trans);
    }
    return err;
}

static vsf_err_t __vk_usbd_ep_send_imp(vk_usbd_dev_t *dev, vk_usbd_trans_t *trans)
{
    uint_fast8_t ep = trans->ep;
    VSF_USBD_DRV_PREPARE(dev);
    uint_fast16_t ep_size = vk_usbd_drv_ep_get_size(ep);
    uint_fast16_t pkg_size = min(ep_size, trans->use_as__vsf_mem_t.size);

    vk_usbd_drv_ep_transaction_write_buffer(ep, trans->cur, pkg_size);
    trans->cur += pkg_size;
    trans->use_as__vsf_mem_t.size -= pkg_size;
    if (!trans->use_as__vsf_mem_t.size && (pkg_size < ep_size)) {
        trans->zlp = false;
    }

    return vk_usbd_drv_ep_transaction_set_data_size(ep, pkg_size);
}

vsf_err_t vk_usbd_ep_send(vk_usbd_dev_t *dev, vk_usbd_trans_t *trans)
{
    VSF_USB_ASSERT((dev != NULL) && (trans != NULL));
    VSF_USBD_DRV_PREPARE(dev);
    vsf_err_t err;

    trans->ep |= USB_DIR_MASK;
    vsf_slist_init_node(vk_usbd_trans_t, node, trans);
    vsf_slist_add_to_head(vk_usbd_trans_t, node, &dev->trans_list, trans);

    if (vk_usbd_drv_ep_get_feature(trans->ep, trans->feature) & USB_DC_FEATURE_TRANSFER) {
        uint_fast32_t size = trans->use_as__vsf_mem_t.size;
        bool zlp = trans->zlp;
        trans->use_as__vsf_mem_t.size = 0;
        trans->zlp = false;
        err = vk_usbd_drv_ep_transfer_send(trans->ep, trans->use_as__vsf_mem_t.buffer, size, zlp);
    } else {
        trans->cur = trans->use_as__vsf_mem_t.buffer;
        err = __vk_usbd_ep_send_imp(dev, trans);
    }
    if (VSF_ERR_NONE != err) {
        vsf_slist_remove(vk_usbd_trans_t, node, &dev->trans_list, trans);
    }
    return err;
}

#if VSF_USBD_CFG_RAW_MODE != ENABLED
// standard request handlers
static int16_t __vk_usbd_get_config(vk_usbd_dev_t *dev, uint_fast8_t value)
{
    for (uint_fast8_t i = 0; i < dev->num_of_config; i++) {
        if (value == dev->config[i].configuration_value) {
            return i;
        }
    }
    return -1;
}

#if VSF_USBD_CFG_AUTOSETUP == ENABLED
static vsf_err_t __vk_usbd_auto_init(vk_usbd_dev_t *dev)
{
    VSF_USB_ASSERT(dev != NULL);
    VSF_USBD_DRV_PREPARE(dev);
    vk_usbd_cfg_t *config;
    vk_usbd_desc_t *desc;
    struct usb_config_desc_t *desc_config;

    uint_fast16_t pos, cur_ifs;
    uint_fast8_t attr, feature;

    config = &dev->config[dev->configuration];

    // config other eps according to descriptors
    desc = vk_usbd_get_descriptor(dev->desc, dev->num_of_desc, USB_DT_CONFIG, dev->configuration, 0);
    VSF_USB_ASSERT(desc != NULL);
    desc_config = (struct usb_config_desc_t *)desc->buffer;
    VSF_USB_ASSERT(     (desc->size == desc_config->wTotalLength)
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
        struct usb_endpoint_desc_t *desc_header =
            (struct usb_endpoint_desc_t *)((uint8_t *)desc_config + pos);
        VSF_USB_ASSERT((desc_header->bLength > 2) && (desc->size >= (pos + desc_header->bLength)));

        switch (desc_header->bDescriptorType) {
        case USB_DT_INTERFACE: {
                struct usb_interface_desc_t *desc_ifs =
                        (struct usb_interface_desc_t *)desc_header;
                cur_ifs = desc_ifs->bInterfaceNumber;
                break;
            }
        case USB_DT_ENDPOINT: {
                struct usb_endpoint_desc_t *desc_ep =
                        (struct usb_endpoint_desc_t *)desc_header;
                uint_fast8_t ep_addr = desc_ep->bEndpointAddress;
                uint_fast8_t ep_attr = desc_ep->bmAttributes;
                uint_fast16_t ep_size = desc_ep->wMaxPacketSize;

                VSF_USB_ASSERT((ep_attr & 0x03) <= 3);
                if (VSF_ERR_NONE != vk_usbd_drv_ep_add(ep_addr, ep_attr & 0x03, ep_size)) {
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

static vsf_err_t __vk_usbd_stdctrl_prepare(vk_usbd_dev_t *dev)
{
    VSF_USBD_DRV_PREPARE(dev);
    vk_usbd_cfg_t *config = &dev->config[dev->configuration];
    vk_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
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
        case USB_REQ_GET_DESCRIPTOR: {
                uint_fast8_t type = (request->wValue >> 8) & 0xFF;
                uint_fast8_t index = request->wValue & 0xFF;
                uint_fast16_t langid = request->wIndex;
                vk_usbd_desc_t *desc = vk_usbd_get_descriptor(dev->desc,
                        dev->num_of_desc, type, index, langid);

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
            if ((request->wIndex != 0) || (__vk_usbd_get_config(dev, request->wValue) < 0)) {
                return VSF_ERR_FAIL;
            }
            dev->configured = false;
            break;
        default:
            return VSF_ERR_FAIL;
        }
    } else if (USB_RECIP_INTERFACE == recip) {
        uint_fast8_t ifs_idx = request->wIndex;
        vk_usbd_ifs_t *ifs = &config->ifs[ifs_idx];
        const vk_usbd_class_op_t *class_op = ifs->class_op;

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
        case USB_REQ_GET_DESCRIPTOR: {
                uint_fast8_t type = (request->wValue >> 8) & 0xFF;
                uint_fast8_t index = request->wValue & 0xFF;
                vk_usbd_desc_t *desc = NULL;

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

        if ((request->bRequestType & USB_DIR_MASK) == USB_DIR_IN) {
            return VSF_ERR_FAIL;
        }

        switch (request->bRequest) {
        case USB_REQ_GET_STATUS:
            if (request->wValue != 0) {
                return VSF_ERR_FAIL;
            }
            if (vk_usbd_drv_ep_is_stalled(ep)) {
                buffer[0] = 1;
            } else {
                buffer[0] = 0;
            }
            buffer[1] = 0;
            size = 2;
            break;
        case USB_REQ_CLEAR_FEATURE: {
                if (request->wValue != USB_ENDPOINT_HALT) {
                    return VSF_ERR_FAIL;
                }

                if (0 == ep) {
                    break;
                } else {
                    vk_usbd_ifs_t *ifs = __vk_usbd_get_ifs_byep(config, ep);
                    VSF_USB_ASSERT(ifs != NULL);
                    const vk_usbd_class_op_t *class_op = ifs->class_op;

                    vk_usbd_drv_ep_clear_stall(ep);
                    if (class_op->request_prepare != NULL) {
                        class_op->request_prepare(dev, ifs);
                    }
                }
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
    ctrl_handler->trans.use_as__vsf_mem_t.buffer = buffer;
    ctrl_handler->trans.use_as__vsf_mem_t.size = size;
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_usbd_stdctrl_process(vk_usbd_dev_t *dev)
{
    struct usb_ctrlrequest_t *request = &dev->ctrl_handler.request;
    uint8_t recip = request->bRequestType & USB_RECIP_MASK;
    vk_usbd_cfg_t *config;
    vk_usbd_ifs_t *ifs;
    VSF_USBD_DRV_PREPARE(dev);

    if (USB_RECIP_DEVICE == recip) {
        switch (request->bRequest) {
        case USB_REQ_SET_ADDRESS:
            dev->address = (uint8_t)request->wValue;
            vk_usbd_drv_set_address(dev->address);
            break;
        case USB_REQ_SET_CONFIGURATION: {
                int_fast16_t config_idx;

                config_idx = __vk_usbd_get_config(dev, request->wValue);
                if (config_idx < 0) {
                    return VSF_ERR_FAIL;
                }
                __vk_usbd_cfg_fini(dev);
                dev->configuration = (uint8_t)config_idx;
                config = &dev->config[dev->configuration];

#if VSF_USBD_CFG_AUTOSETUP == ENABLED
                if (VSF_ERR_NONE != __vk_usbd_auto_init(dev)) {
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
        const vk_usbd_class_op_t *class_op = ifs->class_op;

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

#ifndef WEAK_VSF_USBD_VENDOR_PREPARE
WEAK(vsf_usbd_vendor_prepare)
vsf_err_t vsf_usbd_vendor_prepare(vk_usbd_dev_t *dev)
{
    return VSF_ERR_FAIL;
}
#endif

#ifndef WEAK_VSF_USBD_VENDOR_PROCESS
WEAK(vsf_usbd_vendor_process)
void vsf_usbd_vendor_process(vk_usbd_dev_t *dev)
{
}
#endif

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

static vsf_err_t __vk_usbd_ctrl_prepare(vk_usbd_dev_t *dev)
{
    vk_usbd_cfg_t *config = &dev->config[dev->configuration];
    vk_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    uint_fast8_t type = request->bRequestType & USB_TYPE_MASK;
    vsf_err_t err = VSF_ERR_FAIL;

    if (USB_TYPE_STANDARD == type) {
        err = __vk_usbd_stdctrl_prepare(dev);
    } else if (USB_TYPE_CLASS == type) {
        uint_fast8_t tmp = request->wIndex & 0xFF;
        vk_usbd_ifs_t *ifs = NULL;

        switch (request->bRequestType & USB_RECIP_MASK) {
        case USB_RECIP_INTERFACE:
            if (tmp < config->num_of_ifs) {
                ifs = &config->ifs[tmp];
            }
            break;
        case USB_RECIP_ENDPOINT:
            ifs = __vk_usbd_get_ifs_byep(config, tmp);
            break;
        default:
            VSF_USB_ASSERT(false);
            return VSF_ERR_FAIL;
        }
        if (ifs && ifs->class_op->request_prepare != NULL) {
            err = ifs->class_op->request_prepare(dev, ifs);
        }
    } else if (USB_TYPE_VENDOR == type) {
        err = vsf_usbd_vendor_prepare(dev);
    }

    return err;
}

static void __vk_usbd_ctrl_process(vk_usbd_dev_t *dev)
{
    vk_usbd_cfg_t *config = &dev->config[dev->configuration];
    vk_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    uint_fast8_t type = request->bRequestType & USB_TYPE_MASK;

    if (USB_TYPE_STANDARD == type) {
        __vk_usbd_stdctrl_process(dev);
    } else if (USB_TYPE_CLASS == type) {
        uint_fast8_t tmp = request->wIndex & 0xFF;
        vk_usbd_ifs_t *ifs = NULL;

        switch (request->bRequestType & USB_RECIP_MASK) {
        case USB_RECIP_INTERFACE:
            if (tmp < config->num_of_ifs) {
                ifs = &config->ifs[tmp];
            }
            break;
        case USB_RECIP_ENDPOINT:
            ifs = __vk_usbd_get_ifs_byep(config, tmp);
            break;
        default:
            VSF_USB_ASSERT(false);
            return;
        }
        if (ifs && ifs->class_op->request_process != NULL) {
            ifs->class_op->request_process(dev, ifs);
        }
    } else if (USB_TYPE_VENDOR == type) {
        vsf_usbd_vendor_process(dev);
    }
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_warning=pe111
#endif

#endif      // !VSF_USBD_CFG_RAW_MODE

static void __vk_usbd_setup_status_callback(void *param)
{
    vk_usbd_dev_t *dev = (vk_usbd_dev_t *)param;
    VSF_USBD_DRV_PREPARE(dev);
    vk_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    bool out = (request->bRequestType & USB_DIR_MASK) == USB_DIR_OUT;

    vk_usbd_drv_status_stage(out);
}

#ifndef WEAK_VSF_USBD_NOTIFY_USER
WEAK(vsf_usbd_notify_user)
vsf_err_t vsf_usbd_notify_user(vk_usbd_dev_t *dev, usb_evt_t evt, void *param)
{
    return VSF_ERR_NONE;
}
#endif

static void __vk_usbd_hw_init_reset(vk_usbd_dev_t *dev, bool reset)
{
    VSF_USB_ASSERT(dev != NULL);
    VSF_USBD_DRV_PREPARE(dev);
    usb_dc_cfg_t cfg = {
        .speed          = dev->speed,
        .priority       = VSF_USBD_CFG_HW_PRIORITY,
        .evt_handler    = __vk_usbd_hal_evthandler,
        .param          = dev,
    };
    if (reset) {
        vk_usbd_drv_reset(&cfg);
    } else {
        vk_usbd_drv_init(&cfg);
    }
}

// state machines
static void __vk_usbd_hal_evthandler(void *p, usb_evt_t evt, uint_fast8_t value)
#if VSF_USBD_CFG_USE_EDA == ENABLED
{
    vk_usbd_dev_t *dev = p;
    vsf_eda_post_evt(&dev->eda, (vsf_evt_t)(VSF_EVT_USER + (evt | (value << 8))));
}

static void __vk_usbd_evthandler(vsf_eda_t *eda, vsf_evt_t evt_eda)
{
    vk_usbd_dev_t *dev;
    uint_fast8_t value;
    usb_evt_t evt;

    if (evt_eda < VSF_EVT_USER) {
        return;
    }
    evt_eda -= VSF_EVT_USER;

    dev = container_of(eda, vk_usbd_dev_t, eda);
    value = evt_eda >> 8;
    evt = (usb_evt_t)(evt_eda & 0xFF);
#else
{
    vk_usbd_dev_t *dev = p;
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
        vsf_usbd_notify_user(dev, evt, (void *)(uintptr_t)value);
        break;
    case USB_ON_RESET: {
#if VSF_USBD_CFG_RAW_MODE != ENABLED
            vk_usbd_cfg_t *config;

            __vk_usbd_cfg_fini(dev);
            config = dev->config;
            for (uint_fast8_t i = 0; i < dev->num_of_config; i++, config++) {
                memset(config->ep_ifs_map, -1, sizeof(config->ep_ifs_map));
            }

            dev->configured = false;
            dev->configuration = 0;
            dev->feature = 0;
#endif

            // reset usb hw
            __vk_usbd_hw_init_reset(dev, true);

#if VSF_USBD_CFG_AUTOSETUP == ENABLED
            uint_fast16_t ep_size;
            struct usb_device_desc_t *desc_dev;
            vk_usbd_desc_t *desc = vk_usbd_get_descriptor(dev->desc, dev->num_of_desc, USB_DT_DEVICE, 0, 0);
            VSF_USB_ASSERT(desc != NULL);
            desc_dev = (struct usb_device_desc_t *)desc->buffer;
            VSF_USB_ASSERT(     (desc->size == USB_DT_DEVICE_SIZE)
                   &&   (desc_dev->bLength == USB_DT_DEVICE_SIZE)
                   &&   (desc_dev->bDescriptorType == USB_DT_DEVICE)
                   &&   (desc_dev->bNumConfigurations == dev->num_of_config));

            // config ep0
            ep_size = desc_dev->bMaxPacketSize0;
            if (    vk_usbd_drv_ep_add(0 | USB_DIR_OUT, USB_EP_TYPE_CONTROL, ep_size)
                ||  vk_usbd_drv_ep_add(0 | USB_DIR_IN, USB_EP_TYPE_CONTROL, ep_size)) {
                // TODO:
                return;
            }
#endif

            vsf_usbd_notify_user(dev, evt, NULL);
            vk_usbd_drv_set_address(0);
            break;
        }
    case USB_ON_SETUP: {
            vk_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
            struct usb_ctrlrequest_t *request = &ctrl_handler->request;
            vk_usbd_trans_t *trans = &ctrl_handler->trans;

            vk_usbd_drv_get_setup(request);
            if (    VSF_ERR_NONE != vsf_usbd_notify_user(dev, evt, request)
#if VSF_USBD_CFG_RAW_MODE != ENABLED
                ||  (VSF_ERR_NONE != __vk_usbd_ctrl_prepare(dev))
#endif
                ) {
                vk_usbd_drv_ep_set_stall(0 | USB_DIR_OUT);
                vk_usbd_drv_ep_set_stall(0 | USB_DIR_IN);
                break;
            }

            if (ctrl_handler->trans.use_as__vsf_mem_t.size > request->wLength) {
                ctrl_handler->trans.use_as__vsf_mem_t.size = request->wLength;
            }

            if ((request->bRequestType & USB_DIR_MASK) == USB_DIR_OUT) {
                if (0 == request->wLength) {
                    __vk_usbd_setup_status_callback((void *)dev);
                } else {
                    trans->on_finish = __vk_usbd_setup_status_callback;
                    vk_usbd_ep_recv(dev, trans);
                }
            } else {
                trans->on_finish = __vk_usbd_setup_status_callback;
                trans->zlp = ctrl_handler->trans.use_as__vsf_mem_t.size < request->wLength;
                vk_usbd_ep_send(dev, trans);
            }
            break;
        }
    case USB_ON_STATUS:
        vsf_usbd_notify_user(dev, evt, &dev->ctrl_handler.request);
#if VSF_USBD_CFG_RAW_MODE != ENABLED
        __vk_usbd_ctrl_process(dev);
#endif
        break;
    case USB_ON_IN: {
            uint_fast8_t ep = value | USB_DIR_IN;
            vk_usbd_trans_t *trans = __vk_usbd_get_trans(dev, ep);
            VSF_USB_ASSERT(trans != NULL);

            if (trans->use_as__vsf_mem_t.size) {
                __vk_usbd_ep_send_imp(dev, trans);
            } else if (trans->zlp) {
                trans->zlp = false;
                vk_usbd_drv_ep_transaction_set_data_size(ep, 0);
            } else {
                __vk_usbd_trans_finish(dev, trans);
            }
            break;
        }
    case USB_ON_OUT: {
            uint_fast8_t ep = value | USB_DIR_OUT;
            vk_usbd_trans_t *trans = __vk_usbd_get_trans(dev, ep);
            VSF_USB_ASSERT(trans != NULL);

            if (!trans->use_as__vsf_mem_t.buffer) {
                __vk_usbd_trans_finish(dev, trans);
            } else {
                uint_fast32_t pkg_size = vk_usbd_drv_ep_get_data_size(ep);

                if (vk_usbd_drv_ep_get_feature(trans->ep, trans->feature) & USB_DC_FEATURE_TRANSFER) {
                    VSF_USB_ASSERT(trans->use_as__vsf_mem_t.size >= pkg_size);
                    trans->use_as__vsf_mem_t.size -= pkg_size;
                    __vk_usbd_trans_finish(dev, trans);
                } else {
                    uint_fast16_t ep_size = vk_usbd_drv_ep_get_size(ep);
                    // ignore the over-run data
                    pkg_size = min(pkg_size, trans->use_as__vsf_mem_t.size);
                    vk_usbd_drv_ep_transaction_read_buffer(ep, trans->cur, pkg_size);
                    trans->cur += pkg_size;
                    trans->use_as__vsf_mem_t.size -= pkg_size;

                    // TODO: check trans->zlp
                    if ((trans->use_as__vsf_mem_t.size > 0) && (pkg_size == ep_size)) {
                        vk_usbd_drv_ep_transaction_enable_out(ep);
                    } else {
                        __vk_usbd_trans_finish(dev, trans);
                    }
                }
            }
            break;
        }
    default:
        break;
    }
}

void vk_usbd_connect(vk_usbd_dev_t *dev)
{
    VSF_USB_ASSERT(dev != NULL);
    VSF_USBD_DRV_PREPARE(dev);
    vk_usbd_drv_connect();
}

void vk_usbd_disconnect(vk_usbd_dev_t *dev)
{
    VSF_USB_ASSERT(dev != NULL);
    VSF_USBD_DRV_PREPARE(dev);
    vk_usbd_drv_disconnect();
}

void vk_usbd_wakeup(vk_usbd_dev_t *dev)
{
    VSF_USB_ASSERT(dev != NULL);
    VSF_USBD_DRV_PREPARE(dev);
    vk_usbd_drv_wakeup();
}

#if VSF_USBD_CFG_RAW_MODE != ENABLED
vk_usbd_cfg_t * vk_usbd_get_cur_cfg(vk_usbd_dev_t *dev)
{
    return &dev->config[dev->configuration];
}

vk_usbd_ifs_t * vk_usbd_get_ifs(vk_usbd_dev_t *dev, uint_fast8_t ifs_no)
{
    vk_usbd_cfg_t *config = vk_usbd_get_cur_cfg(dev);
    if (ifs_no < config->num_of_ifs) {
        return &config->ifs[ifs_no];
    }
    return NULL;
}
#endif

void vk_usbd_init(vk_usbd_dev_t *dev)
{
    VSF_USB_ASSERT(dev != NULL);

#if VSF_USBD_CFG_RAW_MODE != ENABLED
    VSF_USB_ASSERT(dev->config != NULL);

    dev->configured = false;
    dev->configuration = 0;
    dev->feature = 0;
#endif

    dev->ctrl_handler.trans.ep = 0;
    dev->ctrl_handler.trans.param = dev;

#if VSF_USBD_CFG_RAW_MODE != ENABLED
    vk_usbd_desc_t *desc;
    vk_usbd_cfg_t *config = dev->config;
    vk_usbd_ifs_t *ifs;
    struct usb_config_desc_t *desc_config;

    for (uint_fast8_t i = 0; i < dev->num_of_config; i++, config++) {
#   if VSF_USBD_CFG_AUTOSETUP == ENABLED
        desc = vk_usbd_get_descriptor(dev->desc, dev->num_of_desc, USB_DT_CONFIG, i, 0);
        VSF_USB_ASSERT(desc != NULL);
        desc_config = (struct usb_config_desc_t *)desc->buffer;
        VSF_USB_ASSERT(     (desc->size == le16_to_cpu(desc_config->wTotalLength))
                        &&  (desc_config->bLength == USB_DT_CONFIG_SIZE)
                        &&  (desc_config->bDescriptorType == USB_DT_CONFIG)
                        &&  (desc_config->bNumInterfaces == config->num_of_ifs));
        config->configuration_value = desc_config->bConfigurationValue;
#   endif

        ifs = config->ifs;
        for (uint_fast8_t i = 0; i < config->num_of_ifs; i++, ifs++) {
            ifs->is_inited = false;
        }
    }
#endif

#if VSF_USBD_CFG_USE_EDA == ENABLED
    dev->eda.fn.evthandler = __vk_usbd_evthandler;
#   if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    dev->eda.on_terminate = NULL;
#   endif
    vsf_eda_init(&dev->eda, VSF_USBD_CFG_EDA_PRIORITY, false);
#endif

    __vk_usbd_hw_init_reset(dev, false);
    vsf_usbd_notify_user(dev, (usb_evt_t)USB_ON_INIT, NULL);
}

// TODO: check fini and re-init
void vk_usbd_fini(vk_usbd_dev_t *dev)
{
    VSF_USB_ASSERT(dev != NULL);
    VSF_USBD_DRV_PREPARE(dev);

    vk_usbd_drv_disconnect();
    vk_usbd_drv_fini();
    vsf_usbd_notify_user(dev, (usb_evt_t)USB_ON_FINI, NULL);
}

// TODO: move stream related code into vk_usbd_stream.c
#if VSF_USBD_CFG_STREAM_EN == ENABLED
#if VSF_USE_SIMPLE_STREAM == ENABLED

static void __vk_usbd_stream_tx_recv(vk_usbd_ep_stream_t *stream_ep, uint_fast16_t ep_size)
{
    uint8_t *buffer;
    uint_fast32_t size, wbuf_size = vsf_stream_get_wbuf(stream_ep->stream, &buffer);

    if (stream_ep->total_size > 0) {
        uint_fast32_t remain_size = stream_ep->total_size - stream_ep->transfered_size;
        size = min(wbuf_size, remain_size);
    } else {
        size = wbuf_size;
    }

    if (size >= ep_size) {
        stream_ep->use_as__vk_usbd_trans_t.use_as__vsf_mem_t.size = size;
        stream_ep->use_as__vk_usbd_trans_t.use_as__vsf_mem_t.buffer = buffer;
    } else {
        stream_ep->use_as__vk_usbd_trans_t.use_as__vsf_mem_t.size = 0;
        stream_ep->use_as__vk_usbd_trans_t.use_as__vsf_mem_t.buffer = NULL;
    }
    stream_ep->cur_size = stream_ep->size;
    vk_usbd_ep_recv(stream_ep->dev, &stream_ep->use_as__vk_usbd_trans_t);
}

static void __vk_usbd_stream_tx_on_trans_finish(void *param)
{
    vk_usbd_ep_stream_t *stream_ep = (vk_usbd_ep_stream_t *)param;
    VSF_USBD_DRV_PREPARE(stream_ep->dev);
    uint_fast16_t ep_size = vk_usbd_drv_ep_get_size(stream_ep->ep);
    vsf_stream_t *stream = stream_ep->stream;
    uint_fast8_t ep = stream_ep->use_as__vk_usbd_trans_t.ep;
    uint_fast32_t pkg_size;

    if (0 == stream_ep->cur_size) {
        uint8_t *buffer;
        uint_fast32_t size;
        pkg_size = vk_usbd_drv_ep_get_data_size(ep);
        for (uint_fast16_t size_read = pkg_size; size_read > 0;) {
            size = vsf_stream_get_wbuf(stream, &buffer);
            size = min(size, size_read);
            vk_usbd_drv_ep_transaction_read_buffer(ep, buffer, size);
            size_read -= size;
        }
    } else {
        pkg_size = stream_ep->cur_size - stream_ep->size;
        stream_ep->cur_size = 0;
    }

    vsf_stream_write(stream, NULL, pkg_size);
    stream_ep->transfered_size += pkg_size;

    if (!stream_ep->total_size || (stream_ep->transfered_size < stream_ep->total_size)) {
        if (vsf_stream_get_free_size(stream) >= ep_size) {
            __vk_usbd_stream_tx_recv(stream_ep, ep_size);
        } else {
            stream_ep->use_as__vk_usbd_trans_t.on_finish = NULL;
        }
    } else if (stream_ep->callback.on_finish != NULL) {
        vsf_stream_disconnect_tx(stream);
        stream_ep->callback.on_finish(stream_ep->callback.param);
    }
}

static void __vk_usbd_stream_tx_evthandler(void *param, vsf_stream_evt_t evt)
{
    vk_usbd_ep_stream_t *stream_ep = (vk_usbd_ep_stream_t *)param;
    VSF_USBD_DRV_PREPARE(stream_ep->dev);
    uint_fast16_t ep_size = vk_usbd_drv_ep_get_size(stream_ep->use_as__vk_usbd_trans_t.ep);
    vsf_stream_t *stream = stream_ep->stream;

    switch (evt) {
    case VSF_STREAM_ON_CONNECT:
    case VSF_STREAM_ON_OUT:
        if (    (NULL == stream_ep->on_finish)
            &&  (   !stream_ep->total_size
                ||  (stream_ep->transfered_size < stream_ep->total_size))) {
            if (vsf_stream_get_free_size(stream) >= ep_size) {
                stream_ep->use_as__vk_usbd_trans_t.on_finish = __vk_usbd_stream_tx_on_trans_finish;
                __vk_usbd_stream_tx_recv(stream_ep, ep_size);
            }
        }
        break;
    }
}

vsf_err_t vk_usbd_ep_recv_stream(vk_usbd_ep_stream_t *stream_ep, uint_fast32_t size)
{
    vk_usbd_trans_t *trans = &stream_ep->use_as__vk_usbd_trans_t;
    vsf_stream_t *stream = stream_ep->stream;

    stream_ep->total_size = size;
    stream_ep->transfered_size = 0;
    stream->tx.param = stream_ep;
    stream->tx.evthandler = __vk_usbd_stream_tx_evthandler;
    trans->param = stream_ep;
    trans->on_finish = NULL;
    trans->buffer = NULL;
    vsf_stream_connect_tx(stream);

    __vk_usbd_stream_tx_evthandler(stream_ep, VSF_STREAM_ON_OUT);
    return VSF_ERR_NONE;
}

static void __vk_usbd_stream_rx_on_trans_finish(void *param)
{
    vk_usbd_ep_stream_t *stream_ep = (vk_usbd_ep_stream_t *)param;
    vsf_stream_t *stream = stream_ep->stream;

    if (stream_ep->cur_size > 0) {
        stream_ep->transfered_size += stream_ep->cur_size;
        vsf_stream_read(stream_ep->stream, NULL, stream_ep->cur_size);
        stream_ep->cur_size = 0;
    }

    if (!stream_ep->total_size || (stream_ep->transfered_size < stream_ep->total_size)) {
        stream_ep->size = vsf_stream_get_rbuf(stream, &stream_ep->buffer);
        if (stream_ep->size > 0) {
            stream_ep->cur_size = stream_ep->size;
            vk_usbd_ep_send(stream_ep->dev, &stream_ep->use_as__vk_usbd_trans_t);
        } else {
            stream_ep->on_finish = NULL;
        }
    } else if (stream_ep->callback.on_finish != NULL) {
        vsf_stream_disconnect_rx(stream);
        stream_ep->callback.on_finish(stream_ep->callback.param);
    }
}

static void __vk_usbd_stream_rx_evthandler(void *param, vsf_stream_evt_t evt)
{
    vk_usbd_ep_stream_t *stream_ep = (vk_usbd_ep_stream_t *)param;
    vk_usbd_dev_t *dev = stream_ep->dev;
    vsf_stream_t *stream = stream_ep->stream;

    switch (evt) {
    case VSF_STREAM_ON_RX:
        if (    (NULL == stream_ep->on_finish)
            &&  (   !stream_ep->total_size
                ||  (stream_ep->transfered_size < stream_ep->total_size))) {
            stream_ep->size = vsf_stream_get_rbuf(stream, &stream_ep->buffer);
            if (stream_ep->size > 0) {
                stream_ep->cur_size = stream_ep->size;
                stream_ep->on_finish = __vk_usbd_stream_rx_on_trans_finish;
                stream_ep->use_as__vk_usbd_trans_t.zlp = stream_ep->zlp_save;
                vk_usbd_ep_send(dev, &stream_ep->use_as__vk_usbd_trans_t);
            }
        }
        break;
    }
}

vsf_err_t vk_usbd_ep_send_stream(vk_usbd_ep_stream_t *stream_ep, uint_fast32_t size)
{
    vk_usbd_trans_t *trans = &stream_ep->use_as__vk_usbd_trans_t;
    vsf_stream_t *stream = stream_ep->stream;

    stream_ep->zlp_save = trans->zlp;
    stream_ep->total_size = size;
    stream_ep->transfered_size = 0;
    stream->rx.param = stream_ep;
    stream->rx.evthandler = __vk_usbd_stream_rx_evthandler;
    trans->param = stream_ep;
    trans->on_finish = NULL;
    vsf_stream_connect_rx(stream);

    __vk_usbd_stream_rx_evthandler(stream_ep, VSF_STREAM_ON_IN);
    return VSF_ERR_NONE;
}

#elif VSF_USE_STREAM == ENABLED

static vsf_err_t __vk_usbd_ep_rcv_pbuf(vk_usbd_ep_stream_t *this_ptr)
{
    vsf_err_t result = VSF_ERR_NOT_READY;
    VSF_USBD_DRV_PREPARE(this_ptr->dev);

    do {
        if (NULL == this_ptr->dev) {
            break ;
        }

        this_ptr->rx_current = vsf_stream_src_new_pbuf(
            &this_ptr->use_as__vsf_stream_src_t,
            //!//! require a big enough pbuf
            vk_usbd_drv_ep_get_size(this_ptr->rx_trans.ep),
            -1);

        if (NULL == this_ptr->rx_current) {
            VSF_USB_ASSERT(false);
            result = VSF_ERR_NOT_ENOUGH_RESOURCES;
            break;
        }

        vk_usbd_trans_t *trans = &this_ptr->rx_trans;
        trans->param = this_ptr;

        trans->use_as__vsf_mem_t.src_ptr = vsf_pbuf_buffer_get(this_ptr->rx_current);
        trans->use_as__vsf_mem_t.size = vsf_pbuf_size_get(this_ptr->rx_current);
        vk_usbd_ep_recv(this_ptr->dev, trans);
        result = VSF_ERR_NONE;
    } while(0);
    return result;
}



static vsf_err_t __vk_usbd_on_ep_rcv(vk_usbd_ep_stream_t *this_ptr)
{
    vsf_err_t result;// = VSF_ERR_NOT_READY;
    vsf_pbuf_t *pbuf;
    uint_fast16_t ep_left_size;
    VSF_USBD_DRV_PREPARE(this_ptr->dev);

    __vsf_interrupt_safe(                               //! this protection might not be necessary
        pbuf = this_ptr->rx_current;
        this_ptr->rx_current = NULL;
        if (pbuf != NULL) {
            ep_left_size = this_ptr->rx_trans.use_as__vsf_mem_t.size;
        }
        result = __vk_usbd_ep_rcv_pbuf(this_ptr);       //! start next rcv
    )
    if (NULL != pbuf) {
        vsf_pbuf_size_set(  pbuf,
                            vk_usbd_drv_ep_get_size(this_ptr->rx_trans.ep) - ep_left_size);
        vsf_stream_src_send_pbuf(&this_ptr->use_as__vsf_stream_src_t, pbuf);
    }

    return result;
}

vsf_err_t vk_usbd_ep_recv_stream(vk_usbd_ep_stream_t *this_ptr)
{
    vsf_err_t result = VSF_ERR_NOT_READY;
    __vsf_interrupt_safe(
        if (NULL == this_ptr->rx_current) {
            result = __vk_usbd_on_ep_rcv(this_ptr);
        }
    )

    return result;
}

static void __vk_usbd_ep_on_stream_rx_finish(void *param)
{
    vk_usbd_ep_stream_t *this_ptr = (vk_usbd_ep_stream_t *)param;

    __vk_usbd_on_ep_rcv(this_ptr);
}

static vsf_err_t __vk_usbd_ep_send_pbuf(vk_usbd_ep_stream_t *this_ptr)
{
    vsf_err_t result = VSF_ERR_NOT_READY;
    do {
        if (NULL == this_ptr->dev) {
            break ;
        }
        this_ptr->tx_current = vsf_stream_usr_fetch_pbuf(&this_ptr->use_as__vsf_stream_usr_t);
        if (NULL == this_ptr->tx_current) {
            result = VSF_ERR_NONE;
            break;
        }

        vk_usbd_trans_t *trans = &this_ptr->tx_trans;
        trans->param = this_ptr;

        trans->use_as__vsf_mem_t.src_ptr = vsf_pbuf_buffer_get(this_ptr->tx_current);
        trans->use_as__vsf_mem_t.size = vsf_pbuf_size_get(this_ptr->tx_current);

        vk_usbd_ep_send(this_ptr->dev, trans);
        result = VSF_ERR_NONE;
    } while(0);
    return result;
}

static void __vk_usbd_ep_on_stream_tx_finish(void *param)
{
    vk_usbd_ep_stream_t *this_ptr = (vk_usbd_ep_stream_t *)param;

    vsf_pbuf_t *ptBuff;

    __vsf_interrupt_safe(
        ptBuff = this_ptr->tx_current;
        this_ptr->tx_current = NULL;
        __vk_usbd_ep_send_pbuf(this_ptr);
    )

    vsf_pbuf_free(ptBuff);       //! free old pbuf
}



vsf_err_t vk_usbd_ep_send_stream(vk_usbd_ep_stream_t *this_ptr)
{
    vsf_err_t result = VSF_ERR_NOT_READY;
    VSF_USB_ASSERT(NULL != this_ptr);

    __vsf_interrupt_safe(
        if (NULL == this_ptr->tx_current) {
            result = __vk_usbd_ep_send_pbuf(this_ptr);
        }
    )
    return result;
}

static void __vk_usbd_on_data_ready_event( void *target_ptr,
                                            vsf_stream_rx_t *ptRX,
                                            vsf_stream_status_t Status)
{
    vk_usbd_ep_stream_t *this_ptr = (vk_usbd_ep_stream_t *)target_ptr;
    vk_usbd_ep_send_stream(this_ptr);
}

void vk_usbd_ep_stream_init(   vk_usbd_ep_stream_t *this_ptr,
                                vk_usbd_ep_stream_cfg_t *cfg)
{
    VSF_USB_ASSERT(NULL != this_ptr);
    this_ptr->tx_trans.on_finish = __vk_usbd_ep_on_stream_tx_finish;
    this_ptr->rx_trans.on_finish = __vk_usbd_ep_on_stream_rx_finish;
    this_ptr->tx_current = NULL;
    this_ptr->rx_current = NULL;

    this_ptr->rx_trans.ep = cfg->rx_ep;
    this_ptr->rx_trans.zlp = false;
    this_ptr->tx_trans.ep = cfg->tx_ep;
    this_ptr->tx_trans.zlp = true;
    this_ptr->dev = NULL;

    //! access protected member of vsf_stream_usr_t
    with_protected(vsf_stream_usr_t, &this_ptr->use_as__vsf_stream_usr_t, {
        if (NULL != _->ptRX) {
            //! register data rdy event handler
            _->ptRX->piMethod->DataReadyEvent.Register(
                _->ptRX,
                (vsf_stream_dat_rdy_evt_t){__vk_usbd_on_data_ready_event, this_ptr});
        }
    });


#if VSF_STREAM_CFG_SUPPORT_OPEN_CLOSE == ENABLED
    vsf_stream_usr_open(&(this_ptr->use_as__vsf_stream_usr_t));
#endif
}

void vk_usbd_ep_stream_connect_dev(vk_usbd_ep_stream_t *this_ptr, vk_usbd_dev_t *dev)
{
    VSF_USB_ASSERT(NULL != this_ptr);
    this_ptr->dev = dev;
    vk_usbd_ep_send_stream(this_ptr);
}

#endif      // VSF_USE_STREAM || VSF_USE_SIMPLE_STREAM
#endif      // VSF_USBD_CFG_STREAM_EN
#endif      // VSF_USE_USB_DEVICE
