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

#if VSF_HAL_USE_GPIO == ENABLED && VSF_HAL_USE_74HC165_GPIO == ENABLED

#define __VSF_HAL_74HC164_GPIO_CLASS_IMPLEMENT
#include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/

#ifndef VSF_GPIO_74HC165_CFG_PROTECT_LEVEL
#   define VSF_GPIO_74HC165_CFG_PROTECT_LEVEL   interrupt
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __vsf_gpio_74hc164_protect              vsf_protect(VSF_GPIO_74HC165_CFG_PROTECT_LEVEL)
#define __vsf_gpio_74hc164_unprotect            vsf_unprotect(VSF_GPIO_74HC165_CFG_PROTECT_LEVEL)

/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

void vsf_74hc165_gpio_init(vsf_74hc165_gpio_t *gpio_ptr)
{
    VSF_HAL_ASSERT( (gpio_ptr != NULL) && (gpio_ptr->op != NULL)
                &&  (gpio_ptr->op->load_control != NULL)
                &&  (gpio_ptr->op->clock_control != NULL)
                &&  (gpio_ptr->op->serial_input != NULL)
                &&  (gpio_ptr->cascade_num <= 4) && (gpio_ptr->cascade_num >= 1));

    if (gpio_ptr->op->ce_control != NULL) {
        gpio_ptr->op->ce_control(gpio_ptr->param, 1);
    }
    gpio_ptr->op->load_control(gpio_ptr->param, 0);
    gpio_ptr->op->clock_control(gpio_ptr->param, 0);
}

void vsf_74hc165_gpio_config_pin(vsf_74hc165_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, uint_fast32_t feature)
{
}

void vsf_74hc165_gpio_set_direction(vsf_74hc165_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_pin_mask_t direction_mask)
{
    VSF_HAL_ASSERT((gpio_ptr != NULL) && ((direction_mask & pin_mask) == 0));
}

vsf_gpio_pin_mask_t vsf_74hc165_gpio_get_direction(vsf_74hc165_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    return 0;
}

vsf_gpio_pin_mask_t vsf_74hc165_gpio_read(vsf_74hc165_gpio_t *gpio_ptr)
{
    uint32_t value = 0;

    VSF_HAL_ASSERT(gpio_ptr != NULL);

    uint32_t pins = gpio_ptr->cascade_num << 3;
    vsf_protect_t orig = __vsf_gpio_74hc164_protect();
    gpio_ptr->op->load_control(gpio_ptr->param, 1);

    if (gpio_ptr->op->ce_control != NULL) {
        gpio_ptr->op->ce_control(gpio_ptr->param, 0);
    }

    for (uint_fast8_t i = 0; i < pins; i++) {
        value = (value << 1) | (gpio_ptr->op->serial_input(gpio_ptr->param) ? 1 : 0);
        gpio_ptr->op->clock_control(gpio_ptr->param, 1);
        gpio_ptr->op->clock_control(gpio_ptr->param, 0);
    }

    if (gpio_ptr->op->ce_control != NULL) {
        gpio_ptr->op->ce_control(gpio_ptr->param, 1);
    }

    gpio_ptr->op->load_control(gpio_ptr->param, 0);
    __vsf_gpio_74hc164_unprotect(orig);
    return value;
}

void vsf_74hc165_gpio_write(vsf_74hc165_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_pin_mask_t value)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(false);
}

void vsf_74hc165_gpio_toggle(vsf_74hc165_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(false);
}

vsf_err_t vsf_74hc165_gpio_pin_interrupt_enable(vsf_74hc165_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_arch_prio_t prio)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(false);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_74hc165_gpio_pin_interrupt_config(vsf_74hc165_gpio_t *gpio_ptr, vsf_gpio_pin_irq_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(false);
    return VSF_ERR_NONE;
}

vsf_gpio_capability_t vsf_74hc165_gpio_capability(vsf_74hc165_gpio_t *gpio_ptr)
{
    uint32_t pins = gpio_ptr->cascade_num << 3;
    return (vsf_gpio_capability_t){
        .is_async                   = false,
        .support_config_pin         = false,
        .support_output_and_set     = false,
        .support_output_and_clear   = false,
        .support_interrupt          = false,
        .pin_count                  = pins,
        .pin_mask                   = (1ULL << pins) - 1,
    };
}

#define VSF_HAL_TEMPLATE_IMP_EXTERN_OP          ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_CAPABILITY ENABLED
#define VSF_GPIO_CFG_IMP_PREFIX                 vsf_74hc165
#define VSF_GPIO_CFG_IMP_UPCASE_PREFIX          VSF_74HC165
#include "hal/driver/common/gpio/gpio_template.inc"

#endif      // VSF_HAL_USE_GPIO && VSF_HAL_USE_74HC165_GPIO
