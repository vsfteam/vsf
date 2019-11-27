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

#define VSF_USBH_IMPLEMENT_vsf_usbh_hcd_urb_t
#define VSF_USBH_IMPLEMENT_CLASS
#include "vsf.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE != ENABLED
#   error "VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE is required"
#endif

#ifndef VSF_USBH_BTHCI_CFG_SCO_IN_NUM
#   define VSF_USBH_BTHCI_CFG_SCO_IN_NUM    8
#endif

#ifndef VSF_USBH_BTHCI_CFG_SCO_OUT_NUM
#   define VSF_USBH_BTHCI_CFG_SCO_OUT_NUM   8
#endif

#ifndef VSF_USBH_BTHCI_CFG_ACL_IN_NUM
#   define VSF_USBH_BTHCI_CFG_ACL_IN_NUM    8
#endif

#ifndef VSF_USBH_BTHCI_CFG_ACL_OUT_NUM
#   define VSF_USBH_BTHCI_CFG_ACL_OUT_NUM   8
#endif

#ifndef VSF_USBH_BTHCI_CFG_URB_BUFSIZE
#   define VSF_USBH_BTHCI_CFG_URB_BUFSIZE   1024
#endif

// VSF_USBH_BTHCI_CFG_URB_OUT_USE_LOCAL_BUFFER MUST be enabled for btstack if fragmentation is used
//  because btstack will overwrite last 4 bytes of previous packet with header of the current packet
#ifndef VSF_USBH_BTHCI_CFG_URB_OUT_USE_LOCAL_BUFFER
#   define VSF_USBH_BTHCI_CFG_URB_OUT_USE_LOCAL_BUFFER          ENABLED

#   ifndef VSF_USBH_BTHCI_CFG_URB_OUT_USE_DYNAMIC_BUFFER
#       define VSF_USBH_BTHCI_CFG_URB_OUT_USE_DYNAMIC_BUFFER    ENABLED
#   endif
#endif



#define BTHCI_PACKET_TYPE_CMD               1
#define BTHCI_PACKET_TYPE_ACL               2
#define BTHCI_PACKET_TYPE_SCO               3
#define BTHCI_PACKET_TYPE_EVENT             4
#define BTHCI_PACKET_TYPE_IN                0x00
#define BTHCI_PACKET_TYPE_OUT               0x80

#if (VSF_USBH_BTHCI_CFG_SCO_OUT_NUM + VSF_USBH_BTHCI_CFG_SCO_IN_NUM) > 0
#   define VSF_USBH_BTHCI_CFG_SCO_EN        ENABLED
#endif

#if (VSF_USBH_BTHCI_CFG_ACL_OUT_NUM + VSF_USBH_BTHCI_CFG_ACL_IN_NUM) > 0
#   define VSF_USBH_BTHCI_CFG_ACL_EN        ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_usbh_bthci_t vsf_usbh_bthci_t;

// input/output control block
struct vsf_usbh_bthci_iocb_t {
    vsf_usbh_urb_t urb;
    uint8_t type;
    uint8_t is_supported    : 1;
    uint8_t is_icb          : 1;
    uint8_t is_busy         : 1;
    uint8_t is_ep0_claimed  : 1;
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
#if VSF_USBH_BTHCI_CFG_ACL_IN_NUM > 0
                    vsf_usbh_bthci_iocb_t acl_icb[VSF_USBH_BTHCI_CFG_ACL_IN_NUM];
#endif
#if VSF_USBH_BTHCI_CFG_SCO_IN_NUM > 0
                    vsf_usbh_bthci_iocb_t sco_icb[VSF_USBH_BTHCI_CFG_SCO_IN_NUM];
#endif
                };
                vsf_usbh_bthci_iocb_t icb[1 + VSF_USBH_BTHCI_CFG_ACL_IN_NUM + VSF_USBH_BTHCI_CFG_SCO_IN_NUM];
            };

            union {
                struct {
                    vsf_usbh_bthci_iocb_t cmd_ocb;
#if VSF_USBH_BTHCI_CFG_ACL_OUT_NUM > 0
                    vsf_usbh_bthci_iocb_t acl_ocb[VSF_USBH_BTHCI_CFG_ACL_OUT_NUM];
#endif
#if VSF_USBH_BTHCI_CFG_SCO_OUT_NUM > 0
                    vsf_usbh_bthci_iocb_t sco_ocb[VSF_USBH_BTHCI_CFG_SCO_OUT_NUM];
#endif
                };
                vsf_usbh_bthci_iocb_t ocb[1 + VSF_USBH_BTHCI_CFG_ACL_OUT_NUM + VSF_USBH_BTHCI_CFG_SCO_OUT_NUM];
            };
        };
        vsf_usbh_bthci_iocb_t iocb[
                        2       // event_icb, cmd_ocb
                    +   VSF_USBH_BTHCI_CFG_ACL_IN_NUM + VSF_USBH_BTHCI_CFG_SCO_IN_NUM
                    +   VSF_USBH_BTHCI_CFG_ACL_OUT_NUM + VSF_USBH_BTHCI_CFG_SCO_OUT_NUM];
    };
};
typedef struct vsf_usbh_bthci_t vsf_usbh_bthci_t;

/*============================ LOCAL VARIABLES ===============================*/

static const vsf_usbh_dev_id_t vsf_usbh_bthci_dev_id[] =
{
    { VSF_USBH_MATCH_DEV_CLASS(USB_CLASS_WIRELESS_CONTROLLER, 0x01, 0x01) },
    // BCM20702
    {
        .match_vendor       = true,
        .idVendor           = 0x0A5C,   // broadcom
        VSF_USBH_MATCH_DEV_CLASS(USB_CLASS_VENDOR_SPEC, 0x01, 0x01)
    },
};

/*============================ PROTOTYPES ====================================*/

static void * vsf_usbh_bthci_probe(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev, vsf_usbh_ifs_parser_t *parser_ifs);
static void vsf_usbh_bthci_disconnect(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev, void *param);

/*============================ GLOBAL VARIABLES ==============================*/

const vsf_usbh_class_drv_t vsf_usbh_bthci_drv = {
    .name       = "bthci_usb",
    .dev_id_num = dimof(vsf_usbh_bthci_dev_id),
    .dev_ids    = vsf_usbh_bthci_dev_id,
    .probe      = vsf_usbh_bthci_probe,
    .disconnect = vsf_usbh_bthci_disconnect,
};

/*============================ PROTOTYPES ====================================*/

#if     defined(WEAK_VSF_USBH_BTHCI_ON_NEW_EXTERN)                              \
    &&  defined(WEAK_VSF_USBH_BTHCI_ON_NEW)
WEAK_VSF_USBH_BTHCI_ON_NEW_EXTERN
#endif

#if     defined(WEAK_VSF_USBH_BTHCI_ON_DEL_EXTERN)                              \
    &&  defined(WEAK_VSF_USBH_BTHCI_ON_DEL)
WEAK_VSF_USBH_BTHCI_ON_DEL_EXTERN
#endif

#if     defined(WEAK_VSF_USBH_BTHCI_ON_PACKET_EXTERN)                           \
    &&  defined(WEAK_VSF_USBH_BTHCI_ON_PACKET)
WEAK_VSF_USBH_BTHCI_ON_PACKET_EXTERN
#endif

SECTION(".text.vsf.kernel.eda")
vsf_err_t __vsf_eda_fini(vsf_eda_t *pthis);

/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSF_USBH_BTHCI_ON_NEW
WEAK(vsf_usbh_bthci_on_new)
void vsf_usbh_bthci_on_new(void *dev, vsf_usbh_dev_id_t *id) {}
#endif

#ifndef WEAK_VSF_USBH_BTHCI_ON_DEL
WEAK(vsf_usbh_bthci_on_del)
void vsf_usbh_bthci_on_del(void *dev) {}
#endif

#ifndef WEAK_VSF_USBH_BTHCI_ON_PACKET
WEAK(vsf_usbh_bthci_on_packet)
void vsf_usbh_bthci_on_packet(void *dev, uint8_t type, uint8_t *packet, uint16_t size) {}
#endif

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
        if ((ocb->type == type) && ocb->is_supported && !ocb->is_busy) {
            if ((ocb->type != BTHCI_PACKET_TYPE_CMD) && !vsf_usbh_urb_is_alloced(&ocb->urb)) {
                vsf_usbh_t *usbh = bthci->usbh;
                vsf_usbh_dev_t *dev = bthci->dev;
                if (VSF_ERR_NONE != vsf_usbh_alloc_urb(usbh, dev, &ocb->urb)) {
                    break;
                }
#if     VSF_USBH_BTHCI_CFG_URB_OUT_USE_LOCAL_BUFFER == ENABLED                  \
    &&  VSF_USBH_BTHCI_CFG_URB_OUT_USE_DYNAMIC_BUFFER != ENABLED
                if (NULL == vsf_usbh_urb_alloc_buffer(&ocb->urb, VSF_USBH_BTHCI_CFG_URB_BUFSIZE)) {
                    vsf_usbh_free_urb(usbh, &ocb->urb);
                    break;
                }
#endif
            }
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
        err = __vsf_eda_crit_npb_enter(&dev->ep0.crit);
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
            __vsf_eda_crit_npb_leave(&dev->ep0.crit);

            err = vsf_usbh_urb_get_status(&dev->ep0.urb);
            if (VSF_ERR_NONE != err) {
                break;
            }

            if (    (VSF_ERR_NONE != vsf_usbh_alloc_urb(usbh, dev, &bthci->event_icb.urb))
                ||  (NULL == vsf_usbh_urb_alloc_buffer(&bthci->event_icb.urb, VSF_USBH_BTHCI_CFG_URB_BUFSIZE))
                ||  (VSF_ERR_NONE != vsf_usbh_submit_urb(usbh, &bthci->event_icb.urb))) {
                break;
            }

#if VSF_USBH_BTHCI_CFG_SCO_IN_NUM > 0
            for (int i = 0; i < VSF_USBH_BTHCI_CFG_SCO_IN_NUM; i++) {
                if (    bthci->sco_icb[i].is_supported
                    &&  (   (VSF_ERR_NONE != vsf_usbh_alloc_urb(usbh, dev, &bthci->sco_icb[i].urb))
                        ||  (NULL == vsf_usbh_urb_alloc_buffer(&bthci->sco_icb[i].urb, VSF_USBH_BTHCI_CFG_URB_BUFSIZE))
                        ||  (VSF_ERR_NONE != vsf_usbh_submit_urb(usbh, &bthci->sco_icb[i].urb)))) {
                    // ignore errors and go on
                    vsf_trace(VSF_TRACE_ERROR, "fail to prepare scoin_icb %d", i);
                    break;
                }
            }
#endif

#if VSF_USBH_BTHCI_CFG_ACL_IN_NUM > 0
            for (int i = 0; i < VSF_USBH_BTHCI_CFG_ACL_IN_NUM; i++) {
                if (    bthci->acl_icb[i].is_supported
                    &&  (   (VSF_ERR_NONE != vsf_usbh_alloc_urb(usbh, dev, &bthci->acl_icb[i].urb))
                        ||  (NULL == vsf_usbh_urb_alloc_buffer(&bthci->acl_icb[i].urb, VSF_USBH_BTHCI_CFG_URB_BUFSIZE))
                        ||  (VSF_ERR_NONE != vsf_usbh_submit_urb(usbh, &bthci->acl_icb[i].urb)))) {
                    // ignore errors and go on
                    vsf_trace(VSF_TRACE_ERROR, "fail to prepare aclin_icb %d", i);
                    break;
                }
            }
#endif

#ifndef WEAK_VSF_USBH_BTHCI_ON_NEW
            vsf_usbh_bthci_on_new(bthci, &bthci->id);
#else
            WEAK_VSF_USBH_BTHCI_ON_NEW(bthci, &bthci->id);
#endif
        } else {
            vsf_usbh_bthci_iocb_t *iocb = vsf_usbh_bthci_get_iocb(bthci, (vsf_usbh_hcd_urb_t *)vsf_eda_get_cur_msg());
            vsf_usbh_urb_t *urb;
            VSF_USB_ASSERT(iocb != NULL);
            urb = &iocb->urb;

            int_fast16_t status = vsf_usbh_urb_get_status(urb);
            void * buffer = vsf_usbh_urb_peek_buffer(urb);
            uint_fast32_t actual_length = vsf_usbh_urb_get_actual_length(urb);

            if (iocb->is_icb) {
                if (URB_OK == status) {
#ifndef WEAK_VSF_USBH_BTHCI_ON_PACKET
                    vsf_usbh_bthci_on_packet(bthci, iocb->type | BTHCI_PACKET_TYPE_IN,
                        buffer, actual_length);
#else
                    WEAK_VSF_USBH_BTHCI_ON_PACKET(bthci, iocb->type | BTHCI_PACKET_TYPE_IN,
                        buffer, actual_length);
#endif
                }
                err = vsf_usbh_submit_urb(usbh, urb);
            } else {
                if (iocb->is_ep0_claimed) {
                    iocb->is_ep0_claimed = false;
                    __vsf_eda_crit_npb_leave(&dev->ep0.crit);
                }
                iocb->is_busy = false;
                if (URB_OK == status) {
#ifndef WEAK_VSF_USBH_BTHCI_ON_PACKET
                    vsf_usbh_bthci_on_packet(bthci, iocb->type | BTHCI_PACKET_TYPE_OUT,
                        buffer, actual_length);
#else
                    WEAK_VSF_USBH_BTHCI_ON_PACKET(bthci, iocb->type | BTHCI_PACKET_TYPE_OUT,
                        buffer, actual_length);
#endif
                }
            }
        }
        break;
    }

    if (err < 0) {
        vsf_usbh_remove_interface(usbh, dev, bthci->ifs);
    }
}

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

static void * vsf_usbh_bthci_probe(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev, vsf_usbh_ifs_parser_t *parser_ifs)
{
    vsf_usbh_ifs_t *ifs = parser_ifs->ifs;
    struct usb_interface_desc_t *desc_ifs = parser_ifs->parser_alt[ifs->cur_alt].desc_ifs;
    struct usb_endpoint_desc_t *desc_ep = parser_ifs->parser_alt[ifs->cur_alt].desc_ep;
    vsf_usbh_bthci_t *bthci;
    uint_fast8_t epaddr;
    bool is_in;

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
        is_in = (epaddr & USB_DIR_MASK) == USB_DIR_IN;
        switch (desc_ep->bmAttributes) {
#if VSF_USBH_BTHCI_CFG_SCO_EN == ENABLED
        case USB_ENDPOINT_XFER_ISOC:
            if (is_in) {
                for (int i = 0; i < VSF_USBH_BTHCI_CFG_SCO_IN_NUM; i++) {
                    vsf_usbh_urb_prepare(&bthci->sco_icb[i].urb, dev, desc_ep);
                    bthci->sco_icb[i].type = BTHCI_PACKET_TYPE_SCO;
                    bthci->sco_icb[i].is_supported = true;
                    bthci->sco_icb[i].is_icb = true;
                }
            } else {
                for (int i = 0; i < VSF_USBH_BTHCI_CFG_SCO_OUT_NUM; i++) {
                    vsf_usbh_urb_prepare(&bthci->sco_ocb[i].urb, dev, desc_ep);
                    bthci->sco_ocb[i].type = BTHCI_PACKET_TYPE_SCO;
                    bthci->sco_ocb[i].is_supported = true;
                    bthci->sco_ocb[i].is_icb = false;
                }
            }
            break;
#endif
#if VSF_USBH_BTHCI_CFG_ACL_EN == ENABLED
        case USB_ENDPOINT_XFER_BULK:
            if (is_in) {
                for (int i = 0; i < VSF_USBH_BTHCI_CFG_SCO_IN_NUM; i++) {
                    vsf_usbh_urb_prepare(&bthci->acl_icb[i].urb, dev, desc_ep);
                    bthci->acl_icb[i].type = BTHCI_PACKET_TYPE_ACL;
                    bthci->acl_icb[i].is_supported = true;
                    bthci->acl_icb[i].is_icb = true;
                }
            } else {
                for (int i = 0; i < VSF_USBH_BTHCI_CFG_SCO_OUT_NUM; i++) {
                    vsf_usbh_urb_prepare(&bthci->acl_ocb[i].urb, dev, desc_ep);
                    bthci->acl_ocb[i].type = BTHCI_PACKET_TYPE_ACL;
                    bthci->acl_ocb[i].is_supported = true;
                    bthci->acl_ocb[i].is_icb = false;
                }
            }
            break;
#endif
        case USB_ENDPOINT_XFER_INT:
            if (is_in) {
                vsf_usbh_urb_prepare(&bthci->event_icb.urb, dev, desc_ep);
                bthci->event_icb.type = BTHCI_PACKET_TYPE_EVENT;
                bthci->event_icb.is_supported = true;
                bthci->event_icb.is_icb = true;
            }
            break;
        }
        desc_ep = (struct usb_endpoint_desc_t *)((uintptr_t)desc_ep + USB_DT_ENDPOINT_SIZE);
    }

    if (!bthci->event_icb.is_supported) {
        goto free_all;
    }

    bthci->dev = dev;
    bthci->usbh = usbh;
    bthci->ifs = ifs;
    bthci->id.idProduct = usbh->parser->desc_device->idProduct;
    bthci->id.idVendor = usbh->parser->desc_device->idVendor;

    bthci->cmd_ocb.type = BTHCI_PACKET_TYPE_CMD;
    bthci->cmd_ocb.is_supported = true;
    bthci->cmd_ocb.is_icb = false;
    bthci->cmd_ocb.urb.urb_hcd = dev->ep0.urb.urb_hcd;

    vsf_eda_set_evthandler(&(bthci->eda), vsf_usbh_bthci_evthandler);
    bthci->eda.on_terminate = vsf_usbh_bthci_on_eda_terminate;
    vsf_eda_init(&bthci->eda, vsf_prio_inherit, false);
    return bthci;

free_all:
    vsf_usbh_bthci_free_all(bthci);
    VSF_USBH_FREE(bthci);
    return NULL;
}

static void vsf_usbh_bthci_disconnect(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev, void *param)
{
    vsf_usbh_bthci_t *bthci = param;

#ifndef WEAK_VSF_USBH_BTHCI_ON_DEL
    vsf_usbh_bthci_on_del(bthci);
#else
    WEAK_VSF_USBH_BTHCI_ON_DEL(bthci);
#endif

    vsf_usbh_bthci_free_all(bthci);
    __vsf_eda_fini(&bthci->eda);
}

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
    urb = &ocb->urb;

    switch (type) {
    case BTHCI_PACKET_TYPE_CMD:
        if (ocb->is_ep0_claimed || __vsf_eda_crit_npb_try_to_enter(&bthci->dev->ep0.crit, 0)) {
            return VSF_ERR_NOT_AVAILABLE;
        } else {
            struct usb_ctrlrequest_t req = {
                .bRequestType    =  USB_TYPE_CLASS | USB_RECIP_DEVICE | USB_DIR_OUT,
                .bRequest        =  0,
                .wValue          =  0,
                .wIndex          =  0,
                .wLength         =  size,
            };

            ocb->is_ep0_claimed = true;
            ocb->is_busy = true;
            vsf_usbh_urb_set_buffer(urb, packet, size);
            return vsf_usbh_control_msg_ex(usbh, bthci->dev, &req, 0, &bthci->eda);
        }
        break;
    case BTHCI_PACKET_TYPE_ACL:
        flags |= URB_ZERO_PACKET;
        break;
    case BTHCI_PACKET_TYPE_SCO:
#if VSF_USBH_BTHCI_CFG_SCO_OUT_NUM > 0
        break;
#else
        VSF_USB_ASSERT(false);
        return VSF_ERR_NOT_SUPPORT;
#endif
    }

#if VSF_USBH_BTHCI_CFG_URB_OUT_USE_LOCAL_BUFFER == ENABLED
#   if VSF_USBH_BTHCI_CFG_URB_OUT_USE_DYNAMIC_BUFFER == ENABLED
    uint8_t *buffer = vsf_usbh_urb_alloc_buffer(&ocb->urb, size);
    if (NULL == buffer) {
        VSF_USB_ASSERT(false);
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }
    memcpy(buffer, packet, size);
#   else
    if (size > VSF_USBH_BTHCI_CFG_URB_BUFSIZE) {
        VSF_USB_ASSERT(false);
        return VSF_ERR_NOT_SUPPORT;
    }
    memcpy(urb->urb_hcd->buffer, packet, size);
    urb->urb_hcd->transfer_length = size;
#   endif
#else
    vsf_usbh_urb_set_buffer(urb, packet, size);
#endif
    ocb->is_busy = true;
    return vsf_usbh_submit_urb_ex(usbh, urb, flags, &bthci->eda);
}

#endif      // VSF_USE_USB_HOST && VSF_USE_USB_HOST_BTHCI
