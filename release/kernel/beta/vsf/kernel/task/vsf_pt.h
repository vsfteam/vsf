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

#ifndef __VSF_PT_H__
#define __VSF_PT_H__

/*============================ INCLUDES ======================================*/
#include "./kernel/vsf_kernel_cfg.h"

#if VSF_KERNEL_CFG_EDA_SUPPORT_PT == ENABLED
#include "../vsf_eda.h"
#include "./__vsf_task_common.h"

/*============================ MACROS ========================================*/

#ifndef this
#   define this        (*ptThis)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/


#if     VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED                          \
    &&  VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
#   define __implement_vsf_pt(__NAME)                                           \
        __implement_vsf_pt_common(__NAME, vsf_eda_frame_t *ptFrame)             \
        {    __vsf_pt_common(__NAME) *ptThis =                                  \
                        (__vsf_pt_common(__NAME) *)(ptFrame->param);            \
            ASSERT(NULL != ptFrame);

#   define __vsf_pt_state()         (ptFrame)->state
#   define __vsf_pt_end()           __vsf_pt_end_closure_common()
#else

#   if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#       define __implement_vsf_pt(__NAME)                                       \
            __implement_vsf_pt_common(__NAME, __vsf_pt_common(__NAME) *ptThis)
#   else
#       define __implement_vsf_pt(__NAME)                                       \
            __implement_vsf_pt_common(__NAME, __NAME *ptThis)
#   endif

#   define __vsf_pt_state()         (ptThis)->chState
#   define __vsf_pt_end()           __vsf_pt_end_common()
#endif


#define __vsf_pt_begin(__state)     __vsf_pt_begin_common(__state)
#define __vsf_pt_entry(__state, ...)__vsf_pt_entry_common(__state, __VA_ARGS__)

#define vsf_pt_begin()              __vsf_pt_begin(__vsf_pt_state())
#define vsf_pt_entry(...)           __vsf_pt_entry(__vsf_pt_state(), __VA_ARGS__)
#define vsf_pt_end()                __vsf_pt_end()

#define __vsf_pt_wait_cond(...)     __vsf_pt_wait_cond_common(__vsf_pt_state())
#define vsf_pt_wfe(__evt)           __vsf_pt_wait_cond((evt != __evr))
#define vsf_pt_wait_for_evt(__evt)  vsf_pt_wfe(__evt)

#define vsf_pt_yield()              vsf_pt_entry(vsf_eda_yield();)


/*! \note please use if-then clause in the vsf_pt_wait_until()
 *!       E.g. 
 *!       vsf_pt_wait_until( if (...) );
 *!       
 *!
 *!       NOTE: wait_for_one(), wait_for_any() and wait_for_all() can be seen as
 *!             an if-then clause. So you can use them directly here. E.g.
 *!
 *!       vsf_pt_wait_until( wait_for_one(&__user_grouped_evts, sem_evt_msk) );
 *!             
 */

#define vsf_pt_wait_until(...)                                                  \
            vsf_pt_entry();                                                     \
            __VA_ARGS__ {} else {                                               \
                return ;                                                        \
            }                                                                   

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   define __vsf_pt_call_sub(__NAME, __TARGET)                                  \
            __vsf_eda_call_eda((vsf_eda_evthandler_t)(__NAME),                  \
                            (__TARGET))


#   define vsf_pt_call_sub(__NAME, __TARGET)                                    \
        vsf_pt_entry();                                                         \
        if (VSF_ERR_NONE != __vsf_pt_call_sub(__NAME, (__TARGET))) {            \
            return ;                                                            \
        }                                                                       \
        vsf_pt_entry(return;); 

#   if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
#       define vsf_pt_call_pt(__NAME, __TARGET)                                 \
            vsf_pt_call_sub(vsf_pt_func(__NAME), (__TARGET))
#   else
#       define vsf_pt_call_pt(__NAME, __TARGET)                                 \
            (__TARGET)->chState = 0;                                            \
            vsf_pt_call_sub(vsf_pt_func(__NAME), (__TARGET))
#   endif
#endif

#define vsf_eda_call_pt(__NAME, __TARGET)                                       \
            __vsf_pt_call_sub(vsf_pt_func(__NAME), (__TARGET))            

#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
#   define __vsf_pt_call_fsm(__NAME, __TARGET)                                  \
            vsf_eda_call_fsm((vsf_fsm_entry_t)(__NAME),                         \
                            (__TARGET))


#   define vsf_pt_call_fsm(__NAME, __TARGET, __RET_ADDR)                        \
        do {                                                                    \
            fsm_rt_t ATPASTE3(__vsf_pt_call_fsm,__LINE__,tReturn);              \
            /*(__TARGET)->chState = 0; */                                       \
            vsf_pt_entry();                                                     \
            ATPASTE3(__vsf_pt_call_fsm,__LINE__,tReturn) =                      \
                __vsf_pt_call_fsm(__NAME, (__TARGET));                          \
            if (fsm_rt_on_going ==                                              \
                ATPASTE3(__vsf_pt_call_fsm,__LINE__,tReturn)) {                 \
                return ;                                                        \
            }                                                                   \
            if (NULL != (__RET_ADDR)) {                                         \
                *(__RET_ADDR) = ATPASTE3(__vsf_pt_call_fsm,__LINE__,tReturn);   \
            }                                                                   \
        } while(0)
        
#   define vsf_pt_call_task(__NAME, __TARGET, __RET_ADDR)                       \
        vsf_pt_call_fsm(vsf_task_func(__NAME), __TARGET, __RET_ADDR)
#endif

#if VSF_CFG_TIMER_EN == ENABLED
#   define vsf_pt_start(...)        vsf_teda_init_ex(__VA_ARGS__)
#else
#   define vsf_pt_start(...)        vsf_eda_init_ex(__VA_ARGS__)
#endif


        
#define implement_vsf_pt(__NAME)        __implement_vsf_pt(__NAME)
        
#define __vsf_pt_func(__NAME)           __vsf_pt_func_common(__NAME)
#define vsf_pt_func(__NAME)             __vsf_pt_func(__NAME)

#define __vsf_pt(__NAME)                __vsf_pt_common(__NAME)
#define vsf_pt(__NAME)                  __vsf_pt(__NAME)

#if     VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED                          \
    &&  VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
#   define __def_vsf_pt(__NAME,...)                                             \
            __def_vsf_pt_common(__NAME, __VA_ARGS__)                            \
            __extern_vsf_pt_common(__NAME, vsf_eda_frame_t *ptFrame);        
#else
#   define __def_vsf_pt(__NAME,...)                                             \
            __def_vsf_pt_common(__NAME, uint8_t chState; __VA_ARGS__)           \
            __extern_vsf_pt_common(__NAME, __vsf_pt_common(__NAME) *ptThis);
#endif

#define def_vsf_pt(__NAME,...)                                                  \
            __def_vsf_pt(__NAME,__VA_ARGS__)

#define __declare_vsf_pt(__NAME)                                                \
            __declare_vsf_pt_common(__NAME)

#define declare_vsf_pt(__NAME)          __declare_vsf_pt(__NAME)

#if     VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED                          \
    &&  VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
#   define __init_vsf_pt(__NAME, __PT, __PRI, ...)                              \
        do {                                                                    \
            vsf_eda_cfg_t ATPASTE3(__,__LINE__,tCFG) = {                        \
                .evthandler = (vsf_eda_evthandler_t)__vsf_pt_func(__NAME),      \
                .priority = (__PRI),                                            \
                .target = &((__PT)->param),                                     \
                __VA_ARGS__                                                     \
            };                                                                  \
            vsf_pt_start( &((__PT)->use_as__vsf_pt_t),                          \
                            &ATPASTE3(__,__LINE__,tCFG));                       \
        } while(0)
#else
#   define __init_vsf_pt(__NAME, __PT, __PRI, ...)                              \
        do {                                                                    \
            vsf_eda_cfg_t ATPASTE3(__,__LINE__,tCFG) = {                        \
                .evthandler = (vsf_eda_evthandler_t)__vsf_pt_func(__NAME),      \
                .priority = (__PRI),                                            \
                .target = &((__PT)->param),                                     \
                __VA_ARGS__                                                     \
            };                                                                  \
            (__PT)->param.chState = 0;                                          \
            vsf_pt_start( &((__PT)->use_as__vsf_pt_t),                          \
                            &ATPASTE3(__,__LINE__,tCFG));                       \
        } while(0)
#endif


#if     VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED                                \
    &&  VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   define vsf_pt_call_thread(__NAME, __TARGET)                                 \
        vsf_eda_call_thread_prepare(__NAME, __TARGET);                          \
        vsf_pt_entry();                                                         \
        if (VSF_ERR_NONE != vsf_eda_call_thread(__TARGET)) {                    \
            return ;                                                            \
        }                                                                       \
        vsf_pt_entry(return;); 
#endif

#define init_vsf_pt(__NAME, __PT, __PRI, ...)                                   \
            __init_vsf_pt(__NAME, __PT, __PRI, __VA_ARGS__)
/*============================ TYPES =========================================*/

#if VSF_CFG_TIMER_EN == ENABLED
typedef vsf_teda_t  vsf_pt_t;
#else
typedef vsf_eda_t  vsf_pt_t;
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif
#endif
