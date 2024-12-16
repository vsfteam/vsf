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

#ifndef __OSA_HAL_DRIVER_GIGADEVICE_GD32H7XX_USB_PRIV_H__
#define __OSA_HAL_DRIVER_GIGADEVICE_GD32H7XX_USB_PRIV_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal.h"

#if VSF_USE_USB_DEVICE == ENABLED || VSF_USE_USB_HOST == ENABLED

// for dedicated vk_dwcotg_hw_info_t
#include "component/usb/driver/otg/dwcotg/vsf_dwcotg_hw.h"

// for IRQn_Type
#include "../vendor/Include/gd32h7xx.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_hw_usb_const_t {
    uint8_t ep_num;
    IRQn_Type irq;
    void *reg;

    implement(vk_dwcotg_hw_info_t)
} vsf_hw_usb_const_t;

typedef struct vsf_hw_usb_t {
    bool is_host;
    struct {
        usb_ip_irqhandler_t irqhandler;
        void *param;
    } callback;
    const vsf_hw_usb_const_t *param;
} vsf_hw_usb_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

#endif
#endif
/* EOF */
