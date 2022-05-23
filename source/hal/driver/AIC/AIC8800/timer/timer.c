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

#define VSF_TIMER_CFG_PREFIX                    vsf_hw
#define VSF_TIMER_CFG_UPPERCASE_PREFIX          VSF_HW

/*============================ INCLUDES ======================================*/

#include "./timer.h"

#if VSF_HAL_USE_TIMER == ENABLED

#include "hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/pmic/pmic_api.h"
#include "hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/sysctrl/sysctrl_api.h"
#include "hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/ticker/reg_timer.h"
#include "../__device.h"

/*============================ MACROS ========================================*/

#define TIMER_CLOCK_FREQ        (26000000U)

/*============================ TYPES =========================================*/

typedef struct AIC_TIM_REG_T {
    __IO uint32_t CTL;          /* 0x000: Control */
    __IO uint32_t TC;           /* 0x004: Timer Counter */
    __IO uint32_t PR;           /* 0x008: Prescale */
    __IO uint32_t PC;           /* 0x00C: Prescale Counter */
    __IO uint32_t MR;           /* 0x010: Match Register */
    __IO uint32_t TD;           /* 0x014: Tick Divider */
    __IO uint32_t IC;           /* 0x018: Interrupt Control */
    __I  uint32_t IS;           /* 0x01C: Interrupt Status */
} AIC_TIM_REG_T;

typedef struct vsf_hw_timer_const_t {
    AIC_TIM_REG_T* reg;
    IRQn_Type irqn;

    // TODO: move to pm
    uint32_t pclk;
    uint32_t oclk;
} vsf_hw_timer_const_t;

typedef struct vsf_hw_timer_t {
#if VSF_TIMER_CFG_IMPLEMENT_OP == ENABLED
    vsf_timer_t vsf_timer;
#endif

    const vsf_hw_timer_const_t *timer_const;
    em_timer_irq_mask_t irq_mask;
    vsf_timer_isr_t isr;
} vsf_hw_timer_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __vsf_hw_timer_irq_handler(vsf_hw_timer_t * timer_ptr)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
    const vsf_hw_timer_const_t *timer_const = timer_ptr->timer_const;
    VSF_HAL_ASSERT(timer_const != NULL);
    AIC_TIM_REG_T* reg = timer_const->reg;
    VSF_HAL_ASSERT(reg != NULL);

    uint32_t status = reg->IS;
    // wrap interrupt
    if (status & (0x01UL << 2)) {
        reg->IC = (0x01UL << 17); // clear interrupt
    }
    // match interrupt
    if (status & (0x01UL /*<< 0*/)) {
        reg->IC = (0x01UL << 16); // clear interrupt
        reg->TC = 0;

        if (timer_ptr->irq_mask & VSF_TIMER_IRQ_MASK_OVERFLOW) {
            if (NULL != timer_ptr->isr.handler_fn) {
                timer_ptr->isr.handler_fn(timer_ptr->isr.target_ptr, (vsf_timer_t *)timer_ptr, VSF_TIMER_IRQ_MASK_OVERFLOW);
            }
        }
    }
}

vsf_err_t vsf_hw_timer_init(vsf_hw_timer_t *timer_ptr, timer_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
    const vsf_hw_timer_const_t *timer_const = timer_ptr->timer_const;
    VSF_HAL_ASSERT(timer_const != NULL);
    AIC_TIM_REG_T* reg = timer_const->reg;
    VSF_HAL_ASSERT(reg != NULL);

    cpusysctrl_pclkme_set(timer_const->pclk);
    cpusysctrl_oclkme_set(timer_const->oclk);

    reg->CTL &= ~0x01UL;                // disable us ticker
    reg->TD = 0x01UL | (0x01UL << 4);   // div 2, div 1 not work
    reg->PR = TIMER_CLOCK_FREQ / 0x02UL / cfg_ptr->freq - 1; // default to 1MHz (1 us ticks)
    reg->MR = cfg_ptr->max_count;

    vsf_timer_isr_t *isr_ptr = &cfg_ptr->isr;
    timer_ptr->isr = *isr_ptr;
    if (isr_ptr->handler_fn != NULL) {
        NVIC_SetPriority(timer_const->irqn, (uint32_t)isr_ptr->prio);
        NVIC_EnableIRQ(timer_const->irqn);
    } else {
        NVIC_DisableIRQ(timer_const->irqn);
    }

    return VSF_ERR_NONE;
}

fsm_rt_t vsf_hw_timer_enable(vsf_hw_timer_t *timer_ptr)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
    VSF_HAL_ASSERT(timer_ptr->timer_const != NULL);
    VSF_HAL_ASSERT(timer_ptr->timer_const->reg != NULL);

    timer_ptr->timer_const->reg->CTL |= 0x01UL;    // enable us ticker

    return fsm_rt_cpl;
}

fsm_rt_t vsf_hw_timer_disable(vsf_hw_timer_t *timer_ptr)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
    VSF_HAL_ASSERT(timer_ptr->timer_const != NULL);
    VSF_HAL_ASSERT(timer_ptr->timer_const->reg != NULL);

    timer_ptr->timer_const->reg->CTL &= ~0x01UL;    // disable us ticker

    return fsm_rt_cpl;
}

void vsf_hw_timer_irq_enable(vsf_hw_timer_t *timer_ptr, em_timer_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != timer_ptr);

    if (irq_mask == VSF_TIMER_IRQ_MASK_OVERFLOW) {
        // enable match interrupt
        timer_ptr->timer_const->reg->IC = 0x01UL;
        timer_ptr->irq_mask = VSF_TIMER_IRQ_MASK_OVERFLOW;
    } else {
        VSF_HAL_ASSERT(0);
    }

}

void vsf_hw_timer_irq_disable(vsf_hw_timer_t *timer_ptr, em_timer_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != timer_ptr);
    if (irq_mask == VSF_TIMER_IRQ_MASK_OVERFLOW) {
        // disable match interrupt
        timer_ptr->timer_const->reg->IC = (0x01UL << 8);
        timer_ptr->irq_mask = 0;
    } else {
        VSF_HAL_ASSERT(0);
    }
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_TIMER_CFG_IMP_LV0(__COUNT, __hal_op)                                \
    static const vsf_hw_timer_const_t __vsf_hw_timer ## __COUNT ## _const = {   \
        .reg    = (AIC_TIM_REG_T *)VSF_HW_TIMER ## __COUNT ## _BASE_ADDRESS,    \
        .irqn   = VSF_HW_TIMER ## __COUNT ## _IRQN,                             \
        .pclk   = VSF_HW_TIMER ## __COUNT ## _PCLK_EN_BIT,                      \
        .oclk   = VSF_HW_TIMER ## __COUNT ## _OCLK_EN_BIT,                      \
    };                                                                          \
    vsf_hw_timer_t vsf_hw_timer ## __COUNT = {                                  \
        .timer_const  = &__vsf_hw_timer ## __COUNT ## _const,                   \
        __hal_op                                                                \
    };                                                                          \
    void VSF_HW_TIMER ## __COUNT ## _IRQHandler(void)                           \
    {                                                                           \
        __vsf_hw_timer_irq_handler(&vsf_hw_timer ## __COUNT);                   \
    }
#include "hal/driver/common/timer/timer_template.inc"

#endif      // VSF_HAL_USE_TIMER
