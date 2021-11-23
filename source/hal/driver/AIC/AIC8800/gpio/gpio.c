/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

#include "./gpio.h"
#if VSF_HAL_USE_GPIO == ENABLED
#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/aic1000lite_regs/aic1000Lite_iomux.h"
#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/pmic/pmic_api.h"

/*============================ MACROS ========================================*/

#ifndef REG_GPIO0_IOMUX
#   define REG_GPIO0_IOMUX                         (0X40503000)
#endif

#define REG_GPIO0_PIN_SEL { \
        1, 1, 0, 0, 0, 0, 0, 0, \
        1, 1, 0, 0, 0, 0, 0, 0, \
    }

#ifndef REG_GPIO1_IOMUX
#   define REG_GPIO1_IOMUX                         (0x50012000)
#endif

#define REG_GPIO1_PIN_SEL { \
        1, 1, 0, 0, 0, 0, 0, 0, \
        0, 0, 0, 0, 0, 0, 0, 0, \
    }

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __VSF_HW_GPIO_PIN_MASK  \
    ((1ul << VSF_HAL_GPIO_PIN_MAX) - 1)

#define __VSF_HW_GPIO_IPM_LV0(__COUNT, IS_PMIC)                                 \
    vsf_gpio_t vsf_gpio##__COUNT = {                                            \
        .GPIO = REG_GPIO##__COUNT,                                              \
        .IOMUX = ((AIC_IOMUX_TypeDef *)REG_GPIO##__COUNT##_IOMUX),              \
        .is_pmic = IS_PMIC,                                                     \
        .pin_sel = REG_GPIO##__COUNT##_PIN_SEL                                  \
    };

#define __VSF_HW_GPIO_IS_VAILID_PIN(__P)                                        \
    (((__P &  __VSF_HW_GPIO_PIN_MASK) != 0) &&                                  \
     ((__P & ~__VSF_HW_GPIO_PIN_MASK) == 0))

#define __VSF_HW_GPIO_IS_VAILID_FEATURE(__F)                                    \
    ((__F & ~(uint32_t)__IO_FEATURE_MASK) == 0)


/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

__VSF_HW_GPIO_IPM_LV0(0, false)
__VSF_HW_GPIO_IPM_LV0(1, true)

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static bool __gpio_is_pmic_mem(vsf_gpio_t *gpio_ptr)
{
    return gpio_ptr->is_pmic;
}

static uint32_t __gpio_reg_read(vsf_gpio_t *gpio_ptr, volatile uint32_t *reg)
{
    if (!__gpio_is_pmic_mem(gpio_ptr)) {
        return *reg;
    } else {
        return PMIC_MEM_READ((unsigned int)reg);
    }
}

static void __gpio_reg_mask_write(vsf_gpio_t *gpio_ptr,
                                  volatile uint32_t *reg,
                                  uint32_t wdata, uint32_t wmask)
{
    if (!__gpio_is_pmic_mem(gpio_ptr)) {
        *reg = (*reg & wmask) | wdata;
    } else {
        PMIC_MEM_MASK_WRITE((unsigned int)reg, wdata, wmask);
    }
}

void vsf_gpio_config_pin(vsf_gpio_t *gpio_ptr, uint32_t pin_mask, uint32_t feature)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(__VSF_HW_GPIO_IS_VAILID_PIN(pin_mask));
    VSF_HAL_ASSERT(__VSF_HW_GPIO_IS_VAILID_FEATURE(feature));

    for (int i = 0; i < VSF_HAL_GPIO_PIN_MAX; i++) {
        if (pin_mask & (1 << i)) {
            uint32_t wdata = feature | gpio_ptr->pin_sel[i];
            // TODO: move to IO
            __gpio_reg_mask_write(gpio_ptr, &gpio_ptr->IOMUX->GPCFG[i],
                                  wdata, __IO_FEATURE_MASK);
        }
    }
}

void vsf_gpio_set_direction(vsf_gpio_t *gpio_ptr, uint32_t direction_mask, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(__VSF_HW_GPIO_IS_VAILID_PIN(pin_mask));

    __gpio_reg_mask_write(gpio_ptr, &gpio_ptr->GPIO->DR, direction_mask, pin_mask);
}

uint32_t vsf_gpio_get_direction(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(__VSF_HW_GPIO_IS_VAILID_PIN(pin_mask));

    return __gpio_reg_read(gpio_ptr, &gpio_ptr->GPIO->DR) & pin_mask;
}

void vsf_gpio_set_input(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(__VSF_HW_GPIO_IS_VAILID_PIN(pin_mask));

    vsf_gpio_set_direction(gpio_ptr, (uint32_t )0, pin_mask);
}

void vsf_gpio_set_output(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(__VSF_HW_GPIO_IS_VAILID_PIN(pin_mask));

    vsf_gpio_set_direction(gpio_ptr, ~(uint32_t )0, pin_mask);
}

void vsf_gpio_switch_direction(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(__VSF_HW_GPIO_IS_VAILID_PIN(pin_mask));

    uint32_t ret = ~vsf_gpio_get_direction(gpio_ptr, pin_mask);
    vsf_gpio_set_direction(gpio_ptr, ret, pin_mask);
}

uint32_t vsf_gpio_read(vsf_gpio_t *gpio_ptr)
{
    uint32_t temp_value, temp_ret;

    VSF_HAL_ASSERT(NULL != gpio_ptr);

    temp_value = __gpio_reg_read(gpio_ptr, &gpio_ptr->GPIO->MR);
    __gpio_reg_mask_write(gpio_ptr, &gpio_ptr->GPIO->MR, ~(uint32_t)0, ~(uint32_t)0);
    temp_ret = __gpio_reg_read(gpio_ptr, &gpio_ptr->GPIO->VR);
    __gpio_reg_mask_write(gpio_ptr, &gpio_ptr->GPIO->MR, temp_value, ~(uint32_t)0);

    return temp_ret;
}

void vsf_gpio_write(vsf_gpio_t *gpio_ptr, uint32_t value, uint32_t pin_mask)
{
    uint32_t temp_value;

    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(__VSF_HW_GPIO_IS_VAILID_PIN(pin_mask));

    temp_value = __gpio_reg_read(gpio_ptr, &gpio_ptr->GPIO->MR);
    __gpio_reg_mask_write(gpio_ptr, &gpio_ptr->GPIO->MR, ~(uint32_t)0, ~(uint32_t)0);
    __gpio_reg_mask_write(gpio_ptr, &gpio_ptr->GPIO->VR, value, pin_mask);
    __gpio_reg_mask_write(gpio_ptr, &gpio_ptr->GPIO->MR, temp_value, ~(uint32_t)0);
}

void vsf_gpio_set(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(__VSF_HW_GPIO_IS_VAILID_PIN(pin_mask));

    vsf_gpio_write(gpio_ptr, ~(uint32_t)0, pin_mask);
}

void vsf_gpio_clear(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(__VSF_HW_GPIO_IS_VAILID_PIN(pin_mask));

    vsf_gpio_write(gpio_ptr, (uint32_t)0, pin_mask);
}

void vsf_gpio_toggle(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(__VSF_HW_GPIO_IS_VAILID_PIN(pin_mask));

    uint32_t ret = ~vsf_gpio_read(gpio_ptr);
    vsf_gpio_write(gpio_ptr, ret, pin_mask);
}

#endif      // VSF_HAL_USE_GPIO
