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

#include "./timer.h"

#if VSF_HAL_USE_TIMER == ENABLED

#include "hal/vsf_hal.h"
#include "hal/driver/vendor_driver.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_TIMER_CFG_MULTI_CLASS
#   define VSF_HW_TIMER_CFG_MULTI_CLASS          VSF_TIMER_CFG_MULTI_CLASS
#endif

#define VSF_TIMER_CFG_IMP_PREFIX                  vsf_hw
#define VSF_TIMER_CFG_IMP_UPCASE_PREFIX           VSF_HW

/* RP2040 has one timer block with 4 alarms.
 * We model it as 4 timer instances, each with 2 channels (alarms).
 * Timer 0 -> alarms 0,1  (IRQ TIMER_IRQ_0)
 * Timer 1 -> alarms 2,3  (IRQ TIMER_IRQ_1)
 * Timer 2 -> alarms 0,1  (IRQ TIMER_IRQ_2)
 * Timer 3 -> alarms 2,3  (IRQ TIMER_IRQ_3)
 *
 * Actually, RP2040 has 4 independent IRQ lines for the timer block,
 * each can be mapped to any alarm via the NVIC.
 * For simplicity, we map timer instance N to TIMER_IRQ_N.
 * All timers share the same timer_hw register block.
 */

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_hw_timer_t {
#if VSF_HW_TIMER_CFG_MULTI_CLASS == ENABLED
    vsf_timer_t               vsf_timer;
#endif
    void                      *reg;
    IRQn_Type                 irqn;
    vsf_timer_isr_t           isr;
    uint32_t                  period;
    vsf_timer_channel_cfg_t   channel_cfg[VSF_HW_TIMER_CHANNEL_COUNT];
    uint8_t                   channel_enabled;
    vsf_timer_irq_mask_t      pending_irq;
} vsf_hw_timer_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static uint8_t __rp2040_timer_idx(VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) *timer_ptr)
{
    for (uint8_t i = 0; i < VSF_HW_TIMER_COUNT; i++) {
        extern VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t)
            VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer, 0);
        if (timer_ptr == &VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer, 0) + i) {
            return i;
        }
    }
    return 0;
}

static uint8_t __rp2040_timer_alarm_index(uint8_t timer_idx, uint8_t channel)
{
    /* Map timer instance + channel to alarm number.
     * Timer 0: alarms 0,1
     * Timer 1: alarms 2,3
     * Timer 2: alarms 0,1
     * Timer 3: alarms 2,3
     */
    uint8_t base = (timer_idx % 2) * 2;
    return base + channel;
}

vsf_err_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_init)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) *timer_ptr,
    vsf_timer_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((NULL != timer_ptr) && (NULL != cfg_ptr));

    // no reset: RP2040 timer is always-on, no per-instance reset control
    // no clock gate: RP2040 timer has no independent clock gate

    timer_ptr->isr = cfg_ptr->isr;
    timer_ptr->period = cfg_ptr->period;
    timer_ptr->channel_enabled = 0;
    timer_ptr->pending_irq = 0;

    if (timer_ptr->isr.handler_fn != NULL) {
        NVIC_SetPriority(timer_ptr->irqn, (uint32_t)timer_ptr->isr.prio);
        NVIC_EnableIRQ(timer_ptr->irqn);
    } else {
        NVIC_DisableIRQ(timer_ptr->irqn);
    }

    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_fini)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) *timer_ptr)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);

    timer_hw_t *hw = (timer_hw_t *)timer_ptr->reg;
    uint8_t timer_idx = __rp2040_timer_idx(timer_ptr);

    for (uint8_t ch = 0; ch < VSF_HW_TIMER_CHANNEL_COUNT; ch++) {
        uint8_t alarm = __rp2040_timer_alarm_index(timer_idx, ch);
        hw->inte &= ~(1u << alarm);
    }
    NVIC_DisableIRQ(timer_ptr->irqn);
    timer_ptr->channel_enabled = 0;
    timer_ptr->pending_irq = 0;
}

fsm_rt_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_enable)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) *timer_ptr)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
    /* RP2040 timer counter is always running, nothing to do */
    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_disable)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) *timer_ptr)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
    /* RP2040 timer counter cannot be stopped (except via PAUSE bit which pauses globally) */
    return fsm_rt_cpl;
}

void VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_irq_enable)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) *timer_ptr,
    vsf_timer_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
    VSF_UNUSED_PARAM(irq_mask);

    /* NVIC is enabled in init() when isr.handler_fn is set.
     * Per-channel alarm enable is handled via timer_ctrl(). */
}

void VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_irq_disable)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) *timer_ptr,
    vsf_timer_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
    VSF_UNUSED_PARAM(irq_mask);

    /* NVIC is disabled in fini().
     * Per-channel alarm disable is handled via timer_ctrl(). */
}

vsf_timer_irq_mask_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_irq_clear)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) *timer_ptr,
    vsf_timer_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
    VSF_UNUSED_PARAM(irq_mask);

    timer_hw_t *hw = (timer_hw_t *)timer_ptr->reg;
    uint8_t timer_idx = __rp2040_timer_idx(timer_ptr);
    uint8_t alarm0 = __rp2040_timer_alarm_index(timer_idx, 0);
    uint8_t alarm1 = __rp2040_timer_alarm_index(timer_idx, 1);
    uint32_t alarm_mask = (1u << alarm0) | (1u << alarm1);

    /* Clear any remaining raw interrupt bits. */
    hw->intr = alarm_mask;

    vsf_timer_irq_mask_t result = timer_ptr->pending_irq;
    timer_ptr->pending_irq = 0;
    return result;
}

vsf_timer_status_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_status)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) *timer_ptr)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
    return (vsf_timer_status_t) {
        .value = timer_ptr->channel_enabled != 0,
    };
}

vsf_err_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_get_configuration)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) *timer_ptr,
    vsf_timer_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != timer_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    cfg_ptr->isr = timer_ptr->isr;
    cfg_ptr->period = timer_ptr->period;
    return VSF_ERR_NONE;
}

vsf_timer_capability_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_capability)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) *timer_ptr)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);

    return (vsf_timer_capability_t){
        .irq_mask               = VSF_TIMER_IRQ_MASK_OVERFLOW,
        .timer_bitlen           = 32,
        .support_pwm            = 0,
        .support_output_compare = 0,
        .support_input_capture  = 0,
        .support_one_pulse      = 0,
        .channel_cnt            = VSF_HW_TIMER_CHANNEL_COUNT,
    };
}

vsf_err_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_set_period)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) *timer_ptr,
    uint32_t period)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
    VSF_UNUSED_PARAM(period);
    VSF_HAL_ASSERT(0);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_ctrl)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) *timer_ptr,
    vsf_timer_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
    VSF_UNUSED_PARAM(ctrl);
    VSF_UNUSED_PARAM(param);
    VSF_HAL_ASSERT(0);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_config)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) *timer_ptr,
    uint8_t channel, vsf_timer_channel_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((timer_ptr != NULL) && (cfg_ptr != NULL));
    VSF_HAL_ASSERT(channel < VSF_HW_TIMER_CHANNEL_COUNT);

    timer_ptr->channel_cfg[channel] = *cfg_ptr;
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_start)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) *timer_ptr,
    uint8_t channel)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
    VSF_HAL_ASSERT(channel < VSF_HW_TIMER_CHANNEL_COUNT);

    timer_hw_t *hw = (timer_hw_t *)timer_ptr->reg;
    uint8_t timer_idx = __rp2040_timer_idx(timer_ptr);
    uint8_t alarm = __rp2040_timer_alarm_index(timer_idx, channel);
    uint32_t period = timer_ptr->channel_cfg[channel].pulse;
    if (period == 0) {
        period = timer_ptr->period;
    }

    /* Compute alarm target: current time + period (in us ticks) */
    uint32_t now = hw->timerawl;
    uint32_t target = now + period;

    /* Clear any pending interrupt before arming */
    hw->intr = (1u << alarm);

    /* Write alarm value and enable interrupt */
    hw->alarm[alarm] = target;
    hw->inte |= (1u << alarm);

    NVIC_ClearPendingIRQ(timer_ptr->irqn);
    NVIC_EnableIRQ(timer_ptr->irqn);

    timer_ptr->channel_enabled |= (1u << channel);

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_stop)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) *timer_ptr,
    uint8_t channel)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
    VSF_HAL_ASSERT(channel < VSF_HW_TIMER_CHANNEL_COUNT);

    timer_hw_t *hw = (timer_hw_t *)timer_ptr->reg;
    uint8_t timer_idx = __rp2040_timer_idx(timer_ptr);
    uint8_t alarm = __rp2040_timer_alarm_index(timer_idx, channel);
    hw->inte &= ~(1u << alarm);
    /* Disarm the alarm by writing to ARMED register */
    hw->armed = (1u << alarm);

    timer_ptr->channel_enabled &= ~(1u << channel);

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_request_start)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) *timer_ptr,
    uint8_t channel, vsf_timer_channel_request_t *request_ptr)
{
    VSF_HAL_ASSERT((timer_ptr != NULL) && (request_ptr != NULL));
    VSF_HAL_ASSERT(channel < VSF_HW_TIMER_CHANNEL_COUNT);

    if (request_ptr->length > 0 && request_ptr->period_buffer != NULL) {
        timer_ptr->channel_cfg[channel].pulse = request_ptr->period_buffer[0];
    }

    return VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_start)(timer_ptr, channel);
}

vsf_err_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_request_stop)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) *timer_ptr,
    uint8_t channel)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
    VSF_HAL_ASSERT(channel < VSF_HW_TIMER_CHANNEL_COUNT);

    return VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_stop)(timer_ptr, channel);
}

vsf_err_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_ctrl)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) *timer_ptr,
    uint8_t channel, vsf_timer_channel_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
    VSF_UNUSED_PARAM(channel);
    VSF_UNUSED_PARAM(ctrl);
    VSF_UNUSED_PARAM(param);
    VSF_HAL_ASSERT(0);
    return VSF_ERR_NOT_SUPPORT;
}

static void VSF_MCONNECT(__, VSF_TIMER_CFG_IMP_PREFIX, _timer_irqhandler)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) *timer_ptr,
    uint8_t alarm0, uint8_t alarm1)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);

    timer_hw_t *hw = (timer_hw_t *)timer_ptr->reg;
    uint32_t ints = hw->ints;
    vsf_timer_irq_mask_t irq_mask = 0;

    /* Check which alarm fired for this timer instance */
    if (ints & (1u << alarm0)) {
        hw->intr = (1u << alarm0);  /* clear interrupt */
        irq_mask |= VSF_TIMER_IRQ_MASK_OVERFLOW;

        /* Re-arm if periodic mode */
        if ((timer_ptr->channel_enabled & 0x01)
            && (timer_ptr->channel_cfg[0].mode & VSF_TIMER_BASE_CONTINUES)) {
            uint32_t period = timer_ptr->channel_cfg[0].pulse;
            if (period == 0) {
                period = timer_ptr->period;
            }
            hw->alarm[alarm0] = hw->timerawl + period;
        } else {
            timer_ptr->channel_enabled &= ~0x01;
        }
    }

    if (ints & (1u << alarm1)) {
        hw->intr = (1u << alarm1);  /* clear interrupt */
        irq_mask |= VSF_TIMER_IRQ_MASK_OVERFLOW;

        /* Re-arm if periodic mode */
        if ((timer_ptr->channel_enabled & 0x02)
            && (timer_ptr->channel_cfg[1].mode & VSF_TIMER_BASE_CONTINUES)) {
            uint32_t period = timer_ptr->channel_cfg[1].pulse;
            if (period == 0) {
                period = timer_ptr->period;
            }
            hw->alarm[alarm1] = hw->timerawl + period;
        } else {
            timer_ptr->channel_enabled &= ~0x02;
        }
    }

    if (irq_mask != 0) {
        timer_ptr->pending_irq |= irq_mask;
        if (timer_ptr->isr.handler_fn != NULL) {
            timer_ptr->isr.handler_fn(timer_ptr->isr.target_ptr,
                                      (vsf_timer_t *)timer_ptr,
                                      irq_mask);
        }
    }
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_TIMER_CFG_MODE_CHECK_UNIQUE                          VSF_HAL_CHECK_MODE_LOOSE
#define VSF_TIMER_CFG_IRQ_MASK_CHECK_UNIQUE                      VSF_HAL_CHECK_MODE_STRICT
#define VSF_TIMER_CFG_REIMPLEMENT_API_CAPABILITY                 ENABLED
#define VSF_TIMER_CFG_REIMPLEMENT_API_GET_CONFIGURATION          ENABLED
#define VSF_TIMER_CFG_REIMPLEMENT_API_IRQ_CLEAR                  ENABLED
#define VSF_TIMER_CFG_REIMPLEMENT_API_CTRL                       ENABLED
#define VSF_TIMER_CFG_REIMPLEMENT_API_STATUS                     ENABLED
#define VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_CONFIG             ENABLED
#define VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_START              ENABLED
#define VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_STOP               ENABLED
#define VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_REQUEST_START      ENABLED
#define VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_REQUEST_STOP       ENABLED
#define VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_CTRL               ENABLED
#define VSF_TIMER_CFG_REIMPLEMENT_API_SET_PERIOD                 ENABLED

#define VSF_TIMER_CFG_IMP_LV0(__IDX, __HAL_OP)                                  \
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t)                            \
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer, __IDX) = {                   \
        .reg = (void *)VSF_MCONNECT(VSF_TIMER_CFG_IMP_UPCASE_PREFIX, _TIMER,    \
                                    __IDX, _REG),                               \
        .irqn = VSF_MCONNECT(VSF_TIMER_CFG_IMP_UPCASE_PREFIX, _TIMER, __IDX,    \
                             _IRQN),                                            \
        __HAL_OP};                                                              \
    VSF_CAL_ROOT void VSF_MCONNECT(VSF_TIMER_CFG_IMP_UPCASE_PREFIX, _TIMER,     \
                                   __IDX, _IRQHandler)(void) {                  \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_TIMER_CFG_IMP_PREFIX, _timer_irqhandler)(          \
            &VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer, __IDX),             \
            (uint8_t)(((__IDX) % 2) * 2 + 0),                                   \
            (uint8_t)(((__IDX) % 2) * 2 + 1));                                  \
        vsf_hal_irq_leave(ctx);                                                 \
    }

#include "hal/driver/common/timer/timer_template.inc"

#endif /* VSF_HAL_USE_TIMER */
/* EOF */
