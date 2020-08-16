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

#include "kernel/vsf_kernel_cfg.h"

#if VSF_USE_KERNEL == ENABLED

#include "./vsf_kernel_common.h"
#include "./vsf_eda.h"

#define __VSF_EDA_SLIST_QUEUE_CLASS_IMPLEMENT
#include "./vsf_eda_slist_queue.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if __VSF_KERNEL_CFG_SUPPORT_EDA_SLIST_QUEUE == ENABLED
typedef struct __vsf_eda_slist_node_wrapper {
    implement_ex(vsf_slist_node_t, node)
} __vsf_eda_slist_node_wrapper;
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if __VSF_KERNEL_CFG_SUPPORT_EDA_SLIST_QUEUE == ENABLED

static bool __vsf_eda_slist_queue_enqueue(vsf_eda_queue_t *this_ptr, void *node)
{
    __vsf_eda_slist_node_wrapper *slist_node = node;
    vsf_slist_init_node(__vsf_eda_slist_node_wrapper, node, slist_node);
    vsf_slist_queue_enqueue(__vsf_eda_slist_node_wrapper, node,
                            &((vsf_eda_slist_queue_t *)this_ptr)->queue,
                            (__vsf_eda_slist_node_wrapper *)node);
    return true;
}

static bool __vsf_eda_slist_queue_dequeue(vsf_eda_queue_t *this_ptr, void **node)
{
    __vsf_eda_slist_node_wrapper *slist_node;
    vsf_slist_queue_dequeue(__vsf_eda_slist_node_wrapper, node,
                            &((vsf_eda_slist_queue_t *)this_ptr)->queue,
                            slist_node);
    *node = slist_node;
    return *node != NULL;
}

vsf_err_t vsf_eda_slist_queue_init(vsf_eda_slist_queue_t *this_ptr, uint_fast16_t max)
{
    this_ptr->use_as__vsf_eda_queue_t.op.enqueue = __vsf_eda_slist_queue_enqueue;
    this_ptr->use_as__vsf_eda_queue_t.op.dequeue = __vsf_eda_slist_queue_dequeue;
    vsf_slist_queue_init(&this_ptr->queue);
    return vsf_eda_queue_init(&this_ptr->use_as__vsf_eda_queue_t, max);
}

#endif
#endif
