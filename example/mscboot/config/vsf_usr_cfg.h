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
#endif

#ifndef VSF_ASSERT
#   define VSF_ASSERT(...)                              if (!(__VA_ARGS__)) {while(1);}
#endif

// tasks runs in idle only, avoid jumping to app in swi isrhandler
#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                     ENABLED
#define VSF_OS_CFG_PRIORITY_NUM                         1

#define VSF_KERNEL_OPT_AVOID_UNNECESSARY_YIELD_EVT      DISABLED
#define VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY         DISABLED

#define USRAPP_CFG_FAKEFAT32                            ENABLED
#define USRAPP_FAKEFAT32_CFG_HEADER                     "fakefat32.h"

#define VSF_USE_USB_DEVICE                              ENABLED
#   define VSF_USBD_CFG_STREAM_EN                       ENABLED
#   define VSF_USBD_USE_MSC                             ENABLED

#define VSF_USE_MAL                                     ENABLED
#   define VSF_MAL_USE_FAKEFAT32_MAL                    ENABLED

#define VSF_USE_FS                                      ENABLED
#   define VSF_FS_USE_MEMFS                             ENABLED
#   define VSF_FS_USE_FATFS                             ENABLED

#define VSF_USE_SCSI                                    ENABLED
#   define VSF_SCSI_USE_MAL_SCSI                        ENABLED

#define VSF_USE_SIMPLE_STREAM                           ENABLED
#define VSF_USE_FIFO                                    ENABLED

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ INCLUDES ======================================*/

#endif
/* EOF */
