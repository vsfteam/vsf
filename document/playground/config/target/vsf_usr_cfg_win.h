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

#ifndef __VSF_USR_CFG_WIN_H__
#define __VSF_USR_CFG_WIN_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

// enable the 4 configurations below to use simple implementation for x86
//  simple implementation will not use ThreadSuspend and ThreadResume and has better CPU usage
//  **** but preempt is not supported ****
//  priority configurations are dependent on MACROs below, so put them here(at top)
//  IMPORTANT: vsf_arch_sleep MUST be called in vsf_plug_in_on_kernel_idle
#define VSF_ARCH_LIMIT_NO_SET_STACK
#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                     ENABLED
#define __VSF_X86_WIN_SINGLE_PRIORITY
//#define __WIN7__

#define VSF_ASSERT(...)                                 assert(__VA_ARGS__)
//#define VSF_ASSERT(...)

#define VSF_HAL_USE_DEBUG_STREAM                        ENABLED
#   define VSF_DEBUG_STREAM_CFG_HW_PRIORITY             vsf_arch_prio_0

#define VSF_HEAP_SIZE                                   0x2000000
#define VSF_HEAP_CFG_MCB_ALIGN_BIT                      12      // 4K alignment

#define VSF_FS_USE_WINFS                                ENABLED
#define VSF_DISP_USE_WINGDI                             ENABLED

#ifdef __WIN7__
// winusb seems fail on win7
#   define VSF_USBH_USE_HCD_LIBUSB                      ENABLED
#       define VSF_LIBUSB_CFG_INSTALL_DRIVER            DISABLED
// console for win7 does not support color by default
#   define VSF_TRACE_CFG_COLOR_EN                       DISABLED
#   define VSH_HAS_COLOR                                0
#   define VSF_WINUSB_CFG_WIN7                          ENABLED
#else
#   define VSF_USBH_USE_HCD_WINUSB                      ENABLED
#       define VSF_WINUSB_CFG_INSTALL_DRIVER            DISABLED
#    define VSF_TRACE_CFG_COLOR_EN                      ENABLED
#endif
#if VSF_USBH_USE_HCD_WINUSB == ENABLED
#   define VSF_WINUSB_HCD_CFG_DEV_NUM                   1
#   define VSF_WINUSB_HCD_DEV0_VID                      0x0A5C      // BCM20702 bthci
#   define VSF_WINUSB_HCD_DEV0_PID                      0x21E8
#elif VSF_USBH_USE_HCD_LIBUSB == ENABLED
// for libusb
//#   define VSF_LIBUSB_CFG_INCLUDE                       "lib\libusb\libusb.h"
// for libusb-win32
#   define VSF_LIBUSB_CFG_INCLUDE                       "lib\libusb-win32\lusb0_usb.h"
#   define VSF_LIBUSB_HCD_CFG_DEV_NUM                   1
#   define VSF_LIBUSB_HCD_DEV0_VID                      0x0A5C      // BCM20702 bthci
#   define VSF_LIBUSB_HCD_DEV0_PID                      0x21E8
#endif
#if VSF_USBH_USE_HCD_WINUSB == ENABLED || VSF_USBH_USE_HCD_LIBUSB == ENABLED
#   define VSF_USBH_CFG_ENABLE_ROOT_HUB                 DISABLED
#   define VSF_USBH_USE_HUB                             DISABLED
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif      // __VSF_USR_CFG_WIN_H__
/* EOF */
