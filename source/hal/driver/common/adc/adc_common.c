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

#define VSF_ADC_CFG_FUNCTION_RENAME DISABLED

#include "hal/driver/driver.h"

#if VSF_HAL_USE_ADC == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/

#if VSF_ADC_CFG_MULTI_CLASS == ENABLED

vsf_err_t vsf_adc_init(vsf_adc_t *adc_ptr, vsf_adc_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(adc_ptr != NULL);
    VSF_HAL_ASSERT(adc_ptr->op != NULL);
    VSF_HAL_ASSERT(adc_ptr->op->init != NULL);

    return adc_ptr->op->init(adc_ptr, cfg_ptr);
}

void vsf_adc_fini(vsf_adc_t *adc_ptr)
{
    VSF_HAL_ASSERT(adc_ptr != NULL);
    VSF_HAL_ASSERT(adc_ptr->op != NULL);
    VSF_HAL_ASSERT(adc_ptr->op->init != NULL);

    adc_ptr->op->fini(adc_ptr);
}

vsf_err_t vsf_adc_get_configuration(vsf_adc_t *adc_ptr, vsf_adc_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(adc_ptr != NULL);
    VSF_HAL_ASSERT(adc_ptr->op != NULL);
    VSF_HAL_ASSERT(adc_ptr->op->get_configuration != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    return adc_ptr->op->get_configuration(adc_ptr, cfg_ptr);
}

fsm_rt_t vsf_adc_enable(vsf_adc_t *adc_ptr)
{
    VSF_HAL_ASSERT(adc_ptr != NULL);
    VSF_HAL_ASSERT(adc_ptr->op != NULL);
    VSF_HAL_ASSERT(adc_ptr->op->enable != NULL);

    return adc_ptr->op->enable(adc_ptr);
}

fsm_rt_t vsf_adc_disable(vsf_adc_t *adc_ptr)
{
    VSF_HAL_ASSERT(adc_ptr != NULL);
    VSF_HAL_ASSERT(adc_ptr->op != NULL);
    VSF_HAL_ASSERT(adc_ptr->op->disable != NULL);

    return adc_ptr->op->disable(adc_ptr);
}

void vsf_adc_irq_enable(vsf_adc_t *adc_ptr, vsf_adc_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(adc_ptr != NULL);
    VSF_HAL_ASSERT(adc_ptr->op != NULL);
    VSF_HAL_ASSERT(adc_ptr->op->irq_enable != NULL);

    adc_ptr->op->irq_enable(adc_ptr, irq_mask);
}

void vsf_adc_irq_disable(vsf_adc_t *adc_ptr, vsf_adc_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(adc_ptr != NULL);
    VSF_HAL_ASSERT(adc_ptr->op != NULL);
    VSF_HAL_ASSERT(adc_ptr->op->irq_disable != NULL);

    adc_ptr->op->irq_disable(adc_ptr, irq_mask);
}

vsf_adc_status_t vsf_adc_status(vsf_adc_t *adc_ptr)
{
    VSF_HAL_ASSERT(adc_ptr != NULL);
    VSF_HAL_ASSERT(adc_ptr->op != NULL);
    VSF_HAL_ASSERT(adc_ptr->op->status != NULL);

    return adc_ptr->op->status(adc_ptr);
}

vsf_adc_capability_t vsf_adc_capability(vsf_adc_t *adc_ptr)
{
    VSF_HAL_ASSERT(adc_ptr != NULL);
    VSF_HAL_ASSERT(adc_ptr->op != NULL);
    VSF_HAL_ASSERT(adc_ptr->op->capability != NULL);

    return adc_ptr->op->capability(adc_ptr);
}

vsf_err_t vsf_adc_channel_request_once(vsf_adc_t *adc_ptr, vsf_adc_channel_cfg_t *channel_cfg, void *buffer_ptr)
{
    VSF_HAL_ASSERT(adc_ptr != NULL);
    VSF_HAL_ASSERT(adc_ptr->op != NULL);
    VSF_HAL_ASSERT(adc_ptr->op->channel_request_once != NULL);

    return adc_ptr->op->channel_request_once(adc_ptr, channel_cfg, buffer_ptr);
}

vsf_err_t vsf_adc_channel_config(vsf_adc_t *adc_ptr, vsf_adc_channel_cfg_t *channel_cfgs_ptr, uint32_t channel_cfgs_cnt)
{
    VSF_HAL_ASSERT(adc_ptr != NULL);
    VSF_HAL_ASSERT(adc_ptr->op != NULL);
    VSF_HAL_ASSERT(adc_ptr->op->channel_config != NULL);

    return adc_ptr->op->channel_config(adc_ptr, channel_cfgs_ptr, channel_cfgs_cnt);
}

vsf_err_t vsf_adc_channel_request(vsf_adc_t *adc_ptr, void *buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT(adc_ptr != NULL);
    VSF_HAL_ASSERT(adc_ptr->op != NULL);
    VSF_HAL_ASSERT(adc_ptr->op->channel_request != NULL);

    return adc_ptr->op->channel_request(adc_ptr, buffer_ptr, count);
}

#endif /* VSF_ADC_CFG_MULTI_CLASS == ENABLED */
#endif /* VSF_HAL_USE_ADC == ENABLED */
