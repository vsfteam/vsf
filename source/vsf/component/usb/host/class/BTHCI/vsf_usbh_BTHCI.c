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

#if VSF_USE_USB_HOST == ENABLED && VSF_USE_USB_HOST_BTHCI == ENABLED

#define VSF_USBH_IMPLEMENT_CLASS
#include "vsf.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE != ENABLED
#   error "VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE is required"
#endif

#ifndef VSF_USBH_BTHCI_CFG_SCO_EN
#   define VSF_USBH_BTHCI_CFG_SCO_EN    DISABLED
#endif

#define BTHCI_PACKET_TYPE_CMD           1
#define BTHCI_PACKET_TYPE_ACL           2
#define BTHCI_PACKET_TYPE_SCO           3
#define BTHCI_PACKET_TYPE_EVENT         4
#define BTHCI_PACKET_TYPE_IN            0x00
#define BTHCI_PACKET_TYPE_OUT           0x80

#define BTHCI_BUFSIZE                    (255 + 3)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_usbh_bthci_t vsf_usbh_bthci_t;

// input/output control block
struct vsf_usbh_bthci_iocb_t {
    vsf_usbh_urb_t urb;
    uint8_t type;
    bool is_icb         : 1;
    bool is_busy        : 1;
    bool is_ep0_claimed : 1;
};
typedef struct vsf_usbh_bthci_iocb_t vsf_usbh_bthci_iocb_t;

struct vsf_usbh_bthci_t {
    vsf_usbh_t *usbh;
    vsf_usbh_dev_t *dev;
    vsf_usbh_ifs_t *ifs;
    vsf_usbh_dev_id_t id;
    vsf_eda_t eda;

    bool is_initing;

    union {
        struct {
            union {
                struct {
                    vsf_usbh_bthci_iocb_t event_icb;
                    vsf_usbh_bthci_iocb_t aclin_icb;
#if VSF_USBH_BTHCI_CFG_SCO_EN == ENABLED
                    vsf_usbh_bthci_iocb_t scoin_icb;
#endif
                };
#if VSF_USBH_BTHCI_CFG_SCO_EN == ENABLED
                vsf_usbh_bthci_iocb_t icb[3];
#else
                vsf_usbh_bthci_iocb_t icb[2];
#endif
            };

            union {
                struct {
                    vsf_usbh_bthci_iocb_t cmd_ocb;
                    vsf_usbh_bthci_iocb_t aclout_ocb;
#if VSF_USBH_BTHCI_CFG_SCO_EN == ENABLED
                    vsf_usbh_bthci_iocb_t scoout_ocb;
#endif
                };
#if VSF_USBH_BTHCI_CFG_SCO_EN == ENABLED
                vsf_usbh_bthci_iocb_t ocb[3];
#else
                vsf_usbh_bthci_iocb_t ocb[2];
#endif
            };
        };

#if VSF_USBH_BTHCI_CFG_SCO_EN == ENABLED
        vsf_usbh_bthci_iocb_t iocb[6];
#else
        vsf_usbh_bthci_iocb_t iocb[4];
#endif
    };
};
typedef struct vsf_usbh_bthci_t vsf_usbh_bthci_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

SECTION(".text.vsf.kernel.eda")
vsf_err_t __vsf_eda_fini(vsf_eda_t *pthis);

/*============================ IMPLEMENTATION ================================*/

WEAK void vsf_usbh_bthci_on_new(void *dev, vsf_usbh_dev_id_t *id) {}
WEAK void vsf_usbh_bthci_on_del(void *dev) {}
WEAK void vsf_usbh_bthci_on_packet(void *dev, uint8_t type, uint8_t *packet, uint16_t size) {}

static vsf_usbh_bthci_iocb_t * vsf_usbh_bthci_get_iocb(vsf_usbh_bthci_t *bthci, vsf_usbh_hcd_urb_t *urb_hcd)
{
    vsf_usbh_bthci_iocb_t *iocb = bthci->iocb;
    for (int i = 0; i < dimof(bthci->iocb); i++, iocb++) {
        if (iocb->urb.urb_hcd == urb_hcd) {
            return iocb;
        }
    }
    return NULL;
}

static vsf_usbh_bthci_iocb_t * vsf_usbh_bthci_get_ocb(vsf_usbh_bthci_t *bthci, uint8_t type)
{
    vsf_usbh_bthci_iocb_t *ocb = bthci->ocb;
    for (int i = 0; i < dimof(bthci->ocb); i++, ocb++) {
        if ((ocb->type == type) && !ocb->is_busy) {
            return ocb;
        }
    }
    return NULL;
}

static void vsf_usbh_bthci_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vsf_usbh_bthci_t *bthci = container_of(eda, vsf_usbh_bthci_t, eda);
    vsf_usbh_dev_t *dev = bthci->dev;
    vsf_usbh_t *usbh = bthci->usbh;
    vsf_err_t err = VSF_ERR_NONE;

    switch (evt) {
    case VSF_EVT_INIT:
        bthci->is_initing = true;
        err = vsf_eda_crit_enter(&dev->ep0.crit, -1);
        if (VSF_ERR_NONE != err) {
            break;
        }
        // fall through
    case VSF_EVT_SYNC:
        err = vsf_usbh_set_interface(usbh, dev, 1, 0);
        break;
    case VSF_EVT_MESSAGE:
        if (bthci->is_initing) {
            bthci->is_initing = false;
            vsf_eda_crit_leave(&dev->ep0.crit);

            err = vsf_usbh_urb_get_status(&dev->ep0.urb);
            if (VSF_ERR_NONE != err) {
                break;
            }

            if (    (VSF_ERR_NONE != vsf_usbh_alloc_urb(usbh, dev, &bthci->aclout_ocb.urb))
                ||  (VSF_ERR_NONE != vsf_usbh_alloc_urb(usbh, dev, &bthci->event_icb.urb))
                ||  (NULL == vsf_usbh_urb_alloc_buffer(&bthci->event_icb.urb, BTHCI_BUFSIZE))
                ||  (VSF_ERR_NONE != vsf_usbh_submit_urb(usbh, &bthci->event_icb.urb))
#if VSF_USBH_BTHCI_CFG_SCO_EN == ENABLED
                ||  (VSF_ERR_NONE != vsf_usbh_alloc_urb(usbh, dev, &bthci->scoin_icb.urb))
                ||  (NULL == vsf_usbh_urb_alloc_buffer(&bthci->scoin_icb.urb, BTHCI_BUFSIZE))
                ||  (VSF_ERR_NONE != vsf_usbh_submit_urb(usbh, &bthci->scoin_icb.urb))
#endif
                ||  (VSF_ERR_NONE != vsf_usbh_alloc_urb(usbh, dev, &bthci->aclin_icb.urb))
                ||  (NULL == vsf_usbh_urb_alloc_buffer(&bthci->aclin_icb.urb, BTHCI_BUFSIZE))
                ||  (VSF_ERR_NONE != vsf_usbh_submit_urb(usbh, &bthci->aclin_icb.urb))) {
                break;
            }
            vsf_usbh_bthci_on_new(bthci, &bthci->id);
        } else {
            vsf_usbh_bthci_iocb_t *iocb = vsf_usbh_bthci_get_iocb(bthci, (vsf_usbh_hcd_urb_t *)vsf_eda_get_cur_msg());
            vsf_usbh_urb_t *urb;
            ASSERT(iocb != NULL);
            urb = &iocb->urb;

            int_fast16_t status = vsf_usbh_urb_get_status(urb);
            void * buffer = vsf_usbh_urb_peek_buffer(urb);
            uint_fast32_t actual_length = vsf_usbh_urb_get_actual_length(urb);

            if (iocb->is_icb) {
                if (URB_OK == status) {
                    vsf_usbh_bthci_on_packet(bthci, iocb->type | BTHCI_PACKET_TYPE_IN,
                        buffer, actual_length);
                }
                err = vsf_usbh_submit_urb(usbh, urb);
            } else {
                if (iocb->is_ep0_claimed) {
                    iocb->is_ep0_claimed = false;
                    vsf_eda_crit_leave(&dev->ep0.crit);
                }
                if (URB_OK == status) {
                    vsf_usbh_bthci_on_packet(bthci, iocb->type | BTHCI_PACKET_TYPE_OUT,
                        buffer, actual_length);
                }
            }
        }
        break;
    }

    if (err < 0) {
        vsf_usbh_remove_interface(usbh, dev, bthci->ifs);
    }
}

static const vsf_usbh_dev_id_t vsf_usbh_bthci_dev_id[] =
{
    {
        .match_dev_class    = true,
        .match_dev_subclass = true,
        .match_dev_protocol = true,
        .bDeviceClass       = 0xE0, // Wireless class: USB_CLASS_WIRELESS_CONTROLLER
        .bDeviceSubClass    = 0x01, // RF subclass
        .bDeviceProtocol    = 0x01, // bt programming protocol
    },
};

static void vsf_usbh_bthci_free_all(vsf_usbh_bthci_t *bthci)
{
    vsf_usbh_t *usbh = bthci->usbh;
    vsf_usbh_bthci_iocb_t *iocb = bthci->iocb;
    for (int i = 0; i < dimof(bthci->iocb); i++, iocb++) {
        if (iocb->urb.urb_hcd != bthci->dev->ep0.urb.urb_hcd) {
            vsf_usbh_free_urb(usbh, &iocb->urb);
        }
    }
}

static void vsf_usbh_bthci_on_eda_terminate(vsf_eda_t *eda)
{
    vsf_usbh_bthci_t *bthci = container_of(eda, vsf_usbh_bthci_t, eda);
    VSF_USBH_FREE(bthci);
}

static void * vsf_usbh_bthci_probe(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev,
        vsf_usbh_ifs_parser_t *parser_ifs)
{
    vsf_usbh_ifs_t *ifs = parser_ifs->ifs;
    struct usb_interface_desc_t *desc_ifs = parser_ifs->parser_alt[ifs->cur_alt].desc_ifs;
    struct usb_endpoint_desc_t *desc_ep = parser_ifs->parser_alt[ifs->cur_alt].desc_ep;
    vsf_usbh_bthci_t *bthci;
    uint_fast8_t epaddr;

    // only probe interface 0
    if (desc_ifs->bInterfaceNumber != 0) {
        return NULL;
    }

    bthci = VSF_USBH_MALLOC(sizeof(vsf_usbh_bthci_t));
    if (NULL == bthci) {
        return NULL;
    }
    memset(bthci, 0, sizeof(vsf_usbh_bthci_t));

    for (int i = 0; i < desc_ifs->bNumEndpoints; i++) {
        if (desc_ep->bLength != USB_DT_ENDPOINT_SIZE) {
            goto free_all;
        }
        epaddr = desc_ep->bEndpointAddress;
        switch (desc_ep->bmAttributes)
        {
#if VSF_USBH_BTHCI_CFG_SCO_EN == ENABLED
        case USB_ENDPOINT_XFER_ISOC:
            if (epaddr & USB_ENDPOINT_DIR_MASK) {
                vsf_usbh_urb_prepare(&bthci->scoin_icb.urb, dev, desc_ep);
            } else {
                vsf_usbh_urb_prepare(&bthci->scoout_ocb.urb, dev, desc_ep);
            }
            break;
#endif
        case USB_ENDPOINT_XFER_BULK:
            if (epaddr & USB_ENDPOINT_DIR_MASK) {
                vsf_usbh_urb_prepare(&bthci->aclin_icb.urb, dev, desc_ep);
            } else {
                vsf_usbh_urb_prepare(&bthci->aclout_ocb.urb, dev, desc_ep);
            }
            break;
        case USB_ENDPOINT_XFER_INT:
            if (epaddr & USB_ENDPOINT_DIR_MASK) {
                vsf_usbh_urb_prepare(&bthci->event_icb.urb, dev, desc_ep);
            }
            break;
        }
        desc_ep = (struct usb_endpoint_desc_t *)((uint32_t)desc_ep + USB_DT_ENDPOINT_SIZE);
    }
    if (    !vsf_usbh_urb_is_valid(&bthci->event_icb.urb)
        ||  !vsf_usbh_urb_is_valid(&bthci->aclin_icb.urb)
        ||  !vsf_usbh_urb_is_valid(&bthci->aclout_ocb.urb)) {
        goto free_all;
    }

    bthci->dev = dev;
    bthci->usbh = usbh;
    bthci->ifs = ifs;
    bthci->id.idProduct = usbh->parser->desc_device->idProduct;
    bthci->id.idVendor = usbh->parser->desc_device->idVendor;

    // event_icb is used for init at startup
    bthci->event_icb.type = BTHCI_PACKET_TYPE_EVENT;
    bthci->event_icb.is_icb = true;
    bthci->aclin_icb.type = BTHCI_PACKET_TYPE_ACL;
    bthci->aclin_icb.is_icb = true;
    bthci->aclout_ocb.type = BTHCI_PACKET_TYPE_ACL;
    bthci->aclout_ocb.is_icb = false;
    bthci->cmd_ocb.type = BTHCI_PACKET_TYPE_CMD;
    bthci->cmd_ocb.is_icb = false;
    bthci->cmd_ocb.urb.urb_hcd = dev->ep0.urb.urb_hcd;
#if VSF_USBH_BTHCI_CFG_SCO_EN == ENABLED
    bthci->scoin_icb.type = BTHCI_PACKET_TYPE_SCO;
    bthci->scoin_icb.is_icb = true;
    bthci->scoout_ocb.type = BTHCI_PACKET_TYPE_SCO;
    bthci->scoout_ocb.is_icb = false;
#endif

    bthci->eda.evthandler = vsf_usbh_bthci_evthandler;
    bthci->eda.on_terminate = vsf_usbh_bthci_on_eda_terminate;
    vsf_eda_init(&bthci->eda, vsf_priority_inherit, false);
    return bthci;

free_all:
    vsf_usbh_bthci_free_all(bthci);
    VSF_USBH_FREE(bthci);
    return NULL;
}

static void vsf_usbh_bthci_disconnect(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev, void *param)
{
    vsf_usbh_bthci_t *bthci = param;

    vsf_usbh_bthci_on_del(bthci);
    vsf_usbh_bthci_free_all(bthci);
    __vsf_eda_fini(&bthci->eda);
}

const vsf_usbh_class_drv_t vsf_usbh_bthci_drv = {
    .name       = "bthci_usb",
    .dev_id_num = dimof(vsf_usbh_bthci_dev_id),
    .dev_ids    = vsf_usbh_bthci_dev_id,
    .probe      = vsf_usbh_bthci_probe,
    .disconnect = vsf_usbh_bthci_disconnect,
};

bool vsf_usbh_bthci_can_send(void *dev, uint8_t type)
{
    vsf_usbh_bthci_t *bthci = dev;
    vsf_usbh_bthci_iocb_t *ocb = vsf_usbh_bthci_get_ocb(bthci, type);
    return ocb != NULL;
}

vsf_err_t vsf_usbh_bthci_send(void *dev, uint8_t type, uint8_t *packet, uint16_t size)
{
    vsf_usbh_bthci_t *bthci = dev;
    vsf_usbh_t *usbh = bthci->usbh;
    vsf_usbh_bthci_iocb_t *ocb = vsf_usbh_bthci_get_ocb(bthci, type);
    vsf_usbh_urb_t *urb;
    uint_fast16_t flags = 0;

    if (!ocb) {
        return VSF_ERR_NOT_AVAILABLE;
    }
    if (size > BTHCI_BUFSIZE) {
        return VSF_ERR_NOT_SUPPORT;
    }

    urb = &ocb->urb;
    vsf_usbh_urb_set_buffer(urb, packet, size);

    switch (type) {
    case BTHCI_PACKET_TYPE_CMD:
        if (ocb->is_ep0_claimed || vsf_eda_crit_enter(&bthci->dev->ep0.crit, 0)) {
            return VSF_ERR_NOT_AVAILABLE;
        }

        ocb->is_ep0_claimed = true;
        do {
            struct usb_ctrlrequest_t req = {
                .bRequestType    =  USB_TYPE_CLASS | USB_RECIP_DEVICE | USB_DIR_OUT,
                .bRequest        =  0,
                .wValue          =  0,
                .wIndex          =  0,
                .wLength         =  size,
            };
            return vsf_usbh_control_msg_ex(usbh, bthci->dev, &req, &bthci->eda);
        } while (0);
        break;
    case BTHCI_PACKET_TYPE_ACL:
        flags |= URB_ZERO_PACKET;
        break;
    case BTHCI_PACKET_TYPE_SCO:
#if VSF_USBH_BTHCI_CFG_SCO_EN == ENABLED
        break;
#else
        return VSF_ERR_NOT_SUPPORT;
#endif
    }
    return vsf_usbh_submit_urb_ex(usbh, urb, flags, &bthci->eda);
}

#endif      // VSF_USE_USB_HOST && VSF_USE_USB_HOST_BTHCI
