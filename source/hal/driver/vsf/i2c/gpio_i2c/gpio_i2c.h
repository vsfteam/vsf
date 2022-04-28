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

#ifndef VSF_GPIO_I2C_CFG_CUSTOM_CALL_BACK_TIMER
#   define VSF_GPIO_I2C_CFG_CUSTOM_CALL_BACK_TIMER          DISALBED
#endif

#if VSF_GPIO_I2C_CFG_CUSTOM_CALL_BACK_TIMER == DISABLED
#   ifdef VSF_GPIO_I2C_INCLUDE_HEADER
#       define VSF_GPIO_I2C_INCLUDE_HEADER                  "kernel/vsf_kernel.h"
#   endif
#endif

#ifndef VSF_GPIO_I2C_CFG_MAX_CLOCK_HZ
#   define VSF_GPIO_I2C_CFG_MAX_CLOCK_HZ                    1000000
#endif

/*============================ INCLUDES ======================================*/

#ifdef VSF_GPIO_I2C_INCLUDE_HEADER
#   include VSF_GPIO_I2C_INCLUDE_HEADER
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if VSF_GPIO_I2C_CFG_CUSTOM_CALL_BACK_TIMER == DISABLED
typedef vsf_callback_timer_t vsf_gpio_i2c_callback_timer_t;
#endif

typedef void vsf_gpio_i2c_cb_fn(vsf_gpio_i2c_callback_timer_t *on_timer);

/*============================ INCLUDES ======================================*/

#define VSF_I2C_CFG_API_DECLARATION_PREFIX              vsf_gpio


#include "hal/driver/common/template/vsf_template_i2c.h"

/*============================ TYPES =========================================*/

vsf_class(vsf_gpio_i2c_t) {
    private_member(
#if VSF_USART_CFG_MULTI_CLASS == ENABLED
        vsf_i2c_t vsf_i2c;
#endif

        vsf_i2c_request_t                   request;
        uint8_t                             gpio_i2c_cmd;
        uint16_t                            data;

        em_i2c_cmd_t cmd;
        em_i2c_irq_mask_t irq_mask;
        em_i2c_irq_mask_t                   enabled_irq_mask;
        uint16_t                            bit_mask;

        vsf_gpio_i2c_callback_timer_t       callback_timer;

        uint8_t is_busy     :1;
        uint8_t is_enabled  :1;
    )

    public_member(
        struct {
            vsf_gpio_t                     *gpio;
            uint32_t                        pin_mask;
        } scl, sda;
    )
};


/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_GPIO_I2C_CFG_CUSTOM_CALL_BACK_TIMER == DISABLED
static inline void vsf_gpio_i2c_callback_timer_init(vsf_gpio_i2c_callback_timer_t *timer_ptr, vsf_gpio_i2c_cb_fn *fn)
{
    timer_ptr->on_timer = fn;
    vsf_callback_timer_init(timer_ptr);
}

static inline void vsf_gpio_i2c_callback_timer_add_us(vsf_gpio_i2c_callback_timer_t *timer_ptr, uint_fast32_t us)
{
    vsf_callback_timer_add_us(timer_ptr, us);
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* (VSF_HAL_USE_I2C == ENABLED) && (__HAL_DRIVER_GPIO_I2C_H__ == ENABLED) */
#endif /* EOF */