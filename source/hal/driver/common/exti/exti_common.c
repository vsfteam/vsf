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

#define VSF_EXTI_CFG_FUNCTION_RENAME DISABLED

#include "hal/driver/driver.h"

#if VSF_HAL_USE_EXTI == ENABLED && VSF_EXTI_CFG_MULTI_CLASS == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_exti_config_channels(vsf_exti_t *exti_ptr, vsf_exti_channel_mask_t channel_mask, vsf_exti_channel_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(exti_ptr != NULL);
    VSF_HAL_ASSERT(exti_ptr->op != NULL);
    VSF_HAL_ASSERT(exti_ptr->op->config_channels != NULL);

    return exti_ptr->op->config_channels(exti_ptr, channel_mask, cfg_ptr);
}

vsf_err_t vsf_exti_init(vsf_exti_t *exti_ptr)
{
    VSF_HAL_ASSERT(exti_ptr != NULL);
    VSF_HAL_ASSERT(exti_ptr->op != NULL);
    VSF_HAL_ASSERT(exti_ptr->op->init != NULL);

    return exti_ptr->op->init(exti_ptr);
}

void vsf_exti_fini(vsf_exti_t *exti_ptr)
{
    VSF_HAL_ASSERT(exti_ptr != NULL);
    VSF_HAL_ASSERT(exti_ptr->op != NULL);
    VSF_HAL_ASSERT(exti_ptr->op->fini != NULL);

    exti_ptr->op->fini(exti_ptr);
}

vsf_exti_status_t vsf_exti_status(vsf_exti_t *exti_ptr)
{
    VSF_HAL_ASSERT(exti_ptr != NULL);
    VSF_HAL_ASSERT(exti_ptr->op != NULL);
    VSF_HAL_ASSERT(exti_ptr->op->status != NULL);

    return exti_ptr->op->status(exti_ptr);
}

vsf_exti_capability_t vsf_exti_capability(vsf_exti_t *exti_ptr)
{
    VSF_HAL_ASSERT(exti_ptr != NULL);
    VSF_HAL_ASSERT(exti_ptr->op != NULL);
    VSF_HAL_ASSERT(exti_ptr->op->capability != NULL);

    return exti_ptr->op->capability(exti_ptr);
}

vsf_err_t vsf_exti_trigger(vsf_exti_t *exti_ptr, vsf_exti_channel_mask_t channel_mask)
{
    VSF_HAL_ASSERT(exti_ptr != NULL);
    VSF_HAL_ASSERT(exti_ptr->op != NULL);
    VSF_HAL_ASSERT(exti_ptr->op->trigger != NULL);

    return exti_ptr->op->trigger(exti_ptr, channel_mask);
}

vsf_err_t vsf_exti_irq_enable(vsf_exti_t *exti_ptr, vsf_exti_channel_mask_t channel_mask)
{
    VSF_HAL_ASSERT(exti_ptr != NULL);
    VSF_HAL_ASSERT(exti_ptr->op != NULL);
    VSF_HAL_ASSERT(exti_ptr->op->irq_enable != NULL);

    return exti_ptr->op->irq_enable(exti_ptr, channel_mask);
}

vsf_err_t vsf_exti_irq_disable(vsf_exti_t *exti_ptr, vsf_exti_channel_mask_t channel_mask)
{
    VSF_HAL_ASSERT(exti_ptr != NULL);
    VSF_HAL_ASSERT(exti_ptr->op != NULL);
    VSF_HAL_ASSERT(exti_ptr->op->irq_disable != NULL);

    return exti_ptr->op->irq_disable(exti_ptr, channel_mask);
}

vsf_exti_channel_mask_t vsf_exti_irq_clear(vsf_exti_t *exti_ptr, vsf_exti_channel_mask_t channel_mask)
{
    VSF_HAL_ASSERT(exti_ptr != NULL);
    VSF_HAL_ASSERT(exti_ptr->op != NULL);
    VSF_HAL_ASSERT(exti_ptr->op->irq_clear != NULL);

    return exti_ptr->op->irq_clear(exti_ptr, channel_mask);
}

vsf_err_t vsf_exti_ctrl(vsf_exti_t *exti_ptr, vsf_exti_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(exti_ptr != NULL);
    VSF_HAL_ASSERT(exti_ptr->op != NULL);
    VSF_HAL_ASSERT(exti_ptr->op->ctrl != NULL);

    return exti_ptr->op->ctrl(exti_ptr, ctrl, param);
}

#endif
