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

#if VSF_HAL_USE_EXTI == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_EXTI_CFG_RETAIN_DEFINED_PREFIX
#   define VSF_EXTI_CFG_RETAIN_DEFINED_PREFIX                       DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ IMPLEMENTATION ================================*/
/*============================ MACROS ========================================*/
/*============================ MACROS ========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#ifndef VSF_HAL_TEMPLATE_IMP_NAME
#   define VSF_HAL_TEMPLATE_IMP_NAME                _exti
#endif

#ifndef VSF_HAL_TEMPLATE_IMP_UPCASE_NAME
#   define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME         _EXTI
#endif

#ifndef VSF_HAL_TEMPLATE_IMP_COUNT_SUFFIX
#   define VSF_HAL_TEMPLATE_IMP_COUNT_SUFFIX        _COUNT
#endif

#ifndef VSF_HAL_TEMPLATE_IMP_MASK_SUFFIX
#   define VSF_HAL_TEMPLATE_IMP_MASK_SUFFIX         _MASK
#endif

#if !defined(VSF_EXTI_CFG_IMP_PREFIX) && !defined(VSF_EXTI_CFG_IMP_DEVICE_PREFIX)
#   error "Please define VSF_EXTI_CFG_IMP_PREFIX in exti driver"
#endif

#if !defined(VSF_EXTI_CFG_IMP_UPCASE_PREFIX) && !defined(VSF_EXTI_CFG_IMP_DEVICE_UPCASE_PREFIX)
#   error "Please define VSF_EXTI_CFG_IMP_UPCASE_PREFIX in exti driver"
#endif

#ifndef VSF_EXTI_CFG_IMP_COUNT_MASK_PREFIX
#   define VSF_EXTI_CFG_IMP_COUNT_MASK_PREFIX       VSF_EXTI_CFG_IMP_UPCASE_PREFIX
#endif

#ifdef VSF_EXTI_CFG_IMP_REMAP_FUNCTIONS
#   define VSF_HAL_CFG_IMP_REMAP_FUNCTIONS          VSF_EXTI_CFG_IMP_REMAP_FUNCTIONS
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#if VSF_EXTI_CFG_RETAIN_DEFINED_PREFIX == DISABLED
#   undef VSF_EXTI_CFG_IMP_PREFIX
#   undef VSF_EXTI_CFG_IMP_UPCASE_PREFIX
#   undef VSF_EXTI_CFG_IMP_DEVICE_PREFIX
#   undef VSF_EXTI_CFG_IMP_DEVICE_UPCASE_PREFIX
#endif

#endif  /* VSF_HAL_USE_EXTI */
