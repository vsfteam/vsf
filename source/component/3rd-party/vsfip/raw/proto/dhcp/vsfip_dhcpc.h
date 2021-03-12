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

#ifndef __VSFIP_DHCPC_H__
#define __VSFIP_DHCPC_H__

/*============================ INCLUDES ======================================*/

#include "component/tcpip/vsf_tcpip_cfg.h"

#if VSF_USE_TCPIP == ENABLED && VSF_USE_VSFIP == ENABLED

#include "../../vsfip.h"
#include "./vsfip_dhcp_common.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsfip_dhcpc_t {
    vsfip_netif_t *netif;
    vsf_teda_t teda;
    vsfip_socket_t *so;
    vsfip_sock_addr_t sockaddr;
    vsfip_netbuf_t *outbuffer;
    vsfip_netbuf_t *inbuffer;
    vsfip_ipaddr_t ipaddr;
    vsfip_ipaddr_t gw;
    vsfip_ipaddr_t netmask;
    vsfip_ipaddr_t dns[2];
    uint32_t xid;
    uint32_t optlen;
    uint32_t retry;
    uint32_t arp_retry;
    uint32_t leasetime;
    uint32_t renew_time;
    uint32_t rebinding_time;
    unsigned ready : 1;
} vsfip_dhcpc_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

// vsfip_dhcpc_start MUST be called protected with netif
extern vsf_err_t vsfip_dhcpc_start(vsfip_netif_t*, vsfip_dhcpc_t*);
extern void vsfip_dhcpc_stop(vsfip_dhcpc_t *);

#endif      // VSF_USE_TCPIP
#endif      // __VSFIP_DHCPC_H__
