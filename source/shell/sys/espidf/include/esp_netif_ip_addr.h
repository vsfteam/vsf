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
 * Clean-room re-implementation of ESP-IDF public API "esp_netif_ip_addr.h".
 *
 * Authored from ESP-IDF v5.x public API only. No code copied from the
 * ESP-IDF source tree. Addresses are stored in network byte order to match
 * the ESP-IDF contract; helper macros mirror the upstream names.
 */

#ifndef __VSF_ESPIDF_ESP_NETIF_IP_ADDR_H__
#define __VSF_ESPIDF_ESP_NETIF_IP_ADDR_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*============================ TYPES =========================================*/

/* IPv4 address in network byte order. */
typedef struct esp_ip4_addr {
    uint32_t    addr;
} esp_ip4_addr_t;

/* IPv6 address in network byte order. zone is the IPv6 scope id
 * (0 when unscoped). */
typedef struct esp_ip6_addr {
    uint32_t    addr[4];
    uint8_t     zone;
} esp_ip6_addr_t;

/* Tagged union covering both address families. type is one of the
 * ESP_IPADDR_TYPE_* constants below. */
typedef struct esp_ip_addr {
    union {
        esp_ip6_addr_t  ip6;
        esp_ip4_addr_t  ip4;
    } u_addr;
    uint8_t     type;
} esp_ip_addr_t;

/* Address family tag values for esp_ip_addr_t::type. */
#define ESP_IPADDR_TYPE_V4              0U
#define ESP_IPADDR_TYPE_V6              6U
#define ESP_IPADDR_TYPE_ANY             46U

/*============================ MACROS ========================================*/

/* Build an esp_ip4_addr_t from four dotted-decimal octets (host-order args,
 * stored in network byte order). Matches ESP-IDF ESP_IP4TOADDR semantics. */
#define ESP_IP4TOUINT32(a, b, c, d)                                         \
        (((uint32_t)((a) & 0xff))       |                                   \
         ((uint32_t)((b) & 0xff) <<  8) |                                   \
         ((uint32_t)((c) & 0xff) << 16) |                                   \
         ((uint32_t)((d) & 0xff) << 24))

#define ESP_IP4TOADDR(a, b, c, d)       ESP_IP4TOUINT32(a, b, c, d)

/* Copy a raw 32-bit (network-order) IPv4 value into an esp_ip4_addr_t. */
#define ESP_IP4ADDR(ipaddr, a, b, c, d)                                     \
        do {                                                                \
            (ipaddr)->addr = ESP_IP4TOADDR(a, b, c, d);                     \
        } while (0)

/* Extract an octet from a network-order IPv4 address. The _n form reads the
 * byte at offset n (0 is the lowest-order byte of the network value). */
#define esp_ip4_addr1(ipaddr)           ((uint8_t)((ipaddr)->addr        & 0xff))
#define esp_ip4_addr2(ipaddr)           ((uint8_t)(((ipaddr)->addr >> 8) & 0xff))
#define esp_ip4_addr3(ipaddr)           ((uint8_t)(((ipaddr)->addr >> 16) & 0xff))
#define esp_ip4_addr4(ipaddr)           ((uint8_t)(((ipaddr)->addr >> 24) & 0xff))

/* printf helper: expands to "%u.%u.%u.%u" argument tuple. */
#define IPSTR                           "%u.%u.%u.%u"
#define IP2STR(ipaddr)                                                      \
        esp_ip4_addr1(ipaddr),                                              \
        esp_ip4_addr2(ipaddr),                                              \
        esp_ip4_addr3(ipaddr),                                              \
        esp_ip4_addr4(ipaddr)

#ifdef __cplusplus
}
#endif

#endif      // __VSF_ESPIDF_ESP_NETIF_IP_ADDR_H__
