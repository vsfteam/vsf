/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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

#include "component/3rd-party/lwip/port/lwip_netdrv_adapter.h"

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
#include "lwip/apps/mdns.h"

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
} usrapp_lwip_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static usrapp_lwip_t __usrapp_lwip;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void vsf_pnp_on_netdrv_prepare(vk_netdrv_t *netdrv)
{
    lwip_netif_set_netdrv(&__usrapp_lwip.netif, netdrv);
}

void vsf_pnp_on_netdrv_connected(vk_netdrv_t *netdrv)
{
    struct netif *netif = vk_netdrv_get_netif(netdrv);

    LOCK_TCPIP_CORE();
    dhcp_set_struct(netif, &__usrapp_lwip.netif_dhcp);
    netif_set_up(netif);
    netif_set_default(netif);

    dhcp_start(netif);
    UNLOCK_TCPIP_CORE();
    vsf_trace(VSF_TRACE_INFO, "dhcpc: start" VSF_TRACE_CFG_LINEEND);
}

static int __lwip_ping(int argc, char *argv[])
{
    // TODO:
    return 0;
}

static void dns_found(const char *name, const ip_addr_t *addr, void *arg)
{
    if (addr != NULL) {
#if LWIP_IPV4 && LWIP_IPV6
        vsf_trace(VSF_TRACE_INFO, "address: %d.%d.%d.%d\n",
            (addr->u_addr.ip4.addr >> 0) & 0xFF, (addr->u_addr.ip4.addr >> 8) & 0xFF,
            (addr->u_addr.ip4.addr >> 16) & 0xFF, (addr->u_addr.ip4.addr >> 24) & 0xFF);
#elif LWIP_IPV4
        vsf_trace(VSF_TRACE_INFO, "address: %d.%d.%d.%d\n",
            (addr->addr >> 0) & 0xFF, (addr->addr >> 8) & 0xFF,
            (addr->addr >> 16) & 0xFF, (addr->addr >> 24) & 0xFF);
#endif
    } else {
        vsf_trace(VSF_TRACE_INFO, "fail to get ip for %s\n", name);
    }
    if (arg != NULL) {
        vsf_eda_post_evt((vsf_eda_t *)arg, VSF_EVT_USER);
    }
}

static int __lwip_nslookup(int argc, char *argv[])
{
    ip_addr_t ipaddr;
    err_t err;

    if (argc != 2) {
        printf("format: %s HOST_NAME\n", argv[0]);
        return 0;
    }

    vsf_eda_t *eda = vsf_eda_get_cur();

    LOCK_TCPIP_CORE();
        err = dns_gethostbyname(argv[1], &ipaddr, dns_found, eda);
    UNLOCK_TCPIP_CORE();

    if (ERR_OK == err) {
        dns_found(argv[1], &ipaddr, NULL);
    } else {
        vsf_thread_wfe(VSF_EVT_USER);
    }
    return 0;
}

#if APP_USE_LINUX_DEMO == ENABLED && APP_USE_LINUX_HTTPD_DEMO == ENABLED
static void __mdns_httpd_srv_txt(struct mdns_service *service, void *txt_usrdata)
{
    mdns_resp_add_service_txtitem(service, "path=/", sizeof("path=/") - 1);
}
#endif

int lwip_main(int argc, char *argv[])
{
    struct dhcp *dhcp = &__usrapp_lwip.netif_dhcp;

    // for AIC8800, tcpip init is called in aic8800_demo_init by API from SDK
#ifndef __AIC8800__
    {
        static bool __is_lwip_inited = false;
        // tcpip_init MUST be called first,
        //  bacause netdrv callback will need buffer inistialized by tcpip_init.
        if (!__is_lwip_inited) {
            __is_lwip_inited = true;
            tcpip_init(NULL, NULL);
        }
    }
#endif

#if VSF_NETDRV_USE_WPCAP == ENABLED
    vsf_err_t err = usrapp_net_common_init(argc >= 2 ? argv[1] : NULL);
#else
    vsf_err_t err = usrapp_net_common_init();
#endif

    if (err != VSF_ERR_NONE) {
        printf("fail to initialize net" VSF_TRACE_CFG_LINEEND);
        return -1;
    }

    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/ping", __lwip_ping);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/nslookup", __lwip_nslookup);

    while (dhcp->state != DHCP_STATE_BOUND) {
        vsf_thread_delay_ms(10);
    }

    // setup mdns
    LOCK_TCPIP_CORE();
    mdns_resp_init();
    if (ERR_OK == mdns_resp_add_netif(&__usrapp_lwip.netif, "vsf", 60 * 10)) {
#if APP_USE_LINUX_DEMO == ENABLED && APP_USE_LINUX_HTTPD_DEMO == ENABLED
        mdns_resp_add_service(&__usrapp_lwip.netif, "vsfweb", "_http",
            DNSSD_PROTO_TCP, 80, 3600, __mdns_httpd_srv_txt, NULL);
#endif
    }
    UNLOCK_TCPIP_CORE();

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
