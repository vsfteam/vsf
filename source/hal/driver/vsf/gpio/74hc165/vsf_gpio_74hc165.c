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

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_GPIO_USE_74HC165 == ENABLED

#include "./vsf_gpio_74hc165.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

void vsf_gpio_74hc165_config_pin(vsf_gpio_74hc165_t *gpio_ptr, uint32_t pin_mask, uint_fast32_t feature)
{
    VSF_HAL_ASSERT( (gpio_ptr != NULL) && (pthis->op != NULL)
                &&  (pthis->op->pl_control ！= NULL)
                &&  (pthis->op->cp_control != NULL)
                &&  (pthis->op->data_input != NULL));

    if (pthis->op->ce_control != NULL) {
        pthis->op->ce_control(pthis->param, 1);
    }
    pthis->op->pl_control(this->param, 1);
    pthis->op->cp_control(this->param, 0);
}

void vsf_gpio_74hc165_set_direction(vsf_gpio_74hc165_t *gpio_ptr, uint32_t direction_mask, uint32_t pin_mask)
{
    VSF_HAL_ASSERT((gpio_ptr != NULL) && (direction_mask & pin_mask == 0));
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
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    // TODO:
    return 0;
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
