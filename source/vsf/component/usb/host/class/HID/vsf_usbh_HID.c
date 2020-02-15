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

#define VSF_EDA_CLASS_INHERIT
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

struct vk_usbh_hid_input_t {
    implement(vk_usbh_hid_eda_t)
#if VSF_USE_INPUT == ENABLED && VSF_USE_INPUT_HID == ENABLED
    implement(vk_input_hid_t)
#endif
};
typedef struct vk_usbh_hid_input_t vk_usbh_hid_input_t;

/*============================ LOCAL VARIABLES ===============================*/

static const vk_usbh_dev_id_t __vk_usbh_hid_input_dev_id[] = {
    {
        .match_ifs_class = true,
        .bInterfaceClass = USB_CLASS_HID,
    },
};
/*============================ PROTOTYPES ====================================*/

static void * __vk_usbh_hid_input_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs);
static void __vk_usbh_hid_input_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbh_class_drv_t vk_usbh_hid_drv = {
    .name       = "hid",
    .dev_id_num = dimof(__vk_usbh_hid_input_dev_id),
    .dev_ids    = __vk_usbh_hid_input_dev_id,
    .probe      = __vk_usbh_hid_input_probe,
    .disconnect = __vk_usbh_hid_input_disconnect,
};

/*============================ PROTOTYPES ====================================*/


#if     defined(WEAK_VSF_USBH_HID_INPUT_ON_DESC_EXTERN)                         \
    &&  defined(WEAK_VSF_USBH_HID_INPUT_ON_DESC)
WEAK_VSF_USBH_HID_INPUT_ON_DESC_EXTERN
#endif

#if     defined(WEAK_VSF_USBH_HID_INPUT_ON_NEW_EXTERN)                          \
    &&  defined(WEAK_VSF_USBH_HID_INPUT_ON_NEW)
WEAK_VSF_USBH_HID_INPUT_ON_NEW_EXTERN
#endif

#if     defined(WEAK_VSF_USBH_HID_INPUT_ON_FREE_EXTERN)                         \
    &&  defined(WEAK_VSF_USBH_HID_INPUT_ON_FREE)
WEAK_VSF_USBH_HID_INPUT_ON_FREE_EXTERN
#endif

#if     defined(WEAK_VSF_USBH_HID_INPUT_ON_REPORT_INPUT_EXTERN)                 \
    &&  defined(WEAK_VSF_USBH_HID_INPUT_ON_REPORT_INPUT)
WEAK_VSF_USBH_HID_INPUT_ON_REPORT_INPUT_EXTERN
#endif

/*============================ IMPLEMENTATION ================================*/

static void __vk_usbh_hid_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbh_hid_eda_t *hid = container_of(eda, vk_usbh_hid_eda_t, use_as__vsf_eda_t);
    vk_usbh_dev_t *dev = hid->dev;

    switch (evt) {
    case VSF_EVT_INIT:
        if (VSF_ERR_NONE != __vsf_eda_crit_npb_enter(&dev->ep0.crit)) {
            break;
        }
        // fall through
    case VSF_EVT_SYNC:
        if (VSF_ERR_NONE != vk_usbh_get_class_descriptor(hid->usbh, hid->dev, hid->ifs->no, USB_DT_REPORT, 0, hid->desc_len)) {
            vk_usbh_remove_interface(hid->usbh, dev, hid->ifs);
            return;
        }
        break;
    case VSF_EVT_MESSAGE:
        // hid->ep0 and hid->user_evthandler share the same memory
        //  so user_evthandler MUST first be used, and then set ep0
        eda->fn.evthandler = hid->user_evthandler;
        hid->ep0 = &dev->ep0;
        vsf_eda_post_evt(eda, VSF_EVT_INIT);
        break;
    }
}

static void __vk_usbh_hid_on_eda_terminate(vsf_eda_t *eda)
{
    vk_usbh_hid_eda_t *hid = container_of(eda, vk_usbh_hid_eda_t, use_as__vsf_eda_t);
    VSF_USBH_FREE(hid);
}

void * vk_usbh_hid_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
            vk_usbh_ifs_parser_t *parser_ifs, uint_fast32_t obj_size, bool has_hid_desc)
{
    vk_usbh_ifs_t *ifs = parser_ifs->ifs;
    vk_usbh_ifs_alt_parser_t *parser_alt = &parser_ifs->parser_alt[ifs->cur_alt];
    struct usb_interface_desc_t *desc_ifs = parser_alt->desc_ifs;
    struct usb_endpoint_desc_t *desc_ep = parser_alt->desc_ep;
    usb_hid_descriptor_t *desc_hid;
    vk_usbh_hid_eda_t *hid;
    uint_fast8_t epaddr;

    if (has_hid_desc) {
        if (VSF_ERR_NONE != vk_usbh_get_extra_descriptor((uint8_t *)desc_ifs,
                parser_alt->desc_size, USB_DT_HID, (void **)&desc_hid)) {
            return NULL;
        }
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
        switch (desc_ep->bmAttributes) {
        case USB_ENDPOINT_XFER_INT:
            if (epaddr & USB_DIR_MASK) {
                vk_usbh_urb_prepare(&hid->urb_in, dev, desc_ep);
            } else {
                vk_usbh_urb_prepare(&hid->urb_out, dev, desc_ep);
            }
            break;
        }
        desc_ep = (struct usb_endpoint_desc_t *)((uintptr_t)desc_ep + USB_DT_ENDPOINT_SIZE);
    }

    hid->usbh = usbh;
    hid->dev = dev;
    hid->ifs = ifs;
    if (has_hid_desc) {
        hid->desc_len = desc_hid->desc[0].wDescriptorLength;
    }

    hid->fn.evthandler = __vk_usbh_hid_evthandler;
    hid->on_terminate = __vk_usbh_hid_on_eda_terminate;
    vsf_eda_init(&hid->use_as__vsf_eda_t, vsf_prio_inherit, false);

    return hid;
free_all:
    VSF_USBH_FREE(hid);
    return NULL;
}

void vk_usbh_hid_disconnect(vk_usbh_hid_eda_t *hid)
{
    vk_usbh_t *usbh = hid->usbh;
    vk_usbh_free_urb(usbh, &hid->urb_in);
    vk_usbh_free_urb(usbh, &hid->urb_out);
    vsf_eda_fini(&hid->use_as__vsf_eda_t);
}

static vsf_err_t vk_usbh_hid_submit_urb(vk_usbh_hid_eda_t *hid, uint8_t *buffer, int_fast32_t size, vk_usbh_urb_t *urb)
{
    vsf_err_t err;

    if (!vk_usbh_urb_is_alloced(urb)) {
        err = vk_usbh_alloc_urb(hid->usbh, hid->dev, urb);
        if (err != VSF_ERR_NONE) { return err; }
    }

    if (size > 0) {
        if (buffer != NULL) {
            vk_usbh_urb_set_buffer(urb, buffer, size);
        } else {
            vk_usbh_urb_alloc_buffer(urb, size);
        }
    }

    return vk_usbh_submit_urb(hid->usbh, urb);
}

vsf_err_t __vk_usbh_hid_send_report_req_imp(vk_usbh_hid_base_t *hid, uint_fast16_t type_id,
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
        vk_usbh_urb_set_buffer(&hid->ep0->urb, report, report_len);
    }
    return vk_usbh_control_msg(hid->usbh, hid->dev, &req);
}

vsf_err_t __vk_usbh_hid_recv_report_req_imp(vk_usbh_hid_base_t *hid, uint_fast16_t type_id,
        uint8_t *report, uint_fast16_t report_len)
{
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_IN,
        .bRequest        =  0x01,
        .wValue          =  type_id,
        .wIndex          =  0,
        .wLength         =  report_len,
    };
    if (report != NULL) {
        vk_usbh_urb_set_buffer(&hid->ep0->urb, report, report_len);
    } else if (report_len > 0) {
        vk_usbh_urb_alloc_buffer(&hid->ep0->urb, report_len);
    }
    return vk_usbh_control_msg(hid->usbh, hid->dev, &req);
}

vsf_err_t __vk_usbh_hid_set_idle_imp(vk_usbh_hid_base_t *hid, uint_fast8_t id, uint_fast8_t duration)
{
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_OUT,
        .bRequest        =  0x0A,
        .wValue          =  (id << 0) | (duration << 8),
        .wIndex          =  hid->ifs->no,
        .wLength         =  0,
    };
    return vk_usbh_control_msg(hid->usbh, hid->dev, &req);
}

uint8_t * __vk_usbh_hid_get_tx_report_imp(vk_usbh_hid_eda_t *hid)
{
    return vk_usbh_urb_peek_buffer(&hid->urb_out);
}

uint8_t * __vk_usbh_hid_get_rx_report_imp(vk_usbh_hid_eda_t *hid)
{
    return vk_usbh_urb_peek_buffer(&hid->urb_in);
}

vsf_err_t __vk_usbh_hid_send_report_imp(vk_usbh_hid_eda_t *hid, uint8_t *buffer, int_fast32_t size)
{
    return vk_usbh_hid_submit_urb(hid, buffer, size, &hid->urb_out);
}

vsf_err_t __vk_usbh_hid_recv_report_imp(vk_usbh_hid_eda_t *hid, uint8_t *buffer, int_fast32_t size)
{
    return vk_usbh_hid_submit_urb(hid, buffer, size, &hid->urb_in);
}




// hid input, which uses vsf_hid
#ifndef WEAK_VSF_USBH_HID_INPUT_ON_DESC
WEAK(vsf_usbh_hid_input_on_desc)
int_fast32_t vsf_usbh_hid_input_on_desc(vk_usbh_hid_input_t *hid, uint8_t *desc_buf, uint_fast32_t desc_len)
{
#   if VSF_USE_INPUT == ENABLED && VSF_USE_INPUT_HID == ENABLED
    return vk_hid_parse_desc(&hid->use_as__vk_input_hid_t, desc_buf, desc_len);
#   else
    return -1;
#   endif
}
#endif

#ifndef WEAK_VSF_USBH_HID_INPUT_ON_REPORT_INPUT
WEAK(vsf_usbh_hid_input_on_report_input)
void vsf_usbh_hid_input_on_report_input(vk_usbh_hid_input_t *hid, uint8_t *report, uint_fast32_t len)
{
#   if VSF_USE_INPUT == ENABLED && VSF_USE_INPUT_HID == ENABLED
    vk_hid_process_input(&hid->use_as__vk_input_hid_t, report, len);
#   endif
}
#endif

#ifndef WEAK_VSF_USBH_HID_INPUT_ON_NEW
WEAK(vsf_usbh_hid_input_on_new)
void vsf_usbh_hid_input_on_new(vk_usbh_hid_input_t *hid)
{
#   if VSF_USE_INPUT == ENABLED && VSF_USE_INPUT_HID == ENABLED
    vk_hid_new_dev(&hid->use_as__vk_input_hid_t);
#   endif
}
#endif

#ifndef WEAK_VSF_USBH_HID_INPUT_ON_FREE
WEAK(vsf_usbh_hid_input_on_free)
void vsf_usbh_hid_input_on_free(vk_usbh_hid_input_t *hid)
{
#   if VSF_USE_INPUT == ENABLED && VSF_USE_INPUT_HID == ENABLED
    vk_hid_free_dev(&hid->use_as__vk_input_hid_t);
#   endif
}
#endif

static void vk_usbh_hid_input_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbh_hid_input_t *hid = container_of(eda, vk_usbh_hid_input_t, use_as__vsf_eda_t);

    switch (evt) {
    case VSF_EVT_INIT: {
            vk_usbh_ep0_t *ep0 = hid->ep0;
            vk_usbh_urb_t *urb = &ep0->urb;
            uint8_t *desc_buf;
            uint_fast32_t desc_len;
            int_fast32_t max_report_size;

            if (vk_usbh_urb_get_status(urb) != URB_OK) {
                __vsf_eda_crit_npb_leave(&ep0->crit);
                vk_usbh_remove_interface(hid->usbh, hid->dev, hid->ifs);
                return;
            }

            desc_buf = vk_usbh_urb_take_buffer(urb);
            desc_len = vk_usbh_urb_get_actual_length(urb);
            __vsf_eda_crit_npb_leave(&ep0->crit);

#ifndef WEAK_VSF_USBH_HID_INPUT_ON_DESC
            max_report_size = vsf_usbh_hid_input_on_desc(hid, desc_buf, desc_len);
#else
            max_report_size = WEAK_VSF_USBH_HID_INPUT_ON_DESC(hid, desc_buf, desc_len);
#endif
            if (max_report_size <= 0) {
                VSF_USBH_FREE(desc_buf);
                vk_usbh_remove_interface(hid->usbh, hid->dev, hid->ifs);
                return;
            }

            VSF_USBH_FREE(desc_buf);
            vk_usbh_hid_recv_report(&hid->use_as__vk_usbh_hid_eda_t, NULL,
                    max_report_size);
        }
        break;
    case VSF_EVT_MESSAGE: {
            vk_usbh_urb_t urb = { .urb_hcd = vsf_eda_get_cur_msg() };

            if (URB_OK == vk_usbh_urb_get_status(&urb)) {
                uint_fast32_t len = vk_usbh_urb_get_actual_length(&urb);
                uint8_t *report = vk_usbh_urb_peek_buffer(&urb);
#ifndef WEAK_VSF_USBH_HID_INPUT_ON_REPORT_INPUT
                vsf_usbh_hid_input_on_report_input(hid, report, len);
#else
                WEAK_VSF_USBH_HID_INPUT_ON_REPORT_INPUT(hid, report, len);
#endif
            }

            vk_usbh_hid_recv_report(&hid->use_as__vk_usbh_hid_eda_t, NULL, 0);
        }
        break;
    }
}

static void * __vk_usbh_hid_input_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs)
{
    vk_usbh_hid_input_t *hid = vk_usbh_hid_probe(usbh, dev, parser_ifs, sizeof(vk_usbh_hid_input_t), true);
    if (hid != NULL) {
        hid->user_evthandler = vk_usbh_hid_input_evthandler;
#ifndef WEAK_VSF_USBH_HID_INPUT_ON_NEW
        vsf_usbh_hid_input_on_new(hid);
#else
        WEAK_VSF_USBH_HID_INPUT_ON_NEW(hid);
#endif
    }
    return hid;
}

static void __vk_usbh_hid_input_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param)
{
    vk_usbh_hid_input_t *hid = param;
    vk_usbh_hid_disconnect(&hid->use_as__vk_usbh_hid_eda_t);
#ifndef WEAK_VSF_USBH_HID_INPUT_ON_FREE
    vsf_usbh_hid_input_on_free(hid);
#else
    WEAK_VSF_USBH_HID_INPUT_ON_FREE(hid);
#endif
}

#endif      // VSF_USE_USB_HOST && VSF_USE_USB_HOST_HID
