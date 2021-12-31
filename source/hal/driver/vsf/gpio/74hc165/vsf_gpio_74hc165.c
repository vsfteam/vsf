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

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_GPIO_USE_74HC165 == ENABLED

#define __VSF_HAL_GPIO_74HC164_CLASS_IMPLEMENT
#include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

void vsf_gpio_74hc165_config_pin(vsf_gpio_74hc165_t *gpio_ptr, uint32_t pin_mask, uint_fast32_t feature)
{
    VSF_HAL_ASSERT( (gpio_ptr != NULL) && (gpio_ptr->op != NULL)
                &&  (gpio_ptr->op->load_control != NULL)
                &&  (gpio_ptr->op->clock_control != NULL)
                &&  (gpio_ptr->op->serial_input != NULL)
                &&  (gpio_ptr->cascade_num <= 4) && (gpio_ptr->cascade_num >= 1));

    if (gpio_ptr->op->ce_control != NULL) {
        gpio_ptr->op->ce_control(gpio_ptr->param, 1);
    }
    gpio_ptr->op->load_control(gpio_ptr->param, 1);
    gpio_ptr->op->clock_control(gpio_ptr->param, 1);
}

void vsf_gpio_74hc165_set_direction(vsf_gpio_74hc165_t *gpio_ptr, uint32_t direction_mask, uint32_t pin_mask)
{
    VSF_HAL_ASSERT((gpio_ptr != NULL) && ((direction_mask & pin_mask) == 0));
}

uint32_t vsf_gpio_74hc165_get_direction(vsf_gpio_74hc165_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    return 0;
}

void vsf_gpio_74hc165_set_input(vsf_gpio_74hc165_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
}

void vsf_gpio_74hc165_set_output(vsf_gpio_74hc165_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(false);
}

void vsf_gpio_74hc165_switch_direction(vsf_gpio_74hc165_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(false);
}

uint32_t vsf_gpio_74hc165_read(vsf_gpio_74hc165_t *gpio_ptr)
{
    uint32_t value = 0;

    VSF_HAL_ASSERT(gpio_ptr != NULL);
    gpio_ptr->op->load_control(gpio_ptr->param, 0);
    gpio_ptr->op->load_control(gpio_ptr->param, 1);

    if (gpio_ptr->op->ce_control != NULL) {
        gpio_ptr->op->ce_control(gpio_ptr->param, 0);
    }

    for (uint_fast8_t i = 0; i < (gpio_ptr->cascade_num << 3); i++) {
        value = (value << 1) | (gpio_ptr->op->serial_input(gpio_ptr->param) ? 1 : 0);
        gpio_ptr->op->clock_control(gpio_ptr->param, 0);
        gpio_ptr->op->clock_control(gpio_ptr->param, 1);
    }

    if (gpio_ptr->op->ce_control != NULL) {
        gpio_ptr->op->ce_control(gpio_ptr->param, 1);
    }
    return value;
}

void vsf_gpio_74hc165_write(vsf_gpio_74hc165_t *gpio_ptr, uint32_t value, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(false);
}

void vsf_gpio_74hc165_set(vsf_gpio_74hc165_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(false);
}

void vsf_gpio_74hc165_clear(vsf_gpio_74hc165_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(false);
}

void vsf_gpio_74hc165_toggle(vsf_gpio_74hc165_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(false);
}

#endif      // VSF_HAL_GPIO_USE_74HC165
