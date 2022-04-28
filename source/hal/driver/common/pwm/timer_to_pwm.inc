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

#if VSF_HAL_USE_PWM == ENABLED

/*============================ INCLUDES ======================================*/

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_hw_pwm_init(vsf_hw_pwm_t *hw_pwm_ptr, pwm_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_pwm_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    VSF_HAL_ASSERT(NULL != hw_pwm_ptr->timer);

    hw_pwm_ptr->cfg = *cfg_ptr;

    timer_cfg_t timer_cfg = {
        .mode = TIMER_MODE_PWM,
        .min_freq = cfg_ptr->mini_clock_freq,
    };

    return vsf_hw_timer_init(hw_pwm_ptr->timer, &timer_cfg);
}

fsm_rt_t vsf_hw_pwm_enable(vsf_hw_pwm_t *hw_pwm_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_pwm_ptr);
    VSF_HAL_ASSERT(NULL != hw_pwm_ptr->timer);

    return vsf_hw_timer_enable(hw_pwm_ptr->timer);
}

fsm_rt_t vsf_hw_pwm_disable(vsf_hw_pwm_t *hw_pwm_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_pwm_ptr);
    VSF_HAL_ASSERT(NULL != hw_pwm_ptr->timer);

    return vsf_hw_timer_disable(hw_pwm_ptr->timer);
}

vsf_err_t vsf_hw_pwm_set(vsf_hw_pwm_t *hw_pwm_ptr, uint8_t channel, uint32_t period, uint32_t pulse)
{
    VSF_HAL_ASSERT(NULL != hw_pwm_ptr);
    VSF_HAL_ASSERT(NULL != hw_pwm_ptr->timer);

    return vsf_hw_timer_pwm_set(hw_pwm_ptr->timer, channel, period, pulse);
}

uint32_t vsf_hw_pwm_get_freq(vsf_hw_pwm_t *hw_pwm_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_pwm_ptr);
    return vsf_hw_timer_get_freq(hw_pwm_ptr->timer);
}

#endif  /* VSF_HAL_USE_PWM */
