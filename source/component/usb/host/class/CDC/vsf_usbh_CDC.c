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

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_CDC == ENABLED

#define __VSF_USBH_CLASS_IMPLEMENT_CLASS__
#define __VSF_USBH_CDC_CLASS_IMPLEMENT

#include "./vsf_usbh_CDC.h"
#include "kernel/vsf_kernel.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void vk_usbh_cdc_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbh_cdc_t *pthis = container_of(eda, vk_usbh_cdc_t, eda);
    vk_usbh_t *usbh = pthis->usbh;
    vk_usbh_urb_t *urb;

    switch (evt) {
    case VSF_EVT_INIT:
        urb = &pthis->urb_evt;
        if (vk_usbh_urb_is_alloced(urb)) {
            vk_usbh_urb_set_buffer(urb, pthis->evt_buffer, pthis->evt_size);
            if (VSF_ERR_NONE != vk_usbh_submit_urb(usbh, urb)) {
                goto failed;
            }
        }
        break;
    case VSF_EVT_MESSAGE: {
            vk_usbh_urb_t urb = { .urb_hcd = vsf_eda_get_cur_msg() };
            vk_usbh_pipe_t pipe = vk_usbh_urb_get_pipe(&urb);

            if (USB_ENDPOINT_XFER_INT == pipe.type) {
                if (vk_usbh_urb_get_status(&urb) != URB_OK) {
                    goto failed;
                } else if (pthis->evthandler != NULL) {
                    pthis->evthandler(pthis, VSF_USBH_CDC_ON_EVENT, NULL);
                    if (VSF_ERR_NONE != vk_usbh_submit_urb(usbh, &urb)) {
                        goto failed;
                    }
                }
            } else /* if (USB_ENDPOINT_XFER_BULK == pipe.type) */ {
                if (pipe.dir_in1out0) {
                    if (pthis->evthandler != NULL) {
                        pthis->evthandler(pthis, VSF_USBH_CDC_ON_RX, (void *)&urb);
                    }
                } else {
                    if (pthis->evthandler != NULL) {
                        pthis->evthandler(pthis, VSF_USBH_CDC_ON_TX, (void *)&urb);
                    }
                }
            }
        }
        break;
    }

    return;
failed:
    vk_usbh_remove_interface(usbh, pthis->dev, pthis->ifs);
}

static void __vk_usbh_cdc_parse_ep(vk_usbh_cdc_t *pthis, struct usb_endpoint_desc_t *desc_ep)
{
    uint_fast8_t epaddr = desc_ep->bEndpointAddress;
    uint_fast8_t eptype = desc_ep->bmAttributes;

    if ((USB_ENDPOINT_XFER_INT == eptype) && (epaddr & USB_DIR_MASK)) {
        vk_usbh_urb_prepare(&pthis->urb_evt, pthis->dev, desc_ep);
    } else if (USB_ENDPOINT_XFER_BULK == eptype) {
        if (epaddr & USB_DIR_MASK) {
            pthis->pipe_rx = vk_usbh_get_pipe_from_ep_desc(pthis->dev, desc_ep);
        } else {
            pthis->pipe_tx = vk_usbh_get_pipe_from_ep_desc(pthis->dev, desc_ep);
        }
    }
}

vsf_err_t vk_usbh_cdc_init(vk_usbh_cdc_t *pthis, vk_usbh_t *usbh,
        vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs)
{
    vk_usbh_ifs_alt_parser_t *parser_alt = &parser_ifs->parser_alt[parser_ifs->ifs->cur_alt];
    struct usb_interface_desc_t *desc_ifs = parser_alt->desc_ifs;
    struct usb_endpoint_desc_t *desc_ep = parser_alt->desc_ep;
    struct usb_class_interface_desc_t *desc;

    pthis->usbh = usbh;
    pthis->dev = dev;
    pthis->ifs = parser_ifs->ifs;
    pthis->data_ifs = -1;

    // parse control interface
    pthis->ctrl_ifs = desc_ifs->bInterfaceNumber;
    for (uint_fast8_t i = 0; i < desc_ifs->bNumEndpoints; i++) {
        if (desc_ep->bLength != USB_DT_ENDPOINT_SIZE) {
            return VSF_ERR_FAIL;
        }
        __vk_usbh_cdc_parse_ep(pthis, desc_ep);
        desc_ep = (struct usb_endpoint_desc_t *)((uintptr_t)desc_ep + USB_DT_ENDPOINT_SIZE);
    }

    desc = (struct usb_class_interface_desc_t *)desc_ifs;
    for (uint_fast8_t parsed_size = 0; parsed_size < parser_alt->desc_size;) {
        if (desc->bDescriptorType == USB_DT_CS_INTERFACE) {
            switch (desc->bDescriptorSubType) {
            case 0x06: {      // Union Functional Descriptor
                    struct usb_cdc_union_descriptor_t *union_desc =
                        (struct usb_cdc_union_descriptor_t *)desc;
                    if (union_desc->bControlInterface != pthis->ctrl_ifs) {
                        // something wrong
                        return VSF_ERR_FAIL;
                    }
                    pthis->data_ifs = union_desc->bSubordinateInterface[0];
                }
                break;
            default:
                if (    (pthis->evthandler != NULL)
                    &&  (VSF_ERR_NONE != pthis->evthandler(pthis, VSF_USBH_CDC_ON_DESC, desc))) {
                    return VSF_ERR_FAIL;
                }
                break;
            }
        }
        parsed_size += desc->bLength;
        desc = (struct usb_class_interface_desc_t *)((uintptr_t)desc + desc->bLength);
    }

    // parse data interface
    if (pthis->data_ifs >= 0) {
        if (pthis->data_ifs >= usbh->parser->num_of_ifs) {
            return VSF_ERR_FAIL;
        }

        parser_ifs = &usbh->parser->parser_ifs[pthis->data_ifs];
        parser_alt = parser_ifs->parser_alt;

        for (uint_fast8_t i = 0; i < parser_ifs->ifs->num_of_alt; i++, parser_alt++) {
            desc_ifs = parser_alt->desc_ifs;
            desc_ep = parser_alt->desc_ep;

            if (    (desc_ifs->bDescriptorType == USB_DT_INTERFACE)
                &&  (desc_ifs->bInterfaceClass == USB_CLASS_CDC_DATA)
                &&  (desc_ifs->bInterfaceSubClass == 0)
                &&  (desc_ifs->bInterfaceProtocol == 0)) {

                if (desc_ifs->bNumEndpoints == 0) {
                    continue;
                }

                for (uint_fast8_t j = 0; j < desc_ifs->bNumEndpoints; j++) {
                    if (    (NULL == desc_ep)
                        ||  (desc_ep->bLength != USB_DT_ENDPOINT_SIZE)
                        ||  (desc_ep->bmAttributes != USB_ENDPOINT_XFER_BULK)) {
                        return VSF_ERR_FAIL;
                    }
                    __vk_usbh_cdc_parse_ep(pthis, desc_ep);
                    desc_ep = vk_usbh_get_next_ep_descriptor(desc_ep,
                        parser_alt->desc_size - ((uintptr_t)desc_ep - (uintptr_t)desc_ifs));
                }
            }
            break;
        }
    }

    if (pthis->evthandler != NULL) {
        pthis->evthandler(pthis, VSF_USBH_CDC_ON_INIT, NULL);
    }

    if (    (   vk_usbh_urb_is_valid(&pthis->urb_evt)
            &&  (pthis->evt_size > 0)
            &&  (pthis->evt_buffer != NULL)
            &&  (VSF_ERR_NONE != vk_usbh_alloc_urb(usbh, dev, &pthis->urb_evt)))) {
        vk_usbh_free_urb(usbh, &pthis->urb_evt);
        return VSF_ERR_FAIL;
    }

    return VSF_ERR_NONE;
}

void vk_usbh_cdc_fini(vk_usbh_cdc_t *pthis)
{
    vk_usbh_free_urb(pthis->usbh, &pthis->urb_evt);
}

vsf_err_t vk_usbh_cdc_prepare_urb(vk_usbh_cdc_t *pthis, bool tx, vk_usbh_urb_t *urb)
{
    vk_usbh_t *usbh = pthis->usbh;
    vk_usbh_dev_t *dev = pthis->dev;

    if (tx) {
        vk_usbh_urb_prepare_by_pipe(urb, dev, pthis->pipe_tx);
    } else {
        vk_usbh_urb_prepare_by_pipe(urb, dev, pthis->pipe_rx);
    }

    return vk_usbh_alloc_urb(usbh, dev, urb);
}

vsf_err_t vk_usbh_cdc_submit_urb(vk_usbh_cdc_t *pthis, vk_usbh_urb_t *urb)
{
    return vk_usbh_submit_urb_ex(pthis->usbh, urb, 0, &pthis->eda);
}

void vk_usbh_cdc_free_urb(vk_usbh_cdc_t *pthis, vk_usbh_urb_t *urb)
{
    vk_usbh_free_urb(pthis->usbh, urb);
}

#endif
