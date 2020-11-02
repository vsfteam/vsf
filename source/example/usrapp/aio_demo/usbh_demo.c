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
        vk_ohci_param_t ohci_param;
    } usbh;
};
typedef struct usbh_demo_const_t usbh_demo_const_t;

struct usbh_demo_t {
    struct {
        vk_usbh_t host;
#if VSF_USBH_USE_HUB == ENABLED
        vk_usbh_class_t hub;
#endif
#if VSF_USBH_USE_ECM == ENABLED
        vk_usbh_class_t ecm;
#endif
#if VSF_USBH_USE_BTHCI == ENABLED
        vk_usbh_class_t bthci;
#endif
#if VSF_USBH_USE_HID == ENABLED
        vk_usbh_class_t hid;
#endif
#if VSF_USBH_USE_DS4 == ENABLED
        vk_usbh_class_t ds4;
#endif
    } usbh;
};
typedef struct usbh_demo_t usbh_demo_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const usbh_demo_const_t usbh_demo_const = {
    .usbh.ohci_param        = {
        .op                 = &VSF_USB_HC0_IP,
        .priority           = vsf_arch_prio_0,
    },
};

static usbh_demo_t usbh_demo = {
    .usbh                       = {
        .host.drv               = &vk_ohci_drv,
        .host.param             = (void *)&usbh_demo_const.usbh.ohci_param,

#if VSF_USBH_USE_HUB == ENABLED
        .hub.drv                = &vk_usbh_hub_drv,
#endif
#if VSF_USBH_USE_ECM == ENABLED
        .ecm.drv                = &vk_usbh_ecm_drv,
#endif
#if VSF_USBH_USE_BTHCI == ENABLED
        .bthci.drv              = &vk_usbh_bthci_drv,
#endif
#if VSF_USBH_USE_HID == ENABLED
        .hid.drv                = &vk_usbh_hid_drv,
#endif
#if VSF_USBH_USE_DS4 == ENABLED
        .ds4.drv                = &vk_usbh_ds4_drv,
#endif
    },
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void usbh_demo_start(void)
{
    vk_ohci_init();
    vk_usbh_init(&usbh_demo.usbh.host);
#if VSF_USBH_USE_HUB == ENABLED
    vk_usbh_register_class(&usbh_demo.usbh.host, &usbh_demo.usbh.hub);
#endif
#if VSF_USBH_USE_ECM == ENABLED
    vk_usbh_register_class(&usbh_demo.usbh.host, &usbh_demo.usbh.ecm);
#endif
#if VSF_USBH_USE_BTHCI == ENABLED
    vk_usbh_register_class(&usbh_demo.usbh.host, &usbh_demo.usbh.bthci);
#endif
#if VSF_USBH_USE_HID == ENABLED
    vk_usbh_register_class(&usbh_demo.usbh.host, &usbh_demo.usbh.hid);
#endif
#if VSF_USBH_USE_DS4 == ENABLED
    vk_usbh_register_class(&usbh_demo.usbh.host, &usbh_demo.usbh.ds4);
#endif
}
