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


//! \note User Level Application Configuration

#ifndef __VSF_USR_CFG_H__
#define __VSF_USR_CFG_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#if     defined(__M484__)

// Application configure
#   define APP_CFG_USE_LINUX_DEMO                       ENABLED
#       define APP_CFG_USE_LINUX_LIBUSB_DEMO            ENABLED
#       define APP_CFG_USE_LINUX_MOUNT_FILE_DEMO        ENABLED
#   define APP_CFG_USE_USBH_DEMO                        ENABLED
#   define APP_CFG_USE_USBD_DEMO                        ENABLED
#   define APP_CFG_USE_SCSI_DEMO                        ENABLED
//  todo: implement audio driver for m484
#   define APP_CFG_USE_AUDIO_DEMO                       DISABLED
//  current tgui demo depends on VSF_USE_DISP_SDL2, which is only available on __WIN__
#   define APP_CFG_USE_TGUI_DEMO                        DISABLED
//  current M484 hardware has no display
#   define APP_CFG_USE_SDL2_DEMO                        DISABLED
//  TODO: need test for c++ support
#   define APP_CFG_USE_CPP_DEMO                         ENABLED

// 3rd-party demos
//  awtk is LGPL, not convenient to implement in MCU
#   define APP_CFG_USE_AWTK_DEMO                        DISABLED
#   define APP_CFG_USE_NNOM_DEMO                        ENABLED
//  current M484 hardware has no display
#   define APP_CFG_USE_LVGL_DEMO                        DISABLED
#   define APP_CFG_USE_BTSTACK_DEMO                     ENABLED
#   define APP_CFG_USE_VSFVM_DEMO                       DISABLED

#elif   defined(__F1C100S__)

// Application configure
#   define APP_CFG_USE_LINUX_DEMO                       ENABLED
#       define APP_CFG_USE_LINUX_LIBUSB_DEMO            DISABLED
#       define APP_CFG_USE_LINUX_MOUNT_FILE_DEMO        DISABLED
//  todo: implement drivers for f1c100s
#   define APP_CFG_USE_USBH_DEMO                        DISABLED
#   define APP_CFG_USE_USBD_DEMO                        ENABLED
#   define APP_CFG_USE_SCSI_DEMO                        DISABLED
#   define APP_CFG_USE_AUDIO_DEMO                       DISABLED
#   define APP_CFG_USE_SDL2_DEMO                        ENABLED
//  current tgui demo depends on VSF_USE_DISP_SDL2, which is only available on __WIN__
#   define APP_CFG_USE_TGUI_DEMO                        DISABLED
//  TODO: need test for c++ support
#   define APP_CFG_USE_CPP_DEMO                         DISABLED

// 3rd-party demos
//  awtk is LGPL, not convenient to implement in MCU
#   define APP_CFG_USE_AWTK_DEMO                        DISABLED
#   define APP_CFG_USE_NNOM_DEMO                        DISABLED
//  current M484 hardware has no display
#   define APP_CFG_USE_LVGL_DEMO                        DISABLED
#   define APP_CFG_USE_BTSTACK_DEMO                     DISABLED
#   define APP_CFG_USE_VSFVM_DEMO                       ENABLED

#elif   defined(__WIN__)

// Application configure
#   define APP_CFG_USE_LINUX_DEMO                       ENABLED
#       define APP_CFG_USE_LINUX_LIBUSB_DEMO            ENABLED
#       define APP_CFG_USE_LINUX_MOUNT_FILE_DEMO        ENABLED
#   define APP_CFG_USE_USBH_DEMO                        ENABLED
#   define APP_CFG_USE_USBD_DEMO                        ENABLED
#   define APP_CFG_USE_SCSI_DEMO                        ENABLED
#   define APP_CFG_USE_AUDIO_DEMO                       ENABLED
#   define APP_CFG_USE_TGUI_DEMO                        ENABLED
#   define APP_CFG_USE_SDL2_DEMO                        ENABLED

// VSF_LINUX_USE_SIMPLE_LIBC conflicts with c++
#   define APP_CFG_USE_CPP_DEMO                         ENABLED
#   define VSF_LINUX_USE_SIMPLE_LIBC                    DISABLED
// if VSF_LINUX_USE_SIMPLE_LIBC is enabled, need VSF_USE_SIMPLE_SSCANF and VSF_USE_SIMPLE_SPRINTF
#   define VSF_USE_SIMPLE_SSCANF                        DISABLED
#   define VSF_USE_SIMPLE_SPRINTF                       DISABLED

// 3rd-party demos
#   define APP_CFG_USE_XBOOT_XUI_DEMO                   DISABLED
#   define APP_CFG_USE_AWTK_DEMO                        ENABLED
#   define APP_CFG_USE_NNOM_DEMO                        ENABLED
#   define APP_CFG_USE_LVGL_DEMO                        ENABLED
#   define APP_CFG_USE_BTSTACK_DEMO                     ENABLED
#   define APP_CFG_USE_VSFVM_DEMO                       ENABLED

#   if APP_CFG_USE_TGUI_DEMO == ENABLED || APP_CFG_USE_XBOOT_XUI_DEMO == ENABLED
#       define APP_CFG_USE_FREETYPE_DEMO                ENABLED
#   endif

#endif


// component configure
#define VSF_USE_HEAP                                    ENABLED
#   define VSF_HEAP_CFG_MCB_MAGIC_EN                    ENABLED

#if APP_CFG_USE_USBH_DEMO == ENABLED
#   define VSF_USE_USB_HOST                             ENABLED
#       define VSF_USBH_CFG_ISO_EN                      ENABLED
#       define VSF_USE_USB_HOST_LIBUSB                  ENABLED
#       define VSF_USE_USB_HOST_HID                     ENABLED
#       define VSF_USE_USB_HOST_DS4                     ENABLED
#       define VSF_USE_USB_HOST_NSPRO                   ENABLED
#       define VSF_USE_USB_HOST_XB360                   ENABLED
#       define VSF_USE_USB_HOST_XB1                     ENABLED
#       define VSF_USE_USB_HOST_MSC                     ENABLED
#       define VSF_USE_USB_HOST_UAC                     ENABLED
#endif

#if APP_CFG_USE_BTSTACK_DEMO == ENABLED
#   define VSF_USE_USB_HOST_BTHCI                       ENABLED
#   define VSF_USE_BTSTACK                              ENABLED
#endif

#if APP_CFG_USE_SDL2_DEMO == ENABLED
#   define VSF_USE_SDL2                                 ENABLED
#endif

#define VSF_USE_VIDEO                                   ENABLED
#define VSF_USE_AUDIO                                   ENABLED
#   define VSF_USE_DECODER_WAV                          ENABLED
#   define VSF_AUDIO_CFG_USE_PLAY                       ENABLED
#   define VSF_AUDIO_CFG_USE_CATURE                     DISABLED

#define VSF_USE_UI                                      ENABLED
#if APP_CFG_USE_AWTK_DEMO == ENABLED
#   define VSF_USE_UI_AWTK                              ENABLED
#endif
#if APP_CFG_USE_LVGL_DEMO == ENABLED
#   define VSF_USE_UI_LVGL                              ENABLED
#endif
#if APP_CFG_USE_XBOOT_XUI_DEMO == ENABLED
#   define VSF_USE_XBOOT                                ENABLED
#endif

// UI runs in vsf_prio_0, other modules runs above vsf_prio_0
#if VSF_USE_UI == ENABLED
#   define VSF_USBH_CFG_EDA_PRIORITY                    vsf_prio_1
#   define APP_CFG_USBH_HW_PRIO                         vsf_arch_prio_1
#endif

#define VSF_USE_INPUT                                   ENABLED
#   define VSF_USE_INPUT_HID                            ENABLED
#   define VSF_USE_INPUT_DS4                            ENABLED
#   define VSF_USE_INPUT_NSPRO                          ENABLED
#   define VSF_USE_INPUT_XB360                          ENABLED
#   define VSF_USE_INPUT_XB1                            ENABLED

// VSF_USE_USB_DEVICE will be enabled if target chip supports USBD
//#define VSF_USE_USB_DEVICE                              ENABLED
#   define VSF_USBD_CFG_USE_EDA                         ENABLED
#   define VSF_USE_USB_DEVICE_CDCACM                    ENABLED
#   define VSF_USE_USB_DEVICE_MSC                       ENABLED
#   define VSF_USE_USB_DEVICE_UVC                       ENABLED
#   define APP_CFG_USBD_VID                             0xA7A8
#   define APP_CFG_USBD_PID                             0x2348

#define VSF_USE_MAL                                     ENABLED
#   define VSF_USE_MEM_MAL                              ENABLED
#   define VSF_USE_FAKEFAT32_MAL                        ENABLED
#   define VSF_USE_SCSI_MAL                             ENABLED
#   define VSF_USE_FILE_MAL                             ENABLED

#define VSF_USE_SCSI                                    ENABLED
#   define VSF_USE_MAL_SCSI                             ENABLED

#define VSF_USE_FS                                      ENABLED
#   define VSF_USE_MEMFS                                ENABLED
#   define VSF_USE_FATFS                                ENABLED

#define VSF_USE_TRACE                                   ENABLED
#define USRAPP_CFG_STDIO_EN                             ENABLED

#define VSF_USE_LINUX                                   ENABLED
#   define VSF_USE_LINUX_LIBUSB                         VSF_USE_USB_HOST
#   define VSF_USE_LINUX_BUSYBOX                        ENABLED

#ifndef USRAPP_CFG_LINUX_TTY_DEBUT_STREAM
#   define USRAPP_CFG_LINUX_TTY_DEBUG_STREAM            0
#   define USRAPP_CFG_LINUX_TTY_UART                    1
#   define USRAPP_CFG_LINUX_TTY_CDC                     2
#endif
#   define USRAPP_CFG_LINUX_TTY                         USRAPP_CFG_LINUX_TTY_DEBUG_STREAM

#define VSF_USE_SERVICE_STREAM                          DISABLED
#define VSF_USE_SERVICE_VSFSTREAM                       ENABLED


#ifndef USRAPP_CFG_DCD_TYPE_DEFAULT
#   define USRAPP_CFG_DCD_TYPE_DEFAULT                  0
#   define USRAPP_CFG_DCD_TYPE_DWCOTG                   1
#   define USRAPP_CFG_DCD_TYPE_MUSB_FDRC                2
#endif

#define USRAPP_CFG_FAKEFAT32                            ENABLED

#if     defined(__M484__)
#   define ASSERT(...)                                  if (!(__VA_ARGS__)) {while(1);};
//#   define ASSERT(...)

#   define VSF_DEBUGGER_CFG_CONSOLE                     VSF_DEBUGGER_CFG_CONSOLE_NULINK_NUCONSOLE
#   define VSF_HEAP_SIZE                                0x10000
#   define VSF_SYSTIMER_FREQ                            (192000000ul)

#   if APP_CFG_USE_USBD_DEMO == ENABLED
#       define VSF_USE_USB_DEVICE                       ENABLED
#           define VSF_USBD_CFG_EDA_PRIORITY            vsf_prio_1
#           define VSF_USBD_CFG_HW_PRIORITY             vsf_arch_prio_1
#           define USRAPP_CFG_USBD_SPEED                USB_SPEED_HIGH
#           define USRAPP_CFG_CDC_NUM                   1
#           define USRAPP_CFG_CDC_TX_STREAM_SIZE        1024
#           define USRAPP_CFG_CDC_RX_STREAM_SIZE        512
#           define USRAPP_CFG_DCD_TYPE                  USRAPP_CFG_DCD_TYPE_DEFAULT
#           define USRAPP_CFG_STREAM_ALIGN              1
#   endif

#   define VSF_USBH_CFG_ENABLE_ROOT_HUB                 ENABLED
#   define VSF_USE_USB_HOST_HUB                         ENABLED
#   define VSF_USE_USB_HOST_HCD_OHCI                    ENABLED

#   define USRAPP_CFG_USBD_DEV                          VSF_USB_DC0

#   define VSF_LINUX_CFG_STACKSIZE                      2048
#   define VSF_TRACE_CFG_COLOR_EN                       DISABLED
#   define VSH_HAS_COLOR                                0
#elif   defined(__F1C100S__)
#   define ASSERT(...)                                  if (!(__VA_ARGS__)) {while(1);};
//#   define ASSERT(...)

#   define VSF_HAL_USE_DEBUG_STREAM                     ENABLED
#   define VSF_HEAP_SIZE                                0x1000000
#   define VSF_HEAP_CFG_MCB_ALIGN_BIT                   12      // 4K alignment
//#   define VSF_SYSTIMER_FREQ                            (24UL * 1000 * 1000)
#   define VSF_KERNEL_CFG_EDA_SUPPORT_TIMER             DISABLED
#   define VSF_KERNEL_CFG_CALLBACK_TIMER                DISABLED
#   define VSF_OS_CFG_PRIORITY_NUM                      1
#   define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                  ENABLED
//  retarget-io for arm9 in iar seems to be mal-functioning, use dimple_stdio
#   define VSF_LINUX_USE_SIMPLE_STDIO                   ENABLED

#   if APP_CFG_USE_USBD_DEMO == ENABLED
#       define VSF_USE_USB_DEVICE                       ENABLED
//          TODO: use actual priority after arm9 arch is ready
#           define VSF_USBD_CFG_EDA_PRIORITY            vsf_prio_0
#           define VSF_USBD_CFG_HW_PRIORITY             vsf_arch_prio_0
#           define USRAPP_CFG_USBD_SPEED                USB_SPEED_HIGH
#           define USRAPP_CFG_CDC_NUM                   1
#           define USRAPP_CFG_CDC_TX_STREAM_SIZE        1024
#           define USRAPP_CFG_CDC_RX_STREAM_SIZE        512
#           define USRAPP_CFG_DCD_TYPE                  USRAPP_CFG_DCD_TYPE_DEFAULT
#           define USRAPP_CFG_STREAM_ALIGN              1
#   endif

#   define VSF_USBH_CFG_ENABLE_ROOT_HUB                 ENABLED
#   define VSF_USE_USB_HOST_HUB                         ENABLED
#   define VSF_USE_USB_HOST_HCD_OHCI                    ENABLED

#   define USRAPP_CFG_USBD_DEV                          VSF_USB_DC0

#   define VSF_USE_DISP_FB                              ENABLED
#       define VSF_DISP_FB_CFG_COPY_FRAME               false
#       define APP_DISP_FB_COLOR                        VSF_DISP_COLOR_RGB666_32
#       define APP_DISP_FB_NUM                          3
//      for VGA 640 * 480 60Hz
//#       define APP_DISP_FB_CFG_PIXEL_CLOCK              (25UL * 1000 * 1000)
//#       define APP_DISP_FB_CFG_H_FP                     16
//#       define APP_DISP_FB_CFG_H_BP                     48
//#       define APP_DISP_FB_CFG_H_SYNC                   96
//#       define APP_DISP_FB_CFG_V_FP                     11
//#       define APP_DISP_FB_CFG_V_BP                     31
//#       define APP_DISP_FB_CFG_V_SYNC                   2
//#       define APP_DISP_FB_WIDTH                        640
//#       define APP_DISP_FB_HEIGHT                       480
//      for LCD
#       define APP_DISP_FB_CFG_PIXEL_CLOCK              (36UL * 1000 * 1000)
#       define APP_DISP_FB_CFG_H_FP                     40
#       define APP_DISP_FB_CFG_H_BP                     40
#       define APP_DISP_FB_CFG_H_SYNC                   48
#       define APP_DISP_FB_CFG_V_FP                     13
#       define APP_DISP_FB_CFG_V_BP                     29
#       define APP_DISP_FB_CFG_V_SYNC                   3
#       define APP_DISP_FB_WIDTH                        800
#       define APP_DISP_FB_HEIGHT                       480

#   if APP_CFG_USE_SDL2_DEMO == ENABLED
#       define APP_CFG_SDL2_DEMO_COLOR_RGB666
#   endif

#   define VSF_LINUX_CFG_STACKSIZE                      (60 * 1024)
#   define VSF_TRACE_CFG_COLOR_EN                       DISABLED
#   define VSH_HAS_COLOR                                1
#elif   defined(__NUC505__)
#   define ASSERT(...)                                  if (!(__VA_ARGS__)) {while(1);};
//#   define ASSERT(...)

#   define VSF_DEBUGGER_CFG_CONSOLE                     VSF_DEBUGGER_CFG_CONSOLE_NULINK_NUCONSOLE
#   define VSF_HEAP_SIZE                                0x4000
#   define VSF_SYSTIMER_FREQ                            (96000000ul)

#   define VSF_USBH_CFG_ENABLE_ROOT_HUB                 ENABLED
#   define VSF_USE_USB_HOST_HUB                         ENABLED
#   define VSF_USE_USB_HOST_HCD_OHCI                    ENABLED
#elif   defined(__WIN__)

// TODO: include for clang only
#   pragma clang diagnostic ignored "-Wbuiltin-requires-header"
#   pragma clang diagnostic ignored "-Wmicrosoft-include"
#   pragma clang diagnostic ignored "-Winconsistent-dllimport"
#   pragma clang diagnostic ignored "-Wimplicit-function-declaration"

#   ifdef __CPU_X64__
#       error x64 is currently not supported
#   endif

#   define ASSERT(...)                                  assert(__VA_ARGS__)
//#   define ASSERT(...)

#   define VSF_HAL_USE_DEBUG_STREAM                     ENABLED
#   define VSF_HEAP_SIZE                                0x1000000
#   define VSF_HEAP_CFG_MCB_ALIGN_BIT                   12      // 4K alignment

#   define VSF_SYSTIMER_FREQ                            (0ul)

#   define VSF_USE_USB_DEVICE                           ENABLED
#       define VSF_USE_USB_DEVICE_DCD_USBIP             ENABLED
#           define VSF_USBIP_SERVER_CFG_DEBUG           ENABLED
#           define VSF_USBIP_SERVER_CFG_DEBUG_TRAFFIC   DISABLED
#           define VSF_USBIP_SERVER_CFG_DEBUG_URB       ENABLED
#       define VSF_USBD_CFG_EDA_PRIORITY                vsf_prio_1
#       define VSF_USBD_CFG_HW_PRIORITY                 vsf_arch_prio_1
#       define USRAPP_CFG_USBD_DEV                      VSF_USB_DC0

#   define VSF_USBH_CFG_ENABLE_ROOT_HUB                 DISABLED
#   define VSF_USE_USB_HOST_HUB                         DISABLED
#   define VSF_USE_USB_HOST_HCD_WINUSB                  ENABLED
#       define VSF_WINUSB_HCD_CFG_DEV_NUM               7
#       define VSF_WINUSB_HCD_DEV0_VID                  0x0A12      // CSR8510 bthci
#       define VSF_WINUSB_HCD_DEV0_PID                  0x0001
#       define VSF_WINUSB_HCD_DEV1_VID                  0x0A5C      // BCM20702 bthci
#       define VSF_WINUSB_HCD_DEV1_PID                  0x21E8
#       define VSF_WINUSB_HCD_DEV2_VID                  0x04F2      // UVC
#       define VSF_WINUSB_HCD_DEV2_PID                  0xB130
#       define VSF_WINUSB_HCD_DEV3_VID                  0x054C      // DS4
#       define VSF_WINUSB_HCD_DEV3_PID                  0x05C4
#       define VSF_WINUSB_HCD_DEV4_VID                  0x054C      // DS4
#       define VSF_WINUSB_HCD_DEV4_PID                  0x09CC
#       define VSF_WINUSB_HCD_DEV5_VID                  0x057E      // NSPRO
#       define VSF_WINUSB_HCD_DEV5_PID                  0x2009
#       define VSF_WINUSB_HCD_DEV6_VID                  0x0D8C      // uac
#       define VSF_WINUSB_HCD_DEV6_PID                  0x013C
//#       define VSF_WINUSB_HCD_DEV6_VID                  0xA7A8      // usbd_demo
//#       define VSF_WINUSB_HCD_DEV6_PID                  0x2348
//#       define VSF_WINUSB_HCD_DEV7_VID                  0x045E      // XB360
//#       define VSF_WINUSB_HCD_DEV7_PID                  0x028E
//#       define VSF_WINUSB_HCD_DEV8_VID                  0x045E      // XB1
//#       define VSF_WINUSB_HCD_DEV8_PID                  0x02EA

#   define VSF_USE_WINFS                                ENABLED

#   if APP_CFG_USE_TGUI_DEMO == ENABLED
#       define APP_CFG_TGUI_RES_DIR                     "./winfs_root/ui/"
#   endif

#   if APP_CFG_USE_XBOOT_XUI_DEMO == ENABLED
#       define APP_CFG_XBOOT_RES_DIR                    "./winfs_root/ui/"
#   endif

#   define VSF_USE_DISP_SDL2                            ENABLED
#       define VSF_DISP_SDL2_CFG_INCLUDE                "lib\SDL2\include\SDL.h"
#       define VSF_DISP_SDL2_CFG_MOUSE_AS_TOUCHSCREEN   ENABLED
#       define VSF_DISP_SDL2_CFG_HW_PRIORITY            vsf_arch_prio_1
#       define APP_DISP_SDL2_HEIGHT                     768
#       define APP_DISP_SDL2_WIDTH                      1024
#       define APP_DISP_SDL2_COLOR                      VSF_DISP_COLOR_RGB565
#       define APP_DISP_SDL2_AMPLIFIER                  1

#   if APP_CFG_USE_SDL2_DEMO == ENABLED
#       define APP_CFG_SDL2_DEMO_COLOR_RGB565
#   endif

#   define VSF_USE_WINSOUND                             ENABLED
#       define VSF_WINSOUND_CFG_TRACE                   DISABLED

#   define VSF_LINUX_CFG_STACKSIZE                      (60 * 1024)
#   define VSF_TRACE_CFG_COLOR_EN                       ENABLED
#   define VSH_ECHO                                     1

/*----------------------------------------------------------------------------*
 * Regarget Weak interface                                                    *
 *----------------------------------------------------------------------------*/

#   define WEAK_VSF_KERNEL_ERR_REPORT
#   define WEAK___POST_VSF_KERNEL_INIT
#   define WEAK_VSF_SYSTIMER_EVTHANDLER
#   define WEAK_VSF_ARCH_REQ___SYSTIMER_RESOLUTION___FROM_USR
#   define WEAK_VSF_ARCH_REQ___SYSTIMER_FREQ___FROM_USR
#   define WEAK_VSF_DRIVER_INIT
#   define WEAK_VSF_HEAP_MALLOC_ALIGNED

#   if VSF_USE_LINUX == ENABLED
#       define WEAK_VSF_LINUX_CREATE_FHS
#   endif

#   if VSF_USE_BTSTACK == ENABLED && VSF_USE_USB_HOST_BTHCI == ENABLED
#       define WEAK_VSF_USBH_BTHCI_ON_NEW
#       define WEAK_VSF_USBH_BTHCI_ON_DEL
#       define WEAK_VSF_USBH_BTHCI_ON_PACKET

#       define WEAK_VSF_BLUETOOTH_H2_ON_NEW
#   endif

#   define WEAK_VSF_SCSI_ON_NEW
#   define WEAK_VSF_SCSI_ON_DELETE

#   if VSF_USE_USB_HOST == ENABLED && VSF_USE_USB_HOST_UAC == ENABLED
#       define WEAK_VSF_USBH_UAC_ON_NEW
#   endif

#   if APP_CFG_USE_VSFVM_DEMO == ENABLED
#       define WEAK_VSF_PLUG_IN_ON_KERNEL_IDLE

#       define WEAK_VSFVM_SET_BYTECODE_IMP
#       define WEAK_VSFVM_GET_RES_IMP
#       define WEAK_VSFVM_GET_BYTECODE_IMP
#   endif

#endif

#if VSF_USE_LINUX == ENABLED
#   define VSF_USE_POSIX                                ENABLED
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ INCLUDES ======================================*/

#if APP_CFG_USE_TGUI_DEMO == ENABLED
#   include "vsf_tgui_cfg.h"
#endif

#endif
/* EOF */
