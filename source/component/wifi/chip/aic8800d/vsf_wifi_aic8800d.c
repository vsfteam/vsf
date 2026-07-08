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
#include "service/heap/vsf_heap.h"

#if VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_AIC8800D == ENABLED

/*============================ MACROS ========================================*/

#define AIC8800D_USB_CMD_TYPE       0x11
#define AIC8800D_USB_DATA_TYPE      0x01
#define AIC8800D_USB_DUMMY_LEN      4

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

    vsf_err_t err = bus_ops->send(wifi, buf, usb_len, NULL);
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

    vsf_wifi_aic8800d_trace_debug("aic8800d: e2a id=0x%04X len=%u"
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
        vsf_wifi_aic8800d_trace_info("aic8800d: connect_ind status=%d"
                VSF_TRACE_CFG_LINEEND, ind->status_code);
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
                info.flags = WIFI_LINK_FLAG_CONNECTED | WIFI_LINK_FLAG_AUTHORIZED;
                vsf_wifi_on_link_up(wifi, &info);
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
        vsf_wifi_aic8800d_trace_debug("aic8800d: scan_result len=%u rssi=%d"
                VSF_TRACE_CFG_LINEEND, ind->length, ind->rssi);
        /* TODO: parse beacon/probe response and call vsf_wifi_on_scan_result() */
        break;
    }
    default:
        break;
    }
}

void vsf_wifi_aic8800d_on_rx(vsf_wifi_t *wifi, uint8_t *buf, uint16_t len)
{
    aic8800d_priv_t *priv = __aic8800d_priv(wifi);
    uint16_t payload_len;
    uint8_t  type;
    (void)priv;

    vsf_trace_info("aic8800d: on_rx len=%u hdr=%02X%02X%02X%02X" VSF_TRACE_CFG_LINEEND,
            (unsigned)len, buf[0], buf[1], buf[2], buf[3]);
    if (len <= 32) {
        vsf_trace_info("aic8800d: on_rx data=");
        for (uint16_t i = 0; i < len; i++) {
            vsf_trace_info("%02X", buf[i]);
        }
        vsf_trace_info(VSF_TRACE_CFG_LINEEND);
    }

    if (len < 4) return;
    payload_len = buf[0] | ((buf[1] & 0x0F) << 8);
    type        = buf[2];
    vsf_trace_info("aic8800d: on_rx type=0x%02X payload=%u" VSF_TRACE_CFG_LINEEND,
            type, payload_len);

    if ((type & AIC8800D_USB_TYPE_CFG) == 0) {
        /* Data path: firmware-delivered Ethernet or 802.11 frame */
        /* TODO: strip USB header + hw_rxhdr, convert to Ethernet, deliver */
        vsf_wifi_aic8800d_trace_debug("aic8800d: rx data payload=%u"
                VSF_TRACE_CFG_LINEEND, payload_len);
        return;
    }

    if (type == AIC8800D_USB_TYPE_CFG_CMD_RSP) {
        /* E2A message follows immediately after the 4-byte USB header
         * (Linux aic_txrxif.c passes skb_inblock->data + 4 to rwnx_rx_handle_msg). */
        const uint8_t e2a_header_size = 4 + 12; /* id/dest/src/param_len/pattern */
        if (len < e2a_header_size) {
            vsf_trace_info("aic8800d: on_rx cmd_rsp too short (%u < %u)" VSF_TRACE_CFG_LINEEND,
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
        vsf_trace_info("aic8800d: on_rx e2a id=0x%04X param_len=%u pattern=0x%08X" VSF_TRACE_CFG_LINEEND,
                msg.id, msg.param_len, msg.pattern);
        if (len < e2a_header_size + msg.param_len) {
            vsf_trace_info("aic8800d: on_rx e2a truncated (%u < %u)" VSF_TRACE_CFG_LINEEND,
                    len, e2a_header_size + msg.param_len);
            return;
        }
        if (msg.param_len > sizeof(msg.param))
            return;
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
        priv->init_step++;
        __aic8800d_send_msg(wifi, AIC8800D_MM_SET_STACK_START_REQ,
                &stack_req, sizeof(stack_req),
                AIC8800D_MM_SET_STACK_START_CFM, &stack_cfm, sizeof(stack_cfm),
                __aic8800d_init_done_cb);
        break;

    case 1:
        vsf_wifi_aic8800d_trace_info("aic8800d: MM_GET_MAC_ADDR_REQ"
                VSF_TRACE_CFG_LINEEND);
        memset(&mac_req, 0, sizeof(mac_req));
        priv->init_step++;
        __aic8800d_send_msg(wifi, AIC8800D_MM_GET_MAC_ADDR_REQ,
                &mac_req, sizeof(mac_req),
                AIC8800D_MM_GET_MAC_ADDR_CFM, &mac_cfm, sizeof(mac_cfm),
                __aic8800d_init_done_cb);
        break;

    case 2:
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
                &reset_req, sizeof(reset_req),
                AIC8800D_MM_RESET_CFM, &reset_cfm, sizeof(reset_cfm),
                __aic8800d_init_done_cb);
        break;

    case 3:
        vsf_wifi_aic8800d_trace_info("aic8800d: MM_VERSION_REQ"
                VSF_TRACE_CFG_LINEEND);
        memset(&version_req, 0, sizeof(version_req));
        priv->init_step++;
        __aic8800d_send_msg(wifi, AIC8800D_MM_VERSION_REQ,
                &version_req, sizeof(version_req),
                AIC8800D_MM_VERSION_CFM, &version_cfm, sizeof(version_cfm),
                __aic8800d_init_done_cb);
        break;

    case 4:
        vsf_wifi_aic8800d_trace_info("aic8800d: FW version=0x%08X"
                VSF_TRACE_CFG_LINEEND, version_cfm.version_lmac);
        vsf_wifi_aic8800d_trace_info("aic8800d: ME_CONFIG_REQ"
                VSF_TRACE_CFG_LINEEND);
        priv->init_step++;
        __aic8800d_send_msg(wifi, AIC8800D_ME_CONFIG_REQ,
                NULL, 0,
                AIC8800D_ME_CONFIG_CFM, NULL, 0,
                __aic8800d_init_done_cb);
        break;

    case 5:
        vsf_wifi_aic8800d_trace_info("aic8800d: ME_CHAN_CONFIG_REQ"
                VSF_TRACE_CFG_LINEEND);
        priv->init_step++;
        __aic8800d_send_msg(wifi, AIC8800D_ME_CHAN_CONFIG_REQ,
                NULL, 0,
                AIC8800D_ME_CHAN_CONFIG_CFM, NULL, 0,
                __aic8800d_init_done_cb);
        break;

    case 6:
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

    case 7:
        priv->vif_idx = add_if_cfm.inst_nbr;
        vsf_wifi_aic8800d_trace_info("aic8800d: vif_idx=%u status=%u"
                VSF_TRACE_CFG_LINEEND, add_if_cfm.inst_nbr, add_if_cfm.status);

        vsf_wifi_aic8800d_trace_info("aic8800d: MM_SET_FILTER_REQ"
                VSF_TRACE_CFG_LINEEND);
        memset(&filter_req, 0, sizeof(filter_req));
        filter_req.filter = 0xFFFFFFFF;
        filter_req.active_filter = 0xFFFFFFFF;
        priv->init_step++;
        __aic8800d_send_msg(wifi, AIC8800D_MM_SET_FILTER_REQ,
                &filter_req, sizeof(filter_req),
                AIC8800D_MM_SET_FILTER_CFM, NULL, 0,
                __aic8800d_init_done_cb);
        break;

    case 8:
        vsf_wifi_aic8800d_trace_info("aic8800d: init done" VSF_TRACE_CFG_LINEEND);
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
        priv->patch_step = 0;
        __aic8800d_patch_config_step(wifi, VSF_ERR_NONE);
        return;
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
    wr->mem_addr = AIC8800D_FW_LOAD_ADDR + priv->fw_offset;
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
        vsf_wifi_aic8800d_trace_info("aic8800d: chip_id=0x%02X chip_mcu_id=%u"
                VSF_TRACE_CFG_LINEEND,
                (unsigned)(uint8_t)(rd_data >> 16), priv->chip_mcu_id);
        /* syscfg_tbl_8800d80 / syscfg_tbl_masked_8800d80 are empty in the
         * default Linux build, so no register writes are required here.
         * If PMIC or chip-specific config tables are added, write them here. */
        priv->fw_load_step = 2;
        __aic8800d_fw_step(wifi, VSF_ERR_NONE);
        break;
    }

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

    priv->fw_buf    = (uint8_t *)__aic8800d_firmware_data;
    priv->fw_size   = __aic8800d_firmware_size;
    priv->fw_offset = 0;
    priv->fw_done   = done;
    priv->fw_load_step = 0;

    __aic8800d_system_config_step(wifi, VSF_ERR_NONE);
    return VSF_ERR_NONE;
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
    __aic8800d_init_step(wifi, VSF_ERR_NONE);
    return VSF_ERR_NONE;
}

static void __aic8800d_fini(vsf_wifi_t *wifi)
{
    aic8800d_priv_t *priv = __aic8800d_priv(wifi);
    if (priv != NULL) {
        vsf_heap_free(priv);
        wifi->chip_priv = NULL;
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
    req.vif_idx  = priv->vif_idx;
    req.chan_cnt = num_channels;
    req.ssid_cnt = 0;
    req.duration = dwell_ms * 1000; /* us */
    for (int i = 0; i < num_channels; i++) {
        req.chan[i].band  = (channels[i] <= 14) ? 0 : 1; /* 0=2.4G, 1=5G */
        req.chan[i].flags = 0;
        freq = channels[i];
        if (channels[i] <= 14) {
            freq = 2412 + (channels[i] - 1) * 5;
        } else {
            freq = 5000 + channels[i] * 5;
        }
        req.chan[i].freq  = freq;
    }

    memset(&req.bssid, 0xFF, sizeof(req.bssid));
    priv->scan_done   = done;
    priv->scan_active = true;

    return __aic8800d_send_msg(wifi, AIC8800D_SCANU_START_REQ,
            &req, sizeof(req),
            AIC8800D_SCANU_START_CFM, NULL, 0,
            NULL);
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
    freq = (channel <= 14) ? (2412 + (channel - 1) * 5)
                            : (5000 + channel * 5);
    req.chan.freq  = freq;

    req.flags = 0;
    req.ctrl_port_ethertype = 0x8E88; /* EAPOL in LE16 */
    req.vif_idx = priv->vif_idx;
    req.auth_type = 0; /* OPEN */

    priv->connect_done   = done;
    priv->connect_active = true;

    return __aic8800d_send_msg(wifi, AIC8800D_SM_CONNECT_REQ,
            &req, sizeof(req),
            AIC8800D_SM_CONNECT_CFM, NULL, 0,
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
    uint16_t usb_len;
    uint8_t *buf;
    vsf_err_t err;

    if (NULL == bus_ops || NULL == bus_ops->send)
        return VSF_ERR_NOT_READY;

    /* TODO: build txdesc_api from Ethernet frame */
    usb_len = 4 + len;
    if (usb_len > AIC8800D_CMD_BUF_SIZE)
        return VSF_ERR_NOT_SUPPORT;

    buf = vsf_heap_malloc(usb_len);
    if (NULL == buf) return VSF_ERR_NOT_ENOUGH_RESOURCES;
    memset(buf, 0, usb_len);

    buf[0] = (usb_len >> 0) & 0xFF;
    buf[1] = (usb_len >> 8) & 0x0F;
    buf[2] = AIC8800D_USB_DATA_TYPE;
    buf[3] = 0;
    memcpy(buf + 4, frame, len);

    err = bus_ops->send(wifi, buf, usb_len, NULL);
    vsf_heap_free(buf);
    return err;
}

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
    .tx             = __aic8800d_tx,
};

#endif      /* VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_AIC8800D == ENABLED */
