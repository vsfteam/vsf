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

#if VSF_HAL_USE_FLASH == ENABLED

/*============================ INCLUDES ======================================*/

/*============================ MACROS ========================================*/

#define vsf_real_flash_t          VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_t)
#define vsf_real_flash_capability VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_capability)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_FLASH_CFG_REIMPLEMENT_CAPABILITY == DISABLED
flash_capability_t vsf_real_flash_capability(vsf_real_flash_t *flash_ptr)
{
    flash_capability_t flash_capability = {
        0,
    };

    return flash_capability;
}
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define __VSF_HAL_TEMPLATE_NAME                     _flash
#define __VSF_HAL_TEMPLATE_UPCASE_NAME              _FLASH

#ifndef VSF_FLASH_CFG_PREFIX
#   error "Please define VSF_FLASH_CFG_PREFIX in flash driver"
#endif

#ifndef VSF_FLASH_CFG_UPPERCASE_PREFIX
#   error "Please define VSF_ADC_CFG_UPPERCASE_PREFIX in flash driver"
#endif

#include "hal/driver/common/template/vsf_template_implementation.h"

#endif  /* VSF_HAL_USE_FLASH */
