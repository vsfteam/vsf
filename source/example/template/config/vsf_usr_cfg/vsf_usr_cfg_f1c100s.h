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

#ifndef __VSF_USR_CFG_F1C100S_H__
#define __VSF_USR_CFG_F1C100S_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

// Application configure
#define APP_USE_LINUX_DEMO                              ENABLED
#   define APP_USE_LINUX_LIBUSB_DEMO                    ENABLED
#   define APP_USE_LINUX_MOUNT_FILE_DEMO                DISABLED
//  todo: implement drivers for f1c100s
#define APP_USE_USBH_DEMO                               ENABLED
#define APP_USE_USBD_DEMO                               DISABLED
#   define APP_USE_USBD_CDC_DEMO                        ENABLED
#   define APP_USE_USBD_MSC_DEMO                        ENABLED
#   define APP_USE_USBD_UVC_DEMO                        ENABLED
#   define APP_USE_USBD_UAC_DEMO                        ENABLED
#define APP_USE_SCSI_DEMO                               ENABLED
#define APP_USE_AUDIO_DEMO                              DISABLED
#define APP_USE_SDL2_DEMO                               ENABLED
//  current tgui demo depends on VSF_DISP_USE_SDL2, which is only available on __WIN__
#define APP_USE_TGUI_DEMO                               DISABLED
//  TODO: need test for c++ support
#define APP_USE_CPP_DEMO                                DISABLED
#define APP_USE_HAL_DEMO                                ENABLED
#   define APP_USE_HAL_USART_DEMO                       ENABLED
// TODO: USART DMA is implemented in f1c100s, so REQUEST_TEST is not supported
#       define APP_USART_DEMO_CFG_REQUEST_TEST              DISABLED
#       define APP_USART_DEMO_CFG_FIFO_WRITE_WITH_ISR_TEST  DISABLED
#       define APP_USART_DEMO_CFG_FIFO_READ_WITH_ISR_TEST   DISABLED
#       define APP_USART_DEMO_CFG_FIFO_ECHO_TEST            ENABLED

// 3rd-party demos
//  awtk is LGPL, not convenient to implement in MCU
#define APP_USE_AWTK_DEMO                               DISABLED
#define APP_USE_NNOM_DEMO                               ENABLED
#define APP_USE_LVGL_DEMO                               ENABLED
#   define APP_LVGL_DEMO_USE_TOUCHSCREEN                ENABLED
#   define APP_LVGL_DEMO_CFG_TOUCH_REMAP                ENABLED
#   define APP_LVGL_DEMO_CFG_COLOR_DEPTH                32
#   define APP_LVGL_DEMO_CFG_HOR_RES                    640
#   define APP_LVGL_DEMO_CFG_VER_RES                    480
#define APP_USE_BTSTACK_DEMO                            ENABLED
#define APP_USE_VSFVM_DEMO                              ENABLED
// select one for tcpip stack
#define APP_USE_VSFIP_DEMO                              DISABLED
#define APP_USE_LWIP_DEMO                               ENABLED


// component configure
#define VSF_USE_HEAP                                    ENABLED
#   define VSF_HEAP_CFG_MCB_MAGIC_EN                    ENABLED
#   define VSF_HEAP_SIZE                                0x1000000
#   define VSF_HEAP_CFG_MCB_ALIGN_BIT                   12      // 4K alignment

#define VSF_USE_VIDEO                                   ENABLED
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
#   define VSF_LINUX_USE_LIBUSB                         VSF_USE_USB_HOST
#   define VSF_LINUX_USE_BUSYBOX                        ENABLED
#   define VSF_LINUX_USE_SIMPLE_LIBC                    ENABLED

#ifndef USRAPP_CFG_LINUX_TTY_DEBUT_STREAM
#   define USRAPP_CFG_LINUX_TTY_DEBUG_STREAM            0
#   define USRAPP_CFG_LINUX_TTY_UART                    1
#   define USRAPP_CFG_LINUX_TTY_CDC                     2
#endif
#   define USRAPP_CFG_LINUX_TTY                         USRAPP_CFG_LINUX_TTY_DEBUG_STREAM

#define VSF_USE_STREAM                                  DISABLED
#define VSF_USE_SIMPLE_STREAM                           ENABLED


#ifndef USRAPP_CFG_DCD_TYPE_DEFAULT
#   define USRAPP_CFG_DCD_TYPE_DEFAULT                  0
#   define USRAPP_CFG_DCD_TYPE_DWCOTG                   1
#   define USRAPP_CFG_DCD_TYPE_MUSB_FDRC                2
#endif

#define USRAPP_CFG_FAKEFAT32                            ENABLED

extern void VSF_DEBUG_STREAM_POLL(void);
#define ASSERT(...)                                     if (!(__VA_ARGS__)) {while(1){VSF_DEBUG_STREAM_POLL();}}
//#define ASSERT(...)

#define VSF_HAL_DRV_CFG_SYSTIMER_POLL_MODE_EN           ENABLED

#define VSF_SYSTIMER_FREQ                               0
#define VSF_KERNEL_CFG_TIMER_MODE                       0
#define vsf_teda_set_timer_ms                           vsf_teda_set_timer
#define vsf_callback_timer_add_ms                       vsf_callback_timer_add
#define vsf_systimer_tick_to_ms(__tick)                 (__tick)
#define vsf_systimer_tick_to_us(__tick)                 ((__tick) * 1000)
#define vsf_systimer_ms_to_tick(__ms)                   (__ms)
#define vsf_systimer_us_to_tick(__us)                   ((__us) / 1000)
typedef unsigned int vsf_systimer_cnt_t;
typedef int vsf_systimer_cnt_signed_t;

#define VSF_HAL_USE_DEBUG_STREAM                        ENABLED

//#define VSF_SYSTIMER_FREQ                               (24UL * 1000 * 1000)
#define VSF_OS_CFG_PRIORITY_NUM                         1
#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                     ENABLED
// retarget-io for arm9 in iar seems to be mal-functioning, use dimple_stdio
#define VSF_LINUX_USE_SIMPLE_STDIO                      ENABLED

#if APP_USE_USBD_DEMO == ENABLED
#   define VSF_USE_USB_DEVICE                           ENABLED
//      TODO: use actual priority after arm9 arch is ready
#       define VSF_USBD_CFG_EDA_PRIORITY                vsf_prio_0
#       define VSF_USBD_CFG_HW_PRIORITY                 vsf_arch_prio_0
#       define USRAPP_CFG_USBD_SPEED                    USB_SPEED_HIGH
#       define USRAPP_CFG_CDC_NUM                       1
#       define USRAPP_CFG_CDC_TX_STREAM_SIZE            1024
#       define USRAPP_CFG_CDC_RX_STREAM_SIZE            512
#       define USRAPP_CFG_DCD_TYPE                      USRAPP_CFG_DCD_TYPE_DEFAULT
#       define USRAPP_CFG_STREAM_ALIGN                  1
#endif

#if APP_USE_USBH_DEMO == ENABLED
#   define VSF_USBH_CFG_ENABLE_ROOT_HUB                 DISABLED
#   define VSF_USBH_USE_HUB                             ENABLED
#endif

#if APP_USE_LWIP_DEMO == ENABLED
// fix priority for lwip
#   define TCPIP_THREAD_PRIO                            vsf_prio_0
#endif

#define USRAPP_CFG_USBD_DEV                             VSF_USB_DC0

#define VSF_DISP_USE_FB                                 ENABLED
#   define VSF_DISP_FB_CFG_COPY_FRAME                   true
#   define APP_DISP_FB_COLOR                            VSF_DISP_COLOR_RGB666_32
#   define APP_DISP_FB_NUM                              3
//  for VGA 640 * 480 60Hz
#   define APP_DISP_FB_CFG_PIXEL_CLOCK                  (25UL * 1000 * 1000)
#   define APP_DISP_FB_CFG_H_FP                         16
#   define APP_DISP_FB_CFG_H_BP                         48
#   define APP_DISP_FB_CFG_H_SYNC                       96
#   define APP_DISP_FB_CFG_V_FP                         11
#   define APP_DISP_FB_CFG_V_BP                         31
#   define APP_DISP_FB_CFG_V_SYNC                       2
#   define APP_DISP_FB_WIDTH                            640
#   define APP_DISP_FB_HEIGHT                           480
//  for LCD
//#   define APP_DISP_FB_CFG_PIXEL_CLOCK                  (36UL * 1000 * 1000)
//#   define APP_DISP_FB_CFG_H_FP                         40
//#   define APP_DISP_FB_CFG_H_BP                         40
//#   define APP_DISP_FB_CFG_H_SYNC                       48
//#   define APP_DISP_FB_CFG_V_FP                         13
//#   define APP_DISP_FB_CFG_V_BP                         29
//#   define APP_DISP_FB_CFG_V_SYNC                       3
//#   define APP_DISP_FB_WIDTH                            800
//#   define APP_DISP_FB_HEIGHT                           480

#if APP_USE_SDL2_DEMO == ENABLED
#   define APP_CFG_SDL2_DEMO_COLOR_RGB666
#endif

#if APP_USE_BTSTACK_DEMO == ENABLED
#   define VSF_USBH_BTHCI_CFG_SCO_IN_NUM                0
#   define VSF_USBH_BTHCI_CFG_SCO_OUT_NUM               0
#   define VSF_USBH_BTHCI_CFG_ACL_IN_NUM                1
#   define VSF_USBH_BTHCI_CFG_ACL_OUT_NUM               1
#endif

#define VSF_USBH_CFG_EDA_PRIORITY                       vsf_prio_0
#define APP_CFG_USBH_HW_PRIO                            vsf_arch_prio_0

#define VSF_LINUX_CFG_STACKSIZE                         (60 * 1024)
#define VSF_TRACE_CFG_COLOR_EN                          ENABLED
#define VSH_HAS_COLOR                                   1

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif      // __VSF_USR_CFG_F1C100S_H__
/* EOF */
