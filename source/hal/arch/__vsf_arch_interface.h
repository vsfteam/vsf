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

#ifndef __VSF_ARCH_INTERFACE_H__
#define __VSF_ARCH_INTERFACE_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "vsf_arch_abstraction.h"

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************
 * IMPORTANT: This header file can only be included by source code files      *
 *            within arch                                                     *
 ******************************************************************************/

/*============================ MACROS ========================================*/

//! \name different models to implement systimer
//! @{
#define VSF_SYSTIMER_IMPL_REQUEST_RESPONSE                      0
#define VSF_SYSTIMER_IMPL_WITH_NORMAL_TIMER                     1
#define VSF_SYSTIMER_IMPL_WITH_COMP_TIMER                       2
//!@}

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*----------------------------------------------------------------------------*
 * APIs or Interfaces each Arch must use or implement                         *
 *----------------------------------------------------------------------------*/

/*! \note initialize architecture specific service
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
extern bool vsf_arch_low_level_init(void);


#ifdef VSF_SYSTIMER_CFG_IMPL_MODE
/*----------------------------------------------------------------------------*
 * System Timer : Implement with Normal Timer (Count down or Count up)        *
 *----------------------------------------------------------------------------*/
#   if VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_WITH_NORMAL_TIMER

/*-------------------------------------------*
 * APIs to be implemented by target arch     *
 *-------------------------------------------*/
/*! \brief initialise systimer without enable it
 */
extern vsf_err_t vsf_systimer_low_level_init(uintmax_t ticks);

/*! \brief disable systimer and return over-flow flag status
 *!
 *! \Note  IMPORTANT: If the timer peripheral automatically clear the over flow
 *!        flag in register when interrupt is served, you have to create a
 *!        software bit to simulate a flag. When the function
 *!        vsf_systimer_low_level_disable() is called, it should return the flag
 *!        status as boolean value and clear the software flag.
 *!
 *! \param none
 *! \retval true  overflow happened
 *! \retval false no overflow happened
 */
extern bool vsf_systimer_low_level_disable(void);

/*! \brief only enable systimer without clearing any flags
 */
extern void vsf_systimer_low_level_enable(void);
extern void vsf_systimer_low_level_int_disable(void);
extern void vsf_systimer_low_level_int_enable(void);
extern void vsf_systimer_set_reload_value( vsf_systimer_cnt_t tick_cnt);
extern void vsf_systimer_reset_counter_value(void);
extern void vsf_systimer_clear_int_pending_bit(void);
extern vsf_systimer_cnt_t vsf_systimer_get_tick_elapsed(void);

/*-------------------------------------------*
 * APIs to be used by target arch            *
 *-------------------------------------------*/
/*! \brief systimer overflow event handler which is called by target timer
 *!        interrupt handler
 *! \Note  IMPORTANT: If the timer peripheral automatically clear the over flow
 *!        flag in register when interrupt is served, you have to create a
 *!        software bit to simulate a flag which will be cleared by calling
 *!        vsf_systimer_low_level_disable().
 */
extern void vsf_systimer_ovf_evt_hanlder(void);
#   endif

/*----------------------------------------------------------------------------*
 * System Timer : Implement with request / response model                     *
 *----------------------------------------------------------------------------*/
#   if VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_REQUEST_RESPONSE

/*-------------------------------------------*
 * APIs to be implemented by target arch     *
 *-------------------------------------------*/
extern vsf_err_t vsf_systimer_init(void);
extern bool vsf_systimer_is_due(vsf_systimer_cnt_t due);

extern vsf_err_t vsf_systimer_start(void);
extern void vsf_systimer_set_idle(void);

extern vsf_systimer_cnt_t vsf_systimer_get(void);
extern bool vsf_systimer_set(vsf_systimer_cnt_t due);

extern vsf_systimer_cnt_t vsf_systimer_us_to_tick(uint_fast32_t time_us);
extern vsf_systimer_cnt_t vsf_systimer_ms_to_tick(uint_fast32_t time_ms);
extern uint_fast32_t vsf_systimer_tick_to_us(vsf_systimer_cnt_t tick);
extern uint_fast32_t vsf_systimer_tick_to_ms(vsf_systimer_cnt_t tick);

/*-------------------------------------------*
 * APIs to be used by target arch            *
 *-------------------------------------------*/
/*! \brief systimer timeout event handler which is called by request response
 *!        service.
 */
extern void vsf_systimer_timeout_evt_hanlder(vsf_systimer_cnt_t tick);
extern uint_fast32_t vsf_arch_req___systimer_freq___from_usr(void);
extern uint_fast32_t vsf_arch_req___systimer_resolution___from_usr(void);
#   endif
#endif      // VSF_SYSTIMER_CFG_IMPL_MODE


#ifdef __cplusplus
}
#endif

#endif
/* EOF */
