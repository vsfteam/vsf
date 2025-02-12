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
#include "./i_reg_pwm.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_PWM_CFG_MULTI_CLASS
#   define VSF_HW_PWM_CFG_MULTI_CLASS           VSF_PWM_CFG_MULTI_CLASS
#endif

#define PWM_PWM_TMR_SEL(n)                      (((n) & 3) << 16)

#define PWM_MAX_FREQ                            (26ul * 1000 * 1000)

/*============================ TYPES =========================================*/

typedef struct vsf_hw_pwm_t {
#if VSF_HW_PWM_CFG_MULTI_CLASS == ENABLED
    vsf_pwm_t vsf_pwm;
#endif

    PWM_REG_T *reg;
    uint32_t freq;

    uint8_t timer_mask;
    int8_t pwm_map[VSF_HW_PWM0_CHANNEL_COUNT];
} vsf_hw_pwm_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_hw_pwm_init(vsf_hw_pwm_t *hw_pwm_ptr, vsf_pwm_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_pwm_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    uint32_t pwm_clock = PWM_MAX_FREQ;
    uint32_t div = pwm_clock / cfg_ptr->freq;
    div = vsf_min(div, 0xFF);
    div = vsf_max(div, 0x01);

    hw_pwm_ptr->freq      = pwm_clock / div;

    PMIC_MEM_WRITE( (unsigned int)(&aic1000liteSysctrl->hclk_div),
                        (AIC1000LITE_SYS_CTRL_CFG_HCLK_DIV_DENOM(div)
                     |  AIC1000LITE_SYS_CTRL_CFG_HCLK_DIV_UPDATE));
    PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteSysctrl->clk_sel),
        (AIC1000LITE_SYS_CTRL_CFG_CLK_PWM_SEL), (AIC1000LITE_SYS_CTRL_CFG_CLK_PWM_SEL));

    memset(hw_pwm_ptr->pwm_map, 0xFF, sizeof(hw_pwm_ptr->pwm_map));

    return VSF_ERR_NONE;
}

void vsf_hw_pwm_fini(vsf_hw_pwm_t *hw_pwm_ptr)
{
    VSF_HAL_ASSERT(hw_pwm_ptr != NULL);
}

fsm_rt_t vsf_hw_pwm_enable(vsf_hw_pwm_t *hw_pwm_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_pwm_ptr);

    return fsm_rt_cpl;
}

fsm_rt_t vsf_hw_pwm_disable(vsf_hw_pwm_t *hw_pwm_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_pwm_ptr);

    return fsm_rt_cpl;
}

vsf_err_t vsf_hw_pwm_set(vsf_hw_pwm_t *hw_pwm_ptr, uint8_t channel, uint32_t period, uint32_t pulse)
{
    VSF_HAL_ASSERT(NULL != hw_pwm_ptr);
    VSF_HAL_ASSERT(channel < VSF_HW_PWM0_CHANNEL_COUNT);

    // TODO: support free timer
    // TODO: support same period

    // search idle timer
    if (hw_pwm_ptr->timer_mask == ((1 << VSF_HW_PWM0_TIMER_COUNT) - 1)) {
        return VSF_ERR_FAIL;
    }
    int timer_index = hw_pwm_ptr->pwm_map[channel];
    if ((timer_index < 0) || (timer_index >= VSF_HW_PWM0_TIMER_COUNT)) {
        for (timer_index = 0; timer_index < VSF_HW_PWM0_TIMER_COUNT; timer_index++) {
            if (!(hw_pwm_ptr->timer_mask & (1 << timer_index))) {
                hw_pwm_ptr->timer_mask |= (1 << timer_index);
                hw_pwm_ptr->pwm_map[channel] = timer_index;
                break;
            }
        }
    }

    PWM_REG_T *reg = hw_pwm_ptr->reg;

    PMIC_MEM_WRITE((unsigned int)(&reg->TMR[timer_index].ld_value), period - 1);
    PMIC_MEM_WRITE((unsigned int)(&reg->TMR[timer_index].cfg), (PWM_TMR_MODE_MASK | PWM_TMR_RUN_MASK));

    PMIC_MEM_WRITE((unsigned int)(&reg->PWM[channel].sta_val), period - 1);
    PMIC_MEM_WRITE((unsigned int)(&reg->PWM[channel].end_val), period - 1 - pulse);

    PMIC_MEM_WRITE((unsigned int)(&reg->PWM[channel].cfg),
                   (PWM_PWM_RUN_MASK | PWM_PWM_TMR_SEL(timer_index) | PWM_PWM_UPDATE_MASK));

    return VSF_ERR_NONE;
}

uint32_t vsf_hw_pwm_get_freq(vsf_hw_pwm_t *hw_pwm_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_pwm_ptr);

    return hw_pwm_ptr->freq;
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_PWM_CFG_CAPABILITY_MIN_FREQ                 (PWM_MAX_FREQ / 255)
#define VSF_PWM_CFG_CAPABILITY_MAX_FREQ                 (PWM_MAX_FREQ)
#define VSF_PWM_CFG_IMP_PREFIX                          vsf_hw
#define VSF_PWM_CFG_IMP_UPCASE_PREFIX                   VSF_HW
#define VSF_PWM_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    vsf_hw_pwm_t vsf_hw_pwm ## __IDX = {                                        \
        .reg = (PWM_REG_T *)VSF_HW_PWM ##__IDX ##_BASE_ADDRESS,                 \
        __HAL_OP                                                                \
    };
#include "hal/driver/common/pwm/pwm_template.inc"

#endif      // VSF_HAL_USE_PWM
