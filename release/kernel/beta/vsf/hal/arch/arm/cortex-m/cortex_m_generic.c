/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

struct __vsf_cm_t {
    struct {
        vsf_swi_handler_t *handler;
        void *param;
#if __ARM_ARCH == 6 || __TARGET_ARCH_THUMB == 3
        bool enabled;
        bool sw_pending_bit;
#endif
        vsf_gint_state_t global_int_state;
    } pendsv;
};
typedef struct __vsf_cm_t __vsf_cm_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static __vsf_cm_t __vsf_cm;

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
    return true;
}

/*----------------------------------------------------------------------------*
 * System Timer Implementation                                                *
 *----------------------------------------------------------------------------*/

#if VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_WITH_NORMAL_TIMER       

vsf_systimer_cnt_t vsf_systimer_get_tick_elapsed(void)
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

void vsf_systimer_set_reload_value(vsf_systimer_cnt_t tick_cnt)
{
    vsf_systick_set_reload((uint32_t)tick_cnt);
}

ROOT ISR(SysTick_Handler)
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

        NVIC_ClearPendingIRQ(SysTick_IRQn);
    vsf_set_interrupt(orig);
    
    return VSF_ERR_NONE;
}

void vsf_systimer_prio_set(vsf_arch_prio_t priority)
{
    NVIC_SetPriority(SysTick_IRQn, priority);
}

#endif

/*----------------------------------------------------------------------------*
 * SWI / PendSV                                                               *
 *----------------------------------------------------------------------------*/
ROOT ISR(PendSV_Handler)
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
    VSF_HAL_ASSERT(false);
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
    #if __ARM_ARCH >= 7 || __TARGET_ARCH_THUMB == 4
        SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
    #elif __ARM_ARCH == 6 || __TARGET_ARCH_THUMB == 3
        __SAFE_ATOM_CODE(
            if (__vsf_cm.pendsv.enabled) {
                SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
            } else {
                __vsf_cm.pendsv.sw_pending_bit = 1;
            }
        )
    #endif
        return;
    }
    VSF_HAL_ASSERT(false);
}

vsf_arch_prio_t vsf_set_base_priority(vsf_arch_prio_t priority)
{
#if __ARM_ARCH >= 7 || __TARGET_ARCH_THUMB == 4
    static vsf_gint_state_t __basepri = 0x100;
    vsf_gint_state_t origlevel = __basepri;
        
        if (0 == priority) {
            __set_BASEPRI(0);
            __vsf_cm.pendsv.global_int_state = DISABLE_GLOBAL_INTERRUPT();
            __basepri = 0;
        } else if (priority >= 0x100) {
            __set_BASEPRI(0);
            __basepri = 0x100;
            SET_GLOBAL_INTERRUPT_STATE(__vsf_cm.pendsv.global_int_state);
            
        } else {
            __set_BASEPRI(priority << (8 - VSF_ARCH_PRI_BIT));
            __basepri = __get_BASEPRI();
        }
    
    return (vsf_arch_prio_t)origlevel;
    
#elif __ARM_ARCH == 6 || __TARGET_ARCH_THUMB == 3
    // TODO: MUST pass multi-priority test case
    static vsf_gint_state_t __basepri = 0x100;
    
    vsf_gint_state_t origlevel = __basepri;
    __SAFE_ATOM_CODE(
        if (priority <= VSF_ARCH_PRIO_0) {
            //! lock sched
            __vsf_cm.pendsv.enabled = false;
            __vsf_cm.pendsv.sw_pending_bit = 0;
        } else /*if (0x100 == priority)*/ {
            //! unload sched
            __vsf_cm.pendsv.enabled = true;
            if (__vsf_cm.pendsv.sw_pending_bit) {
                __vsf_cm.pendsv.sw_pending_bit = 0;
                SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
            }
        }
        __basepri = priority;
    )
    return (vsf_arch_prio_t)origlevel;
#endif
}

/*----------------------------------------------------------------------------*
 * interrupt                                                                  *
 *----------------------------------------------------------------------------*/

vsf_gint_state_t vsf_get_interrupt(void)
{
    return GET_GLOBAL_INTERRUPT_STATE();
}

void vsf_set_interrupt(vsf_gint_state_t level)
{
    SET_GLOBAL_INTERRUPT_STATE(level);
}

vsf_gint_state_t vsf_disable_interrupt(void)
{
    return DISABLE_GLOBAL_INTERRUPT();
}

void vsf_enable_interrupt(void)
{
    ENABLE_GLOBAL_INTERRUPT();
}


/*----------------------------------------------------------------------------*
 * Others: sleep, reset, etc                                                  *
 *----------------------------------------------------------------------------*/

void vsf_arch_sleep(uint32_t mode)
{
    UNUSED_PARAM(mode);
    ENABLE_GLOBAL_INTERRUPT();
    __WFI();
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
