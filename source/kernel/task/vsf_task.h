/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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

#if VSF_USE_KERNEL == ENABLED && VSF_KERNEL_CFG_EDA_SUPPORT_TASK == ENABLED

#include "service/vsf_service.h"
#include "../vsf_eda.h"
#include "./vsf_pt.h"
#include "./vsf_fsm.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if     VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED                          \
    &&  VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == DISABLED                 \
    &&  !defined(__VSF_I_KNOWN_WHAT_WILL_HAPPEN_IF_I_DISABLE_SUBCALL_HAS_RETURN_VALUE__)
#warning \
 Although VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is enabled, but\
 VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE is disabled, hence vsf_task will be treated as\
 VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is disabled. If this is not what you wanted,\
 please set VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE to ENABLED.
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __vsf_task_func(__name)     vsf_task_func_##__name
#define vsf_task_func(__name)       __vsf_task_func(__name)

#define __vsf_task(__name)          task_cb_##__name
#define vsf_task(__name)            __vsf_task(__name)

#if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
#   define __implement_vsf_task(__name)                                         \
        fsm_rt_t vsf_task_func(__name)( uintptr_t local,                        \
                                        vsf_evt_t evt)                          \
        {                                                                       \
            vsf_task(__name) *vsf_pthis =                                       \
                *(vsf_task(__name) **)                                          \
                    ((uintptr_t)local - sizeof(uintptr_t));

#   define vsf_task_begin()

#   define vsf_task_end()                                                       \
            } return fsm_rt_on_going;

#   define vsf_task_state  (vsf_this.fsm_state)

#else
#   define __implement_vsf_task(__name)                                         \
        void vsf_task_func(__name)(__name *obj_ptr, vsf_evt_t evt)              \
        {                                                                       \
            vsf_task(__name) *vsf_pthis = &(obj_ptr->param);

#   define vsf_task_begin()
#   define vsf_task_end()           } __vsf_eda_yield();

#   define vsf_task_state  (vsf_this.fsm_state)
#endif

#define implement_vsf_task(__name)  __implement_vsf_task(__name)
#define imp_vsf_task(__name)        implement_vsf_task(__name)

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
#   define vsf_task_start           vsf_teda_start
#else
#   define vsf_task_start           vsf_eda_start
#endif

#if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
#   define __def_vsf_task(__name,...)                                           \
        struct task_cb_##__name {                                               \
            uint8_t fsm_state;                                                  \
            __VA_ARGS__                                                         \
        };                                                                      \
        struct __name {                                                         \
            implement(vsf_task_t);                                              \
            implement_ex(task_cb_##__name, param);                              \
        };                                                                      \
        extern fsm_rt_t vsf_task_func_##__name( uintptr_t local,                \
                                                vsf_evt_t evt);

#else
#   define __def_vsf_task(__name,...)                                           \
        typedef struct task_cb_##__name {                                       \
            uint8_t fsm_state;                                                  \
            __VA_ARGS__                                                         \
        } task_cb_##__name;                                                     \
        typedef struct __name {                                                 \
            implement(vsf_task_t);                                              \
            implement_ex(task_cb_##__name, param);                              \
        } __name;                                                               \
        extern void vsf_task_func_##__name(struct __name *vsf_pthis, vsf_evt_t evt);
#endif

#define def_vsf_task(__name,...)            __def_vsf_task(__name,__VA_ARGS__)
#define define_vsf_task(__name,...)         def_vsf_task(__name,__VA_ARGS__)
#define end_def_vsf_task(...)
#define end_define_vsf_task(...)

#define __declare_vsf_task(__name)                                              \
            typedef struct __name __name;                                       \
            typedef struct task_cb_##__name  task_cb_##__name;

#define declare_vsf_task(__name)        __declare_vsf_task(__name)
#define dcl_vsf_task(__name)            declare_vsf_task(__name)

#define prepare_vsf_task(__name, __task)                                        \
            do {(__task)->fsm_state = 0; } while(0)

#define prp_vsf_task(__name, __task)        prepare_vsf_task(__name, __task)

#if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
#   define __init_vsf_task(__name, __task, __pri, ...)                          \
        do {                                                                    \
            vsf_eda_cfg_t VSF_MACRO_SAFE_NAME(cfg) = {                          \
                .fn.func = (uintptr_t)vsf_task_func(__name),                    \
                .priority = (__pri),                                            \
                .target = (uintptr_t)&((__task)->param),                        \
                .feature.is_subcall_has_return_value = true,                    \
                __VA_ARGS__                                                     \
            };                                                                  \
            prepare_vsf_task(__name, &((__task)->param));                       \
            vsf_task_start(&((__task)->use_as__vsf_task_t),                     \
                &VSF_MACRO_SAFE_NAME(cfg));                                     \
        } while(0)
#else
#   define __init_vsf_task(__name, __task, __pri, ...)                          \
        do {                                                                    \
            vsf_eda_cfg_t VSF_MACRO_SAFE_NAME(cfg) = {                          \
                .fn.evthandler = (vsf_task_entry_t)vsf_task_func(__name),       \
                .priority = (__pri),                                            \
                .target = NULL,                                                 \
                __VA_ARGS__                                                     \
            };                                                                  \
            prepare_vsf_task(__name, &((__task)->param));                       \
            vsf_task_start(&((__task)->use_as__vsf_task_t),                     \
                &VSF_MACRO_SAFE_NAME(cfg));                                     \
        } while(0)
#endif

#define init_vsf_task(__name, __task, __pri, ...)                               \
            __init_vsf_task(__name, (__task), (__pri), __VA_ARGS__)


#if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
#define vsf_task_call_task(__name, __target, ...)                               \
            __vsf_eda_call_task((vsf_task_entry_t)vsf_task_func(__name),        \
                                (uintptr_t)(__target), (0, ##__VA_ARGS__))
#endif

#if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
#   define vsf_task_call_sub(__name, __target, ...)                             \
            if (VSF_ERR_NONE != __vsf_eda_call_eda(                             \
                    (vsf_task_entry_t)(__name),                                 \
                    (__target),                                                 \
                    (0, ##__VA_ARGS))) {                                        \
                return fsm_rt_on_going;                                         \
            }
#   define vsf_eda_call_task    vsf_task_call_task

#elif VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   define vsf_task_call_sub(__name, __target, ...)                             \
            if (VSF_ERR_NONE != __vsf_eda_call_eda(                             \
                    (vsf_task_entry_t)(__name),                                 \
                    (__target),                                                 \
                    (0, ##__VA_ARGS))) {                                        \
                return ;                                                        \
            }
#   define vsf_eda_call_task(__name, __target)                                  \
        vsf_eda_call_sub((vsf_task_entry_t)vsf_task_func(__name), (__target))
#endif


#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#define vsf_task_call_pt(__name, __target)                                      \
            vsf_task_call_sub(vsf_pt_func(__name), __target)
#endif

#define on_vsf_task_init()                                                      \
        if (VSF_EVT_INIT == evt)

#define on_vsf_task_fini()                                                      \
        if (VSF_EVT_FINI == evt)

#define on_vsf_task_evt(__evt)                                                  \
        if ((__evt) == evt)


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
#if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
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

/*============================ TYPES =========================================*/

#   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
typedef vsf_teda_t              vsf_task_t;
#   else
typedef vsf_eda_t               vsf_task_t;
#   endif

#if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
typedef fsm_rt_t (*vsf_task_entry_t)(uintptr_t target, vsf_evt_t evt);
#else
typedef vsf_eda_evthandler_t    vsf_task_entry_t;
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
SECTION(".text.vsf.kernel.eda_task")
extern fsm_rt_t __vsf_eda_call_task(vsf_task_entry_t entry,
                                    uintptr_t param,
                                    size_t local_size);
#   endif      // VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE
#endif      // VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL

#ifdef __cplusplus
}
#endif

#endif
#endif
/* EOF */
