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

#ifndef __VSF_WIFI_MT76_PRIV_H__
#define __VSF_WIFI_MT76_PRIV_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_wifi_mt76.h"

#if VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_MT76 == ENABLED

/*============================ MACROS ========================================*/

#define MT76_VEND_TYPE_CFG                  ((uint32_t)1 << 30)

#define MT76_ASIC_VERSION                   0x0000
#define MT76_USB_U3DMA_CFG                  (MT76_VEND_TYPE_CFG | 0x9018)
#define MT76_USB_DMA_CFG_RX_DROP_OR_PAD     (1U << 18)
#define MT76_USB_DMA_CFG_RX_BULK_AGG_EN     (1U << 21)
#define MT76_USB_DMA_CFG_RX_BULK_EN         (1U << 22)
#define MT76_USB_DMA_CFG_TX_BULK_EN         (1U << 23)
#define MT76_WPDMA_GLO_CFG                  0x0208
#define MT76_MAC_SYS_CTRL                   0x1004
#define MT76_MAC_SYS_CTRL_RESET_CSR         (1U << 0)
#define MT76_MAC_SYS_CTRL_RESET_BBP         (1U << 1)
#define MT76_MAC_SYS_CTRL_ENABLE_TX         (1U << 2)
#define MT76_MAC_SYS_CTRL_ENABLE_RX         (1U << 3)
#define MT76_RX_FILTR_CFG                   0x1400

#define MT76_FCE_DMA_ADDR                   0x0230
#define MT76_FCE_DMA_LEN                    0x0234
#define MT76_TX_CPU_FROM_FCE_CPU_DESC_IDX   0x09A8
#define MT76_TX_CPU_FROM_FCE_BASE_PTR       0x09A0
#define MT76_TX_CPU_FROM_FCE_MAX_COUNT      0x09A4
#define MT76_FCE_PSE_CTRL                   0x0800
#define MT76_FCE_PDMA_GLOBAL_CONF           0x09C4
#define MT76_FCE_SKIP_FS                    0x0A6C

#define MT76_MCU_COM_REG0                   0x0730
#define MT76_MCU_CLOCK_CTL                  0x0708
#define MT76_MCU_RESET_CTL                  0x070C
#define MT76_MCU_SEMAPHORE_03               0x07BC

#define MT76_REV_E3                         0x22

#define MT76_CHIP_ID_7612                   0x7612
#define MT76_CHIP_ID_7632                   0x7632
#define MT76_CHIP_ID_7662                   0x7662
#define MT76_CHIP_ID_7602                   0x7602

#define MT76_PATCH_HDR_SIZE                 30
#define MT76_FW_HDR_SIZE                    32
#define MT76_ROM_PATCH_OFFSET               0x00090000
#define MT76_ILM_OFFSET                     0x00080000
#define MT76_DLM_OFFSET                     0x00110000
#define MT76_DLM_OFFSET_E3_EXTRA            0x00000800

#define MT76_USB_DMA_CFG_VAL                0x00C00020

#define MT76_WLAN_FUN_CTRL                  0x0080
#define MT76_WLAN_FUN_CTRL_WLAN_EN          (1U << 0)
#define MT76_WLAN_FUN_CTRL_WLAN_CLK_EN      (1U << 1)
#define MT76_WLAN_FUN_CTRL_WLAN_RESET_RF    (1U << 2)
#define MT76_WLAN_FUN_CTRL_FRC_WL_ANT_SEL   (1U << 5)

#define MT76_WLAN_MTC_CTRL                  (0x40000000 | 0x148)
#define MT76_WLAN_MTC_CTRL_MTCMOS_PWR_UP    (1U << 0)
#define MT76_WLAN_MTC_CTRL_PWR_ACK          (1U << 12)
#define MT76_WLAN_MTC_CTRL_PWR_ACK_S        (1U << 13)
#define MT76_WLAN_MTC_CTRL_STATE_UP         (1U << 28)

#define MT76_MAC_CSR0                       0x1000

/* Additional MAC/BBP registers needed for mt76x2u_init_hardware() */
#define MT76_WPDMA_GLO_CFG_TX_DMA_EN        (1U << 0)
#define MT76_WPDMA_GLO_CFG_TX_DMA_BUSY      (1U << 1)
#define MT76_WPDMA_GLO_CFG_RX_DMA_EN        (1U << 2)
#define MT76_WPDMA_GLO_CFG_RX_DMA_BUSY      (1U << 3)

#define MT76_US_CYC_CFG                     0x02a4
#define MT76_US_CYC_CNT                     0x1e

#define MT76_PBF_SYS_CTRL                   0x0400
#define MT76_PBF_CFG                        0x0404
#define MT76_PBF_TX_MAX_PCNT                0x0408
#define MT76_PBF_RX_MAX_PCNT                0x040c
#define MT76_BCN_OFFSET_BASE                0x041c
#define MT76_BCN_OFFSET(_n)                 (MT76_BCN_OFFSET_BASE + ((_n) << 2))

#define MT76_MAC_ADDR_DW0                   0x1008
#define MT76_MAC_ADDR_DW1                   0x100c
#define MT76_MAC_ADDR_DW1_U2ME_MASK         0x00ff0000
#define MT76_MAC_BSSID_DW0                  0x1010
#define MT76_MAC_BSSID_DW1                  0x1014
#define MT76_MAC_BSSID_DW1_MBSS_MODE        (3U << 16)
#define MT76_MAC_BSSID_DW1_MBSS_LOCAL_BIT   (1U << 21)
#define MT76_MAC_BSSID_DW1_MBEACON_N_SHIFT  18

#define MT76_MAX_LEN_CFG                    0x1018
#define MT76_AMPDU_MAX_LEN_20M1S            0x1030
#define MT76_AMPDU_MAX_LEN_20M2S            0x1034

#define MT76_BCN_BYPASS_MASK                0x108c
#define MT76_XIFS_TIME_CFG                  0x1100
#define MT76_BKOFF_SLOT_CFG                 0x1104
#define MT76_BEACON_TIME_CFG                0x1114
#define MT76_BEACON_TIME_CFG_INTVAL_MASK    0xFFFFu
#define MT76_BEACON_TIME_CFG_TIMER_EN       (1U << 16)
#define MT76_BEACON_TIME_CFG_SYNC_MODE      (3U << 17)
#define MT76_BEACON_TIME_CFG_SYNC_MODE_STA  (1U << 17)
#define MT76_BEACON_TIME_CFG_TBTT_EN        (1U << 19)
#define MT76_TBTT_SYNC_CFG                  0x1118
#define MT76_MAC_STATUS                     0x1200
#define MT76_MAC_STATUS_TX                  (1U << 0)
#define MT76_MAC_STATUS_RX                  (1U << 1)
#define MT76_PWR_PIN_CFG                    0x1204
#define MT76_AUX_CLK_CFG                    0x120c
#define MT76_DACCLK_EN_DLY_CFG              0x1264

#define MT76_TX_SW_CFG0                     0x1330
#define MT76_TX_SW_CFG1                     0x1334
#define MT76_TX_SW_CFG2                     0x1338
#define MT76_TXOP_CTRL_CFG                  0x1340
#define MT76_TXOP_ED_CCA_EN                 (1U << 20)
#define MT76_TX_RTS_CFG                     0x1344
#define MT76_TX_RTS_CFG_RETRY_LIMIT         0x000000FF
#define MT76_TX_TIMEOUT_CFG                 0x1348
#define MT76_FCE_L2_STUFF                   0x080c
#define MT76_FCE_L2_STUFF_WR_MPDU_LEN_EN    (1U << 4)
#define MT76_RF_BYPASS_0                    0x0504
#define MT76_RF_SETTING_0                   0x050c
#define MT76_TX_RETRY_CFG                   0x134c
#define MT76_PIFS_TX_CFG                    0x1388
#define MT76_TX_LINK_CFG                    0x1350
#define MT76_HEADER_TRANS_CTRL_REG          0x0260
#define MT76_TSO_CTRL                       0x0250
#define MT76_TX_ALC_VGA3                    0x0778
#define MT76_VHT_HT_FBK_CFG1                0x1358
#define MT76_CCK_PROT_CFG                   0x1364
#define MT76_OFDM_PROT_CFG                  0x1368
#define MT76_MM20_PROT_CFG                  0x136c
#define MT76_MM40_PROT_CFG                  0x1370
#define MT76_GF20_PROT_CFG                  0x1374
#define MT76_GF40_PROT_CFG                  0x1378
#define MT76_EXP_ACK_TIME                   0x1380

/* Band / bandwidth configuration registers ( Linux mt76x02 ) */
#define MT76_TX_BAND_CFG                    0x132c
#define MT76_TX_BAND_CFG_UPPER_40M          (1U << 0)
#define MT76_TX_BAND_CFG_5G                 (1U << 1)
#define MT76_TX_BAND_CFG_2G                 (1U << 2)

#define MT76_EXT_CCA_CFG                    0x141c
#define MT76_EXT_CCA_CFG_CCA0_M             0x00000003
#define MT76_EXT_CCA_CFG_CCA1_M             0x0000000c
#define MT76_EXT_CCA_CFG_CCA2_M             0x00000030
#define MT76_EXT_CCA_CFG_CCA3_M             0x000000c0
#define MT76_EXT_CCA_CFG_CCA_MASK_M         0x00000f00

/* BBP indirect register bases */
#define MT76_BBP_CORE_BASE                  0x2000
#define MT76_BBP_AGC_BASE                   0x2300
#define MT76_BBP_TXO_BASE                   0x2600
#define MT76_BBP_TXBE_BASE                  0x2700
#define MT76_BBP_RXO_BASE                   0x2900
#define MT76_BBP(_type, _n)                 (MT76_BBP_##_type##_BASE + ((_n) << 2))
#define MT76_BBP_CORE_R1_BW_M               (3U << 3)   /* bits 4:3 */
#define MT76_BBP_CORE_R1_BW_20              (0U << 3)
#define MT76_BBP_CORE_R1_BW_40              (2U << 3)
#define MT76_BBP_CORE_R1_BW_80              (3U << 3)
#define MT76_BBP_AGC_R0_BW_M                (7U << 12)  /* bits 14:12 */
#define MT76_BBP_AGC_R0_BW_20               (1U << 12)
#define MT76_BBP_AGC_R0_BW_40               (3U << 12)
#define MT76_BBP_AGC_R0_BW_80               (7U << 12)
#define MT76_BBP_AGC_R0_CTRL_CHAN_M         (3U << 8)   /* bits 9:8 */
#define MT76_BBP_TXBE_R0_CTRL_CHAN_M        (3U << 0)   /* bits 1:0 */

#define MT_TX_STAT_FIFO                     0x1718
#define MT_TX_STAT_FIFO_VALID               (1U << 0)
#define MT_TX_STAT_FIFO_SUCCESS             (1U << 5)
#define MT_TX_STAT_FIFO_AGGR                (1U << 6)
#define MT_TX_STAT_FIFO_ACKREQ              (1U << 7)
#define MT_TX_STAT_FIFO_WCID_M              0x0000FF00
#define MT_TX_STAT_FIFO_WCID_S              8
#define MT_TX_STAT_FIFO_RATE_M              0xFFFF0000
#define MT_TX_STAT_FIFO_RATE_S              16

#define MT_TX_STAT_FIFO_EXT                 0x1798
#define MT_TX_STAT_FIFO_EXT_RETRY_M         0x000000FF
#define MT_TX_STAT_FIFO_EXT_RETRY_S         0
#define MT_TX_STAT_FIFO_EXT_PKTID_M         0x0000FF00
#define MT_TX_STAT_FIFO_EXT_PKTID_S         8

#define MT76_PACKET_ID_HAS_RATE             0x80
#define MT76_PKTID_RATE_M                   0x1F
#define MT76_HT_FBK_TO_LEGACY               0x1384
#define MT76_TX_PROT_CFG6                   0x13e0
#define MT76_TX_PROT_CFG7                   0x13e4
#define MT76_TX_PROT_CFG8                   0x13e8
#define MT76_TX_PWR_CFG_0                   0x1314
#define MT76_TX_PWR_CFG_1                   0x1318
#define MT76_TX_PWR_CFG_2                   0x131c
#define MT76_TX_PWR_CFG_3                   0x1320
#define MT76_TX_PWR_CFG_4                   0x1324
#define MT76_TX_PWR_CFG_7                   0x13d4
#define MT76_TX_PWR_CFG_8                   0x13d8
#define MT76_TX_PWR_CFG_9                   0x13dc

#define MT76_BB_PA_MODE_CFG0                0x1214
#define MT76_BB_PA_MODE_CFG1                0x1218
#define MT76_RF_PA_MODE_CFG0                0x121c
#define MT76_RF_PA_MODE_CFG1                0x1220
#define MT76_RF_PA_MODE_ADJ0                0x1228
#define MT76_RF_PA_MODE_ADJ1                0x122c

#define MT76_TX_SW_CFG0                     0x1330
#define MT76_TX_SW_CFG1                     0x1334

#define MT76_TX0_RF_GAIN_CORR               0x13a0
#define MT76_TX1_RF_GAIN_CORR               0x13a4
#define MT76_TX_ALC_CFG_0                   0x13b0
#define MT76_TX_ALC_CFG_0_CH_INIT_0_M       0x0000003f
#define MT76_TX_ALC_CFG_0_CH_INIT_0_S       0
#define MT76_TX_ALC_CFG_0_CH_INIT_1_M       0x00003f00
#define MT76_TX_ALC_CFG_0_CH_INIT_1_S       8
#define MT76_TX_ALC_CFG_0_LIMIT_0_M         0x003f0000
#define MT76_TX_ALC_CFG_0_LIMIT_0_S         16
#define MT76_TX_ALC_CFG_0_LIMIT_1_M         0x3f000000
#define MT76_TX_ALC_CFG_0_LIMIT_1_S         24
#define MT76_TX_ALC_CFG_2                   0x13a8
#define MT76_TX_ALC_CFG_3                   0x13ac

#define MT76_XIFS_TIME_CFG                  0x1100
#define MT76_XIFS_TIME_CFG_OFDM_SIFS_M      0x0000ff00
#define MT76_XIFS_TIME_CFG_OFDM_SIFS_S      8

#define MT76_LEGACY_BASIC_RATE              0x1408
#define MT76_HT_BASIC_RATE                  0x140c
#define MT76_HT_CTRL_CFG                    0x1410
#define MT76_TX_SW_CFG3                     0x1478
#define MT76_PN_PAD_MODE                    0x150c
#define MT76_TXOP_HLDR_ET                   0x1608
#define MT76_TXOP_HLDR_TX40M_BLK_EN         (1U << 1)
#define MT76_PROT_AUTO_TX_CFG               0x1648

#define MT76_EFUSE_CTRL                     0x0024
#define MT76_PAUSE_ENABLE_CONTROL1          0x0a38
#define MT76_FCE_WLAN_FLOW_CONTROL1         0x0824
#define MT76_WPDMA_DELAY_INT_CFG            0x0210

#define MT76_WCID_ATTR_BASE                 0xa800
#define MT76_WCID_ATTR(_n)                  (MT76_WCID_ATTR_BASE + (_n) * 4)
#define MT76_WCID_ATTR_PAIRWISE             (1U << 0)
#define MT76_WCID_ATTR_PKEY_MODE            (0x7U << 1)
#define MT76_WCID_ATTR_PKEY_MODE_SHIFT      1
#define MT76_WCID_ATTR_BSS_IDX              (0U << 4)
#define MT76_WCID_ATTR_BSS_IDX_EXT          (1U << 11)
#define MT76_WCID_ADDR_BASE                 0x1800
#define MT76_WCID_ADDR(_n)                  (MT76_WCID_ADDR_BASE + (_n) * 8)

#define MT76_WCID_DROP_BASE                 0x106c
#define MT76_WCID_DROP(_n)                  (MT76_WCID_DROP_BASE + ((_n) >> 5) * 4)
#define MT76_WCID_DROP_MASK(_n)             (1U << ((_n) % 32))

#define MT76_WCID_TX_INFO_BASE              0x1c00
#define MT76_WCID_TX_INFO(_n)               (MT76_WCID_TX_INFO_BASE + ((_n) * 8))

#define MT76_WCID_KEY_BASE                  0x8000
#define MT76_WCID_KEY(_n)                   (MT76_WCID_KEY_BASE + (_n) * 32)
#define MT76_WCID_IV_BASE                   0xa000
#define MT76_WCID_IV(_n)                    (MT76_WCID_IV_BASE + (_n) * 8)

#define MT_VIF_WCID(_n)                     (254 - ((_n) & 7))

#define MT76_SKEY_MODE_MASK                 0xFU
#define MT76_SKEY_MODE_SHIFT(_bss, _idx)    (4 * ((_idx) + 4 * ((_bss) & 1)))

enum mt76x02_cipher_type {
    MT76X02_CIPHER_NONE,
    MT76X02_CIPHER_WEP40,
    MT76X02_CIPHER_WEP104,
    MT76X02_CIPHER_TKIP,
    MT76X02_CIPHER_AES_CCMP,
    MT76X02_CIPHER_CKIP40,
    MT76X02_CIPHER_CKIP104,
    MT76X02_CIPHER_CKIP128,
    MT76X02_CIPHER_WAPI,
};
#define MT_WCID_TX_INFO_RATE                0x0000FFFFU
#define MT_WCID_TX_INFO_RATE_S              0
#define MT_WCID_TX_INFO_NSS_SHIFT           16
#define MT_WCID_TX_INFO_NSS_MASK            0x00030000U
#define MT_WCID_TX_INFO_TXPWR_ADJ_SHIFT     18
#define MT_WCID_TX_INFO_TXPWR_ADJ_MASK      0x03FC0000U
#define MT_WCID_TX_INFO_SET                 0x80000000U

#define MT76_SKEY_BASE_0                    0xac00
#define MT76_SKEY_BASE_1                    0xb400
#define MT76_SKEY(_bss, _idx)               \
    (((_bss) & 8) ? (MT76_SKEY_BASE_1 + (4 * ((_bss) & 7) + (_idx)) * 32) \
                  : (MT76_SKEY_BASE_0 + (4 * (_bss) + (_idx)) * 32))
#define MT76_SKEY_MODE_BASE_0               0xb000
#define MT76_SKEY_MODE_BASE_1               0xb3f0
#define MT76_SKEY_MODE(_bss)                \
    (((_bss) & 8) ? (MT76_SKEY_MODE_BASE_1 + ((((_bss) & 7) / 2) << 2)) \
                  : (MT76_SKEY_MODE_BASE_0 + (((_bss) / 2) << 2)))

/* BBP indirect registers */
#define MT76_BBP_CORE_BASE                  0x2000
#define MT76_BBP_IBI_BASE                   0x2100
#define MT76_BBP_AGC_BASE                   0x2300
#define MT76_BBP_TXO_BASE                   0x2600
#define MT76_BBP_TXBE_BASE                  0x2700
#define MT76_BBP_RXO_BASE                   0x2900
#define MT76_BBP(_type, _n)                 (MT76_BBP_##_type##_BASE + ((_n) << 2))

#define MT76_BBP_TXO_4_TX_CMM_BW            (1U << 25)
#define MT76_BBP_RXO_13_RX_CMM_PD_EN        (1U << 8)
#define MT76_BBP_RXO_13_LDPC_RX_EN          (1U << 10)

#define MT76_XO_CTRL5                       0x0114
#define MT76_XO_CTRL5_C2_VAL_MASK           0x7f00
#define MT76_XO_CTRL5_C2_VAL_SHIFT          8
#define MT76_XO_CTRL6                       0x0118
#define MT76_XO_CTRL6_C2_CTRL_MASK          0x7f00
#define MT76_XO_CTRL6_C2_CTRL_SHIFT         8
#define MT76_XO_CTRL7                       0x011c
#define MT76_COEXCFG0                       0x0040
#define MT76_COEXCFG0_COEX_EN               (1U << 0)
#define MT76_TX_ALC_CFG_4                   0x077c
#define MT76_TX_ALC_CFG_4_MPDU_2ND_TC       (1U << 31)
#define MT76_AUTO_RSP_CFG                   0x110c
#define MT76_WMM_AIFSN                      0x0214
#define MT76_WMM_CWMIN                      0x0218
#define MT76_WMM_CWMAX                      0x021c
#define MT76_WMM_TXOP_BASE                  0x0220
#define MT76_WMM_TXOP(_n)                   (MT76_WMM_TXOP_BASE + (((_n) >> 1) << 2))
#define MT76_EDCA_CFG_BASE                  0x1300
#define MT76_EDCA_CFG_AC(_n)                (MT76_EDCA_CFG_BASE + ((_n) << 2))
#define MT76_TXOP_THRES_CFG                 0x13ec

#define MT76_MCU_MSG_CMD_LOAD_CR            2
#define MT76_RF_BBP_CR                      2

#define MT76_MCU_MSG_TYPE_CMD               ((uint32_t)1 << 30)
#define MT76_MCU_MSG_CMD_TYPE_SHIFT         20
#define MT76_MCU_MSG_CMD_SEQ_SHIFT          16
#define MT76_MCU_MSG_PORT_SHIFT             27
#define MT76_MCU_MSG_LEN_MASK               0xFFFF

/* USB DMA header (TXD_INFO) prepended to MCU commands on the INBAND_CMD
 * bulk endpoint.  Firmware upload uses a different (FCE-based) path and does
 * not need this header. */
#define MT76_TXD_INFO_LEN_MASK              0xFFFF
#define MT76_TXD_INFO_DPORT_SHIFT           27
#define MT76_TXD_INFO_TYPE_CMD              ((uint32_t)1 << 30)

#define MT76_CPU_TX_PORT                    2
#define MT76_WLAN_PORT                      0

#define MT76_VEND_DEV_MODE                  0x01

#define MT76_FW_PATCH_CHUNK_PAYLOAD         (2048 - 8)
#define MT76_FW_FW_CHUNK_PAYLOAD            (MT76_TX_URB_SIZE - 8)

#define MT76_CMD_FUN_SET_OP                 1
#define MT76_CMD_POWER_SAVING_OP            20
#define MT76_FUN_Q_SELECT                   1
#define MT76_PWR_RADIO_ON                   0x31
#define MT76_PWR_RADIO_OFF                  0x30

#define MT_EE_READ                          0
#define MT76_EE_MAC_ADDR                    0x004
#define MT76_EE_NIC_CONF_0                  0x034
#define MT76_EE_NIC_CONF_1                  0x036
#define MT76_EE_XTAL_TRIM_1                 0x03a
#define MT76_EE_XTAL_TRIM_2                 0x09e
#define MT76_EE_NIC_CONF_2                  0x042

/* EEPROM RX path gain / RSSI offset fields (MT76x2) */
#define MT76_EE_LNA_GAIN                    0x044
#define MT76_EE_RSSI_OFFSET_2G_0            0x046
#define MT76_EE_RSSI_OFFSET_2G_1            0x048
#define MT76_EE_LNA_GAIN_5GHZ_1             0x049
#define MT76_EE_RSSI_OFFSET_5G_0            0x04a
#define MT76_EE_RSSI_OFFSET_5G_1            0x04c
#define MT76_EE_LNA_GAIN_5GHZ_2             0x04d

/* EEPROM TX power / calibration fields (MT76x2) */
#define MT76_EE_TX_POWER_DELTA_BW40         0x050
#define MT76_EE_TX_POWER_DELTA_BW80         0x052
#define MT76_EE_TX_POWER_EXT_PA_5G          0x054
#define MT76_EE_TX_POWER_0_START_2G         0x056
#define MT76_EE_TX_POWER_1_START_2G         0x05c
#define MT76_EE_TX_POWER_0_START_5G         0x062
#define MT76_EE_TX_POWER_1_START_5G         0x080
#define MT76_EE_TX_POWER_CCK                0x0a0
#define MT76_EE_TX_POWER_OFDM_2G_6M         0x0a2
#define MT76_EE_TX_POWER_OFDM_2G_24M        0x0a4
#define MT76_EE_TX_POWER_OFDM_5G_6M         0x0b2
#define MT76_EE_TX_POWER_OFDM_5G_24M        0x0b4
#define MT76_EE_TX_POWER_HT_MCS0            0x0a6
#define MT76_EE_TX_POWER_HT_MCS4            0x0a8
#define MT76_EE_TX_POWER_HT_MCS8            0x0aa
#define MT76_EE_TX_POWER_HT_MCS12           0x0ac
#define MT76_EE_TX_POWER_VHT_MCS8           0x0be
#define MT76_EE_2G_TARGET_POWER             0x0d0
#define MT76_EE_5G_TARGET_POWER             0x0d2
#define MT76_EE_RF_2G_TSSI_OFF_TXPOWER      0x0f6
#define MT76_EE_RF_2G_RX_HIGH_GAIN          0x0f8
#define MT76_EE_RF_5G_GRP0_1_RX_HIGH_GAIN   0x0fa
#define MT76_EE_RF_5G_GRP2_3_RX_HIGH_GAIN   0x0fc
#define MT76_EE_RF_5G_GRP4_5_RX_HIGH_GAIN   0x0fe

#define MT76_TX_POWER_GROUP_SIZE_5G         5
#define MT76_TX_POWER_GROUPS_5G             6

#define MT76_EE_NIC_CONF_0_PA_INT_2G        (1U << 8)
#define MT76_EE_NIC_CONF_0_PA_INT_5G        (1U << 9)
#define MT76_EE_NIC_CONF_1_TEMP_TX_ALC      (1U << 1)
#define MT76_EE_NIC_CONF_1_TX_ALC_EN        (1U << 13)
#define MT76_EE_NIC_CONF_1_LNA_EXT_2G       (1U << 2)
#define MT76_EE_NIC_CONF_1_LNA_EXT_5G       (1U << 3)
#define MT76_EE_TX_POWER_EXT_PA_5G_EN       (1U << 15)


#define MT76_CMD_INIT_GAIN_OP               3
#define MT76_CMD_SWITCH_CHANNEL_OP          30
#define MT76_CMD_CALIBRATION_OP             31

/* MCU calibration types for CMD_CALIBRATION_OP (MT76x2) */
#define MT76_MCU_CAL_R                      1
#define MT76_MCU_CAL_RXDCOC                 2
#define MT76_MCU_CAL_RC                     3

/* RX filter register */
#define MT_RX_FILTR_CFG                     0x1400
#define MT_RX_FILTR_CFG_CRC_ERR             (1U << 0)
#define MT_RX_FILTR_CFG_PHY_ERR             (1U << 1)
#define MT_RX_FILTR_CFG_PROMISC             (1U << 2)
#define MT_RX_FILTR_CFG_OTHER_BSS           (1U << 3)
#define MT_RX_FILTR_CFG_VER_ERR             (1U << 4)
#define MT_RX_FILTR_CFG_MCAST               (1U << 5)
#define MT_RX_FILTR_CFG_BCAST               (1U << 6)
#define MT_RX_FILTR_CFG_DUP                 (1U << 7)
#define MT_RX_FILTR_CFG_CFACK               (1U << 8)
#define MT_RX_FILTR_CFG_CFEND               (1U << 9)
#define MT_RX_FILTR_CFG_ACK                 (1U << 10)
#define MT_RX_FILTR_CFG_CTS                 (1U << 11)
#define MT_RX_FILTR_CFG_RTS                 (1U << 12)
#define MT_RX_FILTR_CFG_PSPOLL              (1U << 13)
#define MT_RX_FILTR_CFG_BA                  (1U << 14)
#define MT_RX_FILTR_CFG_BAR                 (1U << 15)
#define MT_RX_FILTR_CFG_CTRL_RSV            (1U << 16)

/* Default RX filter for STA mode (matches Linux mt76x2 initvals) */
#define MT_RX_FILTR_CFG_DEFAULT             0x00015f97

/* TX descriptor (TXINFO) flags */
#define MT_TXD_INFO_LEN_MASK                0x0000FFFF
#define MT_TXD_INFO_NEXT_VLD                (1U << 16)
#define MT_TXD_INFO_TX_BURST                (1U << 17)
#define MT_TXD_INFO_80211                   (1U << 19)
#define MT_TXD_INFO_TSO                     (1U << 20)
#define MT_TXD_INFO_CSO                     (1U << 21)
#define MT_TXD_INFO_WIV                     (1U << 24)
#define MT_TXD_INFO_QSEL_MASK               (3U << 25)
#define MT_TXD_INFO_QSEL_SHIFT              25
#define MT_TXD_INFO_DPORT_MASK              (7U << 27)
#define MT_TXD_INFO_DPORT_SHIFT             27
#define MT_TXD_INFO_TYPE_MASK               (3U << 30)

#define MT_QSEL_MGMT                        0
#define MT_QSEL_EDCA                        2
#define MT_WLAN_PORT                        0

/* TXWI */
#define MT_TXWI_FLAGS_FRAG                  (1U << 0)
#define MT_TXWI_FLAGS_MMPS                  (1U << 1)
#define MT_TXWI_FLAGS_CFACK                 (1U << 2)
#define MT_TXWI_FLAGS_TS                    (1U << 3)
#define MT_TXWI_FLAGS_AMPDU                 (1U << 4)
#define MT_TXWI_FLAGS_MPDU_DENSITY_MASK     (7U << 5)
#define MT_TXWI_FLAGS_TXOP_MASK             (3U << 8)
#define MT_TXWI_FLAGS_NDPS                  (1U << 10)
#define MT_TXWI_FLAGS_RTSBWSIG              (1U << 11)
#define MT_TXWI_FLAGS_NDP_BW_MASK           (3U << 12)
#define MT_TXWI_FLAGS_SOUND                 (1U << 14)
#define MT_TXWI_FLAGS_TX_RATE_LUT           (1U << 15)

#define MT_TXWI_ACK_CTL_REQ                 (1U << 0)
#define MT_TXWI_ACK_CTL_NSEQ                (1U << 1)
#define MT_TXWI_ACK_CTL_BA_WINDOW_MASK      (0x3FU << 2)

#define MT_RXWI_RATE_INDEX_MASK             0x003F
#define MT_RXWI_RATE_LDPC                   (1U << 6)
#define MT_RXWI_RATE_BW_MASK                (3U << 7)
#define MT_RXWI_RATE_SGI                    (1U << 9)
#define MT_RXWI_RATE_STBC                   (1U << 10)
#define MT_RXWI_RATE_PHY_MASK               (7U << 13)

#define MT_PHY_TYPE_CCK                     0
#define MT_PHY_TYPE_OFDM                    1
#define MT_PHY_TYPE_HT                      2
#define MT_PHY_TYPE_VHT                     4

/* USB RX DMA header + RXWI layout */
#define MT_DMA_HDR_LEN                      4
#define MT_RX_RXWI_LEN                      32
#define MT_FCE_INFO_LEN                     4

/* RXWI word0 (rxinfo) flags */
#define MT_RXINFO_BA                        (1U << 0)
#define MT_RXINFO_DATA                      (1U << 1)
#define MT_RXINFO_NULL                      (1U << 2)
#define MT_RXINFO_FRAG                      (1U << 3)
#define MT_RXINFO_UNICAST                   (1U << 4)
#define MT_RXINFO_MULTICAST                 (1U << 5)
#define MT_RXINFO_BROADCAST                 (1U << 6)
#define MT_RXINFO_MYBSS                     (1U << 7)
#define MT_RXINFO_CRCERR                    (1U << 8)
#define MT_RXINFO_ICVERR                    (1U << 9)
#define MT_RXINFO_MICERR                    (1U << 10)
#define MT_RXINFO_AMSDU                     (1U << 11)
#define MT_RXINFO_HTC                       (1U << 12)
#define MT_RXINFO_RSSI                      (1U << 13)
#define MT_RXINFO_L2PAD                     (1U << 14)
#define MT_RXINFO_AMPDU                     (1U << 15)
#define MT_RXINFO_DECRYPT                   (1U << 16)
#define MT_RXINFO_PN_LEN                    (7U << 19)
#define MT_RXINFO_PROBE_RESP                (1U << 24)
#define MT_RXINFO_BEACON                    (1U << 25)

/* RXWI word1 (ctl) fields */
#define MT_RXWI_CTL_WCID_MASK               0x00FF
#define MT_RXWI_CTL_MPDU_LEN_MASK           (0x3FFFU << 16)
#define MT_RXWI_CTL_MPDU_LEN_SHIFT          16
#define MT_RXWI_CTL_EOF                     (1U << 31)

/* RXWI size: 32 bytes */
#define MT_RXWI_SIZE                        32

#define __mt76_put_le32(_p, _v)             do { \
        (_p)[0] = (uint8_t)(_v);                \
        (_p)[1] = (uint8_t)((_v) >> 8);         \
        (_p)[2] = (uint8_t)((_v) >> 16);        \
        (_p)[3] = (uint8_t)((_v) >> 24);        \
    } while (0)
#define __mt76_get_le32(_p)                 \
    (   (uint32_t)(_p)[0]                   \
      | ((uint32_t)(_p)[1] << 8)            \
      | ((uint32_t)(_p)[2] << 16)           \
      | ((uint32_t)(_p)[3] << 24))
#define __mt76_get_le16(_p)                 \
    (   (uint16_t)(_p)[0]                   \
      | ((uint16_t)(_p)[1] << 8))
#define __mt76_put_le16(_p, _v)             do { \
        (_p)[0] = (uint8_t)(_v);                \
        (_p)[1] = (uint8_t)((_v) >> 8);         \
    } while (0)
#define __mt76_round_up(_x, _n)             (((_x) + ((_n) - 1)) & ~((_n) - 1))
#define __mt76_min(_a, _b)                  ((_a) < (_b) ? (_a) : (_b))
#define __mt76_rev(_priv)                   ((uint16_t)((_priv)->asic_rev & 0xFFFF))

/* Protection config values from Linux mt76x2/init.c */
#define MT76_PROT_CFG_RATE(_v)              (((_v) & 0x7fff) << 0)
#define MT76_PROT_CFG_NAV(_v)               (((_v) & 0x1) << 15)
#define MT76_PROT_CFG_CTRL(_v)              (((_v) & 0x1) << 16)
#define MT76_PROT_CFG_TXOP_ALLOW(_v)        (((_v) & 0x3f) << 17)
#define MT76_PROT_CFG_RTS_THRESH            (1U << 23)

#define MT76_DEFAULT_PROT_CFG_CCK           \
    (MT76_PROT_CFG_RATE(0x3) | MT76_PROT_CFG_NAV(1) | \
     MT76_PROT_CFG_TXOP_ALLOW(0x3f) | MT76_PROT_CFG_RTS_THRESH)
#define MT76_DEFAULT_PROT_CFG_OFDM          \
    (MT76_PROT_CFG_RATE(0x2004) | MT76_PROT_CFG_NAV(1) | \
     MT76_PROT_CFG_TXOP_ALLOW(0x3f) | MT76_PROT_CFG_RTS_THRESH)
#define MT76_DEFAULT_PROT_CFG_20            \
    (MT76_PROT_CFG_RATE(0x2004) | MT76_PROT_CFG_CTRL(1) | \
     MT76_PROT_CFG_NAV(1) | MT76_PROT_CFG_TXOP_ALLOW(0x17))
#define MT76_DEFAULT_PROT_CFG_40            \
    (MT76_PROT_CFG_RATE(0x2084) | MT76_PROT_CFG_CTRL(1) | \
     MT76_PROT_CFG_NAV(1) | MT76_PROT_CFG_TXOP_ALLOW(0x3f))

/*============================ TYPES =========================================*/

typedef struct mt76_reg_pair_t {
    uint32_t reg;
    uint32_t val;
} mt76_reg_pair_t;

/*============================ TYPES =========================================*/

typedef enum {
    MT76_STATE_IDLE = 0,
    MT76_STATE_INIT_READ_ASIC,
    MT76_STATE_INIT_MCU_Q_SELECT,
    MT76_STATE_INIT_MCU_RADIO_ON,
    MT76_STATE_INIT_EEPROM_LOAD,
    MT76_STATE_INIT_MAC_ADDR,
    MT76_STATE_INIT_USB_DMA,
    MT76_STATE_INIT_MAC_RESET,
    MT76_STATE_INIT_WCID_RESET,
    MT76_STATE_INIT_KEY_RESET,
    MT76_STATE_INIT_BEACON,
    MT76_STATE_INIT_MISC,
    MT76_STATE_INIT_LOAD_CR,
    MT76_STATE_INIT_PHY,
    MT76_STATE_INIT_MAC_STOP,
    MT76_STATE_INIT_READY,
} mt76_state_t;

typedef enum {
    MT76_FW_STATE_RESET_WLAN = 0,
    MT76_FW_STATE_RESET_WLAN_RF,
    MT76_FW_STATE_RESET_WLAN_EN,
    MT76_FW_STATE_POWER_ON_MTCMOS,
    MT76_FW_STATE_POWER_ON_MTCMOS_POLL,
    MT76_FW_STATE_POWER_ON_MTCMOS_CLEAR1,
    MT76_FW_STATE_POWER_ON_MTCMOS_CLEAR2,
    MT76_FW_STATE_POWER_ON_MTCMOS_SET2,
    MT76_FW_STATE_POWER_ON_MTCMOS_CLEAR3,
    MT76_FW_STATE_POWER_ON_AD_DA,
    MT76_FW_STATE_POWER_ON_AD_DA_WRITE,
    MT76_FW_STATE_POWER_ON_WLAN_EN,
    MT76_FW_STATE_POWER_ON_WLAN_EN_WRITE,
    MT76_FW_STATE_POWER_ON_BBP_RST,
    MT76_FW_STATE_POWER_ON_BBP_RST_WRITE,
    MT76_FW_STATE_POWER_ON_RF,
    MT76_FW_STATE_WAIT_MAC,
    MT76_FW_STATE_READ_ASIC_REV,
    MT76_FW_STATE_CHECK_PATCH,
    MT76_FW_STATE_ACQUIRE_SEM,
    MT76_FW_STATE_ENABLE_USB_DMA,
    MT76_FW_STATE_VENDOR_RESET_DELAY,
    MT76_FW_STATE_ENABLE_USB_DMA_CFG,
    MT76_FW_STATE_FCE_PSE,
    MT76_FW_STATE_FCE_BASE,
    MT76_FW_STATE_FCE_MAX,
    MT76_FW_STATE_FCE_PDMA,
    MT76_FW_STATE_FCE_SKIP,
    MT76_FW_STATE_UPLOAD,
    MT76_FW_STATE_ENABLE_PATCH,
    MT76_FW_STATE_RESET_WMT,
    MT76_FW_STATE_WAIT_WMT,
    MT76_FW_STATE_POLL_PATCH,
    MT76_FW_STATE_RELEASE_SEM,
    MT76_FW_STATE_PARSE_FW_HEADER,
    MT76_FW_STATE_LOAD_IVB,
    MT76_FW_STATE_POLL_FW_READY,
    MT76_FW_STATE_SET_COMREG,
    MT76_FW_STATE_REENABLE_FCE,
    MT76_FW_STATE_DONE,
} mt76_fw_state_t;

typedef enum {
    MT76_FW_STAGE_PATCH = 0,
    MT76_FW_STAGE_ILM,
    MT76_FW_STAGE_DLM,
} mt76_fw_stage_t;

typedef enum {
    MT76_FW_SEND_IDLE = 0,
    MT76_FW_SEND_CHUNK,
    MT76_FW_SEND_FCE_LEN,
    MT76_FW_SEND_BULK,
    MT76_FW_SEND_IDX_READ,
    MT76_FW_SEND_IDX_WRITE,
} mt76_fw_send_state_t;

/*============================ INLINE HELPERS ================================*/

#define __mt76_priv(__wifi)       ((mt76_wifi_priv_t *)((__wifi)->chip_priv))

extern const vsf_wifi_mt76_bus_ops_t *__mt76_bus_ops(vsf_wifi_t *wifi);

#define __mt76_cfg_read(__wifi, ...)      __mt76_bus_ops(__wifi)->base.reg_read(__wifi, __VA_ARGS__)
#define __mt76_cfg_write(__wifi, ...)     __mt76_bus_ops(__wifi)->base.reg_write(__wifi, __VA_ARGS__)
#define __mt76_fce_write(__wifi, ...)     __mt76_bus_ops(__wifi)->fce_write(__wifi, __VA_ARGS__)
#define __mt76_dev_cmd(__wifi, ...)       __mt76_bus_ops(__wifi)->dev_cmd(__wifi, __VA_ARGS__)
#define __mt76_dev_class_cmd(__wifi, ...) __mt76_bus_ops(__wifi)->dev_class_cmd(__wifi, __VA_ARGS__)
#define __mt76_mcu_cmd(__wifi, ...)       __mt76_bus_ops(__wifi)->mcu_cmd(__wifi, __VA_ARGS__)
#define __mt76_tx_frame(__wifi, ...)      __mt76_bus_ops(__wifi)->tx_frame(__wifi, __VA_ARGS__)
#define __mt76_rx_submit(__wifi, ...)     __mt76_bus_ops(__wifi)->rx_submit(__wifi, __VA_ARGS__)

#define __mt76_put_le32(_p, _v)             do { \
        (_p)[0] = (uint8_t)(_v);                \
        (_p)[1] = (uint8_t)((_v) >> 8);         \
        (_p)[2] = (uint8_t)((_v) >> 16);        \
        (_p)[3] = (uint8_t)((_v) >> 24);        \
    } while (0)
#define __mt76_get_le32(_p)                 \
    (   (uint32_t)(_p)[0]                   \
      | ((uint32_t)(_p)[1] << 8)            \
      | ((uint32_t)(_p)[2] << 16)           \
      | ((uint32_t)(_p)[3] << 24))
#define __mt76_get_le16(_p)                 \
    (   (uint16_t)(_p)[0]                   \
      | ((uint16_t)(_p)[1] << 8))
#define __mt76_put_le16(_p, _v)             do { \
        (_p)[0] = (uint8_t)(_v);                \
        (_p)[1] = (uint8_t)((_v) >> 8);         \
    } while (0)
#define __mt76_round_up(_x, _n)             (((_x) + ((_n) - 1)) & ~((_n) - 1))
#define __mt76_min(_a, _b)                  ((_a) < (_b) ? (_a) : (_b))
#define __mt76_rev(_priv)                   ((uint16_t)((_priv)->asic_rev & 0xFFFF))

/*============================ PROTOTYPES ====================================*/

/* EEPROM helpers */
extern uint16_t __mt76_eeprom_get_u16(mt76_wifi_priv_t *priv, uint16_t offset);
extern uint8_t  __mt76_eeprom_get_u8(mt76_wifi_priv_t *priv, uint16_t offset);
extern vsf_err_t __mt76_eeprom_load(vsf_wifi_t *wifi, vsf_wifi_done_t done);
extern const uint8_t *__mt76_eeprom_get_mac(vsf_wifi_t *wifi);
extern vsf_err_t __mt76_mac_addr_program_start(vsf_wifi_t *wifi,
                                               const uint8_t mac[6],
                                               vsf_wifi_done_t done);

/* PHY helpers */
extern vsf_err_t __mt76_apply_tx_power(vsf_wifi_t *wifi, uint8_t channel,
                                       uint8_t bw, vsf_wifi_done_t done);
extern void __mt76_read_rx_gain(vsf_wifi_t *wifi, uint8_t channel);

/* MCU helpers */
extern vsf_err_t __mt76_mcu_msg_send(vsf_wifi_t *wifi, uint8_t cmd,
    const uint8_t *payload, uint16_t payload_len, bool wait_resp,
    vsf_wifi_done_t done);

/* Firmware load */
extern vsf_err_t __mt76_firmware_load(vsf_wifi_t *wifi, vsf_wifi_done_t done);
extern bool __mt76_chip_id_valid(uint32_t asic_rev);

/* XTAL fixup (shared with firmware load timer) */
extern vsf_err_t __mt76_mac_fixup_xtal_start(vsf_wifi_t *wifi);
extern void __mt76_mac_fixup_xtal_continue(vsf_wifi_t *wifi, vsf_err_t err);
extern void __mt76_xtal_timer_start(vsf_wifi_t *wifi, uint32_t ms);

/* MAC layer */
extern vsf_err_t __mt76_init(vsf_wifi_t *wifi, vsf_wifi_done_t done);
extern void __mt76_fini(vsf_wifi_t *wifi);
extern vsf_err_t __mt76_set_channel(vsf_wifi_t *wifi, uint8_t channel,
                                    vsf_wifi_done_t done);
extern vsf_err_t __mt76_set_rx_filter(vsf_wifi_t *wifi, uint32_t mask,
                                      vsf_wifi_done_t done);
extern vsf_err_t __mt76_set_mac_addr(vsf_wifi_t *wifi, const uint8_t mac[6],
                                     vsf_wifi_done_t done);
extern vsf_err_t __mt76_set_bssid(vsf_wifi_t *wifi, const uint8_t bssid[6],
                                  vsf_wifi_done_t done);
extern vsf_err_t __mt76_set_auth_mode(vsf_wifi_t *wifi,
                                      const vsf_wifi_auth_cfg_t *cfg,
                                      vsf_wifi_done_t done);
extern vsf_err_t __mt76_connect(vsf_wifi_t *wifi,
                                const uint8_t bssid[6], const uint8_t *ssid,
                                uint8_t ssid_len, uint8_t channel,
                                vsf_wifi_done_t done);
extern vsf_err_t __mt76_disconnect(vsf_wifi_t *wifi, vsf_wifi_done_t done);
extern vsf_err_t __mt76_get_link_info(vsf_wifi_t *wifi,
                                      vsf_wifi_link_info_t *info);
extern void __mt76_parse_rx(vsf_wifi_t *wifi, uint8_t *frame, uint16_t len);
extern vsf_err_t __mt76_tx(vsf_wifi_t *wifi, const uint8_t *frame, uint16_t len);

#if (VSF_WIFI_USE_WPA == ENABLED)
extern const vsf_wifi_crypto_ops_t __mt76_crypto_ops;
#endif

#endif      /* VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_MT76 == ENABLED */
#endif      /* __VSF_WIFI_MT76_PRIV_H__ */
