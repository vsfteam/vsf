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
 *   1. Firmware upload of rt2870.bin (8 KiB) — done.  The wifi layer ships
 *      the blob via vsf_wifi_run_blob; the application links a strong
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
#define RT28XX_EFUSE_DATA3              0x0590
#define RT28XX_EFUSE_DATA2              0x0594
#define RT28XX_EFUSE_DATA1              0x0598
#define RT28XX_EFUSE_DATA0              0x059C
#define RT28XX_EFUSE_KICK               (1u << 30)
#define RT28XX_EFUSE_PRESENT            (1u << 31)

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

#define RT28XX_RX_FILTER_CFG            0x1400
#define RT28XX_RX_PARSER_CFG            0x1404
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
#define RT28XX_BCN_TIME_CFG             0x1800
#define RT28XX_TBTT_TIMER               0x1804
#define RT28XX_INT_TIMER_CFG            0x1808
#define RT28XX_INT_TIMER_EN             0x180C
#define RT28XX_US_CYC_CNT               0x1854

// RX statistics counters (clear-on-read) - used for 0-RX diagnostics
#define RT28XX_RX_STA_CNT0              0x1700  // [15:0]CRC_ERR [31:16]PHY_ERR
#define RT28XX_RX_STA_CNT1              0x1704  // [15:0]FALSE_CCA [31:16]PLCP_ERR
// Channel-busy statistics (rt2800.h). CH_BUSY_STA counts the microseconds the
// PHY measured the medium busy during the last dwell -- a >0 value is the
// decisive proof the PHY/CCA engine is actually listening to RF energy even if
// false_cca stays 0; ==0 means the PHY front end never sees anything.
#define RT28XX_CH_IDLE_STA              0x1130
#define RT28XX_CH_BUSY_STA              0x1134

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
 * TX_PIN_CFG (2T2R): PA_PE_G0(0x2) | LNA_PE_A0(0x100) | LNA_PE_G0(0x200) |
 *             LNA_PE_A1(0x400) | LNA_PE_G1(0x800) | RFTR_EN(0x10000) |
 *             TRSW_EN(0x40000).  Both LNA chains plus the RF T/R switch enables
 *             are REQUIRED for the receiver to hear anything; the old RT30xx
 *             0x00000D0F left them clear.
 * TX_BAND_CFG: BG=1 (2.4 GHz), A=0, HT40_MINUS=0. */
#define RT28XX_TX_PIN_CFG_2G            0x00050F02
#define RT28XX_TX_BAND_CFG_2G           0x00000004

/*============================ OP MACROS =====================================*
 *
 *   RT_OP_REG  — direct 32-bit MAC register write
 *   RT_OP_BBP  — indirect BBP register write   (via BBP_CSR_CFG)
 *   RT_OP_RF   — indirect RF register write    (via RF_CSR_CFG)
 *==========================================================================*/

#define RT_OP_REG(r_, v_)   { (uint16_t)(r_), (uint32_t)(v_) }

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

/* Scratch big enough for the whole init sequence (~270 ops incl. the 84-entry
 * GLRT table) and for a single channel switch (~75 ops).  init / set_channel /
 * connect never run concurrently (the executor is single-flight), so they can
 * share one static buffer. */
static vsf_wifi_op_t __rt28xx_ops_buf[384];

static int __emit_rf(vsf_wifi_op_t *ops, int n, uint8_t reg, uint8_t val)
{
    __rt28xx_rf_shadow[reg] = val;
    ops[n] = (vsf_wifi_op_t)RT_OP_RF(reg, val);
    return n + 1;
}

static int __emit_rf_rmw(vsf_wifi_op_t *ops, int n, uint8_t reg,
        uint8_t mask, uint8_t val)
{
    uint8_t v = (uint8_t)((__rt28xx_rf_shadow[reg] & ~mask) | (val & mask));
    return __emit_rf(ops, n, reg, v);
}

static int __emit_bbp(vsf_wifi_op_t *ops, int n, uint8_t reg, uint8_t val)
{
    __rt28xx_bbp_shadow[reg] = val;
    ops[n] = (vsf_wifi_op_t)RT_OP_BBP(reg, val);
    return n + 1;
}

static int __emit_bbp_rmw(vsf_wifi_op_t *ops, int n, uint8_t reg,
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

static int __rt28xx_build_bbp(vsf_wifi_op_t *ops, int n)
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
    return n;
}

/*=========================== RT5592 RFCSR INIT ==============================*
 * Port of rt2800_init_rfcsr_5592() incl. rf_init_calibration(30) and
 * normal_mode_setup_5xxx().  freq_cal_mode1() is skipped (USB MCU command,
 * frequency offset defaulted to 0).  Pre-RT5592C revision assumed.
 *==========================================================================*/
static int __rt28xx_build_rfcsr(vsf_wifi_op_t *ops, int n)
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
static int __rt28xx_build_init(vsf_wifi_op_t *ops)
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
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_USB_DMA_CFG,   0x00C00080);
    /* PBF_CFG (USB): rt2800_init_registers writes 0xf40006 -- enables the
     * packet-buffer FIFO queues that feed the MAC->USB RX bulk path. */
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_PBF_CFG,       0x00F40006);
    /* WPDMA_GLO_CFG init value (rt2800_init_registers USB branch:6175): DMA
     * disabled, WP_DMA_BURST_SIZE=3 (0x30).  enable_radio later turns the DMA
     * engines on (see tail below). */
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_WPDMA_GLO_CFG, 0x00000030);
    /* ---- MAC defaults (TX_PIN/TX_BAND are programmed per channel) ---- */
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_MAC_MAX_LEN_CFG,     0x0FFF0FFF);
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_LED_CFG,             0x7F031E46);
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_AMPDU_MAX_LEN_20M1S, 0x0000A8FF);
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_AMPDU_MAX_LEN_40M1S, 0x000108EB);
    /* RT5592 TX_SW_CFG (rt2800lib.c:5995): CFG0=0x404, CFG1=0, CFG2=0. */
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_TX_SW_CFG0,          0x00000404);
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_TX_SW_CFG1,          0x00000000);
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_TX_SW_CFG2,          0x00000000);
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_RX_PARSER_CFG,       0x00000000);
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_BCN_TIME_CFG,        0x00006400);
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_TBTT_TIMER,          0x00000020);
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_INT_TIMER_CFG,       0x00000000);
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_INT_TIMER_EN,        0x00000000);
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_US_CYC_CNT,          0x0000001E);
    /* CH_TIME_CFG (rt2800_init_registers:6366): enable the channel-statistics
     * timer (EIFS/NAV/RX/TX busy sources + TMR_EN = 0x1F).  Without it the
     * CH_IDLE_STA/CH_BUSY_STA counters never run -- this was a real omission. */
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_CH_TIME_CFG,        0x0000001F);
    /* ---- RX_FILTER_CFG: drop CRC/PHY errors, keep beacons (bit5/6 clear) -- */
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_RX_FILTER_CFG,       0x00017F93);
    /* ---- XIFS_TIME_CFG: SIFS=16/16, OFDM_XIFS=4, EIFS=314, and crucially
     * BB_RXEND_ENABLE(bit29)=1.  Without BB_RXEND_ENABLE the BBP never raises
     * RXEND to the MAC -> false_cca counter stuck at 0 and no RX frame is
     * ever DMA'd up (zero URB completions).  (rt2800_init_registers:6235) */
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_XIFS_TIME_CFG,      0x33A41010);
    /* ---- PWR_PIN_CFG=0x3 (rt2800_init_registers:6237): RF power pins. ---- */
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_PWR_PIN_CFG,        0x00000003);
    /* ---- MCU_BOOT_SIGNAL: "Send signal during boot time to initialize
     * firmware" (rt2800_enable_radio:10810-10815).  This MUST precede
     * init_bbp/init_rfcsr -- without it the RF subsystem never fully wakes
     * (RFCSR1 RX0/TX0_PD refuse to latch, false_cca stays 0).  mcu_request()
     * encodes the command as: H2M_MAILBOX_CSR(OWNER=1,token/args=0) then
     * HOST_CMD_CSR(HOST_COMMAND=0x72).  The ep0 round-trips cover msleep(1). */
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_H2M_BBP_AGENT,   0x00000000);
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_H2M_MAILBOX_CSR, 0x00000000);
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_H2M_INT_SRC,     0x00000000);
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_H2M_MAILBOX_CSR, RT28XX_H2M_MAILBOX_OWNER);
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_HOST_CMD_CSR,    RT28XX_MCU_BOOT_SIGNAL);
    /* ---- BBP + RF (RT5592) ---- */
    n = __rt28xx_build_bbp(ops, n);
    n = __rt28xx_build_rfcsr(ops, n);
    /* ---- enable radio (mirror rt2800_enable_radio:10841): MAC TX-only ->
     * WPDMA TX_DMA|RX_DMA|TX_WRITEBACK (0x30 burst | 0x01 | 0x04 | 0x40 =
     * 0x75) -> MAC TX|RX.  The natural ep0 round-trip latency covers the
     * udelay(50) between the WPDMA enable and the RX enable. ---- */
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_MAC_SYS_CTRL, RT28XX_MAC_TX_EN);
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_WPDMA_GLO_CFG, 0x00000075);
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_MAC_SYS_CTRL, RT28XX_MAC_TX_EN | RT28XX_MAC_RX_EN);
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
static const vsf_wifi_op_t __rt28xx_pre_fw_script[] = {
    RT_OP_REG(RT28XX_H2M_MAILBOX_CID,    0xFFFFFFFF),
    RT_OP_REG(RT28XX_H2M_MAILBOX_STATUS, 0xFFFFFFFF),
};
static const vsf_wifi_op_t __rt28xx_post_fw_script[] = {
    RT_OP_REG(RT28XX_H2M_MAILBOX_CSR,  0x00000000),
};

/*============================ CHANNEL TABLE =================================*
 * RF5592 frequency-synthesiser values {N, K, mod, R}, selected by the on-chip
 * crystal (20 vs 40 MHz, see __rt28xx_xtal40).  Only 2.4 GHz (ch 1-14) is
 * populated -- that is all `wifi scan` needs.  Verbatim from
 * rf_vals_5592_xtal20 / rf_vals_5592_xtal40 (rt2800lib.c).
 *==========================================================================*/

typedef struct {
    uint16_t n;
    uint8_t  k;
    uint8_t  mod;
    uint8_t  r;
} rt28xx_rf_channel_t;

static const rt28xx_rf_channel_t __rf_vals_5592_xtal20[14] = {
    {482, 4, 10, 3}, {483, 4, 10, 3}, {484, 4, 10, 3}, {485, 4, 10, 3},
    {486, 4, 10, 3}, {487, 4, 10, 3}, {488, 4, 10, 3}, {489, 4, 10, 3},
    {490, 4, 10, 3}, {491, 4, 10, 3}, {492, 4, 10, 3}, {493, 4, 10, 3},
    {494, 4, 10, 3}, {496, 8, 10, 3},
};

static const rt28xx_rf_channel_t __rf_vals_5592_xtal40[14] = {
    {241, 2, 10, 3}, {241, 7, 10, 3}, {242, 2, 10, 3}, {242, 7, 10, 3},
    {243, 2, 10, 3}, {243, 7, 10, 3}, {244, 2, 10, 3}, {244, 7, 10, 3},
    {245, 2, 10, 3}, {245, 7, 10, 3}, {246, 2, 10, 3}, {246, 7, 10, 3},
    {247, 2, 10, 3}, {248, 4, 10, 3},
};

/* Set from MAC_DEBUG_INDEX bit31 during bring-up (1 = 40 MHz crystal). */
static uint8_t __rt28xx_xtal40;

/*============================ HELPERS =======================================*/

/* Port of rt2800_config_channel_rf55xx() 2.4 GHz branch + the RF5592 common
 * tail of rt2800_config_channel().  Single RX/TX chain, antenna 0, lna_gain 0,
 * 20 MHz OFDM (not 11b), TX power clamped to POWER_BOUND.  freq_cal_mode1 and
 * iq_calibrate are skipped (MCU command / EEPROM dependent).  BBP3/BBP4 are
 * left untouched: HT40_MINUS / BANDWIDTH are already 0 from init. */
static int __rt28xx_emit_channel(vsf_wifi_op_t *ops, int n, uint8_t channel)
{
    const rt28xx_rf_channel_t *rf;
    uint8_t rfcsr9, rf2359;

    if (channel < 1)  { channel = 1; }
    if (channel > 14) { channel = 14; }
    rf = __rt28xx_xtal40 ? &__rf_vals_5592_xtal40[channel - 1]
                         : &__rf_vals_5592_xtal20[channel - 1];

    /* N / K / mod / R -> RFCSR8/9/11 */
    n = __emit_rf(ops, n, 8, (uint8_t)(rf->n & 0xFF));
    rfcsr9 = (uint8_t)(rf->k & RFCSR9_K);
    if (rf->n & 0x100)               { rfcsr9 |= RFCSR9_N; }
    if (((rf->mod - 8) & 0x04) >> 2) { rfcsr9 |= RFCSR9_MOD; }
    n = __emit_rf_rmw(ops, n, 9, RFCSR9_K | RFCSR9_N | RFCSR9_MOD, rfcsr9);
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

    /* TX power (no EEPROM cal -> clamp to POWER_BOUND) */
    n = __emit_rf_rmw(ops, n, 49, RFCSR49_TX, RT28XX_POWER_BOUND);
    n = __emit_rf_rmw(ops, n, 50, RFCSR50_TX, RT28XX_POWER_BOUND);

    /* RF block enable: 2T2R -> RF_BLOCK_EN|PLL_PD|TX0_PD|RX0_PD|TX1_PD|RX1_PD */
    n = __emit_rf(ops, n, 1, 0x3F);
    n = __emit_rf(ops, n, 6, 0xE4);
    n = __emit_rf(ops, n, 30, 0x10);   /* not HT40 */
    n = __emit_rf(ops, n, 31, 0x80);
    n = __emit_rf(ops, n, 32, 0x80);
    /* freq_cal_mode1 skipped */
    n = __emit_rf_rmw(ops, n, 3, RFCSR3_VCOCAL_EN, RFCSR3_VCOCAL_EN);

    /* BBP front-end (lna_gain = 0) */
    n = __emit_bbp(ops, n, 62, 0x37);
    n = __emit_bbp(ops, n, 63, 0x37);
    n = __emit_bbp(ops, n, 64, 0x37);
    n = __emit_bbp(ops, n, 79, 0x1C);
    n = __emit_bbp(ops, n, 80, 0x0E);
    n = __emit_bbp(ops, n, 81, 0x3A);
    n = __emit_bbp(ops, n, 82, 0x62);
    /* per-channel GLRT band config */
    n = __emit_bbp(ops, n, 195, 128); n = __emit_bbp(ops, n, 196, 0xE0);
    n = __emit_bbp(ops, n, 195, 129); n = __emit_bbp(ops, n, 196, 0x1F);
    n = __emit_bbp(ops, n, 195, 130); n = __emit_bbp(ops, n, 196, 0x38);
    n = __emit_bbp(ops, n, 195, 131); n = __emit_bbp(ops, n, 196, 0x32);
    n = __emit_bbp(ops, n, 195, 133); n = __emit_bbp(ops, n, 196, 0x28);
    n = __emit_bbp(ops, n, 195, 124); n = __emit_bbp(ops, n, 196, 0x19);

    /* rt2800_config_channel() common tail (RF5592, 2.4 GHz, no external LNA) */
    n = __emit_bbp(ops, n, 62, 0x37);
    n = __emit_bbp(ops, n, 63, 0x37);
    n = __emit_bbp(ops, n, 64, 0x37);
    n = __emit_bbp(ops, n, 86, 0x00);
    n = __emit_bbp(ops, n, 82, 0x84);   /* overrides 0x62 above */
    n = __emit_bbp(ops, n, 75, 0x50);

    /* Band + RX-path pin enables.  LNA_PE/RFTR/TRSW MUST be set or the
     * receiver hears nothing (the old RT30xx 0x00000D0F left them clear). */
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_TX_BAND_CFG, RT28XX_TX_BAND_CFG_2G);
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_TX_PIN_CFG,  RT28XX_TX_PIN_CFG_2G);

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
    /* iq_calibrate skipped (needs EEPROM) */
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

static void __rt28xx_parse_rx(vsf_wifi_t *wifi, uint8_t *frame, uint16_t len)
{
    if (len < (RT28XX_RXINFO_DESC_SIZE + RT28XX_RXWI_DESC_SIZE_5572
             + RT28XX_DOT11_HDR_MIN  + RT28XX_RXD_DESC_SIZE)) {
        return;
    }

    uint32_t rxinfo_w0 = get_unaligned_le32(frame);
    uint16_t rx_pkt_len = (uint16_t)(rxinfo_w0 & 0xFFFF);
    if (rx_pkt_len < RT28XX_RXWI_DESC_SIZE_5572 + RT28XX_DOT11_HDR_MIN) return;
    if ((uint32_t)rx_pkt_len + RT28XX_RXINFO_DESC_SIZE > len) return;

    /* Drop FCS-failed frames using the RXD trailer at frame+RXINFO+rx_pkt_len
     * (ref rt2800usb_fill_rxdone, rt2800usb.c:520-528).  Without this, CRC-
     * corrupted beacons pass the loose mgmt filter and surface as bogus APs
     * (e.g. BSSID FF:FF:..). */
    if ((uint32_t)RT28XX_RXINFO_DESC_SIZE + rx_pkt_len + RT28XX_RXD_DESC_SIZE <= len) {
        uint32_t rxd_w0 = get_unaligned_le32(frame
                + RT28XX_RXINFO_DESC_SIZE + rx_pkt_len);
        if (rxd_w0 & RT28XX_RXD_W0_CRC_ERROR) return;
    }

    uint8_t *rxwi = frame + RT28XX_RXINFO_DESC_SIZE;
    uint32_t rxwi_w0 = get_unaligned_le32(rxwi + 0);
    uint32_t rxwi_w2 = get_unaligned_le32(rxwi + 8);

    uint16_t mpdu_len = (uint16_t)((rxwi_w0 >> 16) & 0xFFFu);
    if (mpdu_len < RT28XX_DOT11_HDR_MIN) return;
    if ((uint32_t)RT28XX_RXINFO_DESC_SIZE + RT28XX_RXWI_DESC_SIZE_5572 + mpdu_len > len) return;

    const uint8_t *hdr = rxwi + RT28XX_RXWI_DESC_SIZE_5572;

    /* 802.11 frame control: type/subtype encoded in the low byte. */
    uint16_t fc      = (uint16_t)hdr[0] | ((uint16_t)hdr[1] << 8);
    uint8_t  type    = (uint8_t)((fc >> 2) & 0x3);   /* 0 = mgmt */
    uint8_t  subtype = (uint8_t)((fc >> 4) & 0xF);   /* 8 = beacon, 5 = probe-resp */
    if (type != 0) return;
    if (subtype != 8 && subtype != 5) return;

    /* Mgmt frame fixed header: FC(2) Dur(2) DA(6) SA(6) BSSID(6) SeqCtrl(2) = 24B.
     * Beacon / probe-resp body opens with: Timestamp(8) Interval(2) Capability(2). */
    if (mpdu_len < RT28XX_DOT11_HDR_MIN + RT28XX_BEACON_FIXED) return;
    const uint8_t *bssid    = hdr + 16;
    const uint8_t *body     = hdr + RT28XX_DOT11_HDR_MIN;
    uint16_t       body_len = (uint16_t)(mpdu_len - RT28XX_DOT11_HDR_MIN);
    uint16_t       capa     = (uint16_t)body[10] | ((uint16_t)body[11] << 8);

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
        }
        ie += 2 + l;
    }

    vsf_wifi_on_scan_result(wifi, &result);
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

/* Crystal-select probe: MAC_DEBUG_INDEX bit31 chooses xtal20 vs xtal40 for
 * the RF5592 channel table.  Read once during bring-up (before any
 * set_channel / connect builds a channel script). */
static uint32_t __rt28xx_macdbg_val;

static void __rt28xx_xtal_select_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE == err) {
        __rt28xx_xtal40 = (__rt28xx_macdbg_val & RT28XX_MAC_DEBUG_INDEX_XTAL) ? 1 : 0;
        vsf_trace_info("rt28xx: MAC_DEBUG_INDEX=0x%08X xtal=%uMHz ASIC=0x%08X rev=0x%04X %s" VSF_TRACE_CFG_LINEEND,
                (unsigned)__rt28xx_macdbg_val, __rt28xx_xtal40 ? 40u : 20u,
                (unsigned)__rt28xx_asic_ver, (unsigned)(__rt28xx_asic_ver & 0xFFFF),
                __rt28xx_is_5592c() ? "(RT5592C+)" : "(pre-RT5592C)");
    } else {
        vsf_trace_warning("rt28xx: MAC_DEBUG_INDEX read err=%d, default 20MHz xtal" VSF_TRACE_CFG_LINEEND, (int)err);
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
    vsf_err_t e = vsf_wifi_run_read(wifi, RT28XX_MAC_DEBUG_INDEX,
            &__rt28xx_macdbg_val, __rt28xx_xtal_select_done);
    if (VSF_ERR_NONE != e) {
        vsf_trace_warning("rt28xx: MAC_DEBUG_INDEX submit err=%d" VSF_TRACE_CFG_LINEEND, (int)e);
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
} __rt28xx_efuse_ctx;

static bool __rt28xx_efuse_kick_clear(uint32_t val)
{
    return !(val & RT28XX_EFUSE_KICK);
}

static void __rt28xx_eeprom_read_start(vsf_wifi_t *wifi)
{
    memset(&__rt28xx_efuse_ctx, 0, sizeof(__rt28xx_efuse_ctx));
    vsf_err_t err = vsf_wifi_run_read(wifi, RT28XX_EFUSE_CTRL,
            &__rt28xx_efuse_ctx.detect_val, __rt28xx_eeprom_after_detect);
    if (VSF_ERR_NONE != err) {
        vsf_trace_warning("rt28xx: efuse detect submit err=%d" VSF_TRACE_CFG_LINEEND, (int)err);
        __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
    }
}

static void __rt28xx_eeprom_after_detect(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) {
        vsf_trace_warning("rt28xx: efuse detect read err=%d" VSF_TRACE_CFG_LINEEND, (int)err);
        __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
        return;
    }
    vsf_trace_info("rt28xx: EFUSE_CTRL=0x%08X (PRESENT=%d)" VSF_TRACE_CFG_LINEEND,
            (unsigned)__rt28xx_efuse_ctx.detect_val,
            (__rt28xx_efuse_ctx.detect_val & RT28XX_EFUSE_PRESENT) ? 1 : 0);
    if (!(__rt28xx_efuse_ctx.detect_val & RT28XX_EFUSE_PRESENT)) {
        /* Off-chip EEPROM path (vendor 0x09 / EEPROM bus access) is not
         * yet wired through the bus_ops layer; bail out gracefully. */
        vsf_trace_warning("rt28xx: external EEPROM mode (no eFuse), MAC read skipped" VSF_TRACE_CFG_LINEEND);
        __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
        return;
    }
    /* KICK row 0, MODE=0 (read), AIN=0 (row index). */
    vsf_wifi_op_t *ops = vsf_wifi_get_scratch_ops(wifi);
    ops[0] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_EFUSE_CTRL,
            RT28XX_EFUSE_KICK | (0u << 16) | (0u << 12));
    vsf_err_t e = vsf_wifi_run_script(wifi, ops, 1, __rt28xx_eeprom_after_kick);
    if (VSF_ERR_NONE != e) {
        vsf_trace_warning("rt28xx: efuse kick submit err=%d" VSF_TRACE_CFG_LINEEND, (int)e);
        __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
    }
}

static void __rt28xx_eeprom_after_kick(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) { __rt28xx_chain_finish(wifi, VSF_ERR_NONE); return; }
    vsf_err_t e = vsf_wifi_run_read_poll(wifi, RT28XX_EFUSE_CTRL,
            __rt28xx_efuse_kick_clear, /* max_retry */ 100, /* interval_ms */ 1,
            __rt28xx_eeprom_after_kick_poll);
    if (VSF_ERR_NONE != e) {
        vsf_trace_warning("rt28xx: efuse kick poll submit err=%d" VSF_TRACE_CFG_LINEEND, (int)e);
        __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
    }
}

static void __rt28xx_eeprom_after_kick_poll(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) {
        vsf_trace_warning("rt28xx: efuse kick busy timeout" VSF_TRACE_CFG_LINEEND);
        __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
        return;
    }
    vsf_err_t e = vsf_wifi_run_read(wifi, RT28XX_EFUSE_DATA3,
            &__rt28xx_efuse_ctx.data3, __rt28xx_eeprom_after_data3);
    if (VSF_ERR_NONE != e) __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
}

static void __rt28xx_eeprom_after_data3(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) { __rt28xx_chain_finish(wifi, VSF_ERR_NONE); return; }
    vsf_err_t e = vsf_wifi_run_read(wifi, RT28XX_EFUSE_DATA2,
            &__rt28xx_efuse_ctx.data2, __rt28xx_eeprom_after_data2);
    if (VSF_ERR_NONE != e) __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
}

static void __rt28xx_eeprom_after_data2(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) { __rt28xx_chain_finish(wifi, VSF_ERR_NONE); return; }
    vsf_err_t e = vsf_wifi_run_read(wifi, RT28XX_EFUSE_DATA1,
            &__rt28xx_efuse_ctx.data1, __rt28xx_eeprom_after_data1);
    if (VSF_ERR_NONE != e) __rt28xx_chain_finish(wifi, VSF_ERR_NONE);
}

static void __rt28xx_eeprom_after_data1(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) { __rt28xx_chain_finish(wifi, VSF_ERR_NONE); return; }
    vsf_err_t e = vsf_wifi_run_read(wifi, RT28XX_EFUSE_DATA0,
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
    vsf_trace_info(
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
        vsf_trace_warning(
                "rt28xx: EEPROM MAC invalid %02X:%02X:%02X:%02X:%02X:%02X (eFuse %s)" VSF_TRACE_CFG_LINEEND,
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
                all_zero ? "all-zero" : all_ff ? "all-FF" : "multicast bit set");
    } else {
        memcpy(wifi->mac, mac, 6);
        vsf_trace_info(
                "rt28xx: MAC %02X:%02X:%02X:%02X:%02X:%02X" VSF_TRACE_CFG_LINEEND,
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
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
        vsf_trace_warning("rt28xx: MCU firmware NOT ready (PBF_SYS_CTRL_READY=0), err=%d" VSF_TRACE_CFG_LINEEND, (int)err);
    } else {
        vsf_trace_info("rt28xx: MCU firmware running (PBF_SYS_CTRL_READY=1)" VSF_TRACE_CFG_LINEEND);
    }
    vsf_err_t e = vsf_wifi_run_read_poll(wifi, RT28XX_ASIC_VER_ID,
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
    vsf_err_t e = vsf_wifi_run_read_poll(wifi, RT28XX_PBF_SYS_CTRL,
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
    err = vsf_wifi_run_script(wifi, __rt28xx_pre_fw_script,
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
     * dead, so no RF/PHY calibration runs and the receiver stays silent. */
    err = vsf_wifi_run_vendor(wifi, RT28XX_USB_DEVICE_MODE,
            RT28XX_USB_MODE_FIRMWARE, 0, __rt28xx_fw_kick_done);
    vsf_trace_info("rt28xx: USB_MODE_FIRMWARE vendor req submit err=%d" VSF_TRACE_CFG_LINEEND, err);
    if (VSF_ERR_NONE != err) {
        /* Bus has no vendor_request primitive: fall back to the old path so
         * non-USB buses still work. */
        __rt28xx_fw_kick_done(wifi, VSF_ERR_NONE);
    }
}

static void __rt28xx_fw_kick_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    vsf_trace_info("rt28xx: USB_MODE_FIRMWARE vendor req done err=%d" VSF_TRACE_CFG_LINEEND, err);
    if (VSF_ERR_NONE != err) {
        __rt28xx_chain_finish(wifi, err);
        return;
    }
    err = vsf_wifi_run_script(wifi, __rt28xx_post_fw_script,
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
    vsf_wifi_blob_t blob = {
        .data       = __rt2870_firmware_data + 4096, /* RT5592: 2nd 4 KiB image */
        .len        = 4096,
        .base_reg   = RT28XX_FW_FIRMWARE_BASE,  /* 0x3000 */
        .chunk_size = 0,                        /* hint; bus picks default */
    };
    vsf_err_t err = vsf_wifi_run_blob(wifi, &blob, __rt28xx_blob_done);
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
    vsf_err_t e = vsf_wifi_run_script(wifi, __rt28xx_ops_buf, (uint16_t)n,
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
    vsf_trace_info("rt28xx: RF_CSR_CFG readback=0x%08X (RFCSR6 data=0x%02X, "
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
    vsf_err_t e = vsf_wifi_run_read_poll(wifi, RT28XX_RF_CSR_CFG,
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
    __rt28xx_ops_buf[0] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_RF_CSR_CFG,
            RT28XX_RF_BUSY | (6u << 8));
    vsf_err_t e = vsf_wifi_run_script(wifi, __rt28xx_ops_buf, 1,
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
        vsf_trace_info("rt28xx: BBP0=0x%02X ready after %u tries" VSF_TRACE_CFG_LINEEND,
                v, (unsigned)__rt28xx_bbp_wait_tries);
        __rt28xx_init_emit(wifi);
        return;
    }
    if (__rt28xx_bbp_wait_tries < 50) {
        __rt28xx_bbp_wait_tries++;
        __rt28xx_bbp_probe_kick(wifi);
        return;
    }
    vsf_trace_warning("rt28xx: BBP not ready (BBP0=0x%02X after %u tries), "
            "continuing" VSF_TRACE_CFG_LINEEND, v, (unsigned)__rt28xx_bbp_wait_tries);
    __rt28xx_init_emit(wifi);
}

/* Read-kick landed; read BBP_CSR_CFG back to capture BBP0's value. */
static void __rt28xx_bbp_kick_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) { __rt28xx_init_emit(wifi); return; }
    __rt28xx_bbp_probe = 0;
    vsf_err_t e = vsf_wifi_run_read(wifi, RT28XX_BBP_CSR_CFG,
            &__rt28xx_bbp_probe, __rt28xx_bbp_probe_done);
    if (VSF_ERR_NONE != e) { __rt28xx_init_emit(wifi); }
}

/* Issue an indirect read request for BBP register 0 (version/status). */
static void __rt28xx_bbp_probe_kick(vsf_wifi_t *wifi)
{
    __rt28xx_ops_buf[0] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_BBP_CSR_CFG,
            RT28XX_BBP_READ_KICK | (0u << 8));
    vsf_err_t e = vsf_wifi_run_script(wifi, __rt28xx_ops_buf, 1,
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
    (void)err;  /* a missing vendor primitive is non-fatal; still deassert */
    __rt28xx_ops_buf[0] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_MAC_SYS_CTRL, 0);
    __rt28xx_ops_buf[1] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_H2M_BBP_AGENT, 0);
    __rt28xx_ops_buf[2] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_H2M_MAILBOX_CSR, 0);
    vsf_err_t e = vsf_wifi_run_script(wifi, __rt28xx_ops_buf, 3,
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
    vsf_err_t e = vsf_wifi_run_vendor(wifi, RT28XX_USB_DEVICE_MODE,
            RT28XX_USB_MODE_RESET, 0, __rt28xx_reset_clear);
    if (VSF_ERR_NONE != e) {
        /* Bus has no vendor primitive: skip the digital-core reset but still
         * complete the MAC/BBP reset deassert so bring-up proceeds. */
        __rt28xx_reset_clear(wifi, VSF_ERR_NONE);
    }
}

/* Stage 1: got the current PBF_SYS_CTRL value -> clear its bit13 via
 * read-modify-write (preserving READY etc.) and assert the MAC/BBP reset
 * (MAC_SYS_CTRL=RESET_CSR|RESET_BBP), mirroring ref rt2800usb_init_registers
 * lines 280-286. */
static void __rt28xx_init_pbf_read_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    uint32_t pbf = (VSF_ERR_NONE == err) ? __rt28xx_init_pbf : 0;
    __rt28xx_ops_buf[0] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_PBF_SYS_CTRL,
            pbf & ~RT28XX_PBF_SYS_CTRL_RESET13);
    __rt28xx_ops_buf[1] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_MAC_SYS_CTRL,
            RT28XX_MAC_SRST | RT28XX_BBP_HRST);
    vsf_err_t e = vsf_wifi_run_script(wifi, __rt28xx_ops_buf, 2,
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
    vsf_err_t e = vsf_wifi_run_read(wifi, RT28XX_PBF_SYS_CTRL,
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
    vsf_trace_info("rt28xx: after ch cfg BBP66=0x%02X (wrote 0x1C, pollerr=%d)"
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
    if (VSF_ERR_NONE != vsf_wifi_run_read(wifi, RT28XX_BBP_CSR_CFG,
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
    vsf_trace_info("rt28xx: after ch cfg RFCSR3=0x%02X (VCOCAL_EN=%u -> %s, "
            "pollerr=%d)" VSF_TRACE_CFG_LINEEND,
            (unsigned)(__rt28xx_rf_probe & 0xFF),
            (unsigned)((__rt28xx_rf_probe >> 7) & 1),
            ((__rt28xx_rf_probe >> 7) & 1) ? "NOT LOCKED" : "locked",
            (int)err);
    /* Chain a BBP66 read to verify the BBP write path. */
    __rt28xx_ops_buf[0] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_BBP_CSR_CFG,
            RT28XX_BBP_READ_KICK | (66u << 8));
    if (VSF_ERR_NONE != vsf_wifi_run_script(wifi, __rt28xx_ops_buf, 1,
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
    if (VSF_ERR_NONE != vsf_wifi_run_read_poll(wifi, RT28XX_RF_CSR_CFG,
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
    vsf_trace_info("rt28xx: after ch cfg RFCSR8=0x%02X (want N&0xff=0x%02X, "
            "pollerr=%d)" VSF_TRACE_CFG_LINEEND,
            (unsigned)(__rt28xx_rf_probe & 0xFF),
            (unsigned)(rf->n & 0xFF), (int)err);
    /* Chain RFCSR3 read (VCO lock status). */
    __rt28xx_ops_buf[0] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_RF_CSR_CFG,
            RT28XX_RF_BUSY | (3u << 8));
    if (VSF_ERR_NONE != vsf_wifi_run_script(wifi, __rt28xx_ops_buf, 1,
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
    if (VSF_ERR_NONE != vsf_wifi_run_read_poll(wifi, RT28XX_RF_CSR_CFG,
            __rt28xx_rf_busy_clear, 50, 1, __rt28xx_setch_rf8_done)) {
        if (__rt28xx_setch_done != NULL) __rt28xx_setch_done(wifi, VSF_ERR_NONE);
    }
}

/* Diagnostic: read RFCSR1 back after a channel is programmed to prove the
 * RX0/TX0 path-enable bits actually latch (init-time 0x3F read back as 0x03,
 * RX0/TX0 absent -- need to confirm the channel write of 0x0F sticks). */
static void __rt28xx_setch_rf1_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    vsf_trace_info("rt28xx: after ch cfg RFCSR1=0x%02X (want 0x0F, BUSY=%u, "
            "pollerr=%d)" VSF_TRACE_CFG_LINEEND,
            (unsigned)(__rt28xx_rf_probe & 0xFF),
            (unsigned)((__rt28xx_rf_probe >> 17) & 1), (int)err);
    /* Chain an RFCSR8 (channel N) read, then RFCSR3 (VCO lock), then BBP66. */
    __rt28xx_ops_buf[0] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_RF_CSR_CFG,
            RT28XX_RF_BUSY | (8u << 8));
    if (VSF_ERR_NONE != vsf_wifi_run_script(wifi, __rt28xx_ops_buf, 1,
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
    if (VSF_ERR_NONE != vsf_wifi_run_read_poll(wifi, RT28XX_RF_CSR_CFG,
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
    __rt28xx_ops_buf[0] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_RF_CSR_CFG,
            RT28XX_RF_BUSY | (1u << 8));
    if (VSF_ERR_NONE != vsf_wifi_run_script(wifi, __rt28xx_ops_buf, 1,
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
    uint32_t ldo_new   = __rt28xx_ldo & ~RT28XX_LDO_CFG0_VLEVEL_MASK; /* 2.4G VLEVEL=0 */
    int n;
    vsf_trace_info("rt28xx: RX_STA_CNT1=0x%08X false_cca=%u plcp_err=%u "
            "MAC_SYS_CTRL=0x%08X (TX_EN=%u RX_EN=%u) LDO_CFG0=0x%08X (VLEVEL=%u) "
            "CH_IDLE=%u CH_BUSY_STA=%u (ch->%u)"
            VSF_TRACE_CFG_LINEEND, __rt28xx_rxsta1, false_cca, plcp_err,
            __rt28xx_macctrl, (unsigned)((__rt28xx_macctrl >> 2) & 1),
            (unsigned)((__rt28xx_macctrl >> 3) & 1), __rt28xx_ldo,
            (unsigned)((__rt28xx_ldo >> 26) & 0x7), __rt28xx_chidle,
            __rt28xx_chbusy, __rt28xx_setch_channel);
    /* config_channel_rf55xx() first writes LDO_CFG0 with LDO_CORE_VLEVEL=0 for
     * 2.4 GHz (read-modify-write, preserving DELAY/BGSEL/LDO25 bits). */
    __rt28xx_ops_buf[0] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_LDO_CFG0, ldo_new);
    n = __rt28xx_emit_channel(__rt28xx_ops_buf, 1, __rt28xx_setch_channel);
    if (VSF_ERR_NONE != vsf_wifi_run_script(wifi, __rt28xx_ops_buf, (uint16_t)n,
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
    if (VSF_ERR_NONE != vsf_wifi_run_read(wifi, RT28XX_CH_BUSY_STA,
            &__rt28xx_chbusy, __rt28xx_setch_after_diag)) {
        __rt28xx_setch_after_diag(wifi, err);
    }
}

/* Chain: LDO_CFG0 captured -> read CH_IDLE_STA before CH_BUSY_STA. */
static void __rt28xx_setch_read_chidle(vsf_wifi_t *wifi, vsf_err_t err)
{
    __rt28xx_chidle = 0;
    if (VSF_ERR_NONE != vsf_wifi_run_read(wifi, RT28XX_CH_IDLE_STA,
            &__rt28xx_chidle, __rt28xx_setch_read_chbusy)) {
        __rt28xx_setch_read_chbusy(wifi, err);
    }
}

/* Chain: MAC_SYS_CTRL captured -> now read LDO_CFG0 before logging/program. */
static void __rt28xx_setch_read_ldo(vsf_wifi_t *wifi, vsf_err_t err)
{
    __rt28xx_ldo = 0;
    if (VSF_ERR_NONE != vsf_wifi_run_read(wifi, RT28XX_LDO_CFG0,
            &__rt28xx_ldo, __rt28xx_setch_read_chidle)) {
        __rt28xx_setch_read_chidle(wifi, err);
    }
}

/* Chain: RX_STA_CNT1 captured -> now read MAC_SYS_CTRL before logging. */
static void __rt28xx_setch_read_macctrl(vsf_wifi_t *wifi, vsf_err_t err)
{
    __rt28xx_macctrl = 0;
    if (VSF_ERR_NONE != vsf_wifi_run_read(wifi, RT28XX_MAC_SYS_CTRL,
            &__rt28xx_macctrl, __rt28xx_setch_read_ldo)) {
        __rt28xx_setch_read_ldo(wifi, err);
    }
}

static vsf_err_t __rt28xx_set_channel(vsf_wifi_t *wifi, uint8_t channel,
        vsf_wifi_done_t done)
{
    if (channel < 1 || channel > 14) return VSF_ERR_INVALID_PARAMETER;
    __rt28xx_setch_done    = done;
    __rt28xx_setch_channel = channel;
    __rt28xx_rxsta1        = 0;
    /* Read prior dwell's RX_STA_CNT1 first, then MAC_SYS_CTRL (diagnostic),
     * then set channel. */
    if (VSF_ERR_NONE != vsf_wifi_run_read(wifi, RT28XX_RX_STA_CNT1,
            &__rt28xx_rxsta1, __rt28xx_setch_read_macctrl)) {
        /* readback unavailable: fall back to plain channel program */
        int n = __rt28xx_emit_channel(__rt28xx_ops_buf, 0, channel);
        return vsf_wifi_run_script(wifi, __rt28xx_ops_buf, (uint16_t)n, done);
    }
    return VSF_ERR_NONE;
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

    /* bssid(2) + channel(~75) + filter(1) -> shared static buffer. */
    vsf_wifi_op_t *ops = __rt28xx_ops_buf;
    int n = 0;
    n = __rt28xx_emit_bssid  (ops, n, bssid);
    n = __rt28xx_emit_channel(ops, n, channel);
    ops[n++] = (vsf_wifi_op_t)RT_OP_REG(RT28XX_RX_FILTER_CFG,
            RT28XX_FILTER_DROP_CRC_ERROR | RT28XX_FILTER_DROP_PHY_ERROR);
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
    .parse_rx      = __rt28xx_parse_rx,
};

#endif      // VSF_USE_WIFI && VSF_WIFI_USE_RT28XX
