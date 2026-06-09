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

#ifndef __VSF_USBH_WIFI_H__
#define __VSF_USBH_WIFI_H__

/*============================ INCLUDES ======================================*/
#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_WIFI == ENABLED

#include "component/wifi/vsf_wifi.h"

#if VSF_USE_WIFI == ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/* Backward-compatibility type aliases — old code using vk_usbh_wifi_*_t
 * names continues to compile without modification. */
typedef vsf_wifi_scan_result_t   vk_usbh_wifi_scan_result_t;
typedef vsf_wifi_link_info_t     vk_usbh_wifi_link_info_t;
typedef vsf_wifi_auth_cfg_t      vk_usbh_wifi_auth_cfg_t;

/*============================ TYPES =========================================*/

typedef struct vk_usbh_wifi_t    vk_usbh_wifi_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbh_class_drv_t vk_usbh_wifi_drv;

/*============================ USB-SPECIFIC API ==============================*/

/* TX path — USB-specific because it ships frames via bulk OUT endpoints.
 * Other buses (SDIO) would have their own send API. */
vsf_err_t    vk_usbh_wifi_send(void *dev, uint8_t *frame, uint16_t len);
bool         vk_usbh_wifi_can_send(void *dev);

/* Retrieve the embedded generic wifi handle from the USB wifi device. */
vsf_wifi_t * vk_usbh_wifi_get_wifi(void *dev);

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_WIFI
#endif      // VSF_USE_USB_HOST && VSF_USBH_USE_WIFI
#endif      // __VSF_USBH_WIFI_H__
