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
    gpio_type                   *reg;
    vsf_gpio_pin_mask_t         pin_mask;
    uint8_t                     idx;
} VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t);
// HW end

/*============================ LOCAL VARIABLES ===============================*/

static const vsf_gpio_mode_t VSF_MCONNECT(__, VSF_GPIO_CFG_IMP_PREFIX, _afmodes)[] = {
    [__VSF_HW_AF_MODE_OUTPUT_PP]    = VSF_GPIO_AF_OUTPUT_PUSH_PULL,
    [__VSF_HW_AF_MODE_OUTPUT_PP_PU] = VSF_GPIO_AF_OUTPUT_PUSH_PULL | VSF_GPIO_PULL_UP,
    [__VSF_HW_AF_MODE_OUTPUT_PP_PD] = VSF_GPIO_AF_OUTPUT_PUSH_PULL | VSF_GPIO_PULL_DOWN,
    [__VSF_HW_AF_MODE_OUTPUT_OD]    = VSF_GPIO_AF_OUTPUT_OPEN_DRAIN,
    [__VSF_HW_AF_MODE_OUTPUT_OD_PU] = VSF_GPIO_AF_OUTPUT_OPEN_DRAIN | VSF_GPIO_PULL_UP,
    [__VSF_HW_AF_MODE_OUTPUT_OD_PD] = VSF_GPIO_AF_OUTPUT_OPEN_DRAIN | VSF_GPIO_PULL_DOWN,
    [__VSF_HW_AF_MODE_INPUT]        = VSF_GPIO_AF_INPUT,
    [__VSF_HW_AF_MODE_INPUT_PU]     = VSF_GPIO_AF_INPUT | VSF_GPIO_PULL_UP,
    [__VSF_HW_AF_MODE_INPUT_PD]     = VSF_GPIO_AF_INPUT | VSF_GPIO_PULL_DOWN,
};

/*============================ IMPLEMENTATION ================================*/

vsf_err_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_port_config_pins)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask,
    vsf_gpio_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(!(pin_mask & ~gpio_ptr->pin_mask));

    gpio_type *reg = gpio_ptr->reg;
    uint32_t mode = cfg_ptr->mode;

    // af mode can be set in mode and af_mode part of cfg_ptr->alternate_function,
    //  and mode has higher priority
    if (!(mode & __VSF_GPIO_AF)) {
        uint32_t af_mode = cfg_ptr->alternate_function >> VSF_HW_GPIO_AF_VALUE_BITS;
        VSF_HAL_ASSERT(af_mode < dimof(VSF_MCONNECT(__, VSF_GPIO_CFG_IMP_PREFIX, _afmodes)));
        mode |= VSF_MCONNECT(__, VSF_GPIO_CFG_IMP_PREFIX, _afmodes)[af_mode];
    }

    uint32_t cfgr = (mode >> 0) & 3;
    uint32_t omode = (mode >> 2) & 1;
    uint32_t pull = (mode >> 3) & 3;
    uint32_t odrvr = (mode >> 5) & 3;
    uint32_t hdrv = (mode >> 7) & 1;
    uint32_t exti = (mode >> 8) & 1;
    uint8_t af_value = cfg_ptr->alternate_function & ((1 << VSF_HW_GPIO_AF_VALUE_BITS) - 1);
    uint32_t current_pin_mask, offset_len2, offset_len4;

    for (int pin = 0; pin < VSF_HW_GPIO_PIN_COUNT; pin++) {
        current_pin_mask = 1 << pin;
        if (pin_mask & current_pin_mask) {
            offset_len2 = pin << 1;

            if (__VSF_GPIO_AF == cfgr) {
                if (pin < 8) {
                    offset_len4 = pin << 2;
                    vsf_atom32_op(&reg->muxl, (_ & ~(15 << offset_len4)) | (af_value << offset_len4));
                } else {
                    offset_len4 = (pin - 8) << 2;
                    vsf_atom32_op(&reg->muxh, (_ & ~(15 << offset_len4)) | (af_value << offset_len4));
                }
            } else if (exti) {
                uint32_t offset = ((pin & 3) << 2);
                vsf_atom32_op(&((&SCFG->exintc1)[pin >> 2]),
                    (_ & ~(0xF << offset)) | (gpio_ptr->idx << offset)
                );
            }
            vsf_atom32_op(&reg->pull, (_ & ~(3 << offset_len2)) | (pull << offset_len2));
            vsf_atom32_op(&reg->cfgr, (_ & ~(3 << offset_len2)) | (cfgr << offset_len2));
            vsf_atom32_op(&reg->omode, (_ & ~(1 << pin)) | (omode << pin));
            vsf_atom32_op(&reg->odrvr, (_ & ~(3 << offset_len2)) | (odrvr << offset_len2));
            vsf_atom32_op(&reg->hdrv, (_ & ~(3 << pin)) | (hdrv << pin));
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
    VSF_HAL_ASSERT(!(pin_mask & ~gpio_ptr->pin_mask));

    gpio_type *reg = gpio_ptr->reg;
    uint32_t offset_len2, cfgr, current_pin_mask;

    for (int i = 0; i < VSF_HW_GPIO_PIN_COUNT; i++) {
        current_pin_mask = 1 << i;
        if (pin_mask & current_pin_mask) {
            cfgr = (direction_mask >> i) & 1;
            offset_len2 = i << 1;

            vsf_atom32_op(&reg->cfgr, (_ & ~(3 << offset_len2)) | (cfgr << offset_len2));
        }
    }
}

vsf_gpio_pin_mask_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_get_direction)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(!(pin_mask & ~gpio_ptr->pin_mask));

    uint32_t cfgr = gpio_ptr->reg->cfgr, current_pin_mask;
    vsf_gpio_pin_mask_t direction = 0;

    for (int i = 0; i < VSF_HW_GPIO_PIN_COUNT; i++, cfgr >>= 2) {
        current_pin_mask = 1 << i;
        if (pin_mask & current_pin_mask) {
            if ((cfgr & 3) == 1) {
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
    return gpio_ptr->reg->idt;
}

vsf_gpio_pin_mask_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_read_output_register)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    return (vsf_gpio_pin_mask_t)gpio_ptr->reg->odt;
}

void VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_write)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask,
    vsf_gpio_pin_mask_t value
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(!(pin_mask & ~gpio_ptr->pin_mask));
    gpio_ptr->reg->scr = (value & pin_mask) | ((~value & pin_mask) << 16);
}

void VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_toggle)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(!(pin_mask & ~gpio_ptr->pin_mask));
    gpio_ptr->reg->togr = pin_mask;
}

vsf_err_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_exti_irq_enable)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_exti_irq_disable)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(!(pin_mask & ~gpio_ptr->pin_mask));
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_exti_irq_config)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr,
    vsf_gpio_exti_irq_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    return VSF_ERR_NOT_SUPPORT;
}

/*\note Implementation of APIs below is optional, because there is default implementation in gpio_template.inc.
 *      VSF_GPIO_CFG_REIMPLEMENT_API_XXXX can be defined to ENABLED to re-write the default implementation for better performance.
 *
 *      The list of APIs and configuration:
 *      VSF_GPIO_CFG_REIMPLEMENT_API_GET_PIN_CONFIGURATION for gpio_get_pin_configuration.
 *          Default implementation will assert(false) to indicate the feature is not implemented.
 *          Hardware drivers should implement this to read actual pin configuration from registers.
 *      VSF_GPIO_CFG_REIMPLEMENT_API_EXTI_IRQ_GET_CONFIGURATION for gpio_exti_irq_get_configuration.
 *          Default implementation will assert(false) to indicate the feature is not implemented.
 *          Hardware drivers should implement this to read actual exti irq configuration from registers.
 *      VSF_GPIO_CFG_REIMPLEMENT_API_READ_OUTPUT_REGISTER for gpio_read_output_register.
 *          Default implementation will assert(false) to indicate the feature is not implemented.
 *          Hardware drivers should implement this to read output register values (ODR).
 *          This is different from gpio_read which reads actual pin levels (IDR).
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
 *              VSF_GPIO_CFG_CAPABILITY_CAN_READ_IN_GPIO_OUTPUT_MODE
 *              VSF_GPIO_CFG_CAPABILITY_CAN_READ_IN_ALTERNATE_MODE
 *              VSF_GPIO_CFG_CAPABILITY_PIN_COUNT
 *              VSF_GPIO_CFG_CAPABILITY_PIN_MASK
 */

vsf_gpio_capability_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_capability)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *gpio_ptr
) {
    return (vsf_gpio_capability_t){
        .is_async                       = false,
        .support_output_and_set         = true,
        .support_output_and_clear       = true,
        .support_interrupt              = true,
        .can_read_in_gpio_output_mode   = true,
        .can_read_in_alternate_mode     = true,
        .pin_count                      = VSF_HW_GPIO_PIN_COUNT,
        .pin_mask                       = gpio_ptr->pin_mask,
    };
}

/*============================ INCLUDES ======================================*/

#define VSF_GPIO_CFG_REIMPLEMENT_API_READ_OUTPUT_REGISTER   ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_CAPABILITY             ENABLED
#define VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_CLEAR    ENABLED
#   define VSF_GPIO_CFG_CHANGE_DIR_FIRST                    DISABLED

#define VSF_GPIO_CFG_IMP_LV0(__IDX, __HAL_OP)                                   \
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t)                              \
        VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio, __IDX) = {                 \
        .reg = VSF_MCONNECT(VSF_GPIO_CFG_IMP_UPCASE_PREFIX, _GPIO_PORT, __IDX, _REG),\
        .pin_mask = VSF_MCONNECT(VSF_GPIO_CFG_IMP_UPCASE_PREFIX, _GPIO_PORT, __IDX, _MASK),\
        .idx = __IDX,                                                           \
        __HAL_OP                                                                \
    };

#include "hal/driver/common/gpio/gpio_template.inc"

#endif      // VSF_HAL_USE_GPIO
