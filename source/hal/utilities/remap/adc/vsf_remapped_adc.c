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

#include "hal/vsf_hal.h"

#if VSF_HAL_USE_ADC == ENABLED

#define __VSF_DISTBUS_CLASS_INHERIT__
#define __VSF_REMAPPED_ADC_CLASS_IMPLEMENT
#include "./vsf_remapped_adc.h"

#if VSF_ADC_CFG_MULTI_CLASS == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_REMAPPED_ADC_CFG_MULTI_CLASS == ENABLED
const vsf_adc_op_t vsf_remapped_adc_op = {
#   undef __VSF_HAL_TEMPLATE_API
#   define __VSF_HAL_TEMPLATE_API   VSF_HAL_TEMPLATE_API_OP

    VSF_ADC_APIS(vsf_remapped_adc)
};
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_remapped_adc_init(vsf_remapped_adc_t *adc, vsf_adc_cfg_t *cfg)
{
    VSF_HAL_ASSERT((adc != NULL) && (adc->target != NULL));
    return vsf_adc_init(adc->target, cfg);
}

void vsf_remapped_adc_fini(vsf_remapped_adc_t *adc)
{
    VSF_HAL_ASSERT((adc != NULL) && (adc->target != NULL));
    vsf_adc_fini(adc->target);
}

vsf_err_t vsf_remapped_adc_get_configuration(vsf_remapped_adc_t *adc, vsf_adc_cfg_t *cfg)
{
    VSF_HAL_ASSERT((adc != NULL) && (adc->target != NULL));
    return vsf_adc_get_configuration(adc->target, cfg);
}

vsf_adc_status_t vsf_remapped_adc_status(vsf_remapped_adc_t *adc)
{
    VSF_HAL_ASSERT((adc != NULL) && (adc->target != NULL));
    return vsf_adc_status(adc->target);
}

vsf_adc_capability_t vsf_remapped_adc_capability(vsf_remapped_adc_t *adc)
{
    VSF_HAL_ASSERT((adc != NULL) && (adc->target != NULL));
    return vsf_adc_capability(adc->target);
}

fsm_rt_t vsf_remapped_adc_enable(vsf_remapped_adc_t *adc)
{
    VSF_HAL_ASSERT((adc != NULL) && (adc->target != NULL));
    return vsf_adc_enable(adc->target);
}

fsm_rt_t vsf_remapped_adc_disable(vsf_remapped_adc_t *adc)
{
    VSF_HAL_ASSERT((adc != NULL) && (adc->target != NULL));
    return vsf_adc_disable(adc->target);
}

void vsf_remapped_adc_irq_enable(vsf_remapped_adc_t *adc, vsf_adc_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT((adc != NULL) && (adc->target != NULL));
    vsf_adc_irq_enable(adc->target, irq_mask);
}

void vsf_remapped_adc_irq_disable(vsf_remapped_adc_t *adc, vsf_adc_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT((adc != NULL) && (adc->target != NULL));
    vsf_adc_irq_disable(adc->target, irq_mask);
}

vsf_err_t vsf_remapped_adc_channel_request_once(vsf_remapped_adc_t *adc,
        vsf_adc_channel_cfg_t *channel_cfg, void *buffer)
{
    VSF_HAL_ASSERT((adc != NULL) && (adc->target != NULL));
    return vsf_adc_channel_request_once(adc->target, channel_cfg, buffer);
}

vsf_err_t vsf_remapped_adc_channel_config(vsf_remapped_adc_t *adc,
        vsf_adc_channel_cfg_t *channel_cfgs, uint32_t channel_cfgs_cnt)
{
    VSF_HAL_ASSERT((adc != NULL) && (adc->target != NULL));
    return vsf_adc_channel_config(adc->target, channel_cfgs, channel_cfgs_cnt);
}

vsf_err_t vsf_remapped_adc_channel_request(vsf_remapped_adc_t *adc,
        void *buffer, uint_fast32_t count)
{
    VSF_HAL_ASSERT((adc != NULL) && (adc->target != NULL));
    return vsf_adc_channel_request(adc->target, buffer, count);
}

#endif
#endif
