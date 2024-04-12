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

#if VSF_HAL_USE_IO == ENABLED

#   include "../vendor/plf/aic8800/src/driver/pmic/pmic_api.h"
#   include "../vendor/plf/aic8800/src/driver/iomux/reg_iomux.h"
#   include "../gpio/i_reg_gpio.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_IO_CFG_MULTI_CLASS
#   define VSF_HW_IO_CFG_MULTI_CLASS    VSF_IO_CFG_MULTI_CLASS
#endif

#define __AIC8800_IO_FUNCTION_MASK      IOMUX_GPIO_CONFIG_SEL_MASK

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_hw_io_gpio_map_t {
    uint8_t funs[VSF_HW_IO_PIN_COUNT];
} vsf_hw_io_gpio_map_t;

typedef struct vsf_hw_io_t {
#if VSF_HW_IO_CFG_MULTI_CLASS == ENABLED
    vsf_io_t vsf_io;
#endif

    struct {
        AIC_IOMUX_TypeDef *IOMUX;
        GPIO_REG_T *GPIO;
        bool is_pmic;
        vsf_hw_io_gpio_map_t *gpio_map;
    } ports[VSF_HW_IO_PORT_COUNT];
} vsf_hw_io_t;

/*============================ IMPLEMENTATION ================================*/

uint32_t aic8800_io_reg_read(bool is_pmic, volatile uint32_t *reg)
{
    if (!is_pmic) {
        return *reg;
    } else {
        return PMIC_MEM_READ((unsigned int)reg);
    }
}

void aic8800_io_reg_mask_write(bool is_pmic, volatile uint32_t *reg,
                                          uint32_t wdata, uint32_t wmask)
{
    if (!is_pmic) {
        *reg = (*reg & ~wmask) | (wdata & wmask);
    } else {
        PMIC_MEM_MASK_WRITE((unsigned int)reg, wdata, wmask);
    }
}

vsf_err_t vsf_hw_io_config_one_pin(vsf_hw_io_t *io_ptr, vsf_io_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(io_ptr != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    uint8_t port_index = cfg_ptr->port_index;
    if (port_index >= VSF_HW_IO_PORT_COUNT) {
        VSF_ASSERT(0);
        return VSF_ERR_INVALID_RANGE;
    }
    uint8_t pin_index = cfg_ptr->pin_index;
    if (pin_index >= VSF_HW_IO_PIN_COUNT) {
        VSF_ASSERT(0);
        return VSF_ERR_INVALID_RANGE;
    }
    if (cfg_ptr->mode & ~__AIC8800_IO_MODE_ALL_BITS) {
        VSF_ASSERT(0);
        return VSF_ERR_NOT_SUPPORT;
    }
    //TODO: check some invalid function, for example GPIOA0 Function 2 is invalid.
    if (cfg_ptr->function >= VSF_HW_IO_FUNCTION_MAX) {
        VSF_ASSERT(0);
        return VSF_ERR_NOT_SUPPORT;
    }

    bool is_pmic = io_ptr->ports[port_index].is_pmic;
    volatile uint32_t *reg = &io_ptr->ports[port_index].IOMUX->GPCFG[pin_index];
    uint32_t wdata = cfg_ptr->mode | cfg_ptr->function;
    uint32_t wmask = __AIC8800_IO_MODE_ALL_BITS | __AIC8800_IO_FUNCTION_MASK;
    if (is_pmic) {
        PMIC_MEM_MASK_WRITE((unsigned int)reg, wdata, wmask);
        if (io_ptr->ports[port_index].gpio_map != NULL) {
            uint8_t * funs = io_ptr->ports[port_index].gpio_map->funs;
            if (funs[pin_index] == cfg_ptr->function) {
                // MR register need to be set 1 when pmic port configured as GPIO
                PMIC_MEM_MASK_WRITE((unsigned int)&io_ptr->ports[pin_index].GPIO->MR,  (1 << pin_index), (1 << pin_index));
            }
        }
    } else {
        *reg = (*reg & ~wmask) | (wdata & wmask);
    }

    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_io_config(vsf_hw_io_t *io_ptr, vsf_io_cfg_t *cfg_ptr, uint_fast8_t count)
{
    VSF_HAL_ASSERT(io_ptr != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);
    VSF_HAL_ASSERT(count != 0);

    for (int i = 0; i < count; i++) {
        vsf_err_t result = vsf_hw_io_config_one_pin(io_ptr, cfg_ptr + i);
        if (result != VSF_ERR_NONE) {
            return result;
        }
    }

    return VSF_ERR_NONE;
}

vsf_io_capability_t vsf_hw_io_capability(vsf_hw_io_t *io)
{
    // TODO: update
    vsf_io_capability_t vsf_io_capability = {
        0,
    };

    return vsf_io_capability;
}

/*============================ INCLUDES ======================================*/

#define VSF_IO_CFG_REIMPLEMENT_API_CAPABILITY ENABLED
#define VSF_IO_CFG_IMP_PREFIX                 vsf_hw
#define VSF_IO_CFG_IMP_UPCASE_PREFIX          VSF_HW
#define __VSF_HW_IOMUX(__IDX, __)                                               \
    .ports[__IDX] = {                                                           \
        .is_pmic = VSF_HW_IO_PORT ## __IDX ## _IS_PMIC,                         \
        .IOMUX = ((AIC_IOMUX_TypeDef *) VSF_HW_IO_PORT ## __IDX ## _IOMUX_REG_BASE),\
        .GPIO = (GPIO_REG_T *)VSF_HW_GPIO ## __IDX ## _BASE_ADDRESS,            \
        .gpio_map = VSF_HW_IO_PORT ## __IDX ## _MAP,                     		\
    },

#define VSF_IO_CFG_IMP_LV0(__IDX, __HAL_OP)                                     \
    vsf_hw_io_t vsf_hw_io = {                                                   \
        VSF_MREPEAT(VSF_HW_IO_PORT_COUNT, __VSF_HW_IOMUX, NULL)                 \
        __HAL_OP                                                                \
    };

#include "hal/driver/common/io/io_template.inc"

#endif      // VSF_HAL_USE_IO
