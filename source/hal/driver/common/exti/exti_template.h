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
/*============================ MACROFIED FUNCTIONS ===========================*/

#ifndef VSF_HAL_TEMPLATE_DEC_RETAIN_DEFINED_PREFIX
#   define VSF_HAL_TEMPLATE_DEC_RETAIN_DEFINED_PREFIX   DISABLED
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_NAME
#   define VSF_HAL_TEMPLATE_DEC_NAME                   _exti
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_UPCASE_NAME
#   define VSF_HAL_TEMPLATE_DEC_UPCASE_NAME            _EXTI
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_COUNT_SUFFIX
#   define VSF_HAL_TEMPLATE_DEC_COUNT_SUFFIX           _COUNT
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_MASK_SUFFIX
#   define VSF_HAL_TEMPLATE_DEC_MASK_SUFFIX            _MASK
#endif

#ifndef __VSF_TEMPLATE_EXTI_H__
#   error "Please include \"vsf_template_exti.h\" before include exti_template.h"
#endif

#if !defined(VSF_EXTI_CFG_DEC_PREFIX) && !defined(VSF_EXTI_CFG_DEC_DEVICE_PREFIX)
#   error "Please define VSF_EXTI_CFG_DEC_PREFIX or VSF_EXTI_CFG_DEC_DEVICE_PREFIX before include exti_template.h"
#endif

#if !defined(VSF_EXTI_CFG_DEC_UPCASE_PREFIX) && !defined(VSF_EXTI_CFG_DEC_DEVICE_UPCASE_PREFIX)
#   error "Please define VSF_EXTI_CFG_DEC_UPCASE_PREFIX or VSF_EXTI_CFG_DEC_DEVICE_UPCASE_PREFIX before include exti_template.h"
#endif

#ifndef VSF_EXTI_CFG_DEC_REMAP_PREFIX
#   define VSF_EXTI_CFG_DEC_REMAP_PREFIX                VSF_EXTI_CFG_DEC_PREFIX
#endif

#ifndef VSF_EXTI_CFG_DEC_COUNT_MASK_PREFIX
#   define VSF_EXTI_CFG_DEC_COUNT_MASK_PREFIX           VSF_EXTI_CFG_DEC_UPCASE_PREFIX
#endif

#define VSF_HAL_TEMPLATE_DEC_INSTANCE_APIS(__prefix_name)                       \
    VSF_EXTI_APIS(__prefix_name)

#include "hal/driver/common/template/vsf_template_instance_declaration.h"

#if VSF_HAL_TEMPLATE_DEC_RETAIN_DEFINED_PREFIX == DISABLED
#   undef VSF_EXTI_CFG_DEC_PREFIX
#   undef VSF_EXTI_CFG_DEC_UPCASE_PREFIX
#   undef VSF_EXTI_CFG_DEC_DEVICE_PREFIX
#   undef VSF_EXTI_CFG_DEC_DEVICE_UPCASE_PREFIX
#endif

#undef VSF_EXTI_CFG_DEC_REMAP_PREFIX
#undef VSF_EXTI_CFG_DEC_COUNT_MASK_PREFIX
#undef VSF_EXTI_CFG_DEC_EXTERN_OP
#undef VSF_EXTI_CFG_IMP_LV0

/*============================ INCLUDES ======================================*/


