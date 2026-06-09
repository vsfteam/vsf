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

/*
 * RT28xx / RT5572 wifi chip driver — bus-agnostic.
 *
 * All chip knowledge lives here.  The wifi layer only knows how to ship
 * (reg, val) pairs through a bus_ops vtable; this file expresses every
 * chip operation as such an op-array and hands it to vsf_wifi_run_script.
 *
 * Static sequences (the chip init table) live in .rodata.  Parameterised
 * sequences (set_channel, set_mac_addr, connect, ...) are built into
 * wifi->scratch_ops at call time via vsf_wifi_get_scratch_ops().
 *
 * Bus independence: this file does NOT include any USB / SDIO / SPI header.
 * The same compiled object can plug into a USB shim, an SDIO shim, etc.,
 * provided the bus driver supplies a reg_write / reg_read implementation.
 *
 * --- Status of the hardware bring-up ----------------------------------------
 *
 * The init script below covers MAC defaults + BBP + RF tables and is enough
 * for register-write smoke tests, but a real RT28xx dongle still needs:
 *
 *   1. Firmware upload of rt2870.bin (8 KiB) — this driver wires it through
 *      vsf_wifi_run_blob; the application links a strong override of
 *      __rt2870_firmware_data[] / __rt2870_firmware_size to ship the blob.
 *   2. EEPROM read for the per-device MAC address, RF chip type, calibration
 *      constants and TX power tables.
 *   3. BBP wake-up sequence (poll BBP_R0 != 0xFF) before BBP defaults.
 *
 * Until those land, the driver only exercises the bus-level register path;
 * the radio will not actually be on the air.  parse_rx is left NULL too:
 * beacon extraction depends on the exact RXWI layout which differs between
 * RT28xx sub-families and can only be locked down with hardware in hand.
 *==========================================================================*/

/*============================ INCLUDES ======================================*/

#include "../../vsf_wifi.h"

#if VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_RT28XX == ENABLED

#include "../../vsf_wifi_priv.h"

/*============================ MAC REGISTERS =================================*/

#define RT28XX_PBF_SYS_CTRL             0x0400
#define RT28XX_USB_DMA_CFG              0x02A0
#define RT28XX_WPDMA_GLO_CFG            0x0208
#define RT28XX_FW_FIRMWARE_BASE         0x3000   /* base for fw upload  */
#define RT28XX_H2M_MAILBOX_CSR          0x7010
#define RT28XX_H2M_MAILBOX_CID          0x7008
#define RT28XX_H2M_MAILBOX_STATUS       0x700C
#define RT28XX_H2M_BBP_AGENT            0x7028

#define RT28XX_ASIC_VER_ID              0x1000
#define RT28XX_MAC_SYS_CTRL             0x1004
#define RT28XX_MAC_ADDR_DW0             0x1008
#define RT28XX_MAC_ADDR_DW1             0x100C
#define RT28XX_MAC_BSSID_DW0            0x1010
#define RT28XX_MAC_BSSID_DW1            0x1014
#define RT28XX_MAC_MAX_LEN_CFG          0x1018
#define RT28XX_BBP_CSR_CFG              0x101C
#define RT28XX_RF_CSR_CFG               0x1020
#define RT28XX_LED_CFG                  0x1024
#define RT28XX_AMPDU_MAX_LEN_20M1S      0x1030
#define RT28XX_AMPDU_MAX_LEN_40M1S      0x1034

#define RT28XX_TX_PIN_CFG               0x1328
#define RT28XX_TX_BAND_CFG              0x132C
#define RT28XX_TX_SW_CFG1               0x1330
#define RT28XX_TX_SW_CFG2               0x1334

#define RT28XX_RX_FILTER_CFG            0x1400
#define RT28XX_RX_PARSER_CFG            0x1404
#define RT28XX_BCN_TIME_CFG             0x1800
#define RT28XX_TBTT_TIMER               0x1804
#define RT28XX_INT_TIMER_CFG            0x1808
#define RT28XX_INT_TIMER_EN             0x180C
#define RT28XX_US_CYC_CNT               0x1854

// MAC_SYS_CTRL bits
#define RT28XX_MAC_SRST                 (1 << 0)
#define RT28XX_BBP_HRST                 (1 << 2)
#define RT28XX_MAC_TX_EN                (1 << 2)
#define RT28XX_MAC_RX_EN                (1 << 3)

// BBP / RF indirect access encoding
#define RT28XX_BBP_BUSY                 (1 << 16)
#define RT28XX_BBP_WRITE                (1 << 17)
#define RT28XX_RF_BUSY                  (1 << 16)
#define RT28XX_RF_WRITE                 (1 << 17)

// RX_FILTER_CFG bits
#define RT28XX_FILTER_DROP_NOT_MYBSS    (1 <<  0)
#define RT28XX_FILTER_DROP_MULTICAST    (1 <<  1)
#define RT28XX_FILTER_DROP_BROADCAST    (1 <<  2)
#define RT28XX_FILTER_DROP_UNICAST      (1 <<  3)
#define RT28XX_FILTER_DROP_CRC_ERROR    (1 <<  4)
#define RT28XX_FILTER_DROP_PHY_ERROR    (1 <<  5)
#define RT28XX_FILTER_DROP_CTRL_FRAME   (1 <<  6)
#define RT28XX_FILTER_DROP_NOT_TO_ME    (1 <<  7)
#define RT28XX_FILTER_DROP_DUPLICATE    (1 << 11)
#define RT28XX_FILTER_DROP_CFEND        (1 << 12)
#define RT28XX_FILTER_DROP_CFACK        (1 << 13)
#define RT28XX_FILTER_DROP_CTS          (1 << 14)
#define RT28XX_FILTER_DROP_RTS          (1 << 15)

// RF register numbers
#define RT28XX_RF1_TUNING               1
#define RT28XX_RF2_BAND                 2
#define RT28XX_RF3_TSSI                 3
#define RT28XX_RF4_FREQ                 4

/*============================ OP MACROS =====================================*
 *
 *   RT_OP_REG  — direct 32-bit MAC register write
 *   RT_OP_BBP  — indirect BBP register write   (via BBP_CSR_CFG)
 *   RT_OP_RF   — indirect RF register write    (via RF_CSR_CFG)
 *==========================================================================*/

#define RT_OP_REG(r_, v_)   { (uint16_t)(r_), (uint32_t)(v_) }

#define RT_OP_BBP(r_, v_)   RT_OP_REG(RT28XX_BBP_CSR_CFG,                      \
        RT28XX_BBP_WRITE | (((uint32_t)(r_) & 0xFF) << 8)                      \
                         |  ((uint32_t)(v_) & 0xFF))

#define RT_OP_RF(r_, v_)    RT_OP_REG(RT28XX_RF_CSR_CFG,                       \
        RT28XX_RF_WRITE  | (((uint32_t)(r_) & 0xFF) << 8)                      \
                         |  ((uint32_t)(v_) & 0xFF))

/*============================ INIT SCRIPT ===================================*/

static const vsf_wifi_op_t __rt28xx_init_script[] = {
    /* ---- MAC reset pulse ---- */
    RT_OP_REG(RT28XX_MAC_SYS_CTRL,        RT28XX_MAC_SRST | RT28XX_BBP_HRST),
    RT_OP_REG(RT28XX_MAC_SYS_CTRL,        0),
    /* ---- USB / DMA bring-up ----
     * USB_DMA_CFG: TX bulk-aggregation enabled, RX bulk timeout = 0x80.
     * WPDMA_GLO_CFG: enable TX/RX DMA, 64B burst.
     * (Names retain the USB_DMA_CFG label even though the chip can sit on
     *  PCIe / SPI in other product families — these are the same MAC regs.)
     */
    RT_OP_REG(RT28XX_USB_DMA_CFG,         0x80050C00),
    RT_OP_REG(RT28XX_WPDMA_GLO_CFG,       0x00000054),
    /* ---- MAC defaults ---- */
    RT_OP_REG(RT28XX_MAC_MAX_LEN_CFG,     0x0FFF0FFF),
    RT_OP_REG(RT28XX_LED_CFG,             0x7F031E46),
    RT_OP_REG(RT28XX_AMPDU_MAX_LEN_20M1S, 0x0000A8FF),
    RT_OP_REG(RT28XX_AMPDU_MAX_LEN_40M1S, 0x000108EB),
    RT_OP_REG(RT28XX_TX_PIN_CFG,          0x00000D0F),
    RT_OP_REG(RT28XX_TX_BAND_CFG,         0x0000259F),
    RT_OP_REG(RT28XX_TX_SW_CFG1,          0x0000259F),
    RT_OP_REG(RT28XX_TX_SW_CFG2,          0x0000259F),
    RT_OP_REG(RT28XX_RX_PARSER_CFG,       0x00000000),
    RT_OP_REG(RT28XX_BCN_TIME_CFG,        0x00006400),
    RT_OP_REG(RT28XX_TBTT_TIMER,          0x00000020),
    RT_OP_REG(RT28XX_INT_TIMER_CFG,       0x00000000),
    RT_OP_REG(RT28XX_INT_TIMER_EN,        0x00000000),
    RT_OP_REG(RT28XX_US_CYC_CNT,          0x0000001E),
    /* ---- BBP defaults ---- */
    RT_OP_BBP( 65, 0x2C), RT_OP_BBP( 66, 0x38),
    RT_OP_BBP( 69, 0x12), RT_OP_BBP( 70, 0x0A),
    RT_OP_BBP( 73, 0x10), RT_OP_BBP( 81, 0x37),
    RT_OP_BBP( 82, 0x62), RT_OP_BBP( 83, 0x6A),
    RT_OP_BBP( 84, 0x99), RT_OP_BBP( 86, 0x00),
    RT_OP_BBP( 91, 0x04), RT_OP_BBP( 92, 0x00),
    RT_OP_BBP(103, 0x00), RT_OP_BBP(105, 0x05),
    RT_OP_BBP(106, 0x35), RT_OP_BBP(128, 0x12),
    /* ---- RF defaults (RT30xx family) ---- */
    RT_OP_RF( 0, 0x50), RT_OP_RF( 1, 0x3F), RT_OP_RF( 2, 0x62), RT_OP_RF( 3, 0x22),
    RT_OP_RF( 4, 0x40), RT_OP_RF( 5, 0x41), RT_OP_RF( 6, 0x44), RT_OP_RF( 7, 0x4D),
    RT_OP_RF( 8, 0xC0), RT_OP_RF( 9, 0x2A), RT_OP_RF(10, 0x4F), RT_OP_RF(11, 0x23),
    RT_OP_RF(12, 0x53), RT_OP_RF(13, 0x23), RT_OP_RF(14, 0xC0), RT_OP_RF(15, 0x00),
    RT_OP_RF(16, 0x55), RT_OP_RF(17, 0x2E), RT_OP_RF(18, 0xD0), RT_OP_RF(19, 0x02),
    RT_OP_RF(20, 0x00), RT_OP_RF(21, 0x00), RT_OP_RF(22, 0x40), RT_OP_RF(23, 0x50),
    RT_OP_RF(24, 0xC0), RT_OP_RF(25, 0x62), RT_OP_RF(26, 0x5F), RT_OP_RF(27, 0x08),
    RT_OP_RF(28, 0x28), RT_OP_RF(29, 0x10), RT_OP_RF(30, 0x28), RT_OP_RF(31, 0x20),
    /* ---- Final TX/RX enable ---- */
    RT_OP_REG(RT28XX_MAC_SYS_CTRL,        RT28XX_MAC_TX_EN | RT28XX_MAC_RX_EN),
};

/* Post-firmware handshake.  Issued only when __rt2870_firmware_size > 0,
 * after the blob upload completes.  Sequence mirrors the rt2x00 driver
 * tail (without the MCU_READY poll — the readback path needs hardware to
 * lock down).  */
static const vsf_wifi_op_t __rt28xx_post_fw_script[] = {
    RT_OP_REG(RT28XX_H2M_BBP_AGENT,    0x00000000),
    RT_OP_REG(RT28XX_H2M_MAILBOX_CSR,  0x00000000),
    RT_OP_REG(RT28XX_H2M_MAILBOX_CID,  0xFFFFFFFF),
    RT_OP_REG(RT28XX_H2M_MAILBOX_STATUS,0xFFFFFFFF),
    /* PBF_SYS_CTRL bit 7 (HOST_RAM_WRITE = 0): tells the MCU to leave
     * patch-RAM mode and start executing firmware. */
    RT_OP_REG(RT28XX_PBF_SYS_CTRL,     0x00000000),
};

/*============================ CHANNEL TABLE =================================*/

typedef struct {
    uint32_t tx_pin_cfg;
    uint32_t tx_band_cfg;
    uint32_t tx_sw_cfg1;
    uint32_t tx_sw_cfg2;
    uint8_t  rf_r1;
    uint8_t  rf_r2;
    uint8_t  rf_r3;
    uint8_t  rf_r4;
} rt28xx_channel_entry_t;

static const rt28xx_channel_entry_t __rt28xx_channel_table[] = {
    //   PIN_CFG     BAND_CFG    SW_CFG1     SW_CFG2    R1  R2  R3  R4
    [0]  = {0},
    [1]  = {0x00000D0F, 0x0000259F, 0x0000259F, 0x0000259F, 0x24,0x02,0x20,0x00},
    [2]  = {0x00000D0F, 0x0000259F, 0x0000259F, 0x0000259F, 0x25,0x02,0x20,0x00},
    [3]  = {0x00000D0F, 0x000025A0, 0x000025A0, 0x000025A0, 0x26,0x02,0x20,0x00},
    [4]  = {0x00000D0F, 0x000025A0, 0x000025A0, 0x000025A0, 0x27,0x02,0x20,0x00},
    [5]  = {0x00000D0F, 0x000025A1, 0x000025A1, 0x000025A1, 0x28,0x02,0x20,0x00},
    [6]  = {0x00000D0F, 0x000025A1, 0x000025A1, 0x000025A1, 0x29,0x02,0x20,0x00},
    [7]  = {0x00000D0F, 0x000025A2, 0x000025A2, 0x000025A2, 0x2A,0x02,0x20,0x00},
    [8]  = {0x00000D0F, 0x000025A2, 0x000025A2, 0x000025A2, 0x2B,0x02,0x20,0x00},
    [9]  = {0x00000E0F, 0x000025A3, 0x000025A3, 0x000025A3, 0x2C,0x02,0x22,0x00},
    [10] = {0x00000E0F, 0x000025A3, 0x000025A3, 0x000025A3, 0x2D,0x02,0x22,0x00},
    [11] = {0x00000E0F, 0x000025A4, 0x000025A4, 0x000025A4, 0x2E,0x02,0x22,0x00},
    [12] = {0x00000E0F, 0x000025A4, 0x000025A4, 0x000025A4, 0x2F,0x02,0x22,0x00},
    [13] = {0x00000E10, 0x000025A5, 0x000025A5, 0x000025A5, 0x30,0x02,0x24,0x00},
    [14] = {0x00000E10, 0x000025A5, 0x000025A5, 0x000025A5, 0x31,0x02,0x24,0x00},
};

/*============================ HELPERS =======================================*/

static int __rt28xx_emit_channel(vsf_wifi_op_t *ops, int n, uint8_t channel)
{
    const rt28xx_channel_entry_t *ch = &__rt28xx_channel_table[channel];
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_TX_PIN_CFG,  ch->tx_pin_cfg);
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_TX_BAND_CFG, ch->tx_band_cfg);
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_TX_SW_CFG1,  ch->tx_sw_cfg1);
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_TX_SW_CFG2,  ch->tx_sw_cfg2);
    ops[n++] = (vsf_wifi_op_t)RT_OP_RF (RT28XX_RF1_TUNING,  ch->rf_r1);
    ops[n++] = (vsf_wifi_op_t)RT_OP_RF (RT28XX_RF2_BAND,    ch->rf_r2);
    ops[n++] = (vsf_wifi_op_t)RT_OP_RF (RT28XX_RF3_TSSI,    ch->rf_r3);
    ops[n++] = (vsf_wifi_op_t)RT_OP_RF (RT28XX_RF4_FREQ,    ch->rf_r4);
    return n;
}

static int __rt28xx_emit_bssid(vsf_wifi_op_t *ops, int n, const uint8_t bssid[6])
{
    uint32_t dw0 = (uint32_t)bssid[0]
                 | ((uint32_t)bssid[1] <<  8)
                 | ((uint32_t)bssid[2] << 16)
                 | ((uint32_t)bssid[3] << 24);
    uint32_t dw1 = (uint32_t)bssid[4] | ((uint32_t)bssid[5] << 8);
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_MAC_BSSID_DW0, dw0);
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_MAC_BSSID_DW1, dw1);
    return n;
}

/*============================ CHIP OPS ======================================*/

/*
 * firmware_load = run_blob(rt2870.bin) -> run_script(post_fw_script).
 *
 * The blob upload and the post-fw register handshake share the dispatcher,
 * so they must be issued sequentially.  The outer `done` callback is
 * stashed in wifi->backend_chain_done while the inner stage is in flight.
 */
static void __rt28xx_post_fw_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    vsf_wifi_done_t outer = wifi->backend_chain_done;
    wifi->backend_chain_done = NULL;
    if (outer != NULL) outer(wifi, err);
}

static void __rt28xx_blob_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) {
        __rt28xx_post_fw_done(wifi, err);
        return;
    }
    err = vsf_wifi_run_script(wifi, __rt28xx_post_fw_script,
            (uint16_t)dimof(__rt28xx_post_fw_script),
            __rt28xx_post_fw_done);
    if (VSF_ERR_NONE != err) {
        __rt28xx_post_fw_done(wifi, err);
    }
}

static vsf_err_t __rt28xx_firmware_load(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    if (__rt2870_firmware_size == 0) {
        if (done != NULL) done(wifi, VSF_ERR_NONE);
        return VSF_ERR_NONE;
    }
    /* Real RT2870 / RT3070 firmware is exactly 8 KiB.  Reject obviously
     * malformed overrides up-front to surface integration mistakes loudly
     * rather than wedging on a half-loaded MCU. */
    if (__rt2870_firmware_size != 8192) {
        if (done != NULL) done(wifi, VSF_ERR_INVALID_PARAMETER);
        return VSF_ERR_INVALID_PARAMETER;
    }

    wifi->backend_chain_done = done;

    vsf_wifi_blob_t blob = {
        .data       = __rt2870_firmware_data,
        .len        = __rt2870_firmware_size,
        .base_reg   = RT28XX_FW_FIRMWARE_BASE,  /* 0x3000 */
        .chunk_size = 0,                        /* hint; bus picks default */
    };
    vsf_err_t err = vsf_wifi_run_blob(wifi, &blob, __rt28xx_blob_done);
    if (VSF_ERR_NONE != err) {
        wifi->backend_chain_done = NULL;
    }
    return err;
}

static vsf_err_t __rt28xx_init(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    return vsf_wifi_run_script(wifi, __rt28xx_init_script,
            (uint16_t)dimof(__rt28xx_init_script), done);
}

static void __rt28xx_fini(vsf_wifi_t *wifi)
{
    (void)wifi;
}

static vsf_err_t __rt28xx_set_channel(vsf_wifi_t *wifi, uint8_t channel,
        vsf_wifi_done_t done)
{
    if (channel < 1 || channel > 14) return VSF_ERR_INVALID_PARAMETER;
    vsf_wifi_op_t *ops = vsf_wifi_get_scratch_ops(wifi);
    int n = __rt28xx_emit_channel(ops, 0, channel);
    return vsf_wifi_run_script(wifi, ops, (uint16_t)n, done);
}

static vsf_err_t __rt28xx_set_rx_filter(vsf_wifi_t *wifi, uint32_t mask,
        vsf_wifi_done_t done)
{
    vsf_wifi_op_t *ops = vsf_wifi_get_scratch_ops(wifi);
    ops[0] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_RX_FILTER_CFG, mask);
    return vsf_wifi_run_script(wifi, ops, 1, done);
}

static vsf_err_t __rt28xx_set_mac_addr(vsf_wifi_t *wifi, const uint8_t mac[6],
        vsf_wifi_done_t done)
{
    uint32_t dw0 = (uint32_t)mac[0]
                 | ((uint32_t)mac[1] <<  8)
                 | ((uint32_t)mac[2] << 16)
                 | ((uint32_t)mac[3] << 24);
    uint32_t dw1 = (uint32_t)mac[4] | ((uint32_t)mac[5] << 8);
    vsf_wifi_op_t *ops = vsf_wifi_get_scratch_ops(wifi);
    ops[0] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_MAC_ADDR_DW0, dw0);
    ops[1] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_MAC_ADDR_DW1, dw1);
    return vsf_wifi_run_script(wifi, ops, 2, done);
}

static vsf_err_t __rt28xx_set_bssid(vsf_wifi_t *wifi, const uint8_t bssid[6],
        vsf_wifi_done_t done)
{
    vsf_wifi_op_t *ops = vsf_wifi_get_scratch_ops(wifi);
    int n = __rt28xx_emit_bssid(ops, 0, bssid);
    return vsf_wifi_run_script(wifi, ops, (uint16_t)n, done);
}

static vsf_err_t __rt28xx_set_auth_mode(vsf_wifi_t *wifi,
        const vsf_wifi_auth_cfg_t *cfg, vsf_wifi_done_t done)
{
    /* Raw-protocol backend: no MLME / WPA handshake. */
    (void)cfg;
    if (done != NULL) done(wifi, VSF_ERR_NONE);
    return VSF_ERR_NONE;
}

static vsf_err_t __rt28xx_connect(vsf_wifi_t *wifi,
        const uint8_t bssid[6], const uint8_t *ssid, uint8_t ssid_len,
        uint8_t channel, vsf_wifi_done_t done)
{
    (void)ssid; (void)ssid_len;
    if (channel < 1 || channel > 14) return VSF_ERR_INVALID_PARAMETER;

    vsf_wifi_op_t *ops = vsf_wifi_get_scratch_ops(wifi);
    int n = 0;
    n = __rt28xx_emit_bssid  (ops, n, bssid);                       /* 2 ops */
    n = __rt28xx_emit_channel(ops, n, channel);                     /* 8 ops */
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_RX_FILTER_CFG,       /* 1 op  */
            RT28XX_FILTER_DROP_CRC_ERROR | RT28XX_FILTER_DROP_PHY_ERROR);
    /* total 11 ops, fits in default scratch (16). */
    wifi->channel = channel;
    return vsf_wifi_run_script(wifi, ops, (uint16_t)n, done);
}

static vsf_err_t __rt28xx_disconnect(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    static const uint8_t zero_bssid[6] = {0};
    vsf_wifi_op_t *ops = vsf_wifi_get_scratch_ops(wifi);
    int n = 0;
    n = __rt28xx_emit_bssid(ops, n, zero_bssid);                    /* 2 ops */
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_RX_FILTER_CFG, 0);   /* 1 op  */
    return vsf_wifi_run_script(wifi, ops, (uint16_t)n, done);
}

static vsf_err_t __rt28xx_get_link_info(vsf_wifi_t *wifi,
        vsf_wifi_link_info_t *info)
{
    memset(info, 0, sizeof(*info));
    info->channel = wifi->channel;
    return VSF_ERR_NONE;
}

/*============================ VTABLE INSTANCE ===============================*/

const vsf_wifi_chip_drv_t vsf_wifi_rt28xx_drv = {
    .name          = "rt28xx",
    .firmware_load = __rt28xx_firmware_load,
    .init          = __rt28xx_init,
    .fini          = __rt28xx_fini,
    .set_channel   = __rt28xx_set_channel,
    .set_rx_filter = __rt28xx_set_rx_filter,
    .set_mac_addr  = __rt28xx_set_mac_addr,
    .set_bssid     = __rt28xx_set_bssid,
    .set_auth_mode = __rt28xx_set_auth_mode,
    .connect       = __rt28xx_connect,
    .disconnect    = __rt28xx_disconnect,
    .get_link_info = __rt28xx_get_link_info,
    /* .parse_rx      = NULL,   // RXWI layout TBD on real hardware */
};

#endif      // VSF_USE_WIFI && VSF_WIFI_USE_RT28XX
