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

#if VSF_HAL_USE_EXTI == ENABLED

#include "hal/vsf_hal.h"

// HW
// for vendor headers
#include "hal/driver/vendor_driver.h"
// HW end

/*============================ MACROS ========================================*/

/*\note VSF_HW_EXTI_CFG_MULTI_CLASS is only for drivers for specified device(hw drivers).
 */

// HW
#ifndef VSF_HW_EXTI_CFG_MULTI_CLASS
#   define VSF_HW_EXTI_CFG_MULTI_CLASS          VSF_EXTI_CFG_MULTI_CLASS
#endif
// HW end

// HW
#define VSF_EXTI_CFG_IMP_PREFIX                 vsf_hw
#define VSF_EXTI_CFG_IMP_UPCASE_PREFIX          VSF_HW
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// HW

typedef struct VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_irq_t) {
    vsf_exti_channel_mask_t channel_mask;
    vsf_exti_isr_handler_t  *handler_fn;
    void                    *target_ptr;
} VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_irq_t);

typedef struct VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_t) {
#if VSF_HW_EXTI_CFG_MULTI_CLASS == ENABLED
    vsf_exti_t                  vsf_exti;
#endif
    exint_type                  *reg;
    VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_irq_t)
                                irq[VSF_HW_EXTI_IRQ_NUM];
} VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_t);
// HW end

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_config_channels)(
    VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_t) *exti_ptr,
    vsf_exti_channel_mask_t channel_mask,
    vsf_exti_channel_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != exti_ptr);
    VSF_HAL_ASSERT(!(channel_mask & ~VSF_HW_EXTI_CHANNEL_MASK));

    vsf_exti_mode_t mode = cfg_ptr->mode;
    exint_type *reg = exti_ptr->reg;

    if (mode & VSF_EXTI_MODE_RISING) {
        vsf_atom32_op(&reg->polcfg1, _ | channel_mask);
    } else {
        vsf_atom32_op(&reg->polcfg1, _ & ~channel_mask);
    }
    if (mode & VSF_EXTI_MODE_FALLING) {
        vsf_atom32_op(&reg->polcfg2, _ | channel_mask);
    } else {
        vsf_atom32_op(&reg->polcfg2, _ & ~channel_mask);
    }
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_init)(
    VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_t) *exti_ptr
) {
    VSF_HAL_ASSERT(NULL != exti_ptr);
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_fini)(
    VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_t) *exti_ptr
) {
    VSF_HAL_ASSERT(NULL != exti_ptr);
}

vsf_exti_status_t VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_status)(
    VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_t) *exti_ptr
) {
    VSF_HAL_ASSERT(NULL != exti_ptr);
    exint_type *reg = exti_ptr->reg;
    return (vsf_exti_status_t){
        .enable_mask        = reg->inten,
        .status_mask        = reg->intsts,
    };
}

vsf_err_t VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_irq_enable)(
    VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_t) *exti_ptr,
    vsf_exti_channel_mask_t channel_mask
) {
    VSF_HAL_ASSERT(NULL != exti_ptr);
    VSF_HAL_ASSERT(!(channel_mask & ~VSF_HW_EXTI_CHANNEL_MASK));
    vsf_atom32_op(&exti_ptr->reg->inten, _ | channel_mask);
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_irq_disable)(
    VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_t) *exti_ptr,
    vsf_exti_channel_mask_t channel_mask
) {
    VSF_HAL_ASSERT(NULL != exti_ptr);
    VSF_HAL_ASSERT(!(channel_mask & ~VSF_HW_EXTI_CHANNEL_MASK));
    vsf_atom32_op(&exti_ptr->reg->inten, _ & ~channel_mask);
    return VSF_ERR_NONE;
}

static void VSF_MCONNECT(__, VSF_EXTI_CFG_IMP_PREFIX, _exti_irqhandler)(
    VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_t) *exti_ptr,
    unsigned int irq_idx
) {
    VSF_HAL_ASSERT(NULL != exti_ptr);
    VSF_HAL_ASSERT(irq_idx < dimof(exti_ptr->irq))

    VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_irq_t) *irq = &exti_ptr->irq[irq_idx];
    exint_type *reg = exti_ptr->reg;
    vsf_exti_channel_mask_t irq_status = reg->intsts & irq->channel_mask;

    if ((irq_status != 0) && (irq->handler_fn != NULL)) {
        irq->handler_fn(irq->target_ptr, (vsf_exti_t *)exti_ptr, irq_status);
        reg->intsts |= irq_status;
    }
}

vsf_exti_capability_t VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_capability)(
    VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_t) *exti_ptr
) {
    vsf_exti_capability_t cap = {
        .support_level_trigger          = false,
        .irq_num                        = dimof(exti_ptr->irq),
    };
    for (int i = 0; i < dimof(exti_ptr->irq); i++) {
        cap.irq_channel_mask[i]         = exti_ptr->irq[i].channel_mask;
        cap.channel_mask                |= exti_ptr->irq[i].channel_mask;
    }
    return cap;
}

/*============================ INCLUDES ======================================*/

#define VSF_EXTI_IMP_IRQHANDLER(__IRQ_IDX, __EXTI_IDX)                          \
    void VSF_MCONNECT(VSF_HW_INTERRUPT, VSF_MCONNECT(VSF_EXTI_CFG_IMP_UPCASE_PREFIX, _EXTI, __EXTI_IDX, _IRQ, __IRQ_IDX, _IRQN))(void)\
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_EXTI_CFG_IMP_PREFIX, _exti_irqhandler)(            \
            &VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti, __EXTI_IDX),          \
            __IRQ_IDX                                                           \
        );                                                                      \
        vsf_hal_irq_leave(ctx);                                                 \
    }
#define VSF_EXTI_IMP_IRQ(__IRQ_IDX, __EXTI_IDX)                                 \
        [__IRQ_IDX] = {                                                         \
            .channel_mask = VSF_MCONNECT(VSF_EXTI_CFG_IMP_UPCASE_PREFIX, _EXTI, __EXTI_IDX, _IRQ, __IRQ_IDX, _MASK),\
        },

#define VSF_EXTI_CFG_IMP_LV0(__IDX, __HAL_OP)                                   \
    VSF_MREPEAT(VSF_MCONNECT(VSF_EXTI_CFG_IMP_UPCASE_PREFIX, _EXTI, __IDX, _IRQ_NUM), VSF_EXTI_IMP_IRQHANDLER, __IDX)\
    VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_t)                              \
        VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti, __IDX) = {                 \
        .reg = VSF_MCONNECT(VSF_EXTI_CFG_IMP_UPCASE_PREFIX, _EXTI, __IDX, _REG),\
        .irq = {                                                                \
            VSF_MREPEAT(VSF_MCONNECT(VSF_EXTI_CFG_IMP_UPCASE_PREFIX, _EXTI, __IDX, _IRQ_NUM), VSF_EXTI_IMP_IRQ, __IDX)\
        },                                                                      \
        __HAL_OP                                                                \
    };

#include "hal/driver/common/exti/exti_template.inc"

#endif      // VSF_HAL_USE_GPIO
