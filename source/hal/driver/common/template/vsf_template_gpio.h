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

#ifndef __HAL_DRIVER_GPIO_INTERFACE_H__
#define __HAL_DRIVER_GPIO_INTERFACE_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// multi-class support enabled by default for maximum availability.
#ifndef VSF_GPIO_CFG_MULTI_CLASS
#   define VSF_GPIO_CFG_MULTI_CLASS                 ENABLED
#endif

// application code can redefine it
#ifndef VSF_GPIO_CFG_PREFIX
#   if VSF_GPIO_CFG_MULTI_CLASS == ENABLED
#       define VSF_GPIO_CFG_PREFIX                  vsf
#   elif defined(VSF_HW_GPIO_COUNT) && (VSF_HW_GPIO_COUNT != 0)
#       define VSF_GPIO_CFG_PREFIX                  vsf_hw
#   else
#       define VSF_GPIO_CFG_PREFIX                  vsf
#   endif
#endif

#ifndef VSF_GPIO_CFG_FUNCTION_RENAME
#   define VSF_GPIO_CFG_FUNCTION_RENAME             ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_GPIO_APIS(__prefix_name)                                                                                                                                            \
    __VSF_HAL_TEMPLATE_API(__prefix_name, gpio_capability_t, gpio, capability,       VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr)                                            \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, config_pin,       VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask, uint_fast32_t feature)  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, set_direction,    VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t direction_mask, uint32_t pin_mask)\
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint32_t,          gpio, get_direction,    VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)                         \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, set_input,        VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)                         \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, set_output,       VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)                         \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, switch_direction, VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)                         \
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint32_t,          gpio, read,             VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr)                                            \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, write,            VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t value, uint32_t pin_mask)         \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, set,              VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)                         \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, clear,            VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)                         \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, toggle,           VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)                         \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, output_and_set,   VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)                         \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, output_and_clear, VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)

/*============================ TYPES =========================================*/

typedef struct gpio_capability_t {
    inherit(vsf_peripheral_capability_t)

    // Asynchronous GPIO
    //  they are only guaranteed to be sequential when operating the current port consecutively.
    //
    //  When they are used together with other ports, they are not guaranteed to
    //  be sequential for the operation of the pins of different ports.
    uint8_t is_async;

    // available pin mask
    // may be:
    //  0x000000FF (8  pins),
    //  0x0000FFFF (16  pins),
    //  0xFFFFFFFF (32 pins),
    //  0xFFFFFFFE (32 pins, but pin0 cannot be used as GPIO)
    uint32_t avail_pin_mask;
} gpio_capability_t;

typedef struct vsf_gpio_t vsf_gpio_t;

typedef struct vsf_gpio_op_t {
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP

    VSF_GPIO_APIS(vsf)
} vsf_gpio_op_t;

#if VSF_GPIO_CFG_MULTI_CLASS == ENABLED
struct vsf_gpio_t  {
    const vsf_gpio_op_t * op;
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_gpio_config_pin(vsf_gpio_t *gpio_ptr,
                                uint32_t pin_mask,
                                uint_fast32_t feature);

/*! \brief set the specified pins to corresponding directions
 *!        pseudo code:
 *!        uint32_t temp = VSF_GPIOx.DIR;
 *!        temp &= ~pin_mask;
 *!        VSF_GPIOx.DIR = temp | (direction_bitmap & pin_mask);
 *!
 *! \param gpio_ptr the address of target vsf_gpio_t object (given port)
 *! \param direction_bitmap direction bitmap which will be ANDDed with pin_mask
 *!          direction value is 1 for output
 *!          direction value is 0 for input
 *! \param pin_mask pin mask to mark the target pin within a given port
 *! \return none
 */
extern void vsf_gpio_set_direction( vsf_gpio_t *gpio_ptr,
                                    uint32_t direction_mask,
                                    uint32_t pin_mask);

extern uint32_t vsf_gpio_get_direction(vsf_gpio_t *gpio_ptr,
                                       uint32_t pin_mask);

extern void vsf_gpio_set_input(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);

extern void vsf_gpio_set_output(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);

extern void vsf_gpio_switch_direction(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);

extern uint32_t vsf_gpio_read(vsf_gpio_t *gpio_ptr);

/*! \brief set the specified pins to corresponding value
 *!        pseudo code:
 *!        VSF_GPIOx.OUT &= ~pin_mask;
 *!        VSF_GPIOx.OUT |= (value & pin_mask);
 *!
 *! \param gpio_ptr the address of target vsf_gpio_t object (given port)
 *! \param value value bitmap which will be ANDDed with pin_mask
 *! \param pin_mask pin mask to mark the target pin within a given port
 *! \return none
 */
extern void vsf_gpio_write( vsf_gpio_t *gpio_ptr,
                                uint32_t value,
                                uint32_t pin_mask);

extern void vsf_gpio_set(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);

extern void vsf_gpio_clear(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);

extern void vsf_gpio_output_and_set(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);

extern void vsf_gpio_output_and_clear(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);

extern void vsf_gpio_toggle(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);

extern gpio_capability_t vsf_gpio_capability(vsf_gpio_t *gpio_ptr);

/*============================ MACROS ========================================*/

#if VSF_GPIO_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_gpio_t                           VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_t)
#   define vsf_gpio_capability(__GPIO)            VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_capability)       ((__vsf_gpio_t *)__GPIO)
#   define vsf_gpio_config_pin(__GPIO, ...)       VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_config_pin)       ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_set_direction(__GPIO, ...)    VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set_direction)    ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_get_direction(__GPIO, ...)    VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_get_direction)    ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_set_input(__GPIO, ...)        VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set_input)        ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_set_output(__GPIO, ...)       VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set_output)       ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_switch_direction(__GPIO, ...) VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_switch_direction) ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_read(__GPIO)                  VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_read)             ((__vsf_gpio_t *)__GPIO)
#   define vsf_gpio_write(__GPIO, ...)            VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_write)            ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_set(__GPIO, ...)              VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set)              ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_clear(__GPIO, ...)            VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_clear)            ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_output_and_set(__GPIO, ...)   VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_output_and_set)   ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_output_and_clear(__GPIO, ...) VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_output_and_clear) ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_toggle(__GPIO, ...)           VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_toggle)           ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif  /* __HAL_DRIVER_GPIO_INTERFACE_H__ */
