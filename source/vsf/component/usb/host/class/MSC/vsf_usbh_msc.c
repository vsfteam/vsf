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

#if VSF_USE_USB_HOST == ENABLED && VSF_USE_USB_HOST_MSC == ENABLED

#define VSF_EDA_CLASS_INHERIT
#define VSF_USBH_IMPLEMENT_CLASS
#define VSF_USBH_MSC_IMPLEMENT
#define VSF_SCSI_INHERIT
#include "vsf.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE != ENABLED
#   error "VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE is required"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct vk_usbh_msc_t {
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

    vsf_eda_t *eda;
    uint64_t addr;
    uint32_t size;
    enum {
        VSF_USBH_MSC_STATE_COMMAND,
        VSF_USBH_MSC_STATE_DATA,
        VSF_USBH_MSC_STATE_REPLY,
    } state;
};
typedef struct vk_usbh_msc_t vk_usbh_msc_t;

/*============================ PROTOTYPES ====================================*/

static void * __vk_usbh_msc_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs);
static void __vk_usbh_msc_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param);

static void __vk_usbh_msc_scsi_init(uintptr_t target, vsf_evt_t evt);
static void __vk_usbh_msc_scsi_fini(uintptr_t target, vsf_evt_t evt);
static void __vk_usbh_msc_scsi_execute(uintptr_t target, vsf_evt_t evt);
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
static void __vk_usbh_msc_scsi_execute_stream(uintptr_t target, vsf_evt_t evt);
#endif

extern void vsf_scsi_on_new(vk_scsi_t *scsi);
extern void vsf_scsi_on_delete(vk_scsi_t *scsi);

/*============================ LOCAL VARIABLES ===============================*/

static const vk_usbh_dev_id_t __vk_usbh_msc_dev_id[] = {
    { VSF_USBH_MATCH_IFS_CLASS(USB_CLASS_MASS_STORAGE, 6, 0x50) },
};

const i_scsi_drv_t __vk_usbh_msc_scsi_drv = {
    .init           = __vk_usbh_msc_scsi_init,
    .fini           = __vk_usbh_msc_scsi_fini,
    .buffer         = NULL,
    .execute        = __vk_usbh_msc_scsi_execute,
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
    .execute_stream = __vk_usbh_msc_scsi_execute_stream,
#endif
};
typedef struct i_scsi_drv_t i_scsi_drv_t;

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

static void __vk_usbh_msc_scsi_init(uintptr_t target, vsf_evt_t evt)
{
    vk_scsi_return((vk_scsi_t *)target, VSF_ERR_NONE);
}

static void __vk_usbh_msc_scsi_fini(uintptr_t target, vsf_evt_t evt)
{
    vk_scsi_return((vk_scsi_t *)target, VSF_ERR_NONE);
}

static void __vk_usbh_msc_scsi_execute_do(uintptr_t target, vsf_evt_t evt, bool is_stream)
{
    vk_usbh_msc_t *msc = container_of(target, vk_usbh_msc_t, scsi);
    vsf_mem_t *mem = &msc->scsi.args.mem;
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
    vsf_stream_t *stream = msc->scsi.args.stream;
#endif

    switch (evt) {
    case VSF_EVT_INIT: {
            // TODO: add mutex for protection
            uint8_t *cbd = msc->scsi.args.cbd;
            uint_fast8_t cbd_len = vk_scsi_get_command_len(cbd);
            scsi_cmd_code_t cmd_code = (scsi_cmd_code_t)(cbd[0] & 0x1F);
            bool is_rw = vk_scsi_get_rw_param(cbd, &msc->addr, &msc->size);

            msc->state = VSF_USBH_MSC_STATE_COMMAND;
            memset(&msc->buffer.cbw, 0, sizeof(msc->buffer.cbw));
            msc->buffer.cbw.dCBWSignature = cpu_to_le32(USB_MSC_CBW_SIGNATURE);
            msc->buffer.cbw.dCBWTag = cpu_to_le32(0x5A5A5A5A);
            if (is_stream) {
                VSF_USB_ASSERT(is_rw);
            } else {
                msc->size = mem->nSize;
            }
            msc->scsi.result.reply_len = msc->size;
            msc->buffer.cbw.dCBWDataTransferLength = cpu_to_le32(msc->size);
            msc->buffer.cbw.bmCBWFlags = (SCSI_CMDCODE_WRITE == cmd_code) ? 0x00 : 0x80;
            msc->buffer.cbw.bCBWLUN = 0;
            msc->buffer.cbw.bCBWCBLength = cbd_len;
            memcpy(msc->buffer.cbw.CBWCB, msc->scsi.args.cbd, cbd_len);

            vk_usbh_urb_set_buffer(&msc->urb_out, &msc->buffer.cbw, sizeof(msc->buffer.cbw));
            vk_usbh_submit_urb(msc->usbh, &msc->urb_out);
        }
        break;
    case VSF_EVT_MESSAGE: {
            vk_usbh_urb_t urb = { .urb_hcd = vsf_eda_get_cur_msg() };
            if (URB_OK != vk_usbh_urb_get_status(&urb)) {
                vk_scsi_return(&msc->scsi, VSF_ERR_FAIL);
                break;
            }

            switch (msc->state) {
            case VSF_USBH_MSC_STATE_COMMAND:
                if (!msc->size) {
                    goto reply_stage;
                }

                msc->state = VSF_USBH_MSC_STATE_DATA;
                if (is_stream) {
                    // TODO: add stream support
                    VSF_USB_ASSERT(false);
                } else {
                    if (msc->scsi.args.mem.nSize > 0) {
                        vk_usbh_urb_t * urb = (msc->buffer.cbw.bmCBWFlags & 0x80) ? &msc->urb_in : &msc->urb_out;
                        vk_usbh_urb_set_buffer(urb, msc->scsi.args.mem.pchBuffer, msc->scsi.args.mem.nSize);
                        vk_usbh_submit_urb(msc->usbh, urb);
                    } else {
                        goto reply_stage;
                    }
                }
                break;
            case VSF_USBH_MSC_STATE_DATA: {
                    uint_fast32_t actual_length = vk_usbh_urb_get_actual_length(&urb);
                    vk_usbh_pipe_t pipe = vk_usbh_urb_get_pipe(&urb);

                    VSF_USB_ASSERT(msc->size >= actual_length);
                    msc->size -= vk_usbh_urb_get_actual_length(&urb);
                    actual_length %= pipe.size;
                    if (!msc->size || ((actual_length > 0) && (actual_length < pipe.size))) {
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
                msc->scsi.result.reply_len -= msc->buffer.csw.dCSWDataResidue;
                vk_scsi_return((vk_scsi_t *)target, msc->buffer.csw.dCSWStatus == 0 ? VSF_ERR_NONE : VSF_ERR_FAIL);
                break;
            }
        }
        break;
    }
}

static void __vk_usbh_msc_scsi_execute(uintptr_t target, vsf_evt_t evt)
{
    __vk_usbh_msc_scsi_execute_do(target, evt, false);
}

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
static void __vk_usbh_msc_scsi_execute_stream(uintptr_t target, vsf_evt_t evt)
{
    VSF_USB_ASSERT(false);
    // not supported yet
//    __vk_usbh_msc_scsi_execute_do(target, evt, true);
}
#endif

static void __vk_usbh_msc_free_urb(vk_usbh_msc_t *msc)
{
    vk_usbh_t *usbh = msc->usbh;
    vk_usbh_free_urb(usbh, &msc->urb_in);
    vk_usbh_free_urb(usbh, &msc->urb_out);
}

static void __vk_usbh_msc_on_eda_terminate(vsf_eda_t *eda)
{
    vk_usbh_msc_t *msc = container_of(eda, vk_usbh_msc_t, eda);
    VSF_USBH_FREE(msc);
}

static void * __vk_usbh_msc_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs)
{
    vk_usbh_ifs_t *ifs = parser_ifs->ifs;
    vk_usbh_ifs_alt_parser_t *parser_alt = &parser_ifs->parser_alt[ifs->cur_alt];
    struct usb_interface_desc_t *desc_ifs = parser_alt->desc_ifs;
    struct usb_endpoint_desc_t *desc_ep = parser_ifs->parser_alt[ifs->cur_alt].desc_ep;
    uint_fast8_t epaddr;
    vk_usbh_msc_t *msc;

    if (desc_ifs->bNumEndpoints != 2) { return NULL; }
    msc = VSF_USBH_MALLOC(sizeof(vk_usbh_msc_t));
    if (NULL == msc) { return NULL; }
    memset(msc, 0, sizeof(*msc));

    for (int i = 0; i < desc_ifs->bNumEndpoints; i++) {
        if (    (desc_ep->bLength != USB_DT_ENDPOINT_SIZE)
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
        desc_ep = (struct usb_endpoint_desc_t *)((uintptr_t)desc_ep + USB_DT_ENDPOINT_SIZE);
    }

    msc->usbh = usbh;
    msc->dev = dev;
    msc->ifs = ifs;

    msc->scsi.drv = &__vk_usbh_msc_scsi_drv;
    vsf_scsi_on_new(&msc->scsi);
    return msc;

free_all:
    __vk_usbh_msc_free_urb(msc);
    VSF_USBH_FREE(msc);
    return NULL;
}

static void __vk_usbh_msc_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param)
{
    vk_usbh_msc_t *msc = (vk_usbh_msc_t *)param;
    vsf_scsi_on_delete(&msc->scsi);
    __vk_usbh_msc_free_urb(msc);
}

#endif
