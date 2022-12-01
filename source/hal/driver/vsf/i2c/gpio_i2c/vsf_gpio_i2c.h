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

#ifndef __HAL_DRIVER_GPIO_I2C_H__
#define __HAL_DRIVER_GPIO_I2C_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if (VSF_HAL_USE_I2C == ENABLED) && (VSF_HAL_USE_GPIO_I2C == ENABLED)

#if defined(__VSF_HAL_GPIO_I2C_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ INCLUDES ======================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_dcl_class(vsf_gpio_i2c_t)

typedef void (*vsf_gpio_i2c_delay)(vsf_gpio_i2c_t *gpio_i2c_ptr);

vsf_class(vsf_gpio_i2c_t) {
    private_member(
#if VSF_USART_CFG_MULTI_CLASS == ENABLED
        vsf_i2c_t vsf_i2c;
#endif

        vsf_i2c_cfg_t                       cfg;
        vsf_i2c_irq_mask_t                  irq_mask;
        vsf_i2c_irq_mask_t                  enabled_irq_mask;
        uint16_t                            transfered_count;

        bool                                is_busy;
        bool                                is_enabled;
    )

    public_member(
        vsf_gpio_t                          *port;
        vsf_gpio_i2c_delay                  fn_delay;
        uint8_t                             scl_pin;
        uint8_t                             sda_pin;
    )
};


/*============================ INCLUDES ======================================*/

#define VSF_I2C_CFG_DEC_PREFIX              vsf_gpio
#define VSF_I2C_CFG_DEC_UPCASE_PREFIX       VSF_GPIO
#include "hal/driver/common/i2c/i2c_template.h"

#ifdef __cplusplus
}
#endif

#endif /* (VSF_HAL_USE_I2C == ENABLED) && (__HAL_DRIVER_GPIO_I2C_H__ == ENABLED) */
#endif /* EOF */