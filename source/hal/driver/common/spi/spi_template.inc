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
#if VSF_HAL_USE_SPI == ENABLED

#ifndef VSF_SPI_CFG_CS_COUNT
#   define VSF_SPI_CFG_CS_COUNT     1
#endif

#define vsf_real_spi_t          VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_t)
#define vsf_real_spi_capability VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_capability)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_SPI_CFG_REIMPLEMENT_CAPABILITY == DISABLED
spi_capability_t vsf_real_spi_capability(vsf_real_spi_t *spi_ptr)
{
    spi_capability_t spi_capability = {
        0,
    };

    return spi_capability;
}
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define __VSF_HAL_TEMPLATE_NAME                     _spi
#define __VSF_HAL_TEMPLATE_UPCASE_NAME              _SPI

#ifndef VSF_SPI_CFG_PREFIX
#   error "Please define VSF_SPI_CFG_PREFIX in spi driver"
#endif

#ifndef VSF_SPI_CFG_UPPERCASE_PREFIX
#   error "Please define VSF_SPI_CFG_UPPERCASE_PREFIX in spi driver"
#endif

#include "hal/driver/common/template/vsf_template_implementation.h"

#endif // VSF_HAL_USE_SPI == ENABLED
