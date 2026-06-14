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

/*============================ IMPLEMENTATION ================================*/

vsf_err_t __mt76_phy_set_channel(vsf_wifi_t *wifi, uint8_t channel,
                                  uint8_t bw, uint8_t bw_index,
                                  vsf_wifi_done_t done)
{
    /* TODO: BBP/RF band/bw setup + MCU switch channel */
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t __mt76_phy_set_txpower(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    /* TODO: load per-rate TX power from EEPROM */
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t __mt76_phy_calibrate(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    /* TODO: channel calibration via MCU */
    return VSF_ERR_NOT_SUPPORT;
}

#endif      /* VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_MT76 == ENABLED */
