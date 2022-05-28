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

#define VSF_GPIO_CFG_PREFIX                 vsf_hw
#define VSF_GPIO_CFG_UPPERCASE_PREFIX       VSF_HW

/*============================ INCLUDES ======================================*/

#include "./gpio.h"

#if VSF_HAL_USE_GPIO == ENABLED

#include "hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/aic1000lite_regs/aic1000Lite_iomux.h"

// TODO: fixed gpio output_and_set
#define VSF_GPIO_CFG_REIMPLEMENT_OUTPUT_AND_SET         DISABLED

/*============================ MACROS ========================================*/

#define __VSF_HW_GPIO_PIN_MASK                                                  \
    ((1ul << VSF_HW_GPIO_PIN_MAX) - 1)

#define __VSF_HW_GPIO_IS_VAILID_PIN(__P)                                        \
    (((__P &  __VSF_HW_GPIO_PIN_MASK) != 0) &&                                  \
     ((__P & ~__VSF_HW_GPIO_PIN_MASK) == 0))

#define __VSF_HW_GPIO_IS_VAILID_FEATURE(__F)                                    \
    ((__F & ~(uint32_t)__IO_FEATURE_MASK) == 0)


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#ifndef VSF_HW_GPIO_CFG_PROTECT_LEVEL
#   define VSF_HW_GPIO_CFG_PROTECT_LEVEL    interrupt
#endif

#define __vsf_gpio_protect                  vsf_protect(VSF_HW_GPIO_CFG_PROTECT_LEVEL)
#define __vsf_gpio_unprotect                vsf_unprotect(VSF_HW_GPIO_CFG_PROTECT_LEVEL)

/*============================ TYPES =========================================*/

typedef struct vsf_hw_gpio_t {
#if VSF_GPIO_CFG_IMPLEMENT_OP == ENABLED
    vsf_gpio_t vsf_gpio;
#endif

    GPIO_REG_T *GPIO;
    uint32_t output_reg;
    bool is_pmic;

    // TODO: move to IO module
    AIC_IOMUX_TypeDef *IOMUX;
    uint8_t pin_sel[VSF_HW_GPIO_PIN_MAX];
} vsf_hw_gpio_t;

/*============================ IMPLEMENTATION ================================*/

static bool __gpio_is_pmic_mem(vsf_hw_gpio_t *hw_gpio_ptr)
{
    return hw_gpio_ptr->is_pmic;
}

static uint32_t __gpio_reg_read(vsf_hw_gpio_t *hw_gpio_ptr, volatile uint32_t *reg)
{
    if (!__gpio_is_pmic_mem(hw_gpio_ptr)) {
        return *reg;
    } else {
        return PMIC_MEM_READ((unsigned int)reg);
    }
}

static void __gpio_reg_mask_write(vsf_hw_gpio_t *hw_gpio_ptr,
                                  volatile uint32_t *reg,
                                  uint32_t wdata, uint32_t wmask)
{
    if (!__gpio_is_pmic_mem(hw_gpio_ptr)) {
        *reg = (*reg & ~wmask) | (wdata & wmask);
    } else {
        PMIC_MEM_MASK_WRITE((unsigned int)reg, wdata, wmask);
    }
}

void vsf_hw_gpio_config_pin(vsf_hw_gpio_t *hw_gpio_ptr, uint32_t pin_mask, uint_fast32_t feature)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    VSF_HAL_ASSERT(__VSF_HW_GPIO_IS_VAILID_PIN(pin_mask));
    VSF_HAL_ASSERT(__VSF_HW_GPIO_IS_VAILID_FEATURE(feature));

    for (int i = 0; i < VSF_HW_GPIO_PIN_MAX; i++) {
        if (pin_mask & (1 << i)) {
            // TODO: move to IO module
            uint32_t wdata = feature | hw_gpio_ptr->pin_sel[i];
            __gpio_reg_mask_write(hw_gpio_ptr, &hw_gpio_ptr->IOMUX->GPCFG[i],
                                  wdata, __IO_FEATURE_MASK);
        }
    }
}

void vsf_hw_gpio_set_direction(vsf_hw_gpio_t *hw_gpio_ptr, uint32_t direction_mask, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    VSF_HAL_ASSERT(__VSF_HW_GPIO_IS_VAILID_PIN(pin_mask));

    __gpio_reg_mask_write(hw_gpio_ptr, &hw_gpio_ptr->GPIO->DR, direction_mask, pin_mask);
}

uint32_t vsf_hw_gpio_get_direction(vsf_hw_gpio_t *hw_gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    VSF_HAL_ASSERT(__VSF_HW_GPIO_IS_VAILID_PIN(pin_mask));

    return __gpio_reg_read(hw_gpio_ptr, &hw_gpio_ptr->GPIO->DR) & pin_mask;
}

uint32_t vsf_hw_gpio_read(vsf_hw_gpio_t *hw_gpio_ptr)
{
    uint32_t pin_mask, pin_value;
    uint32_t read_mask = ~(uint32_t)0;

    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);

    pin_mask = __gpio_reg_read(hw_gpio_ptr, &hw_gpio_ptr->GPIO->MR);

    __gpio_reg_mask_write(hw_gpio_ptr, &hw_gpio_ptr->GPIO->MR, read_mask, ~(uint32_t)0);
    pin_value = __gpio_reg_read(hw_gpio_ptr, &hw_gpio_ptr->GPIO->VR);
    __gpio_reg_mask_write(hw_gpio_ptr, &hw_gpio_ptr->GPIO->MR, pin_mask, ~(uint32_t)0);

    return pin_value;
}

void vsf_hw_gpio_write(vsf_hw_gpio_t *hw_gpio_ptr, uint32_t value, uint32_t pin_mask)
{
    uint32_t temp_value;

    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    VSF_HAL_ASSERT(__VSF_HW_GPIO_IS_VAILID_PIN(pin_mask));

    temp_value = __gpio_reg_read(hw_gpio_ptr, &hw_gpio_ptr->GPIO->MR);
    __gpio_reg_mask_write(hw_gpio_ptr, &hw_gpio_ptr->GPIO->MR, pin_mask, ~(uint32_t)0);
    __gpio_reg_mask_write(hw_gpio_ptr, &hw_gpio_ptr->GPIO->VR, value, pin_mask);
    __gpio_reg_mask_write(hw_gpio_ptr, &hw_gpio_ptr->GPIO->MR, temp_value, ~(uint32_t)0);

    vsf_protect_t orig = __vsf_gpio_protect();
    hw_gpio_ptr->output_reg &= ~pin_mask;
    hw_gpio_ptr->output_reg |= (value & pin_mask);
    __vsf_gpio_unprotect(orig);
}

void vsf_hw_gpio_toggle(vsf_hw_gpio_t *hw_gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);

    vsf_hw_gpio_write(hw_gpio_ptr, ~hw_gpio_ptr->output_reg, pin_mask);
}

#if VSF_GPIO_CFG_REIMPLEMENT_OUTPUT_AND_SET == ENABLED
void vsf_hw_gpio_output_and_set(vsf_hw_gpio_t *hw_gpio_ptr, uint32_t pin_mask)
{
    // TODO
    VSF_HAL_ASSERT(0);
}
#endif

/*============================ INCLUDES ======================================*/

#define VSF_GPIO_CFG_IMP_LV0(__COUNT, __HAL_OP)                                 \
    vsf_hw_gpio_t vsf_hw_gpio##__COUNT = {                                      \
        .GPIO = REG_GPIO##__COUNT,                                              \
        .IOMUX = ((AIC_IOMUX_TypeDef *)VSF_HW_GPIO##__COUNT##_IOMUX_REG_BASE),  \
        .is_pmic = VSF_HW_GPIO##__COUNT##_IS_PMIC,                              \
        .pin_sel = VSF_HW_GPIO##__COUNT##_PIN_SEL,                              \
        .output_reg = 0,                                                        \
        __HAL_OP                                                                \
    };

#include "hal/driver/common/io/io_template.inc"

#endif      // VSF_HAL_USE_GPIO
