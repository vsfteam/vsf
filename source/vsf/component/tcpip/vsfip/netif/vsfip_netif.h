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

#ifndef __VSFIP_NETIF_H__
#define __VSFIP_NETIF_H__

/*============================ INCLUDES ======================================*/

#include "component/tcpip/vsf_tcpip_cfg.h"

#if VSF_USE_TCPIP == ENABLED

#include "../vsfip.h"

#if     defined(VSFIP_NETIF_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(VSFIP_NETIF_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#ifndef VSFIP_CFG_ARP_CACHE_SIZE
#   define VSFIP_CFG_ARP_CACHE_SIZE         8
#endif
#ifndef VSFIP_CFG_ARP_TIMEOUT
#   define VSFIP_CFG_ARP_TIMEOUT            1000
#endif
#ifndef VSFIP_CFG_ARP_RETRY
#   define VSFIP_CFG_ARP_RETRY              3
#endif

#define VSFIP_ARP_MAXDELAY                  (VSFIP_CFG_ARP_RETRY * VSFIP_CFG_ARP_TIMEOUT)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vsfip_netif_t)

enum vsfip_netif_proto_t {
    VSFIP_NETIF_PROTO_IP = 0x0800,
    VSFIP_NETIF_PROTO_ARP = 0x0806,
    VSFIP_NETIF_PROTO_RARP = 0x8035
};
typedef enum vsfip_netif_proto_t vsfip_netif_proto_t;

struct vsfip_arp_entry_t {
    vsfip_ipmac_assoc assoc;
    uint32_t time;
};
typedef struct vsfip_arp_entry_t vsfip_arp_entry_t;

struct vsfip_netif_op_t {
    vsf_err_t (*header)(vsfip_netbuf_t *netbuf, vsfip_netif_proto_t proto, const vsfip_macaddr_t *dest);
    bool (*can_output)(vsfip_netif_t *netif);
    vsf_err_t (*output)(vsfip_netbuf_t *netbuf);
    void (*input)(vsfip_netbuf_t *netbuf);
    bool (*routable)(vsfip_netif_t *netif, const vsfip_ipaddr_t *dest_addr);
};
typedef struct vsfip_netif_op_t vsfip_netif_op_t;

struct vsfip_netif_arpc_t {
    vsf_teda_t teda;
    vsf_sem_t sem;
    vsf_slist_queue_t request_queue;
    vsfip_netbuf_t *cur_netbuf, *cur_request;
    vsfip_ipaddr_t ip_for_mac;
    uint32_t retry;
};
typedef struct vsfip_netif_arpc_t vsfip_netif_arpc_t;

def_simple_class(vsfip_netif_t) {
    vsf_slist_node_t node;

    const vsfip_netif_op_t *op;
    vsf_netdrv_t *netdrv;

    vsfip_ipaddr_t ip4addr;
    vsfip_ipaddr_t netmask;
    vsfip_ipaddr_t gateway;
    vsfip_ipaddr_t dns[2];

    // output bufferlist and semaphore
    vsf_slist_queue_t output_queue;
    vsf_sem_t output_sem;
    vsf_eda_t output_eda;

    // arp client
    vsfip_netif_arpc_t arpc;

    uint8_t connected   : 1;
    uint8_t quit        : 1;
    uint8_t ref;
    uint16_t arp_time;
    vsfip_arp_entry_t arp_cache[VSFIP_CFG_ARP_CACHE_SIZE];

    // for DHCP
    union {
        void *dhcpc;
        void *dhcpd;
    } dhcp;
};

/*============================ INCLUDES ======================================*/
#include "./vsfip_netdrv_adapter.h"

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsfip_netif_init(vsfip_netif_t *netif);
extern void vsfip_netif_fini(vsfip_netif_t *netif);

extern uint_fast16_t vsfip_netif_mtu(vsfip_netif_t *netif);

extern vsf_err_t vsfip_netif_ip_output(vsfip_netbuf_t *netbuf, bool urgent);

extern void vsfip_netif_ref(vsfip_netif_t *netif);
extern void vsfip_netif_deref(vsfip_netif_t *netif);

extern void vsfip_netif_ip4_input(vsfip_netbuf_t *netbuf);
extern void vsfip_netif_ip6_input(vsfip_netbuf_t *netbuf);
extern void vsfip_netif_arp_input(vsfip_netbuf_t *netbuf);

extern void vsfip_netif_arp_add_assoc(vsfip_netif_t *netif, uint_fast8_t hwlen,
        uint8_t *hwaddr, uint_fast8_t protolen, uint8_t *protoaddr);

// for netbuf adapter
extern void vsfip_netif_on_outputted(vsfip_netif_t *netif, vsfip_netbuf_t *netbuf, vsf_err_t err);
extern void vsfip_netif_on_inputted(vsfip_netif_t *netif, vsfip_netbuf_t *netbuf);

#endif      // VSF_USE_TCPIP
#endif      // __VSFIP_NETIF_H__
