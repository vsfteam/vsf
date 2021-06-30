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

#ifndef __VSF_USR_CFG_AIC8800_H__
#define __VSF_USR_CFG_AIC8800_H__

/*============================ INCLUDES ======================================*/

// for uint32_t of SystemCoreClock
#include <stdint.h>

/*============================ MACROS ========================================*/

//extern uint32_t SystemCoreClock;
// seems systick runs at 240MHz instead of SystemCoreClock
#define VSF_SYSTIMER_FREQ                               (240UL * 1000 * 1000)
// lwip APIs are protected by a mutex, so priority boost SHOULD be supported
//#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                     ENABLED
// configure pool and heap to avoid heap allocating in interrupt
#define VSF_OS_CFG_EVTQ_POOL_SIZE                       128
#define VSF_OS_CFG_DEFAULT_TASK_FRAME_POOL_SIZE         32
#define VSF_POOL_CFG_FEED_ON_HEAP                       DISABLED
// enable VSF_EDA_QUEUE_CFG_REGION and VSF_EDA_QUEUE_CFG_SUPPORT_ISR
#define VSF_EDA_QUEUE_CFG_REGION                        ENABLED
#define VSF_EDA_QUEUE_CFG_SUPPORT_ISR                   ENABLED

// Use unused interrupt as SWI
#define VSF_DEV_SWI_NUM                                 4
#define VSF_DEV_SWI_LIST                                WDT3_IRQn, WDT2_IRQn, WDT1_IRQn, I2CS_IRQn
#define SWI0_IRQHandler                                 WDT3_IRQHandler
#define SWI1_IRQHandler                                 WDT2_IRQHandler
#define SWI2_IRQHandler                                 WDT1_IRQHandler
#define SWI3_IRQHandler                                 I2CS_IRQHandler

// Application configure
#define APP_USE_LINUX_DEMO                              ENABLED
#   define APP_USE_LINUX_LIBUSB_DEMO                    DISABLED
#   define APP_USE_LINUX_MOUNT_FILE_DEMO                DISABLED
#define APP_USE_USBH_DEMO                               DISABLED
// Actually, usbd use usbip_dcd or distbus_dcd, no conflicts with hardware usb host
#define APP_USE_USBD_DEMO                               DISABLED
#   define APP_USE_USBD_CDC_DEMO                        ENABLED
#   define APP_USE_USBD_MSC_DEMO                        ENABLED
#   define APP_USE_USBD_UVC_DEMO                        ENABLED
#   define APP_USE_USBD_UAC_DEMO                        ENABLED
#   define APP_USE_USBD_USER_DEMO                       ENABLED
#define APP_USE_SCSI_DEMO                               DISABLED
#define APP_USE_AUDIO_DEMO                              DISABLED
#define APP_USE_SDL2_DEMO                               DISABLED
#define APP_USE_DISP_DEMO                               DISABLED
#define APP_USE_MS52x_DEMO                              DISABLED
#define VSF_TEST_AIC8800_GPIO                           ENABLED
// if using dl1x5, only RGB565 is supported
#   define APP_SDL2_DEMO_CFG_COLOR_RGB565
#   define APP_SDL2_DEMO_CFG_WIDTH                      256
#   define APP_SDL2_DEMO_CFG_HEIGHT                     256
//  current tgui demo depends on VSF_DISP_USE_SDL2, which is only available on __WIN__
#define APP_USE_TGUI_DEMO                               DISABLED
//  TODO: need test for c++ support
#define APP_USE_CPP_DEMO                                DISABLED
#if APP_USE_CPP_DEMO == ENABLED
#   define __VSF_WORKAROUND_IAR_CPP__
#endif
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
//#   define APP_LVGL_DEMO_USE_TERMINAL                   ENABLED
//#   define APP_LVGL_DEMO_CFG_ANIMINATION                ENABLED

#   define APP_LVGL_DEMO_USE_TOUCHSCREEN                ENABLED
#   define APP_LVGL_DEMO_CFG_TOUCH_REMAP                ENABLED
#   define APP_LVGL_DEMO_CFG_FREETYPE                   ENABLED
#   define APP_LVGL_DEMO_CFG_FREETYPE_MAX_FACES         4
// if using dl1x5(DL1X5 chips from DisplayLink), color_depth should be 16,
//  and hor_res/ver_res should smaller than the hardware resolution
#   define APP_LVGL_DEMO_CFG_COLOR_DEPTH                16
#   define APP_LVGL_DEMO_CFG_HOR_RES                    1920
#   define APP_LVGL_DEMO_CFG_VER_RES                    1080
#   define APP_LVGL_DEMO_CFG_PIXEL_BUFFER_SIZE          (80 * 1024)
// double 80K 16-bit pixels buffer at 0x00100000
#   define APP_LVGL_DEMO_CFG_PIXEL_BUFFER_PTR           0x00100000
#   define APP_LVGL_DEMO_CFG_DOUBLE_BUFFER              ENABLED
#define APP_USE_BTSTACK_DEMO                            DISABLED
// DO NOT use bthci, use on-chip bluetooth
#   define VSF_USBH_USE_BTHCI                           DISABLED
#define APP_USE_VSFVM_DEMO                              DISABLED
// select one for tcpip stack
#define APP_USE_VSFIP_DEMO                              DISABLED
// lwip demo is not compatible with aic8800 sdk, but VSF_USE_LWIP should be defined
#define APP_USE_LWIP_DEMO                               DISABLED
#   define VSF_USE_LWIP                                 ENABLED
#define APP_USE_EVM_DEMO                                DISABLED
#define APP_USE_LUA_DEMO                                DISABLED
#define APP_USE_COREMARK_DEMO                           ENABLED
//#define APP_USE_DISTBUS_DEMO                            ENABLED
//#   define APP_DISTBUS_DEMO_CFG_LWIP                    ENABLED
//#   define APP_USE_DISTBUS_HAL_DEMO                     ENABLED
//#       define APP_USE_DISTBUS_HAL_USBD_DEMO            ENABLED

// demo for AIC8800
#define VSF_CFG_AIC8800_BASE_SPI_TEST                   DISABLED
#define AIC8800_APP_USE_WIFI_DEMO                       DISABLED
#define AIC8800_APP_USE_BT_DEMO                         DISABLED


#if     APP_USE_TGUI_DEMO == ENABLED || APP_USE_XBOOT_XUI_DEMO == ENABLED       \
    ||  (APP_USE_LVGL_DEMO == ENABLED && APP_LVGL_DEMO_CFG_FREETYPE == ENABLED)
#   define APP_USE_FREETYPE_DEMO                        ENABLED
#endif

// component configure
#define VSF_USE_HEAP                                    ENABLED
#   define VSF_HEAP_CFG_MCB_MAGIC_EN                    ENABLED
#   define VSF_HEAP_SIZE                                0x1800

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
#   define USRAPP_FAKEFAT32_CFG_FONT                    ENABLED

// VSF_HAL_USE_DEBUG_STREAM for hardware debug uart
// VSF_DEBUGGER_CFG_CONSOLE for debug console from debugger
// select one ONLY

// APP_LVGL_DEMO_USE_TERMINAL will overwrite DEBUG_STREAM, so DO NOT use hardware DEBUG_STREAM
#if (APP_USE_LVGL_DEMO == ENABLED && APP_LVGL_DEMO_USE_TERMINAL == ENABLED)
#   define VSF_CFG_DEBUG_STREAM_TX_T                    vsf_stream_t
#   define VSF_CFG_DEBUG_STREAM_RX_T                    vsf_mem_stream_t
#else
#   define VSF_HAL_USE_DEBUG_STREAM                     ENABLED
#endif
//#define VSF_DEBUGGER_CFG_CONSOLE                        VSF_DEBUGGER_CFG_CONSOLE_SEGGER_RTT

//#define VSF_ASSERT(...)
#if     VSF_HAL_USE_DEBUG_STREAM == ENABLED                                     \
    ||  (APP_USE_LVGL_DEMO == ENABLED && APP_LVGL_DEMO_USE_TERMINAL == ENABLED)
#   ifndef VSF_ASSERT
#       define VSF_ASSERT(...)                          if (!(__VA_ARGS__)) {while(1);}
#   endif
#else
#   ifdef __cplusplus
extern "C" {
#   endif
extern void VSF_DEBUG_STREAM_POLL(void);
#   ifdef __cplusplus
}
#   endif
#   ifndef VSF_ASSERT
#       define VSF_ASSERT(...)                          if (!(__VA_ARGS__)) {while(1){VSF_DEBUG_STREAM_POLL();}}
#   endif
#endif

#if     APP_USE_USBD_DEMO == ENABLED                                            \
    ||  (APP_USE_DISTBUS_DEMO == ENABLED && APP_USE_DISTBUS_HAL_SLAVE_DEMO == ENABLED)
#   define VSF_USE_USB_DEVICE                           ENABLED
#   if      APP_USE_DISTBUS_DEMO == ENABLED && APP_USE_DISTBUS_HAL_MASTER_DEMO == ENABLED
#       define VSF_HAL_USE_DISTBUS                      ENABLED
#           define VSF_HAL_USE_DISTBUS_USBD             ENABLED
#   elif    APP_USE_DISTBUS_DEMO == ENABLED && APP_USE_DISTBUS_HAL_SLAVE_DEMO == ENABLED
#       define VSF_USBD_USE_DCD_DWCOTG                  ENABLED
#           define USRAPP_USBD_DWCOTG_CFG_ULPI_EN       true
#           define USRAPP_USBD_DWCOTG_CFG_UTMI_EN       false
#           define USRAPP_USBD_DWCOTG_CFG_VBUS_EN       false
#           define USRAPP_USBD_DWCOTG_CFG_DMA_EN        false
#   else
#       define VSF_USBD_USE_DCD_USBIP                   ENABLED
#           define VSF_USBIP_SERVER_CFG_DEBUG           DISABLED
#           define VSF_USBIP_SERVER_CFG_DEBUG_TRAFFIC   DISABLED
#           define VSF_USBIP_SERVER_CFG_DEBUG_URB       DISABLED
#   endif
#       define VSF_USBD_CFG_EDA_PRIORITY                vsf_prio_0
#       define VSF_USBD_CFG_HW_PRIORITY                 vsf_arch_prio_0
#       define USRAPP_CFG_USBD_SPEED                    USB_SPEED_HIGH
#       define USRAPP_CFG_CDC_NUM                       1
#       define USRAPP_CFG_CDC_TX_STREAM_SIZE            1024
#       define USRAPP_CFG_CDC_RX_STREAM_SIZE            512
#endif

#if APP_USE_USBH_DEMO == ENABLED
#   define VSF_USBH_USE_HCD_DWCOTG                      ENABLED
//  VSF_DWCOTG_HCD_CFG_ENABLE_ROOT_HUB is by default disabled, no need root_hub support
#   define VSF_USBH_CFG_ENABLE_ROOT_HUB                 DISABLED
#   define VSF_USBH_USE_DL1X5                           ENABLED
#       define VSF_DISP_USE_DL1X5                       ENABLED
//  enable VSF_USBH_DL1X5_CFG_READ_EDID only if EDID is not available,
//  rewrite vsf_dl1x5_on_new_dev to specify VGA timings
//#           define VSF_USBH_DL1X5_CFG_READ_EDID         DISABLED
#   define VSF_USBH_USE_HUB                             ENABLED
#endif

#if APP_USE_LWIP_DEMO == ENABLED
// fix priority for lwip
#   define TCPIP_THREAD_PRIO                            vsf_prio_0
#endif

#define USRAPP_CFG_USBD_DEV                             VSF_USB_DC0

#if APP_USE_BTSTACK_DEMO == ENABLED
#   if VSF_USBH_USE_BTHCI == ENABLED
#       define VSF_USBH_BTHCI_CFG_SCO_IN_NUM            0
#       define VSF_USBH_BTHCI_CFG_SCO_OUT_NUM           0
#       define VSF_USBH_BTHCI_CFG_ACL_IN_NUM            1
#       define VSF_USBH_BTHCI_CFG_ACL_OUT_NUM           1
#   endif
#endif

#if APP_USE_SDL2_DEMO == ENABLED
#   define VSF_SDL_CFG_WINDOW_PTR                       0x00100000
#   define VSF_SDL_CFG_DISPLAY_HEIGHT                   320
#   define VSF_SDL_CFG_DISPLAY_WIDTH                    480
#endif

#define VSF_LINUX_CFG_STACKSIZE                         (4 * 1024)
#define VSF_TRACE_CFG_COLOR_EN                          ENABLED
#define VSH_HAS_COLOR                                   1

#if APP_USE_LVGL_DEMO == ENABLED && APP_LVGL_DEMO_USE_TERMINAL == ENABLED
// priority of debug stream should be higher than linux kernel(vsf_prio_0)
#   define LV_TERMIAL_APP_STREAM_TX_PRIO                vsf_prio_1

// TODO: lvgl terminal demo support color
#	undef  VSF_TRACE_CFG_COLOR_EN
#	define VSF_TRACE_CFG_COLOR_EN                       DISABLED
#   undef VSH_HAS_COLOR
#	define VSH_HAS_COLOR                                0
#endif
#define VSF_DISP_USE_MIPI_LCD                           DISABLED
#   define WEAK_VK_DISP_MIPI_TE_LINE_ISR_ENABLE_ONCE
#   define WEAK_VK_DISP_MIPI_LCD_IO_INIT
#   define VK_DISP_MIPI_LCD_SUPPORT_HARDWARE_RESET      ENABLED


#   define APP_DISP_DEMO_COLOR                          VSF_DISP_COLOR_RGB565
#   define APP_DISP_DEMO_HEIGHT                         160
#   define APP_DISP_DEMO_WIDTH                          128
//#   define APP_DISP_DEMO_SEQ                            VSF_DISP_MIPI_LCD_ILI9341
#   define APP_DISP_DEMO_SEQ                            VSF_DISP_MIPI_LCD_ST7796S


#define VSF_USE_UI                                      ENABLED

#define VSF_DISP_MIPI_LCD_USE_SPI_INTERFACE				DISABLED
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif      // __VSF_USR_CFG_AIC8800_H__
/* EOF */
