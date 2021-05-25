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

#ifndef __HAL_DRIVER_NUVOTON_M480_SPI_H__
#define __HAL_DRIVER_NUVOTON_M480_SPI_H__
/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "../../__device.h"
#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/spi/reg_spi.h"
#include "./i_reg_spi.h"
#if VSF_HAL_USE_SPI == ENABLED
/*============================ MACROS ========================================*/

#ifndef SPI_AUTO_SLAVE_SELECTION_ENABLE
#   define SPI_AUTO_SLAVE_SELECTION_ENABLE                  DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef enum em_spi_irq_mask_t {
    SPI_IRQ_MASK_TX_CPL = BIT(0),
    SPI_IRQ_MASK_CPL    = BIT(1),   // SPI completed recepiton and transmission
} em_spi_irq_mask_t;

typedef enum em_spi_mode_t {
    SPI_SLAVE                   = 0x00,             //!< select master mode
    SPI_MASTER                  = BIT(0),           //!< select slave mode
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
    spi_reg_t                   *param;
    spi_cfg_t                   cfg;
    em_spi_irq_mask_t           irq_msk;
    spi_status_t                status;
    bool                        is_able;
} vsf_spi_t;
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/
#endif  /*  VSF_HAL_USE_SPI */
#endif  /* EOF */