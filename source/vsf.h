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



//! \note Top Level Configuration

#ifndef __VSF_H__
#define __VSF_H__

#if defined(__cplusplus) && !defined(__IAR_SYSTEMS_ICC__)
#   define __STDC_VERSION__   201112L
#endif

/*============================ INCLUDES ======================================*/
#include "vsf_cfg.h"
#include "utilities/vsf_utilities.h"
#include "hal/arch/vsf_arch.h"
#include "service/vsf_service.h"

#if VSF_USE_KERNEL == ENABLED
#   include "kernel/vsf_kernel.h"
#endif

#include "osa_service/vsf_osa_service.h"
#include "hal/vsf_hal.h"
// TODO: remove later
#include "osa_hal/vsf_osa_hal.h"
#include "component/vsf_component.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef vsf_log_info
#   if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#       define vsf_log_info(__ARG)
#   else
#       define vsf_log_info(...)
#   endif
#endif

#ifndef vsf_log_warning
#   if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#       define vsf_log_warning(__ARG)
#   else
#       define vsf_log_warning(...)
#   endif
#endif

#ifndef vsf_log_debug
#   if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#       define vsf_log_debug(__ARG)
#   else
#       define vsf_log_debug(...)
#   endif
#endif

#if VSF_USE_TRACE == ENABLED
#   if      defined(VSF_DEBUGGER_CFG_CONSOLE)                                   \
        ||  (defined(VSF_HAL_USE_DEBUG_STREAM) && VSF_HAL_USE_DEBUG_STREAM == ENABLED)\
        ||  defined(VSF_CFG_DEBUG_STREAM_TX_T)
    // use default debug stream from debugger/hardware debug uart/user
#       if VSF_USE_SIMPLE_STREAM == ENABLED
#           define vsf_start_trace(...)                                         \
                vsf_trace_init(((vsf_stream_t *)&VSF_DEBUG_STREAM_TX, ##__VA_ARGS__))
#       elif VSF_USE_STREAM == ENABLED
#           define vsf_start_trace(...)                                         \
                vsf_trace_init(((vsf_stream_tx_t *)&VSF_DEBUG_STREAM_TX, ##__VA_ARGS__))
#       endif
#   else
    // no default debug stream, user should define a stream
#       if VSF_USE_SIMPLE_STREAM == ENABLED
#           define vsf_start_trace(__stream)                                    \
                vsf_trace_init((vsf_stream_t *)(__stream))
#       elif VSF_USE_STREAM == ENABLED
#           define vsf_start_trace(__stream)                                    \
                vsf_trace_init((vsf_stream_tx_t *)(__stream))
#       endif
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#ifdef VSF_CFG_DEBUG_STREAM_TX_T
extern VSF_CFG_DEBUG_STREAM_TX_T VSF_DEBUG_STREAM_TX;
#endif

#ifdef VSF_CFG_DEBUG_STREAM_RX_T
extern VSF_CFG_DEBUG_STREAM_RX_T VSF_DEBUG_STREAM_RX;
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ INCLUDES ======================================*/

/*£¡\note If you have one or more private header files with which you want to
 *!       cover all the range where vsf.h is included directly or indirectly, you
 *!       can define the macro VSF_CFG_USR_HEADER with a path string in
 *!       vsf_usr_cfg.h.
 *!
 *!       e.g. some customised VSF contains a set of private definition, usually
 *!            those private definitions are defined in a private header file and
 *!            inserted here with VSF_CFG_USER_HEADER
 *!       e.g. in some application scenarios, users want to insert some header file
 *!            together with vsf.h, then it can be handled with VSF_CFG_USER_HEADER
 */
#ifdef VSF_CFG_USER_HEADER
#   include VSF_CFG_USER_HEADER
#endif

#ifdef __cplusplus
}
#endif
#endif
/* EOF */
