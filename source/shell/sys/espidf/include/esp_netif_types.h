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
 * Clean-room re-implementation of ESP-IDF public API "esp_netif_types.h".
 *
 * Authored from ESP-IDF v5.x public API only. No code copied from the
 * ESP-IDF source tree. Layouts and enum values are kept compatible with
 * the upstream contract so that user code can be dropped in as-is.
 */

#ifndef __VSF_ESPIDF_ESP_NETIF_TYPES_H__
#define __VSF_ESPIDF_ESP_NETIF_TYPES_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "esp_err.h"
#include "esp_event_base.h"
#include "esp_netif_ip_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ TYPES =========================================*/

/* Opaque per-interface handle. The concrete layout lives in the port. */
typedef struct esp_netif_obj      esp_netif_t;

/* I/O driver handle passed to esp_netif_attach(). The port casts this to
 * an esp_netif_driver_base_t * and, if post_attach is set, invokes it so
 * the driver can call esp_netif_set_driver_config(). */
typedef void *                    esp_netif_iodriver_handle;

/* IPv4 configuration snapshot. All fields in network byte order. */
typedef struct {
    esp_ip4_addr_t  ip;
    esp_ip4_addr_t  netmask;
    esp_ip4_addr_t  gw;
} esp_netif_ip_info_t;

/* IPv6 configuration snapshot. */
typedef struct {
    esp_ip6_addr_t  ip;
} esp_netif_ip6_info_t;

/* DNS server record. */
typedef struct {
    esp_ip_addr_t   ip;
} esp_netif_dns_info_t;

/* Which DNS slot to read or write. */
typedef enum {
    ESP_NETIF_DNS_MAIN          = 0,
    ESP_NETIF_DNS_BACKUP,
    ESP_NETIF_DNS_FALLBACK,
    ESP_NETIF_DNS_MAX
} esp_netif_dns_type_t;

/* DHCP client / server state. */
typedef enum {
    ESP_NETIF_DHCP_INIT         = 0,
    ESP_NETIF_DHCP_STARTED,
    ESP_NETIF_DHCP_STOPPED,
    ESP_NETIF_DHCP_STATUS_MAX
} esp_netif_dhcp_status_t;

/* Bitmask of per-interface feature flags (esp_netif_inherent_config_t). */
typedef enum {
    ESP_NETIF_DHCP_CLIENT            = 1 << 0,
    ESP_NETIF_DHCP_SERVER            = 1 << 1,
    ESP_NETIF_FLAG_AUTOUP            = 1 << 2,
    ESP_NETIF_FLAG_GARP              = 1 << 3,
    ESP_NETIF_FLAG_EVENT_IP_MODIFIED = 1 << 4,
    ESP_NETIF_FLAG_IS_PPP            = 1 << 5,
    ESP_NETIF_FLAG_IS_BRIDGE         = 1 << 6,
    ESP_NETIF_FLAG_MLDV6_REPORT      = 1 << 7,
} esp_netif_flags_t;

/* Initial L3 configuration applied when the interface is created. */
typedef struct esp_netif_inherent_config {
    esp_netif_flags_t       flags;
    uint8_t                 mac[6];
    const esp_netif_ip_info_t *ip_info;      /*!< static IP, or NULL for zeros */
    uint32_t                get_ip_event;    /*!< event id for got_ip          */
    uint32_t                lost_ip_event;   /*!< event id for lost_ip         */
    const char *            if_key;          /*!< unique string key            */
    const char *            if_desc;         /*!< human-readable description   */
    int                     route_prio;      /*!< higher wins as default route */
    uint32_t                bridge_info;     /*!< bridge only; unused here     */
} esp_netif_inherent_config_t;

/* I/O driver contract passed back through esp_netif_driver_base_t::post_attach.
 * transmit / driver_free_rx_buffer are invoked by esp_netif when the stack
 * wants to send a frame or release a received buffer. */
typedef struct esp_netif_driver_ifconfig {
    esp_netif_iodriver_handle   handle;
    esp_err_t (*transmit)(void *h, void *buffer, size_t len);
    esp_err_t (*transmit_wrap)(void *h, void *buffer, size_t len, void *netstack_buf);
    void      (*driver_free_rx_buffer)(void *h, void *buffer);
} esp_netif_driver_ifconfig_t;

/* Driver-side base object. Users typedef their own struct with this as the
 * first member so esp_netif_attach() can downcast and call post_attach. */
typedef struct esp_netif_driver_base {
    esp_err_t (*post_attach)(esp_netif_t *netif, esp_netif_iodriver_handle h);
    esp_netif_t *netif;
} esp_netif_driver_base_t;

/* Layered config accepted by esp_netif_new(). stack_config and driver_config
 * are optional; a zero-filled struct is enough when the port supplies its
 * own defaults (see esp_netif_defaults.h). */
typedef struct esp_netif_config {
    const esp_netif_inherent_config_t   *base;
    const void                          *driver_config;
    const void                          *stack_config;
} esp_netif_config_t;

/* --- IP_EVENT --------------------------------------------------------- */

/* Event ids dispatched through the process-wide default event loop on the
 * IP_EVENT base. Values match the ESP-IDF v5.x public contract. */
typedef enum {
    IP_EVENT_STA_GOT_IP         = 0,
    IP_EVENT_STA_LOST_IP,
    IP_EVENT_AP_STAIPASSIGNED,
    IP_EVENT_GOT_IP6,
    IP_EVENT_ETH_GOT_IP,
    IP_EVENT_ETH_LOST_IP,
    IP_EVENT_PPP_GOT_IP,
    IP_EVENT_PPP_LOST_IP,
} ip_event_t;

/* Payload dispatched with IP_EVENT_STA_GOT_IP / IP_EVENT_ETH_GOT_IP. */
typedef struct {
    esp_netif_t            *esp_netif;
    esp_netif_ip_info_t     ip_info;
    bool                    ip_changed;
} ip_event_got_ip_t;

/* Payload dispatched with IP_EVENT_GOT_IP6. */
typedef struct {
    esp_netif_t            *esp_netif;
    esp_netif_ip6_info_t    ip6_info;
    int                     ip_index;
} ip_event_got_ip6_t;

/* Payload dispatched with IP_EVENT_AP_STAIPASSIGNED. */
typedef struct {
    esp_netif_t            *esp_netif;
    esp_ip4_addr_t          ip;
    uint8_t                 mac[6];
} ip_event_ap_staipassigned_t;

/*============================ GLOBALS =======================================*/

/* Event base for all ip_event_t values. Defined in esp_netif_port.c. */
ESP_EVENT_DECLARE_BASE(IP_EVENT);

#ifdef __cplusplus
}
#endif

#endif      // __VSF_ESPIDF_ESP_NETIF_TYPES_H__
