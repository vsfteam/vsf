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

#define VSF_I2S_CFG_IMP_PREFIX                  vsf_hw
#define VSF_I2S_CFG_IMP_UPCASE_PREFIX           VSF_HW

/*============================ INCLUDES ======================================*/

#include "./i2s.h"

#if VSF_HAL_USE_I2S == ENABLED

#include "../vendor/plf/aic8800/src/driver/sysctrl/reg_sysctrl.h"
#include "../vendor/plf/aic8800/src/driver/sysctrl/sysctrl_api.h"
#include "../vendor/plf/aic8800/src/driver/aud_proc/aud_proc.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_I2S_CFG_MULTI_CLASS
#   define VSF_HW_I2S_CFG_MULTI_CLASS           VSF_I2S_CFG_MULTI_CLASS
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_hw_i2s_const_t {
    HWP_AUD_PROC_T *reg;
    uint8_t idx;
    // TODO: move to pm
    uint32_t oclk;
} vsf_hw_i2s_const_t;

typedef struct vsf_hw_i2s_t {
#if VSF_HW_I2S_CFG_MULTI_CLASS == ENABLED
    vsf_i2s_t vsf_i2s;
#endif

    const vsf_hw_i2s_const_t *i2s_const;
    vsf_i2s_isr_t       isr;

    vsf_i2s_cfg_t       cfg;
    vsf_i2s_irq_mask_t  irq_mask;
} vsf_hw_i2s_t;

/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __vsf_hw_i2s_irq_handler(vsf_hw_i2s_t *hw_i2s_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2s_ptr);
    const vsf_hw_i2s_const_t * hw_i2s_const = hw_i2s_ptr->i2s_const;
    VSF_HAL_ASSERT(NULL != hw_i2s_const);

    // TODO: move to pm
    cpusysctrl_hclkme_set(CSC_HCLKME_DMA_EN_BIT | CSC_HCLKME_VPC_EN_BIT);
    cpusysctrl_oclkme_set(hw_i2s_const->oclk);


}

vsf_err_t vsf_hw_i2s_init(vsf_hw_i2s_t *hw_i2s_ptr, vsf_i2s_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2s_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    const vsf_hw_i2s_const_t * hw_i2s_const = hw_i2s_ptr->i2s_const;
    VSF_HAL_ASSERT(NULL != hw_i2s_const);

    hw_i2s_ptr->cfg = *cfg_ptr;

    return VSF_ERR_NONE;
}

vsf_i2s_capability_t vsf_hw_i2s_capability(vsf_hw_i2s_t *hw_i2s_ptr)
{
    VSF_HAL_ASSERT(hw_i2s_ptr != NULL);

    vsf_i2s_capability_t capability = {
        .i2s_capability             = {
            .is_src_supported       = true,
        },
    };
    return capability;
}

void vsf_hw_i2s_fini(vsf_hw_i2s_t *hw_i2s_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2s_ptr);
}

fsm_rt_t vsf_hw_i2s_enable(vsf_hw_i2s_t *hw_i2s_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2s_ptr);

    return fsm_rt_cpl;
}

fsm_rt_t vsf_hw_i2s_disable(vsf_hw_i2s_t *hw_i2s_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2s_ptr);

    return fsm_rt_cpl;
}

void vsf_hw_i2s_irq_enable(vsf_hw_i2s_t *hw_i2s_ptr, vsf_i2s_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != hw_i2s_ptr);
    VSF_HAL_ASSERT((irq_mask & ~I2S_IRQ_MASK_ALL) == 0);

    hw_i2s_ptr->irq_mask |= irq_mask;
}

void vsf_hw_i2s_irq_disable(vsf_hw_i2s_t *hw_i2s_ptr, vsf_i2s_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != hw_i2s_ptr);
    const vsf_hw_i2s_const_t * hw_i2s_const = hw_i2s_ptr->i2s_const;
    VSF_HAL_ASSERT(NULL != hw_i2s_const);
    VSF_HAL_ASSERT((irq_mask & ~I2S_IRQ_MASK_ALL) == 0);

    hw_i2s_ptr->irq_mask &= ~irq_mask;
    if (hw_i2s_ptr->irq_mask == 0) {
//        NVIC_DisableIRQ(hw_i2s_const->irqn);
    }
}

vsf_i2s_status_t vsf_hw_i2s_status(vsf_hw_i2s_t *hw_i2s_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2s_ptr);

    vsf_i2s_status_t status = {
        .use_as__peripheral_status_t.is_busy = false,
    };

    return status;
}

vsf_err_t vsf_hw_i2s_request_rx(vsf_hw_i2s_t *hw_i2s_ptr, void *buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT(hw_i2s_ptr != NULL);
    VSF_HAL_ASSERT(buffer_ptr != NULL);
    VSF_HAL_ASSERT(count > 0);

    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_i2s_request_tx(vsf_hw_i2s_t *hw_i2s_ptr, void *buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT(hw_i2s_ptr != NULL);
    VSF_HAL_ASSERT(buffer_ptr != NULL);
    VSF_HAL_ASSERT(count > 0);

    return VSF_ERR_NONE;
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_I2S_CFG_IMP_LV0(__COUNT, __HAL_OP)                                  \
    static const vsf_hw_i2s_const_t __vsf_hw_i2s ## __COUNT ## _const = {       \
        .reg        = VSF_HW_I2S ## __COUNT ## _REG,                            \
        .idx        = VSF_HW_I2S ## __COUNT ## _IDX,                            \
        .oclk       = VSF_HW_I2S ## __COUNT ## _OCLK,                           \
    };                                                                          \
    vsf_hw_i2s_t vsf_hw_i2s ## __COUNT = {                                      \
        .i2s_const  = &__vsf_hw_i2s ## __COUNT ## _const,                       \
        __HAL_OP                                                                \
    };                                                                          \
    void VSF_HW_I2S ## __COUNT ## _IRQ(void)                                    \
    {                                                                           \
        __vsf_hw_i2s_irq_handler(&vsf_hw_i2s ## __COUNT);                       \
    }
#include "hal/driver/common/i2s/i2s_template.inc"

#endif /* VSF_HAL_USE_I2S */
