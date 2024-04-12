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
#if VSF_HAL_USE_PM == ENABLED

#ifndef VSF_PM_CFG_CS_COUNT
#   define VSF_PM_CFG_CS_COUNT     1
#endif

#define vsf_real_pm_t          VSF_MCONNECT(VSF_PM_CFG_IMP_PREFIX, _pm_t)
#define vsf_real_pm_capability VSF_MCONNECT(VSF_PM_CFG_IMP_PREFIX, _pm_capability)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_PM_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_pm_capability_t vsf_real_pm_capability(vsf_real_pm_t *pm_ptr)
{
    vsf_pm_capability_t pm_capability = {
        0,
    };

    return pm_capability;
}
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                     _pm
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME              _PM

#ifndef VSF_PM_CFG_IMP_PREFIX
#   error "Please define VSF_PM_CFG_IMP_PREFIX in pm driver"
#endif

#ifndef VSF_PM_CFG_IMP_UPCASE_PREFIX
#   error "Please define VSF_PM_CFG_IMP_UPCASE_PREFIX    in pm driver"
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_PM_CFG_IMP_PREFIX
#undef VSF_PM_CFG_IMP_UPCASE_PREFIX

#endif // VSF_HAL_USE_PM == ENABLED
