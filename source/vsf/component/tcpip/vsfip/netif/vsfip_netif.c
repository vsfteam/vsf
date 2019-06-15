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
#include "./vsfip_netdrv_adapter.h"

/*============================ MACROS ========================================*/

#define VSFIP_NETIF_EVT_ARPC_UPDATED        (VSF_EVT_USER + 0)
#define VSFIP_NETIF_EVT_OUTPUTTED           (VSF_EVT_USER + 1)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum vsfip_netif_ARP_op_t {
    ARP_REQUEST     = 1,
    ARP_REPLY       = 2,
    RARP_REQUEST    = 3,
    RARP_REPLY      = 4,
};
typedef enum vsfip_netif_ARP_cmd_t vsfip_netif_ARP_cmd_t;

struct vsfip_arp_head_t
{
    uint16_t hwtype;
    uint16_t prototype;
    uint8_t hwlen;
    uint8_t protolen;
    uint16_t op;
} PACKED;
typedef struct vsfip_arp_head_t vsfip_arp_head_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

SECTION(".text.vsf.kernel.eda")
vsf_err_t __vsf_eda_fini(vsf_eda_t *pthis);

extern void vsfip_ip4_input(vsfip_netbuf_t *netbuf);
extern void vsfip_ip6_input(vsfip_netbuf_t *netbuf);
extern void vsfip_ipbuf_on_finish(vsfip_netbuf_t *netbuf, vsf_err_t err);

static void vsfip_netif_arpc_evthandler(vsf_eda_t *eda, vsf_evt_t evt);

/*============================ IMPLEMENTATION ================================*/

void vsfip_netif_on_outputted(vsfip_netif_t *netif, vsfip_netbuf_t *netbuf, vsf_err_t err)
{
    if (NULL == netbuf->iphead.ip4head) {
        vsfip_netbuf_deref(netbuf);
    } else {
        vsfip_ipbuf_on_finish(netbuf, err);
    }
    vsf_eda_post_evt(&netif->output_eda, VSFIP_NETIF_EVT_OUTPUTTED);
}

void vsfip_netif_on_inputted(vsfip_netif_t *netif, vsfip_netbuf_t *netbuf)
{
    vsfip_netbuf_set_netif(netbuf, netif);
    netif->op->input(netbuf);
}

static void vsfip_netif_destruct(vsfip_netif_t *netif)
{
    __vsf_eda_fini(&netif->arpc.teda.use_as__vsf_eda_t);
    vsf_eda_sync_cancel(&netif->arpc.sem);
    vsf_eda_sync_cancel(&netif->output_sem);
}

static vsf_err_t vsfip_netif_construct(vsfip_netif_t *netif)
{
    vsf_slist_queue_init(&netif->output_queue);
    memset(&netif->arpc, 0, sizeof(netif->arpc));
    netif->arp_time = 1;

    vsf_eda_sem_init(&netif->output_sem, 0);
    vsf_eda_sem_init(&netif->arpc.sem, 0);

    return VSF_ERR_NONE;
}

static void vsfip_netif_output_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
	vsfip_netif_t *netif = container_of(eda, vsfip_netif_t, output_eda);
	vsfip_netbuf_t *netbuf;

	switch (evt) {
	case VSFIP_NETIF_EVT_OUTPUTTED:
	case VSF_EVT_INIT:
    wait_next:
        if (!netif->op->can_output(netif)) {
            break;
        }
		if (VSF_ERR_NONE != vsf_eda_sem_pend(&netif->output_sem, -1)) {
			break;
        }
        // fall through
	case VSF_EVT_SYNC:
        do {
            vsf_sched_lock_status_t origlevel = vsf_sched_lock();
                vsf_slist_queue_dequeue(vsfip_netbuf_t, netif_node, &netif->output_queue, netbuf);
            vsf_sched_unlock(origlevel);
        } while (0);
        if (netbuf != NULL) {
            netif->op->output(netbuf);
        }
        goto wait_next;
	}
}

vsf_err_t vsfip_netif_init(vsfip_netif_t *netif)
{
    vsf_err_t err = vsf_netdrv_init(netif->netdrv);
    if (!err) {
        netif->arpc.teda.evthandler = vsfip_netif_arpc_evthandler;
        err = vsf_teda_init(&netif->arpc.teda, vsf_priority_inherit, false);

        if (!err) {
            netif->output_eda.evthandler = vsfip_netif_output_evthandler;
            err = vsf_eda_init(&netif->output_eda, vsf_priority_inherit, false);
        }
    }
    return err;
}

void vsfip_netif_fini(vsfip_netif_t *netif)
{
    vsf_netdrv_fini(netif->netdrv);
}

uint_fast16_t vsfip_netif_mtu(vsfip_netif_t *netif)
{
    vsf_netdrv_t *netdrv = netif->netdrv;
    if (netdrv != NULL) {
        return netdrv->mtu;
    }
    return 0;
}

#ifdef VSFIP_TRACE_NETIF
void vsfip_netif_trace_ref(vsfip_netif_t *netif)
{
    vsf_trace(VSF_TRACE_DEBUG, "netif %08X ref : %d" VSFCFG_DEBUG_LINEEND, netif, netif->ref);
}
#endif

// first call to vsfip_netif_ref is at vsfip_netif_add,
//    which is called protected from the netdrv,
//    so vsfip_netif_construct is called protected
void vsfip_netif_ref(vsfip_netif_t *netif)
{
    if (!netif->ref++) {
        vsfip_netif_construct(netif);
    }
#ifdef VSFIP_TRACE_NETIF
    vsfip_netif_trace_ref(netif);
#endif
}

void vsfip_netif_deref(vsfip_netif_t *netif)
{
    netif->ref--;
#ifdef VSFIP_TRACE_NETIF
    vsfip_netif_trace_ref(netif);
#endif

    if (!netif->ref) {
        vsfip_netif_destruct(netif);
        netif->netdrv = NULL;
    }
}

static void vsfip_netif_get_mac_broadcast(vsfip_netif_t *netif, vsfip_macaddr_t *macaddr)
{
    macaddr->size = netif->netdrv->macaddr.size;
    memset(macaddr->addr_buf, 0, sizeof(macaddr->addr_buf));
    memset(macaddr->addr_buf, 0xFF, macaddr->size);
}

static bool vsfip_netif_get_mac(vsfip_netif_t *netif, vsfip_ipaddr_t *ip, vsfip_macaddr_t *mac)
{
    if (0xFFFFFFFF == ip->addr32) {
        vsfip_netif_get_mac_broadcast(netif, mac);
        return true;
    }

    mac->size = 0;
    for (uint_fast16_t i = 0; i < VSFIP_CFG_ARP_CACHE_SIZE; i++) {
        if (    netif->arp_cache[i].time
            &&  (ip->addr32 == netif->arp_cache[i].assoc.ip.addr32)) {
            *mac = netif->arp_cache[i].assoc.mac;
            return true;
        }
    }
    return false;
}

static vsf_err_t vsfip_netif_output(vsfip_netbuf_t *netbuf,
        vsfip_netif_proto_t proto, vsfip_macaddr_t *mac, bool urgent)
{
    vsfip_netif_t *netif = netbuf->netif;

    if (netif->op->header(netbuf, proto, mac)) {
        vsfip_netbuf_deref(netbuf);
        return VSF_ERR_FAIL;
    }

    vsf_sched_lock_status_t origlevel = vsf_sched_lock();
    if (urgent) {
        vsf_slist_queue_add_to_head(vsfip_netbuf_t, netif_node, &netif->output_queue, netbuf);
    } else {
        vsf_slist_queue_enqueue(vsfip_netbuf_t, netif_node, &netif->output_queue, netbuf);
    }
    vsf_sched_unlock(origlevel);
    return vsf_eda_sem_post(&netif->output_sem);
}

static void vsfip_netif_get_ipaddr(vsfip_netbuf_t *netbuf, vsfip_ipaddr_t *ipaddr)
{
    uint_fast8_t ipver = *netbuf->buf.pchBuffer >> 4;

    if (4 == ipver) {
        // IPv4
        vsfip_ip4_head_t *ip4_head = netbuf->buf.pObj;
        ipaddr->size = 4;
        ipaddr->addr32 = ip4_head->ipdest;
    } else /* if (6 == ipver) */ {
        // IPv6
        ipaddr->size = 6;
        // TODO
    }
}

static bool vsfip_netif_islocal(vsfip_netif_t *netif, vsfip_ipaddr_t *ipaddr)
{
    if (0xFFFFFFFF == ipaddr->addr32) {
        return true;
    }
    for (uint_fast8_t addr_mask, i = 0; i < netif->netmask.size; i++) {
        addr_mask = netif->netmask.addr_buf[i];
        if ((ipaddr->addr_buf[i] & addr_mask) != (netif->gateway.addr_buf[i] & addr_mask)) {
            return false;
        }
    }
    return true;
}

vsf_err_t vsfip_netif_ip_output(vsfip_netbuf_t *netbuf, bool urgent)
{
    vsfip_netif_t *netif = netbuf->netif;
    vsfip_macaddr_t mac;
    vsfip_ipaddr_t dest, *ip_for_mac;

    vsfip_netif_get_ipaddr(netbuf, &dest);
    ip_for_mac = vsfip_netif_islocal(netif, &dest) || !netif->gateway.size ?
                    &dest : &netif->gateway;
    if (!vsfip_netif_get_mac(netif, ip_for_mac, &mac) && netif->netdrv->macaddr.size) {
        vsf_sched_lock_status_t origlevel = vsf_sched_lock();
        vsf_slist_queue_enqueue(vsfip_netbuf_t, netif_node, &netif->arpc.request_queue, netbuf);
        vsf_sched_unlock(origlevel);
        return vsf_eda_sem_post(&netif->arpc.sem);
    } else if (netbuf->buf.nSize) {
        return vsfip_netif_output(netbuf, VSFIP_NETIF_PROTO_IP, &mac, urgent);
    }
    return VSF_ERR_NONE;
}

void vsfip_netif_ip4_input(vsfip_netbuf_t *netbuf)
{
    vsfip_ip4_input(netbuf);
}

void vsfip_netif_ip6_input(vsfip_netbuf_t *netbuf)
{
    vsfip_ip6_input(netbuf);
}

void vsfip_netif_arp_add_assoc(vsfip_netif_t *netif,
        uint_fast8_t hwlen, uint8_t *hwaddr, uint_fast8_t protolen, uint8_t *protoaddr)
{
    vsfip_arp_entry_t *entry = &netif->arp_cache[0];
    vsfip_ipaddr_t ip;
    vsfip_macaddr_t mac;

    ip.size = protolen;
    memcpy(ip.addr_buf, protoaddr, protolen);
    if (vsfip_netif_get_mac(netif, &ip, &mac)) {
        return;
    }

    for (uint_fast16_t i = 0; i < VSFIP_CFG_ARP_CACHE_SIZE; i++) {
        if (0 == netif->arp_cache[i].time) {
            entry = &netif->arp_cache[i];
            break;
        }
        if (netif->arp_cache[i].time < entry->time) {
            entry = &netif->arp_cache[i];
        }
    }
    entry->assoc.mac.size = hwlen;
    memcpy(entry->assoc.mac.addr_buf, hwaddr, hwlen);
    entry->assoc.ip.size = protolen;
    memcpy(entry->assoc.ip.addr_buf, protoaddr, protolen);
    entry->time = netif->arp_time++;
}

void vsfip_netif_arp_input(vsfip_netbuf_t *netbuf)
{
    vsfip_netif_t *netif = netbuf->netif;
    vsfip_arp_head_t *head = netbuf->buf.pObj;
    uint8_t *ptr = (uint8_t *)head + sizeof(vsfip_arp_head_t);
    uint8_t *bufptr;

    // endian fix
    head->hwtype = be16_to_cpu(head->hwtype);
    head->prototype = be16_to_cpu(head->prototype);
    head->op = be16_to_cpu(head->op);

    switch (head->op) {
    case ARP_REQUEST:
        bufptr = (uint8_t *)head + sizeof(vsfip_arp_head_t);

        if (    (head->hwlen == netif->netdrv->macaddr.size)
            &&  (head->protolen == netif->ip4addr.size)
            &&  (netbuf->buf.nSize >= (sizeof(vsfip_arp_head_t) + 2 * (head->hwlen + head->protolen)))
            &&  !memcmp(bufptr + 2 * head->hwlen + head->protolen, netif->ip4addr.addr_buf, head->protolen)) {

            vsfip_macaddr_t macaddr;
            vsfip_ipaddr_t ipaddr;

            // process the ARP request
            head->hwtype = cpu_to_be16(netif->netdrv->hwtype);
            head->prototype = cpu_to_be16(VSFIP_NETIF_PROTO_IP);
            head->op = cpu_to_be16(ARP_REPLY);
            macaddr.size = head->hwlen;
            memcpy(macaddr.addr_buf, bufptr, head->hwlen);
            memcpy(ipaddr.addr_buf, bufptr + head->hwlen, head->protolen);
            memcpy(bufptr, netif->netdrv->macaddr.addr_buf, head->hwlen);
            memcpy(bufptr + head->hwlen, netif->ip4addr.addr_buf, head->protolen);
            memcpy(bufptr + head->hwlen + head->protolen, macaddr.addr_buf, head->hwlen);
            memcpy(bufptr + 2 * head->hwlen + head->protolen, ipaddr.addr_buf, head->protolen);
            netbuf->buf.nSize = sizeof(*head) + 2 * (head->hwlen + head->protolen);

            // send ARP reply
            vsfip_netif_output(netbuf, VSFIP_NETIF_PROTO_ARP, &macaddr, false);
            return;
        }
        break;
    case ARP_REPLY:
        // for ARP reply, cache and send UPDATE event to netif->arpc.sm_pending
        if (    (head->hwlen != netif->netdrv->macaddr.size)
            ||  (head->protolen != netif->ip4addr.size)
            ||  (netbuf->buf.nSize < (sizeof(vsfip_arp_head_t) + 2 * (head->hwlen + head->protolen)))) {
            break;
        }

        // search a most suitable slot in the ARP cache
        vsfip_netif_arp_add_assoc(netif, head->hwlen, ptr, head->protolen, ptr + netif->netdrv->macaddr.size);
        vsf_eda_post_evt(&netif->arpc.teda.use_as__vsf_eda_t, VSFIP_NETIF_EVT_ARPC_UPDATED);
        break;
    }
    vsfip_netbuf_deref(netbuf);
}

static vsfip_netbuf_t * vsfip_netif_prepare_arp_request(
        vsfip_netif_t *netif, vsfip_ipaddr_t *ipaddr)
{
    vsfip_netbuf_t *netbuf = VSFIP_NETIFBUF_GET(128);

    if (netbuf != NULL) {
        vsfip_macaddr_t *macaddr = &netif->netdrv->macaddr;
        vsfip_arp_head_t *head;
        uint8_t *ptr;

        vsfip_netbuf_set_netif(netbuf, netif);

        head = netbuf->buf.pObj;
        head->hwtype = cpu_to_be16(netif->netdrv->hwtype);
        head->prototype = cpu_to_be16(VSFIP_NETIF_PROTO_IP);
        head->hwlen = (uint8_t)macaddr->size;
        head->protolen = (uint8_t)netif->ip4addr.size;
        head->op = cpu_to_be16(ARP_REQUEST);
        ptr = (uint8_t *)head + sizeof(vsfip_arp_head_t);
        memcpy(ptr, macaddr->addr_buf, macaddr->size);
        ptr += macaddr->size;
        memcpy(ptr, netif->ip4addr.addr_buf, netif->ip4addr.size);
        ptr += netif->ip4addr.size;
        memset(ptr, 0, macaddr->size);
        ptr += macaddr->size;
        memcpy(ptr, ipaddr->addr_buf, ipaddr->size);
        ptr += ipaddr->size;
        netbuf->app.nSize = netbuf->buf.nSize = ptr - (uint8_t *)head;
    }
    return netbuf;
}

static void vsfip_netif_arpc_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vsfip_netif_arpc_t *arpc = container_of(eda, vsfip_netif_arpc_t, teda);
    vsfip_netif_t *netif = container_of(arpc, vsfip_netif_t, arpc);
    vsfip_macaddr_t mac;
    vsfip_ipaddr_t dest;

    switch (evt) {
    case VSF_EVT_INIT:
    wait_next_request:
        if (netif->quit || (VSF_ERR_NONE != vsf_eda_sem_pend(&netif->arpc.sem, -1))) {
            break;
        }

        // fall through
    case VSF_EVT_SYNC:
        do {
            vsf_sched_lock_status_t origlevel = vsf_sched_lock();
                vsf_slist_queue_dequeue(vsf_netbuf_t, netif_node, &netif->arpc.request_queue, netif->arpc.cur_netbuf);
            vsf_sched_unlock(origlevel);
        } while (0);
        if (NULL == netif->arpc.cur_netbuf) {
            goto wait_next_request;
        }

        vsfip_netif_get_ipaddr(netif->arpc.cur_netbuf, &dest);
        // for local ip, send ARP for dest ip
        // for non-local ip, if gateway is valid, send to gateway
        // for non-local ip, if gateway is not valid, send ARP for dest ip
        //         and if proxy ARP is enabled on router, router will reply
        netif->arpc.ip_for_mac = vsfip_netif_islocal(netif, &dest) || !netif->gateway.size ? dest : netif->gateway;
        if (vsfip_netif_get_mac(netif, &netif->arpc.ip_for_mac, &mac)) {
            goto mac_got;
        }

        netif->arpc.retry = VSFIP_CFG_ARP_RETRY;
    retry:
        netif->arpc.cur_request = vsfip_netif_prepare_arp_request(netif, &netif->arpc.ip_for_mac);
        if (NULL == netif->arpc.cur_request) {
            goto failed;
        }

        vsfip_netif_get_mac_broadcast(netif, &mac);
        if (VSF_ERR_NONE != vsfip_netif_output(netif->arpc.cur_request, VSFIP_NETIF_PROTO_ARP, &mac, false)) {
            goto failed;
        }
        // wait for reply with timeout
        vsf_teda_set_timer_ms(VSFIP_CFG_ARP_TIMEOUT);
        break;
    case VSF_EVT_TIMER:
    failed:
        vsfip_netbuf_deref(netif->arpc.cur_netbuf);
        goto wait_next_request;
        break;
    case VSFIP_NETIF_EVT_ARPC_UPDATED:
        vsf_teda_cancel_timer(NULL);

        if (!vsfip_netif_get_mac(netif, &netif->arpc.ip_for_mac, &mac) && (netif->arpc.retry > 0)) {
            netif->arpc.retry--;
            goto retry;
        }

    mac_got:
        if (!mac.size) {
            vsfip_netbuf_deref(netif->arpc.cur_netbuf);
        } else {
            vsfip_netif_output(netif->arpc.cur_netbuf, VSFIP_NETIF_PROTO_IP, &mac, false);
        }
        goto wait_next_request;
    }
}

#endif      // VSF_USE_TCPIP
