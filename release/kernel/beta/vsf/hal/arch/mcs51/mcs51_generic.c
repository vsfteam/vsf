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
#include "hal/driver/driver.h"
#include "hal/arch/__vsf_arch_interface.h"
/*============================ MACROS ========================================*/

#if !__IS_COMPILER_IAR__ && !__IS_COMPILER_51_KEIL__
#   error "compiler not supported"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct __vsf_mcs51_t {
    uint8_t ie;
    vsf_arch_prio_t base_prio;
    vsf_systimer_cnt_t systimer_reload;
};
typedef struct __vsf_mcs51_t __vsf_mcs51_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

NO_INIT static __vsf_mcs51_t __vsf_mcs51;

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
    //memset(&__vsf_mcs51, 0, sizeof(__vsf_mcs51));
    //vsf_systimer_init();
    return true;
}


/*----------------------------------------------------------------------------*
 * System Timer                                                               *
 *----------------------------------------------------------------------------*/

#if VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_WITH_NORMAL_TIMER
vsf_systimer_cnt_t vsf_systimer_get_tick_elapsed(void)
{
    return (((uint32_t)TH0 << 8) + TL0) - __vsf_mcs51.systimer_reload;
}

void vsf_systimer_clear_int_pending_bit(void)
{
#if __IS_COMPILER_IAR__
    TCON_bit.TF0 = 0;
#elif __IS_COMPILER_51_KEIL__
    TF0 = 0;
#endif
}

void vsf_systimer_reset_counter_value(void)
{
    TH0 = (__vsf_mcs51.systimer_reload >> 8) & 0xFF;
    TL0 = (__vsf_mcs51.systimer_reload >> 0) & 0xFF;
}

/*! \brief disable systimer and return over-flow flag status
 *! \param none
 *! \retval true  overflow happened
 *! \retval false no overflow happened
 */
bool vsf_systimer_low_level_disable(void)
{
#if __IS_COMPILER_IAR__
    TCON_bit.TR0 = 0;
    return TCON_bit.TF0;
#elif __IS_COMPILER_51_KEIL__
    TR0 = 0;
    return TF0;
#endif
}

/*! \brief only enable systimer without clearing any flags
 */
void vsf_systimer_low_level_enable(void)
{
#if __IS_COMPILER_IAR__
    TCON_bit.TR0 = 1;
#elif __IS_COMPILER_51_KEIL__
    TR0 = 1;
#endif
}

void vsf_systimer_low_level_int_disable(void)
{
#if __IS_COMPILER_IAR__
    IE_bit.ET0 = 0;
#elif __IS_COMPILER_51_KEIL__
    ET0 = 0;
#endif
}

void vsf_systimer_low_level_int_enable(void)
{
#if __IS_COMPILER_IAR__
    IE_bit.ET0 = 1;
#elif __IS_COMPILER_51_KEIL__
    ET0 = 1;
#endif
}

void vsf_systimer_set_reload_value(vsf_systimer_cnt_t tick_cnt)
{
    __vsf_mcs51.systimer_reload = 0x10000 - tick_cnt;
}

#if __IS_COMPILER_IAR__
ISR(timer0)
#elif __IS_COMPILER_51_KEIL__
ISR(1)
#endif
{
    vsf_systimer_reset_counter_value();
    vsf_systimer_ovf_evt_hanlder();
}

/*! \brief initialise systimer without enable it 
 */
vsf_err_t vsf_systimer_low_level_init(uintmax_t ticks)
{
    TMOD = 1;
    vsf_systimer_set_reload_value(ticks);
    vsf_systimer_reset_counter_value();

    return VSF_ERR_NONE;
}


#warning "todo: implement vsf_systimer_prio_set()"
void vsf_systimer_prio_set(vsf_arch_prio_t priority)
{

}

#endif

/*----------------------------------------------------------------------------*
 * SWI / PendSV                                                               *
 *----------------------------------------------------------------------------*/
void __vsf_mcs51_enabled_irq(uint_fast8_t irq, vsf_arch_prio_t priority)
{
    vsf_gint_state_t state = vsf_disable_interrupt();
        __vsf_mcs51.ie |= 1 << irq;
        if (__vsf_mcs51.base_prio < priority) {
            IE |= 1 << irq;
        }
        switch (priority) {
        case vsf_arch_prio_0:
            IP &= ~(1 << irq);
            break;
        case vsf_arch_prio_1:
            IP |= 1 << irq;
            break;
        }
    vsf_set_interrupt(state);
}

void __vsf_mcs51_disabled_irq(uint_fast8_t irq)
{
    vsf_gint_state_t state = vsf_disable_interrupt();
        __vsf_mcs51.ie |= 1 << irq;
        IE &= ~(1 << irq);
    vsf_set_interrupt(state);
}

vsf_arch_prio_t vsf_set_base_priority(vsf_arch_prio_t priority)
{
    uint_fast8_t ie_mask;
    vsf_arch_prio_t orig = __vsf_mcs51.base_prio;
    vsf_gint_state_t state = vsf_disable_interrupt();
        __vsf_mcs51.base_prio = priority;
        switch (priority) {
        case vsf_arch_prio_0:
            ie_mask = ~IP & __vsf_mcs51.ie;
            break;
        case vsf_arch_prio_1:
            ie_mask = 0xFF;
            break;
        }
        IE &= ~ie_mask;
    vsf_set_interrupt(state);
    return orig;
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

/*----------------------------------------------------------------------------*
 * Others: sleep, reset, etc.                                                 *
 *----------------------------------------------------------------------------*/

void vsf_arch_sleep(uint32_t mode)
{
    ENABLE_GLOBAL_INTERRUPT();
    PCON = mode;
}

/* EOF */
