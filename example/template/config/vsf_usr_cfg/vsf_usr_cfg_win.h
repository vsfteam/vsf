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

#ifndef __VSF_USR_CFG_WIN_H__
#define __VSF_USR_CFG_WIN_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

// enable the 4 configurations below to use simple implementation for x86
//  simple implementation will not use ThreadSuspend and ThreadResume and has better CPU usage
//  **** but preempt is not supported ****
//  priority configurations are dependent on MACROs below, so put them here(at top)
//  IMPORTANT: vsf_arch_sleep MUST be called in vsf_plug_in_on_kernel_idle
//#define VSF_ARCH_PRI_NUM                                1
//#define VSF_ARCH_SWI_NUM                                0
//#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                     ENABLED
//#define __VSF_X86_WIN_SINGLE_PRIORITY

// cpu usage is not supported in windows/linux/macos
#define VSF_KERNEL_CFG_CPU_USAGE                        DISABLED

//#define __WIN7__

// Application configure
#define APP_USE_LINUX_DEMO                              ENABLED
#   define APP_USE_LINUX_LIBUSB_DEMO                    ENABLED
#   define APP_USE_LINUX_MOUNT_DEMO                     ENABLED
#   define APP_USE_LINUX_HTTPD_DEMO                     ENABLED
#       define APP_LINUX_HTTPD_DEMO_CFG_ROOT            "/winfs/webroot"
#   define APP_USE_LINUX_NTPDATE_DEMO                   ENABLED
#   define APP_USE_LINUX_TELNETD_DEMO                   ENABLED
#define APP_USE_USBH_DEMO                               ENABLED
#   define APP_USE_DFU_HOST_DEMO                        ENABLED
#define APP_USE_USBD_DEMO                               ENABLED
#   define APP_USE_USBD_CDC_DEMO                        ENABLED
#   define APP_USE_USBD_MSC_DEMO                        ENABLED
#   define APP_USE_USBD_UVC_DEMO                        ENABLED
#   define APP_USE_USBD_USER_DEMO                       ENABLED
#define APP_USE_SCSI_DEMO                               ENABLED
#define APP_USE_AUDIO_DEMO                              ENABLED
#   define APP_CFG_AUDIO_BUFFER_SIZE                    (VSF_LINUX_CFG_STACKSIZE - (4 * 1024))
#define APP_USE_TGUI_DEMO                               DISABLED
#define APP_USE_TGUI_DESIGNER_DEMO                      DISABLED
#define APP_USE_SDL2_DEMO                               ENABLED
#define APP_USE_DISP_DEMO                               ENABLED
#define APP_USE_KERNEL_TEST                             ENABLED
#define APP_USE_JSON_DEMO                               ENABLED
#define APP_USE_HAL_DEMO                                ENABLED
#   define APP_USE_HAL_USART_DEMO                       ENABLED
#define APP_USE_STREAM_HAL_DEMO                         ENABLED
#   define APP_USE_STREAM_USART_DEMO                    ENABLED
//#define APP_USE_DISTBUS_DEMO                            ENABLED
//#   define APP_DISTBUS_DEMO_CFG_LWIP                    ENABLED
//#   define APP_USE_DISTBUS_HAL_DEMO                     ENABLED
//#       define APP_USE_DISTBUS_HAL_USBD_DEMO            ENABLED

#define APP_USE_CPP_DEMO                                ENABLED
#define VSF_LINUX_USE_SIMPLE_LIBC                       ENABLED
#   define VSF_LINUX_CFG_PRINT_BUFF_SIZE                4096
// if VSF_LINUX_USE_SIMPLE_LIBC is enabled, need VSF_USE_SIMPLE_SSCANF and VSF_USE_SIMPLE_SPRINTF
#if VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   define VSF_LINUX_LIBC_CFG_CPP                       ENABLED
// VSF_LINUX_LIBC_CFG_WRAPPER is required on __WIN__ to avoid API confliction
#   define VSF_LINUX_LIBC_CFG_WRAPPER                   ENABLED
#   define VSF_USE_SIMPLE_SSCANF                        ENABLED
#   define VSF_USE_SIMPLE_SPRINTF                       ENABLED
#   define VSF_USE_SIMPLE_SWPRINTF                      ENABLED
#else
#   define VSF_USE_SIMPLE_SSCANF                        DISABLED
#   define VSF_USE_SIMPLE_SPRINTF                       DISABLED
#   define VSF_USE_SIMPLE_SWPRINTF                      DISABLED
#endif

// 3rd-party demos
// nnom minst demo seems to be broken
#define APP_USE_NNOM_DEMO                               DISABLED
#define APP_USE_LVGL_DEMO                               ENABLED
//#   define APP_LVGL_DEMO_USE_TERMINAL                   ENABLED
//#   define APP_LVGL_DEMO_CFG_ANIMINATION                ENABLED
// _vsnprintf of lvgl conflicts with ucrt
#   define LV_SPRINTF_CUSTOM                            1
#   define APP_LVGL_DEMO_USE_TOUCHSCREEN                ENABLED
#   define APP_LVGL_DEMO_CFG_TOUCH_REMAP                DISABLED
#   define APP_LVGL_DEMO_CFG_FREETYPE                   ENABLED
#   define APP_LVGL_DEMO_CFG_FREETYPE_MAX_FACES         4
#   define APP_LVGL_DEMO_CFG_COLOR_DEPTH                16
#   define APP_LVGL_DEMO_CFG_HOR_RES                    1920
#   define APP_LVGL_DEMO_CFG_VER_RES                    1080
#   define VSF_LVGL_IMP_WAIT_CB                         ENABLED
#define APP_USE_BTSTACK_DEMO                            ENABLED
#ifndef __VSF_X86_WIN_SINGLE_PRIORITY
// vsfvm will hold cpu, so other irq can not be run in single thread mode
#   define APP_USE_VSFVM_DEMO                              ENABLED
#endif
// select one for tcpip stack
#define APP_USE_VSFIP_DEMO                              DISABLED
#define APP_USE_LWIP_DEMO                               ENABLED
#define APP_USE_SOCKET_DEMO                             ENABLED
#define APP_USE_LUA_DEMO                                ENABLED
#define APP_USE_COREMARK_DEMO                           ENABLED

#if     APP_USE_TGUI_DEMO == ENABLED || APP_USE_XBOOT_XUI_DEMO == ENABLED       \
    ||  (APP_USE_LVGL_DEMO == ENABLED && APP_LVGL_DEMO_CFG_FREETYPE == ENABLED)
#   define APP_USE_FREETYPE_DEMO                        ENABLED
#endif

// component configure
#define VSF_USE_HEAP                                    ENABLED
#   define VSF_HEAP_CFG_MCB_MAGIC_EN                    ENABLED
#   define VSF_HEAP_SIZE                                0x2000000
#   define VSF_HEAP_CFG_MCB_ALIGN_BIT                   12      // 4K alignment

#define VSF_USE_VIDEO                                   ENABLED
#define VSF_USE_AUDIO                                   ENABLED
#   define VSF_AUDIO_USE_DECODER_WAV                    ENABLED
#   define VSF_AUDIO_USE_PLAYBACK                       ENABLED
#   define VSF_AUDIO_USE_CATURE                         DISABLED

// UI runs in vsf_prio_0, other modules runs above vsf_prio_1
#if     APP_USE_LVGL_DEMO == ENABLED || APP_USE_XBOOT_XUI_DEMO == ENABLED       \
    ||  APP_USE_TGUI_DEMO == ENABLED || APP_USE_GUILITE_DEMO == ENABLED
#   ifdef __VSF_X86_WIN_SINGLE_PRIORITY
#       define VSF_USBH_CFG_EDA_PRIORITY                vsf_prio_0
#       define APP_CFG_USBH_HW_PRIO                     vsf_arch_prio_0
#   else
#       define VSF_USBH_CFG_EDA_PRIORITY                vsf_prio_1
#       define APP_CFG_USBH_HW_PRIO                     vsf_arch_prio_1
#   endif
#endif

#define VSF_USE_INPUT                                   ENABLED
#   define VSF_INPUT_USE_HID                            ENABLED
#   define VSF_INPUT_USE_DS4                            ENABLED
#   define VSF_INPUT_USE_NSPRO                          ENABLED
#   define VSF_INPUT_USE_XB360                          ENABLED
#   define VSF_INPUT_USE_XB1                            ENABLED

// VSF_USE_USB_DEVICE will be enabled if target chip supports USBD
//#define VSF_USE_USB_DEVICE                              ENABLED
#   define VSF_USBD_CFG_USE_EDA                         ENABLED
#   define VSF_USBD_USE_CDCACM                          ENABLED
#   define VSF_USBD_USE_MSC                             ENABLED
#   define VSF_USBD_USE_UVC                             ENABLED
#   define VSF_USBD_USE_HID                             ENABLED
#   define APP_CFG_USBD_VID                             0xA7A8
#   define APP_CFG_USBD_PID                             0x2348

#define VSF_USE_MAL                                     ENABLED
#   define VSF_MAL_USE_MEM_MAL                          ENABLED
#   define VSF_MAL_USE_FAKEFAT32_MAL                    ENABLED
#   define VSF_MAL_USE_SCSI_MAL                         ENABLED
#   define VSF_MAL_USE_FILE_MAL                         ENABLED

#define VSF_USE_SCSI                                    ENABLED
#   define VSF_SCSI_USE_MAL_SCSI                        ENABLED

#define VSF_USE_FS                                      ENABLED
#   define VSF_FS_USE_MEMFS                             ENABLED
#   define VSF_FS_USE_FATFS                             ENABLED
#   define VSF_USE_LITTLEFS                             ENABLED
#       define VSF_FS_USE_LITTLEFS                      ENABLED

#define VSF_USE_TRACE                                   ENABLED
#define USRAPP_CFG_STDIO_EN                             ENABLED

#define VSF_USE_LINUX                                   ENABLED
#   define VSF_LINUX_CFG_WRAPPER                        ENABLED
#   define VSF_LINUX_SOCKET_CFG_WRAPPER                 ENABLED
#   define VSF_LINUX_USE_SOCKET                         ENABLED
#       define VSF_LINUX_SOCKET_USE_INET                VSF_USE_LWIP
#       define VSF_LINUX_SOCKET_USE_UNIX                ENABLED
#   define VSF_LINUX_USE_DEVFS                          ENABLED
#   define VSF_LINUX_USE_LIBUSB                         VSF_USE_USB_HOST
#   define VSF_LINUX_USE_BUSYBOX                        ENABLED
#   define VSF_LINUX_CFG_RTC                            vsf_hw_rtc0
#if APP_USE_EVM_DEMO == ENABLED
// evm console need printf and getchar
// TODO: add back
#endif

#define VSF_USE_MBEDTLS                                 ENABLED

#ifndef USRAPP_CFG_LINUX_TTY_DEBUT_STREAM
#   define USRAPP_CFG_LINUX_TTY_DEBUG_STREAM            0
#   define USRAPP_CFG_LINUX_TTY_UART                    1
#   define USRAPP_CFG_LINUX_TTY_CDC                     2
#endif
#   define USRAPP_CFG_LINUX_TTY                         USRAPP_CFG_LINUX_TTY_DEBUG_STREAM

#define VSF_USE_STREAM                                  DISABLED
#define VSF_USE_SIMPLE_STREAM                           ENABLED

#define USRAPP_CFG_FAKEFAT32                            ENABLED
#   define USRAPP_FAKEFAT32_CFG_FONT                    ENABLED


#if __IS_COMPILER_LLVM__
#   pragma clang diagnostic ignored "-Wbuiltin-requires-header"
#   pragma clang diagnostic ignored "-Wmicrosoft-include"
#   pragma clang diagnostic ignored "-Winconsistent-dllimport"
#   pragma clang diagnostic ignored "-Wimplicit-function-declaration"
#endif

#ifdef __CPU_X64__
#   error x64 is currently not supported
#endif

#define VSF_ASSERT(...)                                 assert(__VA_ARGS__)
//#define VSF_ASSERT(...)

#define VSF_HAL_USE_DEBUG_STREAM                        ENABLED

#define VSF_SYSTIMER_FREQ                               (0ul)

#define VSF_USE_USB_DEVICE                              ENABLED
#   if APP_USE_DISTBUS_DEMO == ENABLED && APP_USE_DISTBUS_HAL_MASTER_DEMO == ENABLED
#       define VSF_HAL_USE_DISTBUS                      ENABLED
#       if APP_USE_DISTBUS_HAL_USBD_MASTER_DEMO == ENABLED
#           define VSF_HAL_USE_DISTBUS_USBD             ENABLED
#       endif
#   else
#       define VSF_USBD_USE_DCD_USBIP                   ENABLED
#           define VSF_USBIP_SERVER_CFG_DEBUG           ENABLED
#           define VSF_USBIP_SERVER_CFG_DEBUG_TRAFFIC   DISABLED
#           define VSF_USBIP_SERVER_CFG_DEBUG_URB       ENABLED
#   endif
#   ifdef __VSF_X86_WIN_SINGLE_PRIORITY
#       define VSF_USBD_CFG_EDA_PRIORITY                vsf_prio_0
#       define VSF_USBD_CFG_HW_PRIORITY                 vsf_arch_prio_0
#   else
#       define VSF_USBD_CFG_EDA_PRIORITY                vsf_prio_1
#       define VSF_USBD_CFG_HW_PRIORITY                 vsf_arch_prio_1
#   endif
#   define USRAPP_CFG_USBD_DEV                          VSF_USB_DC0

#define VSF_USBH_CFG_ENABLE_ROOT_HUB                    DISABLED
#define VSF_USBH_USE_HUB                                DISABLED
#define VSF_USBH_USE_DL1X5                              ENABLED
#   define VSF_DISP_USE_DL1X5                           ENABLED
#ifdef __WIN7__
// winusb seems fail on win7
#   define VSF_USBH_USE_HCD_LIBUSB                      ENABLED
#       define VSF_LIBUSB_CFG_INSTALL_DRIVER            DISABLED
#else
#   define VSF_USBH_USE_HCD_WINUSB                      ENABLED
#       define VSF_WINUSB_CFG_INSTALL_DRIVER            DISABLED
#endif

#if VSF_USBH_USE_HCD_WINUSB == ENABLED
#   define VSF_WINUSB_HCD_CFG_DEV_NUM                   10
#   define VSF_WINUSB_HCD_DEV0_VID                      0x0A12      // CSR8510 bthci
#   define VSF_WINUSB_HCD_DEV0_PID                      0x0001
#   define VSF_WINUSB_HCD_DEV1_VID                      0x0A5C      // BCM20702 bthci
#   define VSF_WINUSB_HCD_DEV1_PID                      0x21E8
#   define VSF_WINUSB_HCD_DEV2_VID                      0x04F2      // UVC
#   define VSF_WINUSB_HCD_DEV2_PID                      0xB130
#   define VSF_WINUSB_HCD_DEV3_VID                      0x054C      // DS4
#   define VSF_WINUSB_HCD_DEV3_PID                      0x05C4
#   define VSF_WINUSB_HCD_DEV4_VID                      0x054C      // DS4
#   define VSF_WINUSB_HCD_DEV4_PID                      0x09CC
#   define VSF_WINUSB_HCD_DEV5_VID                      0x057E      // NSPRO
#   define VSF_WINUSB_HCD_DEV5_PID                      0x2009
#   define VSF_WINUSB_HCD_DEV6_VID                      0x0D8C      // uac
#   define VSF_WINUSB_HCD_DEV6_PID                      0x013C
//  to use ecm driver for rtl8152, wpcap MUST be disabled
//    rtl8152 is not supported in ecm mode, because winusb does not support set_configuration
#   define VSF_WINUSB_HCD_DEV7_VID                      0x0BDA      // rtl8152
#   define VSF_WINUSB_HCD_DEV7_PID                      0x8152
#   define VSF_WINUSB_HCD_DEV8_VID                      0x17E9      // dl165
#   define VSF_WINUSB_HCD_DEV8_PID                      0x019E
#   define VSF_WINUSB_HCD_DEV9_VID                      0x0EEF      // hid touch screen
#   define VSF_WINUSB_HCD_DEV9_PID                      0x0005
#endif
#if VSF_USBH_USE_HCD_LIBUSB == ENABLED
// for libusb
//#   define VSF_LIBUSB_CFG_INCLUDE                       "lib\libusb\libusb.h"
// for libusb-win32
#   define VSF_LIBUSB_CFG_INCLUDE                       "lib\libusb-win32\lusb0_usb.h"
#   define VSF_LIBUSB_HCD_CFG_DEV_NUM                   2
#   define VSF_LIBUSB_HCD_DEV0_VID                      0x0A12      // CSR8510 bthci
#   define VSF_LIBUSB_HCD_DEV0_PID                      0x0001
#   define VSF_LIBUSB_HCD_DEV1_VID                      0x0A5C      // BCM20702 bthci
#   define VSF_LIBUSB_HCD_DEV1_PID                      0x21E8
#endif

#define VSF_FS_USE_WINFS                                ENABLED

#if APP_USE_BTSTACK_DEMO == ENABLED
#   define HCI_RESET_RESEND_TIMEOUT_MS                  2000
#endif

#if APP_USE_TGUI_DEMO == ENABLED
#   if VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#       define APP_CFG_TGUI_RES_DIR                     "/winfs/ui/"
#   else
#       define APP_CFG_TGUI_RES_DIR                     "./winfs_root/ui/"
#   endif
#endif

#if APP_USE_XBOOT_XUI_DEMO == ENABLED
#   if VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#       define APP_CFG_XBOOT_RES_DIR                    "/winfs/ui/"
#   else
#       define APP_CFG_XBOOT_RES_DIR                    "./winfs_root/ui/"
#   endif
#endif

#if APP_USE_VSFIP_DEMO == ENABLED || APP_USE_LWIP_DEMO == ENABLED
#   define VSF_NETDRV_USE_WPCAP                         ENABLED
// VSF_NETDRV_WPCAP_CFG_HW_PRIORITY should be higher than TCPIP_THREAD_PRIO
#   ifdef __VSF_X86_WIN_SINGLE_PRIORITY
#       define VSF_NETDRV_WPCAP_CFG_HW_PRIORITY         vsf_arch_prio_0
#   else
#       define VSF_NETDRV_WPCAP_CFG_HW_PRIORITY         vsf_arch_prio_2
#   endif
#endif

#define VSF_DISP_USE_SDL2                               ENABLED
#   define VSF_DISP_SDL2_CFG_INCLUDE                    "lib\SDL2\include\SDL.h"
#   define VSF_DISP_SDL2_CFG_MOUSE_AS_TOUCHSCREEN       ENABLED
#   ifdef __VSF_X86_WIN_SINGLE_PRIORITY
#       define VSF_DISP_SDL2_CFG_HW_PRIORITY            vsf_arch_prio_0
#   else
#       define VSF_DISP_SDL2_CFG_HW_PRIORITY            vsf_arch_prio_1
#   endif
#   define APP_DISP_SDL2_HEIGHT                         768
#   define APP_DISP_SDL2_WIDTH                          1024
#   define APP_DISP_SDL2_TITLE                          "vsf_screen"
#   define APP_DISP_SDL2_COLOR                          VSF_DISP_COLOR_RGB565
#   define APP_DISP_SDL2_AMPLIFIER                      1

#if APP_USE_SDL2_DEMO == ENABLED
#   define APP_SDL2_DEMO_CFG_COLOR_RGB565
// VSF_SDL_CFG_WRAPPER is required on __WIN__ to avoid API confliction
#   define VSF_SDL_CFG_WRAPPER                          ENABLED
#endif

#define VSF_USE_WINSOUND                                ENABLED
#   define VSF_WINSOUND_CFG_TRACE                       DISABLED

#define VSF_LINUX_CFG_STACKSIZE                         (60 * 1024)
#ifdef __WIN7__
// console for win7 does not support color by default
#   define VSF_TRACE_CFG_COLOR_EN                       DISABLED
#   define VSH_HAS_COLOR                                0
#   define VSF_WINUSB_CFG_WIN7                          ENABLED
#else
#    define VSF_TRACE_CFG_COLOR_EN                      ENABLED
#endif

#if APP_USE_LVGL_DEMO == ENABLED && APP_LVGL_DEMO_USE_TERMINAL == ENABLED
// TODO: lvgl terminal demo support color
#	undef  VSF_TRACE_CFG_COLOR_EN
#	define VSF_TRACE_CFG_COLOR_EN                       DISABLED
#	define VSH_HAS_COLOR                                0

#	define VSF_DEBUGGER_CFG_CONSOLE                     VSF_DEBUGGER_CFG_CONSOLE_USER
#	undef  VSF_HAL_USE_DEBUG_STREAM
#endif

/*----------------------------------------------------------------------------*
 * Regarget Weak interface                                                    *
 *----------------------------------------------------------------------------*/

#define WEAK_VSF_KERNEL_ERR_REPORT
#define WEAK___POST_VSF_KERNEL_INIT
#define WEAK_VSF_SYSTIMER_EVTHANDLER
#define WEAK_VSF_ARCH_REQ___SYSTIMER_RESOLUTION___FROM_USR
#define WEAK_VSF_ARCH_REQ___SYSTIMER_FREQ___FROM_USR
#define WEAK_VSF_DRIVER_INIT
#define WEAK_VSF_HEAP_MALLOC_ALIGNED

#if VSF_USE_LINUX == ENABLED
#   define WEAK_VSF_LINUX_CREATE_FHS
#endif

#if APP_USE_BTSTACK_DEMO == ENABLED && APP_USE_USBH_DEMO == ENABLED
#   define WEAK_VSF_USBH_BTHCI_ON_NEW
#   define WEAK_VSF_USBH_BTHCI_ON_DEL
#   define WEAK_VSF_USBH_BTHCI_ON_PACKET

#   define WEAK_VSF_BLUETOOTH_H2_ON_NEW

// if btstack_application does not contain btstack_install
//  undef WEAK_BTSTACK_INSTALL
#   define WEAK_BTSTACK_INSTALL
// use btstack_main in btstack_application instead of in btstack_demo
#   define WEAK_BTSTACK_MAIN
#endif

#define WEAK_VSF_SCSI_ON_NEW
#define WEAK_VSF_SCSI_ON_DELETE

#if APP_USE_USBH_DEMO == ENABLED
#   define WEAK_VSF_USBH_UAC_ON_NEW
#endif

#if APP_USE_VSFVM_DEMO == ENABLED
#   define WEAK_VSF_PLUG_IN_ON_KERNEL_IDLE

#   define WEAK_VSFVM_SET_BYTECODE_IMP
#   define WEAK_VSFVM_GET_RES_IMP
#   define WEAK_VSFVM_GET_BYTECODE_IMP
#endif

#if APP_USE_VSFIP_DEMO == ENABLED || APP_USE_LWIP_DEMO == ENABLED
#   define WEAK_VSF_PNP_ON_NETDRV_PREPARE
#   define WEAK_VSF_PNP_ON_NETDRV_CONNECTED

#   if APP_USE_VSFIP_DEMO == ENABLED
#       define WEAK_VSFIP_DHCPC_ON_FINISH

#       define WEAK_VSFIP_MEM_SOCKET_GET
#       define WEAK_VSFIP_MEM_SOCKET_FREE
#       define WEAK_VSFIP_MEM_TCP_PCB_GET
#       define WEAK_VSFIP_MEM_TCP_PCB_FREE
#       define WEAK_VSFIP_MEM_NETBUF_GET
#       define WEAK_VSFIP_MEM_NETBUF_FREE
#   endif
#endif

#if APP_USE_SOCKET_DEMO == ENABLED
#   define VSF_TCPIP_USE_WINSOCK                        ENABLED
#endif

#if VSF_USBH_USE_DL1X5 == ENABLED && VSF_DISP_USE_DL1X5 == ENABLED
#   define WEAK_VSF_DL1X5_ON_NEW_DISP
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif      // __VSF_USR_CFG_WIN_H__
/* EOF */
