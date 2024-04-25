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

#ifndef __HAL_DRIVER_VSF_H__
#define __HAL_DRIVER_VSF_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_DISTBUS == ENABLED
#   include "./distbus/driver.h"
#endif

#if VSF_HAL_USE_GPIO == ENABLED
// emulate GPIO driver on 74hc165
#   if VSF_HAL_USE_74HC165_GPIO == ENABLED
#       include "./gpio/74hc165/vsf_gpio_74hc165.h"
#   endif

// emulate GPIO driver on 74hc595
#   if VSF_HAL_USE_74HC595_GPIO == ENABLED
#       include "./gpio/74hc595/vsf_gpio_74hc595.h"
#   endif
#endif

#if VSF_HAL_USE_I2C == ENABLED
// emulate I2C driver with GPIO
#   if VSF_HAL_USE_GPIO_I2C == ENABLED
#       include "./i2c/gpio_i2c/vsf_gpio_i2c.h"
#   endif
#endif

#if VSF_HAL_USE_PWM == ENABLED
// emulate PWM driver with TIMER + GPIO
#   if VSF_HAL_USE_TIMER_GPIO_PWM == ENABLED
#       include "./pwm/timer_gpio/vsf_timer_gpio_pwm.h"
#   endif
#endif

#if VSF_HAL_USE_ADC == ENABLED
#   if VSF_HAL_USE_ADC128D818_ADC == ENABLED
#       include "./adc/TI/adc128d818/vsf_adc128d818.h"
#   endif
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
