/*****************************************************************************
 *   Copyright(C)2009-2020 by VSF Team                                       *
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

#ifndef __OSA_HAL_DRIVER_ALLWINNER_F1X00S_USBH_H__
#define __OSA_HAL_DRIVER_ALLWINNER_F1X00S_USBH_H__

/*============================ INCLUDES ======================================*/

#include "osa_hal/vsf_osa_hal_cfg.h"

#if VSF_USE_USB_HOST == ENABLED

#include "component/usb/host/vsf_usbh.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_usb_hcd_param_t {
    vsf_arch_prio_t priority;
} vsf_usb_hcd_param_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbh_hcd_drv_t vsf_usb_hcd_drv;

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_USB_HOST
#endif      // __OSA_HAL_DRIVER_ALLWINNER_F1X00S_USBH_H__
/* EOF */
