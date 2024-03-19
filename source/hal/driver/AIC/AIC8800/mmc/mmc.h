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

#ifndef __HAL_DRIVER_AIC8800_HW_MMC_H__
#define __HAL_DRIVER_AIC8800_HW_MMC_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_MMC == ENABLED

#include "../__device.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vsf_mmc_transop_t {
    MMC_CMDOP_SINGLE_BLOCK              = (1ul << 8),   // MMC_CMDOP_RW
    MMC_CMDOP_MULTI_BLOCK               = (1ul << 8) | (1ul << 10) | (1ul << 16),
                                                        // MMC_CMDOP_RW | SDMMC_MULTI_BLOCK_MODE | SDMMC_AUTOCMD12_ENABLE
    MMC_CMDOP_WRITE                     = (1ul << 9),   // SDMMC_WRITE_MODE
    MMC_CMDOP_RESP                      = (1ul << 4),   // SDMMC_RESPONSE_ENABLE
    MMC_CMDOP_RESP_SHORT                = (1ul << 5),   // SDMMC_CONFIG_R3
    MMC_CMDOP_RESP_SHORT_CRC            = (0ul << 5),   // SDMMC_CONFIG_Rx
    MMC_CMDOP_RESP_LONG_CRC             = (2ul << 5),   // SDMMC_CONFIG_R2
} vsf_mmc_transop_t;

typedef enum vsf_mmc_irq_mask_t {
    MMC_IRQ_MASK_HOST_RESP_DONE         = (0x1ul << 9), // SDMMC_RESP_DONE_FLAG
    MMC_IRQ_MASK_HOST_DATA_DONE         = (0x1ul << 4), // SDMMC_DATA_DONE_FLAG
    MMC_IRQ_MASK_HOST_ALL               = MMC_IRQ_MASK_HOST_RESP_DONE
                                        | MMC_IRQ_MASK_HOST_DATA_DONE,
} vsf_mmc_irq_mask_t;

typedef enum vsf_mmc_transact_status_t {
    MMC_TRANSACT_STATUS_DONE            = 0,
    MMC_TRANSACT_STATUS_ERR_RESP_NONE   = (0x1ul << 9),
    MMC_TRANSACT_STATUS_ERR_RESP_CRC    = (0x1ul << 8),
    MMC_TRANSACT_STATUS_ERR_DATA_CRC    = (0xFFul << 16),
    MMC_TRANSACT_STATUS_DATA_BUSY       = (0x1ul << 2),
    MMC_TRANSACT_STATUS_BUSY            = (0x1ul << 1),
    MMC_TRANSACT_STATUS_ERR_MASK        = MMC_TRANSACT_STATUS_ERR_RESP_NONE
                                        | MMC_TRANSACT_STATUS_ERR_RESP_CRC
                                        | MMC_TRANSACT_STATUS_ERR_DATA_CRC,
} vsf_mmc_transact_status_t;

/*============================ INCLUDES ======================================*/

#endif /* VSF_HAL_USE_MMC */
#endif /* __HAL_DRIVER_AIC8800_HW_MMC_H__ */
