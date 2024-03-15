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

#if VSF_HAL_USE_I2S == ENABLED

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define vsf_real_i2s_t                                      VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_t)
#define vsf_real_i2s_capability                             VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_capability)

#ifndef VSF_I2S_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_I2S_CFG_REIMPLEMENT_API_CAPABILITY              DISALBED
#endif

#if VSF_I2S_CFG_REIMPLEMENT_API_CAPABILITY == ENABLED
#   ifndef VSF_I2S_CFG_CAPABILITY_IS_SRC_SUPPORTED
#       define VSF_I2S_CFG_CAPABILITY_IS_SRC_SUPPORTED      1
#   endif
#   ifndef VSF_I2S_CFG_CAPABILITY_IS_DBUFFER_SUPPORTED
#       define VSF_I2S_CFG_CAPABILITY_IS_DBUFFER_SUPPORTED  1
#   endif
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_I2S_CFG_REIMPLEMENT_API_CAPABILITY == ENABLED
vsf_i2s_capability_t vsf_real_i2s_capability(vsf_real_i2s_t *i2s_ptr)
{
    vsf_i2s_capability_t i2s_capability = {
        .i2s_capability = {
            .is_src_supported       = VSF_I2S_CFG_CAPABILITY_IS_SRC_SUPPORTED
            .is_dbuffer_supported   = VSF_I2S_CFG_CAPABILITY_IS_DBUFFER_SUPPORTED
        }
    };

    return i2s_capability;
}
#endif

/*============================ MACROS ========================================*/

#undef VSF_I2S_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_I2S_CFG_CAPABILITY_IS_SRC_SUPPORTED
#undef VSF_I2S_CFG_CAPABILITY_IS_DBUFFER_SUPPORTED

#undef vsf_real_i2s_t
#undef vsf_real_i2s_capability

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                     _i2s
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME              _I2S

#ifndef VSF_I2S_CFG_IMP_PREFIX
#   error "Please define VSF_I2S_CFG_IMP_PREFIX in i2s driver"
#endif

#ifndef VSF_I2S_CFG_IMP_UPCASE_PREFIX
#   error "Please define VSF_I2S_CFG_IMP_UPCASE_PREFIX in i2s driver"
#endif

#ifndef VSF_I2S_CFG_IMP_INSTANCE_PREFIX
#   define VSF_I2S_CFG_IMP_INSTANCE_PREFIX         VSF_I2S_CFG_IMP_PREFIX
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_I2S_CFG_IMP_PREFIX
#undef VSF_I2S_CFG_IMP_UPCASE_PREFIX
#undef VSF_I2S_CFG_IMP_INSTANCE_PREFIX
#undef VSF_I2S_CFG_IMP_LV0

#endif // VSF_HAL_USE_I2S == ENABLED
