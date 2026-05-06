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
 * Clean-room re-implementation of the ESP-IDF ROM USB Device API
 * ("esp32s2/rom/usb/usb_device.h").
 *
 * Authored from the public API reference only. No code copied from the
 * ESP-IDF source tree.
 *
 * Standard USB descriptor types (struct usb_device_descriptor etc.) are
 * obtained from VSF's usb_common.h via a conditional include so that types
 * never duplicate.
 */

#ifndef __VSF_ESPIDF_USB_DEVICE_H__
#define __VSF_ESPIDF_USB_DEVICE_H__

/*============================ INCLUDES ======================================*/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "esp_err.h"

#include "utilities/compiler/compiler.h"

#include "component/usb/common/usb_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define MAX_PACKET_SIZE0            64

#define USB_EP_DIR_MASK             0x80
#define USB_EP_DIR_IN               0x80
#define USB_EP_DIR_OUT              0x00

#define USB_TRANS_READ              BIT(0)
#define USB_TRANS_WRITE             BIT(1)
#define USB_TRANS_NO_ZLP            BIT(2)

/*============================ TYPES =========================================*/

enum usb_dc_status_code {
    USB_DC_ERROR,
    USB_DC_RESET,
    USB_DC_CONNECTED,
    USB_DC_CONFIGURED,
    USB_DC_DISCONNECTED,
    USB_DC_SUSPEND,
    USB_DC_RESUME,
    USB_DC_INTERFACE,
    USB_DC_SET_HALT,
    USB_DC_CLEAR_HALT,
    USB_DC_UNKNOWN
};

enum usb_dc_ep_cb_status_code {
    USB_DC_EP_SETUP,
    USB_DC_EP_DATA_OUT,
    USB_DC_EP_DATA_IN,
};

struct usb_setup_packet {
    uint8_t  bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} VSF_CAL_PACKED;

typedef void (*usb_status_callback)(enum usb_dc_status_code status_code,
                                    uint8_t *param);

typedef void (*usb_ep_callback)(uint8_t ep,
                                enum usb_dc_ep_cb_status_code cb_status);

typedef int (*usb_request_handler)(struct usb_setup_packet *setup,
                                   int32_t *transfer_len,
                                   uint8_t **payload_data);

typedef void (*usb_interface_config)(uint8_t bInterfaceNumber);

typedef void (*usb_transfer_callback)(uint8_t ep, int tsize, void *priv);

struct usb_ep_cfg_data {
    usb_ep_callback ep_cb;
    uint8_t         ep_addr;
};

struct usb_interface_cfg_data {
    usb_request_handler class_handler;
    usb_request_handler vendor_handler;
    usb_request_handler custom_handler;
    uint8_t            *payload_data;
    uint8_t            *vendor_data;
};

struct usb_cfg_data {
    const uint8_t                  *usb_device_description;
    const void                     *interface_descriptor;
    usb_interface_config            interface_config;
    usb_status_callback             cb_usb_status;
    struct usb_interface_cfg_data   interface;
    uint8_t                         num_endpoints;
    struct usb_ep_cfg_data         *endpoint;
};

/*============================ PROTOTYPES ====================================*/

int usb_set_config(struct usb_cfg_data *config);
int usb_deconfig(void);
int usb_enable(struct usb_cfg_data *config);
int usb_disable(void);

int usb_write_would_block(uint8_t ep);
int usb_write(uint8_t ep, const uint8_t *data, uint32_t data_len,
              uint32_t *bytes_ret);
int usb_read(uint8_t ep, uint8_t *data, uint32_t max_data_len,
             uint32_t *ret_bytes);

int usb_ep_set_stall(uint8_t ep);
int usb_ep_clear_stall(uint8_t ep);

int usb_ep_read_wait(uint8_t ep, uint8_t *data, uint32_t max_data_len,
                     uint32_t *read_bytes);
int usb_ep_read_continue(uint8_t ep);

void usb_transfer_ep_callback(uint8_t ep,
                              enum usb_dc_ep_cb_status_code cb_status);
int usb_transfer(uint8_t ep, uint8_t *data, size_t dlen, unsigned int flags,
                 usb_transfer_callback cb, void *priv);
int usb_transfer_sync(uint8_t ep, uint8_t *data, size_t dlen,
                      unsigned int flags);
void usb_cancel_transfer(uint8_t ep);

void usb_dev_deinit(void);
void usb_dev_resume(int configuration);
int usb_dev_get_configuration(void);

#ifdef __cplusplus
}
#endif

#endif      /* __VSF_ESPIDF_USB_DEVICE_H__ */
