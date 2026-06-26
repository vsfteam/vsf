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

#if VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_MT76 == ENABLED

#include "../../vsf_wifi_priv.h"

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
#define MT76_TX_ALC_CFG_0                   0x13b0
#define MT76_TX_ALC_VGA3                    0x0778
#define MT76_VHT_HT_FBK_CFG1                0x1358
#define MT76_CCK_PROT_CFG                   0x1364
#define MT76_OFDM_PROT_CFG                  0x1368
#define MT76_MM20_PROT_CFG                  0x136c
#define MT76_MM40_PROT_CFG                  0x1370
#define MT76_GF20_PROT_CFG                  0x1374
#define MT76_GF40_PROT_CFG                  0x1378
#define MT76_EXP_ACK_TIME                   0x1380

#define MT_TX_STAT_FIFO                     0x1718
#define MT_TX_STAT_FIFO_VALID               (1U << 0)
#define MT_TX_STAT_FIFO_SUCCESS             (1U << 5)
#define MT_TX_STAT_FIFO_AGGR                (1U << 6)
#define MT_TX_STAT_FIFO_ACKREQ              (1U << 7)
#define MT_TX_STAT_FIFO_WCID_M              0x0000FF00
#define MT_TX_STAT_FIFO_WCID_S              8
#define MT_TX_STAT_FIFO_RATE_M              0xFFFF0000
#define MT_TX_STAT_FIFO_RATE_S              16
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

#define MT76_WCID_TX_INFO_BASE              0xc000
#define MT76_WCID_TX_INFO(_n)               (MT76_WCID_TX_INFO_BASE + ((_n) * 4))

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
#define MT76_BBP_TXBE_BASE                  0x2700
#define MT76_BBP(_type, _n)                 (MT76_BBP_##_type##_BASE + ((_n) << 2))

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
#define MT76_EXT_CCA_CFG                    0x1260
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

static uint16_t __mt76_eeprom_get_u16(mt76_wifi_priv_t *priv, uint16_t offset)
{
    return (uint16_t)priv->eeprom[offset] |
           ((uint16_t)priv->eeprom[offset + 1] << 8);
}

#define MT76_CMD_SWITCH_CHANNEL_OP          30

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

typedef struct mt76_reg_pair_t {
    uint32_t reg;
    uint32_t val;
} mt76_reg_pair_t;

static const mt76_reg_pair_t __mt76_mac_initvals[] = {
    { MT76_PBF_SYS_CTRL,        0x00080c00 },
    { MT76_PBF_CFG,             0x1efebcff },
    { MT76_FCE_PSE_CTRL,        0x00000001 },
    { MT76_MAC_SYS_CTRL,        0x00000000 },
    { MT76_MAX_LEN_CFG,         0x003e3f00 },
    { MT76_AMPDU_MAX_LEN_20M1S, 0xaaa99887 },
    { MT76_AMPDU_MAX_LEN_20M2S, 0x000000aa },
    { MT76_XIFS_TIME_CFG,       0x33a40d0a },
    { MT76_BKOFF_SLOT_CFG,      0x00000209 },
    { MT76_TBTT_SYNC_CFG,       0x00422010 },
    { MT76_PWR_PIN_CFG,         0x00000000 },
    { 0x1238,                   0x001700c8 },
    { MT76_TX_SW_CFG0,          0x00101001 },
    { MT76_TX_SW_CFG1,          0x00010000 },
    { MT76_TX_SW_CFG2,          0x00000000 },
    { MT76_TXOP_CTRL_CFG,       0x0400583f },
    { MT76_TX_RTS_CFG,          0x00ffff20 },
    { MT76_TX_TIMEOUT_CFG,      0x000a2290 },
    { MT76_TX_RETRY_CFG,        0x47f01f0f },
    { MT76_EXP_ACK_TIME,        0x002c00dc },
    { MT76_TX_PROT_CFG6,        0xe3f42004 },
    { MT76_TX_PROT_CFG7,        0xe3f42084 },
    { MT76_TX_PROT_CFG8,        0xe3f42104 },
    { MT76_PIFS_TX_CFG,         0x00060fff },
    { MT_RX_FILTR_CFG,          0x00015f97 },
    { MT76_LEGACY_BASIC_RATE,   0x0000017f },
    { MT76_HT_BASIC_RATE,       0x00004003 },
    { MT76_PN_PAD_MODE,         0x00000003 },
    { MT76_TXOP_HLDR_ET,        0x00000002 },
    { 0x0a44,                   0x00000000 },
    { MT76_HEADER_TRANS_CTRL_REG, 0x00000000 },
    { MT76_TSO_CTRL,            0x00000000 },
    { MT76_AUX_CLK_CFG,         0x00000000 },
    { MT76_DACCLK_EN_DLY_CFG,   0x00000000 },
    { MT76_TX_ALC_CFG_4,        0x00000000 },
    { MT76_TX_ALC_VGA3,         0x00000000 },
    { MT76_TX_ALC_CFG_0,        0x3a3a3a3a },
    { MT76_TX_PWR_CFG_0,        0x3a3a3a3a },
    { MT76_TX_PWR_CFG_1,        0x3a3a3a3a },
    { MT76_TX_PWR_CFG_2,        0x3a3a3a3a },
    { MT76_TX_PWR_CFG_3,        0x3a3a3a3a },
    { MT76_TX_PWR_CFG_4,        0x3a3a3a3a },
    { MT76_TX_PWR_CFG_7,        0x3a3a3a3a },
    { MT76_TX_PWR_CFG_8,        0x0000003a },
    { MT76_TX_PWR_CFG_9,        0x0000003a },
    { MT76_EFUSE_CTRL,          0x0000d000 },
    { MT76_PAUSE_ENABLE_CONTROL1, 0x0000000a },
    { MT76_FCE_WLAN_FLOW_CONTROL1, 0x60401c18 },
    { MT76_WPDMA_DELAY_INT_CFG, 0x94ff0000 },
    { MT76_TX_SW_CFG3,          0x00000004 },
    { MT76_HT_FBK_TO_LEGACY,    0x00001818 },
    { MT76_VHT_HT_FBK_CFG1,     0xedcba980 },
    { MT76_PROT_AUTO_TX_CFG,    0x00830083 },
    { MT76_HT_CTRL_CFG,         0x000001ff },
    { MT76_TX_LINK_CFG,         0x00001020 },
};

static const mt76_reg_pair_t __mt76_prot_initvals[] = {
    { MT76_CCK_PROT_CFG,  MT76_DEFAULT_PROT_CFG_CCK },
    { MT76_OFDM_PROT_CFG, MT76_DEFAULT_PROT_CFG_OFDM },
    { MT76_MM20_PROT_CFG, MT76_DEFAULT_PROT_CFG_20 },
    { MT76_MM40_PROT_CFG, MT76_DEFAULT_PROT_CFG_40 },
    { MT76_GF20_PROT_CFG, MT76_DEFAULT_PROT_CFG_20 },
    { MT76_GF40_PROT_CFG, MT76_DEFAULT_PROT_CFG_40 },
};

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

/*============================ LOCAL VARIABLES ===============================*/

/*============================ FORWARD DECLARATIONS ==========================*/

static mt76_wifi_priv_t *__mt76_priv(vsf_wifi_t *wifi);
static const vsf_wifi_mt76_bus_ops_t *__mt76_bus_ops(vsf_wifi_t *wifi);

static void __mt76_init_next(vsf_wifi_t *wifi, vsf_err_t err);

static void __mt76_fw_next(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_finish(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_reset_wlan_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_power_on_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_wait_mac_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_power_on_rf_continue(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_power_on_rf_step(vsf_wifi_t *wifi);
static void __mt76_fw_read_asic_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_check_patch_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_acquire_sem_start(vsf_wifi_t *wifi);
static void __mt76_fw_acquire_sem_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_release_sem(vsf_wifi_t *wifi);
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

#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
static void __mt76_fw_timer(vsf_callback_timer_t *timer);
static void __mt76_fw_timer_start(vsf_wifi_t *wifi, uint32_t ms);
static void __mt76_xtal_timer(vsf_callback_timer_t *timer);
static void __mt76_xtal_timer_start(vsf_wifi_t *wifi, uint32_t ms);
#endif

static vsf_err_t __mt76_mac_fixup_xtal_start(vsf_wifi_t *wifi);
static void __mt76_mac_fixup_xtal_continue(vsf_wifi_t *wifi, vsf_err_t err);

static vsf_err_t __mt76_mcu_msg_send(vsf_wifi_t *wifi, uint8_t cmd,
    const uint8_t *payload, uint16_t payload_len, bool wait_resp,
    vsf_wifi_done_t done);
static void __mt76_mcu_msg_out_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_on_rx(vsf_wifi_t *wifi, uint8_t *buf, uint16_t len);
static void __mt76_on_rx_pkt(vsf_wifi_t *wifi, uint8_t *buf, uint16_t len);

/*============================ PROTOTYPES ====================================*/

extern vsf_err_t __mt76_firmware_load(vsf_wifi_t *wifi, vsf_wifi_done_t done);
extern vsf_err_t __mt76_init(vsf_wifi_t *wifi, vsf_wifi_done_t done);
extern void __mt76_fini(vsf_wifi_t *wifi);
extern vsf_err_t __mt76_eeprom_load(vsf_wifi_t *wifi, vsf_wifi_done_t done);
extern const uint8_t *__mt76_eeprom_get_mac(vsf_wifi_t *wifi);
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

/*============================ IMPLEMENTATION ================================*/

static mt76_wifi_priv_t *__mt76_priv(vsf_wifi_t *wifi)
{
    return (mt76_wifi_priv_t *)wifi->chip_priv;
}

static bool __mt76_chip_id_valid(uint32_t asic_rev);

static const vsf_wifi_mt76_bus_ops_t *__mt76_bus_ops(vsf_wifi_t *wifi)
{
    VSF_WIFI_ASSERT(wifi->bus_ops != NULL);
    return (const vsf_wifi_mt76_bus_ops_t *)wifi->bus_ops;
}

#define __mt76_cfg_read(__wifi, ...)      __mt76_bus_ops(__wifi)->base.reg_read(__wifi, __VA_ARGS__)
#define __mt76_cfg_write(__wifi, ...)     __mt76_bus_ops(__wifi)->base.reg_write(__wifi, __VA_ARGS__)
#define __mt76_fce_write(__wifi, ...)     __mt76_bus_ops(__wifi)->fce_write(__wifi, __VA_ARGS__)
#define __mt76_dev_cmd(__wifi, ...)       __mt76_bus_ops(__wifi)->dev_cmd(__wifi, __VA_ARGS__)
#define __mt76_dev_class_cmd(__wifi, ...) __mt76_bus_ops(__wifi)->dev_class_cmd(__wifi, __VA_ARGS__)
#define __mt76_mcu_cmd(__wifi, ...)       __mt76_bus_ops(__wifi)->mcu_cmd(__wifi, __VA_ARGS__)
#define __mt76_tx_frame(__wifi, ...)      __mt76_bus_ops(__wifi)->tx_frame(__wifi, __VA_ARGS__)
#define __mt76_rx_submit(__wifi, ...)     __mt76_bus_ops(__wifi)->rx_submit(__wifi, __VA_ARGS__)

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

static vsf_err_t __mt76_mcu_msg_send(vsf_wifi_t *wifi, uint8_t cmd,
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

    uint8_t seq = ++priv->mcu_seq;
    if (seq == 0) {
        seq = ++priv->mcu_seq;
    }
    seq &= 0x0F;

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

static void __mt76_xtal_timer_start(vsf_wifi_t *wifi, uint32_t ms)
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

static void __mt76_xtal_timer_start(vsf_wifi_t *wifi, uint32_t ms)
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

static bool __mt76_chip_id_valid(uint32_t asic_rev)
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

/*============================ EEPROM / MAC address ==========================*/

static void __mt76_eeprom_load_continue(vsf_wifi_t *wifi, vsf_err_t err);

vsf_err_t __mt76_eeprom_load_start(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    priv->eeprom_offset = 0;
    priv->eeprom_step   = 0;
    priv->eeprom_done   = done;

    uint32_t addr = priv->eeprom_offset & ~0xF;
    uint32_t val  = MT_EFUSE_CTRL_KICK |
                    ((uint32_t)MT_EE_READ << 6) |
                    (addr << 16);

    vsf_err_t step_err = __mt76_cfg_write(wifi, MT_EFUSE_CTRL, val,
                                          __mt76_eeprom_load_continue);
    if (step_err != VSF_ERR_NONE) {
        priv->eeprom_done = NULL;
        if (done != NULL) done(wifi, step_err);
    }
    return step_err;
}

static void __mt76_eeprom_load_continue(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_wifi_done_t done = priv->eeprom_done;

    if (err != VSF_ERR_NONE) {
        priv->eeprom_done = NULL;
        if (done != NULL) done(wifi, err);
        return;
    }

    switch (priv->eeprom_step) {
    case 0:
        priv->eeprom_step = 1;
        err = __mt76_cfg_read(wifi, MT_EFUSE_DATA(0), &priv->eeprom_data[0],
                              __mt76_eeprom_load_continue);
        break;
    case 1:
        priv->eeprom_step = 2;
        err = __mt76_cfg_read(wifi, MT_EFUSE_DATA(1), &priv->eeprom_data[1],
                              __mt76_eeprom_load_continue);
        break;
    case 2:
        priv->eeprom_step = 3;
        err = __mt76_cfg_read(wifi, MT_EFUSE_DATA(2), &priv->eeprom_data[2],
                              __mt76_eeprom_load_continue);
        break;
    case 3:
        priv->eeprom_step = 4;
        err = __mt76_cfg_read(wifi, MT_EFUSE_DATA(3), &priv->eeprom_data[3],
                              __mt76_eeprom_load_continue);
        break;
    case 4: {
        uint8_t *dst = &priv->eeprom[priv->eeprom_offset];
        for (int i = 0; i < 4; i++) {
            uint32_t val = priv->eeprom_data[i];
            dst[4 * i + 0] = (uint8_t)(val >> 0);
            dst[4 * i + 1] = (uint8_t)(val >> 8);
            dst[4 * i + 2] = (uint8_t)(val >> 16);
            dst[4 * i + 3] = (uint8_t)(val >> 24);
        }

        priv->eeprom_offset += 16;
        if (priv->eeprom_offset >= MT76_EEPROM_SIZE) {
            priv->eeprom_done = NULL;
            if (done != NULL) done(wifi, VSF_ERR_NONE);
            return;
        }

        priv->eeprom_step = 0;
        uint32_t addr = priv->eeprom_offset & ~0xF;
        uint32_t val  = MT_EFUSE_CTRL_KICK |
                        ((uint32_t)MT_EE_READ << 6) |
                        (addr << 16);
        err = __mt76_cfg_write(wifi, MT_EFUSE_CTRL, val,
                               __mt76_eeprom_load_continue);
        break;
    }
    default:
        err = VSF_ERR_BUG;
        break;
    }

    if (err != VSF_ERR_NONE) {
        priv->eeprom_done = NULL;
        if (done != NULL) done(wifi, err);
    }
}

static void __mt76_mac_addr_program_continue(vsf_wifi_t *wifi, vsf_err_t err);

static vsf_err_t __mt76_mac_addr_program_start(vsf_wifi_t *wifi,
                                               const uint8_t mac[6],
                                               vsf_wifi_done_t done)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    memcpy(priv->mac_addr, mac, 6);
    priv->mac_addr_step = 0;
    priv->mac_addr_done = done;

    uint32_t dw0 = ((uint32_t)mac[0] << 0) |
                   ((uint32_t)mac[1] << 8) |
                   ((uint32_t)mac[2] << 16) |
                   ((uint32_t)mac[3] << 24);

    vsf_err_t step_err = __mt76_cfg_write(wifi, MT_MAC_ADDR_DW0, dw0,
                                          __mt76_mac_addr_program_continue);
    if (step_err != VSF_ERR_NONE) {
        priv->mac_addr_done = NULL;
        return step_err;
    }
    return VSF_ERR_NONE;
}

static void __mt76_mac_addr_program_continue(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_wifi_done_t done = priv->mac_addr_done;

    if (err != VSF_ERR_NONE) {
        priv->mac_addr_done = NULL;
        if (done != NULL) done(wifi, err);
        return;
    }

    const uint8_t *mac = priv->mac_addr;
    uint32_t dw1;
    vsf_err_t step_err = VSF_ERR_NONE;

    switch (priv->mac_addr_step) {
    case 0:
        dw1 = ((uint32_t)mac[4] << 0) |
              ((uint32_t)mac[5] << 8) |
              MT_MAC_ADDR_DW1_U2ME_MASK;
        priv->mac_addr_step = 1;
        step_err = __mt76_cfg_write(wifi, MT_MAC_ADDR_DW1, dw1,
                                    __mt76_mac_addr_program_continue);
        break;
    case 1:
        dw1 = ((uint32_t)mac[0] << 0) |
              ((uint32_t)mac[1] << 8) |
              ((uint32_t)mac[2] << 16) |
              ((uint32_t)mac[3] << 24);
        priv->mac_addr_step = 2;
        step_err = __mt76_cfg_write(wifi, MT_MAC_BSSID_DW0, dw1,
                                    __mt76_mac_addr_program_continue);
        break;
    case 2:
        dw1 = ((uint32_t)mac[4] << 0) |
              ((uint32_t)mac[5] << 8) |
              MT_MAC_BSSID_DW1_MBSS_MODE |
              MT_MAC_BSSID_DW1_MBSS_LOCAL_BIT |
              (7U << MT_MAC_BSSID_DW1_MBEACON_N_SHIFT);
        priv->mac_addr_step = 3;
        step_err = __mt76_cfg_write(wifi, MT_MAC_BSSID_DW1, dw1,
                                    __mt76_mac_addr_program_continue);
        break;
    case 3:
        priv->mac_addr_done = NULL;
        if (done != NULL) done(wifi, VSF_ERR_NONE);
        return;
    default:
        step_err = VSF_ERR_BUG;
        break;
    }

    if (step_err != VSF_ERR_NONE) {
        priv->mac_addr_done = NULL;
        if (done != NULL) done(wifi, step_err);
    }
}

vsf_err_t __mt76_eeprom_load(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    return __mt76_eeprom_load_start(wifi, done);
}

void __mt76_eeprom_parse_hw_cap(vsf_wifi_t *wifi)
{
    /* TODO: parse TX/RX path, PA/LNA, etc. */
    (void)wifi;
}

const uint8_t *__mt76_eeprom_get_mac(vsf_wifi_t *wifi)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    return &priv->eeprom[MT76_EE_MAC_ADDR];
}

static void __mt76_init_mac_reset_reenable_dma_done(vsf_wifi_t *wifi, vsf_err_t err);

static void __mt76_mac_fixup_dma_read_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) {
        __mt76_init_next(wifi, err);
        return;
    }
    /* Preserve all current USB DMA bits and only flip the ones Linux
     * init_dma toggles.  This keeps the lower-byte aggregation timeout and
     * EP_OUT_VALID mask intact while ensuring TX/RX bulk and drop-or-pad
     * are enabled and aggregation is disabled. */
    uint32_t val = priv->fw_idx;
    val |= MT76_USB_DMA_CFG_RX_DROP_OR_PAD |
           MT76_USB_DMA_CFG_RX_BULK_EN |
           MT76_USB_DMA_CFG_TX_BULK_EN;
    val &= ~MT76_USB_DMA_CFG_RX_BULK_AGG_EN;
    vsf_err_t step_err = __mt76_cfg_write(wifi, MT76_USB_U3DMA_CFG, val,
                                          __mt76_init_mac_reset_reenable_dma_done);
    if (step_err != VSF_ERR_NONE) {
        __mt76_init_next(wifi, step_err);
    }
}

static void __mt76_mac_fixup_xtal_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) {
        __mt76_init_next(wifi, err);
        return;
    }
    /* Re-enable USB DMA after mac_reset with a read-modify-write so we do
     * not clobber device-specific bits (e.g. EP_OUT_VALID, aggregation
     * timeout) that the firmware already configured. */
    vsf_err_t step_err = __mt76_cfg_read(wifi, MT76_USB_U3DMA_CFG, &priv->fw_idx,
                                          __mt76_mac_fixup_dma_read_done);
    if (step_err != VSF_ERR_NONE) {
        __mt76_init_next(wifi, step_err);
    }
}

static void __mt76_init_mac_reset_reenable_dma_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) {
        __mt76_init_next(wifi, err);
        return;
    }
    priv->state = MT76_STATE_INIT_MAC_ADDR;
    priv->init_substate = 0;
    __mt76_init_next(wifi, VSF_ERR_NONE);
}

static vsf_err_t __mt76_mac_fixup_xtal_start(vsf_wifi_t *wifi)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    uint16_t eep_val = __mt76_eeprom_get_u16(priv, MT76_EE_XTAL_TRIM_2);
    int8_t offset = 0;

    offset = (int8_t)(eep_val & 0x7f);
    if ((eep_val & 0xff) == 0xff) {
        offset = 0;
    } else if (eep_val & 0x80) {
        offset = -offset;
    }

    eep_val >>= 8;
    if ((eep_val == 0) || (eep_val == 0xff)) {
        eep_val = __mt76_eeprom_get_u16(priv, MT76_EE_XTAL_TRIM_1) & 0xff;
        if ((eep_val == 0) || (eep_val == 0xff)) {
            eep_val = 0x14;
        }
    }

    eep_val &= 0x7f;
    priv->xtal_val = (uint16_t)(eep_val + offset);
    priv->xtal_step = 0;
    __mt76_mac_fixup_xtal_continue(wifi, VSF_ERR_NONE);
    return VSF_ERR_NONE;
}

static void __mt76_mac_fixup_xtal_continue(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_err_t step_err = VSF_ERR_NONE;

    if (err != VSF_ERR_NONE) {
        __mt76_mac_fixup_xtal_done(wifi, err);
        return;
    }

    switch (priv->xtal_step) {
    case 0: {
        priv->xtal_step = 1;
        step_err = __mt76_cfg_read(wifi,
            MT76_VEND_TYPE_CFG | MT76_XO_CTRL5, &priv->fw_idx,
            __mt76_mac_fixup_xtal_continue);
        break;
    }
    case 1: {
        uint32_t val = priv->fw_idx;
        val &= ~MT76_XO_CTRL5_C2_VAL_MASK;
        val |= ((uint32_t)priv->xtal_val & 0x7f) << MT76_XO_CTRL5_C2_VAL_SHIFT;
        priv->xtal_step = 2;
        step_err = __mt76_cfg_write(wifi,
            MT76_VEND_TYPE_CFG | MT76_XO_CTRL5, val,
            __mt76_mac_fixup_xtal_continue);
        break;
    }
    case 2: {
        priv->xtal_step = 3;
        step_err = __mt76_cfg_read(wifi,
            MT76_VEND_TYPE_CFG | MT76_XO_CTRL6, &priv->fw_idx,
            __mt76_mac_fixup_xtal_continue);
        break;
    }
    case 3: {
        uint32_t val = priv->fw_idx | MT76_XO_CTRL6_C2_CTRL_MASK;
        priv->xtal_step = 4;
        step_err = __mt76_cfg_write(wifi,
            MT76_VEND_TYPE_CFG | MT76_XO_CTRL6, val,
            __mt76_mac_fixup_xtal_continue);
        break;
    }
    case 4: {
        priv->xtal_step = 5;
        step_err = __mt76_cfg_write(wifi, MT76_RF_BYPASS_0, 0x06000000,
                                    __mt76_mac_fixup_xtal_continue);
        break;
    }
    case 5: {
        priv->xtal_step = 6;
        step_err = __mt76_cfg_write(wifi, MT76_RF_SETTING_0, 0x08800000,
                                    __mt76_mac_fixup_xtal_continue);
        break;
    }
    case 6: {
        priv->xtal_step = 7;
        __mt76_xtal_timer_start(wifi, 5);
        break;
    }
    case 7: {
        priv->xtal_step = 8;
        step_err = __mt76_cfg_write(wifi, MT76_RF_BYPASS_0, 0,
                                    __mt76_mac_fixup_xtal_continue);
        break;
    }
    case 8: {
        priv->xtal_step = 9;
        step_err = __mt76_cfg_read(wifi, MT76_XIFS_TIME_CFG, &priv->fw_idx,
                                   __mt76_mac_fixup_xtal_continue);
        break;
    }
    case 9: {
        uint32_t val = priv->fw_idx;
        val &= ~((uint32_t)0xFF << 8);
        val |= ((uint32_t)0x0D << 8);
        priv->xtal_step = 10;
        step_err = __mt76_cfg_write(wifi, MT76_XIFS_TIME_CFG, val,
                                    __mt76_mac_fixup_xtal_continue);
        break;
    }
    case 10: {
        priv->xtal_step = 11;
        step_err = __mt76_cfg_read(wifi, MT76_BKOFF_SLOT_CFG, &priv->fw_idx,
                                   __mt76_mac_fixup_xtal_continue);
        break;
    }
    case 11: {
        uint32_t val = priv->fw_idx;
        val &= ~((uint32_t)0x0F << 8);
        val |= ((uint32_t)0x01 << 8);
        priv->xtal_step = 12;
        step_err = __mt76_cfg_write(wifi, MT76_BKOFF_SLOT_CFG, val,
                                    __mt76_mac_fixup_xtal_continue);
        break;
    }
    case 12: {
        priv->xtal_step = 13;
        step_err = __mt76_cfg_read(wifi, MT76_FCE_L2_STUFF, &priv->fw_idx,
                                   __mt76_mac_fixup_xtal_continue);
        break;
    }
    case 13: {
        uint32_t val = priv->fw_idx & ~MT76_FCE_L2_STUFF_WR_MPDU_LEN_EN;
        priv->xtal_step = 14;
        step_err = __mt76_cfg_write(wifi, MT76_FCE_L2_STUFF, val,
                                    __mt76_mac_fixup_xtal_continue);
        break;
    }
    case 14: {
        uint16_t nic_conf2 = __mt76_eeprom_get_u16(priv, MT76_EE_NIC_CONF_2);
        uint32_t xo7;
        switch ((nic_conf2 >> 9) & 0x03) {
        case 1:
            xo7 = 0x5c1feed0;
            break;
        case 0:
        default:
            xo7 = 0x5c1fee80;
            break;
        }
        priv->xtal_step = 15;
        step_err = __mt76_cfg_write(wifi, MT76_XO_CTRL7, xo7,
                                    __mt76_mac_fixup_xtal_continue);
        break;
    }
    case 15:
        __mt76_mac_fixup_xtal_done(wifi, VSF_ERR_NONE);
        return;
    default:
        step_err = VSF_ERR_BUG;
        break;
    }

    if (step_err != VSF_ERR_NONE) {
        __mt76_mac_fixup_xtal_done(wifi, step_err);
    }
}

/*============================ Initialization ================================*/

static void __mt76_init_mcu_q_select_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) {
        __mt76_init_next(wifi, err);
        return;
    }
    priv->state = MT76_STATE_INIT_MCU_RADIO_ON;
    __mt76_init_next(wifi, VSF_ERR_NONE);
}

static void __mt76_init_ready_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_wifi_done_t done = priv->pending_done;

    priv->pending_done = NULL;
    if (done != NULL) done(wifi, err);
}

static void __mt76_init_ready(vsf_wifi_t *wifi)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    priv->state = MT76_STATE_INIT_READY;
    priv->on_rx = __mt76_on_rx;
    priv->on_rx_pkt = __mt76_on_rx_pkt;
    /* Enable MAC TX/RX so the firmware can deliver packets/beacons.
     * Linux does this in mt76x02u_mac_start after interface up. */
    vsf_err_t step_err = __mt76_cfg_write(wifi, MT76_MAC_SYS_CTRL,
        MT76_MAC_SYS_CTRL_ENABLE_TX | MT76_MAC_SYS_CTRL_ENABLE_RX,
        __mt76_init_ready_done);
    if (step_err != VSF_ERR_NONE) {
        __mt76_init_ready_done(wifi, step_err);
    }
}

static void __mt76_eeprom_load_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_mac_addr_done(vsf_wifi_t *wifi, vsf_err_t err);

static void __mt76_init_mcu_radio_on_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_wifi_done_t done = priv->pending_done;

    if (err != VSF_ERR_NONE) {
        priv->pending_done = NULL;
        if (done != NULL) done(wifi, err);
        return;
    }

    priv->state = MT76_STATE_INIT_EEPROM_LOAD;
    vsf_err_t step_err = __mt76_eeprom_load(wifi, __mt76_eeprom_load_done);
    if (step_err != VSF_ERR_NONE) {
        priv->pending_done = NULL;
        if (done != NULL) done(wifi, step_err);
    }
}

static void __mt76_eeprom_load_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_wifi_done_t init_done = priv->pending_done;

    if (err != VSF_ERR_NONE) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: failed to load EEPROM" VSF_TRACE_CFG_LINEEND);
        priv->pending_done = NULL;
        if (init_done != NULL) init_done(wifi, err);
        return;
    }

    const uint8_t *mac = __mt76_eeprom_get_mac(wifi);
    vsf_wifi_chip_mt76_trace_info(
        "mt76: EEPROM loaded, MAC=%02X:%02X:%02X:%02X:%02X:%02X" VSF_TRACE_CFG_LINEEND,
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    memcpy(wifi->mac, mac, 6);

    /* Default chainmask: MT76x2 (7612/7632/7662) is 2T2R, MT7602 is 1T1R. */
    {
        uint16_t chip_id = (uint16_t)(priv->asic_rev >> 16);
        if ((chip_id == MT76_CHIP_ID_7612) ||
            (chip_id == MT76_CHIP_ID_7632) ||
            (chip_id == MT76_CHIP_ID_7662)) {
            priv->chainmask = 0x0202;
        } else {
            priv->chainmask = 0x0101;
        }
    }

    /* Cache the RX filter value that mac_reset initvals will write. */
    priv->rxfilter = MT_RX_FILTR_CFG_DEFAULT;

    priv->state = MT76_STATE_INIT_USB_DMA;
    __mt76_init_next(wifi, VSF_ERR_NONE);
}

static void __mt76_init_mac_addr_wait_idle_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: mac_addr wait idle err=%d" VSF_TRACE_CFG_LINEEND, err);
        __mt76_init_next(wifi, err);
        return;
    }
    vsf_wifi_chip_mt76_trace_info(
        "mt76: mac_addr wait idle status=0x%08X idx=%u" VSF_TRACE_CFG_LINEEND,
        (unsigned)priv->fw_idx, priv->init_idx);
    if ((priv->fw_idx & (MT76_MAC_STATUS_TX | MT76_MAC_STATUS_RX)) == 0 ||
            priv->init_idx == 0) {
        priv->state = MT76_STATE_INIT_WCID_RESET;
        priv->init_substate = 0;
        __mt76_init_next(wifi, VSF_ERR_NONE);
        return;
    }
    priv->init_idx--;
    vsf_err_t step_err = __mt76_cfg_read(wifi, MT76_MAC_STATUS, &priv->fw_idx,
                                         __mt76_init_mac_addr_wait_idle_done);
    if (step_err != VSF_ERR_NONE) {
        __mt76_init_next(wifi, step_err);
    }
}

static void __mt76_mac_addr_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    if (err != VSF_ERR_NONE) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: failed to set MAC address" VSF_TRACE_CFG_LINEEND);
        vsf_wifi_done_t init_done = priv->pending_done;
        priv->pending_done = NULL;
        if (init_done != NULL) init_done(wifi, err);
        return;
    }

    /* Wait for MAC TX/RX to become idle after setting the MAC address,
     * matching Linux mt76x02_wait_for_txrx_idle() in init_hardware(). */
    priv->state = MT76_STATE_INIT_MAC_ADDR;
    priv->init_idx = 100;
    vsf_err_t step_err = __mt76_cfg_read(wifi, MT76_MAC_STATUS, &priv->fw_idx,
                                         __mt76_init_mac_addr_wait_idle_done);
    if (step_err != VSF_ERR_NONE) {
        __mt76_init_next(wifi, step_err);
    }
}

static void __mt76_init_usb_dma_read_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    if (err != VSF_ERR_NONE) {
        __mt76_init_next(wifi, err);
        return;
    }

    uint32_t val = priv->fw_idx;
    val |= MT76_USB_DMA_CFG_RX_DROP_OR_PAD |
           MT76_USB_DMA_CFG_RX_BULK_EN |
           MT76_USB_DMA_CFG_TX_BULK_EN;
    val &= ~MT76_USB_DMA_CFG_RX_BULK_AGG_EN;

    vsf_wifi_chip_mt76_trace_info(
        "mt76: init usb_dma cfg=0x%08X -> 0x%08X" VSF_TRACE_CFG_LINEEND,
        (unsigned)priv->fw_idx, (unsigned)val);

    priv->state = MT76_STATE_INIT_MAC_RESET;
    priv->init_substate = 0;
    vsf_err_t step_err = __mt76_cfg_write(wifi, MT76_USB_U3DMA_CFG, val,
                                          __mt76_init_next);
    if (step_err != VSF_ERR_NONE) {
        __mt76_init_next(wifi, step_err);
    }
}

static void __mt76_read_asic_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: failed to read ASIC version" VSF_TRACE_CFG_LINEEND);
        __mt76_init_next(wifi, err);
        return;
    }

    if (!__mt76_chip_id_valid(priv->asic_rev)) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: unsupported chip 0x%04X" VSF_TRACE_CFG_LINEEND,
            (unsigned)(priv->asic_rev >> 16));
        __mt76_init_next(wifi, VSF_ERR_NOT_SUPPORT);
        return;
    }

    vsf_wifi_chip_mt76_trace_info(
        "mt76: ASIC version = 0x%08X" VSF_TRACE_CFG_LINEEND,
        (unsigned)priv->asic_rev);

    priv->state = MT76_STATE_INIT_READ_ASIC;
    __mt76_init_next(wifi, VSF_ERR_NONE);
}

vsf_err_t __mt76_init(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    priv->pending_done = done;
    priv->state        = MT76_STATE_INIT_READ_ASIC;
    /* Install RX handlers early so that MCU command responses received
     * during the init sequence (e.g. LOAD_CR) are routed to the chip
     * driver instead of being dropped. */
    priv->on_rx        = __mt76_on_rx;
    priv->on_rx_pkt    = __mt76_on_rx_pkt;
    return __mt76_cfg_read(wifi, MT76_ASIC_VERSION,
                           &priv->asic_rev, __mt76_read_asic_done);
}

static void __mt76_init_next(vsf_wifi_t *wifi, vsf_err_t err);

static void __mt76_init_table_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) {
        __mt76_init_next(wifi, err);
        return;
    }
    priv->init_idx++;
    __mt76_init_next(wifi, VSF_ERR_NONE);
}

static void __mt76_init_mac_reset_substate_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) {
        __mt76_init_next(wifi, err);
        return;
    }
    priv->init_substate++;
    __mt76_init_next(wifi, VSF_ERR_NONE);
}

static void __mt76_init_wcid_reset_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) {
        __mt76_init_next(wifi, err);
        return;
    }
    priv->init_idx++;
    if (priv->init_idx >= 256) {
        __mt76_init_next(wifi, VSF_ERR_NONE);
        return;
    }
    vsf_err_t step_err = __mt76_cfg_write(wifi, MT76_WCID_ATTR(priv->init_idx),
                                          MT76_WCID_ATTR_BSS_IDX,
                                          __mt76_init_wcid_reset_done);
    if (step_err != VSF_ERR_NONE) {
        __mt76_init_next(wifi, step_err);
    }
}

static void __mt76_init_key_reset_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) {
        __mt76_init_next(wifi, err);
        return;
    }
    priv->init_idx2++;
    if (priv->init_idx2 >= 4) {
        priv->init_idx2 = 0;
        priv->init_idx++;
        if (priv->init_idx >= 16) {
            __mt76_init_next(wifi, VSF_ERR_NONE);
            return;
        }
    }
    vsf_err_t step_err = __mt76_cfg_write(wifi,
        MT76_SKEY(priv->init_idx, priv->init_idx2), 0,
        __mt76_init_key_reset_done);
    if (step_err != VSF_ERR_NONE) {
        __mt76_init_next(wifi, step_err);
    }
}

static void __mt76_init_beacon_offset_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) {
        __mt76_init_next(wifi, err);
        return;
    }
    priv->init_idx++;
    if (priv->init_idx >= 4) {
        __mt76_init_next(wifi, VSF_ERR_NONE);
        return;
    }
    uint32_t slot_size = (8192 / 16) & ~63;
    uint32_t offset = priv->init_idx * slot_size;
    vsf_err_t step_err = __mt76_cfg_write(wifi, MT76_BCN_OFFSET(priv->init_idx),
                                          offset,
                                          __mt76_init_beacon_offset_done);
    if (step_err != VSF_ERR_NONE) {
        __mt76_init_next(wifi, step_err);
    }
}

static void __mt76_init_txrx_idle_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: txrx idle err=%d" VSF_TRACE_CFG_LINEEND, err);
        __mt76_init_next(wifi, err);
        return;
    }
    vsf_wifi_chip_mt76_trace_info(
        "mt76: txrx idle poll status=0x%08X idx=%u" VSF_TRACE_CFG_LINEEND,
        (unsigned)priv->fw_idx, priv->init_idx);
    if ((priv->fw_idx & (MT76_MAC_STATUS_TX | MT76_MAC_STATUS_RX)) == 0 ||
            priv->init_idx == 0) {
        if (priv->state == MT76_STATE_INIT_MAC_RESET) {
            priv->state = MT76_STATE_INIT_MAC_ADDR;
        } else {
            priv->state = MT76_STATE_INIT_READY;
        }
        priv->init_substate = 0;
        __mt76_init_next(wifi, VSF_ERR_NONE);
        return;
    }
    priv->init_idx--;
    vsf_err_t step_err = __mt76_cfg_read(wifi, MT76_MAC_STATUS, &priv->fw_idx,
                                         __mt76_init_txrx_idle_done);
    if (step_err != VSF_ERR_NONE) {
        __mt76_init_next(wifi, step_err);
    }
}

static void __mt76_init_load_cr_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (err != VSF_ERR_NONE) {
        __mt76_init_next(wifi, err);
        return;
    }
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    priv->state = MT76_STATE_INIT_PHY;
    priv->init_substate = 0;
    vsf_wifi_chip_mt76_trace_info(
        "mt76: init phy start" VSF_TRACE_CFG_LINEEND);
    __mt76_init_next(wifi, VSF_ERR_NONE);
}

static void __mt76_init_next(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_wifi_done_t done = priv->pending_done;

    vsf_wifi_chip_mt76_trace_debug(
        "mt76: init_next state=%d err=%d" VSF_TRACE_CFG_LINEEND,
        (int)priv->state, (int)err);

    if (err != VSF_ERR_NONE) {
        priv->pending_done = NULL;
        if (done != NULL) done(wifi, err);
        return;
    }

    switch (priv->state) {
    case MT76_STATE_INIT_READ_ASIC: {
        uint8_t payload[8];
        __mt76_put_le32(payload + 0, MT76_FUN_Q_SELECT);
        __mt76_put_le32(payload + 4, 1);
        priv->state = MT76_STATE_INIT_MCU_Q_SELECT;
        vsf_err_t step_err = __mt76_mcu_msg_send(wifi, MT76_CMD_FUN_SET_OP,
                                    payload, sizeof(payload), false,
                                    __mt76_init_mcu_q_select_done);
        if (step_err != VSF_ERR_NONE) {
            __mt76_init_next(wifi, step_err);
        }
        break;
    }

    case MT76_STATE_INIT_MCU_RADIO_ON: {
        uint8_t payload[8];
        __mt76_put_le32(payload + 0, MT76_PWR_RADIO_ON);
        __mt76_put_le32(payload + 4, 0);
        priv->state = MT76_STATE_INIT_EEPROM_LOAD;
        vsf_err_t step_err = __mt76_mcu_msg_send(wifi, MT76_CMD_POWER_SAVING_OP,
                                    payload, sizeof(payload), false,
                                    __mt76_init_mcu_radio_on_done);
        if (step_err != VSF_ERR_NONE) {
            __mt76_init_next(wifi, step_err);
        }
        break;
    }

    case MT76_STATE_INIT_USB_DMA: {
        vsf_wifi_chip_mt76_trace_info(
            "mt76: init usb_dma" VSF_TRACE_CFG_LINEEND);
        vsf_err_t step_err = __mt76_cfg_read(wifi, MT76_USB_U3DMA_CFG,
                                             &priv->fw_idx,
                                             __mt76_init_usb_dma_read_done);
        if (step_err != VSF_ERR_NONE) {
            __mt76_init_next(wifi, step_err);
        }
        break;
    }

    case MT76_STATE_INIT_MAC_ADDR: {
        vsf_wifi_chip_mt76_trace_info(
            "mt76: init mac_addr" VSF_TRACE_CFG_LINEEND);
        vsf_err_t step_err = __mt76_set_mac_addr(wifi, wifi->mac,
                                                 __mt76_mac_addr_done);
        if (step_err != VSF_ERR_NONE) {
            __mt76_init_next(wifi, step_err);
        }
        break;
    }

    case MT76_STATE_INIT_MAC_RESET: {
        vsf_wifi_chip_mt76_trace_info(
            "mt76: init mac_reset sub=%u idx=%u" VSF_TRACE_CFG_LINEEND,
            priv->init_substate, priv->init_idx);
        switch (priv->init_substate) {
        case 0: {
            /* Reset WPDMA global configuration before reloading initvals. */
            vsf_err_t step_err = __mt76_cfg_write(wifi, MT76_WPDMA_GLO_CFG,
                                                  0x00000030,
                                                  __mt76_init_mac_reset_substate_done);
            if (step_err != VSF_ERR_NONE) {
                __mt76_init_next(wifi, step_err);
            }
            break;
        }

        case 1: {
            /* PBF max counts */
            static const mt76_reg_pair_t pbf_vals[] = {
                { MT76_PBF_TX_MAX_PCNT, 0xefef3f1f },
                { MT76_PBF_RX_MAX_PCNT, 0xfebf },
            };
            if (priv->init_idx < (sizeof(pbf_vals) / sizeof(pbf_vals[0]))) {
                vsf_err_t step_err = __mt76_cfg_write(wifi,
                    pbf_vals[priv->init_idx].reg,
                    pbf_vals[priv->init_idx].val,
                    __mt76_init_table_done);
                if (step_err != VSF_ERR_NONE) {
                    __mt76_init_next(wifi, step_err);
                }
                break;
            }
            priv->init_idx = 0;
            priv->init_substate = 2;
        }
        /* fall through */
        case 2: {
            /* main mac initvals */
            if (priv->init_idx < (sizeof(__mt76_mac_initvals) / sizeof(__mt76_mac_initvals[0]))) {
                vsf_err_t step_err = __mt76_cfg_write(wifi,
                    __mt76_mac_initvals[priv->init_idx].reg,
                    __mt76_mac_initvals[priv->init_idx].val,
                    __mt76_init_table_done);
                if (step_err != VSF_ERR_NONE) {
                    __mt76_init_next(wifi, step_err);
                }
                break;
            }
            priv->init_idx = 0;
            priv->init_substate = 3;
        }
        /* fall through */
        case 3: {
            /* protection initvals */
            if (priv->init_idx < (sizeof(__mt76_prot_initvals) / sizeof(__mt76_prot_initvals[0]))) {
                vsf_err_t step_err = __mt76_cfg_write(wifi,
                    __mt76_prot_initvals[priv->init_idx].reg,
                    __mt76_prot_initvals[priv->init_idx].val,
                    __mt76_init_table_done);
                if (step_err != VSF_ERR_NONE) {
                    __mt76_init_next(wifi, step_err);
                }
                break;
            }
            priv->init_idx = 0;
            priv->init_substate = 4;
        }
        /* fall through */
        case 4: {
            /* post-initval direct writes */
            static const mt76_reg_pair_t post_vals[] = {
                { MT76_TX_LINK_CFG,   0x00001020 },
                { MT76_AUTO_RSP_CFG,  0x00000013 },
                { MT76_MAX_LEN_CFG,   0x00002f00 },
                { MT76_WMM_AIFSN,     0x00002273 },
                { MT76_WMM_CWMIN,     0x00002344 },
                { MT76_WMM_CWMAX,     0x000034aa },
            };
            if (priv->init_idx < (sizeof(post_vals) / sizeof(post_vals[0]))) {
                vsf_err_t step_err = __mt76_cfg_write(wifi,
                    post_vals[priv->init_idx].reg,
                    post_vals[priv->init_idx].val,
                    __mt76_init_table_done);
                if (step_err != VSF_ERR_NONE) {
                    __mt76_init_next(wifi, step_err);
                }
                break;
            }
            priv->init_idx = 0;
            priv->init_substate = 5;
        }
        /* fall through */
        case 5: {
            /* De-assert MAC/BBP reset. */
            vsf_err_t step_err = __mt76_cfg_write(wifi, MT76_MAC_SYS_CTRL, 0,
                                                  __mt76_init_mac_reset_substate_done);
            if (step_err != VSF_ERR_NONE) {
                __mt76_init_next(wifi, step_err);
            }
            break;
        }

        case 6: {
            /* MT76x2-specific fix-ups inside mt76x2u_mac_reset(). */
            static const mt76_reg_pair_t fixup_vals[] = {
                { MT76_COEXCFG0,      0x00000000 },
                { MT76_EXT_CCA_CFG,   0x0000f000 },
                { MT76_TX_ALC_CFG_4,  0x00000000 },
            };
            if (priv->init_idx < (sizeof(fixup_vals) / sizeof(fixup_vals[0]))) {
                vsf_err_t step_err = __mt76_cfg_write(wifi,
                    fixup_vals[priv->init_idx].reg,
                    fixup_vals[priv->init_idx].val,
                    __mt76_init_table_done);
                if (step_err != VSF_ERR_NONE) {
                    __mt76_init_next(wifi, step_err);
                }
                break;
            }
            priv->init_idx = 0;
            priv->init_substate = 7;
        }
        /* fall through */
        case 7: {
            /* USB xtal fixup: handled by a dedicated sub-state machine. */
            vsf_err_t step_err = __mt76_mac_fixup_xtal_start(wifi);
            if (step_err != VSF_ERR_NONE) {
                __mt76_init_next(wifi, step_err);
            }
            break;
        }
        }
        break;
    }

    case MT76_STATE_INIT_WCID_RESET: {
        if (priv->init_idx >= 256) {
            priv->state = MT76_STATE_INIT_KEY_RESET;
            priv->init_idx = 0;
            priv->init_idx2 = 0;
            __mt76_init_next(wifi, VSF_ERR_NONE);
            break;
        }
        vsf_err_t step_err = __mt76_cfg_write(wifi,
                                              MT76_WCID_ATTR(priv->init_idx),
                                              MT76_WCID_ATTR_BSS_IDX,
                                              __mt76_init_wcid_reset_done);
        if (step_err != VSF_ERR_NONE) {
            __mt76_init_next(wifi, step_err);
        }
        break;
    }

    case MT76_STATE_INIT_KEY_RESET: {
        if (priv->init_idx >= 16) {
            priv->state = MT76_STATE_INIT_BEACON;
            priv->init_substate = 0;
            __mt76_init_next(wifi, VSF_ERR_NONE);
            break;
        }
        vsf_err_t step_err = __mt76_cfg_write(wifi,
                                              MT76_SKEY(priv->init_idx, priv->init_idx2),
                                              0,
                                              __mt76_init_key_reset_done);
        if (step_err != VSF_ERR_NONE) {
            __mt76_init_next(wifi, step_err);
        }
        break;
    }

    case MT76_STATE_INIT_BEACON: {
        switch (priv->init_substate) {
        case 0: {
            /* Disable beacon timer/TBTT, set sync mode */
            priv->init_substate = 1;
            vsf_err_t step_err = __mt76_cfg_write(wifi, MT76_BEACON_TIME_CFG,
                MT76_BEACON_TIME_CFG_SYNC_MODE,
                __mt76_init_table_done);
            if (step_err != VSF_ERR_NONE) {
                __mt76_init_next(wifi, step_err);
            }
            break;
        }

        case 1: {
            /* Beacon bypass mask */
            priv->init_idx = 0;
            priv->init_substate = 2;
            vsf_err_t step_err = __mt76_cfg_write(wifi, MT76_BCN_BYPASS_MASK,
                                                  0xffff,
                                                  __mt76_init_table_done);
            if (step_err != VSF_ERR_NONE) {
                __mt76_init_next(wifi, step_err);
            }
            break;
        }

        case 2: {
            /* BCN_OFFSET[0..3] */
            priv->init_substate = 3;
            vsf_err_t step_err = __mt76_cfg_write(wifi, MT76_BCN_OFFSET(0), 0,
                                                  __mt76_init_beacon_offset_done);
            if (step_err != VSF_ERR_NONE) {
                __mt76_init_next(wifi, step_err);
            }
            break;
        }

        case 3:
            priv->state = MT76_STATE_INIT_MISC;
            priv->init_substate = 0;
            vsf_wifi_chip_mt76_trace_info(
                "mt76: init misc start" VSF_TRACE_CFG_LINEEND);
            __mt76_init_next(wifi, VSF_ERR_NONE);
            break;
        }
        break;
    }

    case MT76_STATE_INIT_MISC: {
        static const mt76_reg_pair_t misc_vals[] = {
            { MT76_US_CYC_CFG,      0x1e },
            { MT76_TXOP_CTRL_CFG,   0x583f },
        };
        if (priv->init_idx < (sizeof(misc_vals) / sizeof(misc_vals[0]))) {
            vsf_err_t step_err = __mt76_cfg_write(wifi,
                misc_vals[priv->init_idx].reg,
                misc_vals[priv->init_idx].val,
                __mt76_init_table_done);
            if (step_err != VSF_ERR_NONE) {
                __mt76_init_next(wifi, step_err);
            }
            break;
        }
        priv->init_idx = 0;
        priv->state = MT76_STATE_INIT_LOAD_CR;
        vsf_wifi_chip_mt76_trace_info(
            "mt76: init load_cr start" VSF_TRACE_CFG_LINEEND);
        __mt76_init_next(wifi, VSF_ERR_NONE);
        break;
    }

    case MT76_STATE_INIT_LOAD_CR: {
        uint8_t payload[8];
        uint16_t nic_conf0 = __mt76_eeprom_get_u16(priv, MT76_EE_NIC_CONF_0);
        uint16_t nic_conf1 = __mt76_eeprom_get_u16(priv, MT76_EE_NIC_CONF_1);
        uint32_t cfg = (1U << 31) |
                       ((uint32_t)(nic_conf0 >> 8) & 0x00FF) |
                       (((uint32_t)nic_conf1 & 0x00FF) << 8);
        payload[0] = MT76_RF_BBP_CR;  /* cr_mode  */
        payload[1] = 0;               /* temp     */
        payload[2] = 0;               /* channel  */
        payload[3] = 0;               /* _pad0    */
        __mt76_put_le32(payload + 4, cfg);
        vsf_wifi_chip_mt76_trace_info(
            "mt76: load_cr cfg=0x%08X conf0=0x%04X conf1=0x%04X" VSF_TRACE_CFG_LINEEND,
            (unsigned)cfg, (unsigned)nic_conf0, (unsigned)nic_conf1);
        vsf_err_t step_err = __mt76_mcu_msg_send(wifi,
            MT76_MCU_MSG_CMD_LOAD_CR, payload, sizeof(payload), true,
            __mt76_init_load_cr_done);
        if (step_err != VSF_ERR_NONE) {
            __mt76_init_next(wifi, step_err);
        }
        break;
    }

    case MT76_STATE_INIT_PHY: {
        switch (priv->init_substate) {
        case 0: {
            priv->init_substate = 1;
            vsf_err_t step_err = __mt76_cfg_read(wifi, MT76_BBP(AGC, 0),
                                                 &priv->fw_idx,
                                                 __mt76_init_table_done);
            if (step_err != VSF_ERR_NONE) {
                __mt76_init_next(wifi, step_err);
            }
            break;
        }

        case 1: {
            /* mt76x02_phy_set_rxpath: clear bit 4, set/clear bit 3
             * according to the number of RX chains. */
            uint32_t val = priv->fw_idx & ~((1U << 4) | (1U << 3));
            if ((priv->chainmask & 0x0F) == 2) {
                val |= (1U << 3);
            }
            vsf_wifi_chip_mt76_trace_info(
                "mt76: phy_set_rxpath agc0=0x%08X -> 0x%08X mask=0x%04X"
                VSF_TRACE_CFG_LINEEND,
                (unsigned)priv->fw_idx, (unsigned)val,
                (unsigned)priv->chainmask);
            priv->init_substate = 2;
            vsf_err_t step_err = __mt76_cfg_write(wifi, MT76_BBP(AGC, 0), val,
                                                  __mt76_init_table_done);
            if (step_err != VSF_ERR_NONE) {
                __mt76_init_next(wifi, step_err);
            }
            break;
        }

        case 2: {
            priv->init_substate = 3;
            vsf_err_t step_err = __mt76_cfg_read(wifi, MT76_BBP(TXBE, 5),
                                                 &priv->fw_idx,
                                                 __mt76_init_table_done);
            if (step_err != VSF_ERR_NONE) {
                __mt76_init_next(wifi, step_err);
            }
            break;
        }

        case 3: {
            /* mt76x02_phy_set_txdac: set/clear bits [1:0] according to
             * the number of TX chains. */
            uint32_t val = priv->fw_idx & ~0x3U;
            if (((priv->chainmask >> 8) & 0x0F) == 2) {
                val |= 0x3U;
            }
            vsf_wifi_chip_mt76_trace_info(
                "mt76: phy_set_txdac txbe5=0x%08X -> 0x%08X mask=0x%04X"
                VSF_TRACE_CFG_LINEEND,
                (unsigned)priv->fw_idx, (unsigned)val,
                (unsigned)priv->chainmask);
            priv->state = MT76_STATE_INIT_MAC_STOP;
            priv->init_substate = 0;
            vsf_wifi_chip_mt76_trace_info(
                "mt76: init mac_stop start" VSF_TRACE_CFG_LINEEND);
            vsf_err_t step_err = __mt76_cfg_write(wifi, MT76_BBP(TXBE, 5), val,
                                                  __mt76_init_table_done);
            if (step_err != VSF_ERR_NONE) {
                __mt76_init_next(wifi, step_err);
            }
            break;
        }
        }
        break;
    }

    case MT76_STATE_INIT_MAC_STOP: {
        priv->init_idx = 100;
        vsf_err_t step_err = __mt76_cfg_read(wifi, MT76_MAC_STATUS,
                                             &priv->fw_idx,
                                             __mt76_init_txrx_idle_done);
        if (step_err != VSF_ERR_NONE) {
            __mt76_init_next(wifi, step_err);
        }
        break;
    }

    case MT76_STATE_INIT_READY:
        vsf_wifi_chip_mt76_trace_info(
            "mt76: init ready" VSF_TRACE_CFG_LINEEND);
        __mt76_init_ready(wifi);
        break;

    default:
        priv->pending_done = NULL;
        if (done != NULL) done(wifi, VSF_ERR_BUG);
        break;
    }
}

static void __mt76_mcu_wait_timeout(vsf_wifi_t *wifi)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_wifi_chip_mt76_trace_error(
        "mt76: mcu resp timeout, wanted seq=%u" VSF_TRACE_CFG_LINEEND,
        (unsigned)priv->mcu_wait_seq);
    priv->mcu_wait_resp = false;
    vsf_wifi_done_t done = priv->mcu_wait_done;
    priv->mcu_wait_done = NULL;
    if (done != NULL) done(wifi, VSF_ERR_FAIL);
}

static void __mt76_on_rx(vsf_wifi_t *wifi, uint8_t *buf, uint16_t len)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    if (!priv->mcu_wait_resp) {
        return;
    }

    if (len < 4) {
        /* RX URB timed out or completed with error while we were waiting. */
        __mt76_mcu_wait_timeout(wifi);
        return;
    }

    uint32_t info = __mt76_get_le32(buf);
    uint8_t evt = (uint8_t)((info >> 20) & 0x0F);
    uint8_t seq = (uint8_t)((info >> 16) & 0x0F);

    vsf_wifi_chip_mt76_trace_info(
        "mt76: mcu evt=0x%02X seq=%u len=%u wait=%d" VSF_TRACE_CFG_LINEEND,
        evt, seq, len, (int)priv->mcu_wait_resp);

    /* Commands that expect a response (LOAD_CR, SWITCH_CHANNEL_OP) complete
     * only after we drain CMD_RESP and see EVT_CMD_DONE for our sequence.
     * If a stale response arrives first, drop it and re-arm the RX URB so we
     * can receive the response for the current command. */
    if ((seq == priv->mcu_wait_seq) && (evt == 0)) {
        priv->mcu_wait_resp = false;
        vsf_wifi_done_t done = priv->mcu_wait_done;
        priv->mcu_wait_done = NULL;
        vsf_wifi_chip_mt76_trace_info(
            "mt76: mcu resp matched seq=%u" VSF_TRACE_CFG_LINEEND, seq);
        if (done != NULL) done(wifi, VSF_ERR_NONE);
        return;
    }

    /* Stale response: re-submit RX to keep draining the response queue. */
    vsf_wifi_chip_mt76_trace_info(
        "mt76: mcu resp stale seq=%u wanted=%u, re-arm rx" VSF_TRACE_CFG_LINEEND,
        seq, (unsigned)priv->mcu_wait_seq);
    vsf_err_t rx_err = __mt76_rx_submit(wifi, NULL, 0, MT76_EP_IN_CMD_RESP);
    if (rx_err != VSF_ERR_NONE) {
        __mt76_mcu_wait_timeout(wifi);
    }
}

static void __mt76_on_rx_pkt(vsf_wifi_t *wifi, uint8_t *buf, uint16_t len)
{
    __mt76_parse_rx(wifi, buf, len);
}

void __mt76_fini(vsf_wifi_t *wifi)
{
    /* TODO: stop TX/RX, power down */
    (void)wifi;
}

/*============================ WiFi hooks ====================================*/

/* mt76x2_mac_resume: re-enable MAC TX/RX after a channel switch. */
static void __mt76_mac_start_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_wifi_done_t done = priv->pending_done;
    priv->pending_done = NULL;
    vsf_wifi_chip_mt76_trace_info(
        "mt76: mac_start done err=%d" VSF_TRACE_CFG_LINEEND, (int)err);
    if (done != NULL) done(wifi, err);
}

static vsf_err_t __mt76_mac_start(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    priv->pending_done = done;
    vsf_wifi_chip_mt76_trace_debug(
        "mt76: mac_start" VSF_TRACE_CFG_LINEEND);
    return __mt76_cfg_write(wifi, MT76_MAC_SYS_CTRL,
        MT76_MAC_SYS_CTRL_ENABLE_TX | MT76_MAC_SYS_CTRL_ENABLE_RX,
        __mt76_mac_start_done);
}

/* mt76x2_mac_stop: halt MAC TX/RX before switching channel.
 * Uses priv->set_channel_substate as the state counter and
 * priv->set_channel_saved_rts to preserve TX_RTS_CFG. */
static void __mt76_mac_stop_done(vsf_wifi_t *wifi, vsf_err_t err);

static void __mt76_mac_stop_step(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    if (err != VSF_ERR_NONE) {
        __mt76_mac_stop_done(wifi, err);
        return;
    }

    switch (priv->set_channel_substate) {
    case 0: {
        vsf_err_t step_err = __mt76_cfg_read(wifi, MT76_TXOP_CTRL_CFG,
                                             &priv->fw_idx,
                                             __mt76_mac_stop_step);
        priv->set_channel_substate = 1;
        if (step_err != VSF_ERR_NONE) {
            __mt76_mac_stop_done(wifi, step_err);
        }
        break;
    }

    case 1: {
        uint32_t val = priv->fw_idx & ~MT76_TXOP_ED_CCA_EN;
        vsf_err_t step_err = __mt76_cfg_write(wifi, MT76_TXOP_CTRL_CFG, val,
                                              __mt76_mac_stop_step);
        priv->set_channel_substate = 2;
        if (step_err != VSF_ERR_NONE) {
            __mt76_mac_stop_done(wifi, step_err);
        }
        break;
    }

    case 2: {
        vsf_err_t step_err = __mt76_cfg_read(wifi, MT76_TXOP_HLDR_ET,
                                             &priv->fw_idx,
                                             __mt76_mac_stop_step);
        priv->set_channel_substate = 3;
        if (step_err != VSF_ERR_NONE) {
            __mt76_mac_stop_done(wifi, step_err);
        }
        break;
    }

    case 3: {
        uint32_t val = priv->fw_idx & ~MT76_TXOP_HLDR_TX40M_BLK_EN;
        vsf_err_t step_err = __mt76_cfg_write(wifi, MT76_TXOP_HLDR_ET, val,
                                              __mt76_mac_stop_step);
        priv->set_channel_substate = 4;
        if (step_err != VSF_ERR_NONE) {
            __mt76_mac_stop_done(wifi, step_err);
        }
        break;
    }

    case 4: {
        vsf_err_t step_err = __mt76_cfg_write(wifi, MT76_MAC_SYS_CTRL, 0,
                                              __mt76_mac_stop_step);
        priv->set_channel_substate = 5;
        if (step_err != VSF_ERR_NONE) {
            __mt76_mac_stop_done(wifi, step_err);
        }
        break;
    }

    case 5: {
        vsf_err_t step_err = __mt76_cfg_read(wifi, MT76_TX_RTS_CFG,
                                             &priv->set_channel_saved_rts,
                                             __mt76_mac_stop_step);
        priv->set_channel_substate = 6;
        if (step_err != VSF_ERR_NONE) {
            __mt76_mac_stop_done(wifi, step_err);
        }
        break;
    }

    case 6: {
        uint32_t val = priv->set_channel_saved_rts &
                       ~MT76_TX_RTS_CFG_RETRY_LIMIT;
        vsf_err_t step_err = __mt76_cfg_write(wifi, MT76_TX_RTS_CFG, val,
                                              __mt76_mac_stop_step);
        priv->set_channel_substate = 7;
        if (step_err != VSF_ERR_NONE) {
            __mt76_mac_stop_done(wifi, step_err);
        }
        break;
    }

    case 7: {
        /* Start polling: init_idx is the retry budget (max 300). */
        priv->init_idx = 300;
        vsf_err_t step_err = __mt76_cfg_read(wifi, MT76_MAC_STATUS,
                                             &priv->fw_idx,
                                             __mt76_mac_stop_step);
        priv->set_channel_substate = 8;
        if (step_err != VSF_ERR_NONE) {
            __mt76_mac_stop_done(wifi, step_err);
        }
        break;
    }

    case 8: {
        if ((priv->fw_idx & (MT76_MAC_STATUS_TX | MT76_MAC_STATUS_RX)) != 0) {
            /* MAC still active; continue polling or time out. */
            if (priv->init_idx == 0) {
                vsf_wifi_chip_mt76_trace_info(
                    "mt76: mac_stop tx/rx active timeout"
                    VSF_TRACE_CFG_LINEEND);
                __mt76_mac_stop_done(wifi, VSF_ERR_FAIL);
                return;
            }
            priv->init_idx--;
            vsf_err_t step_err = __mt76_cfg_read(wifi, MT76_MAC_STATUS,
                                                 &priv->fw_idx,
                                                 __mt76_mac_stop_step);
            if (step_err != VSF_ERR_NONE) {
                __mt76_mac_stop_done(wifi, step_err);
            }
            return;
        }
        /* MAC TX/RX idle; check BBP(IBI,12). */
        vsf_err_t step_err = __mt76_cfg_read(wifi, MT76_BBP(IBI, 12),
                                             &priv->fw_idx,
                                             __mt76_mac_stop_step);
        priv->set_channel_substate = 9;
        if (step_err != VSF_ERR_NONE) {
            __mt76_mac_stop_done(wifi, step_err);
        }
        break;
    }

    case 9: {
        if (priv->fw_idx != 0) {
            /* BBP busy; continue polling from MAC_STATUS or time out. */
            if (priv->init_idx == 0) {
                vsf_wifi_chip_mt76_trace_info(
                    "mt76: mac_stop bbp busy timeout"
                    VSF_TRACE_CFG_LINEEND);
                __mt76_mac_stop_done(wifi, VSF_ERR_FAIL);
                return;
            }
            priv->init_idx--;
            priv->set_channel_substate = 8;
            vsf_err_t step_err = __mt76_cfg_read(wifi, MT76_MAC_STATUS,
                                                 &priv->fw_idx,
                                                 __mt76_mac_stop_step);
            if (step_err != VSF_ERR_NONE) {
                __mt76_mac_stop_done(wifi, step_err);
            }
            return;
        }
        /* MAC fully stopped: restore RTS config and finish. */
        vsf_err_t step_err = __mt76_cfg_write(wifi, MT76_TX_RTS_CFG,
                                              priv->set_channel_saved_rts,
                                              __mt76_mac_stop_done);
        if (step_err != VSF_ERR_NONE) {
            __mt76_mac_stop_done(wifi, step_err);
        }
        break;
    }
    }
}

static void __mt76_mac_stop_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_wifi_done_t done = priv->pending_done;
    priv->pending_done = NULL;
    vsf_wifi_chip_mt76_trace_info(
        "mt76: mac_stop done err=%d" VSF_TRACE_CFG_LINEEND, (int)err);
    if (done != NULL) done(wifi, err);
}

static vsf_err_t __mt76_mac_stop(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    priv->pending_done = done;
    priv->set_channel_substate = 0;
    vsf_wifi_chip_mt76_trace_debug(
        "mt76: mac_stop" VSF_TRACE_CFG_LINEEND);
    __mt76_mac_stop_step(wifi, VSF_ERR_NONE);
    return VSF_ERR_NONE;
}

static void __mt76_set_channel_start_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_wifi_chip_mt76_trace_debug(
        "mt76: set_channel_start_done err=%d" VSF_TRACE_CFG_LINEEND, (int)err);
    vsf_wifi_done_t done = priv->set_channel_done;
    priv->set_channel_done = NULL;
    if (done != NULL) done(wifi, err);
}

static void __mt76_set_channel_second_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    vsf_wifi_chip_mt76_trace_debug(
        "mt76: set_channel_second_done err=%d" VSF_TRACE_CFG_LINEEND, (int)err);
    if (err != VSF_ERR_NONE) {
        __mt76_set_channel_start_done(wifi, err);
        return;
    }
    __mt76_mac_start(wifi, __mt76_set_channel_start_done);
}

static void __mt76_set_channel_first_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_wifi_chip_mt76_trace_debug(
        "mt76: set_channel_first_done err=%d" VSF_TRACE_CFG_LINEEND, (int)err);
    if (err != VSF_ERR_NONE) {
        __mt76_set_channel_start_done(wifi, err);
        return;
    }

    uint8_t payload[8];
    payload[0] = priv->set_channel_channel; /* idx */
    payload[1] = priv->set_channel_scan;    /* scan */
    payload[2] = 0;                         /* bw: 20 MHz */
    payload[3] = 0;                         /* pad */
    /* chainmask: MT7612U is 2T2R, use 0x0202 */
    payload[4] = 0x02;
    payload[5] = 0x02;
    /* ext_chan: 0xe0 + bw_index for 20 MHz primary channel */
    payload[6] = 0xe0 + priv->set_channel_bw_index;
    payload[7] = 0;

    vsf_err_t step_err = __mt76_mcu_msg_send(wifi,
        MT76_CMD_SWITCH_CHANNEL_OP, payload, sizeof(payload), true,
        __mt76_set_channel_second_done);
    if (step_err != VSF_ERR_NONE) {
        __mt76_set_channel_start_done(wifi, step_err);
    }
}

static void __mt76_set_channel_filter_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) {
        vsf_wifi_done_t done = priv->set_channel_done;
        priv->set_channel_done = NULL;
        if (done != NULL) done(wifi, err);
        return;
    }

    uint8_t payload[8];
    payload[0] = priv->set_channel_channel; /* idx */
    payload[1] = priv->set_channel_scan;    /* scan */
    payload[2] = 0;                         /* bw: 20 MHz */
    payload[3] = 0;                         /* pad */
    /* chainmask: MT7612U is 2T2R, use 0x0202 */
    payload[4] = 0x02;
    payload[5] = 0x02;
    /* first command has no extension channel info */
    payload[6] = 0;
    payload[7] = 0;

    vsf_err_t step_err = __mt76_mcu_msg_send(wifi,
        MT76_CMD_SWITCH_CHANNEL_OP, payload, sizeof(payload), true,
        __mt76_set_channel_first_done);
    if (step_err != VSF_ERR_NONE) {
        vsf_wifi_done_t done = priv->set_channel_done;
        priv->set_channel_done = NULL;
        if (done != NULL) done(wifi, step_err);
    }
}

static void __mt76_set_channel_stop_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) {
        vsf_wifi_done_t done = priv->set_channel_done;
        priv->set_channel_done = NULL;
        if (done != NULL) done(wifi, err);
        return;
    }

    /* Scan mode: clear OTHER_BSS so beacons/probe-responses from any BSS
     * are received. */
    uint32_t filter = priv->rxfilter & ~MT_RX_FILTR_CFG_OTHER_BSS;
    vsf_wifi_chip_mt76_trace_debug(
        "mt76: scan rx_filter=0x%08X" VSF_TRACE_CFG_LINEEND,
        (unsigned)filter);
    vsf_err_t step_err = __mt76_cfg_write(wifi, MT_RX_FILTR_CFG, filter,
                                          __mt76_set_channel_filter_done);
    if (step_err != VSF_ERR_NONE) {
        vsf_wifi_done_t done = priv->set_channel_done;
        priv->set_channel_done = NULL;
        if (done != NULL) done(wifi, step_err);
    }
}

static vsf_err_t __mt76_set_channel_ex(vsf_wifi_t *wifi, uint8_t channel,
                                       bool scan, vsf_wifi_done_t done)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    priv->set_channel_channel  = channel;
    priv->set_channel_scan     = scan ? 1 : 0;
    priv->set_channel_bw_index = 0;     /* 20 MHz */
    priv->set_channel_done     = done;

    vsf_wifi_chip_mt76_trace_debug(
        "mt76: set_channel=%u scan=%d" VSF_TRACE_CFG_LINEEND,
        (unsigned)channel, (int)scan);

    return __mt76_mac_stop(wifi, __mt76_set_channel_stop_done);
}

vsf_err_t __mt76_set_channel(vsf_wifi_t *wifi, uint8_t channel,
                             vsf_wifi_done_t done)
{
    return __mt76_set_channel_ex(wifi, channel, true, done);
}

static void __mt76_test_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_wifi_done_t done = priv->pending_done;
    priv->pending_done = NULL;
    vsf_wifi_chip_mt76_trace_debug(
        "mt76: test mcu cmd done err=%d" VSF_TRACE_CFG_LINEEND, (int)err);
    if (done != NULL) done(wifi, err);
}

static void __mt76_set_rx_filter_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (err != VSF_ERR_NONE) {
        __mt76_test_done(wifi, err);
        return;
    }
    uint8_t payload[8];
    __mt76_put_le32(payload + 0, MT76_FUN_Q_SELECT);
    __mt76_put_le32(payload + 4, 1);
    vsf_err_t step_err = __mt76_mcu_msg_send(wifi, MT76_CMD_FUN_SET_OP,
                                payload, sizeof(payload), false,
                                __mt76_test_done);
    if (step_err != VSF_ERR_NONE) {
        __mt76_test_done(wifi, step_err);
    }
}

vsf_err_t __mt76_set_rx_filter(vsf_wifi_t *wifi, uint32_t mask,
                               vsf_wifi_done_t done)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    uint32_t filter = (mask != 0) ? mask : MT_RX_FILTR_CFG_DEFAULT;
    vsf_wifi_chip_mt76_trace_debug(
        "mt76: set_rx_filter=0x%08X" VSF_TRACE_CFG_LINEEND, (unsigned)filter);
    priv->pending_done = done;
    return __mt76_cfg_write(wifi, MT_RX_FILTR_CFG, filter, __mt76_set_rx_filter_done);
}

vsf_err_t __mt76_set_mac_addr(vsf_wifi_t *wifi, const uint8_t mac[6],
                              vsf_wifi_done_t done)
{
    return __mt76_mac_addr_program_start(wifi, mac, done);
}

static void __mt76_set_bssid_script_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_wifi_done_t done = priv->pending_done;
    priv->pending_done = NULL;
    if (done != NULL) done(wifi, err);
}

vsf_err_t __mt76_set_bssid(vsf_wifi_t *wifi, const uint8_t bssid[6],
                           vsf_wifi_done_t done)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_wifi_reg_op_t *ops = wifi->scratch_ops;

    ops[0].reg = MT_MAC_APC_BSSID_L(0);
    ops[0].val = ((uint32_t)bssid[0])        |
                 ((uint32_t)bssid[1] << 8)  |
                 ((uint32_t)bssid[2] << 16) |
                 ((uint32_t)bssid[3] << 24);
    ops[1].reg = MT_MAC_APC_BSSID_H(0);
    ops[1].val = ((uint32_t)bssid[4])        |
                 ((uint32_t)bssid[5] << 8);

    priv->pending_done = done;
    vsf_wifi_chip_mt76_trace_info(
        "mt76: set_bssid=%02X:%02X:%02X:%02X:%02X:%02X"
        VSF_TRACE_CFG_LINEEND,
        bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);

    return vsf_wifi_reg_run_script(wifi, ops, 2,
                                   __mt76_set_bssid_script_done);
}

vsf_err_t __mt76_set_auth_mode(vsf_wifi_t *wifi,
                               const vsf_wifi_auth_cfg_t *cfg,
                               vsf_wifi_done_t done)
{
    (void)wifi; (void)cfg;
    /* MT76 does not need the auth config programmed ahead of connect; the
     * WPA2 4-way handshake is handled by the wifi layer with software CCMP. */
    if (done != NULL) done(wifi, VSF_ERR_NONE);
    return VSF_ERR_NONE;
}

static void __mt76_connect_script_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_wifi_done_t done = priv->connect_done;
    priv->connect_done = NULL;
    if (done != NULL) done(wifi, err);
}

static void __mt76_connect_channel_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    if (err != VSF_ERR_NONE) {
        vsf_wifi_done_t done = priv->connect_done;
        priv->connect_done = NULL;
        if (done != NULL) done(wifi, err);
        return;
    }

    /* Program the AP BSSID so the hardware accepts frames from this BSS,
     * then allocate WCID 1 for the AP so that subsequent data frames
     * (EAPOL, 4-way handshake) are treated as unicast by the MAC.
     * Linux mt76x02_sta_add uses WCID 1 for the first station in STA mode
     * and vif_idx=0, so the WCID attribute uses BSS_IDX=0 (no extension).
     *
     * Also enable STA-mode TSF sync and program standard WMM EDCA parameters;
     * these are normally done by mac80211 bss_info_changed/conf_tx after
     * association and are required for the AP to accept uplink data frames. */
    const uint8_t *bssid = wifi->mlme_bssid;
    vsf_wifi_reg_op_t *ops = wifi->scratch_ops;
    ops[0].reg = MT_MAC_APC_BSSID_L(0);
    ops[0].val = ((uint32_t)bssid[0])        |
                 ((uint32_t)bssid[1] << 8)  |
                 ((uint32_t)bssid[2] << 16) |
                 ((uint32_t)bssid[3] << 24);
    ops[1].reg = MT_MAC_APC_BSSID_H(0);
    ops[1].val = ((uint32_t)bssid[4])        |
                 ((uint32_t)bssid[5] << 8);

    /* WCID 1 is used for unicast RX decryption: match frames addressed to
     * our STA MAC (RA) so PTK-encrypted frames from the AP are decrypted.
     * Using the AP BSSID here causes broadcast/multicast frames from the AP
     * to match this pairwise WCID and fail hardware decryption. */
    const uint8_t *sta_mac = wifi->mac;
    ops[2].reg = MT76_WCID_ATTR(1);
    ops[2].val = MT76_WCID_ATTR_BSS_IDX;
    ops[3].reg = MT76_WCID_ADDR(1);
    ops[3].val = ((uint32_t)sta_mac[0])        |
                 ((uint32_t)sta_mac[1] << 8)  |
                 ((uint32_t)sta_mac[2] << 16) |
                 ((uint32_t)sta_mac[3] << 24);
    ops[4].reg = MT76_WCID_ADDR(1) + 4;
    ops[4].val = ((uint32_t)sta_mac[4])        |
                 ((uint32_t)sta_mac[5] << 8);
    ops[5].reg = MT76_WCID_DROP(1);
    ops[5].val = 0;

    /* WCID TX info: tell the hardware the default rate/nss/power for this
     * station.  Without the SET flag the chip may fall back to unknown
     * parameters and silently fail to transmit data frames (EAPOL M2/M4).
     * Use CCK 1 Mbps / nss=1 / txpwr_adj=0 as the conservative default. */
    ops[6].reg = MT76_WCID_TX_INFO(1);
    ops[6].val = MT_WCID_TX_INFO_SET
               | (1U << MT_WCID_TX_INFO_NSS_SHIFT)
               | 0x0000U;               /* CCK 1 Mbps */

    /* Beacon interval 100 TU << 4, timer enable, STA TSF sync, TBTT enable. */
    ops[7].reg = MT76_BEACON_TIME_CFG;
    ops[7].val = (100U << 4)
               | MT76_BEACON_TIME_CFG_TIMER_EN
               | MT76_BEACON_TIME_CFG_SYNC_MODE_STA
               | MT76_BEACON_TIME_CFG_TBTT_EN;

    /* Standard WMM EDCA parameters (AP uses these for admission control). */
    ops[8].reg = MT76_EDCA_CFG_AC(0);  /* BE */
    ops[8].val = (10U << 16) | (4U << 12) | (3U << 8) | 0U;
    ops[9].reg = MT76_EDCA_CFG_AC(1);  /* BK */
    ops[9].val = (10U << 16) | (4U << 12) | (7U << 8) | 0U;
    ops[10].reg = MT76_EDCA_CFG_AC(2);  /* VI */
    ops[10].val = (4U << 16) | (3U << 12) | (2U << 8) | 94U;
    ops[11].reg = MT76_EDCA_CFG_AC(3); /* VO */
    ops[11].val = (3U << 16) | (2U << 12) | (2U << 8) | 47U;

    ops[12].reg = MT76_WMM_TXOP(0);    /* BE/BK TXOP = 0 */
    ops[12].val = 0x00000000U;
    ops[13].reg = MT76_WMM_TXOP(1);    /* VI/VO TXOP = 94/47 */
    ops[13].val = 0x002F005EU;

    /* Group/multicast WCID for this BSS (used for broadcast data frames).
     * Set its address to broadcast so frames with RA = ff:ff:ff:ff:ff:ff
     * match this WCID and use the GTK instead of falling back to a pairwise
     * WCID that lacks the group key. */
    ops[14].reg = MT76_WCID_ATTR(MT_VIF_WCID(0));
    ops[14].val = MT76_WCID_ATTR_BSS_IDX;
    ops[15].reg = MT76_WCID_ADDR(MT_VIF_WCID(0));
    ops[15].val = 0xFFFFFFFFU;
    ops[16].reg = MT76_WCID_ADDR(MT_VIF_WCID(0)) + 4;
    ops[16].val = 0x0000FFFFU;
    ops[17].reg = MT76_WCID_DROP(MT_VIF_WCID(0));
    ops[17].val = 0;
    ops[18].reg = MT76_WCID_TX_INFO(MT_VIF_WCID(0));
    ops[18].val = MT_WCID_TX_INFO_SET
                | (1U << MT_WCID_TX_INFO_NSS_SHIFT)
                | 0x0000U;               /* CCK 1 Mbps */

    vsf_wifi_chip_mt76_trace_info(
        "mt76: program wcid=1 bssid=%02X:%02X:%02X:%02X:%02X:%02X bssidx=0 tx_info=0x%08X"
        VSF_TRACE_CFG_LINEEND,
        bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5],
        (unsigned)ops[6].val);

    vsf_err_t step_err = vsf_wifi_reg_run_script(wifi, ops, 19,
                                                 __mt76_connect_script_done);
    if (step_err != VSF_ERR_NONE) {
        vsf_wifi_done_t done = priv->connect_done;
        priv->connect_done = NULL;
        if (done != NULL) done(wifi, step_err);
    }
}

vsf_err_t __mt76_connect(vsf_wifi_t *wifi,
                         const uint8_t bssid[6], const uint8_t *ssid,
                         uint8_t ssid_len, uint8_t channel,
                         vsf_wifi_done_t done)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    (void)bssid; (void)ssid; (void)ssid_len;
    if (priv->connect_done != NULL) {
        return VSF_ERR_NOT_AVAILABLE;
    }

    priv->connect_done = done;
    vsf_wifi_chip_mt76_trace_info(
        "mt76: connect ch=%u bssid=%02X:%02X:%02X:%02X:%02X:%02X"
        VSF_TRACE_CFG_LINEEND,
        (unsigned)channel,
        wifi->mlme_bssid[0], wifi->mlme_bssid[1], wifi->mlme_bssid[2],
        wifi->mlme_bssid[3], wifi->mlme_bssid[4], wifi->mlme_bssid[5]);

    /* If we are already on the target channel (e.g. after a single-channel
     * scan), skip the channel switch and just program the BSSID.  This avoids
     * a firmware stall seen when re-issuing SWITCH_CHANNEL_OP with scan=0. */
    vsf_err_t err;
    if (wifi->channel == channel) {
        __mt76_connect_channel_done(wifi, VSF_ERR_NONE);
        err = VSF_ERR_NONE;
    } else {
        err = __mt76_set_channel_ex(wifi, channel, false,
                                    __mt76_connect_channel_done);
    }
    if (err != VSF_ERR_NONE) {
        priv->connect_done = NULL;
    }
    return err;
}

vsf_err_t __mt76_disconnect(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    /* TODO: clear BSSID/WCID via mcu_cmd, disable TX/RX */
    if (done != NULL) done(wifi, VSF_ERR_NOT_SUPPORT);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t __mt76_get_link_info(vsf_wifi_t *wifi, vsf_wifi_link_info_t *info)
{
    (void)info;
    /* TODO: read RSSI/rate */
    return VSF_ERR_NOT_SUPPORT;
}

/* Map RSN cipher suite type (IEEE 802.11-2012 Table 8-101) to VSF cipher. */
static uint8_t __mt76_rsn_cipher(uint8_t cipher)
{
    switch (cipher) {
    case 1: return WIFI_CIPHER_WEP40;
    case 2: return WIFI_CIPHER_TKIP;
    case 4: return WIFI_CIPHER_CCMP;
    case 5: return WIFI_CIPHER_WEP104;
    default: return WIFI_CIPHER_NONE;
    }
}

/* Parse an RSN IE body (after tag+len) and fill security fields.
 * Only WPA2-PSK is recognised (AKM suite 00-0F-AC-02). */
static void __mt76_parse_rsn(const uint8_t *body, uint8_t len,
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
    if (!memcmp(p, oui, 3)) group = __mt76_rsn_cipher(p[3]);
    p += 4;
    if (p + 2 > end) return;
    count = (uint16_t)(p[0] | ((uint16_t)p[1] << 8)); p += 2;
    for (k = 0; k < count; k++) {
        if (p + 4 > end) return;
        if (!memcmp(p, oui, 3)) {
            uint8_t c = __mt76_rsn_cipher(p[3]);
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
    }
}

void __mt76_parse_rx(vsf_wifi_t *wifi, uint8_t *frame, uint16_t len)
{
    uint16_t pos = 0;

    while (pos + MT_DMA_HDR_LEN <= len) {
        uint16_t dma_len = (uint16_t)frame[pos] |
                           ((uint16_t)frame[pos + 1] << 8);
        uint16_t entry_total = MT_DMA_HDR_LEN + dma_len;
        uint16_t next_pos = (entry_total + 3u) & ~3u;

        if (dma_len == 0 || entry_total > len - pos) {
            vsf_wifi_chip_mt76_trace_debug(
                "mt76: parse_rx break pos=%u len=%u dma_len=%u entry_total=%u"
                VSF_TRACE_CFG_LINEEND, pos, len, dma_len, entry_total);
            break;
        }

        uint8_t *entry = frame + pos;
        uint8_t *rxwi  = entry + MT_DMA_HDR_LEN;
        uint32_t rxinfo = __mt76_get_le32(rxwi);
        uint32_t ctl    = __mt76_get_le32(rxwi + 4);
        uint16_t mpdu_len = (uint16_t)((ctl & MT_RXWI_CTL_MPDU_LEN_MASK)
                                        >> MT_RXWI_CTL_MPDU_LEN_SHIFT);
        uint16_t hdr_pad  = (rxinfo & MT_RXINFO_L2PAD) ? 2 : 0;

        vsf_wifi_chip_mt76_trace_debug(
            "mt76: parse_rx entry pos=%u dma_len=%u mpdu_len=%u hdr_pad=%u "
            "fc=0x%02X%02X scanning=%d"
            VSF_TRACE_CFG_LINEEND,
            pos, dma_len, mpdu_len, hdr_pad,
            rxwi[MT_RX_RXWI_LEN + 1], rxwi[MT_RX_RXWI_LEN],
            (int)wifi->scanning);

        if ((mpdu_len != 0) &&
            (mpdu_len <= dma_len - MT_RX_RXWI_LEN - hdr_pad)) {
            /* The 802.11 frame starts right after the RXWI.  When
             * MT_RXINFO_L2PAD is set the hardware inserts 2 padding bytes
             * between the 802.11 header and the body; remove them in-place
             * so the upper layer sees a contiguous header+body. */
            uint8_t *dot11    = rxwi + MT_RX_RXWI_LEN;
            uint16_t frame_len = mpdu_len;

            if (hdr_pad != 0) {
                uint8_t type = dot11[0] & 0x0C;
                uint16_t hdr_len = 24;
                if (type == 0x08) {
                    uint8_t subtype = (dot11[0] >> 4) & 0x0F;
                    if (subtype & 0x08) hdr_len += 2;
                }
                if (frame_len > hdr_len) {
                    memmove(dot11 + hdr_len,
                            dot11 + hdr_len + hdr_pad,
                            frame_len - hdr_len);
                }
            }

            /* If the hardware decrypted the frame (MT_RXINFO_DECRYPT), the CCMP
             * header has already been stripped and the remaining length is
             * reduced by PN_LEN*4 bytes.  Tell the upper layer the frame is
             * plaintext by clearing the Protected bit so software CCMP decap
             * is skipped. */
            vsf_wifi_chip_mt76_trace_debug(
                "mt76: rxinfo=0x%08X ctl=0x%08X wcid=%u key_idx=%u bss_idx=%u"
                VSF_TRACE_CFG_LINEEND,
                (unsigned)rxinfo, (unsigned)ctl,
                (unsigned)(ctl & MT_RXWI_CTL_WCID_MASK),
                (unsigned)((ctl >> 8) & 0x03U),
                (unsigned)((ctl >> 10) & 0x07U));
            if (rxinfo & MT_RXINFO_DECRYPT) {
                uint8_t pn_len = (uint8_t)((rxinfo & MT_RXINFO_PN_LEN) >> 19);
                if (pn_len != 0) {
                    uint16_t strip = (uint16_t)(pn_len * 4);
                    if (frame_len >= strip) {
                        frame_len -= strip;
                    }
                }
                dot11[1] &= ~0x40U;
            }

            uint8_t type = dot11[0] & 0x0C;
            if (type == 0x00) {                       /* management type */
                uint8_t subtype = (dot11[0] >> 4) & 0x0F;
                vsf_wifi_chip_mt76_trace_debug(
                    "mt76: parse_rx mgmt subtype=%u scanning=%d"
                    VSF_TRACE_CFG_LINEEND,
                    (unsigned)subtype, (int)wifi->scanning);
                if ((subtype == 5 || subtype == 8) && wifi->scanning) {
                    const uint8_t *bssid = dot11 + 16;
                    const uint8_t *body  = dot11 + 24;
                    uint16_t body_len = frame_len - 24;

                    if (body_len >= 12) {
                        vsf_wifi_scan_result_t result;
                        memset(&result, 0, sizeof(result));
                        memcpy(result.bssid, bssid, 6);
                        result.capability = (uint16_t)body[10] |
                                            ((uint16_t)body[11] << 8);
                        result.channel    = wifi->channel;
                        result.flags      = (subtype == 5) ? 1u : 0u;
                        result.rssi       = (int8_t)rxwi[12]; /* rssi[0] */

                        const uint8_t *ie  = body + 12;
                        const uint8_t *end = body + body_len;
                        while (ie + 2 <= end) {
                            uint8_t tag = ie[0];
                            uint8_t l   = ie[1];
                            if (ie + 2 + l > end) break;
                            if (tag == 0 && l <= 32) {
                                memcpy(result.ssid, ie + 2, l);
                                result.ssid_len = l;
                            } else if (tag == 3 && l == 1) {
                                result.channel = ie[2];
                            } else if (tag == 48) {
                                __mt76_parse_rsn(ie + 2, l, &result);
                            }
                            ie += 2 + l;
                        }

                        vsf_wifi_chip_mt76_trace_info(
                            "mt76: scan bssid=%02X:%02X:%02X:%02X:%02X:%02X "
                            "ssid=%.*s ch=%u rssi=%d caps=0x%04X"
                            VSF_TRACE_CFG_LINEEND,
                            bssid[0], bssid[1], bssid[2], bssid[3],
                            bssid[4], bssid[5],
                            result.ssid_len, result.ssid,
                            result.channel, result.rssi, result.capability);

                        vsf_wifi_on_scan_result(wifi, &result);
                    } else {
                        vsf_wifi_chip_mt76_trace_info(
                            "mt76: scan body too short body_len=%u"
                            VSF_TRACE_CFG_LINEEND, body_len);
                    }
                } else if (!wifi->scanning &&
                           (subtype == 0xB || subtype == 0x1 ||
                            subtype == 0xC || subtype == 0xA)) {
                    /* auth / assoc-resp / deauth / disassoc -> MLME */
                    vsf_wifi_mlme_rx(wifi, dot11, frame_len);
                }
            } else if (type == 0x08) {                /* data type */
                vsf_wifi_chip_mt76_trace_debug(
                    "mt76: parse_rx data fc=0x%02X%02X len=%u state=%u"
                    VSF_TRACE_CFG_LINEEND,
                    dot11[1], dot11[0],
                    (unsigned)frame_len, (unsigned)wifi->mlme_state);
                if (!wifi->scanning &&
                    ((wifi->mlme_state == WIFI_MLME_4WAY) ||
                     (wifi->mlme_state == WIFI_MLME_RUN))) {
                    vsf_wifi_data_rx(wifi, dot11, frame_len);
                }
            }
        }

        if (next_pos <= pos) break;
        pos = next_pos;
    }
}

/*============================================================================
 * TX status FIFO polling (debug): after each data frame read MT_TX_STAT_FIFO
 * once to see whether the AP ACKed it.  This is a one-shot delayed read so it
 * does not block the caller.
 *===========================================================================*/
#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
static void __mt76_txstat_read_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) {
        vsf_wifi_chip_mt76_trace_info(
            "mt76: txstat read failed err=%d" VSF_TRACE_CFG_LINEEND, (int)err);
        return;
    }
    uint32_t v = priv->txstat_val;
    if (!(v & MT_TX_STAT_FIFO_VALID)) {
        vsf_wifi_chip_mt76_trace_info(
            "mt76: txstat FIFO empty" VSF_TRACE_CFG_LINEEND);
        return;
    }
    vsf_wifi_chip_mt76_trace_info(
        "mt76: txstat success=%u ack_req=%u wcid=%u rate=0x%04X raw=0x%08X"
        VSF_TRACE_CFG_LINEEND,
        !!(v & MT_TX_STAT_FIFO_SUCCESS),
        !!(v & MT_TX_STAT_FIFO_ACKREQ),
        (unsigned)((v & MT_TX_STAT_FIFO_WCID_M) >> MT_TX_STAT_FIFO_WCID_S),
        (unsigned)((v & MT_TX_STAT_FIFO_RATE_M) >> MT_TX_STAT_FIFO_RATE_S),
        (unsigned)v);
}

static void __mt76_txstat_timer(vsf_callback_timer_t *timer)
{
    mt76_wifi_priv_t *priv = vsf_container_of(timer, mt76_wifi_priv_t, txstat_timer);
    __mt76_cfg_read(priv->wifi, MT_TX_STAT_FIFO, &priv->txstat_val,
                    __mt76_txstat_read_done);
}

static void __mt76_txstat_poll(vsf_wifi_t *wifi)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_callback_timer_init(&priv->txstat_timer);
    priv->txstat_timer.on_timer = __mt76_txstat_timer;
    vsf_callback_timer_add_ms(&priv->txstat_timer, 150);
}
#else
static void __mt76_txstat_poll(vsf_wifi_t *wifi) { (void)wifi; }
#endif

#if VSF_WIFI_USE_WPA == ENABLED
static void __mt76_crypto_key_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_wifi_done_t done = priv->crypto_done;

    priv->crypto_done = NULL;
    vsf_wifi_chip_mt76_trace_info(
        "mt76: crypto install done err=%d" VSF_TRACE_CFG_LINEEND,
        (int)err);
    if (done != NULL) {
        done(wifi, err);
    }

    /* Hardware TX encryption is not yet reliable on this chip/driver
     * combination (broadcast/unicast CCMP frames are ACKed but appear to be
     * dropped by the AP upper layer).  Keep the keys in the WCID tables for
     * hardware RX decryption and let the generic WPA layer perform software
     * CCMP encap on TX.  This isolates the RX path while the TX bug is being
     * root-caused. */
    if (err == VSF_ERR_NONE) {
        wifi->wpa_hw_crypto = false;
    }
}

static vsf_err_t __mt76_crypto_install_key(vsf_wifi_t *wifi,
    uint8_t key_idx, bool pairwise,
    const uint8_t *key, uint8_t key_len,
    const uint8_t *mac, vsf_wifi_done_t done)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    (void)mac;

    if (priv->crypto_done != NULL) {
        return VSF_ERR_NOT_AVAILABLE;
    }
    if (key_len > 32) {
        return VSF_ERR_INVALID_PARAMETER;
    }

    /* The generic handshake sets wpa_hw_crypto = true when crypto_ops is
     * supplied.  Program the keys into the MT76 WCID key table so the firmware
     * can hardware-encrypt/decrypt CCMP data frames. */

    uint8_t key_data[32];
    memset(key_data, 0, sizeof(key_data));
    if ((key != NULL) && (key_len > 0)) {
        memcpy(key_data, key, key_len);
    }

    uint8_t wcid = pairwise ? 1 : MT_VIF_WCID(0);
    uint8_t iv_data[8];
    /* Start the CCMP PN at 1; a zero PN is rejected by many APs as an
     * invalid initial value.  The layout follows Linux mt76x02_mac_wcid_set_key
     * for CCMP with Ext IV:
     *   pn[15:0]  -> iv_data[1:0]
     *   pn[47:16] -> iv_data[7:4]
     *   iv_data[3] = 0x20 | (key_idx << 6) (Ext IV flag + key index). */
    uint64_t pn = 1;
    memset(iv_data, 0, sizeof(iv_data));
    iv_data[0] = (uint8_t)(pn & 0xFFU);
    iv_data[1] = (uint8_t)((pn >> 8) & 0xFFU);
    iv_data[2] = 0;
    iv_data[3] = (uint8_t)(0x20U | (key_idx << 6));
    iv_data[4] = (uint8_t)((pn >> 16) & 0xFFU);
    iv_data[5] = (uint8_t)((pn >> 24) & 0xFFU);
    iv_data[6] = (uint8_t)((pn >> 32) & 0xFFU);
    iv_data[7] = (uint8_t)((pn >> 40) & 0xFFU);

    /* Linux uses vif_idx=0 in STA mode, so BSS_IDX=0 (no extension).  The
     * cipher mode and pairwise flag are the only non-zero attribute bits. */
    uint32_t attr = (MT76X02_CIPHER_AES_CCMP << MT76_WCID_ATTR_PKEY_MODE_SHIFT);
    if (pairwise) {
        attr |= MT76_WCID_ATTR_PAIRWISE;
    }

    vsf_wifi_reg_op_t *ops = wifi->scratch_ops;
    uint16_t n = 0;

    for (int i = 0; i < 8; i++) {
        ops[n].reg = MT76_WCID_KEY(wcid) + i * 4;
        ops[n].val = __mt76_get_le32(key_data + i * 4);
        n++;
    }
    ops[n].reg = MT76_WCID_IV(wcid);
    ops[n].val = __mt76_get_le32(iv_data + 0);
    n++;
    ops[n].reg = MT76_WCID_IV(wcid) + 4;
    ops[n].val = __mt76_get_le32(iv_data + 4);
    n++;
    ops[n].reg = MT76_WCID_ATTR(wcid);
    ops[n].val = attr;
    n++;

    if (!pairwise) {
        for (int i = 0; i < 8; i++) {
            ops[n].reg = MT76_SKEY(0, key_idx) + i * 4;
            ops[n].val = __mt76_get_le32(key_data + i * 4);
            n++;
        }
        ops[n].reg = MT76_SKEY_MODE(0);
        ops[n].val = (uint32_t)MT76X02_CIPHER_AES_CCMP
                   << MT76_SKEY_MODE_SHIFT(0, key_idx);
        n++;
    }

    priv->crypto_done = done;
    vsf_wifi_chip_mt76_trace_info(
        "mt76: install_key pairwise=%d key_idx=%u wcid=%u n=%u"
        VSF_TRACE_CFG_LINEEND,
        (int)pairwise, (unsigned)key_idx, (unsigned)wcid, (unsigned)n);

    vsf_err_t err = vsf_wifi_reg_run_script(wifi, ops, n,
                                            __mt76_crypto_key_done);
    if (err != VSF_ERR_NONE) {
        priv->crypto_done = NULL;
    }
    return err;
}

static const vsf_wifi_crypto_ops_t __mt76_crypto_ops = {
    .install_key = __mt76_crypto_install_key,
};
#endif      /* VSF_WIFI_USE_WPA == ENABLED */

/* Queue index used for management frames (probe-request, auth, assoc).
 * On USB this maps to the AC_BE bulk-out endpoint. */
#define MT76_TX_QUEUE_MGMT                  1

/* Software-managed 802.11 sequence number for MT76.  The chip's NSEQ bit
 * does not seem to increment the sequence number for our current WCID setup,
 * leaving every data frame with seq=0; the AP ACKs the radio frame but drops
 * it as a duplicate, causing the 4-way handshake to stall. */
static uint16_t __mt76_tx_seq;

vsf_err_t __mt76_tx(vsf_wifi_t *wifi, const uint8_t *frame, uint16_t len)
{
    if (len < 2) return VSF_ERR_INVALID_PARAMETER;

    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    /* The MT76 MAC overwrites the Duration field of *unprotected* QoS Data
     * frames to 0, which causes APs to discard unprotected frames (EAPOL-Key
     * M2/M4) at higher layers even though they ACK them at the PHY.  Convert
     * only unprotected QoS EAPOL-Key frames to plain Data so the Duration we
     * write is preserved.  Post-handshake data frames are left as QoS Data to
     * match the RT5572 reference and Linux mt76x02 behavior. */
    uint8_t  frame_buf[1536];
    const uint8_t *tx_frame = frame;
    uint16_t tx_len = len;
    uint8_t  fc0 = frame[0];
    uint8_t  fc1 = frame[1];
    uint8_t  subtype = (fc0 >> 4) & 0x0F;
    bool     qos = subtype == 8;
    bool     protected = (fc1 & 0x40) != 0;
    bool     is_eapol = false;
    bool     is_null = (subtype == 4);
    if ((fc0 & 0x0C) == 0x08) {
        /* Detect EAPOL before any header rewrite.  The netdrv always sends QoS
         * Data with LLC/SNAP at offset 26; if QoS has already been stripped the
         * SNAP header moves to offset 24. */
        uint16_t snap_off = (qos && !protected && len >= 34) ? 26 : 24;
        if (len >= snap_off + 8) {
            is_eapol = (frame[snap_off + 0] == 0xAA)
                    && (frame[snap_off + 1] == 0xAA)
                    && (frame[snap_off + 6] == 0x88)
                    && (frame[snap_off + 7] == 0x8E);
        }
    }
    if (qos && !protected && is_eapol && len >= 26 && len <= sizeof(frame_buf)) {
        frame_buf[0] = (fc0 & 0x0F);            /* subtype 0: Data, keep Type */
        memcpy(frame_buf + 1, frame + 1, 23);   /* FC[1], Duration, addrs, Seq */
        memcpy(frame_buf + 24, frame + 26, len - 26); /* skip QoS Control */
        tx_frame = frame_buf;
        tx_len = len - 2;
        fc0 = frame_buf[0];
        qos = false;
        vsf_wifi_chip_mt76_trace_debug(
            "mt76: QoS->plain Data EAPOL tx_len=%u"
            VSF_TRACE_CFG_LINEEND, (unsigned)tx_len);
    }

    /* Compute the 802.11 header length so we can insert the L2 pad that
     * MT76 hardware expects when the header is not a multiple of 4 bytes.
     * The payload must start at a 4-byte aligned offset from the TXWI;
     * for a 26-byte QoS Data header this means 2 bytes of padding.
     * Ref: RT5572 reference path uses (-hdr_len) & 3 for the same purpose. */
    fc1 = tx_frame[1];
    bool     to_ds    = (fc1 & 0x01) != 0;
    bool     from_ds  = (fc1 & 0x02) != 0;
    uint16_t hdrlen   = 24;

    if (to_ds && from_ds) hdrlen += 6; /*_addr4 */
    if (qos)              hdrlen += 2;
    if ((fc1 & 0x80) != 0) hdrlen += 4;

    /* Classify the frame before allocating the DMA buffer. */
    uint8_t type    = fc0 & 0x0C;
    bool    is_data = (type == 0x08);
    bool    multicast = (tx_frame[16] & 0x01) != 0;
    /* Null Data frames (keepalive) must stay unencrypted: the AP needs to ACK
     * them at the PHY/MAC layer, and many APs do not decrypt/ACK an encrypted
     * Null frame from a STA.  This matches the RT5572 reference path. */
    bool hw_encrypt = wifi->wpa_hw_crypto && is_data && !is_eapol && !is_null;
    /* For hardware CCMP the firmware inserts the CCMP header/MIC from the
     * WCID_IV/key registers, so the host frame must be plaintext and WIV=0.
     * For software CCMP (and all non-data frames) WIV=1 keeps the firmware
     * from overwriting the IV already placed in the frame/TWI. */

    /* For hardware-encrypted broadcast/multicast frames the firmware builds
     * the CCMP AAD from only the QoS TID bits, while the AP verifies the
     * standard AAD that includes the Ack Policy bits.  The netdrv sets Ack
     * Policy = No Ack (byte0 = 0x20) for broadcasts, which produces an AAD
     * mismatch; strip QoS so the AAD is unambiguous.
     * For software-encrypted frames the MIC is already computed over the
     * original QoS header, so stripping it here would corrupt the AAD and
     * make the AP drop the frame. */
    bool     strip_qos    = multicast && qos && hw_encrypt;
    uint16_t air_hdr_len  = hdrlen - (strip_qos ? 2U : 0U);

    uint16_t mpdu_len = tx_len - (hdrlen - air_hdr_len);
    uint8_t  hdr_pad  = (uint8_t)((-air_hdr_len) & 3U);
    uint16_t txwi_len = 20;
    uint16_t payload_after_txwi = mpdu_len + hdr_pad;
    /* MT_TXD_INFO_LEN = length of (TXWI + 802.11 frame + header pad) rounded
     * up to 4 bytes.  The 4-byte TXINFO itself and a 4-byte trailing pad are
     * added by the USB layer and must NOT be counted in this field. */
    uint16_t info_len = (txwi_len + payload_after_txwi + 3u) & ~3u;
    uint16_t total_after_txinfo = 4 + info_len;
    uint16_t pad = ((total_after_txinfo + 3u) & ~3u) + 4 - total_after_txinfo;
    uint16_t total = total_after_txinfo + pad;

    if (total > sizeof(priv->tx_buf)) {
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }

    uint8_t *buf = priv->tx_buf;

    /* The TX buffer is reused for firmware commands and frames.  Zero the
     * whole transfer first so the 4-byte DMA alignment padding between the
     * end of the 802.11 frame and the trailing pad is not filled with stale
     * bytes from a previous command.  Such garbage becomes part of the MPDU
     * if the firmware uses the USB transfer length instead of len_ctl,
     * causing FCS errors and making the AP miss handshake frames like M2. */
    memset(buf, 0, total);

    /* USB queue 1 is the AC_BE bulk endpoint; Linux uses EDCA qsel for all
     * non-HCCA frames on this endpoint. */
    uint8_t qsel = MT_QSEL_EDCA;

    uint32_t info = ((uint32_t)info_len & MT_TXD_INFO_LEN_MASK)
                  | MT_TXD_INFO_80211
                  | (((uint32_t)qsel & 3U) << MT_TXD_INFO_QSEL_SHIFT)
                  | (((uint32_t)MT_WLAN_PORT & 7U) << MT_TXD_INFO_DPORT_SHIFT);
    /* WIV=1 for software-managed IV; WIV=0 for firmware-managed WCID IV. */
    if (!hw_encrypt) {
        info |= MT_TXD_INFO_WIV;
    }
    __mt76_put_le32(buf, info);

    uint8_t *txwi = buf + 4;
    memset(txwi, 0, txwi_len);
    /* Management frames (auth/assoc/probe) and post-handshake data are sent at
     * CCK 1 Mbps.  OFDM 6 Mbps was tried earlier, but this AP/STA combination
     * ACKs the frames while the upper layer appears to drop them; falling back
     * to the same CCK rate that works for EAPOL isolates whether the issue is
     * the rate/MCS selection. */
    uint16_t rate = 0;                          /* CCK 1 Mbps for EAPOL/mgmt */
    /* Linux mt76x02: WCID 1 is used for unicast ACK routing; WCID 254 is the
     * group/multicast WCID for this BSS.  For hardware-encrypted data frames
     * the firmware uses the key stored in the selected WCID. */
    uint8_t wcid;
    if (is_data) {
        if (protected && !hw_encrypt) {
            /* Software CCMP: the frame already carries a valid CCMP header/MIC.
             * Do not route it through the WCID key table, or the hardware will
             * encrypt it a second time.  Use WCID 0xff (no key) and keep WIV=1
             * so the firmware forwards the MPDU as-is.  This matches Linux
             * mt76x02_mac_write_txwi() when no hw_key is assigned. */
            wcid = 0xff;
        } else if (multicast) {
            /* Broadcast/multicast frames transmitted by a STA are encrypted
             * with the GTK, not the pairwise key.  The GTK lives in the group
             * WCID (254) programmed during M3.  Linux mt76x02 routes these
             * frames through the VIF WCID so the firmware picks up the GTK
             * and the correct key index for the CCMP header. */
            wcid = MT_VIF_WCID(0);
        } else {
            /* Unicast data to the AP uses the pairwise key in WCID 1. */
            wcid = 1;
        }
    } else {
        wcid = 0xff;
    }
    /* Post-handshake unicast data frames are sent at OFDM 6 Mbps.  Broadcast
     * /multicast data and EAPOL-Key frames use CCK 1 Mbps for robust delivery. */
    if (is_data && !is_eapol && !multicast) {
        rate = 0x2000U;                     /* OFDM 6 Mbps */
    }
    __mt76_put_le16(txwi + 0, 0);       /* flags */
    __mt76_put_le16(txwi + 2, rate);
    /* ack_ctl: request ACK for unicast frames only.  Broadcast/multicast
     * frames must not expect an ACK; otherwise the firmware keeps retrying
     * and never reports TX done.  Do NOT use NSEQ: on this chip/driver combo
     * the hardware leaves the sequence number at 0 for every frame, causing
     * the AP to treat M2/M4 as duplicates.  We assign a software-managed seq. */
    txwi[4] = multicast ? 0 : MT_TXWI_ACK_CTL_REQ;
    txwi[5] = wcid;                     /* wcid: 1=AP unicast, 254=bcast/mcast, ff=mgmt */
    __mt76_put_le16(txwi + 6, mpdu_len);/* len_ctl = real MPDU length */
    txwi[16] = (uint8_t)(wifi->mlme_aid & 0xFFU);   /* AID low byte */
    /* txstream: Linux mt76x02 uses the number of TX chains (chainmask low
     * nibble) and selects the stream encoding by PHY type:
     *   - E4+ and HT/VHT frames: 0x13
     *   - E3+ and non-HT (CCK/OFDM) frames: 0x93
     * MT7612U is 2T2R and rev 0x44 >= E4.  The wrong encoding for CCK/OFDM
     * EAPOL frames causes the chip to report TX success while the frame is
     * never radiated, so the 4-way handshake stalls. */
    {
        uint8_t nstreams = (uint8_t)(priv->chainmask & 0x0F);
        uint16_t rev = __mt76_rev(priv);
        uint8_t phy = (uint8_t)((rate & MT_RXWI_RATE_PHY_MASK) >> 13);
        if (nstreams > 1) {
            if (rev >= 0x40) {
                txwi[17] = (phy == MT_PHY_TYPE_HT) ? 0x13 : 0x93;
            } else if (rev >= MT76_REV_E3) {
                txwi[17] = 0x93;
            }
        } else {
            txwi[17] = 0;
        }
    }
    txwi[18] = 0;                       /* ctl2 (TX power adjustment) */
    /* pktid: non-zero pktid makes the hardware push a TX status entry
     * for this frame; without it MT_TX_STAT_FIFO stays empty. */
    txwi[19] = is_data ? 1 : 0;         /* pktid */

    uint8_t *dst = buf + 4 + txwi_len;
    if (hw_encrypt) {
        if (tx_len < hdrlen) {
            return VSF_ERR_INVALID_PARAMETER;
        }
        memcpy(dst, tx_frame, air_hdr_len);
        /* Firmware-managed CCMP: mark the frame Protected and let the chip
         * insert the IV/EIV from the WCID_IV registers. */
        dst[1] |= 0x40U;                            /* Protected */
    } else {
        memcpy(dst, tx_frame, air_hdr_len);
    }
    if (strip_qos) {
        /* Convert QoS Data -> plain Data by clearing the subtype bits. */
        dst[0] &= 0x0FU;
    }
    if (hdr_pad) {
        memset(dst + air_hdr_len, 0, hdr_pad);
    }
    memcpy(dst + air_hdr_len + hdr_pad, tx_frame + hdrlen, tx_len - hdrlen);

    /* iv(4) at txwi[8..11] and eiv(4) at txwi[12..15].
     * Linux mt76x02 leaves these zero for software-encrypted frames: WIV=1
     * tells the firmware to use the CCMP header already present in the MPDU,
     * and WCID=0xff ensures it does not run the frame through the WCID key
     * table.  Copying PN here caused the firmware to re-encrypt with a
     * firmware-generated PN, producing an undecryptable double-encrypted
     * frame. */
    (void)protected;

    /* EAPOL-Key frames (4-way handshake) must be sent at a rate the AP will
     * ACK reliably.  MT76 at OFDM 6 Mbps results in no ACK and deauth reason 15.
     * Use CCK 1 Mbps and the same Duration that the RT5572 reference driver
     * uses for M2/M4 (0x002c); a larger value seems to be overwritten to 0 by
     * the MT76 MAC when the frame is sent as QoS Data, so we converted the
     * frame to plain Data above. */
    if (is_eapol) {
        rate = 0;                                   /* CCK 1 Mbps */
        __mt76_put_le16(txwi + 2, rate);
    }
    /* MT76 does not auto-fill Duration for plain Data frames; a zero Duration
     * causes many APs to drop the frame even if they ACK it.  Use the same
     * NAV value that works for EAPOL-Key frames (0x002c) for unicast frames
     * only.  Broadcast/multicast frames must keep Duration=0 because they set
     * no NAV; a non-zero value makes many APs discard them.
     * For software-CCMP frames the MPDU is already encrypted when it reaches
     * us; overwriting Duration here would corrupt the MIC and make the AP
     * drop the frame silently.  Those frames already carry the duration chosen
     * by the upper layer (0 for broadcasts, which is correct). */
    if ((type == 0x08) && !protected && !multicast) {
        __mt76_put_le16(dst + 2, 0x002CU);
    }

    if (mpdu_len >= 24) {
        __mt76_tx_seq++;
        __mt76_put_le16(dst + 22, (uint16_t)(__mt76_tx_seq << 4));
    }
    memset(buf + total - pad, 0, pad);

    vsf_wifi_chip_mt76_trace_debug(
        "mt76: tx frame fc=0x%02X%02X len=%u total=%u hdr_pad=%u wcid=%u rate=0x%04X hw_enc=%u"
        VSF_TRACE_CFG_LINEEND,
        fc1, fc0, (unsigned)mpdu_len, (unsigned)total, hdr_pad,
        (unsigned)wcid, (unsigned)rate, (unsigned)hw_encrypt);

    vsf_err_t err = __mt76_tx_frame(wifi, buf, total, MT76_TX_QUEUE_MGMT, NULL);
    if ((err == VSF_ERR_NONE) && (type == 0x08)) {
        /* Temporarily disabled: txstat polling uses EP0 reg_read and collides
         * with other cfg_read calls after link-up, triggering an assertion.
         * Re-enable once EP0 accesses are properly serialized. */
        // __mt76_txstat_poll(wifi);
    }
    return err;
}

const vsf_wifi_chip_drv_t vsf_wifi_mt76_drv = {
    .name           = "mt7612u",
    .firmware_load  = __mt76_firmware_load,
    .init           = __mt76_init,
    .fini           = __mt76_fini,
    .set_channel    = __mt76_set_channel,
    .set_rx_filter  = __mt76_set_rx_filter,
    .set_mac_addr   = __mt76_set_mac_addr,
    .set_bssid      = __mt76_set_bssid,
    .set_auth_mode  = __mt76_set_auth_mode,
    .connect        = __mt76_connect,
    .disconnect     = __mt76_disconnect,
    .get_link_info  = __mt76_get_link_info,
    .parse_rx       = __mt76_parse_rx,
    .build_tx       = NULL,
    .tx             = __mt76_tx,
/* Hardware key-table programming is required: without a valid WCID key the
 * MT76 firmware silently drops protected data frames (software-CCMP TX with
 * WIV=1 never reaches the air).  For WIV=0 the firmware inserts the CCMP
 * header/MIC itself from the WCID key/IV registers. */
#if (VSF_WIFI_USE_WPA == ENABLED)
    .crypto_ops     = &__mt76_crypto_ops,
#endif
};

#endif      /* VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_MT76 == ENABLED */
