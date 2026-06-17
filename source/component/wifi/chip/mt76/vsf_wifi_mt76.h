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

/*============================ TYPES =========================================*/

/* MT76 bus operations.
 *
 * These are chip-level communication primitives, not USB-specific calls.  On
 * USB the bus driver maps them to ep0 vendor requests and bulk URBs; on other
 * buses (e.g. SDIO) the same chip driver would use a different bus_ops
 * implementation that maps to the appropriate host/controller primitives. */
typedef struct vsf_wifi_mt76_bus_ops_t {
    /* Called by the chip driver once initialization is complete and the data
     * path can be enabled.  The bus driver starts RX URBs / DMA here. */
    void (*on_ready)(vsf_wifi_t *wifi);

    /* Read a 32-bit configuration/register word.  On USB this becomes an
     * ep0 vendor IN request; the exact layout is bus-driver private. */
    vsf_err_t (*cfg_read)(vsf_wifi_t *wifi,
        uint32_t addr, uint32_t *out, vsf_wifi_done_t done);

    /* Write a 32-bit configuration/register word.  On USB this becomes an
     * ep0 vendor OUT request; the exact layout is bus-driver private. */
    vsf_err_t (*cfg_write)(vsf_wifi_t *wifi,
        uint32_t addr, uint32_t val, vsf_wifi_done_t done);

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

    uint32_t                asic_rev;
    uint8_t                 mac_addr[6];
    uint8_t                 eeprom[MT76_EEPROM_SIZE];

    uint8_t                 mcu_seq;
    uint8_t                 state;

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
#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
    vsf_callback_timer_t    fw_timer;
#endif

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
