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

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#ifndef VSF_HW_GPIO_CFG_PROTECT_LEVEL
#   define VSF_HW_GPIO_CFG_PROTECT_LEVEL    interrupt
#endif

#define __vsf_gpio_protect                  vsf_protect(VSF_HW_GPIO_CFG_PROTECT_LEVEL)
#define __vsf_gpio_unprotect                vsf_unprotect(VSF_HW_GPIO_CFG_PROTECT_LEVEL)

#define __AIC8800_IO_IS_VAILID_PIN(__P)                                         \
    (((__P &  VSF_HW_IO_PIN_MASK) != 0) &&                                      \
     ((__P & ~VSF_HW_IO_PIN_MASK) == 0))

/*============================ TYPES =========================================*/

typedef struct vsf_hw_gpio_t {
#if VSF_HW_GPIO_CFG_MULTI_CLASS == ENABLED
    vsf_gpio_t vsf_gpio;
#endif

    GPIO_REG_T *GPIO;
    AIC_IOMUX_TypeDef *IOMUX;
    vsf_gpio_isr_t *isrs;

    uint16_t output_reg;
    uint8_t is_pmic;
    IRQn_Type irqn;
    uint16_t gpio_pin_mask; // in pmic port, must set bit to 1 in MR register
    uint16_t gpio_pin_isr_mask;
} vsf_hw_gpio_t;

/*============================ IMPLEMENTATION ================================*/

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

void vsf_hw_gpio_config_pin(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_mode_t mode)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    VSF_HAL_ASSERT(__AIC8800_IO_IS_VAILID_PIN(pin_mask));
    mode = mode & __AIC8800_IO_MODE_ALL_BITS;

    for (int i = 0; i < VSF_HW_IO_PIN_COUNT; i++) {
        uint32_t current_pin_mask = 1 << i;
        if (pin_mask & current_pin_mask) {
            aic8800_io_reg_mask_write(hw_gpio_ptr->is_pmic, &hw_gpio_ptr->IOMUX->GPCFG[i],
                                  mode, __AIC8800_IO_MODE_ALL_BITS);
            if (hw_gpio_ptr->is_pmic) {
                PMIC_MEM_MASK_WRITE((unsigned int)&hw_gpio_ptr->GPIO->MR, current_pin_mask, current_pin_mask);
            }
        }
    }
}

void vsf_hw_gpio_set_direction(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_pin_mask_t direction_mask)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    VSF_HAL_ASSERT(__AIC8800_IO_IS_VAILID_PIN(pin_mask));
    // GPIOB0 can only be used in the input direction
    VSF_HAL_ASSERT(   (hw_gpio_ptr->is_pmic != 1)
                   || ((pin_mask & VSF_PIN0_MASK) != VSF_PIN0_MASK)
                   || ((direction_mask & VSF_PIN0_MASK) != VSF_PIN0_MASK));

    aic8800_io_reg_mask_write(hw_gpio_ptr->is_pmic, &hw_gpio_ptr->GPIO->DR, direction_mask, pin_mask);
}

vsf_gpio_pin_mask_t vsf_hw_gpio_get_direction(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    VSF_HAL_ASSERT(__AIC8800_IO_IS_VAILID_PIN(pin_mask));

    return aic8800_io_reg_read(hw_gpio_ptr->is_pmic, &hw_gpio_ptr->GPIO->DR) & pin_mask;
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
    VSF_HAL_ASSERT(__AIC8800_IO_IS_VAILID_PIN(pin_mask));

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

vsf_err_t vsf_hw_gpio_config_exti_interrupt(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_arch_prio_t prio)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    VSF_HAL_ASSERT(vsf_arch_prio_invalid != prio);

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
        NVIC_SetPriority(hw_gpio_ptr->irqn, prio);
        NVIC_EnableIRQ(hw_gpio_ptr->irqn);
    }

    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_gpio_pin_interrupt_disable(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
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


vsf_err_t vsf_hw_gpio_config_exti(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_irq_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    if (hw_gpio_ptr->is_pmic) {
        return VSF_ERR_NOT_SUPPORT;
    }
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr->isrs);

    uint32_t pin_mask = cfg_ptr->pin_mask;

    if (cfg_ptr->mode == VSF_GPIO_INT_MODE_NONE) {
        hw_gpio_ptr->GPIO->TIR &= ~pin_mask;
        hw_gpio_ptr->GPIO->ICR &= ~pin_mask;
        for (int i = 0; i < VSF_HW_IO_PIN_COUNT; i++) {
            if (pin_mask & (1 << i)) {
                hw_gpio_ptr->isrs[i].handler_fn = NULL;
                hw_gpio_ptr->isrs[i].target_ptr = NULL;
            }
        }
        return VSF_ERR_NONE;
    } else {
        for (int i = 0; i < VSF_HW_IO_PIN_COUNT; i++) {
            if (pin_mask & (1 << i)) {
                hw_gpio_ptr->isrs[i] = cfg_ptr->isr;
            }
        }

        hw_gpio_ptr->GPIO->DR  &= ~pin_mask;
        hw_gpio_ptr->GPIO->TIR |=  pin_mask;

        switch (cfg_ptr->mode) {
        case VSF_GPIO_INT_MODE_LOW_LEVEL:
            hw_gpio_ptr->GPIO->TELR |=  pin_mask;
            hw_gpio_ptr->GPIO->TER  &= ~pin_mask;
            hw_gpio_ptr->GPIO->TLR  &= ~pin_mask;
            break;

        case VSF_GPIO_INT_MODE_HIGH_LEVEL:
            hw_gpio_ptr->GPIO->TELR |=  pin_mask;
            hw_gpio_ptr->GPIO->TER  &= ~pin_mask;
            hw_gpio_ptr->GPIO->TLR  |=  pin_mask;
            break;

        case VSF_GPIO_INT_MODE_RISING:
            hw_gpio_ptr->GPIO->TELR &= ~pin_mask;
            hw_gpio_ptr->GPIO->TER  &= ~pin_mask;
            hw_gpio_ptr->GPIO->TLR  |=  pin_mask;
            break;

        case VSF_GPIO_INT_MODE_FALLING:
            hw_gpio_ptr->GPIO->TELR &= ~pin_mask;
            hw_gpio_ptr->GPIO->TER  &= ~pin_mask;
            hw_gpio_ptr->GPIO->TLR  &= ~pin_mask;
            break;

        case VSF_GPIO_INT_MODE_RISING_FALLING:
            hw_gpio_ptr->GPIO->TELR &= ~pin_mask;
            hw_gpio_ptr->GPIO->TER  |= pin_mask;
            break;
        }

        uint32_t fr_mask = 0;
        for (int i = 0; i < VSF_HW_IO_PIN_COUNT / 4; i++) {
            if (pin_mask & (0x03 << i)) {
                fr_mask |= (0x07 << i);
            }
        }
        hw_gpio_ptr->GPIO->FR  |= fr_mask;
        hw_gpio_ptr->GPIO->ICR |= pin_mask;
    }

    return VSF_ERR_NONE;
}

void __vsf_hw_gpio_irq_handler(vsf_hw_gpio_t *hw_gpio_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    GPIO_REG_T *GPIOx = hw_gpio_ptr->GPIO;
    VSF_HAL_ASSERT(NULL != GPIOx);
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr->isrs);

    uint32_t irq_pin_mask = GPIOx->ISR;
    while (irq_pin_mask) {
        uint32_t pin = 31 - __CLZ(irq_pin_mask);
        uint32_t pin_mask = 0x01UL << pin;
        VSF_HAL_ASSERT(pin < VSF_HW_IO_PIN_COUNT);
        GPIOx->IRR = pin_mask;

        //copy from gpio_api.c
        // I don't know why the ISB is called here.
        __ISB();

        irq_pin_mask = GPIOx->ISR;

        VSF_HAL_ASSERT(NULL != hw_gpio_ptr->isrs[pin].handler_fn);
        hw_gpio_ptr->isrs[pin].handler_fn(hw_gpio_ptr->isrs[pin].target_ptr,
                                         (vsf_gpio_t *)hw_gpio_ptr, pin_mask);
    }
}

/*============================ INCLUDES ======================================*/

#define VSF_GPIO_CFG_IMP_PREFIX                 vsf_hw
#define VSF_GPIO_CFG_IMP_UPCASE_PREFIX          VSF_HW
#define VSF_GPIO_CFG_IMP_LV0(__IDX, __HAL_OP)                                   \
    static vsf_gpio_isr_t __vsf_hw_gpio ## __IDX ## isr[VSF_HW_IO_PIN_COUNT];   \
    vsf_hw_gpio_t vsf_hw_gpio ## __IDX = {                                      \
        .GPIO = (GPIO_REG_T *)VSF_HW_GPIO ## __IDX ## _BASE_ADDRESS,            \
        .irqn = VSF_HW_GPIO ## __IDX ## _IRQ_IDX,                               \
        .IOMUX = ((AIC_IOMUX_TypeDef *)VSF_HW_IO_PORT ## __IDX ## _IOMUX_REG_BASE),\
        .is_pmic = VSF_HW_IO_PORT ## __IDX ## _IS_PMIC,                         \
        .gpio_pin_mask = VSF_HW_IO_PORT ## __IDX ## _GPIO_PIN_MASK,             \
        .isrs = VSF_HW_IO_PORT ## __IDX ## _IS_PMIC ?                           \
                NULL : __vsf_hw_gpio ## __IDX ## isr,                           \
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
