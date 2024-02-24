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

#define VSF_RNG_CFG_IMP_PREFIX                  vsf_hw
#define VSF_RNG_CFG_IMP_UPCASE_PREFIX           VSF_HW

/*============================ INCLUDES ======================================*/

#include "../driver.h"

#if VSF_HAL_USE_RNG == ENABLED

// for VSF_MCONNECT
#include "utilities/vsf_utilities.h"
// for vsf_trace
#include "service/vsf_service.h"

#include <Windows.h>

/*============================ MACROS ========================================*/

#ifndef VSF_HW_RNG_CFG_MULTI_CLASS
#   define VSF_HW_RNG_CFG_MULTI_CLASS           VSF_RNG_CFG_MULTI_CLASS
#endif

/*============================ TYPES =========================================*/

typedef struct vsf_hw_rng_t {
#if VSF_HW_RNG_CFG_MULTI_CLASS == ENABLED
    vsf_rng_t vsf_rng;
#endif
    HCRYPTPROV hProv;
} vsf_hw_rng_t;

/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_hw_rng_init(vsf_hw_rng_t *rng)
{
    if ((HCRYPTPROV)0 == rng->hProv) {
        if (!CryptAcquireContext(&rng->hProv, NULL, NULL, PROV_RSA_FULL, 0)) {
            DWORD error = GetLastError();
            vsf_trace_error("Fail to initialize RNG, errcode is %d" VSF_TRACE_CFG_LINEEND, error);
            return VSF_ERR_FAIL;
        }
    }
    return VSF_ERR_NONE;
}

void vsf_hw_rng_fini(vsf_hw_rng_t *rng)
{
    if (rng->hProv != (HCRYPTPROV)0) {
        CryptReleaseContext(rng->hProv, 0);
        rng->hProv = (HCRYPTPROV)0;
    }
}

vsf_err_t vsf_hw_rng_generate_request(vsf_hw_rng_t *rng, uint32_t *buffer, uint32_t num,
            void *param, vsf_rng_on_ready_callback_t *on_ready)
{
    VSF_HAL_ASSERT(rng->hProv != (HCRYPTPROV)0);
    if (!CryptGenRandom(rng->hProv, num, (BYTE *)buffer)) {
        DWORD error = GetLastError();
        vsf_trace_error("Fail to generate RNG, errcode is %d" VSF_TRACE_CFG_LINEEND, error);
        return VSF_ERR_FAIL;
    }
    if (on_ready != NULL) {
        on_ready(param, buffer, num);
    }
    return VSF_ERR_NONE;
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_RNG_CFG_REIMPLEMENT_API_CAPABILITY  ENABLED
#define VSF_RNG_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    vsf_hw_rng_t VSF_MCONNECT(vsf_hw_rng, __IDX) = {                            \
        __HAL_OP                                                                \
    };
#include "hal/driver/common/rng/rng_template.inc"

#endif /* VSF_HAL_USE_AD */
