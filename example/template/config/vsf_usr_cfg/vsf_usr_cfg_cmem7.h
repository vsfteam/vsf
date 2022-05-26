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

#ifndef __VSF_USR_CFG_CMEM7_H__
#define __VSF_USR_CFG_CMEM7_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define VSF_SYSTIMER_FREQ                                   (266666667)
#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                         ENABLED

// Application configure
#   define APP_USE_LINUX_DEMO                           ENABLED
#       define APP_USE_LINUX_LIBUSB_DEMO                DISABLED
#       define APP_USE_LINUX_MOUNT_DEMO                 DISABLED
#   define APP_USE_USBH_DEMO                            DISABLED
#   define APP_USE_USBD_DEMO                            ENABLED
#       define APP_USE_USBD_CDC_DEMO                    ENABLED
#       define APP_USE_USBD_MSC_DEMO                    ENABLED
#       define APP_USE_USBD_UVC_DEMO                    DISABLED
#       define APP_USE_USBD_UAC_DEMO                    ENABLED
#   define APP_USE_SCSI_DEMO                            DISABLED
#   define APP_USE_AUDIO_DEMO                           DISABLED
//  current tgui demo depends on VSF_DISP_USE_SDL2, which is only available on __WIN__
#   define APP_USE_TGUI_DEMO                            DISABLED
//  current M484 hardware has no display
#   define APP_USE_SDL2_DEMO                            DISABLED
//  TODO: need test for c++ support
#   define APP_USE_CPP_DEMO                             DISABLED
#		define APP_USE_KERNEL_TEST						ENABLED

// 3rd-party demos
//  awtk is LGPL, not convenient to implement in MCU
#   define APP_USE_AWTK_DEMO                            DISABLED
#   define APP_USE_NNOM_DEMO                            DISABLED
//  current M484 hardware has no display
#   define APP_USE_LVGL_DEMO                            DISABLED
#   define APP_LVGL_DEMO_USE_TOUCHSCREEN                ENABLED
#       define APP_LVGL_DEMO_CFG_TOUCH_REMAP            ENABLED
#       define APP_LVGL_DEMO_CFG_COLOR_DEPTH            32
#       define APP_LVGL_DEMO_CFG_HOR_RES                640
#       define APP_LVGL_DEMO_CFG_VER_RES                480

#   define APP_USE_BTSTACK_DEMO                         DISABLED
#   define APP_USE_VSFVM_DEMO                           DISABLED
// select one for tcpip stack
#   define APP_USE_VSFIP_DEMO                           DISABLED
#   define APP_USE_LWIP_DEMO                            DISABLED

#   define APP_USE_FREETYPE_DEMO                        ENABLED

// component configure
#define VSF_USE_HEAP                                    ENABLED
#   define VSF_HEAP_CFG_MCB_MAGIC_EN                    ENABLED
#   define VSF_HEAP_SIZE                                (32 * 1024)
//#   define VSF_SYSTIMER_FREQ                            (25000000ul)

#define VSF_USE_VIDEO                                   ENABLED
#define VSF_USE_AUDIO                                   ENABLED
#   define VSF_AUDIO_USE_DECODER_WAV                    ENABLED
#   define VSF_AUDIO_USE_PLAY                           ENABLED
#   define VSF_AUDIO_USE_CATURE                         ENABLED

// UI runs in vsf_prio_0, other modules runs above vsf_prio_1
#if APP_USE_AWTK_DEMO == ENABLED || APP_USE_LVGL_DEMO == ENABLED || APP_USE_XBOOT_XUI_DEMO == ENABLED || APP_USE_TGUI_DEMO == ENABLED
#   define VSF_USBH_CFG_EDA_PRIORITY                    vsf_prio_1
#   define APP_CFG_USBH_HW_PRIO                         vsf_arch_prio_1
#endif

#define VSF_USE_INPUT                                   DISABLED
#   define VSF_INPUT_USE_HID                            DISABLED
#   define VSF_INPUT_USE_DS4                            DISABLED
#   define VSF_INPUT_USE_NSPRO                          DISABLED
#   define VSF_INPUT_USE_XB360                          DISABLED
#   define VSF_INPUT_USE_XB1                            DISABLED

// VSF_USE_USB_DEVICE will be enabled if target chip supports USBD
//#define VSF_USE_USB_DEVICE                              ENABLED
#   define VSF_USBD_CFG_USE_EDA                         ENABLED
#   define VSF_USBD_USE_CDCACM                          ENABLED
#   define VSF_USBD_USE_MSC                             ENABLED
#   define VSF_USBD_USE_UVC                             ENABLED
#   define VSF_USBD_USE_UAC                             ENABLED
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
#   define VSF_LINUX_CFG_WRAPPER                        ENABLED
#   define VSF_LINUX_USE_BUSYBOX                        ENABLED
#   define VSF_LINUX_USE_SIMPLE_LIBC                    ENABLED
#   define VSF_LINUX_USE_SIMPLE_TIME                    ENABLED
#   define VSF_LINUX_CFG_RELATIVE_PATH                  ENABLED

#if VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
// WRAPPERs are required on __LINUX__ to avoid API confliction
#   define VSF_LINUX_CFG_WRAPPER                        ENABLED
#   define VSF_LINUX_LIBC_CFG_WRAPPER                   ENABLED
#   define VSF_LINUX_LIBGEN_CFG_WRAPPER                 ENABLED
#endif

#ifndef USRAPP_CFG_LINUX_TTY_DEBUG_STREAM
#   define USRAPP_CFG_LINUX_TTY_DEBUG_STREAM            0
#   define USRAPP_CFG_LINUX_TTY_UART                    1
#   define USRAPP_CFG_LINUX_TTY_CDC                     2
#endif
#   define USRAPP_CFG_LINUX_TTY                         USRAPP_CFG_LINUX_TTY_DEBUG_STREAM

#define VSF_USE_STREAM                                  DISABLED
#define VSF_USE_SIMPLE_STREAM                           ENABLED

#define USRAPP_CFG_FAKEFAT32                            ENABLED

extern void VSF_DEBUG_STREAM_POLL(void);
#define VSF_ASSERT(...)                                 if (!(__VA_ARGS__)) {while(1);};
#define VSF_USB_ASSERT(...)                             if (!(__VA_ARGS__)) { vsf_trace_debug( # __VA_ARGS__ VSF_TRACE_CFG_LINEEND);  while(1) { VSF_DEBUG_STREAM_POLL(); }};

#define VSF_HAL_USE_DEBUG_STREAM                        ENABLED

#if APP_USE_USBD_DEMO == ENABLED
#   define VSF_USE_USB_DEVICE                           ENABLED
#       define VSF_USBD_USE_DCD_DWCOTG                  ENABLED
#       define VSF_USBD_CFG_EDA_PRIORITY                vsf_prio_0
#       define VSF_USBD_CFG_HW_PRIORITY                 vsf_arch_prio_0
#       define VSF_USBD_CFG_TRACE                       DISABLED
#       define VSF_USBD_CFG_TRACE_EVT                   DISABLED
#       define USRAPP_CFG_USBD_SPEED                    USB_SPEED_FULL
#       define USRAPP_CFG_CDC_NUM                       1
#       define USRAPP_CFG_CDC_TX_STREAM_SIZE            1024
#       define USRAPP_CFG_CDC_RX_STREAM_SIZE            512

#       define USRAPP_USBD_DWCOTG_CFG_ULPI_EN           true
#       define USRAPP_USBD_DWCOTG_CFG_UTMI_EN           false
#       define USRAPP_USBD_DWCOTG_CFG_VBUS_EN           false
#       define USRAPP_USBD_DWCOTG_CFG_DMA_EN            false
#endif

#define VSF_USBH_CFG_ENABLE_ROOT_HUB                    ENABLED
#define VSF_USBH_USE_HUB                                ENABLED
#define VSF_USBH_USE_HCD_OHCI                           ENABLED

#define USRAPP_CFG_USBD_DEV                             VSF_USB_DC0

#define VSF_LINUX_CFG_STACKSIZE                         (4 * 1024)
#define VSF_TRACE_CFG_COLOR_EN                          DISABLED
#define VSH_HAS_COLOR                                   0

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif      // __VSF_USR_CFG_CMEM7_H__
/* EOF */
