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

#if VSF_HAL_USE_IO == ENABLED

#include "hal/driver/common/template/vsf_template_hal_driver.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

// TODO: When the IO multiplexing function is in GPIO, we need a
//       vsf_io_config_one_pin template based on the vsf_gpio_config_pin implementation

#define vsf_real_io_t                   VSF_MCONNECT(VSF_IO_CFG_IMP_PREFIX, _io_t)
#define vsf_real_io_capability          VSF_MCONNECT(VSF_IO_CFG_IMP_PREFIX, _io_capability)

#ifndef VSF_IO_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_IO_CFG_REIMPLEMENT_API_CAPABILITY              DISABLED
#endif

#if VSF_IO_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
#   ifndef VSF_IO_CFG_CAPABILITY_PIN_COUNT
#       define VSF_IO_CFG_CAPABILITY_PIN_COUNT              8
#   endif
#   ifndef VSF_IO_CFG_CAPABILITY_PIN_MASK
#       define VSF_IO_CFG_CAPABILITY_PIN_MASK               8
#   endif
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_IO_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_io_capability_t vsf_real_io_capability(vsf_real_io_t *io_ptr)
{
    vsf_io_capability_t io_capability = {
        .pin_count    = VSF_IO_CFG_CAPABILITY_PIN_COUNT,
        .pin_mask     = VSF_IO_CFG_CAPABILITY_PIN_MASK,
    };

    return io_capability;
}
#endif

/*============================ MACROS ========================================*/

#undef VSF_IO_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_IO_CFG_CAPABILITY_PIN_COUNT
#undef VSF_IO_CFG_CAPABILITY_PIN_MASK
#undef vsf_real_io_t
#undef vsf_real_io_capability

/*============================ MACROS ========================================*/

#ifdef VSF_IO_CFG_IMP_REMAP_PREFIX

#   define vsf_imp_io_t                 VSF_MCONNECT(VSF_IO_CFG_IMP_PREFIX, _io_t)
#   define vsf_imp_io_config_one_pin    VSF_MCONNECT(VSF_IO_CFG_IMP_PREFIX, _io_config_one_pin)
#   define vsf_imp_io_config            VSF_MCONNECT(VSF_IO_CFG_IMP_PREFIX, _io_config)
#   define vsf_imp_io_capability        VSF_MCONNECT(VSF_IO_CFG_IMP_PREFIX, _io_capability)

#   define vsf_remap_io_t               VSF_MCONNECT(VSF_IO_CFG_IMP_REMAP_PREFIX, _io_t)
#   define vsf_remap_io_config_one_pin  VSF_MCONNECT(VSF_IO_CFG_IMP_REMAP_PREFIX, _io_config_one_pin)
#   define vsf_remap_io_config          VSF_MCONNECT(VSF_IO_CFG_IMP_REMAP_PREFIX, _io_config)
#   define vsf_remap_io_capability      VSF_MCONNECT(VSF_IO_CFG_IMP_REMAP_PREFIX, _io_capability)

#   define VSF_IO_CFG_IMP_REMAP_FUNCTIONS                                                                               \
        vsf_err_t vsf_imp_io_config_one_pin(vsf_imp_io_t *io_ptr, vsf_io_cfg_t *cfg_ptr)                  \
        {                                                                                                 \
            VSF_HAL_ASSERT(io_ptr != NULL);                                                               \
            VSF_HAL_ASSERT(cfg_ptr != NULL);                                                              \
            return vsf_remap_io_config_one_pin(io_ptr, cfg_ptr);                                          \
        }                                                                                                 \
        vsf_err_t vsf_imp_io_config(vsf_imp_io_t *io_ptr, vsf_io_cfg_t *cfg_ptr, uint_fast8_t count)      \
        {                                                                                                 \
            VSF_HAL_ASSERT(io_ptr != NULL);                                                               \
            VSF_HAL_ASSERT(cfg_ptr != NULL);                                                              \
            VSF_HAL_ASSERT(count != 0);                                                                   \
            return vsf_remap_io_config(io_ptr, cfg_ptr, count);                                           \
        }                                                                                                 \
        vsf_io_capability_t vsf_imp_io_capability(vsf_imp_io_t *io_ptr)                                   \
        {                                                                                                 \
            VSF_HAL_ASSERT(io_ptr != NULL);                                                               \
            return vsf_remap_io_capability(io_ptr);                                                       \
        }
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                   _io
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME            _IO

#ifndef VSF_IO_CFG_IMP_PREFIX
#   error "Please define VSF_IO_CFG_IMP_PREFIX in io driver"
#endif

#ifndef VSF_IO_CFG_IMP_UPCASE_PREFIX
#   error "Please define VSF_IO_CFG_IMP_UPCASE_PREFIX in io driver"
#endif

#ifndef VSF_IO_CFG_IMP_COUNT_MASK_PREFIX
#   define VSF_IO_CFG_IMP_COUNT_MASK_PREFIX         VSF_IO_CFG_IMP_UPCASE_PREFIX
#endif

#ifdef VSF_IO_CFG_IMP_REMAP_FUNCTIONS
#   define VSF_HAL_CFG_IMP_REMAP_FUNCTIONS          VSF_IO_CFG_IMP_REMAP_FUNCTIONS
#endif

#define VSF_HAL_TEMPLATE_IMP_REMOVE_ARRAY
#define VSF_HAL_TEMPLATE_IMP_DEFINED_COUNT          1
#define VSF_HAL_TEMPLATE_IMP_DEFINED_MASK           1

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_IO_CFG_IMP_PREFIX
#undef VSF_IO_CFG_IMP_COUNT_MASK_PREFIX
#undef VSF_IO_CFG_IMP_UPCASE_PREFIX
#undef VSF_IO_CFG_IMP_LV0
#undef VSF_IO_CFG_IMP_REMAP_FUNCTIONS
#undef VSF_IO_CFG_IMP_HAS_OP
#undef VSF_IO_CFG_IMP_EXTERN_OP

#undef vsf_imp_io_t
#undef vsf_imp_io_config_one_pin
#undef vsf_imp_io_config
#undef vsf_imp_io_capability

#undef vsf_remap_io_t
#undef vsf_remap_io_config_one_pin
#undef vsf_remap_io_config
#undef vsf_remap_io_capability

#endif  /* VSF_HAL_USE_IO */
