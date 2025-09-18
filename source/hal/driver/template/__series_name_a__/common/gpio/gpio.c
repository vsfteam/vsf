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

#include "../../__device.h"

// HW
// for vendor headers
#include "hal/driver/vendor_driver.h"
// HW end

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
    vsf_gpio_exti_irq_cfg_t     irq_cfg;
} VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t);
// HW end

/*============================ IMPLEMENTATION ================================*/

vsf_err_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_port_config_pins)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask,
    vsf_gpio_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_set_direction)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask,
    vsf_gpio_pin_mask_t direction_mask
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);
}

vsf_gpio_pin_mask_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_get_direction)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    return 0;
}

vsf_gpio_pin_mask_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_read)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    return 0;
}

void VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_write)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask,
    vsf_gpio_pin_mask_t value
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);
}

void VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_toggle)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);
}

vsf_err_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_exti_irq_enable)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_exti_irq_disable)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_exti_irq_config)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_exti_irq_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    return VSF_ERR_NONE;
}

static uint32_t VSF_MCONNECT(__, VSF_GPIO_CFG_IMP_PREFIX, _gpio_get_irq_mask)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr
) {
    // implement this function in the device file
    VSF_HAL_ASSERT(0);
    return 0;
}

static void VSF_MCONNECT(__, VSF_GPIO_CFG_IMP_PREFIX, _gpio_clear_irq_flag)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    uint32_t irq_pin_mask
) {
    // implement this function in the device file
    VSF_HAL_ASSERT(0);
}

static void VSF_MCONNECT(__, VSF_GPIO_CFG_IMP_PREFIX, _gpio_irqhandler)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    uint32_t irq_pin_mask = VSF_MCONNECT(__, VSF_GPIO_CFG_IMP_PREFIX, _gpio_get_irq_mask)(gpio_ptr);
    VSF_MCONNECT(__, VSF_GPIO_CFG_IMP_PREFIX, _gpio_clear_irq_flag)(gpio_ptr, irq_pin_mask);

    VSF_HAL_ASSERT(NULL != gpio_ptr->irq_cfg.handler_fn);
    gpio_ptr->irq_cfg.handler_fn(
        gpio_ptr->irq_cfg.target_ptr,
        (vsf_gpio_t *)gpio_ptr,
        irq_pin_mask
    );
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

void VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_output_and_set)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);
}

void VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_output_and_clear)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);
}

vsf_gpio_capability_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_capability)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr
) {
    return (vsf_gpio_capability_t){
        .is_async                   = false,
        .support_output_and_set     = true,
        .support_output_and_clear   = true,
        .support_interrupt          = 1,
        .pin_count                  = 16,
        .pin_mask                   = 0xFFFF,
    };
}

/*============================ INCLUDES ======================================*/

#define VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_SET         ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_CLEAR       ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_CAPABILITY             ENABLED

#define VSF_GPIO_CFG_IMP_LV0(__IDX, __HAL_OP)                                   \
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t)                              \
        VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio, __IDX) = {                 \
        __HAL_OP                                                                \
    };                                                                          \
    void VSF_MCONNECT(VSF_GPIO_CFG_IMP_UPCASE_PREFIX, _GPIO, __IDX,  _IRQHandler)(void)\
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_GPIO_CFG_IMP_PREFIX, _gpio_irqhandler)(            \
            &VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio, __IDX)                \
        );                                                                      \
        vsf_hal_irq_leave(ctx);                                                 \
    }

#include "hal/driver/common/gpio/gpio_template.inc"

#endif      // VSF_HAL_USE_GPIO
