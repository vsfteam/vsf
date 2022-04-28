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

#define VSF_TIMER_CFG_PREFIX                    vsf_hw
#define VSF_TIMER_CFG_UPPERCASE_PREFIX          VSF_HW

/*============================ INCLUDES ======================================*/

#include "./timer.h"

#if VSF_HAL_USE_TIMER == ENABLED

#include "hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/pmic/pmic_api.h"
#include "../__device.h"

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/

typedef struct vsf_hw_timer_t {
#if VSF_TIMER_CFG_IMPLEMENT_OP == ENABLED
    vsf_timer_t vsf_timer;
#endif

    uint32_t freq;
    timer_cfg_t cfg;

} vsf_hw_timer_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_hw_timer_init(vsf_hw_timer_t *hw_timer_ptr, timer_cfg_t *cfg_ptr)
{

    return VSF_ERR_NONE;
}

fsm_rt_t vsf_hw_timer_enable(vsf_hw_timer_t *hw_timer_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_timer_ptr);

    return fsm_rt_cpl;
}

fsm_rt_t vsf_hw_timer_disable(vsf_hw_timer_t *hw_timer_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_timer_ptr);

    return fsm_rt_cpl;
}

void vsf_hw_timer_irq_enable(vsf_hw_timer_t *hw_timer_ptr, em_timer_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != hw_timer_ptr);
}

void vsf_hw_timer_irq_disable(vsf_hw_timer_t *hw_timer_ptr, em_timer_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != hw_timer_ptr);
}

vsf_err_t vsf_hw_timer_pwm_set(vsf_hw_timer_t *hw_timer_ptr, uint8_t channel, uint32_t period, uint32_t pulse)
{
    VSF_HAL_ASSERT(NULL != hw_timer_ptr);

    VSF_HAL_ASSERT(0);
    return VSF_ERR_FAIL;
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_TIMER_CFG_IMP_LV0(__COUNT, __hal_op)                                \
    vsf_hw_timer_t vsf_hw_timer##__COUNT = {                                    \
        .freq = 0,                                                              \
        __hal_op                                                                \
    };
#include "hal/driver/common/timer/timer_template.inc"

#endif      // VSF_HAL_USE_TIMER
