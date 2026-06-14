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

#define MT76_EE_MAC_ADDR            0x004
#define MT76_EE_NIC_CONF_0          0x034
#define MT76_EE_NIC_CONF_1          0x036
#define MT76_EE_NIC_CONF_2          0x042

/*============================ IMPLEMENTATION ================================*/

vsf_err_t __mt76_eeprom_load(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    /* TODO: read EEPROM via USB vendor request */
    return VSF_ERR_NOT_SUPPORT;
}

void __mt76_eeprom_parse_hw_cap(vsf_wifi_t *wifi)
{
    /* TODO: parse TX/RX path, PA/LNA, etc. */
}

const uint8_t *__mt76_eeprom_get_mac(vsf_wifi_t *wifi)
{
    mt76_wifi_priv_t *priv = (mt76_wifi_priv_t *)wifi->chip_priv;
    return &priv->eeprom[MT76_EE_MAC_ADDR];
}

#endif      /* VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_MT76 == ENABLED */
