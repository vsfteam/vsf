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

#ifndef __VSF_USR_CFG_LINUX_H__
#define __VSF_USR_CFG_LINUX_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

// arch configure
// linux support does not support preemption, so need evtq in idle
#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                     ENABLED

#define VSF_ASSERT(...)                                 assert(__VA_ARGS__)
//#define VSF_ASSERT(...)

#define VSF_HAL_USE_DEBUG_STREAM                        ENABLED
#define VSF_SYSTIMER_FREQ                               (0ul)

// necessary to avoid segment fault
#define VSF_USE_SIMPLE_SSCANF                           ENABLED
#define VSF_USE_SIMPLE_SPRINTF                          ENABLED

// Application configure

// app configurations to vsf configurations

// component configure
#define VSF_HEAP_SIZE                                   0x1000000
#define VSF_HEAP_CFG_MCB_ALIGN_BIT                      12      // 4K alignment

#define VSF_USBH_CFG_ENABLE_ROOT_HUB                    DISABLED
#define VSF_USBH_USE_HUB                                DISABLED
#define VSF_USBH_USE_HCD_LIBUSB                         ENABLED

#if VSF_USBH_USE_HCD_LIBUSB == ENABLED
#   define VSF_LIBUSB_CFG_INCLUDE                       "lib\libusb\libusb.h"
#   define VSF_LIBUSB_HCD_CFG_DEV_NUM                   1
#   define VSF_LIBUSB_HCD_DEV0_VID                      0x0A5C      // BCM20702 bthci
#   define VSF_LIBUSB_HCD_DEV0_PID                      0x21E8
#endif

#define VSF_DISP_USE_SDL2                               ENABLED
#   define VSF_DISP_SDL2_CFG_INCLUDE                    <SDL2/SDL.h>
#   define VSF_DISP_SDL2_CFG_MOUSE_AS_TOUCHSCREEN       ENABLED
#   define VSF_DISP_SDL2_CFG_HW_PRIORITY                vsf_arch_prio_0
#   define APP_DISP_SDL2_HEIGHT                         768
#   define APP_DISP_SDL2_WIDTH                          1024
#   define APP_DISP_SDL2_TITLE                          "vsf_screen"
#   define APP_DISP_SDL2_COLOR                          VSF_DISP_COLOR_RGB565
#   define APP_DISP_SDL2_AMPLIFIER                      1

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif      // __VSF_USR_CFG_LINUX_H__
/* EOF */
