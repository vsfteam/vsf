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

#include "./adc.h"

#if VSF_HAL_USE_ADC == ENABLED

#include "hal/vsf_hal.h"
#include "hal/driver/vendor_driver.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_ADC_CFG_MULTI_CLASS
#   define VSF_HW_ADC_CFG_MULTI_CLASS          VSF_ADC_CFG_MULTI_CLASS
#endif

#define VSF_ADC_CFG_IMP_PREFIX                  vsf_hw
#define VSF_ADC_CFG_IMP_UPCASE_PREFIX           VSF_HW

/* Supported IRQ mask — must match capability() */
#define __VSF_HW_ADC_SUPPORTED_IRQ_MASK         VSF_ADC_IRQ_MASK_CPL

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) {
#if VSF_HW_ADC_CFG_MULTI_CLASS == ENABLED
    vsf_adc_t               vsf_adc;
#endif
    adc_hw_t                *reg;
    IRQn_Type               irqn;
    uint32_t                rst_bit;
    vsf_adc_isr_t           isr;
    vsf_adc_cfg_t           cfg;
    struct {
        uint32_t            is_enabled : 1;
        uint32_t            is_busy    : 1;
        uint32_t                       : 30;
    } status;
    uint16_t                *buffer;
    uint_fast32_t           count;
    uint_fast32_t           offset;
} VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static inline void __vsf_hw_adc_irq_clear_pending(adc_hw_t *reg)
{
    // INTS is clear-on-read: reading it acknowledges all pending interrupts.
    VSF_UNUSED_PARAM(reg->ints);
}

/*============================ IMPLEMENTATION ================================*/

vsf_err_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_init)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr,
    vsf_adc_cfg_t *cfg_ptr
)
{
    VSF_HAL_ASSERT(NULL != adc_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    adc_hw_t *reg = adc_ptr->reg;
    VSF_HAL_ASSERT(NULL != reg);

    // Unreset ADC block
    uint32_t rst_bit = adc_ptr->rst_bit;
    resets_hw->reset &= ~rst_bit;
    // Spin-wait: reset deassert -> reset_done is a few clk_ref cycles (< 1 us).
    // If this hangs, the peripheral clock or reset wiring is misconfigured.
    while (!(resets_hw->reset_done & rst_bit));

    // Store config
    adc_ptr->cfg = *cfg_ptr;
    adc_ptr->isr = cfg_ptr->isr;

    if (cfg_ptr->isr.handler_fn != NULL) {
        NVIC_SetPriority(adc_ptr->irqn, (uint32_t)cfg_ptr->isr.prio);
        NVIC_EnableIRQ(adc_ptr->irqn);
    }

    adc_ptr->status.is_enabled = false;
    adc_ptr->status.is_busy    = false;

    // Disable ADC before configuration
    reg->cs = 0;

    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_fini)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr
)
{
    VSF_HAL_ASSERT(NULL != adc_ptr);

    adc_hw_t *reg = adc_ptr->reg;
    VSF_HAL_ASSERT(NULL != reg);
    reg->cs = 0;
    adc_ptr->status.is_enabled = false;
    adc_ptr->status.is_busy    = false;
}

fsm_rt_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_enable)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr
)
{
    VSF_HAL_ASSERT(NULL != adc_ptr);

    adc_hw_t *reg = adc_ptr->reg;
    VSF_HAL_ASSERT(NULL != reg);
    reg->cs |= ADC_CS_EN_BITS;
    adc_ptr->status.is_enabled = true;

    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_disable)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr
)
{
    VSF_HAL_ASSERT(NULL != adc_ptr);

    adc_hw_t *reg = adc_ptr->reg;
    VSF_HAL_ASSERT(NULL != reg);
    reg->cs &= ~ADC_CS_EN_BITS;
    adc_ptr->status.is_enabled = false;

    return fsm_rt_cpl;
}

void VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_irq_enable)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr,
    vsf_adc_irq_mask_t irq_mask
)
{
    VSF_HAL_ASSERT(NULL != adc_ptr);
    VSF_HAL_ASSERT(0 == (irq_mask & ~__VSF_HW_ADC_SUPPORTED_IRQ_MASK));

    adc_hw_t *reg = adc_ptr->reg;
    VSF_HAL_ASSERT(NULL != reg);
    if (irq_mask & VSF_ADC_IRQ_MASK_CPL) {
        reg->inte |= ADC_INTE_FIFO_BITS;
    }
}

void VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_irq_disable)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr,
    vsf_adc_irq_mask_t irq_mask
)
{
    VSF_HAL_ASSERT(NULL != adc_ptr);
    VSF_HAL_ASSERT(0 == (irq_mask & ~__VSF_HW_ADC_SUPPORTED_IRQ_MASK));

    adc_hw_t *reg = adc_ptr->reg;
    VSF_HAL_ASSERT(NULL != reg);
    if (irq_mask & VSF_ADC_IRQ_MASK_CPL) {
        reg->inte &= ~ADC_INTE_FIFO_BITS;
    }
}

vsf_adc_irq_mask_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_irq_clear)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr,
    vsf_adc_irq_mask_t irq_mask
)
{
    VSF_HAL_ASSERT(NULL != adc_ptr);
    VSF_HAL_ASSERT(0 == (irq_mask & ~__VSF_HW_ADC_SUPPORTED_IRQ_MASK));

    adc_hw_t *reg = adc_ptr->reg;
    VSF_HAL_ASSERT(NULL != reg);
    vsf_adc_irq_mask_t cleared = 0;

    if (irq_mask & VSF_ADC_IRQ_MASK_CPL) {
        __vsf_hw_adc_irq_clear_pending(reg);
        cleared |= VSF_ADC_IRQ_MASK_CPL;
    }

    return cleared;
}

vsf_adc_status_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_status)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr
)
{
    VSF_HAL_ASSERT(NULL != adc_ptr);

    adc_hw_t *reg = adc_ptr->reg;
    VSF_HAL_ASSERT(NULL != reg);
    vsf_adc_status_t status = {
        .is_busy = !(reg->cs & ADC_CS_READY_BITS),
    };

    return status;
}

vsf_adc_capability_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_capability)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr
)
{
    VSF_HAL_ASSERT(NULL != adc_ptr);

    return (vsf_adc_capability_t) {
        .irq_mask      = VSF_ADC_IRQ_MASK_CPL,
        .max_data_bits = VSF_HW_ADC_MAX_DATA_BITS,
        .channel_count = VSF_HW_ADC_CHANNEL_COUNT,
    };
}

vsf_err_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_get_configuration)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr,
    vsf_adc_cfg_t *cfg_ptr
)
{
    VSF_HAL_ASSERT(NULL != adc_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    *cfg_ptr = adc_ptr->cfg;
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_channel_config)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr,
    vsf_adc_channel_cfg_t *channel_cfgs_ptr,
    uint32_t channel_cfgs_cnt
)
{
    VSF_HAL_ASSERT(NULL != adc_ptr);
    VSF_HAL_ASSERT(NULL != channel_cfgs_ptr);
    VSF_HAL_ASSERT(0 != channel_cfgs_cnt);

    // RP2040 ADC channel config is minimal:
    // - For channels 0-3, GPIO26-29 must be configured as analog input by user/BSP
    // - For channel 4 (temp sensor), TS_EN is handled in request_once
    // We just validate channel numbers here.
    for (uint32_t i = 0; i < channel_cfgs_cnt; i++) {
        if (channel_cfgs_ptr[i].channel >= VSF_HW_ADC_CHANNEL_COUNT) {
            return VSF_ERR_INVALID_RANGE;
        }
    }

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_channel_request_once)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr,
    vsf_adc_channel_cfg_t *channel_cfg_ptr,
    void *buffer_ptr
)
{
    VSF_HAL_ASSERT(NULL != adc_ptr);
    VSF_HAL_ASSERT(NULL != channel_cfg_ptr);
    VSF_HAL_ASSERT(NULL != buffer_ptr);

    adc_hw_t *reg = adc_ptr->reg;
    VSF_HAL_ASSERT(NULL != reg);
    uint8_t channel = channel_cfg_ptr->channel;

    if (channel >= VSF_HW_ADC_CHANNEL_COUNT) {
        return VSF_ERR_INVALID_RANGE;
    }

    if (!adc_ptr->status.is_enabled) {
        return VSF_ERR_NOT_READY;
    }

    // Wait for previous conversion to complete
    // Spin-wait: ADC conversion time is fixed by 96 clk_adc cycles.
    // No IRQ for single-shot completion in polling mode; must wait before starting next.
    while (!(reg->cs & ADC_CS_READY_BITS));

    adc_ptr->status.is_busy = true;

    // Set channel and enable temp sensor if needed
    // RP2040 channel 4 is the internal temperature sensor; GPIO channels 0-3 must
    // be configured as analog inputs by the user/BSP before sampling.
    uint32_t cs = reg->cs;
    cs &= ~(ADC_CS_AINSEL_BITS | ADC_CS_RROBIN_BITS);
    cs |= ((uint32_t)channel << ADC_CS_AINSEL_LSB);
    if (channel == VSF_HW_ADC_TEMP_SENSOR_CHANNEL) {
        cs |= ADC_CS_TS_EN_BITS;
    } else {
        cs &= ~ADC_CS_TS_EN_BITS;
    }
    reg->cs = cs;

    // Start single conversion
    reg->cs |= ADC_CS_START_ONCE_BITS;

    // Poll until conversion completes
    // Spin-wait: same 96 clk_adc cycles (~2 us at 48 MHz). Must read RESULT after READY.
    while (!(reg->cs & ADC_CS_READY_BITS));

    // Read 12-bit result
    uint16_t result = (uint16_t)(reg->result & ADC_RESULT_BITS);

    // Store result (12-bit right-aligned)
    *(uint16_t *)buffer_ptr = result;

    adc_ptr->status.is_busy = false;

    // VSF HAL convention: callback on completion regardless of polling/IRQ path
    if (adc_ptr->isr.handler_fn != NULL) {
        adc_ptr->isr.handler_fn(adc_ptr->isr.target_ptr,
                                (vsf_adc_t *)adc_ptr,
                                VSF_ADC_IRQ_MASK_CPL);
    }

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_channel_request)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr,
    void *buffer_ptr,
    uint_fast32_t count
)
{
    VSF_HAL_ASSERT(NULL != adc_ptr);
    VSF_HAL_ASSERT(NULL != buffer_ptr);
    VSF_HAL_ASSERT(count > 0);

    if (!adc_ptr->status.is_enabled) {
        return VSF_ERR_NOT_READY;
    }

    adc_hw_t *reg = adc_ptr->reg;
    VSF_HAL_ASSERT(NULL != reg);

    adc_ptr->buffer = (uint16_t *)buffer_ptr;
    adc_ptr->count  = count;
    adc_ptr->offset = 0;
    adc_ptr->status.is_busy = true;

    /* Enable FIFO and set threshold to 1 (interrupt after each sample) */
    reg->fcs = ADC_FCS_EN_BITS | (1u << ADC_FCS_THRESH_LSB);

    /* Enable FIFO interrupt */
    reg->inte |= ADC_INTE_FIFO_BITS;

    /* Start free-running conversions */
    reg->cs |= ADC_CS_START_MANY_BITS;

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_ctrl)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr,
    vsf_adc_ctrl_t ctrl,
    void *param
)
{
    VSF_HAL_ASSERT(NULL != adc_ptr);

    switch (ctrl) {
    default:
        return VSF_ERR_NOT_SUPPORT;
    }
}

static void VSF_MCONNECT(__, VSF_ADC_CFG_IMP_PREFIX, _adc_irqhandler)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr
)
{
    VSF_HAL_ASSERT(NULL != adc_ptr);

    adc_hw_t *reg = adc_ptr->reg;
    VSF_HAL_ASSERT(NULL != reg);
    __vsf_hw_adc_irq_clear_pending(reg);

    if (adc_ptr->status.is_busy && adc_ptr->offset < adc_ptr->count) {
        /* Drain FIFO entries into buffer */
        uint32_t level = (reg->fcs & ADC_FCS_LEVEL_BITS) >> ADC_FCS_LEVEL_LSB;
        while (level-- > 0 && adc_ptr->offset < adc_ptr->count) {
            adc_ptr->buffer[adc_ptr->offset++] = (uint16_t)(reg->fifo & 0xFFF);
        }

        /* If all samples collected, stop free-running and signal completion */
        if (adc_ptr->offset >= adc_ptr->count) {
            reg->cs &= ~ADC_CS_START_MANY_BITS;
            reg->inte &= ~ADC_INTE_FIFO_BITS;
            adc_ptr->status.is_busy = false;

            if (adc_ptr->isr.handler_fn != NULL) {
                adc_ptr->isr.handler_fn(adc_ptr->isr.target_ptr,
                                        (vsf_adc_t *)adc_ptr,
                                        VSF_ADC_IRQ_MASK_CPL);
            }
        }
    } else if (adc_ptr->isr.handler_fn != NULL) {
        adc_ptr->isr.handler_fn(adc_ptr->isr.target_ptr,
                                (vsf_adc_t *)adc_ptr,
                                VSF_ADC_IRQ_MASK_CPL);
    }
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_ADC_CFG_MODE_CHECK_UNIQUE                       VSF_HAL_CHECK_MODE_LOOSE
#define VSF_ADC_CFG_IRQ_MASK_CHECK_UNIQUE                   VSF_HAL_CHECK_MODE_STRICT
#define VSF_ADC_CFG_REIMPLEMENT_API_CAPABILITY              ENABLED
#define VSF_ADC_CFG_REIMPLEMENT_API_GET_CONFIGURATION       ENABLED
#define VSF_ADC_CFG_REIMPLEMENT_API_IRQ_CLEAR               ENABLED
#define VSF_ADC_CFG_REIMPLEMENT_API_CTRL                    ENABLED

#define VSF_ADC_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t)                                \
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc, __IDX) = {                       \
        .reg = (adc_hw_t *)VSF_MCONNECT(VSF_ADC_CFG_IMP_UPCASE_PREFIX, _ADC,    \
                                        __IDX, _REG),                           \
        .rst_bit = VSF_MCONNECT(VSF_ADC_CFG_IMP_UPCASE_PREFIX, _ADC, __IDX,     \
                                _RST_BIT),                                      \
        .irqn    = VSF_MCONNECT(VSF_ADC_CFG_IMP_UPCASE_PREFIX, _ADC, __IDX, _IRQN), \
        __HAL_OP};                                                              \
    VSF_CAL_ROOT void VSF_MCONNECT(VSF_ADC_CFG_IMP_UPCASE_PREFIX, _ADC, __IDX,  \
                                   _IRQHandler)(void) {                         \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_ADC_CFG_IMP_PREFIX, _adc_irqhandler)(              \
            &VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc, __IDX));                \
        vsf_hal_irq_leave(ctx);                                                 \
    }

#include "hal/driver/common/adc/adc_template.inc"

#endif      // VSF_HAL_USE_ADC
/* EOF */
