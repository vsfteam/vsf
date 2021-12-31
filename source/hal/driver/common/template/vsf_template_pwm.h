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

#ifndef __HAL_DRIVER_PWM_INTERFACE_H__
#define __HAL_DRIVER_PWM_INTERFACE_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

//! \name pwm configuration
//! @{
typedef struct pwm_cfg_t pwm_cfg_t;
struct pwm_cfg_t {
    uint32_t mini_clock_freq;    //!< The minimum clock frequency, in Hz
};
//! @}

typedef struct vsf_pwm_t vsf_pwm_t;

//! \name class: pwm_t
//! @{
def_interface(i_pwm_t)
    implement(i_peripheral_t);

    vsf_err_t (*Init)(pwm_cfg_t *pCfg);

    vsf_err_t (*Set)(uint8_t channel, uint32_t period, uint32_t pulse);
    vsf_err_t (*Set_ms)(uint8_t channel, uint32_t period, uint32_t pulse);
    vsf_err_t (*Set_us)(uint8_t channel, uint32_t period, uint32_t pulse);
    vsf_err_t (*Set_ns)(uint8_t channel, uint32_t period, uint32_t pulse);
end_def_interface(i_pwm_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_pwm_init(vsf_pwm_t *pwm_ptr, pwm_cfg_t *cfg_ptr);
extern fsm_rt_t vsf_pwm_enable(vsf_pwm_t *pwm_ptr);
extern fsm_rt_t vsf_pwm_disable(vsf_pwm_t *pwm_ptr);

/**
 *  Set the period width and pluse width for a channel
 *
 * @param[in] pwm_ptr pwm instance
 * @param[in] period pwm period width (in clock counter)
 * @param[in] pulse pwm pluse width (in clock counter)
 */
extern vsf_err_t vsf_pwm_set(vsf_pwm_t *pwm_ptr,
                             uint8_t    channel,
                             uint32_t   period,
                             uint32_t   pulse);

/**
 *  Set the period width(ms) and pluse width(ms) for a channel
 *
 * @param[in] pwm_ptr pwm instance
 * @param[in] period pwm period width (in milli-second)
 * @param[in] pulse pwm pluse width (in milli-second)
 */
extern vsf_err_t vsf_pwm_set_ms(vsf_pwm_t *pwm_ptr,
                                uint8_t    channel,
                                uint32_t   period,
                                uint32_t   pulse);

/**
 *  Set the period width(us) and pluse width(us) for a channel
 *
 * @param[in] pwm_ptr pwm instance
 * @param[in] period pwm period width (in micro-seconds)
 * @param[in] pulse pwm pluse width (in micro-seconds)
 */
extern vsf_err_t vsf_pwm_set_us(vsf_pwm_t *pwm_ptr,
                                uint8_t    channel,
                                uint32_t   period,
                                uint32_t   pulse);

/**
 *  Set the period width(ns) and pluse width(ns) for a channel
 *
 * @param[in] pwm_ptr pwm instance
 * @param[in] period pwm period width (in nano second)
 * @param[in] pulse pwm pluse width (in nano second)
 */
extern vsf_err_t vsf_pwm_set_ns(vsf_pwm_t *pwm_ptr,
                                uint8_t    channel,
                                uint32_t   period,
                                uint32_t   pulse);

/**
 * Get clock frequency
 *
 * @param[in] pwm_ptr pwm instance
 * @return clock frequency (in Hz)
 */
extern uint32_t vsf_pwm_get_freq(vsf_pwm_t *pwm_ptr);

/**
 *  TODO: API for query number of pwm channels
 */

#ifdef __cplusplus
}
#endif

#endif
