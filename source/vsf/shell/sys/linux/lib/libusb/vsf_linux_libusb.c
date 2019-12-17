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

// TODO: implement backend for libusb(use LGPL)

#include "../../vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED && VSF_USE_LINUX_LIBUSB == ENABLED

#define VSF_USBH_IMPLEMENT
#define VSF_LINUX_INHERIT
#include "../../vsf_linux.h"

//#include <stdlib.h>
#include <libusb.h>

#include <poll.h>
#include <pthread.h>

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE != ENABLED
#   error VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE must be enabled
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct vsf_linux_libusb_cb_t {
    vsf_dlist_node_t cbnode;

    libusb_hotplug_callback_handle handle;
    libusb_context *ctx;
    libusb_hotplug_event events;
    libusb_hotplug_flag flags;
    libusb_hotplug_callback_fn cb_fn;

    int vendor_id;
    int product_id;
    int dev_class;

    void *user_data;
};
typedef struct vsf_linux_libusb_cb_t vsf_linux_libusb_cb_t;

struct vsf_linux_libusb_transfer_t {
    vk_usbh_urb_t urb;
    vsf_dlist_node_t transnode;
    vsf_eda_t eda;
    // transfer MUST be the last for variable number of iso_packet_desc
    struct libusb_transfer transfer;
};
typedef struct vsf_linux_libusb_transfer_t vsf_linux_libusb_transfer_t;

struct vsf_linux_libusb_dev_t {
    vsf_dlist_node_t devnode;
    vk_usbh_libusb_dev_t *libusb_dev;
    vk_usbh_urb_t urb;
    union {
        struct {
            vk_usbh_pipe_t pipe_in[16];
            vk_usbh_pipe_t pipe_out[16];
        };
        vk_usbh_pipe_t pipe[32];
    };
};
typedef struct vsf_linux_libusb_dev_t vsf_linux_libusb_dev_t;

struct vsf_linux_libusb_t {
    libusb_hotplug_callback_handle cbhandle;

    int devnum;
    vsf_dlist_t devlist;
    vsf_dlist_t cblist;
    vsf_dlist_t translist;

    vsf_linux_thread_t *thread;
    vk_usbh_libusb_dev_t *cur_dev;
    int fd;
    struct libusb_pollfd pollfd[1];

    vsf_linux_libusb_dev_t *curdev;
};
typedef struct vsf_linux_libusb_t vsf_linux_libusb_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static vsf_linux_libusb_t __vsf_libusb = { 0 };

/*============================ PROTOTYPES ====================================*/

SECTION(".text.vsf.kernel.eda")
extern vsf_err_t __vsf_eda_fini(vsf_eda_t *pthis);

extern int raw_desc_to_config(struct libusb_context *ctx,
    unsigned char *buf, int size, int host_endian,
    struct libusb_config_descriptor **config);

/*============================ IMPLEMENTATION ================================*/

static vsf_linux_libusb_dev_t * __vsf_linux_libusb_get_dev(vk_usbh_libusb_dev_t *dev)
{
    vsf_linux_libusb_dev_t *ldev = NULL;
    vsf_protect_t orig = vsf_protect_sched();
        __vsf_dlist_foreach_unsafe(vsf_linux_libusb_dev_t, devnode, &__vsf_libusb.devlist) {
            if (_->libusb_dev == dev) {
                ldev = _;
                break;
            }
        }
    vsf_unprotect_sched(orig);
    return ldev;
}

static void __vsf_linux_libusb_on_event(void *param, vk_usbh_libusb_dev_t *dev, vk_usbh_libusb_evt_t evt)
{
    if (VSF_USBH_LIBUSB_EVT_ON_ARRIVED == evt) {
        vk_usbh_libusb_open(dev);
    }
    vsf_eda_post_evt_msg(&__vsf_libusb.thread->use_as__vsf_eda_t, VSF_EVT_USER + evt, dev);
}

static void __vsf_linux_libusb_process_cb(vsf_linux_libusb_dev_t *ldev, vk_usbh_libusb_evt_t evt)
{
    __vsf_dlist_foreach_unsafe(vsf_linux_libusb_cb_t, cbnode, &__vsf_libusb.cblist) {
        if (    ((_->vendor_id == LIBUSB_HOTPLUG_MATCH_ANY) || (_->vendor_id == ldev->libusb_dev->vid))
            &&  ((_->product_id == LIBUSB_HOTPLUG_MATCH_ANY) || (_->product_id == ldev->libusb_dev->pid))
            &&  ((_->dev_class == LIBUSB_HOTPLUG_MATCH_ANY) || (_->dev_class == ldev->libusb_dev->class))
            &&  (_->cb_fn != NULL)) {

            switch (evt) {
            case VSF_USBH_LIBUSB_EVT_ON_ARRIVED:
                if (_->events & LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED) {
                    _->cb_fn(_->ctx, (libusb_device *)ldev, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, _->user_data);
                }
                break;
            case VSF_USBH_LIBUSB_EVT_ON_LEFT:
                if (_->events & LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT) {
                    _->cb_fn(_->ctx, (libusb_device *)ldev, LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT, _->user_data);
                }
                break;
            }
        }
    }
}

static void * __vsf_libusb_libusb_thread(void *param)
{
    vsf_linux_libusb_dev_t *ldev;
    vk_usbh_libusb_set_evthandler(NULL, __vsf_linux_libusb_on_event);
    vsf_evt_t evt;
    while (1) {
        evt = vsf_thread_wait();
        switch (evt) {
        case VSF_EVT_USER + VSF_USBH_LIBUSB_EVT_ON_ARRIVED:
            ldev = malloc(sizeof(vsf_linux_libusb_dev_t));
             if (ldev != NULL) {
                ldev->libusb_dev = (vk_usbh_libusb_dev_t *)vsf_eda_get_cur_msg();
                ldev->pipe_in[0] = vk_usbh_get_pipe(ldev->libusb_dev->dev, 0x80, USB_ENDPOINT_XFER_CONTROL, ldev->libusb_dev->ep0size);
                ldev->pipe_out[0] = vk_usbh_get_pipe(ldev->libusb_dev->dev, 0x00, USB_ENDPOINT_XFER_CONTROL, ldev->libusb_dev->ep0size);
                vsf_dlist_add_to_head(vsf_linux_libusb_dev_t, devnode, &__vsf_libusb.devlist, ldev);
                __vsf_libusb.devnum++;
                __vsf_linux_libusb_process_cb(ldev, VSF_USBH_LIBUSB_EVT_ON_ARRIVED);
            }
            break;
        case VSF_EVT_USER + VSF_USBH_LIBUSB_EVT_ON_LEFT:
            ldev = __vsf_linux_libusb_get_dev((vk_usbh_libusb_dev_t *)vsf_eda_get_cur_msg());
            VSF_LINUX_ASSERT(ldev != NULL);
            vsf_dlist_remove(vsf_linux_libusb_dev_t, devnode, &__vsf_libusb.devlist, ldev);
            __vsf_libusb.devnum--;
            __vsf_linux_libusb_process_cb(ldev, VSF_USBH_LIBUSB_EVT_ON_LEFT);
            vk_usbh_libusb_close(ldev->libusb_dev);
            free(ldev);
            break;
        case VSF_EVT_MESSAGE: {
                vsf_linux_libusb_transfer_t *ltransfer;
                vsf_protect_t orig = vsf_protect_sched();
                    vsf_dlist_remove_head(vsf_linux_libusb_transfer_t, transnode, &__vsf_libusb.translist, ltransfer);
                vsf_unprotect_sched(orig);

                if (ltransfer->transfer.callback != NULL) {
                    ltransfer->transfer.callback(&ltransfer->transfer);
                }

                for (int i = 0; i < dimof(__vsf_libusb.pollfd); i++) {
                    if (__vsf_libusb.pollfd[i].fd >= 0) {
                        vsf_linux_fd_rx_trigger(__vsf_libusb.pollfd[i].fd);
                    }
                }
            }
            break;
        }
    }
}

int libusb_init(libusb_context **context)
{
    if (!__vsf_libusb.cbhandle) {
        __vsf_libusb.cbhandle = 1;

        pthread_t pthread;
        pthread_create(&pthread, NULL, __vsf_libusb_libusb_thread, NULL);
        __vsf_libusb.thread = vsf_linux_get_thread(pthread);

        __vsf_libusb.fd = vsf_linux_create_fd(NULL, NULL);
        __vsf_libusb.pollfd[0].fd = __vsf_libusb.fd;
        __vsf_libusb.pollfd[0].events = POLLIN;
    }
    return LIBUSB_SUCCESS;
}

void libusb_exit(struct libusb_context *ctx)
{
    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    if ((__vsf_libusb.thread == thread) && (__vsf_libusb.fd >= 0)) {
        vsf_linux_delete_fd(__vsf_libusb.fd);
    }
}

int libusb_get_next_timeout(libusb_context *ctx, struct timeval *tv)
{
    VSF_LINUX_ASSERT(false);
    return 0;
}

int libusb_has_capability(uint32_t capability)
{
    switch (capability) {
    case LIBUSB_CAP_HAS_CAPABILITY:
    case LIBUSB_CAP_HAS_HOTPLUG:
        return 1;
    default:
        return 0;
    }
}

int libusb_hotplug_register_callback(libusb_context *ctx,
        libusb_hotplug_event events,
        libusb_hotplug_flag flags,
        int vendor_id,
        int product_id,
        int dev_class,
        libusb_hotplug_callback_fn cb_fn,
        void *user_data,
        libusb_hotplug_callback_handle *callback_handle)
{
    vsf_linux_libusb_cb_t *cb = malloc(sizeof(vsf_linux_libusb_cb_t));
    if (NULL == cb) { return LIBUSB_ERROR_NO_MEM; }

    cb->handle = __vsf_libusb.cbhandle++;
    if (callback_handle != NULL) {
        *callback_handle = cb->handle;
    }
    cb->ctx = ctx;
    cb->events = events;
    cb->flags = flags;
    cb->vendor_id = vendor_id;
    cb->product_id = product_id;
    cb->dev_class = dev_class;
    cb->cb_fn = cb_fn;
    cb->user_data = user_data;

    vsf_protect_t orig = vsf_protect_sched();
        vsf_dlist_add_to_head(vsf_linux_libusb_cb_t, cbnode, &__vsf_libusb.cblist, cb);
    vsf_unprotect_sched(orig);
    return LIBUSB_SUCCESS;
}

void libusb_hotplug_deregister_callback(libusb_context *ctx,
        libusb_hotplug_callback_handle callback_handle)
{
    vsf_linux_libusb_cb_t *cb = NULL;
    vsf_protect_t orig = vsf_protect_sched();
        __vsf_dlist_foreach_unsafe(vsf_linux_libusb_cb_t, cbnode, &__vsf_libusb.cblist) {
            if (_->handle == callback_handle) {
                vsf_dlist_remove(vsf_linux_libusb_cb_t, cbnode, &__vsf_libusb.cblist, _);
                cb = _;
                break;
            }
        }
    vsf_unprotect_sched(orig);

    if (cb != NULL) {
        free(cb);
    }
}

ssize_t libusb_get_device_list(libusb_context *ctx, libusb_device *** list)
{
    VSF_LINUX_ASSERT(list != NULL);
    ssize_t devnum = __vsf_libusb.devnum;

    *list = NULL;
    if (devnum > 0) {
        ssize_t i;
        libusb_device **devlist = (libusb_device **)malloc((devnum + 1) * sizeof(libusb_device *));
        if (!devlist) { return 0; }

        vsf_linux_libusb_dev_t *ldev;
        vsf_dlist_peek_head(vsf_linux_libusb_dev_t, devnode, &__vsf_libusb.devlist, ldev);
        for (i = 0; i < devnum; i++) {
            devlist[i] = (libusb_device *)ldev;
            vsf_dlist_peek_next(vsf_linux_libusb_dev_t, devnode, ldev, ldev);
        }
        devlist[i] = NULL;
        *list = devlist;
    }
    return devnum;
}

void libusb_free_device_list(libusb_device **list, int unref_devices)
{
    if (list != NULL) {
        free(list);
    }
}

int libusb_open(libusb_device *dev, libusb_device_handle **dev_handle)
{
    if (dev_handle != NULL) {
        *dev_handle = (libusb_device_handle *)dev;
    }
    return LIBUSB_SUCCESS;
}

void libusb_close(libusb_device_handle *dev_handle)
{
    
}

int libusb_attach_kernel_driver(libusb_device_handle *dev_handle, int interface_number)
{
    return LIBUSB_ERROR_NOT_SUPPORTED;
}

int libusb_detach_kernel_driver(libusb_device_handle *dev_handle, int interface_number)
{
    return LIBUSB_ERROR_NOT_SUPPORTED;
}

int libusb_kernel_driver_active(libusb_device_handle *dev_handle, int interface_number)
{
    return LIBUSB_ERROR_NOT_SUPPORTED;
}

uint8_t libusb_get_bus_number(libusb_device *dev)
{
    return 0;
}

libusb_device *libusb_ref_device(libusb_device *dev)
{
    return dev;
}

void libusb_unref_device(libusb_device *dev)
{
}

uint8_t libusb_get_device_address(libusb_device *dev)
{
    vsf_linux_libusb_dev_t *ldev = (vsf_linux_libusb_dev_t *)dev;
    return ldev->libusb_dev->address;
}

static vk_usbh_pipe_t __vsf_linux_libusb_get_pipe(vsf_linux_libusb_dev_t *ldev, unsigned char endpoint)
{
    unsigned char epaddr = endpoint & 0x7F;
    return ((endpoint & USB_DIR_MASK) == USB_DIR_IN) ? ldev->pipe_in[epaddr] : ldev->pipe_out[epaddr];
}

int libusb_get_max_packet_size(libusb_device *dev, unsigned char endpoint)
{
    vsf_linux_libusb_dev_t *ldev = (vsf_linux_libusb_dev_t *)dev;
    unsigned char epaddr = endpoint & 0x7F;
    vk_usbh_pipe_t pipe = __vsf_linux_libusb_get_pipe(ldev, endpoint);
    return pipe.size;
}

int libusb_get_device_speed(libusb_device *dev)
{
    vsf_linux_libusb_dev_t *ldev = (vsf_linux_libusb_dev_t *)dev;
    switch (ldev->libusb_dev->dev->speed) {
    case USB_SPEED_LOW:     return LIBUSB_SPEED_LOW;
    case USB_SPEED_FULL:    return LIBUSB_SPEED_FULL;
    case USB_SPEED_HIGH:    return LIBUSB_SPEED_HIGH;
    case USB_SPEED_SUPER:   return LIBUSB_SPEED_SUPER;
    default:                return LIBUSB_SPEED_UNKNOWN;
    }
}

static int __vsf_linux_libusb_submit_urb(vsf_linux_libusb_dev_t *ldev)
{
    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    vk_usbh_urb_t *urb = &ldev->urb;

    if (VSF_ERR_NONE != vk_usbh_submit_urb(ldev->libusb_dev->usbh, urb)) {
        return LIBUSB_ERROR_IO;
    }
    vsf_thread_wfe(VSF_EVT_MESSAGE);
    return URB_OK != vk_usbh_urb_get_status(urb) ? LIBUSB_ERROR_IO : LIBUSB_SUCCESS;
}

static vk_usbh_urb_t * __vsf_linux_libusb_get_urb(vsf_linux_libusb_dev_t *ldev)
{
    if (!vk_usbh_urb_is_alloced(&ldev->urb)) {
        if (VSF_ERR_NONE != vk_usbh_alloc_urb(ldev->libusb_dev->usbh, ldev->libusb_dev->dev, &ldev->urb)) {
            VSF_LINUX_ASSERT(false);
            return NULL;
        }
    }
    return &ldev->urb;
}

int libusb_control_transfer(libusb_device_handle *dev_handle,
    uint8_t bRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
    unsigned char *data, uint16_t wLength, unsigned int timeout)
{
    vsf_linux_libusb_dev_t *ldev = (vsf_linux_libusb_dev_t *)dev_handle;
    vk_usbh_urb_t *urb = __vsf_linux_libusb_get_urb(ldev);
    int err;

    if ((bRequestType & USB_DIR_MASK) == USB_DIR_IN) {
        vk_usbh_urb_set_pipe(urb, ldev->pipe_in[0]);
    } else {
        vk_usbh_urb_set_pipe(urb, ldev->pipe_out[0]);
    }
    urb->urb_hcd->setup_packet.bRequestType = bRequestType;
    urb->urb_hcd->setup_packet.bRequest = bRequest;
    urb->urb_hcd->setup_packet.wValue = wValue;
    urb->urb_hcd->setup_packet.wIndex = wIndex;
    urb->urb_hcd->setup_packet.wLength = wLength;
    urb->urb_hcd->buffer = data;
    urb->urb_hcd->transfer_length = wLength;
    urb->urb_hcd->timeout = timeout;
    err = __vsf_linux_libusb_submit_urb(ldev);
    if (!err) { err = vk_usbh_urb_get_actual_length(urb); }
    return err;
}

int libusb_bulk_transfer(libusb_device_handle *dev_handle,
    unsigned char endpoint, unsigned char *data, int length,
    int *actual_length, unsigned int timeout)
{
    vsf_linux_libusb_dev_t *ldev = (vsf_linux_libusb_dev_t *)dev_handle;
    vk_usbh_urb_t *urb = &ldev->urb;
    int ret;

    vk_usbh_urb_set_pipe(urb, __vsf_linux_libusb_get_pipe(ldev, endpoint));
    urb->urb_hcd->buffer = data;
    urb->urb_hcd->transfer_length = length;
    urb->urb_hcd->timeout = timeout;

    ret = __vsf_linux_libusb_submit_urb(ldev);
    if (!ret && (actual_length != NULL)) {
        *actual_length = vk_usbh_urb_get_actual_length(urb);
    }
    return ret;
}

int libusb_interrupt_transfer(libusb_device_handle *dev_handle,
    unsigned char endpoint, unsigned char *data, int length,
    int *actual_length, unsigned int timeout)
{
    vsf_linux_libusb_dev_t *ldev = (vsf_linux_libusb_dev_t *)dev_handle;
    vk_usbh_urb_t *urb = &ldev->urb;
    int ret;

    vk_usbh_urb_set_pipe(urb, __vsf_linux_libusb_get_pipe(ldev, endpoint));
    urb->urb_hcd->buffer = data;
    urb->urb_hcd->transfer_length = length;
    urb->urb_hcd->timeout = timeout;

    ret = __vsf_linux_libusb_submit_urb(ldev);
    if (!ret && (actual_length != NULL))
        *actual_length = vk_usbh_urb_get_actual_length(urb);
    return ret;
}

int libusb_get_device_descriptor(libusb_device *dev, struct libusb_device_descriptor *desc)
{
    int err = libusb_get_descriptor((libusb_device_handle *)dev, USB_DT_DEVICE, 0,
            (unsigned char *)desc, sizeof(struct libusb_device_descriptor));
    return err < 0 ? err : LIBUSB_SUCCESS;
}

int libusb_set_configuration(libusb_device_handle *dev_handle, int configuration)
{
    struct libusb_config_descriptor *config;
    int err = libusb_control_transfer(dev_handle, USB_DIR_OUT,
            USB_REQ_SET_CONFIGURATION, configuration, 0, NULL, 0, 1000);
    if (err < 0) { return err; }

    err = libusb_get_active_config_descriptor((libusb_device *)dev_handle, &config);
    if (err < 0) { return err; }

    libusb_free_config_descriptor(config);
    return err;
}

int libusb_get_configuration(libusb_device_handle *dev_handle, int *config)
{
    *config = 0;
    int err = libusb_control_transfer(dev_handle, USB_DIR_IN,
            USB_REQ_GET_CONFIGURATION, 0, 0, (unsigned char *)config, 1, 1000);
    return err < 0 ? err : LIBUSB_SUCCESS;
}

int libusb_set_interface_alt_setting(libusb_device_handle *dev_handle,
        int interface_number, int alternate_setting)
{
    return libusb_control_transfer(dev_handle,
            LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_STANDARD | LIBUSB_RECIPIENT_INTERFACE,
            LIBUSB_REQUEST_SET_INTERFACE, alternate_setting, interface_number, NULL, 0, 0);
}

int libusb_release_interface(libusb_device_handle *dev_handle, int interface_number)
{
    return LIBUSB_SUCCESS;
}

int libusb_claim_interface(libusb_device_handle *dev_handle, int interface_number)
{
    return LIBUSB_SUCCESS;
}

struct libusb_transfer *libusb_alloc_transfer(int iso_packets)
{
    uint32_t size = sizeof(vsf_linux_libusb_transfer_t) +
            iso_packets * sizeof(struct libusb_iso_packet_descriptor);
    vsf_linux_libusb_transfer_t *ltransfer = calloc(1, size);
    if (ltransfer != NULL) {
        ltransfer->transfer.num_iso_packets = iso_packets;
        return &ltransfer->transfer;
    }
    return NULL;
}

static void __vsf_linux_libusb_transfer_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vsf_linux_libusb_transfer_t *ltransfer = container_of(eda, vsf_linux_libusb_transfer_t, eda);
    vsf_linux_libusb_dev_t *ldev = (vsf_linux_libusb_dev_t *)ltransfer->transfer.dev_handle;
    vk_usbh_urb_t *urb = &ltransfer->urb;
    vsf_protect_t orig;

    switch (evt) {
    case VSF_EVT_MESSAGE:
        ltransfer->transfer.actual_length = vk_usbh_urb_get_actual_length(urb);
        ltransfer->transfer.status = vk_usbh_urb_get_status(urb) == URB_OK ?
                LIBUSB_TRANSFER_COMPLETED : LIBUSB_TRANSFER_ERROR;
        vk_usbh_free_urb(ldev->libusb_dev->usbh, urb);
        
        orig = vsf_protect_sched();
            vsf_dlist_add_to_tail(vsf_linux_libusb_transfer_t, transnode, &__vsf_libusb.translist, ltransfer);
        vsf_unprotect_sched(orig);
        vsf_eda_post_msg(&__vsf_libusb.thread->use_as__vsf_eda_t, urb);
        break;
    }
}

int libusb_submit_transfer(struct libusb_transfer *transfer)
{
    vsf_linux_libusb_transfer_t *ltransfer = container_of(transfer, vsf_linux_libusb_transfer_t, transfer);
    vsf_linux_libusb_dev_t *ldev = (vsf_linux_libusb_dev_t *)transfer->dev_handle;
    int err = LIBUSB_SUCCESS;
    vk_usbh_urb_t *urb = &ltransfer->urb;

    if (!vk_usbh_urb_is_alloced(urb)) {
        if (VSF_ERR_NONE != vk_usbh_alloc_urb(ldev->libusb_dev->usbh, ldev->libusb_dev->dev, urb)) {
            return -1;
        }
    }
    urb->urb_hcd->buffer = transfer->buffer;
    urb->urb_hcd->transfer_length = transfer->length;
    urb->urb_hcd->timeout = transfer->timeout;
    vk_usbh_urb_set_pipe(urb, __vsf_linux_libusb_get_pipe(ldev, ltransfer->transfer.endpoint));

    switch (transfer->type) {
    case LIBUSB_TRANSFER_TYPE_CONTROL: {
            struct usb_ctrlrequest_t *request = (struct usb_ctrlrequest_t *)transfer->buffer;
            urb->urb_hcd->setup_packet = *request;
            urb->urb_hcd->buffer = (uint8_t *)urb->urb_hcd->buffer + sizeof(struct usb_ctrlrequest_t);
            urb->urb_hcd->transfer_length -= sizeof(struct usb_ctrlrequest_t);
        }
        break;
    case LIBUSB_TRANSFER_TYPE_ISOCHRONOUS:
#ifndef VSFHAL_HCD_ISO_EN
        return -1;
#else
        urb->urb_hcd->iso_packet.number_of_packets = transfer->num_iso_packets;
        break;
#endif
    case LIBUSB_TRANSFER_TYPE_BULK:
    case LIBUSB_TRANSFER_TYPE_INTERRUPT:
        break;
    }

    vsf_eda_set_evthandler(&ltransfer->eda, __vsf_linux_libusb_transfer_evthandler);
    vsf_eda_init(&ltransfer->eda, vsf_prio_inherit, false);

    if (VSF_ERR_NONE != vk_usbh_submit_urb_ex(ldev->libusb_dev->usbh, urb, 0, &ltransfer->eda)) {
        err = LIBUSB_ERROR_IO;
        __vsf_eda_fini(&ltransfer->eda);
    }
    return err;
}

int libusb_cancel_transfer(struct libusb_transfer *transfer)
{
    vsf_linux_libusb_transfer_t *ltransfer = container_of(transfer, vsf_linux_libusb_transfer_t, transfer);
    vsf_linux_libusb_dev_t *ldev = (vsf_linux_libusb_dev_t *)transfer->dev_handle;

    if (vk_usbh_urb_is_alloced(&ltransfer->urb)) {
        vk_usbh_free_urb(ldev->libusb_dev->usbh, &ltransfer->urb);
    }
    return LIBUSB_SUCCESS;
}

void libusb_free_transfer(struct libusb_transfer *transfer)
{
    if (transfer != NULL) {
        free(transfer);
    }
}

int libusb_handle_events_timeout_completed(libusb_context *ctx,
    struct timeval *tv, int *completed)
{
    vsf_linux_fd_t *sfd;
    for (int i = 0; i < dimof(__vsf_libusb.pollfd); i++) {
        sfd = vsf_linux_get_fd(__vsf_libusb.pollfd[i].fd);
        sfd->rxevt = false;
        sfd->txpend = sfd->rxpend = -1;
    }
    return LIBUSB_SUCCESS;
}

int libusb_handle_events_completed(libusb_context *ctx, int *completed)
{
    struct timeval tv;
    tv.tv_sec = 60;
    tv.tv_usec = 0;
    return libusb_handle_events_timeout_completed(ctx, &tv, completed);
}

int libusb_handle_events_timeout(libusb_context *ctx, struct timeval *tv)
{
    return libusb_handle_events_timeout_completed(ctx, tv, NULL);
}

int libusb_handle_events(libusb_context *ctx)
{
    struct timeval tv;
    tv.tv_sec = 60;
    tv.tv_usec = 0;
    return libusb_handle_events_timeout_completed(ctx, &tv, NULL);
}

const struct libusb_pollfd** libusb_get_pollfds(libusb_context *ctx)
{
    struct libusb_pollfd **pollfd = (struct libusb_pollfd **)
        calloc(dimof(__vsf_libusb.pollfd) + 1, sizeof(struct libusb_pollfd *));

    if (pollfd != NULL) {
        int i;
        for (i = 0; i < dimof(__vsf_libusb.pollfd); i++) {
            pollfd[i] = &__vsf_libusb.pollfd[i];
        }
        pollfd[i] = NULL;
    }
    return (const struct libusb_pollfd **)pollfd;
}

void libusb_free_pollfds(const struct libusb_pollfd **pollfds)
{
    if (pollfds != NULL) {
        free(pollfds);
    }
}

#endif      // VSF_USE_LINUX
