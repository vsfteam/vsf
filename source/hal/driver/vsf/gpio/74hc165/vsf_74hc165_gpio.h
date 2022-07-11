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

#ifndef __VSF_74HC165_GPIO_H__
#define __VSF_74HC165_GPIO_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_GPIO_USE_74HC165 == ENABLED

#if defined(__VSF_HAL_GPIO_74HC164_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define __VSF_74HC165_GPIO_INIT(__CASCADE_NUM, __OP, __PARAM)                   \
            .op                 = (vsf_74hc165_gpio_op_t *)(__OP),              \
            .param              = (__PARAM),                                    \
            .cascade_num        = (__CASCADE_NUM),
#define VSF_74HC165_GPIO_INIT(__CASCADE_NUM, __OP, __PARAM)                     \
            __VSF_74HC165_GPIO_INIT((__CASCADE_NUM), (__OP), (__PARAM))

#define __describe_74hc165_gpio(__name, __cascade_num, __param, __load_control, \
                                __clock_control, __serial_input)                \
            static const vsf_74hc165_gpio_op_t VSF_MCONNECT3(__, __name, _op) = {\
                .load_control   = (__load_control),                             \
                .clock_control  = (__clock_control),                            \
                .serial_input   = (__serial_input),                             \
            };                                                                  \
            vsf_74hc165_gpio_t __name = {                                       \
                __VSF_74HC165_GPIO_INIT((__cascade_num),                        \
                                &VSF_MCONNECT3(__, __name, _op), (__param))     \
            };

#define describe_74hc165_gpio(__name, __cascade_num, __param, __load_control,   \
                                __clock_control, __serial_input)                \
            __describe_74hc165_gpio(__name, (__cascade_num), (__param),         \
                                (__load_control), (__clock_control), (__serial_input))

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_74hc165_gpio_op_t {
    void (*ce_control)(void *param, uint_fast8_t bit);
    void (*load_control)(void *param, uint_fast8_t bit);
    void (*clock_control)(void *param, uint_fast8_t bit);
    uint_fast8_t (*serial_input)(void *param);
} vsf_74hc165_gpio_op_t;

vsf_class(vsf_74hc165_gpio_t) {
    public_member(
        vsf_74hc165_gpio_op_t *op;
        void *param;
        uint8_t cascade_num;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_74hc165_gpio_config_pin(vsf_74hc165_gpio_t *gpio_ptr, uint32_t pin_mask, uint_fast32_t feature);
extern void vsf_74hc165_gpio_set_direction(vsf_74hc165_gpio_t *gpio_ptr, uint32_t direction_mask, uint32_t pin_mask);
extern uint32_t vsf_74hc165_gpio_get_direction(vsf_74hc165_gpio_t *gpio_ptr, uint32_t pin_mask);
extern void vsf_74hc165_gpio_set_input(vsf_74hc165_gpio_t *gpio_ptr, uint32_t pin_mask);
extern void vsf_74hc165_gpio_set_output(vsf_74hc165_gpio_t *gpio_ptr, uint32_t pin_mask);
extern void vsf_74hc165_gpio_switch_direction(vsf_74hc165_gpio_t *gpio_ptr, uint32_t pin_mask);
extern uint32_t vsf_74hc165_gpio_read(vsf_74hc165_gpio_t *gpio_ptr);
extern void vsf_74hc165_gpio_write(vsf_74hc165_gpio_t *gpio_ptr, uint32_t value, uint32_t pin_mask);
extern void vsf_74hc165_gpio_set(vsf_74hc165_gpio_t *gpio_ptr, uint32_t pin_mask);
extern void vsf_74hc165_gpio_clear(vsf_74hc165_gpio_t *gpio_ptr, uint32_t pin_mask);
extern void vsf_74hc165_gpio_toggle(vsf_74hc165_gpio_t *gpio_ptr, uint32_t pin_mask);

#ifdef __cplusplus
}
#endif

#endif
#endif
