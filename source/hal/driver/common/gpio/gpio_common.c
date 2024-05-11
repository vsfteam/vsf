/*****************************************************************************
 *   Cop->right(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a cop-> of the License at                                  *
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

#define VSF_GPIO_CFG_FUNCTION_RENAME DISABLED

#include "hal/driver/driver.h"

#if VSF_HAL_USE_GPIO == ENABLED && VSF_GPIO_CFG_MULTI_CLASS == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void vsf_gpio_config_pin(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_mode_t feature)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op->config_pin != NULL);

    gpio_ptr->op->config_pin(gpio_ptr, pin_mask, feature);
}

void vsf_gpio_set_direction(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_pin_mask_t direction_mask)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op->set_direction != NULL);

    gpio_ptr->op->set_direction(gpio_ptr, pin_mask, direction_mask);
}

vsf_gpio_pin_mask_t vsf_gpio_get_direction(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op->get_direction != NULL);

    return gpio_ptr->op->get_direction(gpio_ptr, pin_mask);
}

void vsf_gpio_set_input(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op->set_input != NULL);

    gpio_ptr->op->set_input(gpio_ptr, pin_mask);
}

void vsf_gpio_set_output(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op->set_output != NULL);

    gpio_ptr->op->set_output(gpio_ptr, pin_mask);
}

void vsf_gpio_switch_direction(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op->switch_direction != NULL);

    gpio_ptr->op->switch_direction(gpio_ptr, pin_mask);
}

vsf_gpio_pin_mask_t vsf_gpio_read(vsf_gpio_t *gpio_ptr)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op->read != NULL);

    return gpio_ptr->op->read(gpio_ptr);
}

void vsf_gpio_write(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_pin_mask_t value)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op->write != NULL);

    gpio_ptr->op->write(gpio_ptr, pin_mask, value);
}

void vsf_gpio_set(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op->set != NULL);

    gpio_ptr->op->set(gpio_ptr, pin_mask);
}

void vsf_gpio_clear(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op->clear != NULL);

    gpio_ptr->op->clear(gpio_ptr, pin_mask);
}

void vsf_gpio_toggle(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op->toggle != NULL);

    gpio_ptr->op->toggle(gpio_ptr, pin_mask);
}

void vsf_gpio_output_and_set(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op->toggle != NULL);

    gpio_ptr->op->output_and_set(gpio_ptr, pin_mask);
}

void vsf_gpio_output_and_clear(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op->toggle != NULL);

    gpio_ptr->op->output_and_clear(gpio_ptr, pin_mask);
}

vsf_gpio_capability_t vsf_gpio_capability(vsf_gpio_t *gpio_ptr)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op->capability != NULL);

    return gpio_ptr->op->capability(gpio_ptr);
}

vsf_err_t vsf_gpio_exit_config(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_irq_cfg_t *cfg)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op->exit_config != NULL);

    return gpio_ptr->op->exit_config(gpio_ptr, cfg);
}

vsf_err_t vsf_gpio_exti_irq_enable(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_arch_prio_t prio)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op->exti_irq_enable != NULL);

    return gpio_ptr->op->exti_irq_enable(gpio_ptr, pin_mask, prio);
}

vsf_err_t vsf_gpio_exti_irq_disable(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op != NULL);
    VSF_HAL_ASSERT(gpio_ptr->op->exti_irq_disable != NULL);

    return gpio_ptr->op->exti_irq_disable(gpio_ptr, pin_mask);
}

#endif
