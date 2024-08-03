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

#if VSF_HAL_USE_IO == ENABLED

#include "hal/vsf_hal.h"

#include "../vendor/Inc/stm32h7xx.h"

/*============================ MACROS ========================================*/

/*\note VSF_HW_IO_CFG_MULTI_CLASS is only for drivers for specified device(hw drivers).
 */

// HW
#ifndef VSF_HW_IO_CFG_MULTI_CLASS
#   define VSF_HW_IO_CFG_MULTI_CLASS            VSF_IO_CFG_MULTI_CLASS
#endif
// HW end

// HW
#define VSF_IO_CFG_IMP_PREFIX                   vsf_hw
#define VSF_IO_CFG_IMP_UPCASE_PREFIX            VSF_HW
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/

extern GPIO_TypeDef * __vsf_hw_gpio_get_regbase(vsf_hw_gpio_t *gpio_ptr);

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// HW
typedef struct VSF_MCONNECT(VSF_IO_CFG_IMP_PREFIX, _io_t) {
#if VSF_HW_IO_CFG_MULTI_CLASS == ENABLED
    vsf_io_t                vsf_io;
#endif
} VSF_MCONNECT(VSF_IO_CFG_IMP_PREFIX, _io_t);
// HW end

/*============================ IMPLEMENTATION ================================*/

vsf_err_t VSF_MCONNECT(VSF_IO_CFG_IMP_PREFIX, _io_config_one_pin)(
    VSF_MCONNECT(VSF_IO_CFG_IMP_PREFIX, _io_t) *io_ptr,
    vsf_io_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(io_ptr != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    vsf_hw_gpio_t *gpio_ptr = vsf_hw_gpios[cfg_ptr->port_index];
    VSF_HAL_ASSERT(gpio_ptr != NULL);
    GPIO_TypeDef *reg = __vsf_hw_gpio_get_regbase(gpio_ptr);
    uint32_t pin = cfg_ptr->pin_index, function = cfg_ptr->function, offset_len4;

    if (VSF_IO_AF == (cfg_ptr->mode & 3)) {
        if (pin < 8) {
            offset_len4 = pin << 2;
            vsf_atom32_op(&reg->AFR[0], (_ & ~(15 << offset_len4)) | (function << offset_len4));
        } else {
            offset_len4 = (pin - 8) << 2;
            vsf_atom32_op(&reg->AFR[1], (_ & ~(15 << offset_len4)) | (function << offset_len4));
        }
    }
    vsf_hw_gpio_config_pin(gpio_ptr, 1 << pin, cfg_ptr->mode);
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_IO_CFG_IMP_PREFIX, _io_config)(
    VSF_MCONNECT(VSF_IO_CFG_IMP_PREFIX, _io_t) *io_ptr,
    vsf_io_cfg_t *cfg_ptr,
    uint_fast8_t count
) {
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

vsf_io_capability_t VSF_MCONNECT(VSF_IO_CFG_IMP_PREFIX, _io_capability)(
    VSF_MCONNECT(VSF_IO_CFG_IMP_PREFIX, _io_t) *io_ptr
) {
    return (vsf_io_capability_t){
        .pin_count                  = 16,
        .pin_mask                   = 0xFFFF,
    };
}

/*============================ INCLUDES ======================================*/

/*\note DO NOT use __IDX in VSF_IO_CFG_IMP_LV0, io module is singleton mode.
 */

#define VSF_IO_CFG_REIMPLEMENT_API_CAPABILITY   ENABLED

#define VSF_IO_CFG_IMP_LV0(__IDX, __HAL_OP)                                     \
    VSF_MCONNECT(VSF_IO_CFG_IMP_PREFIX, _io_t)                                  \
        VSF_MCONNECT(VSF_IO_CFG_IMP_PREFIX, _io) = {                            \
        __HAL_OP                                                                \
    };

#include "hal/driver/common/io/io_template.inc"

#endif      // VSF_HAL_USE_IO
