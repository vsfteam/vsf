/*****************************************************************************
 *   Copyright(C)2009-2020 by VSF Team                                       *
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

#ifndef __OSA_HAL_DRIVER_MULTIPLEX_SPI_H__
#define __OSA_HAL_DRIVER_MULTIPLEX_SPI_H__

/*============================ INCLUDES ======================================*/
#include "osa_hal/vsf_osa_hal_cfg.h"

#if VSF_USE_MULTIPLEX_HAL == ENABLED && VSF_USE_MULTIPLEX_SPI == ENABLED

#include "osa_hal/sw_peripheral/io_peripheral/spi/vsf_io_spi.h"
#include "../vsf_multiplex_hal_common.h"

/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file
 *!        included in this file
 */

#if     defined(__VSF_MULTIPLEX_SPI_CLASS_IMPLEMENT)
#   undef __VSF_MULTIPLEX_SPI_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__

#elif   defined(__VSF_MULTIPLEX_SPI_CLASS_INHERIT)
#   undef __VSF_MULTIPLEX_SPI_CLASS_INHERIT
#   define __PLOOC_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vsf_multiplex_spi_t)

def_simple_class(vsf_multiplex_spi_t) {
    public_member(
        implement(vsf_io_spi_t)
    )
    private_member(
        implement(vsf_multiplex_hal_t)
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_multiplex_spi_init(vsf_multiplex_spi_t *spi);
extern vsf_err_t vsf_multiplex_spi_transact(vsf_multiplex_spi_t *spi, uint8_t *tx, uint8_t *rx, uint_fast32_t size);

#endif
#endif

/* EOF */
