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

#define VSF_IO_CFG_IMP_PREFIX                 vsf_hw
#define VSF_IO_CFG_IMP_UPCASE_PREFIX          VSF_HW

/*============================ INCLUDES ======================================*/

#include "./io.h"

#if VSF_HAL_USE_IO == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_HW_IO_CFG_MULTI_CLASS
#   define VSF_HW_IO_CFG_MULTI_CLASS           VSF_IO_CFG_MULTI_CLASS
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_hw_io_t {
#if VSF_HW_IO_CFG_MULTI_CLASS == ENABLED
    vsf_io_t vsf_io;
#endif

    AIC_IOMUX_TypeDef *IOMUX[VSF_HW_IO_PORT_MAX];
} vsf_hw_io_t;

/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_hw_io_config_one_pin(vsf_hw_io_t *io_ptr, io_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(io_ptr != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    uint16_t port_index = cfg_ptr->port_index;
    if (port_index >= VSF_HW_IO_PORT_MAX) {
        VSF_ASSERT(0);
        return VSF_ERR_INVALID_RANGE;
    }
    uint16_t pin_index = cfg_ptr->pin_index;
    if (pin_index >= VSF_HW_IO_PIN_MAX) {
        VSF_ASSERT(0);
        return VSF_ERR_INVALID_RANGE;
    }
    if (cfg_ptr->feature & ~__HW_IO_FEATURE_ALL_BITS) {
        VSF_ASSERT(0);
        return VSF_ERR_NOT_SUPPORT;
    }
    //TODO: check some invalid function, for example GPIOA0 Function 2 is invalid.
    if (cfg_ptr->function >= VSF_HW_IO_FUNCTION_MAX) {
        VSF_ASSERT(0);
        return VSF_ERR_NOT_SUPPORT;
    }

    __hw_io_reg_mask_write(port_index != 0,
                           &io_ptr->IOMUX[port_index]->GPCFG[pin_index],
                           cfg_ptr->feature | cfg_ptr->function,
                           __HW_IO_FEATURE_ALL_BITS | __VSF_HW_IO_FUNCTION_MASK);

    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_io_config(vsf_hw_io_t *io_ptr, io_cfg_t *cfg_ptr, uint_fast8_t count)
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

/*============================ INCLUDES ======================================*/

#define __VSF_HW_IOMUX(__COUNT, __) \
    ((AIC_IOMUX_TypeDef *) VSF_HW_IO_PORT ## __COUNT ## _IOMUX_REG_BASE),

#define VSF_IO_CFG_IMP_LV0(__COUNT, __HAL_OP)                                   \
    vsf_hw_io_t vsf_hw_io##__COUNT = {                                          \
        .IOMUX = {                                                              \
            VSF_MREPEAT(VSF_HW_IO_PORT_MAX, __VSF_HW_IOMUX, NULL)               \
        },                                                                      \
        __HAL_OP                                                                \
    };

#include "hal/driver/common/io/io_template.inc"

#endif      // VSF_HAL_USE_IO
