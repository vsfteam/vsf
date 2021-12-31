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

#include "./pwm.h"
#if VSF_HAL_USE_PWM == ENABLED
#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/pmic/pmic_api.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define PWM_PWM_TMR_SEL(n)                              (((n) & 3) << 16)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

vsf_pwm_t vsf_pwm0 = {
    .PARAM = (PWM_REG_T *)PWM_BASE_ADDRESS,
    .is_enabled = false,
};
/*============================ LOCAL VARIABLES ===============================*/

uint8_t __map_pwm_tmr[] = {0, 1, 1, 2};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static vsf_err_t __vsf_pwm_init(vsf_pwm_t *pwm_ptr)
{
    // hclk divider, 26M
    uint32_t temp_clock_div;
    if (101960 > pwm_ptr->cfg.mini_clock_freq) {
        temp_clock_div = 0xff;
    } else if(13000000 < pwm_ptr->cfg.mini_clock_freq) {
        temp_clock_div = 1;
    } else {
        temp_clock_div = 26000000 / pwm_ptr->cfg.mini_clock_freq;
    }
    pwm_ptr->cfg.mini_clock_freq = 26000000 / temp_clock_div;
    PMIC_MEM_WRITE( (unsigned int)(&aic1000liteSysctrl->hclk_div),
                        (AIC1000LITE_SYS_CTRL_CFG_HCLK_DIV_DENOM(temp_clock_div)
                     |  AIC1000LITE_SYS_CTRL_CFG_HCLK_DIV_UPDATE));
    // hclk sel
    PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteSysctrl->clk_sel),
        (AIC1000LITE_SYS_CTRL_CFG_CLK_PWM_SEL), (AIC1000LITE_SYS_CTRL_CFG_CLK_PWM_SEL));
    return VSF_ERR_NONE;
}

vsf_err_t vsf_pwm_init(vsf_pwm_t *pwm_ptr, pwm_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((NULL != pwm_ptr) && (NULL != cfg_ptr));
    pwm_ptr->cfg = *cfg_ptr;
    return __vsf_pwm_init(pwm_ptr);
}

fsm_rt_t vsf_pwm_enable(vsf_pwm_t *pwm_ptr)
{
    VSF_HAL_ASSERT(NULL != pwm_ptr);
    pwm_ptr->is_enabled = true;
    return fsm_rt_cpl;
}

fsm_rt_t vsf_pwm_disable(vsf_pwm_t *pwm_ptr)
{
    VSF_HAL_ASSERT(NULL != pwm_ptr);
    pwm_ptr->is_enabled = false;
    return fsm_rt_cpl;
}

vsf_err_t vsf_pwm_set(  vsf_pwm_t *pwm_ptr, uint8_t channel,
                         uint32_t period, uint32_t pulse)
{
    VSF_HAL_ASSERT(NULL != pwm_ptr);
    VSF_HAL_ASSERT(channel < 4);
    if (!pwm_ptr->is_enabled) {
        return VSF_ERR_FAIL;//todo:
    }
    PWM_REG_T *param = pwm_ptr->PARAM;

    PMIC_MEM_WRITE((unsigned int)(&param->TMR[__map_pwm_tmr[channel]].ld_value),
                   period - 1);
    PMIC_MEM_WRITE((unsigned int)(&param->TMR[__map_pwm_tmr[channel]].cfg),
                   (PWM_TMR_MODE_MASK | PWM_TMR_RUN_MASK));

    PMIC_MEM_WRITE((unsigned int)(&param->PWM[channel].sta_val), period - 1);
    PMIC_MEM_WRITE((unsigned int)(&param->PWM[channel].end_val),
                   period - 1 - pulse);

    PMIC_MEM_WRITE( (unsigned int)(&param->PWM[channel].cfg),
                        (PWM_PWM_RUN_MASK
                    |   PWM_PWM_TMR_SEL(__map_pwm_tmr[channel])
                    |   PWM_PWM_UPDATE_MASK));
    return VSF_ERR_NONE;
}

uint32_t vsf_pwm_get_freq(vsf_pwm_t *pwm_ptr)
{
    VSF_HAL_ASSERT(NULL != pwm_ptr);
    return pwm_ptr->cfg.mini_clock_freq;
}

#if VSF_HAL_PWM_IMP_TIME_SETTING == ENABLED
#   include "hal/driver/common/pwm/__pwm.inc"
#endif
#endif      // VSF_HAL_USE_PWM
