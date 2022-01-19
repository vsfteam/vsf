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
        // for __WIN__ mac is initialized by pcap_oid_get_request
#       if !defined(__WIN__) && defined(APP_NETDRV_WPCAP_CFG_MAC)
        .macaddr.addr_buf   = {APP_NETDRV_WPCAP_CFG_MAC},
#       endif
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

    vk_netdrv_prepare((vk_netdrv_t *)&usrapp_net_common.netdrv);
    err = vk_netdrv_connect((vk_netdrv_t *)&usrapp_net_common.netdrv);

#endif
    return err;
}

#endif      // VSF_USE_TCPIP
/* EOF */