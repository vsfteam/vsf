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

/*============================ IMPLEMENTATION ================================*/

/* TX power calibration helpers, mirroring Linux mt76x2/mt76x02. */
typedef struct mt76_rate_power_t {
    union {
        struct {
            int8_t cck[4];
            int8_t ofdm[8];
            int8_t ht[16];
            int8_t vht[2];
        };
        int8_t all[30];
    };
} mt76_rate_power_t;

typedef struct mt76_tx_power_info_t {
    uint8_t target_power;
    int8_t  delta_bw40;
    int8_t  delta_bw80;
    struct {
        int8_t tssi_slope;
        int8_t tssi_offset;
        int8_t target_power;
        int8_t delta;
    } chain[2];
} mt76_tx_power_info_t;

static bool __mt76_field_valid(uint8_t val)
{
    return (val != 0) && (val != 0xff);
}

static int32_t __mt76_sign_extend(uint32_t val, uint8_t size)
{
    bool sign = (val & (1U << (size - 1))) != 0;
    val &= (1U << (size - 1)) - 1;
    return sign ? (int32_t)val : -(int32_t)val;
}

static int32_t __mt76_sign_extend_optional(uint32_t val, uint8_t size)
{
    bool enable = (val & (1U << size)) != 0;
    return enable ? __mt76_sign_extend(val, size) : 0;
}

static int8_t __mt76_rate_power_val(uint8_t val)
{
    if (!__mt76_field_valid(val)) {
        return 0;
    }
    return (int8_t)__mt76_sign_extend_optional(val, 7);
}

static uint8_t __mt76_5g_cal_group(uint8_t channel)
{
    if (channel >= 184 && channel <= 196) return 0; /* JAPAN */
    if (channel <= 48)  return 1; /* UNII_1 */
    if (channel <= 64)  return 2; /* UNII_2 */
    if (channel <= 114) return 3; /* UNII_2E_1 */
    if (channel <= 144) return 4; /* UNII_2E_2 */
    return 5; /* UNII_3 */
}

static void __mt76_get_rate_power(vsf_wifi_t *wifi, mt76_rate_power_t *t,
                                  uint8_t channel)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    bool is_5g = channel > 14;
    uint16_t val;

    memset(t, 0, sizeof(*t));

    val = __mt76_eeprom_get_u16(priv, MT76_EE_TX_POWER_CCK);
    t->cck[0] = t->cck[1] = __mt76_rate_power_val((uint8_t)val);
    t->cck[2] = t->cck[3] = __mt76_rate_power_val((uint8_t)(val >> 8));

    val = __mt76_eeprom_get_u16(priv, is_5g ? MT76_EE_TX_POWER_OFDM_5G_6M
                                              : MT76_EE_TX_POWER_OFDM_2G_6M);
    t->ofdm[0] = t->ofdm[1] = __mt76_rate_power_val((uint8_t)val);
    t->ofdm[2] = t->ofdm[3] = __mt76_rate_power_val((uint8_t)(val >> 8));

    val = __mt76_eeprom_get_u16(priv, is_5g ? MT76_EE_TX_POWER_OFDM_5G_24M
                                              : MT76_EE_TX_POWER_OFDM_2G_24M);
    t->ofdm[4] = t->ofdm[5] = __mt76_rate_power_val((uint8_t)val);
    t->ofdm[6] = t->ofdm[7] = __mt76_rate_power_val((uint8_t)(val >> 8));

    val = __mt76_eeprom_get_u16(priv, MT76_EE_TX_POWER_HT_MCS0);
    t->ht[0] = t->ht[1] = __mt76_rate_power_val((uint8_t)val);
    t->ht[2] = t->ht[3] = __mt76_rate_power_val((uint8_t)(val >> 8));

    val = __mt76_eeprom_get_u16(priv, MT76_EE_TX_POWER_HT_MCS4);
    t->ht[4] = t->ht[5] = __mt76_rate_power_val((uint8_t)val);
    t->ht[6] = t->ht[7] = __mt76_rate_power_val((uint8_t)(val >> 8));

    val = __mt76_eeprom_get_u16(priv, MT76_EE_TX_POWER_HT_MCS8);
    t->ht[8] = t->ht[9] = __mt76_rate_power_val((uint8_t)val);
    t->ht[10] = t->ht[11] = __mt76_rate_power_val((uint8_t)(val >> 8));

    val = __mt76_eeprom_get_u16(priv, MT76_EE_TX_POWER_HT_MCS12);
    t->ht[12] = t->ht[13] = __mt76_rate_power_val((uint8_t)val);
    t->ht[14] = t->ht[15] = __mt76_rate_power_val((uint8_t)(val >> 8));

    val = __mt76_eeprom_get_u16(priv, MT76_EE_TX_POWER_VHT_MCS8);
    if (!is_5g) {
        val >>= 8;
    }
    t->vht[0] = t->vht[1] = __mt76_rate_power_val((uint8_t)(val >> 8));
}

static void __mt76_get_power_info_2g(vsf_wifi_t *wifi,
        mt76_tx_power_info_t *t, uint8_t channel, uint8_t chain,
        uint16_t offset)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    uint8_t data[6];
    uint8_t delta_idx;
    uint16_t val;

    if (channel < 6)       delta_idx = 3;
    else if (channel < 11) delta_idx = 4;
    else                   delta_idx = 5;

    for (int i = 0; i < 6; i++) {
        data[i] = __mt76_eeprom_get_u8(priv, (uint16_t)(offset + i));
    }

    t->chain[chain].tssi_slope  = data[0];
    t->chain[chain].tssi_offset = data[1];
    t->chain[chain].target_power = data[2];
    t->chain[chain].delta = (int8_t)__mt76_sign_extend_optional(data[delta_idx], 7);

    val = __mt76_eeprom_get_u16(priv, MT76_EE_RF_2G_TSSI_OFF_TXPOWER);
    t->target_power = (uint8_t)(val >> 8);
}

static void __mt76_get_power_info_5g(vsf_wifi_t *wifi,
        mt76_tx_power_info_t *t, uint8_t channel, uint8_t chain,
        uint16_t offset)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    uint8_t data[5];
    uint8_t delta_idx;
    uint16_t val;
    uint8_t group = __mt76_5g_cal_group(channel);

    offset = (uint16_t)(offset + group * MT76_TX_POWER_GROUP_SIZE_5G);

    if (channel >= 192)       delta_idx = 4;
    else if (channel >= 184)  delta_idx = 3;
    else if (channel < 44)    delta_idx = 3;
    else if (channel < 52)    delta_idx = 4;
    else if (channel < 58)    delta_idx = 3;
    else if (channel < 98)    delta_idx = 4;
    else if (channel < 106)   delta_idx = 3;
    else if (channel < 116)   delta_idx = 4;
    else if (channel < 130)   delta_idx = 3;
    else if (channel < 149)   delta_idx = 4;
    else if (channel < 157)   delta_idx = 3;
    else                      delta_idx = 4;

    for (int i = 0; i < 5; i++) {
        data[i] = __mt76_eeprom_get_u8(priv, (uint16_t)(offset + i));
    }

    t->chain[chain].tssi_slope  = data[0];
    t->chain[chain].tssi_offset = data[1];
    t->chain[chain].target_power = data[2];
    t->chain[chain].delta = (int8_t)__mt76_sign_extend_optional(data[delta_idx], 7);

    val = __mt76_eeprom_get_u16(priv, MT76_EE_RF_5G_GRP0_1_RX_HIGH_GAIN);
    t->target_power = (uint8_t)(val & 0xff);
}

static bool __mt76_tssi_enabled(vsf_wifi_t *wifi)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    uint16_t ext_pa = __mt76_eeprom_get_u16(priv, MT76_EE_TX_POWER_EXT_PA_5G);
    uint16_t conf1  = __mt76_eeprom_get_u16(priv, MT76_EE_NIC_CONF_1);

    if ((ext_pa & MT76_EE_TX_POWER_EXT_PA_5G_EN) &&
            (conf1 & MT76_EE_NIC_CONF_1_TEMP_TX_ALC)) {
        return false;
    }
    return (conf1 & MT76_EE_NIC_CONF_1_TX_ALC_EN) != 0;
}

static void __mt76_get_power_info(vsf_wifi_t *wifi,
        mt76_tx_power_info_t *t, uint8_t channel)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    uint16_t bw40, bw80;

    memset(t, 0, sizeof(*t));

    bw40 = __mt76_eeprom_get_u16(priv, MT76_EE_TX_POWER_DELTA_BW40);
    bw80 = __mt76_eeprom_get_u16(priv, MT76_EE_TX_POWER_DELTA_BW80);

    if (channel > 14) {
        bw40 >>= 8;
        __mt76_get_power_info_5g(wifi, t, channel, 0, MT76_EE_TX_POWER_0_START_5G);
        __mt76_get_power_info_5g(wifi, t, channel, 1, MT76_EE_TX_POWER_1_START_5G);
    } else {
        __mt76_get_power_info_2g(wifi, t, channel, 0, MT76_EE_TX_POWER_0_START_2G);
        __mt76_get_power_info_2g(wifi, t, channel, 1, MT76_EE_TX_POWER_1_START_2G);
    }

    if (__mt76_tssi_enabled(wifi) || !__mt76_field_valid(t->target_power)) {
        t->target_power = t->chain[0].target_power;
    }

    t->delta_bw40 = __mt76_rate_power_val((uint8_t)bw40);
    t->delta_bw80 = __mt76_rate_power_val((uint8_t)bw80);
}

static bool __mt76_ext_pa_enabled(vsf_wifi_t *wifi, uint8_t channel)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    uint16_t conf0 = __mt76_eeprom_get_u16(priv, MT76_EE_NIC_CONF_0);
    if (channel > 14) {
        return (conf0 & MT76_EE_NIC_CONF_0_PA_INT_5G) == 0;
    }
    return (conf0 & MT76_EE_NIC_CONF_0_PA_INT_2G) == 0;
}

/* Map a 5G channel to its RX high-gain EEPROM group, mirroring Linux
 * mt76x2_get_cal_channel_group(). */
static uint8_t __mt76_5g_rx_gain_group(uint8_t channel)
{
    if (channel >= 184 && channel <= 196) return 0; /* MT_CH_5G_JAPAN    */
    if (channel <= 48)                    return 1; /* MT_CH_5G_UNII_1   */
    if (channel <= 64)                    return 2; /* MT_CH_5G_UNII_2   */
    if (channel <= 114)                   return 3; /* MT_CH_5G_UNII_2E_1*/
    if (channel <= 144)                   return 4; /* MT_CH_5G_UNII_2E_2*/
    return 5;                                         /* MT_CH_5G_UNII_3   */
}

static uint8_t __mt76_get_5g_rx_high_gain(mt76_wifi_priv_t *priv, uint8_t channel)
{
    uint8_t group = __mt76_5g_rx_gain_group(channel);
    uint16_t val;

    switch (group) {
    case 0:
        val = __mt76_eeprom_get_u16(priv, MT76_EE_RF_5G_GRP0_1_RX_HIGH_GAIN);
        return (uint8_t)(val & 0xff);
    case 1:
        val = __mt76_eeprom_get_u16(priv, MT76_EE_RF_5G_GRP0_1_RX_HIGH_GAIN);
        return (uint8_t)(val >> 8);
    case 2:
        val = __mt76_eeprom_get_u16(priv, MT76_EE_RF_5G_GRP2_3_RX_HIGH_GAIN);
        return (uint8_t)(val & 0xff);
    case 3:
        val = __mt76_eeprom_get_u16(priv, MT76_EE_RF_5G_GRP2_3_RX_HIGH_GAIN);
        return (uint8_t)(val >> 8);
    case 4:
        val = __mt76_eeprom_get_u16(priv, MT76_EE_RF_5G_GRP4_5_RX_HIGH_GAIN);
        return (uint8_t)(val & 0xff);
    default:
        val = __mt76_eeprom_get_u16(priv, MT76_EE_RF_5G_GRP4_5_RX_HIGH_GAIN);
        return (uint8_t)(val >> 8);
    }
}

/* Read RX path gain from EEPROM and populate priv->rx_mcu_gain / rx_lna_gain,
 * matching Linux mt76x2_read_rx_gain().  This must be called before
 * CMD_INIT_GAIN_OP on each channel switch. */
void __mt76_read_rx_gain(vsf_wifi_t *wifi, uint8_t channel)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    bool is_5g = channel > 14;
    uint16_t val;
    int8_t lna_2g;
    int8_t lna_5g[3];
    uint8_t lna;
    uint8_t high_gain;

    if (is_5g) {
        high_gain = __mt76_get_5g_rx_high_gain(priv, channel);
    } else {
        val = __mt76_eeprom_get_u16(priv, MT76_EE_RF_2G_RX_HIGH_GAIN);
        high_gain = (uint8_t)(val >> 8);
    }

    /* high_gain is two 4-bit signed nibbles: [chain1][chain0]. */
    if (!__mt76_field_valid(high_gain)) {
        priv->rx_high_gain[0] = 0;
        priv->rx_high_gain[1] = 0;
    } else {
        priv->rx_high_gain[0] = (int8_t)__mt76_sign_extend(high_gain & 0x0f, 4);
        priv->rx_high_gain[1] = (int8_t)__mt76_sign_extend((high_gain >> 4) & 0x0f, 4);
    }

    val = __mt76_eeprom_get_u16(priv, MT76_EE_LNA_GAIN);
    lna_2g = (int8_t)(val & 0xff);
    lna_5g[0] = (int8_t)(val >> 8);

    val = __mt76_eeprom_get_u16(priv, MT76_EE_RSSI_OFFSET_2G_1);
    lna_5g[1] = (int8_t)(val >> 8);

    val = __mt76_eeprom_get_u16(priv, MT76_EE_RSSI_OFFSET_5G_1);
    lna_5g[2] = (int8_t)(val >> 8);

    if (!__mt76_field_valid((uint8_t)lna_5g[1])) lna_5g[1] = lna_5g[0];
    if (!__mt76_field_valid((uint8_t)lna_5g[2])) lna_5g[2] = lna_5g[0];

    if (is_5g) {
        val = __mt76_eeprom_get_u16(priv, MT76_EE_RSSI_OFFSET_5G_0);
    } else {
        val = __mt76_eeprom_get_u16(priv, MT76_EE_RSSI_OFFSET_2G_0);
    }
    priv->rx_rssi_offset[0] = (int8_t)__mt76_sign_extend_optional(val & 0xff, 7);
    priv->rx_rssi_offset[1] = (int8_t)__mt76_sign_extend_optional((val >> 8) & 0xff, 7);

    priv->rx_mcu_gain = ((uint8_t)lna_2g & 0xff)
                      | (((uint8_t)lna_5g[0] & 0xff) << 8)
                      | (((uint8_t)lna_5g[1] & 0xff) << 16)
                      | (((uint8_t)lna_5g[2] & 0xff) << 24);

    val = __mt76_eeprom_get_u16(priv, MT76_EE_NIC_CONF_1);
    if (val & MT76_EE_NIC_CONF_1_LNA_EXT_2G) lna_2g = 0;
    if (val & MT76_EE_NIC_CONF_1_LNA_EXT_5G) {
        lna_5g[0] = 0; lna_5g[1] = 0; lna_5g[2] = 0;
    }

    if (!is_5g) {
        lna = (uint8_t)lna_2g;
    } else if (channel <= 64) {
        lna = (uint8_t)lna_5g[0];
    } else if (channel <= 128) {
        lna = (uint8_t)lna_5g[1];
    } else {
        lna = (uint8_t)lna_5g[2];
    }
    priv->rx_lna_gain = (lna != 0xff) ? (int8_t)__mt76_sign_extend(lna, 8) : 0;
    priv->rx_gain_read = true;

    vsf_wifi_chip_mt76_trace_info(
        "mt76: read_rx_gain ch=%u mcu_gain=0x%08X lna_gain=%d hg=[%d,%d]"
        VSF_TRACE_CFG_LINEEND,
        (unsigned)channel, (unsigned)priv->rx_mcu_gain,
        (int)priv->rx_lna_gain,
        (int)priv->rx_high_gain[0], (int)priv->rx_high_gain[1]);
}

static void __mt76_add_rate_power_offset(mt76_rate_power_t *r, int8_t offset)
{
    for (uint8_t i = 0; i < sizeof(r->all); i++) {
        r->all[i] += offset;
    }
}

static void __mt76_limit_rate_power(mt76_rate_power_t *r, int8_t limit)
{
    for (uint8_t i = 0; i < sizeof(r->all); i++) {
        if (r->all[i] > limit) {
            r->all[i] = limit;
        } else if (r->all[i] < 0) {
            r->all[i] = 0;
        }
    }
}

static int8_t __mt76_get_max_rate_power(mt76_rate_power_t *r)
{
    int8_t ret = 0;
    for (uint8_t i = 0; i < sizeof(r->all); i++) {
        if (r->all[i] > ret) {
            ret = r->all[i];
        }
    }
    return ret;
}

static int8_t __mt76_get_min_rate_power(mt76_rate_power_t *r)
{
    int8_t ret = 0;
    bool first = true;
    for (uint8_t i = 0; i < sizeof(r->all); i++) {
        if (r->all[i] == 0) continue;
        if (first || r->all[i] < ret) {
            ret = r->all[i];
            first = false;
        }
    }
    return ret;
}

static uint32_t __mt76_tx_power_mask(uint8_t v1, uint8_t v2,
                                     uint8_t v3, uint8_t v4)
{
    uint32_t val = 0;
    val |= ((uint32_t)(v1 & 0x3f)) << 0;
    val |= ((uint32_t)(v2 & 0x3f)) << 8;
    val |= ((uint32_t)(v3 & 0x3f)) << 16;
    val |= ((uint32_t)(v4 & 0x3f)) << 24;
    return val;
}

static int __mt76_set_txpower_regs(vsf_wifi_t *wifi, uint8_t channel,
                                   vsf_wifi_reg_op_t *ops, int n)
{
    bool is_5g = channel > 14;
    bool ext_pa = __mt76_ext_pa_enabled(wifi, channel);
    uint32_t pa_mode[2];
    uint32_t pa_mode_adj;
    uint32_t alc2, alc3, alc4;
    uint32_t tx0_gain, tx1_gain;

    if (!is_5g) {
        pa_mode[0] = 0x010055ff;
        pa_mode[1] = 0x00550055;
        alc2 = 0x35160a00;
        alc3 = 0x35160a06;
        if (ext_pa) {
            pa_mode_adj = 0x0000ec00;
            tx0_gain = tx1_gain = 0x3c3c023c;
            alc4 = 0x00001818;
        } else {
            pa_mode_adj = 0xf4000200;
            tx0_gain = tx1_gain = 0x0f3c3c3c;
            alc4 = 0x00000606;
        }
    } else {
        pa_mode[0] = 0x0000ffff;
        pa_mode[1] = 0x00ff00ff;
        if (ext_pa) {
            alc2 = 0x2f0f0400;
            alc3 = 0x2f0f0476;
            pa_mode_adj = 0x04000000;
            tx0_gain = tx1_gain = 0x363c023c;
            alc4 = 0x00001818;
        } else {
            alc2 = 0x1b0f0400;
            alc3 = 0x1b0f0476;
            pa_mode_adj = 0;
            tx0_gain = 0x383c023c;
            tx1_gain = 0x24282e28;
            alc4 = 0;
        }
    }

    ops[n++] = (vsf_wifi_reg_op_t){ MT76_TX_ALC_CFG_2, alc2 };
    ops[n++] = (vsf_wifi_reg_op_t){ MT76_TX_ALC_CFG_3, alc3 };
    ops[n++] = (vsf_wifi_reg_op_t){ MT76_TX_ALC_CFG_4, alc4 };
    ops[n++] = (vsf_wifi_reg_op_t){ MT76_RF_PA_MODE_ADJ0, pa_mode_adj };
    ops[n++] = (vsf_wifi_reg_op_t){ MT76_RF_PA_MODE_ADJ1, pa_mode_adj };
    ops[n++] = (vsf_wifi_reg_op_t){ MT76_BB_PA_MODE_CFG0, pa_mode[0] };
    ops[n++] = (vsf_wifi_reg_op_t){ MT76_BB_PA_MODE_CFG1, pa_mode[1] };
    ops[n++] = (vsf_wifi_reg_op_t){ MT76_RF_PA_MODE_CFG0, pa_mode[0] };
    ops[n++] = (vsf_wifi_reg_op_t){ MT76_RF_PA_MODE_CFG1, pa_mode[1] };
    ops[n++] = (vsf_wifi_reg_op_t){ MT76_TX0_RF_GAIN_CORR, tx0_gain };
    ops[n++] = (vsf_wifi_reg_op_t){ MT76_TX1_RF_GAIN_CORR, tx1_gain };

    vsf_wifi_chip_mt76_trace_info(
            "mt76: txpower regs band=%s ext_pa=%u" VSF_TRACE_CFG_LINEEND,
            is_5g ? "5g" : "2g", (unsigned)ext_pa);
    return n;
}

static int __mt76_configure_tx_delay(vsf_wifi_t *wifi, uint8_t channel,
                                     uint8_t bw, vsf_wifi_reg_op_t *ops, int n)
{
    bool ext_pa = __mt76_ext_pa_enabled(wifi, channel);
    uint32_t cfg0, cfg1;

    if (ext_pa) {
        cfg0 = bw ? 0x000b0c01 : 0x00101101;
        cfg1 = 0x00011414;
    } else {
        cfg0 = bw ? 0x000b0b01 : 0x00101001;
        cfg1 = 0x00021414;
    }

    ops[n++] = (vsf_wifi_reg_op_t){ MT76_TX_SW_CFG0, cfg0 };
    ops[n++] = (vsf_wifi_reg_op_t){ MT76_TX_SW_CFG1, cfg1 };
    return n;
}

#define MT76_MAX_TX_POWER_OPS               32
static vsf_wifi_reg_op_t __mt76_tx_power_ops[MT76_MAX_TX_POWER_OPS];

vsf_err_t __mt76_apply_tx_power(vsf_wifi_t *wifi, uint8_t channel,
                                       uint8_t bw, vsf_wifi_done_t done)
{
    mt76_tx_power_info_t txp;
    mt76_rate_power_t rate_pwr;
    int8_t delta_bw = 0;
    int16_t base_power, delta;
    int16_t txp_0, txp_1;
    int n = 0;
    vsf_err_t err;

    __mt76_get_power_info(wifi, &txp, channel);
    __mt76_get_rate_power(wifi, &rate_pwr, channel);

    if (bw == WIFI_BW_40MHZ_PLUS || bw == WIFI_BW_40MHZ_MINUS) {
        delta_bw = txp.delta_bw40;
    } else if (bw == WIFI_BW_80MHZ) {
        delta_bw = txp.delta_bw80;
    }

    __mt76_add_rate_power_offset(&rate_pwr,
            (int8_t)((int16_t)txp.target_power + delta_bw));
    __mt76_limit_rate_power(&rate_pwr, 0x3f);

    base_power = __mt76_get_min_rate_power(&rate_pwr);
    delta = base_power - (int16_t)txp.target_power;
    txp_0 = (int16_t)txp.chain[0].target_power +
            (int16_t)txp.chain[0].delta + delta;
    txp_1 = (int16_t)txp.chain[1].target_power +
            (int16_t)txp.chain[1].delta + delta;

    int16_t gain = vsf_min(txp_0, txp_1);
    if (gain < 0) {
        base_power -= gain;
        txp_0 -= gain;
        txp_1 -= gain;
    } else if (gain > 0x2f) {
        base_power -= (gain - 0x2f);
        txp_0 = 0x2f;
        txp_1 = 0x2f;
    }

    __mt76_add_rate_power_offset(&rate_pwr, (int8_t)(-base_power));
    __mt76_limit_rate_power(&rate_pwr, 0x3f);

    vsf_wifi_chip_mt76_trace_info(
            "mt76: txpower ch=%u bw=%u target=%u base=%d txp0=%d txp1=%d"
            VSF_TRACE_CFG_LINEEND,
            (unsigned)channel, (unsigned)bw, (unsigned)txp.target_power,
            (int)base_power, (int)txp_0, (int)txp_1);

    /* Emit band-dependent PA/ALC/TX-delay registers followed by the
     * per-rate power table as one register script. */
    n = __mt76_set_txpower_regs(wifi, channel, __mt76_tx_power_ops, n);
    n = __mt76_configure_tx_delay(wifi, channel, bw, __mt76_tx_power_ops, n);

    uint32_t alc0 =
        ((uint32_t)(txp_0 & 0x3f) << MT76_TX_ALC_CFG_0_CH_INIT_0_S) |
        ((uint32_t)(txp_1 & 0x3f) << MT76_TX_ALC_CFG_0_CH_INIT_1_S) |
        ((uint32_t)(0x3f) << MT76_TX_ALC_CFG_0_LIMIT_0_S) |
        ((uint32_t)(0x3f) << MT76_TX_ALC_CFG_0_LIMIT_1_S);

    __mt76_tx_power_ops[n++] = (vsf_wifi_reg_op_t){ MT76_TX_ALC_CFG_0, alc0 };

    __mt76_tx_power_ops[n++] = (vsf_wifi_reg_op_t){ MT76_TX_PWR_CFG_0,
        __mt76_tx_power_mask(rate_pwr.cck[0], rate_pwr.cck[2],
                             rate_pwr.ofdm[0], rate_pwr.ofdm[2]) };
    __mt76_tx_power_ops[n++] = (vsf_wifi_reg_op_t){ MT76_TX_PWR_CFG_1,
        __mt76_tx_power_mask(rate_pwr.ofdm[4], rate_pwr.ofdm[6],
                             rate_pwr.ht[0], rate_pwr.ht[2]) };
    __mt76_tx_power_ops[n++] = (vsf_wifi_reg_op_t){ MT76_TX_PWR_CFG_2,
        __mt76_tx_power_mask(rate_pwr.ht[4], rate_pwr.ht[6],
                             rate_pwr.ht[8], rate_pwr.ht[10]) };
    __mt76_tx_power_ops[n++] = (vsf_wifi_reg_op_t){ MT76_TX_PWR_CFG_3,
        __mt76_tx_power_mask(rate_pwr.ht[12], rate_pwr.ht[14],
                             rate_pwr.ht[0], rate_pwr.ht[2]) };
    __mt76_tx_power_ops[n++] = (vsf_wifi_reg_op_t){ MT76_TX_PWR_CFG_4,
        __mt76_tx_power_mask(rate_pwr.ht[4], rate_pwr.ht[6], 0, 0) };
    __mt76_tx_power_ops[n++] = (vsf_wifi_reg_op_t){ MT76_TX_PWR_CFG_7,
        __mt76_tx_power_mask(rate_pwr.ofdm[7], rate_pwr.vht[0],
                             rate_pwr.ht[7], rate_pwr.vht[1]) };
    __mt76_tx_power_ops[n++] = (vsf_wifi_reg_op_t){ MT76_TX_PWR_CFG_8,
        __mt76_tx_power_mask(rate_pwr.ht[14], 0,
                             rate_pwr.vht[0], rate_pwr.vht[1]) };
    __mt76_tx_power_ops[n++] = (vsf_wifi_reg_op_t){ MT76_TX_PWR_CFG_9,
        __mt76_tx_power_mask(rate_pwr.ht[7], 0,
                             rate_pwr.vht[0], rate_pwr.vht[1]) };

    VSF_ASSERT(n <= MT76_MAX_TX_POWER_OPS);

    err = vsf_wifi_reg_run_script(wifi, __mt76_tx_power_ops,
                                  (uint16_t)n, done);
    if (err != VSF_ERR_NONE) {
        vsf_wifi_chip_mt76_trace_info(
                "mt76: apply_tx_power failed (err=%d)" VSF_TRACE_CFG_LINEEND,
                (int)err);
    }
    return err;
}
#endif      /* VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_MT76 == ENABLED */
