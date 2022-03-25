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

#ifndef __VSF_USR_CFG_H__
#define __VSF_USR_CFG_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#if     defined(__M484__)
#   include "./vsf_usr_cfg/vsf_usr_cfg_m484.h"
#elif   defined(__F1C100S__)
#   include "./vsf_usr_cfg/vsf_usr_cfg_f1c100s.h"
#elif   defined(__WIN__)
#   include "./vsf_usr_cfg/vsf_usr_cfg_win.h"
#elif   defined(__LINUX__)
#   include "./vsf_usr_cfg/vsf_usr_cfg_linux.h"
#elif   defined(__GD32E103__)
#   include "./vsf_usr_cfg/vsf_usr_cfg_gd32e103.h"
#elif   defined(__MT071__)
#   include "./vsf_usr_cfg/vsf_usr_cfg_mt071.h"
#elif   defined(__MPS2__)
#   include "./vsf_usr_cfg/vsf_usr_cfg_mps2.h"
#elif   defined(__ESP32__)
#   include "./vsf_usr_cfg/vsf_usr_cfg_esp32.h"
#elif   defined(__ESP32S2__)
#   include "./vsf_usr_cfg/vsf_usr_cfg_esp32s2.h"
#elif   defined(__AIC8800__)
#   include "./vsf_usr_cfg/vsf_usr_cfg_aic8800.h"
#elif   defined(__CMEM7__)
#   include "./vsf_usr_cfg/vsf_usr_cfg_cmem7.h"
#endif

// components and configurations
#define VSF_USE_TRACE                                   ENABLED
#define VSF_USE_STREAM                                  DISABLED
#define VSF_USE_SIMPLE_STREAM                           ENABLED
#define VSF_USE_FS                                      ENABLED
#if VSF_HAL_USE_USBH == ENABLED
#   define VSF_USE_USB_HOST                             ENABLED
#       define VSF_USBH_USE_BTHCI                       ENABLED
#       define VSF_USBH_USE_LIBUSB                      ENABLED
#endif

#define VSF_USE_BTSTACK                                 ENABLED

#if VSF_USE_BTSTACK == ENABLED
#   define HCI_RESET_RESEND_TIMEOUT_MS                  2000
#   define BTSTACK_OO_USE_HOST                          ENABLED
#       define BTSTACK_OO_USE_HOST_HID                  ENABLED
#           define BTSTACK_OO_USE_HOST_JOYCON           ENABLED
#           define BTSTACK_OO_USE_HOST_DS4              ENABLED
#   define BTSTACK_OO_USE_DEVICE                        DISABLED
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ INCLUDES ======================================*/

#endif
/* EOF */
