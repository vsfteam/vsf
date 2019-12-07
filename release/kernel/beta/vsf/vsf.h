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

/*============================ INCLUDES ======================================*/
#include "vsf_cfg.h"
#include "hal/vsf_hal.h"
#include "service/vsf_service.h"

#if VSF_USE_KERNEL == ENABLED
#   include "kernel/vsf_kernel.h"
#endif

#include "osa_service/vsf_osa_service.h"

#ifdef VSF_CFG_USER_HEADER
#   include VSF_CFG_USER_HEADER
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

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
