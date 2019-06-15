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

#ifndef __VSF_DYNSTACK_H__
#define __VSF_DYNSTACK_H__

/*============================ INCLUDES ======================================*/
#include "service/vsf_service_cfg.h"

#if VSF_USE_DYNARR == ENABLED

#include "../dynarr/vsf_dynarr.h"

#if     defined(VSF_DYNSTACK_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#   undef VSF_DYNSTACK_IMPLEMENT
#elif   defined(VSF_DYNSTACK_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#   undef VSF_DYNSTACK_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/

declare_simple_class(vsf_dynstack_t)

def_simple_class(vsf_dynstack_t) {
    public_member(
        implement(vsf_dynarr_t)
        uint32_t sp;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_dynstack_init(vsf_dynstack_t *stack);
extern void vsf_dynstack_reset(vsf_dynstack_t *stack);
extern void vsf_dynstack_fini(vsf_dynstack_t *stack);
extern void * vsf_dynstack_get(vsf_dynstack_t *stack, uint_fast32_t offset);
extern void * vsf_dynstack_pop(vsf_dynstack_t *stack, uint_fast16_t num);
extern vsf_err_t vsf_dynstack_push(vsf_dynstack_t *stack, void *item, uint_fast16_t num);
extern vsf_err_t vsf_dynstack_push_ext(vsf_dynstack_t *stack, void *ptr, uint_fast32_t len);
extern vsf_err_t vsf_dynstack_pop_ext(vsf_dynstack_t *stack, void *ptr, uint_fast32_t len);

#endif      // VSF_USE_DYNARR
#endif      // __VSF_DYNSTACK_H__
