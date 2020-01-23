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

#ifndef __VSF_KERNEL_SHELL_SIMPLE_H__
#define __VSF_KERNEL_SHELL_SIMPLE_H__

/*============================ INCLUDES ======================================*/
#include "kernel/vsf_kernel_cfg.h"

#if VSF_USE_KERNEL_SIMPLE_SHELL == ENABLED
#include "../../vsf_eda.h"
#include "./vsf_simple_bmevt.h"
/*============================ MACROS ========================================*/

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   define using_basic_ipc            vsf_sync_reason_t reason
#else
#   define using_basic_ipc          
#endif
/*============================ MACROFIED FUNCTIONS ===========================*/

/*----------------------------------------------------------------------------*
 * Common                                                                     *
 *----------------------------------------------------------------------------*/

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   define static_task_instance(__MEMBER)                                       \
        struct {uint_fast16_t tState;__MEMBER;                                  \
        } static TPASTE2(__local_cb, __LINE__),                                 \
            *ptThis = &TPASTE2(__local_cb, __LINE__);                           \
             UNUSED_PARAM(ptThis);
             
#   if __IS_COMPILER_IAR__
#       define features_used(__MEMBER)              __MEMBER;
#       define mem_sharable(__MEMBER)                                           \
            union {uint_fast8_t __zzzz_do_not_use; __MEMBER;};
#       define mem_nonsharable(__MEMBER)            __MEMBER;
#   else
#       define features_used(__MEMBER)              __MEMBER
#       define mem_sharable(__MEMBER)               __MEMBER
#       define mem_nonsharable(__MEMBER)            __MEMBER
#   endif
#else
#   define static_task_instance(...)                                            \
        struct {uint_fast8_t tState;__VA_ARGS__                                 \
        } static TPASTE2(__local_cb, __LINE__),                                 \
            *ptThis = &TPASTE2(__local_cb, __LINE__);                           \
             UNUSED_PARAM(ptThis);
            
#   if __IS_COMPILER_IAR__
#       define features_used(...)              __VA_ARGS__;
#       define mem_sharable(...)                                                \
            union {uint_fast8_t __zzzz_do_not_use; __VA_ARGS__;};
#       define mem_nonsharable(...)            __VA_ARGS__;
#   else
#       define features_used(...)              __VA_ARGS__
#       define mem_sharable(...)               union {__VA_ARGS__};
#       define mem_nonsharable(...)            __VA_ARGS__
#   endif
#endif





#define vsf_yield(__pevt)                                                       \
            for (   vsf_evt_t result = VSF_EVT_INVALID;                         \
                    result == VSF_EVT_INVALID;)                                 \
                if ((result =__vsf_yield(__pevt), result == VSF_EVT_YIELD))

/*----------------------------------------------------------------------------*
 * Delay                                                                      *
 *----------------------------------------------------------------------------*/
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
#   define vsf_delay(__TICK)                                                    \
    if (VSF_EVT_TIMER == __vsf_delay((uint_fast32_t)__TICK))
#   define vsf_delay_ms(__MS)                                                   \
    if (VSF_EVT_TIMER == __vsf_delay((uint_fast32_t)vsf_systimer_ms_to_tick(__MS)))
#   define vsf_delay_us(__US)                                                   \
    if (VSF_EVT_TIMER == __vsf_delay((uint_fast32_t)vsf_systimer_us_to_tick(__US)))
#endif



#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
/*----------------------------------------------------------------------------*
 * IPC Common                                                                 *
 *----------------------------------------------------------------------------*/
#   define on_timeout()                                                         \
                if (VSF_SYNC_TIMEOUT == vsf_eda_get_cur_evt())

/*----------------------------------------------------------------------------*
 * Mutex                                                                      *
 *----------------------------------------------------------------------------*/
#   define vsf_mutex_init(__MUTEX_ADDR)     vsf_eda_mutex_init(__MUTEX_ADDR)
#   define vsf_mutex_leave(__MUTEX_ADDR)    vsf_eda_mutex_leave(__MUTEX_ADDR)

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   define vsf_mutex_enter(___MUTEX_ADDR)                                       \
            for (   ptThis->reason = VSF_SYNC_CANCEL;                              \
                    ptThis->reason == VSF_SYNC_CANCEL;)                                 \
                if ((ptThis->reason =__vsf_mutex_enter((___MUTEX_ADDR),                 \
                    (-1)), ptThis->reason == VSF_SYNC_GET))
                    
#   define vsf_mutex_try_to_enter_timeout(___MUTEX_ADDR, __TIMEOUT)             \
            for (   ptThis->reason = VSF_SYNC_CANCEL;                                   \
                    ptThis->reason == VSF_SYNC_CANCEL;)                                 \
                if ((ptThis->reason =__vsf_mutex_enter((___MUTEX_ADDR),                 \
                    (__TIMEOUT)),                                               \
                    (ptThis->reason == VSF_SYNC_GET || ptThis->reason = VSF_SYNC_TIMEOUT)))

#   define vsf_mutex_try_to_enter_timeout_ms(___MUTEX_ADDR, __TIMEOUT)          \
            for (   ptThis->reason = VSF_SYNC_CANCEL;                                   \
                    ptThis->reason == VSF_SYNC_CANCEL;)                                 \
                if ((ptThis->reason =__vsf_mutex_enter((___MUTEX_ADDR),                 \
                    vsf_systimer_ms_to_tick(__TIMEOUT)),                        \
                    (ptThis->reason == VSF_SYNC_GET || ptThis->reason == VSF_SYNC_TIMEOUT)))

#   define vsf_mutex_try_to_enter_timeout_us(___MUTEX_ADDR, __TIMEOUT)          \
            for (   ptThis->reason = VSF_SYNC_CANCEL;                                   \
                    ptThis->reason == VSF_SYNC_CANCEL;)                                 \
                if ((ptThis->reason =__vsf_mutex_enter((___MUTEX_ADDR),                 \
                    vsf_systimer_us_to_tick(__TIMEOUT)),                        \
                    (ptThis->reason == VSF_SYNC_GET || ptThis->reason == VSF_SYNC_TIMEOUT)))
#else
#   define vsf_mutex_enter(___MUTEX_ADDR)                                       \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_mutex_enter((___MUTEX_ADDR),                 \
                    (-1)), reason == VSF_SYNC_GET))
                    
#   define vsf_mutex_try_to_enter_timeout(___MUTEX_ADDR, __TIMEOUT)             \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_mutex_enter((___MUTEX_ADDR),                 \
                    (__TIMEOUT)),                                               \
                    (reason == VSF_SYNC_GET || reason = VSF_SYNC_TIMEOUT)))

#   define vsf_mutex_try_to_enter_timeout_ms(___MUTEX_ADDR, __TIMEOUT)          \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_mutex_enter((___MUTEX_ADDR),                 \
                    vsf_systimer_ms_to_tick(__TIMEOUT)),                        \
                    (reason == VSF_SYNC_GET || reason == VSF_SYNC_TIMEOUT)))

#   define vsf_mutex_try_to_enter_timeout_us(___MUTEX_ADDR, __TIMEOUT)          \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_mutex_enter((___MUTEX_ADDR),                 \
                    vsf_systimer_us_to_tick(__TIMEOUT)),                        \
                    (reason == VSF_SYNC_GET || reason == VSF_SYNC_TIMEOUT)))
#endif

/*----------------------------------------------------------------------------*
 * Critical Section                                                           *
 *----------------------------------------------------------------------------*/
#   define vsf_crit_init(__CRIT_ADDR)     vsf_eda_crit_init(__CRIT_ADDR)
#   define vsf_crit_leave(__CRIT_ADDR)    vsf_eda_crit_leave(__CRIT_ADDR)

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   define vsf_crit_enter(___CRIT_ADDR)                                         \
            for (   ptThis->reason = VSF_SYNC_CANCEL;                                   \
                    ptThis->reason == VSF_SYNC_CANCEL;)                                 \
                if ((ptThis->reason =__vsf_mutex_enter((___CRIT_ADDR),                  \
                    (-1)), ptThis->reason == VSF_SYNC_GET))
                    
#   define vsf_crit_try_to_enter_timeout(___CRIT_ADDR, __TIMEOUT)               \
            for (   ptThis->reason = VSF_SYNC_CANCEL;                                   \
                    ptThis->reason == VSF_SYNC_CANCEL;)                                 \
                if ((ptThis->reason =__vsf_mutex_enter((___CRIT_ADDR),                  \
                    (__TIMEOUT)),                                               \
                    (ptThis->reason == VSF_SYNC_GET || ptThis->reason = VSF_SYNC_TIMEOUT)))

#   define vsf_crit_try_to_enter_timeout_ms(___CRIT_ADDR, __TIMEOUT)            \
            for (   ptThis->reason = VSF_SYNC_CANCEL;                                   \
                    ptThis->reason == VSF_SYNC_CANCEL;)                                 \
                if ((ptThis->reason =__vsf_mutex_enter((___CRIT_ADDR),                  \
                    vsf_systimer_ms_to_tick(__TIMEOUT)),                        \
                    (ptThis->reason == VSF_SYNC_GET || ptThis->reason == VSF_SYNC_TIMEOUT)))

#   define vsf_crit_try_to_enter_timeout_us(___CRIT_ADDR, __TIMEOUT)            \
            for (   ptThis->reason = VSF_SYNC_CANCEL;                 \
                    ptThis->reason == VSF_SYNC_CANCEL;)                                 \
                if ((ptThis->reason =__vsf_mutex_enter((___CRIT_ADDR),                  \
                    vsf_systimer_us_to_tick(__TIMEOUT)),                        \
                    (ptThis->reason == VSF_SYNC_GET || ptThis->reason == VSF_SYNC_TIMEOUT)))
#else
#   define vsf_crit_enter(___CRIT_ADDR)                                         \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_mutex_enter((___CRIT_ADDR),                  \
                    (-1)), reason == VSF_SYNC_GET))
                    
#   define vsf_crit_try_to_enter_timeout(___CRIT_ADDR, __TIMEOUT)               \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_mutex_enter((___CRIT_ADDR),                  \
                    (__TIMEOUT)),                                               \
                    (reason == VSF_SYNC_GET || reason = VSF_SYNC_TIMEOUT)))

#   define vsf_crit_try_to_enter_timeout_ms(___CRIT_ADDR, __TIMEOUT)            \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_mutex_enter((___CRIT_ADDR),                  \
                    vsf_systimer_ms_to_tick(__TIMEOUT)),                        \
                    (reason == VSF_SYNC_GET || reason == VSF_SYNC_TIMEOUT)))

#   define vsf_crit_try_to_enter_timeout_us(___CRIT_ADDR, __TIMEOUT)            \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_mutex_enter((___CRIT_ADDR),                  \
                    vsf_systimer_us_to_tick(__TIMEOUT)),                        \
                    (reason == VSF_SYNC_GET || reason == VSF_SYNC_TIMEOUT)))
#endif

/*----------------------------------------------------------------------------*
 * Semaphore                                                                  *
 *----------------------------------------------------------------------------*/
#   define vsf_sem_init(__psem, __cnt)                                          \
            vsf_eda_sync_init((__psem), (__cnt), 0x7FFF | VSF_SYNC_AUTO_RST)
#   define vsf_sem_post(__psem)             vsf_eda_sem_post((__psem))
            
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   define vsf_sem_pend(__psem)                                                 \
            for (   ptThis->reason = VSF_SYNC_CANCEL;                                   \
                    ptThis->reason == VSF_SYNC_CANCEL;)                                 \
                if ((ptThis->reason =__vsf_sem_pend((__psem),                           \
                    (-1)), ptThis->reason == VSF_SYNC_GET))
                    
#   define vsf_sem_pend_timeout(__psem, __timeout)                              \
            for (   ptThis->reason = VSF_SYNC_CANCEL;                                   \
                    ptThis->reason == VSF_SYNC_CANCEL;)                                 \
                if ((ptThis->reason =__vsf_sem_pend((__psem),                           \
                    (__timeout)),                                               \
                    (ptThis->reason == VSF_SYNC_GET || ptThis->reason = VSF_SYNC_TIMEOUT)))


#   define vsf_sem_pend_timeout_ms(__psem, __timeout)                           \
            for (   ptThis->reason = VSF_SYNC_CANCEL;                                   \
                    ptThis->reason == VSF_SYNC_CANCEL;)                                 \
                if ((ptThis->reason =__vsf_sem_pend((__psem),                           \
                    vsf_systimer_ms_to_tick(__timeout)),                        \
                    (ptThis->reason == VSF_SYNC_GET || ptThis->reason == VSF_SYNC_TIMEOUT)))

#   define vsf_sem_pend_timeout_us(__psem, __timeout)                           \
            for (   ptThis->reason = VSF_SYNC_CANCEL;                                   \
                    ptThis->reason == VSF_SYNC_CANCEL;)                                 \
                if ((ptThis->reason =__vsf_sem_pend((__psem),                           \
                    vsf_systimer_us_to_tick(__timeout)),                        \
                    (ptThis->reason == VSF_SYNC_GET || ptThis->reason == VSF_SYNC_TIMEOUT)))
#else
#   define vsf_sem_pend(__psem)                                                 \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_sem_pend((__psem),                           \
                    (-1)), reason == VSF_SYNC_GET))
                    
#   define vsf_sem_pend_timeout(__psem, __timeout)                              \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_sem_pend((__psem),                           \
                    (__timeout)),                                               \
                    (reason == VSF_SYNC_GET || reason = VSF_SYNC_TIMEOUT)))


#   define vsf_sem_pend_timeout_ms(__psem, __timeout)                           \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_sem_pend((__psem),                           \
                    vsf_systimer_ms_to_tick(__timeout)),                        \
                    (reason == VSF_SYNC_GET || reason == VSF_SYNC_TIMEOUT)))

#   define vsf_sem_pend_timeout_us(__psem, __timeout)                           \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_sem_pend((__psem),                           \
                    vsf_systimer_us_to_tick(__timeout)),                        \
                    (reason == VSF_SYNC_GET || reason == VSF_SYNC_TIMEOUT)))
#endif

/*----------------------------------------------------------------------------*
 * Event                                                                      *
 *----------------------------------------------------------------------------*/
#   define vsf_trig_init(__ptrig, __set, __auto_rst)                            \
            vsf_eda_trig_init(__ptrig, __set, __auto_rst)
#   define vsf_trig_set(__ptrig)            vsf_eda_trig_set((__ptrig))
#   define vsf_trig_reset(__ptrig)          vsf_eda_trig_reset((__ptrig))

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   define vsf_trig_wait(__ptrig)                                               \
            for (   ptThis->reason = VSF_SYNC_CANCEL;                 \
                    ptThis->reason == VSF_SYNC_CANCEL;)                                 \
                if ((ptThis->reason =__vsf_sem_pend((__ptrig),                          \
                    (-1)), ptThis->reason == VSF_SYNC_GET))
                    
#   define vsf_trig_wait_timeout(__ptrig, __timeout)                            \
            for (   ptThis->reason = VSF_SYNC_CANCEL;                 \
                    ptThis->reason == VSF_SYNC_CANCEL;)                                 \
                if ((ptThis->reason =__vsf_sem_pend((__ptrig),                          \
                    (__timeout)),                                               \
                    (ptThis->reason == VSF_SYNC_GET || ptThis->reason = VSF_SYNC_TIMEOUT)))


#   define vsf_trig_wait_timeout_ms(__ptrig, __timeout)                         \
            for (   ptThis->reason = VSF_SYNC_CANCEL;                 \
                    ptThis->reason == VSF_SYNC_CANCEL;)                                 \
                if ((ptThis->reason =__vsf_sem_pend((__ptrig),                          \
                    vsf_systimer_ms_to_tick(__timeout)),                        \
                    (ptThis->reason == VSF_SYNC_GET || ptThis->reason == VSF_SYNC_TIMEOUT)))

#   define vsf_trig_wait_timeout_us(__ptrig, __timeout)                         \
            for (   ptThis->reason = VSF_SYNC_CANCEL;                 \
                    ptThis->reason == VSF_SYNC_CANCEL;)                                 \
                if ((ptThis->reason =__vsf_sem_pend((__ptrig),                          \
                    vsf_systimer_us_to_tick(__timeout)),                        \
                    (ptThis->reason == VSF_SYNC_GET || ptThis->reason == VSF_SYNC_TIMEOUT)))
#else
#   define vsf_trig_wait(__ptrig)                                               \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_sem_pend((__ptrig),                          \
                    (-1)), reason == VSF_SYNC_GET))
                    
#   define vsf_trig_wait_timeout(__ptrig, __timeout)                            \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_sem_pend((__ptrig),                          \
                    (__timeout)),                                               \
                    (reason == VSF_SYNC_GET || reason = VSF_SYNC_TIMEOUT)))


#   define vsf_trig_wait_timeout_ms(__ptrig, __timeout)                         \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_sem_pend((__ptrig),                          \
                    vsf_systimer_ms_to_tick(__timeout)),                        \
                    (reason == VSF_SYNC_GET || reason == VSF_SYNC_TIMEOUT)))

#   define vsf_trig_wait_timeout_us(__ptrig, __timeout)                         \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_sem_pend((__ptrig),                          \
                    vsf_systimer_us_to_tick(__timeout)),                        \
                    (reason == VSF_SYNC_GET || reason == VSF_SYNC_TIMEOUT)))
#endif

#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
SECTION("text.vsf.kernel.__vsf_delay")
extern vsf_evt_t __vsf_delay(uint_fast32_t ms);
#endif

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
SECTION("text.vsf.kernel.__vsf_sem_pend")
extern 
vsf_sync_reason_t __vsf_sem_pend(vsf_sem_t *psem, int_fast32_t time_out);

SECTION("text.vsf.kernel.vsf_mutex_enter")
extern 
vsf_sync_reason_t __vsf_mutex_enter(vsf_mutex_t *pmtx, int_fast32_t time_out);
#endif

#endif

SECTION("text.vsf.kernel.vsf_yield")
extern vsf_evt_t __vsf_yield(void);

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
SECTION("text.vsf.kernel.__vsf_call_eda")
extern vsf_err_t __vsf_call_eda(uintptr_t evthandler, uintptr_t param);

SECTION("text.vsf.kernel.__vsf_call_fsm")
extern fsm_rt_t __vsf_call_fsm(vsf_fsm_entry_t entry, uintptr_t param);
#endif
#endif
/* EOF */
