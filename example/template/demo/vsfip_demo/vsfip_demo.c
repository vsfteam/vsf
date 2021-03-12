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

#if APP_USE_VSFIP_DEMO == ENABLED && VSF_USE_VSFIP == ENABLED

#include "../common/usrapp_common.h"

#include "component/3rd-party/vsfip/raw/vsfip.h"
#include "component/3rd-party/vsfip/raw/proto/dhcp/vsfip_dhcpc.h"
#include "component/3rd-party/vsfip/raw/proto/dns/vsfip_dnsc.h"

#include "shell/sys/linux/vsf_linux.h"
#include "shell/sys/linux/port/busybox/busybox.h"

/*============================ MACROS ========================================*/

#if APP_USE_LINUX_DEMO != ENABLED
#   error vsfip_demo runs under linux subsystem
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct usrapp_vsfip_t {
    vsfip_netif_t netif;
    vsfip_dhcpc_t dhcp_client;
    vsfip_ipaddr_t ip_addr;
    vsf_eda_t *eda;
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

void vsf_pnp_on_netdrv_connect(vk_netdrv_t *netdrv)
{
    __usrapp_vsfip.netif.op = &vsfip_eth_op;
    vsfip_netif_set_netdrv(&__usrapp_vsfip.netif, netdrv);
}

void vsf_pnp_on_netdrv_connected(vk_netdrv_t *netdrv)
{
    vsf_trace(VSF_TRACE_INFO, "dhcpc: start" VSF_TRACE_CFG_LINEEND);
    vsfip_dhcpc_start(netdrv->adapter.netif, &__usrapp_vsfip.dhcp_client);
}

void vsfip_dhcpc_on_finish(vsfip_dhcpc_t *dhcpc)
{
    if (dhcpc->ready) {
        vsf_trace(VSF_TRACE_INFO, 
                    "dhcpc:" VSF_TRACE_CFG_LINEEND
                        "\tipaddr: %d.%d.%d.%d" VSF_TRACE_CFG_LINEEND
                        "\tnetmask: %d.%d.%d.%d" VSF_TRACE_CFG_LINEEND
                        "\tgateway: %d.%d.%d.%d" VSF_TRACE_CFG_LINEEND
                        "\tdns: %d.%d.%d.%d" VSF_TRACE_CFG_LINEEND,
                    dhcpc->ipaddr.addr_buf[0], dhcpc->ipaddr.addr_buf[1],
                    dhcpc->ipaddr.addr_buf[2], dhcpc->ipaddr.addr_buf[3],
                    dhcpc->netmask.addr_buf[0], dhcpc->netmask.addr_buf[1],
                    dhcpc->netmask.addr_buf[2], dhcpc->netmask.addr_buf[3],
                    dhcpc->gw.addr_buf[0], dhcpc->gw.addr_buf[1],
                    dhcpc->gw.addr_buf[2], dhcpc->gw.addr_buf[3],
                    dhcpc->dns[0].addr_buf[0], dhcpc->dns[0].addr_buf[1],
                    dhcpc->dns[0].addr_buf[2], dhcpc->dns[0].addr_buf[3]);

        if (dhcpc->dns[0].size > 0) {
            vsfip_dnsc_set_server(0, &dhcpc->dns[0]);
        }
        if (dhcpc->dns[1].size > 0) {
            vsfip_dnsc_set_server(0, &dhcpc->dns[1]);
        }
    } else {
        vsf_trace(VSF_TRACE_INFO, "dhcpc: failed" VSF_TRACE_CFG_LINEEND);
    }
    vsf_eda_post_evt(__usrapp_vsfip.eda, VSF_EVT_USER);
}

static int __vsfip_ping(int argc, char *argv[])
{
    // TODO:
    return 0;
}

static int __vsfip_nslookup(int argc, char *argv[])
{
    vsfip_addr_t domain_ip;
    vsf_err_t err;

    if (argc != 2) {
        printf("format: %s HOST_NAME\n", argv[0]);
        return 0;
    }

    vsfip_gethostbyname(argv[1], &domain_ip);
    err = vsf_eda_get_return_value();
    if (VSF_ERR_NONE == err) {
        printf("address: %d.%d.%d.%d\n",
            domain_ip.addr_buf[0], domain_ip.addr_buf[1],
            domain_ip.addr_buf[2], domain_ip.addr_buf[3]);
    } else {
        printf("fail to get ip for %s\n", argv[1]);
    }
    return 0;
}

int vsfip_main(int argc, char *argv[])
{
    if (__usrapp_vsfip.eda != NULL) {
        printf("vsfip already started\n");
        return -1;
    }

    __usrapp_vsfip.eda = vsf_eda_get_cur();
    vsfip_init();
    vsfip_dnsc_init();

#if VSF_NETDRV_USE_WPCAP == ENABLED
    if (argc != 2) {
        printf("format: %s NETDRV_NAME\r\n", argv[0]);
        return -1;
    }
    usrapp_net_common_init(argv[1]);
#else
    usrapp_net_common_init();
#endif

    busybox_bind("/sbin/ping", __vsfip_ping);
    busybox_bind("/sbin/nslookup", __vsfip_nslookup);

    // wait for dhcp ready
    vsf_thread_wfe(VSF_EVT_USER);
    return 0;
}

#endif      // APP_USE_TCPIP_DEMO && VSF_USE_VSFIP
