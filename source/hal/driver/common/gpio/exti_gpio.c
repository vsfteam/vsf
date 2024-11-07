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

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ IMPLEMENTATION ================================*/

static void __exti_gpio_isr_handler(void *target_ptr, vsf_gpio_t *gpio_ptr,
                                         vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    VSF_HAL_ASSERT(pin_mask != 0);

    vsf_exti_gpio_t *exti_gpio_ptr = (vsf_exti_gpio_t *)target_ptr;
    VSF_HAL_ASSERT(exti_gpio_ptr != NULL);
    VSF_HAL_ASSERT(exti_gpio_ptr->gpio != NULL);
    VSF_HAL_ASSERT(exti_gpio_ptr->exti_irq != NULL);

    while (pin_mask) {
        uint32_t pin = 31 - vsf_clz32(pin_mask);
        VSF_HAL_ASSERT(pin < VSF_HW_GPIO_PIN_COUNT);
        pin_mask &= ~(1 << pin);

        if (exti_gpio_ptr->exti_irq[pin].handler_fn != NULL) {
            exti_gpio_ptr->exti_irq[pin].handler_fn(
                exti_gpio_ptr->exti_irq[pin].target_ptr,
                (vsf_gpio_t *)exti_gpio_ptr->gpio,
                0x01UL << pin);
        }
    }
}

vsf_err_t vsf_exti_gpio_port_config_pins(vsf_exti_gpio_t *exti_gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(exti_gpio_ptr != NULL);
    VSF_HAL_ASSERT(exti_gpio_ptr->gpio != NULL);
    VSF_HAL_ASSERT(pin_mask != 0);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    return vsf_gpio_port_config_pins(exti_gpio_ptr->gpio, pin_mask, cfg_ptr);
}

void vsf_exti_gpio_set_direction(vsf_exti_gpio_t *exti_gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_pin_mask_t direction_mask)
{
    VSF_HAL_ASSERT(exti_gpio_ptr != NULL);
    VSF_HAL_ASSERT(exti_gpio_ptr->gpio != NULL);
    VSF_HAL_ASSERT(pin_mask != 0);

    vsf_gpio_set_direction(exti_gpio_ptr->gpio, pin_mask, direction_mask);
}

vsf_gpio_pin_mask_t vsf_exti_gpio_get_direction(vsf_exti_gpio_t *exti_gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(exti_gpio_ptr != NULL);
    VSF_HAL_ASSERT(exti_gpio_ptr->gpio != NULL);
    VSF_HAL_ASSERT(pin_mask != 0);

    return vsf_gpio_get_direction(exti_gpio_ptr->gpio, pin_mask);
}

vsf_gpio_pin_mask_t vsf_exti_gpio_read(vsf_exti_gpio_t *exti_gpio_ptr)
{
    VSF_HAL_ASSERT(exti_gpio_ptr != NULL);
    VSF_HAL_ASSERT(exti_gpio_ptr->gpio != NULL);

    return vsf_gpio_read(exti_gpio_ptr->gpio);
}

void vsf_exti_gpio_write(vsf_exti_gpio_t *exti_gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_pin_mask_t value)
{
    VSF_HAL_ASSERT(exti_gpio_ptr != NULL);
    VSF_HAL_ASSERT(exti_gpio_ptr->gpio != NULL);
    VSF_HAL_ASSERT(pin_mask != 0);

    vsf_gpio_write(exti_gpio_ptr->gpio, pin_mask, value);
}

void vsf_exti_gpio_toggle(vsf_exti_gpio_t *exti_gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(exti_gpio_ptr != NULL);
    VSF_HAL_ASSERT(exti_gpio_ptr->gpio != NULL);
    VSF_HAL_ASSERT(pin_mask != 0);

    vsf_gpio_toggle(exti_gpio_ptr->gpio, pin_mask);
}

vsf_gpio_capability_t vsf_exti_gpio_capability(vsf_exti_gpio_t *exti_gpio_ptr)
{
    VSF_HAL_ASSERT(exti_gpio_ptr != NULL);
    VSF_HAL_ASSERT(exti_gpio_ptr->gpio != NULL);

    return vsf_gpio_capability(exti_gpio_ptr->gpio);
}

vsf_err_t vsf_exti_gpio_exti_irq_config(vsf_exti_gpio_t *exti_gpio_ptr, vsf_gpio_exti_irq_cfg_t *irq_cfg_ptr)
{
    VSF_HAL_ASSERT(exti_gpio_ptr != NULL);
    VSF_HAL_ASSERT(exti_gpio_ptr->gpio != NULL);
    VSF_HAL_ASSERT(irq_cfg_ptr != NULL);

    return vsf_gpio_exti_irq_config(exti_gpio_ptr->gpio, irq_cfg_ptr);
}

vsf_err_t vsf_exti_gpio_exti_irq_enable(vsf_exti_gpio_t *exti_gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(exti_gpio_ptr != NULL);
    VSF_HAL_ASSERT(exti_gpio_ptr->gpio != NULL);
    VSF_HAL_ASSERT(pin_mask != 0);

    return vsf_gpio_exti_irq_enable(exti_gpio_ptr->gpio, pin_mask);
}

vsf_err_t vsf_exti_gpio_exti_irq_disable(vsf_exti_gpio_t *exti_gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(exti_gpio_ptr != NULL);
    VSF_HAL_ASSERT(exti_gpio_ptr->gpio != NULL);
    VSF_HAL_ASSERT(pin_mask != 0);

    return vsf_gpio_exti_irq_disable(exti_gpio_ptr->gpio, pin_mask);
}

vsf_err_t vsf_exti_gpio_exti_irq_pin_config(vsf_exti_gpio_t *exti_gpio_ptr,
                                            vsf_gpio_pin_mask_t pin_mask,
                                            vsf_gpio_exti_irq_cfg_t *irq_cfg_ptr)
{
    VSF_HAL_ASSERT(exti_gpio_ptr != NULL);
    VSF_HAL_ASSERT(exti_gpio_ptr->gpio != NULL);
    VSF_HAL_ASSERT(exti_gpio_ptr->exti_irq != NULL);
    VSF_HAL_ASSERT(pin_mask != 0);
    VSF_HAL_ASSERT(irq_cfg_ptr != NULL);

    while (pin_mask) {
        uint32_t pin = 31 - vsf_clz32(pin_mask);
        VSF_HAL_ASSERT(pin < VSF_HW_GPIO_PIN_COUNT);
        pin_mask &= ~(1 << pin);

        exti_gpio_ptr->exti_irq[pin].handler_fn = irq_cfg_ptr->handler_fn;
        exti_gpio_ptr->exti_irq[pin].target_ptr = irq_cfg_ptr->target_ptr;
    }

    if (exti_gpio_ptr->prio == vsf_arch_prio_invalid) {
        exti_gpio_ptr->prio = irq_cfg_ptr->prio;

        vsf_gpio_exti_irq_cfg_t exti_irq_cfg = {
            .handler_fn = __exti_gpio_isr_handler,
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

#define VSF_GPIO_CFG_IMP_PREFIX                         vsf_exti
#define VSF_GPIO_CFG_IMP_UPCASE_PREFIX                  VSF_EXTI
#define VSF_GPIO_CFG_IMP_EXTERN_OP                      ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_CAPABILITY         ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_PORTS_CONFIG_PIN   ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_PORTS_CONFIG_PINS  ENABLED
#include "hal/driver/common/gpio/gpio_template.inc"

#endif      // VSF_HAL_USE_GPIO