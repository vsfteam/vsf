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

#ifndef __VSF_REMAPPED_RNG_H__
#define __VSF_REMAPPED_RNG_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_RNG == ENABLED

#if VSF_RNG_CFG_MULTI_CLASS == ENABLED

#if     defined(__VSF_REMAPPED_RNG_CLASS_IMPLEMENT)
#   undef __VSF_REMAPPED_RNG_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_REMAPPED_RNG_CLASS_INHERIT__)
#   undef __VSF_REMAPPED_RNG_CLASS_INHERIT__
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_REMAPPED_RNG_CFG_MULTI_CLASS
#   define VSF_REMAPPED_RNG_CFG_MULTI_CLASS     VSF_RNG_CFG_MULTI_CLASS
#endif

#if VSF_REMAPPED_RNG_CFG_MULTI_CLASS == ENABLED
#   define __describe_remapped_rng_op()         .op = &vsf_remapped_rng_op,
#else
#   define __describe_remapped_rng_op()
#endif

#define __describe_remapped_rng(__name, __rng)                                  \
    vsf_remapped_rng_t __name = {                                               \
        __describe_remapped_rng_op()                                            \
        .target = (vsf_rng_t *) & __rng,                                        \
    };

#define describe_remapped_rng(__name, __rng)                                    \
            __describe_remapped_rng(__name, __rng)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_class(vsf_remapped_rng_t) {
#if VSF_REMAPPED_RNG_CFG_MULTI_CLASS == ENABLED
    public_member(
        implement(vsf_rng_t)
    )
#endif
    public_member(
        vsf_rng_t                           *target;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_REMAPPED_RNG_CFG_MULTI_CLASS == ENABLED
extern const vsf_rng_op_t vsf_remapped_rng_op;
#endif

/*============================ PROTOTYPES ====================================*/
/*============================ INCLUDES ======================================*/


#define VSF_RNG_CFG_DEC_PREFIX              vsf_remapped
#define VSF_RNG_CFG_DEC_UPCASE_PREFIX       VSF_REMAPPED
#include "hal/driver/common/rng/rng_template.h"

#ifdef __cplusplus
}
#endif

#endif      // VSF_RNG_CFG_MULTI_CLASS
#endif      // VSF_HAL_USE_RNG
#endif
/* EOF */

