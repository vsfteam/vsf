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

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"
#include "netif/ppp_oe.h"

#include "lwip/tcpip.h"

/*============================ MACROS ========================================*/

#ifndef TCPIP_CFG_HOSTNAME
#   define TCPIP_CFG_HOSTNAME           "lwip"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

WEAK void lwip_request__addr__from_user(ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gateway)
{
}

// ethernetif implementation
static err_t ethernetif_low_level_output(struct netif *netif, struct pbuf *p)
{
    vsf_netdrv_t *netdrv = netif->state;
  
#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

    pbuf_ref(p);

    vsf_protect_t orig = vsf_protect_sched();
    if (!vsf_netdrv_can_output(netdrv)) {
        vsf_eda_t *eda = vsf_eda_get_cur();
        ASSERT((netdrv->eda_pending != NULL) && vsf_eda_is_stack_owner(eda));
        netdrv->eda_pending = eda;
        vsf_unprotect_sched(orig);
        vsf_thread_wfe(VSF_EVT_USER);
    } else {
        vsf_unprotect_sched(orig);
    }

    vsf_netdrv_output(netdrv, p);
    return ERR_OK;
}

err_t ethernetif_init(struct netif *netif)
{
    vsf_netdrv_t *netdrv;

    ASSERT(netif != NULL);
    netdrv = netif->state;
    ASSERT((netdrv != NULL) && (netdrv->macaddr.size <= NETIF_MAX_HWADDR_LEN));

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = TCPIP_CFG_HOSTNAME;
#endif /* LWIP_NETIF_HOSTNAME */

    /*
     * Initialize the snmp variables and counters inside the struct netif.
     * The last argument should be replaced with your link speed, in units
     * of bits per second.
     */
    NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

    netif->name[0] = 'X';
    netif->name[1] = 'X';

    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) */
    netif->output = etharp_output;
    netif->linkoutput = ethernetif_low_level_output;

    /* initialize the hardware */
    vsf_netdrv_init(netdrv);

    netif->hwaddr_len = netdrv->macaddr.size;
    memcpy(netif->hwaddr, netdrv->macaddr.addr_buf, netif->hwaddr_len);
    netif->mtu = netdrv->mtu;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

    return ERR_OK;
}

// adapter
static vsf_err_t lwip_netdrv_adapter_on_connect(void *netif)
{
    struct netif *lwip_netif = netif;
    ip_addr_t ipaddr = { .addr = 0 }, netmask = { .addr = 0 }, gateway = { .addr = 0 };

    lwip_request__addr__from_user(&ipaddr, &netmask, &gateway);
    netif_add(lwip_netif, &ipaddr, &netmask, &gateway, lwip_netif->state,
              ethernetif_init, tcpip_input);
    netif_set_default(lwip_netif);
    return VSF_ERR_NONE;
}

static void lwip_netdrv_adapter_on_disconnect(void *netif)
{
    netif_remove((struct netif *)netif);
}

static void lwip_netdrv_adapter_on_outputted(void *netif, void *netbuf, vsf_err_t err)
{
    struct netif *lwip_netif = netif;
    vsf_netdrv_t *netdrv = lwip_netif->state;

#if ETH_PAD_SIZE
    pbuf_header((struct pbuf *)netbuf, ETH_PAD_SIZE); /* reclaim the padding word */
#endif
    pbuf_free((struct pbuf *)netbuf);
    LINK_STATS_INC(link.xmit);

    vsf_eda_t *eda = netdrv->eda_pending;
    if (eda != NULL) {
        netdrv->eda_pending = NULL;
        vsf_eda_post_evt(eda, VSF_EVT_USER);
        return;
    }
}

static void lwip_netdrv_adapter_on_inputted(void *netif, void *netbuf, uint_fast32_t size)
{
    struct netif *lwip_netif = netif;
    struct pbuf *pbuf = netbuf;
    struct eth_hdr *ethhdr;

#if ETH_PAD_SIZE
    pbuf_header(pbuf, ETH_PAD_SIZE); /* reclaim the padding word */
#endif
    ethhdr = pbuf->payload;
    switch (htons(ethhdr->type)) {
    /* IP or ARP packet? */
    case ETHTYPE_IP:
    case ETHTYPE_ARP:
#if PPPOE_SUPPORT
    /* PPPoE packet? */
    case ETHTYPE_PPPOEDISC:
    case ETHTYPE_PPPOE:
#endif /* PPPOE_SUPPORT */
    /* full packet send to tcpip_thread to process */
    if (lwip_netif->input(pbuf, lwip_netif)!=ERR_OK) {
        LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
        pbuf_free(pbuf);
        pbuf = NULL;
    }
    break;

    default:
        pbuf_free(pbuf);
        pbuf = NULL;
        break;
    }
}

static uint8_t * lwip_netdrv_adapter_header(void *netbuf, int32_t len)
{
    struct pbuf *pbuf = netbuf;

    pbuf_header(pbuf, len);
    return pbuf->payload;
}

static void * lwip_netdrv_adapter_alloc_buf(void *netif, uint_fast32_t size)
{
    void *netbuf;

#if ETH_PAD_SIZE
    size += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif
    netbuf = pbuf_alloc(PBUF_RAW, size, PBUF_POOL);
    if (netbuf != NULL) {
#if ETH_PAD_SIZE
        pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif
    }
    return netbuf;
}

static void * lwip_netdrv_adapter_read_buf(void *netbuf, vsf_mem_t *mem)
{
    ASSERT((netbuf != NULL) && (mem != NULL))
    struct pbuf *pbuf = netbuf;

    mem->pchBuffer = pbuf->payload;
    mem->nSize = pbuf->len;
    return pbuf->next;
}

static void lwip_netdrv_adapter_free_buf(void *netbuf)
{
    pbuf_free((struct pbuf *)netbuf);
}

static const vsf_netdrv_adapter_t lwip_netdrv_adapter = {
    .on_connect     = lwip_netdrv_adapter_on_connect,
    .on_disconnect  = lwip_netdrv_adapter_on_disconnect,

    .alloc_buf      = lwip_netdrv_adapter_alloc_buf,
    .free_buf       = lwip_netdrv_adapter_free_buf,
    .read_buf       = lwip_netdrv_adapter_read_buf,

    .header         = lwip_netdrv_adapter_header,
    .on_outputted   = lwip_netdrv_adapter_on_outputted,
    .on_inputted    = lwip_netdrv_adapter_on_inputted,
};

void lwip_netif_set_netdrv(struct netif *netif, vsf_netdrv_t *netdrv)
{
    netif->state = netdrv;
    netdrv->netif = (void *)netif;
    netdrv->adapter = &lwip_netdrv_adapter;
}

vsf_netdrv_t * lwip_netif_get_netdrv(struct netif *netif)
{
    return (vsf_netdrv_t *)netif->state;
}

#endif      // VSF_USE_TCPIP
