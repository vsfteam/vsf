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

#include "vsf_cfg.h"

#if AIC8800_APP_USE_WIFI_DEMO == ENABLED

#include <unistd.h>

#include "fhost_config.h"
#include "fhost.h"

#include "rwnx_defs.h"
#include "rwnx_msg_tx.h"
#include "wlan_if.h"
#include "sleep_api.h"

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

static int __wifi_connect_main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("format: %s SSID [PASSWD]\r\n", argv[0]);
        return -1;
    }

    if (wlan_connected) {
        printf("wlan already connected\r\n");
        return -1;
    }

    char *ssid = argv[1], *pass = argc >= 3 ? argv[2] : "";
    set_mac_address(NULL);
    // wlan_start_sta MUST be called with higher priority than internal wpa(vsf_prio_0).
    vsf_prio_t prio = vsf_thread_set_priority(vsf_prio_1);
        int ret = wlan_start_sta((uint8_t *)ssid, (uint8_t *)pass, 0);
    vsf_thread_set_priority(prio);

    wlan_connected = 0 == ret ? 1 : 0;
    if (wlan_connected) {
        printf("wifi connected.\r\n");
    } else {
        printf("fail to connect %s.\r\n", argv[1]);
    }

    return 0;
}

int fhost_application_init(void)
{
    busybox_bind("/sbin/wifi_ap", __wifi_ap_main);
    busybox_bind("/sbin/wifi_scan", __wifi_scan_main);
    busybox_bind("/sbin/wifi_connect", __wifi_connect_main);
    return 0;
}

void aic8800_wifi_start(void)
{
#if PLF_WIFI_STACK
    rwnx_ipc_init(&hw_env, &ipc_shared_env);
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
