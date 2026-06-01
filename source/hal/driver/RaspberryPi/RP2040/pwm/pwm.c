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

#if VSF_HAL_USE_PWM == ENABLED

#include "hal/vsf_hal.h"
#include "hal/driver/vendor_driver.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_PWM_CFG_MULTI_CLASS
#   define VSF_HW_PWM_CFG_MULTI_CLASS              VSF_PWM_CFG_MULTI_CLASS
#endif

#define VSF_PWM_CFG_IMP_PREFIX                     vsf_hw
#define VSF_PWM_CFG_IMP_UPCASE_PREFIX              VSF_HW

#define VSF_PWM_CFG_REIMPLEMENT_API_IRQ_ENABLE     ENABLED
#define VSF_PWM_CFG_REIMPLEMENT_API_IRQ_DISABLE    ENABLED
#define VSF_PWM_CFG_REIMPLEMENT_API_IRQ_CLEAR      ENABLED

/* RP2040 PWM clock source is clk_sys (125 MHz typical).
 * Divider is 8.4 fixed-point: INT[11:4] + FRAC[3:0].
 * Counter is 16-bit, TOP is wrap value.
 * Frequency = clk_sys / divider / (TOP + 1) [trailing-edge]
 * Frequency = clk_sys / divider / (2 * TOP) [phase-correct]
 *
 * We use trailing-edge mode (PH_CORRECT = 0) as default.
 */

#define __RP2040_PWM_DIV_SHIFT                     4
#define __RP2040_PWM_MAX_TOP                       0xFFFFu
#define __RP2040_PWM_MAX_DIV_INT                   255
#define __RP2040_PWM_MIN_FREQ                      1

/* RP2040 PWM: each slice drives two channels (A/B). */
#define __RP2040_PWM_CHANNEL_PER_SLICE             2

/*============================ TYPES =========================================*/

typedef struct VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t) {
#if VSF_HW_PWM_CFG_MULTI_CLASS == ENABLED
    vsf_pwm_t               vsf_pwm;
#endif
    uint8_t                 slice_idx;
    uint8_t                 is_enabled;
    IRQn_Type               irqn;
    uint32_t                rst_bit;
    /* Cached configuration for get_freq */
    uint32_t                clk_sys_hz;
    uint32_t                div_value;      /* 8.4 fixed-point divider */
    uint16_t                top_value;
    vsf_pwm_isr_t           isr;
} VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/

/* Compute divider (8.4 fixed-point) from desired frequency and TOP value.
 * divider = clk_sys / freq / (TOP + 1)
 * Returns the 8.4 fixed-point value to write to DIV register.
 */
static uint32_t __rp2040_pwm_compute_div(uint32_t clk_sys, uint32_t freq, uint16_t top)
{
    uint32_t cycles = top + 1;
    /* divider = clk_sys / (freq * cycles), in 8.4 fixed-point */
    uint64_t div_16 = ((uint64_t)clk_sys << 4) / ((uint64_t)freq * cycles);
    if (div_16 < 16) {
        div_16 = 16;    /* minimum divider is 1.0 = 0x10 */
    }
    if (div_16 > 0xFFF) {
        div_16 = 0xFFF; /* max divider is 255 + 15/16 */
    }
    return (uint32_t)div_16;
}

/* Compute actual frequency from divider and TOP */
static uint32_t __rp2040_pwm_actual_freq(uint32_t clk_sys, uint32_t div_16, uint16_t top)
{
    uint32_t cycles = top + 1;
    /* freq = clk_sys / (divider * cycles), divider = div_16 / 16 */
    return (uint32_t)(((uint64_t)clk_sys * 16) / ((uint64_t)div_16 * cycles));
}

vsf_err_t VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_init)(
    VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t) *pwm_ptr,
    vsf_pwm_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT((NULL != pwm_ptr) && (NULL != cfg_ptr));

    uint8_t slice = pwm_ptr->slice_idx;
    pwm_slice_hw_t *slice_hw = &pwm_hw->slice[slice];

    /* Deassert reset. All PWM slices share one reset domain on RP2040. */
    uint32_t rst_bit = pwm_ptr->rst_bit;
    resets_hw->reset &= ~rst_bit;
    // spin-wait: reset deassert -> reset_done is a few clk_ref cycles (< 1 us)
    while (!(resets_hw->reset_done & rst_bit));

    /* Disable slice before configuration */
    slice_hw->csr = 0;
    pwm_ptr->is_enabled = 0;

    /* Reset counter */
    slice_hw->ctr = 0;

    /* Default TOP = 0xFFFF (max resolution) */
    uint16_t top = __RP2040_PWM_MAX_TOP;
    slice_hw->top = top;
    pwm_ptr->top_value = top;

    /* Set compare to 0 initially */
    slice_hw->cc = 0;

    /* Compute divider from requested frequency */
    uint32_t sys_hz = clock_get_hz(clk_sys);
    pwm_ptr->clk_sys_hz = sys_hz;

    uint32_t freq = cfg_ptr->freq;
    if (freq == 0) {
        return VSF_ERR_INVALID_PARAMETER;
    }

    uint32_t div_16 = __rp2040_pwm_compute_div(sys_hz, freq, top);
    pwm_ptr->div_value = div_16;
    slice_hw->div = div_16;

    pwm_ptr->isr = cfg_ptr->isr;

    /* NVIC configuration: RP2040 has a single PWM IRQ shared by all slices. */
    if (cfg_ptr->isr.handler_fn != NULL) {
        NVIC_SetPriority(pwm_ptr->irqn, (uint32_t)cfg_ptr->isr.prio);
        NVIC_EnableIRQ(pwm_ptr->irqn);
    } else {
        NVIC_DisableIRQ(pwm_ptr->irqn);
    }

    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_fini)(
    VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t) *pwm_ptr
) {
    VSF_HAL_ASSERT(pwm_ptr != NULL);

    uint8_t slice = pwm_ptr->slice_idx;
    pwm_hw->slice[slice].csr = 0;
    pwm_ptr->is_enabled = 0;

    NVIC_DisableIRQ(pwm_ptr->irqn);
}

fsm_rt_t VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_enable)(
    VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t) *pwm_ptr
) {
    VSF_HAL_ASSERT(pwm_ptr != NULL);

    uint8_t slice = pwm_ptr->slice_idx;
    pwm_hw->slice[slice].csr |= PWM_CH0_CSR_EN_BITS;
    pwm_ptr->is_enabled = 1;

    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_disable)(
    VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t) *pwm_ptr
) {
    VSF_HAL_ASSERT(pwm_ptr != NULL);

    uint8_t slice = pwm_ptr->slice_idx;
    pwm_hw->slice[slice].csr &= ~PWM_CH0_CSR_EN_BITS;
    pwm_ptr->is_enabled = 0;

    return fsm_rt_cpl;
}

vsf_err_t VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_set)(
    VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t) *pwm_ptr,
    uint8_t channel,
    uint32_t period,
    uint32_t pulse
) {
    VSF_HAL_ASSERT(pwm_ptr != NULL);
    VSF_HAL_ASSERT(channel < __RP2040_PWM_CHANNEL_PER_SLICE);

    uint8_t slice = pwm_ptr->slice_idx;
    pwm_slice_hw_t *slice_hw = &pwm_hw->slice[slice];

    /* period maps to TOP (wrap value), pulse maps to CC compare value */
    if (period > __RP2040_PWM_MAX_TOP) {
        period = __RP2040_PWM_MAX_TOP;
    }
    if (period == 0) {
        period = 1;
    }
    if (pulse > period) {
        pulse = period;
    }

    /* Update TOP if period changed */
    if ((uint16_t)period != pwm_ptr->top_value) {
        slice_hw->top = (uint16_t)period;
        pwm_ptr->top_value = (uint16_t)period;
    }

    /* Update compare value for the requested channel */
    uint32_t cc = slice_hw->cc;
    if (channel == 0) {
        cc = (cc & 0xFFFF0000u) | (pulse & 0xFFFFu);
    } else {
        cc = (cc & 0x0000FFFFu) | ((pulse & 0xFFFFu) << 16);
    }
    slice_hw->cc = cc;

    return VSF_ERR_NONE;
}

uint32_t VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_get_freq)(
    VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t) *pwm_ptr
) {
    VSF_HAL_ASSERT(pwm_ptr != NULL);

    return __rp2040_pwm_actual_freq(pwm_ptr->clk_sys_hz,
                                    pwm_ptr->div_value,
                                    pwm_ptr->top_value);
}

vsf_err_t VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_get_configuration)(
    VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t) *pwm_ptr,
    vsf_pwm_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != pwm_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    cfg_ptr->freq = __rp2040_pwm_actual_freq(pwm_ptr->clk_sys_hz,
                                              pwm_ptr->div_value,
                                              pwm_ptr->top_value);
    return VSF_ERR_NONE;
}

vsf_pwm_capability_t VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_capability)(
    VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t) *pwm_ptr
) {
    VSF_HAL_ASSERT(pwm_ptr != NULL);

    uint32_t sys_hz = clock_get_hz(clk_sys);
    uint32_t max_freq = sys_hz / 1 / 2;    /* divider=1, TOP=1 */
    uint32_t min_freq = sys_hz / 255 / (__RP2040_PWM_MAX_TOP + 1);
    if (min_freq < 1) {
        min_freq = 1;
    }

    return (vsf_pwm_capability_t) {
        .max_freq = max_freq,
        .min_freq = min_freq,
    };
}

vsf_err_t VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_ctrl)(
    VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t) *pwm_ptr,
    vsf_pwm_ctrl_t ctrl,
    void *param
) {
    VSF_HAL_ASSERT(pwm_ptr != NULL);

    VSF_HAL_ASSERT(0);
    return VSF_ERR_NOT_SUPPORT;
}

void VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_irq_enable)(
    VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t) *pwm_ptr,
    vsf_pwm_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(pwm_ptr != NULL);

    uint8_t slice = pwm_ptr->slice_idx;
    pwm_hw->inte |= (1u << slice);
    NVIC_EnableIRQ(pwm_ptr->irqn);
}

void VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_irq_disable)(
    VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t) *pwm_ptr,
    vsf_pwm_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(pwm_ptr != NULL);

    uint8_t slice = pwm_ptr->slice_idx;
    pwm_hw->inte &= ~(1u << slice);
}

vsf_pwm_irq_mask_t VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_irq_clear)(
    VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t) *pwm_ptr,
    vsf_pwm_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(pwm_ptr != NULL);

    uint8_t slice = pwm_ptr->slice_idx;
    uint32_t slice_mask = (1u << slice);
    uint32_t ints = pwm_hw->ints & slice_mask;

    if (ints) {
        pwm_hw->intr = slice_mask;
        return VSF_PWM_IRQ_MASK_WRAP;
    }
    return 0;
}

/*============================ MACROS ========================================*/

#define VSF_PWM_CFG_MODE_CHECK_UNIQUE                 VSF_HAL_CHECK_MODE_LOOSE
#define VSF_PWM_CFG_IRQ_MASK_CHECK_UNIQUE             VSF_HAL_CHECK_MODE_STRICT
#define VSF_PWM_CFG_REIMPLEMENT_API_CAPABILITY        ENABLED
#define VSF_PWM_CFG_REIMPLEMENT_API_CTRL              ENABLED
#define VSF_PWM_CFG_REIMPLEMENT_API_GET_CONFIGURATION ENABLED

#define VSF_PWM_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t)                                \
    VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm, __IDX) = {                       \
        .slice_idx = __IDX,                                                     \
        .irqn =                                                                 \
            VSF_MCONNECT(VSF_PWM_CFG_IMP_UPCASE_PREFIX, _PWM, __IDX, _IRQN),    \
        .rst_bit = VSF_MCONNECT(VSF_PWM_CFG_IMP_UPCASE_PREFIX, _PWM, __IDX,     \
                                _RST_BIT),                                      \
        __HAL_OP};

#include "hal/driver/common/pwm/pwm_template.inc"

// RP2040 has one NVIC IRQ line for all 8 PWM slices (shared, not per-instance).
// The handler lives outside IMP_LV0 for this reason.
// The handler name omits the slice index to reflect the shared nature.
VSF_CAL_ROOT void VSF_HW_PWM_IRQHandler(void) // quality: allow-hardcoded-irq
{
    uintptr_t ctx = vsf_hal_irq_enter();
    uint32_t ints = pwm_hw->ints;
    for (uint8_t slice = 0; slice < VSF_HW_PWM_COUNT; slice++) {
        uint32_t slice_mask = (1u << slice);
        if (ints & slice_mask) {
            vsf_hw_pwm_t *pwm_ptr = vsf_hw_pwms[slice];
            if (pwm_ptr->isr.handler_fn != NULL) {
                pwm_ptr->isr.handler_fn(pwm_ptr->isr.target_ptr,
                                        (vsf_pwm_t *)pwm_ptr,
                                        VSF_PWM_IRQ_MASK_WRAP);
            }
        }
    }
    pwm_hw->intr = ints;
    vsf_hal_irq_leave(ctx);
}

#endif /* VSF_HAL_USE_PWM */
