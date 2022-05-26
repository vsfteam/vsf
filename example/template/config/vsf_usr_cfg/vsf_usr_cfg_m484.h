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

#ifndef __VSF_USR_CFG_M484_H__
#define __VSF_USR_CFG_M484_H__

/*============================ INCLUDES ======================================*/

#define __VSF_HEADER_ONLY_SHOW_COMPILER_INFO__
#include "utilities/compiler/compiler.h"

/*============================ MACROS ========================================*/

#if __IS_COMPILER_IAR__
#   define VSF_KERNEL_CFG_THREAD_STACK_CHECK            ENABLED
#   define VSF_KERNEL_GET_STACK_FROM_JMPBUF(__JMPBUF)   ((*(__JMPBUF))[4] & 0xFFFFFFFF)
#elif __IS_COMPILER_GCC__
// strtoxxx in newlib has dependency issues, implement in simple_stdlib
#   define VSF_LINUX_SIMPLE_STDLIB_USE_STRTOXX          ENABLED
#   define VSF_USE_SIMPLE_SPRINTF                       ENABLED
#   define VSF_USE_SIMPLE_SSCANF                        ENABLED
#endif

#define VSF_SYSTIMER_FREQ                               (192000000ul)

// Application configure
#define APP_USE_LINUX_DEMO                              ENABLED
#   define APP_USE_LINUX_LIBUSB_DEMO                    DISABLED
#   define APP_USE_LINUX_MOUNT_DEMO                     DISABLED
#define APP_USE_USBH_DEMO                               ENABLED
#define APP_USE_USBD_DEMO                               ENABLED
#   define APP_USE_USBD_CDC_DEMO                        ENABLED
#   define APP_USE_USBD_MSC_DEMO                        ENABLED
#   define APP_USE_USBD_UVC_DEMO                        ENABLED
#define APP_USE_SCSI_DEMO                               ENABLED
//  todo: implement audio driver for m484
#define APP_USE_AUDIO_DEMO                              DISABLED
//  current tgui demo depends on VSF_DISP_USE_SDL2, which is only available on __WIN__
#define APP_USE_TGUI_DEMO                               DISABLED
//  current M484 hardware has no display
#define APP_USE_SDL2_DEMO                               DISABLED
#define APP_USE_DISP_DEMO                               DISABLED
//  TODO: need test for c++ support
//#define APP_USE_CPP_DEMO                                ENABLED
#if APP_USE_CPP_DEMO == ENABLED
#   define __VSF_WORKAROUND_IAR_CPP__
#endif

#define APP_USE_HAL_DEMO                                DISABLED
#define APP_USE_HAL_SPI_DEMO                            DISABLED

// 3rd-party demos
// nnom minst demo seems to be broken
#define APP_USE_NNOM_DEMO                               DISABLED
#define APP_USE_LVGL_DEMO                               DISABLED
#   define APP_LVGL_DEMO_USE_TOUCHSCREEN                ENABLED
#   define APP_LVGL_DEMO_CFG_TOUCH_REMAP                ENABLED
// if using dl1x5(DL1X5 chips from DisplayLink), color_depth should be 16,
//  and hor_res/ver_res should smaller than the hardware resolution
#   define APP_LVGL_DEMO_CFG_COLOR_DEPTH                16
#   define APP_LVGL_DEMO_CFG_HOR_RES                    320
#   define APP_LVGL_DEMO_CFG_VER_RES                    240
#   define APP_LVGL_DEMO_CFG_PIXEL_BUFFER_SIZE          (4 * 1024)
#define APP_USE_BTSTACK_DEMO                            DISABLED
#define APP_USE_VSFVM_DEMO                              DISABLED
// select one for tcpip stack
#define APP_USE_VSFIP_DEMO                              DISABLED
#define APP_USE_LWIP_DEMO                               DISABLED

// component configure
#define VSF_USE_HEAP                                    ENABLED
#   define VSF_HEAP_CFG_MCB_MAGIC_EN                    ENABLED
#   define VSF_HEAP_SIZE                                0x10000

#define VSF_USE_VIDEO                                   ENABLED
#define VSF_USE_AUDIO                                   ENABLED
#   define VSF_AUDIO_USE_DECODER_WAV                    ENABLED
#   define VSF_AUDIO_USE_PLAY                           ENABLED
#   define VSF_AUDIO_USE_CATURE                         DISABLED

// UI runs in vsf_prio_0, other modules runs above vsf_prio_1
#if APP_USE_XBOOT_XUI_DEMO == ENABLED || APP_USE_TGUI_DEMO == ENABLED
#   define VSF_USBH_CFG_EDA_PRIORITY                    vsf_prio_1
#   define APP_CFG_USBH_HW_PRIO                         vsf_arch_prio_1
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

#define VSF_USE_TRACE                                   ENABLED
#define USRAPP_CFG_STDIO_EN                             ENABLED

#define VSF_USE_LINUX                                   ENABLED
#   define VSF_LINUX_USE_LIBUSB                         VSF_USE_USB_HOST
#   define VSF_LINUX_USE_BUSYBOX                        ENABLED

#   if APP_USE_CPP_DEMO != ENABLED
// simple_libc does not compatible with cpp, so if cpp is used, DO NOT use simple_libc
//  make sure in inclue path, simple_libc is removed if cpp is used
#       define VSF_LINUX_USE_SIMPLE_LIBC                ENABLED
#           define VSF_LINUX_USE_SIMPLE_STDIO           ENABLED
#           define VSF_LINUX_USE_SIMPLE_STRING          ENABLED
#           define VSF_LINUX_USE_SIMPLE_TIME            ENABLED
#           define VSF_LINUX_USE_SIMPLE_STDLIB          ENABLED
#           define VSF_LINUX_USE_SIMPLE_CTYPE           ENABLED
#   endif

#ifndef USRAPP_CFG_LINUX_TTY_DEBUT_STREAM
#   define USRAPP_CFG_LINUX_TTY_DEBUG_STREAM            0
#   define USRAPP_CFG_LINUX_TTY_UART                    1
#   define USRAPP_CFG_LINUX_TTY_CDC                     2
#endif
#   define USRAPP_CFG_LINUX_TTY                         USRAPP_CFG_LINUX_TTY_DEBUG_STREAM

#define VSF_USE_STREAM                                  DISABLED
#define VSF_USE_SIMPLE_STREAM                           ENABLED

#define USRAPP_CFG_FAKEFAT32                            ENABLED

#define VSF_ASSERT(...)                                 if (!(__VA_ARGS__)) {while(1);};
//#define VSF_ASSERT(...)

#define VSF_DEBUGGER_CFG_CONSOLE                        VSF_DEBUGGER_CFG_CONSOLE_NULINK_NUCONSOLE

#if APP_USE_USBD_DEMO == ENABLED
#   define VSF_USE_USB_DEVICE                           ENABLED
#       define VSF_USBD_CFG_EDA_PRIORITY                vsf_prio_1
#       define VSF_USBD_CFG_HW_PRIORITY                 vsf_arch_prio_1
#       define USRAPP_CFG_USBD_SPEED                    USB_SPEED_HIGH
#       define USRAPP_CFG_CDC_NUM                       1
#       define USRAPP_CFG_CDC_TX_STREAM_SIZE            1024
#       define USRAPP_CFG_CDC_RX_STREAM_SIZE            512
#endif

#if APP_USE_USBH_DEMO == ENABLED
#   define VSF_USBH_CFG_ENABLE_ROOT_HUB                 ENABLED
#   define VSF_USBH_USE_HUB                             ENABLED
#   define VSF_USBH_USE_HCD_OHCI                        ENABLED
#   define VSF_USBH_USE_DL1X5                           ENABLED
#       define VSF_DISP_USE_DL1X5                       ENABLED
#endif

#define USRAPP_CFG_USBD_DEV                             VSF_USB_DC0

// if VSF_KERNEL_CFG_EDA_FAST_SUB_CALL is enabled, use 4096
//  else 2048 is enough
#define VSF_LINUX_CFG_STACKSIZE                         4096
#define VSF_TRACE_CFG_COLOR_EN                          DISABLED
#define VSH_HAS_COLOR                                   0

#if APP_USE_DISP_DEMO == ENABLED
#   define VSF_USE_UI                                      ENABLED
#   define VSF_DISP_USE_MIPI_LCD                           ENABLED
#      define WEAK_VK_DISP_MIPI_TE_LINE_ISR_ENABLE_ONCE
#      define WEAK_VK_DISP_MIPI_LCD_IO_INIT
#      define VK_DISP_MIPI_LCD_SUPPORT_HARDWARE_RESET      ENABLED

#      define APP_DISP_DEMO_HEIGHT                         320
#      define APP_DISP_DEMO_WIDTH                          480
#      define APP_DISP_DEMO_COLOR                          VSF_DISP_COLOR_RGB565
#      define APP_DISP_DEMO_SEQ                            VSF_DISP_MIPI_LCD_ST7796S
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif      // __VSF_USR_CFG_M484_H__
/* EOF */
