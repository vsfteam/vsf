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

#ifndef __HAL_DRIVER_RP2040_SPI_H__
#define __HAL_DRIVER_RP2040_SPI_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_SPI == ENABLED

#include "../__device.h"

/*============================ MACROS ========================================*/

#define VSF_SPI_CFG_REIMPLEMENT_TYPE_MODE         ENABLED

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if VSF_SPI_CFG_REIMPLEMENT_TYPE_MODE == ENABLED
typedef enum vsf_spi_mode_t {
    VSF_SPI_MASTER                  = 0x00ul << 0,
    VSF_SPI_SLAVE                   = 0x01ul << 0,
    VSF_SPI_MSB_FIRST               = 0x00ul << 1,
    VSF_SPI_LSB_FIRST               = 0x01ul << 1,

    /* CPOL/CPHA encoded at PL022 CR0 bit positions (6/7) for direct mask use */
    VSF_SPI_CPOL_LOW                = 0x00,
    VSF_SPI_CPOL_HIGH               = (1u << 6),    /* PL022 SPO */
    VSF_SPI_CPHA_LOW                = 0x00,
    VSF_SPI_CPHA_HIGH               = (1u << 7),    /* PL022 SPH */
    VSF_SPI_MODE_0                  = VSF_SPI_CPOL_LOW  | VSF_SPI_CPHA_LOW,
    VSF_SPI_MODE_1                  = VSF_SPI_CPOL_LOW  | VSF_SPI_CPHA_HIGH,
    VSF_SPI_MODE_2                  = VSF_SPI_CPOL_HIGH | VSF_SPI_CPHA_LOW,
    VSF_SPI_MODE_3                  = VSF_SPI_CPOL_HIGH | VSF_SPI_CPHA_HIGH,

    VSF_SPI_CS_SOFTWARE_MODE        = 0x00ul << 4,
    VSF_SPI_CS_HARDWARE_MODE        = 0x01ul << 4,

    VSF_SPI_DATASIZE_8              = 0x00ul << 8,
    VSF_SPI_DATASIZE_16             = 0x01ul << 8,
    VSF_SPI_DATASIZE_32             = 0x02ul << 8,
} vsf_spi_mode_t;

/* Masks (VSF_SPI_DIR_MODE_MASK, VSF_SPI_MODE_MASK, etc.) are defined by the
 * template's anonymous enum after vsf_spi_mode_t is reimplemented. */
#endif

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

#endif      /* VSF_HAL_USE_SPI */
#endif      /* __HAL_DRIVER_RP2040_SPI_H__ */
/* EOF */
