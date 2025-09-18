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

#define __VSF_HAL_USE_GPIO_EXTI_CLASS_IMPLEMENT
#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_GPIO == ENABLED

#include "hal/driver/driver.h"
#include "exti_gpio.h"

/*============================ MACROS ========================================*/

#ifdef VSF_GPIO_EXTI_GPIO_EXTI_CFG_CALL_PREFIX
#   undef VSF_GPIO_EXTI_CFG_IMP_PREFIX
#   define VSF_GPIO_EXTI_CFG_IMP_PREFIX         VSF_GPIO_EXTI_CFG_CALL_PREFIX
#endif

#if VSF_GPIO_CFG_PIN_COUNT > 32
#   error "TODO: Support for more than 32bit cases"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ IMPLEMENTATION ================================*/

static void __gpio_irq_distributor_handler(void *target_ptr, vsf_gpio_t *gpio_ptr,
                                           vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(pin_mask != 0);

    vsf_gpio_irq_distributor_t *exti_gpio_ptr = (vsf_gpio_irq_distributor_t *)target_ptr;
    VSF_HAL_ASSERT(exti_gpio_ptr != NULL);
    VSF_HAL_ASSERT(exti_gpio_ptr->gpio != NULL);
    VSF_HAL_ASSERT(exti_gpio_ptr->exti_irq != NULL);

    while (pin_mask) {
        uint32_t pin = (VSF_GPIO_CFG_PIN_COUNT - 1) - vsf_clz32(pin_mask);
        pin_mask &= ~(1 << pin);

        if (exti_gpio_ptr->exti_irq[pin].handler_fn != NULL) {
            exti_gpio_ptr->exti_irq[pin].handler_fn(
                exti_gpio_ptr->exti_irq[pin].target_ptr,
                (vsf_gpio_t *)exti_gpio_ptr->gpio,
                0x01UL << pin);
        }
    }
}

vsf_err_t vsf_gpio_irq_distributor_pin_config(vsf_gpio_irq_distributor_t *exti_gpio_ptr,
                                              vsf_gpio_pin_mask_t pin_mask,
                                              vsf_gpio_exti_irq_cfg_t *irq_cfg_ptr)
{
    VSF_HAL_ASSERT(exti_gpio_ptr != NULL);
    VSF_HAL_ASSERT(exti_gpio_ptr->gpio != NULL);
    VSF_HAL_ASSERT(exti_gpio_ptr->exti_irq != NULL);
    VSF_HAL_ASSERT(pin_mask != 0);
    VSF_HAL_ASSERT(irq_cfg_ptr != NULL);

    while (pin_mask) {
        uint32_t pin = (VSF_GPIO_CFG_PIN_COUNT - 1) - vsf_clz32(pin_mask);
        pin_mask &= ~(1 << pin);

        exti_gpio_ptr->exti_irq[pin].handler_fn = irq_cfg_ptr->handler_fn;
        exti_gpio_ptr->exti_irq[pin].target_ptr = irq_cfg_ptr->target_ptr;
    }

    if (exti_gpio_ptr->prio == vsf_arch_prio_invalid) {
        exti_gpio_ptr->prio = irq_cfg_ptr->prio;

        vsf_gpio_exti_irq_cfg_t exti_irq_cfg = {
            .handler_fn = __gpio_irq_distributor_handler,
            .target_ptr = exti_gpio_ptr,
            .prio = exti_gpio_ptr->prio,
        };
        return vsf_gpio_exti_irq_config(exti_gpio_ptr->gpio, &exti_irq_cfg);
    } else if (exti_gpio_ptr->prio == irq_cfg_ptr->prio) {
        return VSF_ERR_NONE;
    } else {
        return VSF_ERR_INVALID_PARAMETER;
    }
}

/*============================ LOCAL VARIABLES ===============================*/

#endif      // VSF_HAL_USE_GPIO