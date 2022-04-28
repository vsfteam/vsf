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

#ifndef __HAL_DRIVER_AIC8800_SPI_H__
#define __HAL_DRIVER_AIC8800_SPI_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_SPI == ENABLED

#include "../__device.h"

/*============================ MACROS ========================================*/

#define VSF_SPI_CFG_REIMPLEMENT_MODE            ENABLED

#define SPI_DATASIZE_TO_BYTE(__S)               \
    (((((__S) & SPI_DATASIZE_MASK) >> 2) + 7) / 8)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum em_spi_mode_t {
    SPI_SLAVE                        = 0x00ul << 11,
    SPI_MASTER                       = 0x01ul << 11,
    SPI_DIR_MODE_MASK                = SPI_SLAVE | SPI_MASTER,

    SPI_MODE_0                       = ((0x01ul << 0) |(0x01ul << 1) | (0x00ul << 13)),
    SPI_MODE_1                       = ((0x00ul << 0) |(0x00ul << 1) | (0x00ul << 13)),
    SPI_MODE_2                       = ((0x01ul << 0) |(0x01ul << 1) | (0x01ul << 13)),
    SPI_MODE_3                       = ((0x00ul << 0) |(0x00ul << 1) | (0x01ul << 13)),
    SPI_CLOCK_MODE_MASK              =  SPI_MODE_0 | SPI_MODE_1
                                      | SPI_MODE_2 | SPI_MODE_3,

    SPI_DATASIZE_8                   = 0x08ul << 2,      //!< datasize is 8 bits
    SPI_DATASIZE_9                   = 0x09ul << 2,
    SPI_DATASIZE_10                  = 0x0aul << 2,
    SPI_DATASIZE_11                  = 0x0Bul << 2,
    SPI_DATASIZE_12                  = 0x0Cul << 2,
    SPI_DATASIZE_13                  = 0x0Dul << 2,
    SPI_DATASIZE_14                  = 0x0Eul << 2,
    SPI_DATASIZE_15                  = 0x0Ful << 2,
    SPI_DATASIZE_16                  = 0x10ul << 2,
    SPI_DATASIZE_17                  = 0x11ul << 2,
    SPI_DATASIZE_18                  = 0x12ul << 2,
    SPI_DATASIZE_19                  = 0x13ul << 2,
    SPI_DATASIZE_20                  = 0x14ul << 2,
    SPI_DATASIZE_21                  = 0x15ul << 2,
    SPI_DATASIZE_22                  = 0x16ul << 2,
    SPI_DATASIZE_23                  = 0x17ul << 2,
    SPI_DATASIZE_24                  = 0x18ul << 2,
    SPI_DATASIZE_25                  = 0x19ul << 2,
    SPI_DATASIZE_26                  = 0x1aul << 2,
    SPI_DATASIZE_27                  = 0x1Bul << 2,
    SPI_DATASIZE_28                  = 0x1Cul << 2,
    SPI_DATASIZE_29                  = 0x1Dul << 2,
    SPI_DATASIZE_30                  = 0x1Eul << 2,
    SPI_DATASIZE_31                  = 0x1Ful << 2,
    SPI_DATASIZE_32                  = 0x20ul << 2,
    SPI_DATASIZE_MASK                = 0x3Ful << 2,

    SPI_AUTO_CS_DISABLE              = 0x01ul << 8,
    SPI_AUTO_CS_ENABLE               = 0x00ul << 8,
    SPI_AUTO_CS_MASK                 = SPI_AUTO_CS_DISABLE | SPI_AUTO_CS_ENABLE,

    SPI_MSB_FIRST                    = 0x00ul << 9,
    SPI_LSB_FIRST                    = 0x01ul << 9,
    SPI_BIT_ORDER_MASK               = SPI_MSB_FIRST | SPI_LSB_FIRST,


    __SPI_HW_MODE_MASK               =  SPI_DIR_MODE_MASK
                                      | SPI_CLOCK_MODE_MASK
                                      | SPI_BIT_ORDER_MASK
                                      | SPI_AUTO_CS_MASK,

    SPI_MODE_MASK                    = __SPI_HW_MODE_MASK | SPI_DATASIZE_MASK,
} em_spi_mode_t;

/*============================ INCLUDES ======================================*/

#define VSF_SPI_CFG_API_DECLARATION_PREFIX          vsf_hw
#define VSF_SPI_CFG_INSTANCE_DECLARATION_PREFIX     VSF_HW
#include "hal/driver/common/spi/spi_template.h"

#endif  /*  VSF_HAL_USE_SPI */
#endif  /* EOF */
