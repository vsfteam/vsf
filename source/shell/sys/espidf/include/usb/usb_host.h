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
 * Clean-room re-implementation of ESP-IDF public API "usb/usb_host.h".
 *
 * Authored from ESP-IDF v5.x public API reference only. No code copied from
 * the ESP-IDF source tree.
 *
 * Standard USB descriptor types (usb_device_desc_t etc.) are obtained from
 * VSF's usb_common.h via a conditional include so that types never duplicate
 * regardless of which headers the application pulls in first.
 */

#ifndef __VSF_ESPIDF_USB_HOST_H__
#define __VSF_ESPIDF_USB_HOST_H__

/*============================ INCLUDES ======================================*/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "esp_err.h"

#include "FreeRTOS.h"
#include "task.h"

#include "utilities/compiler/compiler.h"

#include "component/usb/common/usb_common.h"

/* usb_str_desc_t is not in VSF's usb_common.h -- define it here. */
#ifndef __vsf_espidf_usb_str_desc_t
#   define __vsf_espidf_usb_str_desc_t
    typedef struct {
        uint8_t  bLength;
        uint8_t  bDescriptorType;
        uint8_t  bString[];
    } VSF_CAL_PACKED usb_str_desc_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*============================ TYPES =========================================*/

/* Opaque handles */
typedef void * usb_host_client_handle_t;
typedef void * usb_device_handle_t;

/* USB Host Library configuration (ESP-IDF v5.x layout) */
typedef struct {
    bool        skip_phy_setup;
    int         intr_flags;
    bool      (*enum_filter_cb)(const usb_device_desc_t *dev_desc,
                                uint8_t *bConfigurationValue);
    uint32_t    peripheral_map;
} usb_host_config_t;

/* Client event types */
typedef enum {
    USB_HOST_CLIENT_EVENT_NEW_DEV,
    USB_HOST_CLIENT_EVENT_DEV_GONE,
} usb_host_client_event_t;

/* Client event message */
typedef struct usb_host_client_event_msg {
    usb_host_client_event_t event;
    union {
        struct {
            uint8_t address;
        } new_dev;
        struct {
            usb_device_handle_t dev_hdl;
        } dev_gone;
    };
} usb_host_client_event_msg_t;

/* Client event callback type */
typedef void (*usb_host_client_event_cb_t)(
    const usb_host_client_event_msg_t *event_msg, void *arg);

/* USB Host Client configuration */
typedef struct {
    bool        is_synchronous;
    int         max_num_event_msg;
    union {
        struct {
            usb_host_client_event_cb_t client_event_callback;
            void *callback_arg;
        } async;
    };
} usb_host_client_config_t;

/* Device information */
typedef struct {
    uint8_t              dev_addr;
    uint8_t              bConfigurationValue;
    int                  speed;
    struct {
        usb_device_handle_t dev_hdl;
        uint8_t              port_num;
    } parent;
    const usb_str_desc_t *str_desc_manufacturer;
    const usb_str_desc_t *str_desc_product;
    const usb_str_desc_t *str_desc_serial_number;
} usb_device_info_t;

/* USB transfer status codes */
typedef enum {
    USB_TRANSFER_STATUS_COMPLETED = 0,
    USB_TRANSFER_STATUS_ERROR     = 1,
    USB_TRANSFER_STATUS_TIMED_OUT = 2,
    USB_TRANSFER_STATUS_CANCELED  = 3,
    USB_TRANSFER_STATUS_STALL     = 4,
    USB_TRANSFER_STATUS_OVERFLOW  = 5,
    USB_TRANSFER_STATUS_NO_DEVICE = 6,
    USB_TRANSFER_STATUS_SKIPPED   = 7,
} usb_transfer_status_t;

/* USB transfer type */
typedef enum {
    USB_TRANSFER_TYPE_CTRL   = 0,
    USB_TRANSFER_TYPE_ISOCHRONOUS = 1,
    USB_TRANSFER_TYPE_BULK   = 2,
    USB_TRANSFER_TYPE_INTR   = 3,
} usb_transfer_type_t;

/* ISO packet descriptor */
typedef struct {
    uint32_t offset;
    uint32_t length;
    uint32_t actual_length;
    usb_transfer_status_t status;
} usb_isoc_packet_desc_t;

/* Forward declaration */
struct usb_transfer_s;

/* Transfer completion callback */
typedef void (*usb_transfer_cb_t)(struct usb_transfer_s *transfer);

/* USB Transfer structure */
typedef struct usb_transfer_s {
    usb_device_handle_t     device_handle;
    uint8_t                 bEndpointAddress;
    usb_transfer_type_t     type;
    uint8_t                 bmAttributes;
    uint16_t                wMaxPacketSize;
    int                     num_bytes;
    int                     actual_num_bytes;
    uint32_t                flags;
    uint32_t                timeout_ms;
    usb_transfer_status_t   status;
    usb_transfer_cb_t       callback;
    void                   *context;
    int                     num_isoc_packets;
    usb_isoc_packet_desc_t *isoc_packet_desc;
    uint8_t                *data_buffer;
    uint32_t                data_buffer_size;
    void                   *reserved[4];
} usb_transfer_t;

/* Library info */
typedef struct {
    int num_devices;
    int num_clients;
} usb_host_lib_info_t;

/* Library event flags */
#define USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS  (1 << 0)
#define USB_HOST_LIB_EVENT_FLAGS_ALL_FREE    (1 << 1)

/* Transfer flags */
#define USB_TRANSFER_FLAG_ZERO_PACK  (1 << 0)

/* USB setup packet (8 bytes, USB 2.0 standard) */
typedef struct {
    uint8_t  bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} VSF_CAL_PACKED usb_setup_packet_t;

/*============================ PROTOTYPES ====================================*/

esp_err_t usb_host_install(const usb_host_config_t *config);
esp_err_t usb_host_uninstall(void);
esp_err_t usb_host_lib_handle_events(TickType_t timeout_ticks,
                                     uint32_t *event_flags_ret);
esp_err_t usb_host_lib_info(usb_host_lib_info_t *lib_info_ret);
esp_err_t usb_host_device_free_all(void);
esp_err_t usb_host_client_register(const usb_host_client_config_t *client_config,
                                   usb_host_client_handle_t *client_hdl_ret);
esp_err_t usb_host_client_deregister(usb_host_client_handle_t client_hdl);
esp_err_t usb_host_client_handle_events(usb_host_client_handle_t client_hdl,
                                        TickType_t timeout_ticks);
esp_err_t usb_host_client_unblock(usb_host_client_handle_t client_hdl);
esp_err_t usb_host_device_open(usb_host_client_handle_t client_hdl,
                               uint8_t dev_addr,
                               usb_device_handle_t *dev_hdl_ret);
esp_err_t usb_host_device_close(usb_host_client_handle_t client_hdl,
                                usb_device_handle_t dev_hdl);
esp_err_t usb_host_device_info(usb_device_handle_t dev_hdl,
                               usb_device_info_t *dev_info);
esp_err_t usb_host_get_device_descriptor(usb_device_handle_t dev_hdl,
                                         const usb_device_desc_t **dev_desc_ret);
esp_err_t usb_host_get_active_config_descriptor(usb_device_handle_t dev_hdl,
                                                const usb_config_desc_t **config_desc_ret);
void usb_print_device_descriptor(const usb_device_desc_t *dev_desc);
void usb_print_config_descriptor(const usb_config_desc_t *config_desc,
                                 const usb_interface_desc_t *ifc_desc);
void usb_print_string_descriptor(const usb_str_desc_t *str_desc);
esp_err_t usb_host_transfer_alloc(size_t data_buffer_size,
                                  int num_isoc_packets,
                                  usb_transfer_t **transfer_ret);
esp_err_t usb_host_transfer_free(usb_transfer_t *transfer);
esp_err_t usb_host_transfer_submit(usb_transfer_t *transfer);
esp_err_t usb_host_transfer_submit_control(usb_host_client_handle_t client_hdl,
                                           usb_device_handle_t dev_hdl,
                                           const usb_setup_packet_t *setup_packet,
                                           usb_transfer_t *transfer);
esp_err_t usb_host_interface_claim(usb_host_client_handle_t client_hdl,
                                   usb_device_handle_t dev_hdl,
                                   uint8_t bInterfaceNumber,
                                   uint8_t bAlternateSetting);
esp_err_t usb_host_interface_release(usb_host_client_handle_t client_hdl,
                                     usb_device_handle_t dev_hdl,
                                     uint8_t bInterfaceNumber);
esp_err_t usb_host_endpoint_halt(usb_device_handle_t dev_hdl,
                                 uint8_t bEndpointAddress);
esp_err_t usb_host_endpoint_flush(usb_device_handle_t dev_hdl,
                                  uint8_t bEndpointAddress);

#ifdef __cplusplus
}
#endif

#endif      // __VSF_ESPIDF_USB_HOST_H__
