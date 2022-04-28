/*****************************************************************************
 *   Cop->right(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a cop-> of the License at                                  *
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

#define VSF_RNG_CFG_FUNCTION_RENAME DISABLED

#include "hal/driver/driver.h"

#if VSF_HAL_USE_RNG == ENABLED
#if VSF_RNG_CFG_MULTI_CLASS == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/


vsf_err_t vsf_rng_init(vsf_rng_t *rng_ptr)
{
    VSF_HAL_ASSERT(rng_ptr != NULL);
    VSF_HAL_ASSERT(rng_ptr->op != NULL);
    VSF_HAL_ASSERT(rng_ptr->op->init != NULL);

    return rng_ptr->op->init(rng_ptr);
}

void vsf_rng_fini(vsf_rng_t *rng_ptr)
{
    VSF_HAL_ASSERT(rng_ptr != NULL);
    VSF_HAL_ASSERT(rng_ptr->op != NULL);
    VSF_HAL_ASSERT(rng_ptr->op->fini != NULL);

    rng_ptr->op->fini(rng_ptr);
}

vsf_err_t vsf_rng_generate_request(vsf_rng_t *rng_ptr, uint32_t *buffer, uint32_t num, void *param,
                                   vsf_rng_on_ready_callback_t * on_ready_cb)
{
    VSF_HAL_ASSERT(rng_ptr != NULL);
    VSF_HAL_ASSERT(rng_ptr->op != NULL);
    VSF_HAL_ASSERT(rng_ptr->op->generate_request != NULL);

    return rng_ptr->op->generate_request(rng_ptr, buffer, num, param, on_ready_cb);
}

rng_capability_t vsf_rng_capability(vsf_rng_t *rng_ptr)
{
    VSF_HAL_ASSERT(rng_ptr != NULL);
    VSF_HAL_ASSERT(rng_ptr->op != NULL);
    VSF_HAL_ASSERT(rng_ptr->op->capability != NULL);

    return rng_ptr->op->capability(rng_ptr);
}

#endif /* VSF_RNG_CFG_MULTI_CLASS == ENABLED */
#endif /* VSF_HAL_USE_RNG == ENABLED */
