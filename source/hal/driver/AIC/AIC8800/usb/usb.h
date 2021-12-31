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

#ifndef __OSA_HAL_DRIVER_AIC_AIC8800_USB_H__
#define __OSA_HAL_DRIVER_AIC_AIC8800_USB_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal.h"

#if     (VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_DCD_DWCOTG == ENABLED)   \
    ||  (VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_HCD_DWCOTG == ENABLED)

#include "hal/driver/common/template/vsf_template_usb.h"

// for dedicated vk_dwcotg_hw_info_t
#include "component/usb/driver/otg/dwcotg/vsf_dwcotg_hw.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define ____USB_OTG_DEF(__N, __VALUE)                                           \
    extern aic8800_usb_t USB_OTG##__N##_IP;                                     \
    extern const i_usb_dc_ip_t VSF_USB_DC##__N##_IP;                            \
    extern const i_usb_hc_ip_t VSF_USB_HC##__N##_IP;
#define __USB_OTG_DEF(__N, __VALUE)         ____USB_OTG_DEF(__N, __VALUE)
#define USB_OTG_DEF(__N, __VALUE)           __USB_OTG_DEF(__N, __VALUE)

/*============================ TYPES =========================================*/

typedef struct aic8800_usb_const_t {
    uint8_t dc_ep_num;
    uint8_t hc_ep_num;
    void *reg;

    implement(vk_dwcotg_hw_info_t)
} aic8800_usb_const_t;

typedef struct aic8800_usb_t {
    bool is_host;
    struct {
        usb_ip_irqhandler_t irqhandler;
        void *param;
    } callback;
    const aic8800_usb_const_t *param;
} aic8800_usb_t;

/*============================ INCLUDES ======================================*/

#include "./hc/usbh.h"
#include "./dc/usbd.h"

/*============================ GLOBAL VARIABLES ==============================*/

VSF_MREPEAT(USB_OTG_COUNT, USB_OTG_DEF, NULL)

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

#endif
#endif
/* EOF */
