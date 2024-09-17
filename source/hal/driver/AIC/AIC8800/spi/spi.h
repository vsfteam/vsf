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
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vsf_spi_mode_t {
    VSF_SPI_SLAVE                       = 0x00ul << 11,
    VSF_SPI_MASTER                      = 0x01ul << 11,

    VSF_SPI_MODE_0                      = ((0x01ul << 0) |(0x01ul << 1) | (0x00ul << 13)),
    VSF_SPI_MODE_1                      = ((0x00ul << 0) |(0x00ul << 1) | (0x00ul << 13)),
    VSF_SPI_MODE_2                      = ((0x01ul << 0) |(0x01ul << 1) | (0x01ul << 13)),
    VSF_SPI_MODE_3                      = ((0x00ul << 0) |(0x00ul << 1) | (0x01ul << 13)),

    VSF_SPI_DATASIZE_BIT_OFFSET         = 2,
    VSF_SPI_DATASIZE_DIFF               = 0,
    VSF_SPI_DATASIZE_8                  = 0x08ul << 2,      //!< datasize is 8 bits
    VSF_SPI_DATASIZE_9                  = 0x09ul << 2,
    VSF_SPI_DATASIZE_10                 = 0x0aul << 2,
    VSF_SPI_DATASIZE_11                 = 0x0Bul << 2,
    VSF_SPI_DATASIZE_12                 = 0x0Cul << 2,
    VSF_SPI_DATASIZE_13                 = 0x0Dul << 2,
    VSF_SPI_DATASIZE_14                 = 0x0Eul << 2,
    VSF_SPI_DATASIZE_15                 = 0x0Ful << 2,
    VSF_SPI_DATASIZE_16                 = 0x10ul << 2,
    VSF_SPI_DATASIZE_17                 = 0x11ul << 2,
    VSF_SPI_DATASIZE_18                 = 0x12ul << 2,
    VSF_SPI_DATASIZE_19                 = 0x13ul << 2,
    VSF_SPI_DATASIZE_20                 = 0x14ul << 2,
    VSF_SPI_DATASIZE_21                 = 0x15ul << 2,
    VSF_SPI_DATASIZE_22                 = 0x16ul << 2,
    VSF_SPI_DATASIZE_23                 = 0x17ul << 2,
    VSF_SPI_DATASIZE_24                 = 0x18ul << 2,
    VSF_SPI_DATASIZE_25                 = 0x19ul << 2,
    VSF_SPI_DATASIZE_26                 = 0x1aul << 2,
    VSF_SPI_DATASIZE_27                 = 0x1Bul << 2,
    VSF_SPI_DATASIZE_28                 = 0x1Cul << 2,
    VSF_SPI_DATASIZE_29                 = 0x1Dul << 2,
    VSF_SPI_DATASIZE_30                 = 0x1Eul << 2,
    VSF_SPI_DATASIZE_31                 = 0x1Ful << 2,
    VSF_SPI_DATASIZE_32                 = 0x20ul << 2,
    __AIC8800_VSF_SPI_DATASIZE_MASK     = VSF_SPI_DATASIZE_8  | VSF_SPI_DATASIZE_9  | VSF_SPI_DATASIZE_10 | VSF_SPI_DATASIZE_11 |
                                          VSF_SPI_DATASIZE_12 | VSF_SPI_DATASIZE_13 | VSF_SPI_DATASIZE_14 | VSF_SPI_DATASIZE_15 |
                                          VSF_SPI_DATASIZE_16 | VSF_SPI_DATASIZE_17 | VSF_SPI_DATASIZE_18 | VSF_SPI_DATASIZE_19 |
                                          VSF_SPI_DATASIZE_20 | VSF_SPI_DATASIZE_21 | VSF_SPI_DATASIZE_22 | VSF_SPI_DATASIZE_23 |
                                          VSF_SPI_DATASIZE_24 | VSF_SPI_DATASIZE_25 | VSF_SPI_DATASIZE_26 | VSF_SPI_DATASIZE_27 |
                                          VSF_SPI_DATASIZE_28 | VSF_SPI_DATASIZE_29 | VSF_SPI_DATASIZE_30 | VSF_SPI_DATASIZE_31 |
                                          VSF_SPI_DATASIZE_32,

    VSF_SPI_CS_SOFTWARE_MODE            = 0x01ul << 8,
    VSF_SPI_CS_HARDWARE_MODE            = 0x00ul << 8,

    VSF_SPI_MSB_FIRST                   = 0x00ul << 9,
    VSF_SPI_LSB_FIRST                   = 0x01ul << 9,

    __SPI_HW_MODE_MASK                  = VSF_SPI_SLAVE |
                                          VSF_SPI_MASTER |
                                          VSF_SPI_MODE_0 |
                                          VSF_SPI_MODE_1 |
                                          VSF_SPI_MODE_2 |
                                          VSF_SPI_MODE_3 |
                                          VSF_SPI_CS_SOFTWARE_MODE |
                                          VSF_SPI_CS_HARDWARE_MODE |
                                          VSF_SPI_MSB_FIRST |
                                          VSF_SPI_LSB_FIRST |
                                          __AIC8800_VSF_SPI_DATASIZE_MASK,

    // unsupporte mode
    VSF_SPI_DATASIZE_4                  = 0x04ul << 16,      //!< datasize is 8 bits
    VSF_SPI_DATASIZE_5                  = 0x05ul << 16,
    VSF_SPI_DATASIZE_6                  = 0x06ul << 16,
    VSF_SPI_DATASIZE_7                  = 0x07ul << 16,

    VSF_SPI_LOOP_BACK                   = 0x01ul << 19,
} vsf_spi_mode_t;

/*============================ INCLUDES ======================================*/

#endif  /*  VSF_HAL_USE_SPI */
#endif  /* EOF */
