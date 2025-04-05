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

#if VSF_HAL_USE_PWM == ENABLED

#include "hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/pmic/pmic_api.h"
#include "hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/sysctrl/sysctrl_api.h"
#include "hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/pwm/reg_pwm.h"

/*============================ MACROS ========================================*/

#define AIC_PWM_REG(__N)            \
    ((__IO uint32_t *)((uint32_t)&AIC_PWM->__N + hw_pwm_ptr->index * 0x10))

#ifndef VSF_HW_PWM_CFG_MULTI_CLASS
#   define VSF_HW_PWM_CFG_MULTI_CLASS           VSF_PWM_CFG_MULTI_CLASS
#endif


#define AIC_PWM_CLOCK                   (32 * 1000)

/*============================ TYPES =========================================*/

typedef struct vsf_aic_pwm_const_t {
    uint8_t  max_chn_cnt;
    uint32_t pclk_mask;
    uint32_t oclk_mask;
    uint32_t perclk;
} vsf_aic_pwm_const_t;

typedef struct vsf_aic_pwm_t {
#if VSF_HW_PWM_CFG_MULTI_CLASS == ENABLED
    vsf_pwm_t vsf_pwm;
#endif
    uint8_t  index;
    uint32_t freq;

    const vsf_aic_pwm_const_t * pwm_const;

} vsf_aic_pwm_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_aic_pwm_init(vsf_aic_pwm_t *hw_pwm_ptr, vsf_pwm_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_pwm_ptr);
    VSF_HAL_ASSERT(NULL != hw_pwm_ptr->pwm_const);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    VSF_HAL_ASSERT(hw_pwm_ptr->index < VSF_AIC_PWM_COUNT);

    cpusysctrl_pclkme_set(hw_pwm_ptr->pwm_const->pclk_mask);
    cpusysctrl_oclkme_set(hw_pwm_ptr->pwm_const->oclk_mask);

    uint32_t pwm_clock = 32 * 1000;
    uint32_t div = pwm_clock / cfg_ptr->freq;

    uint32_t prescale;
    prescale = vsf_min(div, 0xFF);
    prescale = vsf_max(div, 0x01);

    hw_pwm_ptr->freq = pwm_clock / prescale;

    *AIC_PWM_REG(PWM0_PRESCALE) = PWM_PWM_PRESCALE(prescale);

    return VSF_ERR_NONE;
}

fsm_rt_t vsf_aic_pwm_enable(vsf_aic_pwm_t *hw_pwm_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_pwm_ptr);
    VSF_HAL_ASSERT(hw_pwm_ptr->index < VSF_AIC_PWM_COUNT);

    return fsm_rt_cpl;
}

fsm_rt_t vsf_aic_pwm_disable(vsf_aic_pwm_t *hw_pwm_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_pwm_ptr);
    VSF_HAL_ASSERT(hw_pwm_ptr->index < VSF_AIC_PWM_COUNT);

    AIC_PWM->PWM_CTRL &= ~((PWM_PWM0_RUN | PWM_PWM0_MODE_SEL) << (hw_pwm_ptr->index * 4));

    return fsm_rt_cpl;
}

vsf_err_t vsf_aic_pwm_set(vsf_aic_pwm_t *hw_pwm_ptr, uint8_t channel, uint32_t period, uint32_t pulse)
{
    VSF_HAL_ASSERT(NULL != hw_pwm_ptr);
    VSF_HAL_ASSERT(hw_pwm_ptr->index < VSF_AIC_PWM_COUNT);
    VSF_HAL_ASSERT(pulse <= period);
    VSF_HAL_ASSERT(channel == 0);

    if ((0 == period) || (period > 0x100)) {
        return VSF_ERR_FAIL;
    }
    period -= 1;

    if (pulse > 0xFF) {
        return VSF_ERR_FAIL;
    }

    *AIC_PWM_REG(PWM0_CNT) = PWM_PWM0_DUTY(period) | PWM_PWM0_MOD(pulse);
    AIC_PWM->PWM_CTRL |= ((PWM_PWM0_RUN | PWM_PWM0_MODE_SEL) << (hw_pwm_ptr->index * 4));

    return VSF_ERR_NONE;
}

uint32_t vsf_aic_pwm_get_freq(vsf_aic_pwm_t *hw_pwm_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_pwm_ptr);

    return hw_pwm_ptr->freq;
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_PWM_CFG_CAPABILITY_MAX_FREQ         AIC_PWM_CLOCK
#define VSF_PWM_CFG_CAPABILITY_MIN_FREQ         (AIC_PWM_CLOCK / 256)
#define VSF_PWM_CFG_IMP_PREFIX                  vsf_aic
#define VSF_PWM_CFG_IMP_UPCASE_PREFIX           VSF_AIC
#define VSF_PWM_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    static const vsf_aic_pwm_const_t vsf_aic_pwm ## __IDX ## _const= {          \
        .max_chn_cnt = VSF_AIC_PWM ## __IDX ## _CHANNEL_COUNT,                  \
        .pclk_mask   = CSC_PCLKME_PWM_EN_BIT,                                   \
        .oclk_mask   = CSC_OCLKME_PWM_EN_BIT,                                   \
    };                                                                          \
    vsf_aic_pwm_t vsf_aic_pwm ## __IDX = {                                      \
        .index        = __IDX,                                                  \
        .pwm_const    = &vsf_aic_pwm ## __IDX ## _const,                        \
        __HAL_OP                                                                \
    };
#include "hal/driver/common/pwm/pwm_template.inc"

#endif      // VSF_HAL_USE_PWM
