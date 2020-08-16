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

#ifndef __VSF_EDA_SLIST_QUEUE_H__
#define __VSF_EDA_SLIST_QUEUE_H__

/*============================ INCLUDES ======================================*/
#include "kernel/vsf_kernel_cfg.h"

#if VSF_USE_KERNEL == ENABLED

#if     defined(__VSF_QUEUE_CLASS_IMPLEMENT)
#   undef __VSF_QUEUE_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__VSF_QUEUE_CLASS_INHERIT)
#   undef __VSF_QUEUE_CLASS_INHERIT
#   define __PLOOC_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_SUPPORT_MSG_QUEUE == ENABLED

#   define vsf_msg_queue_t              vsf_eda_slist_queue_t
#   define vsf_eda_msg_queue_init(__queue, __max)                               \
                vsf_eda_slist_queue_init((__queue), (__max))
#   define vsf_eda_msg_queue_send(__queue, __node, __timeout)                   \
                vsf_eda_queue_send((vsf_eda_queue_t *)(__queue), (__node), (__timeout))
#   define vsf_eda_msg_queue_send_get_reason(__queue, __evt, __node)            \
                vsf_eda_queue_send_get_reason((vsf_eda_queue_t *)(__queue), (__evt), (__node))
#   define vsf_eda_msg_queue_recv(__queue, __node, __timeout)                   \
                vsf_eda_queue_recv((vsf_eda_queue_t *)(__queue), (void **)(__node), (__timeout))
#   define vsf_eda_msg_queue_recv_get_reason(__queue, __evt, __node)            \
                vsf_eda_queue_recv_get_reason((vsf_eda_queue_t *)(__queue), (__evt), (void **)(__node))

#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if __VSF_KERNEL_CFG_SUPPORT_EDA_SLIST_QUEUE == ENABLED

declare_simple_class(vsf_eda_slist_queue_t)

def_simple_class(vsf_eda_slist_queue_t) {
    public_member(
        implement(vsf_eda_queue_t)
    )
    private_member(
        vsf_slist_queue_t queue;
    )
};

#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if __VSF_KERNEL_CFG_SUPPORT_EDA_SLIST_QUEUE == ENABLED
extern vsf_err_t vsf_eda_slist_queue_init(vsf_eda_slist_queue_t *pthis, uint_fast16_t max);
#endif

#endif
#endif      // __VSF_EDA_SLIST_QUEUE_H__
