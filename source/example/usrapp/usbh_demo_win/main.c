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

#include "vsf.h"
#include "component/usb/driver/hcd/libusb_hcd/vsf_libusb_hcd.h"

/*============================ MACROS ========================================*/

#define GENERATE_HEX(value)                TPASTE2(0x, value)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct usrapp_const_t {
    vsf_libusb_hcd_param_t libusb_hcd_param;
};
typedef struct usrapp_const_t usrapp_const_t;

struct usrapp_t {
    vsf_usbh_t usbh;
#if VSF_USE_TCPIP == ENABLED && VSF_USE_USB_HOST_ECM == ENABLED
    vsf_usbh_class_t ecm;
#endif
#if VSF_USE_USB_HOST_BTHCI == ENABLED
    vsf_usbh_class_t bthci;
#endif
#if VSF_USE_USB_HOST_HID == ENABLED
    vsf_usbh_class_t hid;
#endif
#if VSF_USE_USB_HOST_DS4 == ENABLED
    vsf_usbh_class_t ds4;
#endif
};
typedef struct usrapp_t usrapp_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const usrapp_const_t usrapp_const = {
    .libusb_hcd_param           = {
        .priority               = vsf_arch_prio_0,
    },
};

static usrapp_t usrapp = {
    .usbh.drv                   = &vsf_libusb_hcd_drv,
    .usbh.param                 = (void *)&usrapp_const.libusb_hcd_param,

#if VSF_USE_TCPIP == ENABLED && VSF_USE_USB_HOST_ECM == ENABLED
    .ecm.drv                    = &vsf_usbh_ecm_drv,
#endif
#if VSF_USE_USB_HOST_BTHCI == ENABLED
    .bthci.drv                  = &vsf_usbh_bthci_drv,
#endif
#if VSF_USE_USB_HOST_HID == ENABLED
    .hid.drv                    = &vsf_usbh_hid_drv,
#endif
#if VSF_USE_USB_HOST_DS4 == ENABLED
    .ds4.drv                    = &vsf_usbh_ds4_drv,
#endif
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

int main(void)
{
    vsf_trace_init(NULL);
    vsf_stdio_init();

    vsf_usbh_init(&usrapp.usbh);
#if VSF_USE_TCPIP == ENABLED && VSF_USE_USB_HOST_ECM == ENABLED
    vsf_usbh_register_class(&usrapp.usbh, &usrapp.ecm);
#endif
#if VSF_USE_USB_HOST_BTHCI == ENABLED
    vsf_usbh_register_class(&usrapp.usbh, &usrapp.bthci);
#endif
#if VSF_USE_USB_HOST_HID == ENABLED
    vsf_usbh_register_class(&usrapp.usbh, &usrapp.hid);
#endif
#if VSF_USE_USB_HOST_DS4 == ENABLED
    vsf_usbh_register_class(&usrapp.usbh, &usrapp.ds4);
#endif

    return 0;
}

/* EOF */
