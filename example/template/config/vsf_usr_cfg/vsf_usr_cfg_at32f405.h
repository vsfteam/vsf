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

//extern uint32_t SystemCoreClock;
// DO NOT use SystemCoreClock for VSF_SYSTIMER_FREQ, because systimer is initialized
//  in vsf_arch_init, which is earlier than initialization of SystemCoreClock in
//  vsf_driver_init.
#define VSF_SYSTIMER_FREQ                               (216UL * 1000 * 1000)

// configure pool and heap to avoid heap allocating in interrupt
#define VSF_OS_CFG_EVTQ_POOL_SIZE                       128
#define VSF_POOL_CFG_FEED_ON_HEAP                       DISABLED

// Application configure
#define APP_USE_USBH_DEMO                               ENABLED
// Actually, usbd use usbip_dcd or distbus_dcd, no conflicts with hardware usb host
#define APP_USE_USBD_DEMO                               ENABLED
#   define APP_USE_USBD_CDC_DEMO                        ENABLED
#   define APP_USE_USBD_MSC_DEMO                        DISABLED
#   define APP_USE_USBD_UVC_DEMO                        DISABLED
#   define APP_USE_USBD_UAC_DEMO                        DISABLED
#   define APP_USE_USBD_USER_DEMO                       DISABLED

//  TODO: CPP support is not ready in hal
#define APP_USE_CPP_DEMO                                DISABLED
#   if APP_USE_CPP_DEMO == ENABLED
#       define __VSF_CPP__
#   endif

// component configure
#define VSF_USE_HEAP                                    ENABLED
#   define VSF_HEAP_CFG_MCB_MAGIC_EN                    ENABLED
#   define VSF_HEAP_SIZE                                0x2000

// VSF_USE_USB_DEVICE will be enabled if target chip supports USBD
#define VSF_USE_USB_DEVICE                              DISABLED
#   define VSF_USBD_CFG_USE_EDA                         ENABLED
#   define VSF_USBD_USE_CDCACM                          ENABLED
#   define VSF_USBD_USE_MSC                             DISABLED
#   define VSF_USBD_USE_UVC                             DISABLED
#   define VSF_USBD_USE_UAC                             DISABLED
#   define VSF_USBD_USE_HID                             DISABLED
#   define APP_CFG_USBD_VID                             0xA7A8
#   define APP_CFG_USBD_PID                             0x2348

#define VSF_USE_TRACE                                   DISABLED
#define USRAPP_CFG_STDIO_EN                             ENABLED

#define VSF_USE_LINUX                                   DISABLED

#define VSF_USE_STREAM                                  DISABLED
#define VSF_USE_SIMPLE_STREAM                           ENABLED

#define USRAPP_CFG_FAKEFAT32                            DISABLED
#   define USRAPP_FAKEFAT32_CFG_FONT                    DISABLED

//#define VSF_ASSERT(...)
#ifndef VSF_ASSERT
#   define VSF_ASSERT(...)                              if (!(__VA_ARGS__)) {while(1);}
#endif

#if APP_USE_USBD_DEMO == ENABLED
#   define VSF_USBD_CFG_EDA_PRIORITY                    vsf_prio_0
#   define VSF_USBD_CFG_HW_PRIORITY                     vsf_arch_prio_0
#   define USRAPP_CFG_USBD_SPEED                        USB_SPEED_FULL

#   define VSF_USBH_USE_HCD_DWCOTG                      ENABLED

#   define VSF_USBD_USE_DCD_DWCOTG                      ENABLED
#   define VSF_USBD_CFG_SPEED                           USB_SPEED_FULL


#endif

#if APP_USE_USBH_DEMO == ENABLED
#   define VSF_USBH_USE_HCD_DWCOTG                      ENABLED
#   define VSF_USBH_CFG_ENABLE_ROOT_HUB                 DISABLED
#   define VSF_USBH_USE_HUB                             ENABLED
#endif

#define USRAPP_CFG_USBD_DEV                             VSF_USB_DC0

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif      // __VSF_USR_CFG_AT32F405_H__
/* EOF */
