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

#if VSF_USE_USB_HOST == ENABLED && VSF_USE_USB_HOST_HID == ENABLED

#define VSF_USBH_IMPLEMENT_CLASS
#define VSF_USBH_HID_IMPLEMENT
#include "vsf.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE != ENABLED
#   error "VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE is required"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct usb_hid_class_descriptor_t
{
    uint8_t bDescriptorType;
    uint16_t wDescriptorLength;
} PACKED;
typedef struct usb_hid_class_descriptor_t usb_hid_class_descriptor_t;

struct usb_hid_descriptor_t
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdHID;
    uint8_t bCountryCode;
    uint8_t bNumDescriptors;

    usb_hid_class_descriptor_t desc[1];
} PACKED;
typedef struct usb_hid_descriptor_t usb_hid_descriptor_t;

struct vsf_usbh_hid_input_t {
    implement(vsf_usbh_hid_eda_t)
#if VSF_INPUT_CFG_HID_EN == ENABLED
    implement(vsf_input_hid_t)
#endif
};
typedef struct vsf_usbh_hid_input_t vsf_usbh_hid_input_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

SECTION(".text.vsf.kernel.eda")
vsf_err_t __vsf_eda_fini(vsf_eda_t *pthis);

/*============================ IMPLEMENTATION ================================*/

static void vsf_usbh_hid_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vsf_usbh_hid_eda_t *hid = container_of(eda, vsf_usbh_hid_eda_t, use_as__vsf_eda_t);
    vsf_usbh_dev_t *dev = hid->dev;

    switch (evt) {
    case VSF_EVT_INIT:
        if (VSF_ERR_NONE != vsf_eda_crit_enter(&dev->ep0.crit, -1)) {
            break;
        }
        // fall through
    case VSF_EVT_SYNC:
        if (VSF_ERR_NONE != vsf_usbh_get_class_descriptor(hid->usbh, hid->dev, hid->ifs->no, USB_DT_REPORT, 0, hid->desc_len)) {
            vsf_usbh_remove_interface(hid->usbh, dev, hid->ifs);
            return;
        }
        break;
    case VSF_EVT_MESSAGE:
        hid->evthandler = hid->user_evthandler;
        hid->ep0 = &dev->ep0;
        vsf_eda_post_evt(eda, VSF_EVT_INIT);
        break;
    }
}

static void vsf_usbh_hid_on_eda_terminate(vsf_eda_t *eda)
{
    vsf_usbh_hid_eda_t *hid = container_of(eda, vsf_usbh_hid_eda_t, use_as__vsf_eda_t);
    VSF_USBH_FREE(hid);
}

void * vsf_usbh_hid_probe(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev,
            vsf_usbh_ifs_parser_t *parser_ifs, uint_fast32_t obj_size)
{
    vsf_usbh_ifs_t *ifs = parser_ifs->ifs;
    vsf_usbh_ifs_alt_parser_t *parser_alt = &parser_ifs->parser_alt[ifs->cur_alt];
    struct usb_interface_desc_t *desc_ifs = parser_alt->desc_ifs;
    struct usb_endpoint_desc_t *desc_ep = parser_alt->desc_ep;
    usb_hid_descriptor_t *desc_hid;
    vsf_usbh_hid_eda_t *hid;
    uint_fast8_t epaddr;

    if (VSF_ERR_NONE != vsf_usbh_get_extra_descriptor((uint8_t *)desc_ifs,
            parser_alt->desc_size, USB_DT_HID, (void **)&desc_hid)) {
        return NULL;
    }

    hid = VSF_USBH_MALLOC(obj_size);
    if (NULL == hid) {
        return NULL;
    }
    memset(hid, 0, obj_size);

    for (int i = 0; i < desc_ifs->bNumEndpoints; i++) {
        if (desc_ep->bLength != USB_DT_ENDPOINT_SIZE) {
            goto free_all;
        }
        epaddr = desc_ep->bEndpointAddress;
        switch (desc_ep->bmAttributes)
        {
        case USB_ENDPOINT_XFER_INT:
            if (epaddr & USB_ENDPOINT_DIR_MASK) {
                vsf_usbh_urb_prepare(&hid->urb_in, dev, desc_ep);
            } else {
                vsf_usbh_urb_prepare(&hid->urb_out, dev, desc_ep);
            }
            break;
        }
        desc_ep = (struct usb_endpoint_desc_t *)((uint32_t)desc_ep + USB_DT_ENDPOINT_SIZE);
    }

    hid->usbh = usbh;
    hid->dev = dev;
    hid->ifs = ifs;
    hid->desc_len = desc_hid->desc[0].wDescriptorLength;

    hid->evthandler = vsf_usbh_hid_evthandler;
    hid->on_terminate = vsf_usbh_hid_on_eda_terminate;
    vsf_eda_init(&hid->use_as__vsf_eda_t, vsf_priority_inherit, false);

    return hid;
free_all:
    VSF_USBH_FREE(hid);
    return NULL;
}

void vsf_usbh_hid_disconnect(vsf_usbh_hid_eda_t *hid)
{
    vsf_usbh_t *usbh = hid->usbh;
    vsf_usbh_free_urb(usbh, &hid->urb_in);
    vsf_usbh_free_urb(usbh, &hid->urb_out);
    __vsf_eda_fini(&hid->use_as__vsf_eda_t);
}

static vsf_err_t vsf_usbh_hid_submit_urb(vsf_usbh_hid_eda_t *hid, uint8_t *buffer, int_fast32_t size, vsf_usbh_urb_t *urb)
{
    vsf_err_t err;

    if (!vsf_usbh_urb_is_alloced(urb)) {
        err = vsf_usbh_alloc_urb(hid->usbh, hid->dev, urb);
        if (err != VSF_ERR_NONE) { return err; }
    }

    if (size > 0) {
        if (buffer != NULL) {
            vsf_usbh_urb_set_buffer(urb, buffer, size);
        } else {
            vsf_usbh_urb_alloc_buffer(urb, size);
        }
    }

    return vsf_usbh_submit_urb(hid->usbh, urb);
}

vsf_err_t __vsf_usbh_hid_send_report_req_imp(vsf_usbh_hid_base_t *hid, uint_fast16_t type_id,
        uint8_t *report, uint_fast16_t report_len)
{
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_OUT,
        .bRequest        =  0x09,
        .wValue          =  type_id,
        .wIndex          =  0,
        .wLength         =  report_len,
    };
    if (report != NULL) {
        vsf_usbh_urb_set_buffer(&hid->ep0->urb, report, report_len);
    }
    return vsf_usbh_control_msg(hid->usbh, hid->dev, &req);
}

vsf_err_t __vsf_usbh_hid_recv_report_req_imp(vsf_usbh_hid_base_t *hid, uint_fast16_t type_id,
        uint8_t *report, uint_fast16_t report_len)
{
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_OUT,
        .bRequest        =  0x01,
        .wValue          =  type_id,
        .wIndex          =  0,
        .wLength         =  report_len,
    };
    if (report != NULL) {
        vsf_usbh_urb_set_buffer(&hid->ep0->urb, report, report_len);
    } else if (report_len > 0) {
        vsf_usbh_urb_alloc_buffer(&hid->ep0->urb, report_len);
    }
    return vsf_usbh_control_msg(hid->usbh, hid->dev, &req);
}

vsf_err_t __vsf_usbh_hid_set_idle_imp(vsf_usbh_hid_base_t *hid, uint_fast8_t id, uint_fast8_t duration)
{
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_OUT,
        .bRequest        =  0x0A,
        .wValue          =  (id << 0) | (duration << 8),
        .wIndex          =  hid->ifs->no,
        .wLength         =  0,
    };
    return vsf_usbh_control_msg(hid->usbh, hid->dev, &req);
}

uint8_t * __vsf_usbh_hid_get_tx_report_imp(vsf_usbh_hid_eda_t *hid)
{
    return vsf_usbh_urb_peek_buffer(&hid->urb_out);
}

uint8_t * __vsf_usbh_hid_get_rx_report_imp(vsf_usbh_hid_eda_t *hid)
{
    return vsf_usbh_urb_peek_buffer(&hid->urb_in);
}

vsf_err_t __vsf_usbh_hid_send_report_imp(vsf_usbh_hid_eda_t *hid, uint8_t *buffer, int_fast32_t size)
{
    return vsf_usbh_hid_submit_urb(hid, buffer, size, &hid->urb_out);
}

vsf_err_t __vsf_usbh_hid_recv_report_imp(vsf_usbh_hid_eda_t *hid, uint8_t *buffer, int_fast32_t size)
{
    return vsf_usbh_hid_submit_urb(hid, buffer, size, &hid->urb_in);
}




// hid input, which uses vsf_hid
WEAK int_fast32_t vsf_usbh_hid_input_on_desc(vsf_usbh_hid_input_t *hid, uint8_t *desc_buf, uint_fast32_t desc_len)
{
#if VSF_INPUT_CFG_HID_EN == ENABLED
    return vsf_hid_parse_desc(&hid->use_as__vsf_input_hid_t, desc_buf, desc_len);
#else
    return -1;
#endif
}

WEAK void vsf_usbh_hid_input_on_report_input(vsf_usbh_hid_input_t *hid, uint8_t *report, uint_fast32_t len)
{
#if VSF_INPUT_CFG_HID_EN == ENABLED
    vsf_hid_process_input(&hid->use_as__vsf_input_hid_t, report, len);
#endif
}

WEAK void vsf_usbh_hid_input_on_new(vsf_usbh_hid_input_t *hid)
{
#if VSF_INPUT_CFG_HID_EN == ENABLED
    vsf_hid_new_dev(&hid->use_as__vsf_input_hid_t);
#endif
}

WEAK void vsf_usbh_hid_input_on_free(vsf_usbh_hid_input_t *hid)
{
#if VSF_INPUT_CFG_HID_EN == ENABLED
    vsf_hid_free_dev(&hid->use_as__vsf_input_hid_t);
#endif
}

static void vsf_usbh_hid_input_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vsf_usbh_hid_input_t *hid = container_of(eda, vsf_usbh_hid_input_t, use_as__vsf_eda_t);

    switch (evt) {
    case VSF_EVT_INIT:
        do {
            vsf_usbh_ep0_t *ep0 = hid->ep0;
            vsf_usbh_urb_t *urb = &ep0->urb;
            uint8_t *desc_buf;
            uint_fast32_t desc_len, max_report_size;

            if (vsf_usbh_urb_get_status(urb) != URB_OK) {
                vsf_eda_crit_leave(&ep0->crit);
                vsf_usbh_remove_interface(hid->usbh, hid->dev, hid->ifs);
                return;
            }

            desc_buf = vsf_usbh_urb_take_buffer(urb);
            desc_len = vsf_usbh_urb_get_actual_length(urb);
            vsf_eda_crit_leave(&ep0->crit);

            max_report_size = vsf_usbh_hid_input_on_desc(hid, desc_buf, desc_len);
            if (max_report_size <= 0) {
                VSF_USBH_FREE(desc_buf);
                vsf_usbh_remove_interface(hid->usbh, hid->dev, hid->ifs);
                return;
            }

            VSF_USBH_FREE(desc_buf);
            vsf_usbh_hid_recv_report(&hid->use_as__vsf_usbh_hid_eda_t, NULL,
                    max_report_size);
        } while (0);
        break;
    case VSF_EVT_MESSAGE:
        do {
            vsf_usbh_urb_t urb = { .urb_hcd = vsf_eda_get_cur_msg() };

            if (URB_OK == vsf_usbh_urb_get_status(&urb)) {
                uint_fast32_t len = vsf_usbh_urb_get_actual_length(&urb);
                uint8_t *report = vsf_usbh_urb_peek_buffer(&urb);
                vsf_usbh_hid_input_on_report_input(hid, report, len);
            }

            vsf_usbh_hid_recv_report(&hid->use_as__vsf_usbh_hid_eda_t, NULL, 0);
        } while (0);
        break;
    }
}

static void * vsf_usbh_hid_input_probe(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev,
            vsf_usbh_ifs_parser_t *parser_ifs)
{
    vsf_usbh_hid_input_t *hid = vsf_usbh_hid_probe(usbh, dev, parser_ifs, sizeof(vsf_usbh_hid_input_t));
    if (hid != NULL) {
        hid->user_evthandler = vsf_usbh_hid_input_evthandler;
        vsf_usbh_hid_input_on_new(hid);
    }
    return hid;
}

static void vsf_usbh_hid_input_disconnect(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev, void *param)
{
    vsf_usbh_hid_input_t *hid = param;
    vsf_usbh_hid_disconnect(&hid->use_as__vsf_usbh_hid_eda_t);
    vsf_usbh_hid_input_on_free(hid);
}

static const vsf_usbh_dev_id_t vsf_usbh_hid_input_dev_id[] = {
    {
        .match_int_class = true,
        .bInterfaceClass = USB_CLASS_HID,
    },
};

const vsf_usbh_class_drv_t vsf_usbh_hid_drv = {
    .name       = "hid",
    .dev_id_num = dimof(vsf_usbh_hid_input_dev_id),
    .dev_ids    = vsf_usbh_hid_input_dev_id,
    .probe      = vsf_usbh_hid_input_probe,
    .disconnect = vsf_usbh_hid_input_disconnect,
};

#endif      // VSF_USE_USB_HOST && VSF_USE_USB_HOST_HID
