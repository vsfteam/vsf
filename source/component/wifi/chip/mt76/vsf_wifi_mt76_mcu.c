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

static void __mt76_mcu_msg_out_done(vsf_wifi_t *wifi, vsf_err_t err);

/*============================ IMPLEMENTATION ================================*/

/*============================ MCU helpers ===================================*/

static void __mt76_mcu_msg_out_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    vsf_wifi_chip_mt76_trace_info(
        "mt76: mcu out_done err=%d wait_resp=%d wanted_seq=%u"
        VSF_TRACE_CFG_LINEEND,
        (int)err, (int)priv->mcu_wait_resp, (unsigned)priv->mcu_wait_seq);

    if (err != VSF_ERR_NONE) {
        /* OUT transfer failed; the command was not accepted. */
        priv->mcu_wait_resp = false;
        vsf_wifi_done_t done = priv->mcu_wait_done;
        priv->mcu_wait_done = NULL;
        if (done != NULL) done(wifi, err);
        return;
    }

    /* Linux mt76x02u_mcu_send_msg submits the OUT URB first, waits for it
     * to complete, and then waits for the CMD_RESP IN URB.  Keep the same
     * ordering here: arm the response URB only after the OUT transfer has
     * been ACKed by the device. */
    if (priv->mcu_wait_resp) {
        vsf_err_t rx_err = __mt76_rx_submit(wifi, NULL, 0,
                             MT76_EP_IN_CMD_RESP);
        vsf_wifi_chip_mt76_trace_info(
            "mt76: mcu rx_submit err=%d" VSF_TRACE_CFG_LINEEND, (int)rx_err);
        if (rx_err != VSF_ERR_NONE) {
            priv->mcu_wait_resp = false;
            vsf_wifi_done_t done = priv->mcu_wait_done;
            priv->mcu_wait_done = NULL;
            if (done != NULL) done(wifi, rx_err);
        }
    }
}

vsf_err_t __mt76_mcu_msg_send(vsf_wifi_t *wifi, uint8_t cmd,
    const uint8_t *payload, uint16_t payload_len, bool wait_resp,
    vsf_wifi_done_t done)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    uint16_t padded = (payload_len + 3) & ~3;
    /* USB DMA header + padded payload + trailing zero.
     * The MCU command type/seq/cmd fields live in the dma_info flags,
     * matching the Linux mt76x02u_skb_dma_info layout.
     * The length field in TXINFO is the rounded payload length (xfer len),
     * not including the 4-byte TXINFO header. */
    uint16_t xfer_len = padded;
    uint16_t total    = 4 + xfer_len + 4;

    if (total > sizeof(priv->tx_buf)) {
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }

    /* MCU command sequence number is 4-bit (firmware echoes it in the
     * CMD_RESP endpoint).  Cycle 1..15 and never use 0, matching Linux
     * mt76x02u behaviour. */
    uint8_t seq = priv->mcu_seq + 1;
    if (seq > 15) {
        seq = 1;
    }
    priv->mcu_seq = seq;

    if (wait_resp) {
        priv->mcu_wait_resp = true;
        priv->mcu_wait_seq  = seq;
        priv->mcu_wait_done = done;
        done = __mt76_mcu_msg_out_done;
    }

    uint32_t dma_info = ((uint32_t)xfer_len & MT76_TXD_INFO_LEN_MASK)
                      | (((uint32_t)MT76_CPU_TX_PORT & 0x07) << MT76_TXD_INFO_DPORT_SHIFT)
                      | MT76_MCU_MSG_TYPE_CMD
                      | (((uint32_t)cmd & 0x7F) << MT76_MCU_MSG_CMD_TYPE_SHIFT)
                      | (((uint32_t)seq & 0x0F) << MT76_MCU_MSG_CMD_SEQ_SHIFT);

    uint8_t *buf = priv->tx_buf;
    __mt76_put_le32(buf + 0, dma_info);
    if (payload_len > 0) {
        memcpy(buf + 4, payload, payload_len);
    }
    memset(buf + 4 + payload_len, 0, total - 4 - payload_len);

    return __mt76_mcu_cmd(wifi, priv->tx_buf, total, done);
}
#endif      /* VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_MT76 == ENABLED */
