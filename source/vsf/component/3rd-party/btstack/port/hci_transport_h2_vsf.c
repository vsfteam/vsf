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

#include "vsf.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USE_USB_HOST_BTHCI == ENABLED

#include "btstack_config.h"
#include "btstack_debug.h"
#include "hci.h"
#include "hci_transport.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct hci_transport_h2_param_t {
    bool is_opened;
    bool is_notified;
    void *dev;
    void (*packet_handler)(uint8_t packet_type, uint8_t *packet, uint16_t size);
};
typedef struct hci_transport_h2_param_t hci_transport_h2_param_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static hci_transport_h2_param_t hci_transport_h2_param;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

WEAK vsf_err_t vsf_bluetooth_h2_on_new(void *dev, vsf_usbh_dev_id_t *id)
{
    return VSF_ERR_NOT_SUPPORT;
}

static int hci_transport_h2_open(void)
{
    if (hci_transport_h2_param.dev) {
        hci_transport_h2_param.is_opened = true;
        return 0;
    }
    return -1;
}

static int hci_transport_h2_close(void)
{
    hci_transport_h2_param.is_opened = false;
    return 0;
}

static void hci_transport_h2_register_packet_handler(
        void (*handler)(uint8_t packet_type, uint8_t *packet, uint16_t size))
{
    hci_transport_h2_param.packet_handler = handler;
}

static int hci_transport_h2_can_send_packet_now(uint8_t packet_type)
{
    if (hci_transport_h2_param.is_opened) {
        return vsf_usbh_bthci_can_send(hci_transport_h2_param.dev, packet_type);
    }
    return 0;
}

static int hci_transport_h2_send_packet(uint8_t packet_type, uint8_t *packet, int size)
{
    if (hci_transport_h2_param.is_opened) {
        return vsf_usbh_bthci_send(hci_transport_h2_param.dev, packet_type, packet, size);
    }
    return 0;
}

void vsf_usbh_bthci_on_new(void *dev, vsf_usbh_dev_id_t *id)
{
    if (!hci_transport_h2_param.dev) {
        hci_transport_h2_param.dev = dev;
        if (!hci_transport_h2_param.is_notified) {
            hci_transport_h2_param.is_notified = true;
            if (VSF_ERR_NONE != vsf_bluetooth_h2_on_new(dev, id)) {
                hci_transport_h2_param.dev = NULL;
                hci_transport_h2_param.is_notified = false;
            }
        }
    }
}

void vsf_usbh_bthci_on_del(void *dev)
{
    if (dev == hci_transport_h2_param.dev) {
        hci_transport_h2_param.dev = NULL;
    }
}

void vsf_usbh_bthci_on_packet(void *dev, uint8_t type, uint8_t *packet, uint16_t size)
{
    const uint8_t event[] = {HCI_EVENT_TRANSPORT_PACKET_SENT, 0};

    if (    hci_transport_h2_param.is_opened
        &&  (dev == hci_transport_h2_param.dev)
        &&  (NULL != hci_transport_h2_param.packet_handler)) {

        if (type & 0x80) {
            type = HCI_EVENT_PACKET;
            packet = (uint8_t *)event;
            size = sizeof(event);
        }
        hci_transport_h2_param.packet_handler(type, packet, size);
    }
}

static void hci_transport_h2_init(const void *transport_config)
{
    
}

static const hci_transport_t hci_transport_h2 = {
    .name = "H2_VSF",
    .init = hci_transport_h2_init,
    .open = hci_transport_h2_open,
    .close = hci_transport_h2_close,
    .register_packet_handler = hci_transport_h2_register_packet_handler,
    .can_send_packet_now = hci_transport_h2_can_send_packet_now,
    .send_packet = hci_transport_h2_send_packet,
};

const hci_transport_t * hci_transport_usb_instance(void)
{
    return &hci_transport_h2;
}

#endif      // VSF_USE_USB_HOST && VSF_USE_USB_HOST_BTHCI
