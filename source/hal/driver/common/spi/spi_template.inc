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

#if VSF_SPI_CFG_IMPLEMENT_OP == ENABLED
#   define VSF_SPI_OP_VAR           VSF_MCONNECT(__, VSF_SPI_CFG_IMP_PREFIX, _spi_op)
#   define VSF_SPI_OP               .vsf_spi.op = & VSF_SPI_OP_VAR,
#else
#   define VSF_SPI_OP
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

#if VSF_SPI_CFG_IMPLEMENT_OP == ENABLED

#   undef VSF_SPI_API
#   define  VSF_SPI_API(__prefix_name, __return, __name, ...)                   \
        VSF_TEMPLATE_HAL_API_OP(__prefix_name, _spi_, __return, __name, __VA_ARGS__)

static const vsf_spi_op_t VSF_SPI_OP_VAR = {
    VSF_SPI_APIS(VSF_SPI_CFG_IMP_PREFIX)
};

#endif

/*============================ IMPLEMENTATION ================================*/

spi_capability_t vsf_spi_capability(vsf_spi_t *spi_ptr)
{
    spi_capability_t spi_capability = {
        .cs_count = VSF_SPI_CFG_CS_COUNT,
    };

    return spi_capability;
}

/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_MCONNECT(VSF_SPI_CFG_IMP_UPPERCASE_PREFIX, _SPI_COUNT)
#   define __VSF_SPI_IMP_COUNT VSF_MCONNECT(VSF_SPI_CFG_IMP_UPPERCASE_PREFIX, _SPI_COUNT)

#   if VSF_MCONNECT(VSF_SPI_CFG_IMP_UPPERCASE_PREFIX, _SPI_MASK)
#       define __VSF_SPI_IMP_MASK    VSF_MCONNECT(VSF_SPI_CFG_IMP_UPPERCASE_PREFIX, _SPI_MASK)
#   else
#       define __VSF_SPI_IMP_MASK    ((1ul << __VSF_SPI_IMP_COUNT) - 1)
#   endif

#   if __VSF_SPI_IMP_MASK & (1 << 0)
        VSF_SPI_CFG_IMP_LV0(0, NULL)
#   endif
#   if __VSF_SPI_IMP_MASK & (1 << 1)
        VSF_SPI_CFG_IMP_LV0(1, NULL)
#   endif
#   if __VSF_SPI_IMP_MASK & (1 << 2)
        VSF_SPI_CFG_IMP_LV0(2, NULL)
#   endif
#   if __VSF_SPI_IMP_MASK & (1 << 3)
        VSF_SPI_CFG_IMP_LV0(3, NULL)
#   endif
#   if __VSF_SPI_IMP_MASK & (1 << 4)
        VSF_SPI_CFG_IMP_LV0(4, NULL)
#   endif
#   if __VSF_SPI_IMP_MASK & (1 << 5)
        VSF_SPI_CFG_IMP_LV0(5, NULL)
#   endif
#   if __VSF_SPI_IMP_MASK & (1 << 6)
        VSF_SPI_CFG_IMP_LV0(6, NULL)
#   endif
#   if __VSF_SPI_IMP_MASK & (1 << 7)
        VSF_SPI_CFG_IMP_LV0(7, NULL)
#   endif

#   undef __VSF_SPI_IMP_COUNT
#   undef __VSF_SPI_IMP_MASK
#   undef VSF_SPI_CFG_IMP_LV0
#endif


#endif // VSF_HAL_USE_SPI == ENABLED
