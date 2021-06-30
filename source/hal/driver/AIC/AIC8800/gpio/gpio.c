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
 *  limitations under the License.                                           *x
 *                                                                           *
 ****************************************************************************/



/*============================ INCLUDES ======================================*/

#include "vsf.h"
#if VSF_HAL_USE_GPIO == ENABLED
#include "./gpio.h"
/*============================ MACROS ========================================*/

#define GPIO_OUTPUT                         (1)
#define GPIO_INPUT                          (0)
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

 typedef void (*target_fn_t)(int gpio_idx, void *(target));
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/

INLINE static void __pin_mask_resolve(void *target, target_fn_t function, uint32_t pin_mask)
{
    int i;
    for (i = 0; i < 16; i++) {
        if (0 != (pin_mask & (1 << i))) {
            function(i, target);
        }
    }
}

static void __vsf_gpio_config_pin(int gpio_idx, void *target)
{
    uint32_t sel_val, local_val;
    vsf_gpio_t *gpio_ptr = (vsf_gpio_t *)target;
    //todo:
//    uint32_t feature = *(uint32_t *)gpio_ptr->current_param;
    if (GPIO_PORT_A == gpio_ptr->port) {
        if ((gpio_idx < 2) || ((gpio_idx > 7) && (gpio_idx < 10))) {
            sel_val = 1;
        } else {
            sel_val = 0;
        }
    } else if (GPIO_PORT_B == gpio_ptr->port) {
        sel_val = 2;
    } else {
        VSF_HAL_ASSERT(false);
    }
    local_val = gpio_ptr->reg.iomux->GPCFG[gpio_idx];
    gpio_ptr->reg.iomux->GPCFG[gpio_idx] = local_val | ((sel_val << IOMUX_GPIO_CONFIG_SEL_LSB) & IOMUX_GPIO_CONFIG_SEL_MASK);
}

static void __vsf_gpio_set_direction(int gpio_idx, void *target)
{
    vsf_gpio_t *gpio_ptr = (vsf_gpio_t *)target;
    uint_fast32_t direction_mask = *(uint32_t *)gpio_ptr->current_param;
    if (0 != (direction_mask & (GPIO_OUTPUT << gpio_idx))) {
        gpio_ptr->reg.gpio->DR |= GPIO_OUTPUT << gpio_idx;
    } else {
        gpio_ptr->reg.gpio->DR &= ~(GPIO_OUTPUT << gpio_idx);
    }
}

static void __vsf_gpio_get_direction(int gpio_idx, void *target)
{
    vsf_gpio_t *gpio_ptr = (vsf_gpio_t *)target;
    uint_fast32_t ret = *(uint32_t *)gpio_ptr->current_param;
    if (0 != gpio_ptr->reg.gpio->DR & (1 << gpio_idx)) {
        ret |= 1 << gpio_idx;
    }
}

static void __vsf_gpio_write(int gpio_idx, void *target)
{
    vsf_gpio_t *gpio_ptr = (vsf_gpio_t *)target;
    uint_fast32_t value = *(uint32_t *)gpio_ptr->current_param;
    if (0 != (value & (1 << gpio_idx))) {
        gpio_ptr->reg.gpio->VR |= 1 << gpio_idx;
    } else {
        gpio_ptr->reg.gpio->VR &= ~(1 << gpio_idx);
    }
}

void vsf_gpio_config_pin(vsf_gpio_t *gpio_ptr, uint32_t pin_mask, uint_fast32_t feature)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    gpio_ptr->current_param = (void *)&feature;
    __pin_mask_resolve((void *)gpio_ptr, __vsf_gpio_config_pin, pin_mask);
    gpio_ptr->current_param = NULL;
    gpio_ptr->reg.gpio->MR |=  pin_mask;
}

void vsf_gpio_set_direction(vsf_gpio_t *gpio_ptr, uint_fast32_t direction_mask, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    gpio_ptr->current_param = (void *)&direction_mask;
    __pin_mask_resolve((void *)gpio_ptr, __vsf_gpio_set_direction, pin_mask);
    gpio_ptr->current_param = NULL;
}

uint_fast32_t vsf_gpio_get_direction(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    uint_fast32_t ret = 0ul;
    gpio_ptr->current_param = (void *)&ret;
    __pin_mask_resolve((void *)gpio_ptr, __vsf_gpio_get_direction, pin_mask);
    gpio_ptr->current_param = NULL;
}

void vsf_gpio_set_input(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    vsf_gpio_set_direction(gpio_ptr, GPIO_PIN_ALL_INPUT, pin_mask);
}

void vsf_gpio_set_output(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    vsf_gpio_set_direction(gpio_ptr, pin_mask, pin_mask);
}

void vsf_gpio_switch_direction(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    uint_fast32_t ret = ~vsf_gpio_get_direction(gpio_ptr, pin_mask);
    vsf_gpio_set_direction(gpio_ptr, ret, pin_mask);
}

uint_fast32_t vsf_gpio_read(vsf_gpio_t *gpio_ptr)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    return (AIC_GPIO->VR & GPIO_PIN_ALL);
}

void vsf_gpio_write( vsf_gpio_t *gpio_ptr, uint_fast32_t value, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    gpio_ptr->current_param = (void *)&value;
    __pin_mask_resolve((void *)gpio_ptr, __vsf_gpio_write, pin_mask);
    gpio_ptr->current_param = NULL;
}

void vsf_gpio_set(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    vsf_gpio_write(gpio_ptr, 0xffff, pin_mask);
}

void vsf_gpio_clear(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    vsf_gpio_write(gpio_ptr, 0ul, pin_mask);
}

void vsf_gpio_toggle(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    uint_fast32_t ret = vsf_gpio_read(gpio_ptr);
    vsf_gpio_write(gpio_ptr, ~(ret & pin_mask), pin_mask);
}

#endif /* EOF */
