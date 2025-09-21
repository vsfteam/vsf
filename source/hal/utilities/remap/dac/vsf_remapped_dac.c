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

#if VSF_HAL_USE_DAC == ENABLED

#define __VSF_DISTBUS_CLASS_INHERIT__
#define __VSF_REMAPPED_DAC_CLASS_IMPLEMENT
#include "./vsf_remapped_dac.h"

#if VSF_DAC_CFG_MULTI_CLASS == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_REMAPPED_DAC_CFG_MULTI_CLASS == ENABLED
const vsf_dac_op_t vsf_remapped_dac_op = {
#   undef __VSF_HAL_TEMPLATE_API
#   define __VSF_HAL_TEMPLATE_API   VSF_HAL_TEMPLATE_API_OP

    VSF_DAC_APIS(vsf_remapped_dac)
};
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_remapped_dac_init(vsf_remapped_dac_t *dac, vsf_dac_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((dac != NULL) && (dac->target != NULL));
    return vsf_dac_init(dac->target, cfg_ptr);
}

void vsf_remapped_dac_fini(vsf_remapped_dac_t *dac)
{
    VSF_HAL_ASSERT((dac != NULL) && (dac->target != NULL));
    vsf_dac_fini(dac->target);
}

vsf_err_t vsf_remapped_dac_get_configuration(vsf_remapped_dac_t *dac, vsf_dac_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((dac != NULL) && (dac->target != NULL));
    return vsf_dac_get_configuration(dac->target, cfg_ptr);
}

vsf_dac_status_t vsf_remapped_dac_status(vsf_remapped_dac_t *dac)
{
    VSF_HAL_ASSERT((dac != NULL) && (dac->target != NULL));
    return vsf_dac_status(dac->target);
}

vsf_dac_capability_t vsf_remapped_dac_capability(vsf_remapped_dac_t *dac)
{
    VSF_HAL_ASSERT((dac != NULL) && (dac->target != NULL));
    return vsf_dac_capability(dac->target);
}

fsm_rt_t vsf_remapped_dac_enable(vsf_remapped_dac_t *dac)
{
    VSF_HAL_ASSERT((dac != NULL) && (dac->target != NULL));
    return vsf_dac_enable(dac->target);
}

fsm_rt_t vsf_remapped_dac_disable(vsf_remapped_dac_t *dac)
{
    VSF_HAL_ASSERT((dac != NULL) && (dac->target != NULL));
    return vsf_dac_disable(dac->target);
}

void vsf_remapped_dac_irq_enable(vsf_remapped_dac_t *dac, vsf_dac_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT((dac != NULL) && (dac->target != NULL));
    vsf_dac_irq_enable(dac->target, irq_mask);
}

void vsf_remapped_dac_irq_disable(vsf_remapped_dac_t *dac, vsf_dac_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT((dac != NULL) && (dac->target != NULL));
    vsf_dac_irq_disable(dac->target, irq_mask);
}

vsf_err_t vsf_remapped_dac_channel_request_once(vsf_remapped_dac_t *dac,
        vsf_dac_channel_cfg_t *channel_cfg, uint_fast16_t value)
{
    VSF_HAL_ASSERT((dac != NULL) && (dac->target != NULL));
    return vsf_dac_channel_request_once(dac->target, channel_cfg, value);
}

vsf_err_t vsf_remapped_dac_channel_config(vsf_remapped_dac_t *dac,
        vsf_dac_channel_cfg_t *channel_cfgs, uint_fast8_t channel_cfgs_cnt)
{
    VSF_HAL_ASSERT((dac != NULL) && (dac->target != NULL));
    return vsf_dac_channel_config(dac->target, channel_cfgs, channel_cfgs_cnt);
}

vsf_err_t vsf_remapped_dac_channel_request(vsf_remapped_dac_t *dac,
        void *buffer, uint_fast32_t count)
{
    VSF_HAL_ASSERT((dac != NULL) && (dac->target != NULL));
    return vsf_dac_channel_request(dac->target, buffer, count);
}

#endif
#endif
