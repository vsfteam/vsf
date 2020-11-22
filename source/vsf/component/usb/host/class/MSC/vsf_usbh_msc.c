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

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_MSC == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_USBH_CLASS_IMPLEMENT_CLASS__
#define __VSF_SCSI_CLASS_INHERIT__

#include "kernel/vsf_kernel.h"
#include "../../vsf_usbh.h"
#include "./vsf_usbh_msc.h"
#include "component/scsi/vsf_scsi.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE != ENABLED
#   error "VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE is required"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vk_usbh_msc_t {
    vk_usbh_t *usbh;
    vk_usbh_dev_t *dev;
    vk_usbh_ifs_t *ifs;

    vk_usbh_urb_t urb_in;
    vk_usbh_urb_t urb_out;

    union {
        usb_msc_cbw_t cbw;
        usb_msc_csw_t csw;
    } buffer;

#if VSF_USE_SCSI == ENABLED
    vk_scsi_t scsi;
#endif

    vsf_eda_t eda;
    uint64_t addr;
    uint32_t total_size;
    uint32_t remain_size;
    uint8_t max_lun;
    enum {
        VSF_USBH_MSC_STATE_COMMAND,
        VSF_USBH_MSC_STATE_DATA,
        VSF_USBH_MSC_STATE_REPLY,
    } state;
} vk_usbh_msc_t;

/*============================ PROTOTYPES ====================================*/

static void * __vk_usbh_msc_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs);
static void __vk_usbh_msc_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param);

dcl_vsf_peda_methods(static, __vk_usbh_msc_scsi_init)
dcl_vsf_peda_methods(static, __vk_usbh_msc_scsi_fini)
dcl_vsf_peda_methods(static, __vk_usbh_msc_scsi_execute)
#if VSF_USE_SIMPLE_STREAM == ENABLED
dcl_vsf_peda_methods(static, __vk_usbh_msc_scsi_execute_stream)
#endif

extern void vsf_scsi_on_new(vk_scsi_t *scsi);
extern void vsf_scsi_on_delete(vk_scsi_t *scsi);

/*============================ LOCAL VARIABLES ===============================*/

static const vk_usbh_dev_id_t __vk_usbh_msc_dev_id[] = {
    { VSF_USBH_MATCH_IFS_CLASS(USB_CLASS_MASS_STORAGE, 6, 0x50) },
};

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

const vk_scsi_drv_t __vk_usbh_msc_scsi_drv = {
    .init           = (vsf_peda_evthandler_t)vsf_peda_func(__vk_usbh_msc_scsi_init),
    .fini           = (vsf_peda_evthandler_t)vsf_peda_func(__vk_usbh_msc_scsi_fini),
    .buffer         = NULL,
    .execute        = (vsf_peda_evthandler_t)vsf_peda_func(__vk_usbh_msc_scsi_execute),
#if VSF_USE_SIMPLE_STREAM == ENABLED
    .execute_stream = (vsf_peda_evthandler_t)vsf_peda_func(__vk_usbh_msc_scsi_execute_stream),
#endif
};

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbh_class_drv_t vk_usbh_msc_drv = {
    .name       = "msc",
    .dev_id_num = dimof(__vk_usbh_msc_dev_id),
    .dev_ids    = __vk_usbh_msc_dev_id,
    .probe      = __vk_usbh_msc_probe,
    .disconnect = __vk_usbh_msc_disconnect,
};

/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSF_SCSI_ON_NEW
WEAK(vsf_scsi_on_new)
void vsf_scsi_on_new(vk_scsi_t *scsi)
{
}
#endif

#ifndef WEAK_VSF_SCSI_ON_DELETE
WEAK(vsf_scsi_on_delete)
void vsf_scsi_on_delete(vk_scsi_t *scsi)
{
}
#endif

__vsf_component_peda_ifs_entry(__vk_usbh_msc_scsi_init, vk_scsi_init)
{
    vsf_peda_begin();
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_usbh_msc_scsi_fini, vk_scsi_fini)
{
    vsf_peda_begin();
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

#if __IS_COMPILER_IAR__
//! transfer of control bypasses initialization of variables
//! integer conversion resulted in a change of sign
#   pragma diag_suppress=pe546,pe068
#endif

static void __vk_usbh_msc_scsi_execute_do(vk_usbh_msc_t *msc, vsf_evt_t evt, uint8_t *cbd, bool is_stream, void *mem_stream)
{
    switch (evt) {
    case VSF_EVT_INIT: {
            // TODO: add mutex for protection
            uint_fast8_t cbd_len = vk_scsi_get_command_len(cbd);
            scsi_cmd_code_t cmd_code = (scsi_cmd_code_t)(cbd[0] & 0x1F);
            bool is_rw = vk_scsi_get_rw_param(cbd, &msc->addr, &msc->total_size);

            msc->state = VSF_USBH_MSC_STATE_COMMAND;
            memset(&msc->buffer.cbw, 0, sizeof(msc->buffer.cbw));
            msc->buffer.cbw.dCBWSignature = cpu_to_le32(USB_MSC_CBW_SIGNATURE);
            msc->buffer.cbw.dCBWTag = cpu_to_le32(0x5A5A5A5A);
            if (is_stream) {
                VSF_USB_ASSERT(is_rw);
            } else {
                msc->total_size = ((vsf_mem_t *)mem_stream)->size;
            }
            msc->remain_size = msc->total_size;
            msc->buffer.cbw.dCBWDataTransferLength = cpu_to_le32(msc->total_size);
            msc->buffer.cbw.bmCBWFlags = (SCSI_CMDCODE_WRITE == cmd_code) ? 0x00 : 0x80;
            msc->buffer.cbw.bCBWLUN = 0;
            msc->buffer.cbw.bCBWCBLength = cbd_len;
            memcpy(msc->buffer.cbw.CBWCB, cbd, cbd_len);

            vk_usbh_urb_set_buffer(&msc->urb_out, &msc->buffer.cbw, sizeof(msc->buffer.cbw));
            vk_usbh_submit_urb(msc->usbh, &msc->urb_out);
        }
        break;
    case VSF_EVT_MESSAGE: {
            vk_usbh_urb_t urb = { .urb_hcd = vsf_eda_get_cur_msg() };
            if (URB_OK != vk_usbh_urb_get_status(&urb)) {
                vsf_eda_return(VSF_ERR_FAIL);
                break;
            }

            switch (msc->state) {
            case VSF_USBH_MSC_STATE_COMMAND:
                if (!msc->remain_size) {
                    goto reply_stage;
                }

                msc->state = VSF_USBH_MSC_STATE_DATA;
                if (is_stream) {
                    // TODO: add stream support
                    VSF_USB_ASSERT(false);
                } else {
                    if (((vsf_mem_t *)mem_stream)->size > 0) {
                        vk_usbh_urb_t * urb = (msc->buffer.cbw.bmCBWFlags & 0x80) ? &msc->urb_in : &msc->urb_out;
                        vk_usbh_urb_set_buffer(urb, ((vsf_mem_t *)mem_stream)->buffer, ((vsf_mem_t *)mem_stream)->size);
                        vk_usbh_submit_urb(msc->usbh, urb);
                    } else {
                        goto reply_stage;
                    }
                }
                break;
            case VSF_USBH_MSC_STATE_DATA: {
                    uint_fast32_t actual_length = vk_usbh_urb_get_actual_length(&urb);
                    vk_usbh_pipe_t pipe = vk_usbh_urb_get_pipe(&urb);

                    VSF_USB_ASSERT(msc->remain_size >= actual_length);
                    msc->remain_size -= vk_usbh_urb_get_actual_length(&urb);
                    actual_length %= pipe.size;
                    if (!msc->remain_size || ((actual_length > 0) && (actual_length < pipe.size))) {
                    reply_stage:
                        msc->state = VSF_USBH_MSC_STATE_REPLY;
                        vk_usbh_urb_set_buffer(&msc->urb_in, &msc->buffer.csw, sizeof(msc->buffer.csw));
                        vk_usbh_submit_urb(msc->usbh, &msc->urb_in);
                        break;
                    }
                }

                if (is_stream) {
                    // TODO: add stream support
                    VSF_USB_ASSERT(false);
                } else {
                    // non-stream mode, should not reach here
                    VSF_USB_ASSERT(false);
                }
                break;
            case VSF_USBH_MSC_STATE_REPLY:
                vsf_eda_return(msc->buffer.csw.dCSWStatus == 0 ? msc->total_size - msc->remain_size : VSF_ERR_FAIL);
                break;
            }
        }
        break;
    }
}

#if __IS_COMPILER_IAR__
//! transfer of control bypasses initialization of variables
//! integer conversion resulted in a change of sign
#   pragma diag_warning=pe546,pe068
#endif

__vsf_component_peda_ifs_entry(__vk_usbh_msc_scsi_execute, vk_scsi_execute)
{
    vsf_peda_begin();
    vk_usbh_msc_t *msc = container_of(&vsf_this, vk_usbh_msc_t, scsi);
    __vk_usbh_msc_scsi_execute_do(msc, evt, vsf_local.cbd, false, &vsf_local.mem);
    vsf_peda_end();
}

#if VSF_USE_SIMPLE_STREAM == ENABLED
__vsf_component_peda_ifs_entry(__vk_usbh_msc_scsi_execute_stream, vk_scsi_execute_stream)
{
    vsf_peda_begin();
    VSF_USB_ASSERT(false);
    // not supported yet
//    vk_usbh_msc_t *msc = container_of(&vsf_this, vk_usbh_msc_t, scsi);
//    __vk_usbh_msc_scsi_execute_do(msc, evt, vsf_local.cbd, true, &vsf_local.stream);
    vsf_peda_end();
}
#endif

static void __vk_usbh_msc_free_urb(vk_usbh_msc_t *msc)
{
    vk_usbh_t *usbh = msc->usbh;
    vk_usbh_free_urb(usbh, &msc->urb_in);
    vk_usbh_free_urb(usbh, &msc->urb_out);
}

static vsf_err_t __vk_usbh_msc_get_max_lun(vk_usbh_msc_t *msc)
{
    VSF_USB_ASSERT((msc != NULL) && (msc->usbh != NULL) && (msc->dev != NULL) && (msc->ifs != NULL));
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_IN,
        .bRequest        =  USB_MSC_REQ_GET_MAX_LUN,
        .wValue          =  0,
        .wIndex          =  msc->ifs->no,
        .wLength         =  1,
    };
    vk_usbh_urb_set_buffer(&msc->dev->ep0.urb, &msc->max_lun, 1);
    return vk_usbh_control_msg(msc->usbh, msc->dev, &req);
}

static void __vk_usbh_msc_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbh_msc_t *msc = container_of(eda, vk_usbh_msc_t, eda);
    vk_usbh_dev_t *dev = msc->dev;

    switch (evt) {
    case VSF_EVT_INIT:
        if (VSF_ERR_NONE != __vsf_eda_crit_npb_enter(&dev->ep0.crit)) {
            break;
        }
        // fall through
    case VSF_EVT_SYNC:
        if (VSF_ERR_NONE != __vk_usbh_msc_get_max_lun(msc)) {
            vk_usbh_remove_interface(msc->usbh, dev, msc->ifs);
            return;
        }
        break;
    case VSF_EVT_MESSAGE:
        __vsf_eda_crit_npb_leave(&dev->ep0.crit);
        vsf_scsi_on_new(&msc->scsi);
        break;
    }
}

static void __vk_usbh_msc_on_eda_terminate(vsf_eda_t *eda)
{
    vk_usbh_msc_t *msc = container_of(eda, vk_usbh_msc_t, eda);
    vsf_usbh_free(msc);
}

static void * __vk_usbh_msc_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs)
{
    vk_usbh_ifs_t *ifs = parser_ifs->ifs;
    vk_usbh_ifs_alt_parser_t *parser_alt = &parser_ifs->parser_alt[ifs->cur_alt];
    struct usb_interface_desc_t *desc_ifs = parser_alt->desc_ifs;
    struct usb_endpoint_desc_t *desc_ep = parser_alt->desc_ep;
    uint_fast8_t epaddr;
    vk_usbh_msc_t *msc;

    if (desc_ifs->bNumEndpoints != 2) { return NULL; }
    msc = vsf_usbh_malloc(sizeof(vk_usbh_msc_t));
    if (NULL == msc) { return NULL; }
    memset(msc, 0, sizeof(*msc));

    msc->usbh = usbh;
    msc->dev = dev;
    msc->ifs = ifs;

    for (int i = 0; i < desc_ifs->bNumEndpoints; i++) {
        if (    (NULL == desc_ep)
            ||  (desc_ep->bLength != USB_DT_ENDPOINT_SIZE)
            ||  (desc_ep->bmAttributes != USB_ENDPOINT_XFER_BULK)) {
            goto free_all;
        }
        epaddr = desc_ep->bEndpointAddress;
        if ((epaddr & USB_DIR_MASK) == USB_DIR_IN) {
            vk_usbh_urb_prepare(&msc->urb_in, dev, desc_ep);
            vk_usbh_alloc_urb(usbh, dev, &msc->urb_in);
        } else {
            vk_usbh_urb_prepare(&msc->urb_out, dev, desc_ep);
            vk_usbh_alloc_urb(usbh, dev, &msc->urb_out);
        }

        desc_ep = vk_usbh_get_next_ep_descriptor(desc_ep,
            parser_alt->desc_size - ((uintptr_t)desc_ep - (uintptr_t)desc_ifs));
    }

    msc->scsi.drv = &__vk_usbh_msc_scsi_drv;
    msc->eda.fn.evthandler = __vk_usbh_msc_evthandler;
    msc->eda.on_terminate = __vk_usbh_msc_on_eda_terminate;
    vsf_eda_init(&msc->eda, vsf_prio_inherit, false);
    return msc;

free_all:
    __vk_usbh_msc_free_urb(msc);
    vsf_usbh_free(msc);
    return NULL;
}

static void __vk_usbh_msc_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param)
{
    vk_usbh_msc_t *msc = (vk_usbh_msc_t *)param;
    vsf_scsi_on_delete(&msc->scsi);
    __vk_usbh_msc_free_urb(msc);
}

#endif
