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

#ifndef __HAL_DRIVER_${SERIES}_USBH_H__
#define __HAL_DRIVER_${SERIES}_USBH_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_USBH == ENABLED

#include "../usb.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vk_hw_usbh_param_t {
    const vsf_hw_usb_t *dev;
    vsf_arch_prio_t priority;
} vk_hw_usbh_param_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbh_hcd_drv_t vk_hw_usbh_drv;

/*============================ PROTOTYPES ====================================*/

#endif      // VSF_HAL_USE_USBH
#endif      // __HAL_DRIVER_${SERIES}_USBH_H__
/* EOF */
