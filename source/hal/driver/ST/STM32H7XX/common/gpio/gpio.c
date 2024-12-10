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

#if VSF_HAL_USE_GPIO == ENABLED

#include "hal/vsf_hal.h"

#include "../vendor/cmsis_device_h7/Include/stm32h7xx.h"

/*============================ MACROS ========================================*/

/*\note VSF_HW_GPIO_CFG_MULTI_CLASS is only for drivers for specified device(hw drivers).
 */

// HW
#ifndef VSF_HW_GPIO_CFG_MULTI_CLASS
#   define VSF_HW_GPIO_CFG_MULTI_CLASS          VSF_GPIO_CFG_MULTI_CLASS
#endif
// HW end

// HW
#define VSF_GPIO_CFG_IMP_PREFIX                 vsf_hw
#define VSF_GPIO_CFG_IMP_UPCASE_PREFIX          VSF_HW
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// HW
typedef struct VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) {
#if VSF_HW_GPIO_CFG_MULTI_CLASS == ENABLED
    vsf_gpio_t                  vsf_gpio;
#endif
    GPIO_TypeDef                *reg;
} VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t);
// HW end

/*============================ IMPLEMENTATION ================================*/

GPIO_TypeDef * __vsf_hw_gpio_get_regbase(vsf_hw_gpio_t *gpio_ptr)
{
    return gpio_ptr->reg;
}

vsf_err_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_port_config_pins)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask,
    vsf_gpio_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    GPIO_TypeDef *reg = gpio_ptr->reg;
    uint32_t offset_len2;
    vsf_gpio_mode_t mode = cfg_ptr->mode;
    uint32_t otype = (mode >> 2) & 1;
    uint32_t pupd = (mode >> 3) & 3;
    uint32_t ospeed = (mode >> 5) & 3;
    uint32_t current_pin_mask;

    mode = (mode >> 0) & 3;
    for (int i = 0; i < VSF_HW_GPIO_PIN_COUNT; i++) {
        current_pin_mask = 1 << i;
        if (pin_mask & current_pin_mask) {
            offset_len2 = i << 1;

            vsf_atom32_op(&reg->MODER, (_ & ~(3 << offset_len2)) | (mode << offset_len2));
            vsf_atom32_op(&reg->OTYPER, (_ & ~(1 << i)) | (otype << i));
            vsf_atom32_op(&reg->OSPEEDR, (_ & ~(3 << offset_len2)) | (ospeed << offset_len2));
            vsf_atom32_op(&reg->PUPDR, (_ & ~(3 << offset_len2)) | (pupd << offset_len2));
        }
    }
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_set_direction)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask,
    vsf_gpio_pin_mask_t direction_mask
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    GPIO_TypeDef *reg = gpio_ptr->reg;
    uint32_t mode, current_pin_mask, offset_len2;

    for (int i = 0; i < VSF_HW_GPIO_PIN_COUNT; i++) {
        current_pin_mask = 1 << i;
        if (pin_mask & current_pin_mask) {
            mode = (direction_mask >> i) & 1;
            offset_len2 = i << 1;

            vsf_atom32_op(&reg->MODER, (_ & ~(3 << offset_len2)) | (mode << offset_len2));
        }
    }
}

vsf_gpio_pin_mask_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_get_direction)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    uint32_t mode = gpio_ptr->reg->MODER, current_pin_mask;
    vsf_gpio_pin_mask_t direction = 0;

    for (int i = 0; i < VSF_HW_GPIO_PIN_COUNT; i++, mode >>= 2) {
        current_pin_mask = 1 << i;
        if (pin_mask & current_pin_mask) {
            if ((mode & 3) == 1) {
                direction |= current_pin_mask;
            }
        }
    }
    return direction;
}

vsf_gpio_pin_mask_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_read)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    return gpio_ptr->reg->IDR;
}

void VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_write)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask,
    vsf_gpio_pin_mask_t value
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    value &= pin_mask;
    vsf_atom32_op(&gpio_ptr->reg->ODR, (_ & ~pin_mask) | value);
}

void VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_toggle)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    vsf_atom32_op(&gpio_ptr->reg->ODR, _ ^ pin_mask);
}

void VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_set)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(0 != pin_mask);

    gpio_ptr->reg->BSRR |= pin_mask;
}

void VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_clear)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(0 != pin_mask);

    gpio_ptr->reg->BSRR |= pin_mask << 16;
}

vsf_err_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_exti_irq_enable)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(false);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_exti_irq_disable)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(false);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_exti_irq_config)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_exti_irq_cfg_t *irq_cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(NULL != irq_cfg_ptr);
    VSF_HAL_ASSERT(false);
    return VSF_ERR_NOT_SUPPORT;
}

/*\note Implementation of APIs below is optional, because there is default implementation in gpio_template.inc.
 *      VSF_GPIO_CFG_REIMPLEMENT_API_XXXX can be defined to ENABLED to re-write the default implementation for better performance.
 *
 *      The list of APIs and configuration:
 *      VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_SET for gpio_output_and_set, *** glitchless ***
 *          If VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_SET is 0, default implementation will assert(false).
 *          If VSF_GPIO_CFG_CHANGE_DIR_FIRST is ENABLED, default implementation will call gpio_set_output and then gpio_clear.
 *          If VSF_GPIO_CFG_CHANGE_DIR_FIRST is DISABLED, default implementation will call gpio_clear and then gpio_set_output.
 *      VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_CLEAR for gpio_output_and_clear, *** glitchless ***
 *          If VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_CLEAR is 0, default implementation will assert(false).
 *          If VSF_GPIO_CFG_CHANGE_DIR_FIRST is ENABLED, default implementation will call gpio_set_output and then gpio_clear.
 *          If VSF_GPIO_CFG_CHANGE_DIR_FIRST is DISABLED, default implementation will call gpio_clear and then gpio_set_output.
 *      VSF_GPIO_CFG_REIMPLEMENT_API_SET_INPUT for gpio_set_input.
 *          Default implementation will call vsf_real_gpio_set_direction.
 *      VSF_GPIO_CFG_REIMPLEMENT_API_SET_OUTPUT for gpio_set_output.
 *          Default implementation will call vsf_real_gpio_set_direction.
 *      VSF_GPIO_CFG_REIMPLEMENT_API_SWITCH_DIRECTION for gpio_switch_direction.
 *          Default implementation will call gpio_get_direction then gpio_switch_direction.
 *      VSF_GPIO_CFG_REIMPLEMENT_API_SET for gpio_set.
 *          Default implementation will call gpio_write.
 *      VSF_GPIO_CFG_REIMPLEMENT_API_CLEAR for gpio_clear.
 *          Default implementation will call gpio_write.
 *      VSF_GPIO_CFG_REIMPLEMENT_API_CAPABILITY for gpio_capability.
 *          Default implementation will use macros below to initialize capability structure:
 *              VSF_GPIO_CFG_CAPABILITY_IS_ASYNC
 *              VSF_GPIO_CFG_CAPABILITY_SUPPORT_CONFIG_PIN
 *              VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_SET
 *              VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_CLEAR
 *              VSF_GPIO_CFG_CAPABILITY_SUPPORT_INTERRUPT
 *              VSF_GPIO_CFG_CAPABILITY_PIN_COUNT
 *              VSF_GPIO_CFG_CAPABILITY_PIN_MASK
 */

vsf_gpio_capability_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_capability)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr
) {
    return (vsf_gpio_capability_t){
        .is_async                   = false,
        .support_output_and_set     = true,
        .support_output_and_clear   = true,
        .support_interrupt          = false,
        .pin_count                  = 16,
        .pin_mask                   = 0xFFFF,
    };
}

/*============================ INCLUDES ======================================*/

#define VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_SET      1
#define VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_CLEAR    1
#define VSF_GPIO_CFG_CHANGE_DIR_FIRST                       DISABLED

#define VSF_GPIO_CFG_REIMPLEMENT_API_SET                    ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_CLEAR                  ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_CAPABILITY             ENABLED

#define VSF_GPIO_CFG_IMP_LV0(__IDX, __HAL_OP)                                   \
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t)                              \
        VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio, __IDX) = {                 \
        __HAL_OP                                                                \
        .reg                        = VSF_HW_GPIO_PORT ## __IDX ## _REG_BASE,   \
    };

#include "hal/driver/common/gpio/gpio_template.inc"

#endif      // VSF_HAL_USE_GPIO
