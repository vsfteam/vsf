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

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE != ENABLED
#   error "VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE is required"
#endif

#ifndef VSFIP_CFG_DHCPC_TIMEOUT
#   define VSFIP_CFG_DHCPC_TIMEOUT          2000
#endif
#ifndef VSFIP_DHCPC_RETRY_CNT
#   define VSFIP_DHCPC_RETRY_CNT            10
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum vsfip_dhcpc_evt_t {
    VSFIP_DHCP_EVT_READY            = VSF_EVT_USER + 0,
    VSFIP_DHCP_EVT_SEND_DISCOVER    = VSF_EVT_USER + 1,
    VSFIP_DHCP_EVT_SEND_REQUEST     = VSF_EVT_USER + 2,
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

SECTION(".text.vsf.kernel.eda")
vsf_err_t __vsf_eda_fini(vsf_eda_t *pthis);

/*============================ IMPLEMENTATION ================================*/

static vsf_err_t vsfip_dhcpc_init_msg(vsfip_dhcpc_t *dhcpc, uint_fast8_t op)
{
    vsfip_netif_t *netif = dhcpc->netif;
    vsfip_netbuf_t *netbuf;
    vsfip_dhcp_head_t *head;

    dhcpc->outbuffer = VSFIP_UDPBUF_GET(sizeof(vsfip_dhcp_head_t));
    if (NULL == dhcpc->outbuffer) {
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }

    netbuf = dhcpc->outbuffer;
    vsfip_netbuf_set_netif(netbuf, dhcpc->netif);

    head = netbuf->app.pObj;
    memset(head, 0, sizeof(vsfip_dhcp_head_t));
    head->op = DHCP_TOSERVER;
    head->htype = netif->netdrv->hwtype;
    head->hlen = netif->netdrv->macaddr.size;
    head->xid = dhcpc->xid;
    // shift right 10-bit for div 1000
    head->secs = 0;
    memcpy(head->chaddr, netif->netdrv->macaddr.addr_buf, netif->netdrv->macaddr.size);
    head->magic = cpu_to_be32(DHCP_MAGIC);
    dhcpc->optlen = 0;
    vsfip_dhcp_append_opt(netbuf, &dhcpc->optlen, DHCP_OPT_MSGTYPE,
                            DHCP_OPT_MSGTYPE_LEN, (uint8_t *)&op);

    do {
        uint_fast16_t tmp16 = cpu_to_be16(576);
        // RFC 2132 9.10, message size MUST be >= 576
        vsfip_dhcp_append_opt(netbuf, &dhcpc->optlen, DHCP_OPT_MAXMSGSIZE,
                            DHCP_OPT_MAXMSGSIZE_LEN, (uint8_t *)&tmp16);
    } while (0);

    do {
        uint8_t requestlist[] = {DHCP_OPT_SUBNETMASK,
            DHCP_OPT_ROUTER, DHCP_OPT_DNSSERVER, DHCP_OPT_BROADCAST};
        vsfip_dhcp_append_opt(netbuf, &dhcpc->optlen, DHCP_OPT_PARAMLIST,
                                sizeof(requestlist), requestlist);
    } while (0);

#ifdef TCPIP_CFG_HOSTNAME
    vsfip_dhcp_append_opt(netbuf, &dhcpc->optlen, DHCP_OPT_HOSTNAME,
            sizeof(TCPIP_CFG_HOSTNAME) - 1, (uint8_t *)TCPIP_CFG_HOSTNAME);
#endif

    return VSF_ERR_NONE;
}

static void vsfip_dhcpc_input(void *param, vsfip_netbuf_t *netbuf)
{
    vsfip_dhcpc_t *dhcpc = param;
    vsfip_netif_t *netif = dhcpc->netif;
    vsfip_dhcp_head_t *head;
    uint_fast8_t optlen;
    uint8_t *optptr;

    head = netbuf->app.pObj;
    if (    (head->op != DHCP_TOCLIENT)
        ||  (head->magic != cpu_to_be32(DHCP_MAGIC))
        ||  memcmp(head->chaddr, netif->netdrv->macaddr.addr_buf, netif->netdrv->macaddr.size)
        ||  (head->xid != dhcpc->xid)) {
        goto exit;
    }

    optlen = vsfip_dhcp_get_opt(netbuf, DHCP_OPT_MSGTYPE, &optptr);
    if (optlen != DHCP_OPT_MSGTYPE_LEN) {
        goto exit;
    }

    switch (optptr[0]) {
    case DHCP_OP_OFFER:
        dhcpc->ipaddr.size = 4;
        dhcpc->ipaddr.addr32 = head->yiaddr;
        vsfip_netbuf_deref(netbuf);
        vsf_eda_post_evt(&dhcpc->teda.use_as__vsf_eda_t, VSFIP_DHCP_EVT_SEND_REQUEST);
        break;
    case DHCP_OP_ACK:
        optlen = vsfip_dhcp_get_opt(netbuf, DHCP_OPT_LEASE_TIME, &optptr);
        dhcpc->leasetime = (4 == optlen) ? get_unaligned_be32(optptr) : 0;
        optlen = vsfip_dhcp_get_opt(netbuf, DHCP_OPT_RENEW_TIME, &optptr);
        dhcpc->renew_time = (4 == optlen) ? get_unaligned_be32(optptr) : 0;
        optlen = vsfip_dhcp_get_opt(netbuf, DHCP_OPT_REBINDING_TIME, &optptr);
        dhcpc->rebinding_time = (4 == optlen) ? get_unaligned_be32(optptr) : 0;
        optlen = vsfip_dhcp_get_opt(netbuf, DHCP_OPT_SUBNETMASK, &optptr);
        dhcpc->netmask.size = optlen;
        dhcpc->netmask.addr32 = (4 == optlen) ? *(uint32_t *)optptr : 0;
        optlen = vsfip_dhcp_get_opt(netbuf, DHCP_OPT_ROUTER, &optptr);
        dhcpc->gw.size = optlen;
        dhcpc->gw.addr32 = (4 == optlen) ? *(uint32_t *)optptr : 0;
        optlen = vsfip_dhcp_get_opt(netbuf, DHCP_OPT_DNSSERVER, &optptr);
        dhcpc->dns[0].size = dhcpc->dns[1].size = 0;
        if (optlen >= 4) {
            dhcpc->dns[0].size = 4;
            dhcpc->dns[0].addr32 = *(uint_fast32_t *)optptr;
            if (optlen >= 8) {
                dhcpc->dns[1].size = 4;
                dhcpc->dns[1].addr32 = *(uint_fast32_t *)(optptr + 4);
            }
        }

        vsfip_netbuf_deref(netbuf);
        vsf_eda_post_evt(&dhcpc->teda.use_as__vsf_eda_t, VSFIP_DHCP_EVT_READY);
        break;
    case DHCP_OP_NAK:
        vsfip_netbuf_deref(netbuf);
        dhcpc->ipaddr.size = 0;
        break;
    default:
    exit:
        vsfip_netbuf_deref(netbuf);
    }
}

static void vsfip_dhcpc_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vsfip_dhcpc_t *dhcpc = container_of(eda, vsfip_dhcpc_t, teda);
    vsfip_netif_t *netif = dhcpc->netif;

    switch (evt) {
    case VSF_EVT_INIT:
        dhcpc->ready = true;
        dhcpc->retry = 0;

    retry:
        dhcpc->so = vsfip_socket(AF_INET, IPPROTO_UDP);
        if (NULL == dhcpc->so) {
            goto cleanup;
        }
        vsfip_socket_cb(dhcpc->so, dhcpc, vsfip_dhcpc_input, NULL);
        if (vsfip_bind(dhcpc->so, DHCP_CLIENT_PORT)) {
            goto cleanup;
        }
        vsfip_listen(dhcpc->so, 0);

        // if address already allocated, do resume, send request again
        if (dhcpc->ipaddr.size != 0) {
            dhcpc->netif->ip4addr = dhcpc->ipaddr;
            goto dhcp_request;
        }

        // fall through
    case VSFIP_DHCP_EVT_SEND_DISCOVER:
        memset(&netif->ip4addr, 0, sizeof(netif->ip4addr));
        dhcpc->ipaddr.size = 0;
        if (vsfip_dhcpc_init_msg(dhcpc, (uint8_t)DHCP_OP_DISCOVER) < 0) {
            goto cleanup;
        }
        vsfip_dhcp_end_opt(dhcpc->outbuffer, &dhcpc->optlen);
        dhcpc->sockaddr.addr.addr32 = 0xFFFFFFFF;
        vsfip_udp_async_send(dhcpc->so, &dhcpc->sockaddr, dhcpc->outbuffer);
        dhcpc->so->remote_sockaddr.addr.addr32 = VSFIP_IPADDR_ANY;

        vsf_teda_set_timer_ms(VSFIP_CFG_DHCPC_TIMEOUT + VSFIP_ARP_MAXDELAY);
        break;
    case VSFIP_DHCP_EVT_SEND_REQUEST:
    dhcp_request:
        vsf_teda_cancel_timer(NULL);
        if (vsfip_dhcpc_init_msg(dhcpc, (uint8_t)DHCP_OP_REQUEST) < 0) {
            goto cleanup;
        }
        vsfip_dhcp_append_opt(dhcpc->outbuffer, &dhcpc->optlen, DHCP_OPT_REQIP,
                dhcpc->ipaddr.size, dhcpc->ipaddr.addr_buf);
        vsfip_dhcp_end_opt(dhcpc->outbuffer, &dhcpc->optlen);
        dhcpc->sockaddr.addr.addr32 = 0xFFFFFFFF;
        vsfip_udp_async_send(dhcpc->so, &dhcpc->sockaddr, dhcpc->outbuffer);
        dhcpc->so->remote_sockaddr.addr.addr32 = VSFIP_IPADDR_ANY;

        vsf_teda_set_timer_ms(VSFIP_CFG_DHCPC_TIMEOUT + VSFIP_ARP_MAXDELAY);
        break;
    case VSFIP_DHCP_EVT_READY:
        vsf_teda_cancel_timer(NULL);

        // update netif->ipaddr
        dhcpc->ready = 1;
        netif->ip4addr = dhcpc->ipaddr;
        netif->gateway = dhcpc->gw;
        netif->netmask = dhcpc->netmask;
        netif->dns[0] = dhcpc->dns[0];
        netif->dns[1] = dhcpc->dns[1];

        // timer out for resume
//      vsf_teda_set_timer_ms(2000);
        goto cleanup;
        break;
    case VSF_EVT_TIMER:
        // maybe need to resume, set the ready to false
        dhcpc->ready = false;
    cleanup:
        if (dhcpc->so != NULL) {
            vsfip_close(dhcpc->so);
            dhcpc->so = NULL;
        }
        if (!dhcpc->ready) {
            if (++dhcpc->retry < VSFIP_DHCPC_RETRY_CNT) {
                goto retry;
            } else {
                vsfip_dhcpc_stop(dhcpc);
            }
        }
        break;
    }
}

void vsfip_dhcpc_stop(vsfip_dhcpc_t *dhcpc)
{
    if (dhcpc->netif != NULL) {
        if (dhcpc->so != NULL) {
            vsfip_close(dhcpc->so);
            dhcpc->so = NULL;
        }
        __vsf_eda_fini(&dhcpc->teda.use_as__vsf_eda_t);
    }
}

static void vsfip_dhcpc_on_eda_terminate(vsf_eda_t *eda)
{
    vsfip_dhcpc_t *dhcpc = container_of(eda, vsfip_dhcpc_t, teda);
#ifdef VSFIP_TRACE_NETIF
    vsf_trace(VSF_TRACE_DEBUG, "vsfip_dhcpc deref");
#endif
    vsfip_netif_deref(dhcpc->netif);
    dhcpc->netif = NULL;
}

// vsfip_dhcpc_start MUST be called protected with netif
vsf_err_t vsfip_dhcpc_start(vsfip_netif_t *netif, vsfip_dhcpc_t *dhcpc)
{
    ASSERT((netif != NULL) && (dhcpc != NULL));

    netif->dhcp.dhcpc = dhcpc;
    dhcpc->netif = netif;
#ifdef VSFIP_TRACE_NETIF
    vsf_trace(VSF_TRACE_DEBUG, "vsfip_dhcpc ref");
#endif
    vsfip_netif_ref(netif);

    dhcpc->xid = 0xABCDEF67;
    dhcpc->sockaddr.port = DHCP_SERVER_PORT;
    dhcpc->sockaddr.addr.size = 4;

    dhcpc->teda.evthandler = vsfip_dhcpc_evthandler;
    dhcpc->teda.on_terminate = vsfip_dhcpc_on_eda_terminate;
    vsf_err_t err = vsf_teda_init(&dhcpc->teda, vsf_priority_inherit, false);
    if (VSF_ERR_NONE != err) {
        vsfip_dhcpc_on_eda_terminate(&dhcpc->teda.use_as__vsf_eda_t);
    }
    return err;
}

#endif      // VSF_USE_TCPIP
