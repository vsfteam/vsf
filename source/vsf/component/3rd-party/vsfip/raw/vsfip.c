/*****************************************************************************
 *   Copyright(C)2009-2019 by SimonQian                                      *
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

// TODO:
// add ICMP support
// add ip_router
// ....

/*============================ INCLUDES ======================================*/

#include "component/tcpip/vsf_tcpip_cfg.h"

#if VSF_USE_TCPIP == ENABLED && VSF_USE_VSFIP == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSFIP_CLASS_IMPLEMENT
#include "kernel/vsf_kernel.h"
#include "./vsfip.h"

/*============================ MACROS ========================================*/

#define VSFIP_TCP_RETRY         3
#define VSFIP_TCP_ATO           10

#define VSFIP_TCPOPT_MSS        0x02
#define VSFIP_TCPOPT_MSS_LEN    0x04

#define VSFIP_ICMP_ECHO_REPLY   0
#define VSFIP_ICMP_ECHO         8

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsfip_t {
    vsf_slist_t netif_list;

    vsf_slist_t udp_list;
#if VSFIP_CFG_TCP_EN == ENABLED
    vsf_slist_t tcp_list;
#endif

    vsf_teda_t teda;

    uint8_t quit;
    uint16_t udp_port;
#if VSFIP_CFG_TCP_EN == ENABLED
    uint16_t tcp_port;
#endif
    uint16_t ip_id;
    uint32_t tsn;
} vsfip_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static NO_INIT vsfip_t __vsfip;

/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vsfip_ip4_output(vsfip_socket_t *socket, vsfip_netbuf_t *netbuf);
static vsf_err_t __vsfip_ip6_output(vsfip_socket_t *socket, vsfip_netbuf_t *netbuf);
static void __vsfip_icmp_input(vsfip_netbuf_t *netbuf);

static void __vsfip_udp_input(vsfip_netbuf_t *netbuf);
static void __vsfip_udp_socket_input(vsfip_socket_t *socket, vsfip_netbuf_t *netbuf);

#if VSFIP_CFG_TCP_EN == ENABLED
static void __vsfip_tcp_socket_disable(vsfip_socket_t *socket);
static void __vsfip_tcp_input(vsfip_netbuf_t *netbuf);
static void __vsfip_tcp_socket_input(vsfip_socket_t *socket, vsfip_netbuf_t *netbuf);
#endif

extern vsfip_socket_t * vsfip_mem_socket_get(void);
extern void vsfip_mem_socket_free(vsfip_socket_t *socket);
extern vsfip_tcp_pcb_t * vsfip_mem_tcp_pcb_get(void);
extern void vsfip_mem_tcp_pcb_free(vsfip_tcp_pcb_t *tcp_pcb);
extern vsfip_netbuf_t * vsfip_mem_netbuf_get(uint_fast32_t size);
extern void vsfip_mem_netbuf_free(vsfip_netbuf_t *netbuf);

extern vsfip_netif_t * vsfip_ip_route_imp(vsfip_ipaddr_t *addr);

/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSFIP_MEM_SOCKET_GET
WEAK(vsfip_mem_socket_get)
vsfip_socket_t * vsfip_mem_socket_get(void)
{
    return NULL;
}
#endif

#ifndef WEAK_VSFIP_MEM_SOCKET_FREE
WEAK(vsfip_mem_socket_free)
void vsfip_mem_socket_free(vsfip_socket_t *socket)
{
}
#endif

#if VSFIP_CFG_TCP_EN == ENABLED
#   ifndef WEAK_VSFIP_MEM_TCP_PCB_GET
WEAK(vsfip_mem_tcp_pcb_get)
vsfip_tcp_pcb_t * vsfip_mem_tcp_pcb_get(void)
{
    return NULL;
}
#   endif

#   ifndef WEAK_VSFIP_MEM_TCP_PCB_FREE
WEAK(vsfip_mem_tcp_pcb_free)
void vsfip_mem_tcp_pcb_free(vsfip_tcp_pcb_t *tcp_pcb)
{
}
#   endif
#endif

#ifndef WEAK_VSFIP_MEM_NETBUF_GET
WEAK(vsfip_mem_netbuf_get)
vsfip_netbuf_t * vsfip_mem_netbuf_get(uint_fast32_t size)
{
    return NULL;
}
#endif

#ifndef WEAK_VSFIP_MEM_NETBUF_FREE
WEAK(vsfip_mem_netbuf_free)
void vsfip_mem_netbuf_free(vsfip_netbuf_t *netbuf)
{
}
#endif

// socket buffer
static vsfip_socket_t * __vsfip_socket_get(void)
{
    vsfip_socket_t *socket = vsfip_mem_socket_get();
    if (socket != NULL) {
        socket->family = AF_INET;
        socket->netif = NULL;
    }
    return socket;
}

static void __vsfip_socket_free(vsfip_socket_t *socket)
{
    if (socket != NULL) {
        if (socket->netif != NULL) {
#ifdef VSFIP_TRACE_NETIF
            vsf_trace_debug("socket deref");
#endif
            vsfip_netif_deref(socket->netif);
            socket->netif = NULL;
        }
        vsfip_mem_socket_free(socket);
    }
}

void vsfip_socket_set_netif(vsfip_socket_t *socket, vsfip_netif_t *netif)
{
    if (!socket->netif) {
        socket->netif = netif;
#ifdef VSFIP_TRACE_NETIF
        vsf_trace_debug("socket ref");
#endif
        vsfip_netif_ref(netif);
    }
}

#if VSFIP_CFG_TCP_EN == ENABLED
// tcp pcb buffer
static vsfip_tcp_pcb_t* __vsfip_tcp_pcb_get(void)
{
    vsfip_tcp_pcb_t *tcp_pcb = vsfip_mem_tcp_pcb_get();
    if (tcp_pcb != NULL) {
        tcp_pcb->state = VSFIP_TCPSTAT_CLOSED;
    }
    return tcp_pcb;
}

static void __vsfip_tcp_pcb_free(vsfip_tcp_pcb_t *pcb)
{
    if (pcb != NULL) {
        vsfip_mem_tcp_pcb_free(pcb);
    }
}
#endif

// buffer
vsfip_netbuf_t * vsfip_netbuf_get(uint32_t size)
{
    // reserve VSFIP_PROTO_HEADLEN as backup
    vsfip_netbuf_t *netbuf = vsfip_mem_netbuf_get(size + VSFIP_PROTO_HEADLEN);
    if (netbuf != NULL) {
        uint8_t *buffer = netbuf->buffer;
        memset(netbuf, 0, sizeof(*netbuf));
        netbuf->buffer = buffer;
        netbuf->ref = 1;
        netbuf->buf.buffer = netbuf->app.buffer = netbuf->buffer + VSFIP_PROTO_HEADLEN;
        netbuf->buf.size = netbuf->app.size = size;
    }
    return netbuf;
}

vsfip_netbuf_t * vsfip_appbuf_get(uint_fast32_t header_size, uint_fast32_t app_size)
{
    vsfip_netbuf_t *netbuf = vsfip_netbuf_get(header_size + app_size);
    if (netbuf != NULL) {
        vsfip_netbuf_header(netbuf, -header_size);
    }
    return netbuf;
}

void vsfip_netbuf_ref(vsfip_netbuf_t *netbuf)
{
    if (netbuf != NULL) {
        netbuf->ref++;
    }
}

void vsfip_netbuf_deref(vsfip_netbuf_t *netbuf)
{
    if ((netbuf != NULL) && netbuf->ref) {
        if (!--netbuf->ref) {
            if (netbuf->netif != NULL) {
#ifdef VSFIP_TRACE_NETIF
                vsf_trace_debug("buffer deref");
#endif
                vsfip_netif_deref(netbuf->netif);
                netbuf->netif = NULL;
            }
            vsfip_mem_netbuf_free(netbuf);
        }
    }
}

void vsfip_netbuf_set_netif(vsfip_netbuf_t *netbuf, vsfip_netif_t *netif)
{
    if (netbuf->netif != netif) {
        if (netbuf->netif != NULL) {
#ifdef VSFIP_TRACE_NETIF
            vsf_trace_debug("buffer deref");
#endif
            vsfip_netif_deref(netbuf->netif);
        }

        netbuf->netif = netif;
#ifdef VSFIP_TRACE_NETIF
        vsf_trace_debug("buffer ref");
#endif
        vsfip_netif_ref(netif);
    }
}

uint8_t * vsfip_netbuf_header(vsfip_netbuf_t *netbuf, int_fast16_t head_size)
{
    uint32_t cur_header_len = &netbuf->app.buffer[netbuf->pos] - netbuf->buf.buffer;
    bool backup = netbuf->pos > 0;

    if (    ((head_size > 0) && backup && ((cur_header_len + head_size) > VSFIP_PROTO_HEADLEN))
        ||  ((head_size < 0) && (netbuf->buf.size < -head_size))) {
        return NULL;
    }

    netbuf->buf.buffer -= head_size;
    netbuf->buf.size += head_size;

    if ((head_size > 0) && backup) {
        cur_header_len += head_size;
        memcpy(netbuf->buffer + VSFIP_PROTO_HEADLEN - cur_header_len,
                netbuf->buf.buffer, head_size);
    }
    return netbuf->buf.buffer;
}

// bufq
static void __vsfip_bufq_free(vsf_dlist_t *queue)
{
    vsfip_netbuf_t *netbuf;
    while (!vsf_dlist_is_empty(queue)) {
        vsf_dlist_queue_dequeue(vsfip_netbuf_t, proto_node, queue, netbuf);
        vsfip_netbuf_deref(netbuf);
    }
}

static uint_fast32_t __vsfip_bufq_len(vsf_dlist_t *queue)
{
    uint_fast32_t len = 0;

    __vsf_dlist_foreach_unsafe(vsfip_netbuf_t, proto_node, (vsf_slist_t *)queue) {
        len += _->app.size;
    }
    return len;
}

// netif
vsf_err_t vsfip_netif_add(vsfip_netif_t *netif)
{
    netif->ip4addr.size = 4;
    netif->ref = 0;
#ifdef VSFIP_TRACE_NETIF
    vsf_trace_debug("netif ref");
#endif
    vsfip_netif_ref(netif);
    vsfip_netif_init(netif);

    // add to netif list
    vsf_sched_lock_status_t origlevel = vsf_sched_lock();
        vsf_slist_add_to_head(vsfip_netif_t, node, &__vsfip.netif_list, netif);
    vsf_sched_unlock(origlevel);
    return VSF_ERR_NONE;
}

static void __vsfip_netif_cleanup_socket(vsfip_netif_t *netif)
{
    vsf_sched_lock_status_t origlevel = vsf_sched_lock();

        __vsf_slist_foreach_unsafe(vsfip_socket_t, node, &__vsfip.udp_list) {
            if (_->netif == netif) {
                vsf_eda_sync_cancel(&_->input_sem);
            }
        }

#if VSFIP_CFG_TCP_EN == ENABLED
        __vsf_slist_foreach_unsafe(vsfip_socket_t, node, &__vsfip.tcp_list) {
            if (_->netif == netif) {
                __vsfip_tcp_socket_disable(_);
            }

            vsf_slist_t *child_list = &_->listener.child;
            __vsf_slist_foreach_unsafe(vsfip_socket_t, node, child_list) {
                if (_->netif == netif) {
                    __vsfip_tcp_socket_disable(_);
                }
            }
        }
#endif
    vsf_sched_unlock(origlevel);
}

vsf_err_t vsfip_netif_remove(vsfip_netif_t *netif)
{
    vsf_sched_lock_status_t origlevel = vsf_sched_lock();
        netif = vsf_slist_remove(vsfip_netif_t, node, &__vsfip.netif_list, netif);
    vsf_sched_unlock(origlevel);

    if (netif != NULL) {
        __vsfip_netif_cleanup_socket(netif);
        vsfip_netif_fini(netif);
#ifdef VSFIP_TRACE_NETIF
        vsf_trace_debug("netif deref");
#endif
        vsfip_netif_deref(netif);
    }

    return VSF_ERR_NONE;
}

#ifndef WEAK_VSFIP_IP_ROUTE_IMP
WEAK(vsfip_ip_route_imp)
vsfip_netif_t * vsfip_ip_route_imp(vsfip_ipaddr_t *addr)
{
    return NULL;
}
#endif

// vsfip_ip_route MUST be called protected
static vsfip_netif_t * __vsfip_ip_route(vsfip_ipaddr_t *addr)
{
    __vsf_slist_foreach_unsafe(vsfip_netif_t, node, &__vsfip.netif_list) {
        if ((_->op->routable != NULL) && _->op->routable(_, addr)) {
            return _;
        }
    }
    return vsfip_ip_route_imp(addr);
}

static void __vsfip_netbuf_route(vsfip_socket_t *socket,
        vsfip_netbuf_t *netbuf, vsfip_ipaddr_t *addr)
{
    ASSERT(socket != NULL);
    vsf_sched_lock_status_t origlevel = vsf_sched_lock();
        vsfip_netbuf_set_netif(netbuf,
            (socket->netif != NULL) ? socket->netif : __vsfip_ip_route(addr));
    vsf_sched_unlock(origlevel);
}

static void __vsfip_socket_route(vsfip_socket_t *socket, vsfip_ipaddr_t *addr)
{
    ASSERT(socket != NULL);
    if (!socket->netif) {
        vsf_sched_lock_status_t origlevel = vsf_sched_lock();
            vsfip_socket_set_netif(socket, __vsfip_ip_route(addr));
        vsf_sched_unlock(origlevel);
    }
}

vsf_err_t vsfip_init(void)
{
    memset(&__vsfip, 0, sizeof(__vsfip));
    __vsfip.udp_port = VSFIP_CFG_UDP_PORT;
#if VSFIP_CFG_TCP_EN == ENABLED
    __vsfip.tcp_port = VSFIP_CFG_TCP_PORT;
#endif

//    __vsfip.teda.fn.evthandler = vsfip_evthandler;
//    return vsf_teda_init(&__vsfip.teda, vsf_prio_inherit, false);
    return VSF_ERR_NONE;
}

uint_fast16_t vsfip_get_port(vsfip_sock_proto_t proto)
{
    uint_fast16_t port = 0;
    vsf_sched_lock_status_t origlevel = vsf_sched_lock();

        switch (proto) {
#if VSFIP_CFG_TCP_EN == ENABLED
        case IPPROTO_TCP:
            port = __vsfip.tcp_port++;
            break;
#endif
        case IPPROTO_UDP:
            port = __vsfip.udp_port++;
            break;
        }
    vsf_sched_unlock(origlevel);
    return port;
}

vsf_err_t vsfip_fini(void)
{
//    return vsf_eda_post_evt(&__vsfip.teda, VSF_EVT_FINI);
    return VSF_ERR_NONE;
}

static uint_fast16_t __vsfip_checksum(uint8_t *data, uint16_t len)
{
    uint_fast32_t checksum = 0;

    while (len > 1) {
        checksum += get_unaligned_be16(data);
        data += 2;
        len -= 2;
    }
    if (1 == len) {
        checksum += (uint_fast16_t)(*data) << 8;
    }
    checksum = (checksum & 0xFFFF) + (checksum >> 16);
    checksum = (checksum & 0xFFFF) + (checksum >> 16);
    return (uint_fast16_t)checksum;
}

static uint_fast16_t __vsfip_proto_checksum(vsfip_socket_t *socket, vsfip_netbuf_t *netbuf)
{
    uint_fast32_t checksum = __vsfip_checksum(netbuf->buf.buffer, netbuf->buf.size);
    vsfip_ipaddr_t *local_addr = &netbuf->netif->ip4addr;
    vsfip_ipaddr_t *remote_addr = &socket->remote_sockaddr.addr;

    checksum += get_unaligned_be16(&local_addr->addr_buf[0]);
    checksum += get_unaligned_be16(&local_addr->addr_buf[2]);
    checksum += get_unaligned_be16(&remote_addr->addr_buf[0]);
    checksum += get_unaligned_be16(&remote_addr->addr_buf[2]);
    checksum += socket->protocol;
    checksum += netbuf->buf.size;
    checksum = (checksum & 0xFFFF) + (checksum >> 16);
    checksum = (checksum & 0xFFFF) + (checksum >> 16);
    return (uint_fast16_t)checksum;
}

// ip
static bool __vsfip_ip_is_match(vsfip_ipaddr_t *addr1, vsfip_ipaddr_t *addr2)
{
    return  (addr1->size == addr2->size)
        &&  !memcmp(addr1->addr_buf, addr2->addr_buf, addr1->size);
}

static vsf_err_t __vsfip_ip_output(vsfip_socket_t *socket, vsfip_netbuf_t *netbuf)
{
    return (AF_INET == socket->family) ?
                __vsfip_ip4_output(socket, netbuf) : __vsfip_ip6_output(socket, netbuf);
}

// ipv4
static vsfip_netbuf_t * __vsfip_ip4_reass(vsfip_netbuf_t *netbuf)
{
    // TODO: add ip_reass support
//    if (netbuf->iphead.ip4head->offset & (VSFIP_IPH_MF | VSFIP_IPH_OFFSET_MASK))
//    {
//        vsfip_netbuf_deref(netbuf);
//        return NULL;
//    }

    return netbuf;
}

static bool __vsfip_ip4_is_broadcast(vsfip_ipaddr_t *addr, vsfip_netif_t *netif)
{
    uint_fast32_t addr32 = addr->addr32;
    uint_fast32_t netmask32 = netif->netmask.addr32;
    uint_fast32_t netaddr32 = netif->ip4addr.addr32 & netmask32;

    return  (addr32 == VSFIP_IPADDR_ANY)
        ||  (addr32 == ~VSFIP_IPADDR_ANY)
        ||  (addr32 == (netaddr32 | (0xFFFFFFFF & ~netmask32)));
}

#if VSFIP_CFG_IP_FORWORD_EN == ENABLED
static void __vsfip_ip4_forward(vsfip_netbuf_t *netbuf,
                    vsfip_addr_t *addr, vsfip_netif_t *netif_in)
{
    vsfip_ip4head_t *iphead = netbuf->buf.obj_ptr;
    uint_fast32_t checksum;

    // TODO: skip for link-local address

    netbuf->netif = __vsfip_ip_route(addr);
    if ((NULL == netbuf->netif) || (netbuf->netif == netif_in) || !--iphead->ttl) {
        goto release_buf;
    }

    // add checksum by 0x100(dec 1 in ttl)
    checksum = be16_to_cpu(iphead->checksum) + 0x100;
    checksum += checksum >> 16;
    iphead->checksum = cpu_to_be16(checksum);

    vsfip_netif_ip_output(buf, false);
    return;
release_buf:
    vsfip_netbuf_deref(netbuf);
    return;
}
#endif

void vsfip_ip4_input(vsfip_netbuf_t *netbuf)
{
    vsfip_ip4_head_t *ip4head = netbuf->buf.obj_ptr;
    vsfip_ipaddr_t ipaddr;
    uint_fast16_t iph_hlen = VSFIP_IP4H_HLEN(ip4head) * 4;

    // ip header check
    if (__vsfip_checksum((uint8_t *)ip4head, iph_hlen) != 0xFFFF) {
        goto release_buf;
    }

    ipaddr.size = 4;
    ipaddr.addr32 = ip4head->ipdest;
    // forward ip if not for us:
    //         not boradcast and netif->ipaddr is valid and ipaddr not match
    if (    !__vsfip_ip4_is_broadcast(&ipaddr, netbuf->netif)
        &&  (netbuf->netif->ip4addr.size > 0)
        &&  !__vsfip_ip_is_match(&ipaddr, &netbuf->netif->ip4addr)) {
#if VSFIP_CFG_IP_FORWORD_EN == ENABLED
        __vsfip_ip4_forward(netbuf, &ipaddr, netif);
#else
        goto release_buf;
#endif
    }
    netbuf->iphead.ip4head = ip4head;

    // endian fix
    ip4head->len = be16_to_cpu(ip4head->len);
    ip4head->id = be16_to_cpu(ip4head->id);
    ip4head->offset = be16_to_cpu(ip4head->offset);
    ip4head->checksum = be16_to_cpu(ip4head->checksum);
    if (    (VSFIP_IP4H_V(ip4head) != 4)
        ||  (iph_hlen > ip4head->len)
        ||  (ip4head->len > netbuf->buf.size)) {
        goto release_buf;
    }
    netbuf->buf.size = ip4head->len - iph_hlen;
    netbuf->buf.buffer += iph_hlen;

    // ip reassembly
    if (!(ip4head->offset & VSFIP_IPH_DF)) {
        netbuf = __vsfip_ip4_reass(netbuf);
        if (NULL == netbuf) {
            return;
        }
    }

/*    if (is multicast)
    {
    }
    else
*/    {
        switch (ip4head->proto) {
        case IPPROTO_UDP:
            __vsfip_udp_input(netbuf);
            break;
#if VSFIP_CFG_TCP_EN == ENABLED
        case IPPROTO_TCP:
            __vsfip_tcp_input(netbuf);
            break;
#endif
        case IPPROTO_ICMP:
            __vsfip_icmp_input(netbuf);
            break;
        default:
            goto release_buf;
            break;
        }
    }
    return;
release_buf:
    vsfip_netbuf_deref(netbuf);
}

static void __vsfip_add_ip4head_common(vsfip_netbuf_t *ipbuf)
{
    vsfip_ip4_head_t *ip4head = ipbuf->iphead.ip4head;
    uint_fast32_t iph_hlen = VSFIP_IP4H_HLEN(ip4head) * 4;
    uint_fast32_t payload_len = ipbuf->buf.size - iph_hlen;
    uint_fast16_t mtu = vsfip_netif_mtu(ipbuf->netif);
    uint_fast16_t offset, checksum;
    bool fragement = false;

    if (ipbuf->buf.size > mtu) {
        payload_len = ((mtu - iph_hlen) >> 3) << 3;
        ipbuf->buf.size = payload_len + iph_hlen;
        fragement = true;
    }

    ip4head->len = cpu_to_be16(ipbuf->buf.size);
    offset = (ipbuf->pos >> 3) | (fragement ? VSFIP_IPH_MF : 0);
    ip4head->offset = cpu_to_be16(offset);
    ip4head->checksum = cpu_to_be16(0);
    checksum = ~__vsfip_checksum((uint8_t *)ip4head, sizeof(*ip4head));
    ip4head->checksum = cpu_to_be16(checksum);
}

static vsf_err_t __vsfip_add_ip4head(vsfip_socket_t *socket, vsfip_netbuf_t *ipbuf)
{
    vsfip_ip_pcb_t *pcb = &socket->pcb.ip_pcb;
    vsfip_ip4_head_t *ip4head;

    if (NULL == vsfip_netbuf_header(ipbuf, sizeof(vsfip_ip4_head_t))) {
        return VSF_ERR_FAIL;
    }

    ip4head = ipbuf->iphead.ip4head = ipbuf->buf.obj_ptr;
    ip4head->ver_hl = 0x45;        // IPv4 and 5 dwrod header len
    ip4head->tos = 0;
    ip4head->id = cpu_to_be16(pcb->id);
    ip4head->ttl = 32;
    ip4head->proto = (uint8_t)socket->protocol;
    ip4head->ipsrc = ipbuf->netif->ip4addr.addr32;
    ip4head->ipdest = socket->remote_sockaddr.addr.addr32;
    __vsfip_add_ip4head_common(ipbuf);
    return VSF_ERR_NONE;
}

void vsfip_ipbuf_on_finish(vsfip_netbuf_t *netbuf, vsf_err_t err)
{
    bool backup = netbuf->pos > 0;
    uint8_t *pbackup = netbuf->buffer + VSFIP_PROTO_HEADLEN;
    uint8_t *cur_app = &netbuf->app.buffer[netbuf->pos];
    uint8_t iphead[32];

    memcpy(iphead, netbuf->iphead.ip4head, sizeof(iphead));
    // recover payload data(over-wriitten by headers) if netbuf will not be freed
    if (backup) {
        uint_fast32_t header_len = cur_app - netbuf->buf.buffer;
        memcpy(netbuf->buf.buffer, pbackup - header_len, header_len);
    }

    netbuf->pos = &netbuf->buf.buffer[netbuf->buf.size] - netbuf->app.buffer;
    if (err || (netbuf->pos >= netbuf->app.size)) {
        vsfip_netbuf_deref(netbuf);
    } else {
        uint_fast32_t iph_hlen = VSFIP_IP4H_HLEN((vsfip_ip4_head_t *)iphead) * 4;

        netbuf->buf.buffer = &netbuf->app.buffer[netbuf->pos];
        netbuf->buf.size = &netbuf->app.buffer[netbuf->app.size] - netbuf->buf.buffer;

        vsfip_netbuf_header(netbuf, iph_hlen);
        memcpy(netbuf->buf.buffer, iphead, iph_hlen);
        netbuf->iphead.ip4head = netbuf->buf.obj_ptr;

        __vsfip_add_ip4head_common(netbuf);
        vsfip_netif_ip_output(netbuf, true);
    }
}

static vsf_err_t __vsfip_ip4_output(vsfip_socket_t *socket, vsfip_netbuf_t *netbuf)
{
    vsfip_ip_pcb_t *pcb = &socket->pcb.ip_pcb;
    vsf_err_t err;
    vsf_sched_lock_status_t origlevel;

    if (NULL == netbuf->netif) {
        __vsfip_netbuf_route(socket, netbuf, &socket->remote_sockaddr.addr);
        if (NULL == netbuf->netif) {
            err = VSF_ERR_FAIL;
            goto cleanup;
        }
    }

    origlevel = vsf_sched_lock();
        pcb->id = __vsfip.ip_id++;
    vsf_sched_unlock(origlevel);

    err = __vsfip_add_ip4head(socket, netbuf);
    if (err) { goto cleanup; }

    return vsfip_netif_ip_output(netbuf, false);
cleanup:
    vsfip_netbuf_deref(netbuf);
    return err;
}

// ipv6
void vsfip_ip6_input(vsfip_netbuf_t *netbuf)
{
    vsfip_netbuf_deref(netbuf);
}

static vsf_err_t __vsfip_ip6_output(vsfip_socket_t *socket, vsfip_netbuf_t *netbuf)
{
    vsfip_netbuf_deref(netbuf);
    return VSF_ERR_NONE;
}

// socket layer
vsfip_socket_t * vsfip_socket(vsfip_sock_familt_t family, vsfip_sock_proto_t protocol)
{
    vsfip_socket_t *socket = __vsfip_socket_get();

    if (socket != NULL) {
        memset(socket, 0, sizeof(vsfip_socket_t));
        socket->family = family;
        socket->protocol = protocol;
        vsf_dlist_init(&socket->input_queue);
        vsf_dlist_init(&socket->output_queue);
        vsf_eda_sem_init(&socket->input_sem, 0);

#if VSFIP_CFG_TCP_EN == ENABLED
        if (IPPROTO_TCP == protocol) {
            socket->can_rx = true;
            socket->proto_callback.on_input = __vsfip_tcp_socket_input;

            vsfip_tcp_pcb_t *pcb = __vsfip_tcp_pcb_get();
            if (NULL == pcb) {
                __vsfip_socket_free(socket);
                return NULL;
            }
            memset(pcb, 0, sizeof(vsfip_tcp_pcb_t));
            pcb->rclose = pcb->lclose = true;
            pcb->state = VSFIP_TCPSTAT_CLOSED;
            pcb->rx_window = VSFIP_CFG_TCP_RX_WINDOW;
            pcb->tx_window = VSFIP_CFG_TCP_TX_WINDOW;
            socket->pcb.proto_pcb = pcb;
        } else
#endif
        if (IPPROTO_UDP == protocol) {
            socket->proto_callback.on_input = __vsfip_udp_socket_input;
        }
    }

    return socket;
}

static vsfip_socket_t * __vsfip_child_socket(vsfip_socket_t *father, vsfip_sock_addr_t *sockaddr)
{
    vsfip_socket_t *socket_new = vsfip_socket(father->family, father->protocol);

    if (socket_new != NULL) {
        socket_new->can_rx = father->can_rx;
        socket_new->tx_timeout_ms = father->tx_timeout_ms;
        socket_new->rx_timeout_ms = father->rx_timeout_ms;
        memcpy(&socket_new->remote_sockaddr, sockaddr, sizeof(vsfip_sock_addr_t));
        memcpy(&socket_new->local_sockaddr, &father->local_sockaddr, sizeof(vsfip_sock_addr_t));
        socket_new->father = father;
        vsf_slist_add_to_head(vsfip_socket_t, node, &father->listener.child, socket_new);
    }
    return socket_new;
}

static vsf_err_t __vsfip_free_socket(vsfip_socket_t *socket)
{
    vsf_slist_t *list;

    vsf_sched_lock_status_t origlevel = vsf_sched_lock();
        if (socket->father != NULL) {
            list = &socket->father->listener.child;
        } else {
            switch (socket->protocol) {
            case IPPROTO_UDP:
                list = &__vsfip.udp_list;
                break;
#if VSFIP_CFG_TCP_EN == ENABLED
            case IPPROTO_TCP:
                list = &__vsfip.tcp_list;
                break;
#endif
            }
        }
        vsf_slist_remove(vsfip_socket_t, node, list, socket);
    vsf_sched_unlock(origlevel);

    // remove pending incoming buffer
    __vsfip_bufq_free(&socket->input_queue);
    __vsfip_bufq_free(&socket->output_queue);

#if VSFIP_CFG_TCP_EN == ENABLED
    if ((IPPROTO_TCP == socket->protocol) && (socket->pcb.proto_pcb != NULL)) {
        vsfip_tcp_pcb_t *tcp_pcb = socket->pcb.proto_pcb;

//        if ((VSFIP_TCPSTAT_LISTEN == tcppcb->state) && (tcppcb->rx_sm != NULL)) {
//            vsfip_tcp_postevt(&tcppcb->rx_sm, VSFIP_EVT_TCP_CONNECTFAIL);
//        }

        __vsfip_tcp_pcb_free(tcp_pcb);
    }
#endif
    __vsfip_socket_free(socket);
    return VSF_ERR_NONE;
}

vsf_err_t vsfip_close(vsfip_socket_t *socket)
{
    if ((socket->listener.backlog > 0) && (socket->listener.accepted_num > 0)) {
        socket->listener.closing = true;
        return VSF_ERR_NONE;
    }

    if (socket->father != NULL) {
        vsfip_socket_t *father = socket->father;

        if (vsf_slist_remove(vsfip_socket_t, node, &father->listener.child, socket)) {
            father->listener.accepted_num--;
        }

        if (father->listener.closing && !father->listener.accepted_num) {
            __vsfip_free_socket(father);
        }
    }
    return __vsfip_free_socket(socket);
}

void vsfip_socket_cb(vsfip_socket_t *socket,
                void *param, void (*on_input)(void *, vsfip_netbuf_t *),
                void (*on_outputted)(void *))
{
    socket->user_callback.param = param;
    socket->user_callback.on_input = on_input;
    socket->user_callback.on_outputted = on_outputted;
}

vsf_err_t vsfip_bind(vsfip_socket_t *socket, uint_fast16_t port)
{
    vsf_slist_t *list;

    vsf_sched_lock_status_t origlevel = vsf_sched_lock();
        switch (socket->protocol) {
        case IPPROTO_UDP:
            list = &__vsfip.udp_list;
            break;
#if VSFIP_CFG_TCP_EN == ENABLED
        case IPPROTO_TCP:
            list = &__vsfip.tcp_list;
            break;
#endif
        }

        // check if port already binded
        __vsf_slist_foreach_unsafe(vsfip_socket_t, node, list) {
            if ((_->local_sockaddr.port == port) && (_->netif == socket->netif)) {
                vsf_sched_unlock(origlevel);
                return VSF_ERR_FAIL;
            }
        }

        // for global socket(netif is NULL), append to the list
        // for netif socket, link from begin
        // so netif socket will be matched first, then global socket
        if (!socket->netif) {
            vsf_slist_append(vsfip_socket_t, node, list, socket);
        } else {
            vsf_slist_add_to_head(vsfip_sockt_t, node, list, socket);
        }
    vsf_sched_unlock(origlevel);
    socket->local_sockaddr.port = port;
    return VSF_ERR_NONE;
}

// for UDP port, vsfip_listen will enable rx for the socket
// for TCP port, vsfip_listen is same as listen socket API
vsf_err_t vsfip_listen(vsfip_socket_t *socket, uint_fast8_t backlog)
{
    socket->can_rx = true;

#if VSFIP_CFG_TCP_EN == ENABLED
    if (socket->protocol == IPPROTO_TCP) {
        vsfip_tcp_pcb_t *tcp_pcb = socket->pcb.proto_pcb;

        if (0 == backlog) {
            return VSF_ERR_INVALID_PARAMETER;
        }
        if (socket->listener.backlog > 0) {
            return VSF_ERR_FAIL;
        }

        socket->listener.backlog = backlog;
        tcp_pcb->state = VSFIP_TCPSTAT_LISTEN;
    }
#endif

    return VSF_ERR_NONE;
}

// proto common
// TODO: fix for IPv6
static void __vsfip_proto_input(vsf_slist_t *list, vsfip_netbuf_t *netbuf, vsfip_proto_port_t *port)
{
    vsfip_ip4_head_t *iphead = netbuf->iphead.ip4head;
    vsfip_sock_addr_t *remote_addr, *local_addr;
    uint_fast32_t remote_ipaddr = iphead->ipsrc;
    vsfip_socket_t *socket = NULL;

    // find the socket
    __vsf_slist_foreach_unsafe(vsfip_socket_t, node, list) {
        local_addr = &_->local_sockaddr;
        // if port match and
        //         local addr is ADDR_ANY or local addr match ipdest in iphead
        if (    (local_addr->port == port->dst)
            &&  (!local_addr->addr.addr32 || (local_addr->addr.addr32 == iphead->ipdest))) {
            if (_->listener.backlog == 0) {
                // normal socket
                remote_addr = &_->remote_sockaddr;
                if (    (   (remote_addr->addr.addr32 == VSFIP_IPADDR_ANY)
                         || (remote_addr->addr.addr32 == remote_ipaddr))
                    &&  (   (0 == remote_addr->port)
                         || (remote_addr->port == port->src))) {
                    socket = _;
                    break;
                }
            } else {
                socket = _;
                // listener socket
                list = &_->listener.child;
                __vsf_slist_foreach_unsafe(vsfip_socket_t, node, list) {
                    remote_addr = &_->remote_sockaddr;
                    if (    (remote_addr->addr.addr32 == remote_ipaddr)
                        &&  (remote_addr->port == port->src)) {
                        socket = _;
                        break;
                    }
                }
                break;
            }
        }
    }

    if (    (NULL == socket)
        ||  !socket->can_rx
        ||  ((socket->netif != NULL) && (netbuf->netif != socket->netif))) {
        goto discard_buffer;
    }

    if (socket->proto_callback.on_input != NULL) {
        socket->proto_callback.on_input(socket, netbuf);
        return;
    }
discard_buffer:
    vsfip_netbuf_deref(netbuf);
}

// UDP
static void __vsfip_udp_socket_input(vsfip_socket_t *socket, vsfip_netbuf_t *netbuf)
{
    if (socket->user_callback.on_input != NULL) {
        socket->user_callback.on_input(socket->user_callback.param, netbuf);
    } else {
        netbuf->ttl = VSFIP_CFG_TTL_INPUT;
        vsf_dlist_queue_enqueue(vsfip_netbuf_t, proto_node, &socket->input_queue, netbuf);
        vsf_eda_sem_post(&socket->input_sem);
    }
}

static void __vsfip_udp_input(vsfip_netbuf_t *netbuf)
{
    vsfip_udp_head_t *udphead = netbuf->buf.obj_ptr;

    // endian fix
    udphead->port.src = be16_to_cpu(udphead->port.src);
    udphead->port.dst = be16_to_cpu(udphead->port.dst);
    udphead->len = be16_to_cpu(udphead->len);
    udphead->checksum = be16_to_cpu(udphead->checksum);

    netbuf->app.buffer = netbuf->buf.buffer + sizeof(vsfip_udp_head_t);
    netbuf->app.size = netbuf->buf.size - sizeof(vsfip_udp_head_t);
    __vsfip_proto_input(&__vsfip.udp_list, netbuf, &udphead->port);
}

static vsf_err_t __vsfip_udp_add_head(vsfip_socket_t *socket, vsfip_netbuf_t *netbuf)
{
    vsfip_udp_head_t *udphead;
    uint_fast16_t checksum;

    if (NULL == vsfip_netbuf_header(netbuf, sizeof(vsfip_udp_head_t))) {
        return VSF_ERR_FAIL;
    }

    // checksum need netif
    if (NULL == netbuf->netif) {
        __vsfip_netbuf_route(socket, netbuf, &socket->remote_sockaddr.addr);
        if (NULL == netbuf->netif) {
            return VSF_ERR_FAIL;
        }
    }

    udphead = netbuf->buf.obj_ptr;
    udphead->port.src = cpu_to_be16(socket->local_sockaddr.port);
    udphead->port.dst = cpu_to_be16(socket->remote_sockaddr.port);
    udphead->len = cpu_to_be16(netbuf->buf.size);
    udphead->checksum = cpu_to_be16(0x0000);

    checksum = ~__vsfip_proto_checksum(socket, netbuf);
    udphead->checksum = cpu_to_be16(checksum);
    return VSF_ERR_NONE;
}

vsf_err_t vsfip_udp_async_send(vsfip_socket_t *socket, vsfip_sock_addr_t *sockaddr, vsfip_netbuf_t *netbuf)
{
    vsf_err_t err = VSF_ERR_NONE;

    if ((NULL == socket) || (NULL == sockaddr) || (NULL == netbuf)) {
        err = VSF_ERR_INVALID_PARAMETER;
        goto cleanup;
    }
    socket->remote_sockaddr = *sockaddr;

    // allocate local port if not set
    if (    (0 == socket->local_sockaddr.port)
        &&  vsfip_bind(socket, vsfip_get_port(IPPROTO_UDP))) {
        err = VSF_ERR_FAIL;
        goto cleanup;
    }

    // add udp header
    netbuf->pos = 0;
    netbuf->buf = netbuf->app;
    err = __vsfip_udp_add_head(socket, netbuf);
    if (err) { goto cleanup; }

    err = __vsfip_ip_output(socket, netbuf);
    if (!err && (socket->user_callback.on_outputted != NULL)) {
        socket->user_callback.on_outputted(socket->user_callback.param);
    }
    return err;
cleanup:
    vsfip_netbuf_deref(netbuf);
    return err;
}

vsf_err_t vsfip_udp_send(vsfip_socket_t *socket, vsfip_sock_addr_t *sockaddr, vsfip_netbuf_t *buf)
{
    return vsfip_udp_async_send(socket, sockaddr, buf);
}

// TODO: fix for IPv6
static vsfip_netbuf_t * __vsfip_udp_get_netbuf(vsfip_socket_t *socket, vsfip_sock_addr_t *addr)
{
    vsfip_netbuf_t *netbuf = NULL;
    vsfip_ip4_head_t *iphead;

    __vsf_dlist_foreach_unsafe(vsfip_netbuf_t, proto_node, &socket->input_queue) {
        iphead = _->iphead.ip4head;
        if (VSFIP_IPADDR_ANY == addr->addr.addr32) {
            addr->addr.addr32 = iphead->ipsrc;
        }
        if (addr->addr.addr32 == iphead->ipsrc) {
            vsf_dlist_remove(vsfip_netbuf_t, proto_node, &socket->input_queue, _);
            _->app.buffer = _->buf.buffer + sizeof(vsfip_udp_head_t);
            _->app.size = _->buf.size - sizeof(vsfip_udp_head_t);
            netbuf = _;
            break;
        }
    }

    return netbuf;
}

vsf_err_t vsfip_udp_async_recv(vsfip_socket_t *socket, vsfip_sock_addr_t *sockaddr, vsfip_netbuf_t **netbuf)
{
    if ((NULL == socket) || (NULL == sockaddr) || (NULL == netbuf) || !socket->local_sockaddr.port) {
        return VSF_ERR_INVALID_PARAMETER;
    }
    socket->remote_sockaddr = *sockaddr;
    *netbuf = __vsfip_udp_get_netbuf(socket, sockaddr);
    return *netbuf ? VSF_ERR_NONE : VSF_ERR_NOT_READY;
}

__vsf_component_peda_private_entry(__vsfip_udp_recv,
    vsfip_sock_addr_t * sockaddr;
    vsfip_netbuf_t ** netbuf;
) {
    vsfip_socket_t *socket = (vsfip_socket_t *)&vsf_this;
    vsf_err_t err;

    vsf_peda_begin();

    switch (evt) {
    case VSF_EVT_INIT:
        if (VSF_ERR_NONE != vsf_eda_sem_pend(&socket->input_sem, vsf_systimer_ms_to_tick(socket->rx_timeout_ms))) {
            break;
        }
        goto recv_udp_packet;
    case VSF_EVT_SYNC:
    case VSF_EVT_TIMER: {
            vsf_sync_reason_t reason = vsf_eda_sync_get_reason(&socket->input_sem, evt);
            if (VSF_SYNC_GET == reason) {
            recv_udp_packet:
                // got sem, recv again, will not fail
                err = vsfip_udp_async_recv(socket, vsf_local.sockaddr, vsf_local.netbuf);
                VSF_TCPIP_ASSERT(VSF_ERR_NONE == err);
                vsf_eda_return(err);
                break;
            } else if (reason != VSF_SYNC_PENDING) {
                vsf_eda_return(VSF_ERR_FAIL);
            }
        }
        break;
    }

    vsf_peda_end();
}

vsf_err_t vsfip_udp_recv(vsfip_socket_t *socket, vsfip_sock_addr_t *sockaddr, vsfip_netbuf_t **netbuf)
{
    vsf_err_t err;

    __vsf_component_call_peda(__vsfip_udp_recv, err, socket,
        .sockaddr   = sockaddr,
        .netbuf     = netbuf,
    );
    return err;
}

// TCP
#if VSFIP_CFG_TCP_EN == ENABLED
static uint32_t vsfip_get_tsn(void)
{
    vsf_sched_lock_status_t origlevel = vsf_sched_lock();
    uint_fast32_t tsn = __vsfip.tsn;
    __vsfip.tsn += 0x10000;
    vsf_sched_unlock(origlevel);
    return tsn;
}

static void __vsfip_tcp_input(vsfip_netbuf_t *netbuf)
{
    vsfip_tcp_head_t *tcphead = netbuf->buf.obj_ptr;

    // endian fix
    tcphead->port.src = be16_to_cpu(tcphead->port.src);
    tcphead->port.dst = be16_to_cpu(tcphead->port.dst);
    tcphead->seq = be16_to_cpu(tcphead->seq);
    tcphead->ackseq = be16_to_cpu(tcphead->ackseq);
    tcphead->headlen = (tcphead->headlen >> 4) << 2;
    tcphead->window_size = be16_to_cpu(tcphead->window_size);
    tcphead->checksum = be16_to_cpu(tcphead->checksum);
    tcphead->urgent_ptr = be16_to_cpu(tcphead->urgent_ptr);

    netbuf->proto_node.addr = tcphead->seq;
    netbuf->app.buffer = netbuf->buf.buffer + tcphead->headlen;
    netbuf->app.size = netbuf->buf.size - tcphead->headlen;
    __vsfip_proto_input(&__vsfip.tcp_list, netbuf, &tcphead->port);
}

static vsf_err_t __vsfip_tcp_add_head(vsfip_socket_t *socket, vsfip_netbuf_t *netbuf, uint_fast8_t flags)
{
    vsfip_tcp_pcb_t *pcb = socket->pcb.proto_pcb;
    uint8_t headlen = sizeof(vsfip_tcp_head_t);
    vsfip_tcp_head_t *head;
    uint_fast16_t window_size;
    uint_fast16_t checksum;
    uint8_t *option;

    if (flags & VSFIP_TCPFLAG_SYN) {
        headlen += 4;
    }
    if (NULL == vsfip_netbuf_header(netbuf, headlen)) {
        return VSF_ERR_FAIL;
    }

    __vsfip_netbuf_route(socket, netbuf, &socket->remote_sockaddr.addr);
    head = netbuf->buf.obj_ptr;
    option = (uint8_t *)head + sizeof(vsfip_tcp_head_t);
    memset(head, 0, sizeof(vsfip_tcp_head_t));
    head->port.src = cpu_to_be16(socket->local_sockaddr.port);
    head->port.dst = cpu_to_be16(socket->remote_sockaddr.port);
    head->seq = cpu_to_be32(pcb->lseq);
    netbuf->proto_node.addr = pcb->lseq;
    head->ackseq = cpu_to_be32(pcb->rseq);
    head->headlen = (headlen >> 2) << 4;
    flags |= (pcb->rseq != 0) ? VSFIP_TCPFLAG_ACK : 0;
    head->flags = flags;

    window_size = pcb->rx_window - __vsfip_bufq_len(&socket->input_queue);
    head->window_size = cpu_to_be16(window_size);

    head->checksum = cpu_to_be16(0);
    head->urgent_ptr = cpu_to_be16(0);

    if (flags & VSFIP_TCPFLAG_SYN) {
        // add MSS
        *option++ = VSFIP_TCPOPT_MSS;
        *option++ = VSFIP_TCPOPT_MSS_LEN;
        put_unaligned_be16(VSFIP_CFG_TCP_MSS, option);
    }

    checksum = ~__vsfip_proto_checksum(socket, netbuf);
    head->checksum = cpu_to_be16(checksum);

    pcb->acked_rseq = pcb->rseq;
    return VSF_ERR_NONE;
}

static vsf_err_t __vsfip_tcp_send_do(vsfip_socket_t *socket, vsfip_netbuf_t *netbuf, uint_fast8_t flags)
{
    vsfip_tcp_pcb_t *pcb = socket->pcb.proto_pcb;

    netbuf->pos = 0;
    netbuf->buf = netbuf->app;
    if (__vsfip_tcp_add_head(socket, netbuf, flags) < 0) {
        vsfip_netbuf_deref(netbuf);
        return VSF_ERR_FAIL;
    }
    __vsfip_ip_output(socket, netbuf);

    if ((flags & (VSFIP_TCPFLAG_FIN | VSFIP_TCPFLAG_SYN)) || (netbuf->app.size > 0)) {
        pcb->flags = flags;
    }
    pcb->lseq += netbuf->app.size;
    return VSF_ERR_NONE;
}

static vsf_err_t __vsfip_tcp_sendflags(vsfip_socket_t *socket, uint_fast8_t flags)
{
    vsfip_tcp_pcb_t *pcb = socket->pcb.proto_pcb;
    vsfip_netbuf_t *netbuf;

    // even if netbuf not available, also need timeout to resend
    if (flags & (VSFIP_TCPFLAG_SYN | VSFIP_TCPFLAG_FIN)) {
        pcb->tx_timeout_ms = socket->tx_timeout_ms;
    }

    // TODO: set netbuf->netif if already connected
    netbuf = VSFIP_TCPBUF_GET(0);
    return netbuf ? __vsfip_tcp_send_do(socket, netbuf, flags) : VSF_ERR_NOT_ENOUGH_RESOURCES;
}

static void __vsfip_tcp_socket_tick(vsfip_socket_t *socket)
{
    vsfip_tcp_pcb_t *pcb = socket->pcb.proto_pcb;

    if (pcb->tx_timeout_ms > 0) {
        pcb->tx_timeout_ms--;
        if (0 == pcb->tx_timeout_ms) {
            if (--pcb->tx_retry) {
                pcb->lseq = pcb->acked_lseq;
                __vsfip_tcp_sendflags(socket, pcb->flags);
                if (pcb->flags & (VSFIP_TCPFLAG_SYN | VSFIP_TCPFLAG_FIN)) {
                    pcb->lseq++;
                }
                goto check_rx_to;
            }

            // timer out
            if (socket->father && (pcb->flags & VSFIP_TCPFLAG_SYN)) {
                // child socket fail to connect, no need to send event
                vsfip_close(socket);
                goto check_rx_to;
            }

            switch (pcb->state) {
            case VSFIP_TCPSTAT_SYN_SENT:
            case VSFIP_TCPSTAT_SYN_GET:
            case VSFIP_TCPSTAT_LASTACK:
                pcb->state = VSFIP_TCPSTAT_CLOSED;
            }
//            vsfip_tcp_postevt(&pcb->tx_sm, VSFIP_EVT_SOCKET_TIMEOUT);
            return;
        }
    }

check_rx_to:
    if (pcb->rx_timeout_ms > 0) {
        pcb->rx_timeout_ms--;
        if (0 == pcb->rx_timeout_ms) {
//            vsfip_tcp_postevt(&pcb->rx_sm, VSFIP_EVT_SOCKET_TIMEOUT);
            return;
        }
    }

    __vsf_dlist_foreach_unsafe(vsfip_netbuf_t, proto_node, &socket->output_queue) {
        if (!--_->ttl) {
            // if _->retry is 0
            //         always retry
            // if passive close(remote sent FIN, VSFIP_TCPSTAT_CLOSEWAIT mode)
            // if timer_out and retry count reach 0
            //         free bufferlist
            if (    (!_->retry || --_->retry)
                &&  (pcb->state != VSFIP_TCPSTAT_CLOSEWAIT)) {
                uint_fast32_t lseq = pcb->lseq;
                pcb->lseq = _->proto_node.addr;
                vsfip_netbuf_ref(_);
                _->ttl = socket->tx_timeout_ms;
                __vsfip_tcp_send_do(socket, _, VSFIP_TCPFLAG_PSH);
                pcb->lseq = lseq;
            } else {
                // flush bufferlist
                __vsfip_bufq_free(&socket->output_queue);
                pcb->lseq = pcb->acked_lseq;

                // TODO: how to fix if tcp packet time out?
                pcb->tx_retry = 3;
                __vsfip_tcp_sendflags(socket, VSFIP_TCPFLAG_FIN);
                pcb->lseq++;
                pcb->state = pcb->rclose ? VSFIP_TCPSTAT_LASTACK : VSFIP_TCPSTAT_FINWAIT1;
                pcb->lclose = true;

//                vsfip_tcp_postevt(&pcb->tx_sm, VSFIP_EVT_SOCKET_TIMEOUT);
                return;
            }
        }
    }

    if ((pcb->acked_rseq != pcb->rseq) && !--pcb->ack_tick) {
        __vsfip_tcp_sendflags(socket, VSFIP_TCPFLAG_ACK);
        pcb->ack_tick = VSFIP_TCP_ATO;
    }
}

static void __vsfip_tcp_data_input(vsfip_socket_t *socket, vsfip_netbuf_t *netbuf)
{
    vsfip_tcp_pcb_t *pcb = socket->pcb.proto_pcb;

    pcb->rseq += netbuf->app.size;
    pcb->ack_tick = VSFIP_TCP_ATO;
    if (socket->user_callback.on_input != NULL) {
        socket->user_callback.on_input(socket->user_callback.param, netbuf);
    } else {
        netbuf->ttl = VSFIP_CFG_TTL_INPUT;
        vsf_dlist_queue_enqueue(vsfip_netbuf_t, proto_node, &socket->input_queue, netbuf);
    }
}

static void __vsfip_tcp_socket_disable(vsfip_socket_t *socket)
{
    vsfip_tcp_pcb_t *pcb = socket->pcb.proto_pcb;
    pcb->disabled = true;
//    if (pcb->tx_sm)
//        vsfip_tcp_postevt(&pcb->tx_sm, VSFIP_EVT_NETIF_REMOVED);
//    if (pcb->rx_sm)
//        vsfip_tcp_postevt(&pcb->rx_sm, VSFIP_EVT_NETIF_REMOVED);
}

static void __vsfip_tcp_socket_input(vsfip_socket_t *socket, vsfip_netbuf_t *netbuf)
{
    vsfip_tcp_pcb_t *pcb = socket->pcb.proto_pcb;
    vsfip_tcp_head_t head = *(vsfip_tcp_head_t *)netbuf->buf.obj_ptr;
    bool release_buffer = true;
    vsfip_tcp_stat_t pre_state = pcb->state;

    if ((socket->listener.backlog > 0) && (head.flags != VSFIP_TCPFLAG_SYN)) {
        // for listern socket, can only accept SYN packet
        // maybe some child socket fails and close, but remote still send packet
        // the packet will be directed to the listener socket
        vsfip_netbuf_deref(netbuf);
        return;
    }

    pcb->rwnd = head.window_size;
    if (head.flags & VSFIP_TCPFLAG_RST) {
        vsfip_netbuf_deref(netbuf);
        pcb->tx_timeout_ms = 0;
        pcb->rx_timeout_ms = 0;
        pcb->state = VSFIP_TCPSTAT_CLOSED;
        __vsfip_bufq_free(&socket->input_queue);
        __vsfip_bufq_free(&socket->output_queue);

//        vsfip_tcp_postevt(&pcb->tx_sm, VSFIP_EVT_SOCKET_TIMEOUT);
//        vsfip_tcp_postevt(&pcb->rx_sm, VSFIP_EVT_SOCKET_TIMEOUT);
        return;
    }

re_process:
    switch (pcb->state) {
    case VSFIP_TCPSTAT_INVALID:
        goto release_buf;
    case VSFIP_TCPSTAT_CLOSED:
        __vsfip_tcp_sendflags(socket, VSFIP_TCPFLAG_RST);
        goto release_buf;
    case VSFIP_TCPSTAT_LISTEN:
        if (    (VSFIP_TCPFLAG_SYN == head.flags)
            &&  (socket->listener.accepted_num < socket->listener.backlog)
            &&  !socket->listener.closing) {
            vsfip_sock_addr_t sockaddr;
            vsfip_ip4_head_t *iphead = netbuf->iphead.ip4head;
            vsfip_socket_t *socket_new;
            vsfip_tcp_pcb_t *pcb_new;

            sockaddr.port = head.port.src;
            sockaddr.addr.addr32 = iphead->ipsrc;
            sockaddr.addr.size = 4;

            //new a socket to process the data
            socket_new = __vsfip_child_socket(socket, &sockaddr);
            if (NULL == socket_new) {
                goto release_buf;
            }
            socket->listener.accepted_num++;

            pcb_new = socket_new->pcb.proto_pcb;
            pcb_new->lseq = pcb_new->acked_lseq = vsfip_get_tsn();
            pcb_new->rseq = 0;
            pcb_new->state = VSFIP_TCPSTAT_SYN_GET;
            pcb_new->rseq = head.seq + 1;
            pcb_new->acked_lseq = pcb_new->lseq;

            pcb_new->tx_retry = 3;
            vsfip_socket_set_netif(socket_new, netbuf->netif);
            __vsfip_tcp_sendflags(socket_new, VSFIP_TCPFLAG_SYN | VSFIP_TCPFLAG_ACK);
            pcb_new->lseq++;
            pcb_new->rclose = false;
        }
        goto release_buf;
        break;
    case VSFIP_TCPSTAT_SYN_SENT:
        if (VSFIP_TCPFLAG_SYN == head.flags) {
            // simultaneous open
            pcb->rseq = head.seq + 1;

            pcb->tx_retry = 3;
            __vsfip_tcp_sendflags(socket, VSFIP_TCPFLAG_SYN | VSFIP_TCPFLAG_ACK);
            pcb->lseq++;
            pcb->state = VSFIP_TCPSTAT_SYN_GET;
        } else if ((head.flags & (VSFIP_TCPFLAG_SYN | VSFIP_TCPFLAG_ACK)) && (head.ackseq == pcb->lseq)) {
            pcb->tx_timeout_ms = 0;

            // send ACK
            pcb->rseq = head.seq + 1;
            pcb->acked_lseq = pcb->lseq;

            __vsfip_tcp_sendflags(socket, VSFIP_TCPFLAG_ACK);
        connected:
            pcb->rclose = pcb->lclose = false;
            pcb->state = VSFIP_TCPSTAT_ESTABLISHED;
            if (release_buffer) {
                vsfip_netbuf_deref(netbuf);
            }
            if (!pcb->tx_sm && socket->father) {
                pcb = socket->father->pcb.proto_pcb;
            }
//            vsfip_tcp_postevt(&pcb->tx_sm, VSFIP_EVT_TCP_CONNECTOK);
            return;
        }
        goto release_buf;
        break;
    case VSFIP_TCPSTAT_SYN_GET:
        if (VSFIP_TCPFLAG_SYN == head.flags) {
            // get resend SYN(previous SYN + ACK lost), reply again
            pcb->lseq = pcb->acked_lseq;
            pcb->tx_retry = 3;
            __vsfip_tcp_sendflags(socket, VSFIP_TCPFLAG_SYN | VSFIP_TCPFLAG_ACK);
            pcb->lseq++;
        } else if ((head.flags & VSFIP_TCPFLAG_ACK) && (head.ackseq == (pcb->acked_lseq + 1))) {
            pcb->tx_timeout_ms = 0;
            pcb->acked_lseq = pcb->lseq = head.ackseq;
            if (netbuf->app.size > 0) {
                if (pcb->rseq == head.seq) {
                    release_buffer = false;
                    __vsfip_tcp_data_input(socket, netbuf);
                } else {
                    // send the ACK with the latest valid seq to remote
                    __vsfip_tcp_sendflags(socket, VSFIP_TCPFLAG_ACK);
                }
            }
            goto connected;
        } else if (VSFIP_TCPFLAG_FIN & head.flags) {
            pcb->tx_retry = 3;
            __vsfip_tcp_sendflags(socket, VSFIP_TCPFLAG_FIN);
            pcb->lseq++;
            pcb->state = VSFIP_TCPSTAT_FINWAIT1;
            pcb->lclose = true;
        }
        goto release_buf;
    case VSFIP_TCPSTAT_ESTABLISHED:
        if (netbuf->app.size > 0) {
            if (pcb->rseq == head.seq) {
                release_buffer = false;
                __vsfip_tcp_data_input(socket, netbuf);
//                vsfip_tcp_postevt(&pcb->rx_sm, VSFIP_EVT_TCP_RXOK);
                if (pcb->state != pre_state) {
                    goto re_process;
                }
            } else {
                __vsfip_tcp_sendflags(socket, VSFIP_TCPFLAG_ACK);
            }
        }
    case VSFIP_TCPSTAT_CLOSEWAIT:
        if ((head.flags & VSFIP_TCPFLAG_ACK) && (head.ackseq != pcb->acked_lseq)) {
            // ACK received
            pcb->tx_timeout_ms = 0;
            pcb->acked_lseq = head.ackseq;

            __vsf_dlist_foreach_unsafe(vsfip_netbuf_t, proto_node, &socket->output_queue) {
                if (pcb->acked_lseq >= (_->proto_node.addr + _->app.size)) {
//                    vsfq_dequeue(&socket->outq);
                    if (socket->user_callback.on_outputted != NULL) {
                        socket->user_callback.on_outputted(socket->user_callback.param);
                    }
                    vsfip_netbuf_deref(netbuf);
                } else {
                    break;
                }
            }
//            vsfip_tcp_postevt(&pcb->tx_sm, VSFIP_EVT_TCP_TXOK);
            if (pcb->state != pre_state) {
                goto re_process;
            }
        }

        if (head.flags & VSFIP_TCPFLAG_FIN) {
            // passive close
        remote_close:
            // clear FIN, in case next state is CLOSEWAIT, and process again
            head.flags &= ~VSFIP_TCPFLAG_FIN;
            pcb->rclose = true;
            pcb->rseq = head.seq + 1;
            __vsfip_tcp_sendflags(socket, VSFIP_TCPFLAG_ACK);

            // remote close, when local socket is closed
            // set the pcb->state = VSFIP_TCPSTAT_LASTACK
            pre_state = pcb->state = pcb->lclose ? VSFIP_TCPSTAT_CLOSED : VSFIP_TCPSTAT_CLOSEWAIT;
            vsfip_tcp_postevt(&pcb->rx_sm, VSFIP_EVT_TCP_RXFAIL);
            if (pcb->state != pre_state) {
                goto re_process;
            }
            if (VSFIP_TCPSTAT_CLOSED == pcb->state) {
                vsfip_tcp_postevt(&pcb->tx_sm, VSFIP_EVT_TCP_CLOSED);
            }
        }

        if (release_buffer) {
            goto release_buf;
        }
        break;
    case VSFIP_TCPSTAT_FINWAIT1:
        // local close, but still can receive data from remote
        if ((head.flags & VSFIP_TCPFLAG_ACK) && (head.ackseq == pcb->lseq)) {
            pcb->lclose = true;
            pcb->tx_timeout_ms = 0;
            pcb->acked_lseq = pcb->lseq;
            if (head.flags & VSFIP_TCPFLAG_FIN) {
                goto remote_close;
            }
            pcb->state = VSFIP_TCPSTAT_FINWAIT2;
        }
        goto check_data_in_FIN;
    case VSFIP_TCPSTAT_FINWAIT2:
        if (head.flags & VSFIP_TCPFLAG_FIN) {
            goto remote_close;
        }
    check_data_in_FIN:
        if (netbuf->app.size > 0) {
            if (pcb->rseq == head.seq) {
                pcb->rseq += netbuf->app.size;
            }
            __vsfip_tcp_sendflags(socket, VSFIP_TCPFLAG_ACK);
        }
        goto release_buf;
    case VSFIP_TCPSTAT_LASTACK:
        // maybe receive FIN again because previous ACK is lost
        if (head.flags & VSFIP_TCPFLAG_FIN) {
            __vsfip_tcp_sendflags(socket, VSFIP_TCPFLAG_ACK);
        }
        if ((VSFIP_TCPFLAG_ACK == head.flags) && (head.ackseq == pcb->lseq)) {
            pcb->lclose = true;
            pcb->tx_timeout_ms = 0;
            pcb->acked_lseq = pcb->lseq;
            pcb->state = VSFIP_TCPSTAT_CLOSED;
            vsfip_netbuf_deref(netbuf);
            vsfip_tcp_postevt(&pcb->tx_sm, VSFIP_EVT_TCP_CLOSED);
        } else {
            goto release_buf;
        }
        break;
    default:
    release_buf:
        vsfip_netbuf_deref(netbuf);
        break;
    }
}

void vsfip_tcp_config_window(vsfip_socket_t *socket, uint_fast32_t rx_window, uint_fast32_t tx_window)
{
    vsfip_tcp_pcb_t *pcb = socket->pcb.proto_pcb;
    pcb->rx_window = rx_window;
    pcb->tx_window = tx_window;
}

static vsfip_socket_t * __vsfip_tcp_accept_try(vsfip_socket_t *socket)
{
    vsfip_tcp_pcb_t *pcb = socket->pcb.proto_pcb;

    if (pcb->disabled) { return NULL; }
    __vsf_slist_foreach_unsafe(vsfip_socket_t, node, &socket->listener.child) {
        pcb = _->pcb.proto_pcb;
        if ((pcb->state == VSFIP_TCPSTAT_ESTABLISHED) && !_->accepted) {
            _->accepted = true;
            return _;
        }
    }
    return NULL;
}

vsf_err_t vsfip_tcp_async_send(vsfip_socket_t *socket, vsfip_netbuf_t *netbuf)
{
    vsfip_tcp_pcb_t *pcb = socket->pcb.proto_pcb;
    vsf_err_t err = VSF_ERR_NONE;
    uint_fast32_t size, window;

    if (pcb->tx_sm != NULL) {
        err = VSF_ERR_BUG;
        goto cleanup;
    }
    if (pcb->lclose || pcb->disabled) {
        err = VSF_ERR_FAIL;
        goto cleanup;
    }

    size = __vsfip_bufq_len(&socket->output_queue);
    window = min(pcb->tx_window, pcb->rwnd);
    if ((size + netbuf->app.size) > window) {
        return VSF_ERR_NOT_READY;
    }

    netbuf->retry = VSFIP_TCP_RETRY;
    netbuf->ttl = socket->tx_timeout_ms;
    vsf_dlist_queue_enqueue(vsfip_netbuf_t, proto_node, &socket->output_queue, netbuf);
    vsfip_netbuf_ref(netbuf);
    if (__vsfip_tcp_send_do(socket, netbuf, VSFIP_TCPFLAG_PSH) != 0) {
        err = VSF_ERR_FAIL;
        goto cleanup;
    }
    return VSF_ERR_NONE;
cleanup:
    vsfip_netbuf_deref(netbuf);
    return err;
}

vsf_err_t vsfip_tcp_async_recv(vsfip_socket_t *socket, vsfip_netbuf_t **netbuf)
{
    vsfip_tcp_pcb_t *pcb = socket->pcb.proto_pcb;
    vsfip_netbuf_t *netbuf_tmp;

    if (pcb->rx_sm != NULL) {
        return VSF_ERR_BUG;
    }
    if (pcb->rclose || pcb->disabled) {
        return VSF_ERR_FAIL;
    }

    vsf_dlist_queue_dequeue(vsfip_netbuf_t, proto_node, &socket->input_queue, netbuf_tmp);
    *netbuf = netbuf_tmp;
    return (NULL == *netbuf) ? VSF_ERR_NOT_READY : VSF_ERR_NONE;
}
#endif      // VSFIP_CFG_TCP_EN

// ICMP
static void __vsfip_icmp_input(vsfip_netbuf_t *netbuf)
{
    vsfip_ip4_head_t *iphead = netbuf->iphead.ip4head;
    uint_fast16_t iph_hlen = VSFIP_IP4H_HLEN(iphead) * 4;
    vsfip_icmp_head_t *icmphead = netbuf->buf.obj_ptr;

    netbuf->app.buffer = netbuf->buf.buffer + sizeof(vsfip_icmp_head_t);
    netbuf->app.size = netbuf->buf.size - sizeof(vsfip_icmp_head_t);
    if (icmphead->type == VSFIP_ICMP_ECHO) {
        uint_fast32_t swap_ip_cache;

        //swap ipaddr and convert to bigger endian
        swap_ip_cache = iphead->ipsrc;
        iphead->ipsrc = iphead->ipdest;
        iphead->ipdest = swap_ip_cache;
        iphead->len = cpu_to_be16(iphead->len);
        iphead->id = cpu_to_be16(iphead->id);
        iphead->checksum = cpu_to_be16(iphead->checksum);

        icmphead->type = VSFIP_ICMP_ECHO_REPLY;
        icmphead->checksum += cpu_to_be16(VSFIP_ICMP_ECHO << 8);
        if (icmphead->checksum >= cpu_to_be16(0xffff - (VSFIP_ICMP_ECHO << 8))) {
            icmphead->checksum++;
        }

        if (NULL != vsfip_netbuf_header(netbuf, iph_hlen)) {
            vsfip_netif_ip_output(netbuf, false);
        }
        return;
    }
    // TODO:
    vsfip_netbuf_deref(netbuf);
}

// helper
// pton
vsf_err_t vsfip_ip4_pton(vsfip_ipaddr_t *ip, char *domain)
{
    uint_fast8_t i;
    char *str = domain;

    //may not head by zero
    i = atoi(str);
    if (i == 0) {
        return VSF_ERR_FAIL;
    }
    //is vaild num
    ip->addr_buf[0] = i;

    for (i = 1; i < 4; i++) {
        str = strchr(str, '.');
        if (str == NULL) {
            return VSF_ERR_FAIL;
        }
        str++;
        ip->addr_buf[i] = atoi(str);
    }
    ip->size = 4;
    return VSF_ERR_NONE;
}

#endif      // VSF_USE_TCPIP
