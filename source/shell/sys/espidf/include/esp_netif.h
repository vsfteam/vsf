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
 * Clean-room re-implementation of ESP-IDF public API "esp_netif.h".
 *
 * Authored from ESP-IDF v5.x public API only. No code copied from the
 * ESP-IDF source tree. The VSF port is backed by component/tcpip/netdrv
 * (vk_netdrv_t) + the lwIP raw API; all IP-layer state lives inside lwIP.
 *
 * Coverage: the subset the stage-3 roadmap depends on -- lifecycle,
 * start/stop, IP info, DHCP client, DNS, hostname, MAC, driver attach.
 * DHCP server, PPP, bridge, SLAAC helpers are not implemented yet.
 */

#ifndef __VSF_ESPIDF_ESP_NETIF_H__
#define __VSF_ESPIDF_ESP_NETIF_H__

#include "esp_err.h"
#include "esp_event_base.h"
#include "esp_netif_ip_addr.h"
#include "esp_netif_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ LIFECYCLE =====================================*/

/* One-time process-wide init. May be called multiple times; subsequent
 * calls are no-ops. Usually invoked internally by vsf_espidf_init(); user
 * apps can call it explicitly for defensive reasons. */
esp_err_t esp_netif_init(void);

/* Deinitialize the process-wide netif layer. Typically unused on VSF. */
esp_err_t esp_netif_deinit(void);

/* Allocate a new interface described by cfg. cfg->base must be non-NULL.
 * Returns NULL on allocation failure or exhausted instance pool. */
esp_netif_t * esp_netif_new(const esp_netif_config_t *cfg);

/* Release an interface. If it is still running, it is stopped first. */
void esp_netif_destroy(esp_netif_t *esp_netif);

/*============================ DRIVER ATTACH =================================*/

/* Bind a driver handle to the interface. driver_handle is cast to
 * esp_netif_driver_base_t *; if its post_attach pointer is set, the port
 * invokes it so the driver can call esp_netif_set_driver_config(). */
esp_err_t esp_netif_attach(esp_netif_t *esp_netif,
                           esp_netif_iodriver_handle driver_handle);

/* Store the driver ifconfig in the esp_netif. Usually called from the
 * driver's post_attach implementation. */
esp_err_t esp_netif_set_driver_config(esp_netif_t *esp_netif,
                                      const esp_netif_driver_ifconfig_t *cfg);

/*---------- VSF extension ---------------------------------------------------*/

/* VSF-specific attach path: bind a vk_netdrv_t directly. This is the
 * primary attach API when the link driver is a native VSF netdrv (e.g.
 * the wpcap shim). Mutually exclusive with esp_netif_attach(). */
struct vk_netdrv;
esp_err_t esp_netif_attach_netdrv(esp_netif_t *esp_netif,
                                  struct vk_netdrv *netdrv);

/*============================ ACTIONS =======================================*/

/* Canonical lifecycle actions. esp_netif_action_start() brings the lwIP
 * netif into the stack (netif_add), applies the configured static IP when
 * no DHCP client flag is set, and enables link_up + netif_up. The
 * _connected variant is invoked by drivers when the L2 layer reports
 * carrier; on VSF it is equivalent to _start. */
esp_err_t esp_netif_action_start(esp_netif_t *esp_netif,
                                 esp_event_base_t base,
                                 int32_t event_id, void *data);
esp_err_t esp_netif_action_stop(esp_netif_t *esp_netif,
                                esp_event_base_t base,
                                int32_t event_id, void *data);
esp_err_t esp_netif_action_connected(esp_netif_t *esp_netif,
                                     esp_event_base_t base,
                                     int32_t event_id, void *data);
esp_err_t esp_netif_action_disconnected(esp_netif_t *esp_netif,
                                        esp_event_base_t base,
                                        int32_t event_id, void *data);
esp_err_t esp_netif_action_got_ip(esp_netif_t *esp_netif,
                                  esp_event_base_t base,
                                  int32_t event_id, void *data);

/*============================ IP CONFIGURATION ==============================*/

esp_err_t esp_netif_set_ip_info(esp_netif_t *esp_netif,
                                const esp_netif_ip_info_t *ip_info);
esp_err_t esp_netif_get_ip_info(esp_netif_t *esp_netif,
                                esp_netif_ip_info_t *ip_info);

esp_err_t esp_netif_dhcpc_start(esp_netif_t *esp_netif);
esp_err_t esp_netif_dhcpc_stop(esp_netif_t *esp_netif);
esp_err_t esp_netif_dhcpc_get_status(esp_netif_t *esp_netif,
                                     esp_netif_dhcp_status_t *status);

esp_err_t esp_netif_dhcps_start(esp_netif_t *esp_netif);
esp_err_t esp_netif_dhcps_stop(esp_netif_t *esp_netif);
esp_err_t esp_netif_dhcps_get_status(esp_netif_t *esp_netif,
                                     esp_netif_dhcp_status_t *status);

esp_err_t esp_netif_set_dns_info(esp_netif_t *esp_netif,
                                 esp_netif_dns_type_t type,
                                 esp_netif_dns_info_t *dns);
esp_err_t esp_netif_get_dns_info(esp_netif_t *esp_netif,
                                 esp_netif_dns_type_t type,
                                 esp_netif_dns_info_t *dns);

/*============================ L2 / METADATA =================================*/

esp_err_t esp_netif_set_mac(esp_netif_t *esp_netif, uint8_t mac[6]);
esp_err_t esp_netif_get_mac(esp_netif_t *esp_netif, uint8_t mac[6]);

esp_err_t esp_netif_set_hostname(esp_netif_t *esp_netif, const char *hostname);
esp_err_t esp_netif_get_hostname(esp_netif_t *esp_netif, const char **hostname);

bool          esp_netif_is_netif_up(esp_netif_t *esp_netif);
const char *  esp_netif_get_desc(esp_netif_t *esp_netif);
const char *  esp_netif_get_ifkey(esp_netif_t *esp_netif);
int           esp_netif_get_route_prio(esp_netif_t *esp_netif);
esp_netif_flags_t esp_netif_get_flags(esp_netif_t *esp_netif);

/*============================ ENUMERATION ===================================*/

/* Linear iteration over existing interfaces. Pass NULL to get the first
 * one; the returned pointer stays valid until esp_netif_destroy. */
esp_netif_t * esp_netif_next(esp_netif_t *esp_netif);
size_t        esp_netif_get_nr_of_ifs(void);

/* Lookup by string key (matches esp_netif_inherent_config_t::if_key). */
esp_netif_t * esp_netif_get_handle_from_ifkey(const char *ifkey);

#ifdef __cplusplus
}
#endif

#endif      // __VSF_ESPIDF_ESP_NETIF_H__
