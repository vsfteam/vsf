/*****************************************************************************
 *   Copyright(C)2009-2026 by VSF Team                                       *
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

/*
 * Port implementation for "usb/usb_device.h" on VSF.
 *
 * Bridges the ESP-IDF ROM USB Device API onto VSF's USB Device framework
 * (component/usb/device/). The board layer provides a pre-configured
 * vk_usbd_dev_t with the correct DCD driver already set; the espidf shim
 * wraps the ROM API (usb_set_config / usb_enable / usb_write / usb_read etc.)
 * on top.
 *
 * Architecture:
 *   The caller-supplied vk_usbd_dev_t is expected to be compiled with
 *   VSF_USBD_CFG_AUTOSETUP (default on) so that standard Chapter 9 requests
 *   are handled by the VSF core. The bridge injects a custom class
 *   operation vtable (__vsf_espidf_usbd_bridge_op) that intercepts
 *   class/vendor requests and delegates them to user-registered handlers.
 *
 *   Endpoint transfers are bridged through vk_usbd_ep_send / vk_usbd_ep_recv
 *   with FreeRTOS binary semaphore for synchronous completion.
 */

/*============================ INCLUDES ======================================*/

#include "../vsf_espidf_cfg.h"

#if VSF_USE_ESPIDF == ENABLED && VSF_ESPIDF_CFG_USE_USB_DEVICE == ENABLED

/* Full IMPLEMENT access: needed to read ctrl_handler.request / trans fields
 * from the bridge class operation callbacks which receive a raw
 * vk_usbd_dev_t pointer. The CLASS-only scope would hide these fields. */
#define __VSF_USBD_CLASS_IMPLEMENT

#include "usb/usb_device.h"
#include "component/usb/device/vsf_usbd.h"

#include "../vsf_espidf.h"
#include "service/trace/vsf_trace.h"

#include <string.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/*============================ MACROS ========================================*/

#define VSF_ESPIDF_USBD_TRACE(msg, ...)                                         \
    vsf_trace_info("espidf_usbd: " msg VSF_TRACE_CFG_LINEEND, ##__VA_ARGS__)

#define VSF_ESPIDF_USBD_TRACE_ERR(msg, ...)                                     \
    vsf_trace_error("espidf_usbd: " msg VSF_TRACE_CFG_LINEEND, ##__VA_ARGS__)

/*============================ TYPES =========================================*/

typedef struct __vsf_espidf_usbd {
    bool                is_inited;
    bool                is_enabled;
    bool                is_configured;
    uint8_t             configuration;
    vk_usbd_dev_t      *vsf_usbd;

    /* user callbacks from usb_set_config() */
    usb_status_callback         status_cb;
    usb_interface_config        interface_cfg_cb;
    struct usb_interface_cfg_data user_iface;
    uint8_t                     num_endpoints;
    struct usb_ep_cfg_data     *endpoints;

    SemaphoreHandle_t   state_lock;
} __vsf_espidf_usbd_t;

/*============================ LOCAL VARIABLES ===============================*/

static __vsf_espidf_usbd_t __vsf_espidf_usbd;

/*============================ FORWARD DECLARATIONS ==========================*/

static void __usbd_notify_status(enum usb_dc_status_code code, uint8_t *param);

/*============================ HELPER: VSF CLASS OP IMPLEMENTATION ===========*/

static vk_usbd_desc_t * __bridge_get_desc(vk_usbd_dev_t *dev,
                                          uint_fast8_t type,
                                          uint_fast8_t index,
                                          uint_fast16_t langid)
{
    /*
     * Walk the device descriptor table and return the matching entry.
     * The board-supplied vk_usbd_dev_t carries its own descriptors;
     * we simply forward to the standard lookup.
     */
    __vsf_espidf_usbd_t *b = &__vsf_espidf_usbd;
    if (!b->vsf_usbd) return NULL;
    return vk_usbd_get_descriptor(b->vsf_usbd->desc,
                                  b->vsf_usbd->num_of_desc,
                                  type, index, langid);
}

static vsf_err_t __bridge_request_prepare(vk_usbd_dev_t *dev,
                                          vk_usbd_ifs_t *ifs)
{
    __vsf_espidf_usbd_t *b = &__vsf_espidf_usbd;
    (void)ifs;

    struct usb_ctrlrequest_t *req = &dev->ctrl_handler.request;
    struct usb_setup_packet setup;

    setup.bmRequestType = req->bmRequestType;
    setup.bRequest      = req->bRequest;
    setup.wValue        = req->wValue;
    setup.wIndex        = req->wIndex;
    setup.wLength       = req->wLength;

    /* custom handler gets first chance */
    if (b->user_iface.custom_handler) {
        int32_t transfer_len = (int32_t)setup.wLength;
        uint8_t *payload = b->user_iface.payload_data;
        int rc = b->user_iface.custom_handler(&setup, &transfer_len, &payload);
        if (rc == 0) {
            if (req->bmRequestType & USB_DIR_IN) {
                dev->ctrl_handler.trans.buffer = payload;
                dev->ctrl_handler.trans.size   = (uint32_t)transfer_len;
            }
            return VSF_ERR_NONE;
        }
    }

    /* class handler */
    if (b->user_iface.class_handler) {
        int32_t transfer_len = (int32_t)setup.wLength;
        uint8_t *payload = b->user_iface.payload_data;
        int rc = b->user_iface.class_handler(&setup, &transfer_len, &payload);
        if (rc == 0) {
            if (req->bmRequestType & USB_DIR_IN) {
                dev->ctrl_handler.trans.buffer = payload;
                dev->ctrl_handler.trans.size   = (uint32_t)transfer_len;
            }
            return VSF_ERR_NONE;
        }
    }

    /* vendor handler */
    if (b->user_iface.vendor_handler) {
        int32_t transfer_len = (int32_t)setup.wLength;
        uint8_t *payload = b->user_iface.vendor_data;
        int rc = b->user_iface.vendor_handler(&setup, &transfer_len, &payload);
        if (rc == 0) {
            if (req->bmRequestType & USB_DIR_IN) {
                dev->ctrl_handler.trans.buffer = payload;
                dev->ctrl_handler.trans.size   = (uint32_t)transfer_len;
            }
            return VSF_ERR_NONE;
        }
    }

    return VSF_ERR_NOT_READY;
}

static vsf_err_t __bridge_request_process(vk_usbd_dev_t *dev,
                                          vk_usbd_ifs_t *ifs)
{
    (void)dev;
    (void)ifs;
    /*
     * The data stage has already been handled by the core.
     * For OUT transfers, the payload is in dev->ctrl_handler.trans.buffer.
     * For IN transfers, the response was sent from the handler's buffer.
     * No additional processing needed.
     */
    return VSF_ERR_NONE;
}

static vsf_err_t __bridge_init(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    __vsf_espidf_usbd_t *b = &__vsf_espidf_usbd;
    (void)ifs;

    b->is_configured = true;
    __usbd_notify_status(USB_DC_CONFIGURED, NULL);

    if (b->interface_cfg_cb) {
        b->interface_cfg_cb(0);  /* single-interface bridge */
    }

    return VSF_ERR_NONE;
}

static vsf_err_t __bridge_fini(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    __vsf_espidf_usbd_t *b = &__vsf_espidf_usbd;
    (void)dev; (void)ifs;

    b->is_configured = false;
    return VSF_ERR_NONE;
}

static const vk_usbd_class_op_t __vsf_espidf_usbd_bridge_op = {
    .use_as__vk_usbd_class_op_t.get_desc          = __bridge_get_desc,
    .use_as__vk_usbd_class_op_t.request_prepare   = __bridge_request_prepare,
    .use_as__vk_usbd_class_op_t.request_process   = __bridge_request_process,
    .use_as__vk_usbd_class_op_t.init              = __bridge_init,
    .use_as__vk_usbd_class_op_t.fini              = __bridge_fini,
};

/*============================ INTERNAL HELPERS ==============================*/

static void __usbd_ensure_lock(void)
{
    if (!__vsf_espidf_usbd.state_lock) {
        __vsf_espidf_usbd.state_lock = xSemaphoreCreateMutex();
    }
}

static void __usbd_notify_status(enum usb_dc_status_code code, uint8_t *param)
{
    __vsf_espidf_usbd_t *b = &__vsf_espidf_usbd;
    if (b->status_cb) {
        b->status_cb(code, param);
    }
}

static struct usb_ep_cfg_data * __usbd_find_ep(uint8_t ep_addr)
{
    __vsf_espidf_usbd_t *b = &__vsf_espidf_usbd;
    for (uint8_t i = 0; i < b->num_endpoints; i++) {
        if (b->endpoints[i].ep_addr == ep_addr)
            return &b->endpoints[i];
    }
    return NULL;
}

/*
 * Transfer completion callback -- signals the waiting semaphore.
 */
static void __usbd_trans_finish(void *param)
{
    /* Called from EDA task context (not ISR), safe to use plain Give. */
    SemaphoreHandle_t sem = (SemaphoreHandle_t)param;
    xSemaphoreGive(sem);
}

/*
 * Generic synchronous EP I/O: allocate a trans, submit, wait for completion.
 */
static int __usbd_ep_io(uint8_t ep, uint8_t *data, uint32_t len,
                        bool is_tx, uint32_t *bytes_ret)
{
    __vsf_espidf_usbd_t *b = &__vsf_espidf_usbd;

    if (!b->is_enabled || !b->vsf_usbd)
        return -1;

    vk_usbd_trans_t *trans = malloc(sizeof(*trans));
    if (!trans)
        return -1;
    memset(trans, 0, sizeof(*trans));

    trans->ep      = ep;
    trans->buffer  = data;
    trans->size    = len;
    trans->zlp     = false;
    trans->notify_eda = false;
    trans->on_finish = __usbd_trans_finish;

    SemaphoreHandle_t sem = xSemaphoreCreateBinary();
    if (!sem) {
        free(trans);
        return -1;
    }
    trans->param = sem;

    vsf_err_t err;
    if (is_tx) {
        err = vk_usbd_ep_send(b->vsf_usbd, trans);
    } else {
        err = vk_usbd_ep_recv(b->vsf_usbd, trans);
    }

    if (err != VSF_ERR_NONE) {
        vSemaphoreDelete(sem);
        free(trans);
        return -1;
    }

    if (xSemaphoreTake(sem, pdMS_TO_TICKS(5000)) != pdTRUE) {
        VSF_ESPIDF_USBD_TRACE_ERR("transfer timeout ep=0x%02x", ep);
        vSemaphoreDelete(sem);
        return -1;
    }

    if (bytes_ret)
        *bytes_ret = len;

    vSemaphoreDelete(sem);
    free(trans);
    return 0;
}

/*============================ PUBLIC API ====================================*/

const vk_usbd_class_op_t * vsf_espidf_usbd_get_bridge_op(void)
{
    return &__vsf_espidf_usbd_bridge_op;
}

int usb_set_config(struct usb_cfg_data *config)
{
    __vsf_espidf_usbd_t *b = &__vsf_espidf_usbd;
    int rc = 0;

    __usbd_ensure_lock();
    xSemaphoreTake(b->state_lock, portMAX_DELAY);

    if (!config) {
        rc = -1;
        goto out;
    }

    b->status_cb        = config->cb_usb_status;
    b->interface_cfg_cb = config->interface_config;
    b->user_iface       = config->interface;
    b->num_endpoints    = config->num_endpoints;
    b->endpoints        = config->endpoint;

    VSF_ESPIDF_USBD_TRACE("set_config: %u endpoints, callback=%p",
                          b->num_endpoints, (void *)b->status_cb);

out:
    xSemaphoreGive(b->state_lock);
    return rc;
}

int usb_deconfig(void)
{
    __vsf_espidf_usbd_t *b = &__vsf_espidf_usbd;

    __usbd_ensure_lock();
    xSemaphoreTake(b->state_lock, portMAX_DELAY);

    b->status_cb        = NULL;
    b->interface_cfg_cb = NULL;
    memset(&b->user_iface, 0, sizeof(b->user_iface));
    b->num_endpoints    = 0;
    b->endpoints        = NULL;
    b->is_configured    = false;
    b->configuration    = 0;

    VSF_ESPIDF_USBD_TRACE("deconfig");

    xSemaphoreGive(b->state_lock);
    return 0;
}

int usb_enable(struct usb_cfg_data *config)
{
    __vsf_espidf_usbd_t *b = &__vsf_espidf_usbd;

    __usbd_ensure_lock();
    xSemaphoreTake(b->state_lock, portMAX_DELAY);

    if (b->is_enabled) {
        /* already enabled -- re-enable is a no-op */
        xSemaphoreGive(b->state_lock);
        return 0;
    }

    if (config) {
        /* Accept descriptors for compatibility; the board's vk_usbd_dev_t
         * holds the authoritative descriptors. */
        b->status_cb        = config->cb_usb_status;
        b->interface_cfg_cb = config->interface_config;
        b->user_iface       = config->interface;
        b->num_endpoints    = config->num_endpoints;
        b->endpoints        = config->endpoint;
    }

    b->vsf_usbd = vsf_espidf_get_usbd();
    if (!b->vsf_usbd) {
        VSF_ESPIDF_USBD_TRACE_ERR("enable: no vk_usbd_dev_t from board");
        xSemaphoreGive(b->state_lock);
        return -1;
    }

    vk_usbd_init(b->vsf_usbd);
    vk_usbd_connect(b->vsf_usbd);

    b->is_enabled = true;

    VSF_ESPIDF_USBD_TRACE("enabled");

    xSemaphoreGive(b->state_lock);

    __usbd_notify_status(USB_DC_CONNECTED, NULL);

    return 0;
}

int usb_disable(void)
{
    __vsf_espidf_usbd_t *b = &__vsf_espidf_usbd;

    __usbd_ensure_lock();
    xSemaphoreTake(b->state_lock, portMAX_DELAY);

    if (!b->is_enabled) {
        xSemaphoreGive(b->state_lock);
        return 0;
    }

    if (b->vsf_usbd) {
        vk_usbd_disconnect(b->vsf_usbd);
        vk_usbd_fini(b->vsf_usbd);
    }

    b->is_enabled    = false;
    b->is_configured = false;

    VSF_ESPIDF_USBD_TRACE("disabled");

    xSemaphoreGive(b->state_lock);

    __usbd_notify_status(USB_DC_DISCONNECTED, NULL);

    return 0;
}

int usb_write_would_block(uint8_t ep)
{
    (void)ep;
    /* In VSF's device stack, writes are queued and do not block
     * on FIFO fullness (the DCD manages flow control). */
    return 0;
}

int usb_write(uint8_t ep, const uint8_t *data, uint32_t data_len,
              uint32_t *bytes_ret)
{
    VSF_ESPIDF_USBD_TRACE("write ep=0x%02x len=%u", ep, (unsigned)data_len);
    return __usbd_ep_io(ep, (uint8_t *)data, data_len, true, bytes_ret);
}

int usb_read(uint8_t ep, uint8_t *data, uint32_t max_data_len,
             uint32_t *ret_bytes)
{
    VSF_ESPIDF_USBD_TRACE("read ep=0x%02x len=%u", ep, (unsigned)max_data_len);
    return __usbd_ep_io(ep, data, max_data_len, false, ret_bytes);
}

int usb_ep_set_stall(uint8_t ep)
{
    __vsf_espidf_usbd_t *b = &__vsf_espidf_usbd;
    if (!b->is_enabled || !b->vsf_usbd) return -1;

    vsf_err_t err = vk_usbd_ep_stall(b->vsf_usbd, ep);
    VSF_ESPIDF_USBD_TRACE("stall ep=0x%02x -> %d", ep, err);
    return (err == VSF_ERR_NONE) ? 0 : -1;
}

int usb_ep_clear_stall(uint8_t ep)
{
    __vsf_espidf_usbd_t *b = &__vsf_espidf_usbd;
    if (!b->is_enabled || !b->vsf_usbd) return -1;

    /* VSF provides stall via vk_usbd_ep_stall, but clear is at DCD level.
     * The vk_usbd_dev_t has a drv member (i_usb_dc_t) that exposes
     * Ep.ClearStall. We access it via the device's driver. */
#ifdef __VSF_USBD_CFG_DRV_INTERFACE
    if (b->vsf_usbd->drv) {
        b->vsf_usbd->drv->Ep.ClearStall(ep);
    }
#else
    /* OO-style: the DCD functions are statically bound.
     * The stall clear is normally handled automatically after
     * a SETUP/CLEAR_FEATURE(HALT) via the Chapter 9 handler.
     * For explicit user clear, we rely on the endpoint being
     * re-enabled. This is a known limitation for the OO path. */
    (void)ep;
#endif
    VSF_ESPIDF_USBD_TRACE("clear_stall ep=0x%02x", ep);
    return 0;
}

int usb_ep_read_wait(uint8_t ep, uint8_t *data, uint32_t max_data_len,
                     uint32_t *read_bytes)
{
    /* Same as usb_read in our single-packet model */
    return usb_read(ep, data, max_data_len, read_bytes);
}

int usb_ep_read_continue(uint8_t ep)
{
    /* In VSF, re-enabling the OUT endpoint is managed internally.
     * The core calls EnableOut after each read completes.
     * usb_ep_read_continue is a no-op here. */
    (void)ep;
    return 0;
}

void usb_transfer_ep_callback(uint8_t ep,
                              enum usb_dc_ep_cb_status_code cb_status)
{
    __vsf_espidf_usbd_t *b = &__vsf_espidf_usbd;
    struct usb_ep_cfg_data *ep_cfg = __usbd_find_ep(ep);
    if (ep_cfg && ep_cfg->ep_cb) {
        ep_cfg->ep_cb(ep, cb_status);
    }
    (void)b;
}

typedef struct __vsf_espidf_usbd_async_trans {
    vk_usbd_trans_t         trans;          /* must be first, freed together */
    usb_transfer_callback   user_cb;
    void                   *user_priv;
    SemaphoreHandle_t       done_sem;
    int                     result;
} __vsf_espidf_usbd_async_trans_t;

static void __usbd_async_trans_finish(void *param)
{
    __vsf_espidf_usbd_async_trans_t *at =
        (__vsf_espidf_usbd_async_trans_t *)param;
    if (at->user_cb) {
        at->user_cb(0, at->result, at->user_priv);
    }
    if (at->done_sem) {
        xSemaphoreGive(at->done_sem);
    }
    free(at);
}

int usb_transfer(uint8_t ep, uint8_t *data, size_t dlen, unsigned int flags,
                 usb_transfer_callback cb, void *priv)
{
    __vsf_espidf_usbd_t *b = &__vsf_espidf_usbd;
    if (!b->is_enabled || !b->vsf_usbd) return -1;

    bool is_tx = (flags & USB_TRANS_WRITE) ? true : false;
    bool no_zlp = (flags & USB_TRANS_NO_ZLP) ? true : false;

    __vsf_espidf_usbd_async_trans_t *at =
        malloc(sizeof(*at));
    if (!at) return -1;
    memset(at, 0, sizeof(*at));
    at->user_cb   = cb;
    at->user_priv = priv;
    at->result    = (int)dlen;

    vk_usbd_trans_t *trans = &at->trans;
    trans->ep     = ep;
    trans->buffer = data;
    trans->size   = (uint32_t)dlen;
    trans->zlp    = !no_zlp;
    trans->notify_eda = false;
    trans->on_finish = __usbd_async_trans_finish;
    trans->param = at;

    vsf_err_t err;
    if (is_tx) {
        err = vk_usbd_ep_send(b->vsf_usbd, trans);
    } else {
        err = vk_usbd_ep_recv(b->vsf_usbd, trans);
    }

    if (err != VSF_ERR_NONE) {
        free(at);
        return -1;
    }

    return 0;
}

int usb_transfer_sync(uint8_t ep, uint8_t *data, size_t dlen,
                      unsigned int flags)
{
    __vsf_espidf_usbd_t *b = &__vsf_espidf_usbd;
    if (!b->is_enabled || !b->vsf_usbd) return -1;

    bool is_tx = (flags & USB_TRANS_WRITE) ? true : false;
    bool no_zlp = (flags & USB_TRANS_NO_ZLP) ? true : false;

    __vsf_espidf_usbd_async_trans_t *at = malloc(sizeof(*at));
    if (!at) return -1;
    memset(at, 0, sizeof(*at));
    at->result   = (int)dlen;
    at->done_sem = xSemaphoreCreateBinary();
    if (!at->done_sem) { free(at); return -1; }

    vk_usbd_trans_t *trans = &at->trans;
    trans->ep     = ep;
    trans->buffer = data;
    trans->size   = (uint32_t)dlen;
    trans->zlp    = !no_zlp;
    trans->notify_eda = false;
    trans->on_finish = __usbd_async_trans_finish;
    trans->param = at;

    vsf_err_t err;
    if (is_tx) {
        err = vk_usbd_ep_send(b->vsf_usbd, trans);
    } else {
        err = vk_usbd_ep_recv(b->vsf_usbd, trans);
    }

    if (err != VSF_ERR_NONE) {
        vSemaphoreDelete(at->done_sem);
        free(at);
        return -1;
    }

    if (xSemaphoreTake(at->done_sem, pdMS_TO_TICKS(5000)) != pdTRUE) {
        VSF_ESPIDF_USBD_TRACE_ERR("transfer_sync timeout ep=0x%02x", ep);
        vSemaphoreDelete(at->done_sem);
        free(at);
        return -1;
    }

    int result = at->result;
    vSemaphoreDelete(at->done_sem);
    free(at);
    return result;
}

void usb_cancel_transfer(uint8_t ep)
{
    /* Cancellation is not directly supported in the VSF device stack.
     * The user can simply stop processing the completion callback. */
    (void)ep;
    VSF_ESPIDF_USBD_TRACE("cancel_transfer ep=0x%02x (no-op)", ep);
}

void usb_dev_deinit(void)
{
    __vsf_espidf_usbd_t *b = &__vsf_espidf_usbd;
    usb_disable();
    usb_deconfig();
    b->is_inited = false;
}

void usb_dev_resume(int configuration)
{
    __vsf_espidf_usbd_t *b = &__vsf_espidf_usbd;
    b->configuration = (uint8_t)configuration;
}

int usb_dev_get_configuration(void)
{
    return (int)__vsf_espidf_usbd.configuration;
}

#endif      /* VSF_USE_ESPIDF && VSF_ESPIDF_CFG_USE_USB_DEVICE */
