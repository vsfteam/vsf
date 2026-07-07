/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software       *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "./vsf_wifi_mt76.h"
#include "./vsf_wifi_mt76_priv.h"

#if VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_MT76 == ENABLED

#include "../../vsf_wifi_priv.h"

/*============================ FORWARD DECLARATIONS ==========================*/

static void __mt76_fw_finish(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_next(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_power_on_rf_step(vsf_wifi_t *wifi);
static void __mt76_fw_send_start(vsf_wifi_t *wifi);
static void __mt76_fw_send_chunk(vsf_wifi_t *wifi);
static void __mt76_fw_send_fce_addr_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_send_fce_len_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_send_bulk_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_send_idx_read_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_send_idx_write_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_poll_patch_start(vsf_wifi_t *wifi);
static void __mt76_fw_poll_patch_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_poll_fw_ready_start(vsf_wifi_t *wifi);
static void __mt76_fw_poll_fw_ready_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_acquire_sem_start(vsf_wifi_t *wifi);
static void __mt76_fw_acquire_sem_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_release_sem(vsf_wifi_t *wifi);
static void __mt76_fw_parse_header(vsf_wifi_t *wifi);

/*============================ IMPLEMENTATION ================================*/

/*============================ Firmware load =================================*/

#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
static void __mt76_fw_timer(vsf_callback_timer_t *timer)
{
    mt76_wifi_priv_t *priv = vsf_container_of(timer, mt76_wifi_priv_t, fw_timer);
    __mt76_fw_next(priv->wifi, VSF_ERR_NONE);
}

static void __mt76_fw_timer_start(vsf_wifi_t *wifi, uint32_t ms)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_callback_timer_init(&priv->fw_timer);
    priv->fw_timer.on_timer = __mt76_fw_timer;
    vsf_callback_timer_add_ms(&priv->fw_timer, ms);
}
static void __mt76_xtal_timer(vsf_callback_timer_t *timer)
{
    mt76_wifi_priv_t *priv = vsf_container_of(timer, mt76_wifi_priv_t, fw_timer);
    __mt76_mac_fixup_xtal_continue(priv->wifi, VSF_ERR_NONE);
}

void __mt76_xtal_timer_start(vsf_wifi_t *wifi, uint32_t ms)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_callback_timer_init(&priv->fw_timer);
    priv->fw_timer.on_timer = __mt76_xtal_timer;
    vsf_callback_timer_add_ms(&priv->fw_timer, ms);
}
#else
static void __mt76_fw_timer_start(vsf_wifi_t *wifi, uint32_t ms)
{
    (void)wifi; (void)ms;
}

void __mt76_xtal_timer_start(vsf_wifi_t *wifi, uint32_t ms)
{
    (void)wifi; (void)ms;
    __mt76_mac_fixup_xtal_continue(wifi, VSF_ERR_NONE);
}
#endif

static void __mt76_fw_finish(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_wifi_done_t done = priv->pending_done;

    priv->pending_done = NULL;
    priv->fw_state     = MT76_FW_STATE_DONE;
    priv->fw_send_state= MT76_FW_SEND_IDLE;

    if (err != VSF_ERR_NONE) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: firmware load failed (%d)" VSF_TRACE_CFG_LINEEND, err);
    } else {
        vsf_wifi_chip_mt76_trace_info(
            "mt76: firmware load completed" VSF_TRACE_CFG_LINEEND);
    }

    if (done != NULL) {
        done(wifi, err);
    }
}

bool __mt76_chip_id_valid(uint32_t asic_rev)
{
    uint16_t chip_id = (uint16_t)(asic_rev >> 16);
    return (chip_id == MT76_CHIP_ID_7612) ||
           (chip_id == MT76_CHIP_ID_7632) ||
           (chip_id == MT76_CHIP_ID_7662) ||
           (chip_id == MT76_CHIP_ID_7602);
}

static void __mt76_fw_reset_wlan_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
        return;
    }

    uint32_t val = priv->fw_idx;
    val &= ~MT76_WLAN_FUN_CTRL_FRC_WL_ANT_SEL;

    if (val & MT76_WLAN_FUN_CTRL_WLAN_EN) {
        val |= MT76_WLAN_FUN_CTRL_WLAN_RESET_RF;
        priv->fw_state = MT76_FW_STATE_RESET_WLAN_RF;
        err = __mt76_cfg_write(wifi, MT76_WLAN_FUN_CTRL, val,
                               __mt76_fw_next);
    } else {
        priv->fw_state = MT76_FW_STATE_RESET_WLAN_EN;
        err = __mt76_cfg_write(wifi, MT76_WLAN_FUN_CTRL, val,
                               __mt76_fw_next);
    }

    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
    }
}

static void __mt76_fw_power_on_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
        return;
    }

    uint32_t val = priv->fw_idx;
    uint32_t expected = MT76_WLAN_MTC_CTRL_MTCMOS_PWR_UP |
                        MT76_WLAN_MTC_CTRL_PWR_ACK |
                        MT76_WLAN_MTC_CTRL_PWR_ACK_S |
                        MT76_WLAN_MTC_CTRL_STATE_UP;

    if ((val & expected) == expected) {
        priv->fw_state = MT76_FW_STATE_POWER_ON_MTCMOS_CLEAR1;
        __mt76_fw_next(wifi, VSF_ERR_NONE);
    } else {
        /* Retry after 1 ms. */
        __mt76_fw_timer_start(wifi, 1);
    }
}

static void __mt76_fw_wait_mac_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
        return;
    }

    uint32_t val = priv->fw_idx;
    if (val != 0 && val != 0xFFFFFFFF) {
        priv->fw_state = MT76_FW_STATE_READ_ASIC_REV;
        __mt76_fw_next(wifi, VSF_ERR_NONE);
    } else {
        if (priv->fw_poll_ms == 0) {
            vsf_wifi_chip_mt76_trace_error(
                "mt76: MAC did not become ready" VSF_TRACE_CFG_LINEEND);
            __mt76_fw_finish(wifi, VSF_ERR_FAIL);
            return;
        }
        priv->fw_poll_ms--;
        __mt76_fw_timer_start(wifi, 5);
    }
}

static void __mt76_fw_power_on_rf_continue(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
        return;
    }
    __mt76_fw_power_on_rf_step(wifi);
}

static void __mt76_fw_power_on_rf_step(vsf_wifi_t *wifi)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    uint32_t shift = priv->fw_rf_unit ? 8 : 0;
    vsf_err_t err = VSF_ERR_NONE;

    vsf_wifi_chip_mt76_trace_info(
        "mt76: power_on_rf unit=%u step=%u" VSF_TRACE_CFG_LINEEND,
        priv->fw_rf_unit, priv->fw_rf_step);

    switch (priv->fw_rf_step) {
    case 0:
        priv->fw_rf_step = 1;
        err = __mt76_cfg_read(wifi, 0x40000130, &priv->fw_idx,
                              __mt76_fw_power_on_rf_continue);
        break;
    case 1:
        priv->fw_idx |= (1U << 0) << shift;
        priv->fw_rf_step = 2;
        err = __mt76_cfg_write(wifi, 0x40000130, priv->fw_idx,
                               __mt76_fw_power_on_rf_continue);
        break;
    case 2:
        priv->fw_rf_step = 3;
        __mt76_fw_timer_start(wifi, 1);
        break;
    case 3:
        priv->fw_rf_step = 4;
        err = __mt76_cfg_read(wifi, 0x40000130, &priv->fw_idx,
                              __mt76_fw_power_on_rf_continue);
        break;
    case 4: {
        uint32_t val = ((1U << 1) | (1U << 3) | (1U << 4) | (1U << 5)) << shift;
        priv->fw_idx |= val;
        priv->fw_rf_step = 5;
        err = __mt76_cfg_write(wifi, 0x40000130, priv->fw_idx,
                               __mt76_fw_power_on_rf_continue);
        break;
    }
    case 5:
        priv->fw_rf_step = 6;
        __mt76_fw_timer_start(wifi, 1);
        break;
    case 6:
        priv->fw_rf_step = 7;
        err = __mt76_cfg_read(wifi, 0x40000130, &priv->fw_idx,
                              __mt76_fw_power_on_rf_continue);
        break;
    case 7:
        priv->fw_idx &= ~((1U << 2) << shift);
        priv->fw_rf_step = 8;
        err = __mt76_cfg_write(wifi, 0x40000130, priv->fw_idx,
                               __mt76_fw_power_on_rf_continue);
        break;
    case 8:
        priv->fw_rf_step = 9;
        __mt76_fw_timer_start(wifi, 1);
        break;
    case 9:
        priv->fw_rf_step = 10;
        err = __mt76_cfg_read(wifi, 0x40000130, &priv->fw_idx,
                              __mt76_fw_power_on_rf_continue);
        break;
    case 10:
        priv->fw_idx |= (1U << 0) | (1U << 16);
        priv->fw_rf_step = 11;
        err = __mt76_cfg_write(wifi, 0x40000130, priv->fw_idx,
                               __mt76_fw_power_on_rf_continue);
        break;
    case 11:
        priv->fw_rf_step = 12;
        __mt76_fw_timer_start(wifi, 1);
        break;
    case 12:
        priv->fw_rf_step = 13;
        err = __mt76_cfg_read(wifi, 0x4000001c, &priv->fw_idx,
                              __mt76_fw_power_on_rf_continue);
        break;
    case 13: {
        uint32_t val = priv->fw_idx & ~0xffU;
        val |= 0x30;
        priv->fw_rf_step = 14;
        err = __mt76_cfg_write(wifi, 0x4000001c, val,
                               __mt76_fw_power_on_rf_continue);
        break;
    }
    case 14:
        priv->fw_rf_step = 15;
        err = __mt76_cfg_write(wifi, 0x40000014, 0x484f,
                               __mt76_fw_power_on_rf_continue);
        break;
    case 15:
        priv->fw_rf_step = 16;
        __mt76_fw_timer_start(wifi, 1);
        break;
    case 16:
        priv->fw_rf_step = 17;
        err = __mt76_cfg_read(wifi, 0x40000130, &priv->fw_idx,
                              __mt76_fw_power_on_rf_continue);
        break;
    case 17:
        priv->fw_idx |= (1U << 17);
        priv->fw_rf_step = 18;
        err = __mt76_cfg_write(wifi, 0x40000130, priv->fw_idx,
                               __mt76_fw_power_on_rf_continue);
        break;
    case 18:
        priv->fw_rf_step = 19;
        __mt76_fw_timer_start(wifi, 1);
        break;
    case 19:
        priv->fw_rf_step = 20;
        err = __mt76_cfg_read(wifi, 0x40000130, &priv->fw_idx,
                              __mt76_fw_power_on_rf_continue);
        break;
    case 20:
        priv->fw_idx &= ~(1U << 16);
        priv->fw_rf_step = 21;
        err = __mt76_cfg_write(wifi, 0x40000130, priv->fw_idx,
                               __mt76_fw_power_on_rf_continue);
        break;
    case 21:
        priv->fw_rf_step = 22;
        __mt76_fw_timer_start(wifi, 1);
        break;
    case 22:
        priv->fw_rf_step = 23;
        err = __mt76_cfg_read(wifi, 0x4000014c, &priv->fw_idx,
                              __mt76_fw_power_on_rf_continue);
        break;
    case 23: {
        uint32_t val = priv->fw_idx | (1U << 19) | (1U << 20);
        priv->fw_rf_step = 24;
        err = __mt76_cfg_write(wifi, 0x4000014c, val,
                               __mt76_fw_power_on_rf_continue);
        break;
    }
    case 24:
        priv->fw_rf_step = 25;
        err = __mt76_cfg_read(wifi, 0x0530, &priv->fw_idx,
                              __mt76_fw_power_on_rf_continue);
        break;
    case 25: {
        uint32_t val = priv->fw_idx | 0xf;
        priv->fw_rf_step = 26;
        err = __mt76_cfg_write(wifi, 0x0530, val,
                               __mt76_fw_power_on_rf_continue);
        break;
    }
    case 26:
        if (priv->fw_rf_unit == 0) {
            priv->fw_rf_unit = 1;
            priv->fw_rf_step = 0;
            __mt76_fw_power_on_rf_step(wifi);
        } else {
            priv->fw_state = MT76_FW_STATE_WAIT_MAC;
            __mt76_fw_next(wifi, VSF_ERR_NONE);
        }
        return;
    default:
        err = VSF_ERR_BUG;
        break;
    }

    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
    }
}

static void __mt76_fw_read_asic_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
        return;
    }

    priv->asic_rev = priv->fw_idx;
    if (!__mt76_chip_id_valid(priv->asic_rev)) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: unsupported chip 0x%04X" VSF_TRACE_CFG_LINEEND,
            (unsigned)(priv->asic_rev >> 16));
        __mt76_fw_finish(wifi, VSF_ERR_NOT_SUPPORT);
        return;
    }

    vsf_wifi_chip_mt76_trace_info(
        "mt76: ASIC version = 0x%08X" VSF_TRACE_CFG_LINEEND,
        (unsigned)priv->asic_rev);

    /* Patch applied flag location depends on revision. */
    if (__mt76_rev(priv) >= MT76_REV_E3) {
        priv->fw_patch_reg  = MT76_MCU_CLOCK_CTL;
        priv->fw_patch_mask = 0x00000001;
    } else {
        priv->fw_patch_reg  = MT76_MCU_COM_REG0;
        priv->fw_patch_mask = 0x00000002;
    }

    priv->fw_state = MT76_FW_STATE_CHECK_PATCH;
    __mt76_fw_next(wifi, VSF_ERR_NONE);
}

static void __mt76_fw_check_patch_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
        return;
    }

    if (priv->fw_idx & priv->fw_patch_mask) {
        vsf_wifi_chip_mt76_trace_info(
            "mt76: ROM patch already applied" VSF_TRACE_CFG_LINEEND);
        /* The previous firmware is still running (fini is currently a no-op).
         * Skip the entire upload and use the existing runtime firmware. */
        __mt76_fw_finish(wifi, VSF_ERR_NONE);
        return;
    }

    priv->fw_stage = MT76_FW_STAGE_PATCH;
    if (((uint16_t)(priv->asic_rev >> 16)) == MT76_CHIP_ID_7612) {
        priv->fw_state = MT76_FW_STATE_ENABLE_USB_DMA;
    } else {
        priv->fw_state = MT76_FW_STATE_ACQUIRE_SEM;
        priv->fw_sem_ms = 600;
    }
    __mt76_fw_next(wifi, VSF_ERR_NONE);
}

static void __mt76_fw_acquire_sem_start(vsf_wifi_t *wifi)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    if (priv->fw_sem_ms == 0) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: ROM patch semaphore timeout" VSF_TRACE_CFG_LINEEND);
        __mt76_fw_finish(wifi, VSF_ERR_FAIL);
        return;
    }

    vsf_err_t err = __mt76_cfg_read(wifi, MT76_MCU_SEMAPHORE_03,
                                     &priv->fw_idx, __mt76_fw_acquire_sem_done);
    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
    }
}

static void __mt76_fw_acquire_sem_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) { __mt76_fw_finish(wifi, err); return; }

    if (priv->fw_idx & 0x00000001) {
        if (priv->fw_stage == MT76_FW_STAGE_ILM) {
            priv->fw_state = MT76_FW_STATE_RELEASE_SEM;
        } else {
            priv->fw_state = MT76_FW_STATE_ENABLE_USB_DMA;
        }
        __mt76_fw_next(wifi, VSF_ERR_NONE);
        return;
    }

    if (priv->fw_sem_ms > 0) {
        priv->fw_sem_ms--;
    }
    if (priv->fw_sem_ms == 0) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: ROM patch semaphore timeout" VSF_TRACE_CFG_LINEEND);
        __mt76_fw_finish(wifi, VSF_ERR_FAIL);
        return;
    }

    __mt76_fw_timer_start(wifi, 1);
}

static void __mt76_fw_release_sem(vsf_wifi_t *wifi)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    priv->fw_state = MT76_FW_STATE_PARSE_FW_HEADER;
    vsf_err_t err = __mt76_cfg_write(wifi, MT76_MCU_SEMAPHORE_03, 1,
                                      __mt76_fw_next);
    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
    }
}

static void __mt76_fw_parse_header(vsf_wifi_t *wifi)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    const uint8_t *hdr = __mt76_main_firmware_data;
    uint32_t total = __mt76_main_firmware_size;
    uint32_t ilm_len, dlm_len;

    if (total < MT76_FW_HDR_SIZE) {
        __mt76_fw_finish(wifi, VSF_ERR_FAIL);
        return;
    }

    ilm_len = __mt76_get_le32(hdr + 0);
    dlm_len = __mt76_get_le32(hdr + 4);

    if ((MT76_FW_HDR_SIZE + ilm_len + dlm_len) != total) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: firmware size mismatch" VSF_TRACE_CFG_LINEEND);
        __mt76_fw_finish(wifi, VSF_ERR_FAIL);
        return;
    }

    priv->fw_ilm_len = ilm_len;
    priv->fw_dlm_len = dlm_len;
    priv->fw_stage   = MT76_FW_STAGE_ILM;
    priv->fw_state   = MT76_FW_STATE_ENABLE_USB_DMA;

    vsf_wifi_chip_mt76_trace_info(
        "mt76: firmware ILM=%lu DLM=%lu" VSF_TRACE_CFG_LINEEND,
        (unsigned long)ilm_len, (unsigned long)dlm_len);

    __mt76_fw_next(wifi, VSF_ERR_NONE);
}

static void __mt76_fw_send_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
        return;
    }

    priv->fw_state = priv->fw_send_next_state;
    __mt76_fw_next(wifi, VSF_ERR_NONE);
}

static void __mt76_fw_send_start(vsf_wifi_t *wifi)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    const uint8_t *data = NULL;
    uint32_t len = 0;
    uint32_t dst = 0;

    switch (priv->fw_stage) {
    case MT76_FW_STAGE_PATCH:
        if (__mt76_rom_patch_firmware_size <= MT76_PATCH_HDR_SIZE) {
            __mt76_fw_finish(wifi, VSF_ERR_FAIL);
            return;
        }
        data = __mt76_rom_patch_firmware_data + MT76_PATCH_HDR_SIZE;
        len  = __mt76_rom_patch_firmware_size - MT76_PATCH_HDR_SIZE;
        dst  = MT76_ROM_PATCH_OFFSET;
        priv->fw_send_next_state = MT76_FW_STATE_ENABLE_PATCH;
        break;
    case MT76_FW_STAGE_ILM:
        data = __mt76_main_firmware_data + MT76_FW_HDR_SIZE;
        len  = priv->fw_ilm_len;
        dst  = MT76_ILM_OFFSET;
        priv->fw_send_next_state = MT76_FW_STATE_UPLOAD;
        priv->fw_stage           = MT76_FW_STAGE_DLM;
        break;
    case MT76_FW_STAGE_DLM:
        data = __mt76_main_firmware_data + MT76_FW_HDR_SIZE + priv->fw_ilm_len;
        len  = priv->fw_dlm_len;
        dst  = MT76_DLM_OFFSET;
        if (__mt76_rev(priv) >= MT76_REV_E3) {
            dst += MT76_DLM_OFFSET_E3_EXTRA;
        }
        priv->fw_send_next_state = MT76_FW_STATE_LOAD_IVB;
        break;
    default:
        __mt76_fw_finish(wifi, VSF_ERR_BUG);
        return;
    }

    priv->fw_data        = data;
    priv->fw_len         = len;
    priv->fw_pos         = 0;
    priv->fw_dst_offset  = dst;
    priv->fw_max_payload = (priv->fw_stage == MT76_FW_STAGE_PATCH)
                         ? MT76_FW_PATCH_CHUNK_PAYLOAD
                         : MT76_FW_FW_CHUNK_PAYLOAD;
    priv->fw_send_state  = MT76_FW_SEND_CHUNK;

    __mt76_fw_send_chunk(wifi);
}

static void __mt76_fw_send_chunk(vsf_wifi_t *wifi)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    uint32_t left   = priv->fw_len - priv->fw_pos;
    uint32_t chunk  = __mt76_min(left, priv->fw_max_payload);
    uint32_t rounded= __mt76_round_up(chunk, 4);
    uint32_t total  = 4 + rounded + 4;
    uint8_t *buf    = priv->tx_buf;

    VSF_WIFI_ASSERT(total <= sizeof(priv->tx_buf));

    uint32_t info = MT76_MCU_MSG_TYPE_CMD
                  | (((uint32_t)MT76_CPU_TX_PORT & 0x07) << MT76_MCU_MSG_PORT_SHIFT)
                  | (chunk & MT76_MCU_MSG_LEN_MASK);

    __mt76_put_le32(buf, info);
    memcpy(buf + 4, priv->fw_data + priv->fw_pos, chunk);
    memset(buf + 4 + chunk, 0, (rounded - chunk) + 4);

    priv->fw_send_state = MT76_FW_SEND_CHUNK;

    vsf_err_t err = __mt76_fce_write(wifi, MT76_FCE_DMA_ADDR,
                                     priv->fw_dst_offset + priv->fw_pos,
                                     __mt76_fw_send_fce_addr_done);
    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
    }
}

static void __mt76_fw_send_fce_addr_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) { __mt76_fw_finish(wifi, err); return; }

    uint32_t left    = priv->fw_len - priv->fw_pos;
    uint32_t chunk   = __mt76_min(left, priv->fw_max_payload);
    uint32_t rounded = __mt76_round_up(chunk, 4);

    priv->fw_send_state = MT76_FW_SEND_FCE_LEN;
    err = __mt76_fce_write(wifi, MT76_FCE_DMA_LEN, rounded << 16,
                           __mt76_fw_send_fce_len_done);
    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
    }
}

static void __mt76_fw_send_fce_len_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) { __mt76_fw_finish(wifi, err); return; }

    uint32_t left    = priv->fw_len - priv->fw_pos;
    uint32_t chunk   = __mt76_min(left, priv->fw_max_payload);
    uint32_t rounded = __mt76_round_up(chunk, 4);
    uint32_t total   = 4 + rounded + 4;

    priv->fw_send_state = MT76_FW_SEND_BULK;
    err = __mt76_tx_frame(wifi, priv->tx_buf, (uint16_t)total, 0,
                          __mt76_fw_send_bulk_done);
    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
    }
}

static void __mt76_fw_send_bulk_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) { __mt76_fw_finish(wifi, err); return; }

    priv->fw_send_state = MT76_FW_SEND_IDX_READ;
    err = __mt76_cfg_read(wifi, MT76_TX_CPU_FROM_FCE_CPU_DESC_IDX,
                          &priv->fw_idx, __mt76_fw_send_idx_read_done);
    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
    }
}

static void __mt76_fw_send_idx_read_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) { __mt76_fw_finish(wifi, err); return; }

    priv->fw_idx++;
    priv->fw_send_state = MT76_FW_SEND_IDX_WRITE;
    err = __mt76_cfg_write(wifi, MT76_TX_CPU_FROM_FCE_CPU_DESC_IDX,
                           priv->fw_idx, __mt76_fw_send_idx_write_done);
    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
    }
}

static void __mt76_fw_send_idx_write_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) { __mt76_fw_finish(wifi, err); return; }

    uint32_t left   = priv->fw_len - priv->fw_pos;
    uint32_t chunk  = __mt76_min(left, priv->fw_max_payload);
    priv->fw_pos   += chunk;

    if (priv->fw_pos < priv->fw_len) {
        __mt76_fw_send_chunk(wifi);
    } else {
        priv->fw_send_state = MT76_FW_SEND_IDLE;
        __mt76_fw_send_done(wifi, VSF_ERR_NONE);
    }
}

static void __mt76_fw_poll_patch_start(vsf_wifi_t *wifi)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    if (priv->fw_poll_ms == 0) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: ROM patch did not apply" VSF_TRACE_CFG_LINEEND);
        __mt76_fw_finish(wifi, VSF_ERR_FAIL);
        return;
    }

    vsf_err_t err = __mt76_cfg_read(wifi, priv->fw_patch_reg,
                                     &priv->fw_idx, __mt76_fw_poll_patch_done);
    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
    }
}

static void __mt76_fw_poll_patch_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) { __mt76_fw_finish(wifi, err); return; }

    if (priv->fw_idx & priv->fw_patch_mask) {
        priv->fw_state = MT76_FW_STATE_RELEASE_SEM;
        __mt76_fw_next(wifi, VSF_ERR_NONE);
        return;
    }

    if (priv->fw_poll_ms > 0) {
        priv->fw_poll_ms--;
    }
    if (priv->fw_poll_ms == 0) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: ROM patch apply timeout" VSF_TRACE_CFG_LINEEND);
        __mt76_fw_finish(wifi, VSF_ERR_FAIL);
        return;
    }

    __mt76_fw_timer_start(wifi, 1);
}

static void __mt76_fw_poll_fw_ready_start(vsf_wifi_t *wifi)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    if (priv->fw_poll_ms == 0) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: firmware did not start" VSF_TRACE_CFG_LINEEND);
        __mt76_fw_finish(wifi, VSF_ERR_FAIL);
        return;
    }

    vsf_err_t err = __mt76_cfg_read(wifi, MT76_MCU_COM_REG0,
                                     &priv->fw_idx, __mt76_fw_poll_fw_ready_done);
    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
    }
}

static void __mt76_fw_poll_fw_ready_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) { __mt76_fw_finish(wifi, err); return; }

    if (priv->fw_idx & 0x00000001) {
        priv->fw_state = MT76_FW_STATE_SET_COMREG;
        __mt76_fw_next(wifi, VSF_ERR_NONE);
        return;
    }

    if (priv->fw_poll_ms > 0) {
        priv->fw_poll_ms--;
    }
    if (priv->fw_poll_ms == 0) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: firmware start timeout" VSF_TRACE_CFG_LINEEND);
        __mt76_fw_finish(wifi, VSF_ERR_FAIL);
        return;
    }

    __mt76_fw_timer_start(wifi, 1);
}

static void __mt76_fw_next(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_err_t step_err;

    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
        return;
    }

    vsf_wifi_chip_mt76_trace_info(
        "mt76: fw_state=%d" VSF_TRACE_CFG_LINEEND, priv->fw_state);

    switch (priv->fw_state) {
    case MT76_FW_STATE_RESET_WLAN:
        step_err = __mt76_cfg_read(wifi, MT76_WLAN_FUN_CTRL,
                                   &priv->fw_idx, __mt76_fw_reset_wlan_done);
        break;

    case MT76_FW_STATE_RESET_WLAN_RF:
        priv->fw_state = MT76_FW_STATE_RESET_WLAN_EN;
        priv->fw_idx &= ~MT76_WLAN_FUN_CTRL_WLAN_RESET_RF;
        step_err = __mt76_cfg_write(wifi, MT76_WLAN_FUN_CTRL, priv->fw_idx,
                                    __mt76_fw_next);
        break;

    case MT76_FW_STATE_RESET_WLAN_EN: {
        uint32_t val = priv->fw_idx | MT76_WLAN_FUN_CTRL_WLAN_EN |
                       MT76_WLAN_FUN_CTRL_WLAN_CLK_EN;
        priv->fw_state = MT76_FW_STATE_POWER_ON_MTCMOS;
        step_err = __mt76_cfg_write(wifi, MT76_WLAN_FUN_CTRL, val,
                                    __mt76_fw_next);
        break;
    }

    case MT76_FW_STATE_POWER_ON_MTCMOS:
        priv->fw_state = MT76_FW_STATE_POWER_ON_MTCMOS_POLL;
        step_err = __mt76_cfg_write(wifi, MT76_WLAN_MTC_CTRL,
                                    MT76_WLAN_MTC_CTRL_MTCMOS_PWR_UP,
                                    __mt76_fw_next);
        break;

    case MT76_FW_STATE_POWER_ON_MTCMOS_POLL:
        step_err = __mt76_cfg_read(wifi, MT76_WLAN_MTC_CTRL,
                                   &priv->fw_idx, __mt76_fw_power_on_done);
        break;

    case MT76_FW_STATE_POWER_ON_MTCMOS_CLEAR1:
        priv->fw_state = MT76_FW_STATE_POWER_ON_MTCMOS_CLEAR2;
        priv->fw_idx &= ~(0x7fU << 16);
        step_err = __mt76_cfg_write(wifi, MT76_WLAN_MTC_CTRL, priv->fw_idx,
                                    __mt76_fw_next);
        break;

    case MT76_FW_STATE_POWER_ON_MTCMOS_CLEAR2:
        priv->fw_state = MT76_FW_STATE_POWER_ON_MTCMOS_SET2;
        priv->fw_idx &= ~(0xfU << 24);
        step_err = __mt76_cfg_write(wifi, MT76_WLAN_MTC_CTRL, priv->fw_idx,
                                    __mt76_fw_next);
        break;

    case MT76_FW_STATE_POWER_ON_MTCMOS_SET2:
        priv->fw_state = MT76_FW_STATE_POWER_ON_MTCMOS_CLEAR3;
        priv->fw_idx |= (0xfU << 24);
        step_err = __mt76_cfg_write(wifi, MT76_WLAN_MTC_CTRL, priv->fw_idx,
                                    __mt76_fw_next);
        break;

    case MT76_FW_STATE_POWER_ON_MTCMOS_CLEAR3:
        priv->fw_state = MT76_FW_STATE_POWER_ON_AD_DA;
        priv->fw_idx &= 0xfffff000U;
        step_err = __mt76_cfg_write(wifi, MT76_WLAN_MTC_CTRL, priv->fw_idx,
                                    __mt76_fw_next);
        break;

    case MT76_FW_STATE_POWER_ON_AD_DA:
        priv->fw_state = MT76_FW_STATE_POWER_ON_AD_DA_WRITE;
        step_err = __mt76_cfg_read(wifi, 0x40001204,
                                   &priv->fw_idx, __mt76_fw_next);
        break;

    case MT76_FW_STATE_POWER_ON_AD_DA_WRITE:
        priv->fw_state = MT76_FW_STATE_POWER_ON_WLAN_EN;
        step_err = __mt76_cfg_write(wifi, 0x40001204,
                                    priv->fw_idx & ~(1U << 3),
                                    __mt76_fw_next);
        break;

    case MT76_FW_STATE_POWER_ON_WLAN_EN:
        priv->fw_state = MT76_FW_STATE_POWER_ON_WLAN_EN_WRITE;
        step_err = __mt76_cfg_read(wifi, 0x40000080,
                                   &priv->fw_idx, __mt76_fw_next);
        break;

    case MT76_FW_STATE_POWER_ON_WLAN_EN_WRITE:
        priv->fw_state = MT76_FW_STATE_POWER_ON_BBP_RST;
        step_err = __mt76_cfg_write(wifi, 0x40000080,
                                    priv->fw_idx | (1U << 0),
                                    __mt76_fw_next);
        break;

    case MT76_FW_STATE_POWER_ON_BBP_RST:
        priv->fw_state = MT76_FW_STATE_POWER_ON_BBP_RST_WRITE;
        step_err = __mt76_cfg_read(wifi, 0x40000064,
                                   &priv->fw_idx, __mt76_fw_next);
        break;

    case MT76_FW_STATE_POWER_ON_BBP_RST_WRITE:
        priv->fw_state = MT76_FW_STATE_POWER_ON_RF;
        priv->fw_rf_unit = 0;
        priv->fw_rf_step = 0;
        step_err = VSF_ERR_NONE;
        __mt76_fw_power_on_rf_step(wifi);
        break;

    case MT76_FW_STATE_POWER_ON_RF:
        /* Continuation handled by __mt76_fw_power_on_rf_step. */
        __mt76_fw_power_on_rf_step(wifi);
        return;

    case MT76_FW_STATE_WAIT_MAC:
        priv->fw_poll_ms = 100;
        step_err = __mt76_cfg_read(wifi, MT76_MAC_CSR0,
                                   &priv->fw_idx, __mt76_fw_wait_mac_done);
        break;

    case MT76_FW_STATE_READ_ASIC_REV:
        step_err = __mt76_cfg_read(wifi, MT76_ASIC_VERSION,
                                   &priv->fw_idx, __mt76_fw_read_asic_done);
        break;

    case MT76_FW_STATE_CHECK_PATCH:
        step_err = __mt76_cfg_read(wifi, priv->fw_patch_reg,
                                   &priv->fw_idx, __mt76_fw_check_patch_done);
        break;

    case MT76_FW_STATE_ACQUIRE_SEM:
        priv->fw_sem_ms = 600;
        step_err = VSF_ERR_NONE;
        __mt76_fw_acquire_sem_start(wifi);
        break;

    case MT76_FW_STATE_ENABLE_USB_DMA:
        /* Linux issues MT_VEND_DEV_MODE reset before each FCE setup;
         * match its enumeration sequence. */
        priv->fw_state = MT76_FW_STATE_VENDOR_RESET_DELAY;
        step_err = __mt76_dev_cmd(wifi, MT76_VEND_DEV_MODE, 0x1, 0,
                                  __mt76_fw_next);
        break;

    case MT76_FW_STATE_VENDOR_RESET_DELAY:
        priv->fw_state = MT76_FW_STATE_ENABLE_USB_DMA_CFG;
        __mt76_fw_timer_start(wifi, 10);
        step_err = VSF_ERR_NONE;
        break;

    case MT76_FW_STATE_ENABLE_USB_DMA_CFG:
        priv->fw_state = MT76_FW_STATE_FCE_PSE;
        step_err = __mt76_cfg_write(wifi, MT76_USB_U3DMA_CFG,
                                    MT76_USB_DMA_CFG_VAL, __mt76_fw_next);
        break;

    case MT76_FW_STATE_FCE_PSE:
        priv->fw_state = MT76_FW_STATE_FCE_BASE;
        step_err = __mt76_cfg_write(wifi, MT76_FCE_PSE_CTRL, 1, __mt76_fw_next);
        break;

    case MT76_FW_STATE_FCE_BASE:
        priv->fw_state = MT76_FW_STATE_FCE_MAX;
        step_err = __mt76_cfg_write(wifi, MT76_TX_CPU_FROM_FCE_BASE_PTR,
                                    0x400230, __mt76_fw_next);
        break;

    case MT76_FW_STATE_FCE_MAX:
        priv->fw_state = MT76_FW_STATE_FCE_PDMA;
        step_err = __mt76_cfg_write(wifi, MT76_TX_CPU_FROM_FCE_MAX_COUNT,
                                    1, __mt76_fw_next);
        break;

    case MT76_FW_STATE_FCE_PDMA:
        priv->fw_state = MT76_FW_STATE_FCE_SKIP;
        step_err = __mt76_cfg_write(wifi, MT76_FCE_PDMA_GLOBAL_CONF,
                                    0x44, __mt76_fw_next);
        break;

    case MT76_FW_STATE_FCE_SKIP:
        priv->fw_state = MT76_FW_STATE_UPLOAD;
        step_err = VSF_ERR_NONE;
        __mt76_fw_send_start(wifi);
        break;

    case MT76_FW_STATE_UPLOAD:
        step_err = VSF_ERR_NONE;
        __mt76_fw_send_start(wifi);
        break;

    case MT76_FW_STATE_ENABLE_PATCH: {
        static const uint8_t enable_patch[] = {
            0x6f, 0xfc, 0x08, 0x01,
            0x20, 0x04, 0x00, 0x00,
            0x00, 0x09, 0x00
        };
        priv->fw_state = MT76_FW_STATE_RESET_WMT;
        step_err = __mt76_dev_class_cmd(wifi, MT76_VEND_DEV_MODE, 0x12, 0,
                    enable_patch, sizeof(enable_patch), __mt76_fw_next);
        break;
    }

    case MT76_FW_STATE_RESET_WMT: {
        static const uint8_t reset_wmt[] = {
            0x6f, 0xfc, 0x05, 0x01,
            0x07, 0x01, 0x00, 0x04
        };
        priv->fw_state = MT76_FW_STATE_WAIT_WMT;
        step_err = __mt76_dev_class_cmd(wifi, MT76_VEND_DEV_MODE, 0x12, 0,
                    reset_wmt, sizeof(reset_wmt), __mt76_fw_next);
        break;
    }

    case MT76_FW_STATE_WAIT_WMT:
        priv->fw_state = MT76_FW_STATE_POLL_PATCH;
        priv->fw_poll_ms = 100;
        __mt76_fw_timer_start(wifi, 20);
        step_err = VSF_ERR_NONE;
        break;

    case MT76_FW_STATE_POLL_PATCH:
        step_err = VSF_ERR_NONE;
        __mt76_fw_poll_patch_start(wifi);
        break;

    case MT76_FW_STATE_RELEASE_SEM:
        step_err = VSF_ERR_NONE;
        __mt76_fw_release_sem(wifi);
        break;

    case MT76_FW_STATE_PARSE_FW_HEADER:
        step_err = VSF_ERR_NONE;
        __mt76_fw_parse_header(wifi);
        break;

    case MT76_FW_STATE_LOAD_IVB:
        priv->fw_state = MT76_FW_STATE_POLL_FW_READY;
        priv->fw_poll_ms = 100;
        step_err = __mt76_dev_cmd(wifi, MT76_VEND_DEV_MODE, 0x12, 0,
                                  __mt76_fw_next);
        break;

    case MT76_FW_STATE_POLL_FW_READY:
        step_err = VSF_ERR_NONE;
        __mt76_fw_poll_fw_ready_start(wifi);
        break;

    case MT76_FW_STATE_SET_COMREG:
        priv->fw_state = MT76_FW_STATE_REENABLE_FCE;
        step_err = __mt76_cfg_write(wifi, MT76_MCU_COM_REG0, 0x02,
                                    __mt76_fw_next);
        break;

    case MT76_FW_STATE_REENABLE_FCE:
        priv->fw_state = MT76_FW_STATE_DONE;
        step_err = __mt76_cfg_write(wifi, MT76_FCE_PSE_CTRL, 1,
                                    __mt76_fw_next);
        break;

    case MT76_FW_STATE_DONE:
        __mt76_fw_finish(wifi, VSF_ERR_NONE);
        return;

    default:
        step_err = VSF_ERR_BUG;
        break;
    }

    if (step_err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, step_err);
    }
}

vsf_err_t __mt76_firmware_load(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    priv->pending_done = done;
    priv->fw_state     = MT76_FW_STATE_RESET_WLAN;
    priv->fw_send_state= MT76_FW_SEND_IDLE;
    priv->mcu_seq      = 0;

    vsf_wifi_chip_mt76_trace_info(
        "mt76: loading firmware" VSF_TRACE_CFG_LINEEND);

    __mt76_fw_next(wifi, VSF_ERR_NONE);
    return VSF_ERR_NONE;
}
#endif      /* VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_MT76 == ENABLED */
