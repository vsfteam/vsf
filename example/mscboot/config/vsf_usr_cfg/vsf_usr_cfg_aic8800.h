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
#define VSF_POOL_CFG_FEED_ON_HEAP                       DISABLED

// Application configure
#define __MSCBOT_BOOTLOADER_SIZE                        (64 * 1024)
// reserve __MSCBOT_BOOTLOADER_SIZE and 16K reserved configuration area for wifi
#define MSCBOT_CFG_FW_SIZE                              ((2 * 1024 * 1024) - __MSCBOT_BOOTLOADER_SIZE - (16 * 1024))
#define MSCBOT_CFG_FW_ADDR                              __MSCBOT_BOOTLOADER_SIZE

#define APP_CFG_USBD_VID                                0xA7A8
#define APP_CFG_USBD_PID                                0x2348

#define VSF_USBD_USE_DCD_DWCOTG                         ENABLED
#   define USRAPP_USBD_DWCOTG_CFG_ULPI_EN               true
#   define USRAPP_USBD_DWCOTG_CFG_UTMI_EN               false
#   define USRAPP_USBD_DWCOTG_CFG_VBUS_EN               false
#   define USRAPP_USBD_DWCOTG_CFG_DMA_EN                false
#   define USRAPP_CFG_USBD_SPEED                        USB_SPEED_HIGH

#define USRAPP_CFG_USBD_DEV                             VSF_USB_DC0

#define VSF_USE_HEAP                                    ENABLED
#   define VSF_HEAP_CFG_MCB_MAGIC_EN                    ENABLED
#   define VSF_HEAP_CFG_MCB_ALIGN_BIT                   4
#   define VSF_HEAP_ADDR                                0x00100000
#   define VSF_HEAP_SIZE                                0x50000

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif      // __VSF_USR_CFG_AIC8800_H__
/* EOF */
