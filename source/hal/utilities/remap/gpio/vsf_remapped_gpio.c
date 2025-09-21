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

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal.h"

#if VSF_HAL_USE_GPIO == ENABLED

#define __VSF_DISTBUS_CLASS_INHERIT__
#define __VSF_REMAPPED_GPIO_CLASS_IMPLEMENT
#include "./vsf_remapped_gpio.h"

#if VSF_GPIO_CFG_MULTI_CLASS == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_REMAPPED_GPIO_CFG_MULTI_CLASS == ENABLED
const vsf_gpio_op_t vsf_remapped_gpio_op = {
#   undef __VSF_HAL_TEMPLATE_API
#   define __VSF_HAL_TEMPLATE_API   VSF_HAL_TEMPLATE_API_OP

    VSF_GPIO_APIS(vsf_remapped_gpio)
};
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_remapped_gpio_port_config_pins(vsf_remapped_gpio_t *gpio, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_cfg_t *cfg)
{
    VSF_HAL_ASSERT((gpio != NULL) && (gpio->target != NULL));
    return vsf_gpio_port_config_pins(gpio->target, pin_mask, cfg);
}

vsf_err_t vsf_remapped_gpio_get_pin_configuration(vsf_remapped_gpio_t *gpio, uint16_t pin_index, vsf_gpio_cfg_t *cfg)
{
    VSF_HAL_ASSERT((gpio != NULL) && (gpio->target != NULL));
    return vsf_gpio_get_pin_configuration(gpio->target, pin_index, cfg);
}

void vsf_remapped_gpio_set_direction(vsf_remapped_gpio_t *gpio, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_pin_mask_t direction_mask)
{
    VSF_HAL_ASSERT((gpio != NULL) && (gpio->target != NULL));
    vsf_gpio_set_direction(gpio->target, pin_mask, direction_mask);
}

vsf_gpio_pin_mask_t vsf_remapped_gpio_get_direction(vsf_remapped_gpio_t *gpio, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT((gpio != NULL) && (gpio->target != NULL));
    return vsf_gpio_get_direction(gpio->target, pin_mask);
}

void vsf_remapped_gpio_switch_direction(vsf_remapped_gpio_t *gpio, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT((gpio != NULL) && (gpio->target != NULL));
    vsf_gpio_switch_direction(gpio->target, pin_mask);
}

void vsf_remapped_gpio_set_input(vsf_remapped_gpio_t *gpio, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT((gpio != NULL) && (gpio->target != NULL));
    vsf_gpio_set_input(gpio->target, pin_mask);
}

void vsf_remapped_gpio_set_output(vsf_remapped_gpio_t *gpio, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT((gpio != NULL) && (gpio->target != NULL));
    vsf_gpio_set_output(gpio->target, pin_mask);
}

vsf_gpio_pin_mask_t vsf_remapped_gpio_read(vsf_remapped_gpio_t *gpio)
{
    VSF_HAL_ASSERT((gpio != NULL) && (gpio->target != NULL));
    return vsf_gpio_read(gpio->target);
}

vsf_gpio_pin_mask_t vsf_remapped_gpio_read_output_register(vsf_remapped_gpio_t *gpio)
{
    VSF_HAL_ASSERT((gpio != NULL) && (gpio->target != NULL));
    return vsf_gpio_read_output_register(gpio->target);
}

void vsf_remapped_gpio_write(vsf_remapped_gpio_t *gpio, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_pin_mask_t value)
{
    VSF_HAL_ASSERT((gpio != NULL) && (gpio->target != NULL));
    vsf_gpio_write(gpio->target, pin_mask, value);
}

void vsf_remapped_gpio_toggle(vsf_remapped_gpio_t *gpio, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT((gpio != NULL) && (gpio->target != NULL));
    vsf_gpio_toggle(gpio->target, pin_mask);
}

void vsf_remapped_gpio_set(vsf_remapped_gpio_t *gpio, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT((gpio != NULL) && (gpio->target != NULL));
    vsf_gpio_set(gpio->target, pin_mask);
}

void vsf_remapped_gpio_clear(vsf_remapped_gpio_t *gpio, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT((gpio != NULL) && (gpio->target != NULL));
    vsf_gpio_clear(gpio->target, pin_mask);
}

void vsf_remapped_gpio_output_and_set(vsf_remapped_gpio_t *gpio, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT((gpio != NULL) && (gpio->target != NULL));
    vsf_gpio_output_and_set(gpio->target, pin_mask);
}

void vsf_remapped_gpio_output_and_clear(vsf_remapped_gpio_t *gpio, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT((gpio != NULL) && (gpio->target != NULL));
    vsf_gpio_output_and_clear(gpio->target, pin_mask);
}

vsf_gpio_capability_t vsf_remapped_gpio_capability(vsf_remapped_gpio_t *gpio)
{
    VSF_HAL_ASSERT((gpio != NULL) && (gpio->target != NULL));
    return vsf_gpio_capability(gpio->target);
}

vsf_err_t vsf_remapped_gpio_exti_irq_config(vsf_remapped_gpio_t *gpio, vsf_gpio_exti_irq_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((gpio != NULL) && (gpio->target != NULL));
    return vsf_gpio_exti_irq_config(gpio->target, cfg_ptr);
}

vsf_err_t vsf_remapped_gpio_exti_irq_get_configuration(vsf_remapped_gpio_t *gpio, vsf_gpio_exti_irq_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((gpio != NULL) && (gpio->target != NULL));
    return vsf_gpio_exti_irq_get_configuration(gpio->target, cfg_ptr);
}

vsf_err_t vsf_remapped_gpio_exti_irq_enable(vsf_remapped_gpio_t *gpio, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT((gpio != NULL) && (gpio->target != NULL));
    return vsf_gpio_exti_irq_enable(gpio->target, pin_mask);
}

vsf_err_t vsf_remapped_gpio_exti_irq_disable(vsf_remapped_gpio_t *gpio, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT((gpio != NULL) && (gpio->target != NULL));
    return vsf_gpio_exti_irq_disable(gpio->target, pin_mask);
}

#endif
#endif
