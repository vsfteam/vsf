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

/* USB vendor request types used by MT76 */
#define MT76_VEND_READ_CFG          0x01
#define MT76_VEND_WRITE_CFG         0x02
#define MT76_VEND_READ_EEPROM       0x03
#define MT76_VEND_MULTI_READ        0x07
#define MT76_VEND_MULTI_WRITE       0x06
#define MT76_VEND_WRITE_FCE         0x05
#define MT76_VEND_DEV_MODE          0x01

/*============================ IMPLEMENTATION ================================*/

vsf_err_t __mt76_usb_reg_read(vsf_wifi_t *wifi, uint32_t reg,
                               uint32_t *out, vsf_wifi_done_t done)
{
    /* TODO: USB control transfer read */
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t __mt76_usb_reg_write(vsf_wifi_t *wifi, uint32_t reg,
                                uint32_t val, vsf_wifi_done_t done)
{
    /* TODO: USB control transfer write */
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t __mt76_usb_bulk_out(vsf_wifi_t *wifi, const uint8_t *data,
                               uint16_t len, uint8_t ep_idx,
                               vsf_wifi_done_t done)
{
    /* TODO: USB bulk OUT */
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t __mt76_usb_bulk_in_submit(vsf_wifi_t *wifi, uint8_t *buf,
                                     uint16_t len, uint8_t ep_idx)
{
    /* TODO: submit bulk IN URB */
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t __mt76_usb_vendor_req(vsf_wifi_t *wifi, uint8_t req,
                                 uint8_t req_type, uint16_t value,
                                 uint16_t index, void *buf, uint16_t len,
                                 vsf_wifi_done_t done)
{
    /* TODO: generic vendor request */
    return VSF_ERR_NOT_SUPPORT;
}

#endif      /* VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_MT76 == ENABLED */
