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

#include "./usrapp_common.h"

#if VSF_USE_USB_HOST_HCD_LIBUSB == ENABLED
#   include "component/usb/driver/hcd/libusb_hcd/vsf_libusb_hcd.h"
#elif VSF_USE_USB_HOST_HCD_WINUSB == ENABLED
#   include "component/usb/driver/hcd/winusb_hcd/vsf_winusb_hcd.h"
#endif

/*============================ MACROS ========================================*/

#if USRAPP_CFG_FAKEFAT32 == ENABLED
#   ifndef USRAPP_CFG_FAKEFAT32_SIZE
#       define USRAPP_CFG_FAKEFAT32_SIZE                (512 * 0x1040)
#   endif
#   if !defined(USRAPP_CFG_FAKEFAT32_ROOT) && defined(USRAPP_CFG_MEMFS_ROOT)
#       define USRAPP_CFG_FAKEFAT32_ROOT                USRAPP_CFG_MEMFS_ROOT
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_USE_USB_HOST == ENABLED
const usrapp_common_const_t usrapp_common_const = {
#   if VSF_USE_USB_HOST == ENABLED
    .usbh                   = {
#       if VSF_USE_USB_HOST_HCD_OHCI == ENABLED
        .ohci_param         = {
            .op             = &VSF_USB_HC0_IP,
            .priority       = vsf_arch_prio_0,
        },
#       elif VSF_USE_USB_HOST_HCD_LIBUSB == ENABLED
        .libusb_hcd_param = {
            .priority = vsf_arch_prio_0,
        },
#       elif VSF_USE_USB_HOST_HCD_WINUSB == ENABLED
        .winusb_hcd_param = {
            .priority = vsf_arch_prio_0,
        },
#       endif
    },
#   endif
};
#endif

#if VSF_USE_USB_HOST == ENABLED || VSF_USE_FS == ENABLED || VSF_USE_UI == ENABLED
usrapp_common_t usrapp_common = {
#   if VSF_USE_USB_HOST == ENABLED
    .usbh                   = {
#       if VSF_USE_USB_HOST_HCD_OHCI == ENABLED
        .host.drv           = &vk_ohci_drv,
        .host.param         = (void *)&__usrapp_common_const.usbh.ohci_param,
#       elif VSF_USE_USB_HOST_HCD_LIBUSB == ENABLED
        .host.drv           = &vsf_libusb_hcd_drv,
        .host.param         = (void*)&__usrapp_common_const.usbh.libusb_hcd_param,
#       elif VSF_USE_USB_HOST_HCD_WINUSB == ENABLED
        .host.drv           = &vsf_winusb_hcd_drv,
        .host.param         = (void*)&usrapp_common_const.usbh.winusb_hcd_param,
#       endif

#       if VSF_USE_USB_HOST_HUB == ENABLED
        .hub.drv            = &vk_usbh_hub_drv,
#       endif
#       if VSF_USE_TCPIP == ENABLED && VSF_USE_USB_HOST_ECM == ENABLED
        .ecm.drv            = &vk_usbh_ecm_drv,
#       endif
#       if VSF_USE_USB_HOST_LIBUSB == ENABLED
        .libusb.drv         = &vk_usbh_libusb_drv,
#       endif
#       if VSF_USE_USB_HOST_BTHCI == ENABLED
        .bthci.drv          = &vk_usbh_bthci_drv,
#       endif
#       if VSF_USE_USB_HOST_HID == ENABLED
        .hid.drv            = &vk_usbh_hid_drv,
#       endif
#       if VSF_USE_USB_HOST_DS4 == ENABLED
        .ds4.drv            = &vk_usbh_ds4_drv,
#       endif
#       if VSF_USE_USB_HOST_NSPRO == ENABLED
        .nspro.drv          = &vk_usbh_nspro_drv,
#       endif
#       if VSF_USE_USB_HOST_XB360 == ENABLED
        .xb360.drv          = &vk_usbh_xb360_drv,
#       endif
#       if VSF_USE_USB_HOST_XB1 == ENABLED
        .xb1.drv            = &vk_usbh_xb1_drv,
#       endif
    },
#   endif

#   if VSF_USE_MAL == ENABLED && VSF_USE_FAKEFAT32_MAL == ENABLED
    .mal                        = {
#       if VSF_USE_FAKEFAT32_MAL == ENABLED
        .fakefat32              = {
            .drv                = &VK_FAKEFAT32_MAL_DRV,
            .sector_size        = 512,
            .sector_number      = USRAPP_CFG_FAKEFAT32_SIZE / 512,
            .sectors_per_cluster= 8,
            .volume_id          = 0x12345678,
            .disk_id            = 0x9ABCEF01,
            .root               = {
                .name           = "ROOT",
                .d.child        = (vk_memfs_file_t *)USRAPP_CFG_FAKEFAT32_ROOT,
                .d.child_num    = dimof(USRAPP_CFG_FAKEFAT32_ROOT),
            },
        },
#       endif
    },
#   endif

#   if VSF_USE_FS == ENABLED
    .fs                         = {
#       if VSF_USE_MEMFS == ENABLED
        .memfs_info             = {
            .root               = {
                .d.child        = (vk_memfs_file_t *)USRAPP_CFG_MEMFS_ROOT,
                .d.child_num    = dimof(USRAPP_CFG_MEMFS_ROOT),
                .d.child_size   = sizeof(vk_fakefat32_file_t),
            },
        },
#       endif
#       if VSF_USE_WINFS == ENABLED
        .winfs_info             = {
            .root               = {
                .name           = USRAPP_CFG_WINFS_ROOT,
            },
        },
#       endif
#       if VSF_USE_FATFS == ENABLED
#           if VSF_USE_FAKEFAT32_MAL == ENABLED
        .fatfs_info_fakefat32   = {
            .fat32_mal          = {
                .drv            = &VK_MIM_MAL_DRV,
                .host_mal       = &usrapp_common.mal.fakefat32.use_as__vk_mal_t,
                .offset         = 0x40 * 512,
                .size           = (USRAPP_CFG_FAKEFAT32_SIZE - 0x40) * 512,
            },
            .mal                = &usrapp_common.fs.fatfs_info_fakefat32.fat32_mal.use_as__vk_mal_t,
            init_fatfs_info(usrapp_common.fs.fatfs_info_fakefat32, 512, 1)
        },
#           endif
#       endif
    },
#   endif

#   if VSF_USE_UI == ENABLED
    .ui                         = {
#       if VSF_USE_DISP_DRV_SDL2 == ENABLED
        .disp                   = {
            .param              = {
                .height         = APP_DISP_SDL2_HEIGHT,
                .width          = APP_DISP_SDL2_WIDTH,
                .drv            = &vk_disp_drv_sdl2,
                .color          = APP_DISP_SDL2_COLOR,
            },
            .amplifier          = APP_DISP_SDL2_AMPLIFIER,
        },
#       endif
    },
#   endif
};
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/* EOF */