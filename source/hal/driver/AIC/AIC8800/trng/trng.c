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

#include "../driver.h"

#if VSF_HAL_USE_RNG == ENABLED

#include "hal/vsf_hal.h"
// for VSF_MCONNECT
#include "utilities/vsf_utilities.h"

#include "reg_trng.h"
#include "reg_sysctrl.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_RNG_CFG_MULTI_CLASS
#   define VSF_HW_RNG_CFG_MULTI_CLASS           VSF_RNG_CFG_MULTI_CLASS
#endif

/*============================ TYPES =========================================*/

typedef struct vsf_hw_rng_t {
#if VSF_HW_RNG_CFG_MULTI_CLASS == ENABLED
    vsf_rng_t vsf_rng;
#endif

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

    cpusysctrl_pclkmd_set(rng->pclk);
}

vsf_err_t vsf_hw_rng_generate_request(vsf_hw_rng_t *rng, uint32_t *buffer, uint32_t num,
            void *param, vsf_rng_on_ready_callback_t *on_ready)
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

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_RNG_CFG_REIMPLEMENT_API_CAPABILITY  ENABLED
#define VSF_RNG_CFG_IMP_PREFIX                  vsf_hw
#define VSF_RNG_CFG_IMP_UPCASE_PREFIX           VSF_HW
#define VSF_RNG_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    vsf_hw_rng_t VSF_MCONNECT(vsf_hw_rng, __IDX) = {                            \
        VSF_MCONNECT(VSF_HW_RNG, __IDX, _CONFIG)                                \
        __HAL_OP                                                                \
    };                                                                          \
    void VSF_MCONNECT(VSF_HW_RNG, __IDX, _IRQHandler)(void)                     \
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        vsf_hw_rng_t *rng = &VSF_MCONNECT(vsf_hw_rng, __IDX);                   \
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
        vsf_hal_irq_leave(ctx);                                                 \
    }
#include "hal/driver/common/rng/rng_template.inc"

#endif /* VSF_HAL_USE_AD */
