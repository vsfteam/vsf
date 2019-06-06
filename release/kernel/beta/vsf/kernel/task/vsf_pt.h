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
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/


#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   define __implement_vsf_pt(__NAME)                                           \
        void vsf_pt_func_##__NAME(vsf_eda_frame_t *ptFrame, vsf_evt_t evt)      \
        {                                                                       \
            pt_cb_##__NAME *ptThis = (pt_cb_##__NAME *)(ptFrame->param);        \
            uint8_t *ptState = &(ptFrame->state);                               \
            ASSERT(NULL != ptFrame);  

#define __vsf_pt_begin()    enum {                                              \
                                count_offset = __COUNTER__ + 1,                 \
                            };                                                  \
                            switch (*ptState) {                                 \
                                case __COUNTER__ - count_offset:
                                        
#define __vsf_pt_entry(...)     (*ptState) =                                    \
                                    (__COUNTER__ - count_offset + 1) >> 1;      \
                                __VA_ARGS__;                                    \
                                case (__COUNTER__ - count_offset) >> 1:

#   define __vsf_pt_end()                                                       \
                }   /* for switch */                                            \
            }vsf_eda_return();
#else
#   define __implement_vsf_pt(__NAME)                                           \
        void vsf_pt_func_##__NAME(__NAME *ptThis, vsf_evt_t evt)

#define __vsf_pt_begin()    enum {                                              \
                                count_offset = __COUNTER__ + 1,                 \
                            };                                                  \
                            switch (ptThis->chState) {                          \
                                case __COUNTER__ - count_offset:
                                        
#define __vsf_pt_entry(...)     ptThis->chState =                               \
                                    (__COUNTER__ - count_offset + 1) >> 1;      \
                                __VA_ARGS__;                                    \
                                case (__COUNTER__ - count_offset) >> 1:

#   define __vsf_pt_end()                                                       \
            } vsf_eda_return();
#endif


                                        



#define vsf_pt_begin()              __vsf_pt_begin()
#define vsf_pt_entry(...)           __vsf_pt_entry(__VA_ARGS__)
#define vsf_pt_end()                __vsf_pt_end()

#define __vsf_pt_wait_cond(...)                                                 \
    do {                                                                        \
        evt = VSF_EVT_INVALID;                                                  \
        vsf_pt_entry();                                                         \
        if (__VA_ARGS__){                                                       \
            return ;                                                            \
        }                                                                       \
    } while (0)

#define vsf_pt_yield()          vsf_pt_entry(vsf_eda_yield();)


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


#define vsf_pt_wfe(__evt)               __vsf_pt_wait_cond( (evt != __evr))

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   define __vsf_pt_call_sub(__NAME, __TARGET)                                  \
            vsf_eda_call_eda((vsf_eda_evthandler_t)(__NAME),                    \
                            (__TARGET))


#   define vsf_pt_call_sub(__NAME, __TARGET)                                    \
        vsf_pt_entry();                                                         \
        if (VSF_ERR_NONE != __vsf_pt_call_sub(__NAME, (__TARGET))) {            \
            return ;                                                            \
        }                                                                       \
        vsf_pt_entry(return;); 

#   define vsf_call_pt(__NAME, __TARGET)                                        \
            vsf_pt_call_sub(vsf_pt_func(__NAME), (__TARGET))
        
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
#   define __vsf_pt_call_fsm(__NAME, __TARGET)                                  \
            vsf_eda_call_fsm((vsf_fsm_entry_t)(__NAME),                         \
                            (__TARGET))


#   define vsf_pt_call_fsm(__NAME, __TARGET, __RET_ADDR)                        \
        do {                                                                    \
            fsm_rt_t ATPASTE3(__,__LINE__,tReturn);                             \
            /*(__TARGET)->chState = 0; */                                       \
            vsf_pt_entry();                                                     \
            ATPASTE3(__,__LINE__,tReturn) =                                     \
                __vsf_pt_call_fsm(__NAME, (__TARGET));                          \
            if (fsm_rt_on_going == ATPASTE3(__,__LINE__,tReturn)) {             \
                return ;                                                        \
            }                                                                   \
            if (NULL != (__RET_ADDR)) {                                         \
                *(__RET_ADDR) = ATPASTE3(__,__LINE__,tReturn);                  \
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
        
#define __vsf_pt_func(__NAME)           vsf_pt_func_##__NAME        
#define vsf_pt_func(__NAME)             __vsf_pt_func(__NAME)

#define __vsf_pt(__NAME)                pt_cb_##__NAME
#define vsf_pt(__NAME)                  __vsf_pt(__NAME)

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   define __def_vsf_pt(__NAME,...)                                             \
        struct pt_cb_##__NAME {                                                 \
            __VA_ARGS__                                                         \
        };                                                                      \
        struct __NAME {                                                         \
            implement(vsf_pt_t);                                                \
            implement_ex(pt_cb_##__NAME, param);                                \
        };
#else
#   define __def_vsf_pt(__NAME,...)                                             \
        struct pt_cb_##__NAME {                                                 \
            uint8_t chState;                                                    \
            __VA_ARGS__                                                         \
        };                                                                      \
        struct __NAME {                                                         \
            implement(vsf_pt_t);                                                \
            implement_ex(pt_cb_##__NAME, param);                                \
        };
#endif

#define def_vsf_pt(__NAME,...)                                                  \
            __def_vsf_pt(__NAME,__VA_ARGS__)

#define __declare_vsf_pt(__NAME)                                                \
            typedef struct __NAME __NAME;                                       \
            typedef struct pt_cb_##__NAME  pt_cb_##__NAME;

#define declare_vsf_pt(__NAME)          __declare_vsf_pt(__NAME)

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
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
