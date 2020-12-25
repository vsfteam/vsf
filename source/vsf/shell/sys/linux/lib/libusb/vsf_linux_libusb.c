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

#include "../../vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_LIBUSB == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_LINUX_CLASS_INHERIT__

// libusb is another upper layer for usb host, so it need to access some private members
// for vk_usbh_get_pipe and urb private members
#define __VSF_USBH_CLASS_IMPLEMENT

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../include/unistd.h"
#   include "../../include/libusb.h"

#   include "../../include/poll.h"
#   include "../../include/pthread.h"
#   include "../../include/sys/time.h"
#else
#   include <unistd.h>
#   include <libusb.h>

#   include <poll.h>
#   include <pthread.h>
#   include <sys/time.h>
#endif

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_STDLIB == ENABLED
#   include "../../include/simple_libc/stdlib.h"
#else
#   include <stdlib.h>
#endif

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_STRING == ENABLED
#   include "../../include/simple_libc/string.h"
#else
#   include <string.h>
#endif

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE != ENABLED
#   error VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE must be enabled
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_linux_libusb_cb_t {
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
} vsf_linux_libusb_cb_t;

typedef struct vsf_linux_libusb_transfer_t {
    vk_usbh_urb_t urb;
    vsf_dlist_node_t transnode;
    vsf_eda_t eda;
    // transfer MUST be the last for variable number of iso_packet_desc
    struct libusb_transfer transfer;
} vsf_linux_libusb_transfer_t;

typedef struct vsf_linux_libusb_dev_t {
    vsf_dlist_node_t devnode;
    vk_usbh_libusb_dev_t *libusb_dev;
    bool is_in_newlist;
    union {
        struct {
            vk_usbh_pipe_t pipe_in[16];
            vk_usbh_pipe_t pipe_out[16];
        };
        vk_usbh_pipe_t pipe[32];
    };
} vsf_linux_libusb_dev_t;

typedef struct vsf_linux_libusb_t {
    libusb_hotplug_callback_handle cbhandle;
    vsf_dlist_t cblist;

    int devnum;
    vsf_trig_t dev_trig;
    vsf_dlist_t devlist;
    vsf_dlist_t devlist_new;
    vsf_dlist_t devlist_del;
    vsf_linux_thread_t *core_thread;

    bool is_to_exit;
    vsf_trig_t trans_trig;
    vsf_dlist_t translist_done;
    vsf_linux_thread_t *user_thread;

    int fd;
    struct libusb_pollfd pollfd[1];
} vsf_linux_libusb_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static vsf_linux_libusb_t __vsf_libusb = { 0 };

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __vsf_linux_libusb_on_event(void *param, vk_usbh_libusb_dev_t *dev, vk_usbh_libusb_evt_t evt)
{
    vsf_linux_libusb_dev_t *ldev;
    vsf_protect_t orig;

    if (VSF_USBH_LIBUSB_EVT_ON_ARRIVED == evt) {
        vk_usbh_libusb_open(dev);
        ldev = malloc(sizeof(vsf_linux_libusb_dev_t));
        if (ldev != NULL) {
            ldev->libusb_dev = dev;
            ldev->is_in_newlist = true;
            ldev->pipe_in[0] = vk_usbh_get_pipe(dev->dev, 0x80, USB_ENDPOINT_XFER_CONTROL, dev->ep0size);
            ldev->pipe_out[0] = vk_usbh_get_pipe(dev->dev, 0x00, USB_ENDPOINT_XFER_CONTROL, dev->ep0size);
            dev->user_data = ldev;
            orig = vsf_protect_sched();
                vsf_dlist_add_to_tail(vsf_linux_libusb_dev_t, devnode, &__vsf_libusb.devlist_new, ldev);
            vsf_unprotect_sched(orig);
        }
    } else if (VSF_USBH_LIBUSB_EVT_ON_LEFT) {
        ldev = (vsf_linux_libusb_dev_t *)dev->user_data;
        orig = vsf_protect_sched();
        if (ldev->is_in_newlist) {
            vsf_dlist_remove(vsf_linux_libusb_dev_t, devnode, &__vsf_libusb.devlist_new, ldev);
            vsf_unprotect_sched(orig);
            return;
        } else {
            vsf_dlist_remove(vsf_linux_libusb_dev_t, devnode, &__vsf_libusb.devlist, ldev);
            vsf_dlist_add_to_tail(vsf_linux_libusb_dev_t, devnode, &__vsf_libusb.devlist_del, ldev);
            vsf_unprotect_sched(orig);
        }
    } else {
        return;
    }
    vsf_eda_trig_set(&__vsf_libusb.dev_trig);
}

static void __vsf_linux_libusb_process_cb(vsf_linux_libusb_dev_t *ldev, vk_usbh_libusb_evt_t evt)
{
    __vsf_dlist_foreach_unsafe(vsf_linux_libusb_cb_t, cbnode, &__vsf_libusb.cblist) {
        if (    ((_->vendor_id == LIBUSB_HOTPLUG_MATCH_ANY) || (_->vendor_id == ldev->libusb_dev->vid))
            &&  ((_->product_id == LIBUSB_HOTPLUG_MATCH_ANY) || (_->product_id == ldev->libusb_dev->pid))
            &&  ((_->dev_class == LIBUSB_HOTPLUG_MATCH_ANY) || (_->dev_class == ldev->libusb_dev->c))
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

static void * __vsf_libusb_libusb_core_thread(void *param)
{
    vsf_linux_libusb_dev_t *ldev;
    vsf_protect_t orig;
    vk_usbh_libusb_set_evthandler(NULL, __vsf_linux_libusb_on_event);
    while (1) {
        vsf_thread_trig_pend(&__vsf_libusb.dev_trig, -1);

        while (!vsf_dlist_is_empty(&__vsf_libusb.devlist_del)) {
            orig = vsf_protect_sched();
                vsf_dlist_remove_head(vsf_linux_libusb_dev_t, devnode, &__vsf_libusb.devlist_del, ldev);
            vsf_unprotect_sched(orig);

            __vsf_libusb.devnum--;
            __vsf_linux_libusb_process_cb(ldev, VSF_USBH_LIBUSB_EVT_ON_LEFT);
            vk_usbh_libusb_close(ldev->libusb_dev);
            free(ldev);
        }
        while (!vsf_dlist_is_empty(&__vsf_libusb.devlist_new)) {
            orig = vsf_protect_sched();
                vsf_dlist_remove_head(vsf_linux_libusb_dev_t, devnode, &__vsf_libusb.devlist_new, ldev);
                vsf_dlist_add_to_tail(vsf_linux_libusb_dev_t, devnode, &__vsf_libusb.devlist, ldev);
                ldev->is_in_newlist = false;
            vsf_unprotect_sched(orig);

            __vsf_libusb.devnum++;
            __vsf_linux_libusb_process_cb(ldev, VSF_USBH_LIBUSB_EVT_ON_ARRIVED);
        }
    }
    return NULL;
}

static void * __vsf_libusb_libusb_user_thread(void *param)
{
    vsf_linux_libusb_transfer_t *ltransfer;
//    vsf_protect_t orig;

    while (1) {
        vsf_thread_trig_pend(&__vsf_libusb.trans_trig, -1);

        while (!vsf_dlist_is_empty(&__vsf_libusb.translist_done)) {
            // all thread running under same priority as processor priority
            //  so no need to protect
//            orig = vsf_protect_sched();
                vsf_dlist_remove_head(vsf_linux_libusb_transfer_t, transnode, &__vsf_libusb.translist_done, ltransfer);
//            vsf_unprotect_sched(orig);

            if (ltransfer->transfer.callback != NULL) {
                ltransfer->transfer.callback(&ltransfer->transfer);
            }

            for (int i = 0; i < dimof(__vsf_libusb.pollfd); i++) {
                if (__vsf_libusb.pollfd[i].fd >= 0) {
                    vsf_linux_fd_rx_trigger(__vsf_libusb.pollfd[i].fd);
                }
            }
        }

        if (__vsf_libusb.is_to_exit) {
            pthread_exit(0);
        }
    }
    return NULL;
}

void vsf_linux_libusb_startup(void)
{
    if (!__vsf_libusb.cbhandle) {
        __vsf_libusb.cbhandle = 1;
        vsf_eda_trig_init(&__vsf_libusb.dev_trig, false, true);

        pthread_t pthread;
        pthread_create(&pthread, NULL, __vsf_libusb_libusb_core_thread, NULL);
        __vsf_libusb.core_thread = vsf_linux_get_thread(pthread);
    }
}

int libusb_init(libusb_context **context)
{
    if (__vsf_libusb.fd != 0) {
        VSF_LINUX_ASSERT(false);
    }

    __vsf_libusb.fd = vsf_linux_create_fd(NULL, NULL);
    __vsf_libusb.pollfd[0].fd = __vsf_libusb.fd;
    __vsf_libusb.pollfd[0].events = POLLIN;

    __vsf_libusb.is_to_exit = false;
    vsf_eda_trig_init(&__vsf_libusb.trans_trig, false, true);
    pthread_t pthread;
    pthread_create(&pthread, NULL, __vsf_libusb_libusb_user_thread, NULL);
    __vsf_libusb.user_thread = vsf_linux_get_thread(pthread);
    return LIBUSB_SUCCESS;
}

void libusb_exit(struct libusb_context *ctx)
{
    if (__vsf_libusb.fd >= 0) {
        __vsf_libusb.is_to_exit = true;
        vsf_eda_trig_set(&__vsf_libusb.trans_trig);
        pthread_join(__vsf_libusb.user_thread->tid, NULL);
        __vsf_libusb.user_thread = NULL;

        int fd = __vsf_libusb.fd;
        __vsf_libusb.fd = 0;
        vsf_linux_delete_fd(fd);
    }
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

int libusb_get_next_timeout(libusb_context *ctx, struct timeval *tv)
{
    VSF_LINUX_ASSERT(false);
    return 0;
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_warning=pe111
#endif

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

libusb_device * libusb_ref_device(libusb_device *dev)
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

static vk_usbh_pipe_t * __vsf_libusb_get_pipe(vsf_linux_libusb_dev_t *ldev, unsigned char endpoint)
{
    unsigned char epaddr = endpoint & 0x7F;
    return ((endpoint & USB_DIR_MASK) == USB_DIR_IN) ? &ldev->pipe_in[epaddr] : &ldev->pipe_out[epaddr];
}

int libusb_get_max_packet_size(libusb_device *dev, unsigned char endpoint)
{
    vsf_linux_libusb_dev_t *ldev = (vsf_linux_libusb_dev_t *)dev;
    vk_usbh_pipe_t *pipe = __vsf_libusb_get_pipe(ldev, endpoint);
    return pipe->size;
}

int libusb_get_device_speed(libusb_device *dev)
{
    vsf_linux_libusb_dev_t *ldev = (vsf_linux_libusb_dev_t *)dev;
    return ldev->libusb_dev->dev->speed;
}

static int __vsf_libusb_submit_urb(vsf_linux_libusb_dev_t *ldev)
{
    vk_usbh_urb_t *urb = &ldev->libusb_dev->urb;

    if (VSF_ERR_NONE != vk_usbh_submit_urb(ldev->libusb_dev->usbh, urb)) {
        return LIBUSB_ERROR_IO;
    }
    vsf_thread_wfe(VSF_EVT_MESSAGE);
    return URB_OK != vk_usbh_urb_get_status(urb) ? LIBUSB_ERROR_IO : LIBUSB_SUCCESS;
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

static vk_usbh_urb_t * __vsf_libusb_get_urb(vsf_linux_libusb_dev_t *ldev)
{
    vk_usbh_urb_t *urb = &ldev->libusb_dev->urb;
    if (!vk_usbh_urb_is_alloced(urb)) {
        if (VSF_ERR_NONE != vk_usbh_alloc_urb(ldev->libusb_dev->usbh, ldev->libusb_dev->dev, urb)) {
            VSF_LINUX_ASSERT(false);
            return NULL;
        }
    }
    return urb;
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_warning=pe111
#endif

int libusb_control_transfer(libusb_device_handle *dev_handle,
    uint8_t bRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
    unsigned char *data, uint16_t wLength, unsigned int timeout)
{
    vsf_linux_libusb_dev_t *ldev = (vsf_linux_libusb_dev_t *)dev_handle;
    vk_usbh_urb_t *urb = __vsf_libusb_get_urb(ldev);
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
    err = __vsf_libusb_submit_urb(ldev);
    if (!err) { err = vk_usbh_urb_get_actual_length(urb); }
    return err;
}

int libusb_bulk_transfer(libusb_device_handle *dev_handle,
    unsigned char endpoint, unsigned char *data, int length,
    int *actual_length, unsigned int timeout)
{
    vsf_linux_libusb_dev_t *ldev = (vsf_linux_libusb_dev_t *)dev_handle;
    vk_usbh_pipe_t *pipe = __vsf_libusb_get_pipe(ldev, endpoint);
    vk_usbh_urb_t *urb = &ldev->libusb_dev->urb;
    int ret;

    vk_usbh_urb_set_pipe(urb, *pipe);
    urb->urb_hcd->buffer = data;
    urb->urb_hcd->transfer_length = length;
    urb->urb_hcd->timeout = timeout;

    ret = __vsf_libusb_submit_urb(ldev);
    *pipe = urb->urb_hcd->pipe;
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
    vk_usbh_pipe_t *pipe = __vsf_libusb_get_pipe(ldev, endpoint);
    vk_usbh_urb_t *urb = &ldev->libusb_dev->urb;
    int ret;

    vk_usbh_urb_set_pipe(urb, *pipe);
    urb->urb_hcd->buffer = data;
    urb->urb_hcd->transfer_length = length;
    urb->urb_hcd->timeout = timeout;

    ret = __vsf_libusb_submit_urb(ldev);
    *pipe = urb->urb_hcd->pipe;
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

int libusb_get_string_descriptor_ascii(libusb_device_handle *dev_handle,
    uint8_t desc_index, unsigned char *data, int length)
{
    unsigned char buf[255];
    uint_fast16_t langid;
    int err;

    err = libusb_get_string_descriptor(dev_handle, 0, 0, buf, sizeof(buf));
    if (err < 0) { return err; }
    else if (err < 4) { return LIBUSB_ERROR_IO; }

    langid = get_unaligned_le16(&buf[2]);
    err = libusb_get_string_descriptor(dev_handle, desc_index, langid, buf, sizeof(buf));
    if (err < 0) { return err; }
    else if ((err != buf[0]) || (buf[1] != LIBUSB_DT_STRING)) { return LIBUSB_ERROR_IO; }

    int pos = 0;
    for (int i = 2; (i < buf[0]) && (pos < length - 1); i += 2) {
        if ((buf[i] & 0x80) || buf[i + 1]) {
            data[pos++] = '?';
        } else {
            data[pos++] = buf[i];
        }
    }
    data[pos] = '\0';
    return pos;
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
    return 0;
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

void libusb_free_config_descriptor(struct libusb_config_descriptor *config)
{
    if (config != NULL) {
        for (uint_fast8_t i = 0; i < config->bNumInterfaces; i++) {
            free((void *)config->interface[i].altsetting);
        }
        if (config->desc != NULL) {
            free((void *)config->desc);
        }
        free(config);
    }
}

int libusb_get_descriptor(libusb_device_handle *dev_handle,
    uint8_t desc_type, uint8_t desc_index, unsigned char *data, int length)
{
    return libusb_control_transfer(dev_handle, LIBUSB_ENDPOINT_IN,
        LIBUSB_REQUEST_GET_DESCRIPTOR, (uint16_t) ((desc_type << 8) | desc_index),
        0, data, (uint16_t) length, 1000);
}

int libusb_get_string_descriptor(libusb_device_handle *dev_handle,
    uint8_t desc_index, uint16_t langid, unsigned char *data, int length)
{
    return libusb_control_transfer(dev_handle, LIBUSB_ENDPOINT_IN,
        LIBUSB_REQUEST_GET_DESCRIPTOR, (uint16_t)((LIBUSB_DT_STRING << 8) | desc_index),
        langid, data, (uint16_t) length, 1000);
}

static int __raw_desc_to_config(vsf_linux_libusb_dev_t *ldev, unsigned char *buf, struct libusb_config_descriptor **config)
{
    struct usb_config_desc_t *desc_config = (struct usb_config_desc_t *)buf;
    struct usb_interface_desc_t *desc_ifs;
    struct usb_descriptor_header_t *desc_header =
                (struct usb_descriptor_header_t *)desc_config, *header_tmp;
    struct libusb_config_descriptor *lconfig;

    struct libusb_interface *interface;
    struct libusb_interface_descriptor *interface_desc;
    struct libusb_endpoint_descriptor *endpoint_desc;

    uint_fast16_t size = desc_config->wTotalLength, len, tmpsize;
    uint_fast8_t ifs_no, alt_num, ep_num, reach_endpoint = false;

    enum {
        STAGE_NONE = 0,
        STAGE_ALLOC_ALT,
        STAGE_PROBE_ALT,
    } stage;

    if (desc_header->bDescriptorType != USB_DT_CONFIG) {
        return LIBUSB_ERROR_IO;
    }

    len = desc_config->bNumInterfaces * sizeof(struct libusb_interface);
    lconfig = calloc(1, len + sizeof(*lconfig));
    if (NULL == lconfig) { return LIBUSB_ERROR_NO_MEM; }
    interface = lconfig->interface = (struct libusb_interface *)&lconfig[1];
    lconfig->desc = (const unsigned char *)desc_config;
    lconfig->use_as__usb_config_desc_t = *desc_config;

    size -= desc_header->bLength;
    desc_header = (struct usb_descriptor_header_t *)((uint8_t *)desc_header + desc_header->bLength);

    interface_desc = NULL;
    endpoint_desc = NULL;
    stage = desc_header->bDescriptorType == USB_DT_INTERFACE ? STAGE_ALLOC_ALT : STAGE_NONE;
    ifs_no = 0;
    alt_num = 0;
    ep_num = 0;
    tmpsize = size;
    header_tmp = desc_header;
    while ((size > 0) && (size >= desc_header->bLength)) {
        switch (desc_header->bDescriptorType) {
        case USB_DT_INTERFACE:
            desc_ifs = (struct usb_interface_desc_t *)desc_header;
            if (!interface->altsetting) {
                if (desc_ifs->bInterfaceNumber == ifs_no) {
                    alt_num++;
                } else {
                alloc_alt:
                    interface->num_altsetting = alt_num;
                    interface->altsetting = calloc(1,
                            alt_num * sizeof(struct libusb_interface_descriptor)
                        +   ep_num * sizeof(struct libusb_endpoint_descriptor));
                    if (NULL == interface->altsetting) {
                        libusb_free_config_descriptor(lconfig);
                        return LIBUSB_ERROR_NO_MEM;
                    }

                    stage = STAGE_PROBE_ALT;
                    interface_desc = (struct libusb_interface_descriptor *)&interface->altsetting[-1];
                    endpoint_desc = (struct libusb_endpoint_descriptor *)&interface->altsetting[alt_num];
                    size = tmpsize;
                    desc_header = header_tmp;
                    alt_num = 0;
                    ep_num = 0;
                    reach_endpoint = false;
                    continue;
                }
            } else {
                if (desc_ifs->bInterfaceNumber == ifs_no) {
                    (++interface_desc)->use_as__usb_interface_desc_t = *desc_ifs;
                    if (interface_desc->bNumEndpoints > 0) {
                        interface_desc->endpoint = endpoint_desc;
                    }
                } else {
                probe_alt:
                    interface_desc = NULL;
                    endpoint_desc = NULL;
                    stage = size > 0 ? STAGE_ALLOC_ALT : STAGE_NONE;
                    ifs_no++;
                    if (ifs_no < lconfig->bNumInterfaces) {
                        interface++;
                    }
                    tmpsize = size;
                    header_tmp = desc_header;
                    continue;
                }
            }
            break;
        case USB_DT_ENDPOINT:
            reach_endpoint = true;
            if (endpoint_desc) {
                endpoint_desc->use_as__usb_endpoint_desc_t = *(struct usb_endpoint_desc_t *)desc_header;
                if ((endpoint_desc->bEndpointAddress & USB_DIR_MASK) == USB_DIR_IN) {
                    ldev->pipe_in[endpoint_desc->bEndpointAddress & 0x0F] =
                        vk_usbh_get_pipe_from_ep_desc(ldev->libusb_dev->dev, (struct usb_endpoint_desc_t *)&endpoint_desc->use_as__usb_endpoint_desc_t);
                } else {
                    ldev->pipe_out[endpoint_desc->bEndpointAddress & 0x0F] =
                        vk_usbh_get_pipe_from_ep_desc(ldev->libusb_dev->dev, (struct usb_endpoint_desc_t *)&endpoint_desc->use_as__usb_endpoint_desc_t);
                }
                endpoint_desc++;
            } else {
                ep_num++;
            }
            break;
        default:
            if ((interface_desc != NULL) && (interface_desc >= &interface->altsetting[0])) {
                if (!reach_endpoint) {
                    if (NULL == interface_desc->extra) {
                        interface_desc->extra = (const unsigned char *)desc_header;
                    }
                    interface_desc->extra_length += desc_header->bLength;
                } else if (endpoint_desc != NULL) {
                    if (NULL == endpoint_desc->extra) {
                        endpoint_desc->extra = (const unsigned char *)desc_header;
                    }
                    endpoint_desc->extra_length += desc_header->bLength;
                }
            }
        }

        size -= desc_header->bLength;
        desc_header = (struct usb_descriptor_header_t *)((uint8_t *)desc_header + desc_header->bLength);
        if (!size && stage) {
            if (stage == STAGE_ALLOC_ALT) {
                goto alloc_alt;
            } else /*if (stage == STAGE_PROBE_ALT)*/ {
                desc_ifs = (struct usb_interface_desc_t *)desc_header;
                goto probe_alt;
            }
        }
    }
    if (config != NULL) {
        *config = lconfig;
    }
    return 0;
}

int libusb_get_active_config_descriptor(libusb_device *dev,
        struct libusb_config_descriptor **config)
{
    int err, config_val;

    err = libusb_get_configuration((libusb_device_handle *)dev, &config_val);
    if (err != LIBUSB_SUCCESS) { return err; }
    return libusb_get_config_descriptor(dev, config_val - 1, config);
}

int libusb_get_config_descriptor(libusb_device *dev, uint8_t config_index,
        struct libusb_config_descriptor **config)
{
    struct usb_config_desc_t config_desc;
    unsigned char *buf = NULL;
    int err;

    err = libusb_get_descriptor((libusb_device_handle *)dev, USB_DT_CONFIG,
            config_index, (unsigned char *)&config_desc, LIBUSB_DT_CONFIG_SIZE);
    if (err < 0) { return err; }
    if (err < LIBUSB_DT_CONFIG_SIZE) {
        return LIBUSB_ERROR_IO;
    }
    config_desc.wTotalLength = le16_to_cpu(config_desc.wTotalLength);

    buf = malloc(config_desc.wTotalLength);
    if (!buf) { return LIBUSB_ERROR_NO_MEM; }

    err = libusb_get_descriptor((libusb_device_handle *)dev, USB_DT_CONFIG,
            config_index, buf, config_desc.wTotalLength);
    if (err >= 0) {
        vsf_linux_libusb_dev_t *ldev = (vsf_linux_libusb_dev_t *)dev;
        memset(&ldev->pipe_in[1], 0, sizeof(vk_usbh_pipe_t) * (dimof(ldev->pipe_in) - 1));
        memset(&ldev->pipe_out[1], 0, sizeof(vk_usbh_pipe_t) * (dimof(ldev->pipe_out) - 1));

        err = __raw_desc_to_config(ldev, buf, config);
        if (err < 0) {
            free(buf);
        }
    }

    return err;
}

void libusb_free_ss_endpoint_companion_descriptor(
	struct libusb_ss_endpoint_companion_descriptor *ep_comp)
{
}

int libusb_get_ss_endpoint_companion_descriptor(
	struct libusb_context *ctx,
	const struct libusb_endpoint_descriptor *endpoint,
	struct libusb_ss_endpoint_companion_descriptor **ep_comp)
{
    return -1;
}

struct libusb_transfer * libusb_alloc_transfer(int iso_packets)
{
    uint32_t size = sizeof(vsf_linux_libusb_transfer_t) +
            iso_packets * sizeof(struct libusb_iso_packet_descriptor);
    vsf_linux_libusb_transfer_t *ltransfer = calloc(1, size);
    if (ltransfer != NULL) {
        ltransfer->urb.pipe.is_pipe = true;
        ltransfer->transfer.num_iso_packets = iso_packets;
        return &ltransfer->transfer;
    }
    return NULL;
}

static void __vsf_libusb_transfer_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vsf_linux_libusb_transfer_t *ltransfer = container_of(eda, vsf_linux_libusb_transfer_t, eda);
    vsf_linux_libusb_dev_t *ldev = (vsf_linux_libusb_dev_t *)ltransfer->transfer.dev_handle;
    vk_usbh_pipe_t *pipe = __vsf_libusb_get_pipe(ldev, ltransfer->transfer.endpoint);
    vk_usbh_urb_t *urb = &ltransfer->urb;
//    vsf_protect_t orig;

    switch (evt) {
    case VSF_EVT_MESSAGE:
        *pipe = urb->urb_hcd->pipe;
        ltransfer->transfer.actual_length = vk_usbh_urb_get_actual_length(urb);
        ltransfer->transfer.status = vk_usbh_urb_get_status(urb) == URB_OK ?
                LIBUSB_TRANSFER_COMPLETED : LIBUSB_TRANSFER_ERROR;
        vk_usbh_free_urb(ldev->libusb_dev->usbh, urb);

        // all thread running under same priority as processor priority
        //  so no need to protect
//        orig = vsf_protect_sched();
            vsf_dlist_add_to_tail(vsf_linux_libusb_transfer_t, transnode, &__vsf_libusb.translist_done, ltransfer);
//        vsf_unprotect_sched(orig);
        vsf_eda_trig_set(&__vsf_libusb.trans_trig);
        break;
    }
}

int libusb_submit_transfer(struct libusb_transfer *transfer)
{
    vsf_linux_libusb_transfer_t *ltransfer = container_of(transfer, vsf_linux_libusb_transfer_t, transfer);
    vsf_linux_libusb_dev_t *ldev = (vsf_linux_libusb_dev_t *)transfer->dev_handle;
    vk_usbh_pipe_t *pipe = __vsf_libusb_get_pipe(ldev, ltransfer->transfer.endpoint);
    vk_usbh_urb_t *urb = &ltransfer->urb;
    int err = LIBUSB_SUCCESS;

    if (!vk_usbh_urb_is_alloced(urb)) {
        if (VSF_ERR_NONE != vk_usbh_alloc_urb(ldev->libusb_dev->usbh, ldev->libusb_dev->dev, urb)) {
            return -1;
        }
    }
    urb->urb_hcd->buffer = transfer->buffer;
    urb->urb_hcd->transfer_length = transfer->length;
    urb->urb_hcd->timeout = transfer->timeout;
    vk_usbh_urb_set_pipe(urb, *pipe);

    switch (transfer->type) {
    case LIBUSB_TRANSFER_TYPE_CONTROL: {
            struct usb_ctrlrequest_t *request = (struct usb_ctrlrequest_t *)transfer->buffer;
            urb->urb_hcd->setup_packet = *request;
            urb->urb_hcd->buffer = (uint8_t *)urb->urb_hcd->buffer + sizeof(struct usb_ctrlrequest_t);
            urb->urb_hcd->transfer_length -= sizeof(struct usb_ctrlrequest_t);
        }
        break;
    case LIBUSB_TRANSFER_TYPE_ISOCHRONOUS:
#if VSF_USBH_CFG_ISO_EN == ENABLED
        urb->urb_hcd->iso_packet.number_of_packets = transfer->num_iso_packets;
        break;
#else
        return -1;
#endif
    case LIBUSB_TRANSFER_TYPE_BULK:
    case LIBUSB_TRANSFER_TYPE_INTERRUPT:
        break;
    }

    ltransfer->eda.fn.evthandler = __vsf_libusb_transfer_evthandler;
    vsf_eda_init(&ltransfer->eda, vsf_prio_inherit, false);

    if (VSF_ERR_NONE != vk_usbh_submit_urb_ex(ldev->libusb_dev->usbh, urb, 0, &ltransfer->eda)) {
        err = LIBUSB_ERROR_IO;
        vsf_eda_fini(&ltransfer->eda);
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
    int timeout_ms = tv->tv_sec * 1000 + tv->tv_usec / 1000;
    poll((struct pollfd *)__vsf_libusb.pollfd, dimof(__vsf_libusb.pollfd), timeout_ms);
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
