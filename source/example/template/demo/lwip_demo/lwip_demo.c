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

#if APP_USE_LWIP_DEMO == ENABLED && VSF_USE_LWIP == ENABLED

#include "../common/usrapp_common.h"
#include "shell/sys/linux/vsf_linux.h"
#include "shell/sys/linux/port/busybox/busybox.h"

#include "component/3rd-party/lwip/2.1.2/port/lwip_netdrv_adapter.h"

/* lwIP core includes */
#include "lwip/opt.h"

#include "lwip/sys.h"
#include "lwip/timeouts.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/init.h"
#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "lwip/api.h"

#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/dns.h"
#include "lwip/dhcp.h"
#include "lwip/prot/dhcp.h"
#include "lwip/autoip.h"

/* lwIP netif includes */
#include "lwip/etharp.h"
#include "netif/ethernet.h"

/* applications includes */

/*============================ MACROS ========================================*/

#if APP_USE_LINUX_DEMO != ENABLED
#   error lwip_demo runs under linux subsystem
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct usrapp_lwip_t {
    struct netif netif;
    struct dhcp netif_dhcp;
    vsf_eda_t *eda;
} usrapp_lwip_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static usrapp_lwip_t __usrapp_lwip;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void lwip_init_done(void * arg)
{
    usrapp_lwip_t *usrapp_lwip = arg;
    dhcp_set_struct(netif_default, &usrapp_lwip->netif_dhcp);
    netif_set_up(netif_default);

    vsf_trace(VSF_TRACE_INFO, "dhcpc: start" VSF_TRACE_CFG_LINEEND);
    dhcp_start(netif_default);
}

void vsf_pnp_on_netdrv_connect(vk_netdrv_t *netdrv)
{
    lwip_netif_set_netdrv(&__usrapp_lwip.netif, netdrv);
}

void vsf_pnp_on_netdrv_connected(vk_netdrv_t *netdrv)
{
    tcpip_init(lwip_init_done, &__usrapp_lwip);
}

static int __lwip_ping(int argc, char *argv[])
{
    // TODO:
    return 0;
}

static void dns_found(const char *name, const ip_addr_t *addr, void *arg)
{
    if (addr != NULL) {
        vsf_trace(VSF_TRACE_INFO, "address: %d.%d.%d.%d\n",
            (addr->u_addr.ip4.addr >> 0) & 0xFF, (addr->u_addr.ip4.addr >> 8) & 0xFF,
            (addr->u_addr.ip4.addr >> 16) & 0xFF, (addr->u_addr.ip4.addr >> 24) & 0xFF);
    } else {
        vsf_trace(VSF_TRACE_INFO, "fail to get ip for %s\n", name);
    }
    vsf_eda_post_evt(__usrapp_lwip.eda, VSF_EVT_USER);
}

static int __lwip_nslookup(int argc, char *argv[])
{
    ip_addr_t ipaddr;
    err_t err;

    if (argc != 2) {
        printf("format: %s HOST_NAME\n", argv[0]);
        return 0;
    }

    __usrapp_lwip.eda = vsf_eda_get_cur();

    LOCK_TCPIP_CORE();
        err = dns_gethostbyname(argv[1], &ipaddr, dns_found, NULL);
    UNLOCK_TCPIP_CORE();

    if (ERR_OK == err) {
        dns_found(argv[1], &ipaddr, NULL);
    }
    vsf_thread_wfe(VSF_EVT_USER);
    return 0;
}

int lwip_main(int argc, char *argv[])
{
    struct dhcp *dhcp = &__usrapp_lwip.netif_dhcp;
#if VSF_NETDRV_USE_WPCAP == ENABLED
    if (argc != 2) {
        printf("format: %s NETDRV_NAME\r\n", argv[0]);
        return -1;
    }
    vsf_err_t err = usrapp_net_common_init(argv[1]);
#else
    vsf_err_t err = usrapp_net_common_init();
#endif

    if (err != VSF_ERR_NONE) {
        printf("fail to initialize net" VSF_TRACE_CFG_LINEEND);
        return -1;
    }

    busybox_bind("/sbin/ping", __lwip_ping);
    busybox_bind("/sbin/nslookup", __lwip_nslookup);

    while (dhcp->state != DHCP_STATE_BOUND) {
        vsf_thread_delay_ms(10);
    }

    printf( "dhcpc:" VSF_TRACE_CFG_LINEEND
                "\tipaddr: %d.%d.%d.%d" VSF_TRACE_CFG_LINEEND
                "\tnetmask: %d.%d.%d.%d" VSF_TRACE_CFG_LINEEND
                "\tgateway: %d.%d.%d.%d" VSF_TRACE_CFG_LINEEND,
            (dhcp->offered_ip_addr.addr >> 0) & 0xFF, (dhcp->offered_ip_addr.addr >> 8) & 0xFF,
            (dhcp->offered_ip_addr.addr >> 16) & 0xFF, (dhcp->offered_ip_addr.addr >> 24) & 0xFF,
            (dhcp->offered_sn_mask.addr >> 0) & 0xFF, (dhcp->offered_sn_mask.addr >> 8) & 0xFF,
            (dhcp->offered_sn_mask.addr >> 16) & 0xFF, (dhcp->offered_sn_mask.addr >> 24) & 0xFF,
            (dhcp->offered_gw_addr.addr >> 0) & 0xFF, (dhcp->offered_gw_addr.addr >> 8) & 0xFF,
            (dhcp->offered_gw_addr.addr >> 16) & 0xFF, (dhcp->offered_gw_addr.addr >> 24) & 0xFF);
    return 0;
}

#endif      // APP_USE_TCPIP_DEMO && VSF_USE_VSFIP
