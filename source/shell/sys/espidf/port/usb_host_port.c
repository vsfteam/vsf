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
 * Port implementation for "usb/usb_host.h" on VSF.
 *
 * Bridges the ESP-IDF USB Host Library API onto VSF's USB Host framework
 * (component/usb/host/). The board layer provides a pre-configured
 * vk_usbh_t with the correct HCD driver already set; the espidf shim
 * adds the ESP-IDF bridge class driver and manages the client/device
 * model on top.
 *
 * Device and client tables are dynamically allocated (raw linked lists)
 * -- no compile-time count limits, matching ESP-IDF behaviour.
 */

/*============================ INCLUDES ======================================*/

#include "../vsf_espidf_cfg.h"

#if VSF_USE_ESPIDF == ENABLED && VSF_ESPIDF_CFG_USE_USB_HOST == ENABLED

/* Full IMPLEMENT access: needed to read HCD URB fields (status, actual_length,
 * buffer) from the transfer completion callback which receives a raw
 * vk_usbh_hcd_urb_t pointer.  The CLASS-only scope would hide these fields. */
#define __VSF_USBH_CLASS_IMPLEMENT

#include "usb/usb_host.h"
#include "component/usb/host/vsf_usbh.h"

#include "../vsf_espidf.h"
#include "service/trace/vsf_trace.h"
#include "kernel/vsf_kernel.h"

#include <string.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "event_groups.h"
#include "semphr.h"
#include "task.h"

/*============================ MACROS ========================================*/

#define VSF_ESPIDF_USBH_TRACE(msg, ...)                                         \
    vsf_trace_info("espidf_usb: " msg VSF_TRACE_CFG_LINEEND, ##__VA_ARGS__)

#define VSF_ESPIDF_CFG_USB_HOST_EVT_QUEUE_DEPTH    16

/*============================ TYPES =========================================*/

/* Linked-list node for device / client management (raw pointer, no OOC). */
typedef struct __vsf_espidf_usbh_dev_node {
    struct __vsf_espidf_usbh_dev_node *next;
    bool                        is_connected;
    bool                        is_opened;
    uint8_t                     dev_addr;
    vk_usbh_dev_t              *vsf_dev;
    usb_device_desc_t          *dev_desc;
    usb_config_desc_t          *cfg_desc;
    uint32_t                    cfg_desc_size;
    usb_host_client_handle_t    owning_client;
} __vsf_espidf_usbh_dev_node_t;

typedef struct __vsf_espidf_usbh_client_node {
    struct __vsf_espidf_usbh_client_node *next;
    usb_host_client_handle_t    client_hdl;
    usb_host_client_config_t    config;
    usb_host_client_event_cb_t  event_cb;
    void                       *callback_arg;
    QueueHandle_t               event_queue;
} __vsf_espidf_usbh_client_node_t;

/* Bridge internal singleton */
typedef struct __vsf_espidf_usbh {
    bool                        is_installed;
    vk_usbh_t                  *vsf_usbh;
    vk_usbh_class_t             bridge_class;
    vk_usbh_class_drv_t         bridge_drv;

    __vsf_espidf_usbh_dev_node_t   *device_head;
    __vsf_espidf_usbh_client_node_t *client_head;
    SemaphoreHandle_t           state_lock;

    EventGroupHandle_t          lib_events;
    int                         num_clients;
    int                         num_devices;
} __vsf_espidf_usbh_t;

/* Wrapped transfer */
typedef struct {
    usb_transfer_t              esp_transfer;
    vk_usbh_urb_t               vsf_urb;
    bool                        is_submitted;
} __vsf_espidf_usbh_transfer_t;

/*============================ LOCAL VARIABLES ===============================*/

static __vsf_espidf_usbh_t __vsf_espidf_usbh;

/*============================ FORWARD DECLARATIONS ==========================*/

static void * __vsf_espidf_usbh_bridge_probe(vk_usbh_t *usbh,
        vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs);
static void __vsf_espidf_usbh_bridge_disconnect(vk_usbh_t *usbh,
        vk_usbh_dev_t *dev, void *param);

/*============================ LINKED-LIST HELPERS ===========================*/

static void __push_event_to_all_clients(usb_host_client_event_t event,
                                        uint8_t dev_addr,
                                        usb_device_handle_t dev_hdl)
{
    __vsf_espidf_usbh_t *b = &__vsf_espidf_usbh;
    for (__vsf_espidf_usbh_client_node_t *c = b->client_head; c; c = c->next) {
        usb_host_client_event_msg_t *msg = vsf_usbh_malloc(sizeof(*msg));
        if (!msg) continue;
        msg->event = event;
        if (event == USB_HOST_CLIENT_EVENT_NEW_DEV)
            msg->new_dev.address = dev_addr;
        else
            msg->dev_gone.dev_hdl = dev_hdl;
        xQueueSend(c->event_queue, &msg, (TickType_t)0);
    }
}

static __vsf_espidf_usbh_dev_node_t *
__find_dev_by_handle(usb_device_handle_t hdl)
{
    for (__vsf_espidf_usbh_dev_node_t *d = __vsf_espidf_usbh.device_head;
         d; d = d->next) {
        if ((usb_device_handle_t)d == hdl) return d;
    }
    return NULL;
}

static __vsf_espidf_usbh_dev_node_t *
__find_dev_by_addr(uint8_t addr)
{
    for (__vsf_espidf_usbh_dev_node_t *d = __vsf_espidf_usbh.device_head;
         d; d = d->next) {
        if (d->is_connected && d->dev_addr == addr) return d;
    }
    return NULL;
}

/*============================ BRIDGE CLASS DRIVER ===========================*/

static const vk_usbh_dev_id_t __match_ids[] = {
    { .match_flags = 0 },   /* catch-all */
};

static void * __vsf_espidf_usbh_bridge_probe(vk_usbh_t *usbh,
        vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs)
{
    __vsf_espidf_usbh_t *b = &__vsf_espidf_usbh;
    vk_usbh_dev_parser_t *parser = usbh->parser;
    usb_interface_desc_t *desc_ifs =
        parser_ifs->parser_alt[parser_ifs->ifs->cur_alt].desc_ifs;

    if (desc_ifs->bInterfaceClass == USB_CLASS_HUB)
        return NULL;    /* defer to VSF built-in HUB driver */

    xSemaphoreTake(b->state_lock, portMAX_DELAY);

    /* Already tracked? */
    for (__vsf_espidf_usbh_dev_node_t *d = b->device_head; d; d = d->next) {
        if (d->vsf_dev == dev) {
            xSemaphoreGive(b->state_lock);
            return d;
        }
    }

    __vsf_espidf_usbh_dev_node_t *d = vsf_usbh_malloc(sizeof(*d));
    if (!d) { xSemaphoreGive(b->state_lock); return NULL; }
    memset(d, 0, sizeof(*d));
    d->is_connected = true;
    d->dev_addr = dev->devnum;
    d->vsf_dev = dev;

    if (parser->desc_device) {
        d->dev_desc = vsf_usbh_malloc(sizeof(usb_device_desc_t));
        if (d->dev_desc) memcpy(d->dev_desc, parser->desc_device,
                                sizeof(usb_device_desc_t));
    }
    if (parser->desc_config) {
        d->cfg_desc_size = parser->desc_config->wTotalLength;
        d->cfg_desc = vsf_usbh_malloc(d->cfg_desc_size);
        if (d->cfg_desc) memcpy(d->cfg_desc, parser->desc_config,
                                d->cfg_desc_size);
    }

    /* Prepend to list */
    d->next = b->device_head;
    b->device_head = d;
    b->num_devices++;

    VSF_ESPIDF_USBH_TRACE("bridge: new device addr=%d vid=0x%04X pid=0x%04X",
        dev->devnum,
        parser->desc_device ? parser->desc_device->idVendor : 0,
        parser->desc_device ? parser->desc_device->idProduct : 0);

    __push_event_to_all_clients(USB_HOST_CLIENT_EVENT_NEW_DEV,
                                dev->devnum, NULL);

    xSemaphoreGive(b->state_lock);
    return d;
}

static void __vsf_espidf_usbh_bridge_disconnect(vk_usbh_t *usbh,
        vk_usbh_dev_t *dev, void *param)
{
    __vsf_espidf_usbh_t *b = &__vsf_espidf_usbh;
    __vsf_espidf_usbh_dev_node_t *entry = (__vsf_espidf_usbh_dev_node_t *)param;

    xSemaphoreTake(b->state_lock, portMAX_DELAY);

    if (entry && entry->is_connected) {
        VSF_ESPIDF_USBH_TRACE("bridge: device disconnected addr=%d",
                              entry->dev_addr);

        __push_event_to_all_clients(USB_HOST_CLIENT_EVENT_DEV_GONE,
                                    0, (usb_device_handle_t)entry);

        /* Remove from list */
        __vsf_espidf_usbh_dev_node_t **pp = &b->device_head;
        while (*pp && *pp != entry) pp = &(*pp)->next;
        if (*pp) { *pp = entry->next; b->num_devices--; }

        if (entry->dev_desc) vsf_usbh_free(entry->dev_desc);
        if (entry->cfg_desc) vsf_usbh_free(entry->cfg_desc);
        vsf_usbh_free(entry);
    }

    if (b->num_devices == 0 && b->lib_events) {
        xEventGroupSetBits(b->lib_events,
                           USB_HOST_LIB_EVENT_FLAGS_ALL_FREE);
    }

    xSemaphoreGive(b->state_lock);
}

/*============================ TRANSFER COMPLETION ===========================*/

static void __transfer_complete(void *param, vk_usbh_hcd_urb_t *urb_hcd)
{
    usb_transfer_t *t = (usb_transfer_t *)param;

    switch (urb_hcd->status) {
    case 0:   t->status = USB_TRANSFER_STATUS_COMPLETED;  break;
    case -1:  t->status = USB_TRANSFER_STATUS_ERROR;      break;
    case -4:  t->status = USB_TRANSFER_STATUS_TIMED_OUT;  break;
    case 2:   t->status = USB_TRANSFER_STATUS_CANCELED;   break;
    default:  t->status = USB_TRANSFER_STATUS_ERROR;      break;
    }
    t->actual_num_bytes = (int)urb_hcd->actual_length;

    /* Don't call urb_hcd free_buffer here -- the user buffer was set via
     * vk_usbh_urb_set_buffer() which clears free_buffer, so there is
     * nothing to free.  urb_hcd fields are accessible because we compile
     * with __VSF_USBH_CLASS_IMPLEMENT. */

    if (t->callback) t->callback(t);
}

/*============================ PUBLIC API IMPLEMENTATION =====================*/

esp_err_t usb_host_install(const usb_host_config_t *config)
{
    __vsf_espidf_usbh_t *b = &__vsf_espidf_usbh;

    if (b->is_installed) return ESP_ERR_INVALID_STATE;

    vk_usbh_t *usbh = vsf_espidf_get_usbh();
    if (!usbh || !usbh->use_as__vk_usbh_hcd_t.drv) {
        VSF_ESPIDF_USBH_TRACE("install: no USBH provided");
        return ESP_ERR_INVALID_STATE;
    }

    memset(b, 0, sizeof(*b));
    b->vsf_usbh = usbh;
    (void)config;

    b->state_lock = xSemaphoreCreateMutex();
    if (!b->state_lock) return ESP_ERR_NO_MEM;

    b->lib_events = xEventGroupCreate();
    if (!b->lib_events) { vSemaphoreDelete(b->state_lock); return ESP_ERR_NO_MEM; }

    b->bridge_drv.name       = "espidf-usbh-bridge";
    b->bridge_drv.dev_id_num = 1;
    b->bridge_drv.dev_ids    = __match_ids;
    b->bridge_drv.probe      = __vsf_espidf_usbh_bridge_probe;
    b->bridge_drv.disconnect = __vsf_espidf_usbh_bridge_disconnect;
    b->bridge_class.drv      = &b->bridge_drv;

    vsf_err_t err = vk_usbh_init(b->vsf_usbh);
    if (err) {
        VSF_ESPIDF_USBH_TRACE("install: vk_usbh_init failed: %d", err);
        vEventGroupDelete(b->lib_events);
        vSemaphoreDelete(b->state_lock);
        return ESP_ERR_NOT_SUPPORTED;
    }

#if VSF_USBH_USE_HUB == ENABLED
    { extern vk_usbh_class_t vk_usbh_hub_class;
      vk_usbh_register_class(b->vsf_usbh, &vk_usbh_hub_class); }
#endif
    vk_usbh_register_class(b->vsf_usbh, &b->bridge_class);
    b->is_installed = true;

    VSF_ESPIDF_USBH_TRACE("install: ready");
    return ESP_OK;
}

esp_err_t usb_host_uninstall(void)
{
    __vsf_espidf_usbh_t *b = &__vsf_espidf_usbh;
    if (!b->is_installed) return ESP_ERR_INVALID_STATE;

    /* Drain and free clients */
    __vsf_espidf_usbh_client_node_t *c = b->client_head;
    while (c) {
        __vsf_espidf_usbh_client_node_t *next = c->next;
        void *msg;
        while (xQueueReceive(c->event_queue, &msg, 0) == pdTRUE)
            vsf_usbh_free(msg);
        vQueueDelete(c->event_queue);
        vsf_usbh_free(c);
        c = next;
    }
    b->client_head = NULL;

    /* Free device entries */
    __vsf_espidf_usbh_dev_node_t *d = b->device_head;
    while (d) {
        __vsf_espidf_usbh_dev_node_t *next = d->next;
        if (d->dev_desc) vsf_usbh_free(d->dev_desc);
        if (d->cfg_desc) vsf_usbh_free(d->cfg_desc);
        vsf_usbh_free(d);
        d = next;
    }
    b->device_head = NULL;

    vk_usbh_fini(b->vsf_usbh);

    if (b->lib_events) { vEventGroupDelete(b->lib_events); }
    if (b->state_lock) { vSemaphoreDelete(b->state_lock); }
    b->is_installed = false;
    return ESP_OK;
}

esp_err_t usb_host_lib_handle_events(TickType_t timeout,
                                     uint32_t *flags)
{
    __vsf_espidf_usbh_t *b = &__vsf_espidf_usbh;
    if (!b->is_installed) return ESP_ERR_INVALID_STATE;
    if (!flags) return ESP_ERR_INVALID_ARG;
    EventBits_t bits = xEventGroupWaitBits(b->lib_events,
        USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS |
        USB_HOST_LIB_EVENT_FLAGS_ALL_FREE,
        pdTRUE, pdFALSE, timeout);
    *flags = (uint32_t)bits;
    return ESP_OK;
}

esp_err_t usb_host_lib_info(usb_host_lib_info_t *info)
{
    __vsf_espidf_usbh_t *b = &__vsf_espidf_usbh;
    if (!b->is_installed) return ESP_ERR_INVALID_STATE;
    if (!info) return ESP_ERR_INVALID_ARG;
    xSemaphoreTake(b->state_lock, portMAX_DELAY);
    info->num_devices = b->num_devices;
    info->num_clients = b->num_clients;
    xSemaphoreGive(b->state_lock);
    return ESP_OK;
}

esp_err_t usb_host_device_free_all(void)
{
    __vsf_espidf_usbh_t *b = &__vsf_espidf_usbh;
    if (!b->is_installed) return ESP_ERR_INVALID_STATE;
    xSemaphoreTake(b->state_lock, portMAX_DELAY);
    for (__vsf_espidf_usbh_dev_node_t *d = b->device_head; d; d = d->next) {
        d->is_opened = false;
        d->owning_client = NULL;
    }
    xSemaphoreGive(b->state_lock);
    return ESP_OK;
}

esp_err_t usb_host_client_register(const usb_host_client_config_t *cfg,
                                   usb_host_client_handle_t *out)
{
    __vsf_espidf_usbh_t *b = &__vsf_espidf_usbh;
    if (!b->is_installed) return ESP_ERR_INVALID_STATE;
    if (!cfg || !out) return ESP_ERR_INVALID_ARG;

    __vsf_espidf_usbh_client_node_t *c = vsf_usbh_malloc(sizeof(*c));
    if (!c) return ESP_ERR_NO_MEM;
    memset(c, 0, sizeof(*c));

    c->event_queue = xQueueCreate(VSF_ESPIDF_CFG_USB_HOST_EVT_QUEUE_DEPTH,
                                  sizeof(void *));
    if (!c->event_queue) { vsf_usbh_free(c); return ESP_ERR_NO_MEM; }

    c->config       = *cfg;
    c->event_cb     = cfg->async.client_event_callback;
    c->callback_arg = cfg->async.callback_arg;
    c->client_hdl   = c;

    xSemaphoreTake(b->state_lock, portMAX_DELAY);
    c->next = b->client_head;
    b->client_head = c;
    b->num_clients++;
    xSemaphoreGive(b->state_lock);

    *out = c;
    return ESP_OK;
}

esp_err_t usb_host_client_deregister(usb_host_client_handle_t hdl)
{
    __vsf_espidf_usbh_t *b = &__vsf_espidf_usbh;
    if (!b->is_installed) return ESP_ERR_INVALID_STATE;
    __vsf_espidf_usbh_client_node_t *c =
        (__vsf_espidf_usbh_client_node_t *)hdl;
    if (!c) return ESP_ERR_INVALID_ARG;

    void *msg;
    while (xQueueReceive(c->event_queue, &msg, 0) == pdTRUE)
        vsf_usbh_free(msg);

    xSemaphoreTake(b->state_lock, portMAX_DELAY);
    __vsf_espidf_usbh_client_node_t **pp = &b->client_head;
    while (*pp && *pp != c) pp = &(*pp)->next;
    if (*pp) { *pp = c->next; b->num_clients--; }
    xSemaphoreGive(b->state_lock);

    vQueueDelete(c->event_queue);
    vsf_usbh_free(c);

    if (b->num_clients == 0 && b->lib_events)
        xEventGroupSetBits(b->lib_events,
                           USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS);
    return ESP_OK;
}

esp_err_t usb_host_client_handle_events(usb_host_client_handle_t hdl,
                                        TickType_t timeout)
{
    __vsf_espidf_usbh_t *b = &__vsf_espidf_usbh;
    if (!b->is_installed) return ESP_ERR_INVALID_STATE;
    __vsf_espidf_usbh_client_node_t *c =
        (__vsf_espidf_usbh_client_node_t *)hdl;
    if (!c) return ESP_ERR_INVALID_ARG;

    void *msg_ptr = NULL;
    if (xQueueReceive(c->event_queue, &msg_ptr, timeout) != pdTRUE)
        return ESP_ERR_TIMEOUT;

    usb_host_client_event_msg_t *m = (usb_host_client_event_msg_t *)msg_ptr;
    if (c->event_cb) c->event_cb(m, c->callback_arg);
    vsf_usbh_free(m);
    return ESP_OK;
}

esp_err_t usb_host_client_unblock(usb_host_client_handle_t hdl)
{
    __vsf_espidf_usbh_client_node_t *c =
        (__vsf_espidf_usbh_client_node_t *)hdl;
    if (!c) return ESP_ERR_INVALID_ARG;
    usb_host_client_event_msg_t *m = vsf_usbh_malloc(sizeof(*m));
    if (m) { memset(m, 0, sizeof(*m)); xQueueSend(c->event_queue, &m, 0); }
    return ESP_OK;
}

esp_err_t usb_host_device_open(usb_host_client_handle_t cli,
                               uint8_t addr, usb_device_handle_t *out)
{
    __vsf_espidf_usbh_t *b = &__vsf_espidf_usbh;
    if (!b->is_installed) return ESP_ERR_INVALID_STATE;
    if (!out) return ESP_ERR_INVALID_ARG;

    xSemaphoreTake(b->state_lock, portMAX_DELAY);
    __vsf_espidf_usbh_dev_node_t *d = __find_dev_by_addr(addr);
    if (!d) { xSemaphoreGive(b->state_lock); return ESP_ERR_NOT_FOUND; }
    d->is_opened = true;
    d->owning_client = cli;
    *out = (usb_device_handle_t)d;
    xSemaphoreGive(b->state_lock);
    return ESP_OK;
}

esp_err_t usb_host_device_close(usb_host_client_handle_t cli,
                                usb_device_handle_t hdl)
{
    __vsf_espidf_usbh_t *b = &__vsf_espidf_usbh;
    if (!b->is_installed) return ESP_ERR_INVALID_STATE;
    (void)cli;

    xSemaphoreTake(b->state_lock, portMAX_DELAY);
    __vsf_espidf_usbh_dev_node_t *d = __find_dev_by_handle(hdl);
    if (!d) { xSemaphoreGive(b->state_lock); return ESP_ERR_NOT_FOUND; }
    d->is_opened = false;
    d->owning_client = NULL;
    xSemaphoreGive(b->state_lock);
    return ESP_OK;
}

esp_err_t usb_host_device_info(usb_device_handle_t hdl,
                               usb_device_info_t *info)
{
    __vsf_espidf_usbh_t *b = &__vsf_espidf_usbh;
    if (!b->is_installed) return ESP_ERR_INVALID_STATE;
    if (!info) return ESP_ERR_INVALID_ARG;

    xSemaphoreTake(b->state_lock, portMAX_DELAY);
    __vsf_espidf_usbh_dev_node_t *d = __find_dev_by_handle(hdl);
    if (!d) { xSemaphoreGive(b->state_lock); return ESP_ERR_NOT_FOUND; }

    memset(info, 0, sizeof(*info));
    info->dev_addr = d->dev_addr;
    if (d->vsf_dev)
        info->speed = (d->vsf_dev->speed > 2) ? 2 : d->vsf_dev->speed;
    if (d->cfg_desc)
        info->bConfigurationValue = d->cfg_desc->bConfigurationValue;
#if VSF_USBH_USE_HUB == ENABLED
    if (d->vsf_dev && d->vsf_dev->dev_parent) {
        for (__vsf_espidf_usbh_dev_node_t *p = b->device_head; p; p = p->next) {
            if (p->vsf_dev == d->vsf_dev->dev_parent) {
                info->parent.dev_hdl = (usb_device_handle_t)p;
                break;
            }
        }
        info->parent.port_num = d->vsf_dev->index + 1;
    }
#endif
    xSemaphoreGive(b->state_lock);
    return ESP_OK;
}

esp_err_t usb_host_get_device_descriptor(usb_device_handle_t hdl,
                                         const usb_device_desc_t **out)
{
    __vsf_espidf_usbh_t *b = &__vsf_espidf_usbh;
    if (!b->is_installed) return ESP_ERR_INVALID_STATE;
    if (!out) return ESP_ERR_INVALID_ARG;

    xSemaphoreTake(b->state_lock, portMAX_DELAY);
    __vsf_espidf_usbh_dev_node_t *d = __find_dev_by_handle(hdl);
    if (!d || !d->dev_desc) { xSemaphoreGive(b->state_lock); return ESP_ERR_NOT_FOUND; }
    *out = d->dev_desc;
    xSemaphoreGive(b->state_lock);
    return ESP_OK;
}

esp_err_t usb_host_get_active_config_descriptor(usb_device_handle_t hdl,
                                                const usb_config_desc_t **out)
{
    __vsf_espidf_usbh_t *b = &__vsf_espidf_usbh;
    if (!b->is_installed) return ESP_ERR_INVALID_STATE;
    if (!out) return ESP_ERR_INVALID_ARG;

    xSemaphoreTake(b->state_lock, portMAX_DELAY);
    __vsf_espidf_usbh_dev_node_t *d = __find_dev_by_handle(hdl);
    if (!d || !d->cfg_desc) { xSemaphoreGive(b->state_lock); return ESP_ERR_NOT_FOUND; }
    *out = d->cfg_desc;
    xSemaphoreGive(b->state_lock);
    return ESP_OK;
}

void usb_print_device_descriptor(const usb_device_desc_t *d)
{
    if (!d) return;
    VSF_ESPIDF_USBH_TRACE("Device: vid=0x%04X pid=0x%04X class=0x%02X "
        "sub=0x%02X proto=0x%02X ep0=%d cfgcnt=%d",
        d->idVendor, d->idProduct, d->bDeviceClass, d->bDeviceSubClass,
        d->bDeviceProtocol, d->bMaxPacketSize0, d->bNumConfigurations);
}

void usb_print_config_descriptor(const usb_config_desc_t *c,
                                 const usb_interface_desc_t *i)
{
    (void)i;
    if (!c) return;
    VSF_ESPIDF_USBH_TRACE("Config: total=%d ifs=%d val=%d attr=0x%02X power=%d",
        c->wTotalLength, c->bNumInterfaces, c->bConfigurationValue,
        c->bmAttributes, c->bMaxPower);
}

void usb_print_string_descriptor(const usb_str_desc_t *s)
{
    if (!s) return;
    VSF_ESPIDF_USBH_TRACE("String: len=%d", s->bLength);
}

esp_err_t usb_host_transfer_alloc(size_t bufsz, int iso_pkts,
                                  usb_transfer_t **out)
{
    if (!out) return ESP_ERR_INVALID_ARG;
    size_t sz = sizeof(__vsf_espidf_usbh_transfer_t);
    if (iso_pkts > 0) sz += iso_pkts * sizeof(usb_isoc_packet_desc_t);
    __vsf_espidf_usbh_transfer_t *x = calloc(1, sz);
    if (!x) return ESP_ERR_NO_MEM;
    x->esp_transfer.data_buffer = (uint8_t *)malloc(bufsz);
    if (!x->esp_transfer.data_buffer && bufsz > 0) { free(x); return ESP_ERR_NO_MEM; }
    x->esp_transfer.data_buffer_size = (uint32_t)bufsz;
    x->esp_transfer.num_isoc_packets = iso_pkts;
    x->esp_transfer.num_bytes = (int)bufsz;
    x->esp_transfer.status = USB_TRANSFER_STATUS_COMPLETED;
    if (iso_pkts > 0)
        x->esp_transfer.isoc_packet_desc = (usb_isoc_packet_desc_t *)(x + 1);
    *out = &x->esp_transfer;
    return ESP_OK;
}

esp_err_t usb_host_transfer_free(usb_transfer_t *t)
{
    if (!t) return ESP_ERR_INVALID_ARG;
    __vsf_espidf_usbh_t *b = &__vsf_espidf_usbh;
    __vsf_espidf_usbh_transfer_t *x = (__vsf_espidf_usbh_transfer_t *)t;
    if (x->is_submitted) {
        vk_usbh_unlink_urb(b->vsf_usbh, &x->vsf_urb);
        x->is_submitted = false;
    }
    free(x->esp_transfer.data_buffer);
    free(x);
    return ESP_OK;
}

esp_err_t usb_host_transfer_submit(usb_transfer_t *t)
{
    __vsf_espidf_usbh_t *b = &__vsf_espidf_usbh;
    if (!b->is_installed) return ESP_ERR_INVALID_STATE;
    if (!t) return ESP_ERR_INVALID_ARG;

    __vsf_espidf_usbh_dev_node_t *d = __find_dev_by_handle(t->device_handle);
    if (!d || !d->vsf_dev) return ESP_ERR_NOT_FOUND;

    __vsf_espidf_usbh_transfer_t *x = (__vsf_espidf_usbh_transfer_t *)t;

    static const int map[] = {
        [USB_TRANSFER_TYPE_CTRL]  = USB_ENDPOINT_XFER_CONTROL,
        [USB_TRANSFER_TYPE_ISOCHRONOUS] = USB_ENDPOINT_XFER_ISOC,
        [USB_TRANSFER_TYPE_BULK]  = USB_ENDPOINT_XFER_BULK,
        [USB_TRANSFER_TYPE_INTR]  = USB_ENDPOINT_XFER_INT,
    };
    if (t->type >= 4) return ESP_ERR_INVALID_ARG;

    vk_usbh_pipe_t pipe = vk_usbh_get_pipe(d->vsf_dev,
        t->bEndpointAddress, map[t->type], t->wMaxPacketSize, 0);
    vk_usbh_urb_prepare_by_pipe(&x->vsf_urb, d->vsf_dev, pipe);

    vsf_err_t err = vk_usbh_alloc_urb(b->vsf_usbh, d->vsf_dev, &x->vsf_urb);
    if (err) return ESP_ERR_NO_MEM;

    vk_usbh_urb_set_buffer(&x->vsf_urb, t->data_buffer,
                           (uint_fast32_t)t->num_bytes);
    vk_usbh_urb_set_complete(&x->vsf_urb, __transfer_complete, t);

    uint_fast16_t f = (t->flags & USB_TRANSFER_FLAG_ZERO_PACK) ? URB_ZERO_PACKET : 0;
    err = vk_usbh_submit_urb_ex(b->vsf_usbh, &x->vsf_urb, f, NULL);
    if (err) { vk_usbh_free_urb(b->vsf_usbh, &x->vsf_urb); return ESP_FAIL; }
    x->is_submitted = true;
    return ESP_OK;
}

esp_err_t usb_host_transfer_submit_control(
        usb_host_client_handle_t cli, usb_device_handle_t hdl,
        const usb_setup_packet_t *setup,
        usb_transfer_t *t)
{
    __vsf_espidf_usbh_t *b = &__vsf_espidf_usbh;
    if (!b->is_installed) return ESP_ERR_INVALID_STATE;
    if (!setup || !t) return ESP_ERR_INVALID_ARG;
    (void)cli;

    __vsf_espidf_usbh_dev_node_t *d = __find_dev_by_handle(hdl);
    if (!d || !d->vsf_dev) return ESP_ERR_NOT_FOUND;

    vk_usbh_dev_t *vd = d->vsf_dev;
    bool is_in = (setup->bmRequestType & USB_DIR_IN) != 0;

    struct usb_ctrlrequest_t req = {
        .bRequestType = setup->bmRequestType, .bRequest = setup->bRequest,
        .wValue = setup->wValue, .wIndex = setup->wIndex, .wLength = setup->wLength,
    };

    /* OUT data stage: pre-fill EP0 URB buffer */
    if (!is_in && req.wLength > 0 && t->data_buffer) {
        vk_usbh_urb_t *urb = &vd->ep0.urb;
        if (!vd->is_ep0_rdy) {
            urb->pipe = vk_usbh_get_pipe(vd, 0, USB_ENDPOINT_XFER_CONTROL, 64);
            if (vk_usbh_alloc_urb(b->vsf_usbh, vd, urb)) {
                t->status = USB_TRANSFER_STATUS_ERROR; return ESP_FAIL;
            }
            vd->is_ep0_rdy = true;
        }
        vk_usbh_urb_set_buffer(urb, t->data_buffer, (uint_fast32_t)t->num_bytes);
    }

    vsf_err_t err = vk_usbh_control_msg(b->vsf_usbh, vd, &req);
    if (err) { t->status = USB_TRANSFER_STATUS_ERROR; t->actual_num_bytes = 0; return ESP_FAIL; }

    if (is_in && req.wLength > 0 && t->data_buffer) {
        vk_usbh_urb_t *urb = &vd->ep0.urb;
        if (vk_usbh_urb_is_alloced(urb)) {
            t->actual_num_bytes = (int)vk_usbh_urb_get_actual_length(urb);
            void *data = vk_usbh_urb_peek_buffer(urb);
            if (data && t->actual_num_bytes > 0) {
                size_t n = (size_t)t->actual_num_bytes;
                if (n > t->data_buffer_size) n = t->data_buffer_size;
                memcpy(t->data_buffer, data, n);
            }
        }
    } else { t->actual_num_bytes = 0; }

    t->status = USB_TRANSFER_STATUS_COMPLETED;
    if (t->callback) t->callback(t);
    return ESP_OK;
}

esp_err_t usb_host_interface_claim(usb_host_client_handle_t cli,
                                   usb_device_handle_t hdl,
                                   uint8_t ifs, uint8_t alt)
{ (void)cli; (void)hdl; (void)ifs; (void)alt; return ESP_OK; }

esp_err_t usb_host_interface_release(usb_host_client_handle_t cli,
                                     usb_device_handle_t hdl, uint8_t ifs)
{ (void)cli; (void)hdl; (void)ifs; return ESP_OK; }

esp_err_t usb_host_endpoint_halt(usb_device_handle_t hdl, uint8_t ep)
{
    __vsf_espidf_usbh_t *b = &__vsf_espidf_usbh;
    if (!b->is_installed) return ESP_ERR_INVALID_STATE;
    __vsf_espidf_usbh_dev_node_t *d = __find_dev_by_handle(hdl);
    if (!d || !d->vsf_dev) return ESP_ERR_NOT_FOUND;
    vk_usbh_clear_endpoint_halt(b->vsf_usbh, d->vsf_dev, ep);
    return ESP_OK;
}

esp_err_t usb_host_endpoint_flush(usb_device_handle_t hdl, uint8_t ep)
{ (void)hdl; (void)ep; return ESP_OK; }

#endif      /* VSF_USE_ESPIDF && VSF_ESPIDF_CFG_USE_USB_HOST */
