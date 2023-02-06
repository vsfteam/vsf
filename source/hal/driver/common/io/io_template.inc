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

#include "hal/driver/common/template/vsf_template_hal_driver.h"

/*============================ MACROS ========================================*/

#if VSF_HAL_USE_IO == ENABLED

/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_real_io_t   VSF_MCONNECT(VSF_IO_CFG_IMP_PREFIX, _io_t)

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_IO_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_io_capability_t vsf_real_io_capability(vsf_real_io_t *io_ptr)
{
    vsf_io_capability_t io_capability = {
        0,
    };

    return io_capability;
}
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define __VSF_HAL_TEMPLATE_NAME                     _io
#define __VSF_HAL_TEMPLATE_UPCASE_NAME              _IO

#ifndef VSF_IO_CFG_IMP_PREFIX
#   error "Please define VSF_IO_CFG_IMP_PREFIX in io driver"
#endif

#ifndef VSF_IO_CFG_IMP_UPCASE_PREFIX
#   error "Please define VSF_IO_CFG_IMP_UPCASE_PREFIX in io driver"
#endif

#define __VSF_HAL_TEMPLATE_CFG_REMOVE_ARRAY
#define __VSF_HAL_TEMPLATE_IMP_COUNT                1

#include "hal/driver/common/template/vsf_template_implementation.h"

#endif // VSF_HAL_USE_IO == ENABLED
