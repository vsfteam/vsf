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

#ifndef __USRAPP_COMMON_H__
#define __USRAPP_COMMON_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"

#if VSF_USE_USB_HOST_HCD_LIBUSB == ENABLED
#   include "component/usb/driver/hcd/libusb_hcd/vsf_libusb_hcd.h"
#elif VSF_USE_USB_HOST_HCD_WINUSB == ENABLED
#   include "component/usb/driver/hcd/winusb_hcd/vsf_winusb_hcd.h"
#endif

#if VSF_USE_UI == ENABLED && VSF_USE_UI_LVGL == ENABLED
#   include "lvgl/lvgl.h"
#   include "lv_conf.h"
#   include "component/3rd-party/littlevgl/6.0/port/vsf_lvgl_port.h"
#endif

#if VSF_USE_MEMFS == ENABLED
#   include "./fakefat32.h"
#endif

/*============================ MACROS ========================================*/

#if VSF_USE_MEMFS == ENABLED
#   define USRAPP_CFG_MEMFS_ROOT    __fakefat32_root
#endif
#if VSF_USE_WINFS == ENABLED
#   define USRAPP_CFG_WINFS_ROOT    "winfs_root"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if VSF_USE_USB_HOST == ENABLED
struct usrapp_common_const_t {
#   if VSF_USE_USB_HOST == ENABLED
    struct {
#       if VSF_USE_USB_HOST_HCD_OHCI == ENABLED
        vk_ohci_param_t ohci_param;
#       elif VSF_USE_USB_HOST_HCD_LIBUSB == ENABLED
        vsf_libusb_hcd_param_t libusb_hcd_param;
#       elif VSF_USE_USB_HOST_HCD_WINUSB == ENABLED
        vsf_winusb_hcd_param_t winusb_hcd_param;
#       endif
    } usbh;
#   endif
};
typedef struct usrapp_common_const_t usrapp_common_const_t;
#endif

#if VSF_USE_USB_HOST == ENABLED || VSF_USE_FS == ENABLED || VSF_USE_UI == ENABLED
struct usrapp_common_t {
#   if VSF_USE_USB_HOST == ENABLED
    struct {
        vk_usbh_t host;
#       if VSF_USE_USB_HOST_HUB == ENABLED
        vk_usbh_class_t hub;
#       endif
#       if VSF_USE_USB_HOST_LIBUSB == ENABLED
        vk_usbh_class_t libusb;
#       endif
#       if VSF_USE_TCPIP == ENABLED && VSF_USE_USB_HOST_ECM == ENABLED
        vk_usbh_class_t ecm;
#       endif
#       if VSF_USE_USB_HOST_BTHCI == ENABLED
        vk_usbh_class_t bthci;
#       endif
#       if VSF_USE_USB_HOST_HID == ENABLED
        vk_usbh_class_t hid;
#       endif
#       if VSF_USE_USB_HOST_DS4 == ENABLED
        vk_usbh_class_t ds4;
#       endif
#       if VSF_USE_USB_HOST_NSPRO == ENABLED
        vk_usbh_class_t nspro;
#       endif
#       if VSF_USE_USB_HOST_XB360 == ENABLED
        vk_usbh_class_t xb360;
#       endif
#       if VSF_USE_USB_HOST_XB1 == ENABLED
        vk_usbh_class_t xb1;
#       endif
    } usbh;
#   endif

#   if VSF_USE_MAL == ENABLED && VSF_USE_FAKEFAT32_MAL == ENABLED
    struct {
#       if VSF_USE_FAKEFAT32_MAL == ENABLED
        vk_fakefat32_mal_t fakefat32;
#       endif
    } mal;
#   endif

#   if VSF_USE_FS == ENABLED
    struct {
#       if VSF_USE_MEMFS == ENABLED
        vk_memfs_info_t memfs_info;
#       endif
#       if VSF_USE_FATFS == ENABLED
#           if VSF_USE_FAKEFAT32_MAL == ENABLED
        struct {
            vk_mim_mal_t fat32_mal;
            implement_fatfs_info(512, 1);
        } fatfs_info_fakefat32;
#           endif
#       endif
#       if VSF_USE_WINFS == ENABLED
        vk_winfs_info_t winfs_info;
#       endif
    } fs;
#   endif

#   if VSF_USE_UI == ENABLED
    struct {
#       if VSF_USE_DISP_DRV_SDL2 == ENABLED
        vk_disp_sdl2_t disp;
#       endif

#       if VSF_USE_UI_LVGL == ENABLED
        struct {
            vk_input_notifier_t notifier;
            vk_touchscreen_evt_t ts_evt;
            lv_disp_buf_t disp_buf;
            lv_color_t color[LV_VER_RES_MAX][LV_HOR_RES_MAX];
        } lvgl;
#       endif
    } ui;
#   endif
};
typedef struct usrapp_common_t usrapp_common_t;
#endif

/*============================ GLOBAL VARIABLES ==============================*/

extern const usrapp_common_const_t usrapp_common_const;
extern usrapp_common_t usrapp_common;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __USRAPP_COMMON_H__
/* EOF */