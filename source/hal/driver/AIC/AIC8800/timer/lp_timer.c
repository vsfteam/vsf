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

#include "../driver.h"

#if VSF_HAL_USE_TIMER == ENABLED

#include "hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/pmic/pmic_api.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_TIMER_CFG_MULTI_CLASS
#   define VSF_HW_TIMER_CFG_MULTI_CLASS           VSF_TIMER_CFG_MULTI_CLASS
#endif

/*============================ TYPES =========================================*/

typedef struct vsf_hw_lp_timer_t {
#if VSF_HW_TIMER_CFG_MULTI_CLASS == ENABLED
    vsf_timer_t vsf_timer;
#endif

    uint32_t freq;
    vsf_timer_cfg_t cfg;

} vsf_hw_lp_timer_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_hw_lp_timer_init(vsf_hw_lp_timer_t *timer_ptr, vsf_timer_cfg_t *cfg_ptr)
{
    return VSF_ERR_NONE;
}

fsm_rt_t vsf_hw_lp_timer_enable(vsf_hw_lp_timer_t *timer_ptr)
{
    VSF_HAL_ASSERT(NULL != timer_ptr);

    return fsm_rt_cpl;
}

fsm_rt_t vsf_hw_lp_timer_disable(vsf_hw_lp_timer_t *timer_ptr)
{
    VSF_HAL_ASSERT(NULL != timer_ptr);

    return fsm_rt_cpl;
}

void vsf_hw_lp_timer_irq_enable(vsf_hw_lp_timer_t *timer_ptr, vsf_timer_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != timer_ptr);
}

void vsf_hw_lp_timer_irq_disable(vsf_hw_lp_timer_t *timer_ptr, vsf_timer_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != timer_ptr);
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_TIMER_CFG_IMP_PREFIX                    vsf_hw_lp
#define VSF_TIMER_CFG_IMP_UPCASE_PREFIX             VSF_HW_LP
#define VSF_TIMER_CFG_IMP_LV0(__IDX, __HAL_OP)                                  \
    vsf_hw_lp_timer_t vsf_hw_lp_timer ## __IDX = {                              \
        .freq = 0,                                                              \
        __HAL_OP                                                                \
    };
#include "hal/driver/common/timer/timer_template.inc"

#endif      // VSF_HAL_USE_TIMER
