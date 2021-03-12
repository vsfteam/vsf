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

#ifndef __VSF_DYNARR_H__
#define __VSF_DYNARR_H__

/*============================ INCLUDES ======================================*/
#include "service/vsf_service_cfg.h"
#include "utilities/vsf_utilities.h"

#if VSF_USE_DYNARR == ENABLED

#if     defined(__VSF_DYNARR_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#   undef __VSF_DYNARR_CLASS_IMPLEMENT
#elif   defined(__VSF_DYNARR_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#   undef __VSF_DYNARR_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/

dcl_simple_class(vsf_dynarr_t)
def_simple_class(vsf_dynarr_t) {

    public_member(
        uint16_t item_size;
        uint8_t item_num_per_buf_bitlen;
        uint8_t buf_num_per_table_bitlen;
    )

    private_member(
        vsf_slist_t table_list;
        uint32_t length;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_dynarr_init(vsf_dynarr_t *dynarr);
extern void vsf_dynarr_fini(vsf_dynarr_t *dynarr);
extern uint_fast32_t vsf_dynarr_get_size(vsf_dynarr_t *dynarr);
extern vsf_err_t vsf_dynarr_set_size(vsf_dynarr_t *dynarr, uint_fast32_t size);
extern void * vsf_dynarr_get(vsf_dynarr_t *dynarr, uint_fast32_t pos);

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_DYNARR
#endif      // __VSF_DYNARR_H__
