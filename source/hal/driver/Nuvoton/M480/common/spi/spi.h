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
#ifndef __HAL_DRIVER_NUVOTON_M480_SPI_H__
#define __HAL_DRIVER_NUVOTON_M480_SPI_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "../../__device.h"

#if VSF_HAL_USE_SPI == ENABLED

#include "hal/driver/common/template/vsf_template_hal_driver.h"

/*============================ MACROS ========================================*/
#define SPI_SSCTL_POS               21

#define VSF_SPI_CFG_REIMPLEMENT_MODE            ENABLED
#define VSF_SPI_CFG_REIMPLEMENT_STATUS          ENABLED
#define VSF_SPI_CFG_REIMPLEMENT_IRQ_MASK        ENABLED

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum em_spi_irq_mask_t {
    SPI_IRQ_MASK_TX_CPL = 1 << 0,
    SPI_IRQ_MASK_CPL    = 1 << 1,   // SPI completed recepiton and transmission
} em_spi_irq_mask_t;

typedef enum em_spi_mode_t {
    // CTL: [1 << 0 ~ 1 << 20]
    SPI_MASTER          = 0x00,
    SPI_SLAVE           = SPI_CTL_SLAVE_Msk,
    SPI_DIR_MODE_MASK   = SPI_SLAVE | SPI_MASTER,

    SPI_MODE_0          = SPI_CTL_TXNEG_Msk,
    SPI_MODE_1          = SPI_CTL_RXNEG_Msk,
    SPI_MODE_2          = SPI_CTL_CLKPOL_Msk | SPI_CTL_RXNEG_Msk,
    SPI_MODE_3          = SPI_CTL_CLKPOL_Msk | SPI_CTL_TXNEG_Msk,
    SPI_CLOCK_MODE_MASK = SPI_MODE_0 | SPI_MODE_1 | SPI_MODE_2 | SPI_MODE_3,

    SPI_MSB_FIRST       = 0,
    SPI_LSB_FIRST       = SPI_CTL_LSB_Msk,
    SPI_BIT_ORDER_MASK  = SPI_MSB_FIRST | SPI_LSB_FIRST,

    SPI_DATASIZE_8      = ( 8 << SPI_CTL_DWIDTH_Pos),
    SPI_DATASIZE_9      = ( 9 << SPI_CTL_DWIDTH_Pos),
    SPI_DATASIZE_10     = (10 << SPI_CTL_DWIDTH_Pos),
    SPI_DATASIZE_11     = (11 << SPI_CTL_DWIDTH_Pos),
    SPI_DATASIZE_12     = (12 << SPI_CTL_DWIDTH_Pos),
    SPI_DATASIZE_13     = (13 << SPI_CTL_DWIDTH_Pos),
    SPI_DATASIZE_14     = (14 << SPI_CTL_DWIDTH_Pos),
    SPI_DATASIZE_15     = (15 << SPI_CTL_DWIDTH_Pos),
    SPI_DATASIZE_16     = (16 << SPI_CTL_DWIDTH_Pos),
    SPI_DATASIZE_17     = (17 << SPI_CTL_DWIDTH_Pos),
    SPI_DATASIZE_18     = (18 << SPI_CTL_DWIDTH_Pos),
    SPI_DATASIZE_19     = (19 << SPI_CTL_DWIDTH_Pos),
    SPI_DATASIZE_20     = (20 << SPI_CTL_DWIDTH_Pos),
    SPI_DATASIZE_21     = (21 << SPI_CTL_DWIDTH_Pos),
    SPI_DATASIZE_22     = (22 << SPI_CTL_DWIDTH_Pos),
    SPI_DATASIZE_23     = (23 << SPI_CTL_DWIDTH_Pos),
    SPI_DATASIZE_24     = (24 << SPI_CTL_DWIDTH_Pos),
    SPI_DATASIZE_25     = (25 << SPI_CTL_DWIDTH_Pos),
    SPI_DATASIZE_26     = (26 << SPI_CTL_DWIDTH_Pos),
    SPI_DATASIZE_27     = (27 << SPI_CTL_DWIDTH_Pos),
    SPI_DATASIZE_28     = (28 << SPI_CTL_DWIDTH_Pos),
    SPI_DATASIZE_29     = (29 << SPI_CTL_DWIDTH_Pos),
    SPI_DATASIZE_30     = (30 << SPI_CTL_DWIDTH_Pos),
    SPI_DATASIZE_31     = (31 << SPI_CTL_DWIDTH_Pos),
    SPI_DATASIZE_32     = ( 0 << SPI_CTL_DWIDTH_Pos),
    SPI_DATASIZE_MASK   = SPI_CTL_DWIDTH_Msk,

    SPI_FULL_DUPLEX     = 0,
    SPI_HALF_DUPLEX     = SPI_CTL_HALFDPX_Msk,
    SPI_DUPLEX_MASK     = SPI_HALF_DUPLEX,

    // This bit is used to select the data input/output direction
    // in half-duplex transfer and Dual/Quad transfer
    SPI_DATA_INPUT_DIRECTION  = 0,
    SPI_DATA_OUTPUT_DIRECTION = SPI_CTL_DATDIR_Msk,

    SPI_RECEIVE_ONLY_MODE_DISABLED = 0,
    SPI_RECEIVE_ONLY_MODE_ENABLED  = SPI_CTL_RXONLY_Msk,

    // SSCTL, [1 << 21 ~ 1 << 24]
    SPI_AUTO_SLAVE_SELECTION     = SPI_SSCTL_AUTOSS_Pos + SPI_SSCTL_POS,
    SPI_AUTO_SLAVE_SELECTION_MSK = 1 << (SPI_SSCTL_AUTOSS_Pos + SPI_SSCTL_POS),

    SPI_AUTO_SLAVE_SELECTION_DISABLE  = 0,
    SPI_AUTO_SLAVE_SELECTION_ENABLE   = SPI_AUTO_SLAVE_SELECTION_MSK,


    SPI_SLAVE_SELECTION_ACTIVE_LOW    = 0,
    SPI_SLAVE_SELECTION_ACTIVE_HIGH   = 1 << (SPI_SSCTL_SSACTPOL_Pos + SPI_SSCTL_POS),
} em_spi_mode_t;

typedef struct spi_status_t {
    union {
        inherit(peripheral_status_t)
        uint32_t                value;
    };
} spi_status_t;

/*============================ INCLUDES ======================================*/

#define VSF_SPI_CFG_API_DECLARATION_PREFIX          vsf_hw
#define VSF_SPI_CFG_INSTANCE_DECLARATION_PREFIX     VSF_HW
#include "hal/driver/common/template/vsf_template_spi.h"

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif
#endif
