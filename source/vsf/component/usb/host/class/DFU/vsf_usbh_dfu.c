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

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_DFU == ENABLED

// for vsf_eda_fini
#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_USBH_CLASS_IMPLEMENT_CLASS__
#include "../../vsf_usbh.h"
#include "./vsf_usbh_dfu.h"

#include "component/usb/common/class/DFU/vsf_usb_dfu.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vk_usbh_dfu_t {
    vsf_teda_t teda;

    vk_usbh_t *usbh;
    vk_usbh_dev_t *dev;
    vk_usbh_ifs_t *ifs;

    vsf_eda_t *eda_notify;
    uint8_t *buffer;
    uint16_t cur_size;
    uint16_t size;
    vsf_err_t err;
    uint8_t request;
    uint8_t is_zero_blocked     : 1;
    uint8_t is_to_get_status    : 1;

    uint8_t state;
    uint8_t attributes;
    uint16_t detach_timeout_ms;
    uint16_t transfer_size;
    uint16_t block_number;

    usb_dfu_status_t status;
} vk_usbh_dfu_t;

/*============================ LOCAL VARIABLES ===============================*/

static const vk_usbh_dev_id_t __vk_usbh_dfu_dev_id[] = {
    { VSF_USBH_MATCH_IFS_CLASS(0xFE, 0x01, 0x02) },
};

/*============================ PROTOTYPES ====================================*/

static void *__vk_usbh_dfu_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs);
static void __vk_usbh_dfu_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param);

extern void vsf_usbh_dfu_on_new(void *dfu);
extern void vsf_usbh_dfu_on_free(void *dfu);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbh_class_drv_t vk_usbh_dfu_drv = {
    .name       = "dfu",
    .dev_id_num = dimof(__vk_usbh_dfu_dev_id),
    .dev_ids    = __vk_usbh_dfu_dev_id,
    .probe      = __vk_usbh_dfu_probe,
    .disconnect = __vk_usbh_dfu_disconnect,
};

/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSF_USBH_DFU_ON_NEW
WEAK(vsf_usbh_dfu_on_new)
void vsf_usbh_dfu_on_new(void *dfu)
{
}
#endif

#ifndef WEAK_VSF_USBH_DFU_ON_NEW
WEAK(vsf_usbh_dfu_on_free)
void vsf_usbh_dfu_on_free(void *dfu)
{
}
#endif

static vsf_err_t __vk_usbh_dfu_detach(vk_usbh_dfu_t *dfu, uint_fast16_t timeout)
{
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_OUT,
        .bRequest        =  USB_DFUREQ_DETACH,
        .wValue          =  timeout,
        .wIndex          =  dfu->ifs->no,
        .wLength         =  0,
    };
    return vk_usbh_control_msg(dfu->usbh, dfu->dev, &req);
}

static vsf_err_t __vk_usbh_dfu_download(vk_usbh_dfu_t *dfu, uint8_t *buffer, uint_fast16_t size)
{
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_OUT,
        .bRequest        =  USB_DFUREQ_DNLOAD,
        .wValue          =  dfu->block_number++,
        .wIndex          =  dfu->ifs->no,
        .wLength         =  size,
    };
    vk_usbh_urb_set_buffer(&dfu->dev->ep0.urb, buffer, size);
    return vk_usbh_control_msg(dfu->usbh, dfu->dev, &req);
}

static vsf_err_t __vk_usbh_dfu_upload(vk_usbh_dfu_t *dfu, uint8_t *buffer, uint_fast16_t size)
{
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_IN,
        .bRequest        =  USB_DFUREQ_UPLOAD,
        .wValue          =  dfu->block_number++,
        .wIndex          =  dfu->ifs->no,
        .wLength         =  size,
    };
    vk_usbh_urb_set_buffer(&dfu->dev->ep0.urb, buffer, size);
    return vk_usbh_control_msg(dfu->usbh, dfu->dev, &req);
}

static vsf_err_t __vk_usbh_dfu_get_status(vk_usbh_dfu_t *dfu)
{
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_IN,
        .bRequest        =  USB_DFUREQ_GETSTATUS,
        .wValue          =  0,
        .wIndex          =  dfu->ifs->no,
        .wLength         =  sizeof(dfu->status),
    };
    vk_usbh_urb_set_buffer(&dfu->dev->ep0.urb, &dfu->status, sizeof(dfu->status));
    return vk_usbh_control_msg(dfu->usbh, dfu->dev, &req);
}

static vsf_err_t __vk_usbh_dfu_clear_status(vk_usbh_dfu_t *dfu)
{
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_OUT,
        .bRequest        =  USB_DFUREQ_CLRSTATUS,
        .wValue          =  0,
        .wIndex          =  dfu->ifs->no,
        .wLength         =  0,
    };
    return vk_usbh_control_msg(dfu->usbh, dfu->dev, &req);
}

static vsf_err_t __vk_usbh_dfu_get_state(vk_usbh_dfu_t *dfu)
{
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_IN,
        .bRequest        =  USB_DFUREQ_GETSTATE,
        .wValue          =  0,
        .wIndex          =  dfu->ifs->no,
        .wLength         =  1,
    };
    vk_usbh_urb_set_buffer(&dfu->dev->ep0.urb, &dfu->state, 1);
    return vk_usbh_control_msg(dfu->usbh, dfu->dev, &req);
}

static vsf_err_t __vk_usbh_dfu_abort(vk_usbh_dfu_t *dfu)
{
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_OUT,
        .bRequest        =  USB_DFUREQ_ABORT,
        .wValue          =  0,
        .wIndex          =  dfu->ifs->no,
        .wLength         =  0,
    };
    return vk_usbh_control_msg(dfu->usbh, dfu->dev, &req);
}

static void __vk_usbh_dfu_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbh_dfu_t *dfu = container_of(eda, vk_usbh_dfu_t, teda);
    vsf_err_t err;

    switch (evt) {
    case VSF_EVT_MESSAGE:{
            vk_usbh_urb_t urb = { .urb_hcd = vsf_eda_get_cur_msg() };
            if (URB_OK != vk_usbh_urb_get_status(&urb)) {
            do_failure:
                dfu->err = VSF_ERR_FAIL;

            do_return:
                __vsf_eda_crit_npb_leave(&dfu->dev->ep0.crit);
                vsf_eda_post_evt(dfu->eda_notify, VSF_EVT_RETURN);
                break;
            }

            if (dfu->is_to_get_status) {
                dfu->size -= dfu->cur_size;
                dfu->is_to_get_status = false;
                __vk_usbh_dfu_get_status(dfu);
                break;
            } else if (dfu->status.bState != 0x00) {
                goto do_failure;
            }
        }
        // fall through
    case VSF_EVT_USER:
        if ((dfu->size > 0) || !dfu->is_zero_blocked) {
            dfu->cur_size = min(dfu->transfer_size, dfu->size);

            switch (dfu->request) {
            case USB_DFUREQ_DNLOAD:
                err = __vk_usbh_dfu_download(dfu, dfu->buffer, dfu->cur_size);
                break;
            case USB_DFUREQ_UPLOAD:
                // TODO: not supported now
                VSF_USB_ASSERT(false);
                err = __vk_usbh_dfu_upload(dfu, dfu->buffer, dfu->cur_size);
                break;
            default:
                dfu->err = VSF_ERR_INVALID_PARAMETER;
                goto do_return;
            }

            dfu->err = err;
            dfu->is_to_get_status = true;
            if (0 == dfu->cur_size) {
                dfu->is_zero_blocked = true;
            }
            if (VSF_ERR_NONE != err) {
                goto do_return;
            }
        }
        break;
    }
}

vsf_err_t vk_usbh_dfu_get_result(void *dfu_ptr)
{
    return ((vk_usbh_dfu_t *)dfu_ptr)->err;
}

vsf_err_t __vk_usbh_dfu_operate(void *dfu_ptr, uint8_t *buffer, uint_fast32_t size, uint8_t request)
{
    vk_usbh_dfu_t *dfu = dfu_ptr;
    vk_usbh_dev_t *dev = dfu->dev;

    if (VSF_ERR_NONE != __vsf_eda_crit_npb_try_to_enter(&dev->ep0.crit, 0)) {
        return VSF_ERR_NOT_AVAILABLE;
    }

    dfu->eda_notify = vsf_eda_get_cur();
    dfu->request = request;
    dfu->buffer = buffer;
    dfu->size = size;
    dfu->cur_size = 0;
    dfu->is_zero_blocked = false;
    return vsf_eda_post_evt(&dfu->teda.use_as__vsf_eda_t, VSF_EVT_USER);
}

static void __vk_usbh_dfu_on_eda_terminate(vsf_eda_t *eda)
{
    vk_usbh_dfu_t *dfu = container_of(eda, vk_usbh_dfu_t, teda);
    vsf_usbh_free(dfu);
}

static void __vk_usbh_dfu_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param)
{
    vk_usbh_dfu_t *dfu = param;

    vsf_usbh_dfu_on_free(dfu);
    vsf_eda_fini(&dfu->teda.use_as__vsf_eda_t);
}

static void *__vk_usbh_dfu_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs)
{
    vk_usbh_ifs_t *ifs = parser_ifs->ifs;
    vk_usbh_ifs_alt_parser_t *parser_alt = &parser_ifs->parser_alt[ifs->cur_alt];
    struct usb_interface_desc_t *desc_ifs = parser_alt->desc_ifs;
    usb_dfu_functional_descriptor_t *functional_desc;
    vk_usbh_dfu_t *dfu;

    if (VSF_ERR_NONE != vk_usbh_get_extra_descriptor((uint8_t *)desc_ifs, parser_alt->desc_size,
                            USB_DFU_FUNCTIONAL_DESC, (void **)&functional_desc)) {
        return NULL;
    }

    dfu = vsf_usbh_malloc(sizeof(vk_usbh_dfu_t));
    if (dfu != NULL) {
        memset(dfu, 0, sizeof(*dfu));

        dfu->attributes = functional_desc->bmAttributes;
        dfu->detach_timeout_ms = functional_desc->wDetachTimeOut;
        dfu->transfer_size = functional_desc->wTransferSize;

        dfu->usbh = usbh;
        dfu->dev = dev;
        dfu->ifs = parser_ifs->ifs;

        dfu->teda.fn.evthandler = __vk_usbh_dfu_evthandler;
        dfu->teda.on_terminate = __vk_usbh_dfu_on_eda_terminate;
        if (VSF_ERR_NONE == vsf_teda_init(&dfu->teda, vsf_prio_inherit, false)) {
            vsf_usbh_dfu_on_new(dfu);
        }
    }
    return dfu;
}

#endif
