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

#define __VSF_USBH_CLASS_IMPLEMENT_CLASS__
#define __VSF_USBD_CLASS_INHERIT__
#define __VSF_EDA_CLASS_INHERIT__
#include "./vsf_usbmitm.h"

/*============================ MACROS ========================================*/

#ifndef VSF_USB_MITM_CFG_MAX_TRANSFER_SIZE
#   define VSF_USB_MITM_CFG_MAX_TRANSFER_SIZE           1024
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    VSF_EVT_USBD_ON_RESET           = VSF_EVT_USER + 0,
    VSF_EVT_USBD_ON_SETUP           = VSF_EVT_USER + 1,
    VSF_EVT_USBD_ON_STATUS_QUERY    = VSF_EVT_USER + 2,
    VSF_EVT_USBD_ON_STATUS          = VSF_EVT_USER + 3,
    VSF_EVT_USBD_ON_EP              = VSF_EVT_USER + 4,
};

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

static void __vsf_usb_mitm_libusb_evthandler(void *param, vk_usbh_libusb_dev_t *dev, vk_usbh_libusb_evt_t evt)
{
    vsf_usb_mitm_t *mitm = param;

    switch (evt) {
    case VSF_USBH_LIBUSB_EVT_ON_ARRIVED:
        if (NULL == mitm->usbh.libusb_dev) {
            mitm->usbh.libusb_dev = dev;
            vk_usbd_connect(&mitm->usb_dev);
        }
        break;
    case VSF_USBH_LIBUSB_EVT_ON_LEFT:
        if (mitm->usbh.libusb_dev == dev) {
            vk_usbd_disconnect(&mitm->usb_dev);
            vk_usbd_fini(&mitm->usb_dev);

            for (int i = 0; i < dimof(mitm->config_desc); i++) {
                if (mitm->config_desc[i] != NULL) {
                    vsf_heap_free(mitm->config_desc[i]);
                    mitm->config_desc[i] = NULL;
                }
            }
            mitm->cur_config_desc = NULL;
        }
        break;
    }
}

static vsf_err_t __vsf_usb_mitm_usbh_control_msg(vsf_usb_mitm_t *mitm)
{
    VSF_ASSERT(!mitm->usbh.is_control_requesting);
    vsf_err_t err = vk_usbh_control_msg(&mitm->usb_host, mitm->usbh.libusb_dev->dev, &mitm->request);
    if (err != VSF_ERR_NONE) {
        vsf_trace_error("usbh: fail to submit control message\n");
    } else {
        mitm->usbh.is_control_requesting = true;
    }
    return err;
}

static vsf_err_t __vsf_usb_mitm_usbd_ep_recv(vk_usbd_dev_t *dev, vsf_usb_mitm_trans_t *trans)
{
    trans->mem_save.buffer = trans->buffer;
    trans->mem_save.size = trans->size;
    return vk_usbd_ep_recv(dev, &trans->use_as__vk_usbd_trans_t);
}

static vsf_err_t __vsf_usb_mitm_usbd_ep_send(vk_usbd_dev_t *dev, vsf_usb_mitm_trans_t *trans)
{
    trans->mem_save.buffer = trans->buffer;
    trans->mem_save.size = trans->size;
    return vk_usbd_ep_send(dev, &trans->use_as__vk_usbd_trans_t);
}

static void __vsf_usb_mitm_notify_user(vsf_usb_mitm_t *mitm, vsf_usb_mitm_evt_t evt, void *param)
{
    if (mitm->callback != NULL) {
        if ((evt == USB_ON_IN) || (evt == USB_ON_OUT) || (evt == USB_ON_PREPARE_DATA)) {
            vk_usbd_trans_t *trans = param;
            if (!(trans->ep & 0x7F)) {
                mitm->usbd.control_trans.use_as__vk_usbd_trans_t = *trans;
                param = &mitm->usbd.control_trans;
            }
        }
        mitm->callback(mitm, evt, param);
    }
}

vsf_err_t vsf_usbd_notify_user(vk_usbd_dev_t *dev, usb_evt_t evt, void *param)
{
    vsf_usb_mitm_t *mitm = container_of(dev, vsf_usb_mitm_t, usb_dev);

    __vsf_usb_mitm_notify_user(mitm, (vsf_usb_mitm_evt_t)evt, param);
    switch (evt) {
    case USB_ON_RESET:
        vsf_eda_post_evt(&mitm->teda.use_as__vsf_eda_t, VSF_EVT_USBD_ON_RESET);
        break;
    case USB_ON_SETUP:
        mitm->request = *(struct usb_ctrlrequest_t *)param;
        vsf_eda_post_evt(&mitm->teda.use_as__vsf_eda_t, VSF_EVT_USBD_ON_SETUP);
        return VSF_ERR_NOT_READY;
    case USB_ON_STATUS:
        vsf_eda_post_evt(&mitm->teda.use_as__vsf_eda_t, VSF_EVT_USBD_ON_STATUS);
        break;
    case USB_ON_IN:
    case USB_ON_OUT: {
            vk_usbd_trans_t *trans = param;
            if (trans->ep & 0x0F) {
                vsf_eda_post_evt(&mitm->teda.use_as__vsf_eda_t, VSF_EVT_USBD_ON_EP + trans->ep);
            }
        }
        break;
    case USB_ON_STATUS_QUERY:
        vsf_eda_post_evt(&mitm->teda.use_as__vsf_eda_t, VSF_EVT_USBD_ON_STATUS_QUERY);
        return VSF_ERR_NOT_READY;
    }
    return VSF_ERR_NONE;
}

#define __usb_desc_next_header(__ptr)                                           \
    (struct usb_descriptor_header_t *)((uint8_t *)(__ptr) + ((struct usb_descriptor_header_t *)(__ptr))->bLength)

static vsf_err_t __vsf_usb_mitm_apply_interface(vsf_usb_mitm_t *mitm, struct usb_interface_desc_t *ifs_desc, uint8_t *end)
{
    struct usb_endpoint_desc_t *ep_desc;

    for (   struct usb_descriptor_header_t *header = __usb_desc_next_header(ifs_desc);
            (uint8_t *)header < end;
            header = __usb_desc_next_header(header)) {
        switch (header->bDescriptorType) {
        case USB_DT_INTERFACE:
            return VSF_ERR_NONE;
        case USB_DT_ENDPOINT:
            ep_desc = (struct usb_endpoint_desc_t *)header;

            bool is_in = (ep_desc->bEndpointAddress & USB_DIR_MASK) == USB_DIR_IN;
            VSF_ASSERT((ep_desc->bEndpointAddress & 0x0F) != 0);
            uint8_t idx = ((ep_desc->bEndpointAddress & 0x0F) - 1) + (is_in ? 15 : 0);
            vsf_usb_mitm_urb_t *mitm_urb = &mitm->usbh.urb[idx];
            vsf_usb_mitm_trans_t *mitm_trans = &mitm->usbd.trans[idx];

            uint8_t ep_idx = (ep_desc->bEndpointAddress & 0x0F) + (is_in ? 16 : 0);
            if (!(mitm->usbd.ep_mask & (1 << ep_idx))) {
                VSF_USBD_DRV_PREPARE(&mitm->usb_dev);
                vk_usbd_drv_ep_add(ep_desc->bEndpointAddress,
                    ep_desc->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK, ep_desc->wMaxPacketSize);
                mitm->usbd.ep_mask |= 1 << ep_idx;
                mitm_trans->ep = ep_desc->bEndpointAddress;
            }

            uint16_t urb_size;
            uint8_t urb_num;
            switch (ep_desc->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) {
            case USB_ENDPOINT_XFER_CONTROL: VSF_ASSERT(false); break;
            case USB_ENDPOINT_XFER_INT:
            case USB_ENDPOINT_XFER_ISOC:    urb_num = VSF_USB_MITM_USBH_URB_NUM;
                                            urb_size = ep_desc->wMaxPacketSize; break;
            case USB_ENDPOINT_XFER_BULK:    urb_num = 1;
                                            urb_size = VSF_USB_MITM_CFG_MAX_TRANSFER_SIZE; break;
            }

            VSF_FIFO_INIT(&mitm_urb->ready_fifo);

            bool is_to_break = false;
            vk_usbh_urb_t *urb;
            for (int i = 0; i < urb_num; i++) {
                urb = &mitm_urb->usbh_urb[i];
                if (vk_usbh_urb_is_alloced(urb)) {
                    is_to_break = true;
                    break;
                }
                vk_usbh_urb_prepare(urb, mitm->usbh.libusb_dev->dev, ep_desc);
                if (VSF_ERR_NONE != vk_usbh_alloc_urb(&mitm->usb_host, mitm->usbh.libusb_dev->dev, urb)) {
                    vsf_trace_error("usbh: fail to allocate urb for %s%d\n",
                            is_in ? "IN" : "OUT", ep_desc->bEndpointAddress & 0x0F);
                    is_to_break = true;
                    break;
                }

                switch (ep_desc->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) {
                case USB_ENDPOINT_XFER_INT:
                case USB_ENDPOINT_XFER_ISOC:
                    urb->urb_hcd->interval = ep_desc->bInterval;
                    break;
                }

                if (NULL == vk_usbh_urb_alloc_buffer(urb, urb_size)) {
                    vsf_trace_error("usbh: fail to allocate urb buffer for %s%d\n",
                            is_in ? "IN" : "OUT", ep_desc->bEndpointAddress & 0x0F);
                    is_to_break = true;
                    break;
                }

                if (is_in) {
                    if ((ep_desc->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_ISOC) {
                        if (VSF_ERR_NONE != vk_usbh_submit_urb_iso(&mitm->usb_host, urb, 0)) {
                            vsf_trace_error("usbh: fail to submit urb for %s%d\n",
                                    is_in ? "IN" : "OUT", ep_desc->bEndpointAddress & 0x0F);
                            is_to_break = true;
                            break;
                        }
                    } else {
                        if (VSF_ERR_NONE != vk_usbh_submit_urb(&mitm->usb_host, urb)) {
                            vsf_trace_error("usbh: fail to submit urb for %s%d\n",
                                    is_in ? "IN" : "OUT", ep_desc->bEndpointAddress & 0x0F);
                            is_to_break = true;
                            break;
                        }
                    }
                }
            }
            if (is_to_break) {
                break;
            }

            if (!is_in) {
                for (int i = 0; i < urb_num; i++) {
                    urb = &mitm_urb->usbh_urb[i];
                    VSF_FIFO_PUSH(&mitm_urb->ready_fifo, &urb);
                }
                mitm_trans->buffer = vsf_usbh_malloc(urb_size);
                mitm_trans->size = urb_size;
                if (VSF_ERR_NONE != __vsf_usb_mitm_usbd_ep_recv(&mitm->usb_dev, mitm_trans)) {
                    vsf_trace_error("usbd: fail to recv from %s%d\n",
                            is_in ? "IN" : "OUT", ep_desc->bEndpointAddress & 0x0F);
                    break;
                }
                mitm->usbd.trans_busy |= 1 << idx;
            }
            break;
        }
    }
    return VSF_ERR_NONE;
}

static vsf_err_t __vsf_usb_mitm_apply_config(vsf_usb_mitm_t *mitm, struct usb_config_desc_t *config_desc)
{
    uint8_t *end = (uint8_t *)config_desc + config_desc->wTotalLength;
    struct usb_interface_desc_t *ifs_desc_prev = NULL, *ifs_desc;
    uint8_t ifs_num = 0xFF, ifs_cnt = 0;

    for (   struct usb_descriptor_header_t *header = __usb_desc_next_header(config_desc);
            (uint8_t *)header < end;
            header = __usb_desc_next_header(header)) {
        switch (header->bDescriptorType) {
        case USB_DT_INTERFACE:
            ifs_desc = (struct usb_interface_desc_t *)header;
            if (0xFF == ifs_num) {
            update_ifs:
                ifs_num = ifs_desc->bInterfaceNumber;
                ifs_desc_prev = ifs_desc;
                ifs_cnt = 0;
                break;
            }
            if (ifs_num != ifs_desc->bInterfaceNumber) {
                if (ifs_cnt) {
                    goto update_ifs;
                }

                VSF_ASSERT(ifs_desc_prev != NULL);
                __vsf_usb_mitm_apply_interface(mitm, ifs_desc_prev, end);
                goto update_ifs;
            } else if (ifs_num == ifs_desc->bInterfaceNumber) {
                ifs_cnt++;
                break;
            }
            break;
        }
    }
    // apply the last interface
    if ((ifs_desc_prev != NULL) && !ifs_cnt) {
        __vsf_usb_mitm_apply_interface(mitm, ifs_desc_prev, end);
    }
    return VSF_ERR_NONE;
}

static vsf_err_t __vsf_usb_mitm_apply_interface_alt(vsf_usb_mitm_t *mitm, uint8_t ifs, uint8_t alt)
{
    struct usb_config_desc_t *config_desc = (struct usb_config_desc_t *)mitm->cur_config_desc;
    uint8_t *end = (uint8_t *)config_desc + config_desc->wTotalLength;
    struct usb_interface_desc_t *ifs_desc;

    for (   struct usb_descriptor_header_t *header = __usb_desc_next_header(config_desc);
            (uint8_t *)header < end;
            header = __usb_desc_next_header(header)) {
        switch (header->bDescriptorType) {
        case USB_DT_INTERFACE:
            ifs_desc = (struct usb_interface_desc_t *)header;
            if ((ifs == ifs_desc->bInterfaceNumber) && (alt == ifs_desc->bAlternateSetting)) {
                return __vsf_usb_mitm_apply_interface(mitm, ifs_desc, end);
            }
            break;
        }
    }
    return VSF_ERR_NONE;
}

static void __vsf_usb_mitm_host_on_stdreq(vsf_usb_mitm_t *mitm, uint8_t *buffer, uint_fast32_t actual_length)
{
    if ((mitm->request.bRequestType & USB_TYPE_MASK) == USB_TYPE_STANDARD) {
        if ((mitm->request.bRequestType & USB_RECIP_MASK) == USB_RECIP_DEVICE) {
            switch (mitm->request.bRequest) {
            case USB_REQ_SET_ADDRESS:
                vk_usbh_update_address(mitm->usbh.libusb_dev->dev, mitm->usbh.libusb_dev->dev->devnum);
                break;
            case USB_REQ_GET_DESCRIPTOR:
                switch (mitm->request.wValue >> 8) {
                case USB_DT_CONFIG: {
                        struct usb_config_desc_t *config_desc = (struct usb_config_desc_t *)buffer;
                        if (    (config_desc->wTotalLength == actual_length)
                            &&  (mitm->request.wIndex < dimof(mitm->config_desc))) {
                            mitm->config_desc[mitm->request.wIndex] = vsf_heap_realloc(mitm->config_desc[mitm->request.wIndex], actual_length);
                            if (NULL == mitm->config_desc[mitm->request.wIndex]) {
                                vsf_trace_error("usb_mitm: fail to allocate config_desc buffer\n");
                                break;
                            }

                            memcpy(mitm->config_desc[mitm->request.wIndex], config_desc, actual_length);
                        }
                    }
                    break;
                }
                break;
            case USB_REQ_SET_CONFIGURATION:
                for (int i = 0; i < dimof(mitm->config_desc); i++) {
                    if (mitm->request.wValue == mitm->config_desc[i][5]) {
                        mitm->cur_config_desc = mitm->config_desc[i];
                        break;
                    }
                }

                if (NULL == mitm->cur_config_desc) {
                    vsf_trace_error("usb_mitm: fail to get config_desc %d\n", mitm->request.wValue);
                    break;
                }

                if (VSF_ERR_NONE != __vsf_usb_mitm_apply_config(mitm, (struct usb_config_desc_t *)mitm->cur_config_desc)) {
                    vsf_trace_error("usb_mitm: fail to apply config_desc %d\n", mitm->request.wValue);
                    break;
                }
                break;
            }
        } else if ((mitm->request.bRequestType & USB_RECIP_MASK) == USB_RECIP_INTERFACE) {
            switch (mitm->request.bRequest) {
            case USB_REQ_SET_INTERFACE:
                if (VSF_ERR_NONE != __vsf_usb_mitm_apply_interface_alt(mitm, mitm->request.wIndex, mitm->request.wValue)) {
                    vsf_trace_error("usb_mitm: fail to apply interface %d alt %d\n", mitm->request.wIndex, mitm->request.wValue);
                    break;
                }
                break;
            }
        }
    }
}

static void __vsf_usb_mitm_update_ep(vsf_usb_mitm_t *mitm, uint8_t idx)
{
    vsf_usb_mitm_urb_t *mitm_urb = &mitm->usbh.urb[idx];
    vsf_usb_mitm_trans_t *mitm_trans = &mitm->usbd.trans[idx];
    bool is_in = idx >= 15;

    if (    !(mitm->usbd.trans_busy & (1 << idx))
        &&  (VSF_FIFO_GET_NUMBER(&mitm_urb->ready_fifo) > 0)) {
        vk_usbh_urb_t *urb;
        VSF_FIFO_POP(&mitm_urb->ready_fifo, &urb);

        uint_fast32_t actual_length = vk_usbh_urb_get_actual_length(urb);
        uint_fast32_t transfer_length = urb->urb_hcd->transfer_length;
        void *buffer = vk_usbh_urb_take_buffer(urb);
        vk_usbh_pipe_t pipe = vk_usbh_urb_get_pipe(urb);

        if (mitm_trans->buffer != NULL) {
            if (is_in) {
                vk_usbh_urb_set_buffer(urb, mitm_trans->buffer, transfer_length);
            } else {
                vk_usbh_urb_set_buffer(urb, mitm_trans->buffer, mitm_trans->mem_save.size - mitm_trans->size);
            }
        } else if (NULL == vk_usbh_urb_alloc_buffer(urb, transfer_length)) {
            vsf_trace_error("usbh: fail to alloc buffer for urb%d\n", idx);
            return;
        }

        if (pipe.type == USB_ENDPOINT_XFER_ISOC) {
            if (VSF_ERR_NONE != vk_usbh_submit_urb_iso(&mitm->usb_host, urb, 0)) {
                vsf_trace_error("usbh: fail to submit urb%d\n", idx);
                return;
            }
        } else {
            if (VSF_ERR_NONE != vk_usbh_submit_urb(&mitm->usb_host, urb)) {
                vsf_trace_error("usbh: fail to submit urb%d\n", idx);
                return;
            }
        }

        uint16_t urb_size;
        switch (pipe.type) {
        case USB_ENDPOINT_XFER_CONTROL: VSF_ASSERT(false); break;
        case USB_ENDPOINT_XFER_INT:
        case USB_ENDPOINT_XFER_ISOC:    urb_size = pipe.size; break;
        case USB_ENDPOINT_XFER_BULK:    urb_size = VSF_USB_MITM_CFG_MAX_TRANSFER_SIZE; break;
        }
        mitm_trans->size = is_in ? actual_length : urb_size;
        mitm_trans->buffer = buffer;

        vsf_err_t err = is_in ?
                    __vsf_usb_mitm_usbd_ep_send(&mitm->usb_dev, mitm_trans) :
                    __vsf_usb_mitm_usbd_ep_recv(&mitm->usb_dev, mitm_trans);
        if (err != VSF_ERR_NONE) {
            vsf_trace_error("usbd: fail to submit trans%d\n", idx);
            return;
        }
        mitm->usbd.trans_busy |= 1 << idx;
    }
}

static void __vsf_usb_mitm_evthadler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vsf_usb_mitm_t *mitm = container_of(eda, vsf_usb_mitm_t, teda);
    vk_usbh_dev_t *usbh_dev = mitm->usbh.libusb_dev->dev;

    switch (evt) {
    case VSF_EVT_INIT:
        memset(mitm->config_desc, 0, sizeof(mitm->config_desc));
        memset(mitm->ifs_desc_cur, 0, sizeof(mitm->ifs_desc_cur));
        mitm->cur_config_desc = NULL;
        mitm->is_setup_pending = false;
        memset(&mitm->usbh, 0, sizeof(mitm->usbh));
        memset(&mitm->usbd, 0, sizeof(mitm->usbd));

        mitm->usb_dev.ctrl_handler.trans.buffer = NULL;
        vk_usbd_init(&mitm->usb_dev);
        vk_usbd_disconnect(&mitm->usb_dev);

        mitm->usbh.libusb.drv = &vk_usbh_libusb_drv;
        vk_usbh_libusb_set_evthandler(mitm, __vsf_usb_mitm_libusb_evthandler);
#if VSF_USBH_USE_HUB == ENABLED
        mitm->usbh.hub.drv = &vk_usbh_hub_drv;
#endif

        vk_usbh_init(&mitm->usb_host);
        vk_usbh_register_class(&mitm->usb_host, &mitm->usbh.libusb);
#if VSF_USBH_USE_HUB == ENABLED
        vk_usbh_register_class(&mitm->usb_host, &mitm->usbh.hub);
#endif
        break;
    case VSF_EVT_TIMER:
        if (mitm->usbh.is_resetting) {
            if (vk_usbh_is_dev_resetting(&mitm->usb_host, usbh_dev)) {
                vsf_teda_set_timer_ms(10);
                return;
            }

            mitm->usbh.is_resetting = false;
            if (mitm->is_setup_pending) {
            process_pending_setup:
                mitm->is_setup_pending = false;
                if (VSF_ERR_NONE != __vsf_usb_mitm_usbh_control_msg(mitm)) {
                    vk_usbd_ep_stall(&mitm->usb_dev, 0);
                }
            }
        } else {
            VSF_ASSERT(false);
        }
        break;
    case VSF_EVT_MESSAGE: {
            vk_usbh_urb_t urb = { .urb_hcd = vsf_eda_get_cur_msg() };
            vk_usbh_pipe_t pipe = vk_usbh_urb_get_pipe(&urb);
            int_fast16_t status = vk_usbh_urb_get_status(&urb);
            uint_fast32_t actual_length = vk_usbh_urb_get_actual_length(&urb);

            if (0 == pipe.endpoint) {
                bool is_out_with_data = ((mitm->request.bRequestType & USB_DIR_MASK) == USB_DIR_OUT)
                                    &&  (mitm->request.wLength > 0);
                uint8_t *buffer = vk_usbh_urb_take_buffer(&urb);

                VSF_ASSERT(mitm->usbh.is_control_requesting);
                mitm->usbh.is_control_requesting = false;
                if (status != URB_OK) {
                    vk_usbd_ep_stall(&mitm->usb_dev, 0);
                    vsf_usbh_free(buffer);
                } else {
                    if (!is_out_with_data) {
                        mitm->usb_dev.ctrl_handler.trans.buffer = buffer;
                        mitm->usb_dev.ctrl_handler.trans.size = actual_length;
                        mitm->usbd.control_trans.mem_save = mitm->usb_dev.ctrl_handler.trans.use_as__vsf_mem_t;
                        __vsf_usb_mitm_notify_user(mitm, USB_ON_PREPARE_DATA, &mitm->usb_dev.ctrl_handler.trans);
                        __vsf_usb_mitm_host_on_stdreq(mitm, buffer, actual_length);
                        vk_usbd_stdreq_data_stage(&mitm->usb_dev);
                    } else {
                        __vsf_usb_mitm_host_on_stdreq(mitm, buffer, actual_length);
                        vsf_usbh_free(buffer);
                        vk_usbd_stdreq_status_stage(&mitm->usb_dev);
                    }
                }
                if (mitm->is_setup_pending) {
                    goto process_pending_setup;
                }
            } else {
                VSF_ASSERT(pipe.endpoint != 0);
                uint8_t idx = (pipe.endpoint - 1) + (!!pipe.dir_in1out0 ? 15 : 0);
                vk_usbh_urb_t *usbh_urb = NULL;

                for (int i = 0; i < VSF_USB_MITM_USBH_URB_NUM; i++) {
                    if (mitm->usbh.urb[idx].usbh_urb[i].urb_hcd == urb.urb_hcd) {
                        usbh_urb = &mitm->usbh.urb[idx].usbh_urb[i];
                        break;
                    }
                }
                VSF_ASSERT(usbh_urb != NULL);

                VSF_FIFO_PUSH(&mitm->usbh.urb[idx].ready_fifo, &usbh_urb);
                if (URB_OK == status) {
                    __vsf_usb_mitm_update_ep(mitm, idx);
                }
            }
        }
        break;
    case VSF_EVT_USBD_ON_RESET:
        if (!mitm->usbh.is_resetting) {
            mitm->usbh.is_resetting = true;
            vk_usbh_reset_dev(&mitm->usb_host, usbh_dev);

            if (mitm->usb_dev.ctrl_handler.trans.buffer != NULL) {
                vsf_usbh_free(mitm->usb_dev.ctrl_handler.trans.buffer);
                mitm->usb_dev.ctrl_handler.trans.buffer = NULL;
            }
            for (int i = 0; i < dimof(mitm->usbd.trans); i++) {
                if (mitm->usbd.trans[i].buffer != NULL) {
                    vsf_usbh_free(mitm->usbd.trans[i].buffer);
                    mitm->usbd.trans[i].buffer = NULL;
                }
            }

            VSF_USBD_DRV_PREPARE(&mitm->usb_dev);
            vk_usbd_drv_ep_add(0 | USB_DIR_OUT, USB_EP_TYPE_CONTROL, mitm->usbh.libusb_dev->ep0size);
            vk_usbd_drv_ep_add(0 | USB_DIR_IN, USB_EP_TYPE_CONTROL, mitm->usbh.libusb_dev->ep0size);
            mitm->usbd.ep_mask = 0x00010001;

            vsf_teda_set_timer_ms(10);
        }
        break;
    case VSF_EVT_USBD_ON_SETUP:
        // patch address for SET_ADDRESS request
        if (    ((mitm->request.bRequestType & USB_TYPE_MASK) == USB_TYPE_STANDARD)
            &&  ((mitm->request.bRequestType & USB_RECIP_MASK) == USB_RECIP_DEVICE)
            &&  (mitm->request.bRequest == USB_REQ_SET_ADDRESS)) {
            mitm->usbd.address = (uint8_t)mitm->request.wValue;
            mitm->request.wValue = mitm->usbh.libusb_dev->dev->devnum;
        }

        if (((mitm->request.bRequestType & USB_DIR_MASK) == USB_DIR_OUT) && (mitm->request.wLength > 0)) {
            mitm->usb_dev.ctrl_handler.trans.buffer = vsf_usbh_malloc(mitm->request.wLength);
            mitm->usb_dev.ctrl_handler.trans.size = mitm->request.wLength;
            mitm->usbd.control_trans.mem_save = mitm->usb_dev.ctrl_handler.trans.use_as__vsf_mem_t;
            if (NULL == mitm->usb_dev.ctrl_handler.trans.buffer) {
                vsf_trace_error("usbd: fail to allocate request buffer\n");
                break;
            }
            vk_usbd_stdreq_data_stage(&mitm->usb_dev);
        } else if (mitm->usbh.is_resetting || mitm->usbh.is_control_requesting) {
            mitm->is_setup_pending = true;
        } else if (VSF_ERR_NONE != __vsf_usb_mitm_usbh_control_msg(mitm)) {
            vk_usbd_ep_stall(&mitm->usb_dev, 0);
        }
        break;
    case VSF_EVT_USBD_ON_STATUS_QUERY:
        if (((mitm->request.bRequestType & USB_DIR_MASK) == USB_DIR_OUT) && (mitm->request.wLength > 0)) {
            __vsf_usb_mitm_notify_user(mitm, USB_ON_PREPARE_DATA, &mitm->usb_dev.ctrl_handler.trans);
            vk_usbh_urb_set_buffer(&mitm->usbh.libusb_dev->dev->ep0.urb,
                mitm->usb_dev.ctrl_handler.trans.buffer, mitm->request.wLength);
            mitm->usb_dev.ctrl_handler.trans.buffer = NULL;

            if (mitm->usbh.is_resetting || mitm->usbh.is_control_requesting) {
                mitm->is_setup_pending = true;
            } else if (VSF_ERR_NONE != __vsf_usb_mitm_usbh_control_msg(mitm)) {
                vk_usbd_ep_stall(&mitm->usb_dev, 0);
            }
        } else {
            vk_usbd_stdreq_status_stage(&mitm->usb_dev);
        }
        break;
    case VSF_EVT_USBD_ON_STATUS:
        // set address for SET_ADDRESS request
        if (    ((mitm->request.bRequestType & USB_TYPE_MASK) == USB_TYPE_STANDARD)
            &&  ((mitm->request.bRequestType & USB_RECIP_MASK) == USB_RECIP_DEVICE)
            &&  (mitm->request.bRequest == USB_REQ_SET_ADDRESS)) {
            VSF_USBD_DRV_PREPARE(&mitm->usb_dev);
            vk_usbd_drv_set_address(mitm->usbd.address);
        }
        if (mitm->usb_dev.ctrl_handler.trans.buffer != NULL) {
            vsf_usbh_free(mitm->usb_dev.ctrl_handler.trans.buffer);
            mitm->usb_dev.ctrl_handler.trans.buffer = NULL;
        }
        break;
    default: {
            uint8_t ep = evt - VSF_EVT_USBD_ON_EP;
            VSF_ASSERT((ep & 0x0F) != 0);
            uint8_t idx = ((ep & 0x0F) - 1) + ((ep & USB_DIR_MASK) == USB_DIR_IN ? 15 : 0);
            VSF_ASSERT(mitm->usbd.trans_busy & (1 << idx));
            mitm->usbd.trans_busy &= ~(1 << idx);
            __vsf_usb_mitm_update_ep(mitm, idx);
        }
        break;
    }
}

void vsf_usb_mitm_start(vsf_usb_mitm_t *mitm)
{
    mitm->teda.fn.evthandler = __vsf_usb_mitm_evthadler;
#   if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    mitm->teda.on_terminate = NULL;
#   endif
    vsf_eda_init((vsf_eda_t *)&mitm->teda, vsf_prio_0);
}
