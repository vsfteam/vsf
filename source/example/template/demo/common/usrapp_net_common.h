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

#ifndef __USRAPP_NET_COMMON_H__
#define __USRAPP_NET_COMMON_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"

#if VSF_USE_TCPIP == ENABLED

#include "component/tcpip/netdrv/vsf_netdrv.h"

#if VSF_USE_VSFIP == ENABLED
#   include "component/3rd-party/vsfip/raw/vsfip.h"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if VSF_NETDRV_USE_WPCAP == ENABLED
typedef struct usrapp_net_common_t {
#   if VSF_NETDRV_USE_WPCAP == ENABLED
    vk_netdrv_wpcap_t netdrv;
#   endif
} usrapp_net_common_t;
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_NETDRV_USE_WPCAP == ENABLED
extern usrapp_net_common_t usrapp_net_common;
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t usrapp_net_common_init(
#if VSF_NETDRV_USE_WPCAP == ENABLED
    char *name
#else
    void
#endif
);

#endif      // VSF_USE_TCPIP
#endif      // __USRAPP_NET_COMMON_H__
/* EOF */