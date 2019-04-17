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

#if VSF_USE_USB_HOST == ENABLED && VSF_USE_USB_HOST_HCD_OHCI == ENABLED

#include "../../vsf_usbh.h"
#include "hal/interface/vsf_interface_usb.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct vsf_ohci_param_t {
    vsf_usb_hc_t *hc;
    vsf_arch_priority_t priority;
};
typedef struct vsf_ohci_param_t vsf_ohci_param_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_usbh_hcd_drv_t vsf_ohci_drv;

/*============================ PROTOTYPES ====================================*/

void vsf_ohci_init(void);

#endif

#endif // __VSF_OHCI_H___
