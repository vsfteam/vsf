/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_UVC == ENABLED

#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_USBH_CLASS_IMPLEMENT_CLASS__
#define __VSF_USBH_UVC_CLASS_IMPLEMENT

#include "kernel/vsf_kernel.h"
#include "../../vsf_usbh.h"
#include "./vsf_usbh_uvc.h"
// for USB_EP_TYPE_ISO
#include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE != ENABLED
#   error "VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE is required"
#endif

#if VSF_USE_SIMPLE_STREAM != ENABLED
#   error "VSF_USE_SIMPLE_STREAM is required"
#endif

#ifndef VSF_USBH_UVC_CFG_URB_WITH_BUFFER
//  if enabled, urb will have dedicated buffer
#   define VSF_USBH_UVC_CFG_URB_WITH_BUFFER     ENABLED
#endif

#define vsf_usbh_uvc_task_t                     vsf_eda_t

#if VSF_USBH_UVC_CFG_URB_NUM_PER_STREAM > 1
#   if VSF_USBH_UVC_CFG_URB_WITH_BUFFER != ENABLED
#       warning VSF_USBH_UVC_CFG_URB_WITH_BUFFER MUST be enabled for multiple urb per stream
#       undef VSF_USBH_UVC_CFG_URB_WITH_BUFFER
#       define VSF_USBH_UVC_CFG_URB_WITH_BUFFER ENABLED
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    USB_VIDEO_SUBCLASS_VIDEOCONTROL     = 0x01,
    USB_VIDEO_SUBCLASS_VIDEOSTREAMING   = 0x02,
};

enum {
    VSF_USBH_UVC_EVT_CONNECT            = VSF_EVT_USER + 0,
    VSF_USBH_UVC_EVT_DISCONNECT         = VSF_EVT_USER + 1,
    VSF_USBH_UVC_EVT_STREAM             = VSF_EVT_USER + 2,
    VSF_USBH_UVC_EVT_SUBMIT_REQ         = VSF_EVT_USER + 3,
};

typedef struct vk_usbh_uvc_t {
    vk_usbh_t *usbh;
    vk_usbh_dev_t *dev;
    vk_usbh_ifs_t *ifs;

    uint8_t frame_idx;
    uint8_t format_idx;
    uint8_t urb_mask;
    uint8_t is_ep0_busy         : 1;
    uint8_t is_req_pending      : 1;
    uint8_t is_cur_req          : 1;
    uint8_t is_connected        : 1;
    uint8_t is_to_disconnect    : 1;
    uint16_t next_frame;

    struct usb_ctrlrequest_t req;
    void *req_data;

    vsf_usbh_uvc_task_t task;
    vk_usbh_urb_t urb[VSF_USBH_UVC_CFG_URB_NUM_PER_STREAM];
    vsf_stream_t *stream;
    void *param;
    usb_uvc_vc_interface_header_desc_t *vc_header;
    usb_uvc_vs_interface_header_desc_t *vs_header;
    usb_uvc_format_desc_t **formats;
} vk_usbh_uvc_t;

/*============================ PROTOTYPES ====================================*/

static void * __vk_usbh_uvc_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs);
static void __vk_usbh_uvc_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param);

extern void vsf_usbh_uvc_on_new(void *uvc, usb_uvc_vc_interface_header_desc_t *vc_header,
            usb_uvc_vs_interface_header_desc_t *vs_header);
extern void vsf_usbh_uvc_on_del(void *uvc);

static bool __vk_usbh_uvc_submit_urb_iso(vk_usbh_uvc_t *uvc, uint_fast8_t urb_idx);

/*============================ LOCAL VARIABLES ===============================*/

static const vk_usbh_dev_id_t __vk_usbh_uvc_dev_id[] = {
    // Video Control
    { VSF_USBH_MATCH_IFS_CLASS(USB_CLASS_VIDEO, USB_VIDEO_SUBCLASS_VIDEOCONTROL, 0x00) },
};

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbh_class_drv_t vk_usbh_uvc_drv = {
    .name       = "uvc",
    .dev_id_num = dimof(__vk_usbh_uvc_dev_id),
    .dev_ids    = __vk_usbh_uvc_dev_id,
    .probe      = __vk_usbh_uvc_probe,
    .disconnect = __vk_usbh_uvc_disconnect,
};

/*============================ IMPLEMENTATION ================================*/

int16_t vsf_usbh_uvc_get_desc(uint8_t *buf, uint_fast16_t size,
        uint_fast8_t subtype, void **ptr)
{
    struct usb_uvc_desc_header_t *header;
    uint8_t *buf_orig = buf;

    VSF_USB_ASSERT((buf != NULL) && (ptr != NULL));

    while (size >= sizeof(struct usb_uvc_desc_header_t)) {
        if (vk_usbh_get_extra_descriptor(buf, size, 0x24, (void **)&header) < 0) {
            break;
        }
        if (header->bDescriptorSubtype == subtype) {
            *ptr = header;
            return (uint8_t *)header - buf_orig;
        }

        buf = (uint8_t *)header + header->bLength;
        size -= header->bLength;
    }
    return -1;
}

usb_uvc_format_desc_t * vsf_usbh_uvc_get_format(void *uvc_ptr, uint_fast8_t format_idx)
{
    vk_usbh_uvc_t *uvc = uvc_ptr;

    VSF_USB_ASSERT((uvc != NULL) && (format_idx < uvc->vs_header->bNumFormats));
    return uvc->formats[format_idx];
}

vsf_err_t __vsf_usbh_uvc_submit_req(void *uvc_ptr, void *data, struct usb_ctrlrequest_t *req)
{
    vk_usbh_uvc_t *uvc = uvc_ptr;

    VSF_USB_ASSERT((uvc != NULL) && (req != NULL));

    if (uvc->is_req_pending) {
        return VSF_ERR_NOT_AVAILABLE;
    }
    uvc->is_req_pending = true;
    uvc->req = *req;
    uvc->req_data = data;
    return vsf_eda_post_evt((vsf_eda_t *)&uvc->task, VSF_USBH_UVC_EVT_SUBMIT_REQ);
}

vsf_err_t vsf_usbh_uvc_connect_stream(void *param, uint_fast8_t format_idx, uint_fast8_t frame_idx, vsf_stream_t *stream)
{
    vk_usbh_uvc_t *uvc = param;
    VSF_USB_ASSERT(format_idx < uvc->vs_header->bNumFormats);
    VSF_USB_ASSERT(frame_idx < uvc->formats[format_idx]->bNumFrameDescriptors);

    vsf_protect_t orig = vsf_protect_sched();
    if (!uvc->stream) {
        uvc->format_idx = format_idx;
        uvc->frame_idx = frame_idx;
        uvc->stream = stream;
        vsf_unprotect_sched(orig);
        return vsf_eda_post_evt((vsf_eda_t *)&uvc->task, VSF_USBH_UVC_EVT_CONNECT);
    }
    vsf_unprotect_sched(orig);
    return VSF_ERR_FAIL;
}

void vsf_usbh_uvc_disconnect_stream(void *param)
{
    vk_usbh_uvc_t *uvc = param;
    vsf_eda_post_evt((vsf_eda_t *)&uvc->task, VSF_USBH_UVC_EVT_DISCONNECT);
}

WEAK(vsf_usbh_uvc_on_new)
void vsf_usbh_uvc_on_new(void *uvc, usb_uvc_vc_interface_header_desc_t *vc_header,
        usb_uvc_vs_interface_header_desc_t *vs_header)
{
    // TODO: call av layer as default processor
}

WEAK(vsf_usbh_uvc_on_del)
void vsf_usbh_uvc_on_del(void *uvc)
{
    // TODO: call av layer as default processor
}

static void __vk_usbh_uvc_free_urb(vk_usbh_uvc_t *uvc)
{
    vk_usbh_t *usbh = uvc->usbh;

#if VSF_USBH_UVC_CFG_URB_NUM_PER_STREAM > 1
    for (uint_fast8_t j = 0; j < VSF_USBH_UVC_CFG_URB_NUM_PER_STREAM; j++) {
        vk_usbh_free_urb(usbh, &uvc->urb[j]);
    }
#else
    vk_usbh_free_urb(usbh, &uvc->urb[0]);
#endif
}

static void __vk_usbh_uvc_on_eda_terminate(vsf_eda_t *eda)
{
    vk_usbh_uvc_t *uvc = vsf_container_of(eda, vk_usbh_uvc_t, task);
    vsf_usbh_free(uvc);
}

static void __vk_usbh_uvc_iso_complete(void *param, vk_usbh_hcd_urb_t *urb_hcd)
{
    vk_usbh_uvc_t *uvc = param;
    vk_usbh_urb_t urb = { .urb_hcd = urb_hcd };

#if VSF_USBH_UVC_CFG_URB_NUM_PER_STREAM > 1
    vk_usbh_pipe_t pipe = vk_usbh_urb_get_pipe(&urb);
    int_fast8_t urb_idx = -1;
    for (uint_fast8_t i = 0; i < VSF_USBH_UVC_CFG_URB_NUM_PER_STREAM; i++) {
        if (uvc_stream->urb[i].urb_hcd == urb.urb_hcd) {
            urb_idx = i;
            break;
        }
    }
    VSF_USB_ASSERT((urb_idx >= 0) && (urb_idx < VSF_USBH_UVC_CFG_URB_NUM_PER_STREAM));
#endif

    vsf_stream_t *stream = uvc->stream;
    uint_fast32_t actual_length = vk_usbh_urb_get_actual_length(&urb);
    // TODO: make sure actual_length is good
#if VSF_USBH_UVC_CFG_URB_WITH_BUFFER == ENABLED
    vsf_stream_write(stream, vk_usbh_urb_peek_buffer(&urb), actual_length);
#else
    vsf_stream_write(stream, NULL, actual_length);
#endif

#if VSF_USBH_UVC_CFG_URB_NUM_PER_STREAM > 1
    __vk_usbh_uvc_submit_urb_iso(uvc, urb_idx);
#else
    __vk_usbh_uvc_submit_urb_iso(uvc, 0);
#endif
}

static bool __vk_usbh_uvc_submit_urb_iso(vk_usbh_uvc_t *uvc, uint_fast8_t urb_idx)
{
    vk_usbh_urb_t *urb = &uvc->urb[urb_idx];
    vsf_stream_t *stream = uvc->stream;
    uint_fast16_t frame_size;
    uint_fast32_t size;
#if VSF_USBH_UVC_CFG_URB_WITH_BUFFER != ENABLED
    uint8_t *ptr;
#endif

    frame_size = vk_usbh_urb_get_pipe(urb).size;
#if VSF_USBH_UVC_CFG_URB_WITH_BUFFER == ENABLED
    size = vsf_stream_get_free_size(stream);
#else
    size = vsf_stream_get_wbuf(stream, &ptr);
#endif

    if (size >= frame_size) {
#if VSF_USBH_UVC_CFG_URB_WITH_BUFFER == ENABLED
#else
        vk_usbh_urb_set_buffer(urb, ptr, frame_size);
#endif

        uint_fast16_t start_frame = uvc->next_frame;
        uint_fast16_t cur_frame = vk_usbh_get_frame(uvc->usbh);
        bool positive = start_frame >= cur_frame;
        if ((start_frame != 0) && (positive || ((cur_frame - start_frame) > (0x07FF >> 1)))) {
            start_frame = start_frame - cur_frame;
            if (!positive) {
                start_frame += 0x07FF;
            }

            uvc->next_frame++;
        } else {
            uvc->next_frame = cur_frame + 1;
            start_frame = 0;
        }
        uvc->urb_mask |= 1 << urb_idx;
        vk_usbh_urb_set_complete(urb, __vk_usbh_uvc_iso_complete, uvc);
        vk_usbh_submit_urb_iso(uvc->usbh, urb, start_frame);
        return true;
    }

    uvc->urb_mask &= ~(1 << urb_idx);
    return false;
}

void __vk_usbh_uvc_stream_evthandler(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt)
{
    vk_usbh_uvc_t *uvc = param;

    if ((VSF_STREAM_ON_CONNECT == evt) || (VSF_STREAM_ON_TX == evt)) {
        vsf_eda_post_evt((vsf_eda_t *)&uvc->task, VSF_USBH_UVC_EVT_STREAM);
    }
}

static void __vk_usbh_uvc_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbh_uvc_t *uvc = vsf_container_of(eda, vk_usbh_uvc_t, task);
    vk_usbh_dev_t *dev = uvc->dev;
    vsf_stream_t *stream = uvc->stream;

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_usbh_uvc_on_new(uvc, uvc->vc_header, uvc->vs_header);
        break;
    case VSF_EVT_MESSAGE: {
            vk_usbh_urb_t urb = { .urb_hcd = vsf_eda_get_cur_msg() };
            vk_usbh_pipe_t pipe = vk_usbh_urb_get_pipe(&urb);
            int_fast16_t status = vk_usbh_urb_get_status(&urb);

            if (0 == pipe.endpoint) {
                if (uvc->is_cur_req) {
                    // TODO: notify user
                    uvc->is_req_pending = false;
                    goto check_next_ctrl_transfer;
                }

                if ((stream != NULL) && !uvc->is_connected) {
                    if (URB_OK == status) {
#if VSF_USBH_UVC_CFG_URB_WITH_BUFFER == ENABLED
                        uint_fast16_t epsize = vk_usbh_urb_get_pipe(&uvc->urb[0]).size;
#endif

                        for (uint_fast8_t i = 0; i < VSF_USBH_UVC_CFG_URB_NUM_PER_STREAM; i++) {
                            if (    (VSF_ERR_NONE != vk_usbh_alloc_urb(uvc->usbh, dev, &uvc->urb[i]))
#if VSF_USBH_UVC_CFG_URB_WITH_BUFFER == ENABLED
                                ||  (NULL == vk_usbh_urb_alloc_buffer(&uvc->urb[i], epsize))
#endif
                            ) {
                                goto disconnect_stream;
                            }
                        }

                        uvc->is_connected = true;
                        stream->tx.param = uvc;
                        stream->tx.evthandler = __vk_usbh_uvc_stream_evthandler;
                        vsf_stream_connect_tx(stream);
                    } else {
                    disconnect_stream:
                        vsf_stream_disconnect_tx(stream);
                        for (uint_fast8_t i = 0; i < VSF_USBH_UVC_CFG_URB_NUM_PER_STREAM; i++) {
                            if (vk_usbh_urb_is_alloced(&uvc->urb[i])) {
                                vk_usbh_free_urb(uvc->usbh, &uvc->urb[i]);
                            }
                        }

                        uvc->stream = NULL;
                    }
                } else if (uvc->is_to_disconnect) {
                    uvc->is_to_disconnect = false;
                    uvc->is_connected = false;
                    uvc->next_frame = 0;
                    goto disconnect_stream;
                }
                goto check_next_ctrl_transfer;
            } else {
                VSF_USB_ASSERT(false);
            }
        }
        break;
    case VSF_USBH_UVC_EVT_STREAM: {
            uint_fast16_t epsize = vk_usbh_urb_get_pipe(&uvc->urb[0]).size;
            uint_fast16_t stream_size;
            int_fast8_t urb_idx;

            stream_size = vsf_stream_get_free_size(uvc->stream);
            if (stream_size >= epsize) {
                urb_idx = vsf_ffz32(uvc->urb_mask | ~((1 << sizeof(uvc->urb_mask)) - 1));
                if (urb_idx >= 0) {
                    __vk_usbh_uvc_submit_urb_iso(uvc, urb_idx);
                }
            }
        }
        break;
    case VSF_USBH_UVC_EVT_DISCONNECT:
        uvc->is_to_disconnect = true;
        // fall through
    case VSF_USBH_UVC_EVT_CONNECT:
        // fall through
    case VSF_USBH_UVC_EVT_SUBMIT_REQ:
        if (uvc->is_ep0_busy) {
            break;
        }
        uvc->is_ep0_busy = true;
        if (VSF_ERR_NONE != __vsf_eda_crit_npb_enter(&dev->ep0.crit)) {
            break;
        }
        break;
    case VSF_EVT_SYNC:
    check_next_ctrl_transfer:
        uvc->is_cur_req = false;
        uint_fast8_t alt_setting = 0xFF;

        // to open a stream, use alt_setting 1 for simplicity, maybe fix this later to use alt_setting from user
        // to close a stream, use alt_setting 0
        if ((uvc->stream != NULL) && !uvc->is_connected) {
            alt_setting = 1;
        } else if (uvc->is_to_disconnect) {
            alt_setting = 0;
        }

        if (alt_setting != 0xFF) {
            vk_usbh_set_interface(uvc->usbh, dev, uvc->vc_header->baInterfaceNr[0], alt_setting);
            return;
        }

        if (uvc->is_req_pending) {
            uvc->is_cur_req = true;
            if (uvc->req_data != NULL) {
                vk_usbh_urb_set_buffer(&uvc->dev->ep0.urb, uvc->req_data, uvc->req.wLength);
            }
            if (VSF_ERR_NONE == vk_usbh_control_msg(uvc->usbh, uvc->dev, &uvc->req)) {
                return;
            } else {
                // TODO: notify user
            }
        }

        // if runs here, no pending operation, exit critical section
        __vsf_eda_crit_npb_leave(&dev->ep0.crit);
        uvc->is_ep0_busy = false;
        break;
    }
}

static void * __vk_usbh_uvc_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs)
{
    vk_usbh_ifs_t *ifs = parser_ifs->ifs;
    vk_usbh_ifs_alt_parser_t *parser_alt = &parser_ifs->parser_alt[ifs->cur_alt];
    struct usb_interface_desc_t *desc_ifs = parser_alt->desc_ifs;
    vk_usbh_uvc_t *uvc = NULL;

    if (USB_VIDEO_SUBCLASS_VIDEOCONTROL == desc_ifs->bInterfaceSubClass) {
        uint8_t *parser_ptr = (uint8_t *)desc_ifs + desc_ifs->bLength;
        usb_uvc_vc_interface_header_desc_t *vc_header = (usb_uvc_vc_interface_header_desc_t *)parser_ptr;
        vk_usbh_ifs_parser_t *parser_vs_ifs = &usbh->parser->parser_ifs[vc_header->baInterfaceNr[0]];
        usb_uvc_vs_interface_header_desc_t *vs_header;

        desc_ifs = parser_vs_ifs->parser_alt[0].desc_ifs;
        parser_ptr = (uint8_t *)desc_ifs + desc_ifs->bLength;
        vs_header = (usb_uvc_vs_interface_header_desc_t *)parser_ptr;;

        if (    (vc_header->bDescriptorType != USB_DT_CS_INTERFACE)
            ||  (vc_header->bDescriptorSubtype != 0x01)     /* HEADER */
            ||  (le16_to_cpu(vc_header->bcdVDC) != 0x0100)  /* VERSION */) {
            return NULL;
        }

        uvc = vsf_usbh_malloc(sizeof(vk_usbh_uvc_t) + sizeof(usb_uvc_format_desc_t *) * vs_header->bNumFormats
                        + vc_header->wTotalLength + vs_header->wTotalLength);
        if (NULL == uvc) { return NULL; }
        memset(uvc, 0, sizeof(*uvc));
        uvc->formats = (usb_uvc_format_desc_t **)&uvc[1];
        uvc->vc_header = (usb_uvc_vc_interface_header_desc_t *)&uvc->formats[vs_header->bNumFormats];
        uvc->vs_header = (usb_uvc_vs_interface_header_desc_t *)((uint8_t *)uvc->vc_header + vc_header->wTotalLength);
        memcpy(uvc->vc_header, vc_header, vc_header->wTotalLength);
        memcpy(uvc->vs_header, vs_header, vs_header->wTotalLength);
        vc_header = uvc->vc_header;
        vs_header = uvc->vs_header;
        uvc->usbh = usbh;
        uvc->dev = dev;
        uvc->ifs = ifs;
        // TODO: save endpoint sizes for different format/frame
        uvc->urb_mask = (uint8_t)(~((1UL << VSF_USBH_UVC_CFG_URB_NUM_PER_STREAM) - 1));

        usb_uvc_desc_header_t *header = (usb_uvc_desc_header_t *)vs_header;
        uint16_t size = vs_header->wTotalLength, idx = 0;
        while (size > 0) {
            switch (header->bDescriptorSubtype) {
            case 4:             // VS_FORMAT_UNCOMPRESSED
            case 6:             // VS_FORMAT_MJPEG
                uvc->formats[idx++] = (usb_uvc_format_desc_t *)header;
                break;
            }
            if (idx >= vs_header->bNumFormats) {
                break;
            }
            header = (usb_uvc_desc_header_t *)((uint8_t *)header + header->bLength);
        }
    }

    uvc->task.fn.evthandler = __vk_usbh_uvc_evthandler;
    uvc->task.on_terminate = __vk_usbh_uvc_on_eda_terminate;
#ifdef VSF_USBH_UVC_CFG_PRIORITY
    vsf_eda_init((vsf_eda_t *)&uvc->task, VSF_USBH_UVC_CFG_PRIORITY);
#else
    vsf_eda_init((vsf_eda_t *)&uvc->task);
#endif
#if VSF_KERNEL_CFG_TRACE == ENABLED
    vsf_kernel_trace_eda_info((vsf_eda_t *)&uvc->task, "usbh_uvc_task", NULL, 0);
#endif
    return uvc;
}

static void __vk_usbh_uvc_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param)
{
    vk_usbh_uvc_t *uvc = (vk_usbh_uvc_t *)param;
    __vk_usbh_uvc_free_urb(uvc);

    if (uvc->is_connected) {
        vsf_stream_disconnect_tx(uvc->stream);
    }
    vsf_eda_fini((vsf_eda_t *)&uvc->task);
}

#endif
