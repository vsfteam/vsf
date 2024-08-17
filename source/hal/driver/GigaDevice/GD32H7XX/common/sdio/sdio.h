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

#ifndef __HAL_DRIVER_GIGADEVICE_GD32H7XX_SDIO_H__
#define __HAL_DRIVER_GIGADEVICE_GD32H7XX_SDIO_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_SDIO == ENABLED

#include "../__device.h"

/*============================ MACROS ========================================*/

#define VSF_SDIO_CFG_REIMPLEMENT_TYPE_REQOP     ENABLED
#define VSF_SDIO_CFG_REIMPLEMENT_TYPE_IRQ_MASK  ENABLED
#define VSF_SDIO_CFG_REIMPLEMENT_TYPE_REQSTS    ENABLED

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vsf_sdio_reqop_t {
    // 0 .. 7: DATAEN/DATADIR/TRANSMOD in SDIO.DATACTL
    // 8 .. 15: TREN/TRSTOP/CMDRESP/HOLD in SDIO.CMDCTL shift left by 2

    __SDIO_CMDOP_TREN                   = ((1ul << 6) << 2),
    __SDIO_CMDOP_TRSTOP                 = ((1ul << 7) << 2),
    __SDIO_CMDOP_HOLD                   = ((1ul << 13) << 2),
    __SDIO_CMDOP_DATAEN                 = (1ul << 0) | __SDIO_CMDOP_TREN,
    __SDIO_CMDOP_DATADIR                = (1ul << 1),
    SDIO_CMDOP_BYTE                     = (1ul << 2),
    SDIO_CMDOP_STREAM                   = (2ul << 2),
    SDIO_CMDOP_SINGLE_BLOCK             = (0ul << 2),
    SDIO_CMDOP_MULTI_BLOCK              = (3ul << 2),
    SDIO_CMDOP_READ                     = __SDIO_CMDOP_DATAEN | __SDIO_CMDOP_DATADIR,
    SDIO_CMDOP_WRITE                    = __SDIO_CMDOP_DATAEN,

    // CSMEN in SDIO.CMDCTL
    __SDIO_CMDOP_RESP                   = ((1ul << 12) << 2),
    // CMDRESP in SDIO.CMDCTL
    __SDIO_CMDOP_RESP_NONE              = ((0ul << 8) << 2),
    __SDIO_CMDOP_RESP_SHORT_CRC         = ((1ul << 8) << 2),
    __SDIO_CMDOP_RESP_SHORT             = ((2ul << 8) << 2),
    __SDIO_CMDOP_RESP_LONG_CRC          = ((3ul << 8) << 2),
#define SDIO_RESP_NONE                  __SDIO_CMDOP_RESP_NONE
#define SDIO_RESP_R1                    (__SDIO_CMDOP_RESP | __SDIO_CMDOP_RESP_SHORT_CRC)
#define SDIO_RESP_R1B                   (__SDIO_CMDOP_RESP | __SDIO_CMDOP_RESP_SHORT_CRC | SDIO_CMDOP_RESP_BUSY)
#define SDIO_RESP_R2                    (__SDIO_CMDOP_RESP | __SDIO_CMDOP_RESP_LONG_CRC)
#define SDIO_RESP_R3                    (__SDIO_CMDOP_RESP | __SDIO_CMDOP_RESP_SHORT)
#define SDIO_RESP_R4                    (__SDIO_CMDOP_RESP | __SDIO_CMDOP_RESP_SHORT)
#define SDIO_RESP_R5                    (__SDIO_CMDOP_RESP | __SDIO_CMDOP_RESP_SHORT_CRC)
#define SDIO_RESP_R6                    (__SDIO_CMDOP_RESP | __SDIO_CMDOP_RESP_SHORT_CRC)
#define SDIO_RESP_R7                    (__SDIO_CMDOP_RESP | __SDIO_CMDOP_RESP_SHORT_CRC)

    SDIO_CMDOP_CLKHOLD                  = __SDIO_CMDOP_HOLD,
    SDIO_CMDOP_TRANS_STOP               = __SDIO_CMDOP_TRSTOP,

    // 16 .. : not care
    SDIO_CMDOP_RESP_BUSY                = (1ul << 16),
} vsf_sdio_reqop_t;

typedef enum vsf_sdio_irq_mask_t {
    // CMDSEND/CMDRECV in SDIO.STAT
    SDIO_IRQ_MASK_HOST_RESP_DONE        = (0x1ul << 6) | (1 << 7),
    // DTEND in SDIO.STAT
    SDIO_IRQ_MASK_HOST_DATA_DONE        = (0x1ul << 8),
    // DATABOR in SDIO.STAT
    SDIO_IRQ_MASK_HOST_DATA_ABORT       = (0x1ul << 11),
    SDIO_IRQ_MASK_HOST_ALL              = SDIO_IRQ_MASK_HOST_RESP_DONE
                                        | SDIO_IRQ_MASK_HOST_DATA_DONE
                                        | SDIO_IRQ_MASK_HOST_DATA_ABORT,
} vsf_sdio_irq_mask_t;

typedef enum vsf_sdio_reqsts_t {
    SDIO_REQSTS_DONE                    = 0,
    // CMDTMOUT in SDIO.STAT
    SDIO_REQSTS_ERR_RESP_NONE           = (0x1ul << 2),
    // CCRCERR in SDIO.STAT
    SDIO_REQSTS_ERR_RESP_CRC            = (0x1ul << 0),
    // DTCRCERR in SDIO.STAT
    SDIO_REQSTS_ERR_DATA_CRC            = (0x1ul << 1),
    // DAT0BSY in SDIO.STAT
    SDIO_REQSTS_DATA_BUSY               = (0x1ul << 20),
    // DAT0BSYEND in SDIO.STAT
    SDIO_REQSTS_BUSY                    = (0x1ul << 21),
    SDIO_REQSTS_ERR_MASK                = SDIO_REQSTS_ERR_RESP_NONE
                                        | SDIO_REQSTS_ERR_RESP_CRC
                                        | SDIO_REQSTS_ERR_DATA_CRC,
} vsf_sdio_reqsts_t;

/*============================ INCLUDES ======================================*/

#endif      // VSF_HAL_USE_SDIO
#endif      // __HAL_DRIVER_GIGADEVICE_GD32H7XX_SDIO_H__
