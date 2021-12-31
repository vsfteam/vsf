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


//! \note Top Level Application Configuration

#ifndef __TOP_APP_CFG_H__
#define __TOP_APP_CFG_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define VSF_ASSERT(...)                         if (!(__VA_ARGS__)) {while(1);};
//#define VSF_ASSERT(...)

#define APP_CFG_USBD_VID                        0xA7A8
#define APP_CFG_USBD_PID                        0x2348
#define APP_CFG_USBD_EP0_SIZE                   64
#define APP_CFG_USBD_VENDOR_STR                 u"vsf"
#define APP_CFG_USBD_PRODUCT_STR                u"vsf_dfu"
#define APP_CFG_USBD_SERIAL_STR                 u"0000"
#define APP_CFG_USBD_WEBUSB_URL                 "devanlai.github.io/webdfu/dfu-util/"

#define VSF_OS_CFG_PRIORITY_NUM                 1
#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE             DISABLED
#define VSF_KERNEL_CFG_SUPPORT_SYNC             DISABLED
#define VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED DISABLED
#define VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL     DISABLED
#define VSF_KERNEL_CFG_EDA_SUPPORT_TIMER        DISABLED
#define VSF_KERNEL_CFG_CALLBACK_TIMER           DISABLED
#define VSF_KERNEL_CFG_SUPPORT_MSG_QUEUE        DISABLED
#define VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT     DISABLED
#define VSF_OS_CFG_MAIN_MODE                    VSF_OS_CFG_MAIN_MODE_IDLE
#define VSF_USE_KERNEL_SIMPLE_SHELL             DISABLED
#define VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE          DISABLED
#define VSF_KERNEL_CFG_SUPPORT_THREAD           DISABLED
#define VSF_KERNEL_CFG_EDA_SUPPORT_PT           DISABLED
#define VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE DISABLED

#define VSF_USE_USB_DEVICE                      ENABLED
#   define VSF_USBD_CFG_USE_EDA                 DISABLED
#   define VSF_USBD_CFG_HW_PRIORITY             vsf_arch_prio_0
#   define VSF_USBD_CFG_EDA_PRIORITY            vsf_prio_0
#   define VSF_USBD_CFG_RAW_MODE                ENABLED
#   define VSF_USBD_CFG_STREAM_EN               DISABLED

#define VSF_USE_TRACE                           ENABLED
#define VSF_USE_FIFO                            ENABLED
#define VSF_USE_SIMPLE_STREAM                   ENABLED

// heap is enabled by default, disable manually
#define VSF_USE_HEAP                            DISABLED

#if defined(__M484__)

#define VSF_SYSTIMER_FREQ                       (192000000ul)
#define VSF_DEBUGGER_CFG_CONSOLE                VSF_DEBUGGER_CFG_CONSOLE_NULINK_NUCONSOLE

#   if VSF_USE_USB_DEVICE == ENABLED
#       define APP_CFG_USBD_SPEED               USB_DC_SPEED_HIGH
#       define VSF_USBD_CFG_DRV_LV0             m480_usbd_hs
#       define VSF_USBD_CFG_DRV_OBJ             USB_DC0
#   endif

#elif defined(__WIN__)

#define VSF_SYSTIMER_FREQ                       (0ul)
#define VSF_HAL_USE_DEBUG_STREAM                ENABLED

#if VSF_USE_USB_DEVICE == ENABLED
#   define VSF_USBD_USE_DCD_USBIP               ENABLED
#   define APP_CFG_USBD_SPEED                   USB_DC_SPEED_HIGH

#   define VSF_USBD_CFG_DRV_LV0_OO
#       define VSF_USBD_CFG_DRV_LV0_OO_PREFIX   vk_usbip_usbd
#       define VSF_USBD_CFG_DRV_LV0_OO_OBJ      usrapp_usbd_common.usbip_dcd
#   define VSF_USBD_CFG_DRV_LV0_OO_OBJ_HEADER   "usrapp_usbd_common.h"

    // usbip_dcd depends on timer support
#   undef VSF_KERNEL_CFG_EDA_SUPPORT_TIMER
#   define VSF_KERNEL_CFG_EDA_SUPPORT_TIMER     ENABLED

    // usbip_dcd depends on heap
#   undef VSF_USE_HEAP
#   define VSF_USE_HEAP                         ENABLED
#endif

//  weak

#if VSF_USE_USB_DEVICE == ENABLED
#   define WEAK_VSF_USBD_NOTIFY_USER
#endif

#define WEAK_VSF_KERNEL_ERR_REPORT
#define WEAK___POST_VSF_KERNEL_INIT
#define WEAK_VSF_DRIVER_INIT

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
#   define WEAK_VSF_SYSTIMER_EVTHANDLER
#endif

#if VSF_USE_HEAP == ENABLED
#   define WEAK_VSF_HEAP_MALLOC_ALIGNED
#endif

#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */