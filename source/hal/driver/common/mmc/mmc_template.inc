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

#if VSF_HAL_USE_MMC == ENABLED

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define vsf_real_mmc_t                                VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_t)
#define vsf_real_mmc_capability                       VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_capability)

#ifndef VSF_MMC_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_MMC_CFG_REIMPLEMENT_API_CAPABILITY              DISALBED
#endif

#if VSF_MMC_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
#   ifndef VSF_MMC_CFG_CAPCBILITY_MAX_FREQ_HZ
#       define VSF_MMC_CFG_CAPCBILITY_MAX_FREQ_HZ           (50 * 1000 * 1000)
#   endif
#   ifndef VSF_MMC_CFG_CAPCBILITY_BUS_WIDTH
#       define VSF_MMC_CFG_CAPCBILITY_BUS_WIDTH             MMC_CAP_BUS_WIDTH_8
#   endif
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_MMC_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_mmc_capability_t vsf_real_mmc_capability(vsf_real_mmc_t *mmc_ptr)
{
    vsf_mmc_capability_t mmc_capability = {
        .max_freq_hz = VSF_MMC_CFG_CAPCBILITY_MAX_FREQ_HZ,
        .bus_width   = VSF_MMC_CFG_CAPCBILITY_BUS_WIDTH,
    };

    return mmc_capability;
}
#endif

/*============================ MACROS ========================================*/

#undef VSF_MMC_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_MMC_CFG_CAPCBILITY_MAX_FREQ_HZ
#undef VSF_MMC_CFG_CAPCBILITY_BUS_WIDTH
#undef vsf_real_mmc_t
#undef vsf_real_mmc_capability

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                     _mmc
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME              _MMC

#ifndef VSF_MMC_CFG_IMP_PREFIX
#   error "Please define VSF_MMC_CFG_IMP_PREFIX in mmc driver"
#endif

#ifndef VSF_MMC_CFG_IMP_UPCASE_PREFIX
#   error "Please define VSF_MMC_CFG_IMP_UPCASE_PREFIX in mmc driver"
#endif

#ifndef VSF_MMC_CFG_IMP_INSTANCE_PREFIX
#   define VSF_MMC_CFG_IMP_INSTANCE_PREFIX         VSF_MMC_CFG_IMP_PREFIX
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_MMC_CFG_IMP_PREFIX
#undef VSF_MMC_CFG_IMP_INSTANCE_PREFIX
#undef VSF_MMC_CFG_IMP_UPCASE_PREFIX
#undef VSF_MMC_CFG_IMP_LV0

#endif  /* VSF_HAL_USE_MMC */
