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

#include "../driver.h"

#if VSF_HAL_USE_GPIO == ENABLED

#include "hal/vsf_hal.h"
#include "./i_reg_gpio.h"
#include "../vendor/plf/aic8800/src/driver/iomux/reg_iomux.h"
#include "../vendor/plf/aic8800/src/driver/aic1000lite_regs/aic1000Lite_iomux.h"
#include "../vendor/plf/aic8800/src/driver/pmic/pmic_api.h"

#define VSF_GPIO_CFG_REIMPLEMENT_API_CAPABILITY             ENABLED
#define VSF_GPIO_CFG_CHANGE_DIR_FIRST                       ENABLED
#define VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_CLEAR    1
#define VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_SET      1

/*============================ MACROS ========================================*/

#ifndef VSF_HW_GPIO_CFG_MULTI_CLASS
#   define VSF_HW_GPIO_CFG_MULTI_CLASS      VSF_GPIO_CFG_MULTI_CLASS
#endif

#ifndef VSF_HW_EXTI_GPIO_CFG_MULTI_CLASS
#   define VSF_HW_EXTI_GPIO_CFG_MULTI_CLASS VSF_GPIO_CFG_MULTI_CLASS
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#ifndef VSF_HW_GPIO_CFG_PROTECT_LEVEL
#   define VSF_HW_GPIO_CFG_PROTECT_LEVEL    interrupt
#endif

#define __vsf_gpio_protect                  vsf_protect(VSF_HW_GPIO_CFG_PROTECT_LEVEL)
#define __vsf_gpio_unprotect                vsf_unprotect(VSF_HW_GPIO_CFG_PROTECT_LEVEL)

#define __AIC8800_GPIO_IS_VAILID_PIN(__P)                                       \
    (((__P &  VSF_HW_GPIO_PIN_MASK) != 0) &&                                    \
     ((__P & ~VSF_HW_GPIO_PIN_MASK) == 0))

#define __AIC8800_GPIO_FUNCTION_MASK      IOMUX_GPIO_CONFIG_SEL_MASK

/*============================ TYPES =========================================*/

typedef struct vsf_hw_gpio_t {
#if VSF_HW_GPIO_CFG_MULTI_CLASS == ENABLED
    vsf_gpio_t vsf_gpio;
#endif

    GPIO_REG_T        *GPIO;
    AIC_IOMUX_TypeDef *IOMUX;
    uint16_t           output_reg;
    uint8_t            is_pmic;
    IRQn_Type          irqn;
    uint16_t gpio_pin_mask; // in pmic port, must set bit to 1 in MR register
    uint16_t gpio_pin_isr_mask;
    const uint8_t *funs;

    struct {
        vsf_gpio_exti_isr_handler_t *handler_fn;
        void                        *target_ptr;
    } exti;
} vsf_hw_gpio_t;

/*============================ IMPLEMENTATION ================================*/

static uint32_t __gpio_reg_read(bool is_pmic, volatile uint32_t *reg)
{
    if (!is_pmic) {
        return *reg;
    } else {
        return PMIC_MEM_READ((unsigned int)reg);
    }
}

static void __gpio_reg_mask_write(bool is_pmic, volatile uint32_t *reg,
                                  uint32_t wdata, uint32_t wmask)
{
    if (!is_pmic) {
        *reg = (*reg & ~wmask) | (wdata & wmask);
    } else {
        PMIC_MEM_MASK_WRITE((unsigned int)reg, wdata, wmask);
    }
}

static void __aic8800_gpio_init(vsf_hw_gpio_t *hw_gpio_ptr)
{
    // When the PMIC is used to access the GPIO,
    // critical area protection is already provided,
    // so there is no need to use MR registers for protection.
    // And the MR register should be set to 1 correctly when the IO is a GPIO.

    // This API should be used at initialization time
    if (hw_gpio_ptr->is_pmic && (hw_gpio_ptr->gpio_pin_mask != 0)) {
        PMIC_MEM_MASK_WRITE((unsigned int)&hw_gpio_ptr->GPIO->MR,
                            hw_gpio_ptr->gpio_pin_mask, hw_gpio_ptr->gpio_pin_mask);
    }
}

void __vsf_hw_aic8800_gpio_init(void)
{
    for (int i = 0; i < dimof(vsf_hw_gpios); i++) {
        __aic8800_gpio_init(vsf_hw_gpios[i]);
    }
}

vsf_err_t vsf_hw_gpio_port_config_pins(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    vsf_gpio_mode_t mode = cfg_ptr->mode;
    VSF_HAL_ASSERT(__AIC8800_GPIO_IS_VAILID_PIN(pin_mask));
    const uint8_t * funs = hw_gpio_ptr->funs;

    vsf_gpio_mode_t base_mode = mode & VSF_GPIO_MODE_MASK;
    vsf_gpio_mode_t exti_mode = mode & VSF_GPIO_EXTI_MODE_MASK;

    uint32_t wmask = __AIC8800_GPIO_MODE_ALL_BITS | __AIC8800_GPIO_FUNCTION_MASK;
    uint32_t wdata_mode = cfg_ptr->mode;
    uint32_t wdata_af;

    for (int i = 0; i < VSF_HW_GPIO_PIN_COUNT; i++) {
        uint32_t current_pin_mask = 1 << i;
        if (pin_mask & current_pin_mask) {
            volatile uint32_t *reg = &hw_gpio_ptr->IOMUX->GPCFG[i];
            bool need_set;
            if (base_mode == VSF_GPIO_AF) {
                wdata_af = cfg_ptr->alternate_function;
                need_set = (cfg_ptr->alternate_function == funs[i]);
            } else {
                wdata_af = funs[i];
                need_set = true;
            }
            __gpio_reg_mask_write(hw_gpio_ptr->is_pmic, reg, wdata_mode | wdata_af, wmask);
            if (hw_gpio_ptr->is_pmic && need_set) {
                // MR register need to be set 1 when pmic port configured as GPIO
                PMIC_MEM_MASK_WRITE((unsigned int)&hw_gpio_ptr->GPIO->MR, current_pin_mask, current_pin_mask);
            }
        }
    }

    if (base_mode == VSF_GPIO_EXTI) {
        if (hw_gpio_ptr->is_pmic) {
            return VSF_ERR_NOT_SUPPORT;
        }

        if (exti_mode == VSF_GPIO_EXTI_MODE_NONE) {
            hw_gpio_ptr->GPIO->TIR &= ~pin_mask;
            hw_gpio_ptr->GPIO->ICR &= ~pin_mask;
            return VSF_ERR_NONE;
        } else {
            hw_gpio_ptr->GPIO->DR  &= ~pin_mask;
            hw_gpio_ptr->GPIO->TIR |=  pin_mask;

            switch (exti_mode) {
            case VSF_GPIO_EXTI_MODE_LOW_LEVEL:
                hw_gpio_ptr->GPIO->TELR |=  pin_mask;
                hw_gpio_ptr->GPIO->TER  &= ~pin_mask;
                hw_gpio_ptr->GPIO->TLR  &= ~pin_mask;
                break;

            case VSF_GPIO_EXTI_MODE_HIGH_LEVEL:
                hw_gpio_ptr->GPIO->TELR |=  pin_mask;
                hw_gpio_ptr->GPIO->TER  &= ~pin_mask;
                hw_gpio_ptr->GPIO->TLR  |=  pin_mask;
                break;

            case VSF_GPIO_EXTI_MODE_RISING:
                hw_gpio_ptr->GPIO->TELR &= ~pin_mask;
                hw_gpio_ptr->GPIO->TER  &= ~pin_mask;
                hw_gpio_ptr->GPIO->TLR  |=  pin_mask;
                break;

            case VSF_GPIO_EXTI_MODE_FALLING:
                hw_gpio_ptr->GPIO->TELR &= ~pin_mask;
                hw_gpio_ptr->GPIO->TER  &= ~pin_mask;
                hw_gpio_ptr->GPIO->TLR  &= ~pin_mask;
                break;

            case VSF_GPIO_EXTI_MODE_RISING_FALLING:
                hw_gpio_ptr->GPIO->TELR &= ~pin_mask;
                hw_gpio_ptr->GPIO->TER  |= pin_mask;
                break;

            default:
                VSF_HAL_ASSERT(0);
                break;
            }

            uint32_t fr_mask = 0;
            for (int i = 0; i < VSF_HW_GPIO_PIN_COUNT / 4; i++) {
                if (pin_mask & (0x03 << i)) {
                    fr_mask |= (0x07 << i);
                }
            }
            hw_gpio_ptr->GPIO->FR  |= fr_mask;
            hw_gpio_ptr->GPIO->ICR |= pin_mask;
        }
    } else if ((base_mode == VSF_GPIO_INPUT) || (base_mode == VSF_GPIO_EXTI)) {
        __gpio_reg_mask_write(hw_gpio_ptr->is_pmic, &hw_gpio_ptr->GPIO->DR, 0, pin_mask);
    } else if ((base_mode == VSF_GPIO_OUTPUT_PUSH_PULL) || (base_mode == VSF_GPIO_OUTPUT_OPEN_DRAIN)) {
        __gpio_reg_mask_write(hw_gpio_ptr->is_pmic, &hw_gpio_ptr->GPIO->DR, pin_mask, pin_mask);
    }

    return VSF_ERR_NONE;
}

void vsf_hw_gpio_set_direction(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_pin_mask_t direction_mask)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    VSF_HAL_ASSERT(__AIC8800_GPIO_IS_VAILID_PIN(pin_mask));
    // GPIOB0 can only be used in the input direction
    VSF_HAL_ASSERT(   (hw_gpio_ptr->is_pmic != 1)
                   || ((pin_mask & VSF_PIN0_MASK) != VSF_PIN0_MASK)
                   || ((direction_mask & VSF_PIN0_MASK) != VSF_PIN0_MASK));

    __gpio_reg_mask_write(hw_gpio_ptr->is_pmic, &hw_gpio_ptr->GPIO->DR, direction_mask, pin_mask);
}

vsf_gpio_pin_mask_t vsf_hw_gpio_get_direction(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    VSF_HAL_ASSERT(__AIC8800_GPIO_IS_VAILID_PIN(pin_mask));

    return __gpio_reg_read(hw_gpio_ptr->is_pmic, &hw_gpio_ptr->GPIO->DR) & pin_mask;
}

vsf_gpio_pin_mask_t vsf_hw_gpio_read(vsf_hw_gpio_t *hw_gpio_ptr)
{
    uint32_t origin_pin_mask, pin_value;
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);

    if (!hw_gpio_ptr->is_pmic) {
        origin_pin_mask = hw_gpio_ptr->GPIO->MR;
        hw_gpio_ptr->GPIO->MR = VSF_HW_GPIO_PIN_MASK;
        pin_value = hw_gpio_ptr->GPIO->VR;
        hw_gpio_ptr->GPIO->MR = origin_pin_mask;
    } else {
        pin_value = PMIC_MEM_READ((unsigned int)&hw_gpio_ptr->GPIO->VR);
    }

    return pin_value;
}

void vsf_hw_gpio_write(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_pin_mask_t value)
{
    uint32_t origin_pin_mask;

    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    VSF_HAL_ASSERT(__AIC8800_GPIO_IS_VAILID_PIN(pin_mask));

    if (!hw_gpio_ptr->is_pmic) {
        origin_pin_mask = hw_gpio_ptr->GPIO->MR;
        hw_gpio_ptr->GPIO->MR = pin_mask;
        hw_gpio_ptr->GPIO->VR = value;
        hw_gpio_ptr->GPIO->MR = origin_pin_mask;
    } else {
        PMIC_MEM_MASK_WRITE((unsigned int)&hw_gpio_ptr->GPIO->VR, value , pin_mask);
    }

    vsf_protect_t orig = __vsf_gpio_protect();
    hw_gpio_ptr->output_reg = (hw_gpio_ptr->output_reg & ~pin_mask) | (value & pin_mask);
    __vsf_gpio_unprotect(orig);
}

void vsf_hw_gpio_toggle(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);

    vsf_hw_gpio_write(hw_gpio_ptr, pin_mask, ~hw_gpio_ptr->output_reg);
}

vsf_gpio_capability_t vsf_hw_gpio_capability(vsf_hw_gpio_t *hw_gpio_ptr)
{
    vsf_gpio_capability_t gpio_capability = {
        .is_async = hw_gpio_ptr->is_pmic,
        .support_output_and_set = VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_SET,
        .support_output_and_clear = VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_CLEAR,
        .support_interrupt = 1,
        .pin_count = VSF_HW_GPIO_PIN_COUNT,
        .pin_mask = VSF_HW_GPIO_PIN_MASK,
    };

    return gpio_capability;
}

vsf_err_t vsf_hw_gpio_exti_irq_config(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_exti_irq_cfg_t *irq_cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);

    if (hw_gpio_ptr->is_pmic) {
        return VSF_ERR_NOT_SUPPORT;
    }

    NVIC_SetPriority(hw_gpio_ptr->irqn, irq_cfg_ptr->prio);
    hw_gpio_ptr->exti.handler_fn = irq_cfg_ptr->handler_fn;
    hw_gpio_ptr->exti.target_ptr = irq_cfg_ptr->target_ptr;

    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_gpio_exti_irq_enable(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    uint16_t pin_mask_orig;
    vsf_protect_t orig;

    if (hw_gpio_ptr->is_pmic) {
        return VSF_ERR_NOT_SUPPORT;
    }

    orig = __vsf_gpio_protect();
    pin_mask_orig = hw_gpio_ptr->gpio_pin_isr_mask;
    hw_gpio_ptr->gpio_pin_isr_mask |= pin_mask;
    __vsf_gpio_unprotect(orig);

    if (!pin_mask_orig) {
        NVIC_EnableIRQ(hw_gpio_ptr->irqn);
    }


    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_gpio_exti_irq_disable(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    uint16_t pin_mask_orig;
    vsf_protect_t orig;

    if (hw_gpio_ptr->is_pmic) {
        return VSF_ERR_NOT_SUPPORT;
    }

    orig = __vsf_gpio_protect();
    hw_gpio_ptr->gpio_pin_isr_mask &= ~pin_mask;
    pin_mask_orig = hw_gpio_ptr->gpio_pin_isr_mask;
    __vsf_gpio_unprotect(orig);

    if (!pin_mask_orig) {
        NVIC_DisableIRQ(hw_gpio_ptr->irqn);
    }

    return VSF_ERR_NONE;
}

void __vsf_hw_gpio_irq_handler(vsf_hw_gpio_t *hw_gpio_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    GPIO_REG_T *GPIOx = hw_gpio_ptr->GPIO;
    VSF_HAL_ASSERT(NULL != GPIOx);

    uint32_t irq_pin_mask = GPIOx->ISR;
    GPIOx->IRR = irq_pin_mask;

    //copy from gpio_api.c
    // I don't know why the ISB is called here.
    __ISB();

    if (hw_gpio_ptr->exti.handler_fn != NULL) {
        hw_gpio_ptr->exti.handler_fn(hw_gpio_ptr->exti.target_ptr,
                                     (vsf_gpio_t *)hw_gpio_ptr, irq_pin_mask);
    }
}

/*============================ INCLUDES ======================================*/

#define VSF_GPIO_CFG_IMP_PREFIX                 vsf_hw
#define VSF_GPIO_CFG_IMP_UPCASE_PREFIX          VSF_HW
#define VSF_GPIO_CFG_IMP_LV0(__IDX, __HAL_OP)                                   \
    static const uint8_t __vsf_hw_gpio ## __IDX ## funs[] =                     \
        VSF_HW_GPIO ## __IDX ## _MAP;                                           \
    vsf_hw_gpio_t vsf_hw_gpio ## __IDX = {                                      \
        .GPIO = (GPIO_REG_T *)VSF_HW_GPIO ## __IDX ## _BASE_ADDRESS,            \
        .IOMUX = ((AIC_IOMUX_TypeDef *)VSF_HW_GPIO ## __IDX ## _IOMUX_REG_BASE),\
        .irqn = VSF_HW_GPIO ## __IDX ## _IRQ_IDX,                               \
        .is_pmic = VSF_HW_GPIO ## __IDX ## _IS_PMIC,                            \
        .gpio_pin_mask = VSF_HW_GPIO ## __IDX ## _GPIO_PIN_MASK,                \
        .funs = __vsf_hw_gpio ## __IDX ## funs,                                 \
        __HAL_OP                                                                \
    };                                                                          \
    void VSF_HW_GPIO ## __IDX ## _IRQ(void)                                     \
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        __vsf_hw_gpio_irq_handler(&vsf_hw_gpio ## __IDX);                       \
        vsf_hal_irq_leave(ctx);                                                 \
    }

#include "hal/driver/common/gpio/gpio_template.inc"

#endif      // VSF_HAL_USE_GPIO
