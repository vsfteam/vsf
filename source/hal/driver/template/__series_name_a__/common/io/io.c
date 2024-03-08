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
    return (vsf_io_capability_t){ 0 };
}

/*============================ INCLUDES ======================================*/

/*\note DO NOT use __IDX in VSF_IO_CFG_IMP_LV0, io module is singleton mode.
 */

#define VSF_IO_CFG_IMP_LV0(__IDX, __HAL_OP)                                     \
    VSF_MCONNECT(VSF_IO_CFG_IMP_PREFIX, _io_t)                                  \
        VSF_MCONNECT(VSF_IO_CFG_IMP_PREFIX, _io) = {                            \
        __HAL_OP                                                                \
    };

#include "hal/driver/common/io/io_template.inc"

#endif      // VSF_HAL_USE_IO
