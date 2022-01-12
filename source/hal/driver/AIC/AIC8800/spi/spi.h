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
#include "./i_reg_spi.h"
#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/iomux/reg_iomux.h"

/*============================ MACROS ========================================*/

#define VSF_SPI_REIMPLEMENT_MODE                ENABLED

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum em_spi_mode_t {
    SPI_SLAVE                        = 0x00ul << 0,
    SPI_MASTER                       = 0x01ul << 0,
    SPI_DIR_MODE_MASK                = SPI_SLAVE | SPI_MASTER,

    SPI_MODE_0                       = 0x00ul << 1,
    SPI_MODE_1                       = 0x01ul << 1,
    SPI_MODE_2                       = 0x02ul << 1,
    SPI_MODE_3                       = 0x02ul << 1,
    SPI_CLOCK_MODE_MASK              =  SPI_MODE_0 | SPI_MODE_1
                                      | SPI_MODE_2 | SPI_MODE_3,

    SPI_MSB_FIRST                    = 0x00ul << 3,
    SPI_LSB_FIRST                    = 0x01ul << 3,
    SPI_BIT_ORDER_MASK               = SPI_MSB_FIRST | SPI_LSB_FIRST,

    SPI_AUTO_CS_DISABLE              = 0x00ul << 4,
    SPI_AUTO_CS_ENABLE               = 0x01ul << 4,
    SPI_AUTO_CS_MASK                 = SPI_AUTO_CS_DISABLE | SPI_AUTO_CS_ENABLE,

    //SPI_SLAVE_SELECTION_ACTIVE_LOW   = 0,
    //SPI_SLAVE_SELECTION_ACTIVE_HIGH  = BIT(5),

    __SPI_HW_MODE_MASK               =  SPI_DIR_MODE_MASK
                                      | SPI_CLOCK_MODE_MASK
                                      | SPI_BIT_ORDER_MASK
                                      | SPI_AUTO_CS_MASK,

    SPI_DATASIZE_8                   = 0x00ul << 8, //todo
    SPI_DATASIZE_MASK                = 0x1Ful << 8,

    SPI_MODE_MASK                    = __SPI_HW_MODE_MASK | SPI_DATASIZE_MASK,
} em_spi_mode_t;


typedef struct vsf_hw_spi_t vsf_hw_spi_t;

/*============================ INCLUDES ======================================*/

#include "hal/driver/common/template/vsf_template_spi.h"

/*============================ GLOBAL VARIABLES ==============================*/

extern vsf_hw_spi_t vsf_spi0;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/
#endif  /*  VSF_HAL_USE_SPI */
#endif  /* EOF */