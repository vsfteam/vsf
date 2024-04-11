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

#if VSF_HAL_GPIO_USE_74HC595 == ENABLED

#define __VSF_HAL_GPIO_74HC595_CLASS_IMPLEMENT
#include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/

#ifndef VSF_GPIO_74HC595_CFG_PROTECT_LEVEL
#   define VSF_GPIO_74HC595_CFG_PROTECT_LEVEL   interrupt
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __vsf_gpio_74hc595_protect              vsf_protect(VSF_GPIO_74HC595_CFG_PROTECT_LEVEL)
#define __vsf_gpio_74hc595_unprotect            vsf_unprotect(VSF_GPIO_74HC595_CFG_PROTECT_LEVEL)

/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

void vsf_74hc595_gpio_init(vsf_74hc595_gpio_t *gpio_ptr)
{
    VSF_HAL_ASSERT( (gpio_ptr != NULL) && (gpio_ptr->op != NULL)
                &&  (gpio_ptr->op->oe_control != NULL)
                &&  (gpio_ptr->op->latch_control != NULL)
                &&  (gpio_ptr->op->clock_control != NULL)
                &&  (gpio_ptr->op->serial_output != NULL)
                &&  (gpio_ptr->cascade_num <= 4) && (gpio_ptr->cascade_num >= 1));

    gpio_ptr->output = false;
    gpio_ptr->op->oe_control(gpio_ptr->param, 1);
    gpio_ptr->op->latch_control(gpio_ptr->param, 0);
    gpio_ptr->op->clock_control(gpio_ptr->param, 0);
}

void vsf_74hc595_gpio_config_pin(vsf_74hc595_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, uint_fast32_t feature)
{

}

void vsf_74hc595_gpio_set_direction(vsf_74hc595_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_pin_mask_t direction_mask)
{
    vsf_gpio_pin_mask_t allmask = (1ULL << (8 * gpio_ptr->cascade_num)) - 1;
    VSF_UNUSED_PARAM(allmask);
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT( (((direction_mask & pin_mask) == 0)         && (!gpio_ptr->output || (pin_mask == allmask)))
                ||  (((direction_mask & pin_mask) == pin_mask)  && (gpio_ptr->output || (pin_mask == allmask))));

    if ((direction_mask & pin_mask) == 0) {
        gpio_ptr->op->oe_control(gpio_ptr->param, 1);
        gpio_ptr->output = false;
    } else if (!gpio_ptr->output) {
        gpio_ptr->op->oe_control(gpio_ptr->param, 0);
        gpio_ptr->output = true;
    }
}

vsf_gpio_pin_mask_t vsf_74hc595_gpio_get_direction(vsf_74hc595_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    return gpio_ptr->output ? pin_mask : 0;
}

vsf_gpio_pin_mask_t vsf_74hc595_gpio_read(vsf_74hc595_gpio_t *gpio_ptr)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(false);
    return 0;
}

void vsf_74hc595_gpio_write(vsf_74hc595_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_pin_mask_t value)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);

    uint32_t pins = gpio_ptr->cascade_num << 3;
    vsf_gpio_pin_mask_t msb = 1 << (pins - 1);
    vsf_protect_t orig = __vsf_gpio_74hc595_protect();
    for (uint_fast8_t i = 0; i < pins; i++, value <<= 1) {
        gpio_ptr->op->serial_output(gpio_ptr->param, value & msb ? 1 : 0);
        gpio_ptr->op->clock_control(gpio_ptr->param, 1);
        gpio_ptr->op->clock_control(gpio_ptr->param, 0);
    }

    gpio_ptr->op->latch_control(gpio_ptr->param, 1);
    gpio_ptr->op->latch_control(gpio_ptr->param, 0);
    __vsf_gpio_74hc595_unprotect(orig);
}

void vsf_74hc595_gpio_toggle(vsf_74hc595_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(false);
}

vsf_err_t vsf_74hc595_gpio_pin_interrupt_enable(vsf_74hc595_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_arch_prio_t prio)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(false);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_74hc595_gpio_pin_interrupt_config(vsf_74hc595_gpio_t *gpio_ptr, vsf_gpio_pin_irq_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(false);
    return VSF_ERR_NONE;
}

vsf_gpio_capability_t vsf_74hc595_gpio_capability(vsf_74hc595_gpio_t *gpio_ptr)
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
#define VSF_GPIO_CFG_IMP_PREFIX                 vsf_74hc595
#define VSF_GPIO_CFG_IMP_UPCASE_PREFIX          VSF_74HC595
#include "hal/driver/common/gpio/gpio_template.inc"

#endif      // VSF_HAL_GPIO_USE_74HC595
