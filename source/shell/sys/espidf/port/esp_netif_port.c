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
 * Port implementation for "esp_netif.h" on VSF.
 *
 * Architecture:
 *   - Each esp_netif_t owns a lwIP struct netif plus a handle to the
 *     underlying vk_netdrv_t provided by the user (usually a vk_netdrv
 *     driver such as wpcap or an on-chip Ethernet MAC).
 *   - Instances are drawn from a small static pool; ESP-IDF never exposes
 *     more than a handful of interfaces (STA / AP / ETH / PPP), so
 *     dynamic allocation buys us nothing here.
 *   - tcpip_init() is invoked once from esp_netif_init(); the caller must
 *     have configured lwipopts.h before build so the required features
 *     (LWIP_DHCP, LWIP_DNS, LWIP_NETIF_STATUS_CALLBACK) are on. Missing
 *     features are reported as ESP_ERR_NOT_SUPPORTED per-API.
 *   - Link-layer bringup flows through vk_netdrv_connect(); the existing
 *     lwip_netdrv_adapter invokes netif_add() under the tcpip core lock
 *     and populates addresses from the weak lwip_req___addr___from_user
 *     hook (default zeros -- static IP / DHCP are applied afterwards by
 *     esp_netif_set_ip_info() / esp_netif_dhcpc_start()).
 *   - IP_EVENT_*_GOT_IP / _LOST_IP are derived from lwIP's status callback
 *     and posted through the default event loop provided by esp_event.
 */

/*============================ INCLUDES ======================================*/

#include "../vsf_espidf_cfg.h"

#if VSF_USE_ESPIDF == ENABLED && VSF_ESPIDF_CFG_USE_NETIF == ENABLED

#if !(defined(VSF_USE_TCPIP) && (VSF_USE_TCPIP == ENABLED)) || \
    !(defined(VSF_USE_LWIP)  && (VSF_USE_LWIP  == ENABLED))
#   error "VSF_ESPIDF_CFG_USE_NETIF requires VSF_USE_TCPIP and VSF_USE_LWIP"
#endif

#include "esp_netif.h"
#include "esp_event.h"

#include "../vsf_espidf.h"
#include "kernel/vsf_kernel.h"

#define __VSF_NETDRV_CLASS_INHERIT_NETIF__
#include "component/tcpip/vsf_tcpip.h"

#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "lwip/dhcp.h"
#include "lwip/dns.h"
#include "lwip/ip_addr.h"

/* Forward-declared in the lwIP port; included here so the type is complete. */
extern void lwip_netif_set_netdrv(struct netif *netif, vk_netdrv_t *netdrv);

#include <string.h>

/*============================ MACROS ========================================*/

#ifndef VSF_ESPIDF_CFG_NETIF_MAX_INSTANCES
#   define VSF_ESPIDF_CFG_NETIF_MAX_INSTANCES    4
#endif

/*============================ TYPES =========================================*/

struct esp_netif_obj {
    /* Pool slot management. */
    bool                            in_use;
    bool                            is_started;

    /* User configuration (cfg.base is copied by value to decouple the
     * esp_netif lifetime from the cfg object on the caller stack). */
    esp_netif_inherent_config_t     base;
    esp_netif_ip_info_t             static_ip;  /*!< resolved from base.ip_info */
    bool                            has_static_ip;

    /* Link-layer binding. Exactly one of { netdrv, driver_ifcfg.handle }
     * is populated at runtime; attach_netdrv() takes the first path,
     * esp_netif_attach() the second. */
    vk_netdrv_t                    *netdrv;
    esp_netif_driver_ifconfig_t     driver_ifcfg;

    /* lwIP-side state. lwip_netif lives inside the slot so that its
     * storage outlives all internal lwIP bookkeeping. */
    struct netif                    lwip_netif;
    esp_netif_dhcp_status_t         dhcpc_status;
    esp_netif_dhcp_status_t         dhcps_status;
    bool                            got_ip_posted;
    char                            hostname[32];
};

/*============================ GLOBAL VARIABLES ==============================*/

ESP_EVENT_DEFINE_BASE(IP_EVENT);

/*============================ LOCAL VARIABLES ===============================*/

static struct {
    bool                            is_inited;
    struct esp_netif_obj            pool[VSF_ESPIDF_CFG_NETIF_MAX_INSTANCES];
} __vsf_espidf_netif = { 0 };

/*============================ HELPERS =======================================*/

static struct esp_netif_obj * __alloc_slot(void)
{
    for (size_t i = 0; i < VSF_ESPIDF_CFG_NETIF_MAX_INSTANCES; i++) {
        if (!__vsf_espidf_netif.pool[i].in_use) {
            struct esp_netif_obj *o = &__vsf_espidf_netif.pool[i];
            memset(o, 0, sizeof(*o));
            o->in_use = true;
            o->dhcpc_status = ESP_NETIF_DHCP_INIT;
            o->dhcps_status = ESP_NETIF_DHCP_INIT;
            return o;
        }
    }
    return NULL;
}

static void __free_slot(struct esp_netif_obj *o)
{
    memset(o, 0, sizeof(*o));
}

/* Copy the flat uint32 network-order address across the two representations. */
static inline void __ip4_to_lwip(const esp_ip4_addr_t *src, ip_addr_t *dst)
{
    memset(dst, 0, sizeof(*dst));
#if LWIP_IPV4 && LWIP_IPV6
    dst->type = IPADDR_TYPE_V4;
    dst->u_addr.ip4.addr = src->addr;
#else
    dst->addr = src->addr;
#endif
}

static inline uint32_t __lwip_ip4_raw(const ip_addr_t *src)
{
#if LWIP_IPV4 && LWIP_IPV6
    return src->u_addr.ip4.addr;
#else
    return src->addr;
#endif
}

/* Translate a lwIP struct netif snapshot into the ESP-IDF IP info block. */
static void __capture_ip_info(struct netif *n, esp_netif_ip_info_t *out)
{
    memset(out, 0, sizeof(*out));
#if LWIP_IPV4 && LWIP_IPV6
    out->ip.addr      = netif_ip4_addr(n)->addr;
    out->netmask.addr = netif_ip4_netmask(n)->addr;
    out->gw.addr      = netif_ip4_gw(n)->addr;
#else
    out->ip.addr      = n->ip_addr.addr;
    out->netmask.addr = n->netmask.addr;
    out->gw.addr      = n->gw.addr;
#endif
}

/* lwIP status-callback trampoline -- posts IP_EVENT_*_GOT_IP / _LOST_IP
 * through the default event loop. Fires on netif_set_up / netif_set_down
 * and on DHCP state changes; we filter by the UP flag and by whether an
 * address is actually assigned. */
#if LWIP_NETIF_STATUS_CALLBACK
static void __status_cb(struct netif *n)
{
    /* Walk the pool to find the matching esp_netif slot. */
    struct esp_netif_obj *self = NULL;
    for (size_t i = 0; i < VSF_ESPIDF_CFG_NETIF_MAX_INSTANCES; i++) {
        if (__vsf_espidf_netif.pool[i].in_use &&
            &__vsf_espidf_netif.pool[i].lwip_netif == n) {
            self = &__vsf_espidf_netif.pool[i];
            break;
        }
    }
    if (self == NULL) {
        return;
    }

    bool has_addr = (netif_is_up(n) != 0) &&
#if LWIP_IPV4 && LWIP_IPV6
                    (netif_ip4_addr(n)->addr != 0);
#else
                    (n->ip_addr.addr != 0);
#endif

    if (has_addr && !self->got_ip_posted) {
        ip_event_got_ip_t evt = { 0 };
        evt.esp_netif = self;
        __capture_ip_info(n, &evt.ip_info);
        evt.ip_changed = true;
        self->got_ip_posted = true;
        (void)esp_event_post(IP_EVENT, (int32_t)self->base.get_ip_event,
                             &evt, sizeof(evt), 0);
    } else if (!has_addr && self->got_ip_posted) {
        self->got_ip_posted = false;
        if (self->base.lost_ip_event != 0) {
            (void)esp_event_post(IP_EVENT, (int32_t)self->base.lost_ip_event,
                                 NULL, 0, 0);
        }
    }
}
#endif /* LWIP_NETIF_STATUS_CALLBACK */

/*============================ LIFECYCLE =====================================*/

esp_err_t esp_netif_init(void)
{
    if (__vsf_espidf_netif.is_inited) {
        return ESP_OK;
    }
    tcpip_init(NULL, NULL);
    __vsf_espidf_netif.is_inited = true;
    return ESP_OK;
}

esp_err_t esp_netif_deinit(void)
{
    /* lwIP has no matching tcpip_deinit; this is best-effort. */
    return ESP_OK;
}

void vsf_espidf_netif_init(void)
{
    (void)esp_netif_init();
}

esp_netif_t * esp_netif_new(const esp_netif_config_t *cfg)
{
    if ((cfg == NULL) || (cfg->base == NULL)) {
        return NULL;
    }
    (void)esp_netif_init();
    struct esp_netif_obj *o = __alloc_slot();
    if (o == NULL) {
        return NULL;
    }
    o->base = *cfg->base;
    if (cfg->base->ip_info != NULL) {
        o->static_ip     = *cfg->base->ip_info;
        o->has_static_ip = true;
    }
    return o;
}

void esp_netif_destroy(esp_netif_t *netif)
{
    if (netif == NULL || !netif->in_use) {
        return;
    }
    if (netif->is_started) {
        (void)esp_netif_action_stop(netif, NULL, 0, NULL);
    }
    __free_slot(netif);
}

/*============================ DRIVER ATTACH =================================*/

esp_err_t esp_netif_attach(esp_netif_t *netif,
                           esp_netif_iodriver_handle driver_handle)
{
    if (netif == NULL || driver_handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_netif_driver_base_t *base = (esp_netif_driver_base_t *)driver_handle;
    base->netif = netif;
    if (base->post_attach != NULL) {
        return base->post_attach(netif, driver_handle);
    }
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_netif_set_driver_config(esp_netif_t *netif,
                                      const esp_netif_driver_ifconfig_t *cfg)
{
    if (netif == NULL || cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    netif->driver_ifcfg = *cfg;
    return ESP_OK;
}

esp_err_t esp_netif_attach_netdrv(esp_netif_t *netif, struct vk_netdrv *netdrv)
{
    if (netif == NULL || netdrv == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    netif->netdrv = (vk_netdrv_t *)netdrv;
    /* Apply MAC override from the inherent config, if provided. */
    static const uint8_t __zero_mac[6] = { 0 };
    if (memcmp(netif->base.mac, __zero_mac, 6) != 0) {
        memcpy(((vk_netdrv_t *)netdrv)->macaddr.addr_buf, netif->base.mac, 6);
        ((vk_netdrv_t *)netdrv)->macaddr.size = 6;
    }
    /* Bind the lwIP netif <-> netdrv. After this the adapter op-set is
     * populated and vk_netdrv_prepare / _connect can safely run. */
    lwip_netif_set_netdrv(&netif->lwip_netif, (vk_netdrv_t *)netdrv);
    return ESP_OK;
}

/*============================ ACTIONS =======================================*/

esp_err_t esp_netif_action_start(esp_netif_t *netif,
                                 esp_event_base_t base, int32_t id, void *data)
{
    (void)base; (void)id; (void)data;
    if (netif == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (netif->is_started) {
        return ESP_OK;
    }
    if (netif->netdrv == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    /* Adapter.op was populated by esp_netif_attach_netdrv. The weak
     * vsf_pnp_on_netdrv_prepare hook is a no-op; calling prepare is
     * still required to assert the binding and run any user hook. */
    vk_netdrv_prepare(netif->netdrv);
    vsf_err_t rc = vk_netdrv_connect(netif->netdrv);
    if (rc != VSF_ERR_NONE) {
        return ESP_FAIL;
    }

    struct netif *n = &netif->lwip_netif;

    LOCK_TCPIP_CORE();
#if LWIP_NETIF_HOSTNAME
    if (netif->hostname[0] != '\0') {
        n->hostname = netif->hostname;
    }
#endif
#if LWIP_NETIF_STATUS_CALLBACK
    netif_set_status_callback(n, __status_cb);
#endif
    netif_set_link_up(n);

    /* Apply static IP if provided and DHCP client is not requested. */
    if (netif->has_static_ip && !(netif->base.flags & ESP_NETIF_DHCP_CLIENT)) {
        ip4_addr_t ip4, mask4, gw4;
        ip4.addr   = netif->static_ip.ip.addr;
        mask4.addr = netif->static_ip.netmask.addr;
        gw4.addr   = netif->static_ip.gw.addr;
#if LWIP_IPV4 && LWIP_IPV6
        netif_set_addr(n, &ip4, &mask4, &gw4);
#else
        netif_set_addr(n, &ip4, &mask4, &gw4);
#endif
    }

    /* netif must be UP before dhcp_start / static addr takes effect and
     * before packets can be transmitted. ESP_NETIF_FLAG_AUTOUP in this
     * port controls default-route election, not the UP state itself. */
    netif_set_up(n);

    /* Default route election: first netif to start takes it; an AUTOUP
     * flagged netif overrides a previously elected non-AUTOUP default.
     * esp_netif_set_default_netif() (future) may override manually. */
    {
        struct netif *cur = netif_default;
        bool take = (cur == NULL) || (netif->base.flags & ESP_NETIF_FLAG_AUTOUP);
        if (take) {
            netif_set_default(n);
        }
    }
    UNLOCK_TCPIP_CORE();

    /* DHCP client starts after the netif is up. */
    if (netif->base.flags & ESP_NETIF_DHCP_CLIENT) {
#if LWIP_DHCP
        LOCK_TCPIP_CORE();
        (void)dhcp_start(n);
        UNLOCK_TCPIP_CORE();
        netif->dhcpc_status = ESP_NETIF_DHCP_STARTED;
#endif
    }

    netif->is_started = true;
    return ESP_OK;
}

esp_err_t esp_netif_action_stop(esp_netif_t *netif,
                                esp_event_base_t base, int32_t id, void *data)
{
    (void)base; (void)id; (void)data;
    if (netif == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!netif->is_started) {
        return ESP_OK;
    }
    struct netif *n = &netif->lwip_netif;

#if LWIP_DHCP
    if (netif->dhcpc_status == ESP_NETIF_DHCP_STARTED) {
        LOCK_TCPIP_CORE();
        dhcp_stop(n);
        UNLOCK_TCPIP_CORE();
        netif->dhcpc_status = ESP_NETIF_DHCP_STOPPED;
    }
#endif
    LOCK_TCPIP_CORE();
    netif_set_down(n);
    netif_set_link_down(n);
#if LWIP_NETIF_STATUS_CALLBACK
    netif_set_status_callback(n, NULL);
#endif
    UNLOCK_TCPIP_CORE();

    vk_netdrv_disconnect(netif->netdrv);   /* adapter removes the lwIP netif */
    netif->is_started = false;
    netif->got_ip_posted = false;
    return ESP_OK;
}

esp_err_t esp_netif_action_connected(esp_netif_t *netif,
                                     esp_event_base_t b, int32_t id, void *d)
{
    return esp_netif_action_start(netif, b, id, d);
}

esp_err_t esp_netif_action_disconnected(esp_netif_t *netif,
                                        esp_event_base_t b, int32_t id, void *d)
{
    return esp_netif_action_stop(netif, b, id, d);
}

esp_err_t esp_netif_action_got_ip(esp_netif_t *netif,
                                  esp_event_base_t b, int32_t id, void *d)
{
    (void)netif; (void)b; (void)id; (void)d;
    return ESP_OK;
}

/*============================ IP CONFIGURATION ==============================*/

esp_err_t esp_netif_set_ip_info(esp_netif_t *netif,
                                const esp_netif_ip_info_t *ip_info)
{
    if (netif == NULL || ip_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    netif->static_ip     = *ip_info;
    netif->has_static_ip = true;

    if (netif->is_started) {
        ip4_addr_t ip4, mask4, gw4;
        ip4.addr   = ip_info->ip.addr;
        mask4.addr = ip_info->netmask.addr;
        gw4.addr   = ip_info->gw.addr;
        LOCK_TCPIP_CORE();
        netif_set_addr(&netif->lwip_netif, &ip4, &mask4, &gw4);
        UNLOCK_TCPIP_CORE();
    }
    return ESP_OK;
}

esp_err_t esp_netif_get_ip_info(esp_netif_t *netif, esp_netif_ip_info_t *out)
{
    if (netif == NULL || out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    __capture_ip_info(&netif->lwip_netif, out);
    return ESP_OK;
}

esp_err_t esp_netif_dhcpc_start(esp_netif_t *netif)
{
#if LWIP_DHCP
    if (netif == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!netif->is_started) {
        /* Latch intent; the start action will pick it up. */
        netif->base.flags |= ESP_NETIF_DHCP_CLIENT;
        netif->dhcpc_status = ESP_NETIF_DHCP_INIT;
        return ESP_OK;
    }
    LOCK_TCPIP_CORE();
    err_t rc = dhcp_start(&netif->lwip_netif);
    UNLOCK_TCPIP_CORE();
    if (rc != ERR_OK) {
        return ESP_FAIL;
    }
    netif->dhcpc_status = ESP_NETIF_DHCP_STARTED;
    return ESP_OK;
#else
    (void)netif;
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t esp_netif_dhcpc_stop(esp_netif_t *netif)
{
#if LWIP_DHCP
    if (netif == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (netif->is_started && (netif->dhcpc_status == ESP_NETIF_DHCP_STARTED)) {
        LOCK_TCPIP_CORE();
        dhcp_stop(&netif->lwip_netif);
        UNLOCK_TCPIP_CORE();
    }
    netif->dhcpc_status = ESP_NETIF_DHCP_STOPPED;
    netif->base.flags &= ~ESP_NETIF_DHCP_CLIENT;
    return ESP_OK;
#else
    (void)netif;
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t esp_netif_dhcpc_get_status(esp_netif_t *netif,
                                     esp_netif_dhcp_status_t *status)
{
    if (netif == NULL || status == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    *status = netif->dhcpc_status;
    return ESP_OK;
}

esp_err_t esp_netif_dhcps_start(esp_netif_t *netif)
{
    (void)netif;
    return ESP_ERR_NOT_SUPPORTED;   /* TODO: raw lwIP has no DHCPS */
}
esp_err_t esp_netif_dhcps_stop(esp_netif_t *netif)
{
    (void)netif;
    return ESP_ERR_NOT_SUPPORTED;
}
esp_err_t esp_netif_dhcps_get_status(esp_netif_t *netif,
                                     esp_netif_dhcp_status_t *status)
{
    if (netif == NULL || status == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    *status = netif->dhcps_status;
    return ESP_OK;
}

esp_err_t esp_netif_set_dns_info(esp_netif_t *netif,
                                 esp_netif_dns_type_t type,
                                 esp_netif_dns_info_t *dns)
{
#if LWIP_DNS
    if (netif == NULL || dns == NULL || type >= ESP_NETIF_DNS_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    ip_addr_t srv;
    __ip4_to_lwip(&dns->ip.u_addr.ip4, &srv);
    LOCK_TCPIP_CORE();
    dns_setserver((u8_t)type, &srv);
    UNLOCK_TCPIP_CORE();
    return ESP_OK;
#else
    (void)netif; (void)type; (void)dns;
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t esp_netif_get_dns_info(esp_netif_t *netif,
                                 esp_netif_dns_type_t type,
                                 esp_netif_dns_info_t *dns)
{
#if LWIP_DNS
    if (netif == NULL || dns == NULL || type >= ESP_NETIF_DNS_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    LOCK_TCPIP_CORE();
    const ip_addr_t *srv = dns_getserver((u8_t)type);
    UNLOCK_TCPIP_CORE();
    memset(dns, 0, sizeof(*dns));
    dns->ip.type          = ESP_IPADDR_TYPE_V4;
    dns->ip.u_addr.ip4.addr = __lwip_ip4_raw(srv);
    return ESP_OK;
#else
    (void)netif; (void)type; (void)dns;
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

/*============================ L2 / METADATA =================================*/

esp_err_t esp_netif_set_mac(esp_netif_t *netif, uint8_t mac[6])
{
    if (netif == NULL || mac == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    memcpy(netif->base.mac, mac, 6);
    if (netif->netdrv != NULL) {
        memcpy(netif->netdrv->macaddr.addr_buf, mac, 6);
        netif->netdrv->macaddr.size = 6;
    }
    if (netif->is_started) {
        memcpy(netif->lwip_netif.hwaddr, mac, 6);
        netif->lwip_netif.hwaddr_len = 6;
    }
    return ESP_OK;
}

esp_err_t esp_netif_get_mac(esp_netif_t *netif, uint8_t mac[6])
{
    if (netif == NULL || mac == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (netif->netdrv != NULL && netif->netdrv->macaddr.size == 6) {
        memcpy(mac, netif->netdrv->macaddr.addr_buf, 6);
    } else {
        memcpy(mac, netif->base.mac, 6);
    }
    return ESP_OK;
}

esp_err_t esp_netif_set_hostname(esp_netif_t *netif, const char *hostname)
{
#if LWIP_NETIF_HOSTNAME
    if (netif == NULL || hostname == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    strncpy(netif->hostname, hostname, sizeof(netif->hostname) - 1);
    netif->hostname[sizeof(netif->hostname) - 1] = '\0';
    if (netif->is_started) {
        LOCK_TCPIP_CORE();
        netif->lwip_netif.hostname = netif->hostname;
        UNLOCK_TCPIP_CORE();
    }
    return ESP_OK;
#else
    (void)netif; (void)hostname;
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t esp_netif_get_hostname(esp_netif_t *netif, const char **hostname)
{
#if LWIP_NETIF_HOSTNAME
    if (netif == NULL || hostname == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    *hostname = netif->hostname[0] ? netif->hostname : NULL;
    return ESP_OK;
#else
    (void)netif; (void)hostname;
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

bool esp_netif_is_netif_up(esp_netif_t *netif)
{
    if (netif == NULL || !netif->is_started) {
        return false;
    }
    return netif_is_up(&netif->lwip_netif) != 0;
}

const char * esp_netif_get_desc(esp_netif_t *netif)
{
    return (netif != NULL) ? netif->base.if_desc : NULL;
}

const char * esp_netif_get_ifkey(esp_netif_t *netif)
{
    return (netif != NULL) ? netif->base.if_key : NULL;
}

int esp_netif_get_route_prio(esp_netif_t *netif)
{
    return (netif != NULL) ? netif->base.route_prio : 0;
}

esp_netif_flags_t esp_netif_get_flags(esp_netif_t *netif)
{
    return (netif != NULL) ? netif->base.flags : (esp_netif_flags_t)0;
}

/*============================ ENUMERATION ===================================*/

esp_netif_t * esp_netif_next(esp_netif_t *prev)
{
    size_t start = 0;
    if (prev != NULL) {
        start = (size_t)(prev - __vsf_espidf_netif.pool) + 1;
    }
    for (size_t i = start; i < VSF_ESPIDF_CFG_NETIF_MAX_INSTANCES; i++) {
        if (__vsf_espidf_netif.pool[i].in_use) {
            return &__vsf_espidf_netif.pool[i];
        }
    }
    return NULL;
}

size_t esp_netif_get_nr_of_ifs(void)
{
    size_t n = 0;
    for (size_t i = 0; i < VSF_ESPIDF_CFG_NETIF_MAX_INSTANCES; i++) {
        if (__vsf_espidf_netif.pool[i].in_use) {
            n++;
        }
    }
    return n;
}

esp_netif_t * esp_netif_get_handle_from_ifkey(const char *ifkey)
{
    if (ifkey == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < VSF_ESPIDF_CFG_NETIF_MAX_INSTANCES; i++) {
        struct esp_netif_obj *o = &__vsf_espidf_netif.pool[i];
        if (o->in_use && o->base.if_key != NULL &&
            strcmp(o->base.if_key, ifkey) == 0) {
            return o;
        }
    }
    return NULL;
}

#endif      // VSF_USE_ESPIDF && VSF_ESPIDF_CFG_USE_NETIF
