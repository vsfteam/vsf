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

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED

#define __VSF_USBH_CLASS_IMPLEMENT_CLASS__
#include "../../vsf_usbh.h"
#include "./vsf_usbh_rtl8152.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/

static const vk_usbh_dev_id_t __vk_usbh_rtl8152_dev_id[] = {
    { VSF_USBH_MATCH_VID_PID(0x0BDA, 0x8152) },
};

/*============================ PROTOTYPES ====================================*/

static void *__vk_usbh_rtl8152_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs);

#if VSF_USBH_USE_LIBUSB == ENABLED
extern void __vk_usbh_libusb_block_dev(vk_usbh_dev_t *dev);
#endif

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbh_class_drv_t vk_usbh_rtl8152_drv = {
    .name       = "rtl8152",
    .dev_id_num = dimof(__vk_usbh_rtl8152_dev_id),
    .dev_ids    = __vk_usbh_rtl8152_dev_id,
    .probe      = __vk_usbh_rtl8152_probe,
};

/*============================ IMPLEMENTATION ================================*/

static void *__vk_usbh_rtl8152_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs)
{
#if VSF_USBH_USE_LIBUSB == ENABLED
    __vk_usbh_libusb_block_dev(dev);
#endif
    return NULL;
}

#endif
