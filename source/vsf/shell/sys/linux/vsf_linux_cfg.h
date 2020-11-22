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

/*============================ INCLUDES ======================================*/

//! \note do not move this pre-processor statement to other places
#include "../../../vsf_cfg.h"

#ifndef __VSF_LINUX_CFG_H__
#define __VSF_LINUX_CFG_H__

/*============================ MACROS ========================================*/

#ifndef VSF_LINUX_ASSERT
#   define VSF_LINUX_ASSERT                     ASSERT
#endif

// signal support is not implemented
#ifndef VSF_LINUX_CFG_SUPPORT_SIG
#   define VSF_LINUX_CFG_SUPPORT_SIG            DISABLED
#endif

// to use simple libc
//  1. enable VSF_LINUX_USE_SIMPLE_LIBC
//  2. add "shell/sys/linux/include/simple_libc to include path
//  IMPORTANT: c++ is not supported if simple libc enabled
#if VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   ifndef VSF_LINUX_USE_SIMPLE_STDIO
#       define VSF_LINUX_USE_SIMPLE_STDIO       ENABLED
#   endif
#   ifndef VSF_LINUX_USE_SIMPLE_STRING
#       define VSF_LINUX_USE_SIMPLE_STRING      ENABLED
#   endif
#   ifndef VSF_LINUX_USE_SIMPLE_TIME
#       define VSF_LINUX_USE_SIMPLE_TIME        ENABLED
#   endif
#   ifndef VSF_LINUX_USE_SIMPLE_STDLIB
#       define VSF_LINUX_USE_SIMPLE_STDLIB      ENABLED
#   endif
#   ifndef VSF_LINUX_USE_SIMPLE_CTYPE
#       define VSF_LINUX_USE_SIMPLE_CTYPE       ENABLED
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/



#endif
/* EOF */