/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless requir by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "vsf.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USE_USB_HOST_HCD_LIBUSB == ENABLED
#   include "component/usb/driver/hcd/libusb_hcd/vsf_libusb_hcd.h"
#endif

#include "fakefat32.h"

/*============================ MACROS ========================================*/

#ifndef USRAPP_CFG_FAT32_SIZE
#   define USRAPP_CFG_FAT32_SIZE                (512 * 0x10000)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if VSF_USE_USB_HOST == ENABLED
struct usrapp_const_t {
#   if VSF_USE_USB_HOST == ENABLED
    struct {
#       if VSF_USE_USB_HOST_HCD_OHCI == ENABLED
        vsf_ohci_param_t ohci_param;
#       elif VSF_USE_USB_HOST_HCD_LIBUSB == ENABLED
        vsf_libusb_hcd_param_t libusb_hcd_param;
#       endif
    } usbh;
#   endif
};
typedef struct usrapp_const_t usrapp_const_t;
#endif

struct usrapp_t {
#if VSF_USE_USB_HOST == ENABLED
    struct {
        vsf_usbh_t host;
        vsf_usbh_class_t msc;
    } usbh;
#endif

    struct {
        vsf_fakefat32_mal_t fakefat32;
    } mal;

    struct {
        uint8_t buffer[1024];
        vsf_mem_stream_t mem_stream;
    } stream;
};
typedef struct usrapp_t usrapp_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

#if VSF_USE_USB_HOST == ENABLED
static const usrapp_const_t usrapp_const = {
#   if VSF_USE_USB_HOST == ENABLED
#       if VSF_USE_USB_HOST_HCD_OHCI == ENABLED
    .usbh.ohci_param        = {
        .op                 = &VSF_USB_HC0_IP,
        .priority           = vsf_arch_prio_0,
    },
#       elif VSF_USE_USB_HOST_HCD_LIBUSB == ENABLED
    .usbh.libusb_hcd_param  = {
        .priority = vsf_arch_prio_0,
    },
#       endif
#   endif
};
#endif

static usrapp_t usrapp = {
#if VSF_USE_USB_HOST == ENABLED
    .usbh                       = {
#   if VSF_USE_USB_HOST_HCD_OHCI == ENABLED
        .host.drv           = &vsf_ohci_drv,
        .host.param         = (void *)&usrapp_const.usbh.ohci_param,
#   elif VSF_USE_USB_HOST_HCD_LIBUSB == ENABLED
        .host.drv           = &vsf_libusb_hcd_drv,
        .host.param         = (void*)&usrapp_const.usbh.libusb_hcd_param,
#   endif
//        .msc.drv            = &vsf_usbh_msc_drv,
    },
#endif

    .mal                    = {
        .fakefat32          = {
            .drv                = &vsf_fakefat32_mal_drv,
            .sector_size        = 512,
            .sector_number      = USRAPP_CFG_FAT32_SIZE / 512,
            .sectors_per_cluster= 8,
            .volume_id          = 0x12345678,
            .disk_id            = 0x9ABCEF01,
            .root               = {
                .name           = "ROOT",
                .d.child        = (vsf_memfs_file_t *)fakefat32_root,
                .d.child_num    = dimof(fakefat32_root),
            },
        },
    },
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void vsf_input_on_touchscreen(vsf_touchscreen_evt_t *ts_evt)
{
}

// TODO: SDL require that main need argc and argv
int main(int argc, char *argv[])
{
#if VSF_USE_TRACE == ENABLED
    vsf_trace_init(NULL);
    vsf_stdio_init();
#endif

#if VSF_USE_USB_HOST == ENABLED
    vsf_usbh_init(&usrapp.usbh.host);
//    vsf_usbh_register_class(&usrapp.usbh.host, &usrapp.usbh.msc);
#endif
    return 0;
}

/* EOF */
