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

#ifndef __VSF_74HC595_GPIO_H__
#define __VSF_74HC595_GPIO_H__

/* Implement GPIO device driver on 74HC595
   connection:
    control api     <==>    signal(pin) <==>    signal(pin) shared with 74HC165
    oe_control      <==>    nOE(13)
    latch_control   <==>    ST_CP(12)   <==>    nPL(1)
    clock_control   <==>    SH_CP(11)   <==>    CP(2)
    serial_output   <==>    DS(14)
 */

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_GPIO == ENABLED && VSF_HAL_USE_74HC595_GPIO == ENABLED

#if defined(__VSF_HAL_74HC595_GPIO_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_74HC595_GPIO_CFG_MULTI_CLASS
#   define VSF_74HC595_GPIO_CFG_MULTI_CLASS VSF_GPIO_CFG_MULTI_CLASS
#endif

#if VSF_74HC595_GPIO_CFG_MULTI_CLASS == ENABLED
#   define __VSF_74HC595_GPIO_HALOP         .vsf_gpio.op = &vsf_74hc595_gpio_op,
#else
#   define __VSF_74HC595_GPIO_HALOP
#endif

#define __VSF_74HC595_GPIO_INIT(__CASCADE_NUM, __OP, __PARAM)                   \
            __VSF_74HC595_GPIO_HALOP                                            \
            .op                 = (vsf_74hc595_op_t *)(__OP),                   \
            .param              = (__PARAM),                                    \
            .cascade_num        = (__CASCADE_NUM),
#define VSF_74HC595_GPIO_INIT(__CASCADE_NUM, __OP, __PARAM)                     \
            __VSF_74HC595_GPIO_INIT((__CASCADE_NUM), (__OP), (__PARAM))

#define __describe_74hc595_gpio(__name, __cascade_num, __param, __oe_control,   \
                __latch_control, __clock_control, __serial_output)              \
            static const vsf_74hc595_op_t VSF_MCONNECT3(__, __name, _op) = {    \
                .oe_control     = (__oe_control),                               \
                .latch_control  = (__latch_control),                            \
                .clock_control  = (__clock_control),                            \
                .serial_output  = (__serial_output),                            \
            };                                                                  \
            vsf_74hc595_gpio_t __name = {                                       \
                __VSF_74HC595_GPIO_INIT((__cascade_num),                        \
                                &VSF_MCONNECT3(__, __name, _op), (__param))     \
            };

#define describe_74hc595_gpio(__name, __cascade_num, __param, __oe_control,     \
                __latch_control, __clock_control, __serial_output)              \
            __describe_74hc595_gpio(__name, (__cascade_num), (__param),         \
                (__oe_control), (__latch_control), (__clock_control), (__serial_output))

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_74hc595_op_t {
    void (*oe_control)(void *param, uint_fast8_t bit);
    void (*latch_control)(void *param, uint_fast8_t bit);
    void (*clock_control)(void *param, uint_fast8_t bit);
    void (*serial_output)(void *param, uint_fast8_t bit);
} vsf_74hc595_op_t;

vsf_class(vsf_74hc595_gpio_t) {
    public_member(
#if VSF_74HC595_GPIO_CFG_MULTI_CLASS == ENABLED
        vsf_gpio_t vsf_gpio;
#endif
        vsf_74hc595_op_t *op;
        void *param;
        uint32_t output_value;
        uint8_t cascade_num;
        bool output;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_74HC595_GPIO_CFG_MULTI_CLASS == ENABLED
extern const vsf_gpio_op_t vsf_74hc595_gpio_op;
#endif

/*============================ PROTOTYPES ====================================*/

extern void vsf_74hc595_gpio_init(vsf_74hc595_gpio_t *gpio_ptr);
extern void vsf_74hc595_gpio_port_config_pins(vsf_74hc595_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_cfg_t *cfg_ptr);
extern void vsf_74hc595_gpio_set_direction(vsf_74hc595_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_pin_mask_t direction_mask);
extern vsf_gpio_pin_mask_t vsf_74hc595_gpio_get_direction(vsf_74hc595_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask);
extern void vsf_74hc595_gpio_set_input(vsf_74hc595_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask);
extern void vsf_74hc595_gpio_set_output(vsf_74hc595_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask);
extern void vsf_74hc595_gpio_switch_direction(vsf_74hc595_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask);
extern vsf_gpio_pin_mask_t vsf_74hc595_gpio_read(vsf_74hc595_gpio_t *gpio_ptr);
extern void vsf_74hc595_gpio_write(vsf_74hc595_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_pin_mask_t value);
extern void vsf_74hc595_gpio_set(vsf_74hc595_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask);
extern void vsf_74hc595_gpio_clear(vsf_74hc595_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask);
extern void vsf_74hc595_gpio_toggle(vsf_74hc595_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask);

#ifdef __cplusplus
}
#    endif

#endif
#endif
