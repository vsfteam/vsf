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

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_HCD_LIBUSB == ENABLED

#define __VSF_USBH_CLASS_IMPLEMENT_HCD__
#define __VSF_USBH_CLASS_IMPLEMENT_HUB__
#define __VSF_EDA_CLASS_INHERIT__

#include "kernel/vsf_kernel.h"
#include "component/usb/host/vsf_usbh.h"
#include "./vsf_libusb_hcd.h"
#include "utilities/ooc_class.h"

#ifndef VSF_LIBUSB_CFG_INCLUDE
#   include "libusb.h"
#else
#   include VSF_LIBUSB_CFG_INCLUDE
#endif

#if defined(__WIN__) && VSF_LIBUSB_CFG_INSTALL_DRIVER == ENABLED
#   include "libwdi.h"
#endif

/*=================== replacement for libusb 1.0 APIs ========================*/

#ifndef LIBUSB_API_VERSION
// seems libusb 0.1
// implement libusb 1.0 API wrapper

enum libusb_error {
    LIBUSB_SUCCESS                      = 0,
    LIBUSB_ERROR_IO                     = -1,
    LIBUSB_ERROR_INVALID_PARAM          = -2,
    LIBUSB_ERROR_ACCESS                 = -3,
    LIBUSB_ERROR_NO_DEVICE              = -4,
    LIBUSB_ERROR_NOT_FOUND              = -5,
    LIBUSB_ERROR_BUSY                   = -6,
    LIBUSB_ERROR_TIMEOUT                = -7,
    LIBUSB_ERROR_OVERFLOW               = -8,
    LIBUSB_ERROR_PIPE                   = -9,
    LIBUSB_ERROR_INTERRUPTED            = -10,
    LIBUSB_ERROR_NO_MEM                 = -11,
    LIBUSB_ERROR_NOT_SUPPORTED          = -12,
    LIBUSB_ERROR_OTHER                  = -99,
};

#define LIBUSB_CALL
#define libusb_hotplug_register_callback(...)   (LIBUSB_ERROR_NOT_SUPPORTED)
#define libusb_has_capability(...)              (false)

#define libusb_close                    usb_close
// TODO: usb_reset will make device handle invalid
//#define libusb_reset_device             usb_reset
#define libusb_reset_device
#define libusb_claim_interface          usb_claim_interface
#define libusb_control_transfer         usb_control_msg

typedef usb_dev_handle                  libusb_device_handle;
typedef struct usb_device               libusb_device;
typedef void *                          libusb_context;
typedef enum {
    LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED = 0x01U,
    LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT    = 0x02U,
} libusb_hotplug_event;

struct libusb_device_descriptor {
    struct usb_device_descriptor;
};
struct libusb_config_descriptor {
    struct usb_config_descriptor;
};

enum libusb_speed {
    LIBUSB_SPEED_UNKNOWN,
    LIBUSB_SPEED_LOW,
    LIBUSB_SPEED_FULL,
    LIBUSB_SPEED_HIGH,
    LIBUSB_SPEED_SUPER,
    LIBUSB_SPEED_SUPER_PLUS,
};
#define libusb_get_device_speed(__dev)  (LIBUSB_SPEED_UNKNOWN)
#define libusb_get_device(__handle)     ((libusb_device *)usb_device(__handle))

int LIBUSB_CALL libusb_init(libusb_context **ctx)
{
    usb_init();
    usb_find_busses();
    return LIBUSB_SUCCESS;
}

libusb_device_handle * LIBUSB_CALL libusb_open_device_with_vid_pid(
    libusb_context *ctx, uint16_t vendor_id, uint16_t product_id)
{
    struct usb_bus *bus;
    struct usb_device *dev;

    usb_find_devices();
    for (bus = usb_get_busses(); bus; bus = bus->next) {
        for (dev = bus->devices; dev; dev = dev->next) {
            if (    (dev->descriptor.idVendor == vendor_id)
                &&  (dev->descriptor.idProduct == product_id)) {

                return usb_open(dev);
            }
        }
    }
    return NULL;
}

int LIBUSB_CALL libusb_get_config_descriptor_by_value(libusb_device *dev,
	uint8_t bConfigurationValue, struct libusb_config_descriptor **config)
{
    *config = (struct libusb_config_descriptor *)dev->config;
    VSF_USB_ASSERT(bConfigurationValue == dev->config->bConfigurationValue);
    return LIBUSB_SUCCESS;
}

void LIBUSB_CALL libusb_free_config_descriptor(
	struct libusb_config_descriptor *config)
{
    // config is not dynamically alloced
}

int LIBUSB_CALL libusb_bulk_transfer(libusb_device_handle *dev_handle,
	unsigned char endpoint, unsigned char *data, int length,
	int *actual_length, unsigned int timeout)
{
    int result;
    if (endpoint & 0x80) {
        result = usb_bulk_read(dev_handle, endpoint, (char *)data, length, timeout);
    } else {
        result = usb_bulk_write(dev_handle, endpoint, (char *)data, length, timeout);
    }
    if (actual_length != NULL) {
        *actual_length = result;
    }
    return LIBUSB_SUCCESS;
}

int LIBUSB_CALL libusb_interrupt_transfer(libusb_device_handle *dev_handle,
	unsigned char endpoint, unsigned char *data, int length,
	int *actual_length, unsigned int timeout)
{
    int result;
    if (endpoint & 0x80) {
        result = usb_interrupt_read(dev_handle, endpoint, (char *)data, length, timeout);
    } else {
        result = usb_interrupt_write(dev_handle, endpoint, (char *)data, length, timeout);
    }
    if (actual_length != NULL) {
        *actual_length = result;
    }
    return LIBUSB_SUCCESS;
}

#endif

/*============================ MACROS ========================================*/

// TODO: remove to user configuration
//#define VSF_LIBUSB_HCD_CFG_TRACE_URB_EN             ENABLED
//#define VSF_LIBUSB_HCD_CFG_TRACE_IRQ_EN             ENABLED

#define VSF_EVT_LIBUSB_HCD_BASE                     ((VSF_EVT_USER + 0x100) & ~0xFF)

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_LIBUSB_HCD_DEF_DEV(__N, __BIT)                                      \
            {                                                                   \
                .vid = VSF_LIBUSB_HCD_DEV##__N##_VID,                           \
                .pid = VSF_LIBUSB_HCD_DEV##__N##_PID,                           \
                .addr = -1,                                                     \
            },

/*============================ TYPES =========================================*/

typedef struct vk_libusb_hcd_dev_t {
    uint16_t vid, pid;
    libusb_device_handle *handle;
    vk_usbh_dev_t *dev;

    enum usb_device_speed_t speed;
    enum {
        VSF_LIBUSB_HCD_DEV_STATE_DETACHED,
        VSF_LIBUSB_HCD_DEV_STATE_DETACHING,
        VSF_LIBUSB_HCD_DEV_STATE_ATTACHED,
    } state;

    int8_t addr;
    union {
        uint8_t value;
        struct {
            uint8_t is_resetting    : 1;
            uint8_t is_attaching    : 1;
            uint8_t is_detaching    : 1;
            uint8_t is_detached     : 1;
        };
    } evt_mask;

    vsf_arch_irq_thread_t irq_thread;
    vsf_arch_irq_request_t irq_request;
    vsf_dlist_t urb_pending_list;
} vk_libusb_hcd_dev_t;

typedef struct vk_libusb_hcd_t {
    libusb_context *ctx;

    vk_libusb_hcd_dev_t devs[VSF_LIBUSB_HCD_CFG_DEV_NUM];
    vk_usbh_hcd_t *hcd;
    uint32_t new_mask;
    uint8_t cur_dev_idx;
    bool is_hotplug_supported;

    vsf_eda_t *init_eda;
    vsf_arch_irq_thread_t init_thread;
    vsf_teda_t teda;
    vsf_sem_t sem;
    vsf_dlist_t urb_list;
} vk_libusb_hcd_t;

typedef struct vk_libusb_hcd_urb_t {
    vsf_dlist_node_t urb_node;
    vsf_dlist_node_t urb_pending_node;

    enum {
        VSF_LIBUSB_HCD_URB_STATE_IDLE,
        VSF_LIBUSB_HCD_URB_STATE_QUEUED,
        VSF_LIBUSB_HCD_URB_STATE_SUBMITTING,
        VSF_LIBUSB_HCD_URB_STATE_WAIT_TO_FREE,
        VSF_LIBUSB_HCD_URB_STATE_TO_FREE,
    } state;

    bool is_irq_enabled;
    bool is_msg_processed;

    vsf_arch_irq_thread_t irq_thread;
    vsf_arch_irq_request_t irq_request;
} vk_libusb_hcd_urb_t;

typedef enum vk_libusb_hcd_evt_t {
    VSF_EVT_LIBUSB_HCD_ATTACH           = VSF_EVT_LIBUSB_HCD_BASE + 0x100,
    VSF_EVT_LIBUSB_HCD_DETACH           = VSF_EVT_LIBUSB_HCD_BASE + 0x200,
    VSF_EVT_LIBUSB_HCD_READY            = VSF_EVT_LIBUSB_HCD_BASE + 0x300,
} vk_libusb_hcd_evt_t;

/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vk_libusb_hcd_init_evthandler(vsf_eda_t *eda, vsf_evt_t evt, vk_usbh_hcd_t *hcd);
static vsf_err_t __vk_libusb_hcd_fini(vk_usbh_hcd_t *hcd);
static vsf_err_t __vk_libusb_hcd_suspend(vk_usbh_hcd_t *hcd);
static vsf_err_t __vk_libusb_hcd_resume(vk_usbh_hcd_t *hcd);
static uint_fast16_t __vk_libusb_hcd_get_frame_number(vk_usbh_hcd_t *hcd);
static vsf_err_t __vk_libusb_hcd_alloc_device(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);
static void __vk_libusb_hcd_free_device(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);
static vk_usbh_hcd_urb_t * __vk_libusb_hcd_alloc_urb(vk_usbh_hcd_t *hcd);
static void __vk_libusb_hcd_free_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
static vsf_err_t __vk_libusb_hcd_submit_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
static vsf_err_t __vk_libusb_hcd_relink_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
static vsf_err_t __vk_libusb_hcd_reset_dev(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);
static bool __vk_libusb_hcd_is_dev_reset(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);

static void __vk_libusb_hcd_dev_thread(void *arg);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbh_hcd_drv_t vk_libusb_hcd_drv = {
    .init_evthandler    = __vk_libusb_hcd_init_evthandler,
    .fini               = __vk_libusb_hcd_fini,
    .suspend            = __vk_libusb_hcd_suspend,
    .resume             = __vk_libusb_hcd_resume,
    .get_frame_number   = __vk_libusb_hcd_get_frame_number,
    .alloc_device       = __vk_libusb_hcd_alloc_device,
    .free_device        = __vk_libusb_hcd_free_device,
    .alloc_urb          = __vk_libusb_hcd_alloc_urb,
    .free_urb           = __vk_libusb_hcd_free_urb,
    .submit_urb         = __vk_libusb_hcd_submit_urb,
    .relink_urb         = __vk_libusb_hcd_relink_urb,
    .reset_dev          = __vk_libusb_hcd_reset_dev,
    .is_dev_reset       = __vk_libusb_hcd_is_dev_reset,
};

/*============================ LOCAL VARIABLES ===============================*/

static vk_libusb_hcd_t __vk_libusb_hcd = {
    .devs = {
        REPEAT_MACRO(VSF_LIBUSB_HCD_CFG_DEV_NUM, VSF_LIBUSB_HCD_DEF_DEV, NULL)
    },
};

/*============================ IMPLEMENTATION ================================*/

#if VSF_LIBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
static void __vk_libusb_hcd_trace_urb(vk_usbh_hcd_urb_t *urb, char *msg)
{
    vk_usbh_pipe_t pipe = urb->pipe;
    vsf_trace_info("libusb_urb EP%d_%s(%08X): %s\r\n", pipe.endpoint,
        pipe.dir_in1out0 ? "IN" : "OUT", urb, msg);
}
#endif

#if VSF_LIBUSB_HCD_CFG_TRACE_IRQ_EN == ENABLED
static void __vk_libusb_hcd_trace_dev_irq(vk_libusb_hcd_dev_t *libusb_dev, char *msg)
{
    vsf_trace_info("libusb_dev_irq(%08X): %s\r\n", &libusb_dev->irq_thread, msg);
}

static void __vk_libusb_hcd_trace_hcd_irq(char *msg)
{
    vsf_trace_info("libusb_hcd_irq(%08X): %s\r\n", &__vk_libusb_hcd.init_thread, msg);
}

static void __vk_libusb_hcd_trace_urb_irq(vk_usbh_hcd_urb_t *urb, char *msg)
{
    vk_usbh_pipe_t pipe = urb->pipe;
    vsf_trace_info("libusb_urb EP%d_%s(%08X): irq %s\r\n", pipe.endpoint,
        pipe.dir_in1out0 ? "IN" : "OUT", urb, msg);
}
#endif

#if defined(__WIN__) && VSF_LIBUSB_CFG_INSTALL_DRIVER == ENABLED
static bool __vk_libusb_ensure_driver(uint_fast16_t vid, uint_fast16_t pid, bool force)
{
    struct wdi_options_create_list cl_options = {
        .list_all           = TRUE,
        .list_hubs          = TRUE,
        .trim_whitespaces   = TRUE,
    };
    struct wdi_options_prepare_driver pd_options = {
#   ifndef LIBUSB_API_VERSION
        .driver_type        = WDI_LIBUSB0,
#   else
        .driver_type        = WDI_WINUSB,
#   endif
    };
    struct wdi_device_info *device, *list;
    int r = WDI_ERROR_OTHER;

    wdi_set_log_level(3);
    r = wdi_create_list(&list, &cl_options);
    if (r != WDI_SUCCESS) {
        return false;
    }

    for (device = list; device != NULL; device = device->next) {
        if (    (device->vid == vid) && (device->pid == pid) && !device->is_composite
            &&  (   force ||
#   ifndef LIBUSB_API_VERSION
                    strcmp(device->driver, "libusb0")
#   else
                    strcmp(device->driver, "WinUSB")
#   endif
                )) {
            if (wdi_prepare_driver(device, "usb_driver", "libusb_device.inf", &pd_options) == WDI_SUCCESS) {
                wdi_install_driver(device, "usb_driver", "libusb_device.inf", NULL);
            }
            break;
        }
    }
    wdi_destroy_list(list);
    return true;
}
#endif

static void __vk_libusb_hcd_on_left(vk_libusb_hcd_dev_t *libusb_dev)
{
    libusb_dev->evt_mask.is_detaching = true;
    __vsf_arch_irq_request_send(&libusb_dev->irq_request);
}

static void __vk_libusb_hcd_on_arrived(vk_libusb_hcd_dev_t *libusb_dev)
{
    libusb_device *device = libusb_get_device(libusb_dev->handle);
    switch (libusb_get_device_speed(device)) {
        case LIBUSB_SPEED_UNKNOWN:
            libusb_dev->speed = USB_SPEED_UNKNOWN;
            break;
        case LIBUSB_SPEED_LOW:
            libusb_dev->speed = USB_SPEED_LOW;
            break;
        case LIBUSB_SPEED_FULL:
            libusb_dev->speed = USB_SPEED_FULL;
            break;
        case LIBUSB_SPEED_HIGH:
            libusb_dev->speed = USB_SPEED_HIGH;
            break;
        case LIBUSB_SPEED_SUPER_PLUS:
        case LIBUSB_SPEED_SUPER:
            libusb_dev->speed = USB_SPEED_SUPER;
            break;
    }
    libusb_dev->evt_mask.is_attaching = true;
    __vsf_arch_irq_request_send(&libusb_dev->irq_request);
}

static int LIBUSB_CALL __vk_libusb_hcd_hotplug_cb(libusb_context *ctx, libusb_device *device,
    libusb_hotplug_event event, void *user_data)
{
    struct libusb_device_descriptor desc_device;
    vk_libusb_hcd_dev_t *libusb_dev = NULL;

    if (LIBUSB_SUCCESS == libusb_get_device_descriptor(device, &desc_device)) {
        for (int i = 0; i < dimof(__vk_libusb_hcd.devs); i++) {
            if (    (__vk_libusb_hcd.devs[i].vid == desc_device.idVendor)
                &&  (__vk_libusb_hcd.devs[i].pid == desc_device.idProduct)) {
                libusb_dev = &__vk_libusb_hcd.devs[i];
                break;
            }
        }

        if (libusb_dev != NULL) {
            switch (event) {
            case LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED:
                if (NULL == libusb_dev->handle) {
                    if (LIBUSB_SUCCESS == libusb_open(device, &libusb_dev->handle)) {
                        __vk_libusb_hcd_on_arrived(libusb_dev);
                    } else {
                        libusb_dev->handle = NULL;
                    }
                }
                break;
            case LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT:
                if (libusb_dev->handle != NULL) {
                    __vk_libusb_hcd_on_left(libusb_dev);
                }
                break;
            }
        }
    }
    return 0;
}

// return 0 on success, non-0 otherwise
static int __vk_libusb_hcd_init(void)
{
    vk_libusb_hcd_param_t *param = __vk_libusb_hcd.hcd->param;
    vk_libusb_hcd_dev_t *libusb_dev;
    int err;

    err = libusb_init(&__vk_libusb_hcd.ctx);
    if (err < 0) {
        vsf_trace_error("fail to init libusb, errcode is %d\r\n", err);
        return err;
    }

    __vk_libusb_hcd.is_hotplug_supported = libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG);

    for (int i = 0; i < dimof(__vk_libusb_hcd.devs); i++) {
        libusb_dev = &__vk_libusb_hcd.devs[i];
        libusb_dev->state = VSF_LIBUSB_HCD_DEV_STATE_DETACHED;
        libusb_dev->evt_mask.value = 0;
        libusb_dev->handle = NULL;
        libusb_dev->addr = -1;

        __vsf_arch_irq_request_init(&libusb_dev->irq_request);
#if VSF_LIBUSB_HCD_CFG_TRACE_IRQ_EN == ENABLED
        __vk_libusb_hcd_trace_dev_irq(libusb_dev, "init");
#endif
        __vsf_arch_irq_init(&libusb_dev->irq_thread, "libusb_hcd_dev", __vk_libusb_hcd_dev_thread, param->priority);

        if (__vk_libusb_hcd.is_hotplug_supported) {
            libusb_hotplug_register_callback(__vk_libusb_hcd.ctx,
                LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,
                LIBUSB_HOTPLUG_ENUMERATE, libusb_dev->vid, libusb_dev->pid, LIBUSB_HOTPLUG_MATCH_ANY,
                __vk_libusb_hcd_hotplug_cb, NULL, NULL);
        }
    }
    return 0;
}

// TODO: call libusb_claim_interface for non-control transfer
static int __vk_libusb_hcd_submit_urb_do(vk_usbh_hcd_urb_t *urb)
{
    vk_usbh_hcd_dev_t *dev = urb->dev_hcd;
    vk_libusb_hcd_dev_t *libusb_dev = dev->dev_priv;
    vk_usbh_pipe_t pipe = urb->pipe;

    if (NULL == libusb_dev->handle) {
        return LIBUSB_ERROR_NO_DEVICE;
    }

    switch (pipe.type) {
    case USB_ENDPOINT_XFER_CONTROL:
        if (pipe.endpoint != 0) {
            return LIBUSB_ERROR_INVALID_PARAM;
        } else {
            struct usb_ctrlrequest_t *setup = &urb->setup_packet;

            if (    ((USB_RECIP_DEVICE | USB_DIR_OUT) == setup->bRequestType)
                &&  (USB_REQ_SET_CONFIGURATION == setup->bRequest)) {
#if 0
                return libusb_set_configuration(libusb_dev->handle, setup->wValue);
#else
                // TODO; libusb_set_configuration will fail on windows paltform
                return 0;
#endif
            } else {
                return libusb_control_transfer(libusb_dev->handle, setup->bRequestType,
                        setup->bRequest, setup->wValue, setup->wIndex, urb->buffer,
                        setup->wLength, urb->timeout);
            }
        }
    case USB_ENDPOINT_XFER_ISOC:
        // TODO: add support to iso transfer
        return LIBUSB_ERROR_NOT_SUPPORTED;
    case USB_ENDPOINT_XFER_BULK: {
            int actual_length;
            int err = libusb_bulk_transfer(libusb_dev->handle,
                        (pipe.dir_in1out0 ? 0x80 : 0x00) | pipe.endpoint,
                        urb->buffer, urb->transfer_length, &actual_length, urb->timeout);
            if (err < 0) {
                return err;
            } else {
                return actual_length;
            }
        }
    case USB_ENDPOINT_XFER_INT: {
            int actual_length;
            int err = libusb_interrupt_transfer(libusb_dev->handle,
                        (pipe.dir_in1out0 ? 0x80 : 0x00) | pipe.endpoint,
                        urb->buffer, urb->transfer_length, &actual_length, urb->timeout);
            if (err < 0) {
                return err;
            } else {
                return actual_length;
            }
        }
        break;
    }
    return LIBUSB_ERROR_INVALID_PARAM;
}

static void __vk_libusb_hcd_dev_thread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = arg;
    vk_libusb_hcd_dev_t *libusb_dev = container_of(irq_thread, vk_libusb_hcd_dev_t, irq_thread);
    vsf_arch_irq_request_t *irq_request = &libusb_dev->irq_request;
    int idx = libusb_dev - &__vk_libusb_hcd.devs[0];

    __vsf_arch_irq_set_background(irq_thread);
    while (1) {
        __vsf_arch_irq_request_pend(irq_request);

        if (libusb_dev->evt_mask.is_attaching) {
            libusb_dev->evt_mask.is_attaching = false;
            __vsf_arch_irq_start(irq_thread);
                vsf_eda_post_evt(&__vk_libusb_hcd.teda.use_as__vsf_eda_t, VSF_EVT_LIBUSB_HCD_ATTACH | idx);
            __vsf_arch_irq_end(irq_thread, false);
        }
        if (libusb_dev->evt_mask.is_detaching) {
            __vsf_arch_irq_start(irq_thread);
                vsf_eda_post_evt(&__vk_libusb_hcd.teda.use_as__vsf_eda_t, VSF_EVT_LIBUSB_HCD_DETACH | idx);
            __vsf_arch_irq_end(irq_thread, false);
        }
        if (libusb_dev->evt_mask.is_detached) {
            libusb_close(libusb_dev->handle);
            libusb_dev->handle = NULL;
            libusb_dev->evt_mask.is_detached = false;
        }
        if (libusb_dev->evt_mask.is_resetting) {
            libusb_reset_device(libusb_dev->handle);
            libusb_dev->evt_mask.is_resetting = false;
        }
    }
}

static void __vk_libusb_hcd_urb_thread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = arg;
    vk_libusb_hcd_urb_t *libusb_urb = container_of(irq_thread, vk_libusb_hcd_urb_t, irq_thread);
    vk_usbh_hcd_urb_t *urb = container_of(libusb_urb, vk_usbh_hcd_urb_t, priv);
    vsf_arch_irq_request_t *irq_request = &libusb_urb->irq_request;
    bool is_to_free;
    int actual_length;

    __vsf_arch_irq_set_background(irq_thread);
    while (1) {
        __vsf_arch_irq_request_pend(irq_request);

        while (!libusb_urb->is_msg_processed) {
            Sleep(1);
        }

        is_to_free = VSF_LIBUSB_HCD_URB_STATE_TO_FREE == libusb_urb->state;
        if (!is_to_free) {
            vk_usbh_hcd_dev_t *dev = urb->dev_hcd;
            vk_libusb_hcd_dev_t *libusb_dev = dev->dev_priv;

#if VSF_LIBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
            __vsf_arch_irq_start(irq_thread);
                __vk_libusb_hcd_trace_urb(urb, "submitting");
            __vsf_arch_irq_end(irq_thread, false);
#endif
            actual_length = __vk_libusb_hcd_submit_urb_do(urb);
#if VSF_LIBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
            __vsf_arch_irq_start(irq_thread);
                __vk_libusb_hcd_trace_urb(urb, "done");
            __vsf_arch_irq_end(irq_thread, false);
#endif
            if (actual_length < 0) {
                urb->status = actual_length;
            } else {
                urb->status = URB_OK;
                urb->actual_length = actual_length;

                if (USB_ENDPOINT_XFER_CONTROL == urb->pipe.type) {
                    struct usb_ctrlrequest_t *setup = &urb->setup_packet;

                    // set configuration is handled here
                    if (    ((USB_RECIP_DEVICE | USB_DIR_OUT) == setup->bRequestType)
                        &&  (USB_REQ_SET_CONFIGURATION == setup->bRequest)) {

                        int config = setup->wValue;
                        struct libusb_config_descriptor *config_desc;

                        if (LIBUSB_SUCCESS == libusb_get_config_descriptor_by_value(
                                    libusb_get_device(libusb_dev->handle), config, &config_desc)) {
                            for (uint8_t i = 0; i < config_desc->bNumInterfaces; i++) {
                                libusb_claim_interface(libusb_dev->handle, i);
                            }
                            libusb_free_config_descriptor(config_desc);
                        }
                    }
                }
            }

#if VSF_LIBUSB_HCD_CFG_REMOVE_ON_ERROR == ENABLED
            if (urb->status == LIBUSB_ERROR_IO) {
                // device removed
#   if VSF_LIBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
                __vsf_arch_irq_start(irq_thread);
                    __vk_libusb_hcd_trace_urb(urb, "device removed, to free");
                __vsf_arch_irq_end(irq_thread, false);
#   endif
                if (libusb_urb->state != VSF_LIBUSB_HCD_URB_STATE_TO_FREE) {
                    libusb_urb->state = VSF_LIBUSB_HCD_URB_STATE_WAIT_TO_FREE;
                }
                __vk_libusb_hcd_on_left(libusb_dev);
            }
#endif
        }

        __vsf_arch_irq_start(irq_thread);
#if VSF_LIBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
            __vk_libusb_hcd_trace_urb(urb, "post msg in irq");
#endif
            if (is_to_free) {
                libusb_urb->is_irq_enabled = false;
#if VSF_LIBUSB_HCD_CFG_TRACE_IRQ_EN == ENABLED
                __vk_libusb_hcd_trace_urb_irq(urb, "fini");
#endif
            }
            libusb_urb->is_msg_processed = false;
            vsf_eda_post_msg(&__vk_libusb_hcd.teda.use_as__vsf_eda_t, urb);
        __vsf_arch_irq_end(irq_thread, false);

        if (is_to_free) {
            __vsf_arch_irq_fini(irq_thread);
            __vsf_arch_irq_request_fini(irq_request);
            return;
        }
    }
}

static void __vk_libusb_hcd_init_thread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = arg;

    __vsf_arch_irq_set_background(irq_thread);
        __vk_libusb_hcd_init();
    __vsf_arch_irq_start(irq_thread);
        vsf_eda_post_evt(__vk_libusb_hcd.init_eda, VSF_EVT_LIBUSB_HCD_READY);
    __vsf_arch_irq_end(irq_thread, false);

    while (!__vk_libusb_hcd.is_hotplug_supported) {
        vk_libusb_hcd_dev_t *libusb_dev = &__vk_libusb_hcd.devs[0];
        for (int i = 0; i < dimof(__vk_libusb_hcd.devs); i++, libusb_dev++) {
            if ((NULL == libusb_dev->handle) && (0 == libusb_dev->evt_mask.value)) {
#if defined(__WIN__) && VSF_LIBUSB_CFG_INSTALL_DRIVER == ENABLED
                __vk_libusb_ensure_driver(libusb_dev->vid, libusb_dev->pid, false);
#endif
                libusb_dev->handle = libusb_open_device_with_vid_pid(
                    __vk_libusb_hcd.ctx, libusb_dev->vid, libusb_dev->pid);
                if (libusb_dev->handle != NULL) {
                    __vk_libusb_hcd_on_arrived(libusb_dev);
                }
            }
        }
        Sleep(100);
    }
#if VSF_LIBUSB_HCD_CFG_TRACE_IRQ_EN == ENABLED
    __vsf_arch_irq_start(irq_thread);
        __vk_libusb_hcd_trace_hcd_irq("fini");
    __vsf_arch_irq_end(irq_thread, false);
#endif
    __vsf_arch_irq_fini(irq_thread);
}






static bool __vk_libusb_hcd_free_urb_do(vk_usbh_hcd_urb_t *urb)
{
    vk_libusb_hcd_urb_t *libusb_urb = (vk_libusb_hcd_urb_t *)urb->priv;
    if (libusb_urb->is_irq_enabled) {
#if VSF_LIBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
        __vk_libusb_hcd_trace_urb(urb, "irq running, try to exit first");
#endif
        VSF_USB_ASSERT(VSF_LIBUSB_HCD_URB_STATE_TO_FREE == libusb_urb->state);
        __vsf_arch_irq_request_send(&libusb_urb->irq_request);
        return false;
    } else {
#if VSF_LIBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
        __vk_libusb_hcd_trace_urb(urb, "freed");
#endif
        vk_usbh_hcd_urb_free_buffer(urb);
        vsf_usbh_free(urb);
        return true;
    }
}

static void __vk_libusb_hcd_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_libusb_hcd_t *libusb = container_of(eda, vk_libusb_hcd_t, teda);

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_dlist_init(&__vk_libusb_hcd.urb_list);
        vsf_eda_sem_init(&__vk_libusb_hcd.sem, 0);
        vsf_teda_set_timer_ms(100);

    wait_next_urb:
        if (vsf_eda_sem_pend(&__vk_libusb_hcd.sem, -1)) {
            break;
        }
        // fall through
    case VSF_EVT_SYNC: {
            vk_libusb_hcd_urb_t *libusb_urb;
            vsf_protect_t orig = vsf_protect_sched();
                vsf_dlist_remove_head(vk_libusb_hcd_urb_t, urb_node,
                        &__vk_libusb_hcd.urb_list, libusb_urb);
#if VSF_LIBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
                vk_usbh_hcd_urb_t *urb = container_of(libusb_urb, vk_usbh_hcd_urb_t, priv);
                __vk_libusb_hcd_trace_urb(urb, "dequeued-");
#endif
            vsf_unprotect_sched(orig);

            if (libusb_urb != NULL) {
                vk_usbh_hcd_urb_t *urb = container_of(libusb_urb, vk_usbh_hcd_urb_t, priv);

                if (VSF_LIBUSB_HCD_URB_STATE_TO_FREE == libusb_urb->state) {
                    __vk_libusb_hcd_free_urb_do(urb);
                } else {
                    vk_usbh_hcd_dev_t *dev = urb->dev_hcd;
                    vk_libusb_hcd_dev_t *libusb_dev = dev->dev_priv;

                    if (USB_ENDPOINT_XFER_CONTROL == urb->pipe.type) {
                        struct usb_ctrlrequest_t *setup = &urb->setup_packet;

                        // set address is handled here
                        if (    ((USB_RECIP_DEVICE | USB_DIR_OUT) == setup->bRequestType)
                            &&  (USB_REQ_SET_ADDRESS == setup->bRequest)) {

                            VSF_USB_ASSERT(0 == libusb_dev->addr);
                            libusb_dev->addr = setup->wValue;
                            urb->status = URB_OK;
                            urb->actual_length = 0;

                            VSF_USB_ASSERT(vsf_dlist_is_empty(&libusb_dev->urb_pending_list));
                            vsf_dlist_add_to_tail(vk_libusb_hcd_urb_t, urb_pending_node, &libusb_dev->urb_pending_list, libusb_urb);
                            vsf_eda_post_msg(&__vk_libusb_hcd.teda.use_as__vsf_eda_t, urb);
#if VSF_LIBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
                            __vk_libusb_hcd_trace_urb(urb, "done");
#endif
                            goto wait_next_urb;
                        }
                    }

                    if (!urb->pipe.dir_in1out0) {
                        vk_libusb_hcd_urb_t *libusb_urb_head;

                        // add to urb_pending_list for out transfer
                        vsf_dlist_add_to_tail(vk_libusb_hcd_urb_t, urb_pending_node, &libusb_dev->urb_pending_list, libusb_urb);

                        vsf_dlist_peek_head(vk_libusb_hcd_urb_t, urb_pending_node, &libusb_dev->urb_pending_list, libusb_urb_head);
                        if (libusb_urb_head != libusb_urb) {
                            goto wait_next_urb;
                        }
                    }

                    // irq_thread will process the urb
                    libusb_urb->state = VSF_LIBUSB_HCD_URB_STATE_SUBMITTING;
#if VSF_LIBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
                    __vk_libusb_hcd_trace_urb(urb, "send submit request");
#endif
                    __vsf_arch_irq_request_send(&libusb_urb->irq_request);
                }
            }
            goto wait_next_urb;
        }
        break;
    case VSF_EVT_TIMER:
        if (__vk_libusb_hcd.new_mask != 0) {
            vk_usbh_t *usbh = (vk_usbh_t *)libusb->hcd;
            if (NULL == usbh->dev_new) {
                int idx = ffz(~__vk_libusb_hcd.new_mask);
                VSF_USB_ASSERT(idx < dimof(__vk_libusb_hcd.devs));
                vk_libusb_hcd_dev_t *libusb_dev = &__vk_libusb_hcd.devs[idx];
                VSF_USB_ASSERT(vsf_dlist_is_empty(&libusb_dev->urb_pending_list));
                __vk_libusb_hcd.cur_dev_idx = idx;
                __vk_libusb_hcd.new_mask &= ~(1 << idx);
                libusb_dev->addr = 0;
                libusb_dev->dev = vk_usbh_new_device((vk_usbh_t *)libusb->hcd, libusb_dev->speed, NULL, 0);
                libusb_dev->state = VSF_LIBUSB_HCD_DEV_STATE_ATTACHED;
            }
        }
        vsf_teda_set_timer_ms(100);
        break;
    case VSF_EVT_MESSAGE: {
            vk_usbh_hcd_urb_t *urb = vsf_eda_get_cur_msg();
            VSF_USB_ASSERT((urb != NULL) && urb->pipe.is_pipe);
            vk_libusb_hcd_urb_t *libusb_urb = (vk_libusb_hcd_urb_t *)urb->priv;

#if VSF_LIBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
            __vk_libusb_hcd_trace_urb(urb, "get msg in hcd task");
#endif

            do {
                if (!urb->pipe.dir_in1out0) {
                    vk_usbh_hcd_dev_t *dev = urb->dev_hcd;
                    if (NULL == dev) { break; }
                    vk_libusb_hcd_dev_t *libusb_dev = dev->dev_priv;
                    if (NULL == libusb_dev) { break; }
                    vk_libusb_hcd_urb_t *libusb_urb_head;

                    if (VSF_LIBUSB_HCD_DEV_STATE_ATTACHED == libusb_dev->state) {
                        vsf_dlist_remove_head(vk_libusb_hcd_urb_t, urb_pending_node, &libusb_dev->urb_pending_list, libusb_urb_head);
                        VSF_USB_ASSERT(libusb_urb_head == libusb_urb);
                        vsf_dlist_peek_head(vk_libusb_hcd_urb_t, urb_pending_node, &libusb_dev->urb_pending_list, libusb_urb_head);
                        if (libusb_urb_head != NULL) {
                            libusb_urb_head->state = VSF_LIBUSB_HCD_URB_STATE_SUBMITTING;
#if VSF_LIBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
                            __vk_libusb_hcd_trace_urb(urb, "send submit request");
#endif
                            __vsf_arch_irq_request_send(&libusb_urb_head->irq_request);
                        }
                    }
                }
            } while (0);

            if (VSF_LIBUSB_HCD_URB_STATE_TO_FREE == libusb_urb->state) {
                if (!__vk_libusb_hcd_free_urb_do(urb)) {
                    libusb_urb->is_msg_processed = true;
                }
            } else {
#if VSF_LIBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
                __vk_libusb_hcd_trace_urb(urb, "notify");
#endif

                vsf_eda_post_msg(urb->eda_caller, urb);
                libusb_urb->state = VSF_LIBUSB_HCD_URB_STATE_IDLE;
                libusb_urb->is_msg_processed = true;
            }
        }
        break;
    default: {
            int idx = evt & 0xFF;
            VSF_USB_ASSERT(idx < dimof(__vk_libusb_hcd.devs));
            vk_libusb_hcd_dev_t *libusb_dev = &__vk_libusb_hcd.devs[idx];

            switch (evt & ~0xFF) {
            case VSF_EVT_LIBUSB_HCD_ATTACH:
                if (libusb_dev->state != VSF_LIBUSB_HCD_DEV_STATE_ATTACHED) {
                    __vk_libusb_hcd.new_mask |= 1 << idx;
                }
                break;
            case VSF_EVT_LIBUSB_HCD_DETACH:
                if (libusb_dev->state != VSF_LIBUSB_HCD_DEV_STATE_DETACHED) {
                    libusb_dev->state = VSF_LIBUSB_HCD_DEV_STATE_DETACHED;
                    vk_usbh_disconnect_device((vk_usbh_t *)libusb->hcd, libusb_dev->dev);
                    vsf_dlist_init(&libusb_dev->urb_pending_list);
                    libusb_dev->evt_mask.is_detached = true;
                    libusb_dev->evt_mask.is_detaching = false;
                    __vsf_arch_irq_request_send(&libusb_dev->irq_request);
                } else {
                    libusb_dev->evt_mask.is_detaching = false;
                }
                break;
            }
        }
    }
}

static vsf_err_t __vk_libusb_hcd_init_evthandler(vsf_eda_t *eda, vsf_evt_t evt, vk_usbh_hcd_t *hcd)
{
    vk_libusb_hcd_param_t *param = hcd->param;

    switch (evt) {
    case VSF_EVT_INIT:
        __vk_libusb_hcd.hcd = hcd;
        __vk_libusb_hcd.new_mask = 0;
        __vk_libusb_hcd.cur_dev_idx = 0;

        __vk_libusb_hcd.teda.fn.evthandler = __vk_libusb_hcd_evthandler;
        vsf_teda_init(&__vk_libusb_hcd.teda, vsf_prio_inherit, false);

        __vk_libusb_hcd.init_eda = eda;
#if VSF_LIBUSB_HCD_CFG_TRACE_IRQ_EN == ENABLED
        __vk_libusb_hcd_trace_hcd_irq("init");
#endif
        __vsf_arch_irq_init(&__vk_libusb_hcd.init_thread, "libusb_hcd_init", __vk_libusb_hcd_init_thread, param->priority);
        break;
    case VSF_EVT_LIBUSB_HCD_READY:
        return VSF_ERR_NONE;
    }
    return VSF_ERR_NOT_READY;
}

static vsf_err_t __vk_libusb_hcd_fini(vk_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_libusb_hcd_suspend(vk_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_libusb_hcd_resume(vk_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static uint_fast16_t __vk_libusb_hcd_get_frame_number(vk_usbh_hcd_t *hcd)
{
    return 0;
}

static vsf_err_t __vk_libusb_hcd_alloc_device(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{
    VSF_USB_ASSERT(__vk_libusb_hcd.cur_dev_idx < dimof(__vk_libusb_hcd.devs));
    dev->dev_priv = &__vk_libusb_hcd.devs[__vk_libusb_hcd.cur_dev_idx];
    return VSF_ERR_NONE;
}

static void __vk_libusb_hcd_free_device(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{
    dev->dev_priv = NULL;
}

static vk_usbh_hcd_urb_t * __vk_libusb_hcd_alloc_urb(vk_usbh_hcd_t *hcd)
{
    uint_fast32_t size = sizeof(vk_usbh_hcd_urb_t) + sizeof(vk_libusb_hcd_urb_t);
    vk_usbh_hcd_urb_t *urb = vsf_usbh_malloc(size);

    if (urb != NULL) {
        memset(urb, 0, size);

        vk_libusb_hcd_urb_t *libusb_urb = (vk_libusb_hcd_urb_t *)urb->priv;
        vk_libusb_hcd_param_t *param = __vk_libusb_hcd.hcd->param;
        __vsf_arch_irq_request_init(&libusb_urb->irq_request);
#if VSF_LIBUSB_HCD_CFG_TRACE_IRQ_EN == ENABLED
        __vk_libusb_hcd_trace_urb_irq(urb, "init");
#endif
        libusb_urb->is_msg_processed = true;
        libusb_urb->is_irq_enabled = true;
        __vsf_arch_irq_init(&libusb_urb->irq_thread, "libusb_hcd_urb", __vk_libusb_hcd_urb_thread, param->priority);
    }
    return urb;
}

static void __vk_libusb_hcd_free_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    vk_libusb_hcd_urb_t *libusb_urb = (vk_libusb_hcd_urb_t *)urb->priv;
    if (VSF_LIBUSB_HCD_URB_STATE_TO_FREE != libusb_urb->state) {
#if VSF_LIBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
        __vk_libusb_hcd_trace_urb(urb, "to free");
#endif
        vsf_protect_t orig = vsf_protect_int();
            libusb_urb->state = VSF_LIBUSB_HCD_URB_STATE_TO_FREE;
        if (libusb_urb->is_irq_enabled) {
            vsf_unprotect_int(orig);
            __vsf_arch_irq_request_send(&libusb_urb->irq_request);
            return;
        }
        vsf_unprotect_int(orig);
    }
}

static vsf_err_t __vk_libusb_hcd_submit_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    vk_libusb_hcd_urb_t *libusb_urb = (vk_libusb_hcd_urb_t *)urb->priv;
    vsf_dlist_init_node(vk_libusb_hcd_urb_t, urb_node, libusb_urb);
    vsf_dlist_init_node(vk_libusb_hcd_urb_t, urb_pending_node, libusb_urb);
    vsf_protect_t orig = vsf_protect_sched();
        vsf_dlist_add_to_tail(vk_libusb_hcd_urb_t, urb_node, &__vk_libusb_hcd.urb_list, libusb_urb);
        libusb_urb->state = VSF_LIBUSB_HCD_URB_STATE_QUEUED;
#if VSF_LIBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
        __vk_libusb_hcd_trace_urb(urb, "enqueued+");
#endif
    vsf_unprotect_sched(orig);
    vsf_eda_sem_post(&__vk_libusb_hcd.sem);
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_libusb_hcd_relink_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    return __vk_libusb_hcd_submit_urb(hcd, urb);
}

static vsf_err_t __vk_libusb_hcd_reset_dev(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{
    vk_libusb_hcd_dev_t *libusb_dev = (vk_libusb_hcd_dev_t *)dev->dev_priv;
    libusb_dev->evt_mask.is_resetting = true;
    __vsf_arch_irq_request_send(&libusb_dev->irq_request);
    return VSF_ERR_NONE;
}

static bool __vk_libusb_hcd_is_dev_reset(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{
    vk_libusb_hcd_dev_t *libusb_dev = (vk_libusb_hcd_dev_t *)dev->dev_priv;
    return libusb_dev->evt_mask.is_resetting;
}

#endif
