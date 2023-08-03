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

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_HCD_WEBUSB == ENABLED

#define __VSF_USBH_CLASS_IMPLEMENT_HCD__
#define __VSF_USBH_CLASS_IMPLEMENT_HUB__
#define __VSF_EDA_CLASS_INHERIT__

#define __OOC_CPP__

#include "kernel/vsf_kernel.h"
#include "component/usb/host/vsf_usbh.h"
#include "./vsf_webusb_hcd.h"

#include <emscripten.h>
#include <emscripten/val.h>

extern "C" {

/*============================ MACROS ========================================*/

// TODO: remove to user configuration
//#define VSF_WEBUSB_HCD_CFG_TRACE_URB_EN             ENABLED
//#define VSF_WEBUSB_HCD_CFG_TRACE_IRQ_EN             ENABLED

#define VSF_EVT_WEBUSB_HCD_BASE                     ((VSF_EVT_USER + 0x100) & ~0xFF)

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_WEBUSB_HCD_DEF_DEV(__N, __BIT)                                      \
            {                                                                   \
                .vid = VSF_WEBUSB_HCD_DEV##__N##_VID,                           \
                .pid = VSF_WEBUSB_HCD_DEV##__N##_PID,                           \
                .addr = -1,                                                     \
            },

/*============================ TYPES =========================================*/

enum webusb_error {
    WEBUSB_SUCCESS                  = 0,
    WEBUSB_ERROR_IO                 = -1,
    WEBUSB_ERROR_INVALID_PARAM      = -2,
    WEBUSB_ERROR_ACCESS             = -3,
    WEBUSB_ERROR_NO_DEVICE          = -4,
    WEBUSB_ERROR_NOT_FOUND          = -5,
    WEBUSB_ERROR_BUSY               = -6,
    WEBUSB_ERROR_TIMEOUT            = -7,
    WEBUSB_ERROR_OVERFLOW           = -8,
    WEBUSB_ERROR_PIPE               = -9,
    WEBUSB_ERROR_INTERRUPTED        = -10,
    WEBUSB_ERROR_NO_MEM             = -11,
    WEBUSB_ERROR_NOT_SUPPORTED      = -12,
    WEBUSB_ERROR_OTHER              = -99,
};

typedef enum vk_webusb_hcd_dev_state_t {
    VSF_WEBUSB_HCD_DEV_STATE_DETACHED,
    VSF_WEBUSB_HCD_DEV_STATE_DETACHING,
    VSF_WEBUSB_HCD_DEV_STATE_ATTACHED,
} vk_webusb_hcd_dev_state_t;

typedef struct vk_webusb_hcd_dev_t {
    uint16_t vid, pid;
    vk_usbh_dev_t *dev;
    emscripten::val handle;

    enum usb_device_speed_t speed;
    vk_webusb_hcd_dev_state_t state;

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
} vk_webusb_hcd_dev_t;

typedef struct vk_webusb_hcd_t {
    vk_webusb_hcd_dev_t *devs;
    int dev_num;

    vk_usbh_hcd_t *hcd;
    uint32_t new_mask;
    uint8_t cur_dev_idx;

    vsf_eda_t *init_eda;
    vsf_arch_irq_thread_t init_thread;
    vsf_teda_t teda;
    vsf_sem_t sem;
    vsf_dlist_t urb_list;
} vk_webusb_hcd_t;

typedef enum vk_webusb_hcd_state_t {
    VSF_WEBUSB_HCD_URB_STATE_IDLE,
    VSF_WEBUSB_HCD_URB_STATE_QUEUED,
    VSF_WEBUSB_HCD_URB_STATE_SUBMITTING,
    VSF_WEBUSB_HCD_URB_STATE_WAIT_TO_FREE,
    VSF_WEBUSB_HCD_URB_STATE_TO_FREE,
} vk_webusb_hcd_state_t;

typedef struct vk_webusb_hcd_urb_t {
    vsf_dlist_node_t urb_node;
    vsf_dlist_node_t urb_pending_node;

    vk_webusb_hcd_state_t state;

    bool is_irq_enabled;
    bool is_msg_processed;

    vsf_arch_irq_thread_t irq_thread;
    vsf_arch_irq_request_t irq_request;
} vk_webusb_hcd_urb_t;

typedef enum vk_webusb_hcd_evt_t {
    VSF_EVT_WEBUSB_HCD_ATTACH           = VSF_EVT_WEBUSB_HCD_BASE + 0x100,
    VSF_EVT_WEBUSB_HCD_DETACH           = VSF_EVT_WEBUSB_HCD_BASE + 0x200,
    VSF_EVT_WEBUSB_HCD_READY            = VSF_EVT_WEBUSB_HCD_BASE + 0x300,
} vk_webusb_hcd_evt_t;

/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vk_webusb_hcd_init_evthandler(vsf_eda_t *eda, vsf_evt_t evt, vk_usbh_hcd_t *hcd);
static vsf_err_t __vk_webusb_hcd_fini(vk_usbh_hcd_t *hcd);
static vsf_err_t __vk_webusb_hcd_suspend(vk_usbh_hcd_t *hcd);
static vsf_err_t __vk_webusb_hcd_resume(vk_usbh_hcd_t *hcd);
static uint_fast16_t __vk_webusb_hcd_get_frame_number(vk_usbh_hcd_t *hcd);
static vsf_err_t __vk_webusb_hcd_alloc_device(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);
static void __vk_webusb_hcd_free_device(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);
static vk_usbh_hcd_urb_t * __vk_webusb_hcd_alloc_urb(vk_usbh_hcd_t *hcd);
static void __vk_webusb_hcd_free_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
static vsf_err_t __vk_webusb_hcd_submit_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
static vsf_err_t __vk_webusb_hcd_relink_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
static vsf_err_t __vk_webusb_hcd_reset_dev(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);
static bool __vk_webusb_hcd_is_dev_reset(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);

static void __vk_webusb_hcd_dev_thread(void *arg);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbh_hcd_drv_t vk_webusb_hcd_drv = {
    .init_evthandler    = __vk_webusb_hcd_init_evthandler,
    .fini               = __vk_webusb_hcd_fini,
    .suspend            = __vk_webusb_hcd_suspend,
    .resume             = __vk_webusb_hcd_resume,
    .get_frame_number   = __vk_webusb_hcd_get_frame_number,
    .alloc_device       = __vk_webusb_hcd_alloc_device,
    .free_device        = __vk_webusb_hcd_free_device,
    .alloc_urb          = __vk_webusb_hcd_alloc_urb,
    .free_urb           = __vk_webusb_hcd_free_urb,
    .submit_urb         = __vk_webusb_hcd_submit_urb,
    .relink_urb         = __vk_webusb_hcd_relink_urb,
    .reset_dev          = __vk_webusb_hcd_reset_dev,
    .is_dev_reset       = __vk_webusb_hcd_is_dev_reset,
};

/*============================ LOCAL VARIABLES ===============================*/

static vk_webusb_hcd_t __vk_webusb_hcd = { 0 };

/*============================ IMPLEMENTATION ================================*/

#if VSF_WEBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
static void __vk_webusb_hcd_trace_urb(vk_usbh_hcd_urb_t *urb, char *msg)
{
    vk_usbh_pipe_t pipe = urb->pipe;
    vsf_trace_info("webusb_urb EP%d_%s(%08X): %s\r\n", pipe.use_as__vk_usbh_pipe_flag_t.endpoint,
        pipe.use_as__vk_usbh_pipe_flag_t.dir_in1out0 ? "IN" : "OUT", urb, msg);
}
#endif

#if VSF_WEBUSB_HCD_CFG_TRACE_IRQ_EN == ENABLED
static void __vk_webusb_hcd_trace_dev_irq(vk_webusb_hcd_dev_t *webusb_dev, char *msg)
{
    vsf_trace_info("webusb_dev_irq(%08X): %s\r\n", &webusb_dev->irq_thread, msg);
}

static void __vk_webusb_hcd_trace_hcd_irq(char *msg)
{
    vsf_trace_info("webusb_hcd_irq(%08X): %s\r\n", &__vk_webusb_hcd.init_thread, msg);
}

static void __vk_webusb_hcd_trace_urb_irq(vk_usbh_hcd_urb_t *urb, char *msg)
{
    vk_usbh_pipe_t pipe = urb->pipe;
    vsf_trace_info("webusb_urb EP%d_%s(%08X): irq %s\r\n", pipe.use_as__vk_usbh_pipe_flag_t.endpoint,
        pipe.use_as__vk_usbh_pipe_flag_t.dir_in1out0 ? "IN" : "OUT", urb, msg);
}
#endif

static void __vk_webusb_hcd_on_left(vk_webusb_hcd_dev_t *webusb_dev)
{
    webusb_dev->evt_mask.is_detaching = true;
    __vsf_arch_irq_request_send(&webusb_dev->irq_request);
}

static void __vk_webusb_hcd_on_arrived(vk_webusb_hcd_dev_t *webusb_dev)
{
    webusb_dev->speed = USB_SPEED_UNKNOWN;
    webusb_dev->evt_mask.is_attaching = true;
    __vsf_arch_irq_request_send(&webusb_dev->irq_request);
}

// return 0 on success, < 0 otherwise
static int __vk_webusb_hcd_init(void)
{
    emscripten::val navigator = emscripten::val::global("navigator");
    if (!navigator["usb"].as<bool>()) {
        vsf_trace_error("webusb is not supported" VSF_TRACE_CFG_LINEEND);
        return -1;
    }

    return 0;
}

static int __vk_webusb_set_configuration(vk_webusb_hcd_dev_t *webusb_dev, int configuration)
{
    webusb_dev->handle.call<emscripten::val>("selectConfiguration", configuration).await();
    return 0;
}

static int __vk_webusb_set_interface_alt_setting(vk_webusb_hcd_dev_t *webusb_dev, int interface_number, int alternate_setting)
{
    webusb_dev->handle.call<emscripten::val>("selectAlternateInterface", interface_number, alternate_setting).await();
    return 0;
}

static int __vk_webusb_control_transfer(vk_webusb_hcd_dev_t *webusb_dev, uint8_t bmRequestType, uint8_t bRequest,
    uint16_t wValue, uint16_t wIndex, unsigned char *data, uint16_t wLength, unsigned int timeout)
{
    return 0;
}

static int __vk_webusb_bulk_transfer(vk_webusb_hcd_dev_t *webusb_dev, unsigned char endpoint, unsigned char *data,
    int length, int *transferred, unsigned int timeout)
{
    return 0;
}

static int __vk_webusb_interrupt_transfer(vk_webusb_hcd_dev_t *webusb_dev, unsigned char endpoint, unsigned char *data,
    int length, int *transferred, unsigned int timeout)
{
    return 0;
}

static int __vk_webusb_open(vk_webusb_hcd_dev_t *webusb_dev)
{
    webusb_dev->handle.call<emscripten::val>("open").await();
    return 0;
}

static void __vk_webusb_close(vk_webusb_hcd_dev_t *webusb_dev)
{
    webusb_dev->handle.call<emscripten::val>("close").await();
//    webusb_dev->handle = NULL;
}

static int __vk_webusb_reset_device(vk_webusb_hcd_dev_t *webusb_dev)
{
    webusb_dev->handle.call<emscripten::val>("reset").await();
    return 0;
}

static int __vk_webusb_claim_interface(vk_webusb_hcd_dev_t *webusb_dev, int interface_number)
{
    webusb_dev->handle.call<emscripten::val>("claimInterface", interface_number).await();
    return 0;
}

// TODO: call webusb_claim_interface for non-control transfer
static int __vk_webusb_hcd_submit_urb_do(vk_usbh_hcd_urb_t *urb)
{
    vk_usbh_hcd_dev_t *dev = urb->dev_hcd;
    vk_webusb_hcd_dev_t *webusb_dev = (vk_webusb_hcd_dev_t *)dev->dev_priv;
    vk_usbh_pipe_t pipe = urb->pipe;

    switch (pipe.use_as__vk_usbh_pipe_flag_t.type) {
    case USB_ENDPOINT_XFER_CONTROL:
        if (pipe.use_as__vk_usbh_pipe_flag_t.endpoint != 0) {
            return WEBUSB_ERROR_INVALID_PARAM;
        } else {
            usb_ctrlrequest_t *setup = &urb->setup_packet;

            if (    ((USB_RECIP_DEVICE | USB_DIR_OUT) == setup->bRequestType)
                &&  (USB_REQ_SET_CONFIGURATION == setup->bRequest)) {
                return __vk_webusb_set_configuration(webusb_dev, setup->wValue);
            } else if ( ((USB_RECIP_INTERFACE | USB_DIR_OUT) == setup->bRequestType)
                    &&  (USB_REQ_SET_INTERFACE == setup->bRequest)) {
                return __vk_webusb_set_interface_alt_setting(webusb_dev, setup->wIndex, setup->wValue);
            } else {
                return __vk_webusb_control_transfer(webusb_dev, setup->bRequestType,
                        setup->bRequest, setup->wValue, setup->wIndex, (unsigned char *)urb->buffer,
                        setup->wLength, urb->timeout);
            }
        }
    case USB_ENDPOINT_XFER_ISOC:
        // TODO: add support to iso transfer
        return WEBUSB_ERROR_NOT_SUPPORTED;
    case USB_ENDPOINT_XFER_BULK: {
            int actual_length;
            int err = __vk_webusb_bulk_transfer(webusb_dev,
                        (pipe.use_as__vk_usbh_pipe_flag_t.dir_in1out0 ? 0x80 : 0x00) | pipe.use_as__vk_usbh_pipe_flag_t.endpoint,
                        (unsigned char *)urb->buffer, urb->transfer_length, &actual_length, urb->timeout);
            if (err < 0) {
                return err;
            } else {
                return actual_length;
            }
        }
    case USB_ENDPOINT_XFER_INT: {
            int actual_length;
            int err = __vk_webusb_interrupt_transfer(webusb_dev,
                        (pipe.use_as__vk_usbh_pipe_flag_t.dir_in1out0 ? 0x80 : 0x00) | pipe.use_as__vk_usbh_pipe_flag_t.endpoint,
                        (unsigned char *)urb->buffer, urb->transfer_length, &actual_length, urb->timeout);
            if (err < 0) {
                return err;
            } else {
                return actual_length;
            }
        }
        break;
    }
    return WEBUSB_ERROR_INVALID_PARAM;
}

static void __vk_webusb_hcd_dev_thread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = (vsf_arch_irq_thread_t *)arg;
    vk_webusb_hcd_dev_t *webusb_dev = container_of(irq_thread, vk_webusb_hcd_dev_t, irq_thread);
    vsf_arch_irq_request_t *irq_request = &webusb_dev->irq_request;
    int idx = webusb_dev - __vk_webusb_hcd.devs;

    __vsf_arch_irq_set_background(irq_thread);
    while (1) {
        __vsf_arch_irq_request_pend(irq_request);

        if (webusb_dev->evt_mask.is_attaching) {
            webusb_dev->evt_mask.is_attaching = false;
            __vsf_arch_irq_start(irq_thread);
                vsf_eda_post_evt(&__vk_webusb_hcd.teda.use_as__vsf_eda_t, VSF_EVT_WEBUSB_HCD_ATTACH | idx);
            __vsf_arch_irq_end(irq_thread, false);
        }
        if (webusb_dev->evt_mask.is_detaching) {
            __vsf_arch_irq_start(irq_thread);
                vsf_eda_post_evt(&__vk_webusb_hcd.teda.use_as__vsf_eda_t, VSF_EVT_WEBUSB_HCD_DETACH | idx);
            __vsf_arch_irq_end(irq_thread, false);
        }
        if (webusb_dev->evt_mask.is_detached) {
            __vk_webusb_close(webusb_dev);
            webusb_dev->evt_mask.is_detached = false;
        }
        if (webusb_dev->evt_mask.is_resetting) {
            __vk_webusb_reset_device(webusb_dev);
            webusb_dev->evt_mask.is_resetting = false;
        }
    }
}

static void __vk_webusb_hcd_urb_thread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = (vsf_arch_irq_thread_t *)arg;
    vk_webusb_hcd_urb_t *webusb_urb = container_of(irq_thread, vk_webusb_hcd_urb_t, irq_thread);
    vk_usbh_hcd_urb_t *urb = container_of(webusb_urb, vk_usbh_hcd_urb_t, priv);
    vsf_arch_irq_request_t *irq_request = &webusb_urb->irq_request;
    bool is_to_free;
    int actual_length;

    __vsf_arch_irq_set_background(irq_thread);
    while (1) {
        __vsf_arch_irq_request_pend(irq_request);

        while (!webusb_urb->is_msg_processed) {
            __vsf_arch_irq_sleep(1);
        }

        is_to_free = VSF_WEBUSB_HCD_URB_STATE_TO_FREE == webusb_urb->state;
        if (!is_to_free) {
            vk_usbh_hcd_dev_t *dev = urb->dev_hcd;
            vk_webusb_hcd_dev_t *webusb_dev = (vk_webusb_hcd_dev_t *)dev->dev_priv;

#if VSF_WEBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
            __vsf_arch_irq_start(irq_thread);
                __vk_webusb_hcd_trace_urb(urb, "submitting");
            __vsf_arch_irq_end(irq_thread, false);
#endif
            actual_length = __vk_webusb_hcd_submit_urb_do(urb);
#if VSF_WEBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
            __vsf_arch_irq_start(irq_thread);
                __vk_webusb_hcd_trace_urb(urb, "done");
            __vsf_arch_irq_end(irq_thread, false);
#endif
            if (actual_length < 0) {
                urb->status = actual_length;
            } else {
                urb->status = URB_OK;
                urb->actual_length = actual_length;

                if (USB_ENDPOINT_XFER_CONTROL == urb->pipe.use_as__vk_usbh_pipe_flag_t.type) {
                    usb_ctrlrequest_t *setup = &urb->setup_packet;

                    // set configuration is handled here
                    if (    ((USB_RECIP_DEVICE | USB_DIR_OUT) == setup->bRequestType)
                        &&  (USB_REQ_SET_CONFIGURATION == setup->bRequest)) {

                        usb_config_desc_t *config_desc = (usb_config_desc_t *)urb->buffer;
                        for (uint8_t i = 0; i < config_desc->bNumInterfaces; i++) {
                            __vk_webusb_claim_interface(webusb_dev, i);
                        }
                    }
                }
            }

#if VSF_WEBUSB_HCD_CFG_REMOVE_ON_ERROR == ENABLED
            if (urb->status == WEBUSB_ERROR_IO) {
                // device removed
#   if VSF_WEBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
                __vsf_arch_irq_start(irq_thread);
                    __vk_webusb_hcd_trace_urb(urb, "device removed, to free");
                __vsf_arch_irq_end(irq_thread, false);
#   endif
                if (webusb_urb->state != VSF_WEBUSB_HCD_URB_STATE_TO_FREE) {
                    webusb_urb->state = VSF_WEBUSB_HCD_URB_STATE_WAIT_TO_FREE;
                }
                __vk_webusb_hcd_on_left(webusb_dev);
            }
#endif
        }

        __vsf_arch_irq_start(irq_thread);
#if VSF_WEBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
            __vk_webusb_hcd_trace_urb(urb, "post msg in irq");
#endif
            if (is_to_free) {
                webusb_urb->is_irq_enabled = false;
#if VSF_WEBUSB_HCD_CFG_TRACE_IRQ_EN == ENABLED
                __vk_webusb_hcd_trace_urb_irq(urb, "fini");
#endif
            }
            webusb_urb->is_msg_processed = false;
            vsf_eda_post_msg(&__vk_webusb_hcd.teda.use_as__vsf_eda_t, urb);
        __vsf_arch_irq_end(irq_thread, false);

        if (is_to_free) {
            __vsf_arch_irq_fini(irq_thread);
            __vsf_arch_irq_request_fini(irq_request);
            return;
        }
    }
}

static void __vk_webusb_hcd_init_thread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = (vsf_arch_irq_thread_t *)arg;

    __vsf_arch_irq_set_background(irq_thread);
        if (__vk_webusb_hcd_init() < 0) {
            VSF_USB_ASSERT(false);
        }
    __vsf_arch_irq_start(irq_thread);
        vsf_eda_post_evt(__vk_webusb_hcd.init_eda, VSF_EVT_WEBUSB_HCD_READY);
    __vsf_arch_irq_end(irq_thread, false);

    emscripten::val usb = emscripten::val::global("navigator")["usb"];
    emscripten::val filter = emscripten::val::object();
    filter.set("filters", emscripten::val::array());
    usb.call<emscripten::val>("requestDevice", filter).await();

    emscripten::val devices = emscripten::val::global("navigator")["usb"].call<emscripten::val>("getDevices").await();
    __vk_webusb_hcd.dev_num = devices["length"].as<int>();
    if (__vk_webusb_hcd.dev_num > 0) {
        __vk_webusb_hcd.devs = (vk_webusb_hcd_dev_t *)vsf_usbh_malloc(sizeof(vk_webusb_hcd_dev_t) * __vk_webusb_hcd.dev_num);
        VSF_USB_ASSERT(__vk_webusb_hcd.devs != NULL);

        for (int i = 0; i < __vk_webusb_hcd.dev_num; i++) {
            vk_webusb_hcd_dev_t *webusb_dev = &__vk_webusb_hcd.devs[i];

            webusb_dev->handle = devices[i];
            __vk_webusb_open(webusb_dev);

            __vk_webusb_hcd_on_arrived(webusb_dev);
            // TODO: wait idle for next dev
        }
    }

#if VSF_WEBUSB_HCD_CFG_TRACE_IRQ_EN == ENABLED
    __vsf_arch_irq_start(irq_thread);
        __vk_webusb_hcd_trace_hcd_irq("fini");
    __vsf_arch_irq_end(irq_thread, false);
#endif
    __vsf_arch_irq_fini(irq_thread);
}






#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

static bool __vk_webusb_hcd_free_urb_do(vk_usbh_hcd_urb_t *urb)
{
    // LLCM "-Wcast-align"
    vk_webusb_hcd_urb_t *webusb_urb = (vk_webusb_hcd_urb_t *)urb->priv;
    if (webusb_urb->is_irq_enabled) {
#if VSF_WEBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
        __vk_webusb_hcd_trace_urb(urb, "irq running, try to exit first");
#endif
        VSF_USB_ASSERT(VSF_WEBUSB_HCD_URB_STATE_TO_FREE == webusb_urb->state);
        __vsf_arch_irq_request_send(&webusb_urb->irq_request);
        return false;
    } else {
#if VSF_WEBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
        __vk_webusb_hcd_trace_urb(urb, "freed");
#endif
        if (urb->transfer_flags & __URB_NEED_FREE) {
            vk_usbh_hcd_urb_free_buffer(urb);
            vsf_usbh_free(urb);
        } else {
            urb->status = URB_CANCELED;
            vk_usbh_hcd_urb_complete(urb);
        }
        return true;
    }
}

static void __vk_webusb_hcd_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_webusb_hcd_t *webusb = container_of(eda, vk_webusb_hcd_t, teda);

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_dlist_init(&__vk_webusb_hcd.urb_list);
        vsf_eda_sem_init(&__vk_webusb_hcd.sem, 0);
        vsf_teda_set_timer_ms(100);

    wait_next_urb:
        if (vsf_eda_sem_pend(&__vk_webusb_hcd.sem, -1)) {
            break;
        }
        // fall through
    case VSF_EVT_SYNC: {
            vk_webusb_hcd_urb_t *webusb_urb;
            vsf_protect_t orig = vsf_protect_sched();
                vsf_dlist_remove_head(vk_webusb_hcd_urb_t, urb_node,
                        &__vk_webusb_hcd.urb_list, webusb_urb);
#if VSF_WEBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
                vk_usbh_hcd_urb_t *urb = container_of(webusb_urb, vk_usbh_hcd_urb_t, priv);
                __vk_webusb_hcd_trace_urb(urb, "dequeued-");
#endif
            vsf_unprotect_sched(orig);

            if (webusb_urb != NULL) {
                vk_usbh_hcd_urb_t *urb = container_of(webusb_urb, vk_usbh_hcd_urb_t, priv);

                if (VSF_WEBUSB_HCD_URB_STATE_TO_FREE == webusb_urb->state) {
                    __vk_webusb_hcd_free_urb_do(urb);
                } else {
                    vk_usbh_hcd_dev_t *dev = urb->dev_hcd;
                    vk_webusb_hcd_dev_t *webusb_dev = (vk_webusb_hcd_dev_t *)dev->dev_priv;

                    if (USB_ENDPOINT_XFER_CONTROL == urb->pipe.use_as__vk_usbh_pipe_flag_t.type) {
                        usb_ctrlrequest_t *setup = &urb->setup_packet;

                        // set address is handled here
                        if (    ((USB_RECIP_DEVICE | USB_DIR_OUT) == setup->bRequestType)
                            &&  (USB_REQ_SET_ADDRESS == setup->bRequest)) {

                            VSF_USB_ASSERT(0 == webusb_dev->addr);
                            webusb_dev->addr = setup->wValue;
                            urb->status = URB_OK;
                            urb->actual_length = 0;

                            VSF_USB_ASSERT(vsf_dlist_is_empty(&webusb_dev->urb_pending_list));
                            vsf_dlist_add_to_tail(vk_webusb_hcd_urb_t, urb_pending_node, &webusb_dev->urb_pending_list, webusb_urb);
                            vsf_eda_post_msg(&__vk_webusb_hcd.teda.use_as__vsf_eda_t, urb);
#if VSF_WEBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
                            __vk_webusb_hcd_trace_urb(urb, "done");
#endif
                            goto wait_next_urb;
                        }
                    }

                    if (!urb->pipe.use_as__vk_usbh_pipe_flag_t.dir_in1out0) {
                        vk_webusb_hcd_urb_t *webusb_urb_head;

                        // add to urb_pending_list for out transfer
                        vsf_dlist_add_to_tail(vk_webusb_hcd_urb_t, urb_pending_node, &webusb_dev->urb_pending_list, webusb_urb);

                        vsf_dlist_peek_head(vk_webusb_hcd_urb_t, urb_pending_node, &webusb_dev->urb_pending_list, webusb_urb_head);
                        if (webusb_urb_head != webusb_urb) {
                            goto wait_next_urb;
                        }
                    }

                    // irq_thread will process the urb
                    webusb_urb->state = VSF_WEBUSB_HCD_URB_STATE_SUBMITTING;
#if VSF_WEBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
                    __vk_webusb_hcd_trace_urb(urb, "send submit request");
#endif
                    __vsf_arch_irq_request_send(&webusb_urb->irq_request);
                }
            }
            goto wait_next_urb;
        }
        break;
    case VSF_EVT_TIMER:
        if (__vk_webusb_hcd.new_mask != 0) {
            vk_usbh_t *usbh = (vk_usbh_t *)webusb->hcd;
            if (NULL == usbh->dev_new) {
                int idx = vsf_ffz32(~__vk_webusb_hcd.new_mask);
                VSF_USB_ASSERT(idx < __vk_webusb_hcd.dev_num);
                vk_webusb_hcd_dev_t *webusb_dev = &__vk_webusb_hcd.devs[idx];
                VSF_USB_ASSERT(vsf_dlist_is_empty(&webusb_dev->urb_pending_list));
                __vk_webusb_hcd.cur_dev_idx = idx;
                __vk_webusb_hcd.new_mask &= ~(1 << idx);
                webusb_dev->addr = 0;
                webusb_dev->dev = vk_usbh_new_device((vk_usbh_t *)webusb->hcd, webusb_dev->speed, NULL, 0);
                webusb_dev->state = VSF_WEBUSB_HCD_DEV_STATE_ATTACHED;
            }
        }
        vsf_teda_set_timer_ms(100);
        break;
    case VSF_EVT_MESSAGE: {
            vk_usbh_hcd_urb_t *urb = (vk_usbh_hcd_urb_t *)vsf_eda_get_cur_msg();
            VSF_USB_ASSERT((urb != NULL) && urb->pipe.use_as__vk_usbh_pipe_flag_t.is_pipe);
            // LLCM "-Wcast-align"
            vk_webusb_hcd_urb_t *webusb_urb = (vk_webusb_hcd_urb_t *)urb->priv;

#if VSF_WEBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
            __vk_webusb_hcd_trace_urb(urb, "get msg in hcd task");
#endif

            do {
                if (!urb->pipe.use_as__vk_usbh_pipe_flag_t.dir_in1out0) {
                    vk_usbh_hcd_dev_t *dev = urb->dev_hcd;
                    if (NULL == dev) { break; }
                    vk_webusb_hcd_dev_t *webusb_dev = (vk_webusb_hcd_dev_t *)dev->dev_priv;
                    if (NULL == webusb_dev) { break; }
                    vk_webusb_hcd_urb_t *webusb_urb_head;

                    if (VSF_WEBUSB_HCD_DEV_STATE_ATTACHED == webusb_dev->state) {
                        vsf_dlist_remove_head(vk_webusb_hcd_urb_t, urb_pending_node, &webusb_dev->urb_pending_list, webusb_urb_head);
                        VSF_USB_ASSERT(webusb_urb_head == webusb_urb);
                        vsf_dlist_peek_head(vk_webusb_hcd_urb_t, urb_pending_node, &webusb_dev->urb_pending_list, webusb_urb_head);
                        if (webusb_urb_head != NULL) {
                            webusb_urb_head->state = VSF_WEBUSB_HCD_URB_STATE_SUBMITTING;
#if VSF_WEBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
                            __vk_webusb_hcd_trace_urb(urb, "send submit request");
#endif
                            __vsf_arch_irq_request_send(&webusb_urb_head->irq_request);
                        }
                    }
                }
            } while (0);

            if (VSF_WEBUSB_HCD_URB_STATE_TO_FREE == webusb_urb->state) {
                if (!__vk_webusb_hcd_free_urb_do(urb)) {
                    webusb_urb->is_msg_processed = true;
                }
            } else {
#if VSF_WEBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
                __vk_webusb_hcd_trace_urb(urb, "notify");
#endif

                vk_usbh_hcd_urb_complete(urb);
                webusb_urb->state = VSF_WEBUSB_HCD_URB_STATE_IDLE;
                webusb_urb->is_msg_processed = true;
            }
        }
        break;
    default: {
            int idx = evt & 0xFF;
            VSF_USB_ASSERT(idx < __vk_webusb_hcd.dev_num);
            vk_webusb_hcd_dev_t *webusb_dev = &__vk_webusb_hcd.devs[idx];

            switch (evt & ~0xFF) {
            case VSF_EVT_WEBUSB_HCD_ATTACH:
                if (webusb_dev->state != VSF_WEBUSB_HCD_DEV_STATE_ATTACHED) {
                    __vk_webusb_hcd.new_mask |= 1 << idx;
                }
                break;
            case VSF_EVT_WEBUSB_HCD_DETACH:
                if (webusb_dev->state != VSF_WEBUSB_HCD_DEV_STATE_DETACHED) {
                    webusb_dev->state = VSF_WEBUSB_HCD_DEV_STATE_DETACHED;
                    vk_usbh_disconnect_device((vk_usbh_t *)webusb->hcd, webusb_dev->dev);
                    vsf_dlist_init(&webusb_dev->urb_pending_list);
                    webusb_dev->evt_mask.is_detached = true;
                    webusb_dev->evt_mask.is_detaching = false;
                    __vsf_arch_irq_request_send(&webusb_dev->irq_request);
                } else {
                    webusb_dev->evt_mask.is_detaching = false;
                }
                break;
            }
        }
    }
}

static vsf_err_t __vk_webusb_hcd_init_evthandler(vsf_eda_t *eda, vsf_evt_t evt, vk_usbh_hcd_t *hcd)
{
    vk_webusb_hcd_param_t *param = (vk_webusb_hcd_param_t *)hcd->param;

    switch (evt) {
    case VSF_EVT_INIT:
        __vk_webusb_hcd.hcd = hcd;
        __vk_webusb_hcd.new_mask = 0;
        __vk_webusb_hcd.cur_dev_idx = 0;

        __vk_webusb_hcd.teda.use_as__vsf_eda_t.fn.evthandler = __vk_webusb_hcd_evthandler;
        vsf_teda_init(&__vk_webusb_hcd.teda);

        __vk_webusb_hcd.init_eda = eda;
#if VSF_WEBUSB_HCD_CFG_TRACE_IRQ_EN == ENABLED
        __vk_webusb_hcd_trace_hcd_irq("init");
#endif
        __vsf_arch_irq_init(&__vk_webusb_hcd.init_thread, (char *)"webusb_hcd_init", __vk_webusb_hcd_init_thread, param->priority);
        break;
    case VSF_EVT_WEBUSB_HCD_READY:
        return VSF_ERR_NONE;
    }
    return VSF_ERR_NOT_READY;
}

static vsf_err_t __vk_webusb_hcd_fini(vk_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_webusb_hcd_suspend(vk_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_webusb_hcd_resume(vk_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static uint_fast16_t __vk_webusb_hcd_get_frame_number(vk_usbh_hcd_t *hcd)
{
    return 0;
}

static vsf_err_t __vk_webusb_hcd_alloc_device(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{
    VSF_USB_ASSERT(__vk_webusb_hcd.cur_dev_idx < __vk_webusb_hcd.dev_num);
    dev->dev_priv = &__vk_webusb_hcd.devs[__vk_webusb_hcd.cur_dev_idx];
    return VSF_ERR_NONE;
}

static void __vk_webusb_hcd_free_device(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{
    dev->dev_priv = NULL;
}

static vk_usbh_hcd_urb_t * __vk_webusb_hcd_alloc_urb(vk_usbh_hcd_t *hcd)
{
    uint_fast32_t size = sizeof(vk_usbh_hcd_urb_t) + sizeof(vk_webusb_hcd_urb_t);
    vk_usbh_hcd_urb_t *urb = (vk_usbh_hcd_urb_t *)vsf_usbh_malloc(size);

    if (urb != NULL) {
        memset(urb, 0, size);

        // LLCM "-Wcast-align"
        vk_webusb_hcd_urb_t *webusb_urb = (vk_webusb_hcd_urb_t *)urb->priv;
        vk_webusb_hcd_param_t *param = (vk_webusb_hcd_param_t *)__vk_webusb_hcd.hcd->param;
        __vsf_arch_irq_request_init(&webusb_urb->irq_request);
#if VSF_WEBUSB_HCD_CFG_TRACE_IRQ_EN == ENABLED
        __vk_webusb_hcd_trace_urb_irq(urb, "init");
#endif
        webusb_urb->is_msg_processed = true;
        webusb_urb->is_irq_enabled = true;
        __vsf_arch_irq_init(&webusb_urb->irq_thread, (char *)"webusb_hcd_urb", __vk_webusb_hcd_urb_thread, param->priority);
    }
    return urb;
}

static void __vk_webusb_hcd_free_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    // LLCM "-Wcast-align"
    vk_webusb_hcd_urb_t *webusb_urb = (vk_webusb_hcd_urb_t *)urb->priv;
    if (VSF_WEBUSB_HCD_URB_STATE_TO_FREE != webusb_urb->state) {
#if VSF_WEBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
        __vk_webusb_hcd_trace_urb(urb, "to free");
#endif
        vsf_protect_t orig = vsf_protect_int();
            webusb_urb->state = VSF_WEBUSB_HCD_URB_STATE_TO_FREE;
        if (webusb_urb->is_irq_enabled) {
            vsf_unprotect_int(orig);
            __vsf_arch_irq_request_send(&webusb_urb->irq_request);
            return;
        }
        vsf_unprotect_int(orig);
    }
}

static vsf_err_t __vk_webusb_hcd_submit_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    // LLCM "-Wcast-align"
    vk_webusb_hcd_urb_t *webusb_urb = (vk_webusb_hcd_urb_t *)urb->priv;
    vsf_dlist_init_node(vk_webusb_hcd_urb_t, urb_node, webusb_urb);
    vsf_dlist_init_node(vk_webusb_hcd_urb_t, urb_pending_node, webusb_urb);
    vsf_protect_t orig = vsf_protect_sched();
        vsf_dlist_add_to_tail(vk_webusb_hcd_urb_t, urb_node, &__vk_webusb_hcd.urb_list, webusb_urb);
        webusb_urb->state = VSF_WEBUSB_HCD_URB_STATE_QUEUED;
#if VSF_WEBUSB_HCD_CFG_TRACE_URB_EN == ENABLED
        __vk_webusb_hcd_trace_urb(urb, "enqueued+");
#endif
    vsf_unprotect_sched(orig);
    vsf_eda_sem_post(&__vk_webusb_hcd.sem);
    return VSF_ERR_NONE;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

static vsf_err_t __vk_webusb_hcd_relink_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    return __vk_webusb_hcd_submit_urb(hcd, urb);
}

static vsf_err_t __vk_webusb_hcd_reset_dev(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{
    vk_webusb_hcd_dev_t *webusb_dev = (vk_webusb_hcd_dev_t *)dev->dev_priv;
    webusb_dev->evt_mask.is_resetting = true;
    __vsf_arch_irq_request_send(&webusb_dev->irq_request);
    return VSF_ERR_NONE;
}

static bool __vk_webusb_hcd_is_dev_reset(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{
    vk_webusb_hcd_dev_t *webusb_dev = (vk_webusb_hcd_dev_t *)dev->dev_priv;
    return webusb_dev->evt_mask.is_resetting;
}

// end of extern "C"
}

#endif
