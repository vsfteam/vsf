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

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_RNG == ENABLED

// for VSF_MCONNECT
#include "utilities/vsf_utilities.h"

#include "../device.h"

#include "reg_trng.h"
#include "reg_sysctrl.h"
#include "./trng.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define __VSF_HW_RNG_IMP_LV0(__COUNT, __dont_care)                              \
    vsf_hw_rng_t VSF_MCONNECT(vsf_rng, __COUNT) = {                             \
        VSF_MCONNECT(RNG, __COUNT, _CONFIG)                                     \
    };                                                                          \
    void VSF_MCONNECT(RNG, __COUNT, _IRQHandler)(void)                          \
    {                                                                           \
        vsf_hw_rng_t *rng = &VSF_MCONNECT(vsf_rng, __COUNT);                    \
        AIC_TRNG_TypeDef *reg = rng->reg;                                       \
        uint32_t data = reg->trng_data;                                         \
                                                                                \
        if (!(reg->trng_status & (TRNG_SSECSQ | TRNG_SSEISQ))) {                \
            if (rng->request.buffer != NULL) {                                  \
                rng->request.buffer[rng->request.cur_num++] = data;             \
                if (rng->request.cur_num >= rng->request.num) {                 \
                    uint32_t *buffer = rng->request.buffer;                     \
                    rng->request.buffer = NULL;                                 \
                    reg->trng_en = 0;                                           \
                    if (rng->request.on_ready != NULL) {                        \
                        rng->request.on_ready(rng->request.param, buffer, rng->request.num);\
                    }                                                           \
                } else {                                                        \
                    reg->trng_en = TRNG_STRNGENQ | TRNG_STRNGIEQ;               \
                }                                                               \
            }                                                                   \
        } else {                                                                \
            reg->trng_en = TRNG_STRNGENQ | TRNG_STRNGIEQ;                       \
        }                                                                       \
    }

/*============================ TYPES =========================================*/

typedef struct vsf_hw_rng_t {
    AIC_TRNG_TypeDef *reg;
    uint32_t pclk;

    struct {
        void *param;
        void (*on_ready)(void *param, uint32_t *buffer, uint32_t num);

        uint32_t *buffer;
        uint32_t num;
        uint32_t cur_num;
    } request;
} vsf_hw_rng_t;

/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/

VSF_MREPEAT(RNG_COUNT, __VSF_HW_RNG_IMP_LV0, NULL)

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_hw_rng_init(vsf_hw_rng_t *rng)
{
    cpusysctrl_pclkme_set(rng->pclk);

    AIC_TRNG_TypeDef *reg = rng->reg;
    reg->trng_en = 0;

    NVIC_SetPriority(TRNG_IRQn, vsf_arch_prio_0);
    NVIC_EnableIRQ(TRNG_IRQn);
    return VSF_ERR_NONE;
}

void vsf_hw_rng_fini(vsf_hw_rng_t *rng)
{
    NVIC_DisableIRQ(TRNG_IRQn);

    AIC_TRNG_TypeDef *reg = rng->reg;
    reg->trng_en = 0;

    cpusysctrl_hclkmd_set(rng->pclk);
}

vsf_err_t vsf_hw_rng_generate_request(vsf_hw_rng_t *rng, uint32_t *buffer, uint32_t num,
            void *param, void (*on_ready)(void *param, uint32_t *buffer, uint32_t num))
{
    VSF_HAL_ASSERT(NULL == rng->request.buffer);
    AIC_TRNG_TypeDef *reg = rng->reg;

    rng->request.param = param;
    rng->request.on_ready = on_ready;
    rng->request.buffer = buffer;
    rng->request.num = num;
    rng->request.cur_num = 0;

    reg->trng_en = TRNG_STRNGENQ | TRNG_STRNGIEQ;
    return VSF_ERR_NONE;
}

#endif /* VSF_HAL_USE_AD */
