/*****************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
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

#include "sthal.h"
#include "hal/vsf_hal.h"

#if (VSF_HAL_USE_GPIO == ENABLED) && defined(HAL_GPIO_MODULE_ENABLED)

/*============================ MACROS ========================================*/

#    ifdef VSF_STHAL_CFG_CALL_GPIO_PREFIX
#        undef VSF_GPIO_CFG_PREFIX
#        define VSF_GPIO_CFG_PREFIX VSF_STHAL_CFG_CALL_GPIO_PREFIX
#    endif

#    ifndef VSF_STHAL_CFG_GPIO_EXTI_RPIO
#        define VSF_STHAL_CFG_GPIO_EXTI_RPIO VSF_ARCH_PRIO_1
#    endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

VSF_CAL_WEAK(HAL_GPIO_EXTI_Callback)
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    VSF_UNUSED_PARAM(GPIO_Pin);
}

static void __sthal_gpio_isr_handler(void *target_ptr,
                                     vsf_gpio_t *gpio_ptr,
                                     vsf_gpio_pin_mask_t pin_mask)
{
    HAL_GPIO_EXTI_Callback(pin_mask);
}

void HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init)
{
    VSF_STHAL_ASSERT(GPIOx != NULL);
    vsf_gpio_t *gpio = (vsf_gpio_t *)GPIOx;

    vsf_gpio_cfg_t cfg = {
        .mode = GPIO_Init->Mode | GPIO_Init->Pull | GPIO_Init->Speed,
        .alternate_function =  GPIO_Init->Alternate,
    };
    vsf_gpio_port_config_pins(gpio, GPIO_Init->Pin, &cfg);
    if ((cfg.mode & VSF_GPIO_MODE_MASK) == VSF_GPIO_EXTI) {
        vsf_gpio_exti_irq_cfg_t exti_irq_cfg = {
            .handler_fn = __sthal_gpio_isr_handler,
            .target_ptr = NULL,
            .prio = VSF_STHAL_CFG_GPIO_EXTI_RPIO,
        };
        vsf_gpio_exti_irq_config(gpio, &exti_irq_cfg);
        vsf_gpio_exti_irq_enable(gpio, GPIO_Init->Pin);
    }
}

void HAL_GPIO_DeInit(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin)
{
    VSF_STHAL_ASSERT(GPIOx != NULL);
    vsf_gpio_t *gpio = (vsf_gpio_t *)GPIOx;

    vsf_gpio_cfg_t cfg = {
        .mode = VSF_GPIO_INPUT | VSF_GPIO_NO_PULL_UP_DOWN
    };
    vsf_gpio_port_config_pins(gpio, GPIO_Pin, &cfg);
    vsf_gpio_exti_irq_disable(gpio, GPIO_Pin);
}

GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    VSF_STHAL_ASSERT(GPIOx != NULL);
    vsf_gpio_t *gpio = (vsf_gpio_t *)GPIOx;

    return vsf_gpio_read(gpio) & GPIO_Pin;
}

void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin,
                       GPIO_PinState PinState)
{
    VSF_STHAL_ASSERT(GPIOx != NULL);
    vsf_gpio_t *gpio = (vsf_gpio_t *)GPIOx;

    vsf_gpio_write(gpio, GPIO_Pin, (PinState == GPIO_PIN_RESET) ? 0 : GPIO_Pin);
}

void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    VSF_STHAL_ASSERT(GPIOx != NULL);
    vsf_gpio_t *gpio = (vsf_gpio_t *)GPIOx;

    vsf_gpio_toggle(gpio, GPIO_Pin);
}

HAL_StatusTypeDef HAL_GPIO_LockPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    // This feature is not yet supported in vsf hal
    return HAL_ERROR;
}

void HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin)
{
    // Keep it for compilation
}

#endif
