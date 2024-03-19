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

#ifndef __HAL_DRIVER_AIC8800_I2S_H__
#define __HAL_DRIVER_AIC8800_I2S_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_I2S == ENABLED

#include "../__device.h"

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/

typedef enum vsf_i2s_irq_mask_t {
    VSF_I2S_IRQ_MASK_TX_TGL_BUFFER  = (0x1ul <<  0),
    VSF_I2S_IRQ_MASK_RX_TGL_BUFFER  = (0x1ul <<  1),
} vsf_i2s_irq_mask_t;

typedef enum vsf_i2s_mode_t {
    VSF_I2S_MODE_MASTER             = (1 << 21),        // master mode not tested
    VSF_I2S_MODE_SLAVE              = (0 << 21),

    // todo: how to support LSB mode?
    VSF_I2S_STANDARD_MSB            = 0,
    VSF_I2S_STANDARD_PHILIPS        = (1 << 20) | (1 << 17) | VSF_I2S_STANDARD_MSB,

    VSF_I2S_LRCK_POL                = (1 << 12),
    VSF_I2S_BCK_POL                 = (1 << 13),

    __I2S_HW_FEATURE_MASK           = VSF_I2S_MODE_MASTER | VSF_I2S_MODE_SLAVE |
                                      VSF_I2S_STANDARD_MSB | VSF_I2S_STANDARD_PHILIPS |
                                      VSF_I2S_LRCK_POL | VSF_I2S_BCK_POL,

    // software bits below
    __I2S_DATA_BITLEN_POS           = 24,
    VSF_I2S_DATA_BITLEN_16          = (1 << __I2S_DATA_BITLEN_POS),
    VSF_I2S_DATA_BITLEN_24          = (2 << __I2S_DATA_BITLEN_POS),
    VSF_I2S_DATA_BITLEN_32          = (3 << __I2S_DATA_BITLEN_POS),     // not supported

    __I2S_FRAME_BITLEN_POS          = 26,
    VSF_I2S_FRAME_BITLEN_16         = (1 << __I2S_FRAME_BITLEN_POS),
    VSF_I2S_FRAME_BITLEN_24         = (2 << __I2S_FRAME_BITLEN_POS),
    VSF_I2S_FRAME_BITLEN_32         = (3 << __I2S_FRAME_BITLEN_POS),

    VSF_I2S_MCLK_OUTPUT             = (1 << 28),
    VSF_I2S_STANDARD_LSB            = (1 << 29),
} vsf_i2s_mode_t;

/*============================ INCLUDES ======================================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif /* __HAL_DRIVER_AIC8800_I2S_H__ */
#endif /* EOF */