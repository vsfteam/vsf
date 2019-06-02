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

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct usbh_demo_const_t {
    struct {
        vsf_ohci_param_t ohci_param;
    } usbh;
};
typedef struct usbh_demo_const_t usbh_demo_const_t;

struct usbh_demo_t {
    struct {
        vsf_usbh_t host;
        vsf_usbh_class_t hub;
        vsf_usbh_class_t ecm;
        vsf_usbh_class_t bthci;
        vsf_usbh_class_t hid;
    } usbh;
};
typedef struct usbh_demo_t usbh_demo_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const usbh_demo_const_t usbh_demo_const = {
    .usbh.ohci_param        = {
        .hc                 = (vsf_usb_hc_t *)&USB_HC0,
        .priority           = 0xFF,
    },
};

static usbh_demo_t usbh_demo = {
    .usbh                       = {
        .host.drv               = &vsf_ohci_drv,
        .host.param             = (void *)&usbh_demo_const.usbh.ohci_param,

        .hub.drv                = &vsf_usbh_hub_drv,
        .ecm.drv                = &vsf_usbh_ecm_drv,
        .bthci.drv              = &vsf_usbh_bthci_drv,
        .hid.drv                = &vsf_usbh_hid_drv,
    },
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void usbh_demo_start(void)
{
    vsf_ohci_init();
    vsf_usbh_init(&usbh_demo.usbh.host);
    vsf_usbh_register_class(&usbh_demo.usbh.host, &usbh_demo.usbh.hub);
    vsf_usbh_register_class(&usbh_demo.usbh.host, &usbh_demo.usbh.ecm);
    vsf_usbh_register_class(&usbh_demo.usbh.host, &usbh_demo.usbh.bthci);
    vsf_usbh_register_class(&usbh_demo.usbh.host, &usbh_demo.usbh.hid);
}
