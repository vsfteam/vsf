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

#ifndef __HAL_DRIVER_AIC8800_HW_GPIO_H__
#define __HAL_DRIVER_AIC8800_HW_GPIO_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_GPIO == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vsf_gpio_mode_t {
    // bit 9  : IOMUX_AGPIO_CONFIG_PULL_UP_LSB
    // bit 8  : IOMUX_AGPIO_CONFIG_PULL_DN_LSB
    // bit 16 : IOMUX_GPIO_CONFIG_PULL_FRC_LSB
    __AIC8800_GPIO_MODE_ALL_BITS      = (1 << 8) | (1 << 9) | (1 << 16),

    VSF_GPIO_PULL_UP                  = (1 << 16) | (1 << 9),
    VSF_GPIO_PULL_DOWN                = (1 << 16) | (1 << 8),
    VSF_GPIO_NO_PULL_UP_DOWN          = (0 << 16) | (0 << 8),

    VSF_GPIO_INPUT                    = (0 << 17),
    VSF_GPIO_OUTPUT_OPEN_DRAIN        = (1 << 17),
    VSF_GPIO_OUTPUT_PUSH_PULL         = (2 << 17),
    VSF_GPIO_ANALOG                   = (3 << 17),
    VSF_GPIO_EXTI                     = (4 << 17),
    VSF_GPIO_AF                       = (5 << 17),

    VSF_GPIO_EXTI_MODE_NONE           = (0 << 21),
    VSF_GPIO_EXTI_MODE_LOW_LEVEL      = (1 << 21),
    VSF_GPIO_EXTI_MODE_HIGH_LEVEL     = (2 << 21),
    VSF_GPIO_EXTI_MODE_RISING         = (3 << 21),
    VSF_GPIO_EXTI_MODE_FALLING        = (4 << 21),
    VSF_GPIO_EXTI_MODE_RISING_FALLING = (5 << 21),

} vsf_gpio_mode_t;

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

/**
 \~english
 @brief To ensure that the GPIO is working properly, it needs to be called inside vsf_driver_init()

 \~chinese
 @brief 为了确保 AIC8800 的 GPIO 工作正常，需要在 vsf_driver_init() 里调用这个 API
 */
extern void __vsf_hw_aic8800_gpio_init(void);

#endif /* VSF_HAL_USE_GPIO */
#endif /* __HAL_DRIVER_AIC8800_HW_GPIO_H__ */
