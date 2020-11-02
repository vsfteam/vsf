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

#ifndef __VSF_NETDRV_WPCAP_H__
#define __VSF_NETDRV_WPCAP_H__

/*============================ INCLUDES ======================================*/

#include "../../../vsf_tcpip_cfg.h"

#if VSF_USE_TCPIP == ENABLED && VSF_NETDRV_USE_WPCAP == ENABLED

#if     defined(__VSF_NETDRV_WPCAP_CLASS_IMPLEMENT)
#   undef __VSF_NETDRV_WPCAP_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_simple_class(vk_netdrv_wpcap_t)

def_simple_class(vk_netdrv_wpcap_t) {
    public_member(
        implement(vk_netdrv_t)
        char *name;
    )
    private_member(
        vsf_arch_irq_thread_t irq_thread;
        void *fp;               // actually pcap_t *
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const struct vk_netlink_op_t vk_netdrv_wpcap_netlink_op;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_TCPIP && VSF_NETDRV_USE_WPCAP
#endif      // __VSF_MEMFS_H__
