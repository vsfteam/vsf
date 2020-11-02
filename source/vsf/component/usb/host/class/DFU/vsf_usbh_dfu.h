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

#ifndef __VSF_USBH_DFU_H__
#define __VSF_USBH_DFU_H__


/*============================ INCLUDES ======================================*/
#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_DFU == ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define vk_usbh_dfu_downlaod(__dfu, __buffer, __size)                           \
            __vk_usbh_dfu_operate((__dfu), (__buffer), (__size), USB_DFUREQ_DNLOAD)
#define vk_usbh_dfu_uplaod(__dfu, __buffer, __size)                             \
            __vk_usbh_dfu_operate((__dfu), (__buffer), (__size), USB_DFUREQ_UPLOAD)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbh_class_drv_t vk_usbh_dfu_drv;

/*============================ PROTOTYPES ====================================*/

vsf_err_t vk_usbh_dfu_get_result(void *dfu_ptr);
vsf_err_t __vk_usbh_dfu_operate(void *dfu_ptr, uint8_t *buffer, uint_fast32_t size, uint8_t request);

#ifdef __cplusplus
}
#endif

#endif
#endif
