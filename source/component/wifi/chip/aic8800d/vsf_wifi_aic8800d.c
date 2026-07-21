/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *                                                                           *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software       *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  either express or implied. See the License for the specific language     *
 *  governing permissions and limitations under the License.                 *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "./vsf_wifi_aic8800d.h"
#include "./vsf_wifi_aic8800d_priv.h"
#include "../../vsf_wifi_priv.h"
#if VSF_WIFI_USE_WPA == ENABLED
#include "../../vsf_wifi_wpa.h"
#endif
#include "service/heap/vsf_heap.h"

#if VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_AIC8800D == ENABLED

/*============================ MACROS ========================================*/

#define AIC8800D_USB_CMD_TYPE       0x11
#define AIC8800D_USB_DATA_TYPE      0x01
#define AIC8800D_USB_DUMMY_LEN      4

/* Default TX power table for AIC8800D80 (matches Linux txpwr_lvl_v3) */
static const struct aic8800d_txpwr_lvl_conf_v3 __aic8800d_txpwr_lvl_v3_default = {
    .enable = 1,
    .pwrlvl_11b_11ag_2g4 = { 20, 20, 20, 20, 20, 20, 20, 20, 18, 18, 16, 16 },
    .pwrlvl_11n_11ac_2g4 = { 20, 20, 20, 20, 18, 18, 16, 16, 16, 16 },
    .pwrlvl_11ax_2g4     = { 20, 20, 20, 20, 18, 18, 16, 16, 16, 16, 15, 15 },
    .pwrlvl_11a_5g       = { 0x80, 0x80, 0x80, 0x80, 20, 20, 20, 20, 18, 18, 16, 16 },
    .pwrlvl_11n_11ac_5g  = { 20, 20, 20, 20, 18, 18, 16, 16, 16, 15 },
    .pwrlvl_11ax_5g      = { 20, 20, 20, 20, 18, 18, 16, 16, 16, 15, 14, 14 },
};

/*============================ FORWARD DECLARATIONS ==========================*/

static uint8_t __aic8800d_freq_to_channel(uint16_t freq);
static void __aic8800d_parse_rsn(const uint8_t *body, uint8_t len,
        vsf_wifi_scan_result_t *result);
static void __aic8800d_scan_finish(vsf_wifi_t *wifi);
static void __aic8800d_scan_finish_timer_cb(vsf_callback_timer_t *timer);
static void __aic8800d_connect_cancel_cfm(vsf_wifi_t *wifi, vsf_err_t err);
static void __aic8800d_init_wd_cb(vsf_callback_timer_t *timer);

/*============================ LOCAL FUNCTIONS ===============================*/

aic8800d_priv_t *__aic8800d_priv(vsf_wifi_t *wifi)
{
    return (aic8800d_priv_t *)wifi->chip_priv;
}

static const vsf_wifi_aic8800d_bus_ops_t *__aic8800d_bus_ops(vsf_wifi_t *wifi)
{
    return (const vsf_wifi_aic8800d_bus_ops_t *)wifi->bus_ops;
}

static aic8800d_cmd_t *__aic8800d_cmd_alloc(aic8800d_priv_t *priv)
{
    for (int i = 0; i < AIC8800D_MAX_PENDING_CMDS; i++) {
        if (!priv->cmds[i].busy) {
            priv->cmds[i].busy = true;
            priv->cmds[i].token = priv->next_token++;
            return &priv->cmds[i];
        }
    }
    return NULL;
}

static void __aic8800d_cmd_free(aic8800d_cmd_t *cmd)
{
    if (cmd != NULL) {
        memset(cmd, 0, sizeof(*cmd));
    }
}

static aic8800d_cmd_t *__aic8800d_cmd_find_by_cfm(aic8800d_priv_t *priv,
        uint16_t cfm_id)
{
    for (int i = 0; i < AIC8800D_MAX_PENDING_CMDS; i++) {
        if (priv->cmds[i].busy && priv->cmds[i].cfm_id == cfm_id)
            return &priv->cmds[i];
    }
    return NULL;
}

vsf_err_t __aic8800d_send_msg(vsf_wifi_t *wifi, uint16_t id,
        const void *param, uint16_t param_len,
        uint16_t cfm_id, void *cfm_param, uint16_t cfm_param_len,
        vsf_wifi_done_t done)
{
    aic8800d_priv_t *priv = __aic8800d_priv(wifi);
    const vsf_wifi_aic8800d_bus_ops_t *bus_ops = __aic8800d_bus_ops(wifi);
    uint16_t lmac_len = (uint16_t)(sizeof(struct aic8800d_lmac_msg) + param_len);
    uint16_t usb_len  = (uint16_t)(4 + AIC8800D_USB_DUMMY_LEN + lmac_len);
    uint16_t usb_payload_len = (uint16_t)(lmac_len + 4);
    uint8_t *buf;
    uint16_t idx = 0;

    if (NULL == bus_ops || NULL == bus_ops->send)
        return VSF_ERR_NOT_READY;
    if (usb_len > AIC8800D_CMD_BUF_SIZE)
        return VSF_ERR_NOT_SUPPORT;

    aic8800d_cmd_t *cmd = NULL;
    if (cfm_id != 0) {
        cmd = __aic8800d_cmd_alloc(priv);
        if (NULL == cmd) return VSF_ERR_NOT_ENOUGH_RESOURCES;
        cmd->id            = id;
        cmd->cfm_id        = cfm_id;
        cmd->done          = done;
        cmd->cfm_param      = cfm_param;
        cmd->cfm_param_len  = cfm_param_len;
    }

    buf = vsf_heap_malloc(usb_len);
    if (NULL == buf) {
        __aic8800d_cmd_free(cmd);
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }
    memset(buf, 0, usb_len);

    /* USB header */
    buf[idx++] = (usb_payload_len >> 0) & 0xFF;
    buf[idx++] = (usb_payload_len >> 8) & 0x0F;
    buf[idx++] = AIC8800D_USB_CMD_TYPE;
    buf[idx++] = 0;
    /* Dummy word */
    idx += AIC8800D_USB_DUMMY_LEN;

    /* lmac_msg */
    buf[idx++] = (id >> 0) & 0xFF;
    buf[idx++] = (id >> 8) & 0xFF;
    uint16_t dest_id = AIC8800D_MSG_T(id);
    buf[idx++] = (dest_id >> 0) & 0xFF;
    buf[idx++] = (dest_id >> 8) & 0xFF;
    buf[idx++] = (AIC8800D_DRV_TASK_ID >> 0) & 0xFF;
    buf[idx++] = (AIC8800D_DRV_TASK_ID >> 8) & 0xFF;
    buf[idx++] = (param_len >> 0) & 0xFF;
    buf[idx++] = (param_len >> 8) & 0xFF;

    if (param_len > 0) {
        memcpy(&buf[idx], param, param_len);
    }

    if (id == AIC8800D_SM_CONNECT_REQ) {
        const struct aic8800d_sm_connect_req *creq = (const struct aic8800d_sm_connect_req *)param;
        vsf_wifi_aic8800d_trace_debug("aic8800d: SM_CONNECT_REQ id=0x%04X param_len=%u usb_len=%u" VSF_TRACE_CFG_LINEEND, id, param_len, usb_len);
        vsf_wifi_aic8800d_trace_debug("aic8800d: connect fields: ssid_len=%u bssid=%02X:%02X:%02X:%02X:%02X:%02X freq=%u band=%u flags=0x%08X ethertype=0x%04X ie_len=%u auth=%u uapsd=0x%02X vif=%u rsn_cap=0x%04X" VSF_TRACE_CFG_LINEEND,
                creq->ssid.length,
                creq->bssid.array[0] & 0xFF, (creq->bssid.array[0] >> 8) & 0xFF,
                creq->bssid.array[1] & 0xFF, (creq->bssid.array[1] >> 8) & 0xFF,
                creq->bssid.array[2] & 0xFF, (creq->bssid.array[2] >> 8) & 0xFF,
                creq->chan.freq, creq->chan.band, creq->flags,
                creq->ctrl_port_ethertype, creq->ie_len,
                creq->auth_type, creq->uapsd_queues, creq->vif_idx,
                priv->ap_rsn_cap);
        vsf_wifi_aic8800d_trace_debug("aic8800d: SM_CONNECT_REQ bytes=");
        for (uint16_t i = 0; i < usb_len; i++) {
            vsf_wifi_aic8800d_trace_debug("%02X", buf[i]);
        }
        vsf_wifi_aic8800d_trace_debug(VSF_TRACE_CFG_LINEEND);
    }

    vsf_err_t err = bus_ops->send(wifi, buf, usb_len, NULL);

    /* Dump every LMAC message to a file for offline comparison with Linux pcaps.
     * Use Win32 API directly: stdio fopen() may call getcwd(), which asserts
     * when invoked from a non-process (EDA) context in the VSF linux sim. */
#if defined(_WIN32)
    {
        extern __declspec(dllimport) void *__stdcall CreateFileA(const char *,
                unsigned long, unsigned long, void *, unsigned long, unsigned long, void *);
        extern __declspec(dllimport) int __stdcall WriteFile(void *, const void *,
                unsigned long, unsigned long *, void *);
        extern __declspec(dllimport) int __stdcall CloseHandle(void *);
        void *hf = CreateFileA("C:/project/vsf.demo/aic_vsf_msg_dump.bin",
                0x00000004UL /* FILE_APPEND_DATA */, 0, NULL,
                4UL /* OPEN_ALWAYS */, 0x80UL /* FILE_ATTRIBUTE_NORMAL */, NULL);
        if (hf != (void *)-1 /* INVALID_HANDLE_VALUE */) {
            unsigned long written;
            WriteFile(hf, buf, usb_len, &written, NULL);
            CloseHandle(hf);
        }
    }
#endif

    vsf_heap_free(buf);
    if (VSF_ERR_NONE != err) {
        __aic8800d_cmd_free(cmd);
    }
    return err;
}

void __aic8800d_cmd_cfm(vsf_wifi_t *wifi, aic8800d_cmd_t *cmd,
        const struct aic8800d_ipc_e2a_msg *msg)
{
    if (cmd == NULL) return;
    if (cmd->cfm_param != NULL && msg->param_len > 0) {
        uint16_t copy_len = msg->param_len;
        if (copy_len > cmd->cfm_param_len)
            copy_len = cmd->cfm_param_len;
        memcpy(cmd->cfm_param, msg->param, copy_len);
    }
    vsf_wifi_done_t done = cmd->done;
    __aic8800d_cmd_free(cmd);
    if (done != NULL) {
        done(wifi, VSF_ERR_NONE);
    }
}

static void __aic8800d_handle_e2a(vsf_wifi_t *wifi,
        const struct aic8800d_ipc_e2a_msg *msg)
{
    aic8800d_priv_t *priv = __aic8800d_priv(wifi);

    vsf_wifi_aic8800d_trace_info("aic8800d: e2a id=0x%04X len=%u"
            VSF_TRACE_CFG_LINEEND, msg->id, msg->param_len);

    /* Confirmations first */
    aic8800d_cmd_t *cmd = __aic8800d_cmd_find_by_cfm(priv, msg->id);
    if (cmd != NULL) {
        __aic8800d_cmd_cfm(wifi, cmd, msg);
        return;
    }

    /* Indications */
    switch (msg->id) {
    case AIC8800D_SM_CONNECT_IND: {
        struct aic8800d_sm_connect_ind *ind =
                (struct aic8800d_sm_connect_ind *)msg->param;
        vsf_wifi_aic8800d_trace_info("aic8800d: connect_ind status=%d vif=%u ap=%u qos=%u ch=%u freq=%u"
                VSF_TRACE_CFG_LINEEND,
                ind->status_code, ind->vif_idx, ind->ap_idx,
                ind->qos, ind->ch_idx, ind->center_freq);
        if (ind->status_code == 0) {
            priv->ap_idx  = ind->ap_idx;
            priv->sta_idx = ind->ap_idx; /* AIC uses ap_idx as STA index */
            priv->qos     = ind->qos;
        }
        if (priv->connect_active && priv->connect_done != NULL) {
            priv->connect_active = false;
            vsf_wifi_done_t done = priv->connect_done;
            priv->connect_done = NULL;
            done(wifi, (ind->status_code == 0) ? VSF_ERR_NONE : VSF_ERR_FAIL);
        } else {
            /* async link-up from firmware */
            if (ind->status_code == 0) {
                vsf_wifi_link_info_t info;
                memset(&info, 0, sizeof(info));
                memcpy(info.bssid, &ind->bssid, 6);
                info.channel = __aic8800d_freq_to_channel(ind->center_freq);
                info.flags   = WIFI_LINK_FLAG_CONNECTED | WIFI_LINK_FLAG_AUTHORIZED;
                vsf_wifi_on_link_up(wifi, &info);
            }
        }
        break;
    }
    case AIC8800D_SM_CONNECT_CFM: {
        struct aic8800d_sm_connect_cfm *cfm =
                (struct aic8800d_sm_connect_cfm *)msg->param;
        uint8_t status = (msg->param_len >= 1) ? ((uint8_t *)msg->param)[0] : 0xFF;
        vsf_wifi_aic8800d_trace_info("aic8800d: connect_cfm status=%u"
                VSF_TRACE_CFG_LINEEND, status);
        if (status != 0) {
            /* Synchronous rejection: firmware will not send SM_CONNECT_IND.
             * Fail the pending connect immediately so the upper layer does
             * not block forever. */
            if (priv->connect_active && priv->connect_done != NULL) {
                priv->connect_active = false;
                vsf_wifi_done_t done = priv->connect_done;
                priv->connect_done = NULL;
                done(wifi, VSF_ERR_FAIL);
            }
        }
        break;
    }
    case AIC8800D_SM_DISCONNECT_IND: {
        struct aic8800d_sm_disconnect_ind *ind =
                (struct aic8800d_sm_disconnect_ind *)msg->param;
        vsf_wifi_aic8800d_trace_info("aic8800d: disconnect_ind reason=%d"
                VSF_TRACE_CFG_LINEEND, ind->reason_code);
        vsf_wifi_on_link_down(wifi, (uint8_t)ind->reason_code);
        break;
    }
    case AIC8800D_SCANU_RESULT_IND: {
        struct aic8800d_scanu_result_ind *ind =
                (struct aic8800d_scanu_result_ind *)msg->param;
        const uint8_t *frame = (const uint8_t *)ind->payload;
        uint16_t frame_len = ind->length;
        uint16_t freq = ind->center_freq;

        vsf_wifi_aic8800d_trace_info("aic8800d: scan_result len=%u rssi=%d freq=%u fc=0x%04X"
                VSF_TRACE_CFG_LINEEND, ind->length, ind->rssi, freq, ind->framectrl);

        if (priv->scan_finish_pending) {
            /* Results keep arriving after SCANU_START_CFM; extend the guard
             * window so the upper layer sees all of them before scan_done. */
            vsf_callback_timer_remove(&priv->scan_finish_timer);
            vsf_callback_timer_add_ms(&priv->scan_finish_timer, 500);
        }
        priv->scan_results_received++;

        if (frame_len < 36) break;          /* min mgmt header + fixed params */

        uint8_t fc0 = frame[0];
        uint8_t subtype = (fc0 >> 4) & 0x0F;
        /* beacon(8) or probe response(5) */
        if (subtype != 5 && subtype != 8) break;

        const uint8_t *bssid = frame + 16;  /* address 3 */
        const uint8_t *body  = frame + 24;  /* after sequence control */
        uint16_t body_len = frame_len > 24 ? (frame_len - 24) : 0;
        if (body_len < 12) break;

        vsf_wifi_scan_result_t result;
        memset(&result, 0, sizeof(result));
        memcpy(result.bssid, bssid, 6);
        result.capability = (uint16_t)body[10] | ((uint16_t)body[11] << 8);
        result.channel    = __aic8800d_freq_to_channel(freq);
        result.rssi       = ind->rssi;
        result.flags      = (subtype == 5) ? 1u : 0u; /* bit0 = probe response */

        const uint8_t *ie  = body + 12;
        const uint8_t *end = body + body_len;
        while (ie + 2 <= end) {
            uint8_t tag = ie[0];
            uint8_t l   = ie[1];
            if (ie + 2 + l > end) break;
            if (tag == 0 && l <= 32) {          /* SSID */
                memcpy(result.ssid, ie + 2, l);
                result.ssid_len = l;
            } else if (tag == 3 && l == 1) {   /* DS Param Set */
                result.channel = ie[2];
            } else if (tag == 48) {             /* RSN IE (WPA2) */
                __aic8800d_parse_rsn(ie + 2, l, &result);
            } else if (tag == 61 && l >= 2) {  /* HT Operation IE */
                result.ht40_width = (ie[2 + 1] & 0x01) ? 1 : 0;
                uint8_t sco = (uint8_t)((ie[2 + 1] >> 2) & 0x03);
                result.ht40_plus = (sco == 1) ? 1 : 0;
            } else if (tag == 221 && l >= 7 &&
                       ie[2] == 0x00 && ie[3] == 0x50 &&
                       ie[4] == 0xF2 && ie[5] == 0x02) {
                /* WMM Information/Parameter Element (OUI 00:50:F2, type 2) */
                result.wmm = 1;
            }
            ie += 2 + l;
        }

        vsf_wifi_aic8800d_trace_info(
                "aic8800d: scan bssid=%02X:%02X:%02X:%02X:%02X:%02X "
                "ssid=%.*s ch=%u rssi=%d caps=0x%04X wmm=%u ht40=%u%s rsn_cap=0x%04X"
                VSF_TRACE_CFG_LINEEND,
                bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5],
                result.ssid_len, result.ssid,
                result.channel, result.rssi, result.capability,
                (unsigned)result.wmm,
                (unsigned)result.ht40_width,
                result.ht40_plus ? "+" : (result.ht40_width ? "-" : ""),
                result.rsn_cap);

        /* Cache RSN capabilities keyed by BSSID for connect(). */
        if (priv->scan_rsn_cap_cache_num < AIC8800D_SCAN_RSN_CAP_CACHE_SIZE) {
            uint8_t n = priv->scan_rsn_cap_cache_num;
            memcpy(priv->scan_rsn_cap_cache[n].bssid, bssid, 6);
            priv->scan_rsn_cap_cache[n].rsn_cap = result.rsn_cap;
            priv->scan_rsn_cap_cache_num++;
        }

        vsf_wifi_on_scan_result(wifi, &result);
        break;
    }
    case AIC8800D_SCANU_START_CFM: {
        struct aic8800d_scanu_start_cfm *cfm =
                (struct aic8800d_scanu_start_cfm *)msg->param;
        vsf_wifi_aic8800d_trace_info("aic8800d: scanu_start_cfm vif=%u status=%u result_cnt=%u"
                VSF_TRACE_CFG_LINEEND,
                cfm->vif_idx, cfm->status, cfm->result_cnt);
        if (priv->scan_active) {
            /* 0x1001 (SCANU_START_CFM) reports scan completion with the expected
             * result count. Start a guard window so any trailing
             * SCANU_RESULT_IND indications are processed before scan_done.
             * Do NOT reset scan_results_received here: SCANU_RESULT_IND may
             * arrive before the completion confirm. */
            priv->scan_results_expected = cfm->result_cnt;
            priv->scan_finish_pending = true;
            priv->scan_finish_retries = 0;
            vsf_callback_timer_remove(&priv->scan_finish_timer);
            vsf_callback_timer_add_ms(&priv->scan_finish_timer, 2500);
        }
        break;
    }
    default:
        break;
    }
}

/* Process one AIC8800D USB sub-packet.
 * The firmware aggregates multiple LMAC messages / data frames into a single
 * bulk IN transfer.  Each sub-packet starts with a 4-byte USB header:
 *   [0:1] payload length (12-bit LE), [2] type, [3] flags/reserved.
 * Data sub-packets: type & 0x10 == 0, payload is hw_rxhdr + MPDU.
 * Config sub-packets: type & 0x10 != 0, payload length is cfg payload size. */
static void __aic8800d_on_rx_subpacket(vsf_wifi_t *wifi,
        uint8_t *buf, uint16_t len)
{
    aic8800d_priv_t *priv = __aic8800d_priv(wifi);
    uint16_t payload_len;
    uint8_t  type;
    (void)priv;

    vsf_wifi_aic8800d_trace_debug("aic8800d: on_rx len=%u hdr=%02X%02X%02X%02X" VSF_TRACE_CFG_LINEEND,
            (unsigned)len, buf[0], buf[1], buf[2], buf[3]);
    if (len <= 32) {
        vsf_wifi_aic8800d_trace_debug("aic8800d: on_rx data=");
        for (uint16_t i = 0; i < len; i++) {
            vsf_wifi_aic8800d_trace_debug("%02X", buf[i]);
        }
        vsf_wifi_aic8800d_trace_debug(VSF_TRACE_CFG_LINEEND);
    }

    if (len < 4) return;
    payload_len = buf[0] | ((buf[1] & 0x0F) << 8);
    type        = buf[2];
    vsf_wifi_aic8800d_trace_debug("aic8800d: on_rx type=0x%02X payload=%u" VSF_TRACE_CFG_LINEEND,
            type, payload_len);

    if ((type & AIC8800D_USB_TYPE_CFG) == 0) {
        /* Data path: RX_HWHRD_LEN (60, already includes the 4-byte sub-packet
         * header) of hw rxhdr, then the 802.11 data MPDU.  The firmware has
         * already decrypted CCMP; strip the 802.11 header/LLC and rebuild the
         * Ethernet frame. */
        const uint16_t hdr_total = AIC8800D_RX_HWHDR_LEN;
        if (len < hdr_total + 24) {
            vsf_wifi_aic8800d_trace_debug("aic8800d: rx data too short (%u)" VSF_TRACE_CFG_LINEEND, len);
            return;
        }

        const uint8_t *mpdu = buf + hdr_total;
        uint16_t mpdu_len = len - hdr_total;
        uint8_t fc0 = mpdu[0];
        uint8_t fc1 = mpdu[1];
        uint8_t subtype = (fc0 >> 4) & 0x0F;
        uint8_t to_from_ds = fc1 & 0x03;
        uint8_t hdr_len = 24;
        uint8_t ccmp_len = 0;
        bool is_qos = false;

        if ((fc0 & 0x0F) != 0x08) {
            /* not a data frame */
            return;
        }
        if (subtype == 0x08) { /* QoS data */
            is_qos = true;
            hdr_len = 26;
        }
        if (fc1 & 0x80) /* Order bit = HTC present */
            hdr_len += 4;
        if (fc1 & 0x40) { /* Protected: the firmware keeps the 8-byte CCMP
                           * header between the 802.11 header and the LLC/SNAP
                           * payload. */
            hdr_len += 8;
            ccmp_len = 8;
        }
        (void)is_qos;
        if (mpdu_len < hdr_len + 8)
            return;

        const uint8_t *sa, *da;
        switch (to_from_ds) {
        case 0x01: /* to DS */
            da = mpdu + 16;
            sa = mpdu + 10;
            break;
        case 0x02: /* from DS */
            da = mpdu + 4;
            sa = mpdu + 10;
            break;
        default:
            /* adhoc / WDS: skip for now */
            return;
        }

        const uint8_t *llc = mpdu + hdr_len;
        /* ethertype in network byte order (EAPOL = 0x888E) */
        uint16_t ethertype = ((uint16_t)llc[6] << 8) | (uint16_t)llc[7];
        uint16_t eth_payload_len = mpdu_len - hdr_len - 8;

        vsf_wifi_aic8800d_trace_debug("aic8800d: rx data fc=%02X%02X hdrlen=%u et=0x%04X state=%u da=%02X:%02X:%02X:%02X:%02X:%02X"
                VSF_TRACE_CFG_LINEEND, mpdu[0], mpdu[1], hdr_len, ethertype,
                wifi->mlme_state, da[0], da[1], da[2], da[3], da[4], da[5]);

#if VSF_WIFI_USE_WPA == ENABLED
        if ((ethertype == 0x888E) && (wifi->mlme_state == WIFI_MLME_4WAY)) {
            /* EAPOL frame during the 4-way handshake: hand the EAPOL payload
             * (after the LLC/SNAP header) to the WPA supplicant. */
            uint8_t *eth = vsf_heap_malloc(14 + eth_payload_len);
            if (eth == NULL) return;
            memcpy(eth + 0, da, 6);
            memcpy(eth + 6, sa, 6);
            eth[12] = llc[6];
            eth[13] = llc[7];
            memcpy(eth + 14, llc + 8, eth_payload_len);
            vsf_wifi_aic8800d_trace_info("aic8800d: EAPOL -> supplicant len=%u state=%u anonce=%02X%02X%02X%02X"
                    VSF_TRACE_CFG_LINEEND, eth_payload_len, wifi->mlme_state,
                    eth[14+17], eth[14+18], eth[14+19], eth[14+20]);
            vsf_wifi_eapol_rx(wifi, eth + 14, eth_payload_len);
            vsf_heap_free(eth);
        } else
#endif
        {
            /* Business frame: the netdrv backend expects a naked, decrypted
             * 802.11 data frame (FC .. LLC/payload), NOT an Ethernet frame.
             * The firmware already decrypted CCMP but keeps the 8-byte CCMP
             * header in place; strip it and clear the Protected bit so the
             * generic dot11 -> eth bridge parses the frame correctly. */
            uint16_t base_hdr_len = hdr_len - ccmp_len;
            uint16_t out_len = base_hdr_len + 8 + eth_payload_len;
            uint8_t *out = vsf_heap_malloc(out_len);
            if (out == NULL) return;
            memcpy(out, mpdu, base_hdr_len);
            out[1] &= (uint8_t)~0x40;               /* clear Protected bit */
            memcpy(out + base_hdr_len, llc, 8 + eth_payload_len);
            vsf_wifi_data_rx(wifi, out, out_len);
            vsf_heap_free(out);
        }
        return;
    }

    if (type == AIC8800D_USB_TYPE_CFG_CMD_RSP) {
        /* E2A message follows immediately after the 4-byte USB header
         * (Linux aic_txrxif.c passes skb_inblock->data + 4 to rwnx_rx_handle_msg). */
        const uint8_t e2a_header_size = 4 + 12; /* id/dest/src/param_len/pattern */
        if (len < e2a_header_size) {
            vsf_wifi_aic8800d_trace_error("aic8800d: on_rx cmd_rsp too short (%u < %u)" VSF_TRACE_CFG_LINEEND,
                    len, e2a_header_size);
            return;
        }
        struct aic8800d_ipc_e2a_msg msg;
        memset(&msg, 0, sizeof(msg));
        uint8_t *p = buf + 4;
        msg.id            = p[0] | (p[1] << 8);
        msg.dummy_dest_id = p[2] | (p[3] << 8);
        msg.dummy_src_id  = p[4] | (p[5] << 8);
        msg.param_len     = p[6] | (p[7] << 8);
        msg.pattern       = p[8] | (p[9] << 8) | (p[10] << 16) | (p[11] << 24);
        vsf_wifi_aic8800d_trace_debug("aic8800d: on_rx e2a id=0x%04X param_len=%u pattern=0x%08X" VSF_TRACE_CFG_LINEEND,
                msg.id, msg.param_len, msg.pattern);
        if (len < e2a_header_size + msg.param_len) {
            vsf_wifi_aic8800d_trace_error("aic8800d: on_rx e2a truncated (%u < %u)" VSF_TRACE_CFG_LINEEND,
                    len, e2a_header_size + msg.param_len);
            return;
        }
        if (msg.param_len > sizeof(msg.param)) {
            vsf_wifi_aic8800d_trace_error(
                    "aic8800d: e2a param_len=%u exceeds buffer %u" VSF_TRACE_CFG_LINEEND,
                    msg.param_len, (unsigned)sizeof(msg.param));
            return;
        }
        memcpy(msg.param, p + 12, msg.param_len);
        __aic8800d_handle_e2a(wifi, &msg);
        return;
    }

    if (type == AIC8800D_USB_TYPE_CFG_PRINT) {
        /* firmware print string */
        vsf_wifi_aic8800d_trace_info("aic8800d_fw: %.*s" VSF_TRACE_CFG_LINEEND,
                payload_len, (char *)buf + 4);
        return;
    }
}

void vsf_wifi_aic8800d_on_rx(vsf_wifi_t *wifi, uint8_t *buf, uint16_t len)
{
    uint16_t offset = 0;
    uint8_t  parsed = 0;

    if (len < 4 || buf == NULL) return;

    /* AIC8800D USB RX aggregation: one bulk IN transfer may contain multiple
     * sub-packets.  Sub-packets START at 4-byte aligned offsets, but the LAST
     * one in a URB is not padded — so process with the real length and only
     * round up when advancing to the next one. */
    while (offset + 4 <= len) {
        uint16_t payload_len = buf[offset + 0] | ((buf[offset + 1] & 0x0F) << 8);
        uint8_t  type        = buf[offset + 2];
        uint16_t real_len, walk_len;

        if (payload_len == 0) {
            break;
        }

        if ((type & AIC8800D_USB_TYPE_CFG) == 0) {
            /* data: the pkt_len field already covers the 4-byte sub-packet
             * header (pkt_len = 4 + MPDU_len), so the real content length is
             * pkt_len + hw_rxhdr (Linux: aggr_len = pkt_len + RX_HWHRD_LEN). */
            real_len = AIC8800D_RX_HWHDR_LEN + payload_len;
        } else {
            /* config: pkt_len field is the cfg message length */
            real_len = 4 + payload_len;
        }
        walk_len = (real_len + (AIC8800D_USB_RX_ALIGNMENT - 1))
                & ~(AIC8800D_USB_RX_ALIGNMENT - 1);

        if (offset + real_len > len) {
            vsf_wifi_aic8800d_trace_error(
                    "aic8800d: rx aggregation truncated offset=%u real_len=%u len=%u"
                    VSF_TRACE_CFG_LINEEND,
                    (unsigned)offset, (unsigned)real_len, (unsigned)len);
            break;
        }

        __aic8800d_on_rx_subpacket(wifi, buf + offset, real_len);
        offset += walk_len;
        parsed++;

        /* sanity guard: malformed aggregation could loop forever */
        if (parsed > 64 || real_len == 0) break;
    }
}

/*============================ INIT STATE MACHINE ============================*/

static void __aic8800d_init_step(vsf_wifi_t *wifi, vsf_err_t err);

static void __aic8800d_init_done_cb(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) {
        aic8800d_priv_t *priv = __aic8800d_priv(wifi);
        if (priv->init_done != NULL) {
            vsf_wifi_done_t done = priv->init_done;
            priv->init_done = NULL;
            done(wifi, err);
        }
        return;
    }
    __aic8800d_init_step(wifi, VSF_ERR_NONE);
}

static void __aic8800d_init_step(vsf_wifi_t *wifi, vsf_err_t err)
{
    aic8800d_priv_t *priv = __aic8800d_priv(wifi);
    struct aic8800d_mm_get_mac_addr_cfm mac_cfm;
    struct aic8800d_mm_add_if_cfm add_if_cfm;
    struct aic8800d_mm_version_cfm version_cfm;
    struct aic8800d_mm_reset_cfm reset_cfm;
    struct aic8800d_mm_set_channel_cfm channel_cfm;
    struct aic8800d_mm_start_cfm start_cfm;
    struct aic8800d_mm_set_stack_start_cfm stack_cfm;
    struct aic8800d_mm_set_filter_req filter_req;
    struct aic8800d_mm_set_txpwr_lvl_req txpwr_req;
    struct aic8800d_mm_set_rf_calib_req rf_calib_req;
    struct aic8800d_mm_set_rf_calib_cfm rf_calib_cfm;
    struct aic8800d_mm_add_if_req add_if_req;
    struct aic8800d_mm_reset_req reset_req;
    struct aic8800d_mm_get_mac_addr_req mac_req;
    struct aic8800d_mm_set_stack_start_req stack_req;
    struct aic8800d_mm_version_req version_req;
    struct aic8800d_mm_set_channel_req channel_req;
    (void)err;

    if (VSF_ERR_NONE != err) {
        if (priv->init_done != NULL) {
            vsf_wifi_done_t done = priv->init_done;
            priv->init_done = NULL;
            done(wifi, err);
        }
        return;
    }

    switch (priv->init_step) {
    case 0:
        vsf_wifi_aic8800d_trace_info("aic8800d: MM_SET_STACK_START_REQ"
                VSF_TRACE_CFG_LINEEND);
        memset(&stack_req, 0, sizeof(stack_req));
        stack_req.is_stack_start = 1;
        stack_req.efuse_valid = 0;
        stack_req.set_vendor_info = 0x20; /* CO_BIT(5) */
        stack_req.fwtrace_redir = 0;
        priv->init_step++;
        __aic8800d_send_msg(wifi, AIC8800D_MM_SET_STACK_START_REQ,
                &stack_req, sizeof(stack_req),
                AIC8800D_MM_SET_STACK_START_CFM, &stack_cfm, sizeof(stack_cfm),
                __aic8800d_init_done_cb);
        break;

    case 1:
        vsf_wifi_aic8800d_trace_info("aic8800d: stack_start_cfm 5g=%u vendor=0x%02X"
                VSF_TRACE_CFG_LINEEND, stack_cfm.is_5g_support, stack_cfm.vendor_info);

        vsf_wifi_aic8800d_trace_info("aic8800d: MM_GET_FW_VERSION_REQ"
                VSF_TRACE_CFG_LINEEND);
        {
            struct aic8800d_mm_get_fw_version_req fw_req;
            struct aic8800d_mm_get_fw_version_cfm fw_cfm;
            memset(&fw_req, 0, sizeof(fw_req));
            priv->init_step++;
            __aic8800d_send_msg(wifi, AIC8800D_MM_GET_FW_VERSION_REQ,
                    &fw_req, sizeof(fw_req),
                    AIC8800D_MM_GET_FW_VERSION_CFM, &fw_cfm, sizeof(fw_cfm),
                    __aic8800d_init_done_cb);
        }
        break;

    case 2:
        vsf_wifi_aic8800d_trace_info("aic8800d: MM_SET_TXPWR_IDX_LVL_REQ"
                VSF_TRACE_CFG_LINEEND);
        memset(&txpwr_req, 0, sizeof(txpwr_req));
        txpwr_req.u.txpwr_lvl_v3 = __aic8800d_txpwr_lvl_v3_default;
        priv->init_step++;
        __aic8800d_send_msg(wifi, AIC8800D_MM_SET_TXPWR_IDX_LVL_REQ,
                &txpwr_req, sizeof(txpwr_req.u.txpwr_lvl_v3),
                AIC8800D_MM_SET_TXPWR_IDX_LVL_CFM, NULL, 0,
                __aic8800d_init_done_cb);
        break;

    case 3:
        vsf_wifi_aic8800d_trace_info("aic8800d: MM_SET_RF_CALIB_REQ"
                VSF_TRACE_CFG_LINEEND);
        memset(&rf_calib_req, 0, sizeof(rf_calib_req));
        rf_calib_req.cal_cfg_24g = 0x0f8f;
        rf_calib_req.cal_cfg_5g  = 0x0f0f;
        rf_calib_req.param_alpha = 0x0c34c008;
        rf_calib_req.bt_calib_en = 0;
        rf_calib_req.bt_calib_param = 0x264203;
        /* xtal_cap defaults to enable=0; leave xtal_cap fields zero */
        priv->init_step++;
        __aic8800d_send_msg(wifi, AIC8800D_MM_SET_RF_CALIB_REQ,
                &rf_calib_req, sizeof(rf_calib_req),
                AIC8800D_MM_SET_RF_CALIB_CFM, &rf_calib_cfm, sizeof(rf_calib_cfm),
                __aic8800d_init_done_cb);
        break;

    case 4:
        vsf_wifi_aic8800d_trace_info("aic8800d: MM_GET_MAC_ADDR_REQ"
                VSF_TRACE_CFG_LINEEND);
        memset(&mac_req, 0, sizeof(mac_req));
        priv->init_step++;
        __aic8800d_send_msg(wifi, AIC8800D_MM_GET_MAC_ADDR_REQ,
                &mac_req, sizeof(mac_req),
                AIC8800D_MM_GET_MAC_ADDR_CFM, &mac_cfm, sizeof(mac_cfm),
                __aic8800d_init_done_cb);
        break;

    case 5:
        memcpy(priv->mac_addr, mac_cfm.mac_addr, 6);
        priv->mac_addr_valid = true;
        memcpy(wifi->mac, mac_cfm.mac_addr, 6);
        vsf_wifi_aic8800d_trace_info("aic8800d: MAC="
                "%02X:%02X:%02X:%02X:%02X:%02X" VSF_TRACE_CFG_LINEEND,
                mac_cfm.mac_addr[0], mac_cfm.mac_addr[1], mac_cfm.mac_addr[2],
                mac_cfm.mac_addr[3], mac_cfm.mac_addr[4], mac_cfm.mac_addr[5]);

        vsf_wifi_aic8800d_trace_info("aic8800d: MM_RESET_REQ"
                VSF_TRACE_CFG_LINEEND);
        memset(&reset_req, 0, sizeof(reset_req));
        priv->init_step++;
        __aic8800d_send_msg(wifi, AIC8800D_MM_RESET_REQ,
                &reset_req, 0 /* no param, matches Linux */,
                AIC8800D_MM_RESET_CFM, &reset_cfm, sizeof(reset_cfm),
                __aic8800d_init_done_cb);
        break;

    case 6:
        vsf_wifi_aic8800d_trace_info("aic8800d: MM_VERSION_REQ"
                VSF_TRACE_CFG_LINEEND);
        memset(&version_req, 0, sizeof(version_req));
        priv->init_step++;
        __aic8800d_send_msg(wifi, AIC8800D_MM_VERSION_REQ,
                &version_req, 0 /* no param, matches Linux */,
                AIC8800D_MM_VERSION_CFM, &version_cfm, sizeof(version_cfm),
                __aic8800d_init_done_cb);
        break;

    case 7:
        vsf_wifi_aic8800d_trace_info("aic8800d: FW version=0x%08X"
                VSF_TRACE_CFG_LINEEND, version_cfm.version_lmac);
        vsf_wifi_aic8800d_trace_info("aic8800d: ME_CONFIG_REQ"
                VSF_TRACE_CFG_LINEEND);
        {
            struct aic8800d_me_config_req me_req;
            memset(&me_req, 0, sizeof(me_req));

            me_req.ht_supp  = true;
            me_req.vht_supp = true;
            me_req.he_supp  = true;
            me_req.he_ul_on = false;
            me_req.ps_on    = true;
            me_req.dpsm     = false;
            me_req.ant_div_on = true;
            me_req.tx_lft   = 1000;
            me_req.phy_bw_max = 2; /* PHY_CHNL_BW_80 (Linux use_80 for D81) */

            /* HT capability: matches Linux RWNX_HT_CAPABILITIES converted by
             * rwnx_send_me_config_req(). */
            me_req.ht_cap.ht_capa_info = 0x0001; /* LDPC coding */
            me_req.ht_cap.a_mpdu_param = 0x12;   /* factor=64K, density=16 */
            me_req.ht_cap.mcs_rate[0]  = 0xFF;   /* MCS 0-7 */
            me_req.ht_cap.ht_extended_capa  = 0;
            me_req.ht_cap.tx_beamforming_capa = 0;
            me_req.ht_cap.asel_capa = 0;

            /* VHT: nss=1, MCS0-9 (IEEE80211_VHT_MCS_SUPPORT_0_9), nss2-8 not
             * supported -> 0xFFFA on both maps. */
            me_req.vht_cap.rx_mcs_map = 0xFFFA;
            me_req.vht_cap.tx_mcs_map = 0xFFFA;

            priv->init_step++;
            __aic8800d_send_msg(wifi, AIC8800D_ME_CONFIG_REQ,
                    &me_req, sizeof(me_req),
                    AIC8800D_ME_CONFIG_CFM, NULL, 0,
                    __aic8800d_init_done_cb);
        }
        break;

    case 8:
        vsf_wifi_aic8800d_trace_info("aic8800d: ME_CHAN_CONFIG_REQ"
                VSF_TRACE_CFG_LINEEND);
        {
            struct aic8800d_me_chan_config_req req;
            memset(&req, 0, sizeof(req));

            /* 2.4GHz channels 1-14 */
            req.chan2G4_cnt = AIC8800D_MAC_DOMAINCHANNEL_24G_MAX;
            for (int i = 0; i < AIC8800D_MAC_DOMAINCHANNEL_24G_MAX; i++) {
                req.chan2G4[i].band = 0; /* PHY_BAND_2G4 */
                if (i < 13) {
                    req.chan2G4[i].freq = (aic_u16)(2412 + i * 5);
                } else {
                    req.chan2G4[i].freq = 2484; /* channel 14 */
                }
                req.chan2G4[i].tx_power = 20;
            }

            /* 5GHz common channels */
            static const uint16_t __aic8800d_5g_freqs[] = {
                5180, 5200, 5220, 5240, 5260, 5280, 5300, 5320,
                5500, 5520, 5540, 5560, 5580, 5600, 5620, 5640,
                5660, 5680, 5700, 5720, 5745, 5765, 5785, 5805,
                5825, 5845, 5865, 5885,
            };
            req.chan5G_cnt = dimof(__aic8800d_5g_freqs);
            if (req.chan5G_cnt > AIC8800D_MAC_DOMAINCHANNEL_5G_MAX)
                req.chan5G_cnt = AIC8800D_MAC_DOMAINCHANNEL_5G_MAX;
            for (int i = 0; i < req.chan5G_cnt; i++) {
                req.chan5G[i].band = 1; /* PHY_BAND_5G */
                req.chan5G[i].freq = __aic8800d_5g_freqs[i];
                req.chan5G[i].tx_power = 20;
            }

            priv->init_step++;
            __aic8800d_send_msg(wifi, AIC8800D_ME_CHAN_CONFIG_REQ,
                    &req, sizeof(req),
                    AIC8800D_ME_CHAN_CONFIG_CFM, NULL, 0,
                    __aic8800d_init_done_cb);
        }
        break;

    case 9:
        /* Linux sends MM_SET_COEX_REQ (bt_on=1, enable_nullcts=1) before
         * MM_START_REQ; without it the firmware's BT/WiFi coexistence logic
         * can suppress the WiFi data path. */
        vsf_wifi_aic8800d_trace_info("aic8800d: MM_SET_COEX_REQ"
                VSF_TRACE_CFG_LINEEND);
        {
            struct aic8800d_mm_set_coex_req coex_req;
            memset(&coex_req, 0, sizeof(coex_req));
            coex_req.bt_on         = 1;
            coex_req.enable_nullcts = 1;
            priv->init_step++;
            __aic8800d_send_msg(wifi, AIC8800D_MM_SET_COEX_REQ,
                    &coex_req, sizeof(coex_req),
                    AIC8800D_MM_SET_COEX_CFM, NULL, 0,
                    __aic8800d_init_done_cb);
        }
        break;

    case 10:
        /* Linux sends MM_START_REQ (rwnx_open) only after ME_CONFIG_REQ and
         * ME_CHAN_CONFIG_REQ have been programmed at probe time. Keep the same
         * order: starting the MAC before the channel table is configured makes
         * the firmware reject SM_CONNECT_REQ with CO_BAD_PARAM. */
        vsf_wifi_aic8800d_trace_info("aic8800d: MM_START_REQ"
                VSF_TRACE_CFG_LINEEND);
        {
            struct aic8800d_mm_start_req start_req;
            memset(&start_req, 0, sizeof(start_req));
            start_req.uapsd_timeout = 300;
            start_req.lp_clk_accuracy = 20;
            priv->init_step++;
            __aic8800d_send_msg(wifi, AIC8800D_MM_START_REQ,
                    &start_req, sizeof(start_req),
                    AIC8800D_MM_START_CFM, &start_cfm, sizeof(start_cfm),
                    __aic8800d_init_done_cb);
        }
        break;

    case 11:
        vsf_wifi_aic8800d_trace_info("aic8800d: MM_ADD_IF_REQ"
                VSF_TRACE_CFG_LINEEND);
        memset(&add_if_req, 0, sizeof(add_if_req));
        memcpy(&add_if_req.addr, priv->mac_addr, 6);
        add_if_req.type = 0; /* VIF_STA */
        priv->init_step++;
        __aic8800d_send_msg(wifi, AIC8800D_MM_ADD_IF_REQ,
                &add_if_req, sizeof(add_if_req),
                AIC8800D_MM_ADD_IF_CFM, &add_if_cfm, sizeof(add_if_cfm),
                __aic8800d_init_done_cb);
        break;

    case 12:
        priv->vif_idx = add_if_cfm.inst_nbr;
        vsf_wifi_aic8800d_trace_info("aic8800d: vif_idx=%u status=%u"
                VSF_TRACE_CFG_LINEEND, add_if_cfm.inst_nbr, add_if_cfm.status);

        vsf_wifi_aic8800d_trace_info("aic8800d: MM_SET_FILTER_REQ"
                VSF_TRACE_CFG_LINEEND);
        memset(&filter_req, 0, sizeof(filter_req));
        /* Linux RWNX_DEFAULT_RX_FILTER */
        filter_req.filter = 0x35078788;
        priv->init_step++;
        __aic8800d_send_msg(wifi, AIC8800D_MM_SET_FILTER_REQ,
                &filter_req, sizeof(filter_req),
                AIC8800D_MM_SET_FILTER_CFM, NULL, 0,
                __aic8800d_init_done_cb);
        break;

    case 13:
        vsf_wifi_aic8800d_trace_info("aic8800d: init done" VSF_TRACE_CFG_LINEEND);
        vsf_callback_timer_remove(&priv->init_wd_timer);
        if (priv->init_done != NULL) {
            vsf_wifi_done_t done = priv->init_done;
            priv->init_done = NULL;
            done(wifi, VSF_ERR_NONE);
        }
        break;

    default:
        break;
    }
}

/*============================ CHIP DRIVER HOOKS =============================*/

/*============================ FIRMWARE LOAD =================================*/

static void __aic8800d_fw_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    aic8800d_priv_t *priv = __aic8800d_priv(wifi);
    vsf_wifi_done_t done = priv->fw_done;

    priv->fw_done = NULL;
    /* fw_buf points to embedded const array; do not free. */
    priv->fw_buf = NULL;
    priv->fw_size   = 0;
    priv->fw_offset = 0;

    if (err != VSF_ERR_NONE) {
        vsf_wifi_aic8800d_trace_error("aic8800d: firmware load failed (%d)"
                VSF_TRACE_CFG_LINEEND, err);
    } else {
        vsf_wifi_aic8800d_trace_info("aic8800d: firmware load completed"
                VSF_TRACE_CFG_LINEEND);
    }

    if (done != NULL) {
        done(wifi, err);
    }
}

static void __aic8800d_fw_step(vsf_wifi_t *wifi, vsf_err_t err);

static void __aic8800d_fw_next(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (err != VSF_ERR_NONE) {
        __aic8800d_fw_done(wifi, err);
        return;
    }
    __aic8800d_fw_step(wifi, VSF_ERR_NONE);
}

static void __aic8800d_patch_config_step(vsf_wifi_t *wifi, vsf_err_t err);
static void __aic8800d_rf_config_step(vsf_wifi_t *wifi, vsf_err_t err);

static void __aic8800d_patch_config_next(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (err != VSF_ERR_NONE) {
        vsf_wifi_aic8800d_trace_error("aic8800d: patch config failed (%d)"
                VSF_TRACE_CFG_LINEEND, err);
        __aic8800d_fw_done(wifi, err);
        return;
    }
    __aic8800d_patch_config_step(wifi, VSF_ERR_NONE);
}

static void __aic8800d_patch_config_step(vsf_wifi_t *wifi, vsf_err_t err)
{
    aic8800d_priv_t *priv = __aic8800d_priv(wifi);
    struct aic8800d_dbg_mem_read_req rd_req;
    struct aic8800d_dbg_mem_write_req wr_req;
    struct aic8800d_dbg_start_app_req start_req;
    uint32_t addr;

    if (VSF_ERR_NONE != err) {
        __aic8800d_fw_done(wifi, err);
        return;
    }

    switch (priv->patch_step) {
    case 0:
        vsf_wifi_aic8800d_trace_info("aic8800d: patch config start"
                VSF_TRACE_CFG_LINEEND);
        rd_req.mem_addr = AIC8800D_FW_LOAD_ADDR + 0x0198;
        priv->patch_step = 1;
        __aic8800d_send_msg(wifi, AIC8800D_DBG_MEM_READ_REQ,
                &rd_req, sizeof(rd_req),
                AIC8800D_DBG_MEM_READ_CFM, &priv->patch_rd_cfm, sizeof(priv->patch_rd_cfm),
                __aic8800d_patch_config_next);
        break;

    case 1:
        priv->config_base = priv->patch_rd_cfm.mem_data;
        vsf_wifi_aic8800d_trace_debug("aic8800d: config_base=0x%08X"
                VSF_TRACE_CFG_LINEEND, priv->config_base);
        rd_req.mem_addr = AIC8800D_FW_LOAD_ADDR + 0x01A0;
        priv->patch_step = 2;
        __aic8800d_send_msg(wifi, AIC8800D_DBG_MEM_READ_REQ,
                &rd_req, sizeof(rd_req),
                AIC8800D_DBG_MEM_READ_CFM, &priv->patch_rd_cfm, sizeof(priv->patch_rd_cfm),
                __aic8800d_patch_config_next);
        break;

    case 2:
        priv->patch_str_base = priv->patch_rd_cfm.mem_data;
        vsf_wifi_aic8800d_trace_debug("aic8800d: patch_str_base=0x%08X"
                VSF_TRACE_CFG_LINEEND, priv->patch_str_base);
        rd_req.mem_addr = AIC8800D_FW_LOAD_ADDR + 0x01C;
        priv->patch_step = 3;
        __aic8800d_send_msg(wifi, AIC8800D_DBG_MEM_READ_REQ,
                &rd_req, sizeof(rd_req),
                AIC8800D_DBG_MEM_READ_CFM, &priv->patch_rd_cfm, sizeof(priv->patch_rd_cfm),
                __aic8800d_patch_config_next);
        break;

    case 3:
        priv->fw_version = priv->patch_rd_cfm.mem_data;
        vsf_wifi_aic8800d_trace_debug("aic8800d: fw_version=0x%08X"
                VSF_TRACE_CFG_LINEEND, priv->fw_version);
        if (priv->fw_version > 0x06090100) {
            rd_req.mem_addr = AIC8800D_FW_LOAD_ADDR + 0x01A4;
            priv->patch_step = 4;
            __aic8800d_send_msg(wifi, AIC8800D_DBG_MEM_READ_REQ,
                    &rd_req, sizeof(rd_req),
                    AIC8800D_DBG_MEM_READ_CFM, &priv->patch_rd_cfm, sizeof(priv->patch_rd_cfm),
                    __aic8800d_patch_config_next);
        } else {
            priv->patch_buff_base = 0x001D7000;
            priv->patch_step = 5;
            __aic8800d_patch_config_step(wifi, VSF_ERR_NONE);
        }
        break;

    case 4:
        priv->patch_buff_base = priv->patch_rd_cfm.mem_data;
        vsf_wifi_aic8800d_trace_debug("aic8800d: patch_buff_base=0x%08X"
                VSF_TRACE_CFG_LINEEND, priv->patch_buff_base);
        priv->patch_step = 5;
        /* fall through */

    case 5: {
        uint32_t offset;
        if (priv->patch_str_base == 0) {
            __aic8800d_fw_done(wifi, VSF_ERR_FAIL);
            return;
        }
        offset = (uint32_t)((uint8_t *)&((struct aic8800d_patch_desc *)0)->magic_num - (uint8_t *)0);
        wr_req.mem_addr = priv->patch_str_base + offset;
        wr_req.mem_data = AIC8800D_PATCH_MAGIC_NUM;
        priv->patch_step = 6;
        __aic8800d_send_msg(wifi, AIC8800D_DBG_MEM_WRITE_REQ,
                &wr_req, sizeof(wr_req),
                AIC8800D_DBG_MEM_WRITE_CFM, NULL, 0,
                __aic8800d_patch_config_next);
        break;
    }

    case 6: {
        uint32_t offset;
        offset = (uint32_t)((uint8_t *)&((struct aic8800d_patch_desc *)0)->magic_num_2 - (uint8_t *)0);
        wr_req.mem_addr = priv->patch_str_base + offset;
        wr_req.mem_data = AIC8800D_PATCH_MAGIC_NUM_2;
        priv->patch_step = 7;
        __aic8800d_send_msg(wifi, AIC8800D_DBG_MEM_WRITE_REQ,
                &wr_req, sizeof(wr_req),
                AIC8800D_DBG_MEM_WRITE_CFM, NULL, 0,
                __aic8800d_patch_config_next);
        break;
    }

    case 7: {
        uint32_t offset;
        offset = (uint32_t)((uint8_t *)&((struct aic8800d_patch_desc *)0)->pair_start - (uint8_t *)0);
        wr_req.mem_addr = priv->patch_str_base + offset;
        wr_req.mem_data = priv->patch_buff_base;
        priv->patch_step = 8;
        __aic8800d_send_msg(wifi, AIC8800D_DBG_MEM_WRITE_REQ,
                &wr_req, sizeof(wr_req),
                AIC8800D_DBG_MEM_WRITE_CFM, NULL, 0,
                __aic8800d_patch_config_next);
        break;
    }

    case 8: {
        uint32_t offset;
        offset = (uint32_t)((uint8_t *)&((struct aic8800d_patch_desc *)0)->pair_count - (uint8_t *)0);
        wr_req.mem_addr = priv->patch_str_base + offset;
        wr_req.mem_data = AIC8800D_PATCH_TBL_COUNT;
        priv->patch_step = 9;
        priv->patch_write_idx = 0;
        __aic8800d_send_msg(wifi, AIC8800D_DBG_MEM_WRITE_REQ,
                &wr_req, sizeof(wr_req),
                AIC8800D_DBG_MEM_WRITE_CFM, NULL, 0,
                __aic8800d_patch_config_next);
        break;
    }

    case 9:
        if (priv->patch_write_idx >= AIC8800D_PATCH_TBL_COUNT) {
            priv->patch_step = 11;
            __aic8800d_patch_config_step(wifi, VSF_ERR_NONE);
            return;
        }
        wr_req.mem_addr = priv->patch_buff_base + 8 * priv->patch_write_idx;
        wr_req.mem_data = __aic8800d_patch_tbl[priv->patch_write_idx][0] + priv->config_base;
        priv->patch_step = 10;
        __aic8800d_send_msg(wifi, AIC8800D_DBG_MEM_WRITE_REQ,
                &wr_req, sizeof(wr_req),
                AIC8800D_DBG_MEM_WRITE_CFM, NULL, 0,
                __aic8800d_patch_config_next);
        break;

    case 10:
        wr_req.mem_addr = priv->patch_buff_base + 8 * priv->patch_write_idx + 4;
        wr_req.mem_data = __aic8800d_patch_tbl[priv->patch_write_idx][1];
        priv->patch_write_idx++;
        priv->patch_step = 9;
        __aic8800d_send_msg(wifi, AIC8800D_DBG_MEM_WRITE_REQ,
                &wr_req, sizeof(wr_req),
                AIC8800D_DBG_MEM_WRITE_CFM, NULL, 0,
                __aic8800d_patch_config_next);
        break;

    case 11: {
        uint32_t offset;
        offset = (uint32_t)((uint8_t *)&((struct aic8800d_patch_desc *)0)->block_size[0] - (uint8_t *)0);
        wr_req.mem_addr = priv->patch_str_base + offset;
        wr_req.mem_data = 0;
        priv->patch_step = 12;
        __aic8800d_send_msg(wifi, AIC8800D_DBG_MEM_WRITE_REQ,
                &wr_req, sizeof(wr_req),
                AIC8800D_DBG_MEM_WRITE_CFM, NULL, 0,
                __aic8800d_patch_config_next);
        break;
    }

    case 12: {
        uint32_t offset;
        offset = (uint32_t)((uint8_t *)&((struct aic8800d_patch_desc *)0)->block_size[1] - (uint8_t *)0);
        wr_req.mem_addr = priv->patch_str_base + offset;
        wr_req.mem_data = 0;
        priv->patch_step = 13;
        __aic8800d_send_msg(wifi, AIC8800D_DBG_MEM_WRITE_REQ,
                &wr_req, sizeof(wr_req),
                AIC8800D_DBG_MEM_WRITE_CFM, NULL, 0,
                __aic8800d_patch_config_next);
        break;
    }

    case 13: {
        uint32_t offset;
        offset = (uint32_t)((uint8_t *)&((struct aic8800d_patch_desc *)0)->block_size[2] - (uint8_t *)0);
        wr_req.mem_addr = priv->patch_str_base + offset;
        wr_req.mem_data = 0;
        priv->patch_step = 14;
        __aic8800d_send_msg(wifi, AIC8800D_DBG_MEM_WRITE_REQ,
                &wr_req, sizeof(wr_req),
                AIC8800D_DBG_MEM_WRITE_CFM, NULL, 0,
                __aic8800d_patch_config_next);
        break;
    }

    case 14: {
        uint32_t offset;
        offset = (uint32_t)((uint8_t *)&((struct aic8800d_patch_desc *)0)->block_size[3] - (uint8_t *)0);
        wr_req.mem_addr = priv->patch_str_base + offset;
        wr_req.mem_data = 0;
        priv->patch_step = 15;
        __aic8800d_send_msg(wifi, AIC8800D_DBG_MEM_WRITE_REQ,
                &wr_req, sizeof(wr_req),
                AIC8800D_DBG_MEM_WRITE_CFM, NULL, 0,
                __aic8800d_patch_config_next);
        break;
    }

    case 15:
        vsf_wifi_aic8800d_trace_info("aic8800d: patch config done"
                VSF_TRACE_CFG_LINEEND);
        /* Proceed to RF config, then start app */
        priv->rf_cfg_idx = 0;
        __aic8800d_rf_config_step(wifi, VSF_ERR_NONE);
        break;

    default:
        __aic8800d_fw_done(wifi, VSF_ERR_FAIL);
        break;
    }
}

static void __aic8800d_rf_config_step(vsf_wifi_t *wifi, vsf_err_t err);

static void __aic8800d_rf_config_next(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (err != VSF_ERR_NONE) {
        vsf_wifi_aic8800d_trace_error("aic8800d: rf config failed (%d)"
                VSF_TRACE_CFG_LINEEND, err);
        __aic8800d_fw_done(wifi, err);
        return;
    }
    __aic8800d_rf_config_step(wifi, VSF_ERR_NONE);
}

static void __aic8800d_rf_config_step(vsf_wifi_t *wifi, vsf_err_t err)
{
    aic8800d_priv_t *priv = __aic8800d_priv(wifi);
    struct aic8800d_dbg_mem_mask_write_req req;
    struct aic8800d_dbg_start_app_req start_req;

    if (VSF_ERR_NONE != err) {
        __aic8800d_fw_done(wifi, err);
        return;
    }

    if (priv->rf_cfg_idx >= AIC8800D_RF_CFG_COUNT) {
        vsf_wifi_aic8800d_trace_info("aic8800d: rf config done"
                VSF_TRACE_CFG_LINEEND);
        memset(&start_req, 0, sizeof(start_req));
        start_req.boot_addr = AIC8800D_FW_LOAD_ADDR;
        start_req.boot_type = AIC8800D_FW_START_APP_TYPE;
        /* Linux driver does not wait for DBG_START_APP_CFM; the device may
         * reboot/run the firmware immediately after this command. */
        __aic8800d_send_msg(wifi, AIC8800D_DBG_START_APP_REQ,
                &start_req, sizeof(start_req),
                0, NULL, 0,
                NULL);
        __aic8800d_fw_done(wifi, VSF_ERR_NONE);
        return;
    }

    req.mem_addr = __aic8800d_rf_cfg[priv->rf_cfg_idx][0];
    req.mem_mask = __aic8800d_rf_cfg[priv->rf_cfg_idx][1];
    req.mem_data = __aic8800d_rf_cfg[priv->rf_cfg_idx][2];
    priv->rf_cfg_idx++;

    vsf_wifi_aic8800d_trace_debug("aic8800d: rf config @0x%08X mask=0x%08X data=0x%08X"
            VSF_TRACE_CFG_LINEEND, req.mem_addr, req.mem_mask, req.mem_data);

    __aic8800d_send_msg(wifi, AIC8800D_DBG_MEM_MASK_WRITE_REQ,
            &req, sizeof(req),
            AIC8800D_DBG_MEM_MASK_WRITE_CFM, NULL, 0,
            __aic8800d_rf_config_next);
}

static uint32_t __aic8800d_get_le32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8)
            | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/* Select the firmware bundle for the attached chip and log it.  Only the
 * AIC8800D80 (chip_rev U02/U03) variant used by the current Mercury/TP-Link
 * dongle is supported so far; its aic_load_fw uploads just the patch table
 * and the main image (no adid/patch/ext files).  Key further variants on
 * chip_rev / chip_sub_rev / USB VID:PID here when adding more AIC8800D
 * series chips — always verify the image matches the chip, otherwise the
 * download stalls or the firmware misbehaves (e.g. no data forwarding). */
static void __aic8800d_fw_select_variant(aic8800d_priv_t *priv)
{
    aic8800d_fw_variant_t *v = &priv->fw_variant;
    memset(v, 0, sizeof(*v));

    v->name        = "aic8800d80-u02";
    v->img[AIC8800D_FW_IMG_MAIN].addr = AIC8800D_FW_LOAD_ADDR;
    v->img[AIC8800D_FW_IMG_MAIN].data = __aic8800d_firmware_data;
    v->img[AIC8800D_FW_IMG_MAIN].size = __aic8800d_firmware_size;
    v->patch_table      = __aic8800d_patch_table_data;
    v->patch_table_size = __aic8800d_patch_table_size;

    vsf_wifi_aic8800d_trace_info("aic8800d: fw variant=%s chip_rev=0x%02X size=%u"
            VSF_TRACE_CFG_LINEEND, v->name, priv->chip_rev,
            v->img[AIC8800D_FW_IMG_MAIN].size);
}

static void __aic8800d_fw_set_image(aic8800d_priv_t *priv, uint8_t idx)
{
    priv->fw_addr   = priv->fw_variant.img[idx].addr;
    priv->fw_buf    = (uint8_t *)priv->fw_variant.img[idx].data;
    priv->fw_size   = priv->fw_variant.img[idx].size;
    priv->fw_offset = 0;
}

/* Parse the embedded fw_patch_table_8800d80_u02.bin into a flat (addr, value)
 * write list, replicating aicbt_patch_table_load(): records of
 * [name 16B][type u32][len u32][len * (addr,value) pairs], skip type 0x06
 * (version string) and type >= 1000 / len == 0; fill BTMODE host fields. */
#define AIC8800D_PT_REC_BTMODE    3
#define AIC8800D_PT_REC_VER_INFO  6
static vsf_err_t __aic8800d_patch_table_parse(aic8800d_priv_t *priv)
{
    const uint8_t *p   = priv->fw_variant.patch_table;
    uint32_t size      = priv->fw_variant.patch_table_size;
    uint32_t offset    = 16; /* AICBT_PT_TAG */
    uint16_t count     = 0;

    if (size < 16 || memcmp(p, "AICBT_PT_TAG", 12)) {
        return VSF_ERR_FAIL;
    }
    while (offset + 24 <= size && count < dimof(priv->patch_tbl_pairs)) {
        uint32_t type, len, i;
        offset += 16; /* name */
        type = __aic8800d_get_le32(p + offset);
        len  = __aic8800d_get_le32(p + offset + 4);
        offset += 8;
        if (type >= 1000 || len == 0) {
            continue;
        }
        if (offset + len * 8 > size) {
            return VSF_ERR_FAIL;
        }
        if (type == AIC8800D_PT_REC_VER_INFO) {
            offset += len * 8;
            continue;
        }
        for (i = 0; i < len && count < dimof(priv->patch_tbl_pairs); i++) {
            uint32_t addr = __aic8800d_get_le32(p + offset + i * 8);
            uint32_t val  = __aic8800d_get_le32(p + offset + i * 8 + 4);
            if (type == AIC8800D_PT_REC_BTMODE) {
                /* aicbt_patch_table_load() fills host-side fields (pairs 0-8
                 * only; later pairs keep the values from the file) */
                switch (i) {
                case 0:  val = 1;          break; /* hwinfo < 0 */
                case 1:  val = 0xFFFFFFFF; break; /* hwinfo */
                case 2:  val = 0;          break; /* cpmode = WORK */
                case 3: case 4: case 5: case 6: case 7: case 8:
                         val = 0;          break; /* btmode..txpwr: none for D81 */
                default: break;                   /* keep file value */
                }
            }
            priv->patch_tbl_pairs[count][0] = addr;
            priv->patch_tbl_pairs[count][1] = val;
            count++;
        }
        offset += len * 8;
    }
    priv->patch_tbl_count = count;
    priv->patch_tbl_idx   = 0;
    return VSF_ERR_NONE;
}

static void __aic8800d_patch_table_step(vsf_wifi_t *wifi, vsf_err_t err);

static void __aic8800d_patch_table_next(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (err != VSF_ERR_NONE) {
        vsf_wifi_aic8800d_trace_error("aic8800d: patch table write failed (%d)"
                VSF_TRACE_CFG_LINEEND, err);
        __aic8800d_fw_done(wifi, err);
        return;
    }
    __aic8800d_patch_table_step(wifi, VSF_ERR_NONE);
}

static void __aic8800d_patch_table_step(vsf_wifi_t *wifi, vsf_err_t err)
{
    aic8800d_priv_t *priv = __aic8800d_priv(wifi);
    struct aic8800d_dbg_mem_write_req wr_req;

    if (VSF_ERR_NONE != err) {
        __aic8800d_fw_done(wifi, err);
        return;
    }

    if (priv->patch_tbl_count == 0) {
        if (__aic8800d_patch_table_parse(priv) != VSF_ERR_NONE) {
            vsf_wifi_aic8800d_trace_error("aic8800d: patch table parse failed"
                    VSF_TRACE_CFG_LINEEND);
            __aic8800d_fw_done(wifi, VSF_ERR_FAIL);
            return;
        }
        vsf_wifi_aic8800d_trace_info("aic8800d: patch table writes=%u"
                VSF_TRACE_CFG_LINEEND, priv->patch_tbl_count);
    }

    if (priv->patch_tbl_idx >= priv->patch_tbl_count) {
        /* boot patch table done -> upload the main firmware image */
        priv->fw_img_idx = 3;
        __aic8800d_fw_set_image(priv, 3);
        __aic8800d_fw_step(wifi, VSF_ERR_NONE);
        return;
    }

    wr_req.mem_addr = priv->patch_tbl_pairs[priv->patch_tbl_idx][0];
    wr_req.mem_data = priv->patch_tbl_pairs[priv->patch_tbl_idx][1];
    priv->patch_tbl_idx++;
    __aic8800d_send_msg(wifi, AIC8800D_DBG_MEM_WRITE_REQ,
            &wr_req, sizeof(wr_req),
            AIC8800D_DBG_MEM_WRITE_CFM, NULL, 0,
            __aic8800d_patch_table_next);
}

static void __aic8800d_fw_step(vsf_wifi_t *wifi, vsf_err_t err)
{
    aic8800d_priv_t *priv = __aic8800d_priv(wifi);
    uint32_t chunk, remaining, param_len;
    uint8_t *param;
    struct aic8800d_dbg_mem_block_write_req *wr;

    if (VSF_ERR_NONE != err) {
        __aic8800d_fw_done(wifi, err);
        return;
    }

    if (priv->fw_offset >= priv->fw_size) {
        if (priv->fw_img_idx < 2) {
            /* ADID -> boot patch -> patch ext */
            priv->fw_img_idx++;
            __aic8800d_fw_set_image(priv, priv->fw_img_idx);
        } else if (priv->fw_img_idx == 2) {
            /* ext patch done -> boot patch table writes BEFORE the main
             * firmware image (Linux order: the table configures the RAM/patch
             * mapping that the main image download relies on) */
            __aic8800d_patch_table_step(wifi, VSF_ERR_NONE);
            return;
        } else {
            /* main firmware done -> runtime patch config */
            priv->patch_step = 0;
            __aic8800d_patch_config_step(wifi, VSF_ERR_NONE);
            return;
        }
    }

    remaining = priv->fw_size - priv->fw_offset;
    chunk = remaining > AIC8800D_FW_CHUNK_SIZE ? AIC8800D_FW_CHUNK_SIZE : remaining;

    param_len = (uint32_t)(sizeof(struct aic8800d_dbg_mem_block_write_req) + chunk);
    param = vsf_heap_malloc(param_len);
    if (NULL == param) {
        __aic8800d_fw_done(wifi, VSF_ERR_NOT_ENOUGH_RESOURCES);
        return;
    }
    memset(param, 0, param_len);

    wr = (struct aic8800d_dbg_mem_block_write_req *)param;
    wr->mem_addr = priv->fw_addr + priv->fw_offset;
    wr->mem_size = chunk;
    memcpy(wr->mem_data, priv->fw_buf + priv->fw_offset, chunk);

    priv->fw_offset += chunk;

    vsf_wifi_aic8800d_trace_debug("aic8800d: fw chunk @0x%08X size=%u"
            VSF_TRACE_CFG_LINEEND, wr->mem_addr, chunk);

    vsf_err_t send_err = __aic8800d_send_msg(wifi, AIC8800D_DBG_MEM_BLOCK_WRITE_REQ,
            param, (uint16_t)param_len,
            AIC8800D_DBG_MEM_BLOCK_WRITE_CFM, NULL, 0,
            __aic8800d_fw_next);

    vsf_heap_free(param);

    if (VSF_ERR_NONE != send_err) {
        __aic8800d_fw_done(wifi, send_err);
    }
}

static void __aic8800d_system_config_step(vsf_wifi_t *wifi, vsf_err_t err)
{
    aic8800d_priv_t *priv = __aic8800d_priv(wifi);
    struct aic8800d_dbg_mem_read_req rd_req;

    if (VSF_ERR_NONE != err) {
        __aic8800d_fw_done(wifi, err);
        return;
    }

    switch (priv->fw_load_step) {
    case 0:
        vsf_wifi_aic8800d_trace_info("aic8800d: system_config read 0x40500000"
                VSF_TRACE_CFG_LINEEND);
        rd_req.mem_addr = 0x40500000;
        priv->fw_load_step = 1;
        __aic8800d_send_msg(wifi, AIC8800D_DBG_MEM_READ_REQ,
                &rd_req, sizeof(rd_req),
                AIC8800D_DBG_MEM_READ_CFM, &priv->sysconfig_rd_cfm, sizeof(priv->sysconfig_rd_cfm),
                __aic8800d_system_config_step);
        break;

    case 1: {
        uint32_t rd_data = priv->sysconfig_rd_cfm.mem_data;
        priv->chip_mcu_id = ((rd_data >> 25) & 0x01UL) ? 0 : 1;
        priv->chip_rev    = (uint8_t)(rd_data >> 16);
        vsf_wifi_aic8800d_trace_info("aic8800d: chip_id=0x%02X chip_mcu_id=%u"
                VSF_TRACE_CFG_LINEEND, priv->chip_rev, priv->chip_mcu_id);
        /* chip_rev is known now: pick the matching firmware bundle */
        __aic8800d_fw_select_variant(priv);
        __aic8800d_fw_set_image(priv, 0);
        priv->fw_load_step = 2;
        __aic8800d_system_config_step(wifi, VSF_ERR_NONE);
        break;
    }

    case 2:
        if (priv->syscfg_idx < AIC8800D_SYSCFG_COUNT) {
            struct aic8800d_dbg_mem_write_req wr_req;
            wr_req.mem_addr = __aic8800d_syscfg[priv->syscfg_idx][0];
            wr_req.mem_data = __aic8800d_syscfg[priv->syscfg_idx][1];
            vsf_wifi_aic8800d_trace_info("aic8800d: syscfg write 0x%08X=0x%08X"
                    VSF_TRACE_CFG_LINEEND, wr_req.mem_addr, wr_req.mem_data);
            priv->syscfg_idx++;
            __aic8800d_send_msg(wifi, AIC8800D_DBG_MEM_WRITE_REQ,
                    &wr_req, sizeof(wr_req),
                    AIC8800D_DBG_MEM_WRITE_CFM, NULL, 0,
                    __aic8800d_system_config_step);
        } else {
            priv->fw_load_step = 3;
            __aic8800d_system_config_step(wifi, VSF_ERR_NONE);
        }
        break;

    case 3:
        if (priv->syscfg_masked_idx < AIC8800D_SYSCFG_MASKED_COUNT) {
            struct aic8800d_dbg_mem_mask_write_req wr_req;
            wr_req.mem_addr = __aic8800d_syscfg_masked[priv->syscfg_masked_idx][0];
            wr_req.mem_mask = __aic8800d_syscfg_masked[priv->syscfg_masked_idx][1];
            wr_req.mem_data = __aic8800d_syscfg_masked[priv->syscfg_masked_idx][2];
            vsf_wifi_aic8800d_trace_info("aic8800d: syscfg mask write 0x%08X mask=0x%08X data=0x%08X"
                    VSF_TRACE_CFG_LINEEND, wr_req.mem_addr, wr_req.mem_mask, wr_req.mem_data);
            priv->syscfg_masked_idx++;
            __aic8800d_send_msg(wifi, AIC8800D_DBG_MEM_MASK_WRITE_REQ,
                    &wr_req, sizeof(wr_req),
                    AIC8800D_DBG_MEM_MASK_WRITE_CFM, NULL, 0,
                    __aic8800d_system_config_step);
        } else {
            vsf_wifi_aic8800d_trace_info("aic8800d: system_config done"
                    VSF_TRACE_CFG_LINEEND);
            priv->fw_load_step = 4;
            __aic8800d_fw_step(wifi, VSF_ERR_NONE);
        }
        break;

    default:
        __aic8800d_fw_done(wifi, VSF_ERR_FAIL);
        break;
    }
}

static vsf_err_t __aic8800d_firmware_load(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    aic8800d_priv_t *priv = __aic8800d_priv(wifi);
    const vsf_wifi_aic8800d_bus_ops_t *bus_ops = __aic8800d_bus_ops(wifi);

    vsf_wifi_aic8800d_trace_info("aic8800d: firmware_load entry"
            VSF_TRACE_CFG_LINEEND);

    if (NULL == priv) {
        priv = vsf_heap_malloc(sizeof(aic8800d_priv_t));
        if (NULL == priv) return VSF_ERR_NOT_ENOUGH_RESOURCES;
        memset(priv, 0, sizeof(*priv));
        priv->wifi = wifi;
        priv->bus_ops = bus_ops;
        wifi->chip_priv = priv;
    }

    if ((bus_ops != NULL) && bus_ops->skip_firmware_load) {
        vsf_wifi_aic8800d_trace_info("aic8800d: runtime device, skip firmware load"
                VSF_TRACE_CFG_LINEEND);
        if (done != NULL) {
            done(wifi, VSF_ERR_NONE);
        }
        return VSF_ERR_NONE;
    }

    vsf_wifi_aic8800d_trace_info("aic8800d: loading firmware (embedded, size=%u)"
            VSF_TRACE_CFG_LINEEND, __aic8800d_firmware_size);

    priv->fw_img_idx = 0;
    priv->fw_done   = done;
    priv->fw_load_step = 0;
    priv->patch_tbl_count = 0;
    priv->patch_tbl_idx   = 0;

    __aic8800d_system_config_step(wifi, VSF_ERR_NONE);
    return VSF_ERR_NONE;
}

/* Init watchdog: fires when the init chain stalls on a runtime-mode device
 * (stale firmware does not answer MM_SET_STACK_START_REQ — verified against
 * a previously-connected dongle).  Reboot the chip into boot ROM so it
 * re-enumerates as a boot device and the cold download + init flow runs.
 * Fire-and-forget like the firmware-load start_app: the device disconnects
 * itself on reboot and the usbh teardown handles the rest. */
#define AIC8800D_INIT_WD_TIMEOUT_MS     5000
static void __aic8800d_init_wd_cb(vsf_callback_timer_t *timer)
{
    aic8800d_priv_t *priv = vsf_container_of(timer, aic8800d_priv_t, init_wd_timer);
    vsf_wifi_t *wifi = priv->wifi;
    struct aic8800d_dbg_start_app_req reboot_req;

    vsf_wifi_aic8800d_trace_info("aic8800d: init stalled on runtime device, rebooting to boot ROM"
            VSF_TRACE_CFG_LINEEND);
    memset(&reboot_req, 0, sizeof(reboot_req));
    reboot_req.boot_addr = 2000;    /* firmware delay before reboot (ms) */
    reboot_req.boot_type = AIC8800D_START_APP_REBOOT;
    __aic8800d_send_msg(wifi, AIC8800D_DBG_START_APP_REQ,
            &reboot_req, sizeof(reboot_req),
            0, NULL, 0,
            NULL);
}

static vsf_err_t __aic8800d_init(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    aic8800d_priv_t *priv = __aic8800d_priv(wifi);
    if (priv == NULL) {
        priv = vsf_heap_malloc(sizeof(aic8800d_priv_t));
        if (NULL == priv) return VSF_ERR_NOT_ENOUGH_RESOURCES;
        memset(priv, 0, sizeof(*priv));
        priv->wifi = wifi;
        priv->bus_ops = (const vsf_wifi_aic8800d_bus_ops_t *)wifi->bus_ops;
        wifi->chip_priv = priv;
    }

    priv->init_step  = 0;
    priv->init_done = done;
    vsf_callback_timer_init(&priv->init_wd_timer);
    priv->init_wd_timer.on_timer = __aic8800d_init_wd_cb;
    if ((priv->bus_ops != NULL) && priv->bus_ops->skip_firmware_load) {
        /* Runtime device whose firmware was NOT loaded by us: it may be a
         * stale one from a previous run that never answers the init chain.
         * Arm the watchdog; a healthy firmware completes init in ~2s. */
        vsf_callback_timer_add_ms(&priv->init_wd_timer, AIC8800D_INIT_WD_TIMEOUT_MS);
    }
    priv->scan_finish_timer.on_timer = __aic8800d_scan_finish_timer_cb;
    priv->scan_finish_pending = false;
    __aic8800d_init_step(wifi, VSF_ERR_NONE);
    return VSF_ERR_NONE;
}

static void __aic8800d_fini(vsf_wifi_t *wifi)
{
    aic8800d_priv_t *priv = __aic8800d_priv(wifi);
    if (priv != NULL) {
        vsf_callback_timer_remove(&priv->init_wd_timer);
        vsf_callback_timer_remove(&priv->scan_finish_timer);
        vsf_heap_free(priv);
        wifi->chip_priv = NULL;
    }
}

static uint8_t __aic8800d_freq_to_channel(uint16_t freq)
{
    if (freq == 2484) {
        return 14;
    } else if (freq >= 2412 && freq <= 2472) {
        return (uint8_t)((freq - 2412) / 5 + 1);
    } else if (freq >= 5000 && freq <= 5900) {
        return (uint8_t)((freq - 5000) / 5);
    }
    return 0;
}

static uint8_t __aic8800d_rsn_cipher(uint8_t cipher)
{
    switch (cipher) {
    case 1:  return WIFI_CIPHER_WEP40;
    case 2:  return WIFI_CIPHER_TKIP;
    case 4:  return WIFI_CIPHER_CCMP;
    case 5:  return WIFI_CIPHER_WEP104;
    default: return WIFI_CIPHER_NONE;
    }
}

/* Parse an RSN IE body (after tag+len) and fill security fields.
 * Only WPA2-PSK is recognised (AKM suite 00-0F-AC-02). */
static void __aic8800d_parse_rsn(const uint8_t *body, uint8_t len,
        vsf_wifi_scan_result_t *result)
{
    static const uint8_t oui[3] = { 0x00, 0x0F, 0xAC };
    const uint8_t *p   = body;
    const uint8_t *end = body + len;
    uint16_t count, k;
    bool     has_psk  = false;
    uint8_t  pairwise = WIFI_CIPHER_NONE;
    uint8_t  group    = WIFI_CIPHER_NONE;

    if (len < 8) return;
    p += 2;                             /* skip version */
    if (!memcmp(p, oui, 3)) group = __aic8800d_rsn_cipher(p[3]);
    p += 4;
    if (p + 2 > end) return;
    count = (uint16_t)(p[0] | ((uint16_t)p[1] << 8)); p += 2;
    for (k = 0; k < count; k++) {
        if (p + 4 > end) return;
        if (!memcmp(p, oui, 3)) {
            uint8_t c = __aic8800d_rsn_cipher(p[3]);
            if (c == WIFI_CIPHER_CCMP)              pairwise = WIFI_CIPHER_CCMP;
            else if (pairwise == WIFI_CIPHER_NONE)  pairwise = c;
        }
        p += 4;
    }
    if (p + 2 > end) return;
    count = (uint16_t)(p[0] | ((uint16_t)p[1] << 8)); p += 2;
    for (k = 0; k < count; k++) {
        if (p + 4 > end) return;
        if (!memcmp(p, oui, 3) && (p[3] == 2)) has_psk = true;
        p += 4;
    }

    if (has_psk) {
        result->auth_mode       = WIFI_AUTH_WPA2_PSK;
        result->pairwise_cipher = (pairwise != WIFI_CIPHER_NONE)
                                ? pairwise : WIFI_CIPHER_CCMP;
        result->group_cipher    = group;
        if (p + 2 <= end) {
            result->rsn_cap = (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
        }
    }
}

static void __aic8800d_scan_cfm_cb(vsf_wifi_t *wifi, vsf_err_t err)
{
    aic8800d_priv_t *priv = __aic8800d_priv(wifi);

    if (VSF_ERR_NONE != err) {
        vsf_wifi_done_t done = NULL;
        if (priv != NULL) {
            done = priv->scan_done;
            priv->scan_done = NULL;
            priv->scan_active = false;
            priv->scan_finish_pending = false;
            vsf_callback_timer_remove(&priv->scan_finish_timer);
        }
        if (done != NULL) {
            done(wifi, err);
        }
        return;
    }
    /* 0x1009 (SCANU_START_CFM_ADDITIONAL) is an early "scan started" confirm.
     * The actual scan completion message with result count is 0x1001
     * (SCANU_START_CFM), which is handled in handle_e2a. */
    vsf_wifi_aic8800d_trace_info("aic8800d: scanu_start_cfm_additional (scan started)" VSF_TRACE_CFG_LINEEND);
}

static void __aic8800d_scan_finish_timer_cb(vsf_callback_timer_t *timer)
{
    aic8800d_priv_t *priv = vsf_container_of(timer, aic8800d_priv_t, scan_finish_timer);
    vsf_wifi_t *wifi = priv->wifi;
    vsf_wifi_aic8800d_trace_info("aic8800d: scan finish timer fired pending=%u got=%u/%u"
            VSF_TRACE_CFG_LINEEND,
            priv->scan_finish_pending, priv->scan_results_received,
            priv->scan_results_expected);
    if (priv->scan_finish_pending) {
        /* If results are still arriving, extend the guard window once more.
         * The timer is reset by every incoming SCANU_RESULT_IND while traffic
         * is flowing, so reaching here means results have paused. Bound the
         * number of extensions: a lost SCANU_RESULT_IND (e.g. under RX event
         * flood) must not stall the scan forever — finish with the results
         * that did arrive. */
        if (priv->scan_results_received < priv->scan_results_expected) {
            if (priv->scan_finish_retries < 3) {
                priv->scan_finish_retries++;
                vsf_callback_timer_add_ms(timer, 1500);
                vsf_wifi_aic8800d_trace_info(
                        "aic8800d: scan finish delayed, got %u/%u results" VSF_TRACE_CFG_LINEEND,
                        priv->scan_results_received, priv->scan_results_expected);
                return;
            }
            vsf_wifi_aic8800d_trace_info(
                    "aic8800d: scan finish with %u/%u results (lost IND)" VSF_TRACE_CFG_LINEEND,
                    priv->scan_results_received, priv->scan_results_expected);
        }
        priv->scan_finish_pending = false;
        __aic8800d_scan_finish(wifi);
    }
}

static void __aic8800d_scan_finish(vsf_wifi_t *wifi)
{
    aic8800d_priv_t *priv = __aic8800d_priv(wifi);
    vsf_wifi_done_t done = NULL;

    if (priv != NULL) {
        priv->scan_finish_pending = false;
        vsf_callback_timer_remove(&priv->scan_finish_timer);
        done = priv->scan_done;
        priv->scan_done = NULL;
        priv->scan_active = false;
    }
    if (done != NULL) {
        done(wifi, VSF_ERR_NONE);
    }
}

static vsf_err_t __aic8800d_scan(vsf_wifi_t *wifi,
        const uint8_t *channels, uint8_t num_channels, uint16_t dwell_ms,
        vsf_wifi_done_t done)
{
    aic8800d_priv_t *priv = __aic8800d_priv(wifi);
    struct aic8800d_scanu_start_req req;
    uint16_t freq;

    if (priv == NULL) return VSF_ERR_NOT_READY;
    if (num_channels == 0 || num_channels > AIC8800D_SCAN_CHANNEL_MAX)
        return VSF_ERR_INVALID_PARAMETER;

    memset(&req, 0, sizeof(req));
    vsf_wifi_aic8800d_trace_debug("aic8800d: scan req vif=%u num_channels=%u" VSF_TRACE_CFG_LINEEND,
            priv->vif_idx, num_channels);
    req.vif_idx  = priv->vif_idx;
    req.chan_cnt = num_channels;
    /* Wildcard active scan: one empty SSID entry (length 0), mirroring
     * cfg80211/wpa_supplicant wildcard scans (n_ssids=1, ssid_len=0) so the
     * firmware actively probes instead of passively listening. */
    req.ssid_cnt = 1;
    req.duration = 0;                   /* use firmware default dwell time */
    for (int i = 0; i < num_channels; i++) {
        vsf_wifi_aic8800d_trace_debug("aic8800d: scan chan[%d]=%u" VSF_TRACE_CFG_LINEEND,
                i, channels[i]);
        req.chan[i].band  = (channels[i] <= 14) ? 0 : 1; /* 0=2.4G, 1=5G */
        req.chan[i].flags = 0;
        freq = channels[i];
        if (channels[i] <= 14) {
            freq = 2412 + (channels[i] - 1) * 5;
        } else {
            freq = 5000 + channels[i] * 5;
        }
        req.chan[i].freq      = freq;
        req.chan[i].tx_power  = 20;
        req.chan[i].__pad     = 0;
    }

    memset(&req.bssid, 0xFF, sizeof(req.bssid));
    priv->scan_done           = done;
    priv->scan_active          = true;
    priv->scan_results_received = 0;
    priv->scan_results_expected = 0;
    priv->scan_rsn_cap_cache_num = 0;

    return __aic8800d_send_msg(wifi, AIC8800D_SCANU_START_REQ,
            &req, sizeof(req),
            AIC8800D_SCANU_START_CFM_ADDTIONAL, NULL, 0,
            __aic8800d_scan_cfm_cb);
}


static vsf_err_t __aic8800d_connect(vsf_wifi_t *wifi,
        const uint8_t bssid[6], const uint8_t *ssid, uint8_t ssid_len,
        uint8_t channel, vsf_wifi_done_t done)
{
    aic8800d_priv_t *priv = __aic8800d_priv(wifi);
    struct aic8800d_sm_connect_req req;
    uint16_t freq;

    if (priv == NULL) return VSF_ERR_NOT_READY;
    if (ssid_len > 32) return VSF_ERR_INVALID_PARAMETER;

    memset(&req, 0, sizeof(req));
    req.ssid.length = ssid_len;
    memcpy(req.ssid.array, ssid, ssid_len);
    memcpy(&req.bssid, bssid, 6);

    req.chan.band  = (channel <= 14) ? 0 : 1;
    req.chan.flags = 0;
    /* Use the channel frequency pinned to the scan result. Linux also allows
     * (u16)-1 when cfg80211 has no channel pointer, but the AIC8800D80 firmware
     * appears to require a concrete frequency for SM_CONNECT_REQ validation. */
    if (channel <= 14) {
        req.chan.freq = (uint16_t)(2412 + (channel - 1) * 5);
    } else {
        req.chan.freq = (uint16_t)(5000 + channel * 5);
    }
    req.chan.tx_power  = 0;
    req.chan.__pad     = 0;

    req.flags = 0;
    req.ctrl_port_ethertype = 0; /* no control port for open network */
    req.listen_interval = 0;
    req.dont_wait_bcmc = 0;
    req.vif_idx = priv->vif_idx;
    req.auth_type = 0; /* OPEN */

#if VSF_WIFI_USE_WPA == ENABLED
    /* For WPA2-PSK match Linux flags: WPA_WPA2_IN_USE | CONTROL_PORT_HOST.
     * Include RSN, HT capabilities and WMM IEs in the AssocReq buffer. */
    if (wifi->wpa_auth.auth_mode == WIFI_AUTH_WPA2_PSK) {
        req.flags |= AIC8800D_CONNECTION_FLAG_WPA_WPA2_IN_USE
                  |  AIC8800D_CONNECTION_FLAG_CONTROL_PORT_HOST;
        req.ctrl_port_ethertype = 0x8E88; /* EAPOL in network byte order */

        static const uint8_t __aic8800d_assoc_ie[] = {
            /* RSN IE (tag 48) for WPA2-PSK/CCMP, as emitted by Linux cfg80211 */
            0x30, 0x14,             /* tag, length 20 */
            0x01, 0x00,             /* RSN version 1 */
            0x00, 0x0F, 0xAC, 0x04, /* group cipher = CCMP */
            0x01, 0x00,             /* pairwise count = 1 */
            0x00, 0x0F, 0xAC, 0x04, /* pairwise = CCMP */
            0x01, 0x00,             /* AKM count = 1 */
            0x00, 0x0F, 0xAC, 0x02, /* AKM = PSK */
            0x00, 0x00,             /* RSN capabilities, filled from AP below */
            /* Extended Capabilities IE (tag 127), as emitted by Linux cfg80211 */
            0x7F, 0x0B,
            0x04, 0x00, 0x0A, 0x02, 0x00, 0x40, 0x40, 0x40, 0x00, 0x01, 0x20,
            /* Management MIC / AIC-specific IE (tag 0x3B), captured from Linux */
            0x3B, 0x14,
            0x81, 0x51, 0x52, 0x53, 0x54, 0x73, 0x74, 0x75, 0x76, 0x77,
            0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x80, 0x82,
        };
        uint16_t rsn_cap = 0x8000; /* fallback: most APs set MFPC */
        for (uint8_t k = 0; k < priv->scan_rsn_cap_cache_num; k++) {
            if (!memcmp(priv->scan_rsn_cap_cache[k].bssid, bssid, 6)) {
                rsn_cap = priv->scan_rsn_cap_cache[k].rsn_cap;
                vsf_wifi_aic8800d_trace_info("aic8800d: rsn_cap cache hit idx=%u bssid=%02X:%02X:%02X:%02X:%02X:%02X rsn_cap=0x%04X" VSF_TRACE_CFG_LINEEND,
                        k, bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5], rsn_cap);
                break;
            }
        }
        memcpy(req.ie_buf, __aic8800d_assoc_ie, sizeof(__aic8800d_assoc_ie));
        /* Copy AP RSN capabilities (e.g. MFPC 0x8000) so the firmware sees the
         * same STA RSN cap bits that Linux cfg80211 derives from the beacon.
         * The reference Linux driver stores RSN cap in the IE buffer in
         * big-endian order (matching the air/IE layout). */
        uint8_t *ie_bytes = (uint8_t *)req.ie_buf;
        ie_bytes[20] = (uint8_t)((rsn_cap >> 8) & 0xFF);
        ie_bytes[21] = (uint8_t)(rsn_cap & 0xFF);
        req.ie_len = sizeof(__aic8800d_assoc_ie);
        priv->ap_rsn_cap = rsn_cap;

        /* Cache the STA RSN IE (first 22 bytes of the assoc IE buffer) so the
         * 4-way handshake M2 can echo it in its key_data — the AP verifies M2's
         * RSN IE matches the assoc-req's.  Without this the M2 carries no RSN
         * IE and the AP rejects the handshake. */
        memcpy(wifi->wpa_rsn_ie, ie_bytes, 22);
        wifi->wpa_rsn_ie_len = 22;
    }
#endif

    req.uapsd_queues = 0x01; /* Linux default IEEE80211_WMM_IE_STA_QOSINFO_AC_VO */

    priv->connect_done   = done;
    priv->connect_active = true;
    memcpy(&priv->connect_req, &req, sizeof(req));

    /* Linux cfg80211_connect sends SM_CONNECT_REQ directly; it does not emit a
     * SCANU_CANCEL_REQ first. Send the connect request immediately. */
    return __aic8800d_send_msg(wifi, AIC8800D_SM_CONNECT_REQ,
            &priv->connect_req, sizeof(priv->connect_req),
            0, NULL, 0,
            NULL);
}

static void __aic8800d_connect_cancel_cfm(vsf_wifi_t *wifi, vsf_err_t err)
{
    aic8800d_priv_t *priv = __aic8800d_priv(wifi);

    if (err != VSF_ERR_NONE) {
        if (priv->connect_active && priv->connect_done != NULL) {
            priv->connect_active = false;
            vsf_wifi_done_t done = priv->connect_done;
            priv->connect_done = NULL;
            done(wifi, VSF_ERR_FAIL);
        }
        return;
    }

    __aic8800d_send_msg(wifi, AIC8800D_SM_CONNECT_REQ,
            &priv->connect_req, sizeof(priv->connect_req),
            0, NULL, 0,
            NULL);
}

static vsf_err_t __aic8800d_disconnect(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    aic8800d_priv_t *priv = __aic8800d_priv(wifi);
    struct aic8800d_sm_disconnect_req req;

    if (priv == NULL) return VSF_ERR_NOT_READY;
    memset(&req, 0, sizeof(req));
    req.vif_idx = priv->vif_idx;
    req.reason_code = 3;

    return __aic8800d_send_msg(wifi, AIC8800D_SM_DISCONNECT_REQ,
            &req, sizeof(req),
            AIC8800D_SM_DISCONNECT_CFM, NULL, 0,
            done);
}

static vsf_err_t __aic8800d_get_link_info(vsf_wifi_t *wifi,
        vsf_wifi_link_info_t *info)
{
    (void)wifi; (void)info;
    return VSF_ERR_NOT_SUPPORT;
}

static vsf_err_t __aic8800d_tx(vsf_wifi_t *wifi,
        const uint8_t *frame, uint16_t len)
{
    const vsf_wifi_aic8800d_bus_ops_t *bus_ops = __aic8800d_bus_ops(wifi);
    aic8800d_priv_t *priv = __aic8800d_priv(wifi);
    uint16_t payload_len;
    uint16_t usb_len;
    uint8_t *buf;
    vsf_err_t err;

    if (NULL == bus_ops || NULL == bus_ops->send)
        return VSF_ERR_NOT_READY;
    if (len < 14)
        return VSF_ERR_INVALID_PARAMETER;

    /* USB TX aggregation header + txdesc_api + Ethernet frame payload.
     * The Linux driver builds txdesc_api from the Ethernet header and
     * appends payload after it. */
    payload_len = AIC8800D_TXDESC_API_SIZE + (len - 14);
    usb_len = AIC8800D_USB_TX_HEADER_LEN + AIC8800D_TXDESC_API_SIZE + (len - 14);
    if (usb_len > AIC8800D_CMD_BUF_SIZE)
        return VSF_ERR_NOT_SUPPORT;

    buf = vsf_heap_malloc(usb_len);
    if (NULL == buf) return VSF_ERR_NOT_ENOUGH_RESOURCES;
    memset(buf, 0, usb_len);

    /* USB data TX header (4 bytes): [total_len u16][type=0x01][reserved u8],
     * matching the Linux driver's unaggregated data path (aicwf_usb_send_pkt). */
    buf[0] = (usb_len >> 0) & 0xFF;
    buf[1] = (usb_len >> 8) & 0x0F;
    buf[2] = AIC8800D_USB_DATA_TYPE;
    buf[3] = 0;

    /* host descriptor */
    struct aic8800d_txdesc_api *desc = (struct aic8800d_txdesc_api *)(buf + AIC8800D_USB_TX_HEADER_LEN);
    desc->host.packet_len = len - 14;
    memcpy(&desc->host.eth_dest_addr, frame + 0, 6);
    memcpy(&desc->host.eth_src_addr, frame + 6, 6);
    desc->host.ethertype = (uint16_t)frame[12] | ((uint16_t)frame[13] << 8);
    /* Request a TX confirmation for EAPOL-Key frames (bit31 + desc index),
     * matching the Linux driver (status_desc_addr = (1<<31) | idx). */
    if ((frame[12] == 0x88) && (frame[13] == 0x8E)) {
        desc->host.status_desc_addr = (1u << 31) | 1;
    } else {
        vsf_wifi_aic8800d_trace_debug("aic8800d: biz tx len=%u et=%02X%02X da=%02X:%02X:%02X:%02X:%02X:%02X"
                VSF_TRACE_CFG_LINEEND, len, frame[12], frame[13],
                frame[0], frame[1], frame[2], frame[3], frame[4], frame[5]);
    }
    desc->host.vif_idx = priv->vif_idx;
    desc->host.staid   = priv->sta_idx;
    /* EAPOL-Key frames are sent as QoS data (matching the AP's M1 and the
     * Linux driver's behaviour); other traffic defaults to non-QoS. */
    desc->host.tid     = ((frame[12] == 0x88) && (frame[13] == 0x8E)) ? 0 : 0xFF;
    desc->host.ac      = 1;    /* BE */
    desc->host.flags   = 0;

    /* Ethernet payload follows the descriptor */
    memcpy(buf + AIC8800D_USB_TX_HEADER_LEN + AIC8800D_TXDESC_API_SIZE, frame + 14, len - 14);

    if ((desc->host.ethertype == 0x8E88) || (desc->host.ethertype == 0x888E)) {
        char hexbuf[8 * 48 + 1];
        uint16_t n = (usb_len < 48) ? usb_len : 48;
        for (uint16_t i = 0; i < n; i++) {
            static const char hexdig[] = "0123456789ABCDEF";
            hexbuf[i * 2]     = hexdig[(buf[i] >> 4) & 0xF];
            hexbuf[i * 2 + 1] = hexdig[buf[i] & 0xF];
        }
        hexbuf[n * 2] = '\0';
        vsf_wifi_aic8800d_trace_debug("aic8800d: EAPOL tx usb_len=%u: %s" VSF_TRACE_CFG_LINEEND, usb_len, hexbuf);
    }

    err = bus_ops->send(wifi, buf, usb_len, NULL);
    vsf_heap_free(buf);
    return err;
}

#if VSF_WIFI_USE_WPA == ENABLED
/* Key installation (WPA 4-way handshake backend).
 * PTK (pairwise): MM_KEY_ADD_REQ with sta_idx of the AP; GTK (group): default
 * key with sta_idx=0xFF.  After the GTK is programmed the 802.1X control port
 * is opened via ME_SET_CONTROL_PORT_REQ so business data can flow (Linux
 * flow: KEY_ADD x2 -> SET_CONTROL_PORT). */
static void __aic8800d_key_install_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    aic8800d_priv_t *priv = __aic8800d_priv(wifi);
    vsf_wifi_done_t done = priv->key_done;
    priv->key_done = NULL;
    priv->key_open_port = false;
    if (done != NULL) {
        done(wifi, err);
    }
}

static void __aic8800d_key_add_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    aic8800d_priv_t *priv = __aic8800d_priv(wifi);

    if ((err == VSF_ERR_NONE) && (priv->key_add_cfm.status != 0)) {
        err = VSF_ERR_FAIL;
    }
    if (err != VSF_ERR_NONE) {
        __aic8800d_key_install_done(wifi, err);
        return;
    }

    if (priv->key_open_port) {
        struct aic8800d_me_set_control_port_req req;
        req.sta_idx           = priv->sta_idx;
        req.control_port_open = 1;
        vsf_err_t send_err = __aic8800d_send_msg(wifi,
                AIC8800D_ME_SET_CONTROL_PORT_REQ, &req, sizeof(req),
                AIC8800D_ME_SET_CONTROL_PORT_CFM, NULL, 0,
                __aic8800d_key_install_done);
        if (send_err != VSF_ERR_NONE) {
            __aic8800d_key_install_done(wifi, send_err);
        }
        return;
    }
    __aic8800d_key_install_done(wifi, VSF_ERR_NONE);
}

static vsf_err_t __aic8800d_install_key(vsf_wifi_t *wifi, uint8_t key_idx,
        bool pairwise, const uint8_t *key, uint8_t key_len,
        const uint8_t *bssid, vsf_wifi_done_t done)
{
    aic8800d_priv_t *priv = __aic8800d_priv(wifi);
    struct aic8800d_mm_key_add_req req;
    (void)bssid;

    if ((priv == NULL) || (key == NULL) || (key_len > AIC8800D_SEC_KEY_LEN)) {
        return VSF_ERR_INVALID_PARAMETER;
    }
    if (priv->key_done != NULL) {
        return VSF_ERR_NOT_AVAILABLE;
    }

    memset(&req, 0, sizeof(req));
    req.key_idx      = pairwise ? 0 : key_idx;
    req.sta_idx      = pairwise ? priv->sta_idx : 0xFF;
    req.key_len      = key_len;
    memcpy(req.key, key, key_len);
    req.cipher_suite = AIC8800D_CIPHER_CCMP;
    req.inst_nbr     = priv->vif_idx;
    req.pairwise     = pairwise ? 1 : 0;

    priv->key_done      = done;
    priv->key_open_port = !pairwise;    /* open the control port after GTK */
    vsf_err_t err = __aic8800d_send_msg(wifi, AIC8800D_MM_KEY_ADD_REQ,
            &req, sizeof(req),
            AIC8800D_MM_KEY_ADD_CFM, &priv->key_add_cfm, sizeof(priv->key_add_cfm),
            __aic8800d_key_add_done);
    if (err != VSF_ERR_NONE) {
        priv->key_done      = NULL;
        priv->key_open_port = false;
    }
    return err;
}

static const vsf_wifi_crypto_ops_t __aic8800d_crypto_ops = {
    .install_key = __aic8800d_install_key,
};
#endif  /* VSF_WIFI_USE_WPA == ENABLED */

/*============================ GLOBAL VARIABLES ==============================*/

const vsf_wifi_chip_drv_t vsf_wifi_aic8800d_drv = {
    .name           = "aic8800d",
    .flags          = VSF_WIFI_CHIP_FLAG_FULLMAC,
    .firmware_load  = __aic8800d_firmware_load,
    .init           = __aic8800d_init,
    .fini           = __aic8800d_fini,
    .fullmac = {
        .scan           = __aic8800d_scan,
        .connect        = __aic8800d_connect,
        .disconnect     = __aic8800d_disconnect,
        .get_link_info  = __aic8800d_get_link_info,
    },
#if VSF_WIFI_USE_WPA == ENABLED
    .crypto_ops     = &__aic8800d_crypto_ops,
#endif
    .tx             = __aic8800d_tx,
};

#endif      /* VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_AIC8800D == ENABLED */
