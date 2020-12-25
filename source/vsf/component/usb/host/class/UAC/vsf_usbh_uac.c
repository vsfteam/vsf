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

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_UAC == ENABLED

#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_USBH_CLASS_IMPLEMENT_CLASS__
#define __VSF_USBH_UAC_CLASS_IMPLEMENT

#include "kernel/vsf_kernel.h"
#include "../../vsf_usbh.h"
#include "./vsf_usbh_uac.h"
// for USB_EP_TYPE_ISO
#include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE != ENABLED
#   error "VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE is required"
#endif

#if VSF_USE_SIMPLE_STREAM != ENABLED
#   error "VSF_USE_SIMPLE_STREAM is required"
#endif

#ifndef VSF_USBD_UAC_CFG_STREAM_NUM
//  default stream number is 2, one for IN and one for OUT
#   define VSF_USBD_UAC_CFG_STREAM_NUM          2
#endif

#ifndef VSF_USBH_UAC_CFG_URB_WITH_BUFFER
//  if enabled, urb will have dedicated buffer
#   define VSF_USBH_UAC_CFG_URB_WITH_BUFFER     ENABLED
#endif

#define vsf_usbh_uac_task_t                     vsf_eda_t

#if VSF_USBH_UAC_CFG_URB_NUM_PER_STREAM > 1
#   if VSF_USBH_UAC_CFG_URB_WITH_BUFFER != ENABLED
#       warning VSF_USBH_UAC_CFG_URB_WITH_BUFFER MUST be enabled for multiple urb per stream
#       undef VSF_USBH_UAC_CFG_URB_WITH_BUFFER
#       define VSF_USBH_UAC_CFG_URB_WITH_BUFFER ENABLED
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    USB_AUDIO_SUBCLASS_AUDIOCONTROL     = 0x01,
    USB_AUDIO_SUBCLASS_AUDIOSTREAMING   = 0x02,
    USB_AUDIO_SUBCLASS_MINISTREAMING    = 0x03,
};

enum {
    VSF_USBH_UAC_SUBEVT_MASK            = 0x300,
    VSF_USBH_UAC_SUBEVT_CONNECT         = 0x000,
    VSF_USBH_UAC_SUBEVT_DISCONNECT      = 0x100,
    VSF_USBH_UAC_SUBEVT_STREAM          = 0x200,
    VSF_USBH_UAC_SUBEVT_SUBMIT_REQ      = 0x300,

    VSF_USBH_UAC_EVT_CONNECT            = VSF_EVT_USER + VSF_USBH_UAC_SUBEVT_CONNECT,
    VSF_USBH_UAC_EVT_DISCONNECT         = VSF_EVT_USER + VSF_USBH_UAC_SUBEVT_DISCONNECT,
    VSF_USBH_UAC_EVT_STREAM             = VSF_EVT_USER + VSF_USBH_UAC_SUBEVT_STREAM,
    VSF_USBH_UAC_EVT_SUBMIT_REQ         = VSF_EVT_USER + VSF_USBH_UAC_SUBEVT_SUBMIT_REQ,
};

typedef struct vk_usbh_uac_t {
    vk_usbh_t *usbh;
    vk_usbh_dev_t *dev;
    vk_usbh_ifs_t *ifs;

    uint8_t stream_cur;
    uint8_t stream_num;
    uint8_t is_ep0_busy         : 1;
    uint8_t is_req_pending      : 1;
    uint8_t is_cur_req          : 1;

    struct usb_ctrlrequest_t req;
    void *req_data;

    vsf_usbh_uac_task_t task;
    vk_usbh_uac_stream_t streams[VSF_USBD_UAC_CFG_STREAM_NUM];
    union {
        usb_uac_ac_interface_header_desc_t ac_header;
        // seems IAR does not support zero-length array
        uint8_t ac_interface_desc[1];
    };
} vk_usbh_uac_t;

/*============================ PROTOTYPES ====================================*/

static void * __vk_usbh_uac_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs);
static void __vk_usbh_uac_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param);

extern void vsf_usbh_uac_on_new(void *uac, usb_uac_ac_interface_header_desc_t *ac_header);
extern void vsf_usbh_uac_on_del(void *uac);

/*============================ LOCAL VARIABLES ===============================*/

static const vk_usbh_dev_id_t __vk_usbh_uac_dev_id[] = {
    // Audio Control
    { VSF_USBH_MATCH_IFS_CLASS(USB_CLASS_AUDIO, USB_AUDIO_SUBCLASS_AUDIOCONTROL, 0x00) },
};

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbh_class_drv_t vk_usbh_uac_drv = {
    .name       = "uac",
    .dev_id_num = dimof(__vk_usbh_uac_dev_id),
    .dev_ids    = __vk_usbh_uac_dev_id,
    .probe      = __vk_usbh_uac_probe,
    .disconnect = __vk_usbh_uac_disconnect,
};

/*============================ IMPLEMENTATION ================================*/

vk_usbh_uac_stream_t * vsf_usbh_uac_get_stream_info(void *param, uint_fast8_t stream_idx)
{
    vk_usbh_uac_t *uac = param;
    VSF_USB_ASSERT(stream_idx < uac->stream_num);
    return &uac->streams[stream_idx];
}

vsf_err_t __vsf_usbh_uac_submit_req(void *uac_ptr, void *data, struct usb_ctrlrequest_t *req)
{
    vk_usbh_uac_t *uac = uac_ptr;

    VSF_USB_ASSERT((uac != NULL) && (req != NULL));

    if (uac->is_req_pending) {
        return VSF_ERR_NOT_AVAILABLE;
    }
    uac->is_req_pending = true;
    uac->req = *req;
    uac->req_data = data;
    return vsf_eda_post_evt((vsf_eda_t *)&uac->task, VSF_USBH_UAC_EVT_SUBMIT_REQ);
}

vsf_err_t vsf_usbh_uac_connect_stream(void *param, uint_fast8_t stream_idx, vsf_stream_t *stream)
{
    vk_usbh_uac_t *uac = param;
    VSF_USB_ASSERT(stream_idx < uac->stream_num);
    vk_usbh_uac_stream_t *uac_stream = &uac->streams[stream_idx];

    vsf_protect_t orig = vsf_protect_sched();
    if (!uac_stream->stream) {
        uac_stream->stream = stream;
        vsf_unprotect_sched(orig);
        return vsf_eda_post_evt((vsf_eda_t *)&uac->task, VSF_USBH_UAC_EVT_CONNECT + stream_idx);
    }
    vsf_unprotect_sched(orig);
    return VSF_ERR_FAIL;
}

void vsf_usbh_uac_disconnect_stream(void *param, uint_fast8_t stream_idx)
{
    vk_usbh_uac_t *uac = param;
    VSF_USB_ASSERT(stream_idx < uac->stream_num);
    vsf_eda_post_evt((vsf_eda_t *)&uac->task, VSF_USBH_UAC_EVT_DISCONNECT + stream_idx);
}

#ifndef WEAK_VSF_USBH_UAC_ON_NEW
WEAK(vsf_usbh_uac_on_new)
void vsf_usbh_uac_on_new(void *uac, usb_uac_ac_interface_header_desc_t *ac_header)
{
    // TODO: call av layer as default processor
}
#endif

#ifndef WEAK_VSF_USBH_UAC_ON_DEL
WEAK(vsf_usbh_uac_on_del)
void vsf_usbh_uac_on_del(void *uac)
{
    // TODO: call av layer as default processor
}
#endif

static void __vk_usbh_uac_free_urb(vk_usbh_uac_t *uac)
{
    vk_usbh_t *usbh = uac->usbh;

    for (uint_fast8_t i = 0; i < uac->stream_num; i++) {
#if VSF_USBH_UAC_CFG_URB_NUM_PER_STREAM > 1
        for (uint_fast8_t j = 0; j < VSF_USBH_UAC_CFG_URB_NUM_PER_STREAM; j++) {
            vk_usbh_free_urb(usbh, &uac->streams[i].urb[j]);
        }
#else
        vk_usbh_free_urb(usbh, &uac->streams[i].urb[0]);
#endif
    }
}

static void __vk_usbh_uac_on_eda_terminate(vsf_eda_t *eda)
{
    vk_usbh_uac_t *uac = container_of(eda, vk_usbh_uac_t, task);
    vsf_usbh_free(uac);
}

static bool __vk_usbh_uac_submit_urb_iso(vk_usbh_uac_t *uac, vk_usbh_uac_stream_t *uac_stream, uint_fast8_t urb_idx)
{
    vk_usbh_urb_t *urb = &uac_stream->urb[urb_idx];
    vsf_stream_t *stream = uac_stream->stream;
    uint_fast16_t frame_size;
    uint_fast32_t size;
#if VSF_USBH_UAC_CFG_URB_WITH_BUFFER != ENABLED
    uint8_t *ptr;
#endif

    if (uac_stream->is_in) {
        // for IN, use epsize
        frame_size = vk_usbh_urb_get_pipe(urb).size;
#if VSF_USBH_UAC_CFG_URB_WITH_BUFFER == ENABLED
        size = vsf_stream_get_free_size(stream);
#else
        size = vsf_stream_get_wbuf(stream, &ptr);
#endif
    } else {
        // for OUT, use frame size
        frame_size = uac_stream->channel_num * uac_stream->sample_size * uac_stream->sample_rate / 1000;
#if VSF_USBH_UAC_CFG_URB_WITH_BUFFER == ENABLED
        size = vsf_stream_get_data_size(stream);
#else
        size = vsf_stream_get_rbuf(stream, &ptr);
#endif
    }

    if (size >= frame_size) {
#if VSF_USBH_UAC_CFG_URB_WITH_BUFFER == ENABLED
        if (!uac_stream->is_in) {
            vsf_stream_read(stream, vk_usbh_urb_peek_buffer(urb), frame_size);
        }
#else
        vk_usbh_urb_set_buffer(urb, ptr, frame_size);
#endif

        uint_fast8_t start_frame = uac_stream->next_frame;
        uint_fast16_t cur_frame = vk_usbh_get_frame(uac->usbh);
        bool positive = start_frame >= cur_frame;
        if ((start_frame != 0) && (positive || ((cur_frame - start_frame) > (0x07FF >> 1)))) {
            start_frame = start_frame - cur_frame;
            if (!positive) {
                start_frame += 0x07FF;
            }

            uac_stream->next_frame++;
        } else {
            uac_stream->next_frame = cur_frame + 1;
            start_frame = 0;
        }
        uac_stream->urb_mask |= 1 << urb_idx;
        vk_usbh_submit_urb_iso(uac->usbh, urb, start_frame);
        return true;
    }

    uac_stream->urb_mask &= ~(1 << urb_idx);
    return false;
}

void __vk_usbh_uac_stream_evthandler(void *param, vsf_stream_evt_t evt)
{
    vk_usbh_uac_stream_t *uac_stream = param;

    vk_usbh_uac_stream_t *uac_stream_base = uac_stream - uac_stream->idx;
    vk_usbh_uac_t *uac = container_of(uac_stream_base, vk_usbh_uac_t, streams);

    if (    (VSF_STREAM_ON_CONNECT == evt)
        ||  ((VSF_STREAM_ON_TX == evt) && uac_stream->is_in)
        ||  ((VSF_STREAM_ON_RX == evt) && !uac_stream->is_in)) {
        vsf_eda_post_evt((vsf_eda_t *)&uac->task, VSF_USBH_UAC_EVT_STREAM + uac_stream->idx);
    }
}

static void __vk_usbh_uac_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbh_uac_t *uac = container_of(eda, vk_usbh_uac_t, task);
    vk_usbh_dev_t *dev = uac->dev;
    vk_usbh_uac_stream_t *uac_stream;
    vsf_stream_t *stream;

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_usbh_uac_on_new(uac, &uac->ac_header);
        break;
    case VSF_EVT_MESSAGE: {
            vk_usbh_urb_t urb = { .urb_hcd = vsf_eda_get_cur_msg() };
            vk_usbh_pipe_t pipe;
            int_fast16_t status = vk_usbh_urb_get_status(&urb);

            pipe = vk_usbh_urb_get_pipe(&urb);
            if (0 == pipe.endpoint) {
                if (uac->is_cur_req) {
                    // TODO: notify user
                    uac->is_req_pending = false;
                    goto check_next_ctrl_transfer;
                }

                uac_stream = &uac->streams[uac->stream_cur];
                stream = uac_stream->stream;
                if ((stream != NULL) && !uac_stream->is_connected) {
                    if (URB_OK == status) {
#if VSF_USBH_UAC_CFG_URB_WITH_BUFFER == ENABLED
                        uint_fast16_t epsize = vk_usbh_urb_get_pipe(&uac_stream->urb[0]).size;
                        uint_fast16_t frame_size = uac_stream->channel_num * uac_stream->sample_size * uac_stream->sample_rate / 1000;
#endif

                        for (uint_fast8_t i = 0; i < VSF_USBH_UAC_CFG_URB_NUM_PER_STREAM; i++) {
                            if (    (VSF_ERR_NONE != vk_usbh_alloc_urb(uac->usbh, dev, &uac_stream->urb[i]))
#   if VSF_USBH_UAC_CFG_URB_WITH_BUFFER == ENABLED
                                ||  (NULL == vk_usbh_urb_alloc_buffer(&uac_stream->urb[i], uac_stream->is_in ? epsize : frame_size))
#   endif
                            ) {
                                goto disconnect_stream;
                            }
                        }

                        uac_stream->is_connected = true;
                        if (uac_stream->is_in) {
                            stream->tx.param = uac_stream;
                            stream->tx.evthandler = __vk_usbh_uac_stream_evthandler;
                            vsf_stream_connect_tx(stream);
                        } else {
                            stream->rx.param = uac_stream;
                            stream->rx.evthandler = __vk_usbh_uac_stream_evthandler;
                            vsf_stream_connect_rx(stream);
                        }
                    } else {
                    disconnect_stream:
                        if (uac_stream->is_in) {
                            vsf_stream_disconnect_tx(stream);
                        } else {
                            vsf_stream_disconnect_rx(stream);
                        }

                        for (uint_fast8_t i = 0; i < VSF_USBH_UAC_CFG_URB_NUM_PER_STREAM; i++) {
                            if (vk_usbh_urb_is_alloced(&uac_stream->urb[i])) {
                                vk_usbh_free_urb(uac->usbh, &uac_stream->urb[i]);
                            }
                        }

                        uac_stream->stream = NULL;
                    }
                } else if (uac_stream->is_to_disconnect) {
                    uac_stream->is_to_disconnect = false;
                    uac_stream->is_connected = false;
                    uac_stream->next_frame = 0;
                    goto disconnect_stream;
                }
                goto check_next_ctrl_transfer;
            } else {
                int_fast8_t urb_idx = -1;
                uac_stream = &uac->streams[0];
                for (uint_fast8_t i = 0; i < uac->stream_num; i++, uac_stream++) {
                    if (vk_usbh_urb_get_pipe(&uac_stream->urb[0]).value == pipe.value) {
#if VSF_USBH_UAC_CFG_URB_NUM_PER_STREAM > 1
                        for (uint_fast8_t j = 0; j < VSF_USBH_UAC_CFG_URB_NUM_PER_STREAM; j++) {
                            if (uac_stream->urb[j].urb_hcd == urb.urb_hcd) {
                                urb_idx = j;
                                break;
                            }
                        }
#else
                        urb_idx = 0;
#endif
                        break;
                    }
                }

                VSF_USB_ASSERT((urb_idx >= 0) && (urb_idx < VSF_USBH_UAC_CFG_URB_NUM_PER_STREAM));
                stream = uac_stream->stream;
                uint_fast32_t actual_length = vk_usbh_urb_get_actual_length(&urb);
                // TODO: make sure actual_length is good
                if (uac_stream->is_in) {
#if VSF_USBH_UAC_CFG_URB_WITH_BUFFER == ENABLED
                    vsf_stream_write(stream, vk_usbh_urb_peek_buffer(&urb), actual_length);
#else
                    vsf_stream_write(stream, NULL, actual_length);
#endif
                } else {
#if VSF_USBH_UAC_CFG_URB_WITH_BUFFER == ENABLED
#else
                    vsf_stream_read(stream, NULL, actual_length);
#endif
                }

                __vk_usbh_uac_submit_urb_iso(uac, uac_stream, urb_idx);
            }
        }
        break;
    default: {
            evt -= VSF_EVT_USER;
            uint_fast8_t stream_idx = evt & ((1 << VSF_USBH_UAC_CFG_URB_NUM_PER_STREAM) - 1);
            uac_stream = &uac->streams[stream_idx];
            stream = uac_stream->stream;

            switch (evt & VSF_USBH_UAC_SUBEVT_MASK) {
            case VSF_USBH_UAC_SUBEVT_DISCONNECT:
                VSF_USB_ASSERT(stream != NULL);
                uac_stream->is_to_disconnect = true;
                break;
            case VSF_USBH_UAC_SUBEVT_CONNECT:
                // connect stream first, if set interface failed, disconnect stream to indicate failure
                uac_stream->param = uac;
                uac_stream->idx = stream_idx;
                break;
            case VSF_USBH_UAC_SUBEVT_STREAM: {
                    uint_fast16_t frame_size = uac_stream->channel_num * uac_stream->sample_size * uac_stream->sample_rate / 1000;
                    int_fast8_t urb_idx;

                    if (uac_stream->is_in) {
                        if (vsf_stream_get_free_size(uac_stream->stream) < frame_size) {
                            break;
                        }
                    } else {
                        if (vsf_stream_get_data_size(uac_stream->stream) < frame_size) {
                            break;
                        }
                    }

                    urb_idx = vsf_ffz(uac_stream->urb_mask | ~((1 << sizeof(uac_stream->urb_mask)) - 1));
                    if (urb_idx >= 0) {
                        __vk_usbh_uac_submit_urb_iso(uac, uac_stream, urb_idx);
                    }
                }
                break;
            case VSF_USBH_UAC_SUBEVT_SUBMIT_REQ:
                break;
            }

            // run here means no error, will send control requet
            if (uac->is_ep0_busy) {
                break;
            }
            uac->is_ep0_busy = true;
            if (VSF_ERR_NONE != __vsf_eda_crit_npb_enter(&dev->ep0.crit)) {
                break;
            }
        }
        // fall through
    case VSF_EVT_SYNC:
    check_next_ctrl_transfer:
        uac->is_cur_req = false;
        for (uint_fast8_t i = 0; i < uac->stream_num; i++) {
            uint_fast8_t alt_setting;
            uac_stream = &uac->streams[i];

            // to open a stream, use alt_setting 1 for simplicity, maybe fix this later to use alt_setting from user
            // to close a stream, use alt_setting 0
            if ((uac_stream->stream != NULL) && !uac_stream->is_connected) {
                alt_setting = 1;
            } else if (uac_stream->is_to_disconnect) {
                alt_setting = 0;
            } else {
                continue;
            }

            uac->stream_cur = i;
            if (VSF_ERR_NONE == vk_usbh_set_interface(uac->usbh, dev, uac->ac_header.baInterfaceNr[i], alt_setting)) {
                return;
            }
        }

        if (uac->is_req_pending) {
            uac->is_cur_req = true;
            if (uac->req_data != NULL) {
                vk_usbh_urb_set_buffer(&uac->dev->ep0.urb, uac->req_data, uac->req.wLength);
            }
            if (VSF_ERR_NONE == vk_usbh_control_msg(uac->usbh, uac->dev, &uac->req)) {
                return;
            } else {
                // TODO: notify user
            }
        }

        // if runs here, no pending operation, exit critical section
        __vsf_eda_crit_npb_leave(&dev->ep0.crit);
        uac->is_ep0_busy = false;
        break;
    }
}

static void __vk_usbh_uac_parse_stream_format(vk_usbh_uac_stream_t *uac_stream, uint8_t *desc)
{
    usb_uac_as_interface_desc_t *as_desc = (usb_uac_as_interface_desc_t *)desc;
    uint_fast16_t format = uac_stream->format = as_desc->wFormatTag;
    uint_fast8_t type = format >> 12;
    uint8_t *format_desc = (uint8_t *)as_desc + as_desc->bLength;

    switch (type) {
    case 0: {   /* Audio Data Format Type I */
            usb_uac_as_format_i_desc_t *format_i_desc = (usb_uac_as_format_i_desc_t *)format_desc;
            uac_stream->channel_num = format_i_desc->bNrChannels;
            uac_stream->sample_size = format_i_desc->bSubframeSize;
            uac_stream->sample_rate = ((uint32_t)format_i_desc->tSamFreq[0][0] << 0) +
                ((uint32_t)format_i_desc->tSamFreq[0][1] << 8) + ((uint32_t)format_i_desc->tSamFreq[0][2] << 16);
        }
        break;
    case 1:     /* Audio Data Format Type II */
        break;
    case 3:     /* Audio Data Format Type III */
        break;
    }
}

static void * __vk_usbh_uac_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs)
{
    vk_usbh_ifs_t *ifs = parser_ifs->ifs;
    vk_usbh_ifs_alt_parser_t *parser_alt = &parser_ifs->parser_alt[ifs->cur_alt];
    struct usb_interface_desc_t *desc_ifs = parser_alt->desc_ifs;
    vk_usbh_uac_t *uac = NULL;

    if (USB_AUDIO_SUBCLASS_AUDIOCONTROL == desc_ifs->bInterfaceSubClass) {
        uint8_t *parser_ptr = (uint8_t *)desc_ifs + desc_ifs->bLength;
        usb_uac_ac_interface_header_desc_t *ac_header = (usb_uac_ac_interface_header_desc_t *)parser_ptr;

        if (    (ac_header->bDescriptorType != USB_DT_CS_INTERFACE)
            ||  (ac_header->bDescriptorSubtype != 0x01)     /* HEADER */
            ||  (le16_to_cpu(ac_header->bcdADC) != 0x0100)  /* VERSION */) {
            return NULL;
        }

        uac = vsf_usbh_malloc(sizeof(vk_usbh_uac_t) + ac_header->wTotalLength - sizeof(usb_uac_ac_interface_header_desc_t));
        if (NULL == uac) { return NULL; }
        memset(uac, 0, sizeof(*uac));
        memcpy(&uac->ac_header, ac_header, ac_header->wTotalLength);
        ac_header = &uac->ac_header;
        uac->usbh = usbh;
        uac->dev = dev;
        uac->ifs = ifs;

        vk_usbh_ifs_parser_t *parser_vs_ifs;
        vk_usbh_ifs_alt_parser_t *parser_vs_alt;
        vk_usbh_uac_stream_t *uac_stream;
        uac->stream_num = min(VSF_USBD_UAC_CFG_STREAM_NUM, ac_header->bInCollection);
        for (uint_fast8_t i = 0; i < uac->stream_num; i++) {
            if (ac_header->baInterfaceNr[i] >= usbh->parser->num_of_ifs) {
                goto free_all;
            }

            uac_stream = &uac->streams[i];
            parser_vs_ifs = &usbh->parser->parser_ifs[ac_header->baInterfaceNr[i]];
            for (uint_fast8_t j = 0; j < parser_vs_ifs->ifs->num_of_alt; j++) {
                parser_vs_alt = &parser_vs_ifs->parser_alt[j];
                desc_ifs = parser_vs_alt->desc_ifs;

                if (    (desc_ifs->bInterfaceClass != USB_CLASS_AUDIO)
                    ||  (desc_ifs->bInterfaceSubClass != USB_AUDIO_SUBCLASS_AUDIOSTREAMING)) {
                    goto free_all;
                }

                if (0 == j) {
                    if (desc_ifs->bNumEndpoints != 0) {
                        goto free_all;
                    }
                } else {
                    if (    (desc_ifs->bNumEndpoints != 1)
                        ||  ((parser_vs_alt->desc_ep->bmAttributes & 0x03) != USB_EP_TYPE_ISO)) {
                        goto free_all;
                    }

                    // TODO: different interface maybe have different audio parameter
                    //  currently, use altsetting 1 and interval 1ms
#if VSF_USBH_UAC_CFG_URB_NUM_PER_STREAM > 1
                    for (uint_fast8_t i = 0; i < VSF_USBH_UAC_CFG_URB_NUM_PER_STREAM; i++) {
                        vk_usbh_urb_prepare(&uac_stream->urb[i], uac->dev, parser_vs_alt->desc_ep);
                    }
#else
                    vk_usbh_urb_prepare(&uac_stream->urb[0], uac->dev, parser_vs_alt->desc_ep);
#endif
                    uac_stream->urb_mask = ~((1UL << VSF_USBH_UAC_CFG_URB_NUM_PER_STREAM) - 1);
                    uac_stream->is_in = uac_stream->urb[0].pipe.dir_in1out0;
                    __vk_usbh_uac_parse_stream_format(uac_stream, (uint8_t *)desc_ifs + desc_ifs->bLength);
                    break;
                }
            }
        }
    }

    uac->task.fn.evthandler = __vk_usbh_uac_evthandler;
    uac->task.on_terminate = __vk_usbh_uac_on_eda_terminate;
    vsf_eda_init((vsf_eda_t *)&uac->task, vsf_prio_inherit, false);
    return uac;

free_all:
    __vk_usbh_uac_free_urb(uac);
    vsf_usbh_free(uac);
    return NULL;
}

static void __vk_usbh_uac_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param)
{
    vk_usbh_uac_t *uac = (vk_usbh_uac_t *)param;
    vk_usbh_uac_stream_t *uac_stream;
    __vk_usbh_uac_free_urb(uac);

    for (uint_fast8_t i = 0; i < uac->stream_num; i++) {
        uac_stream = vsf_usbh_uac_get_stream_info(uac, i);
        if (uac_stream->is_in) {
            vsf_stream_disconnect_tx(uac_stream->stream);
        } else {
            vsf_stream_disconnect_rx(uac_stream->stream);
        }
    }
    vsf_eda_fini((vsf_eda_t *)&uac->task);
}

#endif
