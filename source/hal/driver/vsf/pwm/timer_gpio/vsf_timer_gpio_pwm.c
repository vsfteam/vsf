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

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_PWM == ENABLED && VSF_HAL_USE_TIMER_GPIO_PWM == ENABLED

#define __VSF_HAL_TIMER_GPIO_PWM_CLASS_IMPLEMENT
#include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TIMER_GPIO_CFG_PROTECT_LEVEL
#   define VSF_TIMER_GPIO_CFG_PROTECT_LEVEL             interrupt
#endif

#define __vsf_timer_gpio_protect                        vsf_protect(VSF_TIMER_GPIO_CFG_PROTECT_LEVEL)
#define __vsf_timer_gpio_unprotect                      vsf_unprotect(VSF_TIMER_GPIO_CFG_PROTECT_LEVEL)

#ifndef VSF_TIMER_GPIO_MULTI_PWM_CFG_MULTI_CLASS
#   define VSF_TIMER_GPIO_MULTI_PWM_CFG_MULTI_CLASS     VSF_TIMER_GPIO_PWM_CFG_MULTI_CLASS
#endif

#ifndef VSF_TIMER_GPIO_SINGLE_PWM_CFG_MULTI_CLASS
#   define VSF_TIMER_GPIO_SINGLE_PWM_CFG_MULTI_CLASS    VSF_TIMER_GPIO_PWM_CFG_MULTI_CLASS
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __vsf_timer_gpio_multi_pwm_isrhandler(void *target_ptr, vsf_timer_t *timer_ptr, vsf_timer_irq_mask_t irq_mask)
{
    vsf_timer_gpio_multi_pwm_t *pwm_ptr = (vsf_timer_gpio_multi_pwm_t *)target_ptr;
    if (0 == pwm_ptr->cur_pulse) {

    }
}

vsf_err_t vsf_timer_gpio_multi_pwm_init(vsf_timer_gpio_multi_pwm_t *pwm_ptr, vsf_pwm_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != pwm_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    VSF_HAL_ASSERT(NULL != pwm_ptr->timer);

    pwm_ptr->enabled = false;
    pwm_ptr->freq = cfg_ptr->freq;
    return vsf_timer_init(pwm_ptr->timer, &(vsf_timer_cfg_t){
        .mode           = VSF_TIMER_BASE_ONESHOT,
        .period         = 0,
        .freq           = cfg_ptr->freq,
        .isr            = {
            .handler_fn = __vsf_timer_gpio_multi_pwm_isrhandler,
            .target_ptr = pwm_ptr,
            .prio       = VSF_ARCH_PRIO_HIGHEST,
        },
    });
}

void vsf_timer_gpio_multi_pwm_fini(vsf_timer_gpio_multi_pwm_t *pwm_ptr)
{
    VSF_HAL_ASSERT(pwm_ptr != NULL);
    vsf_timer_fini(pwm_ptr->timer);
}

fsm_rt_t vsf_timer_gpio_multi_pwm_enable(vsf_timer_gpio_multi_pwm_t *pwm_ptr)
{
    VSF_HAL_ASSERT(pwm_ptr != NULL);

    if (!pwm_ptr->enabled) {
        pwm_ptr->enabled = true;
        vsf_timer_enable(pwm_ptr->timer);

        pwm_ptr->cur_pulse = 0;
        __vsf_timer_gpio_multi_pwm_isrhandler(pwm_ptr, pwm_ptr->timer, VSF_TIMER_IRQ_MASK_OVERFLOW);
    }
    return fsm_rt_cpl;
}

fsm_rt_t vsf_timer_gpio_multi_pwm_disable(vsf_timer_gpio_multi_pwm_t *pwm_ptr)
{
    VSF_HAL_ASSERT(pwm_ptr != NULL);

    if (pwm_ptr->enabled) {
        pwm_ptr->enabled = false;
        vsf_timer_disable(pwm_ptr->timer);
    }
    return fsm_rt_cpl;
}

vsf_err_t vsf_timer_gpio_multi_pwm_set(vsf_timer_gpio_multi_pwm_t *pwm_ptr, uint8_t channel, uint32_t period, uint32_t pulse)
{
    VSF_HAL_ASSERT(pwm_ptr != NULL);
    VSF_HAL_ASSERT(channel < pwm_ptr->channel_num);
    if (pwm_ptr->period != 0) {
        VSF_HAL_ASSERT(period == pwm_ptr->period);
    }

    return VSF_ERR_NONE;
}

uint32_t vsf_timer_gpio_multi_pwm_get_freq(vsf_timer_gpio_multi_pwm_t *pwm_ptr)
{
    VSF_HAL_ASSERT(pwm_ptr != NULL);

    return pwm_ptr->freq;
}

/*============================ INCLUDES ======================================*/

#define VSF_HAL_TEMPLATE_IMP_EXTERN_OP                  ENABLED
#define VSF_PWM_CFG_IMP_PREFIX                          vsf_timer_gpio_multi
#define VSF_PWM_CFG_IMP_UPCASE_PREFIX                   VSF_TIMER_GPIO_MULTI
#include "hal/driver/common/pwm/pwm_template.inc"

/*============================ IMPLEMENTATION ================================*/

static void __vsf_timer_gpio_single_pwm_update(vsf_timer_gpio_single_pwm_t *pwm_ptr)
{
    pwm_ptr->update_egde = false;
    pwm_ptr->pulse_shadow = pwm_ptr->pulse;
    pwm_ptr->period_shadow = pwm_ptr->period;

    if (pwm_ptr->period_shadow == pwm_ptr->pulse_shadow) {
        pwm_ptr->gpio_control(pwm_ptr->reverse ^ !!pwm_ptr->period_shadow);
        vsf_timer_set_period(pwm_ptr->timer, 0);
        pwm_ptr->timer_started = false;
    } else {
        pwm_ptr->gpio_control(pwm_ptr->reverse);
        vsf_timer_set_period(pwm_ptr->timer, pwm_ptr->pulse_shadow);
        pwm_ptr->timer_started = true;
    }
}

static void __vsf_timer_gpio_single_pwm_isrhandler(void *target_ptr, vsf_timer_t *timer_ptr, vsf_timer_irq_mask_t irq_mask)
{
    vsf_timer_gpio_single_pwm_t *pwm_ptr = (vsf_timer_gpio_single_pwm_t *)target_ptr;
    if (pwm_ptr->update_egde) {
        __vsf_timer_gpio_single_pwm_update(pwm_ptr);
    } else {
        pwm_ptr->gpio_control(!pwm_ptr->reverse);
        vsf_timer_set_period(pwm_ptr->timer, pwm_ptr->period_shadow - pwm_ptr->pulse_shadow);
        pwm_ptr->update_egde = true;
    }
}

vsf_err_t vsf_timer_gpio_single_pwm_init(vsf_timer_gpio_single_pwm_t *pwm_ptr, vsf_pwm_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != pwm_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    VSF_HAL_ASSERT(NULL != pwm_ptr->timer);

    pwm_ptr->freq = cfg_ptr->freq;
    pwm_ptr->timer_started = false;
    vsf_err_t err = vsf_timer_init(pwm_ptr->timer, &(vsf_timer_cfg_t){
        .mode           = VSF_TIMER_BASE_ONESHOT,
        .period         = 0,
        .freq           = cfg_ptr->freq,
        .isr            = {
            .handler_fn = __vsf_timer_gpio_single_pwm_isrhandler,
            .target_ptr = pwm_ptr,
            .prio       = VSF_ARCH_PRIO_HIGHEST,
        },
    });
    if (err != VSF_ERR_NONE) {
        return err;
    }

    vsf_timer_irq_enable(pwm_ptr->timer, VSF_TIMER_IRQ_MASK_OVERFLOW);
    return VSF_ERR_NONE;
}

void vsf_timer_gpio_single_pwm_fini(vsf_timer_gpio_single_pwm_t *pwm_ptr)
{
    VSF_HAL_ASSERT(pwm_ptr != NULL);
    vsf_timer_fini(pwm_ptr->timer);
}

fsm_rt_t vsf_timer_gpio_single_pwm_enable(vsf_timer_gpio_single_pwm_t *pwm_ptr)
{
    VSF_HAL_ASSERT(pwm_ptr != NULL);

    if (!pwm_ptr->enabled) {
        pwm_ptr->enabled = true;
        vsf_timer_enable(pwm_ptr->timer);
        __vsf_timer_gpio_single_pwm_update(pwm_ptr);
    }
    return fsm_rt_cpl;
}

fsm_rt_t vsf_timer_gpio_single_pwm_disable(vsf_timer_gpio_single_pwm_t *pwm_ptr)
{
    VSF_HAL_ASSERT(pwm_ptr != NULL);

    if (pwm_ptr->enabled) {
        pwm_ptr->enabled = false;
    }
    return fsm_rt_cpl;
}

vsf_err_t vsf_timer_gpio_single_pwm_set(vsf_timer_gpio_single_pwm_t *pwm_ptr, uint8_t channel, uint32_t period, uint32_t pulse)
{
    VSF_HAL_ASSERT(pwm_ptr != NULL);
    VSF_HAL_ASSERT(0 == channel);

    // Updating the pwm configuration needs to be protected so that it won't
    // be interrupted by timer interrupts
    vsf_protect_t orig = __vsf_timer_gpio_protect();
    pwm_ptr->period = period;
    pwm_ptr->pulse = pulse;
    if (pwm_ptr->enabled && !pwm_ptr->timer_started) {
        __vsf_timer_gpio_single_pwm_update(pwm_ptr);
    }
    __vsf_timer_gpio_unprotect(orig);

    return VSF_ERR_NONE;
}

uint32_t vsf_timer_gpio_single_pwm_get_freq(vsf_timer_gpio_single_pwm_t *pwm_ptr)
{
    VSF_HAL_ASSERT(pwm_ptr != NULL);
    return pwm_ptr->freq;
}

/*============================ INCLUDES ======================================*/

#define VSF_PWM_CFG_IMP_EXTERN_OP                       ENABLED
#define VSF_PWM_CFG_IMP_PREFIX                          vsf_timer_gpio_single
#define VSF_PWM_CFG_IMP_UPCASE_PREFIX                   VSF_TIMER_GPIO_SINGLE
#include "hal/driver/common/pwm/pwm_template.inc"

#endif      // VSF_HAL_USE_PWM
