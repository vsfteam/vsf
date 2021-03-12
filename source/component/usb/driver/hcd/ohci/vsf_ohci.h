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
#ifndef __VSF_OHCI_H___
#define __VSF_OHCI_H___

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_HCD_OHCI == ENABLED

#include "component/usb/host/vsf_usbh.h"
#include "hal/vsf_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_OHCI_CFG_SETUP_CONTROL == ENABLED
#   define VSF_OHCI_FLAG_NO_DATA   0x10
#   define VSF_OHCI_FLAG_NO_STATUS 0x20
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vk_ohci_param_t {
    const i_usb_hc_ip_t *op;
    vsf_arch_prio_t priority;
} vk_ohci_param_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbh_hcd_drv_t vk_ohci_drv;

/*============================ PROTOTYPES ====================================*/

void vk_ohci_init(void);

#ifdef __cplusplus
}
#endif

#endif
#endif // __VSF_OHCI_H___
