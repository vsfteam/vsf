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

#if VSF_USE_USB_HOST == ENABLED
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct vsf_usbh_libusb_dev_t
{
    vsf_usbh_t *usbh;
    vsf_usbh_dev_t *dev;

    uint16_t vid, pid;
    unsigned opened : 1;
    unsigned removed : 1;
};
typedef struct vsf_usbh_libusb_dev_t vsf_usbh_libusb_dev_t;

enum vsf_usbh_libusb_evt_t
{
    VSF_USBH_LIBUSB_EVT_ON_ARRIVED,
    VSF_USBH_LIBUSB_EVT_ON_LEFT,
};
typedef enum vsf_usbh_libusb_evt_t vsf_usbh_libusb_evt_t;

typedef void (*vsf_usbh_libusb_on_event_t)( void *param,
                                            vsf_usbh_libusb_dev_t *dev,
                                            vsf_usbh_libusb_evt_t evt);

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_usbh_class_drv_t vsf_usbh_libusb_drv;

/*============================ PROTOTYPES ====================================*/

extern void vsf_usbh_libusb_set_evthandler(void *param,
        vsf_usbh_libusb_on_event_t on_event);

extern vsf_err_t vsf_usbh_libusb_open(vsf_usbh_libusb_dev_t *ldev);
extern void vsf_usbh_libusb_close(vsf_usbh_libusb_dev_t *ldev);

#endif
#endif
