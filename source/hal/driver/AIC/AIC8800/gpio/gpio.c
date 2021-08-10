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

#undef REG_GPIO_IOMUX0
#define REG_GPIO_IOMUX0                         (0X40503000)

#undef REG_GPIO_IOMUX1
#define REG_GPIO_IOMUX1                         (0x50012000)

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __VSF_HW_GPIO_IPM_LV0(__COUNT, __DONT_CARE)                             \
    vsf_gpio_t vsf_gpio##__COUNT = {                                            \
        .REG = {                                                                \
        .GPIO = REG_GPIO##__COUNT,                                              \
            .IOMUX = ((AIC_IOMUX_TypeDef *)REG_GPIO_IOMUX##__COUNT),            \
        },                                                                      \
    };

#define aic8800_gpio_def(__count)                                               \
    VSF_MREPEAT(__count, __VSF_HW_GPIO_IPM_LV0, NULL)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

 aic8800_gpio_def(GPIO_COUNT)

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/



void vsf_gpio_config_pin(vsf_gpio_t *gpio_ptr, uint32_t pin_mask, uint32_t feature)
{
    int i;
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    for (i = 0; i < 32; i++) {
        if (pin_mask & (1 << i)) {
            if (&vsf_gpio0 == gpio_ptr) {
                gpio_ptr->REG.IOMUX->GPCFG[i] &= ~IOMUX_GPIO_CONFIG_PULL_UP_MASK;
                gpio_ptr->REG.IOMUX->GPCFG[i] &= ~IOMUX_GPIO_CONFIG_PULL_DN_MASK;
            } else if (&vsf_gpio1 == gpio_ptr) {
                PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteIomux->GPCFG[i]),
                                    0,
                                    (AIC1000LITE_IOMUX_PAD_GPIO_PULL_FRC
                                    |   AIC1000LITE_IOMUX_PAD_GPIO_PULL_UP
                                    |   AIC1000LITE_IOMUX_PAD_GPIO_PULL_DN));
            } else {
                    VSF_HAL_ASSERT(false);
            }
            if (feature & IO_PULL_UP) {
                if (&vsf_gpio0 == gpio_ptr) {
                    gpio_ptr->REG.IOMUX->GPCFG[i] |= IOMUX_GPIO_CONFIG_PULL_UP_MASK;
                } else {
                    PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteIomux->GPCFG[i]),
                                        (AIC1000LITE_IOMUX_PAD_GPIO_PULL_FRC
                                        |   0
                                        |   AIC1000LITE_IOMUX_PAD_GPIO_PULL_DN),
                                        (AIC1000LITE_IOMUX_PAD_GPIO_PULL_FRC
                                        |   AIC1000LITE_IOMUX_PAD_GPIO_PULL_UP
                                        |   AIC1000LITE_IOMUX_PAD_GPIO_PULL_DN));
                }
            }
            if (feature & IO_PULL_DOWN){
                if (&vsf_gpio0 == gpio_ptr) {
                    gpio_ptr->REG.IOMUX->GPCFG[i] |= IOMUX_GPIO_CONFIG_PULL_DN_MASK;
                } else if (&vsf_gpio1 == gpio_ptr) {
                    PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteIomux->GPCFG[i]),
                                        (AIC1000LITE_IOMUX_PAD_GPIO_PULL_FRC
                                        |   0
                                        |   AIC1000LITE_IOMUX_PAD_GPIO_PULL_DN),
                                        (AIC1000LITE_IOMUX_PAD_GPIO_PULL_FRC
                                        |   AIC1000LITE_IOMUX_PAD_GPIO_PULL_UP
                                        |   AIC1000LITE_IOMUX_PAD_GPIO_PULL_DN));
                } else {
                    VSF_HAL_ASSERT(false);
                }
            }
        }
    }
}

void vsf_gpio_set_direction(vsf_gpio_t *gpio_ptr, uint32_t direction_mask, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    if (&vsf_gpio0 == gpio_ptr) {
        uint32_t temp_value;
        temp_value = gpio_ptr->REG.GPIO->DR;
        gpio_ptr->REG.GPIO->DR = (temp_value & (~pin_mask)) | (direction_mask & pin_mask);
    } else if (&vsf_gpio1 == gpio_ptr) {
        PMIC_MEM_MASK_WRITE((unsigned int)(&gpio_ptr->REG.GPIO->DR), direction_mask, pin_mask);
    } else {
        VSF_HAL_ASSERT(false);
    }
}

uint32_t vsf_gpio_get_direction(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    if (&vsf_gpio0 == gpio_ptr) {
        return gpio_ptr->REG.GPIO->DR & pin_mask;
    } else if (&vsf_gpio1 == gpio_ptr) {
        return PMIC_MEM_READ((unsigned int)(gpio_ptr->REG.GPIO->DR)) & pin_mask;
    } else {
        VSF_HAL_ASSERT(false);
    }
}

void vsf_gpio_set_input(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    vsf_gpio_set_direction(gpio_ptr, (uint32_t )0, pin_mask);
}

void vsf_gpio_set_output(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    vsf_gpio_set_direction(gpio_ptr, ~(uint32_t )0, pin_mask);
}

void vsf_gpio_switch_direction(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    uint32_t ret = ~vsf_gpio_get_direction(gpio_ptr, pin_mask);
    vsf_gpio_set_direction(gpio_ptr, ret, pin_mask);
}

uint32_t vsf_gpio_read(vsf_gpio_t *gpio_ptr)
{
    uint32_t temp_value, temp_ret;
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    if (&vsf_gpio0 == gpio_ptr) {
        temp_value = gpio_ptr->REG.GPIO->MR;
        gpio_ptr->REG.GPIO->MR = ~(uint32_t)0;
        temp_ret = gpio_ptr->REG.GPIO->VR & ~(uint32_t )0;
        gpio_ptr->REG.GPIO->MR = temp_value;
    } else if (&vsf_gpio1 == gpio_ptr) {
        temp_value = PMIC_MEM_READ((unsigned int)(&gpio_ptr->REG.GPIO->MR));
        PMIC_MEM_MASK_WRITE((unsigned int)(&gpio_ptr->REG.GPIO->MR), ~(uint32_t)0, ~(uint32_t)0);
        temp_ret = PMIC_MEM_READ((unsigned int)(&gpio_ptr->REG.GPIO->VR)) & ~(uint32_t )0;
        PMIC_MEM_MASK_WRITE((unsigned int)(&gpio_ptr->REG.GPIO->MR), temp_value, ~(uint32_t)0);
    } else {
        VSF_HAL_ASSERT(false);
    }
    return temp_ret;
}

void vsf_gpio_write(vsf_gpio_t *gpio_ptr, uint32_t value, uint32_t pin_mask)
{
    uint32_t temp_value;
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    if (&vsf_gpio0 == gpio_ptr) {
        temp_value = gpio_ptr->REG.GPIO->MR;
        gpio_ptr->REG.GPIO->MR = pin_mask;
        gpio_ptr->REG.GPIO->VR = value;
        gpio_ptr->REG.GPIO->MR = temp_value;
    } else if (&vsf_gpio1 == gpio_ptr) {
        temp_value = PMIC_MEM_READ((unsigned int)(&gpio_ptr->REG.GPIO->MR));
        PMIC_MEM_MASK_WRITE((unsigned int)(&gpio_ptr->REG.GPIO->MR), pin_mask, ~(uint32_t)0);
        PMIC_MEM_MASK_WRITE((unsigned int)(&gpio_ptr->REG.GPIO->VR), value, pin_mask);
        PMIC_MEM_MASK_WRITE((unsigned int)(&gpio_ptr->REG.GPIO->MR), temp_value, ~(uint32_t)0);
    } else {
        VSF_HAL_ASSERT(false);
    }
}

void vsf_gpio_set(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    vsf_gpio_write(gpio_ptr, ~(uint32_t )0, pin_mask);
}

void vsf_gpio_clear(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    vsf_gpio_write(gpio_ptr, (uint32_t )0, pin_mask);
}

void vsf_gpio_toggle(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    uint32_t ret = ~vsf_gpio_read(gpio_ptr);
    vsf_gpio_write(gpio_ptr, ret, pin_mask);
}

#endif      // VSF_HAL_USE_GPIO
