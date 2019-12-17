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

#ifndef __VSF_KERNEL_TASK_H__
#define __VSF_KERNEL_TASK_H__

/*============================ INCLUDES ======================================*/
#include "./kernel/vsf_kernel_cfg.h"

#if VSF_USE_KERNEL == ENABLED
#include "service/vsf_service.h"
#include "../vsf_eda.h"
#include "./vsf_pt.h"
#include "./vsf_thread.h"
#include "./vsf_fsm.h"



/*============================ MACROS ========================================*/
#if     VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED                          \
    &&  VSF_KERNEL_CFG_EDA_SUPPORT_FSM == DISABLED
#warning \
 Although VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is enabled, but\
 VSF_KERNEL_CFG_EDA_SUPPORT_FSM is disabled, hence vsf_task will be treated as\
 VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is disabled. If this is not what you wanted,\
 please set VSF_KERNEL_CFG_EDA_SUPPORT_FSM to ENABLED.
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
#   define __implement_vsf_task(__NAME)                                         \
        fsm_rt_t vsf_task_func_##__NAME(struct task_cb_##__NAME * ptThis,       \
                                        vsf_evt_t evt)                          \
        {                                                                       
            
#   define vsf_task_begin()  

#   define vsf_task_end()                                                       \
            } return fsm_rt_on_going;

#   define vsf_task_state  (ptThis->tState)

#else
#   define __implement_vsf_task(__NAME)                                         \
        void vsf_task_func_##__NAME(__NAME *ptThis, vsf_evt_t evt)
        
#   define vsf_task_begin()  
#   define vsf_task_end()           vsf_eda_yield();
                
#   define vsf_task_state  (ptThis->tState)
#endif

#define implement_vsf_task(__NAME)  __implement_vsf_task(__NAME)
        
        
#define __vsf_task_func(__NAME)     vsf_task_func_##__NAME
#define vsf_task_func(__NAME)       __vsf_task_func(__NAME)

#define __vsf_task(__NAME)          task_cb_##__NAME
#define vsf_task(__NAME)            __vsf_task(__NAME)


#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
#   define vsf_task_start           vsf_teda_init_ex
#else
#   define vsf_task_start           vsf_eda_init_ex
#endif

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
#   define __def_vsf_task(__NAME,__MEMBER)                                      \
        struct task_cb_##__NAME {                                               \
            uint8_t tState;                                                    \
            vsf_sync_reason_t reason;                                           \
            __MEMBER                                                            \
        };                                                                      \
        struct __NAME {                                                         \
            implement(vsf_task_t);                                              \
            implement_ex(task_cb_##__NAME, param);                              \
        };                                                                      \
        extern fsm_rt_t vsf_task_func_##__NAME( struct task_cb_##__NAME *ptThis,\
                                                vsf_evt_t evt);                 
        
#else
#   define __def_vsf_task(__NAME,__MEMBER)                                      \
        struct task_cb_##__NAME {                                               \
            uint8_t tState;                                                    \
            vsf_sync_reason_t reason;                                           \
            __MEMBER                                                            \
        };                                                                      \
        struct __NAME {                                                         \
            implement(vsf_task_t);                                              \
            implement_ex(task_cb_##__NAME, param);                              \
        };                                                                      \
        extern void vsf_task_func_##__NAME(struct __NAME *ptThis, vsf_evt_t evt);
#endif
#else
#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
#   define __def_vsf_task(__NAME,...)                                           \
        struct task_cb_##__NAME {                                               \
            uint8_t tState;                                                    \
            __VA_ARGS__                                                         \
        };                                                                      \
        struct __NAME {                                                         \
            implement(vsf_task_t);                                              \
            implement_ex(task_cb_##__NAME, param);                              \
        };                                                                      \
        extern fsm_rt_t vsf_task_func_##__NAME( struct task_cb_##__NAME *ptThis,\
                                                vsf_evt_t evt);                 
        
#else
#   define __def_vsf_task(__NAME,...)                                           \
        struct task_cb_##__NAME {                                               \
            uint8_t tState;                                                    \
            __VA_ARGS__                                                         \
        };                                                                      \
        struct __NAME {                                                         \
            implement(vsf_task_t);                                              \
            implement_ex(task_cb_##__NAME, param);                              \
        };                                                                      \
        extern void vsf_task_func_##__NAME(struct __NAME *ptThis, vsf_evt_t evt);
#endif
#endif

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#define def_vsf_task(__NAME, __MEMBER)      __def_vsf_task(__NAME, __MEMBER)
#else
#define def_vsf_task(__NAME,...)            __def_vsf_task(__NAME,__VA_ARGS__)
#endif

#define __declare_vsf_task(__NAME)                                              \
            typedef struct __NAME __NAME;                                       \
            typedef struct task_cb_##__NAME  task_cb_##__NAME;

#define declare_vsf_task(__NAME)      __declare_vsf_task(__NAME)

#define prepare_vsf_task(__NAME, __TASK)                                        \
            do {(__TASK)->tState = 0; } while(0)

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
#   define __init_vsf_task(__NAME, __TASK, __PRI)                               \
        do {                                                                    \
            vsf_eda_cfg_t ATPASTE3(__,__LINE__,tCFG) = {                        \
                .fn.fsm_entry = (vsf_task_entry_t)vsf_task_func(__NAME),        \
                .priority = (__PRI),                                            \
                .target = (uintptr_t)&((__TASK)->param),                        \
                .is_fsm = true,                                                 \
            };                                                                  \
            prepare_vsf_task(__NAME, &((__TASK)->param));                       \
            vsf_task_start( &((__TASK)->use_as__vsf_task_t),                    \
                            &ATPASTE3(__,__LINE__,tCFG));                       \
        } while(0)
#else
#   define __init_vsf_task(__NAME, __TASK, __PRI)                               \
        do {                                                                    \
            vsf_eda_cfg_t ATPASTE3(__,__LINE__,tCFG) = {                        \
                .fn.evthandler = (vsf_task_entry_t)vsf_task_func(__NAME),       \
                .priority = (__PRI),                                            \
                .target = NULL,                                                 \
            };                                                                  \
            prepare_vsf_task(__NAME, &((__TASK)->param));                       \
            vsf_task_start( &((__TASK)->use_as__vsf_task_t),                    \
                            &ATPASTE3(__,__LINE__,tCFG));                       \
        } while(0)
#endif
#else
#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
#   define __init_vsf_task(__NAME, __TASK, __PRI, ...)                          \
        do {                                                                    \
            vsf_eda_cfg_t ATPASTE3(__,__LINE__,tCFG) = {                        \
                .fn.fsm_entry = (vsf_task_entry_t)vsf_task_func(__NAME),        \
                .priority = (__PRI),                                            \
                .target = (uintptr_t)&((__TASK)->param),                        \
                .is_fsm = true,                                                 \
                __VA_ARGS__                                                     \
            };                                                                  \
            prepare_vsf_task(__NAME, &((__TASK)->param));                       \
            vsf_task_start( &((__TASK)->use_as__vsf_task_t),                    \
                            &ATPASTE3(__,__LINE__,tCFG));                       \
        } while(0)
#else
#   define __init_vsf_task(__NAME, __TASK, __PRI, ...)                          \
        do {                                                                    \
            vsf_eda_cfg_t ATPASTE3(__,__LINE__,tCFG) = {                        \
                .fn.evthandler = (vsf_task_entry_t)vsf_task_func(__NAME),       \
                .priority = (__PRI),                                            \
                .target = NULL,                                                 \
                __VA_ARGS__                                                     \
            };                                                                  \
            prepare_vsf_task(__NAME, &((__TASK)->param));                       \
            vsf_task_start( &((__TASK)->use_as__vsf_task_t),                    \
                            &ATPASTE3(__,__LINE__,tCFG));                       \
        } while(0)
#endif
#endif
        
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#define init_vsf_task(__NAME, __TASK, __PRI)                                    \
            __init_vsf_task(__NAME, (__TASK), (__PRI))
#else
#define init_vsf_task(__NAME, __TASK, __PRI, ...)                               \
            __init_vsf_task(__NAME, (__TASK), (__PRI), __VA_ARGS__)
#endif


#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
#define vsf_task_call_fsm(__NAME, __TARGET)                                     \
            vsf_eda_call_fsm((vsf_task_entry_t)(__NAME), (uintptr_t)(__TARGET))
#define vsf_task_call_task(__NAME, __TARGET)                                    \
            vsf_eda_call_fsm(   (vsf_task_entry_t)vsf_task_func(__NAME),        \
                                (uintptr_t)(__TARGET))
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
#   define vsf_task_call_sub(__NAME, __TARGET)                                  \
            if (VSF_ERR_NONE != __vsf_eda_call_eda(                             \
                    (vsf_task_entry_t)(__NAME),                                 \
                    (__TARGET))) {                                              \
                return fsm_rt_on_going;                                         \
            }
#   define vsf_eda_call_task    vsf_task_call_task

#elif VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   define vsf_task_call_sub(__NAME, __TARGET)                                  \
            if (VSF_ERR_NONE != __vsf_eda_call_eda(                             \
                    (vsf_task_entry_t)(__NAME),                                 \
                    (__TARGET))) {                                              \
                return ;                                                        \
            }
#   define vsf_eda_call_task(__NAME, __TARGET)                                  \
        vsf_eda_call_sub((vsf_task_entry_t)vsf_task_func(__NAME), (__TARGET))
#endif


#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#define vsf_task_call_pt(__NAME, __TARGET)                                      \
            vsf_task_call_sub(vsf_pt_func(__NAME), __TARGET)
#endif

#define on_vsf_task_init()                                                      \
        if (VSF_EVT_INIT == evt)
        
#define on_vsf_task_fini()                                                      \
        if (VSF_EVT_FINI == evt)
        
#define on_vsf_task_evt(__EVT)                                                  \
        if ((__EVT) == evt)


/*! \note IMPORTENT
 *        For anything you want to wait, which is coming from vsf 
 *        system, you can use vsf_task_wait_until() optionally.
 *        Those includues: VSF_EVT_XXXXX, sempahore, mutex and etc.
 *        
 *        For VSF_EVT_XXXX, please use:
 *        vsf_task_wait_until(  on_vsf_task_evt(VSF_EVT_XXXXX) ) ;
 *        
 *
 *        For sempahore, please use:
 *        vsf_task_wait_until( vsf_sem_pend(...) ) ;
 *        
 *
 *        For time, please use:
 *        vsf_task_wait_until( vsf_delay(...) ) ;
 *        
 */ 
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
                             
#   define vsf_task_wait_until(__IF_CLAUSE)                                     \
            __IF_CLAUSE {} else {                                               \
                return fsm_rt_wait_for_evt;                                     \
            } 
            
#else
#   define vsf_task_wait_until(__IF_CLAUSE)                                     \
            __IF_CLAUSE {} else {                                               \
                return ;                                                        \
            }  
#endif
#else
#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
                             
#   define vsf_task_wait_until(...)                                             \
            __VA_ARGS__ {} else {                                               \
                return fsm_rt_wait_for_evt;                                     \
            } 
            
#else
#   define vsf_task_wait_until(...)                                             \
            __VA_ARGS__ {} else {                                               \
                return ;                                                        \
            }  
#endif
#endif
/*============================ TYPES =========================================*/

#   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
typedef vsf_teda_t  vsf_task_t;
#   else
typedef vsf_eda_t  vsf_task_t;
#   endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
typedef vsf_fsm_entry_t         vsf_task_entry_t;
#else
typedef vsf_eda_evthandler_t    vsf_task_entry_t;
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif
#endif
/* EOF */
