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

#include "component/tcpip/vsf_tcpip_cfg.h"

#if VSF_USE_TCPIP == ENABLED

#define VSF_NETDRV_INHERIT_NETIF
#include "vsf.h"
#include "./vsfip_dhcp_common.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static struct vsfip_ipmac_assoc * vsfip_dhcpd_get_assoc(vsfip_netif_t *netif, uint8_t *hwaddr)
{
    vsfip_dhcpd_t *dhcpd = netif->dhcp.dhcpd;
    uint_fast8_t macsize = netif->netdrv->macaddr.size;
    vsfip_ipmac_assoc *assoc = NULL;

    for (uint_fast16_t i = 0; i < dimof(dhcpd->assoc); i++) {
        if (dhcpd->assoc[i].mac.size == macsize) {
            // allocated assoc
            if (!memcmp(dhcpd->assoc[i].mac.addr_buf, hwaddr, macsize)) {
                return &dhcpd->assoc[i];
            }
        } else if (NULL == assoc) {
            assoc = &dhcpd->assoc[i];
        }
    }

    if (assoc != NULL) {
        uint_fast32_t addr = netif->ip4addr.addr32 & ~netif->netmask.addr32;
        addr = be32_to_cpu(addr) + dhcpd->alloc_idx;
        addr = cpu_to_be32(addr);
        if (addr & netif->netmask.addr32) {
            // no more address available
            return NULL;
        }

        dhcpd->alloc_idx++;
        assoc->ip.size = netif->ip4addr.size;
        assoc->ip.addr32 = addr | (netif->netmask.addr32 & netif->ip4addr.addr32);
        assoc->mac.size = netif->netdrv->macaddr.size;
        memcpy(assoc->mac.addr_buf, hwaddr, netif->netdrv->macaddr.size);
    }
    return assoc;
}

static void vsfip_dhcpd_input(void *param, vsfip_netbuf_t *netbuf)
{
    vsfip_dhcpd_t *dhcpd = param;
    vsfip_netif_t *netif = dhcpd->netif;
    vsfip_dhcp_head_t *head;
    vsfip_ipmac_assoc *assoc;
    uint_fast8_t optlen, op;
    uint8_t *optptr;

    if (netif != netbuf->netif) {
        goto exit;
    }

    head = netbuf->app.pObj;
    if (    (head->op != DHCP_TOSERVER)
        ||  (head->magic != cpu_to_be32(DHCP_MAGIC))
        ||  (head->htype != VSFIP_ETH_HWTYPE) || (head->hlen != 6)) {
        goto exit;
    }

    optlen = vsfip_dhcp_get_opt(netbuf, DHCP_OPT_MSGTYPE, &optptr);
    if (optlen != DHCP_OPT_MSGTYPE_LEN) {
        goto exit;
    }

    switch (optptr[0]) {
    case DHCP_OP_DISCOVER:
        assoc = vsfip_dhcpd_get_assoc(netif, head->chaddr);
        if (NULL == assoc) {
            goto exit;
        }

        op = DHCP_OP_OFFER;

    common_reply:
        head->op = DHCP_TOCLIENT;
        head->secs = 0;
        head->flags = 0;
        head->yiaddr = assoc->ip.addr32;
        head->siaddr = netif->ip4addr.addr32;
        netbuf->app.nSize = sizeof(*head);

        dhcpd->optlen = 0;
        vsfip_dhcp_append_opt(netbuf, &dhcpd->optlen, DHCP_OPT_MSGTYPE,
                    DHCP_OPT_MSGTYPE_LEN, (uint8_t *)&op);
        vsfip_dhcp_append_opt(netbuf, &dhcpd->optlen, DHCP_OPT_SERVERID,
                    netif->ip4addr.size, netif->ip4addr.addr_buf);

        do {
            uint_fast32_t lease_time = cpu_to_be32(0x80000000);
            vsfip_dhcp_append_opt(netbuf, &dhcpd->optlen, DHCP_OPT_LEASE_TIME,
                    netif->ip4addr.size, (uint8_t *)&lease_time);
        } while (0);

        vsfip_dhcp_append_opt(netbuf, &dhcpd->optlen, DHCP_OPT_SUBNETMASK,
                    netif->netmask.size, netif->netmask.addr_buf);
        vsfip_dhcp_append_opt(netbuf, &dhcpd->optlen, DHCP_OPT_ROUTER,
                    netif->ip4addr.size, netif->ip4addr.addr_buf);
        vsfip_dhcp_append_opt(netbuf, &dhcpd->optlen, DHCP_OPT_DNSSERVER,
                    netif->ip4addr.size, netif->ip4addr.addr_buf);
#ifdef TCPIP_CFG_HOSTNAME
        vsfip_dhcp_append_opt(netbuf, &dhcpd->optlen, DHCP_OPT_HOSTNAME,
                    sizeof(TCPIP_CFG_HOSTNAME) - 1, (uint8_t *)TCPIP_CFG_HOSTNAME);
#endif
#ifdef VSFIP_CFG_DOMAIN
        vsfip_dhcp_append_opt(netbuf, &dhcpd->optlen, DHCP_OPT_DOMAIN,
                    sizeof(VSFIP_CFG_DOMAIN) - 1, (uint8_t *)VSFIP_CFG_DOMAIN);
#endif
        vsfip_dhcp_end_opt(netbuf, &dhcpd->optlen);

        dhcpd->sockaddr.addr.addr32 = assoc->ip.addr32;
        vsfip_netif_arp_add_assoc(  dhcpd->netif,
                                    assoc->mac.size, assoc->mac.addr_buf,
                                    assoc->ip.size, assoc->ip.addr_buf);
        vsfip_udp_async_send(dhcpd->so, &dhcpd->sockaddr, netbuf);
        dhcpd->so->remote_sockaddr.addr.addr32 = VSFIP_IPADDR_ANY;
        return;
    case DHCP_OP_REQUEST:
        assoc = vsfip_dhcpd_get_assoc(netif, head->chaddr);
        if (NULL == assoc) {
            goto exit;
        }
        optlen = vsfip_dhcp_get_opt(netbuf, DHCP_OPT_REQIP, &optptr);
        if ((4 == optlen) && (*(uint32_t *)optptr != assoc->ip.addr32)) {
            op = DHCP_OP_NAK;
        } else {
            op = DHCP_OP_ACK;
        }
        goto common_reply;
    }

exit:
    vsfip_netbuf_deref(netbuf);
}

void vsfip_dhcpd_stop(vsfip_dhcpd_t *dhcpd)
{
    if (dhcpd->netif != NULL) {
        if (dhcpd->so != NULL) {
            vsfip_close(dhcpd->so);
            dhcpd->so = NULL;
        }
        dhcpd->netif = NULL;
    }
}

// vsfip_dhcpd_start MUST be called protected with netif
vsf_err_t vsfip_dhcpd_start(vsfip_netif_t *netif, vsfip_dhcpd_t *dhcpd)
{
    ASSERT((netif != NULL) && (dhcpd != NULL));

    dhcpd->alloc_idx = 1;
    netif->dhcp.dhcpd = dhcpd;
    dhcpd->netif = netif;
    memset(&dhcpd->assoc, 0, sizeof(dhcpd->assoc));

    dhcpd->sockaddr.port = DHCP_CLIENT_PORT;
    dhcpd->sockaddr.addr.size = 4;

    dhcpd->so = vsfip_socket(AF_INET, IPPROTO_UDP);
    if (NULL == dhcpd->so) {
        goto cleanup;
    }
    vsfip_socket_cb(dhcpd->so, dhcpd, vsfip_dhcpd_input, NULL);
    if (vsfip_bind(dhcpd->so, DHCP_SERVER_PORT)) {
        goto cleanup;
    }
    vsfip_socket_set_netif(dhcpd->so, netif);
    return vsfip_listen(dhcpd->so, 0);
cleanup:
    vsfip_dhcpd_stop(dhcpd);
    return VSF_ERR_FAIL;
}

#endif      // VSF_USE_TCPIP
