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
/*============================ MACROS ========================================*/

#if VSF_HAL_USE_SPI == ENABLED

#define vsf_real_spi_t          VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t)
#define vsf_real_spi_capability VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_capability)

#ifndef VSF_SPI_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_SPI_CFG_REIMPLEMENT_API_CAPABILITY              DISALBED
#endif

#if VSF_SPI_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
#   ifndef VSF_SPI_CFG_CAPABILITY_SUPPORT_AUTO_CS
#       define VSF_SPI_CFG_CAPABILITY_SUPPORT_AUTO_CS                   1
#   endif
#   ifndef VSF_SPI_CFG_CAPABILITY_SUPPORT_MANUAL_CS
#       define VSF_SPI_CFG_CAPABILITY_SUPPORT_MANUAL_CS                 1
#   endif
#   ifndef VSF_SPI_CFG_CAPABILITY_CS_COUNT
#       define VSF_SPI_CFG_CAPABILITY_CS_COUNT                          1
#   endif
#   ifndef VSF_SPI_CFG_CAPABILITY_MAX_CLOCK_HZ
#       define VSF_SPI_CFG_CAPABILITY_MAX_CLOCK_HZ                      0xFFFFFFFF
#   endif
#   ifndef VSF_SPI_CFG_CAPABILITY_MIN_CLOCK_HZ
#       define VSF_SPI_CFG_CAPABILITY_MIN_CLOCK_HZ                      1
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_SPI_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_spi_capability_t vsf_real_spi_capability(vsf_real_spi_t *spi_ptr)
{
    vsf_spi_capability_t spi_capability = {
        .support_auto_cs    = VSF_SPI_CFG_CAPABILITY_SUPPORT_AUTO_CS,
        .support_manual_cs  = VSF_SPI_CFG_CAPABILITY_SUPPORT_MANUAL_CS,
        .cs_count           = VSF_SPI_CFG_CAPABILITY_CS_COUNT,
        .max_clock_hz       = VSF_SPI_CFG_CAPABILITY_MAX_CLOCK_HZ,
        .min_clock_hz       = VSF_SPI_CFG_CAPABILITY_MIN_CLOCK_HZ,
    };

    return spi_capability;
}
#endif

/*============================ MACROS ========================================*/

#undef VSF_SPI_CFG_CAPABILITY_SUPPORT_AUTO_CS
#undef VSF_SPI_CFG_CAPABILITY_SUPPORT_MANUAL_CS
#undef VSF_SPI_CFG_CAPABILITY_CS_COUNT
#undef VSF_SPI_CFG_CAPABILITY_MAX_CLOCK_HZ
#undef VSF_SPI_CFG_CAPABILITY_MIN_CLOCK_HZ
#undef vsf_real_spi_t
#undef vsf_real_spi_capability

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                   _spi
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME            _SPI

#ifndef VSF_SPI_CFG_IMP_PREFIX
#   error "Please define VSF_SPI_CFG_IMP_PREFIX in spi driver"
#endif

#ifndef VSF_SPI_CFG_IMP_UPCASE_PREFIX
#   error "Please define VSF_SPI_CFG_IMP_UPCASE_PREFIX in spi driver"
#endif

#ifndef VSF_SPI_CFG_IMP_COUNT_MASK_PREFIX
#   define VSF_SPI_CFG_IMP_COUNT_MASK_PREFIX        VSF_SPI_CFG_IMP_UPCASE_PREFIX
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_SPI_CFG_IMP_PREFIX
#undef VSF_SPI_CFG_IMP_COUNT_MASK_PREFIX
#undef VSF_SPI_CFG_IMP_UPCASE_PREFIX
#undef VSF_SPI_CFG_IMP_LV0

#endif  /* VSF_HAL_USE_SPI */

