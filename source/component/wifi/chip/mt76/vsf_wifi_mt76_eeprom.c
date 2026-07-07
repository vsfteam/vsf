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

static void __mt76_eeprom_load_continue(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_mac_addr_program_continue(vsf_wifi_t *wifi, vsf_err_t err);

/*============================ IMPLEMENTATION ================================*/

uint16_t __mt76_eeprom_get_u16(mt76_wifi_priv_t *priv, uint16_t offset)
{
    return (uint16_t)priv->eeprom[offset] |
           ((uint16_t)priv->eeprom[offset + 1] << 8);
}

uint8_t __mt76_eeprom_get_u8(mt76_wifi_priv_t *priv, uint16_t offset)
{
    return priv->eeprom[offset];
}

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

vsf_err_t __mt76_mac_addr_program_start(vsf_wifi_t *wifi,
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
#endif      /* VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_MT76 == ENABLED */
