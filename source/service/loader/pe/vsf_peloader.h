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

/**
 * \file vsf_peloader.h
 * \brief vsf pe loader
 *
 * provides a pe loader implementation
 */

/** @ingroup vsf_loader
 *  @{
 */

#ifndef __VSF_PELOADER_H__
#define __VSF_PELOADER_H__

/*============================ INCLUDES ======================================*/

#include "service/vsf_service_cfg.h"

#if VSF_USE_LOADER == ENABLED && VSF_LOADER_USE_PE == ENABLED

#include <stdint.h>

#if     defined(__VSF_PELOADER_CLASS_IMPLEMENT)
#   undef __VSF_EPLOADER_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_PELOADER_CLASS_INHERIT__)
#   undef __VSF_EPLOADER_CLASS_INHERIT__
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_class(vsf_peloader_t) {
    public_member(
        implement(vsf_loader_t)
    )

    private_member(
        void *ram_base;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const struct vsf_loader_op_t vsf_peloader_op;

/*============================ PROTOTYPES ====================================*/

extern int vsf_peloader_load(vsf_peloader_t *peloader, vsf_loader_target_t *target);
extern void vsf_peloader_cleanup(vsf_peloader_t *peloader);
extern int vsf_peloader_call_init_array(vsf_peloader_t *peloader);
extern void vsf_peloader_call_fini_array(vsf_peloader_t *peloader);

#ifdef __cplusplus
}
#endif

/** @} */   // vsf_peloader

#endif      // VSF_USE_ELFLOADER
#endif      // __VSF_ELFLOADER_H__
