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

#if VSF_USE_USB_HOST_HCD_LIBUSB == ENABLED
#   include "component/usb/driver/hcd/libusb_hcd/vsf_libusb_hcd.h"
#elif VSF_USE_USB_HOST_HCD_WINUSB == ENABLED
#   include "component/usb/driver/hcd/winusb_hcd/vsf_winusb_hcd.h"
#endif

#define VSF_LINUX_INHERIT
#include "shell/sys/linux/vsf_linux.h"
#include "shell/sys/linux/port/busybox/busybox.h"
#include <libusb.h>
#include <sys/mount.h>

#include "fakefat32.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct usrapp_const_t {
#if VSF_USE_USB_HOST == ENABLED
    struct {
#   if VSF_USE_USB_HOST_HCD_OHCI == ENABLED
        vk_ohci_param_t ohci_param;
#   elif VSF_USE_USB_HOST_HCD_LIBUSB == ENABLED
        vsf_libusb_hcd_param_t libusb_hcd_param;
#   elif VSF_USE_USB_HOST_HCD_WINUSB == ENABLED
        vsf_winusb_hcd_param_t winusb_hcd_param;
#   endif
    } usbh;
#endif
};
typedef struct usrapp_const_t usrapp_const_t;

struct usrapp_t {
#if VSF_USE_USB_HOST == ENABLED
    struct {
        vk_usbh_t host;
#   if VSF_USE_USB_HOST_LIBUSB == ENABLED
        vk_usbh_class_t libusb;
#   endif
#   if VSF_USE_TCPIP == ENABLED && VSF_USE_USB_HOST_ECM == ENABLED
        vk_usbh_class_t ecm;
#   endif
    } usbh;
#endif

    struct {
        vk_memfs_info_t memfs_info;
#if VSF_USE_WINFS == ENABLED
        vk_winfs_info_t winfs_info;
#endif
    } fs;
};
typedef struct usrapp_t usrapp_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const usrapp_const_t __usrapp_const = {
#if VSF_USE_USB_HOST == ENABLED
    .usbh                   = {
#   if VSF_USE_USB_HOST_HCD_OHCI == ENABLED
        .ohci_param         = {
            .op             = &VSF_USB_HC0_IP,
            .priority       = vsf_arch_prio_0,
        },
#   elif VSF_USE_USB_HOST_HCD_LIBUSB == ENABLED
        .libusb_hcd_param = {
            .priority = vsf_arch_prio_0,
        },
#   elif VSF_USE_USB_HOST_HCD_WINUSB == ENABLED
        .winusb_hcd_param = {
            .priority = vsf_arch_prio_0,
        },
#   endif
    },
#endif
};

static usrapp_t __usrapp = {
#if VSF_USE_USB_HOST == ENABLED
    .usbh                   = {
#   if VSF_USE_USB_HOST_HCD_OHCI == ENABLED
        .host.drv           = &vk_ohci_drv,
        .host.param         = (void *)&usrapp_const.usbh.ohci_param,
#   elif VSF_USE_USB_HOST_HCD_LIBUSB == ENABLED
        .host.drv           = &vsf_libusb_hcd_drv,
        .host.param         = (void*)&__usrapp_const.usbh.libusb_hcd_param,
#   elif VSF_USE_USB_HOST_HCD_WINUSB == ENABLED
        .host.drv           = &vsf_winusb_hcd_drv,
        .host.param         = (void*)&__usrapp_const.usbh.winusb_hcd_param,
#   endif

#   if VSF_USE_TCPIP == ENABLED && VSF_USE_USB_HOST_ECM == ENABLED
        .ecm.drv            = &vk_usbh_ecm_drv,
#   endif
#   if VSF_USE_USB_HOST_LIBUSB == ENABLED
        .libusb.drv         = &vk_usbh_libusb_drv,
#   endif
    },
#endif
    .fs.memfs_info          = {
        .root               = {
            .d.child        = (vk_memfs_file_t *)__fakefat32_root,
            .d.child_num    = dimof(__fakefat32_root),
            .d.child_size   = sizeof(vk_fakefat32_file_t),
        },
    },
#if VSF_USE_WINFS == ENABLED
    .fs.winfs_info          = {
        .root               = {
            .name           = "winfs_root",
        },
    },
#endif
};

/*============================ PROTOTYPES ====================================*/

extern int lsusb_main(int argc, char *argv[]);

/*============================ IMPLEMENTATION ================================*/

int vsf_linux_create_fhs(void)
{
    busybox_install();

    int fd = creat("/sbin/lsusb", 0);
    if (fd >= 0) {
        vsf_linux_fs_bind_executable(fd, lsusb_main);
        close(fd);
    }

    if (mkdir("/fakefat32", 0)) {
        return -1;
    }
    fd = open("/fakefat32", 0);
    if (fd >= 0) {
        close(fd);
        mount(NULL, "/fakefat32", &vk_memfs_op, 0, &__usrapp.fs.memfs_info);
    }

    if (mkdir("/winfs", 0)) {
        return -1;
    }
    fd = open("/winfs", 0);
    if (fd >= 0) {
        close(fd);
        mount(NULL, "/winfs", &vk_winfs_op, 0, &__usrapp.fs.winfs_info);
    }

    libusb_init(NULL);
    return 0;
}

int main(void)
{
#if VSF_USE_TRACE == ENABLED
    vsf_trace_init(NULL);
#endif

#if VSF_USE_USB_HOST == ENABLED
    vk_usbh_init(&__usrapp.usbh.host);
#   if VSF_USE_TCPIP == ENABLED && VSF_USE_USB_HOST_ECM == ENABLED
    vk_usbh_register_class(&usrapp.usbh.host, &usrapp.ecm);
#   endif
#   if VSF_USE_USB_HOST_LIBUSB == ENABLED
    vk_usbh_register_class(&__usrapp.usbh.host, &__usrapp.usbh.libusb);
#   endif
#endif

    vsf_trace(VSF_TRACE_INFO, "start linux..." VSF_TRACE_CFG_LINEEND);
    vk_fs_init();

    vsf_linux_stdio_stream_t stream = {
        .in     = (vsf_stream_t *)&VSF_DEBUG_STREAM_RX,
        .out    = (vsf_stream_t *)&VSF_DEBUG_STREAM_TX,
        .err    = (vsf_stream_t *)&VSF_DEBUG_STREAM_TX,
    };
    vsf_linux_init(&stream);
    return 0;
}

/* EOF */
