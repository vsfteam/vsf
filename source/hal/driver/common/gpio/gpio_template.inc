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

#include "hal/driver/common/template/vsf_template_hal_driver.h"

#if VSF_HAL_USE_GPIO == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_GPIO_CFG_RETAIN_DEFINED_PREFIX
#   define VSF_GPIO_CFG_RETAIN_DEFINED_PREFIX                       DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_SET_INPUT
#   define VSF_GPIO_CFG_REIMPLEMENT_API_SET_INPUT                   DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_SET_INPUT
#   define VSF_GPIO_CFG_REIMPLEMENT_API_SET_INPUT                   DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_SET_OUTPUT
#   define VSF_GPIO_CFG_REIMPLEMENT_API_SET_OUTPUT                  DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_SWITCH_DIRECTION
#   define VSF_GPIO_CFG_REIMPLEMENT_API_SWITCH_DIRECTION            DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_SET
#   define VSF_GPIO_CFG_REIMPLEMENT_API_SET                         DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_CLEAR
#   define VSF_GPIO_CFG_REIMPLEMENT_API_CLEAR                       DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_SET
#   define VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_SET              DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_CLEAR
#   define VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_CLEAR            DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_GET_PIN_CONFIGURATION
#   define VSF_GPIO_CFG_REIMPLEMENT_API_GET_PIN_CONFIGURATION       DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_READ_OUTPUT_REGISTER
#   define VSF_GPIO_CFG_REIMPLEMENT_API_READ_OUTPUT_REGISTER        DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_PORTS_CONFIG_PIN
#   define VSF_GPIO_CFG_REIMPLEMENT_API_PORTS_CONFIG_PIN            DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_PORTS_CONFIG_PINS
#   define VSF_GPIO_CFG_REIMPLEMENT_API_PORTS_CONFIG_PINS           DISABLED
#endif

#ifndef VSF_GPIO_CFG_CHANGE_DIR_FIRST
#   define VSF_GPIO_CFG_CHANGE_DIR_FIRST                            ENABLED
#endif

#ifndef VSF_GPIO_CFG_CAPABILITY_SUPPORT_CONFIG_PIN
#    define VSF_GPIO_CFG_CAPABILITY_SUPPORT_CONFIG_PIN              1
#endif

#ifndef VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_SET
#    define VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_SET          0
#endif

#ifndef VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_CLEAR
#    define VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_CLEAR        0
#endif

#ifndef VSF_GPIO_CFG_CAPABILITY_SUPPORT_INTERRUPT
#    define VSF_GPIO_CFG_CAPABILITY_SUPPORT_INTERRUPT               1
#endif

#ifndef VSF_GPIO_CFG_CAPABILITY_CAN_READ_IN_GPIO_OUTPUT_MODE
#    define VSF_GPIO_CFG_CAPABILITY_CAN_READ_IN_GPIO_OUTPUT_MODE    0
#endif

#ifndef VSF_GPIO_CFG_CAPABILITY_CAN_READ_IN_ALTERNATE_MODE
#    define VSF_GPIO_CFG_CAPABILITY_CAN_READ_IN_ALTERNATE_MODE      0
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_GPIO_CFG_REIMPLEMENT_API_CAPABILITY                 DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_EXTI_IRQ_GET_CONFIGURATION
#   define VSF_GPIO_CFG_REIMPLEMENT_API_EXTI_IRQ_GET_CONFIGURATION  DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_EXTI_IRQ_CLEAR
#   define VSF_GPIO_CFG_REIMPLEMENT_API_EXTI_IRQ_CLEAR              DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_CTRL
#   define VSF_GPIO_CFG_REIMPLEMENT_API_CTRL                        DISABLED
#endif

#ifdef VSF_GPIO_CFG_IMP_REMAP_PREFIX
#   undef VSF_GPIO_CFG_REIMPLEMENT_API_SET_INPUT
#   undef VSF_GPIO_CFG_REIMPLEMENT_API_SET_OUTPUT
#   undef VSF_GPIO_CFG_REIMPLEMENT_API_SWITCH_DIRECTION
#   undef VSF_GPIO_CFG_REIMPLEMENT_API_SET
#   undef VSF_GPIO_CFG_REIMPLEMENT_API_CLEAR
#   undef VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_SET
#   undef VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_CLEAR
#   undef VSF_GPIO_CFG_REIMPLEMENT_API_GET_PIN_CONFIGURATION
#   undef VSF_GPIO_CFG_REIMPLEMENT_API_READ_OUTPUT_REGISTER
#   undef VSF_GPIO_CFG_REIMPLEMENT_API_CAPABILITY
#   undef VSF_GPIO_CFG_REIMPLEMENT_API_EXTI_IRQ_GET_CONFIGURATION
#   undef VSF_GPIO_CFG_REIMPLEMENT_API_EXTI_IRQ_CLEAR
#   undef VSF_GPIO_CFG_REIMPLEMENT_API_CTRL
#   define VSF_GPIO_CFG_REIMPLEMENT_API_SET_INPUT                   ENABLED
#   define VSF_GPIO_CFG_REIMPLEMENT_API_SET_OUTPUT                  ENABLED
#   define VSF_GPIO_CFG_REIMPLEMENT_API_SWITCH_DIRECTION            ENABLED
#   define VSF_GPIO_CFG_REIMPLEMENT_API_SET                         ENABLED
#   define VSF_GPIO_CFG_REIMPLEMENT_API_CLEAR                       ENABLED
#   define VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_SET              ENABLED
#   define VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_CLEAR            ENABLED
#   define VSF_GPIO_CFG_REIMPLEMENT_API_GET_PIN_CONFIGURATION       ENABLED
#   define VSF_GPIO_CFG_REIMPLEMENT_API_READ_OUTPUT_REGISTER        ENABLED
#   define VSF_GPIO_CFG_REIMPLEMENT_API_CAPABILITY                  ENABLED
#   define VSF_GPIO_CFG_REIMPLEMENT_API_EXTI_IRQ_GET_CONFIGURATION  ENABLED
#   define VSF_GPIO_CFG_REIMPLEMENT_API_EXTI_IRQ_CLEAR              ENABLED
#   define VSF_GPIO_CFG_REIMPLEMENT_API_CTRL                        ENABLED
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
#   ifndef VSF_GPIO_CFG_CAPABILITY_IS_ASYNC
#       define VSF_GPIO_CFG_CAPABILITY_IS_ASYNC                     0
#   endif

#   ifndef VSF_GPIO_CFG_CAPABILITY_PIN_COUNT
#       define VSF_GPIO_CFG_CAPABILITY_PIN_COUNT                    32
#   endif
#   ifndef VSF_GPIO_CFG_CAPABILITY_PIN_MASK
#       define VSF_GPIO_CFG_CAPABILITY_PIN_MASK                     0xFFFFFFFF
#   endif
#endif

#ifndef VSF_GPIO_CFG_REAL_PREFIX
#   if defined(VSF_GPIO_CFG_IMP_RENAME_DEVICE_PREFIX) && (VSF_GPIO_CFG_IMP_RENAME_DEVICE_PREFIX == ENABLED)
#       define VSF_GPIO_CFG_REAL_PREFIX                             VSF_GPIO_CFG_IMP_DEVICE_PREFIX
#   else
#       define VSF_GPIO_CFG_REAL_PREFIX                             VSF_GPIO_CFG_IMP_PREFIX
#   endif
#endif

#if defined(VSF_GPIO_CFG_IMP_RENAME_DEVICE_PREFIX) && (VSF_GPIO_CFG_IMP_RENAME_DEVICE_PREFIX == ENABLED)
#   define vsf_real_gpios                              VSF_MCONNECT(VSF_GPIO_CFG_IMP_DEVICE_PREFIX, s)
#   define vsf_real_gpio_t                             VSF_MCONNECT(VSF_GPIO_CFG_IMP_DEVICE_PREFIX, _t)
#   define vsf_real_gpio_port_config_pins              VSF_MCONNECT(VSF_GPIO_CFG_IMP_DEVICE_PREFIX, _port_config_pins)
#   define vsf_real_gpio_get_pin_configuration         VSF_MCONNECT(VSF_GPIO_CFG_IMP_DEVICE_PREFIX, _get_pin_configuration)
#   define vsf_real_gpio_read                          VSF_MCONNECT(VSF_GPIO_CFG_IMP_DEVICE_PREFIX, _read)
#   define vsf_real_gpio_read_output_register          VSF_MCONNECT(VSF_GPIO_CFG_IMP_DEVICE_PREFIX, _read_output_register)
#   define vsf_real_gpio_set_input                     VSF_MCONNECT(VSF_GPIO_CFG_IMP_DEVICE_PREFIX, _set_input)
#   define vsf_real_gpio_set_output                    VSF_MCONNECT(VSF_GPIO_CFG_IMP_DEVICE_PREFIX, _set_output)
#   define vsf_real_gpio_get_direction                 VSF_MCONNECT(VSF_GPIO_CFG_IMP_DEVICE_PREFIX, _get_direction)
#   define vsf_real_gpio_set_direction                 VSF_MCONNECT(VSF_GPIO_CFG_IMP_DEVICE_PREFIX, _set_direction)
#   define vsf_real_gpio_switch_direction              VSF_MCONNECT(VSF_GPIO_CFG_IMP_DEVICE_PREFIX, _switch_direction)
#   define vsf_real_gpio_write                         VSF_MCONNECT(VSF_GPIO_CFG_IMP_DEVICE_PREFIX, _write)
#   define vsf_real_gpio_set                           VSF_MCONNECT(VSF_GPIO_CFG_IMP_DEVICE_PREFIX, _set)
#   define vsf_real_gpio_clear                         VSF_MCONNECT(VSF_GPIO_CFG_IMP_DEVICE_PREFIX, _clear)
#   define vsf_real_gpio_output_and_set                VSF_MCONNECT(VSF_GPIO_CFG_IMP_DEVICE_PREFIX, _output_and_set)
#   define vsf_real_gpio_output_and_clear              VSF_MCONNECT(VSF_GPIO_CFG_IMP_DEVICE_PREFIX, _output_and_clear)
#   define vsf_real_gpio_capability                    VSF_MCONNECT(VSF_GPIO_CFG_IMP_DEVICE_PREFIX, _capability)
#   define vsf_real_gpio_exti_irq_get_configuration    VSF_MCONNECT(VSF_GPIO_CFG_IMP_DEVICE_PREFIX, _exti_irq_get_configuration)
#   define vsf_real_gpio_exti_irq_clear                VSF_MCONNECT(VSF_GPIO_CFG_IMP_DEVICE_PREFIX, _exti_irq_clear)
#   define vsf_real_gpio_ctrl                          VSF_MCONNECT(VSF_GPIO_CFG_IMP_DEVICE_PREFIX, _ctrl)
#   define vsf_real_gpio_port_config_pin               VSF_MCONNECT(VSF_GPIO_CFG_IMP_DEVICE_PREFIX, _port_config_pin)
#   define vsf_real_gpio_ports_config_pin              VSF_MCONNECT(VSF_GPIO_CFG_IMP_DEVICE_PREFIX, _ports_config_pin)
#   define vsf_real_gpio_ports_config_pins             VSF_MCONNECT(VSF_GPIO_CFG_IMP_DEVICE_PREFIX, _ports_config_pins)
#else
#   define vsf_real_gpios                              VSF_MCONNECT(VSF_GPIO_CFG_REAL_PREFIX, _gpios)
#   define vsf_real_gpio_t                             VSF_MCONNECT(VSF_GPIO_CFG_REAL_PREFIX, _gpio_t)
#   define vsf_real_gpio_port_config_pins              VSF_MCONNECT(VSF_GPIO_CFG_REAL_PREFIX, _gpio_port_config_pins)
#   define vsf_real_gpio_get_pin_configuration         VSF_MCONNECT(VSF_GPIO_CFG_REAL_PREFIX, _gpio_get_pin_configuration)
#   define vsf_real_gpio_read                          VSF_MCONNECT(VSF_GPIO_CFG_REAL_PREFIX, _gpio_read)
#   define vsf_real_gpio_read_output_register          VSF_MCONNECT(VSF_GPIO_CFG_REAL_PREFIX, _gpio_read_output_register)
#   define vsf_real_gpio_set_input                     VSF_MCONNECT(VSF_GPIO_CFG_REAL_PREFIX, _gpio_set_input)
#   define vsf_real_gpio_set_output                    VSF_MCONNECT(VSF_GPIO_CFG_REAL_PREFIX, _gpio_set_output)
#   define vsf_real_gpio_get_direction                 VSF_MCONNECT(VSF_GPIO_CFG_REAL_PREFIX, _gpio_get_direction)
#   define vsf_real_gpio_set_direction                 VSF_MCONNECT(VSF_GPIO_CFG_REAL_PREFIX, _gpio_set_direction)
#   define vsf_real_gpio_switch_direction              VSF_MCONNECT(VSF_GPIO_CFG_REAL_PREFIX, _gpio_switch_direction)
#   define vsf_real_gpio_write                         VSF_MCONNECT(VSF_GPIO_CFG_REAL_PREFIX, _gpio_write)
#   define vsf_real_gpio_set                           VSF_MCONNECT(VSF_GPIO_CFG_REAL_PREFIX, _gpio_set)
#   define vsf_real_gpio_clear                         VSF_MCONNECT(VSF_GPIO_CFG_REAL_PREFIX, _gpio_clear)
#   define vsf_real_gpio_output_and_set                VSF_MCONNECT(VSF_GPIO_CFG_REAL_PREFIX, _gpio_output_and_set)
#   define vsf_real_gpio_output_and_clear              VSF_MCONNECT(VSF_GPIO_CFG_REAL_PREFIX, _gpio_output_and_clear)
#   define vsf_real_gpio_capability                    VSF_MCONNECT(VSF_GPIO_CFG_REAL_PREFIX, _gpio_capability)
#   define vsf_real_gpio_exti_irq_get_configuration    VSF_MCONNECT(VSF_GPIO_CFG_REAL_PREFIX, _gpio_exti_irq_get_configuration)
#   define vsf_real_gpio_exti_irq_clear                VSF_MCONNECT(VSF_GPIO_CFG_REAL_PREFIX, _gpio_exti_irq_clear)
#   define vsf_real_gpio_ctrl                          VSF_MCONNECT(VSF_GPIO_CFG_REAL_PREFIX, _gpio_ctrl)
#   define vsf_real_gpio_port_config_pin               VSF_MCONNECT(VSF_GPIO_CFG_REAL_PREFIX, _gpio_port_config_pin)
#   define vsf_real_gpio_ports_config_pin              VSF_MCONNECT(VSF_GPIO_CFG_REAL_PREFIX, _gpio_ports_config_pin)
#   define vsf_real_gpio_ports_config_pins             VSF_MCONNECT(VSF_GPIO_CFG_REAL_PREFIX, _gpio_ports_config_pins)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_GPIO_CFG_REIMPLEMENT_API_SET_INPUT == DISABLED
void vsf_real_gpio_set_input(vsf_real_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(0 != pin_mask);

    vsf_real_gpio_set_direction(gpio_ptr, pin_mask, 0);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_API_SET_OUTPUT == DISABLED
void vsf_real_gpio_set_output(vsf_real_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(0 != pin_mask);

    vsf_real_gpio_set_direction(gpio_ptr, pin_mask, pin_mask);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_API_SWITCH_DIRECTION == DISABLED
void vsf_real_gpio_switch_direction(vsf_real_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(0 != pin_mask);

    uint32_t ret = ~vsf_real_gpio_get_direction(gpio_ptr, pin_mask);
    vsf_real_gpio_set_direction(gpio_ptr, pin_mask, ret);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_API_SET == DISABLED
void vsf_real_gpio_set(vsf_real_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(0 != pin_mask);

    vsf_real_gpio_write(gpio_ptr, pin_mask, pin_mask);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_API_CLEAR == DISABLED
void vsf_real_gpio_clear(vsf_real_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(0 != pin_mask);

    vsf_real_gpio_write(gpio_ptr, pin_mask, 0);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_SET == DISABLED
void vsf_real_gpio_output_and_set(vsf_real_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(0 != pin_mask);

#if VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_SET == 0
    VSF_HAL_ASSERT(0);
#else
#if VSF_GPIO_CFG_CHANGE_DIR_FIRST == ENABLED
    vsf_real_gpio_set_output(gpio_ptr, pin_mask);
    vsf_real_gpio_set(gpio_ptr, pin_mask);
#else
    vsf_real_gpio_set(gpio_ptr, pin_mask);
    vsf_real_gpio_set_output(gpio_ptr, pin_mask);
#endif
#endif
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_CLEAR == DISABLED
void vsf_real_gpio_output_and_clear(vsf_real_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(0 != pin_mask);

#if VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_CLEAR == 0
    VSF_HAL_ASSERT(0);
#else
#if VSF_GPIO_CFG_CHANGE_DIR_FIRST == ENABLED
    vsf_real_gpio_set_output(gpio_ptr, pin_mask);
    vsf_real_gpio_clear(gpio_ptr, pin_mask);
#else
    vsf_real_gpio_clear(gpio_ptr, pin_mask);
    vsf_real_gpio_set_output(gpio_ptr, pin_mask);
#endif
#endif
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_API_GET_PIN_CONFIGURATION == DISABLED
vsf_err_t vsf_real_gpio_get_pin_configuration(vsf_real_gpio_t *gpio_ptr, uint16_t pin_index, vsf_gpio_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return VSF_ERR_NOT_SUPPORT;
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_API_READ_OUTPUT_REGISTER == DISABLED
vsf_gpio_pin_mask_t vsf_real_gpio_read_output_register(vsf_real_gpio_t *gpio_ptr)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return 0;
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_gpio_capability_t vsf_real_gpio_capability(vsf_real_gpio_t *gpio_ptr)
{
    vsf_gpio_capability_t gpio_capability = {
        .is_async                       = VSF_GPIO_CFG_CAPABILITY_IS_ASYNC,
        .support_output_and_set         = VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_SET,
        .support_output_and_clear       = VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_CLEAR,
        .support_interrupt              = VSF_GPIO_CFG_CAPABILITY_SUPPORT_INTERRUPT,
        .can_read_in_gpio_output_mode   = VSF_GPIO_CFG_CAPABILITY_CAN_READ_IN_GPIO_OUTPUT_MODE,
        .can_read_in_alternate_mode     = VSF_GPIO_CFG_CAPABILITY_CAN_READ_IN_ALTERNATE_MODE,
        .pin_count                      = VSF_GPIO_CFG_CAPABILITY_PIN_COUNT,
        .pin_mask                       = VSF_GPIO_CFG_CAPABILITY_PIN_MASK,
    };

    return gpio_capability;
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_API_EXTI_IRQ_GET_CONFIGURATION == DISABLED
vsf_err_t vsf_real_gpio_exti_irq_get_configuration(vsf_real_gpio_t *gpio_ptr, vsf_gpio_exti_irq_cfg_t *irq_cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(NULL != irq_cfg_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return VSF_ERR_NOT_SUPPORT;
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_API_EXTI_IRQ_CLEAR == DISABLED
vsf_gpio_pin_mask_t vsf_real_gpio_exti_irq_clear(vsf_real_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return 0;
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_API_CTRL == DISABLED
vsf_err_t vsf_real_gpio_ctrl(vsf_real_gpio_t *gpio_ptr, vsf_gpio_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return VSF_ERR_NOT_SUPPORT;
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_API_PORTS_CONFIG_PIN == DISABLED
vsf_err_t vsf_real_gpio_ports_config_pin(vsf_gpio_port_cfg_pin_t *cfg_ptr,
                                         uint_fast8_t             count)
{
    VSF_HAL_ASSERT(cfg_ptr != NULL);
    VSF_HAL_ASSERT(count != 0);

    for (int i = 0; i < count; i++) {
        vsf_gpio_cfg_t cfg = {
            .alternate_function = cfg_ptr[i].alternate_function,
            .mode               = cfg_ptr[i].mode,
        };
        uint16_t port_index = vsf_gpio_get_port(cfg_ptr[i].port_pin_index);
        uint16_t pin_index  = vsf_gpio_get_pin(cfg_ptr[i].port_pin_index);
        vsf_real_gpio_t *gpio_ptr = vsf_real_gpios[port_index];
        vsf_err_t        result =
            vsf_real_gpio_port_config_pins(gpio_ptr, 1 << pin_index, &cfg);
        if (result != VSF_ERR_NONE) {
            return result;
        }
    }

    return VSF_ERR_NONE;
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_API_PORTS_CONFIG_PINS == DISABLED
vsf_err_t vsf_real_gpio_ports_config_pins(vsf_gpio_port_cfg_pins_t *cfg_ptr,
                                          uint_fast8_t              count)
{
    VSF_HAL_ASSERT(cfg_ptr != NULL);
    VSF_HAL_ASSERT(count != 0);

    for (int i = 0; i < count; i++) {
        uint16_t         port_index  = cfg_ptr[i].port_index;
        vsf_real_gpio_t *gpio_ptr = vsf_real_gpios[port_index];
        vsf_gpio_cfg_t   cfg         = {
                      .alternate_function = cfg_ptr[i].alternate_function,
                      .mode               = cfg_ptr[i].mode,
        };
        vsf_err_t result =
            vsf_real_gpio_port_config_pins(gpio_ptr, cfg_ptr[i].pin_mask, &cfg);
        if (result != VSF_ERR_NONE) {
            return result;
        }
    }

    return VSF_ERR_NONE;
}
#endif

/*============================ MACROS ========================================*/

#undef VSF_GPIO_CFG_REIMPLEMENT_TYPE_CAPABILITY
#undef VSF_GPIO_CFG_REIMPLEMENT_TYPE_CFG
#undef VSF_GPIO_CFG_REIMPLEMENT_TYPE_MODE
#undef VSF_GPIO_CFG_RETAIN_DEFINED_PREFIX
#undef VSF_GPIO_CFG_REIMPLEMENT_API_SET_INPUT
#undef VSF_GPIO_CFG_REIMPLEMENT_API_SET_OUTPUT
#undef VSF_GPIO_CFG_REIMPLEMENT_API_SWITCH_DIRECTION
#undef VSF_GPIO_CFG_REIMPLEMENT_API_SET
#undef VSF_GPIO_CFG_REIMPLEMENT_API_CLEAR
#undef VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_SET
#undef VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_CLEAR
#undef VSF_GPIO_CFG_REIMPLEMENT_API_GET_PIN_CONFIGURATION
#undef VSF_GPIO_CFG_REIMPLEMENT_API_READ_OUTPUT_REGISTER
#undef VSF_GPIO_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_GPIO_CFG_REIMPLEMENT_API_EXTI_IRQ_GET_CONFIGURATION
#undef VSF_GPIO_CFG_REIMPLEMENT_API_EXTI_IRQ_CLEAR
#undef VSF_GPIO_CFG_REIMPLEMENT_API_CTRL
#undef VSF_GPIO_CFG_REIMPLEMENT_API_PORTS_CONFIG_PIN
#undef VSF_GPIO_CFG_REIMPLEMENT_API_PORTS_CONFIG_PINS
#undef VSF_GPIO_CFG_CHANGE_DIR_FIRST
#undef VSF_GPIO_CFG_CAPABILITY_SUPPORT_CONFIG_PIN
#undef VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_SET
#undef VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_CLEAR
#undef VSF_GPIO_CFG_CAPABILITY_SUPPORT_INTERRUPT
#undef VSF_GPIO_CFG_CAPABILITY_CAN_READ_IN_GPIO_OUTPUT_MODE
#undef VSF_GPIO_CFG_CAPABILITY_CAN_READ_IN_ALTERNATE_MODE
#undef VSF_GPIO_CFG_CAPABILITY_IS_ASYNC
#undef VSF_GPIO_CFG_CAPABILITY_PIN_COUNT
#undef VSF_GPIO_CFG_CAPABILITY_PIN_MASK
#undef vsf_real_gpios
#undef vsf_real_gpio_t
#undef vsf_real_gpio_port_config_pins
#undef vsf_real_gpio_get_pin_configuration
#undef vsf_real_gpio_read
#undef vsf_real_gpio_read_output_register
#undef vsf_real_gpio_set_input
#undef vsf_real_gpio_set_output
#undef vsf_real_gpio_get_direction
#undef vsf_real_gpio_set_direction
#undef vsf_real_gpio_switch_direction
#undef vsf_real_gpio_write
#undef vsf_real_gpio_set
#undef vsf_real_gpio_clear
#undef vsf_real_gpio_output_and_set
#undef vsf_real_gpio_output_and_clear
#undef vsf_real_gpio_capability
#undef vsf_real_gpio_exti_irq_get_configuration
#undef vsf_real_gpio_exti_irq_clear
#undef vsf_real_gpio_ctrl
#undef vsf_real_gpio_port_config_pin
#undef vsf_real_gpio_ports_config_pin
#undef vsf_real_gpio_ports_config_pins

/*============================ MACROS ========================================*/

#ifdef VSF_GPIO_CFG_IMP_REMAP_PREFIX
#   ifndef VSF_GPIO_CFG_ORIGIN_PREFIX
#       define VSF_GPIO_CFG_ORIGIN_PREFIX       VSF_GPIO_CFG_IMP_PREFIX
#   endif

#   define vsf_origin_gpio_t                    VSF_MCONNECT(VSF_GPIO_CFG_ORIGIN_PREFIX, _gpio_t)
#   define vsf_origin_gpio_capability           VSF_MCONNECT(VSF_GPIO_CFG_ORIGIN_PREFIX, _gpio_capability)
#   define vsf_origin_gpio_port_config_pins     VSF_MCONNECT(VSF_GPIO_CFG_ORIGIN_PREFIX, _gpio_port_config_pins)
#   define vsf_origin_gpio_get_pin_configuration VSF_MCONNECT(VSF_GPIO_CFG_ORIGIN_PREFIX, _gpio_get_pin_configuration)
#   define vsf_origin_gpio_set_direction        VSF_MCONNECT(VSF_GPIO_CFG_ORIGIN_PREFIX, _gpio_set_direction)
#   define vsf_origin_gpio_get_direction        VSF_MCONNECT(VSF_GPIO_CFG_ORIGIN_PREFIX, _gpio_get_direction)
#   define vsf_origin_gpio_set_input            VSF_MCONNECT(VSF_GPIO_CFG_ORIGIN_PREFIX, _gpio_set_input)
#   define vsf_origin_gpio_set_output           VSF_MCONNECT(VSF_GPIO_CFG_ORIGIN_PREFIX, _gpio_set_output)
#   define vsf_origin_gpio_switch_direction     VSF_MCONNECT(VSF_GPIO_CFG_ORIGIN_PREFIX, _gpio_switch_direction)
#   define vsf_origin_gpio_read                 VSF_MCONNECT(VSF_GPIO_CFG_ORIGIN_PREFIX, _gpio_read)
#   define vsf_origin_gpio_read_output_register VSF_MCONNECT(VSF_GPIO_CFG_ORIGIN_PREFIX, _gpio_read_output_register)
#   define vsf_origin_gpio_write                VSF_MCONNECT(VSF_GPIO_CFG_ORIGIN_PREFIX, _gpio_write)
#   define vsf_origin_gpio_set                  VSF_MCONNECT(VSF_GPIO_CFG_ORIGIN_PREFIX, _gpio_set)
#   define vsf_origin_gpio_clear                VSF_MCONNECT(VSF_GPIO_CFG_ORIGIN_PREFIX, _gpio_clear)
#   define vsf_origin_gpio_output_and_set       VSF_MCONNECT(VSF_GPIO_CFG_ORIGIN_PREFIX, _gpio_output_and_set)
#   define vsf_origin_gpio_output_and_clear     VSF_MCONNECT(VSF_GPIO_CFG_ORIGIN_PREFIX, _gpio_output_and_clear)
#   define vsf_origin_gpio_toggle               VSF_MCONNECT(VSF_GPIO_CFG_ORIGIN_PREFIX, _gpio_toggle)
#   define vsf_origin_gpio_exti_irq_enable      VSF_MCONNECT(VSF_GPIO_CFG_ORIGIN_PREFIX, _gpio_exti_irq_enable)
#   define vsf_origin_gpio_exti_irq_disable     VSF_MCONNECT(VSF_GPIO_CFG_ORIGIN_PREFIX, _gpio_exti_irq_disable)
#   define vsf_origin_gpio_exti_irq_clear      VSF_MCONNECT(VSF_GPIO_CFG_ORIGIN_PREFIX, _gpio_exti_irq_clear)
#   define vsf_origin_gpio_exti_irq_config      VSF_MCONNECT(VSF_GPIO_CFG_ORIGIN_PREFIX, _gpio_exti_irq_config)
#   define vsf_origin_gpio_ctrl                 VSF_MCONNECT(VSF_GPIO_CFG_ORIGIN_PREFIX, _gpio_ctrl)

#   define vsf_remap_gpio_t                     VSF_MCONNECT(VSF_GPIO_CFG_IMP_REMAP_PREFIX, _gpio_t)
#   define vsf_remap_gpio_capability            VSF_MCONNECT(VSF_GPIO_CFG_IMP_REMAP_PREFIX, _gpio_capability)
#   define vsf_remap_gpio_port_config_pins      VSF_MCONNECT(VSF_GPIO_CFG_IMP_REMAP_PREFIX, _gpio_port_config_pins)
#   define vsf_remap_gpio_get_pin_configuration VSF_MCONNECT(VSF_GPIO_CFG_IMP_REMAP_PREFIX, _gpio_get_pin_configuration)
#   define vsf_remap_gpio_set_direction         VSF_MCONNECT(VSF_GPIO_CFG_IMP_REMAP_PREFIX, _gpio_set_direction)
#   define vsf_remap_gpio_get_direction         VSF_MCONNECT(VSF_GPIO_CFG_IMP_REMAP_PREFIX, _gpio_get_direction)
#   define vsf_remap_gpio_set_input             VSF_MCONNECT(VSF_GPIO_CFG_IMP_REMAP_PREFIX, _gpio_set_input)
#   define vsf_remap_gpio_set_output            VSF_MCONNECT(VSF_GPIO_CFG_IMP_REMAP_PREFIX, _gpio_set_output)
#   define vsf_remap_gpio_switch_direction      VSF_MCONNECT(VSF_GPIO_CFG_IMP_REMAP_PREFIX, _gpio_switch_direction)
#   define vsf_remap_gpio_read                  VSF_MCONNECT(VSF_GPIO_CFG_IMP_REMAP_PREFIX, _gpio_read)
#   define vsf_remap_gpio_read_output_register  VSF_MCONNECT(VSF_GPIO_CFG_IMP_REMAP_PREFIX, _gpio_read_output_register)
#   define vsf_remap_gpio_write                 VSF_MCONNECT(VSF_GPIO_CFG_IMP_REMAP_PREFIX, _gpio_write)
#   define vsf_remap_gpio_set                   VSF_MCONNECT(VSF_GPIO_CFG_IMP_REMAP_PREFIX, _gpio_set)
#   define vsf_remap_gpio_clear                 VSF_MCONNECT(VSF_GPIO_CFG_IMP_REMAP_PREFIX, _gpio_clear)
#   define vsf_remap_gpio_output_and_set        VSF_MCONNECT(VSF_GPIO_CFG_IMP_REMAP_PREFIX, _gpio_output_and_set)
#   define vsf_remap_gpio_output_and_clear      VSF_MCONNECT(VSF_GPIO_CFG_IMP_REMAP_PREFIX, _gpio_output_and_clear)
#   define vsf_remap_gpio_toggle                VSF_MCONNECT(VSF_GPIO_CFG_IMP_REMAP_PREFIX, _gpio_toggle)
#   define vsf_remap_gpio_exti_irq_enable       VSF_MCONNECT(VSF_GPIO_CFG_IMP_REMAP_PREFIX, _gpio_exti_irq_enable)
#   define vsf_remap_gpio_exti_irq_disable      VSF_MCONNECT(VSF_GPIO_CFG_IMP_REMAP_PREFIX, _gpio_exti_irq_disable)
#   define vsf_remap_gpio_exti_irq_clear        VSF_MCONNECT(VSF_GPIO_CFG_IMP_REMAP_PREFIX, _gpio_exti_irq_clear)
#   define vsf_remap_gpio_exti_irq_config       VSF_MCONNECT(VSF_GPIO_CFG_IMP_REMAP_PREFIX, _gpio_exti_irq_config)
#   define vsf_remap_gpio_ctrl                  VSF_MCONNECT(VSF_GPIO_CFG_IMP_REMAP_PREFIX, _gpio_ctrl)

#   define VSF_GPIO_CFG_IMP_REMAP_FUNCTIONS                                                                                                 \
        vsf_err_t vsf_origin_gpio_port_config_pins(vsf_origin_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_cfg_t *cfg)          \
        {                                                                                                                                   \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                               \
            VSF_HAL_ASSERT(pin_mask != 0);                                                                                                  \
            VSF_HAL_ASSERT(cfg != NULL);                                                                                                    \
            return vsf_remap_gpio_port_config_pins(gpio_ptr, pin_mask, cfg);                                                                \
        }                                                                                                                                   \
        vsf_err_t vsf_origin_gpio_get_pin_configuration(vsf_origin_gpio_t *gpio_ptr, uint16_t pin_index, vsf_gpio_cfg_t *cfg)             \
        {                                                                                                                                   \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                               \
            VSF_HAL_ASSERT(cfg != NULL);                                                                                                    \
            return vsf_remap_gpio_get_pin_configuration(gpio_ptr, pin_index, cfg);                                                          \
        }                                                                                                                                   \
        void vsf_origin_gpio_set_direction(vsf_origin_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_pin_mask_t direction_mask)   \
        {                                                                                                                                   \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                               \
            vsf_remap_gpio_set_direction(gpio_ptr, pin_mask, direction_mask);                                                               \
        }                                                                                                                                   \
        vsf_gpio_pin_mask_t vsf_origin_gpio_get_direction(vsf_origin_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                        \
        {                                                                                                                                   \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                               \
            return vsf_remap_gpio_get_direction(gpio_ptr, pin_mask);                                                                        \
        }                                                                                                                                   \
        void vsf_origin_gpio_set_input(vsf_origin_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                           \
        {                                                                                                                                   \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                               \
            vsf_remap_gpio_set_input(gpio_ptr, pin_mask);                                                                                   \
        }                                                                                                                                   \
        void vsf_origin_gpio_set_output(vsf_origin_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                          \
        {                                                                                                                                   \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                               \
            vsf_remap_gpio_set_output(gpio_ptr, pin_mask);                                                                                  \
        }                                                                                                                                   \
        void vsf_origin_gpio_switch_direction(vsf_origin_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                    \
        {                                                                                                                                   \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                               \
            vsf_remap_gpio_switch_direction(gpio_ptr, pin_mask);                                                                            \
        }                                                                                                                                   \
        vsf_gpio_pin_mask_t vsf_origin_gpio_read(vsf_origin_gpio_t *gpio_ptr)                                                               \
        {                                                                                                                                   \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                               \
            return vsf_remap_gpio_read(gpio_ptr);                                                                                           \
        }                                                                                                                                   \
        vsf_gpio_pin_mask_t vsf_origin_gpio_read_output_register(vsf_origin_gpio_t *gpio_ptr)                                               \
        {                                                                                                                                   \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                               \
            return vsf_remap_gpio_read_output_register(gpio_ptr);                                                                           \
        }                                                                                                                                   \
        void vsf_origin_gpio_write(vsf_origin_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_pin_mask_t value)                    \
        {                                                                                                                                   \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                               \
            vsf_remap_gpio_write(gpio_ptr, pin_mask, value);                                                                                \
        }                                                                                                                                   \
        void vsf_origin_gpio_set(vsf_origin_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                                 \
        {                                                                                                                                   \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                               \
            vsf_remap_gpio_set(gpio_ptr, pin_mask);                                                                                         \
        }                                                                                                                                   \
        void vsf_origin_gpio_clear(vsf_origin_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                               \
        {                                                                                                                                   \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                               \
            vsf_remap_gpio_clear(gpio_ptr, pin_mask);                                                                                       \
        }                                                                                                                                   \
        void vsf_origin_gpio_toggle(vsf_origin_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                              \
        {                                                                                                                                   \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                               \
            vsf_remap_gpio_toggle(gpio_ptr, pin_mask);                                                                                      \
        }                                                                                                                                   \
        void vsf_origin_gpio_output_and_set(vsf_origin_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                      \
        {                                                                                                                                   \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                               \
            vsf_remap_gpio_output_and_set(gpio_ptr, pin_mask);                                                                              \
        }                                                                                                                                   \
        void vsf_origin_gpio_output_and_clear(vsf_origin_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                    \
        {                                                                                                                                   \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                               \
            vsf_remap_gpio_output_and_clear(gpio_ptr, pin_mask);                                                                            \
        }                                                                                                                                   \
        vsf_gpio_capability_t vsf_origin_gpio_capability(vsf_origin_gpio_t *gpio_ptr)                                                       \
        {                                                                                                                                   \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                               \
            return vsf_remap_gpio_capability(gpio_ptr);                                                                                     \
        }                                                                                                                                   \
        vsf_err_t vsf_origin_gpio_exti_irq_enable(vsf_origin_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                \
        {                                                                                                                                   \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                               \
            return vsf_remap_gpio_exti_irq_enable(gpio_ptr, pin_mask);                                                                      \
        }                                                                                                                                   \
        vsf_err_t vsf_origin_gpio_exti_irq_disable(vsf_origin_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                               \
        {                                                                                                                                   \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                               \
            return vsf_remap_gpio_exti_irq_disable(gpio_ptr, pin_mask);                                                                     \
        }                                                                                                                                   \
        vsf_gpio_pin_mask_t vsf_origin_gpio_exti_irq_clear(vsf_origin_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                        \
        {                                                                                                                                   \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                               \
            return vsf_remap_gpio_exti_irq_clear(gpio_ptr, pin_mask);                                                                       \
        }                                                                                                                                   \
        vsf_err_t vsf_origin_gpio_exti_irq_config(vsf_origin_gpio_t *gpio_ptr, vsf_gpio_exti_irq_cfg_t *cfg)                                \
        {                                                                                                                                   \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                               \
            return vsf_remap_gpio_exti_irq_config(gpio_ptr, cfg);                                                                           \
        }                                                                                                                                   \
        vsf_err_t vsf_origin_gpio_ctrl(vsf_origin_gpio_t *gpio_ptr, vsf_gpio_ctrl_t ctrl, void *param)                                      \
        {                                                                                                                                   \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                               \
            return vsf_remap_gpio_ctrl(gpio_ptr, ctrl, param);                                                                              \
        }
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#ifndef VSF_HAL_TEMPLATE_IMP_NAME
#   define VSF_HAL_TEMPLATE_IMP_NAME                _gpio
#endif

#ifndef VSF_HAL_TEMPLATE_IMP_UPCASE_NAME
#   define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME         _GPIO
#endif

#ifndef VSF_HAL_TEMPLATE_IMP_COUNT_SUFFIX
#   define VSF_HAL_TEMPLATE_IMP_COUNT_SUFFIX        _PORT_COUNT
#endif

#ifndef VSF_HAL_TEMPLATE_IMP_MASK_SUFFIX
#   define VSF_HAL_TEMPLATE_IMP_MASK_SUFFIX         _PORT_MASK
#endif

#if !defined(VSF_GPIO_CFG_IMP_PREFIX) && !defined(VSF_GPIO_CFG_IMP_DEVICE_PREFIX)
#   error "Please define VSF_GPIO_CFG_IMP_PREFIX in gpio driver"
#endif

#if !defined(VSF_GPIO_CFG_IMP_UPCASE_PREFIX) && !defined(VSF_GPIO_CFG_IMP_DEVICE_UPCASE_PREFIX)
#   error "Please define VSF_GPIO_CFG_IMP_UPCASE_PREFIX in gpio driver"
#endif

#ifndef VSF_GPIO_CFG_IMP_COUNT_MASK_PREFIX
#   define VSF_GPIO_CFG_IMP_COUNT_MASK_PREFIX       VSF_GPIO_CFG_IMP_UPCASE_PREFIX
#endif

#ifdef VSF_GPIO_CFG_IMP_REMAP_FUNCTIONS
#   define VSF_HAL_CFG_IMP_REMAP_FUNCTIONS          VSF_GPIO_CFG_IMP_REMAP_FUNCTIONS
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#if VSF_GPIO_CFG_RETAIN_DEFINED_PREFIX == DISABLED
#   undef VSF_GPIO_CFG_IMP_PREFIX
#   undef VSF_GPIO_CFG_IMP_UPCASE_PREFIX
#   undef VSF_GPIO_CFG_IMP_DEVICE_PREFIX
#   undef VSF_GPIO_CFG_IMP_DEVICE_UPCASE_PREFIX
#endif

#undef VSF_GPIO_CFG_REAL_PREFIX
#undef VSF_GPIO_CFG_ORIGIN_PREFIX

#undef VSF_GPIO_CFG_IMP_COUNT_MASK_PREFIX
#undef VSF_GPIO_CFG_IMP_LV0
#undef VSF_GPIO_CFG_IMP_REMAP_FUNCTIONS
#undef VSF_GPIO_CFG_IMP_HAS_OP
#undef VSF_GPIO_CFG_IMP_EXTERN_OP
#undef VSF_GPIO_CFG_IMP_REMAP_PREFIX

#undef vsf_origin_gpio_t
#undef vsf_origin_gpio_capability
#undef vsf_origin_gpio_port_config_pins
#undef vsf_origin_gpio_get_pin_configuration
#undef vsf_origin_gpio_set_direction
#undef vsf_origin_gpio_get_direction
#undef vsf_origin_gpio_set_input
#undef vsf_origin_gpio_set_output
#undef vsf_origin_gpio_switch_direction
#undef vsf_origin_gpio_read
#undef vsf_origin_gpio_read_output_register
#undef vsf_origin_gpio_write
#undef vsf_origin_gpio_set
#undef vsf_origin_gpio_clear
#undef vsf_origin_gpio_output_and_set
#undef vsf_origin_gpio_output_and_clear
#undef vsf_origin_gpio_toggle
#undef vsf_origin_gpio_exti_irq_config
#undef vsf_origin_gpio_exti_irq_enable
#undef vsf_origin_gpio_exti_irq_disable
#undef vsf_origin_gpio_exti_irq_clear
#undef vsf_origin_gpio_ctrl

#undef vsf_remap_gpio_t
#undef vsf_remap_gpio_capability
#undef vsf_remap_gpio_port_config_pins
#undef vsf_remap_gpio_get_pin_configuration
#undef vsf_remap_gpio_set_direction
#undef vsf_remap_gpio_get_direction
#undef vsf_remap_gpio_set_input
#undef vsf_remap_gpio_set_output
#undef vsf_remap_gpio_switch_direction
#undef vsf_remap_gpio_read
#undef vsf_remap_gpio_read_output_register
#undef vsf_remap_gpio_write
#undef vsf_remap_gpio_set
#undef vsf_remap_gpio_clear
#undef vsf_remap_gpio_output_and_set
#undef vsf_remap_gpio_output_and_clear
#undef vsf_remap_gpio_toggle
#undef vsf_remap_gpio_exti_irq_config
#undef vsf_remap_gpio_exti_irq_enable
#undef vsf_remap_gpio_exti_irq_disable
#undef vsf_remap_gpio_exti_irq_clear
#undef vsf_remap_gpio_ctrl

#undef VSF_HAL_TEMPLATE_IMP_NAME
#undef VSF_HAL_TEMPLATE_IMP_UPCASE_NAME

/*============================ STATIC ASSERTIONS ==============================*/

/* User-extensible macros for custom mode and IRQ mask values
 * Users can define these macros before including this template to append
 * their custom enum values to the uniqueness checks.
 *
 * Check modes for custom values:
 *   VSF_GPIO_CFG_MODE_CHECK_UNIQUE - Check mode for custom mode values
 *     Default: VSF_HAL_CHECK_MODE_LOOSE (loose mode, no overlapping bits)
 *     Can be set to: VSF_HAL_CHECK_MODE_STRICT (strict mode, no overlapping bits)
 *   VSF_GPIO_CFG_IRQ_MASK_CHECK_UNIQUE - Check mode for custom IRQ mask values
 *     Default: VSF_HAL_CHECK_MODE_STRICT (strict mode, no overlapping bits)
 *     Can be set to: VSF_HAL_CHECK_MODE_LOOSE (loose mode, no overlapping bits)
 *
 * Example usage in vendor driver:
 *   #define VSF_GPIO_CUSTOM_MODE_VALUES           MY_CUSTOM_MODE1, MY_CUSTOM_MODE2
 *   #define VSF_GPIO_CFG_MODE_CHECK_UNIQUE        VSF_HAL_CHECK_MODE_LOOSE
 *   #define VSF_GPIO_CUSTOM_IRQ_MASK_VALUES       MY_CUSTOM_IRQ1, MY_CUSTOM_IRQ2
 *   #define VSF_GPIO_CFG_IRQ_MASK_CHECK_UNIQUE    VSF_HAL_CHECK_MODE_STRICT
 *   #include "hal/driver/common/gpio/gpio_template.inc"
 */


#ifdef VSF_GPIO_CFG_MODE_CHECK_UNIQUE
/* Default mode lists - can be redefined by users before including this file */
#ifndef VSF_GPIO_CHECK_UNIQUE_DIRECTION_MODES
#   define VSF_GPIO_CHECK_UNIQUE_DIRECTION_MODES \
        VSF_GPIO_INPUT, \
        VSF_GPIO_ANALOG, \
        VSF_GPIO_OUTPUT_PUSH_PULL, \
        VSF_GPIO_OUTPUT_OPEN_DRAIN, \
        VSF_GPIO_EXTI
#endif

#ifndef VSF_GPIO_CHECK_UNIQUE_PULL_MODES
#   define VSF_GPIO_CHECK_UNIQUE_PULL_MODES \
        VSF_GPIO_NO_PULL_UP_DOWN, \
        VSF_GPIO_PULL_UP, \
        VSF_GPIO_PULL_DOWN
#endif

/* ==================== GROUP INTERNAL UNIQUENESS CHECKS ==================== */
/* Check uniqueness within each functional group */

/* Check direction mode uniqueness - all direction modes should have different values
 * Mandatory: VSF_GPIO_INPUT, VSF_GPIO_ANALOG, VSF_GPIO_OUTPUT_PUSH_PULL,
 *            VSF_GPIO_OUTPUT_OPEN_DRAIN, VSF_GPIO_EXTI
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_GPIO_CFG_MODE_CHECK_UNIQUE,
    VSF_GPIO_INPUT,
    VSF_GPIO_ANALOG,
    VSF_GPIO_OUTPUT_PUSH_PULL,
    VSF_GPIO_OUTPUT_OPEN_DRAIN,
    VSF_GPIO_EXTI
);

/* Check pull mode uniqueness - all pull modes should have different values
 * Mandatory: VSF_GPIO_NO_PULL_UP_DOWN, VSF_GPIO_PULL_UP, VSF_GPIO_PULL_DOWN
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_GPIO_CFG_MODE_CHECK_UNIQUE,
    VSF_GPIO_NO_PULL_UP_DOWN,
    VSF_GPIO_PULL_UP,
    VSF_GPIO_PULL_DOWN
);

#ifdef VSF_GPIO_CUSTOM_MODE_VALUES
/* Check uniqueness among custom mode values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_GPIO_CFG_MODE_CHECK_UNIQUE, VSF_GPIO_CUSTOM_MODE_VALUES);
#endif

VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_GPIO_CFG_MODE_CHECK_UNIQUE,
    /* Mandatory modes - always included */
    VSF_GPIO_INPUT, VSF_GPIO_ANALOG, VSF_GPIO_OUTPUT_PUSH_PULL,
    VSF_GPIO_OUTPUT_OPEN_DRAIN, VSF_GPIO_EXTI,
    VSF_GPIO_NO_PULL_UP_DOWN, VSF_GPIO_PULL_UP, VSF_GPIO_PULL_DOWN
    /* User-defined modes - appended by user configuration */
#ifdef VSF_GPIO_CUSTOM_MODE_VALUES
    , VSF_GPIO_CUSTOM_MODE_VALUES
#endif
);

#undef VSF_GPIO_CHECK_UNIQUE_DIRECTION_MODES
#undef VSF_GPIO_CHECK_UNIQUE_PULL_MODES
#endif  /* VSF_GPIO_CFG_MODE_CHECK_UNIQUE */

/* ==================== IRQ MASK UNIQUENESS CHECKS ==================== */

#ifdef VSF_GPIO_CFG_IRQ_MASK_CHECK_UNIQUE

#ifdef VSF_GPIO_CUSTOM_IRQ_MASK_VALUES
/* Check uniqueness among custom IRQ mask values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_GPIO_CFG_IRQ_MASK_CHECK_UNIQUE, VSF_GPIO_CUSTOM_IRQ_MASK_VALUES);
#endif

/* Note: GPIO typically doesn't have a standard IRQ mask enum, but custom IRQ masks can be added */
#endif  /* VSF_GPIO_CFG_IRQ_MASK_CHECK_UNIQUE */

#undef VSF_GPIO_CFG_MODE_CHECK_UNIQUE
#undef VSF_GPIO_CFG_IRQ_MASK_CHECK_UNIQUE
#undef VSF_GPIO_CUSTOM_MODE_VALUES
#undef VSF_GPIO_CUSTOM_IRQ_MASK_VALUES

#endif  /* VSF_HAL_USE_GPIO */
