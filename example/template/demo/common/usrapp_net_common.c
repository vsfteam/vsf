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

#define __VSF_NETDRV_CLASS_INHERIT_NETIF__
#include "./usrapp_net_common.h"

#if VSF_USE_TCPIP == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_NETDRV_USE_WPCAP == ENABLED
usrapp_net_common_t usrapp_net_common = {
#   if VSF_NETDRV_USE_WPCAP == ENABLED
    .netdrv                 = {
        .macaddr.size       = TCPIP_ETH_ADDRLEN,
        .macaddr.addr_buf   = {APP_NETDRV_WPCAP_CFG_MAC},
        .mac_header_size    = TCPIP_ETH_HEADSIZE,
        .mtu                = 1500 + TCPIP_ETH_HEADSIZE,
        .hwtype             = TCPIP_ETH_HWTYPE,
    },
#   endif
};
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t usrapp_net_common_init(
#if VSF_NETDRV_USE_WPCAP == ENABLED
    char *name
#else
    void
#endif
)
{
    vsf_err_t err = VSF_ERR_NONE;

#if VSF_NETDRV_USE_WPCAP == ENABLED

#   if VSF_NETDRV_USE_WPCAP == ENABLED
    usrapp_net_common.netdrv.name = name;
    vk_netdrv_set_netlink_op((vk_netdrv_t *)&usrapp_net_common.netdrv, &vk_netdrv_wpcap_netlink_op, NULL);
#   endif

    err = vk_netdrv_connect((vk_netdrv_t *)&usrapp_net_common.netdrv);

#endif
    return err;
}

#endif      // VSF_USE_TCPIP
/* EOF */