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

/* MCU command IDs, aligned with Linux mt76x02_mcu.h */
enum {
    MT76_CMD_FUN_SET_OP         = 1,
    MT76_CMD_LOAD_CR            = 2,
    MT76_CMD_INIT_GAIN_OP       = 3,
    MT76_CMD_RANDOM_READ        = 10,
    MT76_CMD_RANDOM_WRITE       = 12,
    MT76_CMD_SWITCH_CHANNEL_OP  = 30,
    MT76_CMD_CALIBRATION_OP     = 31,
};

/*============================ IMPLEMENTATION ================================*/

vsf_err_t __mt76_mcu_send_cmd(vsf_wifi_t *wifi, uint8_t cmd,
                               const void *data, uint16_t len,
                               bool wait_resp, vsf_wifi_done_t done)
{
    /* TODO: build MCU command packet and send via bulk OUT */
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t __mt76_mcu_set_channel(vsf_wifi_t *wifi, uint8_t channel,
                                  uint8_t bw, uint8_t bw_index, bool scan,
                                  vsf_wifi_done_t done)
{
    /* TODO: CMD_SWITCH_CHANNEL_OP */
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t __mt76_mcu_load_cr(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    /* TODO: CMD_LOAD_CR */
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t __mt76_mcu_set_radio_state(vsf_wifi_t *wifi, bool on,
                                       vsf_wifi_done_t done)
{
    /* TODO: CMD_POWER_SAVING_OP */
    return VSF_ERR_NOT_SUPPORT;
}

#endif      /* VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_MT76 == ENABLED */
