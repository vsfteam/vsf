/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum em_spi_irq_mask_t {
    SPI_IRQ_MASK_TX_CPL = BIT(0),
    SPI_IRQ_MASK_CPL    = BIT(1),
} em_spi_irq_mask_t;

typedef enum em_spi_mode_t {
    SPI_SLAVE                               = 0x00,
    SPI_MASTER                              = BIT(0),

    SPI_MODE_0                              = 0,
    SPI_MODE_1                              = BIT(1),
    SPI_MODE_2                              = BIT(2),
    SPI_MODE_3                              = SPI_MODE_1 | SPI_MODE_2,

    SPI_DATASIZE_8                          = 0, //todo

    SPI_MSB_FIRST                           = 0,
    SPI_LSB_FIRST                           = BIT(3),

    SPI_AUTO_SLAVE_SELECTION_DISABLE        = 0,
    SPI_AUTO_SLAVE_SELECTION_ENABLE         = BIT(4),

    SPI_SLAVE_SELECTION_ACTIVE_LOW          = 0,
    SPI_SLAVE_SELECTION_ACTIVE_HIGH         = BIT(5),
} em_spi_mode_t;
/*============================ INCLUDES ======================================*/

#include "hal/driver/common/template/vsf_template_spi.h"

struct spi_status_t {
    union {
        inherit(peripheral_status_t)
        uint32_t                value;
    };
};

typedef struct vsf_spi_t {
    REG_SPI_T                   *param;
    spi_cfg_t                   cfg;
    em_spi_irq_mask_t           irq_msk;
    spi_status_t                status;
    struct {
        uint32_t                count;
        uint32_t                off_set_64;
        struct {
            void                *out_buff;
            void                *in_buff;
        } buff;
    } __off_set;
    bool                        is_able;
    bool                        is_busy;
} vsf_spi_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern vsf_spi_t vsf_spi0;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/
#endif  /*  VSF_HAL_USE_SPI */
#endif  /* EOF */