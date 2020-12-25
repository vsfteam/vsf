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

#if VSF_USE_USB_HOST == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_USBH_CLASS_IMPLEMENT

#include "./vsf_usbh.h"
#include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/

#define USB_DEFAULT_TIMEOUT         50    // 50ms

#if VSF_USBH_CFG_ENABLE_ROOT_HUB == ENABLED
#   define VSF_USBH_REL             0
#   define VSF_USBH_VER             0
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

#if VSF_USBH_CFG_ENABLE_ROOT_HUB == ENABLED
/* usb 2.0 root hub device descriptor */
static const uint8_t __vk_usb_rh_dev_descriptor[18] = {
    0x12,       /*  __u8  bLength; */
    0x01,       /*  __u8  bDescriptorType; Device */
    0x00, 0x00, /*  __le16 bcdUSB: to be patched */

    0x09,        /*  __u8  bDeviceClass; HUB_CLASSCODE */
    0x00,        /*  __u8  bDeviceSubClass; */
    0x00,       /*  __u8  bDeviceProtocol; [ usb 2.0 no TT ] */
    0x40,       /*  __u8  bMaxPacketSize0; 64 Bytes */

    0x6b, 0x1d, /*  __le16 idVendor; Linux Foundation 0x1d6b */
    0x02, 0x00, /*  __le16 idProduct; device 0x0002 */
    VSF_USBH_VER, VSF_USBH_REL, /*  __le16 bcdDevice */

    0x00,       /*  __u8  iManufacturer; */
    0x00,       /*  __u8  iProduct; */
    0x00,       /*  __u8  iSerialNumber; */
    0x01        /*  __u8  bNumConfigurations; */
};

/* no usb 2.0 root hub "device qualifier" descriptor: one speed only */

static const uint8_t __vk_usbh_rh_config_descriptor[] = {
    /* one configuration */
    0x09,       /*  __u8  bLength; */
    0x02,       /*  __u8  bDescriptorType; Configuration */
    0x19, 0x00, /*  __le16 wTotalLength; */
    0x01,       /*  __u8  bNumInterfaces; (1) */
    0x01,       /*  __u8  bConfigurationValue; */
    0x00,       /*  __u8  iConfiguration; */
    0xc0,       /*  __u8  bmAttributes;
                 Bit 7: must be set,
                     6: Self-powered,
                     5: Remote wakeup,
                     4..0: resvd */
    0x00,       /*  __u8  MaxPower; */

    /* USB 1.1:
     * USB 2.0, single TT organization (mandatory):
     *    one interface, protocol 0
     *
     * USB 2.0, multiple TT organization (optional):
     *    two interfaces, protocols 1 (like single TT)
     *    and 2 (multiple TT mode) ... config is
     *    sometimes settable
     *    NOT IMPLEMENTED
     */

    /* one interface */
    0x09,       /*  __u8  if_bLength; */
    0x04,       /*  __u8  if_bDescriptorType; Interface */
    0x00,       /*  __u8  if_bInterfaceNumber; */
    0x00,       /*  __u8  if_bAlternateSetting; */
    0x01,       /*  __u8  if_bNumEndpoints; */
    0x09,       /*  __u8  if_bInterfaceClass; HUB_CLASSCODE */
    0x00,       /*  __u8  if_bInterfaceSubClass; */
    0x00,       /*  __u8  if_bInterfaceProtocol; [usb1.1 or single tt] */
    0x00,       /*  __u8  if_iInterface; */

    /* one endpoint (status change endpoint) */
    0x07,       /*  __u8  ep_bLength; */
    0x05,       /*  __u8  ep_bDescriptorType; Endpoint */
    0x81,       /*  __u8  ep_bEndpointAddress; IN Endpoint 1 */
    0x03,       /*  __u8  ep_bmAttributes; Interrupt */
    (15 + 1 + 7) / 8, 0x00, /*  __le16 ep_wMaxPacketSize; 1 + (MAX_ROOT_PORTS / 8) */
    0xff        /*  __u8  ep_bInterval; (255ms -- usb 2.0 spec) */
};
#endif

/*============================ PROTOTYPES ====================================*/

extern void vsf_usbh_on_dev_parsed(vk_usbh_dev_t *dev, vk_usbh_dev_parser_t *parser);
extern vsf_err_t vsf_usbh_on_match_interface(
        vk_usbh_dev_parser_t *parser, vk_usbh_ifs_parser_t *parser_ifs);
extern void vsf_usbh_on_remove_interface(vk_usbh_ifs_t *ifs);

/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSF_USBH_ON_DEV_PARSED
WEAK(vsf_usbh_on_dev_parsed)
void vsf_usbh_on_dev_parsed(vk_usbh_dev_t *dev, vk_usbh_dev_parser_t *parser)
{

}
#endif

#ifndef WEAK_VSF_USBH_ON_MATCH_INTERFACE
WEAK(vsf_usbh_on_match_interface)
vsf_err_t vsf_usbh_on_match_interface(
        vk_usbh_dev_parser_t *parser, vk_usbh_ifs_parser_t *parser_ifs)
{
    vsf_trace_info("%s: vid%04X pid%04X interface%d" VSF_TRACE_CFG_LINEEND,
                parser_ifs->ifs->drv->name,
                parser->desc_device->idVendor, parser->desc_device->idProduct,
                parser_ifs->parser_alt[parser_ifs->ifs->cur_alt].desc_ifs->bInterfaceNumber);
    return VSF_ERR_NONE;
}
#endif

#ifndef WEAK_VSF_USBH_ON_REMOVE_INTERFACE
WEAK(vsf_usbh_on_remove_interface)
void vsf_usbh_on_remove_interface(vk_usbh_ifs_t *ifs)
{
    vsf_trace_info("%s: remove interface" VSF_TRACE_CFG_LINEEND, ifs->drv->name);
}
#endif

vk_usbh_pipe_t vk_usbh_get_pipe(vk_usbh_dev_t *dev,
            uint_fast8_t endpoint, uint_fast8_t type, uint_fast16_t size)
{
    uint_fast8_t direction = endpoint & USB_DIR_MASK;
    vk_usbh_pipe_t pipe;

    endpoint &= 0x0F;
    pipe.value =   1|   (size << 2)             /* 10-bit size */
                    |   (endpoint << 13)        /* 4-bit endpoint */
                    |   (type << 17)            /* 2-bit type */
                    |   (dev->speed << 19)      /* 2-bit speed */
                    |   (dev->devnum << 21)     /* 7-bit address */
                    |   (direction << 21);      /* 1-bit direction */
    return pipe;
}

vk_usbh_pipe_t vk_usbh_get_pipe_from_ep_desc(vk_usbh_dev_t *dev,
            struct usb_endpoint_desc_t *desc_ep)
{
    return vk_usbh_get_pipe(dev, desc_ep->bEndpointAddress,
            desc_ep->bmAttributes, le16_to_cpu(desc_ep->wMaxPacketSize));
}

void vk_usbh_urb_prepare_by_pipe(vk_usbh_urb_t *urb, vk_usbh_dev_t *dev,
            vk_usbh_pipe_t pipe)
{
    VSF_USB_ASSERT((urb != NULL) && (dev != NULL));
    urb->pipe = pipe;
}

void vk_usbh_urb_prepare(vk_usbh_urb_t *urb, vk_usbh_dev_t *dev,
            struct usb_endpoint_desc_t *desc_ep)
{
    VSF_USB_ASSERT((urb != NULL) && (dev != NULL) && (desc_ep != NULL));
    urb->pipe = vk_usbh_get_pipe_from_ep_desc(dev, desc_ep);
}

bool vk_usbh_urb_is_valid(vk_usbh_urb_t *urb)
{
    VSF_USB_ASSERT(urb != NULL);
    return urb->pipe.value != 0;
}

bool vk_usbh_urb_is_alloced(vk_usbh_urb_t *urb)
{
    VSF_USB_ASSERT(urb != NULL);
    return !urb->pipe.is_pipe && (urb->urb_hcd != NULL);
}

vk_usbh_pipe_t vk_usbh_urb_get_pipe(vk_usbh_urb_t *urb)
{
    VSF_USB_ASSERT(urb != NULL);
    if (urb->pipe.is_pipe) {
        return urb->pipe;
    } else {
        return urb->urb_hcd->pipe;
    }
}

void vk_usbh_urb_set_pipe(vk_usbh_urb_t *urb, vk_usbh_pipe_t pipe)
{
    VSF_USB_ASSERT(urb != NULL && pipe.is_pipe);
    if (urb->pipe.is_pipe) {
        urb->pipe = pipe;
    } else {
        urb->urb_hcd->pipe = pipe;
    }
}

static void __vk_usbh_urb_reset_buffer(vk_usbh_hcd_urb_t *urb_hcd)
{
    urb_hcd->buffer = NULL;
    urb_hcd->transfer_length = 0;
    urb_hcd->free_buffer = NULL;
}

void vk_usbh_hcd_urb_free_buffer(vk_usbh_hcd_urb_t *urb_hcd)
{
    VSF_USB_ASSERT(urb_hcd != NULL);
    if (urb_hcd->buffer && (urb_hcd->free_buffer != NULL)) {
        urb_hcd->free_buffer(urb_hcd->free_buffer_param);
    }
    __vk_usbh_urb_reset_buffer(urb_hcd);
}

static void __vk_usbh_free_buffer(void *buffer)
{
    vsf_usbh_free(buffer);
}

void * vk_usbh_hcd_urb_alloc_buffer(vk_usbh_hcd_urb_t *urb_hcd, uint_fast16_t size)
{
    VSF_USB_ASSERT((urb_hcd != NULL) && (size > 0));
    vk_usbh_hcd_urb_free_buffer(urb_hcd);
    urb_hcd->buffer = vsf_usbh_malloc(size);
    urb_hcd->transfer_length = size;
    urb_hcd->free_buffer = __vk_usbh_free_buffer;
    urb_hcd->free_buffer_param = urb_hcd->buffer;
    return urb_hcd->buffer;
}

uint_fast16_t vk_usbh_get_frame(vk_usbh_t *usbh)
{
    return usbh->drv->get_frame_number(&usbh->use_as__vk_usbh_hcd_t);
}

static vk_usbh_dev_t * __vk_usbh_alloc_device(vk_usbh_t *usbh)
{
    vk_usbh_dev_t *dev;

    VSF_USB_ASSERT(usbh != NULL);

    dev = vsf_usbh_malloc(sizeof(vk_usbh_dev_t));
    if (NULL == dev) { return NULL; }
    memset(dev, 0, sizeof(vk_usbh_dev_t));

    if (    (usbh->drv->alloc_device != NULL)
        &&  (usbh->drv->alloc_device(&usbh->use_as__vk_usbh_hcd_t, &dev->use_as__vk_usbh_hcd_dev_t) != VSF_ERR_NONE)) {
free_dev:
        vsf_usbh_free(dev);
        return NULL;
    }

    dev->devnum = vsf_bitmap_ffz(&usbh->device_bitmap, VSF_USBH_CFG_MAX_DEVICE);
    if (dev->devnum == 0) { goto free_dev; }
    vsf_bitmap_set(&usbh->device_bitmap, dev->devnum);
    return dev;
}

vk_usbh_dev_t * vk_usbh_new_device(vk_usbh_t *usbh, enum usb_device_speed_t speed,
            vk_usbh_dev_t *dev_parent, uint_fast8_t idx)
{
    vk_usbh_dev_t *dev_new = __vk_usbh_alloc_device(usbh);
    if (dev_new != NULL) {
        dev_new->speed = speed;
        if (dev_parent != NULL) {
#if VSF_USBH_USE_HUB == ENABLED
            dev_new->index = idx;
            dev_new->dev_parent = dev_parent;
            vsf_slist_add_to_head(vk_usbh_dev_t, child_node, &dev_parent->children_list, dev_new);
#else
            VSF_USB_ASSERT(false);
#endif
        }

        // notify to emulate new device
        usbh->dev_new = dev_new;
        vsf_eda_post_evt(&usbh->teda.use_as__vsf_eda_t, VSF_EVT_INIT);
    }
    return dev_new;
}

static void __vk_usbh_clean_device(vk_usbh_t *usbh, vk_usbh_dev_t *dev)
{
    VSF_USB_ASSERT((usbh != NULL) && (dev != NULL));
    vk_usbh_free_urb(usbh, &dev->ep0.urb);
    if (usbh->drv->free_device != NULL) {
        usbh->drv->free_device(&usbh->use_as__vk_usbh_hcd_t, &dev->use_as__vk_usbh_hcd_dev_t);
    }
    if (dev->ifs != NULL) {
        vsf_usbh_free(dev->ifs);
        dev->ifs = NULL;
    }
}

void vk_usbh_reset_dev(vk_usbh_t *usbh, vk_usbh_dev_t *dev)
{
#if VSF_USBH_USE_HUB == ENABLED
    if (dev->dev_parent != NULL) {
        vk_usbh_hub_reset_dev(dev);
    } else
#endif
    if (    (usbh->drv->reset_dev != NULL)
#if VSF_USBH_CFG_ENABLE_ROOT_HUB == ENABLED
        &&  (usbh->dev_rh != dev)
#endif
        ) {
        usbh->drv->reset_dev(&usbh->use_as__vk_usbh_hcd_t, &dev->use_as__vk_usbh_hcd_dev_t);
    }
}

static bool __vk_usbh_is_dev_resetting(vk_usbh_t *usbh, vk_usbh_dev_t *dev)
{
#if VSF_USBH_USE_HUB == ENABLED
    if (dev->dev_parent != NULL) {
        return vk_usbh_hub_is_dev_resetting(dev);
    } else
#endif
    if (    (usbh->drv->is_dev_reset != NULL)
#if VSF_USBH_CFG_ENABLE_ROOT_HUB == ENABLED
        &&  (usbh->dev_rh != dev)
#endif
        ) {
        return usbh->drv->is_dev_reset(&usbh->use_as__vk_usbh_hcd_t, &dev->use_as__vk_usbh_hcd_dev_t);
    }
    return false;
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

vsf_err_t vk_usbh_alloc_urb(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_urb_t *urb)
{
    vk_usbh_pipe_t pipe;

    VSF_USB_ASSERT((usbh != NULL) && (dev != NULL) && (urb != NULL) && urb->pipe.is_pipe);
    pipe = urb->pipe;
    urb->urb_hcd = usbh->drv->alloc_urb(&usbh->use_as__vk_usbh_hcd_t);
    if (urb->urb_hcd != NULL) {
        urb->urb_hcd->pipe = pipe;
        urb->urb_hcd->dev_hcd = &dev->use_as__vk_usbh_hcd_dev_t;
        return VSF_ERR_NONE;
    } else {
        urb->pipe = pipe;
        VSF_USB_ASSERT(false);
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }
}

void vk_usbh_free_urb(vk_usbh_t *usbh, vk_usbh_urb_t *urb)
{
    VSF_USB_ASSERT((usbh != NULL) && (urb != NULL));

    if (vk_usbh_urb_is_alloced(urb)) {
        vk_usbh_pipe_t pipe = urb->urb_hcd->pipe;
        usbh->drv->free_urb(&usbh->use_as__vk_usbh_hcd_t, urb->urb_hcd);
        urb->pipe = pipe;
    }
}

void vk_usbh_urb_free_buffer(vk_usbh_urb_t *urb)
{
    VSF_USB_ASSERT(urb != NULL);
    if (!urb->pipe.is_pipe) {
        vk_usbh_hcd_urb_free_buffer(urb->urb_hcd);
    }
}

void * vk_usbh_urb_alloc_buffer(vk_usbh_urb_t *urb, uint_fast16_t size)
{
    VSF_USB_ASSERT((urb != NULL) && (size > 0) && !urb->pipe.is_pipe);
    return vk_usbh_hcd_urb_alloc_buffer(urb->urb_hcd, size);
}

void * vk_usbh_urb_peek_buffer(vk_usbh_urb_t *urb)
{
    return urb->urb_hcd->buffer;
}

void * vk_usbh_urb_take_buffer(vk_usbh_urb_t *urb)
{
    VSF_USB_ASSERT((urb != NULL) && !urb->pipe.is_pipe);
    void *buffer = vk_usbh_urb_peek_buffer(urb);
    __vk_usbh_urb_reset_buffer(urb->urb_hcd);
    return buffer;
}

void vk_usbh_urb_set_buffer(vk_usbh_urb_t *urb, void *buffer,
            uint_fast32_t size)
{
    VSF_USB_ASSERT((urb != NULL) && !urb->pipe.is_pipe);
    vk_usbh_hcd_urb_t *urb_hcd = urb->urb_hcd;
    vk_usbh_urb_free_buffer(urb);
    urb_hcd->buffer = buffer;
    urb_hcd->transfer_length = size;
}

int_fast16_t vk_usbh_urb_get_status(vk_usbh_urb_t *urb)
{
    VSF_USB_ASSERT((urb != NULL) && (urb->urb_hcd != NULL) && !urb->pipe.is_pipe);
    return urb->urb_hcd->status;
}

uint_fast32_t vk_usbh_urb_get_actual_length(vk_usbh_urb_t *urb)
{
    VSF_USB_ASSERT((urb != NULL) && !urb->pipe.is_pipe);
    return urb->urb_hcd->actual_length;
}

void vk_usbh_remove_interface(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
            vk_usbh_ifs_t *ifs)
{
    VSF_USB_ASSERT((usbh != NULL) && (dev != NULL) && (ifs != NULL));
    const vk_usbh_class_drv_t *drv = ifs->drv;
    if (drv) {
        vsf_usbh_on_remove_interface(ifs);
        drv->disconnect(usbh, dev, ifs->param);
        ifs->drv = NULL;
        ifs->param = NULL;
        ifs->cur_alt = 0;
    }
}

static void __vk_usbh_reset_parser(vk_usbh_dev_parser_t *parser)
{
    if (parser->desc_config != NULL) {
        vsf_usbh_free(parser->desc_config);
        parser->desc_config = NULL;
    }

    if (parser->parser_ifs != NULL) {
        vk_usbh_ifs_parser_t *parser_ifs = parser->parser_ifs;
        for (uint_fast8_t ifs_idx = 0; ifs_idx < parser->num_of_ifs; ifs_idx++, parser_ifs++) {
            vsf_usbh_free(parser_ifs->parser_alt);
        }
        vsf_usbh_free(parser->parser_ifs);
        parser->parser_ifs = NULL;
    }
}

static void __vk_usbh_free_parser(vk_usbh_t *usbh)
{
    vk_usbh_dev_parser_t *parser = usbh->parser;
    if (parser != NULL) {
        __vk_usbh_reset_parser(parser);
        if (parser->desc_device != NULL) {
            vsf_usbh_free(parser->desc_device);
        }
        vsf_usbh_free(parser);
        usbh->parser = NULL;
    }
}

void vk_usbh_disconnect_device(vk_usbh_t *usbh, vk_usbh_dev_t *dev)
{
    VSF_USB_ASSERT((usbh != NULL) && (dev != NULL));

#if VSF_USBH_USE_HUB == ENABLED
    vsf_slist_remove(vk_usbh_dev_t, child_node, &dev->dev_parent->children_list, dev);
#endif

    if (dev->num_of_ifs && (dev->ifs != NULL)) {
        vk_usbh_ifs_t *ifs = dev->ifs;
        for (uint_fast8_t i = 0; i < dev->num_of_ifs; i++, ifs++) {
            vk_usbh_remove_interface(usbh, dev, ifs);
        }
    }

#if VSF_USBH_USE_HUB == ENABLED
    vk_usbh_dev_t *dev_child;
    while (!vsf_slist_is_empty(&dev->children_list)) {
        vsf_slist_remove_from_head_unsafe(vk_usbh_dev_t, child_node, &dev->children_list, dev_child);
        vk_usbh_disconnect_device(usbh, dev_child);
    }
#endif
    __vk_usbh_clean_device(usbh, dev);

    if (dev->devnum != 0) {
        vsf_bitmap_clear(&usbh->device_bitmap, dev->devnum);
    }

    if (usbh->dev_new == dev) {
        __vk_usbh_free_parser(usbh);
        vsf_eda_fini(&usbh->teda.use_as__vsf_eda_t);
        usbh->dev_new = NULL;
    }
    vsf_usbh_free(dev);
}

#if VSF_USBH_CFG_ENABLE_ROOT_HUB == ENABLED
static vsf_err_t __vk_usbh_rh_submit_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb_hcd)
{
    uint_fast16_t typeReq, wValue, wLength;
    struct usb_ctrlrequest_t *req = &urb_hcd->setup_packet;
    uint8_t data[32];
    int_fast16_t len = 0;

    if (urb_hcd->pipe.type == USB_ENDPOINT_XFER_INT) {
        // WARNING: not support int transfer for HUB
        return VSF_ERR_NOT_SUPPORT;
    }

    urb_hcd->actual_length = 0;

    typeReq = (req->bRequestType << 8) | req->bRequest;
    wValue = req->wValue;
    wLength = req->wLength;

    if (wLength > urb_hcd->transfer_length) {
        goto error;
    }

    switch (typeReq) {
    case DeviceRequest | USB_REQ_GET_STATUS:
        data[0] = 1;
        data[1] = 0;
        len = 2;
        break;
    case DeviceOutRequest | USB_REQ_CLEAR_FEATURE:
    case DeviceOutRequest | USB_REQ_SET_FEATURE:
        break;
    case DeviceRequest | USB_REQ_GET_CONFIGURATION:
        data[0] = 1;
        len = 1;
        break;
    case DeviceOutRequest | USB_REQ_SET_CONFIGURATION:
        break;
    case DeviceRequest | USB_REQ_GET_DESCRIPTOR:
        switch ((wValue & 0xff00) >> 8) {
        case USB_DT_DEVICE:
            memcpy(data, __vk_usb_rh_dev_descriptor, sizeof(__vk_usb_rh_dev_descriptor));
            switch (urb_hcd->pipe.speed) {
            case USB_SPEED_LOW:
            case USB_SPEED_FULL:
                ((struct usb_device_desc_t *)data)->bcdUSB = cpu_to_le16(0x0110);
                break;
            case USB_SPEED_HIGH:
                ((struct usb_device_desc_t *)data)->bcdUSB = cpu_to_le16(0x0200);
                break;
            //case USB_SPEED_VARIABLE:
            //    break;
            //case USB_SPEED_SUPER:
            //    break;
            default:
                goto error;
            }
            len = sizeof(__vk_usb_rh_dev_descriptor);
            break;
        case USB_DT_CONFIG:
            len = sizeof(__vk_usbh_rh_config_descriptor);
            memcpy(data, __vk_usbh_rh_config_descriptor, len);
            break;
        default:
            goto error;
        }
        break;
    case DeviceRequest | USB_REQ_GET_INTERFACE:
        data[0] = 0;
        len = 1;
        break;
    case DeviceOutRequest | USB_REQ_SET_INTERFACE:
        break;
    case DeviceOutRequest | USB_REQ_SET_ADDRESS:
        break;
    case EndpointRequest | USB_REQ_GET_STATUS:
        data[0] = 0;
        data[1] = 0;
        len = 2;
        break;
    case EndpointOutRequest | USB_REQ_CLEAR_FEATURE:
    case EndpointOutRequest | USB_REQ_SET_FEATURE:
        break;
    default:
        len = hcd->drv->rh_control(hcd, urb_hcd);
        if (len < 0) {
            goto error;
        }
        goto complete;
    }

    if (len > 0) {
        if (urb_hcd->transfer_length < len) {
            len = urb_hcd->transfer_length;
        }
        urb_hcd->actual_length = len;

        memcpy(urb_hcd->buffer, data, len);
    }

complete:
    urb_hcd->status = URB_OK;
    return vsf_eda_post_msg(urb_hcd->eda_caller, urb_hcd);

error:
    urb_hcd->status = URB_FAIL;
    return VSF_ERR_FAIL;
}
#endif

static vsf_err_t __vk_usbh_submit_urb_imp(vk_usbh_t *usbh, vk_usbh_urb_t *urb, vsf_eda_t *eda)
{
    vk_usbh_hcd_urb_t *urb_hcd;

    VSF_USB_ASSERT(     (usbh != NULL) && (urb != NULL) && (urb->urb_hcd != NULL)
            &&  !urb->pipe.is_pipe);

    urb_hcd = urb->urb_hcd;
    if (NULL == eda) {
        urb_hcd->eda_caller = vsf_eda_get_cur();
    } else {
        urb_hcd->eda_caller = eda;
    }

    urb_hcd->actual_length = 0;
#if VSF_USBH_CFG_ENABLE_ROOT_HUB == ENABLED
    if (urb_hcd->dev_hcd == &usbh->dev_rh->use_as__vk_usbh_hcd_dev_t) {
        return __vk_usbh_rh_submit_urb(&usbh->use_as__vk_usbh_hcd_t, urb_hcd);
    }
#endif

    if (    (   (urb_hcd->pipe.type != USB_ENDPOINT_XFER_INT)
            &&  (urb_hcd->pipe.type != USB_ENDPOINT_XFER_ISOC))
        ||  !urb_hcd->pipe.is_submitted) {
        vsf_err_t err = usbh->drv->submit_urb(&usbh->use_as__vk_usbh_hcd_t, urb_hcd);
        if (VSF_ERR_NONE == err) {
            urb_hcd->pipe.is_submitted = true;
        }
        return err;
    } else {
        return usbh->drv->relink_urb(&usbh->use_as__vk_usbh_hcd_t, urb_hcd);
    }
}

vsf_err_t vk_usbh_submit_urb(vk_usbh_t *usbh, vk_usbh_urb_t *urb)
{
    VSF_USB_ASSERT(vk_usbh_urb_is_alloced(urb));
    return __vk_usbh_submit_urb_imp(usbh, urb, NULL);
}

vsf_err_t vk_usbh_submit_urb_flags(vk_usbh_t *usbh, vk_usbh_urb_t *urb, uint_fast16_t flags)
{
    VSF_USB_ASSERT(vk_usbh_urb_is_alloced(urb));
    urb->urb_hcd->transfer_flags = flags;
    return vk_usbh_submit_urb(usbh, urb);
}

vsf_err_t vk_usbh_submit_urb_ex(vk_usbh_t *usbh, vk_usbh_urb_t *urb, uint_fast16_t flags, vsf_eda_t *eda)
{
    VSF_USB_ASSERT(vk_usbh_urb_is_alloced(urb));
    urb->urb_hcd->transfer_flags = flags;
    return __vk_usbh_submit_urb_imp(usbh, urb, eda);
}

#if VSF_USBH_CFG_ISO_EN == ENABLED
vsf_err_t vk_usbh_submit_urb_iso(vk_usbh_t *usbh, vk_usbh_urb_t *urb, uint_fast8_t start_frame)
{
    VSF_USB_ASSERT(vk_usbh_urb_is_alloced(urb));
    vk_usbh_hcd_iso_packet_t *iso_packet = &urb->urb_hcd->iso_packet;
    vk_usbh_hcd_iso_packet_descriptor_t *frame_desc = iso_packet->frame_desc;
    int_fast32_t size = urb->urb_hcd->transfer_length, cur_offset = 0;
    uint_fast16_t ep_size = urb->urb_hcd->pipe.size, cur_size;

    VSF_USB_ASSERT(size <= ep_size * dimof(iso_packet->frame_desc));
    iso_packet->start_frame = start_frame;
    iso_packet->number_of_packets = 0;
    while (size > 0) {
        iso_packet->number_of_packets++;
        cur_size = min(ep_size, size);
        frame_desc->length = cur_size;
        frame_desc->offset = cur_offset;
        size -= cur_size;
        cur_offset += cur_size;
    }
    return vk_usbh_submit_urb(usbh, urb);
}
#endif

static vk_usbh_urb_t * __vk_usbh_control_msg_common(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
            struct usb_ctrlrequest_t *req)
{
    VSF_USB_ASSERT((usbh != NULL) && (dev != NULL) && (req != NULL));
    vk_usbh_urb_t *urb = &dev->ep0.urb;
    vk_usbh_hcd_urb_t *urb_hcd = urb->urb_hcd;

    if (!dev->is_ep0_rdy) {
        urb->pipe = vk_usbh_get_pipe(dev, 0, USB_ENDPOINT_XFER_CONTROL, 64);
        vsf_err_t err = vk_usbh_alloc_urb(usbh, dev, urb);
        if (err != VSF_ERR_NONE) { return NULL; }
        urb_hcd = dev->ep0.urb.urb_hcd;
        urb_hcd->dev_hcd = &dev->use_as__vk_usbh_hcd_dev_t;
        dev->is_ep0_rdy = true;
    }

    urb_hcd->transfer_length = req->wLength;
    if (    !urb_hcd->buffer && (req->wLength > 0)
        &&  !vk_usbh_urb_alloc_buffer(urb, req->wLength)) {
        return NULL;
    }

    urb_hcd->pipe.dir_in1out0 = (req->bRequestType & USB_DIR_IN) > 0;
    urb_hcd->timeout = 200;

    urb_hcd->setup_packet = *req;
    return urb;
}

vsf_err_t vk_usbh_control_msg(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
            struct usb_ctrlrequest_t *req)
{
    vk_usbh_urb_t * urb = __vk_usbh_control_msg_common(usbh, dev, req);
    if (urb != NULL) {
        return vk_usbh_submit_urb(usbh, urb);
    }
    return VSF_ERR_FAIL;
}

vsf_err_t vk_usbh_control_msg_ex(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
        struct usb_ctrlrequest_t *req, uint_fast16_t flags, vsf_eda_t *eda)
{
    vk_usbh_urb_t * urb = __vk_usbh_control_msg_common(usbh, dev, req);
    if (urb != NULL) {
        return vk_usbh_submit_urb_ex(usbh, urb, flags, eda);
    }
    return VSF_ERR_FAIL;
}

vsf_err_t vk_usbh_set_address(vk_usbh_t *usbh, vk_usbh_dev_t *dev)
{
    VSF_USB_ASSERT((usbh != NULL) && (dev != NULL));
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_RECIP_DEVICE | USB_DIR_OUT,
        .bRequest        =  USB_REQ_SET_ADDRESS,
        .wValue          =  dev->devnum,
        .wIndex          =  0,
        .wLength         =  0,
    };
    return vk_usbh_control_msg(usbh, dev, &req);
}
vsf_err_t vk_usbh_get_descriptor(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
            uint_fast8_t type, uint_fast8_t index, uint_fast16_t size)
{
    VSF_USB_ASSERT((usbh != NULL) && (dev != NULL));
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_RECIP_DEVICE | USB_DIR_IN,
        .bRequest        =  USB_REQ_GET_DESCRIPTOR,
        .wValue          =  (type << 8) + index,
        .wIndex          =  index,
        .wLength         =  size,
    };
    return vk_usbh_control_msg(usbh, dev, &req);
}
vsf_err_t vk_usbh_get_class_descriptor(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
            uint_fast16_t ifs_no, uint_fast8_t type, uint_fast8_t id,
            uint_fast16_t size)
{
    VSF_USB_ASSERT((usbh != NULL) && (dev != NULL));
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_RECIP_INTERFACE | USB_DIR_IN,
        .bRequest        =  USB_REQ_GET_DESCRIPTOR,
        .wValue          =  (type << 8) + id,
        .wIndex          =  ifs_no,
        .wLength         =  size,
    };
    return vk_usbh_control_msg(usbh, dev, &req);
}
vsf_err_t vk_usbh_set_configuration(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
            uint_fast8_t configuration)
{
    VSF_USB_ASSERT((usbh != NULL) && (dev != NULL));
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_RECIP_DEVICE | USB_DIR_OUT,
        .bRequest        =  USB_REQ_SET_CONFIGURATION,
        .wValue          =  configuration,
        .wIndex          =  0,
        .wLength         =  0,
    };
    return vk_usbh_control_msg(usbh, dev, &req);
}
vsf_err_t vk_usbh_set_interface(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
            uint_fast8_t ifs, uint_fast8_t alternate)
{
    VSF_USB_ASSERT((usbh != NULL) && (dev != NULL));
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_RECIP_INTERFACE | USB_DIR_OUT,
        .bRequest        =  USB_REQ_SET_INTERFACE,
        .wValue          =  alternate,
        .wIndex          =  ifs,
        .wLength         =  0,
    };
    return vk_usbh_control_msg(usbh, dev, &req);
}

static bool __vk_usbh_match_id(vk_usbh_dev_parser_t *parser,
        vk_usbh_ifs_parser_t *parser_ifs, const vk_usbh_dev_id_t *dev_id)
{
    vk_usbh_ifs_t *ifs = parser_ifs->ifs;
    struct usb_interface_desc_t *desc_ifs =
                    parser_ifs->parser_alt[ifs->cur_alt].desc_ifs;

    if (    (dev_id->match_vendor && (dev_id->idVendor != parser->desc_device->idVendor))
        ||  (dev_id->match_product && (dev_id->idProduct != parser->desc_device->idProduct))
        ||  (dev_id->match_dev_lo && (dev_id->bcdDevice_lo > parser->desc_device->bcdDevice))
        ||  (dev_id->match_dev_hi && (dev_id->bcdDevice_hi < parser->desc_device->bcdDevice))
        ||  (dev_id->match_dev_class && (dev_id->bDeviceClass != parser->desc_device->bDeviceClass))
        ||  (dev_id->match_dev_subclass && (dev_id->bDeviceSubClass!= parser->desc_device->bDeviceSubClass))
        ||  (dev_id->match_dev_protocol && (dev_id->bDeviceProtocol != parser->desc_device->bDeviceProtocol))
        ||  (dev_id->match_ifs_class && (dev_id->bInterfaceClass != desc_ifs->bInterfaceClass))
        ||  (dev_id->match_ifs_subclass && (dev_id->bInterfaceSubClass != desc_ifs->bInterfaceSubClass))
        ||  (dev_id->match_ifs_protocol && (dev_id->bInterfaceProtocol != desc_ifs->bInterfaceProtocol))) {
        return false;
    }
    return true;
}

static const vk_usbh_class_t *
__vk_usbh_match_interface_driver(vk_usbh_t *usbh, vk_usbh_dev_parser_t *parser,
        vk_usbh_ifs_parser_t *parser_ifs, const vk_usbh_class_t *c)
{
    vk_usbh_ifs_t *ifs = parser_ifs->ifs;
    const vk_usbh_class_drv_t *drv;
    const vk_usbh_dev_id_t *dev_id;

    if (NULL == c) {
        vsf_slist_peek_next(vk_usbh_class_t, node, &usbh->class_list, c);
    } else {
        vsf_slist_peek_next(vk_usbh_class_t, node, &c->node, c);
    }

    while (c != NULL) {
        drv = c->drv;

        dev_id = drv->dev_ids;
        for (uint_fast8_t id_idx = 0; id_idx < drv->dev_id_num; id_idx++, dev_id++) {
            for (uint_fast8_t i = 0; i < ifs->num_of_alt; i++) {
                ifs->cur_alt = i;
                if (__vk_usbh_match_id(parser, parser_ifs, dev_id)) {
                    goto end;
                }
            }
        }
        vsf_slist_peek_next(vk_usbh_class_t, node, &c->node, c);
    }
end:
    return c;
}

static vsf_err_t __vk_usbh_find_intrface_driver(vk_usbh_t *usbh,
        vk_usbh_dev_parser_t *parser, vk_usbh_ifs_parser_t *parser_ifs)
{
    vk_usbh_ifs_t *ifs = parser_ifs->ifs;
    const vk_usbh_class_t *c = NULL;
    void *param;

    do {
        c = __vk_usbh_match_interface_driver(usbh, parser, parser_ifs, c);
        if (c != NULL) {
            const vk_usbh_class_drv_t *drv = c->drv;
            param = drv->probe(usbh, usbh->dev_new, parser_ifs);
            if (param != NULL) {
                ifs->param = param;
                ifs->drv = drv;
                if (VSF_ERR_NONE != vsf_usbh_on_match_interface(parser, parser_ifs)) {
                    drv->disconnect(usbh, usbh->dev_new, param);
                    continue;
                }
                return VSF_ERR_NONE;
            }
        }
    } while (c != NULL);
    return VSF_ERR_FAIL;
}

static vsf_err_t __vk_usbh_parse_config(vk_usbh_t *usbh, vk_usbh_dev_parser_t *parser)
{
    vk_usbh_dev_t *dev = usbh->dev_new;
    struct usb_config_desc_t *desc_config = parser->desc_config;
    struct usb_interface_desc_t *desc_ifs;
    struct usb_descriptor_header_t *desc_header =
                (struct usb_descriptor_header_t *)desc_config, *header_tmp;
    uint_fast16_t size = desc_config->wTotalLength, len, tmpsize;
    vk_usbh_ifs_parser_t *parser_ifs;
    vk_usbh_ifs_alt_parser_t *parser_alt;
    uint_fast8_t ifs_no;

    enum {
        STAGE_NONE = 0,
        STAGE_ALLOC_ALT,
        STAGE_PROBE_ALT,
    } stage;

    if (desc_header->bDescriptorType != USB_DT_CONFIG) {
        return VSF_ERR_NONE;
    }

    parser->num_of_ifs = dev->num_of_ifs = desc_config->bNumInterfaces;

    len = dev->num_of_ifs * sizeof(vk_usbh_ifs_t);
    dev->ifs = vsf_usbh_malloc(len);
    if (!dev->ifs) {
        VSF_USB_ASSERT(false);
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }
    memset(dev->ifs, 0, len);

    len = dev->num_of_ifs * sizeof(vk_usbh_ifs_parser_t);
    parser->parser_ifs = vsf_usbh_malloc(len);
    if (!parser->parser_ifs) {
        VSF_USB_ASSERT(false);
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }
    memset(parser->parser_ifs, 0, len);
    parser_ifs = parser->parser_ifs;
    parser_ifs->ifs = dev->ifs;

    size -= desc_header->bLength;
    desc_header = (struct usb_descriptor_header_t *)((uint8_t *)desc_header + desc_header->bLength);

    parser_alt = NULL;
    stage = STAGE_ALLOC_ALT;
    ifs_no = 0;
    tmpsize = size;
    header_tmp = desc_header;
    while ((size > 0) && (size >= desc_header->bLength)) {
        switch (desc_header->bDescriptorType) {
        case USB_DT_INTERFACE:
            desc_ifs = (struct usb_interface_desc_t *)desc_header;
            if (!parser_ifs->parser_alt) {
                if (desc_ifs->bInterfaceNumber == ifs_no) {
                    parser_ifs->ifs->num_of_alt++;
                } else {
                alloc_alt:
                    parser_ifs->ifs->no = ifs_no;
                    len = parser_ifs->ifs->num_of_alt * sizeof(*parser_ifs->parser_alt);
                    parser_ifs->parser_alt = vsf_usbh_malloc(len);
                    if (!parser_ifs->parser_alt) {
                        VSF_USB_ASSERT(false);
                        return VSF_ERR_NOT_ENOUGH_RESOURCES;
                    }
                    memset(parser_ifs->parser_alt, 0, len);

                    stage = STAGE_PROBE_ALT;
                    parser_alt = parser_ifs->parser_alt - 1;
                    size = tmpsize;
                    desc_header = header_tmp;
                    continue;
                }
            } else {
                if (desc_ifs->bInterfaceNumber == ifs_no) {
                    if (parser_alt > parser_ifs->parser_alt) {
                        parser_alt[-1].desc_size = (uint32_t)desc_ifs - (uint32_t)parser_alt[-1].desc_ifs;
                    }
                    (++parser_alt)->desc_ifs = desc_ifs;
                } else {
                probe_alt:
                    parser_alt->desc_size = (uint32_t)desc_ifs - (uint32_t)parser_alt->desc_ifs;
                    parser_alt = NULL;
                    stage = size > 0 ? STAGE_ALLOC_ALT : STAGE_NONE;
                    ifs_no++;
                    if (ifs_no < dev->num_of_ifs) {
                        parser_ifs[1].ifs = &parser_ifs[0].ifs[1];
                        parser_ifs++;
                    }
                    tmpsize = size;
                    header_tmp = desc_header;
                    continue;
                }
            }
            break;
        case USB_DT_ENDPOINT:
            if (parser_alt) {
                if (!parser_alt->desc_ep) {
                    parser_alt->desc_ep = (struct usb_endpoint_desc_t *)desc_header;
                    parser_alt->num_of_ep = 0;
                }
                parser_alt->num_of_ep++;
            }
            break;
        }

        size -= desc_header->bLength;
        desc_header = (struct usb_descriptor_header_t *)((uint8_t *)desc_header + desc_header->bLength);
        if (!size && stage) {
            if (stage == STAGE_ALLOC_ALT) {
                goto alloc_alt;
            } else /*if (stage == STAGE_PROBE_ALT)*/ {
                desc_ifs = (struct usb_interface_desc_t *)desc_header;
                goto probe_alt;
            }
        }
    }

    vsf_usbh_on_dev_parsed(usbh->dev_new, parser);

    // probe
    size = 0;
    parser_ifs = parser->parser_ifs;
    for (ifs_no = 0; ifs_no < dev->num_of_ifs; ifs_no++, parser_ifs++) {
        if (!__vk_usbh_find_intrface_driver(usbh, parser, parser_ifs)) {
            size++;
        }
    }
    return size > 0 ? VSF_ERR_NONE : VSF_ERR_FAIL;
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_warning=pe111
#endif

static void __vk_usbh_probe_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbh_t *usbh = container_of(eda, vk_usbh_t, teda);
    vk_usbh_dev_parser_t *parser = NULL;
    vk_usbh_dev_t *dev;
    vk_usbh_urb_t *urb;
    uint8_t *buffer;
    vsf_err_t err = VSF_ERR_NONE;

    if (evt != VSF_EVT_INIT) {
        parser = usbh->parser;
    }
    dev = usbh->dev_new;
    urb = &dev->ep0.urb;

    switch (evt) {
    case VSF_EVT_INIT:
        parser = vsf_usbh_malloc(sizeof(*parser));
        if (NULL == parser) { goto parse_failed; }
        memset(parser, 0, sizeof(*parser));
        usbh->parser = parser;

        __vsf_eda_crit_npb_init(&dev->ep0.crit);
        parser->devnum_temp = dev->devnum;
        parser->probe_state = VSF_USBH_PROBE_START;
        dev->devnum = 0;

        // get 64 bytes device descriptor
        err = vk_usbh_get_descriptor(usbh, dev, USB_DT_DEVICE, 0, 64);
        break;
    case VSF_EVT_MESSAGE:
        if (vk_usbh_urb_get_status(urb) != URB_OK) { goto parse_failed; }

        switch (parser->probe_state) {
        case VSF_USBH_PROBE_WAIT_DEVICE_DESC:
            buffer = vk_usbh_urb_take_buffer(urb);
            urb->urb_hcd->pipe.size = buffer[7];
            __vk_usbh_free_buffer(buffer);
            vk_usbh_reset_dev(usbh, dev);
            goto check_device_reset;
        case VSF_USBH_PROBE_WAIT_SET_ADDRESS:
            vsf_teda_set_timer_ms(10);
            break;
        case VSF_USBH_PROBE_WAIT_FULL_DEVICE_DESC:
            parser->desc_device = vk_usbh_urb_take_buffer(urb);
            if (parser->desc_device->bNumConfigurations < 1) {
                goto parse_failed;
            }
            dev->cur_config = 0;

        parse_next_config:
            parser->probe_state = VSF_USBH_PROBE_WAIT_FULL_DEVICE_DESC;
            if (dev->cur_config >= parser->desc_device->bNumConfigurations) {
                goto parse_failed;
            }

            // get 9 bytes configuration
            err = vk_usbh_get_descriptor(usbh, dev, USB_DT_CONFIG, dev->cur_config, 9);
            break;
        case VSF_USBH_PROBE_WAIT_CONFIG_DESC_SIZE:
            parser->desc_config = vk_usbh_urb_take_buffer(urb);
            // get full configuation
            err = vk_usbh_get_descriptor(usbh, dev, USB_DT_CONFIG,
                    dev->cur_config, parser->desc_config->wTotalLength);
            break;
        case VSF_USBH_PROBE_WAIT_FULL_CONFIG_DESC:
            // check wTotalLength
            if (vk_usbh_urb_get_actual_length(urb) != parser->desc_config->wTotalLength) {
                goto parse_failed;
            }

            __vk_usbh_free_buffer(parser->desc_config);
            parser->desc_config = vk_usbh_urb_take_buffer(urb);
            err = vk_usbh_set_configuration(usbh, dev, parser->desc_config->bConfigurationValue);
            break;
        case VSF_USBH_PROBE_WAIT_SET_CONFIG:
            if (__vk_usbh_parse_config(usbh, parser) < 0) {
                __vk_usbh_reset_parser(parser);
                dev->cur_config++;
                goto parse_next_config;
            }

            goto parse_ok;
        }
        break;
    case VSF_EVT_TIMER:
        switch (parser->probe_state) {
        case VSF_USBH_PROBE_WAIT_DEVICE_RESET:
        check_device_reset:
            if (__vk_usbh_is_dev_resetting(usbh, dev)) {
                vsf_teda_set_timer_ms(20);
                return;
            } else {
                // set address
                dev->devnum = parser->devnum_temp;
                err = vk_usbh_set_address(usbh, dev);
            }
            break;
        case VSF_USBH_PROBE_WAIT_ADDRESS_STABLE:
            // update address
            urb->urb_hcd->pipe.address = dev->devnum;
            // get full device descriptor
            err = vk_usbh_get_descriptor(usbh, dev, USB_DT_DEVICE, 0,
                    sizeof(*parser->desc_device));
            break;
        }
    }
    if (err < 0) {
        goto parse_failed;
    }
    parser->probe_state++;
    return;

parse_failed:
    if (dev != NULL) {
        __vk_usbh_clean_device(usbh, dev);
    }
parse_ok:
    vk_usbh_urb_free_buffer(urb);
    __vk_usbh_free_parser(usbh);
    usbh->dev_new = NULL;
}

static void __vk_usbh_init_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbh_t *usbh = container_of(eda, vk_usbh_t, teda);
    vsf_err_t err = usbh->drv->init_evthandler(eda, evt, &usbh->use_as__vk_usbh_hcd_t);

    VSF_USB_ASSERT(err >= 0);
    if (VSF_ERR_NONE == err) {
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
        // init_evthandler should not use frame
        VSF_USB_ASSERT(!eda->state.bits.is_use_frame);
#endif
        eda->fn.evthandler = __vk_usbh_probe_evthandler;

#if VSF_USBH_CFG_ENABLE_ROOT_HUB == ENABLED
        if (usbh->drv->rh_control != NULL) {
            usbh->dev_rh = vk_usbh_new_device(usbh, usbh->rh_speed, NULL, 0);
            VSF_USB_ASSERT(usbh->dev_rh != NULL);
        }
#endif
    }
}

vsf_err_t vk_usbh_init(vk_usbh_t *usbh)
{
    VSF_USB_ASSERT((usbh != NULL) && (usbh->drv != NULL));

    vsf_slist_init(&usbh->class_list);
    vsf_bitmap_reset(&usbh->device_bitmap, VSF_USBH_CFG_MAX_DEVICE);
    vsf_bitmap_set(&usbh->device_bitmap, 0);

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    usbh->teda.on_terminate = NULL;
#endif
    usbh->teda.fn.evthandler = __vk_usbh_init_evthandler;
    return vsf_teda_init(&usbh->teda, VSF_USBH_CFG_EDA_PRIORITY, false);
}

vsf_err_t vk_usbh_fini(vk_usbh_t *usbh)
{
    // TODO
    return VSF_ERR_NONE;
}

void vk_usbh_register_class(vk_usbh_t *usbh, vk_usbh_class_t *c)
{
    VSF_USB_ASSERT((usbh != NULL) && (c != NULL));
    vsf_slist_add_to_head(vk_usbh_class_t, node, &usbh->class_list, c);
}

vsf_err_t vk_usbh_get_extra_descriptor(uint8_t *buf, uint_fast16_t size,
        uint_fast8_t type, void **ptr)
{
    struct usb_descriptor_header_t *header;

    VSF_USB_ASSERT((buf != NULL) && (ptr != NULL));

    while (size >= sizeof(struct usb_descriptor_header_t)) {
        header = (struct usb_descriptor_header_t *)buf;
        if (header->bLength < 2) {
            break;
        }

        if (header->bDescriptorType == type) {
            *ptr = header;
            return VSF_ERR_NONE;
        }

        if (size < header->bLength) {
            break;
        }

        buf += header->bLength;
        size -= header->bLength;
    }
    return VSF_ERR_FAIL;
}

usb_endpoint_desc_t * vk_usbh_get_next_ep_descriptor(usb_endpoint_desc_t *desc_ep, uint_fast16_t size)
{
    if (size > desc_ep->bLength) {
        desc_ep = (struct usb_endpoint_desc_t *)((uintptr_t)desc_ep + desc_ep->bLength);
        size -= desc_ep->bLength;

        if (USB_DT_SS_ENDPOINT_COMP == desc_ep->bDescriptorType) {
            if (size > desc_ep->bLength) {
                desc_ep = (struct usb_endpoint_desc_t *)((uintptr_t)desc_ep + desc_ep->bLength);
            } else {
                desc_ep = NULL;
            }
        }
    } else {
        desc_ep = NULL;
    }
    return desc_ep;
}

#endif
