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

#if VSF_USE_TCPIP == ENABLED && VSF_USE_LWIP == ENABLED

#define __VSF_NETDRV_CLASS_INHERIT_NETIF__
#include "component/tcpip/vsf_tcpip.h"

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"
//#include "netif/ppp_oe.h"

#include "lwip/tcpip.h"

/*============================ MACROS ========================================*/

#ifndef TCPIP_CFG_HOSTNAME
#   define TCPIP_CFG_HOSTNAME           "lwip"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void lwip_req___addr___from_user(ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gateway);

static vsf_err_t __lwip_netdrv_adapter_on_connect(void *netif);
static void __lwip_netdrv_adapter_on_disconnect(void *netif);
static void * __lwip_netdrv_adapter_alloc_buf(void *netif, uint_fast32_t size);
static void __lwip_netdrv_adapter_free_buf(void *netbuf);
static void * __lwip_netdrv_adapter_read_buf(void *netbuf, vsf_mem_t *mem);
static uint8_t * __lwip_netdrv_adapter_header(void *netbuf, int32_t len);
static void __lwip_netdrv_adapter_on_outputted(void *netif, void *netbuf, vsf_err_t err);
static void __lwip_netdrv_adapter_on_inputted(void *netif, void *netbuf, uint_fast32_t size);

/*============================ LOCAL VARIABLES ===============================*/

static const vk_netdrv_adapter_op_t __lwip_netdrv_adapter_op = {
    .on_connect     = __lwip_netdrv_adapter_on_connect,
    .on_disconnect  = __lwip_netdrv_adapter_on_disconnect,

    .alloc_buf      = __lwip_netdrv_adapter_alloc_buf,
    .free_buf       = __lwip_netdrv_adapter_free_buf,
    .read_buf       = __lwip_netdrv_adapter_read_buf,

    .header         = __lwip_netdrv_adapter_header,
    .on_outputted   = __lwip_netdrv_adapter_on_outputted,
    .on_inputted    = __lwip_netdrv_adapter_on_inputted,
};

/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_LWIP_REQ___ADDR___FROM_USER
WEAK(lwip_req___addr___from_user)
void lwip_req___addr___from_user(ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gateway)
{
}
#endif

// ethernetif implementation
static err_t __ethernetif_low_level_output(struct netif *netif, struct pbuf *p)
{
    vk_netdrv_t *netdrv = netif->state;
  
#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

    pbuf_ref(p);

    vsf_protect_t orig = vsf_protect_sched();
    if (!vk_netdrv_can_output(netdrv)) {
        vsf_eda_t *eda = vsf_eda_get_cur();
        ASSERT((NULL == netdrv->adapter.eda_pending) && vsf_eda_is_stack_owner(eda));
        netdrv->adapter.eda_pending = eda;
        vsf_unprotect_sched(orig);
        vsf_thread_wfe(VSF_EVT_USER);
    } else {
        vsf_unprotect_sched(orig);
    }

    vk_netdrv_output(netdrv, p);
    return ERR_OK;
}

err_t ethernetif_init(struct netif *netif)
{
    vk_netdrv_t *netdrv;

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
    NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 100000000);

    netif->name[0] = 'X';
    netif->name[1] = 'X';

    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) */
    netif->output = etharp_output;
    netif->linkoutput = __ethernetif_low_level_output;

    /* initialize the hardware */
    if (VSF_ERR_NONE != vk_netdrv_init(netdrv)) {
        return ERR_IF;
    }

    netif->hwaddr_len = netdrv->macaddr.size;
    memcpy(netif->hwaddr, netdrv->macaddr.addr_buf, netif->hwaddr_len);
    netif->mtu = netdrv->mtu;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

    return ERR_OK;
}

// adapter
static vsf_err_t __lwip_netdrv_adapter_on_connect(void *netif)
{
    struct netif *lwip_netif = netif;
    ip_addr_t ipaddr = { 0 }, netmask = { 0 }, gateway = { 0 };

    lwip_req___addr___from_user(&ipaddr, &netmask, &gateway);

    lwip_netif = netif_add(lwip_netif, &ipaddr.u_addr.ip4, &netmask.u_addr.ip4,
                &gateway.u_addr.ip4, lwip_netif->state,
                ethernetif_init, tcpip_input);
    if (lwip_netif != NULL) {
        netif_set_default(lwip_netif);
        return VSF_ERR_NONE;
    } else {
        return VSF_ERR_FAIL;
    }
}

static void __lwip_netdrv_adapter_on_disconnect(void *netif)
{
    LOCK_TCPIP_CORE();
        // TODO: make sure netif_remove does not depend on thread environment
        netif_remove((struct netif *)netif);
    UNLOCK_TCPIP_CORE();
}

static void __lwip_netdrv_adapter_on_outputted(void *netif, void *netbuf, vsf_err_t err)
{
    struct netif *lwip_netif = netif;
    vk_netdrv_t *netdrv = lwip_netif->state;

#if ETH_PAD_SIZE
    pbuf_header((struct pbuf *)netbuf, ETH_PAD_SIZE); /* reclaim the padding word */
#endif
    pbuf_free((struct pbuf *)netbuf);
    LINK_STATS_INC(link.xmit);

    vsf_eda_t *eda = netdrv->adapter.eda_pending;
    if (eda != NULL) {
        netdrv->adapter.eda_pending = NULL;
        vsf_eda_post_evt(eda, VSF_EVT_USER);
        return;
    }
}

static void __lwip_netdrv_adapter_on_inputted(void *netif, void *netbuf, uint_fast32_t size)
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

static uint8_t * __lwip_netdrv_adapter_header(void *netbuf, int32_t len)
{
    struct pbuf *pbuf = netbuf;

    pbuf_header(pbuf, len);
    return pbuf->payload;
}

static void * __lwip_netdrv_adapter_alloc_buf(void *netif, uint_fast32_t size)
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

static void * __lwip_netdrv_adapter_read_buf(void *netbuf, vsf_mem_t *mem)
{
    ASSERT((netbuf != NULL) && (mem != NULL));
    struct pbuf *pbuf = netbuf;

    mem->buffer = pbuf->payload;
    mem->size = pbuf->len;
    return pbuf->next;
}

static void __lwip_netdrv_adapter_free_buf(void *netbuf)
{
    pbuf_free((struct pbuf *)netbuf);
}

void lwip_netif_set_netdrv(struct netif *netif, vk_netdrv_t *netdrv)
{
    netif->state = netdrv;
    netdrv->adapter.netif = (void *)netif;
    netdrv->adapter.op = &__lwip_netdrv_adapter_op;
}

vk_netdrv_t * lwip_netif_get_netdrv(struct netif *netif)
{
    return (vk_netdrv_t *)netif->state;
}

#endif      // VSF_USE_TCPIP && VSF_USE_LWIP
