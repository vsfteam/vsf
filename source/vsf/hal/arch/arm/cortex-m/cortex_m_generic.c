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
#include "systick/systick.h"
#include "hal/driver/driver.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct __vsf_cm_t {
    struct {
        vsf_swi_handler_t *handler;
        void *pparam;
#if __ARM_ARCH == 6
        bool enabled;
        bool sw_pending_bit;
#endif
        vsf_gint_state_t global_int_state;
    } pendsv;
    struct {
        vsf_systimer_cnt_t tick;
        vsf_systimer_cnt_t unit;
        vsf_systimer_cnt_t max_tick_per_round;
        uint32_t           tick_freq;
    } systimer;
};
typedef struct __vsf_cm_t __vsf_cm_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

NO_INIT static __vsf_cm_t __vsf_cm;

/*============================ PROTOTYPES ====================================*/
extern void vsf_systimer_evthandler(vsf_systimer_cnt_t tick);

/*============================ IMPLEMENTATION ================================*/

WEAK bool on_arch_systimer_tick_evt(vsf_systimer_cnt_t tick)
{
    UNUSED_PARAM(tick);
    return true;
}

static vsf_systimer_cnt_t __vsf_systimer_update(void)
{
    vsf_systimer_cnt_t tick;
   
    tick = vsf_systimer_get();
    __vsf_cm.systimer.tick = tick;
    return tick;
}

static bool __vsf_systimer_set_target(vsf_systimer_cnt_t tick_cnt)
{
    if (0 == tick_cnt) {
        return false;
    }
    tick_cnt *= __vsf_cm.systimer.unit;
    
    vsf_systick_disable();
    vsf_systick_set_reload(tick_cnt);
    SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;    //!< clear pending bit
    vsf_systick_clear_count();              //!< force a reload
    vsf_systick_int_enable();
    vsf_systick_enable();
    return true;
}

WEAK void vsf_systimer_set_idle(void)
{   
//    vsf_trace(VSF_TRACE_DEBUG, "systimer_idle\r\n");
    __SAFE_ATOM_CODE(
        __vsf_systimer_update();
        __vsf_systimer_set_target(__vsf_cm.systimer.max_tick_per_round);
    )
}

ROOT void SysTick_Handler(void)
{   
    vsf_systick_int_disable();
    __vsf_systimer_update();
    vsf_systimer_cnt_t tick = __vsf_cm.systimer.tick;
    vsf_systick_disable();
    if (on_arch_systimer_tick_evt(tick)) {
        vsf_systimer_evthandler(tick);
    }
}

ROOT void PendSV_Handler(void)
{
    if (__vsf_cm.pendsv.handler != NULL) {
        __vsf_cm.pendsv.handler(__vsf_cm.pendsv.pparam);
    }
}

WEAK uint_fast32_t vsf_arch_req___systimer_freq___from_usr(void)
{
    return VSF_GET_MAIN_CLK();
}

WEAK uint_fast32_t vsf_arch_req___systimer_resolution___from_usr(void)
{
    return 1000000ul;
}

/*! \brief initialise SysTick to generate a system timer
 *! \param frequency the target tick frequency in Hz
 *! \return initialization result in vsf_err_t 
 */
WEAK vsf_err_t vsf_systimer_init(void)
{
    //! calculate the cycle count of 1 tick
    uint_fast32_t tick_res = vsf_arch_req___systimer_resolution___from_usr();
    __vsf_cm.systimer.unit = vsf_arch_req___systimer_freq___from_usr() / tick_res;
    __vsf_cm.systimer.tick_freq = tick_res;
    __vsf_cm.systimer.max_tick_per_round = (0x01000000ul / __vsf_cm.systimer.unit) - 1;

    vsf_systick_cfg (
        DISABLE_SYSTICK             |
        SYSTICK_SOURCE_SYSCLK       |
        ENABLE_SYSTICK_INTERRUPT,
        __vsf_cm.systimer.max_tick_per_round * __vsf_cm.systimer.unit
    );

    return VSF_ERR_NONE;
}

WEAK vsf_err_t vsf_systimer_start(void)
{
    __SAFE_ATOM_CODE(
        __vsf_systimer_set_target(__vsf_cm.systimer.max_tick_per_round);
    )
    return VSF_ERR_NONE;
}


WEAK vsf_systimer_cnt_t vsf_systimer_get(void)
{
    vsf_systimer_cnt_t ticks = 0;
    bool auto_update = false;
    __SAFE_ATOM_CODE(
        if (vsf_systick_disable()) {       //!< the match bit will be cleared
            ticks += SYSTICK_RVR;
            auto_update = true;
        }
        ticks += (SYSTICK_RVR - vsf_systick_get_count());
        vsf_systick_enable();
        ticks /= __vsf_cm.systimer.unit;
        ticks += __vsf_cm.systimer.tick;
        if (auto_update) {
            __vsf_cm.systimer.tick = ticks;
        }
    )
    return ticks;
}

WEAK bool vsf_systimer_set(vsf_systimer_cnt_t due)
{
    bool result = false;
    //vsf_systimer_cnt_t unit = __vsf_cm.systimer.unit;
    vsf_systimer_cnt_t max_tick_per_round = __vsf_cm.systimer.max_tick_per_round;

    SAFE_ATOM_CODE(){
        vsf_systimer_cnt_t current = __vsf_systimer_update();
        //vsf_systick_disable();
        vsf_systimer_cnt_t tick_cnt;
//        vsf_trace(VSF_TRACE_DEBUG, "systimer_set: %lld %lld %c\r\n",
//                    current, due, due > current ? '*' : ' ');
        /*
        if (due < current) {
            tick_cnt = 0xFFFFFFFF - current + due + 1;
        } else {
            tick_cnt = due - current;
        }
        */
        if (due > current) {
            tick_cnt = due - current;
            tick_cnt = min(max_tick_per_round, tick_cnt);
            result = __vsf_systimer_set_target(tick_cnt);
        }
    }
    
    return result;
}

WEAK bool vsf_systimer_is_due(vsf_systimer_cnt_t due)
{
    return (__vsf_cm.systimer.tick >= due);
}



WEAK vsf_systimer_cnt_t vsf_systimer_us_to_tick(uint_fast32_t time_us)
{
    return ((uint64_t)  ((uint64_t)time_us 
                            * (uint64_t)__vsf_cm.systimer.tick_freq) 
                        / 1000000ul);
}

WEAK vsf_systimer_cnt_t vsf_systimer_ms_to_tick(uint_fast32_t time_ms)
{
    return ((uint64_t)  ((uint64_t)time_ms 
                            * (uint64_t)__vsf_cm.systimer.tick_freq) 
                        / 1000ul);
}

WEAK uint_fast32_t vsf_systimer_tick_to_us(vsf_systimer_cnt_t tick)
{
    return tick * 1000000ul / __vsf_cm.systimer.tick_freq;
}

WEAK uint_fast32_t vsf_systimer_tick_to_ms(vsf_systimer_cnt_t tick)
{
    return vsf_systimer_tick_to_us(tick) / 1000;
}

/*! \brief initialise a software interrupt
 *! \param idx the index of the software interrupt
 *! \return initialization result in vsf_err_t
 */
vsf_err_t vsf_arch_swi_init(uint_fast8_t idx, 
                            vsf_arch_prio_t priority,
                            vsf_swi_handler_t *handler, 
                            void *pparam)
{
    if (0 == idx) {
        __vsf_cm.pendsv.handler = handler;
        __vsf_cm.pendsv.pparam = pparam;
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
    
    return origlevel;
    
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


/*! \note initialize architecture specific service 
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
bool vsf_arch_init(void)
{
    memset(&__vsf_cm, 0, sizeof(__vsf_cm));
    vsf_systimer_init();
    return true;
}


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

void vsf_arch_sleep(uint32_t mode)
{
    ENABLE_GLOBAL_INTERRUPT();
    __WFI();
}



uint_fast16_t bswap_16(uint_fast16_t value16)
{
    return __REV16(value16);
}

uint_fast32_t bswap_32(uint_fast32_t value32)
{
    return __REV(value32);
}

/* EOF */
