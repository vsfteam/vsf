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
#   define  VSF_IO_CFG_REIMPLEMENT_API_CAPABILITY              DISALBED
#endif

#if VSF_IO_CFG_REIMPLEMENT_API_CAPABILITY == ENABLED
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

/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                     _io
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME              _IO

#ifndef VSF_IO_CFG_IMP_PREFIX
#   error "Please define VSF_IO_CFG_IMP_PREFIX in io driver"
#endif

#ifndef VSF_IO_CFG_IMP_UPCASE_PREFIX
#   error "Please define VSF_IO_CFG_IMP_UPCASE_PREFIX in io driver"
#endif

#ifndef VSF_IO_CFG_IMP_INSTANCE_PREFIX
#   define VSF_IO_CFG_IMP_INSTANCE_PREFIX         VSF_IO_CFG_IMP_PREFIX
#endif

#define VSF_HAL_TEMPLATE_IMP_REMOVE_ARRAY
#define VSF_HAL_TEMPLATE_IMP_DEFINED_COUNT          1
#define VSF_HAL_TEMPLATE_IMP_DEFINED_MASK           1

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_IO_CFG_IMP_PREFIX
#undef VSF_IO_CFG_IMP_INSTANCE_PREFIX
#undef VSF_IO_CFG_IMP_UPCASE_PREFIX
#undef VSF_IO_CFG_IMP_LV0

#endif  /* VSF_HAL_USE_IO */
