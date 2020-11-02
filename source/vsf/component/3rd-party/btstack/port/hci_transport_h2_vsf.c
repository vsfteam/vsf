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

#include "component/vsf_component_cfg.h"

#if VSF_USE_BTSTACK == ENABLED && VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_BTHCI == ENABLED

#include "component/usb/vsf_usb.h"

#include "btstack_config.h"
#include "btstack_debug.h"
#include "hci.h"
#include "hci_transport.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct hci_transport_h2_param_t {
    bool is_opened;
    bool is_notified;
    void *dev;
    void (*packet_handler)(uint8_t packet_type, uint8_t *packet, uint16_t size);
} hci_transport_h2_param_t;

/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_bluetooth_h2_on_new(void *dev, vk_usbh_dev_id_t *id);

static void __hci_transport_h2_init(const void *transport_config);
static int __hci_transport_h2_open(void);
static int __hci_transport_h2_close(void);
static void __hci_transport_h2_register_packet_handler(
        void (*handler)(uint8_t packet_type, uint8_t *packet, uint16_t size));
static int __hci_transport_h2_can_send_packet_now(uint8_t packet_type);
static int __hci_transport_h2_send_packet(uint8_t packet_type, uint8_t *packet, int size);

/*============================ GLOBAL VARIABLES ==============================*/

static const hci_transport_t hci_transport_h2 = {
    .name = "H2_VSF",
    .init = __hci_transport_h2_init,
    .open = __hci_transport_h2_open,
    .close = __hci_transport_h2_close,
    .register_packet_handler = __hci_transport_h2_register_packet_handler,
    .can_send_packet_now = __hci_transport_h2_can_send_packet_now,
    .send_packet = __hci_transport_h2_send_packet,
};

/*============================ LOCAL VARIABLES ===============================*/

static hci_transport_h2_param_t __hci_transport_h2_param;

/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSF_BLUETOOTH_H2_ON_NEW
WEAK(vsf_bluetooth_h2_on_new)
vsf_err_t vsf_bluetooth_h2_on_new(void *dev, vk_usbh_dev_id_t *id)
{
    return VSF_ERR_NOT_SUPPORT;
}
#endif

static int __hci_transport_h2_open(void)
{
    if (__hci_transport_h2_param.dev) {
        __hci_transport_h2_param.is_opened = true;
        return 0;
    }
    return -1;
}

static int __hci_transport_h2_close(void)
{
    __hci_transport_h2_param.is_opened = false;
    return 0;
}

static void __hci_transport_h2_register_packet_handler(
        void (*handler)(uint8_t packet_type, uint8_t *packet, uint16_t size))
{
    __hci_transport_h2_param.packet_handler = handler;
}

static int __hci_transport_h2_can_send_packet_now(uint8_t packet_type)
{
    if (__hci_transport_h2_param.is_opened && (__hci_transport_h2_param.dev != NULL)) {
        return vk_usbh_bthci_can_send(__hci_transport_h2_param.dev, packet_type);
    }
    return 0;
}

static int __hci_transport_h2_send_packet(uint8_t packet_type, uint8_t *packet, int size)
{
    if (__hci_transport_h2_param.is_opened && (__hci_transport_h2_param.dev != NULL)) {
        return vk_usbh_bthci_send(__hci_transport_h2_param.dev, packet_type, packet, size);
    }
    return 0;
}

void vsf_usbh_bthci_on_new(void *dev, vk_usbh_dev_id_t *id)
{
    if (!__hci_transport_h2_param.dev) {
        __hci_transport_h2_param.dev = dev;
        if (!__hci_transport_h2_param.is_notified) {
            __hci_transport_h2_param.is_notified = true;
            if (VSF_ERR_NONE != vsf_bluetooth_h2_on_new(dev, id)) {
                __hci_transport_h2_param.dev = NULL;
                __hci_transport_h2_param.is_notified = false;
            }
        }
    }
}

void vsf_usbh_bthci_on_del(void *dev)
{
    if (dev == __hci_transport_h2_param.dev) {
        __hci_transport_h2_param.dev = NULL;
    }
}

void vsf_usbh_bthci_on_packet(void *dev, uint8_t type, uint8_t *packet, uint16_t size)
{
    const uint8_t event[] = {HCI_EVENT_TRANSPORT_PACKET_SENT, 0};

    if (    __hci_transport_h2_param.is_opened
        &&  (dev == __hci_transport_h2_param.dev)
        &&  (NULL != __hci_transport_h2_param.packet_handler)) {

        if (type & 0x80) {
            type = HCI_EVENT_PACKET;
            packet = (uint8_t *)event;
            size = sizeof(event);
        }
        __hci_transport_h2_param.packet_handler(type, packet, size);
    }
}

static void __hci_transport_h2_init(const void *transport_config)
{
    
}

const hci_transport_t * hci_transport_usb_instance(void)
{
    return &hci_transport_h2;
}

#endif      // VSF_USE_USB_HOST && VSF_USBH_USE_BTHCI
