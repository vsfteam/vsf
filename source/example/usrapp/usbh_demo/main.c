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
#include <stdio.h>
#include <stdarg.h>

/*============================ MACROS ========================================*/

#if VSF_USE_USB_HOST != ENABLED
#   error VSF_USE_USB_HOST MUST be enabled for usbh_demo
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct usrapp_const_t {
    struct {
#if VSF_USBH_USE_HCD_OHCI == ENABLED
        vk_ohci_param_t ohci_param;
#elif VSF_USBH_USE_HCD_LIBUSB == ENABLED
        vsf_libusb_hcd_param_t libusb_hcd_param;
#endif
    } usbh;
};
typedef struct usrapp_const_t usrapp_const_t;

struct usrapp_t {
    struct {
        vk_usbh_t host;
#if VSF_USBH_USE_HUB == ENABLED
        vk_usbh_class_t hub;
#endif
#if VSF_USBH_USE_LIBUSB == ENABLED
        vk_usbh_class_t libusb;
        uint8_t dev_count;
#endif
#if VSF_USBH_USE_HID == ENABLED
        vk_usbh_class_t hid;
#endif
#if VSF_USBH_USE_DS4 == ENABLED
        vk_usbh_class_t ds4;
#endif
#if VSF_USBH_USE_NSPRO == ENABLED
        vk_usbh_class_t nspro;
#endif
#if VSF_USBH_USE_XB360 == ENABLED
        vk_usbh_class_t xb360;
#endif
#if VSF_USBH_USE_XB1 == ENABLED
        vk_usbh_class_t xb1;
#endif
#if VSF_USBH_USE_ECM == ENABLED
        vk_usbh_class_t ecm;
/*
        struct {
            bool inited;
            vsfip_netif_t netif;
            union {
                vsfip_dhcpc_t dhcpc;
                vsfip_dhcpd_t dhcpd;
            };
        } tcpip;
*/
#endif
    } usbh;
};
typedef struct usrapp_t usrapp_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const usrapp_const_t usrapp_const = {
#if VSF_USBH_USE_HCD_OHCI == ENABLED
    .usbh.ohci_param        = {
        .op                 = &VSF_USB_HC0_IP,
        .priority           = vsf_arch_prio_0,
    },
#elif VSF_USBH_USE_HCD_LIBUSB == ENABLED
    .usbh.libusb_hcd_param = {
        .priority = vsf_arch_prio_0,
    },
#endif
};

static usrapp_t usrapp = {
    .usbh                   = {
#if VSF_USBH_USE_HCD_OHCI == ENABLED
        .host.drv           = &vk_ohci_drv,
        .host.param         = (void *)&usrapp_const.usbh.ohci_param,
#elif VSF_USBH_USE_HCD_LIBUSB == ENABLED
        .host.drv           = &vsf_libusb_hcd_drv,
        .host.param         = (void*)&usrapp_const.usbh.libusb_hcd_param,
#endif

#if VSF_USBH_USE_HUB == ENABLED
        .hub.drv            = &vk_usbh_hub_drv,
#endif
#if VSF_USBH_USE_HID == ENABLED
        .hid.drv            = &vk_usbh_hid_drv,
#endif
#if VSF_USBH_USE_DS4 == ENABLED
        .ds4.drv            = &vk_usbh_ds4_drv,
#endif
#if VSF_USBH_USE_NSPRO == ENABLED
        .nspro.drv          = &vk_usbh_nspro_drv,
#endif
#if VSF_USBH_USE_XB360 == ENABLED
        .xb360.drv          = &vk_usbh_xb360_drv,
#endif
#if VSF_USBH_USE_XB1 == ENABLED
        .xb1.drv            = &vk_usbh_xb1_drv,
#endif
#if VSF_USBH_USE_LIBUSB == ENABLED
        .libusb.drv         = &vk_usbh_libusb_drv,
#endif
#if VSF_USBH_USE_ECM == ENABLED
        .ecm.drv            = &vsff_usbh_ecm_drv,
#endif
    },
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_USBH_USE_LIBUSB == ENABLED
static void app_on_libusb_event(void *param,
                                vk_usbh_libusb_dev_t *dev,
                                vk_usbh_libusb_evt_t evt)
{
    usrapp_t *app = (usrapp_t *)param;
    switch (evt) {
    case VSF_USBH_LIBUSB_EVT_ON_ARRIVED:
        app->usbh.dev_count++;
        break;
    case VSF_USBH_LIBUSB_EVT_ON_LEFT:
        app->usbh.dev_count--;
        break;
    }
}
#endif

int main(void)
{
#if VSF_USE_TRACE == ENABLED
    vsf_trace_init((vsf_stream_t *)&VSF_DEBUG_STREAM_TX);
#   if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#   endif
    vsf_trace(VSF_TRACE_INFO, "demo started..." VSF_TRACE_CFG_LINEEND);
#endif

#if VSF_USBH_USE_HCD_OHCI == ENABLED
    vk_ohci_init();
#endif
    vk_usbh_init(&usrapp.usbh.host);
#if VSF_USBH_USE_LIBUSB == ENABLED
    vk_usbh_libusb_set_evthandler(&usrapp, app_on_libusb_event);
    vk_usbh_register_class(&usrapp.usbh.host, &usrapp.usbh.libusb);
#endif
#if VSF_USBH_USE_ECM == ENABLED
    vk_usbh_register_class(&usrapp.usbh.host, &usrapp.usbh.ecm);
#endif
#if VSF_USBH_USE_HID == ENABLED
    vk_usbh_register_class(&usrapp.usbh.host, &usrapp.usbh.hid);
#endif
#if VSF_USBH_USE_DS4 == ENABLED
    vk_usbh_register_class(&usrapp.usbh.host, &usrapp.usbh.ds4);
#endif
#if VSF_USBH_USE_NSPRO == ENABLED
    vk_usbh_register_class(&usrapp.usbh.host, &usrapp.usbh.nspro);
#endif
#if VSF_USBH_USE_XB360 == ENABLED
    vk_usbh_register_class(&usrapp.usbh.host, &usrapp.usbh.xb360);
#endif
#if VSF_USBH_USE_XB1 == ENABLED
    vk_usbh_register_class(&usrapp.usbh.host, &usrapp.usbh.xb1);
#endif
#if VSF_USBH_USE_HUB == ENABLED
    vk_usbh_register_class(&usrapp.usbh.host, &usrapp.usbh.hub);
#endif
    return 0;
}

/* EOF */
