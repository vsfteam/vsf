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

#ifndef __HAL_DRIVER_AIR105_USB_H__
#define __HAL_DRIVER_AIR105_USB_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_USBD == ENABLED || VSF_HAL_USE_USBH == ENABLED

#include "../__device.h"

#include "hal/driver/common/template/vsf_template_usb.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_HAL_USE_USBD == ENABLED && VSF_HAL_USE_USBH == ENABLED
#   define __USB_OTG_DEF(__N, __VALUE)                                          \
    extern vsf_hw_usb_t USB_OTG##__N##_IP;                                      \
    extern const i_usb_hc_ip_t VSF_USB_HC##__N##_IP;                            \
    extern const i_usb_dc_ip_t VSF_USB_DC##__N##_IP;
#elif VSF_HAL_USE_USBD == ENABLED
#   define __USB_OTG_DEF(__N, __VALUE)                                          \
    extern vsf_hw_usb_t USB_OTG##__N##_IP;                                      \
    extern const i_usb_dc_ip_t VSF_USB_DC##__N##_IP;
#elif VSF_HAL_USE_USBH == ENABLED
#   define __USB_OTG_DEF(__N, __VALUE)                                          \
    extern vsf_hw_usb_t USB_OTG##__N##_IP;                                      \
    extern const i_usb_hc_ip_t VSF_USB_HC##__N##_IP;
#endif

#define _USB_OTG_DEF(__N, __VALUE)          __USB_OTG_DEF(__N, __VALUE)
#define USB_OTG_DEF(__N, __VALUE)           _USB_OTG_DEF(__N, __VALUE)

/*============================ TYPES =========================================*/

#ifdef VSF_MUSB_FDRC_PRIV_REG_T
typedef struct vk_musb_fdrc_common_reg_t {
    volatile uint8_t FAddr;
    volatile uint8_t Power;
    volatile uint8_t IntrTx1;
    volatile uint8_t IntrTx2;
    volatile uint8_t IntrRx1;
    volatile uint8_t IntrRx2;
    volatile uint8_t IntrTx1E;
    volatile uint8_t IntrTx2E;
    volatile uint8_t IntrRx1E;
    volatile uint8_t IntrRx2E;
    volatile uint8_t IntrUSB;
    volatile uint8_t IntrUSBE;
    volatile uint8_t Frame1;
    volatile uint8_t Frame2;
    volatile uint8_t Index;
    volatile uint8_t Dummy[1 + 0x50];
    volatile uint8_t DevCtl;
} VSF_CAL_PACKED vk_musb_fdrc_common_reg_t;
#endif

typedef struct vsf_hw_usb_const_t {
    void *reg;
    uint8_t irqn;
} vsf_hw_usb_const_t;

typedef struct vsf_hw_usb_t {
#if VSF_HAL_USE_USBD == ENABLED && VSF_HAL_USE_USBH == ENABLED
    bool is_host;
#endif

    struct {
        usb_ip_irqhandler_t irqhandler;
        void *param;
    } callback;
    const vsf_hw_usb_const_t *param;
} vsf_hw_usb_t;

/*============================ INCLUDES ======================================*/

#include "./hc/usbh.h"
#include "./dc/usbd.h"

/*============================ GLOBAL VARIABLES ==============================*/

VSF_MREPEAT(USB_OTG_COUNT, USB_OTG_DEF, NULL)

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

#endif      // VSF_HAL_USE_USBD || VSF_HAL_USE_USBH
#endif      // __HAL_DRIVER_AIR105_USB_H__
/* EOF */
