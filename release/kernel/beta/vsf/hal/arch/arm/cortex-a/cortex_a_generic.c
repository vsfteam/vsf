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
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
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
    return true;
}

/*----------------------------------------------------------------------------*
 * System Timer Implementation                                                *
 *----------------------------------------------------------------------------*/

#if VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_WITH_NORMAL_TIMER       

vsf_systimer_cnt_t vsf_systimer_get_tick_elapsed(void)
{
    return 0;
}

void vsf_systimer_clear_int_pending_bit(void)
{
    
}

void vsf_systimer_reset_counter_value(void)
{
    
}

/*! \brief disable systimer and return over-flow flag status
 *! \param none
 *! \retval true  overflow happened
 *! \retval false no overflow happened
 */
bool vsf_systimer_low_level_disable(void)
{
    return false;
}

/*! \brief only enable systimer without clearing any flags
 */
void vsf_systimer_low_level_enable(void)
{
    
}

void vsf_systimer_low_level_int_disable(void)
{
    
}

void vsf_systimer_low_level_int_enable(void)
{
    
}

void vsf_systimer_set_reload_value(vsf_systimer_cnt_t tick_cnt)
{
    
}

ROOT ISR(SysTick_Handler)
{   
    vsf_systimer_ovf_evt_hanlder();
}


/*! \brief initialise systimer without enable it 
 */
vsf_err_t vsf_systimer_low_level_init(uintmax_t ticks)
{
    return VSF_ERR_NOT_SUPPORT;
}

#endif

/*----------------------------------------------------------------------------*
 * SWI / PendSV                                                               *
 *----------------------------------------------------------------------------*/

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

/* EOF */
