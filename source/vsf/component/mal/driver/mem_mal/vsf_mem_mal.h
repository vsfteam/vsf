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

#ifndef __VSF_MEM_MAL_H__
#define __VSF_MEM_MAL_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_mal_cfg.h"

#if VSF_USE_MAL == ENABLED && VSF_USE_MEM_MAL == ENABLED

#if     defined(VSF_MEM_MAL_IMPLEMENT)
#   undef VSF_MEM_MAL_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(VSF_MEM_MAL_INHERIT)
#   undef VSF_MEM_MAL_INHERIT
#   define __PLOOC_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vsf_mem_mal_t)

def_simple_class(vsf_mem_mal_t) {
    implement(vsf_mal_t)

    public_member(
        vsf_mem_t mem;
        uint32_t blksz;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_mal_drv_t vsf_mem_mal_drv;

/*============================ PROTOTYPES ====================================*/

#endif      // VSF_USE_MAL && VSF_USE_MEM_MAL
#endif      // __VSF_MEM_MAL_H__
