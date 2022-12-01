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

#ifndef VSF_GPIO_I2C_CFG_MULTI_CLASS
#   define VSF_GPIO_I2C_CFG_MULTI_CLASS     VSF_I2C_CFG_MULTI_CLASS
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_GPIO_I2C_CFG_MULTI_CLASS == ENABLED
#   define __describe_gpio_i2c_op()         .op = &vsf_gpio_i2c_op,
#else
#   define __describe_gpio_i2c_op()
#endif

#define __describe_gpio_i2c(__name, __gpio_port, __fn_delay, __scl_pin, __sda_pin)\
    vsf_gpio_i2c_t __name = {                                                   \
        __describe_gpio_i2c_op()                                                \
        .port           = (__gpio_port),                                        \
        .fn_delay       = (__fn_delay),                                         \
        .scl_pin        = (__scl_pin),                                          \
        .sda_pin        = (__sda_pin),                                          \
    };

#define describe_gpio_i2c(__name, __gpio_port, __fn_delay, __scl_pin, __sda_pin)\
            __describe_gpio_i2c(__name, (__gpio_port), (__fn_delay), (__scl_pin), (__sda_pin))

/*============================ TYPES =========================================*/

vsf_dcl_class(vsf_gpio_i2c_t)

typedef void (*vsf_gpio_i2c_delay)(vsf_gpio_i2c_t *gpio_i2c_ptr);

vsf_class(vsf_gpio_i2c_t) {
#if VSF_GPIO_I2C_CFG_MULTI_CLASS == ENABLED
    public_member(
        implement(vsf_i2c_t)
    )
#endif
    private_member(
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
#define VSF_I2C_CFG_DEC_EXTERN_OP           ENABLED
#include "hal/driver/common/i2c/i2c_template.h"

#ifdef __cplusplus
}
#endif

#endif /* (VSF_HAL_USE_I2C == ENABLED) && (__HAL_DRIVER_GPIO_I2C_H__ == ENABLED) */
#endif /* EOF */