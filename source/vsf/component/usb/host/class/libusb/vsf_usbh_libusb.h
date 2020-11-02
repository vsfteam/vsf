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

#ifndef __VSF_USBH_LIBUSB_H__
#define __VSF_USBH_LIBUSB_H__

/*============================ INCLUDES ======================================*/
#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_LIBUSB == ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vk_usbh_libusb_dev_t {
    vk_usbh_t *usbh;
    vk_usbh_dev_t *dev;
    vk_usbh_urb_t urb;
    void *user_data;

    uint16_t vid, pid;
    uint8_t c;
    uint8_t subc;
    uint8_t protocol;
    uint8_t ep0size;
    uint8_t address;
    bool is_opened;
    bool is_to_remove;
} vk_usbh_libusb_dev_t;

typedef enum vk_usbh_libusb_evt_t {
    VSF_USBH_LIBUSB_EVT_ON_ARRIVED,
    VSF_USBH_LIBUSB_EVT_ON_LEFT,
} vk_usbh_libusb_evt_t;

typedef void (*vk_usbh_libusb_on_event_t)( void *param,
                                            vk_usbh_libusb_dev_t *dev,
                                            vk_usbh_libusb_evt_t evt);

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbh_class_drv_t vk_usbh_libusb_drv;

/*============================ PROTOTYPES ====================================*/

extern void vk_usbh_libusb_set_evthandler(void *param,
        vk_usbh_libusb_on_event_t on_event);

extern vsf_err_t vk_usbh_libusb_open(vk_usbh_libusb_dev_t *ldev);
extern void vk_usbh_libusb_close(vk_usbh_libusb_dev_t *ldev);

#ifdef __cplusplus
}
#endif

#endif
#endif
