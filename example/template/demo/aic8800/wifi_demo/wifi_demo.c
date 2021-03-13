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

#include "fhost.h"
#include "rwnx_defs.h"
#include "rwnx_msg_tx.h"
#include "wlan_if.h"
#include "sleep_api.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

struct rwnx_hw hw_env;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static int __wifi_scan_main(int argc, char *argv[])
{
    struct fhost_vif_tag *fhost_vif = &fhost_env.vif[0];

    for (int idx = 0; idx < NX_VIRT_DEV_MAX; idx++, fhost_vif++) {
        uint8_t mac_addr1 = fhost_vif->mac_addr.array[0] >> 8;
        uint32_t ip_mask = 0x00FFFFFF;
        uint32_t ip_addr = (192 | (168 << 8) | ((241 + idx) << 16) | ((70 + mac_addr1) << 24));

        net_if_set_ip(&fhost_vif->net_if, ip_addr, ip_mask, 0);
    }

    fhost_vif = &fhost_env.vif[0];

    //set_mac_address();
    ipc_host_cntrl_start();

    struct mm_add_if_cfm add_if_cfm;
    rwnx_send_add_if((unsigned char *)&(fhost_vif->mac_addr.array), VIF_STA, 0, &add_if_cfm);

    //dbg("vif_init %d, %d\r\n", add_if_cfm.status, add_if_cfm.inst_nbr);
    vif_info_tab[add_if_cfm.inst_nbr].active = true;
    vif_info_tab[add_if_cfm.inst_nbr].type = VIF_STA;
    fhost_vif->mac_vif = &vif_info_tab[add_if_cfm.inst_nbr];
    fhost_env.mac2fhost_vif[add_if_cfm.inst_nbr] = fhost_vif;
    MAC_ADDR_CPY(&(vif_info_tab[add_if_cfm.inst_nbr].mac_addr), &(fhost_vif->mac_addr));

    struct fhost_cntrl_link *cntrl_link = fhost_cntrl_cfgrwnx_link_open();
    if (cntrl_link == NULL) {
        vsf_trace_error("Failed to open link with control task\n");
        return -1;
    }

    int nb_res = fhost_scan(cntrl_link, 0, NULL);
    vsf_trace_info("Got %d scan results\n", nb_res);

    nb_res = 0;
    struct mac_scan_result result;
    while (fhost_get_scan_results(cntrl_link, nb_res++, 1, &result)) {
        result.ssid.array[result.ssid.length] = '\0'; // set ssid string ending
        vsf_trace_info("(%3d dBm) CH=%3d BSSID=%02x:%02x:%02x:%02x:%02x:%02x SSID=%s\n",
            (int8_t)result.rssi, phy_freq_to_channel(result.chan->band, result.chan->freq),
            ((uint8_t *)result.bssid.array)[0], ((uint8_t *)result.bssid.array)[1],
            ((uint8_t *)result.bssid.array)[2], ((uint8_t *)result.bssid.array)[3],
            ((uint8_t *)result.bssid.array)[4], ((uint8_t *)result.bssid.array)[5],
            (char *)result.ssid.array);
    }

    return 0;
}

static int __wifi_connect_main(int argc, char *argv[])
{
    if (!wlan_connected) {
        char *ssid = "", *pw = "passwd";

#if PLF_HW_PXP
        rtos_task_suspend(5);   // wait for AP starting
#endif
        if (0 == wlan_start_sta((uint8_t *)ssid, (uint8_t *)pw, 0)) {
            wlan_connected = 1;
        }

#if CONFIG_SLEEP_LEVEL == 1
        sleep_level_set(PM_LEVEL_LIGHT_SLEEP);
#elif CONFIG_SLEEP_LEVEL == 2
        sleep_level_set(PM_LEVEL_DEEP_SLEEP);
#elif CONFIG_SLEEP_LEVEL == 3
        sleep_level_set(PM_LEVEL_HIBERNATE);
#endif
        user_sleep_allow(1);
    }

    return 0;
}

int fhost_application_init(void)
{
    busybox_bind("/sbin/wifi_scan", __wifi_scan_main);
    busybox_bind("/sbin/wifi_connect", __wifi_connect_main);
    return 0;
}

void aic8800_wifi_start(void)
{
#if PLF_WIFI_STACK
    rwnx_ipc_init(&hw_env, &ipc_shared_env);
    net_init();
    fhost_init();
#endif
}

#endif      // APP_USE_LINUX_DEMO

/* EOF */
