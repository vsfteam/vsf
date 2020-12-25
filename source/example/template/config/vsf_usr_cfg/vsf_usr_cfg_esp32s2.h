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

#ifndef __VSF_USR_CFG_ESP32S2_H__
#define __VSF_USR_CFG_ESP32S2_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define VSF_ARCH_RTOS_CFG_BASE_PRIORITY                 2
// VSF_ARCH_RTOS_CFG_STACK_DEPTH is in unit VSF_ARCH_RTOS_STACK_T
#define VSF_ARCH_RTOS_CFG_STACK_DEPTH                   1024
// VSF_ARCH_RTOS_CFG_MAIN_STACK_DEPTH is in unit VSF_ARCH_RTOS_STACK_T
#define VSF_ARCH_RTOS_CFG_MAIN_STACK_DEPTH              4096

#define VSF_SYSTIMER_FREQ                               (0ul)
#define VSF_OS_CFG_PRIORITY_NUM                         3
#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                     ENABLED
#define VSF_OS_CFG_MAIN_MODE                            VSF_OS_CFG_MAIN_MODE_IDLE
// VSF_OS_CFG_MAIN_STACK_SIZE is used only in VSF_OS_CFG_MAIN_MODE_THREAD mode,
//  but no harm in other modes
#   define VSF_OS_CFG_MAIN_STACK_SIZE                   4096

// Application configure
#define APP_USE_LINUX_DEMO                              ENABLED
#   define APP_USE_LINUX_LIBUSB_DEMO                    APP_USE_USBH_DEMO
#   define APP_USE_LINUX_MOUNT_FILE_DEMO                DISABLED
#define APP_USE_USBH_DEMO                               ENABLED
#define APP_USE_USBD_DEMO                               DISABLED
#   define APP_USE_USBD_CDC_DEMO                        DISABLED
#   define APP_USE_USBD_MSC_DEMO                        ENABLED
#   define APP_USE_USBD_UVC_DEMO                        DISABLED
#   define APP_USE_USBD_UAC_DEMO                        ENABLED
#define APP_USE_SCSI_DEMO                               ENABLED
#define APP_USE_AUDIO_DEMO                              DISABLED
#define APP_USE_SDL2_DEMO                               DISABLED
//  current tgui demo depends on VSF_DISP_USE_SDL2, which is only available on __WIN__
#define APP_USE_TGUI_DEMO                               DISABLED
//  TODO: need test for c++ support
#define APP_USE_CPP_DEMO                                DISABLED
#define APP_USE_HAL_DEMO                                DISABLED
#   define APP_USE_HAL_USART_DEMO                       DISABLED
#       define APP_USART_DEMO_CFG_REQUEST_TEST              DISABLED
#       define APP_USART_DEMO_CFG_FIFO_WRITE_WITH_ISR_TEST  DISABLED
#       define APP_USART_DEMO_CFG_FIFO_READ_WITH_ISR_TEST   DISABLED
#       define APP_USART_DEMO_CFG_FIFO_ECHO_TEST            ENABLED

// 3rd-party demos
//  awtk is LGPL, not convenient to implement in MCU
#define APP_USE_AWTK_DEMO                               DISABLED
#define APP_USE_NNOM_DEMO                               DISABLED
#define APP_USE_LVGL_DEMO                               DISABLED
#   define APP_LVGL_DEMO_USE_TOUCHSCREEN                ENABLED
#   define APP_LVGL_DEMO_CFG_TOUCH_REMAP                ENABLED
#   define APP_LVGL_DEMO_CFG_COLOR_DEPTH                32
#   define APP_LVGL_DEMO_CFG_HOR_RES                    640
#   define APP_LVGL_DEMO_CFG_VER_RES                    480
#define APP_USE_BTSTACK_DEMO                            ENABLED
#define APP_USE_VSFVM_DEMO                              DISABLED
// select one for tcpip stack
#define APP_USE_VSFIP_DEMO                              DISABLED
#define APP_USE_LWIP_DEMO                               DISABLED


// component configure
#define VSF_USE_HEAP                                    ENABLED
#   define VSF_HEAP_CFG_MCB_MAGIC_EN                    ENABLED
#   define VSF_HEAP_SIZE                                0x10000
#   define VSF_HEAP_CFG_MCB_ALIGN_BIT                   2       // 4-byte alignment

#define VSF_USE_VIDEO                                   DISABLED
#define VSF_USE_AUDIO                                   ENABLED
#   define VSF_AUDIO_USE_DECODER_WAV                    ENABLED
#   define VSF_AUDIO_USE_PLAY                           ENABLED
#   define VSF_AUDIO_USE_CATURE                         DISABLED

#define VSF_USE_INPUT                                   ENABLED
#   define VSF_INPUT_USE_HID                            ENABLED
#   define VSF_INPUT_USE_DS4                            ENABLED
#   define VSF_INPUT_USE_NSPRO                          ENABLED
#   define VSF_INPUT_USE_XB360                          ENABLED
#   define VSF_INPUT_USE_XB1                            ENABLED

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

#if APP_USE_USBD_DEMO == ENABLED
#   define VSF_USE_USB_DEVICE                           ENABLED
#       define VSF_USBD_USE_DCD_DWCOTG                  ENABLED
#       define VSF_USBD_USE_CDCACM                      ENABLED
#       define VSF_USBD_USE_MSC                         ENABLED
#       define VSF_USBD_USE_UVC                         DISABLED
#       define VSF_USBD_USE_UAC                         ENABLED
#       define VSF_USBD_CFG_USE_EDA                     ENABLED
#       define VSF_USBD_USE_CDCACM                      ENABLED
#       define USRAPP_CFG_USBD_SPEED                    USB_DC_SPEED_FULL
#       define APP_CFG_USBD_VID                         0xA7A8
#       define APP_CFG_USBD_PID                         0x2348
#endif

#if APP_USE_USBH_DEMO == ENABLED
#   define VSF_USE_USB_HOST                             ENABLED
#       define VSF_USBH_USE_HCD_DWCOTG                  ENABLED
#       define VSF_USBH_CFG_ENABLE_ROOT_HUB             ENABLED
#       define VSF_USBH_USE_HCD_DWCOTG                  ENABLED
#       define VSF_USBH_USE_HUB                         ENABLED
#endif

#define VSF_USE_TRACE                                   ENABLED
#   define VSF_TRACE_CFG_COLOR_EN                       ENABLED
#   define VSF_CFG_DEBUG_STREAM_TX_T                    vsf_stream_t
#   define VSF_CFG_DEBUG_STREAM_RX_T                    vsf_mem_stream_t

#define VSF_USE_LINUX                                   ENABLED
#   define VSF_LINUX_USE_SIMPLE_LIBC                    ENABLED
#   define VSF_LINUX_USE_LIBUSB                         VSF_USE_USB_HOST
#   define VSF_LINUX_CFG_STACKSIZE                      4096
#   define VSF_LINUX_CFG_PRIO_LOWEST                    vsf_prio_1
#   define VSF_LINUX_CFG_PRIO_HIGHEST                   vsf_prio_1
#   define VSF_LINUX_CFG_RELATIVE_PATH                  ENABLED

extern void vsf_assert(int expression);
#   define ASSERT(...)                                  vsf_assert(__VA_ARGS__)
#   define VSF_LINUX_USE_BUSYBOX                        ENABLED
#   define VSH_ENTER_CHAR                               '\n'

#ifndef USRAPP_CFG_LINUX_TTY_DEBUT_STREAM
#   define USRAPP_CFG_LINUX_TTY_DEBUG_STREAM            0
#   define USRAPP_CFG_LINUX_TTY_UART                    1
#   define USRAPP_CFG_LINUX_TTY_CDC                     2
#endif
#   define USRAPP_CFG_LINUX_TTY                         USRAPP_CFG_LINUX_TTY_DEBUG_STREAM

#define VSF_USE_FIFO                                    ENABLED
#define VSF_USE_SIMPLE_STREAM                           ENABLED

#define USRAPP_CFG_FAKEFAT32                            ENABLED

#if APP_USE_LWIP_DEMO == ENABLED
// fix priority for lwip
#   define TCPIP_THREAD_PRIO                            vsf_prio_0
#endif

#if APP_USE_SDL2_DEMO == ENABLED
#   define APP_CFG_SDL2_DEMO_COLOR_RGB666
#endif

#if APP_USE_BTSTACK_DEMO == ENABLED
#   define VSF_USBH_BTHCI_CFG_SCO_IN_NUM                0
#   define VSF_USBH_BTHCI_CFG_SCO_OUT_NUM               0
#   define VSF_USBH_BTHCI_CFG_ACL_IN_NUM                1
#   define VSF_USBH_BTHCI_CFG_ACL_OUT_NUM               1
#endif

// WEAK, why esp32s2 environment does not support weak functions?
#if APP_USE_SCSI_DEMO == ENABLED
#   define WEAK_VSF_SCSI_ON_NEW
#   define WEAK_VSF_SCSI_ON_DELETE
#endif
#if APP_USE_BTSTACK_DEMO == ENABLED && APP_USE_USBH_DEMO == ENABLED
#   define WEAK_VSF_USBH_BTHCI_ON_NEW
#   define WEAK_VSF_USBH_BTHCI_ON_DEL
#   define WEAK_VSF_USBH_BTHCI_ON_PACKET

#   define WEAK_VSF_BLUETOOTH_H2_ON_NEW
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif      // __VSF_USR_CFG_ESP32S2_H__
/* EOF */
