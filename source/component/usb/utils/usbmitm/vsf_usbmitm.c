/***************************************************************************
 *   Copyright (C) 2009 - 2025 by Simon Qian <vsfos@qq.com>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_USBH_CLASS_IMPLEMENT_CLASS__
#include <vsf.h>
#include <stdarg.h>
#include <stdio.h>

#include "./vsf_usbmitm.h"

typedef struct usbmitm_urbcb_t {
    vk_usbh_urb_t urb;
    uint16_t totalsize;
    uint16_t curpos;
    uint8_t ep;
    uint8_t epsize;
    uint8_t interval;
    uint8_t eptype : 3;
    uint8_t submitted : 1;
    uint8_t needzlp : 1;
    uint8_t ep_inited : 1;
} usbmitm_urbcb_t;

typedef struct usbmitm_t {
    struct {
        vk_usbh_t *host;
        vk_usbh_dev_t *dev;
    } usbh;

    struct {
        struct usb_ctrlrequest_t request;
        enum {
            USB_IDLE = 0, USB_SETUP, URB_SUBMITTED, URB_COMPLETED, USB_STATUS,
        } request_state;
        vsf_teda_t task;
        uint8_t dev_address;
        uint8_t host_address;
        uint8_t ep0size;
        uint8_t *config_desc;
    } usbd;
    union {
        struct {
            usbmitm_urbcb_t in[16];
            usbmitm_urbcb_t out[16];
        };
        usbmitm_urbcb_t all[16 + 16];
    } urbcb;
    usbmitm_urbcb_t *pending_urbcb;
    bool resetting;
} usbmitm_t;

typedef struct usbmitm_cfg_t {
    struct {
        const i_usb_dc_t *drv;
        int32_t int_priority;
    } usbd;
    usbmitm_plugin_t *pluginlist;
} usbmitm_cfg_t;

static usbmitm_cfg_t __usbmitm_cfg;

#define USBMITM_EVT_RESET               (0x200 + 0x1)
#define USBMITM_EVT_SETUP               (0x200 + 0x2)
#define USBMITM_EVT_EPINOUT             (0x200 + 0x10)

#define USBMITM_EVT_EP_MASK             0xF
#define USBMITM_EVT_DIR_MASK            0x100
#define USBMITM_EVT_DIR_IN              0x100
#define USBMITM_EVT_DIR_OUT             0x000
#define USBMITM_EVT_EVT_MASK            ~USBMITM_EVT_EP_MASK
#define USBMITM_EVT_EPIN                (USBMITM_EVT_EPINOUT | USBMITM_EVT_DIR_IN)
#define USBMITM_EVT_EPOUT               (USBMITM_EVT_EPINOUT | USBMITM_EVT_DIR_OUT)
#define USBMITM_EVT_INEP(ep)            (USBMITM_EVT_EPIN + (ep))
#define USBMITM_EVT_OUTEP(ep)           (USBMITM_EVT_EPOUT + (ep))

static vsf_err_t __usbmitm_usbd_ep_recv(usbmitm_urbcb_t *urbcb)
{
    const i_usb_dc_t *drv = __usbmitm_cfg.usbd.drv;
    uint16_t cursize = drv->Ep.GetDataSize(urbcb->ep);
    uint16_t remain = urbcb->totalsize - urbcb->curpos;
    uint16_t epsize = drv->Ep.GetSize(urbcb->ep);

    if (remain > 0) {
        uint8_t *data = (uint8_t *)urbcb->urb.urb_hcd->buffer + urbcb->curpos;

        drv->Ep.Transaction.ReadBuffer(urbcb->ep, data, cursize);
        vsf_trace_info("OUT%d(%d): ", urbcb->ep, cursize);
        vsf_trace_buffer(VSF_TRACE_INFO, data, cursize);
        urbcb->curpos += cursize;
        remain = urbcb->totalsize - urbcb->curpos;
    } else {
        if (!cursize) {
            vsf_trace_info("OUT%d(0): " VSF_TRACE_CFG_LINEEND, urbcb->ep & 0x1F);
        } else {
            vsf_trace_error("Data received without prepared buffer" VSF_TRACE_CFG_LINEEND);
        }
        return VSF_ERR_NONE;
    }
    if ((remain > 0) && (cursize >= epsize)) {
        drv->Ep.Transaction.EnableOut(urbcb->ep);
        return VSF_ERR_NOT_READY;
    }
    return VSF_ERR_NONE;
}

static vsf_err_t __usbmitm_usbd_ep_send(usbmitm_urbcb_t *urbcb)
{
    const i_usb_dc_t *drv = __usbmitm_cfg.usbd.drv;
    uint16_t epsize = drv->Ep.GetSize(urbcb->ep);
    uint16_t remain = urbcb->totalsize - urbcb->curpos;
    uint16_t cursize = vsf_min(epsize, remain);

    if (cursize) {
        uint8_t *data = (uint8_t *)urbcb->urb.urb_hcd->buffer + urbcb->curpos;

        vsf_trace_info("IN%d(%d): ", urbcb->ep & 0x1F, cursize);
        vsf_trace_buffer(VSF_TRACE_INFO, data, cursize, true);
        drv->Ep.Transaction.WriteBuffer(urbcb->ep, data, cursize);
        drv->Ep.Transaction.SetDataSize(urbcb->ep, cursize);

        urbcb->curpos += cursize;
        remain = urbcb->totalsize - urbcb->curpos;
        if (!remain && (cursize < epsize))
            urbcb->needzlp = false;
        return VSF_ERR_NOT_READY;
    } else if (urbcb->needzlp) {
        vsf_trace_info("IN%d(0): " VSF_TRACE_CFG_LINEEND, urbcb->ep & 0x1F);
        urbcb->needzlp = false;
        drv->Ep.Transaction.SetDataSize(urbcb->ep, 0);
        return VSF_ERR_NOT_READY;
    }
    return VSF_ERR_NONE;
}

static vsf_err_t __usbmitm_usbh_prepare_urb(usbmitm_t *usbmitm, usbmitm_urbcb_t *urbcb, uint16_t bufsize)
{
    if (urbcb->submitted) {
        return VSF_ERR_FAIL;
    }
    if (!vk_usbh_urb_is_alloced(&urbcb->urb)) {
        vk_usbh_urb_prepare_by_pipe(&urbcb->urb, usbmitm->usbh.dev,
                __vk_usbh_get_pipe(usbmitm->usbh.dev, urbcb->ep, urbcb->eptype, urbcb->epsize, urbcb->interval));
        vk_usbh_alloc_urb(usbmitm->usbh.host, usbmitm->usbh.dev, &urbcb->urb);
        if (!vk_usbh_urb_is_alloced(&urbcb->urb)) {
            vsf_trace_error("Fail to allocate urb" VSF_TRACE_CFG_LINEEND);
            return VSF_ERR_FAIL;
        }
    }
    vk_usbh_urb_free_buffer(&urbcb->urb);
    if (bufsize && !vk_usbh_urb_alloc_buffer(&urbcb->urb, bufsize)) {
        vsf_trace_error("Fail to allocate transfer buffer" VSF_TRACE_CFG_LINEEND);
        return VSF_ERR_FAIL;
    }
    return VSF_ERR_NONE;
}

static vsf_err_t usbmitm_usbh_submit_urb(usbmitm_t *usbmitm, usbmitm_urbcb_t *urbcb)
{
    vsf_err_t err = VSF_ERR_BUSY;
    if (!urbcb->submitted) {
        err = vk_usbh_submit_urb(usbmitm->usbh.host, &urbcb->urb);
        if (err != VSF_ERR_NONE) {
            vsf_trace_error("Fail to submit urb" VSF_TRACE_CFG_LINEEND);
        } else {
            urbcb->submitted = true;
        }
    }
    return err;
}

static void __usbmitm_usbd_setup_patch(usbmitm_t *usbmitm, struct usb_ctrlrequest_t *request)
{
    uint8_t type = request->bRequestType & USB_TYPE_MASK;
    uint8_t recip = request->bRequestType & USB_RECIP_MASK;

    if (USB_TYPE_STANDARD == type) {
        if (USB_RECIP_DEVICE == recip) {
            switch (request->bRequest) {
            case USB_REQ_SET_ADDRESS:
                usbmitm->usbd.dev_address = request->wValue;
                request->wValue = usbmitm->usbd.host_address;
                break;
            }
        }
    }
}

static void __usbmitm_usbd_setup_process(usbmitm_t *usbmitm, usbmitm_urbcb_t *urbcb)
{
    const i_usb_dc_t *drv = __usbmitm_cfg.usbd.drv;
    struct usb_ctrlrequest_t *request = &usbmitm->usbd.request;
    uint8_t type = request->bRequestType & USB_TYPE_MASK;
    uint8_t recip = request->bRequestType & USB_RECIP_MASK;
    uint8_t *data = urbcb->urb.urb_hcd->buffer;
    uint16_t len = vk_usbh_urb_get_actual_length(&urbcb->urb);

    usbmitm_plugin_t *plugin = __usbmitm_cfg.pluginlist;
    while (plugin != NULL) {
        if (plugin->op->on_SETUP != NULL) {
            plugin->op->on_SETUP(request, URB_OK, data, len);
        }
        plugin = plugin->next;
    }

    if (USB_TYPE_STANDARD == type) {
        if (USB_RECIP_DEVICE == recip) {
            switch (request->bRequest) {
            case USB_REQ_SET_ADDRESS:
                usbmitm->usbh.dev->devnum = usbmitm->usbd.host_address;
                drv->SetAddress(usbmitm->usbd.dev_address);
                break;
            case USB_REQ_GET_DESCRIPTOR:
                switch ((request->wValue >> 8) & 0xFF) {
                case USB_DT_DEVICE:
                    usbmitm->urbcb.in[0].epsize = usbmitm->usbd.ep0size;
                    usbmitm->urbcb.out[0].epsize = usbmitm->usbd.ep0size;
                    break;
                case USB_DT_CONFIG:
                    if (get_unaligned_le16(&data[2]) == len) {
                        if (usbmitm->usbd.config_desc) {
                            vsf_heap_free(usbmitm->usbd.config_desc);
                        }
                        usbmitm->usbd.config_desc = vsf_heap_malloc(len);
                        if (!usbmitm->usbd.config_desc) {
                            break;
                        }
                        memcpy(usbmitm->usbd.config_desc, data, len);

                        plugin = __usbmitm_cfg.pluginlist;
                        while (plugin != NULL) {
                            if (plugin->op->parse_config != NULL) {
                                plugin->op->parse_config(data, len);
                            }
                            plugin = plugin->next;
                        }
                    }
                    break;
                }
                break;
            case USB_REQ_SET_CONFIGURATION:
                if (usbmitm->usbd.config_desc && (request->wValue == usbmitm->usbd.config_desc[5])) {
                    uint16_t pos = USB_DT_CONFIG_SIZE;
                    usb_ep_type_t eptype;
                    uint16_t epsize, epaddr, epindex, epattr, interval;
                    usbmitm_urbcb_t *urbcb;

                    len = get_unaligned_le16(&usbmitm->usbd.config_desc[2]);
                    data = usbmitm->usbd.config_desc;
                    while (len > pos) {
                        switch (data[pos + 1]) {
                        case USB_DT_ENDPOINT:
                            epaddr = data[pos + 2];
                            epattr = data[pos + 3];
                            epsize = get_unaligned_le16(&data[pos + 4]);
                            interval = data[pos + 6];
                            epindex = epaddr & 0x0F;
                            switch (epattr & 0x03) {
                            case 0x00: eptype = USB_EP_TYPE_CONTROL; break;
                            case 0x01: eptype = USB_EP_TYPE_ISO; break;
                            case 0x02: eptype = USB_EP_TYPE_BULK; break;
                            case 0x03: eptype = USB_EP_TYPE_INTERRUPT; break;
                            }

                            urbcb = (epaddr & 0x80) ?
                                        &usbmitm->urbcb.in[epindex] :
                                        &usbmitm->urbcb.out[epindex];
                            if (urbcb->epsize) {
                                // already initialized
                                break;
                            }

                            urbcb->epsize = epsize;
                            urbcb->eptype = eptype;
                            urbcb->interval = interval;
                            drv->Ep.Add(epaddr, eptype, epsize);
                            if (epaddr & 0x80) {
                                if (!__usbmitm_usbh_prepare_urb(usbmitm, urbcb, epsize)) {
                                    usbmitm_usbh_submit_urb(usbmitm, urbcb);
                                    urbcb->ep_inited = true;
                                }
                            } else {
                                drv->Ep.Transaction.EnableOut(epindex);
                            }
                            break;
                        }
                        pos += data[pos];
                    }
                }
                break;
            }
        } else if (USB_RECIP_ENDPOINT == recip) {
            uint8_t ep = request->wIndex & 0xFF;
            uint8_t epdir = request->wIndex * USB_DIR_MASK;

            switch (request->bRequest) {
            case USB_REQ_CLEAR_FEATURE:
                drv->Ep.ClearStall(ep);
                if (USB_DIR_IN == epdir) {
                    usbmitm_usbh_submit_urb(usbmitm, &usbmitm->urbcb.in[ep & 0x1F]);
                } else {
                    drv->Ep.Transaction.EnableOut(ep);
                }
                break;
            }
        }
    }
}

static void __usbmitm_usbd_hal_evthandler(void *p, usb_evt_t evt, uint_fast8_t value)
{
    usbmitm_t *usbmitm = p;
    vsf_eda_t *task = &usbmitm->usbd.task.use_as__vsf_eda_t;

    switch (evt) {
    case USB_ON_RESET:
        vsf_eda_post_evt(task, USBMITM_EVT_RESET);
        break;
    case USB_ON_SETUP:
        vsf_eda_post_evt(task, USBMITM_EVT_SETUP);
        break;
    case USB_ON_IN:
        vsf_eda_post_evt(task, USBMITM_EVT_INEP(value));
        break;
    case USB_ON_OUT:
        vsf_eda_post_evt(task, USBMITM_EVT_OUTEP(value));
        break;
    default:
        break;
    }
}

static void __usbmitm_usbd_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    usbmitm_t *usbmitm = vsf_container_of(eda, usbmitm_t, usbd.task);
    const i_usb_dc_t *drv = __usbmitm_cfg.usbd.drv;
    struct usb_ctrlrequest_t *request = &usbmitm->usbd.request;
    bool isin = (request->bRequestType & USB_DIR_MASK) == USB_DIR_IN;
    usbmitm_urbcb_t *urbcb;
    uint8_t ep;

    vk_usbh_hcd_urb_t *hcd_urb;
    int_fast16_t urb_status;
    uint_fast32_t actual_length;

    switch (evt) {
    case VSF_EVT_ENTER:
    case VSF_EVT_EXIT:
    case VSF_EVT_FINI:
        break;
    case VSF_EVT_INIT:
        drv->Init(&(usb_dc_cfg_t){
            .speed = USB_SPEED_FULL,
            .priority = __usbmitm_cfg.usbd.int_priority,
            .evthandler = __usbmitm_usbd_hal_evthandler,
            .param = &usbmitm,
        });
        drv->Connect();
        break;
    case USBMITM_EVT_RESET:
        if (!usbmitm->resetting) {
            usbmitm->resetting = true;
            vk_usbh_reset_dev(usbmitm->usbh.host, usbmitm->usbh.dev);
            usbmitm->usbh.dev->devnum = 0;
            usbmitm->usbd.request_state = USB_IDLE;
            for (int i = 0; i < dimof(usbmitm->urbcb.all); i++) {
                usbmitm->urbcb.all[i].submitted = false;
                usbmitm->urbcb.all[i].ep_inited = false;
                if (vk_usbh_urb_is_alloced(&usbmitm->urbcb.all[i].urb)) {
                    vk_usbh_free_urb(usbmitm->usbh.host, &usbmitm->urbcb.all[i].urb);
                }
            }
            usbmitm->urbcb.in[0].epsize = usbmitm->urbcb.out[0].epsize = 64;
            usbmitm->urbcb.in[0].eptype = usbmitm->urbcb.out[0].eptype = USB_EP_TYPE_CONTROL;

            drv->Reset(&(usb_dc_cfg_t){
                .speed = USB_SPEED_FULL,
                .priority = __usbmitm_cfg.usbd.int_priority,
                .evthandler = __usbmitm_usbd_hal_evthandler,
                .param = &usbmitm,
            });
            // config ep0
            drv->Ep.Add(0 | USB_DIR_OUT, USB_EP_TYPE_CONTROL, usbmitm->urbcb.out[0].epsize);
            drv->Ep.Add(0 | USB_DIR_IN, USB_EP_TYPE_CONTROL, usbmitm->urbcb.in[0].epsize);
            drv->SetAddress(0);

            vsf_trace_info("Reset" VSF_TRACE_CFG_LINEEND);
        }
        // fall through
    case VSF_EVT_TIMER:
        if (!vk_usbh_is_dev_resetting(usbmitm->usbh.host, usbmitm->usbh.dev)) {
            usbmitm->resetting = false;
            if (usbmitm->pending_urbcb) {
                usbmitm_usbh_submit_urb(usbmitm, usbmitm->pending_urbcb);
                usbmitm->usbd.request_state++;
                usbmitm->pending_urbcb = NULL;
            }
        } else {
            vsf_teda_set_timer_ms(10);
        }
        break;
    case USBMITM_EVT_SETUP:
        if (usbmitm->resetting && usbmitm->pending_urbcb) {
            vsf_trace_info("Setup while resetting" VSF_TRACE_CFG_LINEEND);
            break;
        }
        if (usbmitm->usbd.request_state != USB_IDLE) {
            vsf_trace_info("Setup sequence error" VSF_TRACE_CFG_LINEEND);
            break;
        }
        drv->GetSetup((uint8_t *)&usbmitm->usbd.request);

        isin = (request->bRequestType & USB_DIR_MASK) == USB_DIR_IN;
        urbcb = isin ? &usbmitm->urbcb.in[0] : &usbmitm->urbcb.out[0];

        vsf_trace_info("SETUP: ");
        vsf_trace_buffer(VSF_TRACE_INFO, &usbmitm->usbd.request, sizeof(usbmitm->usbd.request), VSF_TRACE_DF_U8_16_N);

        __usbmitm_usbd_setup_patch(usbmitm, request);
        if (!__usbmitm_usbh_prepare_urb(usbmitm, urbcb, request->wLength)) {
            urbcb->urb.urb_hcd->setup_packet = *request;
            usbmitm->usbd.request_state++;
            if (isin) {
                if (usbmitm->resetting) {
                    usbmitm->pending_urbcb = urbcb;
                } else {
                    usbmitm_usbh_submit_urb(usbmitm, urbcb);
                    usbmitm->usbd.request_state++;
                }
            } else {
                urbcb->curpos = 0;
                urbcb->totalsize = request->wLength;
                if (!urbcb->totalsize) {
                    if (usbmitm->resetting) {
                        usbmitm->pending_urbcb = urbcb;
                    } else {
                        usbmitm_usbh_submit_urb(usbmitm, urbcb);
                        usbmitm->usbd.request_state++;
                    }
                }
            }
        } else {
            vsf_trace_error("Fail to prepare urb" VSF_TRACE_CFG_LINEEND);
        }
        break;
    case VSF_EVT_MESSAGE:
        hcd_urb = (vk_usbh_hcd_urb_t *)vsf_eda_get_cur_msg();
        urbcb = hcd_urb->pipe.dir_in1out0 ? usbmitm->urbcb.in : usbmitm->urbcb.out;
        urbcb += hcd_urb->pipe.endpoint;

        urb_status = vk_usbh_urb_get_status(&urbcb->urb);
        actual_length = vk_usbh_urb_get_actual_length(&urbcb->urb);
        urbcb->submitted = false;
        if (!(urbcb->ep & 0x1F)) {
            struct usb_ctrlrequest_t *request = &usbmitm->usbd.request;
            bool isin = (request->bRequestType & USB_DIR_MASK) == USB_DIR_IN;

            usbmitm->usbd.request_state++;
            if (urb_status == URB_OK) {
                urbcb->curpos = 0;
                if (isin) {
                    uint16_t epsize = drv->Ep.GetSize(urbcb->ep);
                    urbcb->totalsize = actual_length;
                    urbcb->needzlp = urbcb->totalsize > epsize;
                    __usbmitm_usbd_ep_send(urbcb);
                } else {
                    usbmitm->usbd.request_state++;
                    urbcb->totalsize = 0;
                    urbcb->needzlp = true;
                    __usbmitm_usbd_ep_send(urbcb);
                }
            } else {
                // urb failed, stall ep0
                vsf_trace_info("IN0: STALL" VSF_TRACE_CFG_LINEEND);
                drv->Ep.SetStall(0 | USB_DIR_IN);
                drv->Ep.SetStall(0 | USB_DIR_OUT);
                usbmitm->usbd.request_state = USB_IDLE;

                usbmitm_plugin_t *plugin = __usbmitm_cfg.pluginlist;
                while (plugin != NULL) {
                    if (plugin->op->on_SETUP != NULL) {
                        plugin->op->on_SETUP(request, urb_status, NULL, 0);
                    }
                    plugin = plugin->next;
                }
            }
        } else {
            if ((urbcb->ep & USB_DIR_MASK) == USB_DIR_IN) {
                if (urb_status != URB_OK) {
                    vsf_trace_info("IN%d: STALL" VSF_TRACE_CFG_LINEEND, urbcb->ep & 0x1F);
                    drv->Ep.SetStall(urbcb->ep);

                    usbmitm_plugin_t *plugin = __usbmitm_cfg.pluginlist;
                    while (plugin != NULL) {
                        if (plugin->op->on_IN != NULL) {
                            plugin->op->on_IN(urbcb->ep & 0x1F, urb_status, NULL, 0);
                        }
                        plugin = plugin->next;
                    }
                } else {
                    urbcb->curpos = 0;
                    urbcb->totalsize = actual_length;
                    urbcb->needzlp = urbcb->totalsize < urbcb->urb.urb_hcd->transfer_length;
                    __usbmitm_usbd_ep_send(urbcb);
                }
            } else {
                if (urb_status == URB_OK) {
                    drv->Ep.Transaction.EnableOut(urbcb->ep);
                } else {
                    vsf_trace_info("OUT%d: STALL" VSF_TRACE_CFG_LINEEND, urbcb->ep & 0x1F);
                    drv->Ep.SetStall(urbcb->ep);

                    usbmitm_plugin_t *plugin = __usbmitm_cfg.pluginlist;
                    while (plugin != NULL) {
                        if (plugin->op->on_OUT != NULL) {
                            plugin->op->on_OUT(urbcb->ep & 0x1F, urb_status, NULL, 0);
                        }
                        plugin = plugin->next;
                    }
                }
            }
        }
        break;
    default:
        ep = evt & USBMITM_EVT_EP_MASK;
        evt &= USBMITM_EVT_EVT_MASK;
        if (usbmitm->resetting && (ep || !usbmitm->pending_urbcb || (evt != USBMITM_EVT_EPOUT))) {
            vsf_trace_error("Transaction while resetting" VSF_TRACE_CFG_LINEEND);
            break;
        } else {
            switch (evt) {
            case USBMITM_EVT_EPIN:
                urbcb = &usbmitm->urbcb.in[ep];
                if (!ep) {
                    switch (usbmitm->usbd.request_state) {
                    case URB_COMPLETED:
                        // sending reply
                        if (!isin) {
                            vsf_trace_error("Setup sequence error" VSF_TRACE_CFG_LINEEND);
                            break;
                        }
                        if (!__usbmitm_usbd_ep_send(urbcb)) {
                            usbmitm->usbd.request_state++;
                            drv->Ep.Transaction.EnableOut(0);
                        }
                        break;
                    case USB_STATUS:
                        // status sent
                        if (isin) {
                            vsf_trace_error("Setup sequence error" VSF_TRACE_CFG_LINEEND);
                            break;
                        }
                        urbcb = &usbmitm->urbcb.out[0];
                        __usbmitm_usbd_setup_process(usbmitm, urbcb);
                        usbmitm->usbd.request_state = USB_IDLE;
                        break;
                    default:
                        break;
                    }
                } else if (!__usbmitm_usbd_ep_send(urbcb)) {
                    // data sent, submit next urb
                    usbmitm_plugin_t *plugin = __usbmitm_cfg.pluginlist;
                    uint8_t *data = urbcb->urb.urb_hcd->buffer;
                    uint16_t len = vk_usbh_urb_get_actual_length(&urbcb->urb);
                    while (plugin != NULL) {
                        if (plugin->op->on_IN != NULL) {
                            plugin->op->on_IN(urbcb->ep & 0x1F, URB_OK, data, len);
                        }
                        plugin = plugin->next;
                    }

                    usbmitm_usbh_submit_urb(usbmitm, urbcb);
                }
                break;
            case USBMITM_EVT_EPOUT:
                urbcb = &usbmitm->urbcb.out[ep];
                if (!ep) {
                    switch (usbmitm->usbd.request_state) {
                    case USB_SETUP:
                        // receiving data
                        if (isin) {
                            vsf_trace_error("Setup sequence error" VSF_TRACE_CFG_LINEEND);
                            break;
                        }
                        if (!__usbmitm_usbd_ep_recv(urbcb)) {
                            if (usbmitm->resetting) {
                                usbmitm->pending_urbcb = urbcb;
                            } else {
                                usbmitm_usbh_submit_urb(usbmitm, urbcb);
                                usbmitm->usbd.request_state++;
                            }
                        }
                        break;
                    case USB_STATUS:
                        // status received
                        if (!isin) {
                            vsf_trace_error("Setup sequence error" VSF_TRACE_CFG_LINEEND);
                            break;
                        }
                        urbcb->totalsize = urbcb->curpos = 0;
                        __usbmitm_usbd_ep_recv(urbcb);
                        urbcb = &usbmitm->urbcb.in[0];
                        __usbmitm_usbd_setup_process(usbmitm, urbcb);
                        usbmitm->usbd.request_state = USB_IDLE;
                        break;
                    default:
                        break;
                    }
                } else {
                    // data received, submit urb
                    urbcb->curpos = 0;
                    urbcb->totalsize = drv->Ep.GetDataSize(ep);
                    if (!__usbmitm_usbh_prepare_urb(usbmitm, urbcb, urbcb->totalsize)) {
                        __usbmitm_usbd_ep_recv(urbcb);

                        usbmitm_plugin_t *plugin = __usbmitm_cfg.pluginlist;
                        uint8_t *data = urbcb->urb.urb_hcd->buffer;
                        uint16_t len = urbcb->totalsize;
                        while (plugin != NULL) {
                            if (plugin->op->on_OUT != NULL) {
                                plugin->op->on_OUT(urbcb->ep & 0x1F, URB_OK, data, len);
                            }
                            plugin = plugin->next;
                        }

                        usbmitm_usbh_submit_urb(usbmitm, urbcb);
                        urbcb->ep_inited = true;
                    } else {
                        vsf_trace_error("Fail to prepare urb" VSF_TRACE_CFG_LINEEND);
                    }
                }
                break;
            }
        }
    }
}

static void *__vsf_usbh_usbmitm_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs)
{
    usbmitm_t *usbmitm = vsf_usbh_malloc(sizeof(usbmitm_t));
    if (usbmitm != NULL) {
        memset(usbmitm, 0, sizeof(*usbmitm));

        usbmitm_urbcb_t *urbcb;
        for (int i = 0; i < dimof(usbmitm->urbcb.in); i++) {
            urbcb = &usbmitm->urbcb.in[i];
            urbcb->ep = i | USB_DIR_IN;

            urbcb = &usbmitm->urbcb.out[i];
            urbcb->ep = i | USB_DIR_OUT;
        }

        usbmitm->usbd.host_address = dev->devnum;
        usbmitm->usbh.host = usbh;
        usbmitm->usbh.dev = dev;
        usbmitm->usbd.task.fn.evthandler = __usbmitm_usbd_evthandler;
        vsf_teda_init(&usbmitm->usbd.task);
        return usbmitm;
    }
    return NULL;
}

static void __vsf_usbh_usbmitm_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param)
{
    usbmitm_t *usbmitm = param;

    if (usbmitm->usbh.dev == dev) {
        const i_usb_dc_t *drv = __usbmitm_cfg.usbd.drv;

        for (int i = 0; i < dimof(usbmitm->urbcb.all); i++) {
            if (vk_usbh_urb_is_alloced(&usbmitm->urbcb.all[i].urb)) {
                vk_usbh_free_urb(usbh, &usbmitm->urbcb.all[i].urb);
            }
        }
        if (usbmitm->usbd.config_desc != NULL) {
            vsf_heap_free(usbmitm->usbd.config_desc);
            usbmitm->usbd.config_desc = NULL;
        }
        drv->Disconnect();
        drv->Fini();
        __vsf_teda_cancel_timer(&usbmitm->usbd.task);
        vsf_eda_fini(&usbmitm->usbd.task.use_as__vsf_eda_t);
        usbmitm->usbh.dev = NULL;
    }
}

static const vk_usbh_dev_id_t __vsf_usbh_usbmitm_devid[] = {
    {
        .match_flags = VSF_USBH_MATCH_FLAGS_DEV_LO,
        .bcdDevice_lo = 0,
        .bDeviceClass = 1,
    },
};

const vk_usbh_class_drv_t vsf_usbh_usbmitm_drv = {
    .name = "usbmitm",
    .dev_id_num = dimof(__vsf_usbh_usbmitm_devid),
    .dev_ids = __vsf_usbh_usbmitm_devid,
    .probe = __vsf_usbh_usbmitm_probe,
    .disconnect = __vsf_usbh_usbmitm_disconnect,
};

void usbmitm_register_plugin(usbmitm_plugin_t *plugin)
{
    plugin->next = __usbmitm_cfg.pluginlist;
    __usbmitm_cfg.pluginlist = plugin;
}

void usbmitm_init(const i_usb_dc_t *drv, int32_t int_priority)
{
    __usbmitm_cfg.usbd.drv = drv;
    __usbmitm_cfg.usbd.int_priority = int_priority;
}
