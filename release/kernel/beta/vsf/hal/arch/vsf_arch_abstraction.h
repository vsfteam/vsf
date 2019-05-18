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

#ifndef __VSF_ARCH_ABSTRACTION_H__
#define __VSF_ARCH_ABSTRACTION_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"

#if !defined(VSF_ARCH_HEADER)
# if      (defined(__ARM_ARCH_PROFILE) && __ARM_ARCH_PROFILE == 'M') || __TARGET_PROFILE_M
#   define VSF_ARCH_HEADER      "./arm/cortex-m/cortex_m_generic.h"
# elif    defined(__ARM_ARCH_PROFILE) && __ARM_ARCH_PROFILE == 'A' || __TARGET_PROFILE_A
#   define VSF_ARCH_HEADER      "./arm/cortex-m/cortex_m_generic.h"
# elif    defined(__ARM_ARCH_PROFILE) && __ARM_ARCH_PROFILE == 'R' || __TARGET_PROFILE_R
#   define VSF_ARCH_HEADER      "./arm/cortex-m/cortex_r_generic.h"
# elif   defined(__CPU_PC__) && __IS_COMPILER_MSVC__
#   define  VSF_ARCH_HEADER     "./pc/win32/win32_generic.h"
# else
#   error no supported architecture found!
# endif
#endif

#include VSF_ARCH_HEADER

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef void vsf_swi_hanler_t(void *p);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


/*! \brief initialise SysTick to generate a system timer
 *! \param frequency the target frequency in Hz
 *! \return initialization result in vsf_err_t 
 */
extern vsf_err_t vsf_systimer_init(uint32_t tick_freq);
extern vsf_systimer_cnt_t vsf_systimer_get(void);
extern bool vsf_systimer_set(vsf_systimer_cnt_t due);
extern void vsf_systimer_set_idle(void);
extern vsf_systimer_cnt_t vsf_systimer_us_to_tick(uint_fast32_t time_us);
extern vsf_systimer_cnt_t vsf_systimer_ms_to_tick(uint_fast32_t time_ms);
extern bool vsf_systimer_is_due(vsf_systimer_cnt_t due);
extern uint_fast32_t vsf_systimer_tick_to_ms(vsf_systimer_cnt_t tick);
extern uint_fast32_t vsf_systimer_tick_to_us(vsf_systimer_cnt_t tick);

/*! \brief initialise a software interrupt
 *! \param idx the index of the software interrupt
 *! \return initialization result in vsf_err_t
 */
extern vsf_err_t vsf_swi_init(uint_fast8_t idx, uint_fast8_t priority,
        vsf_swi_hanler_t *handler, void *p);

/*! \brief trigger a software interrupt
 *! \param idx the index of the software interrupt
 */
extern void vsf_swi_trigger(uint_fast8_t idx);

extern istate_t vsf_set_base_priority(istate_t priority);

extern ALWAYS_INLINE void vsf_arch_set_pc(uint32_t pc);
extern ALWAYS_INLINE uint32_t vsf_arch_get_lr(void);
extern ALWAYS_INLINE void vsf_arch_set_stack(uint32_t stack);

extern istate_t vsf_get_interrupt(void);
extern void vsf_set_interrupt(istate_t level);
extern istate_t vsf_disable_interrupt(void);
extern void vsf_enable_interrupt(void);

extern void vsf_arch_sleep(uint32_t mode);

#endif
/* EOF */
