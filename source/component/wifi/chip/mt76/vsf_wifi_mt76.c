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

#define MT76_ASIC_VERSION           0x0000
#define MT76_USB_U3DMA_CFG          0x9018
#define MT76_WPDMA_GLO_CFG          0x0208
#define MT76_MAC_SYS_CTRL           0x1004
#define MT76_RX_FILTR_CFG           0x1400

/*============================ LOCAL VARIABLES ===============================*/

/*============================ PROTOTYPES ====================================*/

extern vsf_err_t __mt76_firmware_load(vsf_wifi_t *wifi, vsf_wifi_done_t done);
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

/*============================ IMPLEMENTATION ================================*/

static mt76_wifi_priv_t *__mt76_priv(vsf_wifi_t *wifi)
{
    return (mt76_wifi_priv_t *)wifi->chip_priv;
}

static const vsf_wifi_mt76_bus_ops_t *__mt76_bus_ops(vsf_wifi_t *wifi)
{
    VSF_WIFI_ASSERT(wifi->bus_ops != NULL);
    return (const vsf_wifi_mt76_bus_ops_t *)wifi->bus_ops;
}

#define __mt76_mcu_cmd(__wifi, ...)     __mt76_bus_ops(__wifi)->mcu_cmd(__wifi, __VA_ARGS__)
#define __mt76_tx_frame(__wifi, ...)    __mt76_bus_ops(__wifi)->tx_frame(__wifi, __VA_ARGS__)
#define __mt76_rx_submit(__wifi, ...)   __mt76_bus_ops(__wifi)->rx_submit(__wifi, __VA_ARGS__)

vsf_err_t __mt76_firmware_load(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    (void)done;
    /* TODO: load ROM patch and main firmware via mcu_cmd/tx_frame */
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t __mt76_init(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    (void)done;
    /* TODO: power on, read ASIC version, load EEPROM, init MAC/PHY */
    return VSF_ERR_NOT_SUPPORT;
}

void __mt76_fini(vsf_wifi_t *wifi)
{
    /* TODO: stop TX/RX, power down */
}

vsf_err_t __mt76_set_channel(vsf_wifi_t *wifi, uint8_t channel,
                             vsf_wifi_done_t done)
{
    (void)channel; (void)done;
    /* TODO: CMD_SWITCH_CHANNEL_OP via mcu_cmd */
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t __mt76_set_rx_filter(vsf_wifi_t *wifi, uint32_t mask,
                               vsf_wifi_done_t done)
{
    (void)mask; (void)done;
    /* TODO: write MT_RX_FILTR_CFG via mcu_cmd */
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t __mt76_set_mac_addr(vsf_wifi_t *wifi, const uint8_t mac[6],
                              vsf_wifi_done_t done)
{
    (void)mac; (void)done;
    /* TODO: program MAC address via mcu_cmd */
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t __mt76_set_bssid(vsf_wifi_t *wifi, const uint8_t bssid[6],
                           vsf_wifi_done_t done)
{
    /* TODO: program BSSID */
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t __mt76_set_auth_mode(vsf_wifi_t *wifi,
                               const vsf_wifi_auth_cfg_t *cfg,
                               vsf_wifi_done_t done)
{
    (void)cfg; (void)done;
    /* TODO: store auth config for connect() */
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t __mt76_connect(vsf_wifi_t *wifi,
                         const uint8_t bssid[6], const uint8_t *ssid,
                         uint8_t ssid_len, uint8_t channel,
                         vsf_wifi_done_t done)
{
    (void)bssid; (void)ssid; (void)ssid_len; (void)channel; (void)done;
    /* TODO: set BSSID/WCID/key via mcu_cmd, start rx_submit */
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t __mt76_disconnect(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    (void)done;
    /* TODO: clear BSSID/WCID via mcu_cmd, disable TX/RX */
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t __mt76_get_link_info(vsf_wifi_t *wifi, vsf_wifi_link_info_t *info)
{
    /* TODO: read RSSI/rate */
    return VSF_ERR_NOT_SUPPORT;
}

void __mt76_parse_rx(vsf_wifi_t *wifi, uint8_t *frame, uint16_t len)
{
    /* TODO: parse beacon/probe response during scan */
}

vsf_err_t __mt76_tx(vsf_wifi_t *wifi, const uint8_t *frame, uint16_t len)
{
    (void)frame; (void)len;
    /* TODO: build TXINFO+TXWI and submit tx_frame */
    return VSF_ERR_NOT_SUPPORT;
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
};

#endif      /* VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_MT76 == ENABLED */
