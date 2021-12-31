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

#ifndef __AIC_AIC8800_RTOS_H__
#define __AIC_AIC8800_RTOS_H__

/*============================ INCLUDES ======================================*/

#include "rtos_al.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef AIC8800_OSAL_CFG_PRIORITY_BASE
#   define AIC8800_OSAL_CFG_PRIORITY_BASE       vsf_prio_1
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    TASK_PRIORITY_CONSOLE              = RTOS_TASK_PRIORITY(1),
    TASK_PRIORITY_TEST                 = RTOS_TASK_PRIORITY(1),
    TASK_PRIORITY_BT_TASK              = RTOS_TASK_PRIORITY(2),
    TASK_PRIORITY_INTERSYS_TASK        = RTOS_TASK_PRIORITY(2),
    TASK_PRIORITY_ASIO                 = RTOS_TASK_PRIORITY(3),
    TASK_PRIORITY_AUDIO                = RTOS_TASK_PRIORITY(3),
    TASK_PRIORITY_DSP                  = RTOS_TASK_PRIORITY(2),
    TASK_PRIORITY_WIFI_VOICE           = RTOS_TASK_PRIORITY(1),
    TASK_PRIORITY_WIFI_TCPIP           = RTOS_TASK_PRIORITY(3),
    TASK_PRIORITY_WIFI_CNTRL           = RTOS_TASK_PRIORITY(2),
    TASK_PRIORITY_WIFI_IPC             = RTOS_TASK_PRIORITY(4),
    TASK_PRIORITY_WIFI_WPA             = RTOS_TASK_PRIORITY(1),
    TASK_PRIORITY_WIFI_TG_SEND         = RTOS_TASK_PRIORITY(2),
    TASK_PRIORITY_WIFI_PING_SEND       = RTOS_TASK_PRIORITY(1),
    TASK_PRIORITY_WIFI_IPERF           = RTOS_TASK_PRIORITY(1),
    TASK_PRIORITY_WIFI_SMARTCONF       = RTOS_TASK_PRIORITY(2),
    TASK_PRIORITY_MAX                  = RTOS_TASK_PRIORITY(4),
};

/// Definitions of the different FHOST task stack size requirements
enum {
    TASK_STACK_SIZE_CONSOLE              = 512,
    TASK_STACK_SIZE_TEST                 = 2048,
    TASK_STACK_SIZE_BT_TASK              = 3072,
    TASK_STACK_SIZE_ASIO                 = 2048,
    TASK_STACK_SIZE_AUDIO                = 2048,
    TASK_STACK_SIZE_BLE_TASK_ONLY        = 512,
    TASK_STACK_SIZE_BT_BLE_TASK          = 4096,
    TASK_STACK_SIZE_DSP                  = 512,
    TASK_STACK_SIZE_WIFI_VOICE           = 512,
    TASK_STACK_SIZE_WIFI_TCPIP           = 1024,
    TASK_STACK_SIZE_WIFI_CNTRL           = 640,
    TASK_STACK_SIZE_WIFI_IPC             = 512,
    TASK_STACK_SIZE_WIFI_WPA             = 1024,
    TASK_STACK_SIZE_WIFI_TG_SEND         = 1024,
    TASK_STACK_SIZE_WIFI_PING_SEND       = 512,
    TASK_STACK_SIZE_WIFI_IPERF           = 1024,
    TASK_STACK_SIZE_WIFI_SMARTCONF       = 512,
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
