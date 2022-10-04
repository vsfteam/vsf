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

#define __VSF_MMC_PROBE_CLASS_IMPLEMENT
#include "hal/vsf_hal.h"

#if VSF_HAL_USE_MMC == ENABLED

#include "hal/driver/driver.h"

/*============================ MACROS ========================================*/

#define MMC_SEND_IF_COND_CHECK_PATTERN          0xAA

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_mmc_probe_start(vsf_mmc_t *mmc, vsf_mmc_probe_t *probe)
{
    vsf_mmc_host_set_clock(mmc, 400 * 1000);
    vsf_mmc_host_set_buswidth(mmc, 1);
    vsf_mmc_irq_enable(mmc, MMC_IRQ_MASK_HOST_RESP_DONE | MMC_IRQ_MASK_HOST_DATA_DONE);
    probe->state = VSF_MMC_PROBE_STATE_GO_IDLE;
    return vsf_mmc_host_transact_start(mmc, &(vsf_mmc_trans_t){
        .cmd    = MMC_GO_IDLE_STATE,
        .arg    = 0,
        .op     = MMC_GO_IDLE_STATE_OP,
    });
}

vsf_err_t vsf_mmc_probe_irqhandler(vsf_mmc_t *mmc, vsf_mmc_probe_t *probe,
        vsf_mmc_irq_mask_t irq_mask, vsf_mmc_transact_status_t status,
        uint32_t resp[4])
{
    if (status & MMC_TRANSACT_STATUS_ERR_MASK) {
        return VSF_ERR_FAIL;
    }

    vsf_mmc_trans_t trans = { 0 };
    switch (probe->state) {
    case VSF_MMC_PROBE_STATE_GO_IDLE:
        trans.cmd = SD_SEND_IF_COND;
        trans.arg = (((probe->voltage & SD_OCR_VDD_HIGH) != 0) << 8) | MMC_SEND_IF_COND_CHECK_PATTERN;
        trans.op = SD_SEND_IF_COND_OP;
        break;
    case VSF_MMC_PROBE_STATE_SEND_IF_COND:
        return VSF_ERR_NONE;
        break;
    }

    probe->state++;
    if (VSF_ERR_NONE != vsf_mmc_host_transact_start(mmc, &trans)) {
        return VSF_ERR_FAIL;
    }
    return VSF_ERR_NOT_READY;
}

#endif // VSF_HAL_USE_I2C == ENABLED

