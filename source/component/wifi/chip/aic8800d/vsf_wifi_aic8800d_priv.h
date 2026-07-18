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

#ifndef __VSF_WIFI_AIC8800D_PRIV_H__
#define __VSF_WIFI_AIC8800D_PRIV_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_wifi_aic8800d.h"
#include "../../vsf_wifi_priv.h"

#if VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_AIC8800D == ENABLED

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define AIC8800D_CMD_TIMEOUT_MS     5000
#define AIC8800D_CMD_BUF_SIZE       1536
#define AIC8800D_MAX_CMD_PARAM      1024
#define AIC8800D_MAX_PENDING_CMDS   4

/* Default RF config for AIC8800D80 U02 (pll trx, matches Linux rf_tbl_masked) */
#define AIC8800D_RF_CFG_COUNT       1
static const uint32_t __aic8800d_rf_cfg[AIC8800D_RF_CFG_COUNT][3] = {
    {0x40344058, 0x00800000, 0x00000000},   /* pll trx */
};

/* System config tables for AIC8800D80 (matches Linux syscfg_tbl / syscfg_tbl_masked) */
#define AIC8800D_SYSCFG_COUNT       3
static const uint32_t __aic8800d_syscfg[AIC8800D_SYSCFG_COUNT][2] = {
    {0x40500014, 0x00000101},
    {0x40500018, 0x0000010D},
    {0x40500004, 0x00000010},
};

#define AIC8800D_SYSCFG_MASKED_COUNT    1
static const uint32_t __aic8800d_syscfg_masked[AIC8800D_SYSCFG_MASKED_COUNT][3] = {
    {0x40506024, 0x000000FF, 0x000000DF},
};

/* Firmware binary is embedded as a C array in vsf_wifi_aic8800d_firmware_data.c.
 * The source binary is fmacfw_8800d80_u02.bin from the Linux reference driver.
 * Regenerate the C array when switching to a different firmware file. */
#define AIC8800D_FW_LOAD_ADDR       0x00120000
#define AIC8800D_FW_CHUNK_SIZE      1024
#define AIC8800D_FW_START_APP_TYPE  1

/* USB packet types */
#define AIC8800D_USB_TYPE_DATA          0x00
#define AIC8800D_USB_TYPE_CFG           0x10
#define AIC8800D_USB_TYPE_CFG_CMD_RSP   0x11
#define AIC8800D_USB_TYPE_CFG_DATA_CFM  0x12
#define AIC8800D_USB_TYPE_CFG_PRINT     0x13

/* LMAC tasks */
#define AIC8800D_TASK_MM        0
#define AIC8800D_TASK_DBG       1
#define AIC8800D_TASK_SCAN      2
#define AIC8800D_TASK_SCANU     4
#define AIC8800D_TASK_ME        5
#define AIC8800D_TASK_SM        6
#define AIC8800D_DRV_TASK_ID    100

#define AIC8800D_LMAC_FIRST_MSG(task)   ((uint16_t)((task) << 10))
#define AIC8800D_MSG_I(msg)             ((msg) & ((1u << 10) - 1))
#define AIC8800D_MSG_T(msg)             ((uint8_t)((msg) >> 10))

/* LMAC message IDs - must match AIC8800 reference lmac_msg.h offsets */
#define AIC8800D_MM_RESET_REQ           (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 0)
#define AIC8800D_MM_RESET_CFM           (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 1)
#define AIC8800D_MM_START_REQ           (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 2)
#define AIC8800D_MM_START_CFM           (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 3)
#define AIC8800D_MM_VERSION_REQ         (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 4)
#define AIC8800D_MM_VERSION_CFM         (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 5)
#define AIC8800D_MM_ADD_IF_REQ          (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 6)
#define AIC8800D_MM_ADD_IF_CFM          (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 7)
#define AIC8800D_MM_REMOVE_IF_REQ       (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 8)
#define AIC8800D_MM_REMOVE_IF_CFM       (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 9)
/* MM_STA_ADD/DEL skipped (10-13) */
#define AIC8800D_MM_SET_FILTER_REQ      (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 14)
#define AIC8800D_MM_SET_FILTER_CFM      (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 15)
#define AIC8800D_MM_SET_CHANNEL_REQ     (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 16)
#define AIC8800D_MM_SET_CHANNEL_CFM     (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 17)
/* ... many MM messages omitted ... */
#define AIC8800D_MM_GET_MAC_ADDR_REQ    (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 115)
#define AIC8800D_MM_GET_MAC_ADDR_CFM    (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 116)
#define AIC8800D_MM_GET_FW_VERSION_REQ    (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 128)
#define AIC8800D_MM_GET_FW_VERSION_CFM    (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 129)

#define AIC8800D_MM_SET_STACK_START_REQ (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 123)
#define AIC8800D_MM_SET_STACK_START_CFM (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 124)

#define AIC8800D_MM_SET_RF_CALIB_REQ    (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 105)
#define AIC8800D_MM_SET_RF_CALIB_CFM    (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 106)
#define AIC8800D_MM_SET_TXPWR_IDX_LVL_REQ (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 113)
#define AIC8800D_MM_SET_TXPWR_IDX_LVL_CFM (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 114)
#define AIC8800D_MM_SET_TXPWR_OFST_REQ  (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 115)
#define AIC8800D_MM_SET_TXPWR_OFST_CFM  (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 116)
#define AIC8800D_MM_SET_TXPWR_LVL_ADJ_REQ (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 131)
#define AIC8800D_MM_SET_TXPWR_LVL_ADJ_CFM (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_MM) + 132)

#define AIC8800D_ME_CONFIG_REQ          (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_ME) + 0)
#define AIC8800D_ME_CONFIG_CFM          (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_ME) + 1)
#define AIC8800D_ME_CHAN_CONFIG_REQ     (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_ME) + 2)
#define AIC8800D_ME_CHAN_CONFIG_CFM     (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_ME) + 3)
#define AIC8800D_ME_SET_CONTROL_PORT_REQ (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_ME) + 4)
#define AIC8800D_ME_SET_CONTROL_PORT_CFM (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_ME) + 5)
#define AIC8800D_ME_TRAFFIC_IND_REQ     (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_ME) + 12)
#define AIC8800D_ME_TRAFFIC_IND_CFM     (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_ME) + 13)

#define AIC8800D_SCANU_START_REQ            (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_SCANU) + 0)
#define AIC8800D_SCANU_START_CFM            (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_SCANU) + 1)
#define AIC8800D_SCANU_RESULT_IND           (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_SCANU) + 4)
#define AIC8800D_SCANU_START_CFM_ADDTIONAL  (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_SCANU) + 9)
#define AIC8800D_SCANU_CANCEL_REQ           (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_SCANU) + 10)
#define AIC8800D_SCANU_CANCEL_CFM           (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_SCANU) + 11)

#define AIC8800D_SM_CONNECT_REQ         (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_SM) + 0)
#define AIC8800D_SM_CONNECT_CFM         (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_SM) + 1)
#define AIC8800D_SM_CONNECT_IND         (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_SM) + 2)
#define AIC8800D_SM_DISCONNECT_REQ      (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_SM) + 3)
#define AIC8800D_SM_DISCONNECT_CFM      (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_SM) + 4)
#define AIC8800D_SM_DISCONNECT_IND      (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_SM) + 5)
#define AIC8800D_SM_EXTERNAL_AUTH_REQUIRED_IND (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_SM) + 6)
#define AIC8800D_SM_EXTERNAL_AUTH_REQUIRED_RSP (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_SM) + 7)
#define AIC8800D_SM_FT_AUTH_IND         (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_SM) + 8)
#define AIC8800D_SM_FT_AUTH_RSP         (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_SM) + 9)

#define AIC8800D_DBG_MEM_READ_REQ       (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_DBG) + 0)
#define AIC8800D_DBG_MEM_READ_CFM       (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_DBG) + 1)
#define AIC8800D_DBG_MEM_WRITE_REQ      (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_DBG) + 2)
#define AIC8800D_DBG_MEM_WRITE_CFM      (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_DBG) + 3)
/* DBG_SET_MOD/SEV_FILTER, DBG_ERROR_IND, DBG_GET_SYS_STAT skipped (4-10) */
#define AIC8800D_DBG_MEM_BLOCK_WRITE_REQ (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_DBG) + 11)
#define AIC8800D_DBG_MEM_BLOCK_WRITE_CFM (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_DBG) + 12)
#define AIC8800D_DBG_START_APP_REQ      (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_DBG) + 13)
#define AIC8800D_DBG_START_APP_CFM      (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_DBG) + 14)
#define AIC8800D_DBG_MEM_MASK_WRITE_REQ (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_DBG) + 17)
#define AIC8800D_DBG_MEM_MASK_WRITE_CFM (AIC8800D_LMAC_FIRST_MSG(AIC8800D_TASK_DBG) + 18)

#define AIC8800D_CO_OK                  0
#define AIC8800D_CO_FAIL                1

/*============================ TYPES =========================================*/

typedef uint8_t     aic_u8;
typedef int8_t      aic_s8;
typedef uint16_t    aic_u16;
typedef uint32_t    aic_u32;
typedef bool        aic_bool;

struct aic8800d_mac_addr {
    aic_u16 array[3];
};

struct aic8800d_mac_ssid {
    aic_u8 length;
    aic_u8 array[32];
};

struct aic8800d_mac_chan_def {
    aic_u16 freq;
    aic_u8  band;
    aic_u8  flags;
    aic_s8  tx_power;
    aic_u8  __pad;
};

struct aic8800d_lmac_msg {
    aic_u16 id;
    aic_u16 dest_id;
    aic_u16 src_id;
    aic_u16 param_len;
    aic_u32 param[];
};

struct aic8800d_ipc_e2a_msg {
    aic_u16 id;
    aic_u16 dummy_dest_id;
    aic_u16 dummy_src_id;
    aic_u16 param_len;
    aic_u32 pattern;
    aic_u32 param[(AIC8800D_MAX_CMD_PARAM + 3) / 4];
};

/* Request parameter structures */
struct aic8800d_mm_set_stack_start_req {
    aic_u8 is_stack_start;
    aic_u8 efuse_valid;
    aic_u8 set_vendor_info;
    aic_u8 fwtrace_redir;
};

struct aic8800d_mm_get_fw_version_req {
    aic_u8 dummy;
};

struct aic8800d_mm_get_fw_version_cfm {
    aic_u8 fw_version_len;
    aic_u8 fw_version[63];
};

struct aic8800d_mm_reset_req {
    aic_u32 dummy;
};

struct aic8800d_mm_get_mac_addr_req {
    aic_u8  sta_idx;
};

struct aic8800d_mm_version_req {
    aic_u32 dummy;
};

struct aic8800d_mm_add_if_req {
    aic_u8  type;                   /* MM_STA / MM_AP / ... (must be first, matches Linux) */
    struct aic8800d_mac_addr addr;
    aic_u8  p2p;
};

struct aic8800d_mm_set_filter_req {
    aic_u32 filter;
};

/* TX power level configurations (v1/v2/v3/v4 unions) */
struct aic8800d_txpwr_lvl_conf {
    aic_u8 enable;
    aic_s8 dsss;
    aic_s8 ofdmlowrate_2g4;
    aic_s8 ofdm64qam_2g4;
    aic_s8 ofdm256qam_2g4;
    aic_s8 ofdm1024qam_2g4;
    aic_s8 ofdmlowrate_5g;
    aic_s8 ofdm64qam_5g;
    aic_s8 ofdm256qam_5g;
    aic_s8 ofdm1024qam_5g;
};

struct aic8800d_txpwr_lvl_conf_v2 {
    aic_u8 enable;
    aic_s8 pwrlvl_11b_11ag_2g4[12];
    aic_s8 pwrlvl_11n_11ac_2g4[10];
    aic_s8 pwrlvl_11ax_2g4[12];
};

struct aic8800d_txpwr_lvl_conf_v3 {
    aic_u8 enable;
    aic_s8 pwrlvl_11b_11ag_2g4[12];
    aic_s8 pwrlvl_11n_11ac_2g4[10];
    aic_s8 pwrlvl_11ax_2g4[12];
    aic_s8 pwrlvl_11a_5g[12];
    aic_s8 pwrlvl_11n_11ac_5g[10];
    aic_s8 pwrlvl_11ax_5g[12];
};

struct aic8800d_txpwr_lvl_conf_v4 {
    aic_u8 enable;
    aic_s8 pwrlvl_11b_11ag_2g4[12];
    aic_s8 pwrlvl_11n_11ac_2g4[10];
    aic_s8 pwrlvl_11ax_2g4[12];
    aic_s8 pwrlvl_11a_5g[12];
    aic_s8 pwrlvl_11n_11ac_5g[10];
    aic_s8 pwrlvl_11ax_5g[12];
    aic_s8 pwrlvl_11a_6g[8];
    aic_s8 pwrlvl_11n_11ac_6g[10];
    aic_s8 pwrlvl_11ax_6g[12];
};

struct aic8800d_mm_set_txpwr_lvl_req {
    union {
        struct aic8800d_txpwr_lvl_conf      txpwr_lvl;
        struct aic8800d_txpwr_lvl_conf_v2   txpwr_lvl_v2;
        struct aic8800d_txpwr_lvl_conf_v3   txpwr_lvl_v3;
        struct aic8800d_txpwr_lvl_conf_v4   txpwr_lvl_v4;
    } u;
};

struct aic8800d_txpwr_lvl_adj_conf {
    aic_u8 enable;
    aic_s8 pwrlvl_adj_tbl_2g4[3];
    aic_s8 pwrlvl_adj_tbl_5g[6];
};

struct aic8800d_mm_set_txpwr_lvl_adj_req {
    struct aic8800d_txpwr_lvl_adj_conf txpwr_lvl_adj;
};

struct aic8800d_txpwr_ofst_conf {
    aic_u8 enable;
    aic_s8 chan_1_4;
    aic_s8 chan_5_9;
    aic_s8 chan_10_13;
    aic_s8 chan_36_64;
    aic_s8 chan_100_120;
    aic_s8 chan_122_140;
    aic_s8 chan_142_165;
};

struct aic8800d_txpwr_ofst2x_conf {
    aic_u8 enable;
    aic_s8 pwrofst2x_tbl_2g4[3][3];
    aic_s8 pwrofst2x_tbl_5g[3][6];
};

struct aic8800d_mm_set_txpwr_ofst_req {
    union {
        struct aic8800d_txpwr_ofst_conf     txpwr_ofst;
        struct aic8800d_txpwr_ofst2x_conf   txpwr_ofst2x;
    } u;
};

struct aic8800d_mm_set_rf_calib_req {
    aic_u32 cal_cfg_24g;
    aic_u32 cal_cfg_5g;
    aic_u32 param_alpha;
    aic_u32 bt_calib_en;
    aic_u32 bt_calib_param;
    aic_u8  xtal_cap;
    aic_u8  xtal_cap_fine;
    aic_u8  __pad[2];
};

struct aic8800d_mm_set_rf_calib_cfm {
    aic_u32 rxgain_24g_addr;
    aic_u32 rxgain_5g_addr;
    aic_u32 txgain_24g_addr;
    aic_u32 txgain_5g_addr;
};

struct aic8800d_mm_set_channel_req {
    aic_u8  band;
    aic_u8  type;
    aic_u16 prim20_freq;
    aic_u16 center1_freq;
    aic_u16 center2_freq;
    aic_u8  index;
    aic_u8  dummy;
};

/* Confirmation parameter structures */
struct aic8800d_mm_get_mac_addr_cfm {
    aic_u8  mac_addr[6];
    aic_u8  status;
    aic_u8  pad;
};

struct aic8800d_mm_add_if_cfm {
    aic_u8  status;
    aic_u8  inst_nbr;
};

struct aic8800d_mm_version_cfm {
    aic_u32 version_lmac;
    aic_u32 version_machw_1;
    aic_u32 version_machw_2;
    aic_u32 version_phy_1;
    aic_u32 version_phy_2;
    aic_u32 features;
    aic_u16 max_sta_nb;
    aic_u8  max_vif_nb;
    aic_u8  pad;
};

struct aic8800d_mm_reset_cfm {
    aic_u32 dummy;
};

struct aic8800d_mm_set_channel_cfm {
    aic_u32 radio_cnt;
};

struct aic8800d_mm_set_filter_cfm {
    aic_u32 dummy;
};

struct aic8800d_mm_start_req {
    aic_u32 phy_cfg[16];
    aic_u32 uapsd_timeout;
    aic_u16 lp_clk_accuracy;
};

struct aic8800d_mm_start_cfm {
    aic_u32 dummy;
};

struct aic8800d_mm_set_stack_start_cfm {
    aic_u8 is_5g_support;
    aic_u8 vendor_info;
};

/* ME */
#define AIC8800D_MAC_DOMAINCHANNEL_24G_MAX  14
#define AIC8800D_MAC_DOMAINCHANNEL_5G_MAX   28

#define AIC8800D_MAC_MCS_MAX_LEN            16
#define AIC8800D_MAC_HE_MAC_CAPA_LEN        6
#define AIC8800D_MAC_HE_PHY_CAPA_LEN        11
#define AIC8800D_MAC_HE_PPE_THRES_MAX_LEN   25

struct aic8800d_mac_ht_capability {
    aic_u16 ht_capa_info;
    aic_u8  a_mpdu_param;
    aic_u8  mcs_rate[AIC8800D_MAC_MCS_MAX_LEN];
    aic_u16 ht_extended_capa;
    aic_u32 tx_beamforming_capa;
    aic_u8  asel_capa;
};

struct aic8800d_mac_vht_capability {
    aic_u32 vht_capa_info;
    aic_u16 rx_mcs_map;
    aic_u16 rx_highest;
    aic_u16 tx_mcs_map;
    aic_u16 tx_highest;
};

struct aic8800d_mac_he_mcs_nss_supp {
    aic_u16 rx_mcs_80;
    aic_u16 tx_mcs_80;
    aic_u16 rx_mcs_160;
    aic_u16 tx_mcs_160;
    aic_u16 rx_mcs_80p80;
    aic_u16 tx_mcs_80p80;
};

struct aic8800d_mac_he_capability {
    aic_u8  mac_cap_info[AIC8800D_MAC_HE_MAC_CAPA_LEN];
    aic_u8  phy_cap_info[AIC8800D_MAC_HE_PHY_CAPA_LEN];
    struct aic8800d_mac_he_mcs_nss_supp mcs_supp;
    aic_u8  ppe_thres[AIC8800D_MAC_HE_PPE_THRES_MAX_LEN];
};

struct aic8800d_me_config_req {
    struct aic8800d_mac_ht_capability  ht_cap;
    struct aic8800d_mac_vht_capability vht_cap;
    struct aic8800d_mac_he_capability  he_cap;
    aic_u16 tx_lft;
    aic_u8  phy_bw_max;
    aic_bool ht_supp;
    aic_bool vht_supp;
    aic_bool he_supp;
    aic_bool he_ul_on;
    aic_bool ps_on;
    aic_bool ant_div_on;
    aic_bool dpsm;
};

struct aic8800d_me_chan_config_req {
    struct aic8800d_mac_chan_def chan2G4[AIC8800D_MAC_DOMAINCHANNEL_24G_MAX];
    struct aic8800d_mac_chan_def chan5G[AIC8800D_MAC_DOMAINCHANNEL_5G_MAX];
    aic_u8  chan2G4_cnt;
    aic_u8  chan5G_cnt;
};

/* SCANU */
#define AIC8800D_SCAN_CHANNEL_MAX   42
#define AIC8800D_SCAN_SSID_MAX      3

struct aic8800d_scanu_start_req {
    struct aic8800d_mac_chan_def chan[AIC8800D_SCAN_CHANNEL_MAX];
    struct aic8800d_mac_ssid     ssid[AIC8800D_SCAN_SSID_MAX];
    struct aic8800d_mac_addr     bssid;
    aic_u32 add_ies;
    aic_u16 add_ie_len;
    aic_u8  vif_idx;
    aic_u8  chan_cnt;
    aic_u8  ssid_cnt;
    aic_u16 no_cck;
    aic_u32 duration;
};

struct aic8800d_scanu_start_cfm {
    aic_u8 vif_idx;
    aic_u8 status;
    aic_u8 result_cnt;
};

struct aic8800d_scanu_cancel_req {
};

struct aic8800d_scanu_cancel_cfm {
    aic_u8 status;
};

struct aic8800d_scanu_result_ind {
    aic_u16 length;
    aic_u16 framectrl;
    aic_u16 center_freq;
    aic_u8  band;
    aic_u8  sta_idx;
    aic_u8  inst_nbr;
    aic_s8  rssi;
    aic_u32 payload[];
};

/* SM */
#define AIC8800D_SM_ASSOC_IE_LEN    256

/* mac_connection_flags from Linux lmac_mac.h */
#define AIC8800D_CONNECTION_FLAG_CONTROL_PORT_HOST  (1u << 0)
#define AIC8800D_CONNECTION_FLAG_CONTROL_PORT_NO_ENC (1u << 1)
#define AIC8800D_CONNECTION_FLAG_DISABLE_HT         (1u << 2)
#define AIC8800D_CONNECTION_FLAG_WPA_WPA2_IN_USE    (1u << 3)
#define AIC8800D_CONNECTION_FLAG_MFP_IN_USE         (1u << 4)
#define AIC8800D_CONNECTION_FLAG_REASSOCIATION      (1u << 5)

/* TX/RX data descriptor sizes */
#define AIC8800D_TXDESC_API_SIZE    20  /* sizeof(struct hostdesc) */
#define AIC8800D_RX_HWHDR_LEN       60  /* RX_HWHRD_LEN in Linux driver */
#define AIC8800D_USB_TX_HEADER_LEN  8   /* USB bulk TX aggregation header */
#define AIC8800D_USB_RX_ALIGNMENT   4

struct aic8800d_sm_connect_req {
    struct aic8800d_mac_ssid     ssid;
    aic_u8  __pad_after_ssid;
    struct aic8800d_mac_addr     bssid;
    struct aic8800d_mac_chan_def chan;
    aic_u8  __pad_after_chan[2];
    aic_u32 flags;
    aic_u16 ctrl_port_ethertype;
    aic_u16 ie_len;
    aic_u16 listen_interval;
    aic_u8  dont_wait_bcmc;
    aic_u8  auth_type;
    aic_u8  uapsd_queues;
    aic_u8  vif_idx;
    aic_u32 ie_buf[AIC8800D_SM_ASSOC_IE_LEN / 4];
};

struct aic8800d_sm_connect_cfm {
    aic_u8 status;
};

/* SM_CONNECT_IND carries assoc IEs in a SM_ASSOC_IE_LEN(800)-byte buffer,
 * larger than the 256-byte ie_buf of SM_CONNECT_REQ (Linux lmac_msg.h). */
#define AIC8800D_SM_ASSOC_IND_IE_LEN    800

struct aic8800d_sm_connect_ind {
    aic_u16 status_code;
    struct aic8800d_mac_addr bssid;
    aic_u8  roamed;
    aic_u8  vif_idx;
    aic_u8  ap_idx;
    aic_u8  ch_idx;
    aic_u8  qos;
    aic_u8  acm;
    aic_u16 assoc_req_ie_len;
    aic_u16 assoc_rsp_ie_len;
    aic_u32 assoc_ie_buf[AIC8800D_SM_ASSOC_IND_IE_LEN / 4];
    aic_u16 aid;
    aic_u8  band;
    aic_u16 center_freq;
    aic_u8  width;
    aic_u32 center_freq1;
    aic_u32 center_freq2;
    aic_u32 ac_param[4];
};

struct aic8800d_sm_disconnect_req {
    aic_u8  vif_idx;
    aic_u16 reason_code;
};

struct aic8800d_sm_disconnect_ind {
    aic_u16 reason_code;
    aic_u8  vif_idx;
};

struct aic8800d_hostdesc {
    aic_u16 packet_len;
    aic_u16 flags_ext;
    struct aic8800d_mac_addr eth_dest_addr;
    struct aic8800d_mac_addr eth_src_addr;
    aic_u16 ethertype;
    aic_u8  ac;
    aic_u8  tid;
    aic_u8  vif_idx;
    aic_u8  staid;
    aic_u16 flags;
};

struct aic8800d_txdesc_api {
    struct aic8800d_hostdesc host;
};

/* USB bulk TX aggregation header (8 bytes) */
struct aic8800d_usb_tx_header {
    aic_u8 len_lsb;
    aic_u8 len_msb;     /* bits 0..3 */
    aic_u8 len2_lsb;
    aic_u8 len2_msb;
    aic_u8 len3_lsb;
    aic_u8 len3_msb;
    aic_u8 type;        /* AIC8800D_USB_TYPE_DATA */
    aic_u8 reserved;
};

struct aic8800d_hw_rxhdr {
    /* hw_rxhdr is 60 bytes; we only need the fields used for routing.
     * The full layout includes hwvect (36 bytes), phy_info (8 bytes),
     * flags (4 bytes), pattern (4 bytes). Keep opaque for now. */
    aic_u8  raw[AIC8800D_RX_HWHDR_LEN];
};

/* DBG */
#define AIC8800D_PATCH_MAGIC_NUM        0x48435450  /* "PTCH" */
#define AIC8800D_PATCH_MAGIC_NUM_2      0x50544348  /* "HCTP" */
#define AIC8800D_PATCH_BLOCK_MAX        4

struct aic8800d_patch_desc {
    aic_u32 magic_num;
    aic_u32 pair_start;
    aic_u32 magic_num_2;
    aic_u32 pair_count;
    aic_u32 block_dst[AIC8800D_PATCH_BLOCK_MAX];
    aic_u32 block_src[AIC8800D_PATCH_BLOCK_MAX];
    aic_u32 block_size[AIC8800D_PATCH_BLOCK_MAX];
};

/* Default patch table for AIC8800D80 U02 (2.4G/5G selection, aggregation) */
#define AIC8800D_PATCH_TBL_COUNT        2
static const uint32_t __aic8800d_patch_tbl[AIC8800D_PATCH_TBL_COUNT][2] = {
    {0x00b4, 0xf3010001},   /* 5G + ... */
    {0x0170, 0x0001000A},   /* rx aggr counter */
};

/* Boot-time upload addresses (from fw_patch_table INF record:
 * addr_adid=0x00201940, addr_patch=0x001E0000; the hardcoded defaults in
 * Linux aic_compat_8800d80.h are overridden by aicbt_patch_info_unpack()) */
#define AIC8800D_ADID_ADDR              0x00201940
#define AIC8800D_PATCH_ADDR             0x001E0000

/* Firmware binary (auto-generated from fmacfw_8800d80_u02.bin) */
extern const uint8_t __aic8800d_firmware_data[];
extern const uint32_t __aic8800d_firmware_size;
/* ADID (chip trim/calibration data), auto-generated from fw_adid_8800d80_u02.bin */
extern const uint8_t __aic8800d_adid_data[];
extern const uint32_t __aic8800d_adid_size;
/* Boot patch code, auto-generated from fw_patch_8800d80_u02.bin */
extern const uint8_t __aic8800d_patch_data[];
extern const uint32_t __aic8800d_patch_size;
/* Boot patch table, auto-generated from fw_patch_table_8800d80_u02.bin */
extern const uint8_t __aic8800d_patch_table_data[];
extern const uint32_t __aic8800d_patch_table_size;

struct aic8800d_dbg_mem_read_req {
    aic_u32 mem_addr;
};

struct aic8800d_dbg_mem_read_cfm {
    aic_u32 mem_addr;
    aic_u32 mem_data;
};

struct aic8800d_dbg_mem_write_req {
    aic_u32 mem_addr;
    aic_u32 mem_data;
};

struct aic8800d_dbg_mem_write_cfm {
    aic_u32 mem_addr;
    aic_u32 mem_data;
};

struct aic8800d_dbg_mem_mask_write_req {
    aic_u32 mem_addr;
    aic_u32 mem_mask;
    aic_u32 mem_data;
};

struct aic8800d_dbg_mem_mask_write_cfm {
    aic_u32 mem_addr;
    aic_u32 mem_data;
};

struct aic8800d_dbg_mem_block_write_req {
    aic_u32 mem_addr;
    aic_u32 mem_size;
    aic_u32 mem_data[];
};

struct aic8800d_dbg_mem_block_read_req {
    aic_u32 mem_addr;
    aic_u32 mem_size;
};

struct aic8800d_dbg_mem_block_read_cfm {
    aic_u32 mem_addr;
    aic_u32 mem_size;
    aic_u32 mem_data[];
};

struct aic8800d_dbg_start_app_req {
    aic_u32 boot_addr;
    aic_u32 boot_type;
    aic_u32 boot_param;
};

struct aic8800d_dbg_start_app_cfm {
    aic_u32 status;
};

/* Pending command record */
typedef struct aic8800d_cmd_t {
    uint16_t            id;             /* A2E request message id */
    uint16_t            cfm_id;         /* expected E2A confirmation id */
    uint8_t             token;          /* local token for matching */
    bool                busy;
    vsf_wifi_done_t     done;           /* caller completion callback */
    void               *cfm_param;      /* buffer to copy E2A param into */
    uint16_t            cfm_param_len;  /* size of cfm_param buffer */
} aic8800d_cmd_t;

/* Chip driver private state */
typedef struct aic8800d_priv_t {
    vsf_wifi_t         *wifi;
    const vsf_wifi_aic8800d_bus_ops_t *bus_ops;

    uint8_t             vif_idx;
    uint8_t             mac_addr[6];
    bool                mac_addr_valid;

    /* Command manager */
    aic8800d_cmd_t      cmds[AIC8800D_MAX_PENDING_CMDS];
    uint8_t             next_token;

    /* Init state machine */
    uint8_t             init_step;
    vsf_wifi_done_t     init_done;

    /* Firmware load state */
    uint8_t             fw_load_step;
    uint8_t            *fw_buf;
    uint32_t            fw_size;
    uint32_t            fw_offset;
    uint32_t            fw_addr;
    uint8_t             fw_img_idx;
    vsf_wifi_done_t     fw_done;
    struct aic8800d_dbg_mem_read_cfm sysconfig_rd_cfm;
    uint8_t             chip_mcu_id;

    /* Boot patch table (fw_patch_table_8800d80_u02.bin) write state */
    uint16_t            patch_tbl_idx;
    uint16_t            patch_tbl_count;
    uint32_t            patch_tbl_pairs[96][2];

    /* Patch config state */
    uint8_t             patch_step;
    struct aic8800d_dbg_mem_read_cfm patch_rd_cfm;
    uint32_t            config_base;
    uint32_t            patch_str_base;
    uint32_t            fw_version;
    uint32_t            patch_buff_base;
    uint8_t             patch_write_idx;

    /* System config state */
    uint8_t             syscfg_idx;
    uint8_t             syscfg_masked_idx;

    /* RF config state */
    uint8_t             rf_cfg_idx;

    /* Scan state */
    vsf_wifi_done_t     scan_done;
    bool                scan_active;
    bool                scan_finish_pending;
    uint8_t             scan_finish_retries;
    uint8_t             scan_results_expected;
    uint8_t             scan_results_received;
    vsf_callback_timer_t scan_finish_timer;
#define AIC8800D_SCAN_RSN_CAP_CACHE_SIZE    8
    struct {
        uint8_t  bssid[6];
        uint16_t rsn_cap;
    } scan_rsn_cap_cache[AIC8800D_SCAN_RSN_CAP_CACHE_SIZE];
    uint8_t             scan_rsn_cap_cache_num;

    /* Connect state */
    struct aic8800d_sm_connect_req connect_req;
    vsf_wifi_done_t     connect_done;
    bool                connect_active;
    uint16_t            ap_rsn_cap;

    /* Link state (cached from SM_CONNECT_IND) */
    uint8_t             ap_idx;
    uint8_t             sta_idx;
    bool                qos;
} aic8800d_priv_t;

/*============================ GLOBAL VARIABLES ==============================*/

/*============================ LOCAL VARIABLES ===============================*/

/*============================ PROTOTYPES ====================================*/

aic8800d_priv_t *__aic8800d_priv(vsf_wifi_t *wifi);
vsf_err_t __aic8800d_send_msg(vsf_wifi_t *wifi, uint16_t id,
        const void *param, uint16_t param_len,
        uint16_t cfm_id, void *cfm_param, uint16_t cfm_param_len,
        vsf_wifi_done_t done);
void __aic8800d_cmd_cfm(vsf_wifi_t *wifi, aic8800d_cmd_t *cmd,
        const struct aic8800d_ipc_e2a_msg *msg);

#ifdef __cplusplus
}
#endif

#endif      /* VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_AIC8800D == ENABLED */
#endif      /* __VSF_WIFI_AIC8800D_PRIV_H__ */
