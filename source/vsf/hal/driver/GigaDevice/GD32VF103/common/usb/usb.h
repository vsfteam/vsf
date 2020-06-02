/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

#ifndef __HAL_DRIVER_GIGADEVICE_GD32VF103_USB_H__
#define __HAL_DRIVER_GIGADEVICE_GD32VF103_USB_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "../../__device.h"

//#include "hal/interface/vsf_interface_pm.h"
#include "hal/interface/vsf_interface_usb.h"

// for vsf_dwcotg_hw_info_t
#include "component/usb/driver/otg/dwcotg/vsf_dwcotg_hw.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define __USB_OTG_DEF(__N, __VALUE)                                             \
    extern gd32vf103_usb_t USB_OTG##__N##_IP;                                   \
    extern const i_usb_hc_ip_t VSF_USB_HC##__N##_IP;                            \
    extern const i_usb_dc_ip_t VSF_USB_DC##__N##_IP;
#define _USB_OTG_DEF(__N, __VALUE)          __USB_OTG_DEF(__N, __VALUE)
#define USB_OTG_DEF(__N, __VALUE)           _USB_OTG_DEF(__N, __VALUE)

/*============================ TYPES =========================================*/

struct gd32vf103_usb_const_t {
    uint8_t ep_num;
    IRQn_Type irq;
    void *reg;

    implement(vsf_dwcotg_hw_info_t)
};
typedef struct gd32vf103_usb_const_t gd32vf103_usb_const_t;

struct gd32vf103_usb_t {
    bool is_host;
    struct {
        usb_ip_irq_handler_t irq_handler;
        void *param;
    } callback;
    const gd32vf103_usb_const_t *param;
};
typedef struct gd32vf103_usb_t gd32vf103_usb_t;

/*============================ INCLUDES ======================================*/
#include "./hc/usbh.h"
#include "./dc/usbd.h"

/*============================ GLOBAL VARIABLES ==============================*/

REPEAT_MACRO(USB_OTG_COUNT, USB_OTG_DEF, NULL)

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */
