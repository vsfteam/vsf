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

#define VSF_TIMER_CFG_FUNCTION_RENAME DISABLED

#include "hal/driver/driver.h"

#if VSF_HAL_USE_TIMER == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/

#    if VSF_TIMER_CFG_MULTI_CLASS == ENABLED

vsf_err_t vsf_timer_init(vsf_timer_t *timer_ptr, vsf_timer_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
    VSF_HAL_ASSERT(timer_ptr->op != NULL);
    VSF_HAL_ASSERT(timer_ptr->op->init != NULL);

    return timer_ptr->op->init(timer_ptr, cfg_ptr);
}
void vsf_timer_fini(vsf_timer_t *timer_ptr)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
    VSF_HAL_ASSERT(timer_ptr->op != NULL);
    VSF_HAL_ASSERT(timer_ptr->op->fini != NULL);

    timer_ptr->op->fini(timer_ptr);
}

fsm_rt_t vsf_timer_enable(vsf_timer_t *timer_ptr)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
    VSF_HAL_ASSERT(timer_ptr->op != NULL);
    VSF_HAL_ASSERT(timer_ptr->op->enable != NULL);

    return timer_ptr->op->enable(timer_ptr);
}

fsm_rt_t vsf_timer_disable(vsf_timer_t *timer_ptr)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
    VSF_HAL_ASSERT(timer_ptr->op != NULL);
    VSF_HAL_ASSERT(timer_ptr->op->disable != NULL);

    return timer_ptr->op->disable(timer_ptr);
}

void vsf_timer_irq_enable(vsf_timer_t *timer_ptr, vsf_timer_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
    VSF_HAL_ASSERT(timer_ptr->op != NULL);
    VSF_HAL_ASSERT(timer_ptr->op->irq_enable != NULL);

    timer_ptr->op->irq_enable(timer_ptr, irq_mask);
}

void vsf_timer_irq_disable(vsf_timer_t         *timer_ptr,
                           vsf_timer_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
    VSF_HAL_ASSERT(timer_ptr->op != NULL);
    VSF_HAL_ASSERT(timer_ptr->op->irq_disable != NULL);

    timer_ptr->op->irq_disable(timer_ptr, irq_mask);
}

vsf_timer_status_t vsf_timer_status(vsf_timer_t *timer_ptr)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
    VSF_HAL_ASSERT(timer_ptr->op != NULL);
    VSF_HAL_ASSERT(timer_ptr->op->status != NULL);

    return timer_ptr->op->status(timer_ptr);
}

vsf_timer_capability_t vsf_timer_capability(vsf_timer_t *timer_ptr)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
    VSF_HAL_ASSERT(timer_ptr->op != NULL);
    VSF_HAL_ASSERT(timer_ptr->op->capability != NULL);

    return timer_ptr->op->capability(timer_ptr);
}

vsf_err_t vsf_timer_set_period(vsf_timer_t *timer_ptr, uint32_t period)
{
    VSF_HAL_ASSERT(NULL != timer_ptr);
    VSF_HAL_ASSERT(timer_ptr->op != NULL);
    VSF_HAL_ASSERT(timer_ptr->op->set_period != NULL);

    return timer_ptr->op->set_period(timer_ptr, period);
}

vsf_err_t vsf_timer_ctrl(vsf_timer_t *timer_ptr, vsf_timer_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(NULL != timer_ptr);
    VSF_HAL_ASSERT(timer_ptr->op != NULL);
    VSF_HAL_ASSERT(timer_ptr->op->ctrl != NULL);

    return timer_ptr->op->ctrl(timer_ptr, ctrl, param);
}

vsf_err_t vsf_timer_channel_config(vsf_timer_t *timer_ptr, uint8_t channel,
                                   vsf_timer_channel_cfg_t *channel_ptr)
{
    VSF_HAL_ASSERT(NULL != timer_ptr);
    VSF_HAL_ASSERT(timer_ptr->op != NULL);
    VSF_HAL_ASSERT(timer_ptr->op->channel_config != NULL);

    return timer_ptr->op->channel_config(timer_ptr, channel, channel_ptr);
}

vsf_err_t vsf_timer_channel_start(vsf_timer_t *timer_ptr, uint8_t channel)
{
    VSF_HAL_ASSERT(NULL != timer_ptr);
    VSF_HAL_ASSERT(timer_ptr->op != NULL);
    VSF_HAL_ASSERT(timer_ptr->op->channel_start != NULL);

    return timer_ptr->op->channel_start(timer_ptr, channel);
}

vsf_err_t vsf_timer_channel_stop(vsf_timer_t *timer_ptr, uint8_t channel)
{
    VSF_HAL_ASSERT(NULL != timer_ptr);
    VSF_HAL_ASSERT(timer_ptr->op != NULL);
    VSF_HAL_ASSERT(timer_ptr->op->channel_stop != NULL);

    return timer_ptr->op->channel_stop(timer_ptr, channel);
}

vsf_err_t vsf_timer_channel_ctrl(vsf_timer_t *timer_ptr, uint8_t channel,
                                 vsf_timer_channel_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(NULL != timer_ptr);
    VSF_HAL_ASSERT(timer_ptr->op != NULL);
    VSF_HAL_ASSERT(timer_ptr->op->channel_ctrl != NULL);

    return timer_ptr->op->channel_ctrl(timer_ptr, channel, ctrl, param);
}

#    endif /* VSF_TIMER_CFG_MULTI_CLASS == ENABLED */
#endif     /* VSF_HAL_USE_TIMER == ENABLED */
