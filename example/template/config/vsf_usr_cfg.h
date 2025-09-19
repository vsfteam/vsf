/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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


//! \note User Level Application Configuration

#ifndef __VSF_USR_CFG_H__
#define __VSF_USR_CFG_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#if     defined(__M484__)
#   include "./vsf_usr_cfg/vsf_usr_cfg_m484.h"
#elif   defined(__F1C100S__)
#   include "./vsf_usr_cfg/vsf_usr_cfg_f1c100s.h"
#elif   defined(__WIN__)
#   include "./vsf_usr_cfg/vsf_usr_cfg_win.h"
#elif   defined(__LINUX__)
#   include "./vsf_usr_cfg/vsf_usr_cfg_linux.h"
#elif   defined(__MACOS__)
#   include "./vsf_usr_cfg/vsf_usr_cfg_macos.h"
#elif   defined(__GD32E103__)
#   include "./vsf_usr_cfg/vsf_usr_cfg_gd32e103.h"
#elif   defined(__MT071__)
#   include "./vsf_usr_cfg/vsf_usr_cfg_mt071.h"
#elif   defined(__MPS2__)
#   include "./vsf_usr_cfg/vsf_usr_cfg_mps2.h"
#elif   defined(__ESP32__)
#   include "./vsf_usr_cfg/vsf_usr_cfg_esp32.h"
#elif   defined(__ESP32S2__)
#   include "./vsf_usr_cfg/vsf_usr_cfg_esp32s2.h"
#elif   defined(__AIC8800__)
#   include "./vsf_usr_cfg/vsf_usr_cfg_aic8800.h"
#elif   defined(__CMEM7__)
#   include "./vsf_usr_cfg/vsf_usr_cfg_cmem7.h"
#elif   defined(__STM32H743XI__)
#   include "./vsf_usr_cfg/vsf_usr_cfg_stm32h743xi.h"
#elif   defined(__FAKE_DEVICE__)
#   include "./vsf_usr_cfg/vsf_usr_cfg_fake_device.h"
#endif

// kernel configurations
#define VSF_KERNEL_CFG_EDA_SUPPORT_TASK                 ENABLED
#define VSF_KERNEL_CFG_EDA_SUPPORT_PT                   ENABLED
#define VSF_KERNEL_CFG_EDA_SUPPORT_SIMPLE_FSM           ENABLED

// software independent components, if not used, compiler will optimize
#define VSF_USE_FIFO                                    ENABLED
#define VSF_USE_JSON                                    ENABLED
#define VSF_USE_DISTBUS                                 ENABLED
#   define VSF_DISTBUS_CFG_DEBUG                        ENABLED
#define VSF_USE_HASH                                    ENABLED
#define VSF_HASH_USE_CRC                                ENABLED

#if APP_USE_USBH_DEMO == ENABLED
#   define VSF_USE_USB_HOST                             ENABLED
#       define VSF_USBH_CFG_ISO_EN                      ENABLED
#       define VSF_USBH_USE_LIBUSB                      ENABLED
#       define VSF_USBH_USE_HID                         ENABLED
#       define VSF_USBH_USE_DS4                         ENABLED
#       define VSF_USBH_USE_NSPRO                       ENABLED
#       define VSF_USBH_USE_XB360                       ENABLED
#       define VSF_USBH_USE_XB1                         ENABLED
#   if APP_USE_SCSI_DEMO == ENABLED
#       define VSF_USBH_USE_MSC                         ENABLED
#   endif
#       define VSF_USBH_USE_UAC                         ENABLED
#   if APP_USE_VSFIP_DEMO == ENABLED || APP_USE_LWIP_DEMO == ENABLED
#       define VSF_USBH_USE_ECM                         ENABLED
#   endif
#   if APP_USE_DFU_HOST_DEMO == ENABLED
#       define VSF_USBH_USE_DFU                         ENABLED
#   endif
#endif

#if APP_USE_BTSTACK_DEMO == ENABLED
#   if !defined(VSF_USBH_USE_BTHCI) && APP_USE_USBH_DEMO == ENABLED
#      define VSF_USBH_USE_BTHCI                        ENABLED
#   endif
#   define VSF_USE_BTSTACK                              ENABLED
#       define BTSTACK_OO_USE_DEVICE                    DISABLED
#       define BTSTACK_OO_USE_HOST                      ENABLED
#           define BTSTACK_OO_USE_HOST_HID              ENABLED
#           define BTSTACK_OO_USE_HOST_JOYCON           ENABLED
#           define BTSTACK_OO_USE_HOST_DS4              ENABLED
#endif

#if APP_USE_SDL2_DEMO == ENABLED
#   define VSF_USE_SDL2                                 ENABLED
#endif

#if    APP_USE_LVGL_DEMO == ENABLED ||  APP_USE_LVGL_TERMINAL_DEMO == ENABLED   \
    || APP_USE_XBOOT_XUI_DEMO == ENABLED ||  APP_USE_TGUI_DEMO == ENABLED       \
    || APP_USE_SDL2_DEMO == ENABLED  ||  APP_USE_LLGUI_DEMO == ENABLED          \
    || APP_USE_GUILITE_DEMO == ENABLED  ||  APP_USE_DISP_DEMO == ENABLED
#   define VSF_USE_UI                                   ENABLED
#endif
#if APP_USE_LLGUI_DEMO == ENABLED
#   define VSF_USE_LLGUI                                ENABLED
#endif
#if (APP_USE_LVGL_DEMO == ENABLED) || (APP_USE_LVGL_TERMINAL_DEMO == ENABLED)
#   define VSF_USE_LVGL                                 ENABLED
#endif
#if APP_USE_GATO_DEMO == ENABLED
#   define VSF_USE_GATO                                 ENABLED
#endif
#if APP_USE_XBOOT_XUI_DEMO == ENABLED
#   define VSF_USE_XBOOT                                ENABLED
#endif
#if APP_USE_VSFVM_DEMO == ENABLED
#   define VSFVM_LEXER_DEBUG_EN                         DISABLED
#   define VSFVM_PARSER_DEBUG_EN                        DISABLED
#   define VSFVM_COMPILER_DEBUG_EN                      DISABLED
#endif

#if APP_USE_VSFIP_DEMO == ENABLED || APP_USE_LWIP_DEMO == ENABLED
#   define VSF_USE_TCPIP                                ENABLED
#endif
#if APP_USE_VSFIP_DEMO == ENABLED
#   define VSF_USE_VSFIP                                ENABLED
#   define VSF_USE_LWIP                                 DISABLED
#elif APP_USE_LWIP_DEMO == ENABLED
#   define VSF_USE_VSFIP                                DISABLED
#   define VSF_USE_LWIP                                 ENABLED
#endif

#if VSF_USE_LINUX == ENABLED
#   define VSF_USE_POSIX                                ENABLED
#   define VSF_LINUX_USE_GETOPT                         ENABLED
#   define VSF_LINUX_USE_TERMIOS                        ENABLED
#   if APP_USE_LINUX_DYNLOADER_DEMO == ENABLED
#       define VSF_USE_APPLET                           ENABLED
#       define VSF_LINUX_USE_APPLET                     ENABLED
#       define VSF_USE_LOADER                           ENABLED
#           define VSF_LOADER_USE_ELF                   ENABLED
//#           define VSF_ELFLOADER_CFG_DEBUG              ENABLED
#       ifdef __WIN__
#           define VSF_LOADER_USE_PE                    ENABLED
#       endif
#   endif

#   if APP_USE_FREETYPE_DEMO == ENABLED
#       define VSF_LINUX_FREETYPE_INIT_SCRIPTS                                  \
            "freetype",
#   else
#       define VSF_LINUX_FREETYPE_INIT_SCRIPTS
#   endif

#   if      defined(__WIN__)
#   define VSF_LINUX_HOSTFS_INIT_SCRIPTS                                        \
            "mkdir -p /mnt/hostfs",                                             \
            "mount -t winfs . /mnt/hostfs",                                     \
            "cd /mnt/hostfs",
#   elif    defined(__LINUX__)
#   define VSF_LINUX_HOSTFS_INIT_SCRIPTS                                        \
            "mkdir -p /mnt/hostfs",                                             \
            "mount -t linfs . /mnt/hostfs",                                     \
            "cd /mnt/hostfs",
#   else
#   define VSF_LINUX_HOSTFS_INIT_SCRIPTS
#   endif

#   define VSF_LINUX_CFG_INIT_SCRIPTS                                           \
            "echo \"vsf build on " __DATE__ "\"",                               \
            VSF_LINUX_HOSTFS_INIT_SCRIPTS                                       \
            VSF_LINUX_FREETYPE_INIT_SCRIPTS
#endif

#if APP_USE_EVM_DEMO == ENABLED
#   define VSF_USE_EVM                                  ENABLED
#   if !defined(VSF_EVM_USE_USBH) && VSF_USE_USB_HOST == ENABLED
#       define VSF_EVM_USE_USBH                         ENABLED
#   endif
#   if !defined(VSF_EVM_USE_BLUETOOTH) && VSF_USE_BTSTACK == ENABLED
#       define VSF_EVM_USE_BLUETOOTH                    ENABLED
#   endif
#endif

// lv2 hal
#if APP_USE_STREAM_HAL_DEMO == ENABLED
#   define VSF_USE_STREAM_HAL                           ENABLED
#   if APP_USE_STREAM_USART_DEMO == ENABLED
#       define VSF_USE_STREAM_USART                     ENABLED
#   endif
#endif

#if APP_USE_LUA_DEMO == ENABLED
#   define VSF_USE_LUA                                  ENABLED
#       define VSF_LUA_USE_LOVE                         ENABLED
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ INCLUDES ======================================*/

#if APP_USE_TGUI_DEMO == ENABLED
#   include "vsf_tgui_cfg.h"
#endif

#endif
/* EOF */
