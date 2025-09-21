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

#define VSF_DAC_CFG_FUNCTION_RENAME DISABLED

#include "hal/driver/driver.h"

#if VSF_HAL_USE_DAC == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/

#if VSF_DAC_CFG_MULTI_CLASS == ENABLED

vsf_err_t vsf_dac_init(vsf_dac_t *dac_ptr, vsf_dac_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(dac_ptr != NULL);
    VSF_HAL_ASSERT(dac_ptr->op != NULL);
    VSF_HAL_ASSERT(dac_ptr->op->init != NULL);

    return dac_ptr->op->init(dac_ptr, cfg_ptr);
}

void vsf_dac_fini(vsf_dac_t *dac_ptr)
{
    VSF_HAL_ASSERT(dac_ptr != NULL);
    VSF_HAL_ASSERT(dac_ptr->op != NULL);
    VSF_HAL_ASSERT(dac_ptr->op->init != NULL);

    dac_ptr->op->fini(dac_ptr);
}

vsf_err_t vsf_dac_get_configuration(vsf_dac_t *dac_ptr, vsf_dac_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(dac_ptr != NULL);
    VSF_HAL_ASSERT(dac_ptr->op != NULL);
    VSF_HAL_ASSERT(dac_ptr->op->get_configuration != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    return dac_ptr->op->get_configuration(dac_ptr, cfg_ptr);
}

fsm_rt_t vsf_dac_enable(vsf_dac_t *dac_ptr)
{
    VSF_HAL_ASSERT(dac_ptr != NULL);
    VSF_HAL_ASSERT(dac_ptr->op != NULL);
    VSF_HAL_ASSERT(dac_ptr->op->enable != NULL);

    return dac_ptr->op->enable(dac_ptr);
}

fsm_rt_t vsf_dac_disable(vsf_dac_t *dac_ptr)
{
    VSF_HAL_ASSERT(dac_ptr != NULL);
    VSF_HAL_ASSERT(dac_ptr->op != NULL);
    VSF_HAL_ASSERT(dac_ptr->op->disable != NULL);

    return dac_ptr->op->disable(dac_ptr);
}

void vsf_dac_irq_enable(vsf_dac_t *dac_ptr, vsf_dac_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(dac_ptr != NULL);
    VSF_HAL_ASSERT(dac_ptr->op != NULL);
    VSF_HAL_ASSERT(dac_ptr->op->irq_enable != NULL);

    dac_ptr->op->irq_enable(dac_ptr, irq_mask);
}

void vsf_dac_irq_disable(vsf_dac_t *dac_ptr, vsf_dac_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(dac_ptr != NULL);
    VSF_HAL_ASSERT(dac_ptr->op != NULL);
    VSF_HAL_ASSERT(dac_ptr->op->irq_disable != NULL);

    dac_ptr->op->irq_disable(dac_ptr, irq_mask);
}

vsf_dac_status_t vsf_dac_status(vsf_dac_t *dac_ptr)
{
    VSF_HAL_ASSERT(dac_ptr != NULL);
    VSF_HAL_ASSERT(dac_ptr->op != NULL);
    VSF_HAL_ASSERT(dac_ptr->op->status != NULL);

    return dac_ptr->op->status(dac_ptr);
}

vsf_dac_capability_t vsf_dac_capability(vsf_dac_t *dac_ptr)
{
    VSF_HAL_ASSERT(dac_ptr != NULL);
    VSF_HAL_ASSERT(dac_ptr->op != NULL);
    VSF_HAL_ASSERT(dac_ptr->op->capability != NULL);

    return dac_ptr->op->capability(dac_ptr);
}

vsf_err_t vsf_dac_channel_request_once(vsf_dac_t *dac_ptr, vsf_dac_channel_cfg_t *channel_cfg, uint_fast16_t value)
{
    VSF_HAL_ASSERT(dac_ptr != NULL);
    VSF_HAL_ASSERT(dac_ptr->op != NULL);
    VSF_HAL_ASSERT(dac_ptr->op->channel_request_once != NULL);

    return dac_ptr->op->channel_request_once(dac_ptr, channel_cfg, value);
}

vsf_err_t vsf_dac_channel_config(vsf_dac_t *dac_ptr, vsf_dac_channel_cfg_t *channel_cfgs_ptr, uint_fast8_t channel_cfgs_cnt)
{
    VSF_HAL_ASSERT(dac_ptr != NULL);
    VSF_HAL_ASSERT(dac_ptr->op != NULL);
    VSF_HAL_ASSERT(dac_ptr->op->channel_config != NULL);

    return dac_ptr->op->channel_config(dac_ptr, channel_cfgs_ptr, channel_cfgs_cnt);
}

vsf_err_t vsf_dac_channel_request(vsf_dac_t *dac_ptr, void *buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT(dac_ptr != NULL);
    VSF_HAL_ASSERT(dac_ptr->op != NULL);
    VSF_HAL_ASSERT(dac_ptr->op->channel_request != NULL);

    return dac_ptr->op->channel_request(dac_ptr, buffer_ptr, count);
}

#endif /* VSF_DAC_CFG_MULTI_CLASS == ENABLED */
#endif /* VSF_HAL_USE_DAC == ENABLED */
