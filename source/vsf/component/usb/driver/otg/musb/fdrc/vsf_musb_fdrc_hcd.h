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

#ifndef __VSF_MUSB_FDRC_HCD_H__
#define __VSF_MUSB_FDRC_HCD_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_HCD_MUSB_FDRC == ENABLED

#include "component/usb/vsf_usb.h"
// for i_usb_hc_ip_t
#include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vk_musb_fdrc_hcd_param_t {
    const i_usb_hc_ip_t *op;
    vsf_arch_prio_t priority;
} vk_musb_fdrc_hcd_param_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbh_hcd_drv_t vk_musb_fdrc_hcd_drv;

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

#endif
#endif
/* EOF */
