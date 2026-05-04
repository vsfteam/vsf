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
 * Clean-room re-implementation of ESP-IDF public API "esp_netif_defaults.h".
 *
 * Authored from ESP-IDF v5.x public API only. No code copied from the
 * ESP-IDF source tree. The macros below synthesize inherent-config
 * templates with sensible defaults so user code can write:
 *
 *   esp_netif_inherent_config_t base = ESP_NETIF_INHERENT_DEFAULT_ETH();
 *   esp_netif_config_t cfg = { .base = &base };
 *   esp_netif_t *eth = esp_netif_new(&cfg);
 */

#ifndef __VSF_ESPIDF_ESP_NETIF_DEFAULTS_H__
#define __VSF_ESPIDF_ESP_NETIF_DEFAULTS_H__

#include "esp_netif_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define ESP_NETIF_INHERENT_DEFAULT_ETH()                                    \
    {                                                                       \
        .flags          = (esp_netif_flags_t)                               \
                          (ESP_NETIF_DHCP_CLIENT | ESP_NETIF_FLAG_GARP |    \
                           ESP_NETIF_FLAG_EVENT_IP_MODIFIED),               \
        .mac            = { 0 },                                            \
        .ip_info        = NULL,                                             \
        .get_ip_event   = IP_EVENT_ETH_GOT_IP,                              \
        .lost_ip_event  = IP_EVENT_ETH_LOST_IP,                             \
        .if_key         = "ETH_DEF",                                        \
        .if_desc        = "eth",                                            \
        .route_prio     = 50,                                               \
        .bridge_info    = 0,                                                \
    }

#define ESP_NETIF_INHERENT_DEFAULT_WIFI_STA()                               \
    {                                                                       \
        .flags          = (esp_netif_flags_t)                               \
                          (ESP_NETIF_DHCP_CLIENT | ESP_NETIF_FLAG_GARP |    \
                           ESP_NETIF_FLAG_EVENT_IP_MODIFIED),               \
        .mac            = { 0 },                                            \
        .ip_info        = NULL,                                             \
        .get_ip_event   = IP_EVENT_STA_GOT_IP,                              \
        .lost_ip_event  = IP_EVENT_STA_LOST_IP,                             \
        .if_key         = "WIFI_STA_DEF",                                   \
        .if_desc        = "sta",                                            \
        .route_prio     = 100,                                              \
        .bridge_info    = 0,                                                \
    }

#define ESP_NETIF_INHERENT_DEFAULT_WIFI_AP()                                \
    {                                                                       \
        .flags          = (esp_netif_flags_t)                               \
                          (ESP_NETIF_DHCP_SERVER | ESP_NETIF_FLAG_AUTOUP),  \
        .mac            = { 0 },                                            \
        .ip_info        = NULL,                                             \
        .get_ip_event   = 0,                                                \
        .lost_ip_event  = 0,                                                \
        .if_key         = "WIFI_AP_DEF",                                    \
        .if_desc        = "ap",                                             \
        .route_prio     = 10,                                               \
        .bridge_info    = 0,                                                \
    }

/* Helpers that wrap the inherent-config into a full esp_netif_config_t. */
#define ESP_NETIF_DEFAULT_ETH_CFG(_base)                                    \
    { .base = (_base), .driver_config = NULL, .stack_config = NULL }

#ifdef __cplusplus
}
#endif

#endif      // __VSF_ESPIDF_ESP_NETIF_DEFAULTS_H__
