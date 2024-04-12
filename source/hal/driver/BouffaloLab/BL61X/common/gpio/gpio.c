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

#define VSF_GPIO_CFG_IMP_PREFIX                 vsf_hw
#define VSF_GPIO_CFG_IMP_UPCASE_PREFIX          VSF_HW

/*============================ INCLUDES ======================================*/

#include "./gpio.h"

#if VSF_HAL_USE_GPIO == ENABLED

#include "bflb_gpio.h"
#include "hardware/gpio_reg.h"

#define VSF_GPIO_CFG_REIMPLEMENT_API_CAPABILITY         ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_SET                ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_CLEAR              ENABLED
#define VSF_GPIO_CFG_CAPABILITY_HAS_OUTPUT_AND_SET      1
#define VSF_GPIO_CFG_CAPABILITY_HAS_OUTPUT_AND_CLEAR    1

/*============================ MACROS ========================================*/

#ifndef VSF_HW_GPIO_CFG_MULTI_CLASS
#   define VSF_HW_GPIO_CFG_MULTI_CLASS      VSF_GPIO_CFG_MULTI_CLASS
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#ifndef getreg64
#   define getreg64(a)                      (*(volatile uint64_t *)(a))
#endif
#ifndef putreg64
#   define putreg64(v, a)                   (*(volatile uint64_t *)(a) = (v))
#endif

/*============================ PROTOTYPES ====================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_hw_gpio_t {
#if VSF_HW_GPIO_CFG_MULTI_CLASS == ENABLED
    vsf_gpio_t vsf_gpio;
#endif

    const char *dev_name;
    struct bflb_device_s *dev;
    vsf_gpio_pin_mask_t direction;
} vsf_hw_gpio_t;

/*============================ IMPLEMENTATION ================================*/

void __vsf_hw_bl61x_gpio_init(void)
{
    for (int i = 0; i < dimof(vsf_hw_gpios); i++) {
        vsf_hw_gpios[i]->dev = bflb_device_get_by_name(vsf_hw_gpios[i]->dev_name);
    }
}

void vsf_hw_gpio_config_pin(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_mode_t feature)
{
    VSF_HAL_ASSERT((NULL != hw_gpio_ptr) && (hw_gpio_ptr->dev != NULL));
    VSF_HAL_ASSERT(__BL61X_IO_IS_VAILID_PIN(pin_mask));
    VSF_HAL_ASSERT(__BL61X_IO_IS_VAILID_FEATURE(feature));

    for (int i = 0; i < VSF_HW_IO_PIN_COUNT; i++) {
        if (pin_mask & (1 << i)) {
            bflb_gpio_init(hw_gpio_ptr->dev, i, (uint32_t)feature);
        }
    }
}

void vsf_hw_gpio_set_direction(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_pin_mask_t direction_mask)
{
    VSF_HAL_ASSERT((NULL != hw_gpio_ptr) && (hw_gpio_ptr->dev != NULL));
    VSF_HAL_ASSERT(__BL61X_IO_IS_VAILID_PIN(pin_mask));

    vsf_gpio_pin_mask_t mask = pin_mask & direction_mask;
    if (mask) {
        vsf_hw_gpio_config_pin(hw_gpio_ptr, mask, GPIO_OUTPUT);
        hw_gpio_ptr->direction |= mask;
    }
    mask = pin_mask & ~direction_mask;
    if (mask) {
        vsf_hw_gpio_config_pin(hw_gpio_ptr, mask, GPIO_OUTPUT);
        hw_gpio_ptr->direction &= ~mask;
    }
}

vsf_gpio_pin_mask_t vsf_hw_gpio_get_direction(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT((NULL != hw_gpio_ptr) && (hw_gpio_ptr->dev != NULL));
    VSF_HAL_ASSERT(__BL61X_IO_IS_VAILID_PIN(pin_mask));

    return hw_gpio_ptr->direction;
}

void vsf_hw_gpio_set(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT((NULL != hw_gpio_ptr) && (hw_gpio_ptr->dev != NULL));
    VSF_HAL_ASSERT(__BL61X_IO_IS_VAILID_PIN(pin_mask));

#if VSF_HW_IO_PIN_COUNT > 32
    putreg64(pin_mask, hw_gpio_ptr->dev->reg_base + GLB_GPIO_CFG138_OFFSET);
#else
    putreg32(pin_mask, hw_gpio_ptr->dev->reg_base + GLB_GPIO_CFG138_OFFSET);
#endif
}

void vsf_hw_gpio_clear(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT((NULL != hw_gpio_ptr) && (hw_gpio_ptr->dev != NULL));
    VSF_HAL_ASSERT(__BL61X_IO_IS_VAILID_PIN(pin_mask));

#if VSF_HW_IO_PIN_COUNT > 32
    putreg64(pin_mask, hw_gpio_ptr->dev->reg_base + GLB_GPIO_CFG140_OFFSET);
#else
    putreg32(pin_mask, hw_gpio_ptr->dev->reg_base + GLB_GPIO_CFG140_OFFSET);
#endif
}

vsf_gpio_pin_mask_t vsf_hw_gpio_read(vsf_hw_gpio_t *hw_gpio_ptr)
{
    VSF_HAL_ASSERT((NULL != hw_gpio_ptr) && (hw_gpio_ptr->dev != NULL));

#if VSF_HW_IO_PIN_COUNT > 32
    return getreg64(hw_gpio_ptr->dev->reg_base + GLB_GPIO_CFG128_OFFSET);
#else
    return getreg32(hw_gpio_ptr->dev->reg_base + GLB_GPIO_CFG128_OFFSET);
#endif
}

void vsf_hw_gpio_write(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_pin_mask_t value)
{
    VSF_HAL_ASSERT((NULL != hw_gpio_ptr) && (hw_gpio_ptr->dev != NULL));
    VSF_HAL_ASSERT(__BL61X_IO_IS_VAILID_PIN(pin_mask));

    vsf_gpio_pin_mask_t mask = pin_mask & value;
    if (mask) {
        vsf_hw_gpio_set(hw_gpio_ptr, mask);
    }
    mask = pin_mask & ~value;
    if (mask) {
        vsf_hw_gpio_clear(hw_gpio_ptr, mask);
    }
}

void vsf_hw_gpio_toggle(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    vsf_hw_gpio_write(hw_gpio_ptr, pin_mask, vsf_hw_gpio_read(hw_gpio_ptr));
}

vsf_gpio_capability_t vsf_hw_gpio_capability(vsf_hw_gpio_t *hw_gpio_ptr)
{
    vsf_gpio_capability_t gpio_capability = {
        .support_output_and_set = VSF_GPIO_CFG_CAPABILITY_HAS_OUTPUT_AND_SET,
        .support_output_and_clear = VSF_GPIO_CFG_CAPABILITY_HAS_OUTPUT_AND_CLEAR,
        .pin_count = VSF_HW_GPIO_PIN_COUNT,
        .pin_mask = VSF_HW_GPIO_PIN_MASK,
    };

    return gpio_capability;
}

/*============================ INCLUDES ======================================*/

#define VSF_GPIO_CFG_REIMPLEMENT_API_CAPABILITY     ENABLED
#define VSF_GPIO_CFG_IMP_LV0(__IDX, __HAL_OP)                                   \
    vsf_hw_gpio_t vsf_hw_gpio ## __IDX = {                                      \
        .dev_name       = VSF_HW_IO_PORT ## __IDX ## _DEVNAME,                  \
        __HAL_OP                                                                \
    };

#include "hal/driver/common/gpio/gpio_template.inc"

#endif      // VSF_HAL_USE_GPIO
