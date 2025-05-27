/*****************************************************************************
 *   Cop->right(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a cop-> of the License at                                  *
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

#define VSF_ETH_CFG_FUNCTION_RENAME DISABLED

#include "hal/driver/driver.h"

#if VSF_HAL_USE_ETH == ENABLED

/*============================ ETHROS ========================================*/
/*============================ ETHROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/

#if VSF_ETH_CFG_MULTI_CLASS == ENABLED

vsf_err_t vsf_eth_init(vsf_eth_t *eth_ptr, vsf_eth_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(eth_ptr != NULL);
    VSF_HAL_ASSERT(eth_ptr->op != NULL);
    VSF_HAL_ASSERT(eth_ptr->op->init != NULL);

    return eth_ptr->op->init(eth_ptr, cfg_ptr);
}

void vsf_eth_fini(vsf_eth_t *eth_ptr)
{
    VSF_HAL_ASSERT(eth_ptr != NULL);
    VSF_HAL_ASSERT(eth_ptr->op != NULL);
    VSF_HAL_ASSERT(eth_ptr->op->fini != NULL);

    eth_ptr->op->fini(eth_ptr);
}

fsm_rt_t vsf_eth_enable(vsf_eth_t *eth_ptr)
{
    VSF_HAL_ASSERT(eth_ptr != NULL);
    VSF_HAL_ASSERT(eth_ptr->op != NULL);
    VSF_HAL_ASSERT(eth_ptr->op->enable != NULL);

    return eth_ptr->op->enable(eth_ptr);
}

fsm_rt_t vsf_eth_disable(vsf_eth_t *eth_ptr)
{
    VSF_HAL_ASSERT(eth_ptr != NULL);
    VSF_HAL_ASSERT(eth_ptr->op != NULL);
    VSF_HAL_ASSERT(eth_ptr->op->disable != NULL);

    return eth_ptr->op->disable(eth_ptr);
}

void vsf_eth_irq_enable(vsf_eth_t *eth_ptr, vsf_eth_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(eth_ptr != NULL);
    VSF_HAL_ASSERT(eth_ptr->op != NULL);
    VSF_HAL_ASSERT(eth_ptr->op->irq_enable != NULL);

    eth_ptr->op->irq_enable(eth_ptr, irq_mask);
}

void vsf_eth_irq_disable(vsf_eth_t *eth_ptr, vsf_eth_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(eth_ptr != NULL);
    VSF_HAL_ASSERT(eth_ptr->op != NULL);
    VSF_HAL_ASSERT(eth_ptr->op->irq_disable != NULL);

    eth_ptr->op->irq_disable(eth_ptr, irq_mask);
}

vsf_err_t vsf_eth_send_request(vsf_eth_t *eth_ptr, vsf_eth_send_buf_desc_t *buf_ptr)
{
    VSF_HAL_ASSERT(eth_ptr != NULL);
    VSF_HAL_ASSERT(eth_ptr->op != NULL);
    VSF_HAL_ASSERT(eth_ptr->op->send_request != NULL);

    return eth_ptr->op->send_request(eth_ptr, buf_ptr);
}

vsf_err_t vsf_eth_recv_request(vsf_eth_t *eth_ptr, vsf_eth_recv_buf_desc_t *buf_ptr)
{
    VSF_HAL_ASSERT(eth_ptr != NULL);
    VSF_HAL_ASSERT(eth_ptr->op != NULL);
    VSF_HAL_ASSERT(eth_ptr->op->recv_request != NULL);

    return eth_ptr->op->recv_request(eth_ptr, buf_ptr);
}

vsf_err_t vsf_eth_send_sg_request(vsf_eth_t *eth_ptr, vsf_eth_send_sg_buf_desc_t *buf_ptr, uint32_t sg_count)
{
    VSF_HAL_ASSERT(eth_ptr != NULL);
    VSF_HAL_ASSERT(eth_ptr->op != NULL);
    VSF_HAL_ASSERT(eth_ptr->op->send_sg_request != NULL);

    return eth_ptr->op->send_sg_request(eth_ptr, buf_ptr, sg_count);
}

vsf_err_t vsf_eth_recv_sg_request(vsf_eth_t *eth_ptr, vsf_eth_recv_sg_buf_desc_t *buf_ptr, uint32_t sg_count)
{
    VSF_HAL_ASSERT(eth_ptr != NULL);
    VSF_HAL_ASSERT(eth_ptr->op != NULL);
    VSF_HAL_ASSERT(eth_ptr->op->recv_sg_request != NULL);

    return eth_ptr->op->recv_sg_request(eth_ptr, buf_ptr, sg_count);
}

vsf_err_t vsf_eth_ctrl(vsf_eth_t *eth_ptr, vsf_eth_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(eth_ptr != NULL);
    VSF_HAL_ASSERT(eth_ptr->op != NULL);
    VSF_HAL_ASSERT(eth_ptr->op->ctrl != NULL);

    return eth_ptr->op->ctrl(eth_ptr, ctrl, param);
}

vsf_eth_status_t vsf_eth_status(vsf_eth_t *eth_ptr)
{
    VSF_HAL_ASSERT(eth_ptr != NULL);
    VSF_HAL_ASSERT(eth_ptr->op != NULL);
    VSF_HAL_ASSERT(eth_ptr->op->status != NULL);

    return eth_ptr->op->status(eth_ptr);
}

vsf_eth_capability_t vsf_eth_capability(vsf_eth_t *eth_ptr)
{
    VSF_HAL_ASSERT(eth_ptr != NULL);
    VSF_HAL_ASSERT(eth_ptr->op != NULL);
    VSF_HAL_ASSERT(eth_ptr->op->capability != NULL);

    return eth_ptr->op->capability(eth_ptr);
}

#endif /* VSF_ETH_CFG_MULTI_CLASS == ENABLED */
#endif /* VSF_HAL_USE_ETH == ENABLED */
