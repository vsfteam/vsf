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

#ifndef __VSFVM_EXT_LIBUSB_H__
#define __VSFVM_EXT_LIBUSB_H__

/*============================ INCLUDES ======================================*/

#include "vsf_vm_cfg.h"

#if     ((VSFVM_CFG_RUNTIME_EN == ENABLED) || (VSFVM_CFG_COMPILER_EN == ENABLED))\
    &&  (VSF_USE_USB_HOST == ENABLED) && (VSF_USBH_USE_LIBUSB == ENABLED)

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern const vsfvm_class_t vsfvm_ext_libusb_dev;

/*============================ PROTOTYPES ====================================*/

extern void vsfvm_ext_register_libusb(void);

#endif      // (VSFVM_CFG_RUNTIME_EN || VSFVM_CFG_COMPILER_EN) && VSF_USE_USB_HOST && VSF_USBH_USE_LIBUSB
#endif      // __VSFVM_EXT_LIBUSB_H__
