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

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_RNG == ENABLED

/*\note For IPCore drivers, define __VSF_HAL_${RNG_IP}_RNG_CLASS_IMPLEMENT before including vsf_hal.h.
 *      For peripheral drivers, if IPCore driver is used, define __VSF_HAL_${RNG_IP}_RNG_CLASS_INHERIT__ before including vsf_hal.h
 */

// IPCore
#define __VSF_HAL_${RNG_IP}_RNG_CLASS_IMPLEMENT
// IPCore end
// HW using ${RNG_IP} IPCore driver
#define __VSF_HAL_${RNG_IP}_RNG_CLASS_INHERIT__
// HW end

#include "hal/vsf_hal.h"

// HW
// for vendor headers
#define __VSF_HAL_SHOW_VENDOR_INFO__
#include "hal/driver/vendor_driver.h"
// HW end

/*============================ MACROS ========================================*/

/*\note VSF_HW_RNG_CFG_MULTI_CLASS is only for drivers for specified device(hw drivers).
 *      For other drivers, please define VSF_${RNG_IP}_RNG_CFG_MULTI_CLASS in header file.
 */

// HW
#ifndef VSF_HW_RNG_CFG_MULTI_CLASS
#   define VSF_HW_RNG_CFG_MULTI_CLASS           VSF_RNG_CFG_MULTI_CLASS
#endif
// HW end

// HW
#define VSF_RNG_CFG_IMP_PREFIX                  vsf_hw
#define VSF_RNG_CFG_IMP_UPCASE_PREFIX           VSF_HW
// HW end
// IPCore
#define VSF_RNG_CFG_IMP_PREFIX                  vsf_${rng_ip}
#define VSF_RNG_CFG_IMP_UPCASE_PREFIX           VSF_${RNG_IP}
// IPCore end

/*============================ TYPES =========================================*/

// HW
typedef struct VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_t) {
#if VSF_HW_RNG_CFG_MULTI_CLASS == ENABLED
    vsf_rng_t               vsf_rng;
#endif
    void                    *reg;
} VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_t);
// HW end


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*\note Implementation below is for hw rng only, because there is no requirements
 *          on the APIs of IPCore drivers.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
vsf_err_t VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_init)(
    VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_t) *rng_ptr
) {
    VSF_HAL_ASSERT((NULL != rng_ptr) && (NULL != cfg_ptr));
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_fini)(
    VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_t) *rng_ptr
) {
    VSF_HAL_ASSERT(rng_ptr != NULL);
}

vsf_err_t VSF_MCONNECT(VSF_RNG_GENERATE_CFG_IMP_PREFIX, _rng_generate_request)(
    VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_t) *rng_ptr,
    uint32_t *buffer,
    uint32_t num,
    void *param,
    vsf_rng_on_ready_callback_t * on_ready_cb)
{
    VSF_HAL_ASSERT(rng_ptr != NULL);

    return VSF_ERR_NONE;
}

vsf_rng_capability_t VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_capability)(
    VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_t) *rng_ptr
) {
    VSF_HAL_ASSERT(rng_ptr != NULL);

    return (vsf_rng_capability_t) {
        0
    };
}
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/

/*\note Instantiation below is for hw rng only.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
#define VSF_RNG_CFG_REIMPLEMENT_API_CAPABILITY             ENABLED
#define VSF_RNG_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_t)                                \
        VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng, __IDX) = {                   \
        .reg                = VSF_MCONNECT(VSF_RNG_CFG_IMP_UPCASE_PREFIX, _RNG, __IDX, _REG),\
        __HAL_OP                                                                \
    };                                                                          \
    VSF_CAL_ROOT void VSF_MCONNECT(VSF_RNG_CFG_IMP_UPCASE_PREFIX, _RNG, __IDX, _IRQHandler)(void)\
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_RNG_CFG_IMP_PREFIX, _rng_irqhandler)(              \
            &VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng, __IDX)                  \
        );                                                                      \
        vsf_hal_irq_leave(ctx);                                                 \
    }
#include "hal/driver/common/rng/rng_template.inc"
// HW end

#endif /* VSF_HAL_USE_RNG */
