/*******************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
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
 *  See the License for the specific language governing permissions and       *
 *  limitations under the License.                                           *
 *                                                                           *
 ******************************************************************************/

/*============================ INCLUDES ======================================*/

#include "./rng.h"

#if VSF_HAL_USE_RNG == ENABLED

#include "hal/vsf_hal.h"
#include "hal/driver/vendor_driver.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_RNG_CFG_MULTI_CLASS
#   define VSF_HW_RNG_CFG_MULTI_CLASS           VSF_RNG_CFG_MULTI_CLASS
#endif

#define VSF_RNG_CFG_IMP_PREFIX                  vsf_hw
#define VSF_RNG_CFG_IMP_UPCASE_PREFIX           VSF_HW

/*============================ TYPES =========================================*/

typedef struct VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_t) {
#if VSF_HW_RNG_CFG_MULTI_CLASS == ENABLED
    vsf_rng_t               vsf_rng;
#endif
    uint8_t                 dummy;
} VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_t);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_init)(
    VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_t) *rng_ptr)
{
    VSF_HAL_ASSERT(NULL != rng_ptr);
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_fini)(
    VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_t) *rng_ptr)
{
    VSF_HAL_ASSERT(rng_ptr != NULL);
}

vsf_rng_capability_t VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_capability)(
    VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_t) *rng_ptr)
{
    VSF_HAL_ASSERT(rng_ptr != NULL);

    return (vsf_rng_capability_t) {
        0,
    };
}

vsf_err_t VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_generate_request)(
    VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_t) *rng_ptr,
    uint32_t *buffer,
    uint32_t num,
    void *param,
    vsf_rng_on_ready_callback_t *on_ready_cb)
{
    VSF_HAL_ASSERT(rng_ptr != NULL);
    VSF_HAL_ASSERT(buffer != NULL);
    VSF_HAL_ASSERT(num > 0);

    for (uint32_t w = 0; w < num; w++) {
        uint32_t word = 0;
        for (uint32_t b = 0; b < 32; b++) {
            word <<= 1;
            word |= (rosc_hw->randombit & 1u);
            // Per RP2040 datasheet §2.17.3: ROSC runs at ~1.8–12 MHz (typ 6.5 MHz).
            // RANDOMBIT samples a phase-comparator tap; back-to-back reads yield
            // correlated bits because the oscillator state has not evolved.
            // A brief delay lets ROSC advance at least one full cycle (> ~80 ns)
            // before the next sample.
            // < 1 us
            for (volatile uint32_t d = 0; d < 4; d++);
        }
        buffer[w] = word;
    }

    if (on_ready_cb != NULL) {
        on_ready_cb(param, buffer, num);
    }

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_ctrl)(
    VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_t) *rng_ptr,
    vsf_rng_ctrl_t ctrl,
    void *param)
{
    VSF_HAL_ASSERT(rng_ptr != NULL);
    VSF_UNUSED_PARAM(ctrl);
    VSF_UNUSED_PARAM(param);
    VSF_HAL_ASSERT(0);
    return VSF_ERR_NOT_SUPPORT;
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_RNG_CFG_MODE_CHECK_UNIQUE               VSF_HAL_CHECK_MODE_LOOSE
#define VSF_RNG_CFG_IRQ_MASK_CHECK_UNIQUE           VSF_HAL_CHECK_MODE_STRICT
#define VSF_RNG_CFG_REIMPLEMENT_API_CAPABILITY      ENABLED
#define VSF_RNG_CFG_REIMPLEMENT_API_CTRL            ENABLED

#define VSF_RNG_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_t)                                \
    VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng, __IDX) = {.dummy = 0, __HAL_OP};

#include "hal/driver/common/rng/rng_template.inc"

#endif /* VSF_HAL_USE_RNG */
/* EOF */
