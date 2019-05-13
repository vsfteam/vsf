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

#ifndef __VSFIP_H__
#define __VSFIP_H__

/*============================ INCLUDES ======================================*/

#include "component/tcpip/vsf_tcpip_cfg.h"

#if VSF_USE_TCPIP == ENABLED

#include "../netdrv/vsf_netdrv.h"

#if     defined(VSFIP_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(VSFIP_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#ifndef TCPIP_CFG_HOSTNAME
#   define TCPIP_CFG_HOSTNAME           "vsfip"
#endif

#ifndef VSFIP_CFG_NETIF_HEADLEN
#   warning "VSFIP_CFG_NETIF_HEADLEN not defined, use 64 by default"
#   define VSFIP_CFG_NETIF_HEADLEN      64
#endif

#ifndef VSFIP_CFG_TTL_INPUT
#   define VSFIP_CFG_TTL_INPUT          10
#endif
#ifndef VSFIP_CFG_UDP_PORT
#   define VSFIP_CFG_UDP_PORT           40000
#endif
#ifndef VSFIP_CFG_TCP_PORT
#   define VSFIP_CFG_TCP_PORT           40000
#endif
#ifndef VSFIP_CFG_MTU
#   define VSFIP_CFG_MTU                1500
#endif
#ifndef VSFIP_BUFFER_SIZE
// NETIF_HEAD + 1500(MTU)
#   define VSFIP_BUFFER_SIZE            (VSFIP_CFG_MTU + VSFIP_CFG_NETIF_HEADLEN)
#endif
#ifndef VSFIP_CFG_TCP_MSS
// 1500(MTU) - 20(TCP_HEAD) - 20(IP_HEAD)
#   define VSFIP_CFG_TCP_MSS            (VSFIP_CFG_MTU - VSFIP_IP_HEADLEN - VSFIP_TCP_HEADLEN)
#endif

#define VSFIP_IPADDR_ANY                0

#define VSFIP_IP_HEADLEN                20
#define VSFIP_UDP_HEADLEN               8
#define VSFIP_TCP_HEADLEN               20

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vsfip_socket_t)
declare_simple_class(vsfip_netbuf_t)

typedef vsf_netdrv_addr_t vsfip_addr_t;
typedef vsf_netdrv_addr_t vsfip_ipaddr_t;
typedef vsf_netdrv_addr_t vsfip_macaddr_t;

typedef enum vsfip_sock_familt_t vsfip_sock_familt_t;
typedef enum vsfip_sock_proto_t vsfip_sock_proto_t;
typedef struct vsfip_sock_addr_t vsfip_sock_addr_t;

struct vsfip_ipmac_assoc {
    vsfip_macaddr_t mac;
    vsfip_ipaddr_t ip;
};
typedef struct vsfip_ipmac_assoc vsfip_ipmac_assoc;

/*============================ INCLUDES ======================================*/

#include "./netif/vsfip_netif.h"

/*============================ TYPES =========================================*/

#define VSFIP_BUF_GET(s)            vsfip_netbuf_get(s)
#define VSFIP_NETIFBUF_GET(s)       VSFIP_BUF_GET((s) + VSFIP_CFG_NETIF_HEADLEN)
#define VSFIP_IPBUF_GET(s)          VSFIP_NETIFBUF_GET((s) + VSFIP_IP_HEADLEN)

#define VSFIP_PROTO_HEADLEN         (VSFIP_CFG_NETIF_HEADLEN + VSFIP_IP_HEADLEN)
#define VSFIP_UDPBUF_GET(s)         vsfip_appbuf_get(VSFIP_PROTO_HEADLEN + VSFIP_UDP_HEADLEN, (s))
#define VSFIP_TCPBUF_GET(s)         vsfip_appbuf_get(VSFIP_PROTO_HEADLEN + VSFIP_TCP_HEADLEN, (s))

enum vsfip_sock_familt_t {
    AF_NONE        = 0,
    AF_INET        = 2,
    AF_INET6    = 10,
};

enum vsfip_sock_proto_t {
//    IPPROTO_IP      = 0,
    IPPROTO_ICMP    = 1,
//    IPPROTO_IGMP    = 2,
    IPPROTO_TCP     = 6,
    IPPROTO_UDP     = 17,
//    IPPROTO_SCTP    = 132,
//    IPPROTO_RAW     = 255,
};

struct vsfip_sock_addr_t {
    uint16_t port;
    vsfip_ipaddr_t addr;
};

struct vsfip_ip_pcb_t {
    vsfip_ipaddr_t src;
    vsfip_ipaddr_t dest;
    uint16_t id;
};
typedef struct vsfip_ip_pcb_t vsfip_ip_pcb_t;

// headers
// IPv4
struct vsfip_ip4_head_t {
    uint8_t ver_hl;
    uint8_t tos;
    uint16_t len;
    uint16_t id;
#define VSFIP_IPH_RF            0x8000
#define VSFIP_IPH_DF            0x4000
#define VSFIP_IPH_MF            0x2000
#define VSFIP_IPH_OFFSET_MASK   0x1FFF
    uint16_t offset;
    uint8_t ttl;
    uint8_t proto;
    uint16_t checksum;
    uint32_t ipsrc;
    uint32_t ipdest;
} PACKED;
typedef struct vsfip_ip4_head_t vsfip_ip4_head_t;
#define VSFIP_IP4H_V(h)         ((h)->ver_hl >> 4)
#define VSFIP_IP4H_HLEN(h)      ((h)->ver_hl & 0x0F)

// PROTO PORT
struct vsfip_proto_port_t {
    uint16_t src;
    uint16_t dst;
} PACKED;
typedef struct vsfip_proto_port_t vsfip_proto_port_t;

// UDP
struct vsfip_udp_head_t {
    vsfip_proto_port_t port;
    uint16_t len;
    uint16_t checksum;
} PACKED;
typedef struct vsfip_udp_head_t vsfip_udp_head_t;

// TCP
struct vsfip_tcp_head_t {
    vsfip_proto_port_t port;
    uint32_t seq;
    uint32_t ackseq;
    uint8_t headlen;
    uint8_t flags;
#define VSFIP_TCPFLAG_FIN        (uint16_t)(1 << 0)
#define VSFIP_TCPFLAG_SYN        (uint16_t)(1 << 1)
#define VSFIP_TCPFLAG_RST        (uint16_t)(1 << 2)
#define VSFIP_TCPFLAG_PSH        (uint16_t)(1 << 3)
#define VSFIP_TCPFLAG_ACK        (uint16_t)(1 << 4)
#define VSFIP_TCPFLAG_URG        (uint16_t)(1 << 5)
    uint16_t window_size;
    uint16_t checksum;
    uint16_t urgent_ptr;
} PACKED;
typedef struct vsfip_tcp_head_t vsfip_tcp_head_t;

// ICMP
struct vsfip_icmp_head_t {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    union {
        struct {
            uint16_t flags;
            uint16_t seqnum;
        } echo;
    } body;
} PACKED;
typedef struct vsfip_icmp_head_t vsfip_icmp_head_t;


enum vsfip_tcp_stat_t {
    VSFIP_TCPSTAT_INVALID,
    VSFIP_TCPSTAT_CLOSED,
    VSFIP_TCPSTAT_LISTEN,
    VSFIP_TCPSTAT_SYN_SENT,
    VSFIP_TCPSTAT_SYN_GET,
    VSFIP_TCPSTAT_ESTABLISHED,
    VSFIP_TCPSTAT_FINWAIT1,
    VSFIP_TCPSTAT_FINWAIT2,
    VSFIP_TCPSTAT_CLOSEWAIT,
    VSFIP_TCPSTAT_LASTACK,
};
typedef enum vsfip_tcp_stat_t vsfip_tcp_stat_t;

struct vsfip_tcp_pcb_t {
    vsfip_tcp_stat_t state;
    uint32_t lseq;
    uint32_t acked_lseq;
    uint32_t rseq;
    uint32_t acked_rseq;
    uint32_t rwnd;

    // tx
    uint32_t tx_timeout_ms;         // only for FIN and SYN
    uint32_t tx_retry;              // only for FIN and SYN
    uint32_t tx_window;

    // rx
    uint32_t rx_timeout_ms;
    uint32_t rx_window;

    uint32_t ack_tick;
    uint8_t rclose      : 1;
    uint8_t lclose      : 1;
    uint8_t reset       : 1;
    uint8_t ack_timeout : 1;
    uint8_t disabled    : 1;
    uint8_t flags;

    vsf_err_t err;
};
typedef struct vsfip_tcp_pcb_t vsfip_tcp_pcb_t;

struct vsfip_pcb_t
{
    vsfip_ip_pcb_t ip_pcb;
    void *proto_pcb;
};
typedef struct vsfip_pcb_t vsfip_pcb_t;

// buffer layout:
//    |--VSFIP_PROTO_HEADLEN--||--VSFIP_CFG_NETIF_HEADLEN--||--app--|
//    used for data backup     headers(tcp/udb, ip, eth...) app data
// Note:
//    users can only access app data area
//    If VSFIP_IPBUF_NO_AUTOFREE is set in flags while sending buffer,
//        buffer will not be released after being sent.
def_simple_class(vsfip_netbuf_t) {
    vsf_slist_node_t proto_node;
    vsf_slist_node_t netif_node;
    vsf_mem_t buf;
    vsf_mem_t app;
    uint8_t *buffer;

    union {
        vsfip_ip4_head_t *ip4head;
//        vsfip_ip6_head_t *ip6head;
    } iphead;

    uint8_t ref     : 4;
    uint8_t retry   : 4;
    uint8_t ttl;
    uint16_t pos;

    vsfip_netif_t *netif;
};

def_simple_class(vsfip_socket_t) {
    vsf_slist_node_t node;

    vsfip_sock_familt_t family;
    vsfip_sock_proto_t protocol;

    vsfip_sock_addr_t local_sockaddr;
    vsfip_sock_addr_t remote_sockaddr;

    vsfip_pcb_t pcb;
    vsf_sem_t input_sem;
    vsf_slist_queue_t input_queue;
    vsf_slist_queue_t output_queue;

    struct {
        vsf_slist_t child;
        uint8_t backlog;
        uint8_t accepted_num;
        bool closing;
    } listener;
    bool accepted;
    bool can_rx;
    vsfip_socket_t *father;
    vsfip_netif_t *netif;

    uint32_t tx_timeout_ms;
    uint32_t rx_timeout_ms;

    struct {
        void (*on_input)(struct vsfip_socket_t *so, struct vsfip_netbuf_t *buf);
    } proto_callback;
    struct {
        void (*on_input)(void *param, struct vsfip_netbuf_t *buf);
        void (*on_outputted)(void *param);
        void *param;
    } user_callback;
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsfip_netbuf_t * vsfip_netbuf_get(uint_fast32_t size);
extern vsfip_netbuf_t * vsfip_appbuf_get(uint_fast32_t header_size, uint_fast32_t app_size);
extern void vsfip_netbuf_ref(vsfip_netbuf_t *netbuf);
extern void vsfip_netbuf_deref(vsfip_netbuf_t *netbuf);
extern uint8_t * vsfip_netbuf_header(vsfip_netbuf_t *netbuf, int_fast16_t size);

extern uint_fast32_t vsfip_buflist_read(vsfip_netbuf_t *netbuf, uint_fast32_t offset,
        uint8_t *data, uint_fast32_t len);
extern uint_fast32_t vsfip_buflist_write(vsfip_netbuf_t *netbuf, uint_fast32_t offset,
        uint8_t *data, uint_fast32_t len);

extern vsf_err_t vsfip_netif_add(vsfip_netif_t *netif);
extern vsf_err_t vsfip_netif_remove(vsfip_netif_t *netif);

extern vsf_err_t vsfip_init(void);
extern vsf_err_t vsfip_fini(void);

// vsfip_netbuf_set_netif MUST be called protected
extern void vsfip_netbuf_set_netif(vsfip_netbuf_t *netbuf, vsfip_netif_t *netif);

// different from standard socket call,
// vsfip_socket will return a pointer to vsfip_socket_t structure
extern vsfip_socket_t * vsfip_socket(vsfip_sock_familt_t family, vsfip_sock_proto_t protocol);
extern vsf_err_t vsfip_close(vsfip_socket_t *socket);
// vsfip_socket_set_netif MUST be called protected
extern void vsfip_socket_set_netif(vsfip_socket_t *socket, vsfip_netif_t *netif);
extern void vsfip_socket_cb(vsfip_socket_t *socket, void *param,
        void (*on_input)(void *, vsfip_netbuf_t *),
        void (*on_outputted)(void *));

// for UPD and TCP
extern vsf_err_t vsfip_listen(vsfip_socket_t *socket, uint_fast8_t backlog);
extern vsf_err_t vsfip_bind(vsfip_socket_t *socket, uint_fast16_t port);

// for TCP
extern void vsfip_tcp_config_window(vsfip_socket_t *socket, uint_fast32_t rx_window,
        uint_fast32_t tx_window);
extern vsf_err_t vsfip_tcp_async_send(vsfip_socket_t *socket, vsfip_netbuf_t *buf);
extern vsf_err_t vsfip_tcp_async_recv(vsfip_socket_t *socket, vsfip_netbuf_t **buf);

// for UDP
extern vsf_err_t vsfip_udp_async_send(vsfip_socket_t *socket, vsfip_sock_addr_t *sockaddr, vsfip_netbuf_t *buf);
extern vsf_err_t vsfip_udp_async_recv(vsfip_socket_t *socket, vsfip_sock_addr_t *sockaddr, vsfip_netbuf_t **buf);

// for proto
extern vsf_err_t vsfip_ip4_pton(vsfip_ipaddr_t *ip, char *domain);

#endif      // VSF_USE_TCPIP
#endif      // __VSFIP_H__
