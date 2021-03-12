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

#ifndef __VSFIP_DHCPD_H__
#define __VSFIP_DHCPD_H__

/*============================ INCLUDES ======================================*/

#include "component/tcpip/vsf_tcpip_cfg.h"

#if VSF_USE_TCPIP == ENABLED && VSF_USE_VSFIP == ENABLED

#include "../../vsfip.h"
#include "./vsfip_dhcp_common.h"

/*============================ MACROS ========================================*/

#ifndef VSFIP_CFG_DHCPD_ASSOCNUM
#   define VSFIP_CFG_DHCPD_ASSOCNUM         2
#endif
#ifndef VSFIP_CFG_DOMAIN
#   define VSFIP_CFG_DOMAIN                 "vsfip.net"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsfip_dhcpd_t {
    vsfip_netif_t *netif;

    // private
    vsfip_socket_t *so;
    vsfip_sock_addr_t sockaddr;
    vsfip_ipmac_assoc assoc[VSFIP_CFG_DHCPD_ASSOCNUM];
    uint32_t optlen;
    uint32_t alloc_idx;
} vsfip_dhcpd_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

// vsfip_dhcpd_start MUST be called protected with netif
extern vsf_err_t vsfip_dhcpd_start(vsfip_netif_t*, vsfip_dhcpd_t*);
extern void vsfip_dhcpd_stop(vsfip_dhcpd_t *);

#endif      // VSF_USE_TCPIP
#endif      // __VSFIP_DHCPD_H__
