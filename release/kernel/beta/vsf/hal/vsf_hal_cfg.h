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


//! \note do not move this pre-processor statement to other places
#include "vsf_cfg.h"

#ifndef __VSF_HAL_CFG_H__
#define __VSF_HAL_CFG_H__

//! all hal modules use this configuation file

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#ifndef VSF_HAL_ASSERT
#   define VSF_HAL_ASSERT(__CON)        ASSERT(__CON)
#endif
#else
#ifndef VSF_HAL_ASSERT
#   define VSF_HAL_ASSERT(...)          ASSERT(__VA_ARGS__)
#endif
#endif

#ifndef VSF_HAL_CFG_USE_STREAM
#   define VSF_HAL_CFG_USE_STREAM       VSF_USE_SERVICE_STREAM
#endif

#ifndef VSF_HAL_CFG_USE_VSFSTREAM
#   define VSF_HAL_CFG_USE_VSFSTREAM    VSF_USE_SERVICE_VSFSTREAM
#endif

#ifndef VSF_SYSTIMER_RESOLUTION
#   define VSF_SYSTIMER_RESOLUTION      (1000000)    /*! using default 1us */
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */