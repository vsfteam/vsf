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

#ifndef __VSF_WIFI_MT76_H__
#define __VSF_WIFI_MT76_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_wifi_cfg.h"

#if VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_MT76 == ENABLED

#include "../../vsf_wifi.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define MT76_RX_URB_SIZE            2048
#define MT76_TX_URB_SIZE            4096
#define MT76_MCU_RESP_SIZE          2048

#define MT76_EEPROM_SIZE            512

/* USB endpoint indices shared between the chip driver and the USB bus driver. */
#define MT76_EP_IN_CMD_RESP         1

/* EFUSE control registers */
#define MT_EFUSE_CTRL               0x0024
#define MT_EFUSE_CTRL_AOUT          0x0000003F
#define MT_EFUSE_CTRL_MODE          0x000000C0
#define MT_EFUSE_CTRL_AIN           0x03FF0000
#define MT_EFUSE_CTRL_KICK          0x40000000
#define MT_EFUSE_CTRL_SEL           0x80000000

#define MT_EFUSE_DATA_BASE          0x0028
#define MT_EFUSE_DATA(_n)           (MT_EFUSE_DATA_BASE + ((_n) << 2))

/* MAC address / BSSID registers */
#define MT_MAC_ADDR_DW0             0x1008
#define MT_MAC_ADDR_DW1             0x100c
#define MT_MAC_ADDR_DW1_U2ME_MASK   0x00FF0000

#define MT_MAC_BSSID_DW0            0x1010
#define MT_MAC_BSSID_DW1            0x1014
#define MT_MAC_BSSID_DW1_ADDR       0x0000FFFF
#define MT_MAC_BSSID_DW1_MBSS_MODE  0x00030000
#define MT_MAC_BSSID_DW1_MBSS_LOCAL_BIT 0x00040000
#define MT_MAC_BSSID_DW1_MBEACON_N  0x001C0000
#define MT_MAC_BSSID_DW1_MBEACON_N_SHIFT 18

/* APC BSSID registers: one 64-bit entry per BSS (low 32 + high 16). */
#define MT_MAC_APC_BSSID_BASE       0x1090
#define MT_MAC_APC_BSSID_L(_n)      (MT_MAC_APC_BSSID_BASE + ((_n) * 8))
#define MT_MAC_APC_BSSID_H(_n)      (MT_MAC_APC_BSSID_BASE + ((_n) * 8 + 4))
#define MT_MAC_APC_BSSID_H_ADDR     0x0000FFFF

/*============================ TYPES =========================================*/

/* MT76 bus operations.
 *
 * MT76 is a register-based chip (extensive cfg_read/cfg_write) plus chip-
 * specific primitives for FCE writes, device commands, MCU command frames,
 * TX frame submission and RX URB arming.  The first member MUST be
 * vsf_wifi_reg_bus_t so the generic wifi layer can treat the structure as a
 * standard register bus; chip-private operations follow the shared part.
 *
 * On USB the bus driver maps reg_read/reg_write to ep0 vendor requests and
 * the private ops to bulk URBs / ep0 control transfers; on other buses (e.g.
 * SDIO) the same chip driver would use a different bus_ops implementation
 * that maps to the appropriate host/controller primitives. */
typedef struct vsf_wifi_mt76_bus_ops_t {
    /* Generic register-bus interface.  reg_read/reg_write are used for the
     * many 32-bit configuration/register accesses that MT76 performs. */
    vsf_wifi_reg_bus_t base;

    /* Write an FCE (Forward Control Engine) DMA register.  Used during
     * firmware upload to set the destination address/length before sending
     * the firmware payload as an MCU command frame. */
    vsf_err_t (*fce_write)(vsf_wifi_t *wifi,
        uint32_t addr, uint32_t val, vsf_wifi_done_t done);

    /* Issue a device-mode control command (reset, IVB load).
     * On USB this is an ep0 vendor request. */
    vsf_err_t (*dev_cmd)(vsf_wifi_t *wifi,
        uint8_t req, uint16_t value, uint16_t index, vsf_wifi_done_t done);

    /* Issue a device-mode class request (patch enable / WMT reset).
     * On USB this is an ep0 class request with a small data stage. */
    vsf_err_t (*dev_class_cmd)(vsf_wifi_t *wifi,
        uint8_t req, uint16_t value, uint16_t index,
        const uint8_t *data, uint16_t len, vsf_wifi_done_t done);

    /* Send an MCU command frame.  On USB this is a bulk OUT transfer on the
     * INBAND_CMD endpoint.  The buffer already contains the TXINFO + CMD
     * header + payload layout required by the chip.  Completion is signalled
     * when the bulk OUT URB finishes (the caller separately waits for the
     * CMD_RESP via rx_submit). */
    vsf_err_t (*mcu_cmd)(vsf_wifi_t *wifi,
        const uint8_t *data, uint16_t len, vsf_wifi_done_t done);

    /* Submit a TX 802.11 frame.  On USB this is a bulk OUT transfer on one of
     * the AC endpoints.  The buffer already contains TXINFO + TXWI + frame. */
    vsf_err_t (*tx_frame)(vsf_wifi_t *wifi,
        const uint8_t *data, uint16_t len, uint8_t queue_idx, vsf_wifi_done_t done);

    /* Start / re-arm a single RX reception.  On USB this is a bulk IN URB. */
    vsf_err_t (*rx_submit)(vsf_wifi_t *wifi,
        uint8_t *buf, uint16_t len, uint8_t queue_idx);
} vsf_wifi_mt76_bus_ops_t;

typedef struct mt76_wifi_priv {
    vsf_wifi_t              *wifi;
    void                    *bus_priv;

    /* RX data/event delivery callback registered by the chip driver.  The bus
     * driver invokes this for every completed bulk IN URB. */
    void                    (*on_rx)(vsf_wifi_t *wifi, uint8_t *buf, uint16_t len);

    /* RX packet callback for beacon/probe response frames received on the
     * packet RX endpoint during scanning. */
    void                    (*on_rx_pkt)(vsf_wifi_t *wifi, uint8_t *buf, uint16_t len);

    uint32_t                asic_rev;
    uint8_t                 mac_addr[6];
    uint8_t                 eeprom[MT76_EEPROM_SIZE];

    /* Hardware capability: chainmask from EEPROM (rx = low nibble, tx = high nibble).
     * MT7612U is 2T2R -> 0x0202. */
    uint16_t                chainmask;
    /* Cached RX filter value; updated after MAC reset and during scan. */
    uint32_t                rxfilter;

    /* RX path gain state, read from EEPROM and programmed via CMD_INIT_GAIN_OP
     * after every channel switch (mirrors Linux mt76x2_read_rx_gain). */
    uint32_t                rx_mcu_gain;
    int8_t                  rx_lna_gain;
    int8_t                  rx_high_gain[2];
    int8_t                  rx_rssi_offset[2];
    bool                    rx_gain_read;

    /* One-time RF calibration flags, matching Linux mt76x2u init. */
    bool                    init_cal_done;

    uint8_t                 mcu_seq;
    uint8_t                 state;

    /* MCU command response matching.
     * Some firmware commands (LOAD_CR, SWITCH_CHANNEL_OP) require the host
     * to drain the CMD_RESP endpoint and wait for EVT_CMD_DONE before the
     * next command can be issued. */
    bool                    mcu_wait_resp;
    uint8_t                 mcu_wait_seq;
    vsf_wifi_done_t         mcu_wait_done;

    /* EEPROM loading context */
    uint16_t                eeprom_offset;
    uint8_t                 eeprom_step;
    uint32_t                eeprom_ctrl;
    uint32_t                eeprom_data[4];
    vsf_wifi_done_t         eeprom_done;

    /* async operation context */
    vsf_wifi_done_t         pending_done;
    uint16_t                pending_cmd;

    /* firmware upload context */
    uint8_t                 fw_state;
    uint8_t                 fw_stage;
    uint8_t                 fw_send_state;
    uint8_t                 fw_send_next_state;
    uint32_t                fw_idx;
    uint32_t                fw_ilm_len;
    uint32_t                fw_dlm_len;
    const uint8_t           *fw_data;
    uint32_t                fw_len;
    uint32_t                fw_pos;
    uint32_t                fw_dst_offset;
    uint32_t                fw_max_payload;
    uint32_t                fw_patch_reg;
    uint32_t                fw_patch_mask;
    uint32_t                fw_poll_mask;
    uint16_t                fw_poll_ms;
    uint16_t                fw_sem_ms;
    uint8_t                 fw_rf_unit;
    uint8_t                 fw_rf_step;

    /* MAC address programming context */
    uint8_t                 mac_addr_step;
    vsf_wifi_done_t         mac_addr_done;

    /* Hardware init context (mt76x2u_init_hardware steps) */
    uint16_t                init_idx;
    uint16_t                init_idx2;
    uint8_t                 init_substate;
    uint8_t                 xtal_step;
    uint16_t                xtal_val;

    /* set_channel context */
    uint8_t                 set_channel_channel;
    uint8_t                 set_channel_scan;
    uint8_t                 set_channel_bw;        /* WIFI_BW_xxx hint      */
    uint8_t                 set_channel_bw_index;
    uint8_t                 set_channel_ch_group;
    bool                    set_channel_is_5g;
    uint8_t                 set_channel_state;    /* top-level state */
    uint8_t                 set_channel_substate; /* mac_stop sub-state */
    uint8_t                 set_channel_bbp_substate; /* BBP bw sub-state  */
    uint8_t                 set_channel_post_substate; /* post-switch cal/regs */
    uint32_t                set_channel_saved_rts;
    vsf_wifi_done_t         set_channel_done;     /* original caller callback */
    uint8_t                 last_channel;         /* last tuned channel */

    /* connect context */
    vsf_wifi_done_t         connect_done;

    /* disconnect context */
    vsf_wifi_done_t         disconnect_done;

    /* hardware crypto install context */
    vsf_wifi_done_t         crypto_done;
    /* Multicast/broadcast software-CCMP PN.  MT76 firmware overrides the QoS
     * Control Ack Policy of WIV=0 broadcast frames, so multicast data is
     * software-encrypted locally using this counter while unicast data uses
     * the hardware WCID PN.  Little-endian, mcast_pn[0] = LSB. */
    uint8_t                 mcast_pn[6];
#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
    vsf_callback_timer_t    fw_timer;
    /* one-shot TX status poll for debugging handshake ACK failures */
    vsf_callback_timer_t    txstat_timer;
    uint32_t                txstat_val;
    uint32_t                txstat_ext_val;
#endif

    /* Rate control state: index into __mt76_rate_table, consecutive ACK
     * counter, and the rate value last programmed into WCID 1 TX info. */
    uint8_t                 tx_rate_idx;
    uint8_t                 tx_rate_success_cnt;
    uint16_t                tx_rate_val;

    /* Last received RSSI (from rxwi[12], i.e. rssi[0]).  -128 means "not
     * received yet"; updated for every successfully parsed MPDU. */
    int8_t                  last_rssi;

    /* buffers */
    uint8_t                 rx_buf[MT76_RX_URB_SIZE];
    uint8_t                 tx_buf[MT76_TX_URB_SIZE];
    uint8_t                 mcu_resp_buf[MT76_MCU_RESP_SIZE];
} mt76_wifi_priv_t;

/*============================ LOGGING HELPERS ===============================*/

#if VSF_WIFI_CFG_CHIP_MT76_LOG_LEVEL >= 1
#   define vsf_wifi_chip_mt76_trace_error(...)  vsf_trace_error(__VA_ARGS__)
#else
#   define vsf_wifi_chip_mt76_trace_error(...)  ((void)0)
#endif

#if VSF_WIFI_CFG_CHIP_MT76_LOG_LEVEL >= 2
#   define vsf_wifi_chip_mt76_trace_info(...)   vsf_trace_info(__VA_ARGS__)
#else
#   define vsf_wifi_chip_mt76_trace_info(...)   ((void)0)
#endif

#if VSF_WIFI_CFG_CHIP_MT76_LOG_LEVEL >= 4
#   define vsf_wifi_chip_mt76_trace_debug(...)  vsf_trace_info(__VA_ARGS__)
#else
#   define vsf_wifi_chip_mt76_trace_debug(...)  ((void)0)
#endif

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_wifi_chip_drv_t vsf_wifi_mt76_drv;

/*============================ PROTOTYPES ====================================*/

extern const uint8_t __mt76_rom_patch_firmware_data[];
extern const uint32_t __mt76_rom_patch_firmware_size;
extern const uint8_t __mt76_main_firmware_data[];
extern const uint32_t __mt76_main_firmware_size;

#ifdef __cplusplus
}
#endif

#endif      /* VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_MT76 == ENABLED */
#endif      /* __VSF_WIFI_MT76_H__ */
