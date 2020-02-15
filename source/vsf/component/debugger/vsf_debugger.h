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

#ifndef __VSF_DEBUGGER_H__
#define __VSF_DEBUGGER_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_debugger_cfg.h"

#ifdef VSF_DEBUGGER_CFG_CONSOLE

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED 
#   include "service/vsfstream/vsfstream.h"
#elif VSF_USE_SERVICE_STREAM == ENABLED
#   include "service/stream/vsf_stream.h"
#endif

#if VSF_DEBUGGER_CFG_CONSOLE == VSF_DEBUGGER_CFG_CONSOLE_JLINK_RTT
#elif VSF_DEBUGGER_CFG_CONSOLE == VSF_DEBUGGER_CFG_CONSOLE_NULINK_NUCONSOLE
#   include "./nulink/NuConsole_stream.h"
#elif VSF_DEBUGGER_CFG_CONSOLE == VSF_DEBUGGER_CFG_CONSOLE_USER

#   ifdef VSF_DEBUGGER_CFG_STREAM_TX_T
        extern VSF_DEBUGGER_CFG_STREAM_TX_T VSF_DEBUG_STREAM_TX;
#   endif
#   ifdef VSF_DEBUGGER_CFG_STREAM_RX_T
        extern VSF_DEBUGGER_CFG_STREAM_RX_T VSF_DEBUG_STREAM_RX;
#   endif

#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

#endif      // VSF_DEBUGGER_CFG_CONSOLE
#endif      // __VSF_DEBUGGER_H__
