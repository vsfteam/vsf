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
 *  limitations under the License.                                           *x
 *                                                                           *
 ****************************************************************************/

#ifndef __HAL_DRIVER_TIMER_GPIO_PWM_H__
#define __HAL_DRIVER_TIMER_GPIO_PWM_H__

/* Implement PWM device driver with TIMER + GPIO
 *  PWMs are at the same frequency with different duty cycle.
 *  timer_gpio_multi_pwm not ready:
 *      Used to support multi pwm channels per timer, but will take more time in timer interrupt handler.
 *  timer_gpio_single_pwm:
 *      Used to support single pwm channel per timer.
 */

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_PWM == ENABLED && VSF_HAL_USE_TIMER_GPIO_PWM == ENABLED

// for VSF_MFOREACH
#include "utilities/vsf_utilities.h"

#if defined(__VSF_HAL_TIMER_GPIO_PWM_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_TIMER_GPIO_PWM_CFG_MULTI_CLASS
#   define VSF_TIMER_GPIO_PWM_CFG_MULTI_CLASS       VSF_PWM_CFG_MULTI_CLASS
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __VSF_TIMER_GPIO_MULTI_PWM_INIT(__TIMER, __GPIO, __CHANNEL_NUM, __CHANNELS)\
            .timer              = (vsf_timer_t *)(__TIMER),                     \
            .gpio               = (vsf_gpio_t *)(__GPIO),                       \
            .channels           = (__CHANNELS),                                 \
            .channel_num        = (__CHANNEL_NUM),
#define VSF_TIMER_GPIO_MULTI_PWM_INIT(__TIMER, __GPIO, __CHANNEL_NUM, __CHANNELS)\
            __VSF_TIMER_GPIO_MULTI_PWM_INIT((__TIMER), (__GPIO), (__CHANNEL_NUM), (__CHANNELS))

#define __describe_timer_gpio_multi_pwm_channel(__config)                       \
            { .config = (__config) },

#define __describe_timer_gpio_multi_pwm(__name, __timer, __gpio, ...)           \
            static vsf_timer_gpio_multi_pwm_channel_t VSF_MCONNECT3(__, __name, _channels)[] = {\
                VSF_MFOREACH(__describe_timer_gpio_multi_pwm_channel, __VA_ARGS__)\
            };                                                                  \
            vsf_timer_gpio_multi_pwm_t __name = {                               \
                VSF_TIMER_GPIO_MULTI_PWM_INIT((__timer), (__gpio),              \
                        dimof(VSF_MCONNECT3(__, __name, _channels)),            \
                        VSF_MCONNECT3(__, __name, _channels))                   \
            };

#define describe_timer_gpio_multi_pwm(__name, __timer, __gpio, ...)             \
            __describe_timer_gpio_multi_pwm(__name, (__timer), (__gpio), __VA_ARGS__)


#define __VSF_TIMER_GPIO_SINGLE_PWM_INIT(__TIMER, __GPIO, __CHANNEL_NUM, __CHANNELS)\
            .timer              = (vsf_timer_t *)(__TIMER),                     \
            .gpio               = (vsf_gpio_t *)(__GPIO),                       \
            .channels           = (__CHANNELS),                                 \
            .channel_num        = (__CHANNEL_NUM),
#define VSF_TIMER_GPIO_SINGLE_PWM_INIT(__TIMER, __GPIO, __CHANNEL_NUM, __CHANNELS)\
            __VSF_TIMER_GPIO_SINGLE_PWM_INIT((__TIMER), (__GPIO), (__CHANNEL_NUM), (__CHANNELS))

#define __describe_timer_gpio_single_pwm(__name, __timer, __gpio, __pin)        \
            vsf_timer_gpio_single_pwm_t __name = {                              \
                VSF_TIMER_GPIO_SINGLE_PWM_INIT((__timer), (__gpio),             \
                        dimof(VSF_MCONNECT3(__, __name, _channels)),            \
                        VSF_MCONNECT3(__, __name, _channels))                   \
            };

#define describe_timer_gpio_single_pwm(__name, __timer, __gpio, ...)            \
            __describe_timer_gpio_single_pwm(__name, (__timer), (__gpio), __VA_ARGS__)

/*============================ TYPES =========================================*/

enum {
    VSF_TIMER_GPIO_PWM_NORMAL   = 0,
    VSF_TIMER_GPIO_PWM_REVERSE  = 1,
};

vsf_class(vsf_timer_gpio_multi_pwm_channel_t) {
    public_member(
        union {
            struct {
                uint8_t pin_idx     : 7;
                uint8_t reverse     : 1;
            };
            uint8_t config;
        };
    )
    private_member(
        uint32_t pulse;
        uint32_t pulse_shadow;
    )
};

vsf_class(vsf_timer_gpio_multi_pwm_t) {
    public_member(
#if VSF_TIMER_GPIO_PWM_CFG_MULTI_CLASS == ENABLED
        vsf_pwm_t vsf_pwm;
#endif
        vsf_timer_t *timer;
        vsf_gpio_t *gpio;
        vsf_timer_gpio_multi_pwm_channel_t *channels;
        uint8_t channel_num;
    )
    private_member(
        bool enabled;
        uint32_t period;
        uint32_t freq;
        uint32_t cur_pulse;
    )
};

vsf_class(vsf_timer_gpio_single_pwm_t) {
    public_member(
#if VSF_TIMER_GPIO_PWM_CFG_MULTI_CLASS == ENABLED
        vsf_pwm_t vsf_pwm;
#endif
        vsf_timer_t *timer;
        void (*gpio_control)(uint_fast8_t value);
        uint8_t pin;
        bool reverse;
    )
    private_member(
        bool enabled;
        bool update_egde;
        bool timer_started;
        uint32_t freq;
        uint32_t period;
        uint32_t pulse;
        uint32_t period_shadow;
        uint32_t pulse_shadow;
    )
};

/*============================ INCLUDES ======================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_HAL_USE_PWM && VSF_HAL_USE_TIMER_GPIO_PWM
#endif
/* EOF */
