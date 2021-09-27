/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

#ifndef __HAL_DRIVER_I2C_GPIO_H__
#define __HAL_DRIVER_I2C_GPIO_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"
#if VSF_HAL_USE_I2C_GPIO == ENABLED

//todo: Unified GPIO header file
#if defined(__AIC8800__)

#include "../gpio/gpio.h"

#undef I2C_TEMPLATE_USE_MODULAR_NAME
#define I2C_TEMPLATE_USE_MODULAR_NAME           gpio

/*============================ MACROS ========================================*/

#ifndef  VSF_I2C_GPIO_USE_CALL_BACK_TIMER
#   define VSF_I2C_GPIO_USE_CALL_BACK_TIMER                 vsf_callback_timer_t
#   include "kernel/vsf_kernel.h"
typedef vsf_err_t timer_setting_function_t(VSF_I2C_GPIO_USE_CALL_BACK_TIMER *time_ptr, uint_fast32_t times);
#   define VSF_I2C_GPIO_USE_CALL_BACK_TIME_FN               vsf_callback_timer_add_us
#else
extern void i2c_gpio_callback(i2c_type_ptr *i2c_ptr);
#endif

#ifndef VSF_HAL_I2C_IMP_REQUEST_BY_CMD
#   define VSF_HAL_I2C_IMP_REQUEST_BY_CMD                   ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/

#include "hal/driver/common/template/vsf_template_i2c.h"
#if VSF_HAL_I2C_IMP_REQUEST_BY_CMD == ENABLED
#   include "hal/driver/common/i2c/__i2c_common.h"
#endif
/*============================ TYPES =========================================*/

struct i2c_type_status {
    union {
        inherit(peripheral_status_t)
        struct {
            uint32_t                        : 1;
            uint32_t            is_enabled  : 1;
            uint32_t                        : 30;
        } status_bool;
    };
};

//struct i2c_capability_t {
//    uint32_t                    temp        :32;
//    // TODO
//};

typedef struct __i2c_type_ptr {
    i2c_cfg_t                           cfg;
    i2c_capability_t                    capability;
    i2c_type_status                        status;
    em_i2c_irq_mask_t                   enabled_irq_mask;
    em_i2c_irq_mask_t                   irq_mask;
    em_i2c_cmd_t                        cmd;
    uint16_t                            data;
    uint8_t                             i2c_gpio_cmd;
} __i2c_type_ptr;

typedef struct i2c_type_ptr {
    implement(__i2c_type_ptr)
    VSF_I2C_GPIO_USE_CALL_BACK_TIMER    callback_timer;
    timer_setting_function_t            *time_fn;
    struct {
        vsf_gpio_t                      *gpio;
        uint32_t                        scl_pin_mask;
        uint32_t                        sda_pin_mask;
    } gpio_info;
    uint16_t                            bit_mask;
#if VSF_HAL_I2C_IMP_REQUEST_BY_CMD == ENABLED
    vsf_hal_i2c_def_req_by_cmd()
#endif
} i2c_type_ptr;

/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern i2c_type_ptr vsf_gpio_i2c0;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif /* VSF_HAL_USE_I2C_GPIO */
#endif /* __AIC8800__ */
#endif /* EOF */