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

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_BTHCI == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_USBH_CLASS_IMPLEMENT_CLASS__

#include "kernel/vsf_kernel.h"
#include "../../vsf_usbh.h"
#include "./vsf_usbh_BTHCI.h"

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

// input/output control block
typedef struct vk_usbh_bthci_iocb_t {
    vk_usbh_urb_t urb;
    uint8_t type;
    uint8_t is_supported    : 1;
    uint8_t is_icb          : 1;
    uint8_t is_busy         : 1;
    uint8_t is_ep0_claimed  : 1;
} vk_usbh_bthci_iocb_t;

typedef struct vk_usbh_bthci_t {
    vk_usbh_t *usbh;
    vk_usbh_dev_t *dev;
    vk_usbh_ifs_t *ifs;
    vk_usbh_dev_id_t id;
    vsf_eda_t eda;

    bool is_initing;

    union {
        struct {
            union {
                struct {
                    vk_usbh_bthci_iocb_t event_icb;
#if VSF_USBH_BTHCI_CFG_ACL_IN_NUM > 0
                    vk_usbh_bthci_iocb_t acl_icb[VSF_USBH_BTHCI_CFG_ACL_IN_NUM];
#endif
#if VSF_USBH_BTHCI_CFG_SCO_IN_NUM > 0
                    vk_usbh_bthci_iocb_t sco_icb[VSF_USBH_BTHCI_CFG_SCO_IN_NUM];
#endif
                };
                vk_usbh_bthci_iocb_t icb[1 + VSF_USBH_BTHCI_CFG_ACL_IN_NUM + VSF_USBH_BTHCI_CFG_SCO_IN_NUM];
            };

            union {
                struct {
                    vk_usbh_bthci_iocb_t cmd_ocb;
#if VSF_USBH_BTHCI_CFG_ACL_OUT_NUM > 0
                    vk_usbh_bthci_iocb_t acl_ocb[VSF_USBH_BTHCI_CFG_ACL_OUT_NUM];
#endif
#if VSF_USBH_BTHCI_CFG_SCO_OUT_NUM > 0
                    vk_usbh_bthci_iocb_t sco_ocb[VSF_USBH_BTHCI_CFG_SCO_OUT_NUM];
#endif
                };
                vk_usbh_bthci_iocb_t ocb[1 + VSF_USBH_BTHCI_CFG_ACL_OUT_NUM + VSF_USBH_BTHCI_CFG_SCO_OUT_NUM];
            };
        };
        vk_usbh_bthci_iocb_t iocb[
                        2       // event_icb, cmd_ocb
                    +   VSF_USBH_BTHCI_CFG_ACL_IN_NUM + VSF_USBH_BTHCI_CFG_SCO_IN_NUM
                    +   VSF_USBH_BTHCI_CFG_ACL_OUT_NUM + VSF_USBH_BTHCI_CFG_SCO_OUT_NUM];
    };
} vk_usbh_bthci_t;

/*============================ LOCAL VARIABLES ===============================*/

static const vk_usbh_dev_id_t __vk_usbh_bthci_dev_id[] = {
    { VSF_USBH_MATCH_DEV_CLASS(USB_CLASS_WIRELESS_CONTROLLER, 0x01, 0x01) },
    // BCM20702
    {
        .match_vendor       = true,
        .idVendor           = 0x0A5C,   // broadcom
        VSF_USBH_MATCH_DEV_CLASS(USB_CLASS_VENDOR_SPEC, 0x01, 0x01)
    },
};

/*============================ PROTOTYPES ====================================*/

static void * __vk_usbh_bthci_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs);
static void __vk_usbh_bthci_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbh_class_drv_t vk_usbh_bthci_drv = {
    .name       = "bthci_usb",
    .dev_id_num = dimof(__vk_usbh_bthci_dev_id),
    .dev_ids    = __vk_usbh_bthci_dev_id,
    .probe      = __vk_usbh_bthci_probe,
    .disconnect = __vk_usbh_bthci_disconnect,
};

/*============================ PROTOTYPES ====================================*/

extern void vsf_usbh_bthci_on_new(void *dev, vk_usbh_dev_id_t *id);
extern void vsf_usbh_bthci_on_del(void *dev);
extern void vsf_usbh_bthci_on_packet(void *dev, uint8_t type, uint8_t *packet, uint16_t size);

/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSF_USBH_BTHCI_ON_NEW
WEAK(vsf_usbh_bthci_on_new)
void vsf_usbh_bthci_on_new(void *dev, vk_usbh_dev_id_t *id) {}
#endif

#ifndef WEAK_VSF_USBH_BTHCI_ON_DEL
WEAK(vsf_usbh_bthci_on_del)
void vsf_usbh_bthci_on_del(void *dev) {}
#endif

#ifndef WEAK_VSF_USBH_BTHCI_ON_PACKET
WEAK(vsf_usbh_bthci_on_packet)
void vsf_usbh_bthci_on_packet(void *dev, uint8_t type, uint8_t *packet, uint16_t size) {}
#endif

static vk_usbh_bthci_iocb_t * __vk_usbh_bthci_get_iocb(vk_usbh_bthci_t *bthci, vk_usbh_hcd_urb_t *urb_hcd)
{
    vk_usbh_bthci_iocb_t *iocb = bthci->iocb;
    for (int i = 0; i < dimof(bthci->iocb); i++, iocb++) {
        if (iocb->urb.urb_hcd == urb_hcd) {
            return iocb;
        }
    }
    return NULL;
}

static vk_usbh_bthci_iocb_t * __vk_usbh_bthci_get_ocb(vk_usbh_bthci_t *bthci, uint8_t type)
{
    vk_usbh_bthci_iocb_t *ocb = bthci->ocb;
    for (int i = 0; i < dimof(bthci->ocb); i++, ocb++) {
        if ((ocb->type == type) && ocb->is_supported && !ocb->is_busy) {
            if ((ocb->type != BTHCI_PACKET_TYPE_CMD) && !vk_usbh_urb_is_alloced(&ocb->urb)) {
                vk_usbh_t *usbh = bthci->usbh;
                vk_usbh_dev_t *dev = bthci->dev;
                if (VSF_ERR_NONE != vk_usbh_alloc_urb(usbh, dev, &ocb->urb)) {
                    break;
                }
#if     VSF_USBH_BTHCI_CFG_URB_OUT_USE_LOCAL_BUFFER == ENABLED                  \
    &&  VSF_USBH_BTHCI_CFG_URB_OUT_USE_DYNAMIC_BUFFER != ENABLED
                if (NULL == vk_usbh_urb_alloc_buffer(&ocb->urb, VSF_USBH_BTHCI_CFG_URB_BUFSIZE)) {
                    vk_usbh_free_urb(usbh, &ocb->urb);
                    break;
                }
#endif
            }
            return ocb;
        }
    }
    return NULL;
}

static void __vk_usbh_bthci_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbh_bthci_t *bthci = container_of(eda, vk_usbh_bthci_t, eda);
    vk_usbh_dev_t *dev = bthci->dev;
    vk_usbh_t *usbh = bthci->usbh;
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
        err = vk_usbh_set_interface(usbh, dev, 1, 0);
        break;
    case VSF_EVT_MESSAGE:
        if (bthci->is_initing) {
            bthci->is_initing = false;
            __vsf_eda_crit_npb_leave(&dev->ep0.crit);

            err = vk_usbh_urb_get_status(&dev->ep0.urb);
            if (VSF_ERR_NONE != err) {
                break;
            }

            if (    (VSF_ERR_NONE != vk_usbh_alloc_urb(usbh, dev, &bthci->event_icb.urb))
                ||  (NULL == vk_usbh_urb_alloc_buffer(&bthci->event_icb.urb, VSF_USBH_BTHCI_CFG_URB_BUFSIZE))
                ||  (VSF_ERR_NONE != vk_usbh_submit_urb(usbh, &bthci->event_icb.urb))) {
                break;
            }

#if VSF_USBH_BTHCI_CFG_SCO_IN_NUM > 0
            for (int i = 0; i < VSF_USBH_BTHCI_CFG_SCO_IN_NUM; i++) {
                if (    bthci->sco_icb[i].is_supported
                    &&  (   (VSF_ERR_NONE != vk_usbh_alloc_urb(usbh, dev, &bthci->sco_icb[i].urb))
                        ||  (NULL == vk_usbh_urb_alloc_buffer(&bthci->sco_icb[i].urb, VSF_USBH_BTHCI_CFG_URB_BUFSIZE))
                        ||  (VSF_ERR_NONE != vk_usbh_submit_urb(usbh, &bthci->sco_icb[i].urb)))) {
                    // ignore errors and go on
                    vsf_trace_error("fail to prepare scoin_icb %d", i);
                    break;
                }
            }
#endif

#if VSF_USBH_BTHCI_CFG_ACL_IN_NUM > 0
            for (int i = 0; i < VSF_USBH_BTHCI_CFG_ACL_IN_NUM; i++) {
                if (    bthci->acl_icb[i].is_supported
                    &&  (   (VSF_ERR_NONE != vk_usbh_alloc_urb(usbh, dev, &bthci->acl_icb[i].urb))
                        ||  (NULL == vk_usbh_urb_alloc_buffer(&bthci->acl_icb[i].urb, VSF_USBH_BTHCI_CFG_URB_BUFSIZE))
                        ||  (VSF_ERR_NONE != vk_usbh_submit_urb(usbh, &bthci->acl_icb[i].urb)))) {
                    // ignore errors and go on
                    vsf_trace_error("fail to prepare aclin_icb %d", i);
                    break;
                }
            }
#endif

            vsf_usbh_bthci_on_new(bthci, &bthci->id);
        } else {
            vk_usbh_bthci_iocb_t *iocb = __vk_usbh_bthci_get_iocb(bthci, (vk_usbh_hcd_urb_t *)vsf_eda_get_cur_msg());
            vk_usbh_urb_t *urb;
            VSF_USB_ASSERT(iocb != NULL);
            urb = &iocb->urb;

            int_fast16_t status = vk_usbh_urb_get_status(urb);
            void * buffer = vk_usbh_urb_peek_buffer(urb);
            uint_fast32_t actual_length = vk_usbh_urb_get_actual_length(urb);

            if (iocb->is_icb) {
                if (URB_OK == status) {
                    vsf_usbh_bthci_on_packet(bthci, iocb->type | BTHCI_PACKET_TYPE_IN,
                        buffer, actual_length);
                }
                err = vk_usbh_submit_urb(usbh, urb);
            } else {
                if (iocb->is_ep0_claimed) {
                    iocb->is_ep0_claimed = false;
                    __vsf_eda_crit_npb_leave(&dev->ep0.crit);
                }
                iocb->is_busy = false;
                if (URB_OK == status) {
                    vsf_usbh_bthci_on_packet(bthci, iocb->type | BTHCI_PACKET_TYPE_OUT,
                        buffer, actual_length);
                }
            }
        }
        break;
    }

    if (err < 0) {
        vk_usbh_remove_interface(usbh, dev, bthci->ifs);
    }
}

static void __vk_usbh_bthci_free_all(vk_usbh_bthci_t *bthci)
{
    vk_usbh_t *usbh = bthci->usbh;
    vk_usbh_bthci_iocb_t *iocb = bthci->iocb;
    for (int i = 0; i < dimof(bthci->iocb); i++, iocb++) {
        if (iocb->urb.urb_hcd != bthci->dev->ep0.urb.urb_hcd) {
            vk_usbh_free_urb(usbh, &iocb->urb);
        }
    }
}

static void __vk_usbh_bthci_on_eda_terminate(vsf_eda_t *eda)
{
    vk_usbh_bthci_t *bthci = container_of(eda, vk_usbh_bthci_t, eda);
    vsf_usbh_free(bthci);
}

static void * __vk_usbh_bthci_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs)
{
    vk_usbh_ifs_t *ifs = parser_ifs->ifs;
    struct usb_interface_desc_t *desc_ifs = parser_ifs->parser_alt[ifs->cur_alt].desc_ifs;
    struct usb_endpoint_desc_t *desc_ep = parser_ifs->parser_alt[ifs->cur_alt].desc_ep;
    vk_usbh_bthci_t *bthci;
    uint_fast8_t epaddr;
    bool is_in;

    // only probe interface 0
    if (desc_ifs->bInterfaceNumber != 0) {
        return NULL;
    }

    bthci = vsf_usbh_malloc(sizeof(vk_usbh_bthci_t));
    if (NULL == bthci) {
        return NULL;
    }
    memset(bthci, 0, sizeof(vk_usbh_bthci_t));

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
                    vk_usbh_urb_prepare(&bthci->sco_icb[i].urb, dev, desc_ep);
                    bthci->sco_icb[i].type = BTHCI_PACKET_TYPE_SCO;
                    bthci->sco_icb[i].is_supported = true;
                    bthci->sco_icb[i].is_icb = true;
                }
            } else {
                for (int i = 0; i < VSF_USBH_BTHCI_CFG_SCO_OUT_NUM; i++) {
                    vk_usbh_urb_prepare(&bthci->sco_ocb[i].urb, dev, desc_ep);
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
                    vk_usbh_urb_prepare(&bthci->acl_icb[i].urb, dev, desc_ep);
                    bthci->acl_icb[i].type = BTHCI_PACKET_TYPE_ACL;
                    bthci->acl_icb[i].is_supported = true;
                    bthci->acl_icb[i].is_icb = true;
                }
            } else {
                for (int i = 0; i < VSF_USBH_BTHCI_CFG_SCO_OUT_NUM; i++) {
                    vk_usbh_urb_prepare(&bthci->acl_ocb[i].urb, dev, desc_ep);
                    bthci->acl_ocb[i].type = BTHCI_PACKET_TYPE_ACL;
                    bthci->acl_ocb[i].is_supported = true;
                    bthci->acl_ocb[i].is_icb = false;
                }
            }
            break;
#endif
        case USB_ENDPOINT_XFER_INT:
            if (is_in) {
                vk_usbh_urb_prepare(&bthci->event_icb.urb, dev, desc_ep);
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

    bthci->eda.fn.evthandler = __vk_usbh_bthci_evthandler;
    bthci->eda.on_terminate = __vk_usbh_bthci_on_eda_terminate;
    vsf_eda_init(&bthci->eda, vsf_prio_inherit, false);
    return bthci;

free_all:
    __vk_usbh_bthci_free_all(bthci);
    vsf_usbh_free(bthci);
    return NULL;
}

static void __vk_usbh_bthci_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param)
{
    vk_usbh_bthci_t *bthci = param;

    vsf_usbh_bthci_on_del(bthci);
    __vk_usbh_bthci_free_all(bthci);
    vsf_eda_fini(&bthci->eda);
}

bool vk_usbh_bthci_can_send(void *dev, uint8_t type)
{
    vk_usbh_bthci_t *bthci = dev;
    vk_usbh_bthci_iocb_t *ocb = __vk_usbh_bthci_get_ocb(bthci, type);
    return ocb != NULL;
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

vsf_err_t vk_usbh_bthci_send(void *dev, uint8_t type, uint8_t *packet, uint16_t size)
{
    vk_usbh_bthci_t *bthci = dev;
    vk_usbh_t *usbh = bthci->usbh;
    vk_usbh_bthci_iocb_t *ocb = __vk_usbh_bthci_get_ocb(bthci, type);
    vk_usbh_urb_t *urb;
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

            // allocate a new buffer in case original buffer is not aligned
            // TODO: only necessary if hcd does not support unaligned access
            uint8_t *buffer = vk_usbh_urb_alloc_buffer(&bthci->dev->ep0.urb, size);
            if (NULL == buffer) {
                VSF_USB_ASSERT(false);
                return VSF_ERR_NOT_ENOUGH_RESOURCES;
            }
            memcpy(buffer, packet, size);

//            vk_usbh_urb_set_buffer(urb, packet, size);
            return vk_usbh_control_msg_ex(usbh, bthci->dev, &req, 0, &bthci->eda);
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
    uint8_t *buffer = vk_usbh_urb_alloc_buffer(&ocb->urb, size);
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
    vk_usbh_urb_set_buffer(urb, packet, size);
#endif
    ocb->is_busy = true;
    return vk_usbh_submit_urb_ex(usbh, urb, flags, &bthci->eda);
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_warning=pe111
#endif


#endif      // VSF_USE_USB_HOST && VSF_USBH_USE_BTHCI
