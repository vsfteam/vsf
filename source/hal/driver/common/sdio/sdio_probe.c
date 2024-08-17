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

#define __VSF_SDIO_PROBE_CLASS_IMPLEMENT
#include "hal/vsf_hal.h"

#if VSF_HAL_USE_SDIO == ENABLED

#include "hal/driver/driver.h"

// in case SDIO is not supported by current platform, include below to avoid compile error
#include "hal/driver/common/template/vsf_template_sdio.h"

#include "./sdio_probe.h"

/*============================ MACROS ========================================*/

#define SDIO_SEND_IF_COND_CHECK_PATTERN          0xAA

// OCR bits
#define SD_ACMD41_ARG_OCR_S18R      (0x01UL << 24)
#define SD_ACMD41_ARG_OCR_HCS       (0x01UL << 30)
#define SD_ACMD41_RSP_OCR_S18A      (0x01UL << 24)
#define SD_ACMD41_RSP_OCR_CCS       (0x01UL << 30)
#define SD_ACMD41_RSP_OCR_BUSY      (0x01UL << 31)
#define SDIO_CMD1_RSP_OCR_BUSY      (0x01UL << 31)
#define SDIO_OCR_HVOLTAGE_MASK      0x00FF8000  // High voltage
#define SDIO_OCR_LVOLTAGE_MASK      0x00007F80  // Low voltage
#define SDIO_OCR_DVOLTAGE_MASK      (SDIO_OCR_HVOLTAGE_MASK | SDIO_OCR_LVOLTAGE_MASK) // Dual voltage
#define SDIO_OCR_SECTOR_MODE        0x40000000  // Sector mode
#define SDIO_OCR_ACCESS_MODE_MASK   0x60000000  // Access mode

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vsf_sdio_probe_state_t {
    VSF_SDIO_PROBE_STATE_APP_CMD,

    VSF_SDIO_PROBE_STATE_GO_IDLE,
    VSF_SDIO_PROBE_STATE_SEND_IF_COND,

    VSF_SDIO_PROBE_STATE_SEND_OP_COND_APP,
    VSF_SDIO_PROBE_STATE_SEND_OP_COND,
    VSF_SDIO_PROBE_STATE_SEND_OP_COND_DONE,

    VSF_SDIO_PROBE_STATE_MMC_SEND_OP_COND_GO_IDLE,
    VSF_SDIO_PROBE_STATE_MMC_SEND_OP_COND,
    VSF_SDIO_PROBE_STATE_MMC_SEND_OP_COND_DONE,

    VSF_SDIO_PROBE_STATE_ALL_SEND_CID,
    VSF_SDIO_PROBE_STATE_RELATIVE_ADDR,
    VSF_SDIO_PROBE_STATE_SEND_CSD,
    VSF_SDIO_PROBE_STATE_SET_DSR,
    VSF_SDIO_PROBE_STATE_SELECT_CARD,

    VSF_SDIO_PROBE_STATE_SET_BUS_WIDTH,
    VSF_SDIO_PROBE_STATE_SET_BUS_WIDTH_DONE,

    VSF_SDIO_PROBE_STATE_SET_BLOCK_LEN,
    VSF_SDIO_PROBE_STATE_DONE,
} vsf_sdio_probe_state_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_sdio_probe_start(vsf_sdio_t *sdio, vsf_sdio_probe_t *probe)
{
    VSF_HAL_ASSERT(probe != NULL);
    VSF_HAL_ASSERT(probe->voltage != 0);
    VSF_HAL_ASSERT((probe->bus_width == 1) || (probe->bus_width == 4) || (probe->bus_width == 8));

    vsf_sdio_capability_t capability = vsf_sdio_capability(sdio);
    if (probe->working_clock_hz > capability.max_freq_hz) {
        probe->working_clock_hz = capability.max_freq_hz;
    }
    switch (probe->bus_width) {
    case 1: if (!(capability.bus_width & SDIO_CAP_BUS_WIDTH_1)) { return VSF_ERR_INVALID_PARAMETER; } break;
    case 4: if (!(capability.bus_width & SDIO_CAP_BUS_WIDTH_4)) { return VSF_ERR_INVALID_PARAMETER; } break;
    case 8: if (!(capability.bus_width & SDIO_CAP_BUS_WIDTH_8)) { return VSF_ERR_INVALID_PARAMETER; } break;
    }

    vsf_sdio_set_clock(sdio, 400 * 1000, false);
    vsf_sdio_set_bus_width(sdio, 1);
    vsf_sdio_irq_enable(sdio, SDIO_IRQ_MASK_HOST_RESP_DONE);

    probe->state = VSF_SDIO_PROBE_STATE_GO_IDLE;
    probe->rca = 0;
    probe->is_app_cmd = false;
    probe->is_resp_r1 = false;

    return vsf_sdio_host_request(sdio, &(vsf_sdio_req_t){
        .cmd    = MMC_GO_IDLE_STATE,
        .arg    = 0,
        .op     = MMC_GO_IDLE_STATE_OP,
    });
}

vsf_err_t vsf_sdio_probe_irqhandler(vsf_sdio_t *sdio, vsf_sdio_probe_t *probe,
        vsf_sdio_irq_mask_t irq_mask, vsf_sdio_reqsts_t status,
        uint32_t resp[4])
{
    bool is_failed = !!(status & SDIO_REQSTS_ERR_MASK);
    if (is_failed && !probe->is_to_ignore_fail) {
        return VSF_ERR_FAIL;
    }
    probe->is_to_ignore_fail = false;

    // is_resp_r1 is only set if r1 response should be checked by probe->r1_expected_card_status
    if (probe->is_resp_r1) {
        probe->is_resp_r1 = false;
        if ((resp[0] & probe->r1_expected_card_status_mask) != probe->r1_expected_card_status) {
            if (probe->is_to_retry) {
                probe->is_to_retry = false;
                probe->state--;
            } else {
                return VSF_ERR_FAIL;
            }
        }
    }
    if (probe->delay_ms > 0) {
        probe->delay_ms = 0;
    }

    vsf_sdio_req_t req = { 0 };
    switch (probe->state) {
    case VSF_SDIO_PROBE_STATE_APP_CMD:
    send_app_cmd:
        req.cmd = MMC_APP_CMD;
        req.arg = probe->rca;
        req.op = MMC_APP_CMD_OP;
        probe->r1_expected_card_status |= R1_APP_CMD;
        probe->r1_expected_card_status_mask |= R1_APP_CMD;
        probe->is_resp_r1 = true;
        break;
    case VSF_SDIO_PROBE_STATE_GO_IDLE:
        req.cmd = SD_SEND_IF_COND;
        req.arg = (((probe->voltage & SD_OCR_VDD_HIGH) != 0) << 8) | SDIO_SEND_IF_COND_CHECK_PATTERN;
        req.op = SD_SEND_IF_COND_OP;
        break;
    case VSF_SDIO_PROBE_STATE_SEND_IF_COND:
        if ((resp[0] & 0xFF) != SDIO_SEND_IF_COND_CHECK_PATTERN) {
            return VSF_ERR_FAIL;
        }
        probe->version = SD_VERSION_2;
        probe->state = VSF_SDIO_PROBE_STATE_SEND_OP_COND_APP;
        // fall through
    case VSF_SDIO_PROBE_STATE_SEND_OP_COND_APP:
        probe->is_to_retry = true;
        goto send_app_cmd;
    case VSF_SDIO_PROBE_STATE_SEND_OP_COND:
        req.cmd = SD_APP_OP_COND;
        req.arg = probe->voltage
                |   (SD_VERSION_2 == probe->version ? SD_ACMD41_ARG_OCR_HCS : 0)
                |   (probe->uhs_en ? SD_ACMD41_ARG_OCR_S18R : 0);
        req.op = SD_APP_OP_COND_OP;
        probe->is_to_ignore_fail = true;
        break;
    case VSF_SDIO_PROBE_STATE_SEND_OP_COND_DONE:
        if (is_failed) {
            // failed with sd_send_op_cond, retry with sdio_mmc_send_op_cond
            probe->delay_ms = 1;
            probe->state = VSF_SDIO_PROBE_STATE_MMC_SEND_OP_COND_GO_IDLE;
            return VSF_ERR_NOT_READY;
        }
        if (!(resp[0] & SD_ACMD41_RSP_OCR_BUSY)) {
            probe->delay_ms = 1;
            probe->state = VSF_SDIO_PROBE_STATE_SEND_OP_COND_APP;
            return VSF_ERR_NOT_READY;
        }

        probe->ocr = resp[0];
        probe->high_capacity = !!(resp[0] & SD_ACMD41_RSP_OCR_CCS);
        probe->rca = 0;

        probe->state = VSF_SDIO_PROBE_STATE_MMC_SEND_OP_COND_DONE;
        goto send_cid;
    case VSF_SDIO_PROBE_STATE_MMC_SEND_OP_COND_GO_IDLE:
//    go_idle:
        req.cmd = MMC_GO_IDLE_STATE;
        req.arg = 0;
        req.op = MMC_GO_IDLE_STATE_OP;
        break;
    case VSF_SDIO_PROBE_STATE_MMC_SEND_OP_COND:
        req.cmd = MMC_SEND_OP_COND;
        req.arg = 0;
        req.op = MMC_SEND_OP_COND_OP;
        probe->is_to_ignore_fail = true;
        break;
    case VSF_SDIO_PROBE_STATE_MMC_SEND_OP_COND_DONE:
        if (is_failed) {
            probe->delay_ms = 1;
            probe->state = VSF_SDIO_PROBE_STATE_MMC_SEND_OP_COND;
            return VSF_ERR_NOT_READY;
        }
        if (!(resp[0] & SDIO_CMD1_RSP_OCR_BUSY)) {
            probe->delay_ms = 1;
            probe->state = VSF_SDIO_PROBE_STATE_MMC_SEND_OP_COND;
            return VSF_ERR_NOT_READY;
        }

        probe->ocr = resp[0];
        probe->version = MMC_VERSION_UNKNOWN;
        probe->high_capacity = ((resp[0] & SDIO_OCR_ACCESS_MODE_MASK) == SDIO_OCR_SECTOR_MODE);
        probe->rca = 1 << 16;

    send_cid:
        req.cmd = MMC_ALL_SEND_CID;
        req.arg = 0;
        req.op = MMC_ALL_SEND_CID_OP;
        break;
    case VSF_SDIO_PROBE_STATE_ALL_SEND_CID:
        probe->cid = *(vsf_sdio_cid_t *)resp;

        req.arg = probe->rca;
        if (IS_MMC(probe->version)) {
            req.cmd = MMC_SET_RELATIVE_ADDR;
            req.op = MMC_SET_RELATIVE_ADDR_OP;
        } else {
            req.cmd = SD_SEND_RELATIVE_ADDR;
            req.op = SD_SEND_RELATIVE_ADDR_OP;
        }
        break;
    case VSF_SDIO_PROBE_STATE_RELATIVE_ADDR:
        if (IS_SD(probe->version)) {
            probe->rca = resp[0] & 0xFFFF0000UL;
        }

        req.cmd = MMC_SEND_CSD;
        req.arg = probe->rca;
        req.op = MMC_SEND_CSD_OP;
        break;
    case VSF_SDIO_PROBE_STATE_SEND_CSD:
        probe->csd = *(vsf_sdio_csd_t *)resp;
        if (IS_MMC(probe->version)) {
            switch (probe->csd.mmc.SPEC_VERS) {
            default:
            case 0:     probe->version = MMC_VERSION_1_2;   break;
            case 1:     probe->version = MMC_VERSION_1_4;   break;
            case 2:     probe->version = MMC_VERSION_2_2;   break;
            case 3:     probe->version = MMC_VERSION_3;     break;
            case 4:     probe->version = MMC_VERSION_4;     break;
            }
        }
        if (IS_MMC(probe->version) || !probe->csd.sd_v2.CSD_STRUCTURE) {
            // MMC and SD V1
            probe->capacity = (probe->csd.sd_v1.C_SIZE + 1) << (probe->csd.sd_v1.C_SIZE_MULT + 2);
        } else {
            // SD V2
            probe->capacity = (probe->csd.sd_v2.C_SIZE + 1) << 10;
        }

        if (probe->csd.sd_v2.DSR_IMP) {
            req.cmd = MMC_SET_DSR;
            req.arg = 0;
            req.op = MMC_SET_DSR_OP;
            break;
        } else {
            probe->state = VSF_SDIO_PROBE_STATE_SET_DSR;
            // fall through
        }
    case VSF_SDIO_PROBE_STATE_SET_DSR:
        req.cmd = MMC_SELECT_CARD;
        req.arg = probe->rca;
        req.op = MMC_SELECT_CARD_OP;
        probe->r1_expected_card_status_mask =
            probe->r1_expected_card_status = R1_READY_FOR_DATA;
        probe->is_resp_r1 = true;
        break;
    case VSF_SDIO_PROBE_STATE_SELECT_CARD:
        if (IS_SD(probe->version)) {
            goto send_app_cmd;
        }
        break;
    case VSF_SDIO_PROBE_STATE_SET_BUS_WIDTH:
        req.cmd = SD_APP_SET_BUS_WIDTH;
        switch (probe->bus_width) {
        case 1:     req.arg = SD_BUS_WIDTH_1; break;
        case 4:     req.arg = SD_BUS_WIDTH_4; break;
        case 8:     req.arg = SD_BUS_WIDTH_8; break;
        }
        req.op = SD_APP_SET_BUS_WIDTH_OP;
        probe->r1_expected_card_status_mask = R1_APP_CMD | R1_READY_FOR_DATA | R1_CUR_STATE(R1_STATE_MASK);
        probe->r1_expected_card_status = R1_APP_CMD | R1_READY_FOR_DATA | R1_CUR_STATE(R1_STATE_TRAN);
        probe->is_resp_r1 = true;
        break;
    case VSF_SDIO_PROBE_STATE_SET_BUS_WIDTH_DONE:
        vsf_sdio_set_bus_width(sdio, probe->bus_width);

        req.cmd = MMC_SET_BLOCKLEN;
        req.arg = 1 << probe->csd.sd_v2.READ_BL_LEN;
        req.op = MMC_SET_BLOCKLEN_OP;
        probe->r1_expected_card_status_mask = R1_READY_FOR_DATA | R1_CUR_STATE(R1_STATE_MASK);
        probe->r1_expected_card_status = R1_READY_FOR_DATA | R1_CUR_STATE(R1_STATE_TRAN);
        probe->is_resp_r1 = true;
        break;
    case VSF_SDIO_PROBE_STATE_SET_BLOCK_LEN:
        vsf_sdio_set_clock(sdio, probe->working_clock_hz, false);
        probe->delay_ms = 1;
        probe->state = VSF_SDIO_PROBE_STATE_DONE;
        return VSF_ERR_NOT_READY;
    case VSF_SDIO_PROBE_STATE_DONE:
        return VSF_ERR_NONE;
    }

    probe->state++;
    probe->is_app_cmd = MMC_APP_CMD == req.cmd;
    if (VSF_ERR_NONE != vsf_sdio_host_request(sdio, &req)) {
        return VSF_ERR_FAIL;
    }
    return VSF_ERR_NOT_READY;
}

#endif // VSF_HAL_USE_I2C == ENABLED

