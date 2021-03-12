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

#ifndef __VSF_DWCOTG_HCD_H__
#define __VSF_DWCOTG_HCD_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_HCD_DWCOTG == ENABLED

#include "component/usb/host/vsf_usbh.h"
#include "hal/vsf_hal.h"
#include "./vsf_dwcotg_common.h"

#if     defined(__VSF_DWCOTG_HCD_CLASS_IMPLEMENT)
#   undef __VSF_DWCOTG_HCD_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vk_dwcotg_hcd_param_t {
    const i_usb_hc_ip_t *op;
    vsf_arch_prio_t priority;
} vk_dwcotg_hcd_param_t;

typedef struct vk_dwcotg_hc_ip_info_t {
    implement(usb_hc_ip_info_t)
    implement(vk_dwcotg_hw_info_t)
} vk_dwcotg_hc_ip_info_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbh_hcd_drv_t vk_dwcotg_hcd_drv;

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
#endif
/* EOF */
