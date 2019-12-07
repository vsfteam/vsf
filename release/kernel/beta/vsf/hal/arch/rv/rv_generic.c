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
#define SYSTICK_IRQ         7
#define PENDSV_IRQ          3

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
struct __vsf_rv_t {
    struct {
        vsf_swi_handler_t *handler;
        void *pparam;
        vsf_gint_state_t global_int_state;
    } pendsv;

#if     VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_WITH_NORMAL_TIMER
    vsf_systimer_cnt_t reload;
#endif
};
typedef struct __vsf_rv_t __vsf_rv_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static __vsf_rv_t __vsf_rv;

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
    //! support 16 SWI at max
    eclic_set_nlbits(4);
    //!< set priority to 1 (only high 4 bits are configurable)
    eclic_set_irq_lvl(SYSTICK_IRQ, 0x10);
    //!< set priority to 0 (only high 4 bits are configurable)
    //eclic_set_irq_lvl(PENDSV_IRQ, 0x00);

    vsf_enable_interrupt();
    return true;
}


/*----------------------------------------------------------------------------*
 * System Timer                                                               *
 *----------------------------------------------------------------------------*/

#if VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_WITH_NORMAL_TIMER
vsf_systimer_cnt_t vsf_systimer_get_tick_elapsed(void)
{
    return SYSTICK.COUNTER;
}

void vsf_systimer_clear_int_pending_bit(void)
{
    eclic_clear_pending(SYSTICK_IRQ);
}

void vsf_systimer_reset_counter_value(void)
{
    SYSTICK.COUNTER = 0;
}

/*! \brief disable systimer and return over-flow flag status
 *! \param none
 *! \retval true  overflow happened
 *! \retval false no overflow happened
 */
bool vsf_systimer_low_level_disable(void)
{
    SYSTICK.CTRL = SYSTICK_CTRL_DISABLE_MSK;
}

/*! \brief only enable systimer without clearing any flags
 */
void vsf_systimer_low_level_enable(void)
{
    SYSTICK.CTRL = 0;
}

void vsf_systimer_low_level_int_disable(void)
{
    eclic_disable_interrupt(SYSTICK_IRQ);
}

void vsf_systimer_low_level_int_enable(void)
{
    eclic_enable_interrupt(SYSTICK_IRQ);
}

void vsf_systimer_set_reload_value(vsf_systimer_cnt_t tick_cnt)
{
    __vsf_rv.reload = tick_cnt;
    SYSTICK.COMPARE = tick_cnt;
}

ISR(SysTick_Handler)
{
    vsf_systimer_reset_counter_value();
    vsf_systimer_ovf_evt_hanlder();
}

/*! \brief initialise systimer without enable it 
 */
vsf_err_t vsf_systimer_low_level_init(uintmax_t ticks)
{
#if     VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_WITH_NORMAL_TIMER
//! normal timer
    __vsf_rv.reload= ticks;
#elif   VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_WITH_COMP_TIMER
#endif

    eclic_set_posedge_trig(SYSTICK_IRQ);
}

#endif

/*----------------------------------------------------------------------------*
 * SWI / PendSV                                                               *
 *----------------------------------------------------------------------------*/

ROOT ISR(PendSV_Handler)
{
    SYSTICK.PENDSV = 0;
    if (__vsf_rv.pendsv.handler != NULL) {
        __vsf_rv.pendsv.handler(__vsf_rv.pendsv.pparam);
    }
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
        __vsf_rv.pendsv.handler = handler;
        __vsf_rv.pendsv.pparam = pparam;

        eclic_set_irq_lvl(PENDSV_IRQ, priority << (8 - VSF_ARCH_PRI_BIT));
        eclic_enable_interrupt(PENDSV_IRQ);
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
        SYSTICK.PENDSV = SYSTICK_PENDSV_PENDING_MSK;
        return;
    }
    VSF_HAL_ASSERT(false);
}



vsf_arch_prio_t vsf_set_base_priority(vsf_arch_prio_t priority)
{
    vsf_arch_prio_t original = eclic_get_mth();
    eclic_set_mth(priority << (8 - VSF_ARCH_PRI_BIT) | ((1<<(8 - VSF_ARCH_PRI_BIT)) - 1));
    return original;
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
}

/* EOF */
