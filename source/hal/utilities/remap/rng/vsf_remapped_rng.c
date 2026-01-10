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

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal.h"

#if VSF_HAL_USE_RNG == ENABLED

#define __VSF_DISTBUS_CLASS_INHERIT__
#define __VSF_REMAPPED_RNG_CLASS_IMPLEMENT
#include "./vsf_remapped_rng.h"

#if VSF_RNG_CFG_MULTI_CLASS == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_REMAPPED_RNG_CFG_MULTI_CLASS == ENABLED
const vsf_rng_op_t vsf_remapped_rng_op = {
#   undef __VSF_HAL_TEMPLATE_API
#   define __VSF_HAL_TEMPLATE_API   VSF_HAL_TEMPLATE_API_OP

    VSF_RNG_APIS(vsf_remapped_rng)
};
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_remapped_rng_init(vsf_remapped_rng_t *rng)
{
    VSF_HAL_ASSERT((rng != NULL) && (rng->target != NULL));
    return vsf_rng_init(rng->target);
}

void vsf_remapped_rng_fini(vsf_remapped_rng_t *rng)
{
    VSF_HAL_ASSERT((rng != NULL) && (rng->target != NULL));
    vsf_rng_fini(rng->target);
}

vsf_rng_capability_t vsf_remapped_rng_capability(vsf_remapped_rng_t *rng)
{
    VSF_HAL_ASSERT((rng != NULL) && (rng->target != NULL));
    return vsf_rng_capability(rng->target);
}

vsf_err_t vsf_remapped_rng_generate_request(vsf_remapped_rng_t *rng, uint32_t *buffer, uint32_t num,
                void *param, vsf_rng_on_ready_callback_t * on_ready_cb)
{
    VSF_HAL_ASSERT((rng != NULL) && (rng->target != NULL));
    return vsf_rng_generate_request(rng->target, buffer, num, param, on_ready_cb);
}

vsf_err_t vsf_remapped_rng_ctrl(vsf_remapped_rng_t *rng, vsf_rng_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT((rng != NULL) && (rng->target != NULL));
    return vsf_rng_ctrl(rng->target, ctrl, param);
}

#endif
#endif
