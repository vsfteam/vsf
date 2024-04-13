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

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_PWM == ENABLED

/*\note For IPCore drivers, define __VSF_HAL_${PWM_IP}_PWM_CLASS_IMPLEMENT before including vsf_hal.h.
 *      For peripherial drivers, if IPCore driver is used, define __VSF_HAL_${PWM_IP}_PWM_CLASS_INHERIT__ before including vsf_hal.h
 */

// IPCore
#define __VSF_HAL_${PWM_IP}_PWM_CLASS_IMPLEMENT
// IPCore end
// HW using ${PWM_IP} IPCore driver
#define __VSF_HAL_${PWM_IP}_PWM_CLASS_INHERIT__
// HW end

#include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/

/*\note VSF_HW_PWM_CFG_MULTI_CLASS is only for drivers for specified device(hw drivers).
 *      For other drivers, plase define VSF_${PWM_IP}_PWM_CFG_MULTI_CLASS in header file.
 */

// HW
#ifndef VSF_HW_PWM_CFG_MULTI_CLASS
#   define VSF_HW_PWM_CFG_MULTI_CLASS           VSF_PWM_CFG_MULTI_CLASS
#endif
// HW end

// HW
#define VSF_PWM_CFG_IMP_PREFIX                  vsf_hw
#define VSF_PWM_CFG_IMP_UPCASE_PREFIX           VSF_HW
// HW end
// IPCore
#define VSF_PWM_CFG_IMP_PREFIX                  vsf_${mmc_ip}
#define VSF_PWM_CFG_IMP_UPCASE_PREFIX           VSF_${MMC_IP}
// IPCore end

/*============================ TYPES =========================================*/

// HW
typedef struct VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _mmc_t) {
#if VSF_HW_PWM_CFG_MULTI_CLASS == ENABLED
    vsf_mmc_t               vsf_mmc;
#endif
    void                    *reg;
    vsf_mmc_isr_t           isr;
} VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _mmc_t);
// HW end


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/

/*\note Implementation below is for hw mmc only, because there is no requirements
 *          on the APIs of IPCore drivers.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
vsf_err_t VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_init)(
    VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_t) *mmc_ptr,
    vsf_mmc_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT((NULL != mmc_ptr) && (NULL != cfg_ptr));
    // configure according to cfg_ptr
    mmc_ptr->isr = cfg_ptr->isr;
    // configure interrupt according to cfg_ptr->isr
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_fini)(
    VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_t) *mmc_ptr
) {
    VSF_HAL_ASSERT(mmc_ptr != NULL);
}

void VSF_MCONNECT(VSF_MMC_IRQ_CFG_IMP_PREFIX, _mmc_irq_enable)(
    VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_t) *mmc_ptr,
    vsf_mmc_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(mmc_ptr != NULL);
}

void VSF_MCONNECT(VSF_MMC_IRQ_CFG_IMP_PREFIX, _mmc_irq_disable)(
    VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_t) *mmc_ptr,
    vsf_mmc_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(mmc_ptr != NULL);
}

vsf_mmc_status_t VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_status)(
    VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_t) *mmc_ptr
) {
    VSF_HAL_ASSERT(mmc_ptr != NULL);

    return (vsf_mmc_status_t) {
        transact_status = 0,
        irq_status = 0,
    };
}

vsf_mmc_capability_t VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_capability)(
    VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_t) *mmc_ptr
) {
    VSF_HAL_ASSERT(mmc_ptr != NULL);

    return (vsf_mmc_capability_t) {
        .mmc_capability = {
            .max_freq_hz = 100 * 1000 * 1000,
            .bus_width   = MMC_CAP_BUS_WIDTH_8,
        },
    };
}

vsf_err_t VSF_MCONNECT(VSF_MMC_SET_CFG_IMP_PREFIX, _mmc_set_clock)(
    VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_t) *mmc_ptr,
    uint32_t clock_hz
) {
    VSF_HAL_ASSERT(mmc_ptr != NULL);

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_MMC_SET_BUS_CFG_IMP_PREFIX, _mmc_set_bus_width)(
    VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_t) *mmc_ptr,
    uint8_t bus_width
) {
    VSF_HAL_ASSERT(mmc_ptr != NULL);

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_MMC_HOST_TRANSACT_CFG_IMP_PREFIX, _mmc_host_transact_start)(
    VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_t) *mmc_ptr,
    vsf_mmc_trans_t *trans
) {
    VSF_HAL_ASSERT(mmc_ptr != NULL);

    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_MMC_HOST_TRANSACT_CFG_IMP_PREFIX, _mmc_host_transact_stop)(
    VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_t) *mmc_ptr
) {
    VSF_HAL_ASSERT(mmc_ptr != NULL);
}

static void VSF_MCONNECT(__, VSF_MMC_CFG_IMP_PREFIX, _mmc_irqhandler)(
    VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_t) *mmc_ptr
) {
    VSF_HAL_ASSERT(NULL != mmc_ptr);

    vsf_mmc_irq_mask_t irq_mask = GET_IRQ_MASK(mmc_ptr);
    vsf_mmc_isr_t *isr_ptr = &mmc_ptr->isr;
    if ((irq_mask != 0) && (isr_ptr->handler_fn != NULL)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_mmc_t *)mmc_ptr, irq_mask);
    }
}
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/

/*\note Instantiation below is for hw mmc only.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
#define VSF_MMC_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_t)                                \
        VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc, __IDX) = {                   \
        .reg                = VSF_MCONNECT(VSF_MMC_CFG_IMP_UPCASE_PREFIX, _MMC, __IDX,_REG_),\
        __HAL_OP                                                                \
    };                                                                          \
    VSF_CAL_ROOT void VSF_MCONNECT(VSF_MMC_CFG_IMP_UPCASE_PREFIX, _MMC, __IDX, _IRQHandler)(void)\
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_MMC_CFG_IMP_PREFIX, _mmc_irqhandler)(              \
            &VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc, __IDX)                  \
        );                                                                      \
        vsf_hal_irq_leave(ctx);                                                 \
    }
#include "hal/driver/common/mmc/mmc_template.inc"
// HW end

#endif /* VSF_HAL_USE_MMC */
