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

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_TCPIP == ENABLED

#define VSF_NETDRV_INHERIT_NETIF
#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct vsfip_eth_head_t {
    uint8_t dst_addr[VSFIP_ETH_ADDRLEN];
    uint8_t src_addr[VSFIP_ETH_ADDRLEN];
    uint16_t type;
} PACKED;
typedef struct vsfip_eth_head_t vsfip_eth_head_t;

enum vsfip_eth_type_t {
    VSFIP_ETH_TYPE_IP       = 0x0800,
    VSFIP_ETH_TYPE_IP6      = 0x86DD,
    VSFIP_ETH_TYPE_ARP      = 0x0806,
    VSFIP_ETH_TYPE_RARP     = 0x0835,
};
typedef enum vsfip_eth_type_t vsfip_eth_type_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

bool vsfip_eth_routable(vsfip_netif_t *netif, const vsfip_ipaddr_t *dest_addr)
{
    return (dest_addr->size == 4) && (dest_addr->addr_buf[0] != 127);
}

static vsf_err_t vsfip_eth_header(vsfip_netbuf_t *netbuf,
        vsfip_netif_proto_t proto, const vsfip_macaddr_t *dest_addr)
{
    vsfip_netif_t *netif = netbuf->netif;
    vsfip_eth_head_t *head;

    head = (vsfip_eth_head_t *)vsfip_netbuf_header(netbuf, sizeof(vsfip_eth_head_t));
    if (NULL == head) {
        return VSF_ERR_FAIL;
    }

    memcpy(head->dst_addr, dest_addr->addr_buf, VSFIP_ETH_ADDRLEN);
    memcpy(head->src_addr, netif->netdrv->macaddr.addr_buf, VSFIP_ETH_ADDRLEN);
    head->type = cpu_to_be16(proto);
    return VSF_ERR_NONE;
}

static bool vsfip_eth_can_output(vsfip_netif_t *netif)
{
    return vsf_netdrv_can_output(netif->netdrv);
}

static vsf_err_t vsfip_eth_output(vsfip_netbuf_t *netbuf)
{
    vsfip_netif_t *netif = netbuf->netif;
    return vsf_netdrv_output(netif->netdrv, netbuf);
}

static void vsfip_eth_input(vsfip_netbuf_t *netbuf)
{
    vsfip_eth_head_t *head = netbuf->buf.pObj;

    if (!vsfip_netbuf_header(netbuf, -sizeof(vsfip_eth_head_t))) {
        vsfip_netbuf_deref(netbuf);
    }

    switch (be16_to_cpu(head->type)) {
    case VSFIP_ETH_TYPE_IP:
        vsfip_netif_ip4_input(netbuf);
        break;
    case VSFIP_ETH_TYPE_IP6:
        vsfip_netif_ip6_input(netbuf);
        break;
    case VSFIP_ETH_TYPE_ARP:
        vsfip_netif_arp_input(netbuf);
        break;
    default:
    case VSFIP_ETH_TYPE_RARP:
        // not supported
        vsfip_netbuf_deref(netbuf);
    }
}

const vsfip_netif_op_t vsfip_eth_op = {
    .header     = vsfip_eth_header,
    .input      = vsfip_eth_input,
    .can_output = vsfip_eth_can_output,
    .output     = vsfip_eth_output,
    .routable   = vsfip_eth_routable,
};

#endif      // VSF_USE_TCPIP
