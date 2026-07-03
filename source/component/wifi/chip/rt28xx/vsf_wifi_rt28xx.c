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
 * (reg, val) pairs through a reg_bus vtable; this file expresses every
 * chip operation as such an op-array and hands it to vsf_wifi_reg_run_script.
 *
 * Static sequences (the chip init table) live in .rodata.  Parameterised
 * sequences (set_channel, set_mac_addr, connect, ...) are built into
 * wifi->scratch_ops at call time via vsf_wifi_reg_get_scratch_ops().
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
 *   1. Firmware upload of rt2870.bin (8 KiB) — done.  The wifi layer ships
 *      the blob via vsf_wifi_reg_run_blob; the application links a strong
 *      override of __rt2870_firmware_data[] / __rt2870_firmware_size.
 *   2. MCU-ready handshake — done.  Polls MAC_CSR0 (ASIC_VER_ID) until the
 *      MCU latches a non-trivial chip-ID, mirroring rt2800_wait_for_mcu_ready.
 *   3. eFuse row-0 read for the per-device MAC — done.  RF chip subtype, TX
 *      power tables and calibration constants live in higher rows and are
 *      still TODO; they are not strictly required for scan/RX bring-up.
 *   4. BBP wake-up sequence (poll BBP_R0 != 0xFF) before BBP defaults — TODO.
 *
 * parse_rx is implemented for the RT5572 RXWI layout (5-word / 20-byte
 * info header).  RT2860/RT2870-style 16-byte RXWI variants would need a
 * separate winfo_size derived from the chip subtype — not wired yet.
 *==========================================================================*/

/*============================ INCLUDES ======================================*/

#include "../../vsf_wifi.h"
#include "../../vsf_wifi_priv.h"

#include <stdlib.h>     /* rand() for fallback MAC generation */

#if VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_RT28XX == ENABLED

#include "../../vsf_wifi_priv.h"

/*============================ MAC REGISTERS =================================*/

#define RT28XX_PBF_SYS_CTRL             0x0400
/* PBF_SYS_CTRL bit 7 (PBF_SYS_CTRL_READY, rt2800.h:569).  This is the REAL
 * "MCU has booted" gate: rt2800_load_firmware (rt2800lib.c:762-767) polls
 * this bit after USB_MODE_FIRMWARE before declaring the firmware running.
 * Reading a static chip-ID register (ASIC_VER_ID) does NOT prove the 8051
 * is executing -- only PBF_SYS_CTRL_READY does. */
#define RT28XX_PBF_SYS_CTRL_READY      (1u << 7)
/* PBF_SYS_CTRL bit13 (0x2000) is an unnamed bit that ref rt2800usb_init_registers
 * (rt2800usb.c:280-281) clears via read-modify-write at radio bring-up, right
 * before the digital-core USB_MODE_RESET.  Leaving it set across re-runs is one
 * source of stale PBF/FIFO state.  (NOTE: HOST_RAM_WRITE is bit16, not this.) */
#define RT28XX_PBF_SYS_CTRL_RESET13    (1u << 13)
#define RT28XX_PBF_CFG                  0x0408
#define RT28XX_USB_DMA_CFG              0x02A0
#define RT28XX_WPDMA_GLO_CFG            0x0208
#define RT28XX_FW_FIRMWARE_BASE         0x3000   /* base for fw upload  */

/* USB ep0 vendor request to control the device's operating mode
 * (rt2x00usb.h).  bRequest=USB_DEVICE_MODE(1); wValue selects the mode:
 *   USB_MODE_RESET(1)    - reset the digital core (issued in init_registers)
 *   USB_MODE_FIRMWARE(8) - tell the 8051 MCU to start executing the firmware
 *                          just uploaded to FIRMWARE_IMAGE_BASE.  Without
 *                          this the MCU never runs, so the RF/PHY is never
 *                          brought up and the receiver stays silent. */
#define RT28XX_USB_DEVICE_MODE          1
#define RT28XX_USB_MODE_RESET           1
#define RT28XX_USB_MODE_FIRMWARE        8

/* eFuse / EEPROM access (rt2800 family).  EFUSE_CTRL bit 31 (PRESENT) is
 * latched 1 by hardware iff this die is fitted with an OTP block; bit 30
 * (KICK) starts a row read and self-clears when the data is in the
 * EFUSE_DATAx mirrors.  EFUSE_DATA3 holds the lowest-addressed bytes of
 * the row, EFUSE_DATA0 the highest (the byte order is reversed compared
 * to the register address ordering — same convention Linux rt2x00 uses). */
#define RT28XX_EFUSE_CTRL               0x0580
/* eFuse data registers (ref rt2800.h:664).  DATA0=0x0590 holds the LAST 4
 * bytes of a row, DATA3=0x059C the FIRST 4 bytes ("data is read from end to
 * start", rt2800lib.c:10941).  These were previously swapped, which mis-parsed
 * the MAC (and would corrupt any future EEPROM-calibration reads). */
#define RT28XX_EFUSE_DATA0              0x0590
#define RT28XX_EFUSE_DATA1              0x0594
#define RT28XX_EFUSE_DATA2              0x0598
#define RT28XX_EFUSE_DATA3              0x059C
#define RT28XX_EFUSE_KICK               (1u << 30)
#define RT28XX_EFUSE_PRESENT            (1u << 31)
/* EFUSE_CTRL field positions (ref rt2800.h:656-657).  ADDRESS_IN is the WORD
 * index (u16 units) of the 8-word block to read; it starts at bit 17, NOT 16.
 * The old row-0 read used (x<<16) which only happened to work because AIN==0. */
#define RT28XX_EFUSE_AIN_SHIFT          17      /* EFUSE_CTRL_ADDRESS_IN = 0x03fe0000 */
#define RT28XX_EFUSE_MODE_SHIFT         6       /* EFUSE_CTRL_MODE        = 0x000000c0 */
/* RF frequency-offset calibration (ref rt2800lib.c:2447 rt2800_freq_cal_mode1).
 * EEPROM_FREQ is word 0x1d on RT5592 (ref rt2800lib.c:316); its low byte
 * (EEPROM_FREQ_OFFSET = 0x00ff) feeds RFCSR17_CODE (0x7f), clamped to 0x5f. */
#define RT28XX_EFUSE_FREQ_WORD          0x1d    /* EEPROM_FREQ (RT5592) */
#define RT28XX_EFUSE_FREQ_BLOCK         24      /* 8-word block (0x18) holding word 0x1d */
#define RT28XX_RFCSR17_CODE             0x7f    /* ref RFCSR17_CODE */
#define RT28XX_FREQ_OFFSET_BOUND        0x5f    /* ref FREQ_OFFSET_BOUND */
/* TX IQ calibration (ref rt2800lib.c:4026 rt2800_iq_calibrate, RT5592 only).
 * The EEPROM IQ cal entries are BYTE addresses (ref rt2800.h:2960):
 *   0x130 IQ_GAIN_CAL_TX0_2G   0x131 IQ_PHASE_CAL_TX0_2G
 *   0x133 IQ_GAIN_CAL_TX1_2G   0x134 IQ_PHASE_CAL_TX1_2G
 *   0x13C RF_IQ_COMPENSATION_CONTROL
 *   0x13D RF_IQ_IMBALANCE_COMPENSATION_CONTROL
 * Byte 0x130 == word 0x98 (152) -> falls in the 8-word eFuse block whose AIN
 * is the block-aligned word index 152 (0x98).  Within that block (LE):
 *   DATA3 = words 152..153 -> bytes 0x130(lo) 0x131 0x132 0x133
 *   DATA2 = words 154..155 -> bytes 0x134 0x135 0x136 0x137
 *   DATA0 = words 158..159 -> bytes 0x13C 0x13D 0x13E 0x13F */
#define RT28XX_EFUSE_IQ_BLOCK           152     /* word 0x98, holds bytes 0x130.. */
#define RT28XX_BBP158_IQ_INDEX          158     /* IQ cal index register */
#define RT28XX_BBP159_IQ_VALUE          159     /* IQ cal value register */

#define RT28XX_H2M_MAILBOX_CSR          0x7010
/* rt2800.h:2123/2133 -- the CID/STATUS mailboxes sit at 0x7014/0x701c, NOT
 * 0x7008/0x700C.  The firmware-download handshake clears both to ~0 before
 * the USB_MODE_FIRMWARE vendor request; wrong addresses left the real
 * mailboxes uninitialised, which can stall the MCU boot. */
#define RT28XX_H2M_MAILBOX_CID          0x7014
#define RT28XX_H2M_MAILBOX_STATUS       0x701C
#define RT28XX_H2M_INT_SRC              0x7024
#define RT28XX_H2M_BBP_AGENT            0x7028
/* HOST_CMD_CSR (0x0404): low 8 bits = HOST_COMMAND.  H2M_MAILBOX_CSR OWNER is
 * the high byte (0xff000000); writing OWNER=1 hands the mailbox to the MCU. */
#define RT28XX_HOST_CMD_CSR            0x0404
#define RT28XX_H2M_MAILBOX_OWNER       0x01000000u
#define RT28XX_MCU_BOOT_SIGNAL         0x72

#define RT28XX_ASIC_VER_ID              0x1000
/* ASIC_VER_ID low 16 bits = chip revision (MAC_CSR0_REVISION).  REV_RT5592C =
 * 0x0221 (rt2800.h:90).  The dongle under test reports 0x55920222 -> rev
 * 0x0222 >= 0x0221, i.e. it is an RT5592C (or later) part, which takes a
 * DIFFERENT BBP/RFCSR init tail than the pre-C revision. */
#define RT28XX_REV_RT5592C              0x0221
#define RT28XX_MAC_SYS_CTRL             0x1004
#define RT28XX_MAC_ADDR_DW0             0x1008
#define RT28XX_MAC_ADDR_DW1             0x100C
#define RT28XX_MAC_BSSID_DW0            0x1010
#define RT28XX_MAC_BSSID_DW1            0x1014
#define RT28XX_MAC_MAX_LEN_CFG          0x1018
#define RT28XX_BBP_CSR_CFG              0x101C
/* TX status FIFO (ref rt2800.h:1931).  VALID=bit0, PID_TYPE=bits4:1,
 * TX_SUCCESS=bit5, TX_ACK_REQUIRED=bit7, WCID=bits15:8, MCS=bits20:16.
 * Reading pops one entry; the FIFO is 16 deep and latched per PACKETID. */
#define RT28XX_TX_STA_FIFO             0x1718
/* RF indirect-access serial-bus register is at 0x0500 (rt2800.h RF_CSR_CFG);
 * 0x1020 is RF_CSR_CFG0, a different RT30xx-style register. */
#define RT28XX_RF_CSR_CFG               0x0500
#define RT28XX_LED_CFG                  0x1024
#define RT28XX_AMPDU_MAX_LEN_20M1S      0x1030
#define RT28XX_AMPDU_MAX_LEN_40M1S      0x1034

#define RT28XX_TX_PIN_CFG               0x1328
#define RT28XX_TX_BAND_CFG              0x132C
/* TX_SW_CFG offsets per rt2800.h: CFG0=0x1330, CFG1=0x1334, CFG2=0x1338.
 * The old defines were shifted down by one slot (CFG1=0x1330/CFG2=0x1334),
 * so every TX_SW write landed on the wrong register. */
#define RT28XX_TX_SW_CFG0               0x1330
#define RT28XX_TX_SW_CFG1               0x1334
#define RT28XX_TX_SW_CFG2               0x1338

/* TX timing / retry / protection (rt2800_init_registers).  These were missing
 * and caused the hardware auto-responder to have NO basic-rate table -> ACK
 * frames were never transmitted -> AP retransmitted and eventually gave up. */
#define RT28XX_BKOFF_SLOT_CFG           0x1104
#define RT28XX_TXOP_CTRL_CFG            0x1340
#define RT28XX_TXOP_THRES_CFG           0x133C
#define RT28XX_EDCA_AC0_CFG             0x1300
#define RT28XX_EDCA_AC1_CFG             0x1304
#define RT28XX_EDCA_AC2_CFG             0x1308
#define RT28XX_EDCA_AC3_CFG             0x130C
#define RT28XX_WMM_AIFSN_CFG            0x0214
#define RT28XX_WMM_CWMIN_CFG            0x0218
#define RT28XX_WMM_CWMAX_CFG            0x021C
#define RT28XX_WMM_TXOP0_CFG            0x0220
#define RT28XX_WMM_TXOP1_CFG            0x0224
#define RT28XX_TX_RTS_CFG               0x1344
#define RT28XX_TX_TIMEOUT_CFG           0x1348
#define RT28XX_TX_RTY_CFG               0x134c
#define RT28XX_TX_LINK_CFG              0x1350
#define RT28XX_HT_FBK_CFG0              0x1354
#define RT28XX_HT_FBK_CFG1              0x1358
#define RT28XX_LG_FBK_CFG0              0x135C
#define RT28XX_LG_FBK_CFG1              0x1360
#define RT28XX_BCN_OFFSET0              0x042C
#define RT28XX_BCN_OFFSET1              0x0430
/* EXP_ACK_TIME (0x1380): expected ACK timeout -- critical for auto-responder
 * timing.  Without it ACKs may not be generated in time. */
#define RT28XX_EXP_ACK_TIME             0x1380
/* TXOP_HLDR_ET (0x1608): TXOP holder for RT5592 = 0x82. */
#define RT28XX_TXOP_HLDR_ET             0x1608
/* TX_PWR_CFG_0..4 (rt2800.h:1115-1224): per-rate TX power, 4 bits per rate.
 * With reset-default 0 all rates transmit at MINIMUM power -> ACK/data frames
 * are inaudible to the AP.  Must be programmed to a reasonable level. */
#define RT28XX_TX_PWR_CFG_0             0x1314
#define RT28XX_TX_PWR_CFG_1             0x1318
#define RT28XX_TX_PWR_CFG_2             0x131C
#define RT28XX_TX_PWR_CFG_3             0x1320
#define RT28XX_TX_PWR_CFG_4             0x1324
#define RT28XX_CCK_PROT_CFG             0x1364
#define RT28XX_OFDM_PROT_CFG            0x1368
#define RT28XX_MM20_PROT_CFG            0x136c
#define RT28XX_MM40_PROT_CFG            0x1370
#define RT28XX_GF20_PROT_CFG            0x1374
#define RT28XX_GF40_PROT_CFG            0x1378

#define RT28XX_RX_FILTER_CFG            0x1400
#define RT28XX_AUTO_RSP_CFG             0x1404
/* LEGACY_BASIC_RATE (0x1408): defines which rates the hardware may use for
 * ACK/CTS auto-response.  Without this the auto-responder has no valid rate
 * and simply drops the ACK -> AP retransmits and eventually drops us. */
#define RT28XX_LEGACY_BASIC_RATE        0x1408
#define RT28XX_HT_BASIC_RATE            0x140c
/* XIFS_TIME_CFG (rt2800.h:907).  Bit29 BB_RXEND_ENABLE gates the BBP->MAC
 * RXEND signal: with it 0 the PHY runs but the MAC never sees a frame end,
 * so RX_STA_CNT false_cca stays 0 AND received frames are never handed to
 * the RX DMA (zero URB completions).  init_registers programs the full
 * field set (SIFS=16/16, OFDM_XIFS=4, EIFS=314, BB_RXEND_ENABLE=1). */
#define RT28XX_XIFS_TIME_CFG            0x1100
/* PWR_PIN_CFG (rt2800.h:1037): init_registers writes 0x3 (RF power pins). */
#define RT28XX_PWR_PIN_CFG              0x1204
/* CH_TIME_CFG (rt2800.h:938): channel-statistics timer.  init_registers writes
 * EIFS_BUSY|NAV_BUSY|RX_BUSY|TX_BUSY|TMR_EN = 0x1F.  Without TMR_EN the
 * CH_IDLE_STA/CH_BUSY_STA counters never advance (read back as 0). */
#define RT28XX_CH_TIME_CFG              0x110C
#define RT28XX_BCN_TIME_CFG             0x1114
#define RT28XX_TBTT_SYNC_CFG            0x1118
#define RT28XX_INT_TIMER_CFG            0x1128
#define RT28XX_INT_TIMER_EN             0x112C
#define RT28XX_US_CYC_CNT               0x02A4

// RX statistics counters (clear-on-read) - used for 0-RX diagnostics
#define RT28XX_RX_STA_CNT0              0x1700  // [15:0]CRC_ERR [31:16]PHY_ERR
#define RT28XX_RX_STA_CNT1              0x1704  // [15:0]FALSE_CCA [31:16]PLCP_ERR
// Channel-busy statistics (rt2800.h). CH_BUSY_STA counts the microseconds the
// PHY measured the medium busy during the last dwell -- a >0 value is the
// decisive proof the PHY/CCA engine is actually listening to RF energy even if
// false_cca stays 0; ==0 means the PHY front end never sees anything.
#define RT28XX_CH_IDLE_STA              0x1130
#define RT28XX_CH_BUSY_STA              0x1134

// WCID (Wireless Client ID) table registers (ref rt2800.h:2045-2090)
// Each WCID entry = 8 bytes (6 MAC + 2 reserved); attribute = 4 bytes.
#define RT28XX_MAC_WCID_BASE            0x1800
#define RT28XX_MAC_WCID_ATTR_BASE       0x6800
#define RT28XX_MAC_WCID_ENTRY(idx)      (RT28XX_MAC_WCID_BASE + ((idx) * 8))
#define RT28XX_MAC_WCID_ATTR_ENTRY(idx) (RT28XX_MAC_WCID_ATTR_BASE + ((idx) * 4))
// We use WCID 1 for the associated AP (WCID 0 is reserved/multicast).
#define RT28XX_STA_WCID                 1

// Key table / IVEIV / shared-key-mode registers (ref rt2800.h:2045-2090)
#define RT28XX_PAIRWISE_KEY_TABLE_BASE  0x4000
#define RT28XX_PAIRWISE_KEY_ENTRY(idx)  (RT28XX_PAIRWISE_KEY_TABLE_BASE + ((idx) * 32))
#define RT28XX_SHARED_KEY_TABLE_BASE    0x6C00
#define RT28XX_SHARED_KEY_ENTRY(idx)    (RT28XX_SHARED_KEY_TABLE_BASE + ((idx) * 32))
#define RT28XX_MAC_IVEIV_TABLE_BASE     0x6000
#define RT28XX_MAC_IVEIV_ENTRY(idx)     (RT28XX_MAC_IVEIV_TABLE_BASE + ((idx) * 8))
#define RT28XX_SHARED_KEY_MODE_BASE     0x7000
#define RT28XX_SHARED_KEY_MODE_ENTRY(idx) (RT28XX_SHARED_KEY_MODE_BASE + ((idx) * 4))

// MAC_WCID_ATTRIBUTE field values (ref rt2800.h:2081-2090)
#define RT28XX_WCID_ATTR_KEYTAB         0x00000001
#define RT28XX_WCID_ATTR_CIPHER_SHIFT   1
#define RT28XX_WCID_ATTR_CIPHER_MASK    0x0000000E
#define RT28XX_WCID_ATTR_WIUDF_SHIFT    7
#define RT28XX_WCID_ATTR_WIUDF_MASK     0x00000380
#define RT28XX_CIPHER_NONE              0
#define RT28XX_CIPHER_AES               4

// TXINFO_W0 WIV bit: 0 = hardware inserts IV from IVEIV, 1 = use descriptor IV.
#define RT28XX_TXINFO_W0_WIV            (1u << 24)

// MAC_SYS_CTRL bits
#define RT28XX_MAC_SRST                 (1 << 0)
#define RT28XX_BBP_HRST                 (1 << 1)
#define RT28XX_MAC_TX_EN                (1 << 2)
#define RT28XX_MAC_RX_EN                (1 << 3)

// BBP / RF indirect access encoding
//
// BBP_CSR_CFG (0x101C): [7:0]VALUE [15:8]REGNUM [16]READ_CONTROL(1=read)
//   [17]BUSY(1=kick) [19]BBP_RW_MODE(1=parallel, MUST be set for writes).
// RF_CSR_CFG (0x1020): [7:0]DATA [13:8]REGNUM(6-bit!) [16]WRITE(1=write)
//   [17]BUSY(1=kick).
// NOTE: the previous defines had WRITE/BUSY swapped for RF (BUSY=1<<16,
// WRITE=1<<17), so every RF write only set BUSY with WRITE=0 -> the chip
// treated it as a READ and silently discarded the data.  RF was never
// configured.  BBP also omitted BBP_RW_MODE.  Both are fixed here.
#define RT28XX_BBP_READ_CONTROL         (1 << 16)
#define RT28XX_BBP_BUSY                 (1 << 17)
#define RT28XX_BBP_RW_MODE              (1 << 19)
#define RT28XX_RF_WRITE                 (1 << 16)
#define RT28XX_RF_BUSY                  (1 << 17)

// RX_FILTER_CFG bits (must match rt2800.h bit positions exactly!)
#define RT28XX_FILTER_DROP_CRC_ERROR    (1 <<  0)   // 0x0001
#define RT28XX_FILTER_DROP_PHY_ERROR    (1 <<  1)   // 0x0002
#define RT28XX_FILTER_DROP_NOT_TO_ME    (1 <<  2)   // 0x0004
#define RT28XX_FILTER_DROP_NOT_MYBSS    (1 <<  3)   // 0x0008
#define RT28XX_FILTER_DROP_VER_ERROR    (1 <<  4)   // 0x0010
#define RT28XX_FILTER_DROP_MULTICAST    (1 <<  5)   // 0x0020
#define RT28XX_FILTER_DROP_BROADCAST    (1 <<  6)   // 0x0040
#define RT28XX_FILTER_DROP_DUPLICATE    (1 <<  7)   // 0x0080
#define RT28XX_FILTER_DROP_CFEND_ACK    (1 <<  8)   // 0x0100
#define RT28XX_FILTER_DROP_CFEND        (1 <<  9)   // 0x0200
#define RT28XX_FILTER_DROP_ACK          (1 << 10)   // 0x0400
#define RT28XX_FILTER_DROP_CTS          (1 << 11)   // 0x0800
#define RT28XX_FILTER_DROP_RTS          (1 << 12)   // 0x1000
#define RT28XX_FILTER_DROP_PSPOLL       (1 << 13)   // 0x2000
#define RT28XX_FILTER_DROP_BA           (1 << 14)   // 0x4000
#define RT28XX_FILTER_DROP_BAR          (1 << 15)   // 0x8000
#define RT28XX_FILTER_DROP_CNTL         (1 << 16)   // 0x10000

/*---- RT5592 extra MAC registers ----*/
#define RT28XX_LDO_CFG0                 0x05D4
/* LDO_CFG0_LDO_CORE_VLEVEL = bit26-28 (rt2800.h:689).  config_channel_rf55xx
 * sets it to 0 for 2.4 GHz / 20 MHz, 5 for 5 GHz or HT40.  We were skipping
 * this write entirely -- if the reset default leaves a non-zero core voltage
 * level the RF analog (RX0/TX0 power-down latches in RFCSR1) may never come up,
 * which matches RFCSR1 reading 0x03 (RX0_PD/TX0_PD never latch) and false_cca=0. */
#define RT28XX_LDO_CFG0_VLEVEL_MASK    0x1C000000u
#define RT28XX_MAC_DEBUG_INDEX          0x05E8
#define RT28XX_MAC_DEBUG_INDEX_XTAL     (1u << 31)  /* 1 = 40 MHz xtal */

/*---- RFCSR field bits (RF5592, from rt2x00/rt2800.h) ----*/
#define RFCSR1_RF_BLOCK_EN              0x01
#define RFCSR1_PLL_PD                   0x02
#define RFCSR1_RX0_PD                   0x04
#define RFCSR1_TX0_PD                   0x08
#define RFCSR1_RX1_PD                   0x10
#define RFCSR1_TX1_PD                   0x20
#define RFCSR3_VCOCAL_EN                0x80
#define RFCSR9_K                        0x0F
#define RFCSR9_N                        0x10
#define RFCSR9_MOD                      0x80
#define RFCSR11_R                       0x03
#define RFCSR11_MOD                     0xC0
#define RFCSR30_RX_VCM                  0x18
#define RFCSR30_RF_CALIBRATION          0x80
#define RFCSR38_RX_LO1_EN               0x20
#define RFCSR39_RX_LO2_EN               0x80
#define RFCSR49_TX                      0x3F
#define RFCSR50_TX                      0x3F
#define RT28XX_POWER_BOUND              0x27
#define RT28XX_POWER_BOUND_5G          0x2B   /* POWER_BOUND_5G (rt2800lib.c) */

/*---- BBP field bits (RF5592) ----*/
#define BBP4_MAC_IF_CTRL                0x40
#define BBP4_BANDWIDTH                  0x18
#define BBP27_RX_CHAIN_SEL              0x60
#define BBP27_RX_CHAIN1                 0x20   /* RX_CHAIN_SEL field = chain 1 */
#define BBP105_MLD                      0x04
#define BBP138_RX_ADC1                  0x02
#define BBP138_TX_DAC1                  0x20
#define BBP152_RX_DEFAULT_ANT           0x80
#define BBP254_BIT7                     0x80

/* RT5572/RT5592 is physically 2T2R; the on-chip EEPROM reports RXPATH=TXPATH=2.
 * Configuring a single chain (the original simplification) half-wires the RX
 * front end and leaves the receiver deaf.  Match the hardware: 2 chains. */
#define RT28XX_RX_CHAIN_NUM             2
#define RT28XX_TX_CHAIN_NUM             2

/*---- TX_PIN_CFG / TX_BAND_CFG : 2.4 GHz RX-path enables ----
 * TX_PIN_CFG (2T2R): PA_PE_G0(0x2) | PA_PE_G1(0x8) | LNA_PE_A0(0x100) |
 *             LNA_PE_G0(0x200) | LNA_PE_A1(0x400) | LNA_PE_G1(0x800) |
 *             RFTR_EN(0x10000) | TRSW_EN(0x40000).
 *             BOTH PA chains enabled for proper 2T2R TX power (was missing
 *             PA_PE_G1 which halved TX power).
 * TX_BAND_CFG: BG=1 (2.4 GHz), A=0, HT40_MINUS=0. */
#define RT28XX_TX_PIN_CFG_2G            0x00050F0A
#define RT28XX_TX_BAND_CFG_2G           0x00000004
/*---- TX_PIN_CFG / TX_BAND_CFG : 5 GHz (A-band) path enables ----
 * TX_PIN_CFG (2T2R): PA_PE_A0(0x1) | PA_PE_A1(0x4) | LNA_PE_A0(0x100) |
 *             LNA_PE_G0(0x200) | LNA_PE_A1(0x400) | LNA_PE_G1(0x800) |
 *             RFTR_EN(0x10000) | TRSW_EN(0x40000).  ref config_channel() turns
 *             on the A-band PAs (channel > 14) plus both LNA chains.
 * TX_BAND_CFG: A=1 (5 GHz), BG=0, HT40_MINUS=0. */
#define RT28XX_TX_PIN_CFG_5G            0x00050F05
#define RT28XX_TX_BAND_CFG_5G           0x00000002
/* LDO_CORE_VLEVEL value for 5 GHz / HT40 = 5 (bit26-28). */
#define RT28XX_LDO_CFG0_VLEVEL_5G      (5u << 26)

/*============================ OP MACROS =====================================*
 *
 *   RT_OP_REG  — direct 32-bit MAC register write
 *   RT_OP_BBP  — indirect BBP register write   (via BBP_CSR_CFG)
 *   RT_OP_RF   — indirect RF register write    (via RF_CSR_CFG)
 *==========================================================================*/

#define RT_OP_REG(r_, v_)   { (uint32_t)(r_), (uint32_t)(v_) }

#define RT_OP_BBP(r_, v_)   RT_OP_REG(RT28XX_BBP_CSR_CFG,                      \
        RT28XX_BBP_BUSY | RT28XX_BBP_RW_MODE | (((uint32_t)(r_) & 0xFF) << 8)  \
                         |  ((uint32_t)(v_) & 0xFF))

#define RT_OP_RF(r_, v_)    RT_OP_REG(RT28XX_RF_CSR_CFG,                       \
        RT28XX_RF_WRITE  | RT28XX_RF_BUSY | (((uint32_t)(r_) & 0x3F) << 8)     \
                         |  ((uint32_t)(v_) & 0xFF))

/*===================== SHADOW REGISTERS + EMIT HELPERS ======================*
 *
 * The RT5592 RF/BBP bring-up sequence is full of read-modify-write steps
 * (RFCSR1/3/9/11/30/38/39/49/50, BBP4/105/138/152, ...).  The wifi-layer
 * executor only does blind register writes, so we keep a software shadow of
 * every RF/BBP register and resolve the rmw at op-build time.  The chip
 * powers up with these registers cleared, so the shadow starts at zero and
 * is updated in the exact order the ops will execute -> it always matches
 * the intended on-chip state.
 *==========================================================================*/

static uint8_t __rt28xx_rf_shadow[64];
static uint8_t __rt28xx_bbp_shadow[256];

/* RF frequency offset (RFCSR17_CODE) decoded from EEPROM_FREQ during bring-up.
 * 0 = not yet read / not programmed, in which case the channel script leaves
 * RFCSR17 alone (matches the previous "freq offset defaulted to 0" behaviour).
 * Defined here so __rt28xx_emit_channel() (above the eFuse chain) can read it. */
static uint8_t __rt28xx_freq_offset;

/* TX IQ calibration values decoded from EEPROM (ref rt2800_iq_calibrate).
 * 2.4 GHz only for now (the AP under test is 2.4 GHz).  __rt28xx_iq_valid is
 * set once the eFuse IQ block has been read; until then emit_channel skips
 * the IQ writes and leaves the BBP IQ registers at their init defaults. */
static bool    __rt28xx_iq_valid;
static uint8_t __rt28xx_iq_gain_tx0;    /* byte 0x130, BBP158=0x2c */
static uint8_t __rt28xx_iq_phase_tx0;   /* byte 0x131, BBP158=0x2d */
static uint8_t __rt28xx_iq_gain_tx1;    /* byte 0x133, BBP158=0x4a */
static uint8_t __rt28xx_iq_phase_tx1;   /* byte 0x134, BBP158=0x4b */
static uint8_t __rt28xx_iq_rf_comp;     /* byte 0x13C, BBP158=0x04 (0xff->0) */
static uint8_t __rt28xx_iq_rf_imbal;    /* byte 0x13D, BBP158=0x03 (0xff->0) */

/* Scratch big enough for the whole init sequence (~270 ops incl. the 84-entry
 * GLRT table) and for a single channel switch (~75 ops).  init / set_channel /
 * connect never run concurrently (the executor is single-flight), so they can
 * share one static buffer. */
static vsf_wifi_reg_op_t __rt28xx_ops_buf[384];

static int __emit_rf(vsf_wifi_reg_op_t *ops, int n, uint8_t reg, uint8_t val)
{
    __rt28xx_rf_shadow[reg] = val;
    ops[n] = (vsf_wifi_reg_op_t)RT_OP_RF(reg, val);
    return n + 1;
}

static int __emit_rf_rmw(vsf_wifi_reg_op_t *ops, int n, uint8_t reg,
        uint8_t mask, uint8_t val)
{
    uint8_t v = (uint8_t)((__rt28xx_rf_shadow[reg] & ~mask) | (val & mask));
    return __emit_rf(ops, n, reg, v);
}

static int __emit_bbp(vsf_wifi_reg_op_t *ops, int n, uint8_t reg, uint8_t val)
{
    __rt28xx_bbp_shadow[reg] = val;
    ops[n] = (vsf_wifi_reg_op_t)RT_OP_BBP(reg, val);
    return n + 1;
}

static int __emit_bbp_rmw(vsf_wifi_reg_op_t *ops, int n, uint8_t reg,
        uint8_t mask, uint8_t val)
{
    uint8_t v = (uint8_t)((__rt28xx_bbp_shadow[reg] & ~mask) | (val & mask));
    return __emit_bbp(ops, n, reg, v);
}

/* GLRT (Generalized Likelihood Ratio Test) table, BBP indices 128..211,
 * written through the BBP195 (index) / BBP196 (value) window.  Verbatim
 * from rt2800_init_bbp_5592_glrt(). */
static const uint8_t __rt28xx_glrt_5592[] = {
    0xE0, 0x1F, 0x38, 0x32, 0x08, 0x28, 0x19, 0x0A, 0xFF, 0x00, /* 128~137 */
    0x16, 0x10, 0x10, 0x0B, 0x36, 0x2C, 0x26, 0x24, 0x42, 0x36, /* 138~147 */
    0x30, 0x2D, 0x4C, 0x46, 0x3D, 0x40, 0x3E, 0x42, 0x3D, 0x40, /* 148~157 */
    0x3C, 0x34, 0x2C, 0x2F, 0x3C, 0x35, 0x2E, 0x2A, 0x49, 0x41, /* 158~167 */
    0x36, 0x31, 0x30, 0x30, 0x0E, 0x0D, 0x28, 0x21, 0x1C, 0x16, /* 168~177 */
    0x50, 0x4A, 0x43, 0x40, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, /* 178~187 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 188~197 */
    0x00, 0x00, 0x7D, 0x14, 0x32, 0x2C, 0x36, 0x4C, 0x43, 0x2C, /* 198~207 */
    0x2E, 0x36, 0x30, 0x6E,                                     /* 208~211 */
};

/*=========================== RT5592 BBP INIT ================================*
 * Port of rt2800_init_bbp_5592() (+ init_bbp_early + glrt + freq_calib).
 * Single RX/TX chain, antenna 0 (no EEPROM NIC_CONF).  Revision-aware: the
 * RT5592C+ tail differs from pre-C (see __rt28xx_is_5592c).
 *==========================================================================*/
/* Captured from the MCU-ready poll (ASIC_VER_ID).  Low 16 bits = revision;
 * used to select the RT5592C+ vs pre-C BBP/RFCSR init tail. */
static uint32_t __rt28xx_asic_ver;
static inline bool __rt28xx_is_5592c(void)
{
    return (__rt28xx_asic_ver & 0xFFFFu) >= RT28XX_REV_RT5592C;
}

static int __rt28xx_build_bbp(vsf_wifi_reg_op_t *ops, int n)
{
    int i;

    /* init_bbp_early */
    n = __emit_bbp(ops, n, 65, 0x2C);
    n = __emit_bbp(ops, n, 66, 0x38);
    n = __emit_bbp(ops, n, 68, 0x0B);
    n = __emit_bbp(ops, n, 69, 0x12);
    n = __emit_bbp(ops, n, 70, 0x0A);
    n = __emit_bbp(ops, n, 73, 0x10);
    n = __emit_bbp(ops, n, 81, 0x37);
    n = __emit_bbp(ops, n, 82, 0x62);
    n = __emit_bbp(ops, n, 83, 0x6A);
    n = __emit_bbp(ops, n, 84, 0x99);
    n = __emit_bbp(ops, n, 86, 0x00);
    n = __emit_bbp(ops, n, 91, 0x04);
    n = __emit_bbp(ops, n, 92, 0x00);
    n = __emit_bbp(ops, n, 103, 0x00);
    n = __emit_bbp(ops, n, 105, 0x05);
    n = __emit_bbp(ops, n, 106, 0x35);

    /* BBP105 MLD = (rx_chain == 2); single chain -> 0 (overwritten to 0x3C) */
    n = __emit_bbp_rmw(ops, n, 105, BBP105_MLD, 0);
    n = __emit_bbp_rmw(ops, n, 4, BBP4_MAC_IF_CTRL, BBP4_MAC_IF_CTRL);

    n = __emit_bbp(ops, n, 20, 0x06);
    n = __emit_bbp(ops, n, 31, 0x08);
    n = __emit_bbp(ops, n, 65, 0x2C);
    n = __emit_bbp(ops, n, 68, 0xDD);
    n = __emit_bbp(ops, n, 69, 0x1A);
    n = __emit_bbp(ops, n, 70, 0x05);
    n = __emit_bbp(ops, n, 73, 0x13);
    n = __emit_bbp(ops, n, 74, 0x0F);
    n = __emit_bbp(ops, n, 75, 0x4F);
    n = __emit_bbp(ops, n, 76, 0x28);
    n = __emit_bbp(ops, n, 77, 0x59);
    n = __emit_bbp(ops, n, 84, 0x9A);
    n = __emit_bbp(ops, n, 86, 0x38);
    n = __emit_bbp(ops, n, 88, 0x90);
    n = __emit_bbp(ops, n, 91, 0x04);
    n = __emit_bbp(ops, n, 92, 0x02);
    n = __emit_bbp(ops, n, 95, 0x9A);
    n = __emit_bbp(ops, n, 98, 0x12);
    n = __emit_bbp(ops, n, 103, 0xC0);
    n = __emit_bbp(ops, n, 104, 0x92);
    n = __emit_bbp(ops, n, 105, 0x3C);
    n = __emit_bbp(ops, n, 106, 0x35);
    n = __emit_bbp(ops, n, 128, 0x12);
    n = __emit_bbp(ops, n, 134, 0xD0);
    n = __emit_bbp(ops, n, 135, 0xF6);
    n = __emit_bbp(ops, n, 137, 0x0F);

    /* GLRT */
    for (i = 0; i < (int)dimof(__rt28xx_glrt_5592); i++) {
        n = __emit_bbp(ops, n, 195, (uint8_t)(128 + i));
        n = __emit_bbp(ops, n, 196, __rt28xx_glrt_5592[i]);
    }

    n = __emit_bbp_rmw(ops, n, 4, BBP4_MAC_IF_CTRL, BBP4_MAC_IF_CTRL);
    /* antenna 0 -> RX_DEFAULT_ANT = 1 */
    n = __emit_bbp_rmw(ops, n, 152, BBP152_RX_DEFAULT_ANT, BBP152_RX_DEFAULT_ANT);
    /* RT5592C+ only: set BBP254 bit7 (rt2800_init_bbp_5592:7028). */
    if (__rt28xx_is_5592c()) {
        n = __emit_bbp_rmw(ops, n, 254, BBP254_BIT7, BBP254_BIT7);
    }
    /* init_freq_calibration */
    n = __emit_bbp(ops, n, 142, 0x01);
    n = __emit_bbp(ops, n, 143, 0x39);
    n = __emit_bbp(ops, n, 84, 0x19);
    /* RT5592C+ only: re-assert BBP103=0xC0 (rt2800_init_bbp_5592:7038). */
    if (__rt28xx_is_5592c()) {
        n = __emit_bbp(ops, n, 103, 0xC0);
    }
    /* ---- BBP1 / BBP3 : TX & RX antenna chain config (init_bbp_5592:7055-7060
     * + config_ant for 2T2R).  BBP1=0x50 matches the Windows native driver and
     * is required for CCK short-preamble EAPOL-Key M1 reception; 0x14 causes
     * the PHY to miss M1 and the 4-way handshake times out. ---- */
    n = __emit_bbp(ops, n, 1, 0x50);
    n = __emit_bbp(ops, n, 3, 0x08);   /* RX_ANTENNA=1 (both chains active)   */
    return n;
}

/*=========================== RT5592 RFCSR INIT ==============================*
 * Port of rt2800_init_rfcsr_5592() incl. rf_init_calibration(30) and
 * normal_mode_setup_5xxx().  freq_cal_mode1() is skipped (USB MCU command,
 * frequency offset defaulted to 0).  Pre-RT5592C revision assumed.
 *==========================================================================*/
static int __rt28xx_build_rfcsr(vsf_wifi_reg_op_t *ops, int n)
{
    /* rf_init_calibration(30): pulse RFCSR30 bit7 (RF_CALIBRATION) */
    n = __emit_rf_rmw(ops, n, 30, RFCSR30_RF_CALIBRATION, RFCSR30_RF_CALIBRATION);
    n = __emit_rf_rmw(ops, n, 30, RFCSR30_RF_CALIBRATION, 0);

    n = __emit_rf(ops, n, 1, 0x3F);
    n = __emit_rf(ops, n, 3, 0x08);
    n = __emit_rf(ops, n, 5, 0x10);
    n = __emit_rf(ops, n, 6, 0xE4);
    n = __emit_rf(ops, n, 7, 0x00);
    n = __emit_rf(ops, n, 14, 0x00);
    n = __emit_rf(ops, n, 15, 0x00);
    n = __emit_rf(ops, n, 16, 0x00);
    n = __emit_rf(ops, n, 18, 0x03);
    n = __emit_rf(ops, n, 19, 0x4D);
    n = __emit_rf(ops, n, 20, 0x10);
    n = __emit_rf(ops, n, 21, 0x8D);
    n = __emit_rf(ops, n, 26, 0x82);
    n = __emit_rf(ops, n, 28, 0x00);
    n = __emit_rf(ops, n, 29, 0x10);
    n = __emit_rf(ops, n, 33, 0xC0);
    n = __emit_rf(ops, n, 34, 0x07);
    n = __emit_rf(ops, n, 35, 0x12);
    n = __emit_rf(ops, n, 47, 0x0C);
    n = __emit_rf(ops, n, 53, 0x22);
    n = __emit_rf(ops, n, 63, 0x07);

    n = __emit_rf(ops, n, 2, 0x80);

    /* RT5592C+ only: "Enable DC filter" BBP103=0xC0 (init_rfcsr_5592:8494). */
    if (__rt28xx_is_5592c()) {
        n = __emit_bbp(ops, n, 103, 0xC0);
    }

    /* normal_mode_setup_5xxx(): for a 2T2R part (RXPATH==TXPATH==2) the
     * reference leaves BBP138 untouched -- both ADCs and DACs stay on.  Only a
     * single-chain EEPROM would clear RX_ADC1 / set TX_DAC1, which we must NOT
     * do here or chain-1 RX is killed. */
    n = __emit_rf_rmw(ops, n, 38, RFCSR38_RX_LO1_EN, 0);
    n = __emit_rf_rmw(ops, n, 39, RFCSR39_RX_LO2_EN, 0);
    n = __emit_bbp_rmw(ops, n, 4, BBP4_MAC_IF_CTRL, BBP4_MAC_IF_CTRL);
    n = __emit_rf_rmw(ops, n, 30, RFCSR30_RX_VCM, (uint8_t)(2 << 3));

    /* pre-RT5592C only: RFCSR27=0x03 (init_rfcsr_5592:8499).  On RT5592C+ this
     * write is omitted -- programming it there mis-sets the RF and was a real
     * bug in the original (revision-blind) port. */
    if (!__rt28xx_is_5592c()) {
        n = __emit_rf(ops, n, 27, 0x03);
    }
    return n;
}

/*============================== INIT BUILDER ================================*/
static int __rt28xx_build_init(vsf_wifi_reg_op_t *ops)
{
    int n = 0;

    /* Chip powers up with RF/BBP registers cleared; mirror that so the rmw
     * helpers resolve correctly across this rebuild. */
    memset(__rt28xx_rf_shadow,  0, sizeof(__rt28xx_rf_shadow));
    memset(__rt28xx_bbp_shadow, 0, sizeof(__rt28xx_bbp_shadow));

    /* ---- MAC/BBP reset + BBP-ready poll are performed separately in
     * __rt28xx_init BEFORE this builder runs (mirrors rt2800_enable_radio:
     * wait_bbp_ready precedes init_bbp).  Pulsing BBP_HRST here would knock
     * the BBP back out of service right before we configure it. ---- */
    /* ---- USB / DMA bring-up (see Task1 notes: open both bulk pipes) ----
     * USB_DMA_CFG: TX_BULK_EN(b23)|RX_BULK_EN(b22)|AGG_TIMEOUT=128, AGG off.
     * PHY_CLEAR(b16) MUST be 0 (=1 halts the PHY->USB RX DMA path). */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_USB_DMA_CFG,   0x00C00080);
    /* PBF_CFG (USB): rt2800_init_registers writes 0xf40006 -- enables the
     * packet-buffer FIFO queues that feed the MAC->USB RX bulk path. */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_PBF_CFG,       0x00F40006);
    /* WPDMA_GLO_CFG init value (rt2800_init_registers USB branch:6175): DMA
     * disabled, WP_DMA_BURST_SIZE=3 (0x30).  enable_radio later turns the DMA
     * engines on (see tail below). */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_WPDMA_GLO_CFG, 0x00000030);
    /* ---- MAC defaults (TX_PIN/TX_BAND are programmed per channel) ---- */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_MAC_MAX_LEN_CFG,     0x0FFF0FFF);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_LED_CFG,             0x7F031E46);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_AMPDU_MAX_LEN_20M1S, 0x0000A8FF);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_AMPDU_MAX_LEN_40M1S, 0x000108EB);
    /* RT5592 TX_SW_CFG: match Windows native driver capture (ref/rt5572_win_usb.log
     * reg 0x1330=0x00000404, 0x1334=0x00080606, 0x1338=0).  CFG1=0x00080606 sets
     * the PA switch on/off timing; leaving it 0 (the generic rt2800 default) can
     * leave the PA mis-timed during TX so the on-air frame is malformed and the
     * AP never ACKs it -- consistent with TX_STA_FIFO ack_ok=0 on every frame. */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_TX_SW_CFG0,          0x00000404);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_TX_SW_CFG1,          0x00080606);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_TX_SW_CFG2,          0x00000000);
    /* ---- BASIC RATE tables (rt2800_init_registers:5864-5865) ----
     * LEGACY_BASIC_RATE tells the auto-responder which rates are valid for
     * ACK/CTS.  Without it, AUTO_RSP has no rate -> ACK never transmitted
     * -> AP retransmits and eventually drops us.  THIS WAS THE ROOT CAUSE
     * of the "AP goes silent after 4-way" bug. */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_LEGACY_BASIC_RATE,    0x0000013F);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_HT_BASIC_RATE,        0x00008003);
    /* ---- Backoff / Slot timing (rt2800_init_registers:5880) ---- */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_BKOFF_SLOT_CFG,       0x00000209);
    /* ---- TX retry config: match Windows capture 0x47D01F1F (reg 0x134C).
     * SHORT_RTY=0x1F(31), LONG_RTY=0x1F(31): far more aggressive than our prior
     * 0x47D00407 (7/4).  More on-air retries per frame raise the odds the AP
     * actually hears one of them. */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_TX_RTY_CFG,           0x47D01F1F);
    /* ---- TX timeout (rt2800_init_registers:6041) ----
     * bits[7:0]=RX_ACK_TIMEOUT=0x20(32), bits[15:8]=TX_OP_TIMEOUT=0x0A(10),
     * bits[19:16]=MPDU_LIFETIME=9.  Previous value 0x000A2090 had the fields
     * in the wrong positions (RX_ACK_TIMEOUT=144!) causing excessively long
     * post-TX wait states. */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_TX_TIMEOUT_CFG,       0x00090A20);
    /* ---- TX link config (rt2800_init_registers:6030) ----
     * MFB_LIFETIME=32, TX_CF_ACK_EN=1. */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_TX_LINK_CFG,          0x00001020);
    /* ---- TX power per-rate ----
     * Match the Windows native driver's TX_PWR_CFG values captured while
     * successfully connecting to the SAME AP (ref/rt5572_win_usb.log, regs
     * 0x1314-0x1324 reassembled from 16-bit halves):
     *   CFG0=0xAAAA6666 CFG1/2/3=0xAAAA6688 CFG4=0xFFFF6688.
     * Our previous 0x0C0C0C0C was a guess: its nibble layout (0,C,0,C) is not
     * even a valid per-rate power map, and the hardware TX_STA_FIFO showed
     * EVERY uplink frame to the AP going un-ACKed (ack_ok=0 x34, mcs=0).  The
     * Windows map uses regular nibbles (6/8 for CCK/OFDM, A/F for HT) which is
     * the real calibrated layout for this dongle. */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_TX_PWR_CFG_0,        0xAAAA6666);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_TX_PWR_CFG_1,        0xAAAA6688);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_TX_PWR_CFG_2,        0xAAAA6688);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_TX_PWR_CFG_3,        0xAAAA6688);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_TX_PWR_CFG_4,        0xFFFF6688);
    /* ---- Protection configs (rt2800_init_registers:6094-6170) ---- */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_CCK_PROT_CFG,         0x01740003);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_OFDM_PROT_CFG,        0x01740003);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_MM20_PROT_CFG,        0x01654004);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_MM40_PROT_CFG,        0x03E54084);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_GF20_PROT_CFG,        0x01654004);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_GF40_PROT_CFG,        0x03E54084);
    /* AUTO_RSP_CFG (0x1404): match the Windows native driver's runtime value
     * 0x13 = AUTORESPONDER | BAC_ACK_POLICY | AR_PREAMBLE(short).  The Windows
     * USB capture writes 0x13 every time (ref/rt5572_win_usb.log
     * L2121/7876/8220/17615/18074), whereas the Linux rt2800 init writes 0x07
     * (AUTORESPONDER|BAC_ACK_POLICY|CTS_40_MMODE).  Two differences vs our old
     * 0x07: (1) drop bit2 CTS_40_MMODE -- the target network is 20MHz on ch1 so
     * 40MHz CTS duplicate mode should be off; (2) set bit4 AR_PREAMBLE=1 so the
     * hardware auto-ACK uses short preamble, which the AP expects.  A
     * long-preamble auto-ACK the AP can't decode would explain the observed
     * symptom: AP keeps retransmitting auth/assoc-resp and never advances to
     * EAPOL M1 (handshake timeout). */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_AUTO_RSP_CFG,        0x00000013);
    /* ---- TXOP_CTRL_CFG: match Windows capture 0x0000243F (reg 0x1340).
     * Previously disabled "for regression testing"; the Windows native driver
     * that successfully connects to this AP sets it, so re-enable with its
     * exact value. ---- */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_TXOP_CTRL_CFG,        0x0000243F);
    // ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_TXOP_HLDR_ET,         0x00000082);
    /* ---- TX_RTS_CFG: match Windows capture 0x01092B20 (reg 0x1344).
     * RTS_THRES=0x92B, AUTO_RTS_RETRY=0x20. ---- */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_TX_RTS_CFG,           0x01092B20);
    /* ---- EXP_ACK_TIME: match Windows capture 0x002C00DC (reg 0x1380).
     * Governs the expected-ACK timing window for the auto-responder. ---- */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_EXP_ACK_TIME,         0x002C00DC);
    /* ---- Clear SHARED_KEY_MODE (rt2800_init_registers:6242-6243): garbage
     * in these registers from a previous run may cause the hardware to attempt
     * decryption with non-existent keys -> frames rejected -> no ACK. ---- */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(0x7000, 0);  /* SHARED_KEY_MODE_ENTRY(0) */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(0x7004, 0);  /* SHARED_KEY_MODE_ENTRY(1) */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(0x7008, 0);  /* SHARED_KEY_MODE_ENTRY(2) */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(0x700C, 0);  /* SHARED_KEY_MODE_ENTRY(3) */
    /* WCID 0 is the hardware default for RX frames that don't match another
     * WCID entry (e.g. unencrypted EAPOL-Key M1/M3 before the pairwise key
     * is installed).  Leave its KEYTAB bit set so the cipher engine does not
     * silently drop those frames.  Clear WCID 1 attribute; connect() will set
     * it for the AP once association succeeds. */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(0x6800, 0x00000001u);  /* MAC_WCID_ATTR_ENTRY(0): KEYTAB=1 */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(0x6804, 0);  /* MAC_WCID_ATTR_ENTRY(1) */
    /* Clear MAC_IVEIV_ENTRY(0..1): the IV/EIV per-WCID state used by the
     * cipher engine.  rt2800_init_registers (rt2800lib.c:6256-6257) clears
     * all 256 entries on probe, but at minimum WCID 0/1 must be zeroed.
     *
     * Empirical: with KEYTAB=0, garbage IVEIV state, the chip RXes our
     * unicast EAPOL frames (PHY auto-ACKs them) but silently DROPs them in
     * the cipher engine before USB DMA -- driver sees 0 type=2 frames for
     * our MAC even with RX_FILTER=0 (promiscuous), while frames for OTHER
     * MACs (no WCID match -> no cipher engine) come through fine.  Each
     * IVEIV entry is 8 bytes (2 x 32-bit regs at 0x6000+8*wcid). */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(0x6000, 0);  /* MAC_IVEIV_ENTRY(0).iv */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(0x6004, 0);  /* MAC_IVEIV_ENTRY(0).eiv */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(0x6008, 0);  /* MAC_IVEIV_ENTRY(1).iv */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(0x600C, 0);  /* MAC_IVEIV_ENTRY(1).eiv */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_BCN_TIME_CFG,        0x00006400);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_TBTT_SYNC_CFG,       0x00000020);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_INT_TIMER_CFG,       0x00000000);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_INT_TIMER_EN,        0x00000000);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_US_CYC_CNT,          0x0000001E);
    /* CH_TIME_CFG (rt2800_init_registers:6366): enable the channel-statistics
     * timer (EIFS/NAV/RX/TX busy sources + TMR_EN = 0x1F).  Without it the
     * CH_IDLE_STA/CH_BUSY_STA counters never run -- this was a real omission. */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_CH_TIME_CFG,        0x0000001F);
    /* ---- RX_FILTER_CFG: drop CRC/PHY/NOT_TO_ME errors, keep bc/mc.
     * DROP_NOT_TO_ME(bit2)=1 is CRITICAL: without it, the USB is flooded with
     * all other stations' frames, wasting bandwidth.  Linux driver sets this in
     * non-monitor mode. ---- */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_RX_FILTER_CFG,       0x00017F97);
    /* ---- XIFS_TIME_CFG: SIFS=16/16, OFDM_XIFS=4, EIFS=314, and crucially
     * BB_RXEND_ENABLE(bit29)=1.  Without BB_RXEND_ENABLE the BBP never raises
     * RXEND to the MAC -> false_cca counter stuck at 0 and no RX frame is
     * ever DMA'd up (zero URB completions).  (rt2800_init_registers:6235) */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_XIFS_TIME_CFG,      0x33A41010);
    /* ---- PWR_PIN_CFG=0x3 (rt2800_init_registers:6237): RF power pins. ---- */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_PWR_PIN_CFG,        0x00000003);
    /* ---- MCU_BOOT_SIGNAL: "Send signal during boot time to initialize
     * firmware" (rt2800_enable_radio:10810-10815).  This MUST precede
     * init_bbp/init_rfcsr -- without it the RF subsystem never fully wakes
     * (RFCSR1 RX0/TX0_PD refuse to latch, false_cca stays 0).  mcu_request()
     * encodes the command as: H2M_MAILBOX_CSR(OWNER=1,token/args=0) then
     * HOST_CMD_CSR(HOST_COMMAND=0x72).  The ep0 round-trips cover msleep(1). */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_H2M_BBP_AGENT,   0x00000000);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_H2M_MAILBOX_CSR, 0x00000000);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_H2M_INT_SRC,     0x00000000);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_H2M_MAILBOX_CSR, RT28XX_H2M_MAILBOX_OWNER);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_HOST_CMD_CSR,    RT28XX_MCU_BOOT_SIGNAL);
    /* ---- BBP + RF (RT5592) ---- */
    n = __rt28xx_build_bbp(ops, n);
    n = __rt28xx_build_rfcsr(ops, n);
    /* ---- enable radio (mirror rt2800_enable_radio:10841): MAC TX-only ->
     * WPDMA TX_DMA|RX_DMA|TX_WRITEBACK (0x30 burst | 0x01 | 0x04 | 0x40 =
     * 0x75) -> MAC TX|RX.  The natural ep0 round-trip latency covers the
     * udelay(50) between the WPDMA enable and the RX enable. ---- */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_MAC_SYS_CTRL, RT28XX_MAC_TX_EN);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_WPDMA_GLO_CFG, 0x00000075);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_MAC_SYS_CTRL, RT28XX_MAC_TX_EN | RT28XX_MAC_RX_EN);
    return n;
}

/* Firmware-download handshake, split to match ref rt2800usb_write_firmware
 * (rt2800usb.c:246-262) ordering exactly:
 *   1. clear CID/STATUS mailboxes to ~0  (BEFORE the vendor request)
 *   2. USB_MODE_FIRMWARE vendor request  (issued separately)
 *   3. clear H2M_MAILBOX_CSR to 0        (AFTER the vendor request)
 * Previously CID/STATUS were cleared AFTER the vendor request, and an extra
 * PBF_SYS_CTRL=0 write was issued -- neither matches the reference and the
 * mis-ordered mailbox clear can leave the MCU stuck. */
static const vsf_wifi_reg_op_t __rt28xx_pre_fw_script[] = {
    RT_OP_REG(RT28XX_H2M_MAILBOX_CID,    0xFFFFFFFF),
    RT_OP_REG(RT28XX_H2M_MAILBOX_STATUS, 0xFFFFFFFF),
};
static const vsf_wifi_reg_op_t __rt28xx_post_fw_script[] = {
    RT_OP_REG(RT28XX_H2M_MAILBOX_CSR,  0x00000000),
};

/*============================ CHANNEL TABLE =================================*
 * RF5592 frequency-synthesiser values {channel, N, K, mod, R}, selected by the
 * on-chip crystal (20 vs 40 MHz, see __rt28xx_xtal40).  Both 2.4 GHz (ch 1-14)
 * and 5 GHz (ch 36-196) are populated.  Verbatim from rf_vals_5592_xtal20 /
 * rf_vals_5592_xtal40 (rt2800lib.c).  Channel numbers in the 5 GHz band are
 * not contiguous, so entries are looked up by channel rather than indexed.
 *==========================================================================*/

typedef struct {
    uint16_t channel;
    uint16_t n;
    uint8_t  k;
    uint8_t  mod;
    uint8_t  r;
} rt28xx_rf_channel_t;

static const rt28xx_rf_channel_t __rf_vals_5592_xtal20[] = {
    {1, 482, 4, 10, 3}, {2, 483, 4, 10, 3}, {3, 484, 4, 10, 3},
    {4, 485, 4, 10, 3}, {5, 486, 4, 10, 3}, {6, 487, 4, 10, 3},
    {7, 488, 4, 10, 3}, {8, 489, 4, 10, 3}, {9, 490, 4, 10, 3},
    {10, 491, 4, 10, 3}, {11, 492, 4, 10, 3}, {12, 493, 4, 10, 3},
    {13, 494, 4, 10, 3}, {14, 496, 8, 10, 3},
    {36, 172, 8, 12, 1}, {38, 173, 0, 12, 1}, {40, 173, 4, 12, 1},
    {42, 173, 8, 12, 1}, {44, 174, 0, 12, 1}, {46, 174, 4, 12, 1},
    {48, 174, 8, 12, 1}, {50, 175, 0, 12, 1}, {52, 175, 4, 12, 1},
    {54, 175, 8, 12, 1}, {56, 176, 0, 12, 1}, {58, 176, 4, 12, 1},
    {60, 176, 8, 12, 1}, {62, 177, 0, 12, 1}, {64, 177, 4, 12, 1},
    {100, 183, 4, 12, 1}, {102, 183, 8, 12, 1}, {104, 184, 0, 12, 1},
    {106, 184, 4, 12, 1}, {108, 184, 8, 12, 1}, {110, 185, 0, 12, 1},
    {112, 185, 4, 12, 1}, {114, 185, 8, 12, 1}, {116, 186, 0, 12, 1},
    {118, 186, 4, 12, 1}, {120, 186, 8, 12, 1}, {122, 187, 0, 12, 1},
    {124, 187, 4, 12, 1}, {126, 187, 8, 12, 1}, {128, 188, 0, 12, 1},
    {130, 188, 4, 12, 1}, {132, 188, 8, 12, 1}, {134, 189, 0, 12, 1},
    {136, 189, 4, 12, 1}, {138, 189, 8, 12, 1}, {140, 190, 0, 12, 1},
    {149, 191, 6, 12, 1}, {151, 191, 10, 12, 1}, {153, 192, 2, 12, 1},
    {155, 192, 6, 12, 1}, {157, 192, 10, 12, 1}, {159, 193, 2, 12, 1},
    {161, 193, 6, 12, 1}, {165, 194, 2, 12, 1}, {184, 164, 0, 12, 1},
    {188, 164, 4, 12, 1}, {192, 165, 8, 12, 1}, {196, 166, 0, 12, 1},
};

static const rt28xx_rf_channel_t __rf_vals_5592_xtal40[] = {
    {1, 241, 2, 10, 3}, {2, 241, 7, 10, 3}, {3, 242, 2, 10, 3},
    {4, 242, 7, 10, 3}, {5, 243, 2, 10, 3}, {6, 243, 7, 10, 3},
    {7, 244, 2, 10, 3}, {8, 244, 7, 10, 3}, {9, 245, 2, 10, 3},
    {10, 245, 7, 10, 3}, {11, 246, 2, 10, 3}, {12, 246, 7, 10, 3},
    {13, 247, 2, 10, 3}, {14, 248, 4, 10, 3},
    {36, 86, 4, 12, 1}, {38, 86, 6, 12, 1}, {40, 86, 8, 12, 1},
    {42, 86, 10, 12, 1}, {44, 87, 0, 12, 1}, {46, 87, 2, 12, 1},
    {48, 87, 4, 12, 1}, {50, 87, 6, 12, 1}, {52, 87, 8, 12, 1},
    {54, 87, 10, 12, 1}, {56, 88, 0, 12, 1}, {58, 88, 2, 12, 1},
    {60, 88, 4, 12, 1}, {62, 88, 6, 12, 1}, {64, 88, 8, 12, 1},
    {100, 91, 8, 12, 1}, {102, 91, 10, 12, 1}, {104, 92, 0, 12, 1},
    {106, 92, 2, 12, 1}, {108, 92, 4, 12, 1}, {110, 92, 6, 12, 1},
    {112, 92, 8, 12, 1}, {114, 92, 10, 12, 1}, {116, 93, 0, 12, 1},
    {118, 93, 2, 12, 1}, {120, 93, 4, 12, 1}, {122, 93, 6, 12, 1},
    {124, 93, 8, 12, 1}, {126, 93, 10, 12, 1}, {128, 94, 0, 12, 1},
    {130, 94, 2, 12, 1}, {132, 94, 4, 12, 1}, {134, 94, 6, 12, 1},
    {136, 94, 8, 12, 1}, {138, 94, 10, 12, 1}, {140, 95, 0, 12, 1},
    {149, 95, 9, 12, 1}, {151, 95, 11, 12, 1}, {153, 96, 1, 12, 1},
    {155, 96, 3, 12, 1}, {157, 96, 5, 12, 1}, {159, 96, 7, 12, 1},
    {161, 96, 9, 12, 1}, {165, 97, 1, 12, 1}, {184, 82, 0, 12, 1},
    {188, 82, 4, 12, 1}, {192, 82, 8, 12, 1}, {196, 83, 0, 12, 1},
};

/* Set from MAC_DEBUG_INDEX bit31 during bring-up (1 = 40 MHz crystal). */
static uint8_t __rt28xx_xtal40;

/* Look up the RF synth values for a channel in the active crystal table.
 * Returns NULL for channels not present in the table. */
static const rt28xx_rf_channel_t *__rt28xx_find_rf(uint8_t channel)
{
    const rt28xx_rf_channel_t *tbl;
    uint32_t i, cnt;
    if (__rt28xx_xtal40) {
        tbl = __rf_vals_5592_xtal40;
        cnt = dimof(__rf_vals_5592_xtal40);
    } else {
        tbl = __rf_vals_5592_xtal20;
        cnt = dimof(__rf_vals_5592_xtal20);
    }
    for (i = 0; i < cnt; i++) {
        if (tbl[i].channel == channel) {
            return &tbl[i];
        }
    }
    return NULL;
}

/*============================ HELPERS =======================================*/

/* Port of rt2800_config_channel_rf55xx() (both bands) + the RF5592 common
 * tail of rt2800_config_channel().  2T2R, lna_gain 0, 20 MHz OFDM (not 11b),
 * TX power clamped to POWER_BOUND / POWER_BOUND_5G.  freq_cal_mode1 and
 * iq_calibrate are skipped (MCU command / EEPROM dependent).  BBP3 untouched
 * (HT40_MINUS already 0). */
static int __rt28xx_emit_channel(vsf_wifi_reg_op_t *ops, int n, uint8_t channel)
{
    const rt28xx_rf_channel_t *rf;
    uint8_t rfcsr9, power_bound;
    bool is_5g;

    rf = __rt28xx_find_rf(channel);
    if (NULL == rf) {
        channel = 1;
        rf = __rt28xx_find_rf(1);
    }
    is_5g = (channel > 14);
    power_bound = is_5g ? RT28XX_POWER_BOUND_5G : RT28XX_POWER_BOUND;

    /* N / K / mod / R -> RFCSR8/9/11 (common prefix) */
    n = __emit_rf(ops, n, 8, (uint8_t)(rf->n & 0xFF));
    rfcsr9 = (uint8_t)(rf->k & RFCSR9_K);
    if (rf->n & 0x100)               { rfcsr9 |= RFCSR9_N; }
    if (((rf->mod - 8) & 0x04) >> 2) { rfcsr9 |= RFCSR9_MOD; }
    n = __emit_rf_rmw(ops, n, 9, RFCSR9_K | RFCSR9_N | RFCSR9_MOD, rfcsr9);

    if (!is_5g) {
        uint8_t rf2359;
        /* RFCSR11 R/MOD are immediately overwritten by 0x4A on 2.4 GHz */
        n = __emit_rf(ops, n, 11, 0x4A);

        /* 2.4 GHz fixed RF block */
        n = __emit_rf(ops, n, 10, 0x90);
        n = __emit_rf(ops, n, 12, 0x52);
        n = __emit_rf(ops, n, 13, 0x42);
        n = __emit_rf(ops, n, 22, 0x40);
        n = __emit_rf(ops, n, 24, 0x4A);
        n = __emit_rf(ops, n, 25, 0x80);
        n = __emit_rf(ops, n, 27, 0x42);
        n = __emit_rf(ops, n, 36, 0x80);
        n = __emit_rf(ops, n, 37, 0x08);
        n = __emit_rf(ops, n, 38, 0x89);
        n = __emit_rf(ops, n, 39, 0x1B);
        n = __emit_rf(ops, n, 40, 0x0D);
        n = __emit_rf(ops, n, 41, 0x9B);
        n = __emit_rf(ops, n, 42, 0xD5);
        n = __emit_rf(ops, n, 43, 0x72);
        n = __emit_rf(ops, n, 44, 0x0E);
        n = __emit_rf(ops, n, 45, 0xA2);
        n = __emit_rf(ops, n, 46, 0x6B);
        n = __emit_rf(ops, n, 48, 0x10);
        n = __emit_rf(ops, n, 51, 0x3E);
        n = __emit_rf(ops, n, 52, 0x48);
        n = __emit_rf(ops, n, 54, 0x38);
        n = __emit_rf(ops, n, 56, 0xA1);
        n = __emit_rf(ops, n, 57, 0x00);
        n = __emit_rf(ops, n, 58, 0x39);
        n = __emit_rf(ops, n, 60, 0x45);
        n = __emit_rf(ops, n, 61, 0x91);
        n = __emit_rf(ops, n, 62, 0x39);

        rf2359 = (channel <= 10) ? 0x07 : 0x06;
        n = __emit_rf(ops, n, 23, rf2359);
        n = __emit_rf(ops, n, 59, rf2359);
        n = __emit_rf(ops, n, 55, 0x43);   /* OFDM, non-EP */
    } else {
        /* 5 GHz: RFCSR11 overwritten by 0x40; common A-band RF block */
        n = __emit_rf(ops, n, 10, 0x97);
        n = __emit_rf(ops, n, 11, 0x40);
        n = __emit_rf(ops, n, 25, 0xBF);
        n = __emit_rf(ops, n, 27, 0x42);
        n = __emit_rf(ops, n, 36, 0x00);
        n = __emit_rf(ops, n, 37, 0x04);
        n = __emit_rf(ops, n, 38, 0x85);
        n = __emit_rf(ops, n, 40, 0x42);
        n = __emit_rf(ops, n, 41, 0xBB);
        n = __emit_rf(ops, n, 42, 0xD7);
        n = __emit_rf(ops, n, 45, 0x41);
        n = __emit_rf(ops, n, 48, 0x00);
        n = __emit_rf(ops, n, 57, 0x77);
        n = __emit_rf(ops, n, 60, 0x05);
        n = __emit_rf(ops, n, 61, 0x01);

        if (channel >= 36 && channel <= 64) {
            n = __emit_rf(ops, n, 12, 0x2E);
            n = __emit_rf(ops, n, 13, 0x22);
            n = __emit_rf(ops, n, 22, 0x60);
            n = __emit_rf(ops, n, 23, 0x7F);
            n = __emit_rf(ops, n, 24, (channel <= 50) ? 0x09 : 0x07);
            n = __emit_rf(ops, n, 39, 0x1C);
            n = __emit_rf(ops, n, 43, 0x5B);
            n = __emit_rf(ops, n, 44, 0x40);
            n = __emit_rf(ops, n, 46, 0x00);
            n = __emit_rf(ops, n, 51, 0xFE);
            n = __emit_rf(ops, n, 52, 0x0C);
            n = __emit_rf(ops, n, 54, 0xF8);
            if (channel <= 50) {
                n = __emit_rf(ops, n, 55, 0x06);
                n = __emit_rf(ops, n, 56, 0xD3);
            } else {
                n = __emit_rf(ops, n, 55, 0x04);
                n = __emit_rf(ops, n, 56, 0xBB);
            }
            n = __emit_rf(ops, n, 58, 0x15);
            n = __emit_rf(ops, n, 59, 0x7F);
            n = __emit_rf(ops, n, 62, 0x15);
        } else if (channel >= 100 && channel <= 165) {
            n = __emit_rf(ops, n, 12, 0x0E);
            n = __emit_rf(ops, n, 13, 0x42);
            n = __emit_rf(ops, n, 22, 0x40);
            if (channel <= 153) {
                n = __emit_rf(ops, n, 23, 0x3C);
                n = __emit_rf(ops, n, 24, 0x06);
            } else {
                n = __emit_rf(ops, n, 23, 0x38);
                n = __emit_rf(ops, n, 24, 0x05);
            }
            if (channel <= 138) {
                n = __emit_rf(ops, n, 39, 0x1A);
                n = __emit_rf(ops, n, 43, 0x3B);
                n = __emit_rf(ops, n, 44, 0x20);
                n = __emit_rf(ops, n, 46, 0x18);
            } else {
                n = __emit_rf(ops, n, 39, 0x18);
                n = __emit_rf(ops, n, 43, 0x1B);
                n = __emit_rf(ops, n, 44, 0x10);
                n = __emit_rf(ops, n, 46, 0x08);
            }
            n = __emit_rf(ops, n, 51, (channel <= 124) ? 0xFC : 0xEC);
            n = __emit_rf(ops, n, 52, 0x06);
            n = __emit_rf(ops, n, 54, 0xEB);
            n = __emit_rf(ops, n, 55, (channel <= 138) ? 0x01 : 0x00);
            n = __emit_rf(ops, n, 56, (channel <= 128) ? 0xBB : 0xAB);
            n = __emit_rf(ops, n, 58, (channel <= 116) ? 0x1D : 0x15);
            n = __emit_rf(ops, n, 59, (channel <= 138) ? 0x3F : 0x7C);
            n = __emit_rf(ops, n, 62, (channel <= 116) ? 0x1D : 0x15);
        }
    }

    /* TX power.  The reference uses per-channel EEPROM power (default_power1/2)
     * clamped to power_bound; on 2.4 GHz that value is itself clamped to
     * MAX_G_TXPOWER=0x1f, so RFCSR49/50 never exceed 0x1f there.  We do not yet
     * read the per-channel EEPROM power, so use MAX_G_TXPOWER (0x1f) as a safe
     * upper bound on 2.4 GHz -- this matches the highest value ref would ever
     * program there and avoids the PA over-drive that forcing power_bound
     * (0x27) caused. */
    {
        uint8_t tx_power = is_5g ? power_bound : 0x1F;
        n = __emit_rf_rmw(ops, n, 49, RFCSR49_TX, tx_power);
        n = __emit_rf_rmw(ops, n, 50, RFCSR50_TX, tx_power);
    }

    /* RF block enable: 2T2R -> RF_BLOCK_EN|PLL_PD|TX0_PD|RX0_PD|TX1_PD|RX1_PD */
    n = __emit_rf(ops, n, 1, 0x3F);
    n = __emit_rf(ops, n, 6, 0xE4);
    n = __emit_rf(ops, n, 30, 0x10);   /* not HT40 */
    n = __emit_rf(ops, n, 31, 0x80);
    n = __emit_rf(ops, n, 32, 0x80);
    /* freq_cal_mode1 (ref rt2800lib.c:2447): apply EEPROM frequency offset to
     * RFCSR17_CODE.  Skipped when freq_offset==0 (not read / not programmed),
     * preserving the chip default. */
    if (__rt28xx_freq_offset != 0) {
        n = __emit_rf_rmw(ops, n, 17, RT28XX_RFCSR17_CODE, __rt28xx_freq_offset);
    }
    n = __emit_rf_rmw(ops, n, 3, RFCSR3_VCOCAL_EN, RFCSR3_VCOCAL_EN);

    /* BBP front-end (lna_gain = 0); BBP79/80/81/82 are band dependent */
    n = __emit_bbp(ops, n, 62, 0x37);
    n = __emit_bbp(ops, n, 63, 0x37);
    n = __emit_bbp(ops, n, 64, 0x37);
    n = __emit_bbp(ops, n, 79, is_5g ? 0x18 : 0x1C);
    n = __emit_bbp(ops, n, 80, is_5g ? 0x08 : 0x0E);
    n = __emit_bbp(ops, n, 81, is_5g ? 0x38 : 0x3A);
    n = __emit_bbp(ops, n, 82, is_5g ? 0x92 : 0x62);
    /* per-channel GLRT band config (band dependent) */
    n = __emit_bbp(ops, n, 195, 128); n = __emit_bbp(ops, n, 196, is_5g ? 0xF0 : 0xE0);
    n = __emit_bbp(ops, n, 195, 129); n = __emit_bbp(ops, n, 196, is_5g ? 0x1E : 0x1F);
    n = __emit_bbp(ops, n, 195, 130); n = __emit_bbp(ops, n, 196, is_5g ? 0x28 : 0x38);
    n = __emit_bbp(ops, n, 195, 131); n = __emit_bbp(ops, n, 196, is_5g ? 0x20 : 0x32);
    n = __emit_bbp(ops, n, 195, 133); n = __emit_bbp(ops, n, 196, is_5g ? 0x7F : 0x28);
    n = __emit_bbp(ops, n, 195, 124); n = __emit_bbp(ops, n, 196, is_5g ? 0x7F : 0x19);

    /* rt2800_config_channel() common tail (RF5592, no external LNA) */
    n = __emit_bbp(ops, n, 62, 0x37);
    n = __emit_bbp(ops, n, 63, 0x37);
    n = __emit_bbp(ops, n, 64, 0x37);
    n = __emit_bbp(ops, n, 86, 0x00);
    n = __emit_bbp(ops, n, 82, is_5g ? 0xF2 : 0x84);  /* overrides value above */
    n = __emit_bbp(ops, n, 75, 0x50);

    /* Band + RX-path pin enables.  LNA_PE/RFTR/TRSW MUST be set or the
     * receiver hears nothing (the old RT30xx 0x00000D0F left them clear). */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_TX_BAND_CFG,
            is_5g ? RT28XX_TX_BAND_CFG_5G : RT28XX_TX_BAND_CFG_2G);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_TX_PIN_CFG,
            is_5g ? RT28XX_TX_PIN_CFG_5G : RT28XX_TX_PIN_CFG_2G);

    /* RT5592 GLRT BW + AGC.  bbp_write_with_rx_chain() programs BBP66 once per
     * RX chain (chain 0 then chain 1) by toggling BBP27 RX_CHAIN_SEL. */
    n = __emit_bbp(ops, n, 195, 141); n = __emit_bbp(ops, n, 196, 0x1A);
    n = __emit_bbp_rmw(ops, n, 27, BBP27_RX_CHAIN_SEL, 0);
    n = __emit_bbp(ops, n, 66, 0x1C);
    n = __emit_bbp_rmw(ops, n, 27, BBP27_RX_CHAIN_SEL, BBP27_RX_CHAIN1);
    n = __emit_bbp(ops, n, 66, 0x1C);
    /* config_channel() tail (rt2800lib.c:4526): BBP4 BANDWIDTH=0 for 20 MHz,
     * preserve MAC_IF_CTRL via read-modify-write.  This was missing -- without
     * it the BBP keeps whatever bandwidth the init left and the RX decoder may
     * never lock onto 20 MHz OFDM. */
    n = __emit_bbp_rmw(ops, n, 4, BBP4_BANDWIDTH, 0);
    /* TX IQ calibration (ref rt2800lib.c:4026 rt2800_iq_calibrate, RT5592).
     * Programs the per-chain TX IQ gain/phase and the RF IQ compensation via
     * the BBP158(index)/BBP159(value) pair.  WITHOUT this the TX constellation
     * has an uncorrected IQ imbalance -> high EVM -> the AP fails to decode
     * most of our uplink MPDUs and never ACKs them (RX is unaffected because
     * it does not use the TX IQ path).  2.4 GHz indices only for now. */
    if (__rt28xx_iq_valid && !is_5g) {
        n = __emit_bbp(ops, n, RT28XX_BBP158_IQ_INDEX, 0x2c);
        n = __emit_bbp(ops, n, RT28XX_BBP159_IQ_VALUE, __rt28xx_iq_gain_tx0);
        n = __emit_bbp(ops, n, RT28XX_BBP158_IQ_INDEX, 0x2d);
        n = __emit_bbp(ops, n, RT28XX_BBP159_IQ_VALUE, __rt28xx_iq_phase_tx0);
        n = __emit_bbp(ops, n, RT28XX_BBP158_IQ_INDEX, 0x4a);
        n = __emit_bbp(ops, n, RT28XX_BBP159_IQ_VALUE, __rt28xx_iq_gain_tx1);
        n = __emit_bbp(ops, n, RT28XX_BBP158_IQ_INDEX, 0x4b);
        n = __emit_bbp(ops, n, RT28XX_BBP159_IQ_VALUE, __rt28xx_iq_phase_tx1);
        n = __emit_bbp(ops, n, RT28XX_BBP158_IQ_INDEX, 0x04);
        n = __emit_bbp(ops, n, RT28XX_BBP159_IQ_VALUE, __rt28xx_iq_rf_comp);
        n = __emit_bbp(ops, n, RT28XX_BBP158_IQ_INDEX, 0x03);
        n = __emit_bbp(ops, n, RT28XX_BBP159_IQ_VALUE, __rt28xx_iq_rf_imbal);
    }
    return n;
}

static int __rt28xx_emit_bssid(vsf_wifi_reg_op_t *ops, int n, const uint8_t bssid[6])
{
    uint32_t dw0 = (uint32_t)bssid[0]
                 | ((uint32_t)bssid[1] <<  8)
                 | ((uint32_t)bssid[2] << 16)
                 | ((uint32_t)bssid[3] << 24);
    /* BSS_ID_MASK (bits 17:16) = 3: multi-BSSID / disable strict BSSID match.
     * BSS_BCN_NUM (bits 20:18) = 0: single beacon.
     * Bit 21 (vendor/reserved) = 1: observed in Windows native driver.
     * DW1 high half = 0x23 (bits 21:16 = 100011).
     *
     * The Windows Ralink driver stores OUR OWN STA MAC in MAC_BSSID_DW0/1
     * (ref/rt5572_win_usb.log L23583-23584 reads 0x1014=0x00234103 and then
     * rewrites 0x1010/0x1012 with our MAC before EAPOL-Key M1 arrives).
     * With BSS_ID_MASK=0 the chip runs 1-BSSID mode and compares the incoming
     * frame's BSSID field (addr3 = AP BSSID) against MAC_BSSID (= our MAC),
     * dropping all unicast data frames including plaintext EAPOL M1.
     * Management frames are unaffected because they match on addr1.
     *
     * Setting BSS_ID_MASK=3 matches the Windows driver (DW1 high = 0x0023) and
     * disables that strict BSSID comparison, so frames addressed to us (addr1
     * matches our MAC/WCID) are accepted regardless of the BSSID field.
     * Linux rt2x00 uses the same mask but writes the AP BSSID to MAC_BSSID. */
    uint32_t dw1 = (uint32_t)bssid[4] | ((uint32_t)bssid[5] << 8)
                 | (0x23u << 16);   /* BSS_ID_MASK=3 + vendor bit 21 */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_MAC_BSSID_DW0, dw0);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_MAC_BSSID_DW1, dw1);
    return n;
}

/*============================ RX DESCRIPTOR =================================*
 *
 * USB RX bulk-in payload layout (rt2x00usb / rt2800usb):
 *
 *   | RXINFO (4B) | RXWI (24B for RT5592) | 802.11 frame | pad | RXD (4B) | USB pad |
 *                 |<--------- rx_pkt_len = RXWI + frame ------>|
 *
 * RXINFO_W0[15:0]  = USB_DMA_RX_PKT_LEN.
 * RXWI_W0[27:16]   = MPDU_TOTAL_BYTE_COUNT (12-bit, authoritative 802.11 len).
 *                    (RXWI_W0_MPDU_TOTAL_BYTE_COUNT = FIELD32(0x0fff0000))
 * RXWI_W2[7:0/15:8/23:16] = RSSI/AGC values for antenna 0/1/2 (unsigned).
 *
 * RT5592 uses RXWI_DESC_SIZE_6WORDS = 24 B (NOT the default 16 B).  ref
 * rt2800_get_txwi_rxwi_size (rt2800lib.c:614-618) puts BOTH RT5592 and RT6352
 * on 6-word RXWI; only RT3593/RT3883 use 5 words (20 B) and the remaining
 * parts default to 4 words (16 B).  Hardware confirmed it: with RXWI=16 the
 * 802.11 header parsed 8 bytes early (fc landed on RXWI tail, beacon FC 0x0080
 * showed up at hdr[8]); with RXWI=24 the header lands exactly on FC.
 *==========================================================================*/

#define RT28XX_RXINFO_DESC_SIZE         4
#define RT28XX_RXWI_DESC_SIZE_5572      24
#define RT28XX_RXD_DESC_SIZE            4
/* RXD trailer word-0 flags (ref rt2800usb.h:86/91).  CRC_ERROR marks a frame
 * the PHY received but failed FCS on -- mac80211 drops these; we must too,
 * otherwise corrupted weak-signal beacons leak garbage BSSID/SSID results. */
#define RT28XX_RXD_W0_CRC_ERROR         0x00000100u
#define RT28XX_DOT11_HDR_MIN            24
#define RT28XX_BEACON_FIXED             12      /* timestamp+interval+capa */

/* 802.11 management-frame subtypes (high nibble of FC byte0). */
#define RT28XX_STYPE_ASSOC_RESP         0x1
#define RT28XX_STYPE_PROBE_RESP         0x5
#define RT28XX_STYPE_BEACON             0x8
#define RT28XX_STYPE_DISASSOC           0xA
#define RT28XX_STYPE_AUTH               0xB
#define RT28XX_STYPE_DEAUTH             0xC

/* Map an RSN/WPA cipher-suite selector (OUI 00-0F-AC) type byte to a
 * WIFI_CIPHER_xxx constant. */
static uint8_t __rt28xx_rsn_cipher(uint8_t suite_type)
{
    switch (suite_type) {
    case 1:  return WIFI_CIPHER_WEP40;
    case 2:  return WIFI_CIPHER_TKIP;
    case 4:  return WIFI_CIPHER_CCMP;
    case 5:  return WIFI_CIPHER_WEP104;
    default: return WIFI_CIPHER_NONE;
    }
}

/* Parse an RSN IE body (everything after tag+len) and fill the security
 * fields of a scan result.  Only WPA2-PSK is recognised: the IE must carry
 * the PSK AKM (00-0F-AC-02).  Malformed/truncated IEs are ignored. */
static void __rt28xx_parse_rsn(const uint8_t *body, uint8_t len,
        vsf_wifi_scan_result_t *result)
{
    static const uint8_t oui[3] = { 0x00, 0x0F, 0xAC };
    const uint8_t *p   = body;
    const uint8_t *end = body + len;
    uint16_t count, k;
    bool     has_psk  = false;
    uint8_t  pairwise = WIFI_CIPHER_NONE;
    uint8_t  group    = WIFI_CIPHER_NONE;

    if (len < 8) return;                /* version(2)+group(4)+pwcount(2)    */
    p += 2;                             /* skip version                     */
    if (!memcmp(p, oui, 3)) group = __rt28xx_rsn_cipher(p[3]);
    p += 4;
    /* pairwise cipher suite list */
    if (p + 2 > end) return;
    count = (uint16_t)(p[0] | ((uint16_t)p[1] << 8)); p += 2;
    for (k = 0; k < count; k++) {
        if (p + 4 > end) return;
        if (!memcmp(p, oui, 3)) {
            uint8_t c = __rt28xx_rsn_cipher(p[3]);
            if (c == WIFI_CIPHER_CCMP)              pairwise = WIFI_CIPHER_CCMP;
            else if (pairwise == WIFI_CIPHER_NONE)  pairwise = c;
        }
        p += 4;
    }
    /* AKM suite list */
    if (p + 2 > end) return;
    count = (uint16_t)(p[0] | ((uint16_t)p[1] << 8)); p += 2;
    for (k = 0; k < count; k++) {
        if (p + 4 > end) return;
        if (!memcmp(p, oui, 3) && (p[3] == 2)) has_psk = true;   /* PSK */
        p += 4;
    }

    if (has_psk) {
        result->auth_mode       = WIFI_AUTH_WPA2_PSK;
        result->pairwise_cipher = (pairwise != WIFI_CIPHER_NONE)
                                ? pairwise : WIFI_CIPHER_CCMP;
        result->group_cipher    = group;
    }
}

/* ---- TX_STA_FIFO sampler (diagnostic) -------------------------------------
 * Asynchronously read TX_STA_FIFO from the RX path while associated, to learn
 * whether our uplink data frames (DHCP DISCOVER) are actually ACKed by the AP.
 * 802.11 ACK happens at the MAC layer BEFORE decryption, so:
 *   TX_SUCCESS=1 -> frame reached AP and was ACKed (any later drop is a higher
 *                   layer / decryption issue, NOT a radio/uplink issue);
 *   TX_SUCCESS=0 -> frame never reached AP or AP did not ACK (uplink problem).
 * Single-flight via the wifi-layer script slot; if busy we just skip. */
/* TX_STA_FIFO is a pop-on-read hardware FIFO (ref rt2800usb.c:106): each read
 * pops one entry, and the reader must keep reading until VALID=0 to drain it.
 * The previous single-shot read kept returning the SAME stale head entry
 * (raw stuck at 0x40000189) which falsely looked like 100% TX failure.  Drain
 * the whole FIFO here by re-issuing run_read from the completion callback
 * (script_busy is already cleared by the dispatcher finish) until VALID=0, and
 * accumulate real per-frame ACK stats so success is actually observable. */
static uint32_t __rt28xx_txfifo_val;
static uint32_t __rt28xx_txfifo_ok;
static uint32_t __rt28xx_txfifo_fail;
static void __rt28xx_txfifo_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) {
        return;
    }
    uint32_t v = __rt28xx_txfifo_val;
    if (0 == (v & 0x1u)) {  /* VALID=0 -> FIFO drained */
        return;
    }
    unsigned ack_ok = (unsigned)((v >> 5) & 0x1u);   /* TX_SUCCESS */
    if (ack_ok) {
        __rt28xx_txfifo_ok++;
    } else {
        __rt28xx_txfifo_fail++;
    }
    vsf_wifi_chip_rt28xx_trace_debug("wifi: TX_STA_FIFO ack_ok=%u ack_req=%u wcid=%u pid=%u mcs=%u"
            " raw=0x%08X (ok=%u fail=%u)" VSF_TRACE_CFG_LINEEND,
            ack_ok,
            (unsigned)((v >> 7) & 0x1u),    /* TX_ACK_REQUIRED */
            (unsigned)((v >> 8) & 0xFFu),   /* WCID            */
            (unsigned)((v >> 1) & 0xFu),    /* PID_TYPE        */
            (unsigned)((v >> 16) & 0x7Fu),  /* MCS             */
            (unsigned)v,
            (unsigned)__rt28xx_txfifo_ok, (unsigned)__rt28xx_txfifo_fail);
    /* Re-issue to pop the next entry; stop automatically when VALID=0. */
    vsf_wifi_reg_read(wifi, RT28XX_TX_STA_FIFO,
            &__rt28xx_txfifo_val, __rt28xx_txfifo_done);
}

static void __rt28xx_parse_rx(vsf_wifi_t *wifi, uint8_t *frame, uint16_t len)
{
    /* RT5592 USB bulk-in may deliver multiple aggregated frames in a single
     * transfer even with RX_BULK_AGG_EN=0. Each frame has the layout:
     *   | RXINFO(4) | RXWI(24) | MPDU | pad | RXD(4) | [align]
     *              |<------ rx_pkt_len --------->|
     * We loop over the buffer processing every frame, so that responses
     * addressed to our MAC (e.g. DHCP Offer, ARP Reply) are never missed
     * just because they appear after another STA's frame in the buffer. */
    const uint16_t min_frame = (uint16_t)(RT28XX_RXINFO_DESC_SIZE
            + RT28XX_RXWI_DESC_SIZE_5572 + RT28XX_DOT11_HDR_MIN
            + RT28XX_RXD_DESC_SIZE);

    while (len >= min_frame) {

    /* === RAW-TOP DIAG (temporary): log EVERY frame at the very top of the loop,
     * BEFORE any length-validity break below.  This is the most upstream point we
     * can observe: if a BSSID shows here but NOT in the RAW SCAN DIAG further down,
     * the frame was dropped by our length checks (fixable, root cause B); if a
     * BSSID never shows here at all, the chip never DMA'd it up (root cause A/C,
     * not our software).  addr3(BSSID) sits at MPDU+16; here len>=min_frame so
     * the read is in-bounds.  Remove after debug. */
    /* Also fire whenever a frame's receiver address (addr1/da at MPDU+4) equals
     * our own MAC, REGARDLESS of mlme_state.  The previous gate (mlme_state==4WAY)
     * had a blind spot: if M1 arrives while the state machine is still in ASSOC
     * (before it flips to 4WAY), the frame would never be logged and we'd wrongly
     * conclude "M1 never came".  Matching on da==our-MAC guarantees every frame
     * physically delivered to us is observed, at any handshake stage.  da sits at
     * MPDU+4; here len>=min_frame so the read is in-bounds.  Remove after debug. */
    {
        const uint8_t *ht  = frame + RT28XX_RXINFO_DESC_SIZE + RT28XX_RXWI_DESC_SIZE_5572;
        bool to_us = (ht[4] == wifi->mac[0]) && (ht[5] == wifi->mac[1])
                  && (ht[6] == wifi->mac[2]) && (ht[7] == wifi->mac[3])
                  && (ht[8] == wifi->mac[4]) && (ht[9] == wifi->mac[5]);
        if (wifi->scanning || to_us) {
#if VSF_WIFI_CFG_CHIP_RT28XX_LOG_LEVEL >= 4
            uint16_t fct = (uint16_t)ht[0] | ((uint16_t)ht[1] << 8);
            vsf_wifi_chip_rt28xx_trace_debug("wifi: RAW-TOP fc=%04X type=%u sub=%u prot=%u mlme=%u buflen=%u"
                    " da=%02X:%02X:%02X:%02X:%02X:%02X bssid=%02X:%02X:%02X:%02X:%02X:%02X"
                    VSF_TRACE_CFG_LINEEND,
                    fct, (unsigned)((fct >> 2) & 0x3u), (unsigned)((fct >> 4) & 0xFu),
                    (unsigned)((fct >> 14) & 0x1u), (unsigned)wifi->mlme_state, (unsigned)len,
                    ht[4], ht[5], ht[6], ht[7], ht[8], ht[9],
                    ht[16], ht[17], ht[18], ht[19], ht[20], ht[21]);
            /* When fc protocol-version bits are non-zero (illegal in 802.11),
             * the parsed offset is wrong.  Dump the first 96 bytes of the
             * USB transfer raw, plus a few key offsets, so we can see chip's
             * actual layout (RXINFO/RXWI sizes, possible prefix, aggregation
             * boundary).  Remove after debug. */
            if ((fct & 0x0003u) != 0u) {
                unsigned dump_n = (len < 96u) ? len : 96u;
                char hex[3 * 96 + 1];
                unsigned hi = 0;
                for (unsigned i = 0; i < dump_n; i++) {
                    static const char d[] = "0123456789ABCDEF";
                    hex[hi++] = d[(frame[i] >> 4) & 0xFu];
                    hex[hi++] = d[frame[i] & 0xFu];
                    hex[hi++] = ' ';
                }
                hex[hi] = '\0';
                vsf_wifi_chip_rt28xx_trace_debug("wifi: RAW-DUMP n=%u %s" VSF_TRACE_CFG_LINEEND,
                        dump_n, hex);
            }
#endif
        }
    }

    uint32_t rxinfo_w0 = get_unaligned_le32(frame);
    uint16_t rx_pkt_len = (uint16_t)(rxinfo_w0 & 0xFFFF);
    if (rx_pkt_len < RT28XX_RXWI_DESC_SIZE_5572 + RT28XX_DOT11_HDR_MIN) break;
    if ((uint32_t)rx_pkt_len + RT28XX_RXINFO_DESC_SIZE > len) break;

    /* Drop FCS-failed frames using the RXD trailer at frame+RXINFO+rx_pkt_len
     * (ref rt2800usb_fill_rxdone, rt2800usb.c:520-528).  Without this, CRC-
     * corrupted beacons pass the loose mgmt filter and surface as bogus APs
     * (e.g. BSSID FF:FF:..). */
    bool crc_err = false;
    if ((uint32_t)RT28XX_RXINFO_DESC_SIZE + rx_pkt_len + RT28XX_RXD_DESC_SIZE <= len) {
        uint32_t rxd_w0 = get_unaligned_le32(frame
                + RT28XX_RXINFO_DESC_SIZE + rx_pkt_len);
        if (rxd_w0 & RT28XX_RXD_W0_CRC_ERROR) crc_err = true;
    }
    /* CRC result computed above (crc_err); defer the drop until after the
     * scan diagnostic below so CRC-failed beacons are visible too. */

    uint8_t *rxwi = frame + RT28XX_RXINFO_DESC_SIZE;
    uint32_t rxwi_w0 = get_unaligned_le32(rxwi + 0);
    uint32_t rxwi_w2 = get_unaligned_le32(rxwi + 8);

    /* === RAW SCAN DIAG (temporary): log EVERY frame the chip demodulated and
     * DMA'd up while scanning, BEFORE any mpdu_len validity check below, so a
     * frame dropped at the length checks is still visible.  This distinguishes
     * "dropped at length check (fixable)" from "chip never demodulated it (PHY)".
     * Reads addr3 (BSSID) at the fixed mgmt-header offset.  Remove after debug. */
    if (wifi->scanning) {
        const uint8_t *h0  = rxwi + RT28XX_RXWI_DESC_SIZE_5572;
        uint16_t       fc0 = (uint16_t)h0[0] | ((uint16_t)h0[1] << 8);
        uint16_t       ml0 = (uint16_t)((rxwi_w0 >> 16) & 0xFFFu);
        vsf_wifi_chip_rt28xx_trace_debug("wifi: RAW scan fc=%04X rxlen=%u mpdu=%u buflen=%u crc=%u bssid=%02X:%02X:%02X:%02X:%02X:%02X"
                VSF_TRACE_CFG_LINEEND,
                fc0, (unsigned)rx_pkt_len, (unsigned)ml0, (unsigned)len, (unsigned)crc_err,
                h0[16], h0[17], h0[18], h0[19], h0[20], h0[21]);
        (void)fc0; (void)ml0;
    }

    uint16_t mpdu_len = (uint16_t)((rxwi_w0 >> 16) & 0xFFFu);
    if (mpdu_len < RT28XX_DOT11_HDR_MIN) goto __advance_frame;
    if ((uint32_t)RT28XX_RXINFO_DESC_SIZE + RT28XX_RXWI_DESC_SIZE_5572 + mpdu_len > len)
        goto __advance_frame;

    const uint8_t *hdr = rxwi + RT28XX_RXWI_DESC_SIZE_5572;

    /* === TEMP DIAG 2: dump ALL type=2 frames (no A1 filter) during 4-way, to see
     * if M1 reaches the parser with some unexpected A1.  Remove after debug. */
    if (wifi->mlme_state == WIFI_MLME_4WAY) {
        uint16_t __fc_dbg = (uint16_t)hdr[0] | ((uint16_t)hdr[1] << 8);
        uint8_t  __type_dbg = (uint8_t)((__fc_dbg >> 2) & 0x3);
        if (__type_dbg == 2) {
            static uint32_t __all_data_cnt = 0;
            if (__all_data_cnt < 50) {
                __all_data_cnt++;
                vsf_wifi_chip_rt28xx_trace_debug("wifi: ALLDATA[%u] mpdu=%u W0=0x%08X fc=%02X%02X "
                        "a1=%02X:%02X:%02X:%02X:%02X:%02X "
                        "a2=%02X:%02X:%02X:%02X:%02X:%02X "
                        "a3=%02X:%02X:%02X:%02X:%02X:%02X"
                        VSF_TRACE_CFG_LINEEND,
                        (unsigned)__all_data_cnt, (unsigned)mpdu_len,
                        (unsigned)rxwi_w0, hdr[0], hdr[1],
                        hdr[4], hdr[5], hdr[6], hdr[7], hdr[8], hdr[9],
                        hdr[10], hdr[11], hdr[12], hdr[13], hdr[14], hdr[15],
                        hdr[16], hdr[17], hdr[18], hdr[19], hdr[20], hdr[21]);
            }
        }
    }

    /* 802.11 frame control: type/subtype encoded in the low byte. */
    uint16_t fc      = (uint16_t)hdr[0] | ((uint16_t)hdr[1] << 8);
    uint8_t  type    = (uint8_t)((fc >> 2) & 0x3);   /* 0 = mgmt, 2 = data */
    uint8_t  subtype = (uint8_t)((fc >> 4) & 0xF);   /* 8 = beacon, 5 = probe-resp */

    /* === SCAN DIAGNOSTIC (temporary): log every mgmt beacon / probe-resp that
     * physically arrived — including CRC-failed ones — so we can distinguish
     * "AP never received" from "received but dropped".  Remove after debug. */
    if (wifi->scanning && (type == 0) && ((subtype == 8) || (subtype == 5))) {
        char     dssid[33];
        uint8_t  dssid_len = 0;
        dssid[0] = '\0';
        if (mpdu_len >= RT28XX_DOT11_HDR_MIN + RT28XX_BEACON_FIXED) {
            const uint8_t *b    = hdr + RT28XX_DOT11_HDR_MIN;
            uint16_t       blen = (uint16_t)(mpdu_len - RT28XX_DOT11_HDR_MIN);
            const uint8_t *die  = b + RT28XX_BEACON_FIXED;
            const uint8_t *dend = b + blen;
            while (die + 2 <= dend) {
                uint8_t t = die[0];
                uint8_t l = die[1];
                if (die + 2 + l > dend) break;
                if (t == 0) {
                    uint8_t cc = (l > 32) ? 32 : l;
                    memcpy(dssid, die + 2, cc);
                    dssid[cc] = '\0';
                    dssid_len = cc;
                    break;
                }
                die += 2 + l;
            }
        }
        vsf_wifi_chip_rt28xx_trace_debug("wifi: DIAG sub=%u crc=%u len=%u bssid=%02X:%02X:%02X:%02X:%02X:%02X ssid=\"%s\"(%u)"
                VSF_TRACE_CFG_LINEEND,
                subtype, (unsigned)crc_err, (unsigned)mpdu_len,
                hdr[16], hdr[17], hdr[18], hdr[19], hdr[20], hdr[21],
                dssid, (unsigned)dssid_len);
        (void)dssid_len;
    }

    if (crc_err) goto __advance_frame;

    /* Raw radio mode: deliver the de-descriptored 802.11 frame to the
     * registered raw RX callback and skip the normal WiFi MLME path. */
    if (wifi->raw_radio_active) {
        vsf_wifi_radio_on_rx(wifi, (uint8_t *)hdr, mpdu_len);
        goto __advance_frame;
    }

    /* Diagnose post-handshake frame types (state >= 2 = 4WAY/RUN). */
    if (wifi->mlme_state >= 2) {
        static uint32_t __parse_run_cnt = 0;
        static uint32_t __parse_tome_cnt = 0;
        __parse_run_cnt++;
        /* Log frames addressed to our MAC or broadcast. */
        bool to_me = (memcmp(&hdr[4], wifi->mac, 6) == 0);
        bool bcast  = (hdr[4] & 0x01);  /* multicast/broadcast bit */
        if (to_me || (bcast && type == 2)) {
            if (++__parse_tome_cnt <= 40) {
                vsf_wifi_chip_rt28xx_trace_debug("wifi: parse_rx TO_ME[%u/%u] type=%u sub=%u fc=%04X len=%u prot=%u"
                        VSF_TRACE_CFG_LINEEND,
                        (unsigned)__parse_tome_cnt, (unsigned)__parse_run_cnt,
                        type, subtype, fc, (unsigned)mpdu_len,
                        (unsigned)((fc >> 14) & 1));
            }
        } else if (__parse_run_cnt <= 10) {
            vsf_wifi_chip_rt28xx_trace_debug("wifi: parse_rx[%u] type=%u sub=%u fc=%04X len=%u A1=%02X:%02X:%02X:%02X:%02X:%02X"
                    VSF_TRACE_CFG_LINEEND,
                    (unsigned)__parse_run_cnt, type, subtype, fc, (unsigned)mpdu_len,
                    hdr[4], hdr[5], hdr[6], hdr[7], hdr[8], hdr[9]);
        }
    }

    /* Drain one TX_STA_FIFO entry per few RX frames while associated, to learn
     * whether uplink data frames are ACKed by the AP (see helper above).
     * Also sample during 4WAY: we are stuck there (no M1 received) and need to
     * know whether the AP ACKs OUR frames (assoc-req etc.) -- ack_ok tells us if
     * the AP can hear us at all, which distinguishes "AP never sent M1" from
     * "M1 was sent but we failed to ACK it". */
    if ((wifi->mlme_state == WIFI_MLME_RUN)
            || (wifi->mlme_state == WIFI_MLME_4WAY)) {
        static uint32_t __txfifo_gate = 0;
        if ((++__txfifo_gate & 0x1u) == 0) {
            vsf_wifi_reg_read(wifi, RT28XX_TX_STA_FIFO,
                    &__rt28xx_txfifo_val, __rt28xx_txfifo_done);
        }
    }

    /* Data frames (type 2) are only meaningful once the link is associated.
     * Hand the naked 802.11 frame to the wifi layer, which detects EAPOL
     * (4-way handshake) and forwards business payloads.  Null-data / control
     * subtypes (>= 4 with no body) carry nothing useful and are dropped.
     *
     * RT5592 inserts L2PAD between 802.11 header and payload in the USB RX
     * buffer when the header isn't 4-byte aligned (QoS data: 26B → l2pad=2).
     * MPDU_TOTAL_BYTE_COUNT is the real on-air length (no L2PAD), so we must
     * strip L2PAD before passing the frame up.  Ref: rt2x00queue_remove_l2pad,
     * RXD_W0_L2PAD (bit 14), REQUIRE_L2PAD. */
    if (type == 2) {
        /* === TEMP DIAG: dump first 50 data frames during 4-way, to see if M1
         * ever reaches the parser (vs being dropped at MAC).  Remove after debug. */
        if (wifi->mlme_state == WIFI_MLME_4WAY) {
            static uint32_t __data_diag_cnt = 0;
            if (__data_diag_cnt < 200) {
                uint32_t w1 = get_unaligned_le32(rxwi + 4);
                uint32_t w3 = get_unaligned_le32(rxwi + 12);
                (void)w1; (void)w3;
                __data_diag_cnt++;
                vsf_wifi_chip_rt28xx_trace_debug("wifi: DATA[%u] mpdu=%u W0=0x%08X W1=0x%08X W2=0x%08X W3=0x%08X "
                        "fc=%02X%02X dur=%02X%02X a1=%02X:%02X:%02X:%02X:%02X:%02X "
                        "a2=%02X:%02X:%02X:%02X:%02X:%02X"
                        VSF_TRACE_CFG_LINEEND,
                        (unsigned)__data_diag_cnt, (unsigned)mpdu_len,
                        (unsigned)rxwi_w0, (unsigned)w1, (unsigned)rxwi_w2, (unsigned)w3,
                        hdr[0], hdr[1], hdr[2], hdr[3],
                        hdr[4], hdr[5], hdr[6], hdr[7], hdr[8], hdr[9],
                        hdr[10], hdr[11], hdr[12], hdr[13], hdr[14], hdr[15]);
            }
        }
        if ((wifi->mlme_state == WIFI_MLME_RUN)
                || (wifi->mlme_state == WIFI_MLME_4WAY)) {
            /* Strip RX L2PAD: shift the 802.11 header forward by l2pad bytes
             * so that header and payload become contiguous in memory. */
            uint16_t dot11_hdr_len = (subtype & 0x08) ? 26 : 24;
            uint16_t l2pad = (uint16_t)((-dot11_hdr_len) & 3);
            uint8_t *frame_ptr = (uint8_t *)hdr;
            if (l2pad && (mpdu_len > dot11_hdr_len)) {
                memmove(frame_ptr + l2pad, frame_ptr, dot11_hdr_len);
                frame_ptr += l2pad;
            }
            vsf_wifi_data_rx(wifi, frame_ptr, mpdu_len);
        }
        goto __advance_frame;
    }
    if (type != 0) goto __advance_frame;

    /* Route management subtypes.  Auth / assoc-resp / deauth / disassoc feed
     * the wifi-layer MLME state machine (passing the de-descriptored naked
     * 802.11 frame starting at FC); beacon / probe-resp are scan results
     * consumed only while scanning. */
    switch (subtype) {
    case RT28XX_STYPE_ASSOC_RESP:
    case RT28XX_STYPE_AUTH:
    case RT28XX_STYPE_DEAUTH:
    case RT28XX_STYPE_DISASSOC:
        vsf_wifi_mlme_rx(wifi, hdr, mpdu_len);
        goto __advance_frame;
    case RT28XX_STYPE_BEACON:
    case RT28XX_STYPE_PROBE_RESP:
        if (!wifi->scanning) goto __advance_frame;
        break;
    default:
        goto __advance_frame;
    }

    /* Mgmt frame fixed header: FC(2) Dur(2) DA(6) SA(6) BSSID(6) SeqCtrl(2) = 24B.
     * Beacon / probe-resp body opens with: Timestamp(8) Interval(2) Capability(2). */
    if (mpdu_len < RT28XX_DOT11_HDR_MIN + RT28XX_BEACON_FIXED) goto __advance_frame;
    const uint8_t *bssid    = hdr + 16;
    const uint8_t *body     = hdr + RT28XX_DOT11_HDR_MIN;
    uint16_t       body_len = (uint16_t)(mpdu_len - RT28XX_DOT11_HDR_MIN);
    uint16_t       capa     = (uint16_t)body[10] | ((uint16_t)body[11] << 8);
    uint16_t       bcn_int  = (uint16_t)body[8]  | ((uint16_t)body[9]  << 8);
    (void)bcn_int;

    /* Pick the strongest of three antenna AGC samples (matches Linux
     * rt2800_agc_to_rssi).  Without per-device lna_gain we apply only the
     * canonical -12 dB front-end offset; the result is comparable across
     * frames from the same dongle but not absolute dBm. */
    uint8_t agc0 = (uint8_t)( rxwi_w2        & 0xFF);
    uint8_t agc1 = (uint8_t)((rxwi_w2 >>  8) & 0xFF);
    uint8_t agc2 = (uint8_t)((rxwi_w2 >> 16) & 0xFF);
    uint8_t agc  = agc0;
    if (agc1 > agc) agc = agc1;
    if (agc2 > agc) agc = agc2;
    int signal = (agc != 0) ? (-12 - (int)agc) : -128;
    if (signal < -128) signal = -128;
    if (signal >  127) signal =  127;

    vsf_wifi_scan_result_t result;
    memset(&result, 0, sizeof(result));
    memcpy(result.bssid, bssid, 6);
    result.capability = capa;
    result.channel    = wifi->channel;            /* fall back to dwell ch */
    result.rssi       = (int8_t)signal;
    result.flags      = (subtype == 5) ? 1u : 0u; /* bit 0 = probe-resp */

    /* Walk the IE list to pull SSID (tag 0) and DS Param (tag 3 = channel).
     * Stops cleanly at any malformed length to keep us out of the weeds on
     * truncated USB frames. */
    const uint8_t *ie  = body + RT28XX_BEACON_FIXED;
    const uint8_t *end = body + body_len;
    while (ie + 2 <= end) {
        uint8_t tag = ie[0];
        uint8_t l   = ie[1];
        if (ie + 2 + l > end) break;
        if (tag == 0 && l <= 32) {                  /* SSID */
            memcpy(result.ssid, ie + 2, l);
            result.ssid_len = l;
        } else if (tag == 3 && l == 1) {            /* DS Param Set */
            result.channel = ie[2];
        } else if (tag == 48) {                     /* RSN IE (WPA2) */
            __rt28xx_parse_rsn(ie + 2, l, &result);
            {
                char rsnbuf[80]; int rpos = 0;
                for (uint8_t m = 0; m < l && rpos < (int)sizeof(rsnbuf) - 3; m++)
                    rpos += snprintf(&rsnbuf[rpos], sizeof(rsnbuf) - rpos, "%02X", ie[2 + m]);
                vsf_wifi_chip_rt28xx_trace_debug("wifi: beacon RSN IE len=%u: %s" VSF_TRACE_CFG_LINEEND,
                        (unsigned)l, rsnbuf);
            }
        } else if (tag == 45) {                     /* HT Capabilities */
            {
                char htbuf[80]; int hpos = 0;
                for (uint8_t m = 0; m < l && hpos < (int)sizeof(htbuf) - 3; m++)
                    hpos += snprintf(&htbuf[hpos], sizeof(htbuf) - hpos, "%02X", ie[2 + m]);
                vsf_wifi_chip_rt28xx_trace_debug("wifi: beacon HT CAP IE len=%u: %s" VSF_TRACE_CFG_LINEEND,
                        (unsigned)l, htbuf);
            }
        } else if (tag == 221 && l >= 7 &&
                   ie[2] == 0x00 && ie[3] == 0x50 &&
                   ie[4] == 0xF2 && ie[5] == 0x02) {
            /* WMM Information/Parameter Element (OUI 00:50:F2, OUI type 2). */
            result.wmm = 1;
        }
        ie += 2 + l;
    }

    vsf_wifi_chip_rt28xx_trace_debug("wifi: beacon bssid=%02X:%02X:%02X:%02X:%02X:%02X interval=%u caps=0x%04X wmm=%u"
            VSF_TRACE_CFG_LINEEND,
            bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5],
            (unsigned)bcn_int, (unsigned)capa, (unsigned)result.wmm);
    vsf_wifi_on_scan_result(wifi, &result);
    if (!memcmp(result.bssid, wifi->mlme_bssid, 6)) {
        wifi->bss_wmm = result.wmm != 0;
    }

__advance_frame:
    {
        /* Advance past this frame: RXINFO(4) + rx_pkt_len + RXD(4), aligned to
         * 4 bytes for the next RXINFO.  rx_pkt_len already includes any HW
         * padding so the total should be naturally aligned, but mask anyway. */
        uint32_t consumed = ((uint32_t)RT28XX_RXINFO_DESC_SIZE + rx_pkt_len
                          + RT28XX_RXD_DESC_SIZE + 3u) & ~3u;
        if (consumed >= (uint32_t)len) break;
        frame += consumed;
        len   -= (uint16_t)consumed;
    }
    } /* end while */
}

/*============================ CHIP OPS ======================================*/

/*
 * firmware_load = run_blob(rt2870.bin) -> run_script(post_fw_script)
 *               -> run_read_poll(MAC_CSR0, !=0 && !=~0).
 *
 * The blob upload, the post-fw register handshake and the MCU-ready
 * poll all share the wifi-layer dispatcher, so they must be issued
 * sequentially.  The outer `done` callback is stashed in
 * wifi->backend_chain_done while the inner stages are in flight.
 *
 * MCU-ready check (mirrors Linux rt2x00 rt2800lib.c::rt2800_wait_for_
 * mcu_ready):  poll MAC_CSR0 (== ASIC_VER_ID) until the MCU latches a
 * non-trivial chip-ID.  A read of 0 means the MCU has not started yet;
 * 0xFFFFFFFF typically means USB read failure / device gone.  Any other
 * value indicates the firmware has begun executing.
 */
/* Captured from the MCU-ready poll (ASIC_VER_ID); see __rt28xx_is_5592c. */
static bool __rt28xx_mcu_ready_match(uint32_t val)
{
    if ((val != 0u) && (val != 0xFFFFFFFFu)) {
        __rt28xx_asic_ver = val;
        return true;
    }
    return false;
}

/* Real MCU-boot gate: PBF_SYS_CTRL_READY (bit7).  Set by the 8051 once it
 * starts executing the firmware uploaded to FIRMWARE_IMAGE_BASE.  Unlike
 * ASIC_VER_ID (a static chip-ID readable even with the MCU halted), this bit
 * is 0 until the firmware actually runs, so it is the only honest "firmware
 * alive" signal.  The framework logs the matched / timeout value, so this
 * doubles as the requested MCU-running diagnostic. */
static bool __rt28xx_pbf_ready_match(uint32_t val)
{
    return (val & RT28XX_PBF_SYS_CTRL_READY) != 0u;
}

static void __rt28xx_chain_finish(vsf_wifi_t *wifi, vsf_err_t err)
{
    vsf_wifi_done_t outer = wifi->backend_chain_done;
    wifi->backend_chain_done = NULL;
    if (outer != NULL) outer(wifi, err);
}

static void __rt28xx_mcu_ready_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __rt28xx_eeprom_read_start (vsf_wifi_t *wifi);
static void __rt28xx_eeprom_after_detect   (vsf_wifi_t *wifi, vsf_err_t err);
static void __rt28xx_eeprom_after_kick     (vsf_wifi_t *wifi, vsf_err_t err);
static void __rt28xx_eeprom_after_kick_poll(vsf_wifi_t *wifi, vsf_err_t err);
static void __rt28xx_eeprom_after_data3    (vsf_wifi_t *wifi, vsf_err_t err);
static void __rt28xx_eeprom_after_data2    (vsf_wifi_t *wifi, vsf_err_t err);
static void __rt28xx_eeprom_after_data1    (vsf_wifi_t *wifi, vsf_err_t err);
static void __rt28xx_eeprom_after_data0    (vsf_wifi_t *wifi, vsf_err_t err);
static void __rt28xx_eeprom_after_freq_kick(vsf_wifi_t *wifi, vsf_err_t err);
static void __rt28xx_eeprom_after_freq_poll(vsf_wifi_t *wifi, vsf_err_t err);
static void __rt28xx_eeprom_after_freq_data(vsf_wifi_t *wifi, vsf_err_t err);
static void __rt28xx_eeprom_after_iq_kick  (vsf_wifi_t *wifi, vsf_err_t err);
static void __rt28xx_eeprom_after_iq_poll  (vsf_wifi_t *wifi, vsf_err_t err);
static void __rt28xx_eeprom_after_iq_data3 (vsf_wifi_t *wifi, vsf_err_t err);
static void __rt28xx_eeprom_after_iq_data2 (vsf_wifi_t *wifi, vsf_err_t err);
static void __rt28xx_eeprom_after_iq_data0 (vsf_wifi_t *wifi, vsf_err_t err);

/* Crystal-select probe: MAC_DEBUG_INDEX bit31 chooses xtal20 vs xtal40 for
 * the RF5592 channel table.  Read once during bring-up (before any
 * set_channel / connect builds a channel script). */
static uint32_t __rt28xx_macdbg_val;

static void __rt28xx_xtal_select_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE == err) {
        __rt28xx_xtal40 = (__rt28xx_macdbg_val & RT28XX_MAC_DEBUG_INDEX_XTAL) ? 1 : 0;
        vsf_wifi_chip_rt28xx_trace_info("rt28xx: MAC_DEBUG_INDEX=0x%08X xtal=%uMHz ASIC=0x%08X rev=0x%04X %s" VSF_TRACE_CFG_LINEEND,
                (unsigned)__rt28xx_macdbg_val, __rt28xx_xtal40 ? 40u : 20u,
                (unsigned)__rt28xx_asic_ver, (unsigned)(__rt28xx_asic_ver & 0xFFFF),
                __rt28xx_is_5592c() ? "(RT5592C+)" : "(pre-RT5592C)");
    } else {
        vsf_wifi_chip_rt28xx_trace_debug("rt28xx: MAC_DEBUG_INDEX read err=%d, default 20MHz xtal" VSF_TRACE_CFG_LINEEND, (int)err);
    }
    __rt28xx_eeprom_read_start(wifi);
}

static void __rt28xx_mcu_ready_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) {
        __rt28xx_chain_finish(wifi, err);
        return;
    }
    /* Stage 4a: read MAC_DEBUG_INDEX to select the RF5592 crystal table,
     * then fall through to the eFuse MAC reader (stage 4b). */
    vsf_err_t e = vsf_wifi_reg_read(wifi, RT28XX_MAC_DEBUG_INDEX,
            &__rt28xx_macdbg_val, __rt28xx_xtal_select_done);
    if (VSF_ERR_NONE != e) {
        vsf_wifi_chip_rt28xx_trace_debug("rt28xx: MAC_DEBUG_INDEX submit err=%d" VSF_TRACE_CFG_LINEEND, (int)e);
        __rt28xx_eeprom_read_start(wifi);
    }
}

/*------------------------------------------------------------------------
 * eFuse row-0 reader.
 *
 * Race-free single-instance: backing context lives at file scope because
 * the wifi-layer dispatcher's single-flight rule guarantees there is at
 * most one EEPROM read in flight per process.  When the rt28xx driver
 * later supports multiple concurrent NICs this should move into a
 * per-instance struct (e.g. via vsf_container_of from a chip_priv field).
 *------------------------------------------------------------------------*/
static struct {
    uint32_t detect_val;
    uint32_t data3, data2, data1, data0;
    uint32_t freq_raw;          /* EFUSE_DATA1 of the freq block (words 28..29) */
    uint32_t iq_raw3, iq_raw2, iq_raw0;  /* IQ block DATA3/DATA2/DATA0 */
} __rt28xx_efuse_ctx;

static bool __rt28xx_efuse_kick_clear(uint32_t val)
{
    return !(val & RT28XX_EFUSE_KICK);
}

static void __rt28xx_eeprom_read_start(vsf_wifi_t *wifi)
{
    memset(&__rt28xx_efuse_ctx, 0, sizeof(__rt28xx_efuse_ctx));
    __rt28xx_freq_offset = 0;
    __rt28xx_iq_valid = false;
    vsf_err_t err = vsf_wifi_reg_read(wifi, RT28XX_EFUSE_CTRL,
            &__rt28xx_efuse_ctx.detect_val, __rt28xx_eeprom_after_detect);
    if (VSF_ERR_NONE != err) {
        vsf_wifi_chip_rt28xx_trace_debug("rt28xx: efuse detect submit err=%d" VSF_TRACE_CFG_LINEEND, (int)err);
        __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
    }
}

static void __rt28xx_eeprom_after_detect(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) {
        vsf_wifi_chip_rt28xx_trace_debug("rt28xx: efuse detect read err=%d" VSF_TRACE_CFG_LINEEND, (int)err);
        __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
        return;
    }
    vsf_wifi_chip_rt28xx_trace_info("rt28xx: EFUSE_CTRL=0x%08X (PRESENT=%d)" VSF_TRACE_CFG_LINEEND,
            (unsigned)__rt28xx_efuse_ctx.detect_val,
            (__rt28xx_efuse_ctx.detect_val & RT28XX_EFUSE_PRESENT) ? 1 : 0);
    if (!(__rt28xx_efuse_ctx.detect_val & RT28XX_EFUSE_PRESENT)) {
        /* Off-chip EEPROM path (vendor 0x09 / EEPROM bus access) is not
         * yet wired through the reg_bus layer; bail out gracefully. */
        vsf_wifi_chip_rt28xx_trace_info("rt28xx: external EEPROM mode (no eFuse), MAC read skipped" VSF_TRACE_CFG_LINEEND);
        __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
        return;
    }
    /* KICK row 0, MODE=0 (read), AIN=0 (block index). */
    vsf_wifi_reg_op_t *ops = vsf_wifi_reg_get_scratch_ops(wifi);
    ops[0] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_EFUSE_CTRL,
            RT28XX_EFUSE_KICK | (0u << RT28XX_EFUSE_AIN_SHIFT) | (0u << RT28XX_EFUSE_MODE_SHIFT));
    vsf_err_t e = vsf_wifi_reg_run_script(wifi, ops, 1, __rt28xx_eeprom_after_kick);
    if (VSF_ERR_NONE != e) {
        vsf_wifi_chip_rt28xx_trace_debug("rt28xx: efuse kick submit err=%d" VSF_TRACE_CFG_LINEEND, (int)e);
        __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
    }
}

static void __rt28xx_eeprom_after_kick(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) { __rt28xx_chain_finish(wifi, VSF_ERR_NONE); return; }
    vsf_err_t e = vsf_wifi_reg_read_poll(wifi, RT28XX_EFUSE_CTRL,
            __rt28xx_efuse_kick_clear, /* max_retry */ 100, /* interval_ms */ 1,
            __rt28xx_eeprom_after_kick_poll);
    if (VSF_ERR_NONE != e) {
        vsf_wifi_chip_rt28xx_trace_debug("rt28xx: efuse kick poll submit err=%d" VSF_TRACE_CFG_LINEEND, (int)e);
        __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
    }
}

static void __rt28xx_eeprom_after_kick_poll(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) {
        vsf_wifi_chip_rt28xx_trace_debug("rt28xx: efuse kick busy timeout" VSF_TRACE_CFG_LINEEND);
        __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
        return;
    }
    vsf_err_t e = vsf_wifi_reg_read(wifi, RT28XX_EFUSE_DATA3,
            &__rt28xx_efuse_ctx.data3, __rt28xx_eeprom_after_data3);
    if (VSF_ERR_NONE != e) __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
}

static void __rt28xx_eeprom_after_data3(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) { __rt28xx_chain_finish(wifi, VSF_ERR_NONE); return; }
    vsf_err_t e = vsf_wifi_reg_read(wifi, RT28XX_EFUSE_DATA2,
            &__rt28xx_efuse_ctx.data2, __rt28xx_eeprom_after_data2);
    if (VSF_ERR_NONE != e) __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
}

static void __rt28xx_eeprom_after_data2(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) { __rt28xx_chain_finish(wifi, VSF_ERR_NONE); return; }
    vsf_err_t e = vsf_wifi_reg_read(wifi, RT28XX_EFUSE_DATA1,
            &__rt28xx_efuse_ctx.data1, __rt28xx_eeprom_after_data1);
    if (VSF_ERR_NONE != e) __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
}

static void __rt28xx_eeprom_after_data1(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) { __rt28xx_chain_finish(wifi, VSF_ERR_NONE); return; }
    vsf_err_t e = vsf_wifi_reg_read(wifi, RT28XX_EFUSE_DATA0,
            &__rt28xx_efuse_ctx.data0, __rt28xx_eeprom_after_data0);
    if (VSF_ERR_NONE != e) __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
}

static void __rt28xx_eeprom_after_data0(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) { __rt28xx_chain_finish(wifi, VSF_ERR_NONE); return; }
    /*
     * Decode row 0 layout (16 bytes):
     *   bytes  0..3  = EFUSE_DATA3 (LE)  -> EEPROM words 0..1
     *   bytes  4..7  = EFUSE_DATA2 (LE)  -> EEPROM words 2..3   (MAC[0..3])
     *   bytes  8..11 = EFUSE_DATA1 (LE)  -> EEPROM words 4..5   (MAC[4..5] in low 16 bits)
     *   bytes 12..15 = EFUSE_DATA0 (LE)  -> EEPROM words 6..7
     *
     * Word layout follows Linux rt2800lib (EEPROM_MAC_ADDR_0 = word 2),
     * each register storing its bytes little-endian.
     */
    uint8_t mac[6];
    mac[0] = (uint8_t)(__rt28xx_efuse_ctx.data2 >>  0);
    mac[1] = (uint8_t)(__rt28xx_efuse_ctx.data2 >>  8);
    mac[2] = (uint8_t)(__rt28xx_efuse_ctx.data2 >> 16);
    mac[3] = (uint8_t)(__rt28xx_efuse_ctx.data2 >> 24);
    mac[4] = (uint8_t)(__rt28xx_efuse_ctx.data1 >>  0);
    mac[5] = (uint8_t)(__rt28xx_efuse_ctx.data1 >>  8);
    vsf_wifi_chip_rt28xx_trace_debug(
            "rt28xx: EEPROM row0 D3=0x%08X D2=0x%08X D1=0x%08X D0=0x%08X" VSF_TRACE_CFG_LINEEND,
            (unsigned)__rt28xx_efuse_ctx.data3, (unsigned)__rt28xx_efuse_ctx.data2,
            (unsigned)__rt28xx_efuse_ctx.data1, (unsigned)__rt28xx_efuse_ctx.data0);
    /* Reject obviously invalid MACs before publishing.  A unicast MAC has
     * the multicast bit (bit 0 of the first octet) cleared; eFuse parts
     * shipped blank often read back with that bit set or with all-zero /
     * all-FF, which would brick the host stack if forwarded as-is. */
    bool all_zero  = (mac[0] | mac[1] | mac[2] | mac[3] | mac[4] | mac[5]) == 0;
    bool all_ff    = (mac[0] & mac[1] & mac[2] & mac[3] & mac[4] & mac[5]) == 0xFF;
    bool multicast = (mac[0] & 0x01) != 0;
    if (all_zero || all_ff || multicast) {
        vsf_wifi_chip_rt28xx_trace_info(
                "rt28xx: EEPROM MAC invalid %02X:%02X:%02X:%02X:%02X:%02X (eFuse %s)" VSF_TRACE_CFG_LINEEND,
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
                all_zero ? "all-zero" : all_ff ? "all-FF" : "multicast bit set");
        /* Generate a locally-administered unicast MAC so the stack stays
         * functional even with corrupted eFuse.  Bit 1 of the first octet
         * marks "locally administered"; bit 0 cleared = unicast. */
        for (int i = 0; i < 6; i++) mac[i] = (uint8_t)rand();
        mac[0] = (mac[0] & ~0x01) | 0x02;
        memcpy(wifi->mac, mac, 6);
        vsf_wifi_chip_rt28xx_trace_info(
                "rt28xx: using random MAC %02X:%02X:%02X:%02X:%02X:%02X" VSF_TRACE_CFG_LINEEND,
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    } else {
        memcpy(wifi->mac, mac, 6);
        vsf_wifi_chip_rt28xx_trace_info(
                "rt28xx: MAC %02X:%02X:%02X:%02X:%02X:%02X" VSF_TRACE_CFG_LINEEND,
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    /* MAC done; continue to read the RF frequency-offset word (EEPROM_FREQ,
     * word 0x1d) from a second eFuse block so set_channel can apply it.  A
     * failure here is non-fatal -- freq_offset just stays 0. */
    vsf_wifi_reg_op_t *ops = vsf_wifi_reg_get_scratch_ops(wifi);
    ops[0] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_EFUSE_CTRL, RT28XX_EFUSE_KICK
            | ((uint32_t)RT28XX_EFUSE_FREQ_BLOCK << RT28XX_EFUSE_AIN_SHIFT)
            | (0u << RT28XX_EFUSE_MODE_SHIFT));
    vsf_err_t fe = vsf_wifi_reg_run_script(wifi, ops, 1, __rt28xx_eeprom_after_freq_kick);
    if (VSF_ERR_NONE != fe) {
        vsf_wifi_chip_rt28xx_trace_debug("rt28xx: efuse freq kick submit err=%d" VSF_TRACE_CFG_LINEEND, (int)fe);
        __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
    }
}

/* ---- EEPROM_FREQ (word 0x1d) read chain: kick block 24, poll KICK clear,
 * then read EFUSE_DATA1 which mirrors words 28..29 (word 29 == 0x1d is the
 * high 16 bits).  Decode RFCSR17_CODE and clamp to FREQ_OFFSET_BOUND. ---- */
static void __rt28xx_eeprom_after_freq_kick(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) { __rt28xx_chain_finish(wifi, VSF_ERR_NONE); return; }
    vsf_err_t e = vsf_wifi_reg_read_poll(wifi, RT28XX_EFUSE_CTRL,
            __rt28xx_efuse_kick_clear, /* max_retry */ 100, /* interval_ms */ 1,
            __rt28xx_eeprom_after_freq_poll);
    if (VSF_ERR_NONE != e) __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
}

static void __rt28xx_eeprom_after_freq_poll(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) { __rt28xx_chain_finish(wifi, VSF_ERR_NONE); return; }
    vsf_err_t e = vsf_wifi_reg_read(wifi, RT28XX_EFUSE_DATA1,
            &__rt28xx_efuse_ctx.freq_raw, __rt28xx_eeprom_after_freq_data);
    if (VSF_ERR_NONE != e) __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
}

static void __rt28xx_eeprom_after_freq_data(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) { __rt28xx_chain_finish(wifi, VSF_ERR_NONE); return; }
    /* Block 24: DATA1 holds words 28..29 (LE).  EEPROM_FREQ is word 29 ->
     * high 16 bits; EEPROM_FREQ_OFFSET is its low byte. */
    uint16_t freq_word = (uint16_t)(__rt28xx_efuse_ctx.freq_raw >> 16);
    uint8_t off = (uint8_t)(freq_word & 0x00FFu);     /* EEPROM_FREQ_OFFSET */
    off &= RT28XX_RFCSR17_CODE;                         /* RFCSR17_CODE (0x7f) */
    if (off > RT28XX_FREQ_OFFSET_BOUND) { off = RT28XX_FREQ_OFFSET_BOUND; }
    /* 0xff EEPROM (blank cell) decodes to 0x5f after the clamp, which is a
     * valid albeit extreme code; treat an all-FF raw word as "not programmed"
     * and skip, so a blank part is not pushed to a bogus extreme. */
    if ((freq_word & 0x00FFu) == 0x00FFu) {
        vsf_wifi_chip_rt28xx_trace_debug("rt28xx: EEPROM_FREQ blank (0x%04X), freq cal skipped" VSF_TRACE_CFG_LINEEND,
                (unsigned)freq_word);
    } else {
        __rt28xx_freq_offset = off;
        vsf_wifi_chip_rt28xx_trace_debug("rt28xx: EEPROM_FREQ raw=0x%08X word=0x%04X -> RFCSR17 freq_offset=0x%02X" VSF_TRACE_CFG_LINEEND,
                (unsigned)__rt28xx_efuse_ctx.freq_raw, (unsigned)freq_word, (unsigned)off);
    }
    /* Freq done; continue to read the TX IQ calibration block (byte 0x130 ->
     * word 0x98 / block 152) so emit_channel can program BBP158/159.  Any
     * failure here is non-fatal -- iq_valid just stays false. */
    vsf_wifi_reg_op_t *ops = vsf_wifi_reg_get_scratch_ops(wifi);
    ops[0] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_EFUSE_CTRL, RT28XX_EFUSE_KICK
            | ((uint32_t)RT28XX_EFUSE_IQ_BLOCK << RT28XX_EFUSE_AIN_SHIFT)
            | (0u << RT28XX_EFUSE_MODE_SHIFT));
    vsf_err_t ie = vsf_wifi_reg_run_script(wifi, ops, 1, __rt28xx_eeprom_after_iq_kick);
    if (VSF_ERR_NONE != ie) {
        vsf_wifi_chip_rt28xx_trace_debug("rt28xx: efuse iq kick submit err=%d" VSF_TRACE_CFG_LINEEND, (int)ie);
        __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
    }
}

/* ---- TX IQ calibration block read chain: kick block 152 (word 0x98), poll
 * KICK clear, then read EFUSE_DATA3/DATA2/DATA0 which mirror bytes 0x130.. .
 * Decode the six 2.4 GHz IQ cal bytes (ref rt2800_iq_calibrate). ---- */
static void __rt28xx_eeprom_after_iq_kick(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) { __rt28xx_chain_finish(wifi, VSF_ERR_NONE); return; }
    vsf_err_t e = vsf_wifi_reg_read_poll(wifi, RT28XX_EFUSE_CTRL,
            __rt28xx_efuse_kick_clear, /* max_retry */ 100, /* interval_ms */ 1,
            __rt28xx_eeprom_after_iq_poll);
    if (VSF_ERR_NONE != e) __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
}

static void __rt28xx_eeprom_after_iq_poll(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) { __rt28xx_chain_finish(wifi, VSF_ERR_NONE); return; }
    vsf_err_t e = vsf_wifi_reg_read(wifi, RT28XX_EFUSE_DATA3,
            &__rt28xx_efuse_ctx.iq_raw3, __rt28xx_eeprom_after_iq_data3);
    if (VSF_ERR_NONE != e) __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
}

static void __rt28xx_eeprom_after_iq_data3(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) { __rt28xx_chain_finish(wifi, VSF_ERR_NONE); return; }
    vsf_err_t e = vsf_wifi_reg_read(wifi, RT28XX_EFUSE_DATA2,
            &__rt28xx_efuse_ctx.iq_raw2, __rt28xx_eeprom_after_iq_data2);
    if (VSF_ERR_NONE != e) __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
}

static void __rt28xx_eeprom_after_iq_data2(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) { __rt28xx_chain_finish(wifi, VSF_ERR_NONE); return; }
    vsf_err_t e = vsf_wifi_reg_read(wifi, RT28XX_EFUSE_DATA0,
            &__rt28xx_efuse_ctx.iq_raw0, __rt28xx_eeprom_after_iq_data0);
    if (VSF_ERR_NONE != e) __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
}

static void __rt28xx_eeprom_after_iq_data0(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) { __rt28xx_chain_finish(wifi, VSF_ERR_NONE); return; }
    /* Block 152 (word 0x98), LE byte layout:
     *   DATA3 = bytes 0x130 0x131 0x132 0x133
     *   DATA2 = bytes 0x134 0x135 0x136 0x137
     *   DATA0 = bytes 0x13C 0x13D 0x13E 0x13F */
    uint32_t r3 = __rt28xx_efuse_ctx.iq_raw3;
    uint32_t r2 = __rt28xx_efuse_ctx.iq_raw2;
    uint32_t r0 = __rt28xx_efuse_ctx.iq_raw0;
    __rt28xx_iq_gain_tx0  = (uint8_t)(r3 >>  0);   /* 0x130 */
    __rt28xx_iq_phase_tx0 = (uint8_t)(r3 >>  8);   /* 0x131 */
    __rt28xx_iq_gain_tx1  = (uint8_t)(r3 >> 24);   /* 0x133 */
    __rt28xx_iq_phase_tx1 = (uint8_t)(r2 >>  0);   /* 0x134 */
    {
        uint8_t comp  = (uint8_t)(r0 >> 0);        /* 0x13C */
        uint8_t imbal = (uint8_t)(r0 >> 8);        /* 0x13D */
        __rt28xx_iq_rf_comp  = (comp  != 0xFF) ? comp  : 0;
        __rt28xx_iq_rf_imbal = (imbal != 0xFF) ? imbal : 0;
    }
    /* CRITICAL: only apply TX IQ calibration when the eFuse IQ block is actually
     * programmed.  This dongle's eFuse returns 0xFF for the whole block (D3=D2=
     * D0=0xFFFFFFFF), meaning IQ_GAIN/IQ_PHASE = 0xFF.  0xFF is NOT a valid
     * gain/phase coefficient: forcing it into BBP159 injects a huge TX IQ
     * imbalance that corrupts the TX constellation, so the AP cannot decode ANY
     * of our uplink MPDUs and never ACKs them (observed: TX_STA_FIFO ack_ok=0
     * on every frame incl. mcs=0, AP keeps retransmitting Assoc-Resp, handshake
     * never reaches EAPOL).  Linux only ever reads programmed EEPROM bytes here
     * (never 0xFF for gain/phase).  When the gain/phase bytes are all 0xFF the
     * block is blank: leave iq_valid=false so emit_channel skips IQ entirely and
     * the chip keeps its (calibrated) default TX path. */
    __rt28xx_iq_valid =
            !((__rt28xx_iq_gain_tx0  == 0xFF) && (__rt28xx_iq_phase_tx0 == 0xFF)
           && (__rt28xx_iq_gain_tx1  == 0xFF) && (__rt28xx_iq_phase_tx1 == 0xFF));
    vsf_wifi_chip_rt28xx_trace_debug("rt28xx: IQ cal D3=0x%08X D2=0x%08X D0=0x%08X -> g0=%02X p0=%02X g1=%02X p1=%02X comp=%02X imb=%02X valid=%u" VSF_TRACE_CFG_LINEEND,
            (unsigned)r3, (unsigned)r2, (unsigned)r0,
            __rt28xx_iq_gain_tx0, __rt28xx_iq_phase_tx0,
            __rt28xx_iq_gain_tx1, __rt28xx_iq_phase_tx1,
            __rt28xx_iq_rf_comp, __rt28xx_iq_rf_imbal,
            (unsigned)__rt28xx_iq_valid);
    __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
}

/* Final stage: capture the chip revision from ASIC_VER_ID (static register,
 * used by __rt28xx_is_5592c to pick the BBP/RFCSR init tail).  By this point
 * the PBF_SYS_CTRL_READY poll has already established whether the MCU booted;
 * this read is purely for revision detection, so a non-trivial value is
 * sufficient and its absence is not treated as an MCU failure. */
static void __rt28xx_pbf_ready_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) {
        /* MCU did not raise PBF_SYS_CTRL_READY in time.  Log it but keep the
         * bring-up going so the rest of the chain still yields diagnostics
         * (the read_poll already printed the last PBF_SYS_CTRL value). */
        vsf_wifi_chip_rt28xx_trace_info("rt28xx: MCU firmware NOT ready (PBF_SYS_CTRL_READY=0), err=%d" VSF_TRACE_CFG_LINEEND, (int)err);
    } else {
        vsf_wifi_chip_rt28xx_trace_info("rt28xx: MCU firmware running (PBF_SYS_CTRL_READY=1)" VSF_TRACE_CFG_LINEEND);
    }
    vsf_err_t e = vsf_wifi_reg_read_poll(wifi, RT28XX_ASIC_VER_ID,
            __rt28xx_mcu_ready_match,
            /* max_retry  */ 200,
            /* interval_ms*/ 1,
            __rt28xx_mcu_ready_done);
    if (VSF_ERR_NONE != e) {
        __rt28xx_chain_finish(wifi, e);
    }
}

static void __rt28xx_post_fw_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) {
        __rt28xx_chain_finish(wifi, err);
        return;
    }
    /* Wait for the firmware to actually boot by polling PBF_SYS_CTRL_READY
     * (bit7), exactly as ref rt2800_load_firmware (rt2800lib.c:762-767).
     * This replaces the previous ASIC_VER_ID poll, which only proved the
     * chip-ID register was readable -- not that the MCU was executing. */
    vsf_err_t e = vsf_wifi_reg_read_poll(wifi, RT28XX_PBF_SYS_CTRL,
            __rt28xx_pbf_ready_match,
            /* max_retry  */ 200,
            /* interval_ms*/ 1,
            __rt28xx_pbf_ready_done);
    if (VSF_ERR_NONE != e) {
        __rt28xx_chain_finish(wifi, e);
    }
}

static void __rt28xx_fw_kick_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __rt28xx_pre_fw_done(vsf_wifi_t *wifi, vsf_err_t err);

static const vsf_wifi_rt28xx_bus_ops_t * __rt28xx_bus_ops(vsf_wifi_t *wifi)
{
    return (const vsf_wifi_rt28xx_bus_ops_t *)wifi->bus_ops;
}

static void __rt28xx_blob_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) {
        __rt28xx_chain_finish(wifi, err);
        return;
    }
    /* Firmware bytes are now in patch RAM but the 8051 MCU is still halted.
     * Per ref rt2800usb_write_firmware (rt2800usb.c:246-247) the CID/STATUS
     * mailboxes must be cleared to ~0 BEFORE the USB_MODE_FIRMWARE vendor
     * request, not after. */
    err = vsf_wifi_reg_run_script(wifi, __rt28xx_pre_fw_script,
            (uint16_t)dimof(__rt28xx_pre_fw_script),
            __rt28xx_pre_fw_done);
    if (VSF_ERR_NONE != err) {
        __rt28xx_chain_finish(wifi, err);
    }
}

static void __rt28xx_pre_fw_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) {
        __rt28xx_chain_finish(wifi, err);
        return;
    }
    /* Start the MCU with the ep0 vendor request (USB_DEVICE_MODE /
     * USB_MODE_FIRMWARE).  This is NOT a register write -- it is the
     * documented firmware-download handshake.  Skipping it leaves the MCU
     * dead, so no RF/PHY calibration runs and the receiver stays silent.
     *
     * vendor_request lives in the chip-private bus-ops extension
     * (vsf_wifi_rt28xx_bus_ops_t), not in the generic reg_bus, because it is
     * a USB-specific primitive rather than a chip register semantic. */
    const vsf_wifi_rt28xx_bus_ops_t *bus = __rt28xx_bus_ops(wifi);
    if (bus != NULL && bus->vendor_request != NULL) {
        err = bus->vendor_request(wifi, RT28XX_USB_DEVICE_MODE,
                RT28XX_USB_MODE_FIRMWARE, 0, __rt28xx_fw_kick_done);
        vsf_wifi_chip_rt28xx_trace_debug("rt28xx: USB_MODE_FIRMWARE vendor req submit err=%d" VSF_TRACE_CFG_LINEEND, err);
        if (VSF_ERR_NONE != err) {
            __rt28xx_chain_finish(wifi, err);
        }
    } else {
        vsf_wifi_chip_rt28xx_trace_error(
            "rt28xx: vendor_request missing from bus_ops" VSF_TRACE_CFG_LINEEND);
        __rt28xx_chain_finish(wifi, VSF_ERR_NOT_SUPPORT);
    }
}

static void __rt28xx_fw_kick_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    vsf_wifi_chip_rt28xx_trace_debug("rt28xx: USB_MODE_FIRMWARE vendor req done err=%d" VSF_TRACE_CFG_LINEEND, err);
    if (VSF_ERR_NONE != err) {
        __rt28xx_chain_finish(wifi, err);
        return;
    }
    err = vsf_wifi_reg_run_script(wifi, __rt28xx_post_fw_script,
            (uint16_t)dimof(__rt28xx_post_fw_script),
            __rt28xx_post_fw_done);
    if (VSF_ERR_NONE != err) {
        __rt28xx_chain_finish(wifi, err);
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

    /* rt2870.bin packs two 4 KiB firmware images: the first half
     * (offset 0) targets RT2860/RT2872/RT3070, the second half
     * (offset 4096) targets the newer RT3572/RT5390/RT5392/RT5592.
     * The MCU firmware RAM is only 4 KiB, so we must upload exactly the
     * matching half.  RT5592 takes the second image.  See ref
     * rt2800usb_write_firmware (rt2800usb.c:221-243): RT2860/RT2872/RT3070
     * use offset=0/length=4096, everything else offset=4096/length=4096. */
    vsf_wifi_reg_blob_t blob = {
        .data       = __rt2870_firmware_data + 4096, /* RT5592: 2nd 4 KiB image */
        .len        = 4096,
        .base_reg   = RT28XX_FW_FIRMWARE_BASE,  /* 0x3000 */
        .chunk_size = 0,                        /* hint; bus picks default */
    };
    vsf_err_t err = vsf_wifi_reg_run_blob(wifi, &blob, __rt28xx_blob_done);
    if (VSF_ERR_NONE != err) {
        wifi->backend_chain_done = NULL;
    }
    return err;
}

/*------------------------------------------------------------------------
 * Staged bring-up: MAC/BBP reset -> BBP-ready poll -> BBP/RF init.
 * Mirrors rt2800_enable_radio(), where wait_bbp_ready() gates init_bbp().
 *----------------------------------------------------------------------*/
#define RT28XX_BBP_READ_KICK \
        (RT28XX_BBP_READ_CONTROL | RT28XX_BBP_BUSY | RT28XX_BBP_RW_MODE)

static vsf_wifi_done_t __rt28xx_init_done;
static uint32_t        __rt28xx_bbp_probe;
static uint8_t         __rt28xx_bbp_wait_tries;

static void __rt28xx_bbp_probe_kick(vsf_wifi_t *wifi);
static void __rt28xx_init_built_done(vsf_wifi_t *wifi, vsf_err_t err);

/* Final stage: build and run the full BBP/RF init sequence. */
static void __rt28xx_init_emit(vsf_wifi_t *wifi)
{
    int n = __rt28xx_build_init(__rt28xx_ops_buf);
    vsf_err_t e = vsf_wifi_reg_run_script(wifi, __rt28xx_ops_buf, (uint16_t)n,
            __rt28xx_init_built_done);
    if (VSF_ERR_NONE != e && __rt28xx_init_done != NULL) {
        __rt28xx_init_done(wifi, e);
    }
}

/* Diagnostic: read RFCSR1 back after init (build_init writes 0x3F) to prove
 * the RF indirect WRITE path actually lands data on the chip. */
static uint32_t __rt28xx_rf_probe;

/* Capture each poll sample; stop once the RF engine clears BUSY (bit17). */
static bool __rt28xx_rf_busy_clear(uint32_t val)
{
    __rt28xx_rf_probe = val;
    return ((val >> 17) & 1u) == 0u;
}

static void __rt28xx_rf_verify_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    vsf_wifi_chip_rt28xx_trace_debug("rt28xx: RF_CSR_CFG readback=0x%08X (RFCSR6 data=0x%02X, "
            "init wrote 0xE4, BUSY=%u, pollerr=%d)" VSF_TRACE_CFG_LINEEND,
            (unsigned)__rt28xx_rf_probe, (unsigned)(__rt28xx_rf_probe & 0xFF),
            (unsigned)((__rt28xx_rf_probe >> 17) & 1), (int)err);
    if (__rt28xx_init_done != NULL) __rt28xx_init_done(wifi, VSF_ERR_NONE);
}

static void __rt28xx_rf_verify_kick_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) {
        if (__rt28xx_init_done != NULL) __rt28xx_init_done(wifi, VSF_ERR_NONE);
        return;
    }
    __rt28xx_rf_probe = 0;
    /* RF read latches DATA only after the serial engine clears BUSY; poll it
     * (mirrors rt2800_rfcsr_read's second WAIT_FOR_RFCSR). */
    vsf_err_t e = vsf_wifi_reg_read_poll(wifi, RT28XX_RF_CSR_CFG,
            __rt28xx_rf_busy_clear, /* max_retry */ 50, /* interval_ms */ 1,
            __rt28xx_rf_verify_done);
    if (VSF_ERR_NONE != e) {
        if (__rt28xx_init_done != NULL) __rt28xx_init_done(wifi, VSF_ERR_NONE);
    }
}

/* init script finished; kick an RF read of RFCSR1 (WRITE=0 => read). */
static void __rt28xx_init_built_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) {
        if (__rt28xx_init_done != NULL) __rt28xx_init_done(wifi, err);
        return;
    }
    __rt28xx_ops_buf[0] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_RF_CSR_CFG,
            RT28XX_RF_BUSY | (6u << 8));
    vsf_err_t e = vsf_wifi_reg_run_script(wifi, __rt28xx_ops_buf, 1,
            __rt28xx_rf_verify_kick_done);
    if (VSF_ERR_NONE != e) {
        if (__rt28xx_init_done != NULL) __rt28xx_init_done(wifi, VSF_ERR_NONE);
    }
}

/* Got BBP0 readback: if awake (non 0x00/0xFF) proceed to init; else retry. */
static void __rt28xx_bbp_probe_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    uint8_t v = (uint8_t)(__rt28xx_bbp_probe & 0xFF);
    if (VSF_ERR_NONE == err && v != 0x00 && v != 0xFF) {
        vsf_wifi_chip_rt28xx_trace_debug("rt28xx: BBP0=0x%02X ready after %u tries" VSF_TRACE_CFG_LINEEND,
                v, (unsigned)__rt28xx_bbp_wait_tries);
        __rt28xx_init_emit(wifi);
        return;
    }
    if (__rt28xx_bbp_wait_tries < 50) {
        __rt28xx_bbp_wait_tries++;
        __rt28xx_bbp_probe_kick(wifi);
        return;
    }
    vsf_wifi_chip_rt28xx_trace_info("rt28xx: BBP not ready (BBP0=0x%02X after %u tries), "
            "continuing" VSF_TRACE_CFG_LINEEND, v, (unsigned)__rt28xx_bbp_wait_tries);
    __rt28xx_init_emit(wifi);
}

/* Read-kick landed; read BBP_CSR_CFG back to capture BBP0's value. */
static void __rt28xx_bbp_kick_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) { __rt28xx_init_emit(wifi); return; }
    __rt28xx_bbp_probe = 0;
    vsf_err_t e = vsf_wifi_reg_read(wifi, RT28XX_BBP_CSR_CFG,
            &__rt28xx_bbp_probe, __rt28xx_bbp_probe_done);
    if (VSF_ERR_NONE != e) { __rt28xx_init_emit(wifi); }
}

/* Issue an indirect read request for BBP register 0 (version/status). */
static void __rt28xx_bbp_probe_kick(vsf_wifi_t *wifi)
{
    __rt28xx_ops_buf[0] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_BBP_CSR_CFG,
            RT28XX_BBP_READ_KICK | (0u << 8));
    vsf_err_t e = vsf_wifi_reg_run_script(wifi, __rt28xx_ops_buf, 1,
            __rt28xx_bbp_kick_done);
    if (VSF_ERR_NONE != e) { __rt28xx_init_emit(wifi); }
}

/* Reset pulse + H2M reactivation complete; start polling BBP0. */
static void __rt28xx_reset_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) { __rt28xx_init_emit(wifi); return; }
    __rt28xx_bbp_probe_kick(wifi);
}

/* Holds the PBF_SYS_CTRL value read at the top of init so the bit13 clear is a
 * true read-modify-write (never a blind whole-register write -- a blind
 * PBF_SYS_CTRL=0 was the very bug that previously left the receiver deaf). */
static uint32_t __rt28xx_init_pbf;

/* Stage 3: USB_MODE_RESET issued -> deassert the MAC/BBP reset and clear the
 * H2M mailbox/agent, exactly as ref rt2800usb_init_registers writes
 * MAC_SYS_CTRL=0 after the vendor reset (rt2800usb.c:291). */
static void __rt28xx_reset_clear(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) {
        __rt28xx_init_emit(wifi);
        return;
    }
    __rt28xx_ops_buf[0] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_MAC_SYS_CTRL, 0);
    __rt28xx_ops_buf[1] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_H2M_BBP_AGENT, 0);
    __rt28xx_ops_buf[2] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_H2M_MAILBOX_CSR, 0);
    vsf_err_t e = vsf_wifi_reg_run_script(wifi, __rt28xx_ops_buf, 3,
            __rt28xx_reset_done);
    if (VSF_ERR_NONE != e) { __rt28xx_init_emit(wifi); }
}

/* Stage 2: MAC/BBP reset is asserted -> fire the ep0 USB_MODE_RESET vendor
 * request that resets the digital core (and flushes the dongle's PBF/RX FIFO
 * of any stale frames carried over from a previous run).  ref
 * rt2800usb_init_registers:288. */
static void __rt28xx_reset_vendor_kick(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) { __rt28xx_init_emit(wifi); return; }
    const vsf_wifi_rt28xx_bus_ops_t *bus = __rt28xx_bus_ops(wifi);
    if (bus == NULL || bus->vendor_request == NULL) {
        __rt28xx_init_emit(wifi);
        return;
    }
    vsf_err_t e = bus->vendor_request(wifi, RT28XX_USB_DEVICE_MODE,
            RT28XX_USB_MODE_RESET, 0, __rt28xx_reset_clear);
    if (VSF_ERR_NONE != e) {
        __rt28xx_init_emit(wifi);
    }
}

/* Stage 1: got the current PBF_SYS_CTRL value -> clear its bit13 via
 * read-modify-write (preserving READY etc.) and assert the MAC/BBP reset
 * (MAC_SYS_CTRL=RESET_CSR|RESET_BBP), mirroring ref rt2800usb_init_registers
 * lines 280-286. */
static void __rt28xx_init_pbf_read_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    uint32_t pbf = (VSF_ERR_NONE == err) ? __rt28xx_init_pbf : 0;
    __rt28xx_ops_buf[0] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_PBF_SYS_CTRL,
            pbf & ~RT28XX_PBF_SYS_CTRL_RESET13);
    __rt28xx_ops_buf[1] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_MAC_SYS_CTRL,
            RT28XX_MAC_SRST | RT28XX_BBP_HRST);
    vsf_err_t e = vsf_wifi_reg_run_script(wifi, __rt28xx_ops_buf, 2,
            __rt28xx_reset_vendor_kick);
    if (VSF_ERR_NONE != e) { __rt28xx_init_emit(wifi); }
}

static vsf_err_t __rt28xx_init(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    /* Bring-up order mirrors rt2800_enable_radio -> rt2800usb_init_registers:
     *   1. read PBF_SYS_CTRL, clear bit13, assert MAC+BBP reset,
     *   2. ep0 USB_MODE_RESET vendor request (digital-core reset -- this also
     *      flushes any stale PBF/RX FIFO state left over from a previous run,
     *      which is what made repeat scans degrade into garbage/duplicate
     *      rxinfo without a physical re-plug),
     *   3. deassert reset + clear H2M mailbox/agent,
     *   4. poll BBP0 until the BBP processor is awake (non 0x00/0xFF),
     *   5. only then write the BBP/RF init sequence (build_init).
     * Steps 1-4 run as a small staged async chain; step 5 is build_init. */
    __rt28xx_init_done      = done;
    __rt28xx_bbp_wait_tries = 0;
    __rt28xx_init_pbf       = 0;
    vsf_err_t e = vsf_wifi_reg_read(wifi, RT28XX_PBF_SYS_CTRL,
            &__rt28xx_init_pbf, __rt28xx_init_pbf_read_done);
    if (VSF_ERR_NONE != e) {
        /* Cannot read PBF: fall back to a plain reset (bit13 left as-is). */
        __rt28xx_init_pbf_read_done(wifi, VSF_ERR_FAIL);
    }
    return VSF_ERR_NONE;
}

static void __rt28xx_fini(vsf_wifi_t *wifi)
{
    (void)wifi;
}

static vsf_wifi_done_t __rt28xx_setch_done;
static uint8_t         __rt28xx_setch_channel;
static uint32_t        __rt28xx_rxsta1;
static uint32_t        __rt28xx_macctrl;
static uint32_t        __rt28xx_ldo;
static uint32_t        __rt28xx_chbusy;
static uint32_t        __rt28xx_chidle;
static uint32_t        __rt28xx_setch_bbpprobe;

/* Diagnostic tail: read BBP66 back to prove the indirect BBP WRITE path is
 * effective (we wrote 0x1C in emit_channel).  RF writes are verified working
 * via RFCSR6=0xE4, but BBP0=0x77 is only a read of the version register --
 * if our BBP writes silently fail, the whole RX front-end config is lost and
 * false_cca stays 0. */
static void __rt28xx_setch_bbp_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    vsf_wifi_chip_rt28xx_trace_debug("rt28xx: after ch cfg BBP66=0x%02X (wrote 0x1C, pollerr=%d)"
            VSF_TRACE_CFG_LINEEND, (unsigned)(__rt28xx_setch_bbpprobe & 0xFF),
            (int)err);
    if (__rt28xx_setch_done != NULL) __rt28xx_setch_done(wifi, VSF_ERR_NONE);
}

static void __rt28xx_setch_bbp_kick_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) {
        if (__rt28xx_setch_done != NULL) __rt28xx_setch_done(wifi, VSF_ERR_NONE);
        return;
    }
    __rt28xx_setch_bbpprobe = 0;
    if (VSF_ERR_NONE != vsf_wifi_reg_read(wifi, RT28XX_BBP_CSR_CFG,
            &__rt28xx_setch_bbpprobe, __rt28xx_setch_bbp_done)) {
        if (__rt28xx_setch_done != NULL) __rt28xx_setch_done(wifi, VSF_ERR_NONE);
    }
}

/* Diagnostic: RFCSR3 VCOCAL_EN (bit7) is pulsed high to start a VCO/synth
 * self-calibration; the hardware clears it once the loop locks.  If we read it
 * back still set, the synthesiser never locked -> no LO -> the receiver is
 * stone deaf even though the PHY CCA timer keeps running (CH_IDLE climbs,
 * CH_BUSY stays 0).  This is the decisive test for the "PHY alive but hears
 * nothing" symptom. */
static void __rt28xx_setch_rf3_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    vsf_wifi_chip_rt28xx_trace_debug("rt28xx: after ch cfg RFCSR3=0x%02X (VCOCAL_EN=%u -> %s, "
            "pollerr=%d)" VSF_TRACE_CFG_LINEEND,
            (unsigned)(__rt28xx_rf_probe & 0xFF),
            (unsigned)((__rt28xx_rf_probe >> 7) & 1),
            ((__rt28xx_rf_probe >> 7) & 1) ? "NOT LOCKED" : "locked",
            (int)err);
    /* Chain a BBP66 read to verify the BBP write path. */
    __rt28xx_ops_buf[0] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_BBP_CSR_CFG,
            RT28XX_BBP_READ_KICK | (66u << 8));
    if (VSF_ERR_NONE != vsf_wifi_reg_run_script(wifi, __rt28xx_ops_buf, 1,
            __rt28xx_setch_bbp_kick_done)) {
        if (__rt28xx_setch_done != NULL) __rt28xx_setch_done(wifi, VSF_ERR_NONE);
    }
}

static void __rt28xx_setch_rf3_kick_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) {
        if (__rt28xx_setch_done != NULL) __rt28xx_setch_done(wifi, VSF_ERR_NONE);
        return;
    }
    __rt28xx_rf_probe = 0;
    if (VSF_ERR_NONE != vsf_wifi_reg_read_poll(wifi, RT28XX_RF_CSR_CFG,
            __rt28xx_rf_busy_clear, 50, 1, __rt28xx_setch_rf3_done)) {
        if (__rt28xx_setch_done != NULL) __rt28xx_setch_done(wifi, VSF_ERR_NONE);
    }
}

/* Diagnostic: RFCSR8 holds the synthesiser integer-N (low 8 bits) for the
 * programmed channel.  If the read-back != expected N&0xff, the per-channel RF
 * write never reached the chip (channel-program path broken). */
static void __rt28xx_setch_rf8_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    const rt28xx_rf_channel_t *rf = __rt28xx_xtal40
            ? &__rf_vals_5592_xtal40[__rt28xx_setch_channel - 1]
            : &__rf_vals_5592_xtal20[__rt28xx_setch_channel - 1];
    (void)rf;
    vsf_wifi_chip_rt28xx_trace_debug("rt28xx: after ch cfg RFCSR8=0x%02X (want N&0xff=0x%02X, "
            "pollerr=%d)" VSF_TRACE_CFG_LINEEND,
            (unsigned)(__rt28xx_rf_probe & 0xFF),
            (unsigned)(rf->n & 0xFF), (int)err);
    /* Chain RFCSR3 read (VCO lock status). */
    __rt28xx_ops_buf[0] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_RF_CSR_CFG,
            RT28XX_RF_BUSY | (3u << 8));
    if (VSF_ERR_NONE != vsf_wifi_reg_run_script(wifi, __rt28xx_ops_buf, 1,
            __rt28xx_setch_rf3_kick_done)) {
        if (__rt28xx_setch_done != NULL) __rt28xx_setch_done(wifi, VSF_ERR_NONE);
    }
}

static void __rt28xx_setch_rf8_kick_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) {
        if (__rt28xx_setch_done != NULL) __rt28xx_setch_done(wifi, VSF_ERR_NONE);
        return;
    }
    __rt28xx_rf_probe = 0;
    if (VSF_ERR_NONE != vsf_wifi_reg_read_poll(wifi, RT28XX_RF_CSR_CFG,
            __rt28xx_rf_busy_clear, 50, 1, __rt28xx_setch_rf8_done)) {
        if (__rt28xx_setch_done != NULL) __rt28xx_setch_done(wifi, VSF_ERR_NONE);
    }
}

/* Diagnostic: read RFCSR1 back after a channel is programmed to prove the
 * RX0/TX0 path-enable bits actually latch (init-time 0x3F read back as 0x03,
 * RX0/TX0 absent -- need to confirm the channel write of 0x0F sticks). */
static void __rt28xx_setch_rf1_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    vsf_wifi_chip_rt28xx_trace_debug("rt28xx: after ch cfg RFCSR1=0x%02X (want 0x0F, BUSY=%u, "
            "pollerr=%d)" VSF_TRACE_CFG_LINEEND,
            (unsigned)(__rt28xx_rf_probe & 0xFF),
            (unsigned)((__rt28xx_rf_probe >> 17) & 1), (int)err);
    /* Chain an RFCSR8 (channel N) read, then RFCSR3 (VCO lock), then BBP66. */
    __rt28xx_ops_buf[0] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_RF_CSR_CFG,
            RT28XX_RF_BUSY | (8u << 8));
    if (VSF_ERR_NONE != vsf_wifi_reg_run_script(wifi, __rt28xx_ops_buf, 1,
            __rt28xx_setch_rf8_kick_done)) {
        if (__rt28xx_setch_done != NULL) __rt28xx_setch_done(wifi, VSF_ERR_NONE);
    }
}

static void __rt28xx_setch_rf1_kick_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) {
        if (__rt28xx_setch_done != NULL) __rt28xx_setch_done(wifi, VSF_ERR_NONE);
        return;
    }
    __rt28xx_rf_probe = 0;
    if (VSF_ERR_NONE != vsf_wifi_reg_read_poll(wifi, RT28XX_RF_CSR_CFG,
            __rt28xx_rf_busy_clear, 50, 1, __rt28xx_setch_rf1_done)) {
        if (__rt28xx_setch_done != NULL) __rt28xx_setch_done(wifi, VSF_ERR_NONE);
    }
}

static void __rt28xx_setch_chconf_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    /* Only probe on channel 1 to keep the scan log readable. */
    if (__rt28xx_setch_channel != 1) {
        if (__rt28xx_setch_done != NULL) __rt28xx_setch_done(wifi, err);
        return;
    }
    __rt28xx_ops_buf[0] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_RF_CSR_CFG,
            RT28XX_RF_BUSY | (1u << 8));
    if (VSF_ERR_NONE != vsf_wifi_reg_run_script(wifi, __rt28xx_ops_buf, 1,
            __rt28xx_setch_rf1_kick_done)) {
        if (__rt28xx_setch_done != NULL) __rt28xx_setch_done(wifi, VSF_ERR_NONE);
    }
}

/* Diagnostic step: after reading the prior dwell's RX_STA_CNT1 and the live
 * MAC_SYS_CTRL, log them and then program the requested channel.
 * false_cca>0 proves the RF/PHY front end is alive (so 0 RX frames => USB DMA
 * path); all-zero => RF/PHY dead.  MAC_SYS_CTRL bit3(RX_EN) tells us whether
 * the MAC receiver is actually enabled at dwell time (a 0 there is the root
 * cause of a permanently-zero false_cca). */
static void __rt28xx_setch_after_diag(vsf_wifi_t *wifi, vsf_err_t err)
{
    uint16_t false_cca = (uint16_t)(__rt28xx_rxsta1 & 0xFFFF);
    uint16_t plcp_err  = (uint16_t)(__rt28xx_rxsta1 >> 16);
    (void)false_cca; (void)plcp_err;
    uint32_t ldo_new   = __rt28xx_ldo & ~RT28XX_LDO_CFG0_VLEVEL_MASK;
    int n;
    if (__rt28xx_setch_channel > 14) {
        ldo_new |= RT28XX_LDO_CFG0_VLEVEL_5G;   /* 5 GHz LDO_CORE_VLEVEL=5 */
    }
    vsf_wifi_chip_rt28xx_trace_debug("rt28xx: RX_STA_CNT1=0x%08X false_cca=%u plcp_err=%u "
            "MAC_SYS_CTRL=0x%08X (TX_EN=%u RX_EN=%u) LDO_CFG0=0x%08X (VLEVEL=%u) "
            "CH_IDLE=%u CH_BUSY_STA=%u (ch->%u)"
            VSF_TRACE_CFG_LINEEND, __rt28xx_rxsta1, false_cca, plcp_err,
            __rt28xx_macctrl, (unsigned)((__rt28xx_macctrl >> 2) & 1),
            (unsigned)((__rt28xx_macctrl >> 3) & 1), __rt28xx_ldo,
            (unsigned)((__rt28xx_ldo >> 26) & 0x7), __rt28xx_chidle,
            __rt28xx_chbusy, __rt28xx_setch_channel);
    /* config_channel_rf55xx() first writes LDO_CFG0 with LDO_CORE_VLEVEL=0 for
     * 2.4 GHz / 5 for 5 GHz (read-modify-write, preserving DELAY/BGSEL/LDO25). */
    __rt28xx_ops_buf[0] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_LDO_CFG0, ldo_new);
    n = __rt28xx_emit_channel(__rt28xx_ops_buf, 1, __rt28xx_setch_channel);
    if (VSF_ERR_NONE != vsf_wifi_reg_run_script(wifi, __rt28xx_ops_buf, (uint16_t)n,
            __rt28xx_setch_chconf_done)) {
        if (__rt28xx_setch_done != NULL) {
            __rt28xx_setch_done(wifi, VSF_ERR_FAIL);
        }
    }
}

/* Chain: MAC_SYS_CTRL captured -> read LDO_CFG0 -> read CH_IDLE_STA ->
 * read CH_BUSY_STA.  CH_IDLE>0 with CH_BUSY=0 means the CCA engine runs but
 * always sees the medium idle (sensitivity/AGC); both 0 means the CCA engine
 * never runs at all (PHY clock / enable). */
static void __rt28xx_setch_read_chbusy(vsf_wifi_t *wifi, vsf_err_t err)
{
    __rt28xx_chbusy = 0;
    if (VSF_ERR_NONE != vsf_wifi_reg_read(wifi, RT28XX_CH_BUSY_STA,
            &__rt28xx_chbusy, __rt28xx_setch_after_diag)) {
        __rt28xx_setch_after_diag(wifi, err);
    }
}

/* Chain: LDO_CFG0 captured -> read CH_IDLE_STA before CH_BUSY_STA. */
static void __rt28xx_setch_read_chidle(vsf_wifi_t *wifi, vsf_err_t err)
{
    __rt28xx_chidle = 0;
    if (VSF_ERR_NONE != vsf_wifi_reg_read(wifi, RT28XX_CH_IDLE_STA,
            &__rt28xx_chidle, __rt28xx_setch_read_chbusy)) {
        __rt28xx_setch_read_chbusy(wifi, err);
    }
}

/* Chain: MAC_SYS_CTRL captured -> now read LDO_CFG0 before logging/program. */
static void __rt28xx_setch_read_ldo(vsf_wifi_t *wifi, vsf_err_t err)
{
    __rt28xx_ldo = 0;
    if (VSF_ERR_NONE != vsf_wifi_reg_read(wifi, RT28XX_LDO_CFG0,
            &__rt28xx_ldo, __rt28xx_setch_read_chidle)) {
        __rt28xx_setch_read_chidle(wifi, err);
    }
}

/* Chain: RX_STA_CNT1 captured -> now read MAC_SYS_CTRL before logging. */
static void __rt28xx_setch_read_macctrl(vsf_wifi_t *wifi, vsf_err_t err)
{
    __rt28xx_macctrl = 0;
    if (VSF_ERR_NONE != vsf_wifi_reg_read(wifi, RT28XX_MAC_SYS_CTRL,
            &__rt28xx_macctrl, __rt28xx_setch_read_ldo)) {
        __rt28xx_setch_read_ldo(wifi, err);
    }
}

static vsf_err_t __rt28xx_set_channel(vsf_wifi_t *wifi, uint8_t channel,
        vsf_wifi_done_t done)
{
    if (NULL == __rt28xx_find_rf(channel)) return VSF_ERR_INVALID_PARAMETER;
    __rt28xx_setch_done    = done;
    __rt28xx_setch_channel = channel;
    __rt28xx_rxsta1        = 0;
    /* Read prior dwell's RX_STA_CNT1 first, then MAC_SYS_CTRL (diagnostic),
     * then set channel. */
    if (VSF_ERR_NONE != vsf_wifi_reg_read(wifi, RT28XX_RX_STA_CNT1,
            &__rt28xx_rxsta1, __rt28xx_setch_read_macctrl)) {
        /* readback unavailable: fall back to plain channel program */
        int n = __rt28xx_emit_channel(__rt28xx_ops_buf, 0, channel);
        return vsf_wifi_reg_run_script(wifi, __rt28xx_ops_buf, (uint16_t)n, done);
    }
    return VSF_ERR_NONE;
}

static vsf_err_t __rt28xx_set_rx_filter(vsf_wifi_t *wifi, uint32_t mask,
        vsf_wifi_done_t done)
{
    vsf_wifi_reg_op_t *ops = vsf_wifi_reg_get_scratch_ops(wifi);
    ops[0] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_RX_FILTER_CFG, mask);
    return vsf_wifi_reg_run_script(wifi, ops, 1, done);
}

static vsf_err_t __rt28xx_set_mac_addr(vsf_wifi_t *wifi, const uint8_t mac[6],
        vsf_wifi_done_t done)
{
    uint32_t dw0 = (uint32_t)mac[0]
                 | ((uint32_t)mac[1] <<  8)
                 | ((uint32_t)mac[2] << 16)
                 | ((uint32_t)mac[3] << 24);
    /* UNICAST_TO_ME_MASK (bits 23:16) = 0xFF: all 6 MAC bytes must match
     * for hardware to consider a frame "unicast to me" (auto-ACK, filtering).
     * Ref: rt2800lib.c:2059 rt2x00_set_field32(&reg, MAC_ADDR_DW1_UNICAST_TO_ME_MASK, 0xff). */
    /* Linux rt2800lib sets UNICAST_TO_ME_MASK (bits 23:16) to 0xFF so the
     * hardware matches all six MAC-address bytes for "unicast to me".
     * Keep the high byte clear; only the mask and the two low MAC bytes are
     * documented fields in MAC_ADDR_DW1. */
    uint32_t dw1 = (uint32_t)mac[4] | ((uint32_t)mac[5] << 8)
                 | (0x00FFu << 16);   /* UNICAST_TO_ME_MASK = 0xFF */
    vsf_wifi_reg_op_t *ops = vsf_wifi_reg_get_scratch_ops(wifi);
    ops[0] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_MAC_ADDR_DW0, dw0);
    ops[1] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_MAC_ADDR_DW1, dw1);
    return vsf_wifi_reg_run_script(wifi, ops, 2, done);
}

static vsf_err_t __rt28xx_set_bssid(vsf_wifi_t *wifi, const uint8_t bssid[6],
        vsf_wifi_done_t done)
{
    vsf_wifi_reg_op_t *ops = vsf_wifi_reg_get_scratch_ops(wifi);
    int n = __rt28xx_emit_bssid(ops, 0, bssid);
    return vsf_wifi_reg_run_script(wifi, ops, (uint16_t)n, done);
}

static vsf_err_t __rt28xx_set_auth_mode(vsf_wifi_t *wifi,
        const vsf_wifi_auth_cfg_t *cfg, vsf_wifi_done_t done)
{
    /* Raw-protocol backend: no MLME / WPA handshake. */
    (void)cfg;
    if (done != NULL) done(wifi, VSF_ERR_NONE);
    return VSF_ERR_NONE;
}

static int __rt28xx_emit_wcid(vsf_wifi_reg_op_t *ops, int n, const uint8_t mac[6],
        uint32_t attr)
{
    /* Program WCID 1 entry with AP MAC + the supplied WCID_ATTR.
     *
     * Bit layout (MAC_WCID_ATTRIBUTE_* in rt2800.h):
     *   KEYTAB (bit 0)   = 1 : peer entry is valid / key-installed
     *   CIPHER (bits 1-3)    : 0=none, 1=WEP64, 2=WEP128, 3=TKIP, 4=AES(CCMP)
     *   BSS_IDX(4-6)/WIUDF(7-9)=0
     *
     * For WPA2-PSK STA mode with software CCMP we use KEYTAB=1 and CIPHER=0
     * so the hardware treats WCID 1 as a valid peer but does not try to
     * decrypt frames.  With KEYTAB=0 the RX classifier silently drops unicast
     * data frames (including plaintext EAPOL-Key M1).  A non-zero CIPHER
     * would route encrypted frames through the hardware cipher engine; we
     * want raw encrypted frames for software CCMP.  During disconnect
     * attr=0x00000000 clears the entry. */
    uint32_t lo = (uint32_t)mac[0] | ((uint32_t)mac[1] << 8)
                | ((uint32_t)mac[2] << 16) | ((uint32_t)mac[3] << 24);
    /* The Windows native driver leaves the upper 16 bits of the WCID entry
     * at 0x0000 (ref/rt5572_win_usb.log L17975), not 0xFFFF.  Keep the
     * writable part of the register exactly as observed. */
    uint32_t hi = (uint32_t)mac[4] | ((uint32_t)mac[5] << 8);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_MAC_WCID_ENTRY(RT28XX_STA_WCID),     lo);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_MAC_WCID_ENTRY(RT28XX_STA_WCID) + 4, hi);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_MAC_WCID_ATTR_ENTRY(RT28XX_STA_WCID), attr);
    return n;
}

#if VSF_WIFI_USE_WPA == ENABLED && VSF_WIFI_CFG_RT28XX_HW_CRYPTO == ENABLED

/* Shadow of the four SHARED_KEY_MODE registers so we can update the cipher
 * bits for group keys without an extra register read.  Cleared to 0 in init. */
static uint32_t __rt28xx_shared_key_mode[4];


static void __rt28xx_emit_key32(vsf_wifi_reg_op_t *ops, int *n,
        uint16_t base, const uint8_t key[32])
{
    for (int i = 0; i < 8; i++) {
        uint32_t v = ((uint32_t)key[i*4 + 0])
                   | ((uint32_t)key[i*4 + 1] << 8)
                   | ((uint32_t)key[i*4 + 2] << 16)
                   | ((uint32_t)key[i*4 + 3] << 24);
        ops[(*n)++] = (vsf_wifi_reg_op_t)RT_OP_REG(base + (i * 4), v);
    }
}

static void __rt28xx_emit_iveiv(vsf_wifi_reg_op_t *ops, int *n,
        uint8_t key_idx, uint16_t base)
{
    /* CCMP IV/EIV.  iv[3] bits: ExtIV=0x20, keyid in upper 2 bits. */
    uint32_t iv_lo = 0x20 | ((uint32_t)(key_idx & 3) << 6);
    ops[(*n)++] = (vsf_wifi_reg_op_t)RT_OP_REG(base, iv_lo);       /* iv[0..3] */
    ops[(*n)++] = (vsf_wifi_reg_op_t)RT_OP_REG(base + 4, 0);       /* iv[4..7] */
}

static void __rt28xx_key_install_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (err != VSF_ERR_NONE) {
        vsf_wifi_chip_rt28xx_trace_info(
                "wifi: rt28xx key install script failed (err=%d)"
                VSF_TRACE_CFG_LINEEND, (int)err);
    }
}

static vsf_err_t __rt28xx_crypto_install_key(vsf_wifi_t *wifi, uint8_t key_idx,
        bool pairwise, const uint8_t *key, uint8_t key_len,
        const uint8_t *mac, vsf_wifi_done_t done)
{
    (void)mac;
    if ((key == NULL) || (key_len != 16)) {
        return VSF_ERR_INVALID_PARAMETER;
    }

    uint8_t full_key[32];
    memset(full_key, 0, sizeof(full_key));
    memcpy(full_key, key, key_len);

    vsf_wifi_reg_op_t *ops = vsf_wifi_reg_get_scratch_ops(wifi);
    int n = 0;

    if (pairwise) {
        uint16_t pbase = RT28XX_PAIRWISE_KEY_ENTRY(RT28XX_STA_WCID);
        __rt28xx_emit_key32(ops, &n, pbase, full_key);

        uint32_t attr = RT28XX_WCID_ATTR_KEYTAB
                      | (RT28XX_CIPHER_AES << RT28XX_WCID_ATTR_CIPHER_SHIFT)
                      | (RT28XX_CIPHER_AES << RT28XX_WCID_ATTR_WIUDF_SHIFT);
        ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(
                RT28XX_MAC_WCID_ATTR_ENTRY(RT28XX_STA_WCID), attr);
        __rt28xx_emit_iveiv(ops, &n, 0, RT28XX_MAC_IVEIV_ENTRY(RT28XX_STA_WCID));
    } else {
        uint8_t gtk_idx = key_idx & 3;
        uint8_t hw_idx  = gtk_idx;   /* bssidx = 0 in STA mode */
        uint16_t sbase  = RT28XX_SHARED_KEY_ENTRY(hw_idx);
        __rt28xx_emit_key32(ops, &n, sbase, full_key);

        /* Update shadow and write the SHARED_KEY_MODE register this slot lives in. */
        uint8_t mode_reg = hw_idx / 8;
        uint8_t mode_bit = (hw_idx % 8) * 3;
        __rt28xx_shared_key_mode[mode_reg] &= ~(0x7u << mode_bit);
        __rt28xx_shared_key_mode[mode_reg] |= (uint32_t)RT28XX_CIPHER_AES << mode_bit;
        ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(
                RT28XX_SHARED_KEY_MODE_ENTRY(mode_reg),
                __rt28xx_shared_key_mode[mode_reg]);

        /* IVEIV for the shared key slot so hardware can insert/verify IV. */
        __rt28xx_emit_iveiv(ops, &n, gtk_idx, RT28XX_MAC_IVEIV_ENTRY(hw_idx));
    }

    /* install_key is called from the wifi EDA and must follow the VSF
     * non-blocking pattern: submit the register script and return.  The
     * dispatcher invokes `done` when the script finishes. */
    return vsf_wifi_reg_run_script(wifi, ops, (uint16_t)n,
            done != NULL ? done : __rt28xx_key_install_done);
}

static const vsf_wifi_crypto_ops_t __rt28xx_crypto_ops = {
    .install_key = __rt28xx_crypto_install_key,
};

#endif      /* VSF_WIFI_USE_WPA && VSF_WIFI_CFG_RT28XX_HW_CRYPTO */

static vsf_err_t __rt28xx_connect(vsf_wifi_t *wifi,
        const uint8_t bssid[6], const uint8_t *ssid, uint8_t ssid_len,
        uint8_t channel, vsf_wifi_done_t done)
{
    (void)ssid; (void)ssid_len;
    if (NULL == __rt28xx_find_rf(channel)) return VSF_ERR_INVALID_PARAMETER;

    /* bssid(2) + wcid(3) + channel(~75) + edca/wmm(~20) -> shared static buffer. */
    vsf_wifi_reg_op_t *ops = __rt28xx_ops_buf;
    int n = 0;
    /* MAC_BSSID (0x1010/0x1014): the Windows Ralink driver stores our own
     * STA MAC here with BSS_ID_MASK=3 (ref/rt5572_win_usb.log L6547-6550,
     * L8235-8238, L18043-18047).  With BSS_ID_MASK=3 the strict BSSID
     * comparison is disabled, so frames addressed to us (addr1 matches our
     * MAC/WCID) are accepted regardless of the BSSID field. */
    n = __rt28xx_emit_bssid  (ops, n, wifi->mac);  /* 0x1010 = own MAC, mask=3 */
    /* WCID / peer configuration for WPA2-PSK STA mode (matches Windows):
     *
     *   WCID 0 ATTR   = 0x00000001 (KEYTAB=1) — default fallback WCID used
     *                   for plaintext EAPOL-Key M1/M3 before pairwise key.
     *   WCID 1 entry  = AP BSSID
     *   WCID 1 ATTR   = 0x00000001 (KEYTAB=1, CIPHER=0/none)
     *
     * KEYTAB=1 tells the RX classifier that the WCID is a valid peer, so
     * unicast data frames (including plaintext EAPOL-Key M1) from the AP
     * are DMA'd to USB instead of being silently dropped.  CIPHER is left
     * at 0 because we use software CCMP; a non-zero cipher would make the
     * hardware try to decrypt frames before handing them to the driver. */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_MAC_WCID_ATTR_ENTRY(0), 0x00000001u);
    n = __rt28xx_emit_wcid   (ops, n, bssid, 0x00000001u);
    /* SHARED_KEY_MODE: Windows sets entry 0 to 0x2AA7 at connect time
     * (ref/rt5572_win_usb.log L17978-L17979: 0x7000 = 0x2AA70000).
     * The group-key mode bits are updated by the WPA layer after M3. */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(0x7000, 0x2AA70000u);
    n = __rt28xx_emit_channel(ops, n, channel);
    /* --- EDCA / WMM: match Windows connect sequence (ref/rt5572_win_usb.log
     *     L23597-L23614).  These are the 802.11e/WMM access-category parameters
     *     that the AP expects from an associated STA.  Without them the chip uses
     *     power-on defaults that may violate the AP's admission-control policy,
     *     causing it to silently discard our uplink frames (including ACKs for
     *     its own downlink management frames).  Values are standard WMM
     *     infrastructure-mode defaults: BK(AIFSN=7,CW=4/10,TXOP=0),
     *     BE(4,4,10,0), VI(3,4,4,0x5E=94), VO(2,3,4,0x2F=47). ---- */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_EDCA_AC0_CFG, 0x00064300);  /* BK */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_EDCA_AC1_CFG, 0x000A4700);  /* BE */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_EDCA_AC2_CFG, 0x00043338);  /* VI */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_EDCA_AC3_CFG, 0x0003222F);  /* VO */
    /* WMM parameter values captured from the Windows native driver connect
     * sequence (ref/rt5572_win_usb.log L23609-L23614).  These differ from our
     * earlier hard-coded defaults and match the AP's expected infrastructure
     * EDCA parameters more closely. */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_WMM_AIFSN_CFG, 0x00001273);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_WMM_CWMIN_CFG, 0x00001344);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_WMM_CWMAX_CFG, 0x000034A6);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_WMM_TXOP0_CFG, 0x00000000);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_WMM_TXOP1_CFG, 0x002F005E);
    /* TXOP_THRES_CFG: Windows writes 0 during connect (ref L23656).  A non-zero
     * threshold may prevent the auto-responder from sending ACK when the
     * channel-busy estimate exceeds the limit.  Force to 0 = no restriction. */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_TXOP_THRES_CFG, 0x00000000);
    /* AUTO_RSP_CFG + LEGACY_BASIC_RATE: re-program during connect to match
     * Windows native driver (ref L23615/L23619).  Windows writes 0x017F for
     * LEGACY_BASIC_RATE (includes 12 Mbps rate bit that our init 0x013F omits).
     * AUTO_RSP_CFG = 0x13 = AUTORESPONDER | BAC_ACK_POLICY | AR_PREAMBLE. */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_AUTO_RSP_CFG, 0x00000013);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_LEGACY_BASIC_RATE, 0x0000017F);
    /* BCN_TIME_CFG: enable STA mode TSF sync + TBTT timer.  Windows native
     * driver writes 0x000B0640 in the connect sequence (ref/rt5572_win_usb.log
     * L23441-L23444): BEACON_INTERVAL=0x0640 (1600 TU = 1.6384s scaled),
     * TSF_TICKING=1 (bit16), TSF_SYNC=01 STA mode (bits17-18, follow AP TSF),
     * TBTT_ENABLE=1 (bit19).  Linux rt2800_config_intf does the same via
     * BCN_TIME_CFG_TSF_SYNC = TSF_SYNC_INFRA after STA association.  Without
     * this the chip MAC RX path silently drops to-me unicast data frames
     * (EAPOL M1 etc) even though they are physically received and ACKed --
     * the chip apparently treats the BSS as not yet joined and refuses to
     * DMA data frames to USB.  init writes 0x00006400 (no TSF sync) which is
     * fine for scan/pre-assoc; this overrides for the connected state. */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_BCN_TIME_CFG, 0x000B0640);
    /* RX_FILTER_CFG: drop CRC/PHY/NOT_TO_ME errors, keep bc/mc.
     * DROP_NOT_TO_ME(bit2)=1 is critical: without it the USB is flooded with
     * all other stations' frames, wasting bandwidth. */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_RX_FILTER_CFG, 0x00017F97);
    /* HT_FBK_CFG0 / LG_FBK_CFG0: rate fallback tables for TX rate adaptation.
     * Windows writes these during connect (ref/rt5572_win_usb.log L23640-23643).
     * The hardware reset default for HT_FBK_CFG0 is 0x00004360 (each MCS falls
     * back to a lower MCS) and LG_FBK_CFG0 is 0x00000000.  Explicitly writing
     * them ensures the TX rate adaptation engine is properly configured and
     * may also serve as a MAC register bus "warm-up" that activates the TX/RX
     * data path. */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_HT_FBK_CFG0, 0x00004360);
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_LG_FBK_CFG0, 0x00000000);
    wifi->channel = channel;
    return vsf_wifi_reg_run_script(wifi, ops, (uint16_t)n, done);
}

static vsf_err_t __rt28xx_disconnect(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    static const uint8_t zero_bssid[6] = {0};
    vsf_wifi_reg_op_t *ops = vsf_wifi_reg_get_scratch_ops(wifi);
    int n = 0;
    n = __rt28xx_emit_bssid(ops, n, zero_bssid);                          /* 2 ops */
    n = __rt28xx_emit_wcid (ops, n, zero_bssid, 0x00000000);              /* 3 ops: clear WCID 1 */
    ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_RX_FILTER_CFG, 0);   /* 1 op  */
#if VSF_WIFI_USE_WPA == ENABLED && VSF_WIFI_CFG_RT28XX_HW_CRYPTO == ENABLED
    /* Clear any programmed GTK cipher mode. */
    for (int i = 0; i < 4; i++) {
        __rt28xx_shared_key_mode[i] = 0;
        ops[n++] = (vsf_wifi_reg_op_t)RT_OP_REG(RT28XX_SHARED_KEY_MODE_ENTRY(i), 0);
    }
#endif
    return vsf_wifi_reg_run_script(wifi, ops, (uint16_t)n, done);
}

static vsf_err_t __rt28xx_get_link_info(vsf_wifi_t *wifi,
        vsf_wifi_link_info_t *info)
{
    memset(info, 0, sizeof(*info));
    info->channel = wifi->channel;
    return VSF_ERR_NONE;
}

/*============================ TX DESCRIPTOR =================================*
 *
 * USB TX bulk-out payload layout (rt2x00usb / rt2800usb), mirror of RX:
 *
 *   | TXINFO (4B) | TXWI (20B, RT5592 5-word) | 802.11 frame | pad | USB end pad |
 *                 |<-- TXINFO_W0_USB_DMA_TX_PKT_LEN = TXWI + roundup(frame,4) -->|
 *
 * TXINFO_W0  (ref rt2800usb.h:46-51 / rt2800usb_write_tx_desc):
 *   [15:0] USB_DMA_TX_PKT_LEN  (everything after TXINFO)
 *   bit24  WIV   (1 = no per-frame IV in descriptor; hw uses IVEIV reg)
 *   [26:25]QSEL  (2 = AC_BE)
 * TXWI_W0    (ref rt2800.h:3080-3092):
 *   [31:30]PHYMODE (1 = OFDM), [22:16] MCS (0 = 6 Mbps).  OFDM/MCS0 is legal
 *           on both 2.4 GHz and 5 GHz, so one rate covers every channel.
 * TXWI_W1    (ref rt2800.h:3110-3117):
 *   bit1   NSEQ (1 = hw assigns the 802.11 sequence number)
 *   [27:16]MPDU_TOTAL_BYTE_COUNT (real 802.11 length)
 *   [31:30]PACKETID_ENTRY (1 -> latch result into TX_STA_FIFO, non-zero PID)
 * Words 2-4 are IV/EIV/reserved and must be zero for unencrypted frames.
 *==========================================================================*/

#define RT28XX_TXINFO_DESC_SIZE         4
#define RT28XX_TXWI_DESC_SIZE_5592      20
#define RT28XX_TXINFO_W0_QSEL_BE        (2u << 25)
#define RT28XX_TXWI_W0_PHYMODE_OFDM     (1u << 30)
#define RT28XX_TXWI_W1_ACK              (1u << 0)
#define RT28XX_TXWI_W1_NSEQ             (1u << 1)
#define RT28XX_TXWI_W1_PACKETID_ENTRY1  (1u << 30)

static inline void __rt28xx_put_le32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)( v        & 0xFF);
    p[1] = (uint8_t)((v >>  8) & 0xFF);
    p[2] = (uint8_t)((v >> 16) & 0xFF);
    p[3] = (uint8_t)((v >> 24) & 0xFF);
}

/* drv->build_tx: wrap a raw 802.11 frame into TXINFO + TXWI + frame + pad.
 * The RT2800 hardware requires L2 padding between the 802.11 header and the
 * payload so that the payload starts at a 4-byte aligned offset from the
 * TXWI start (ref: rt2800lib.c L2PAD_SIZE, REQUIRE_L2PAD).  For a 26-byte
 * QoS header: (20 + 26) mod 4 = 2, so 2 bytes of L2PAD are inserted.
 * MPDU_TOTAL_BYTE_COUNT in TXWI is the actual on-air frame size (no L2PAD);
 * the USB DMA pkt_len includes L2PAD. */
static uint16_t __rt28xx_build_tx(vsf_wifi_t *wifi, uint8_t *dst,
        uint16_t dst_cap, const uint8_t *frame, uint16_t frame_len)
{
    (void)wifi;
    /* Temporary diagnostic: identify every non-probe-request frame we hand
     * to the chip so we can correlate TX_STA_FIFO entries with the actual
     * 802.11 header during auth/assoc/4-way.  Probe requests are numerous
     * during scans and only clutter the log. */
    if ((frame_len >= 10) && ((frame[0] & 0xFC) != 0x40)) {
        static uint32_t __tx_build_cnt = 0;
        if (__tx_build_cnt < 80) {
            __tx_build_cnt++;
            vsf_wifi_chip_rt28xx_trace_info("wifi: TX build[%u] fc=%02X%02X a1=%02X:%02X:%02X:%02X:%02X:%02X len=%u"
                    VSF_TRACE_CFG_LINEEND,
                    (unsigned)__tx_build_cnt,
                    frame[0], frame[1],
                    frame[4], frame[5], frame[6], frame[7], frame[8], frame[9],
                    (unsigned)frame_len);
        }
    }
    /* Determine 802.11 header length (QoS data has 2 extra bytes).
     * QoS: Type must be Data (bits[3:2]=10 → byte0 & 0x0C == 0x08) AND
     * subtype bit3 must be set (bit7 of byte0). */
    uint16_t hdr_len = 24;
    if ((frame_len >= 2) && ((frame[0] & 0x0C) == 0x08) && (frame[0] & 0x80)) {
        hdr_len = 26;   /* QoS Data subtype: 2-byte QoS Control present */
    }
    /* L2PAD: align payload to 4 bytes relative to TXWI start. */
    uint16_t l2pad = (uint16_t)((-hdr_len) & 3);

    uint16_t hdr       = RT28XX_TXINFO_DESC_SIZE + RT28XX_TXWI_DESC_SIZE_5592;
    uint16_t frame_pad = (uint16_t)((frame_len + l2pad + 3u) & ~3u);
    uint16_t total     = (uint16_t)(hdr + frame_pad + 4); /* + USB end pad */
    if ((0 == frame_len) || (frame_len > 0x0FFFu) || (total > dst_cap)) {
        return 0;
    }

    uint16_t pkt_len   = (uint16_t)(RT28XX_TXWI_DESC_SIZE_5592 + frame_pad);
    bool is_data = (frame_len >= 2) && ((frame[0] & 0x0Cu) == 0x08u);
#if VSF_WIFI_USE_WPA == ENABLED
    bool hw_encrypt = !wifi->raw_radio_active && wifi->wpa_hw_crypto && is_data;
#else
    bool hw_encrypt = false;
#endif
    bool is_mcast   = (frame_len >= 6) && ((frame[4] & 0x01) != 0);
    uint32_t txinfo_w0 = ((uint32_t)pkt_len & 0xFFFFu)
                       | RT28XX_TXINFO_W0_QSEL_BE
                       | (hw_encrypt ? 0u : RT28XX_TXINFO_W0_WIV);
    uint32_t txwi_w0   = RT28XX_TXWI_W0_PHYMODE_OFDM;   /* OFDM, MCS0 (6 Mbps) */

#if VSF_WIFI_USE_WPA == ENABLED
    uint8_t wcid = hw_encrypt ? (is_mcast ? wifi->wpa_gtk_keyidx : RT28XX_STA_WCID)
                              : RT28XX_STA_WCID;
#else
    uint8_t wcid = RT28XX_STA_WCID;
#endif

    uint32_t txwi_w1   = RT28XX_TXWI_W1_ACK
                       | (is_data ? RT28XX_TXWI_W1_NSEQ : 0u)
                       | ((uint32_t)wcid << 8)
                       | (((uint32_t)frame_len & 0x0FFFu) << 16)
                       | RT28XX_TXWI_W1_PACKETID_ENTRY1;

    __rt28xx_put_le32(dst +  0, txinfo_w0);
    __rt28xx_put_le32(dst +  4, txwi_w0);
    __rt28xx_put_le32(dst +  8, txwi_w1);
    __rt28xx_put_le32(dst + 12, 0);     /* TXWI W2 (IV)  */
    __rt28xx_put_le32(dst + 16, 0);     /* TXWI W3 (EIV) */
    __rt28xx_put_le32(dst + 20, 0);     /* TXWI W4 (RT5592 5-word) */

    /* Copy header, insert L2PAD gap, then copy payload. */
    uint16_t payload_off = (hdr_len < frame_len) ? hdr_len : frame_len;
    memcpy(dst + hdr, frame, payload_off);              /* 802.11 hdr */
    if (hw_encrypt) {
        /* Hardware crypto: mark the frame as protected and let the chip
         * insert the CCMP IV/EIV from the IVEIV registers. */
        dst[hdr + 1] |= 0x40;
        static uint32_t __hw_tx_cnt = 0;
        if (++__hw_tx_cnt <= 20) {
            vsf_wifi_chip_rt28xx_trace_info(
                    "wifi: HW TX[%u] wcid=%u mcast=%u fc=%02X%02X len=%u"
                    VSF_TRACE_CFG_LINEEND,
                    (unsigned)__hw_tx_cnt, (unsigned)wcid, (unsigned)is_mcast,
                    frame[0], frame[1], (unsigned)frame_len);
        }
    }
    if (l2pad > 0) {
        memset(dst + hdr + payload_off, 0, l2pad);      /* L2 padding */
    }
    if (payload_off < frame_len) {
        memcpy(dst + hdr + payload_off + l2pad,
               frame + payload_off, frame_len - payload_off);  /* payload */
    }
    /* zero the 4-byte alignment pad + the trailing USB end pad. */
    uint16_t data_end = (uint16_t)(hdr + payload_off + l2pad + (frame_len - payload_off));
    memset(dst + data_end, 0, (size_t)(total - data_end));
    return total;
}

#ifndef RT28XX_TX_BUF_SIZE
#   define RT28XX_TX_BUF_SIZE   1600
#endif

/* drv->tx: build the on-wire TX descriptor and hand it to the register bus. */
static vsf_err_t __rt28xx_tx(vsf_wifi_t *wifi, const uint8_t *frame, uint16_t len)
{
    static uint32_t __tx_buf32[(RT28XX_TX_BUF_SIZE + 3) / 4];
    uint8_t *tx_buf = (uint8_t *)__tx_buf32;

    uint16_t total = __rt28xx_build_tx(wifi, tx_buf,
            (uint16_t)sizeof(__tx_buf32), frame, len);
    if (0 == total) return VSF_ERR_FAIL;

    if (wifi->reg_bus == NULL || wifi->reg_bus->data_tx == NULL) {
        return VSF_ERR_NOT_SUPPORT;
    }
    return wifi->reg_bus->data_tx(wifi, tx_buf, total);
}

/*============================ RAW RADIO BACKEND =============================*
 * Optional low-level 802.11-frame access for proprietary protocols.  Mutually
 * exclusive with the standard station-mode state machine.
 *===========================================================================*/

#if VSF_USE_WIFI == ENABLED

static void __rt28xx_radio_done_adapter(vsf_wifi_t *wifi, vsf_err_t err)
{
    vsf_wifi_radio_adapter_done(wifi, err);
}

static vsf_err_t __rt28xx_radio_tx(vsf_wifi_radio_t *radio,
        const uint8_t *frame, uint16_t len)
{
    return __rt28xx_tx(radio->wifi, frame, len);
}

static vsf_err_t __rt28xx_radio_set_channel(vsf_wifi_radio_t *radio, uint8_t ch,
        vsf_wifi_radio_done_t done)
{
    vsf_wifi_radio_adapter_done_set(radio, done);
    return __rt28xx_set_channel(radio->wifi, ch, __rt28xx_radio_done_adapter);
}

static vsf_err_t __rt28xx_radio_set_filter(vsf_wifi_radio_t *radio, uint32_t mask,
        vsf_wifi_radio_done_t done)
{
    vsf_wifi_radio_adapter_done_set(radio, done);
    return __rt28xx_set_rx_filter(radio->wifi, mask,
            __rt28xx_radio_done_adapter);
}

static vsf_err_t __rt28xx_radio_init(vsf_wifi_radio_t *radio,
        vsf_wifi_radio_done_t done)
{
    /* Same default filter used during a normal connect: accept beacons /
     * probe-resp, management, broadcast/multicast and unicast-to-me, drop
     * CRC/PHY/NOT_TO_ME errors.  The caller can override with set_filter(). */
    return __rt28xx_radio_set_filter(radio, 0x00017F97, done);
}

static vsf_err_t __rt28xx_radio_fini(vsf_wifi_radio_t *radio,
        vsf_wifi_radio_done_t done)
{
    /* Disable RX by dropping everything. */
    return __rt28xx_radio_set_filter(radio, 0, done);
}

static vsf_err_t __rt28xx_radio_set_ps(vsf_wifi_radio_t *radio, bool sleep,
        vsf_wifi_radio_done_t done)
{
    (void)radio; (void)sleep;
    if (done != NULL) {
        done(radio, VSF_ERR_NONE);
    }
    return VSF_ERR_NONE;
}

static const vsf_wifi_radio_ops_t __rt28xx_radio_ops = {
    .init        = __rt28xx_radio_init,
    .fini        = __rt28xx_radio_fini,
    .tx          = __rt28xx_radio_tx,
    .set_channel = __rt28xx_radio_set_channel,
    .set_filter  = __rt28xx_radio_set_filter,
    .set_ps      = __rt28xx_radio_set_ps,
};

#endif      /* VSF_USE_WIFI */

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
    .parse_rx      = __rt28xx_parse_rx,
    .build_tx      = __rt28xx_build_tx,
    .tx            = __rt28xx_tx,
#if VSF_WIFI_USE_WPA == ENABLED && VSF_WIFI_CFG_RT28XX_HW_CRYPTO == ENABLED
    .crypto_ops    = &__rt28xx_crypto_ops,
#endif
    .radio_ops     = &__rt28xx_radio_ops,
};

#endif      // VSF_USE_WIFI && VSF_WIFI_USE_RT28XX
