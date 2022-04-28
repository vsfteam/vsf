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

#define VSF_PWM_CFG_PREFIX                  vsf_hw
#define VSF_PWM_CFG_UPPERCASE_PREFIX        VSF_HW

/*============================ INCLUDES ======================================*/

#include "./pwm.h"

#if VSF_HAL_USE_PWM == ENABLED

#include "hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/pmic/pmic_api.h"
#include "hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/sysctrl/reg_sysctrl.h"
#include "../__device.h"
#include "./i_reg_pwm.h"

/*============================ MACROS ========================================*/

#define PWM_PWM_TMR_SEL(n)                              (((n) & 3) << 16)

/*============================ TYPES =========================================*/

typedef struct vsf_hw_pwm_t {
#if VSF_PWM_CFG_IMPLEMENT_OP == ENABLED
    vsf_pwm_t vsf_pwm;
#endif

    PWM_REG_T *reg;
    uint32_t freq;

} vsf_hw_pwm_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_hw_pwm_init(vsf_hw_pwm_t *hw_pwm_ptr, pwm_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_pwm_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    uint32_t div;
    uint32_t hclk = cpusysctrl_hclkme_get();
    if (hclk > cfg_ptr->freq * 0xFF) {
        div = 0xFF;
    } else if (hclk < cfg_ptr->freq) {
        div = 0x01;
    } else {
        div = div / cfg_ptr->freq;
    }
    hw_pwm_ptr->freq = hclk / div;

    PMIC_MEM_WRITE( (unsigned int)(&aic1000liteSysctrl->hclk_div),
                        (AIC1000LITE_SYS_CTRL_CFG_HCLK_DIV_DENOM(div)
                     |  AIC1000LITE_SYS_CTRL_CFG_HCLK_DIV_UPDATE));
    PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteSysctrl->clk_sel),
        (AIC1000LITE_SYS_CTRL_CFG_CLK_PWM_SEL), (AIC1000LITE_SYS_CTRL_CFG_CLK_PWM_SEL));

    return VSF_ERR_NONE;
}

fsm_rt_t vsf_hw_pwm_enable(vsf_hw_pwm_t *hw_pwm_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_pwm_ptr);

    return fsm_rt_cpl;
}

fsm_rt_t vsf_hw_pwm_disable(vsf_hw_pwm_t *hw_pwm_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_pwm_ptr);

    return hw_pwm_ptr->freq;
}

vsf_err_t vsf_hw_pwm_set(vsf_hw_pwm_t *hw_pwm_ptr, uint8_t channel, uint32_t period, uint32_t pulse)
{
    static const uint8_t __map_pwm_tmr[] = {0, 1, 1, 2};

    VSF_HAL_ASSERT(NULL != hw_pwm_ptr);
    VSF_HAL_ASSERT(channel < 4);

    PWM_REG_T *reg = hw_pwm_ptr->reg;

    PMIC_MEM_WRITE((unsigned int)(&reg->TMR[__map_pwm_tmr[channel]].ld_value), period - 1);
    PMIC_MEM_WRITE((unsigned int)(&reg->TMR[__map_pwm_tmr[channel]].cfg), (PWM_TMR_MODE_MASK | PWM_TMR_RUN_MASK));

    PMIC_MEM_WRITE((unsigned int)(&reg->PWM[channel].sta_val), period - 1);
    PMIC_MEM_WRITE((unsigned int)(&reg->PWM[channel].end_val), period - 1 - pulse);

    PMIC_MEM_WRITE( (unsigned int)(&reg->PWM[channel].cfg),
                        (PWM_PWM_RUN_MASK
                    |   PWM_PWM_TMR_SEL(__map_pwm_tmr[channel])
                    |   PWM_PWM_UPDATE_MASK));

    return VSF_ERR_NONE;
}

uint32_t vsf_hw_pwm_get_freq(vsf_hw_pwm_t *hw_pwm_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_pwm_ptr);

    return hw_pwm_ptr->freq;
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_PWM_CFG_IMP_LV0(__COUNT, __hal_op)                                  \
    vsf_hw_pwm_t vsf_hw_pwm##__COUNT = {                                        \
        .reg = (PWM_REG_T *)VSF_HW_PWM ##__COUNT ##_BASE_ADDRESS,               \
        __hal_op                                                                \
    };
#include "hal/driver/common/pwm/pwm_template.inc"

#endif      // VSF_HAL_USE_PWM
