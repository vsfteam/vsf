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

#ifndef __VSF_SERVICE_CFG_H__
#define __VSF_SERVICE_CFG_H__

//! all service modules use this configuation file

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
#if VSF_POOL_CFG_FEED_ON_HEAP == ENABLED && VSF_USE_HEAP != ENABLED
#   warning [DEPENDENCY WARNING]\
The Feature VSF_POOL_CFG_FEED_ON_HEAP is enabled but VSF_USE_HEAP is disabled.\
The VSF_USE_HEAP will be forced to ENABLED here to provide required services
#   undef VSF_USE_HEAP
#   define VSF_USE_HEAP     ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */