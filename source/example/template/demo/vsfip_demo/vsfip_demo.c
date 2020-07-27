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

#include "vsf.h"

#if APP_CFG_USE_TCPIP_DEMO == ENABLED

#include "component/3rd-party/vsfip/raw/vsfip.h"
#include "component/3rd-party/vsfip/raw/proto/dhcp/vsfip_dhcpc.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct usrapp_vsfip_t {
    vsfip_netif_t *netif;
    vsfip_dhcpc_t dhcp_client;
    vsfip_ipaddr_t ip_addr;
} usrapp_vsfip_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static usrapp_vsfip_t __usrapp_vsfip;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsfip_socket_t * vsfip_mem_socket_get(void)
{
    return (vsfip_socket_t *)vsf_heap_malloc(sizeof(vsfip_socket_t));
}

void vsfip_mem_socket_free(vsfip_socket_t *socket)
{
    vsf_heap_free(socket);
}

vsfip_tcp_pcb_t * vsfip_mem_tcp_pcb_get(void)
{
    return (vsfip_tcp_pcb_t *)vsf_heap_malloc(sizeof(vsfip_tcp_pcb_t));
}

void vsfip_mem_tcp_pcb_free(vsfip_tcp_pcb_t *tcp_pcb)
{
    vsf_heap_free(tcp_pcb);
}

vsfip_netbuf_t * vsfip_mem_netbuf_get(uint_fast32_t size)
{
    vsfip_netbuf_t *netbuf = vsf_heap_malloc(sizeof(vsfip_netbuf_t) + size);
    if (netbuf != NULL) {
        netbuf->buffer = (uint8_t *)&netbuf[1];
    }
    return netbuf;
}

void vsfip_mem_netbuf_free(vsfip_netbuf_t *netbuf)
{
    vsf_heap_free(netbuf);
}

#endif      // APP_CFG_USE_TCPIP_DEMO
