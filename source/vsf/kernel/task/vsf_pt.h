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

#if VSF_KERNEL_CFG_EDA_SUPPORT_PT == ENABLED && VSF_USE_KERNEL == ENABLED
#include "../vsf_eda.h"
#include "./__vsf_task_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef __cplusplus
#   ifndef this
#       define this        (*this_ptr)
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/


#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   define __implement_vsf_pt(__name)                                           \
            __implement_vsf_pt_common(__name, uintptr_t local)                  \
            {                                                                   \
                __vsf_pt_common(__name) *this_ptr =                             \
                    *(__vsf_pt_common(__name) **)                               \
                        ((uintptr_t)local - sizeof(uintptr_t));

#   define __implement_vsf_pt_ex(__name, __func_name)                           \
            __implement_vsf_pt_common(  __func_name,  uintptr_t local)          \
            {                                                                   \
                __vsf_pt_common(__name) *this_ptr =                             \
                    *(__vsf_pt_common(__name) **)                               \
                        ((uintptr_t)local - sizeof(uintptr_t));

#elif !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L

#   define __implement_vsf_pt(__name)                                           \
            __implement_vsf_pt_common(  __name,                                 \
                                        __internal_##__name *obj_ptr) {           \
                __vsf_pt_common(__name) *this_ptr = &(obj_ptr->param);

#   define __implement_vsf_pt_ex(__name, __func_name)                           \
            __implement_vsf_pt_common(  __func_name,                            \
                                        __internal_##__name *obj_ptr) {           \
                __vsf_pt_common(__name) *this_ptr = &(obj_ptr->param);

#else

#   define __implement_vsf_pt(__name)                                           \
            __implement_vsf_pt_common(__name, __name *obj_ptr) {                  \
                __vsf_pt_common(__name) *this_ptr = &(obj_ptr->param);

#   define __implement_vsf_pt_ex(__name, __func_name)                           \
            __implement_vsf_pt_common(__func_name, __name *obj_ptr) {             \
                __vsf_pt_common(__name) *this_ptr = &(obj_ptr->param);
#endif

#define __vsf_pt_state()         (this_ptr->fsm_state)
#define __vsf_pt_end()           __vsf_pt_end_common()


#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   define __vsf_pt_entry(__state, __code)                                      \
            __vsf_pt_entry_common_ex(__state, __code)
#   define vsf_pt_entry(__code)                                                 \
            __vsf_pt_entry(__vsf_pt_state(), __code)
#else
#   define __vsf_pt_entry(__state, ...)                                         \
            __vsf_pt_entry_common(__state, __VA_ARGS__)
#   define vsf_pt_entry(...)                                                    \
            __vsf_pt_entry(__vsf_pt_state(), __VA_ARGS__)
#endif

#define __vsf_pt_begin(__state)     __vsf_pt_begin_common(__state)


#define vsf_pt_begin()              UNUSED_PARAM(evt);                          \
                                    __vsf_pt_begin(__vsf_pt_state())

#define vsf_pt_end()                __vsf_pt_end() }

#define vsf_pt_wait_for_evt(__evt)  __vsf_pt_wfe_common(__vsf_pt_state(), __evt)
#define vsf_pt_wfe(__evt)           vsf_pt_wait_for_evt(__evt)

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
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   define vsf_pt_wait_until(__if_clause)                                       \
            vsf_pt_entry();                                                     \
            __if_clause {} else {                                               \
                return ;                                                        \
            }
#else
#   define vsf_pt_wait_until(...)                                               \
            vsf_pt_entry();                                                     \
            __VA_ARGS__ {} else {                                               \
                return ;                                                        \
            }
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   define __vsf_pt_call_sub(__name, __target, ...)                             \
            __vsf_eda_call_eda( (uintptr_t)(__name),                            \
                                (uintptr_t)(__target),                          \
                                (0, ##__VA_ARGS__))


#   define vsf_pt_call_sub(__name, __target)                                    \
        vsf_pt_entry();                                                         \
        if (VSF_ERR_NONE != __vsf_pt_call_sub(__name, (__target))) {            \
            return ;                                                            \
        }                                                                       \
        vsf_pt_entry(return;);


#   define vsf_pt_call_pt(__name, __target)                                     \
            (__target)->fsm_state = 0;                                          \
            vsf_pt_call_sub(vsf_pt_func(__name), (__target))

#endif

#define vsf_eda_call_pt(__name, __target)                                       \
            __vsf_pt_call_sub(vsf_pt_func(__name), (__target))

#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
#   define __vsf_pt_call_fsm(__name, __target, ...)                             \
            __vsf_eda_call_fsm( (vsf_fsm_entry_t)(__name),                      \
                                (uintptr_t)(__target),                          \
                                (0, ##__VA_ARGS__))


#   define vsf_pt_call_fsm(__name, __target, __ret_addr, ...)                   \
        do {                                                                    \
            fsm_rt_t CONNECT3(__vsf_pt_call_fsm,__LINE__,tReturn);              \
            vsf_pt_entry();                                                     \
            CONNECT3(__vsf_pt_call_fsm,__LINE__,tReturn) =                      \
                __vsf_pt_call_fsm(__name, (__target), (0, ##__VA_ARGS__));      \
            if (fsm_rt_on_going ==                                              \
                CONNECT3(__vsf_pt_call_fsm,__LINE__,tReturn)) {                 \
                return ;                                                        \
            }                                                                   \
            if (NULL != (__ret_addr)) {                                         \
                *(__ret_addr) = CONNECT3(__vsf_pt_call_fsm,__LINE__,tReturn);   \
            }                                                                   \
        } while(0)

#   define vsf_pt_call_task(__name, __target, __ret_addr, ...)                  \
        vsf_pt_call_fsm(vsf_task_func(__name), __target, __ret_addr, (0, ##__VA_ARGS__))
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
#   define vsf_pt_start                 vsf_teda_start
#else
#   define vsf_pt_start                 vsf_eda_start
#endif



#define implement_vsf_pt(__name)        __implement_vsf_pt(__name)
#define implement_vsf_pt_ex(__name, __FUNC_NAME)                                \
            __implement_vsf_pt_ex(__name, __FUNC_NAME)

#define imp_vsf_pt(__name)              implement_vsf_pt(__name)
#define imp_vsf_pt_ex(__name, __FUNC_NAME)                                      \
            implement_vsf_pt_ex(__name, __FUNC_NAME)

#define __vsf_pt_func(__name)           __vsf_pt_func_common(__name)
#define vsf_pt_func(__name)             __vsf_pt_func(__name)

#define __vsf_pt(__name)                __vsf_pt_common(__name)
#define vsf_pt(__name)                  __vsf_pt(__name)

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L

#   if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
#       define __def_vsf_pt(__name,__member)                                    \
            __def_vsf_pt_common(__name,                                         \
                                uint16_t fsm_state;                             \
                                vsf_sync_reason_t reason;                       \
                                __member)
#   else
#       define __def_vsf_pt(__name,__member)                                    \
            __def_vsf_pt_common(__name,                                         \
                                uint16_t fsm_state;                             \
                                __member)
#   endif

#   define def_vsf_pt(__name,__member)      __def_vsf_pt(__name, __member)
#   define end_def_vsf_pt(__name)
#   define define_vsf_pt(__name,__member)   def_vsf_pt(__name,__member)
#   define end_define_vsf_pt(__name)
#else
#   define __def_vsf_pt(__name,...)                                             \
            __def_vsf_pt_common(__name,                                         \
                                uint8_t fsm_state;                                 \
                                __VA_ARGS__)

#   define def_vsf_pt(__name,...)       __def_vsf_pt(__name,__VA_ARGS__)
#   define end_def_vsf_pt(...)
#   define define_vsf_pt(__name,...)    def_vsf_pt(__name,__VA_ARGS__)
#   define end_define_vsf_pt(...)
#endif



#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   define __declare_vsf_pt(__name)                                             \
            __declare_vsf_pt_common(__name)                                     \
            __extern_vsf_pt_common(__name, uintptr_t local)
#elif !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   define __declare_vsf_pt(__name)                                             \
            __declare_vsf_pt_common(__name)                                     \
            __extern_vsf_pt_common(__name, __internal_##__name *this_ptr)
#else
#   define __declare_vsf_pt(__name)                                             \
            __declare_vsf_pt_common(__name)                                     \
            __extern_vsf_pt_common(__name, __name *this_ptr)
#endif

#define declare_vsf_pt(__name)          __declare_vsf_pt(__name)
#define dcl_vsf_pt(__name)              declare_vsf_pt(__name)

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#define __init_vsf_pt(__name, __pt, __pri)                                      \
        do {                                                                    \
            vsf_eda_cfg_t CONNECT3(__,__LINE__,cfg) = {0};                      \
            CONNECT3(__,__LINE__,cfg).fn.evthandler =                           \
                (vsf_pt_entry_t)__vsf_pt_func(__name);                          \
            CONNECT3(__,__LINE__,cfg).priority = (__pri);                       \
            CONNECT3(__,__LINE__,cfg).target = (uintptr_t)&((__pt)->param);     \
            (__pt)->param.fsm_state = 0;                                        \
            vsf_pt_start( &((__pt)->use_as__vsf_pt_t),                          \
                            &CONNECT3(__,__LINE__,cfg));                        \
        } while(0)
#else
#define __init_vsf_pt(__name, __pt, __pri, ...)                                 \
        do {                                                                    \
            vsf_eda_cfg_t CONNECT3(__,__LINE__,cfg) = {                         \
                .fn.evthandler = (vsf_pt_entry_t)__vsf_pt_func(__name),         \
                .priority = (__pri),                                            \
                .target = (uintptr_t)&((__pt)->param),                          \
                __VA_ARGS__                                                     \
            };                                                                  \
            (__pt)->param.fsm_state = 0;                                        \
            vsf_pt_start( &((__pt)->use_as__vsf_pt_t),                          \
                            &CONNECT3(__,__LINE__,cfg));                        \
        } while(0)
#endif



#if     VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED                                \
    &&  VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   define vsf_pt_call_thread(__name, __target)                                 \
        vsf_eda_call_thread_prepare(__name, __target);                          \
        vsf_pt_entry();                                                         \
        if (VSF_ERR_NONE != vsf_eda_call_thread(__target)) {                    \
            return ;                                                            \
        }                                                                       \
        vsf_pt_entry(return;);
#endif

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#define init_vsf_pt(__name, __pt, __pri)                                        \
            __init_vsf_pt(__name, __pt, __pri)
#else
#define init_vsf_pt(__name, __pt, __pri, ...)                                   \
            __init_vsf_pt(__name, __pt, __pri, __VA_ARGS__)
#endif
/*============================ TYPES =========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
typedef vsf_teda_t  vsf_pt_t;
#else
typedef vsf_eda_t  vsf_pt_t;
#endif

typedef vsf_eda_evthandler_t    vsf_pt_entry_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
#endif
