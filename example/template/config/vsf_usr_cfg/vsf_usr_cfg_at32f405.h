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

#ifndef __VSF_USR_CFG_AT32F405_H__
#define __VSF_USR_CFG_AT32F405_H__

/*============================ INCLUDES ======================================*/

#define __VSF_HEADER_ONLY_SHOW_COMPILER_INFO__
#include "utilities/compiler/compiler.h"

/*============================ MACROS ========================================*/

#if __IS_COMPILER_IAR__
#   define VSF_KERNEL_CFG_THREAD_STACK_CHECK            ENABLED
#   define VSF_KERNEL_GET_STACK_FROM_JMPBUF(__JMPBUF)   ((*(__JMPBUF))[4] & 0xFFFFFFFF)
#elif __IS_COMPILER_GCC__ || __IS_COMPILER_LLVM__
// strtoxxx in newlib has dependency issues, implement in simple_stdlib
#   define VSF_LINUX_SIMPLE_STDLIB_USE_STRTOXX          ENABLED
#   define VSF_USE_SIMPLE_SPRINTF                       ENABLED
#   define VSF_USE_SIMPLE_SSCANF                        ENABLED
#endif

// configure pool and heap to avoid heap allocating in interrupt
#define VSF_OS_CFG_EVTQ_POOL_SIZE                       128
#define VSF_POOL_CFG_FEED_ON_HEAP                       DISABLED
// enable VSF_EDA_QUEUE_CFG_REGION and VSF_EDA_QUEUE_CFG_SUPPORT_ISR
#define VSF_EDA_QUEUE_CFG_REGION                        ENABLED
#define VSF_EDA_QUEUE_CFG_SUPPORT_ISR                   ENABLED

// Application configure
#define APP_USE_LINUX_DEMO                              ENABLED
#   define APP_USE_LINUX_LIBUSB_DEMO                    ENABLED
#   define APP_USE_LINUX_MOUNT_DEMO                     DISABLED
#   define APP_USE_LINUX_HTTPD_DEMO                     DISABLED
#define APP_USE_USBH_DEMO                               DISABLED
// Actually, usbd use usbip_dcd or distbus_dcd, no conflicts with hardware usb host
#define APP_USE_USBD_DEMO                               ENABLED
#   define APP_USE_USBD_CDC_DEMO                        ENABLED
#   define APP_USE_USBD_MSC_DEMO                        ENABLED
#   define APP_USE_USBD_UVC_DEMO                        ENABLED
#   define APP_USE_USBD_UAC_DEMO                        ENABLED
#   define APP_USE_USBD_USER_DEMO                       ENABLED

//  TODO: CPP support is not ready in hal
#define APP_USE_CPP_DEMO                                DISABLED
#   if APP_USE_CPP_DEMO == ENABLED
#       define __VSF_CPP__
#   endif
#define APP_USE_HAL_DEMO                                ENABLED
#   define APP_USE_HAL_ADC_DEMO                         DISABLED
#   define APP_USE_HAL_FLASH_DEMO                       DISABLED
#   define APP_USE_HAL_GPIO_DEMO                        DISABLED
#   define APP_USE_HAL_I2C_DEMO                         DISABLED
#   define APP_USE_HAL_PWM_DEMO                         DISABLED
#   define APP_USE_HAL_RTC_DEMO                         DISABLED
#   define APP_USE_HAL_SPI_DEMO                         ENABLED
#   define APP_USE_HAL_TIMER_DEMO                       DISABLED
#   define APP_USE_HAL_USART_DEMO                       DISABLED
#   define APP_USE_HAL_WDT_DEMO                         ENABLED
#   define APP_USE_HAL_DMA_DEMO                         ENABLED


// component configure
#define VSF_USE_HEAP                                    ENABLED
#   define VSF_HEAP_CFG_MCB_MAGIC_EN                    ENABLED
#   define VSF_HEAP_CFG_MCB_ALIGN_BIT                   4
#   define VSF_HEAP_SIZE                                0x10000

#define VSF_USE_VIDEO                                   ENABLED
#define VSF_USE_AUDIO                                   ENABLED
#   define VSF_AUDIO_USE_DECODER_WAV                    ENABLED
#   define VSF_AUDIO_USE_PLAYBACK                       ENABLED
#   define VSF_AUDIO_USE_CATURE                         DISABLED

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
#   define VSF_LINUX_USE_SOCKET                         ENABLED
#       define VSF_LINUX_SOCKET_USE_INET                VSF_USE_LWIP
#       define VSF_LINUX_SOCKET_USE_UNIX                ENABLED
#   define VSF_LINUX_USE_DEVFS                          ENABLED
#       define VSF_LINUX_DEVFS_USE_RAND                 ENABLED

#   define VSF_LINUX_USE_SIMPLE_LIBC                    ENABLED
#       if __IS_COMPILER_IAR__
// IAR: to support CPP in linux-subsystem, define MACFOs below, and don't include the simple_libc path
//  Note that __VSF_CPP__ MUST also be defined to support CPP
// TODO: enable cpp support
//#           define VSF_LINUX_LIBC_CFG_CPP               ENABLED
#           define VSF_LINUX_USE_SIMPLE_STDIO           ENABLED
#           define VSF_LINUX_USE_SIMPLE_STRING          ENABLED
#           define VSF_LINUX_USE_SIMPLE_TIME            ENABLED
#           define VSF_LINUX_USE_SIMPLE_STDLIB          ENABLED
#           define VSF_LINUX_USE_SIMPLE_CTYPE           ENABLED
#       else
//#           define VSF_LINUX_LIBC_CFG_CPP               ENABLED
#           define VSF_LINUX_USE_SIMPLE_STDIO           ENABLED
#           define VSF_LINUX_USE_SIMPLE_STRING          ENABLED
#           define VSF_LINUX_USE_SIMPLE_TIME            ENABLED
#           define VSF_LINUX_USE_SIMPLE_STDLIB          ENABLED
#           define VSF_LINUX_USE_SIMPLE_CTYPE           ENABLED
#       endif

#ifndef USRAPP_CFG_LINUX_TTY_DEBUG_STREAM
#   define USRAPP_CFG_LINUX_TTY_DEBUG_STREAM            0
#   define USRAPP_CFG_LINUX_TTY_UART                    1
#   define USRAPP_CFG_LINUX_TTY_CDC                     2
#endif
#   define USRAPP_CFG_LINUX_TTY                         USRAPP_CFG_LINUX_TTY_DEBUG_STREAM

#define VSF_USE_STREAM                                  DISABLED
#define VSF_USE_SIMPLE_STREAM                           ENABLED

#define USRAPP_CFG_FAKEFAT32                            ENABLED
#   define USRAPP_FAKEFAT32_CFG_FONT                    DISABLED

// VSF_HAL_USE_DEBUG_STREAM for hardware debug uart
// VSF_DEBUGGER_CFG_CONSOLE for debug console from debugger
// select one ONLY

#define VSF_CFG_DEBUG_STREAM_TX_T                       vsf_stream_t
#define VSF_CFG_DEBUG_STREAM_RX_DECLARE                 declare_mem_stream(VSF_DEBUG_STREAM_RX)
#define VSF_CFG_DEBUG_STREAM_RX_T                       vsf_mem_stream_type(VSF_DEBUG_STREAM_RX)
//#define VSF_HAL_USE_DEBUG_STREAM                        ENABLED
//#define VSF_DEBUGGER_CFG_CONSOLE                        VSF_DEBUGGER_CFG_CONSOLE_SEGGER_RTT

//#define VSF_ASSERT(...)
#ifndef VSF_ASSERT
#   define VSF_ASSERT(...)                              if (!(__VA_ARGS__)) {while(1);}
#endif

#if     APP_USE_USBD_DEMO == ENABLED                                            \
    ||  (APP_USE_DISTBUS_DEMO == ENABLED && APP_USE_DISTBUS_HAL_SLAVE_DEMO == ENABLED)
#   define VSF_USE_USB_DEVICE                           ENABLED
#   if      APP_USE_DISTBUS_DEMO == ENABLED && APP_USE_DISTBUS_HAL_MASTER_DEMO == ENABLED
#       define VSF_HAL_USE_DISTBUS                      ENABLED
#           define VSF_HAL_USE_DISTBUS_USBD             ENABLED
#   elif    APP_USE_DISTBUS_DEMO == ENABLED && APP_USE_DISTBUS_HAL_SLAVE_DEMO == ENABLED
#       define VSF_USBD_USE_DCD_DWCOTG                  ENABLED
#           define USRAPP_USBD_DWCOTG_CFG_ULPI_EN       false
#           define USRAPP_USBD_DWCOTG_CFG_UTMI_EN       false
#           define USRAPP_USBD_DWCOTG_CFG_VBUS_EN       false
#           define USRAPP_USBD_DWCOTG_CFG_DMA_EN        false
#   else
#       define VSF_USBD_USE_DCD_DWCOTG                  ENABLED
#           define USRAPP_USBD_DWCOTG_CFG_ULPI_EN       false
#           define USRAPP_USBD_DWCOTG_CFG_UTMI_EN       false
#           define USRAPP_USBD_DWCOTG_CFG_VBUS_EN       false
#           define USRAPP_USBD_DWCOTG_CFG_DMA_EN        false
#   endif
#       define VSF_USBD_CFG_EDA_PRIORITY                vsf_prio_0
#       define VSF_USBD_CFG_HW_PRIORITY                 vsf_arch_prio_0
#       define USRAPP_CFG_USBD_SPEED                    USB_SPEED_FULL
#       define USRAPP_CFG_CDC_NUM                       1
#       define USRAPP_CFG_CDC_TX_STREAM_SIZE            1024
#       define USRAPP_CFG_CDC_RX_STREAM_SIZE            512
#endif

#if APP_USE_USBH_DEMO == ENABLED
#   define VSF_USBH_USE_HCD_DWCOTG                      ENABLED
//  VSF_DWCOTG_HCD_CFG_ENABLE_ROOT_HUB is by default disabled, no need root_hub support
#   define VSF_USBH_CFG_ENABLE_ROOT_HUB                 DISABLED
#   define VSF_USBH_USE_HUB                             ENABLED
#endif

#define USRAPP_CFG_USBD_DEV                             VSF_USB_DC0

#define VSF_LINUX_CFG_STACKSIZE                         (8 * 1024)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif      // __VSF_USR_CFG_AT32F405_H__
/* EOF */
