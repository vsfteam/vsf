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

#include "vsf_cfg.h"

#if AIC8800_APP_USE_WIFI_DEMO == ENABLED

#include <unistd.h>
#include <stdio.h>

#include "net_al.h"

#include "fhost_config.h"
#include "fhost.h"

#include "rwnx_defs.h"
#include "rwnx_msg_tx.h"
#include "wlan_if.h"
#include "sleep_api.h"

#include "lwip/apps/mdns.h"

/*============================ MACROS ========================================*/

#if __VSF_HAL_SWI_NUM > 0
#   define MFUNC_IN_U8_DEC_VALUE            (__VSF_HAL_SWI_NUM)
#   include "utilities/preprocessor/mf_u8_dec2str.h"
#   define __FHOST_IPC_IRQ_PRIO             MFUNC_OUT_DEC_STR
#else
#   define __FHOST_IPC_IRQ_PRIO             0
#endif

#if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY != ENABLED
#   error "current demo need VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY, if it\'s not supported\
    please make sure wlan_start_sta is called in task with priority higher than(>) vsf_prio_0"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

struct rwnx_hw hw_env;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static int __wifi_ap_main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("format: %s SSID PASSWD [CHANNEL]\r\n", argv[0]);
        return -1;
    }

    char *ssid = argv[1], *pass = argv[2];
    uint8_t channel = 0;
    if (argc > 3) {
        channel = strtoul(argv[3], NULL, 10);
        VSF_ASSERT(channel <= 14);

        if (14 == channel) {
            printf("warning: wifi 2.4G channel 14 is illegal in some countries.\r\n");
        }
    }
    set_ap_channel_num(channel);
    set_mac_address(NULL);
    int ret = wlan_start_ap(0, (uint8_t *)ssid, (uint8_t *)pass);
    if (!ret) {
        printf("wifi ap started.\r\n");
    } else {
        printf("fail to start wifi ap.\r\n");
    }
    return 0;
}

static int __wifi_scan_main(int argc, char *argv[])
{
    int fhost_vif_idx = 0;
    ipc_host_cntrl_start();

    struct fhost_cntrl_link *cntrl_link = fhost_cntrl_cfgrwnx_link_open();
    if (cntrl_link == NULL) {
        printf("fail to open link\r\n");
        return -1;
    }
    if (fhost_set_vif_type(cntrl_link, fhost_vif_idx, VIF_UNKNOWN, false) ||
        fhost_set_vif_type(cntrl_link, fhost_vif_idx, VIF_STA, false)) {
        fhost_cntrl_cfgrwnx_link_close(cntrl_link);
        printf("fail to set link type to sta\r\n");
        return -1;
    }

    int nb_res = fhost_scan(cntrl_link, fhost_vif_idx, NULL);
    printf("%d scan results:\r\n", nb_res);

    nb_res = 0;
    struct mac_scan_result result;
    while (fhost_get_scan_results(cntrl_link, nb_res++, 1, &result)) {
        result.ssid.array[result.ssid.length] = '\0'; // set ssid string ending
        printf("(%3d dBm) CH=%3d BSSID=%02x:%02x:%02x:%02x:%02x:%02x SSID=%s\r\n",
            (int8_t)result.rssi, phy_freq_to_channel(result.chan->band, result.chan->freq),
            ((uint8_t *)result.bssid.array)[0], ((uint8_t *)result.bssid.array)[1],
            ((uint8_t *)result.bssid.array)[2], ((uint8_t *)result.bssid.array)[3],
            ((uint8_t *)result.bssid.array)[4], ((uint8_t *)result.bssid.array)[5],
            (char *)result.ssid.array);
    }
    fhost_cntrl_cfgrwnx_link_close(cntrl_link);

    return 0;
}

#if APP_USE_LINUX_DEMO == ENABLED && APP_USE_LINUX_HTTPD_DEMO == ENABLED
static void __mdns_httpd_srv_txt(struct mdns_service *service, void *txt_usrdata)
{
    mdns_resp_add_service_txtitem(service, "path=/", sizeof("path=/") - 1);
}
#endif

static int __wifi_connect_main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("format: %s SSID [PASSWD]\r\n", argv[0]);
        return -1;
    }

    if (wlan_get_connect_status()) {
#if APP_USE_LINUX_DEMO == ENABLED && APP_USE_LINUX_HTTPD_DEMO == ENABLED
        net_if_t *netif = fhost_to_net_if(0);
        LOCK_TCPIP_CORE();
            mdns_resp_remove_netif(netif);
        UNLOCK_TCPIP_CORE();
#endif
    }

    char *ssid = argv[1], *pass = argc >= 3 ? argv[2] : "";
    set_mac_address(NULL);
    // wlan_start_sta MUST be called with higher priority than internal wpa(AIC8800_OSAL_CFG_PRIORITY_BASE).
    vsf_prio_t prio = vsf_thread_set_priority(AIC8800_OSAL_CFG_PRIORITY_BASE + 1);
        wlan_start_sta((uint8_t *)ssid, (uint8_t *)pass, 0);
    vsf_thread_set_priority(prio);

    if (wlan_get_connect_status()) {
        printf("wifi connected.\r\n");

#if APP_USE_LINUX_DEMO == ENABLED && APP_USE_LINUX_HTTPD_DEMO == ENABLED
        // start mdns, not ready
        net_if_t *netif = fhost_to_net_if(0);
        LOCK_TCPIP_CORE();
            mdns_resp_init();
            if (ERR_OK == mdns_resp_add_netif(netif, "vsf", 60 * 10)) {
#if APP_USE_LINUX_DEMO == ENABLED && APP_USE_LINUX_HTTPD_DEMO == ENABLED
                mdns_resp_add_service(netif, "vsfweb", "_http",
                    DNSSD_PROTO_TCP, 80, 3600, __mdns_httpd_srv_txt, NULL);
#endif
            }
        UNLOCK_TCPIP_CORE();
#endif
        return 0;
    } else {
        printf("fail to connect %s.\r\n", argv[1]);
        return -1;
    }
}

int fhost_application_init(void)
{
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/wifi_ap", __wifi_ap_main);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/wifi_scan", __wifi_scan_main);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/wifi_connect", __wifi_connect_main);
    return 0;
}

void aic8800_wifi_start(void)
{
#if PLF_WIFI_STACK
#   ifdef CONFIG_RWNX_LWIP
#       ifndef CFG_WIFI_RAM_VER
    wifi_patch_prepare();
#       endif
    rwnx_ipc_init(&hw_env, &ipc_shared_env);
#   endif
    net_init();

    {
        fhost_user_cfg_t cfg = {
            .ipc_irq_prio       = VSF_MCONNECT(vsf_arch_prio_, __FHOST_IPC_IRQ_PRIO),
        };
        fhost_init(&cfg);
    }
#endif
}

#endif      // APP_USE_LINUX_DEMO

/* EOF */
