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

#define __VSF_QUEUE_CLASS_IMPLEMENT
#include "./vsf_queue.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if __VSF_KERNEL_CFG_SUPPORT_LIST_QUEUE == ENABLED
struct __vsf_eda_list_node_wrapper {
    implement_ex(vsf_slist_node_t, node)
};
typedef struct __vsf_eda_list_node_wrapper __vsf_eda_list_node_wrapper;
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if __VSF_KERNEL_CFG_SUPPORT_LIST_QUEUE == ENABLED

static bool __vsf_eda_list_queue_enqueue(vsf_queue_t *pthis, void *node)
{
    __vsf_eda_list_node_wrapper *list_node = node;
    vsf_slist_init_node(__vsf_eda_list_node_wrapper, node, list_node);
    vsf_slist_queue_enqueue(__vsf_eda_list_node_wrapper, node,
                            &((vsf_list_queue_t *)pthis)->queue,
                            (__vsf_eda_list_node_wrapper *)node);
    return true;
}

static bool __vsf_eda_list_queue_dequeue(vsf_queue_t *pthis, void **node)
{
    __vsf_eda_list_node_wrapper *list_node;
    vsf_slist_queue_dequeue(__vsf_eda_list_node_wrapper, node,
                            &((vsf_list_queue_t *)pthis)->queue,
                            list_node);
    *node = list_node;
    return *node != NULL;
}

vsf_err_t vsf_eda_list_queue_init(vsf_list_queue_t *pthis, uint_fast16_t max)
{
    pthis->use_as__vsf_queue_t.op.enqueue = __vsf_eda_list_queue_enqueue;
    pthis->use_as__vsf_queue_t.op.dequeue = __vsf_eda_list_queue_dequeue;
    vsf_slist_queue_init(&pthis->queue);
    return vsf_eda_queue_init(&pthis->use_as__vsf_queue_t, max);
}

#endif
#endif
