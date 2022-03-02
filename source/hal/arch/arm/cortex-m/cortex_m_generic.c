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

#include "hal/arch/vsf_arch_abstraction.h"
#include "SysTick/systick.h"
#include "hal/driver/driver.h"
#include "hal/arch/__vsf_arch_interface.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct __vsf_cm_t {
    struct {
        vsf_swi_handler_t *handler;
        void *param;
#if __ARM_ARCH == 6 || __TARGET_ARCH_THUMB == 3
        bool enabled;
        bool sw_pending_bit;
#endif
    } pendsv;
    vsf_arch_prio_t  basepri;
} __vsf_cm_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static __vsf_cm_t __vsf_cm = {
    . basepri = 0x100,
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*----------------------------------------------------------------------------*
 * Infrastructure                                                             *
 *----------------------------------------------------------------------------*/
/*! \note initialize architecture specific service
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
bool vsf_arch_low_level_init(void)
{
    //memset(&__vsf_cm, 0, sizeof(__vsf_cm));
    //vsf_systimer_init();

#if __ARM_ARCH >= 7 && VSF_ARCH_CFG_ALIGNMENT_TEST == ENABLED
#   warning All unaligned access will be treated as faults for alignment test
    /*! disable processor's support for unaligned access for debug purpose */
    SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
#endif
    return true;
}

/*----------------------------------------------------------------------------*
 * System Timer Implementation                                                *
 *----------------------------------------------------------------------------*/

//#if VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_WITH_NORMAL_TIMER

vsf_systimer_tick_t vsf_systimer_get_tick_elapsed(void)
{
    return SYSTICK_RVR - vsf_systick_get_count();
}

void vsf_systimer_clear_int_pending_bit(void)
{
    SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;            //!< clear pending bit
}

void vsf_systimer_reset_counter_value(void)
{
    vsf_systick_clear_count();
}

/*! \brief disable systimer and return over-flow flag status
 *! \param none
 *! \retval true  overflow happened
 *! \retval false no overflow happened
 */
bool vsf_systimer_low_level_disable(void)
{
    return vsf_systick_disable();
}

/*! \brief only enable systimer without clearing any flags
 */
void vsf_systimer_low_level_enable(void)
{
    vsf_systick_enable();
}

void vsf_systimer_low_level_int_disable(void)
{
    vsf_systick_int_disable();
}

void vsf_systimer_low_level_int_enable(void)
{
    vsf_systick_int_enable();
}

void vsf_systimer_set_reload_value(vsf_systimer_tick_t tick_cnt)
{
    vsf_systick_set_reload((uint32_t)tick_cnt);
}

#if VSF_USE_KERNEL == ENABLED && VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
ROOT
#endif
ISR(SysTick_Handler)
{
    vsf_systimer_ovf_evt_hanlder();
}


/*! \brief initialise systimer without enable it
 */
vsf_err_t vsf_systimer_low_level_init(uintmax_t ticks)
{
    vsf_gint_state_t orig = vsf_disable_interrupt();
        vsf_systick_cfg (
            DISABLE_SYSTICK             |
            SYSTICK_SOURCE_SYSCLK       |
            ENABLE_SYSTICK_INTERRUPT,
            (uint32_t)ticks
        );

        // NVIC_ClearPendingIRQ has no effect on IRQn < 0
//        NVIC_ClearPendingIRQ(SysTick_IRQn);
        SCB->ICSR = SCB_ICSR_PENDSTCLR_Msk;
    vsf_set_interrupt(orig);

    return VSF_ERR_NONE;
}

void vsf_systimer_prio_set(vsf_arch_prio_t priority)
{
    NVIC_SetPriority(SysTick_IRQn, priority);
}

//#endif

/*----------------------------------------------------------------------------*
 * SWI / PendSV                                                               *
 *----------------------------------------------------------------------------*/
#if VSF_USE_KERNEL == ENABLED
ROOT
#endif
ISR(PendSV_Handler)
{
    if (__vsf_cm.pendsv.handler != NULL) {
        __vsf_cm.pendsv.handler(__vsf_cm.pendsv.param);
    }
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#pragma diag_suppress=pe111
#endif

/*! \brief initialise a software interrupt
 *! \param idx the index of the software interrupt
 *! \return initialization result in vsf_err_t
 */
vsf_err_t vsf_arch_swi_init(uint_fast8_t idx,
                            vsf_arch_prio_t priority,
                            vsf_swi_handler_t *handler,
                            void *param)
{
    if (0 == idx) {
        __vsf_cm.pendsv.handler = handler;
        __vsf_cm.pendsv.param = param;
#if __ARM_ARCH == 6 || __TARGET_ARCH_THUMB == 3
        __vsf_cm.pendsv.enabled = true;
        __vsf_cm.pendsv.sw_pending_bit = 0;
#endif
        NVIC_SetPriority(PendSV_IRQn, priority);
        return VSF_ERR_NONE;
    }
    VSF_ARCH_ASSERT(false);
    return VSF_ERR_INVALID_PARAMETER;
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#pragma diag_warning=pe111
#endif


/*! \brief trigger a software interrupt
 *! \param idx the index of the software interrupt
 */
void vsf_arch_swi_trigger(uint_fast8_t idx)
{
    if (0 == idx) {
    //! todo: the code is simplified, we need to verify its reilability
    #if __ARM_ARCH >= 7 || __TARGET_ARCH_THUMB == 4
        SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
    #elif __ARM_ARCH == 6 || __TARGET_ARCH_THUMB == 3
        vsf_interrupt_safe_simple(
            if (__vsf_cm.pendsv.enabled) {
                SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
            } else {
                __vsf_cm.pendsv.sw_pending_bit = 1;
            }
        )
    #endif
        return;
    }
    VSF_ARCH_ASSERT(false);
}

vsf_arch_prio_t vsf_get_base_priority(void)
{
    return __vsf_cm.basepri;
}

vsf_arch_prio_t vsf_set_base_priority(vsf_arch_prio_t priority)
{
#if __ARM_ARCH >= 7 || __TARGET_ARCH_THUMB == 4
    //static vsf_gint_state_t __basepri = 0x100;
    vsf_arch_prio_t origlevel = __vsf_cm.basepri;

    VSF_ARCH_ASSERT(priority >= 0);

    /*! \note When BASEPRI = 0, the CPU execution priority level is not raised
     *!       to 0 as one may think (this would have the effect of masking all
     *!       interrupts).
     *!       Instead, with BASEPRI = 0 all masking is disabled.  With masking
     *!        disabled, any interrupt even with the max priority level can
     *!        interrupt the core. On reset, BASEPRI = 0.
     *!
     *!        The only way to raise the CPU execution priority level to 0 is
     *!        through PRIMASK.
     */
    DISABLE_GLOBAL_INTERRUPT();                                             //! do this first for atomicity

    if (0 == priority) {
        //DISABLE_GLOBAL_INTERRUPT();
        __vsf_cm.basepri = 0;
        //__set_BASEPRI(0);
    } else if (priority >= 0x100) {
        __vsf_cm.basepri = 0x100;
        __set_BASEPRI(0);
        ENABLE_GLOBAL_INTERRUPT();
    } else {
        __set_BASEPRI(priority << (8 - VSF_ARCH_PRI_BIT));
        __vsf_cm.basepri = __get_BASEPRI();
        ENABLE_GLOBAL_INTERRUPT();
    }

    return (vsf_arch_prio_t)origlevel;

#elif __ARM_ARCH == 6 || __TARGET_ARCH_THUMB == 3

    vsf_arch_prio_t origlevel = __vsf_cm.basepri;
    VSF_ARCH_ASSERT(priority >= 0);

    DISABLE_GLOBAL_INTERRUPT();


    if (priority) {
        if (priority >= 0x100) {
            __vsf_cm.basepri = 0x100;
        }

        __vsf_cm.basepri = priority;

        if (priority > NVIC_GetPriority(PendSV_IRQn)) {
            __vsf_cm.pendsv.enabled = true;
            if (__vsf_cm.pendsv.sw_pending_bit) {
                __vsf_cm.pendsv.sw_pending_bit = 0;
                SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
            }
        } else {
            __vsf_cm.pendsv.enabled = false;
            __vsf_cm.pendsv.sw_pending_bit = 0;
        }

        ENABLE_GLOBAL_INTERRUPT();
    } else /* if (0 == priority) */{
        __vsf_cm.basepri = 0;
    }

    return (vsf_arch_prio_t)origlevel;

#else
    VSF_ARCH_ASSERT(false);
#endif
}

/*----------------------------------------------------------------------------*
 * interrupt                                                                  *
 *----------------------------------------------------------------------------*/

WEAK(vsf_get_interrupt)
vsf_gint_state_t vsf_get_interrupt(void)
{
    return (vsf_gint_state_t)vsf_get_base_priority();
}

WEAK(vsf_set_interrupt)
vsf_gint_state_t vsf_set_interrupt(vsf_gint_state_t prio)
{
    return (vsf_gint_state_t)vsf_set_base_priority(prio);
}

WEAK(vsf_disable_interrupt)
vsf_gint_state_t vsf_disable_interrupt(void)
{
    return (vsf_gint_state_t)vsf_set_base_priority(vsf_arch_prio_disable_all);
}

WEAK(vsf_enable_interrupt)
vsf_gint_state_t vsf_enable_interrupt(void)
{
    return (vsf_gint_state_t)vsf_set_base_priority(vsf_arch_prio_enable_all);
}

WEAK(vsf_get_interrupt_id)
int vsf_get_interrupt_id(void)
{
    return ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) >> SCB_ICSR_VECTACTIVE_Pos);
}

/*----------------------------------------------------------------------------*
 * Others: sleep, reset, etc                                                  *
 *----------------------------------------------------------------------------*/

WEAK(vsf_arch_sleep)
void vsf_arch_sleep(uint_fast32_t mode)
{
    VSF_UNUSED_PARAM(mode);
    __WFE();
}

/*----------------------------------------------------------------------------*
 * arch enhancement                                                           *
 *----------------------------------------------------------------------------*/

uint_fast16_t bswap_16(uint_fast16_t value16)
{
    return __REV16(value16);
}

uint_fast32_t bswap_32(uint_fast32_t value32)
{
    return __REV(value32);
}

uint_fast8_t __vsf_arch_clz(uintalu_t a)
{
    return __CLZ(a);
}

int_fast8_t __vsf_arch_msb(uintalu_t a)
{
    return 31 - (int_fast8_t)__CLZ(a);
}

/* EOF */
