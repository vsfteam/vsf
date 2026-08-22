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

#ifndef __VSF_USBH_MSC_H__
#define __VSF_USBH_MSC_H__


/*============================ INCLUDES ======================================*/
#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_MSC == ENABLED

#include "component/usb/common/class/MSC/vsf_usb_MSC.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// number of scsi instances the host msc driver exposes(= max LUNs addressable
//  per BOT interface). USB BOT allows up to 16 LUNs(bCBWLUN is 4-bit wide);
//  override to the real LUN count of the target device to save RAM.
#ifndef VSF_USBH_MSC_CFG_MAX_LUN
#   define VSF_USBH_MSC_CFG_MAX_LUN         2
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbh_class_drv_t vk_usbh_msc_drv;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
#endif
