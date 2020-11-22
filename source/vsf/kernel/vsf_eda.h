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
#ifndef __VSF_EDA_H__
#define __VSF_EDA_H__

/*============================ INCLUDES ======================================*/
#include "kernel/vsf_kernel_cfg.h"

#if VSF_USE_KERNEL == ENABLED
#include "hal/arch/vsf_arch.h"
#include "service/vsf_service.h"

/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file
 *!        included in this file
 */

#if     defined(__VSF_EDA_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__VSF_EDA_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define VSF_SYNC_AUTO_RST               0x0000
#define VSF_SYNC_MANUAL_RST             0x8000
#define VSF_SYNC_HAS_OWNER              0x8000

#define VSF_SYNC_MAX                    0x7FFF

/*============================ MACROFIED FUNCTIONS ===========================*/

// SEMAPHORE
#define vsf_eda_sem_init(__psem, __cnt)                                         \
            vsf_eda_sync_init((__psem), (__cnt), VSF_SYNC_MAX | VSF_SYNC_AUTO_RST)
#define vsf_eda_sem_post(__psem)            vsf_eda_sync_increase((__psem))
#define vsf_eda_sem_pend(__psem, __timeout) vsf_eda_sync_decrease((__psem), (__timeout))
#if VSF_KERNEL_CFG_SUPPORT_SYNC_IRQ == ENABLED
#   define vsf_eda_sem_post_irq(__psem)     vsf_eda_sync_increase_irq((__psem))
#endif

// MUTEX
#define vsf_eda_mutex_init(__pmtx)                                              \
            vsf_eda_sync_init(  &((__pmtx)->use_as__vsf_sync_t),                \
                                1 | VSF_SYNC_HAS_OWNER,                         \
                                1 | VSF_SYNC_AUTO_RST)
#define vsf_eda_mutex_try_to_enter(__pmtx, __timeout)                           \
            vsf_eda_sync_decrease(&((__pmtx)->use_as__vsf_sync_t), (__timeout))

#define vsf_eda_mutex_enter(__pmtx)                                             \
            vsf_eda_sync_decrease(&((__pmtx)->use_as__vsf_sync_t), -1)

#define vsf_eda_mutex_leave(__pmtx)                                             \
            vsf_eda_sync_increase(&((__pmtx)->use_as__vsf_sync_t))

// CRIT
#define vsf_eda_crit_init(__pcrit)                                              \
            vsf_eda_mutex_init((__pcrit))

#define vsf_eda_crit_try_to_enter(__pcrit, __timeout)                           \
            vsf_eda_mutex_try_to_enter((__pcrit), (__timeout))

#define vsf_eda_crit_enter(__pcrit)                                             \
            vsf_eda_mutex_enter((__pcrit))

#define vsf_eda_crit_leave(__pcrit)                                             \
            vsf_eda_mutex_leave((__pcrit))

// EVENT
#define vsf_eda_trig_init(__pevt, __set, __auto_rst)                            \
            vsf_eda_sync_init(                                                  \
                    (__pevt),                                                   \
                    (__set),                                                    \
                    1 | ((__auto_rst) ? VSF_SYNC_AUTO_RST : VSF_SYNC_MANUAL_RST))
#define vsf_eda_trig_set(__pevt)            vsf_eda_sync_increase((__pevt))
#define vsf_eda_trig_reset(__pevt)          vsf_eda_sync_force_reset((__pevt))
#define vsf_eda_trig_wait(__pevt, __timeout)                                    \
            vsf_eda_sync_decrease((__pevt), (__timeout))
#if VSF_KERNEL_CFG_SUPPORT_SYNC_IRQ == ENABLED
#   define vsf_eda_trig_set_irq(__pevt)     vsf_eda_sync_increase_irq((__pevt))
#endif

// CRIT without priority boost, internal use only
// only used for edas with same priority
#define __vsf_eda_crit_npb_init(__pcrit)                                        \
            vsf_eda_sync_init(  (__pcrit),                                      \
                                1,                                              \
                                1 | VSF_SYNC_AUTO_RST)

#define __vsf_eda_crit_npb_try_to_enter(__pcrit, __timeout)                     \
            vsf_eda_sync_decrease((__pcrit), (__timeout))

#define __vsf_eda_crit_npb_enter(__pcrit)                                       \
            vsf_eda_sync_decrease((__pcrit), -1)

#define __vsf_eda_crit_npb_leave(__pcrit)                                       \
            vsf_eda_sync_increase((__pcrit))


#   define vsf_eda_return(...)  __vsf_eda_return((uintptr_t)(0, ##__VA_ARGS__))

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
#   define vsf_systimer_get_ms()            vsf_systimer_tick_to_ms(vsf_systimer_get_tick())
#   define vsf_systimer_get_us()            vsf_systimer_tick_to_us(vsf_systimer_get_tick())
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   define vsf_eda_call_eda(__evthandler, ...)                                  \
            __vsf_eda_call_eda((uintptr_t)__evthandler, NULL, (0, ##__VA_ARGS))
#   define vsf_eda_call_param_eda(__param_evthandler, __param, ...)             \
            __vsf_eda_call_eda( (uintptr_t)__param_evthandler,                  \
                                (uintptr_t)__param,                             \
                                (0, ##__VA_ARGS__))

#   define vsf_eda_get_local(...)                                               \
            __vsf_eda_get_local((vsf_eda_t *)(vsf_eda_get_cur(), ##__VA_ARGS__))

#   define __vsf_peda_local(__name)     peda_local_##__name
#   define vsf_peda_local(__name)       __vsf_peda_local(__name)

#   define __vsf_peda_arg(__name)       peda_arg_##__name
#   define vsf_peda_arg(__name)         __vsf_peda_arg(__name)

#   define __vsf_peda_func(__name)      vsf_peda_func_##__name
#   define vsf_peda_func(__name)        __vsf_peda_func(__name)


#   define __vsf_peda_param(__name)     peda_cb_##__name
#   define vsf_peda_param(__name)       __vsf_peda_param(__name)


#   define __declare_vsf_peda_ctx(__name)                                       \
            typedef struct vsf_peda_param(__name)   vsf_peda_param(__name);     \
            typedef struct vsf_peda_arg(__name)     vsf_peda_arg(__name);       \
            typedef struct vsf_peda_local(__name)   vsf_peda_local(__name);
#   define declare_vsf_peda_ctx(__name)     __declare_vsf_peda_ctx(__name)

#   define dcl_vsf_peda_ctx(__name)                                             \
            declare_vsf_peda_ctx(__name)

#   define __declare_vsf_peda(__name)                                           \
            typedef struct __name __name;                                       \
            __declare_vsf_peda_ctx(__name)
#   define declare_vsf_peda(__name)     __declare_vsf_peda(__name)

#   define dcl_vsf_peda(__name)                                                 \
            declare_vsf_peda(__name)

#   define declare_vsf_peda_methods1(__decoration, __name)                      \
            declare_vsf_peda_ctx(__name)                                        \
            __decoration                                                        \
                         void vsf_peda_func(__name)(                            \
                                        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);

#   define declare_vsf_peda_methods2(__decoration, __name,  __func1)            \
            declare_vsf_peda_ctx(__name)                                        \
            __decoration                                                        \
                   void vsf_peda_func(__name)(                                  \
                                        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func1(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);

#   define declare_vsf_peda_methods3(__decoration, __name, __func1, __func2)    \
            declare_vsf_peda_ctx(__name)                                        \
            __decoration                                                        \
                   void vsf_peda_func(__name)(                                  \
                                        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func1(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func2(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);

#   define declare_vsf_peda_methods4(__name, __func1, __func2, __func3)         \
            declare_vsf_peda_ctx(__name)                                        \
            __decoration                                                        \
                   void vsf_peda_func(__name)(                                  \
                                        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func1(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func2(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func3(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);

#   define declare_vsf_peda_methods5(   __decoration, __name, __func1, __func2, \
                                        __func3, __func4)                       \
            declare_vsf_peda_ctx(__name)                                        \
            __decoration                                                        \
                   void vsf_peda_func(__name)(                                  \
                                        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func1(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func2(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func3(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func4(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);


#   define declare_vsf_peda_methods6(   __decoration, __name, __func1, __func2, \
                                        __func3, __func4, __func5)              \
            declare_vsf_peda_ctx(__name)                                        \
            __decoration                                                        \
                   void vsf_peda_func(__name)(                                  \
                                        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func1(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func2(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func3(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func4(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func5(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);

#   define declare_vsf_peda_methods7(   __decoration, __name, __func1, __func2, \
                                        __func3, __func4, __func5, __func6)     \
            declare_vsf_peda_ctx(__name)                                        \
            __decoration                                                        \
                   void vsf_peda_func(__name)(                                  \
                                        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func1(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func2(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func3(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func4(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func5(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func6(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);

#   define declare_vsf_peda_methods8(   __decoration, __name, __func1, __func2, \
                                        __func3, __func4, __func5, __func6,     \
                                        __func7)                                \
            declare_vsf_peda_ctx(__name)                                        \
            __decoration                                                        \
                   void vsf_peda_func(__name)(                                  \
                                        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func1(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func2(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func3(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func4(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func5(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func6(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func7(        struct vsf_peda_local(__name) *this_ptr,\
                                        vsf_evt_t evt);

#define declare_vsf_peda_methods(__decoration, ...)                             \
            __PLOOC_EVAL(declare_vsf_peda_methods, __VA_ARGS__)                 \
                (__decoration, __VA_ARGS__)

#define dcl_vsf_peda_methods(__decoration, ...)                                 \
            declare_vsf_peda_methods(__decoration, __VA_ARGS__)

#   if __IS_COMPILER_IAR__
#       define __def_vsf_peda_ctx4(__name, __param, __arg, __local)             \
        struct vsf_peda_param(__name) {                                         \
            __param                                                             \
            uint8_t PLOOC_CONNECT4(_,__LINE__,__COUNTER__,_canary);             \
        };                                                                      \
        struct vsf_peda_arg(__name) {                                           \
            __arg                                                               \
            uint8_t PLOOC_CONNECT4(_,__LINE__,__COUNTER__,_canary);             \
        };                                                                      \
        struct vsf_peda_local(__name) {                                         \
            implement(vsf_peda_arg(__name))                                     \
            __local                                                             \
        };
#   else
#       define __def_vsf_peda_ctx4(__name, __param, __arg, __local)             \
        struct vsf_peda_param(__name) {                                         \
            __param                                                             \
        };                                                                      \
        struct vsf_peda_arg(__name) {                                           \
            __arg                                                               \
        };                                                                      \
        struct vsf_peda_local(__name) {                                         \
            implement(vsf_peda_arg(__name))                                     \
            __local                                                             \
        };
#endif

#   define __def_vsf_peda4(__name, __param, __arg, __local)                     \
        __def_vsf_peda_ctx4(__name, __param, __arg, __local)                    \
        struct __name {                                                         \
            implement(vsf_peda_t)                                               \
            implement_ex(vsf_peda_param(__name), param)                         \
        };

#   if __IS_COMPILER_IAR__
#       define __def_vsf_peda_ctx3(__name, __param, __arg)                      \
        struct vsf_peda_param(__name) {                                         \
            __param                                                             \
            uint8_t PLOOC_CONNECT4(_,__LINE__,__COUNTER__,_canary);             \
        };                                                                      \
        struct vsf_peda_arg(__name) {                                           \
            __arg                                                               \
            uint8_t PLOOC_CONNECT4(_,__LINE__,__COUNTER__,_canary);             \
        };                                                                      \
        struct vsf_peda_local(__name) {                                         \
            implement(vsf_peda_arg(__name))                                     \
        };
#   else
#       define __def_vsf_peda_ctx3(__name, __param, __arg)                      \
        struct vsf_peda_param(__name) {                                         \
            __param                                                             \
        };                                                                      \
        struct vsf_peda_arg(__name) {                                           \
            __arg                                                               \
        };                                                                      \
        struct vsf_peda_local(__name) {                                         \
            implement(vsf_peda_arg(__name))                                     \
        };
#endif

#   define __def_vsf_peda3(__name, __param, __arg)                              \
        __def_vsf_peda_ctx3(__name, __param, __arg)                             \
        struct __name {                                                         \
            implement(vsf_peda_t)                                               \
            implement_ex(vsf_peda_param(__name), param)                         \
        };


#   if __IS_COMPILER_IAR__
#       define __def_vsf_peda_ctx2(__name, __param)                             \
        struct vsf_peda_param(__name) {                                         \
            __param                                                             \
            uint8_t PLOOC_CONNECT4(_,__LINE__,__COUNTER__,_canary);             \
        };                                                                      \
        struct vsf_peda_arg(__name) {                                           \
            uint8_t PLOOC_CONNECT4(_,__LINE__,__COUNTER__,_canary);             \
        };                                                                      \
        struct vsf_peda_local(__name) {                                         \
            implement(vsf_peda_arg(__name))                                     \
        };
#   else
#       define __def_vsf_peda_ctx2(__name, __param)                             \
        struct vsf_peda_param(__name) {                                         \
            __param                                                             \
        };                                                                      \
        struct vsf_peda_arg(__name) {                                           \
        };                                                                      \
        struct vsf_peda_local(__name) {                                         \
            implement(vsf_peda_arg(__name))                                     \
        };
#endif

#   define __def_vsf_peda_ctx1(__name)                                          \
        __def_vsf_peda_ctx2(__name, )

#   define __def_vsf_peda2(__name, __param)                                     \
        __def_vsf_peda_ctx2(__name, __param)                                    \
        struct __name {                                                         \
            implement(vsf_peda_t)                                               \
            implement_ex(vsf_peda_param(__name), param)                         \
        };

#   define __def_vsf_peda1(__name)                                              \
        __def_vsf_peda_ctx1(__name)                                             \
        struct __name {                                                         \
            implement(vsf_peda_t)                                               \
            implement_ex(vsf_peda_param(__name), param)                         \
        };

#   define def_vsf_peda(...)                                                    \
                __PLOOC_EVAL(__def_vsf_peda, __VA_ARGS__) (__VA_ARGS__)

#   define end_def_vsf_peda(...)

#   define def_vsf_peda_ctx(...)                                                \
                __PLOOC_EVAL(__def_vsf_peda_ctx, __VA_ARGS__)(__VA_ARGS__)

#   define end_def_vsf_peda_ctx(...)

#   define define_vsf_peda_ctx(__name, ...)                                     \
                def_vsf_peda_ctx(__name, __VA_ARGS__)

#   define end_define_vsf_peda_ctx(...)

#   define def_locals(...)              ,##__VA_ARGS__
#   define end_def_locals(...)

#   define define_locals(...)           ,##__VA_ARGS__
#   define end_define_locals(...)

#   define def_args(...)                ,__VA_ARGS__
#   define end_def_args(...)

#   define define_args(...)             ,__VA_ARGS__
#   define end_define_args(...)

#   define define_arguments(...)        ,__VA_ARGS__
#   define end_define_arguments(...)

#   define define_parameters(...)       __VA_ARGS__
#   define end_define_parameters(...)

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
#   define vsf_peda_start          vsf_teda_start
#else
#   define vsf_peda_start          vsf_eda_start
#endif

#   define __init_vsf_peda(__name, __param_eda, __pri, ...)                     \
        do {                                                                    \
            vsf_eda_cfg_t CONNECT3(__,__LINE__,cfg) = {                         \
                .fn.param_evthandler =                                          \
                    (vsf_param_eda_evthandler_t)vsf_peda_func(__name),          \
                .priority = (__pri),                                            \
                .target = (uintptr_t)&((__param_eda)->param),                   \
                .local_size = sizeof(vsf_peda_local(__name)),                   \
                __VA_ARGS__                                                     \
            };                                                                  \
            vsf_peda_start((vsf_peda_t *)(__param_eda),                         \
                            &CONNECT3(__,__LINE__,cfg));                        \
        } while(0)

#   define init_vsf_peda(__name, __param_eda, __pri, ...)                       \
            __init_vsf_peda(__name, (__param_eda), (__pri), __VA_ARGS__)


#   define __implement_vsf_peda2(__name, __func_name)                           \
        void __func_name(   struct vsf_peda_local(__name) *local_ptr,           \
                            vsf_evt_t evt)                                      \
        {                                                                       \
            vsf_peda_param(__name) *this_ptr =                                  \
                *(vsf_peda_param(__name) **)                                    \
                    ((uintptr_t)local_ptr - sizeof(uintptr_t));                 \
            ASSERT(NULL != this_ptr || NULL != local_ptr);

#   define __implement_vsf_peda1(__name)                                        \
        void vsf_peda_func(__name)( struct vsf_peda_local(__name) *local_ptr,   \
                                    vsf_evt_t evt)                              \
        {                                                                       \
            vsf_peda_param(__name) *this_ptr =                                  \
                *(vsf_peda_param(__name) **)                                    \
                    ((uintptr_t)local_ptr - sizeof(uintptr_t));                 \
            ASSERT(NULL != this_ptr || NULL != local_ptr);

#   define vsf_peda_begin()

#   define vsf_peda_end()                                                       \
            }

#   define implement_vsf_peda(...)                                              \
                __PLOOC_EVAL(__implement_vsf_peda, __VA_ARGS__)(__VA_ARGS__)

#   define imp_vsf_peda(...)                                                    \
                implement_vsf_peda(__VA_ARGS__)

#   define vsf_eda_call_peda(__name, __param)                                   \
                vsf_eda_call_param_eda( vsf_peda_func(__name),                  \
                                        (__param),                              \
                                        sizeof(vsf_peda_local(__name)))

#   define vsf_local        (*local_ptr)

#endif
#   define vsf_this         (*this_ptr)

/*============================ TYPES =========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
typedef vsf_systimer_cnt_t     vsf_timer_tick_t;
#endif

enum {
    /*!\ note wait for invalid also means wait for any evt */
    VSF_EVT_INVALID             = -1,       //!< compatible with fsm_rt_err
    VSF_EVT_NONE                = 0,        //!< compatible with fsm_rt_cpl
    VSF_EVT_YIELD               = 1,        //!< compatible with fsm_rt_on_going

    VSF_EVT_SYSTEM              = 0x100,
    VSF_EVT_DUMMY               = VSF_EVT_SYSTEM + 0,
    VSF_EVT_INIT                = VSF_EVT_SYSTEM + 1,
    VSF_EVT_FINI                = VSF_EVT_SYSTEM + 2,
    VSF_EVT_ENTER               = VSF_EVT_SYSTEM + 3,
    VSF_EVT_EXIT                = VSF_EVT_SYSTEM + 4,
    VSF_EVT_RETURN              = VSF_EVT_EXIT,

    // events for time
    VSF_EVT_TIMER               = VSF_EVT_SYSTEM + 5,

    // events for sync
    VSF_EVT_SYNC                = VSF_EVT_SYSTEM + 6,
    VSF_EVT_SYNC_CANCEL         = VSF_EVT_SYSTEM + 7,
    VSF_EVT_SYNC_POLL           = VSF_EVT_SYSTEM + 8,

    // events for message
    VSF_EVT_MESSAGE             = VSF_EVT_SYSTEM + 9,
    VSF_EVT_USER                = VSF_EVT_SYSTEM + 10,
};

dcl_simple_class(vsf_eda_t)
dcl_simple_class(vsf_teda_t)
dcl_simple_class(vsf_sync_t)
dcl_simple_class(vsf_sync_owner_t)
dcl_simple_class(vsf_bmpevt_t)
dcl_simple_class(vsf_bmpevt_pender_t)
dcl_simple_class(vsf_bmpevt_adapter_t)
dcl_simple_class(vsf_bmpevt_adapter_eda_t)
dcl_simple_class(vsf_eda_queue_t)
dcl_simple_class(vsf_callback_timer_t)

typedef int16_t vsf_evt_t;

typedef void (*vsf_eda_evthandler_t)(vsf_eda_t *eda, vsf_evt_t evt);
typedef void (*vsf_eda_on_terminate_t)(vsf_eda_t *eda);
typedef fsm_rt_t (*vsf_fsm_entry_t)(uintptr_t target, vsf_evt_t evt);
typedef void (*vsf_param_eda_evthandler_t)(uintptr_t target, vsf_evt_t evt);

#if VSF_KERNEL_CFG_FRAME_USER_BITS <= 6
#   define __VSF_KERNEL_CFG_FRAME_UINT_TYPE         uint8_t
#elif VSF_KERNEL_CFG_FRAME_USER_BITS <= 14
#   define __VSF_KERNEL_CFG_FRAME_UINT_TYPE         uint16_t
#elif VSF_KERNEL_CFG_FRAME_USER_BITS <= 30
#   define __VSF_KERNEL_CFG_FRAME_UINT_TYPE         uint32_t
#else
#   error  The number of user bits should not exceed 30!!! Please give the macro VSF_KERNEL_CFG_FRAME_USER_BITS a correct value.
#endif

typedef __VSF_KERNEL_CFG_FRAME_UINT_TYPE  __vsf_frame_uint_t;

typedef struct __vsf_eda_frame_state_t {
    union {
        struct {
            __vsf_frame_uint_t is_fsm           : 1;
            __vsf_frame_uint_t is_stack_owner   : 1;
            __vsf_frame_uint_t user             : VSF_KERNEL_CFG_FRAME_USER_BITS;
        } bits;
        __vsf_frame_uint_t                      flag;
    };
    uint16_t local_size;
} __vsf_eda_frame_state_t;

dcl_simple_class(__vsf_eda_frame_t)
def_simple_class(__vsf_eda_frame_t) {
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
    protected_member (
        implement(vsf_slist_node_t)
        union {
            uintptr_t                       func;
            vsf_eda_evthandler_t            evthandler;
            vsf_param_eda_evthandler_t      param_evthandler;
            vsf_fsm_entry_t                 fsm_entry;
        } fn;

#   if  VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED                               \
    ||  VSF_KERNEL_USE_SIMPLE_SHELL == ENABLED
        protected_member (
            __vsf_eda_frame_state_t             state;
        )
#   endif

        union {
            uintptr_t param;
            uintptr_t target;
        } ptr;
    )


#else
    protected_member (
        implement(vsf_slist_node_t)
        union {
            uintptr_t                       func;
            vsf_eda_evthandler_t            evthandler;
            vsf_param_eda_evthandler_t      param_evthandler;
            vsf_fsm_entry_t                 fsm_entry;
        } fn;

#if     VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED                               \
    ||  VSF_KERNEL_USE_SIMPLE_SHELL == ENABLED
        __vsf_eda_frame_state_t             state;
#endif

        union {
            uintptr_t param;
            uintptr_t target;
        } ptr;

    )
#endif
};

typedef struct vsf_eda_cfg_t {
    union {
        uintptr_t                   func;
        vsf_eda_evthandler_t        evthandler;
        vsf_param_eda_evthandler_t  param_evthandler;
        vsf_fsm_entry_t             fsm_entry;
    } fn;
    vsf_prio_t                      priority;
    uintptr_t                       target;
    vsf_eda_on_terminate_t          on_terminate;
    bool                            is_fsm;
    bool                            is_stack_owner;
    uint16_t                        local_size;
} vsf_eda_cfg_t;

typedef union __vsf_eda_state_t {
    struct {
#if VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED == ENABLED
#   if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED
        uint8_t         is_ready        : 1;
        uint8_t         is_new_prio     : 1;
#   endif
        uint8_t         is_to_exit      : 1;
#else
        uint8_t         is_processing   : 1;
#endif

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
    /* if is_limitted, eda can only receive 1 event */
        uint8_t         is_limitted     : 1;
        uint8_t         is_sync_got     : 1;
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
        uint8_t         is_timed        : 1;
#endif
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
        uint8_t         is_use_frame    : 1;
#   if      VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED                           \
        ||  VSF_KERNEL_CFG_EDA_SUPPORT_PT == ENABLED
        uint8_t         is_evt_incoming : 1;
#   endif
#endif

#if VSF_KERNEL_USE_SIMPLE_SHELL == ENABLED
        uint8_t         polling_state   : 1;
        uint8_t         is_stack_owner  : 1;
#endif
    } bits;
    // TODO: flag is not always 16-bit here
    uint16_t            flag;
} __vsf_eda_state_t;

//! \name eda
//! @{
def_simple_class(vsf_eda_t) {

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    protected_member(
        vsf_eda_on_terminate_t      on_terminate;
    )
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    protected_member(

        union {
            vsf_eda_evthandler_t    evthandler;
            vsf_slist_t             frame_list;
            __vsf_eda_frame_t       *frame;
        } fn;
        uintptr_t                   return_value;
    )
#else
    protected_member(
        union {
            vsf_eda_evthandler_t    evthandler;
        } fn;
    )
#endif

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
    protected_member(
        vsf_dlist_node_t    pending_node;
    )
#   endif

#   if VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED == ENABLED
#       if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED
    protected_member(
        vsf_dlist_node_t    rdy_node;
        vsf_slist_queue_t   evt_list;
        uint8_t             cur_priority;
        uint8_t             new_priority;
        uint8_t             priority;
    )
#       else
    protected_member(
        uint8_t             evt_cnt;
        uint8_t             priority;
    )
#       endif
#   else
    protected_member(
        uintptr_t           evt_pending;
    )
#   endif

#   if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
    protected_member(
        /* value holder for enum fsm_rt_t */
        int8_t              fsm_return_state;
    )
#   endif

    protected_member(
        __vsf_eda_state_t   state;
    )

#else
    protected_member(
#   if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
        vsf_dlist_node_t    pending_node;
#   endif

#   if VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED == ENABLED
#       if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED
        vsf_dlist_node_t    rdy_node;
        vsf_slist_queue_t   evt_list;
        uint8_t             cur_priority;
        uint8_t             new_priority;
        uint8_t             priority;
#       else
        uint8_t             evt_cnt;
        uint8_t             priority;
#       endif
#   else
        uintptr_t           evt_pending;
#   endif

#   if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
        /* value holder for enum fsm_rt_t */
        int8_t              fsm_return_state;
#   endif
        __vsf_eda_state_t   state;
    )
#endif
};
//! @}

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
//! \name timed eda
//! @{
def_simple_class(vsf_teda_t)  {
    which(
        implement(vsf_eda_t)
    )
    private_member(
        vsf_dlist_node_t    timer_node;
        vsf_timer_tick_t    due;
    )
};
//! @}

#if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
//! \name callback_timer
//! @{
def_simple_class(vsf_callback_timer_t) {
    public_member(
        void (*on_timer)(vsf_callback_timer_t *timer);
    )
    private_member(
        vsf_dlist_node_t timer_node;
        vsf_timer_tick_t due;
    )
};
//! @}
#endif
#endif

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
//! \name sync
//! @{
def_simple_class(vsf_sync_t) {

    protected_member(
        union {
            struct {
                uint16_t    cur         : 15;
                uint16_t    has_owner   : 1;
            } bits;
            uint16_t        cur_value;
        } cur_union;
        union {
            struct {
                uint16_t    max         : 15;
                uint16_t    manual_rst  : 1;
            } bits;
            uint16_t        max_value;
        } max_union;

        vsf_dlist_t         pending_list;
    )
};
//! @}

//! \name sync_with_owner
//! @{
def_simple_class(vsf_sync_owner_t) {
    which(
        implement(vsf_sync_t)
    )
    private_member(
        vsf_eda_t           *eda_owner;
    )
};
//! @}

#ifndef __VSF_BITMAP_EVT_DEFINED__
#define __VSF_BITMAP_EVT_DEFINED__

#define VSF_BMPEVT_OR               0
#define VSF_BMPEVT_AND              1

typedef struct vsf_bmpevt_adapter_op_t {
    vsf_err_t (*init)(vsf_bmpevt_adapter_t *this_ptr);
    vsf_err_t (*reset)(vsf_bmpevt_adapter_t *this_ptr);
} vsf_bmpevt_adapter_op_t;
#endif

//! \name bmpevt_adapter
//! @{
def_simple_class(vsf_bmpevt_adapter_t) {

    public_member (
        const vsf_bmpevt_adapter_op_t   *op;
        const uint32_t                  mask;
    )
    private_member(
        vsf_bmpevt_t                    *bmpevt_host;
    )
};
//! @}

//! \name bmpevt_adapter_eda
//! @{
def_simple_class(vsf_bmpevt_adapter_eda_t) {
    which(
        implement(vsf_bmpevt_adapter_t)
    )
    private_member(
        vsf_eda_t           eda;
    )
};
//! @}

//! \name bmpevt_pender
//! @{
def_simple_class(vsf_bmpevt_pender_t) {

    public_member (
        uint32_t        mask;
        uint8_t         op  : 1;
    )

    private_member(
        vsf_eda_t       *eda_pending;
    )
};
//! @}

//! \name bmpevt
//! @{
def_simple_class(vsf_bmpevt_t) {

    public_member (
        uint32_t                auto_reset;
        vsf_bmpevt_adapter_t    **adapters;
    )

    private_member(
        vsf_dlist_t             pending_list;
        uint32_t                value;
        uint32_t                cancelled_value;
    )

    private_member(
        union {
            struct {
                uint8_t         adapter_count   : 5;
                uint8_t         is_cancelling   : 1;
                uint8_t         is_polling      : 1;
                uint8_t         is_to_repoll    : 1;
            } bits;
            uint8_t             flag;
        } state;
    )
};
//! @}

#if __VSF_KERNEL_CFG_SUPPORT_GENERIC_QUEUE == ENABLED
typedef struct vsf_eda_queue_op_t {
    bool (*enqueue)(vsf_eda_queue_t *this_ptr, void *node);
    bool (*dequeue)(vsf_eda_queue_t *this_ptr, void **node);
} vsf_eda_queue_op_t;

//! \brief define alias for vsf_eda_queue_t. osa means os-aware
typedef struct vsf_eda_queue_t vsf_osa_queue_t;
typedef struct vsf_eda_queue_op_t vsf_osa_queue_op_t;

//! \name queue
//! @{
def_simple_class(vsf_eda_queue_t) {
    union {
        implement(vsf_sync_t)
#if VSF_KERNEL_CFG_QUEUE_MULTI_TX_EN == ENABLED

        protected_member(
            union {
                uint16_t        __cur_value;
            };
            union {
                struct {
                    uint16_t    __max         : 15;
                    uint16_t    tx_processing : 1;
                };
                uint16_t        __max_value;
            };
        )
#else
        protected_member(
            struct {
                uint16_t        __cur_value;
                uint16_t        __max_value;
                vsf_eda_t       *eda_tx;
            };
        )
#endif
    };

    public_member(
        vsf_eda_queue_op_t  op;
    )

    protected_member(
        vsf_eda_t *eda_rx;
    )
};
//! @}
#endif



#   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
typedef vsf_teda_t  vsf_peda_t;
#   else
typedef vsf_eda_t  vsf_peda_t;
#   endif

// IPC
typedef enum vsf_sync_reason_t {
    VSF_SYNC_FAIL,
    VSF_SYNC_TIMEOUT,
    VSF_SYNC_PENDING,
    VSF_SYNC_GET,
    VSF_SYNC_CANCEL
} vsf_sync_reason_t;

typedef vsf_sync_t          vsf_sem_t;
typedef vsf_sync_t          vsf_trig_t;
// internal use only
typedef vsf_sync_t          __vsf_crit_npb_t;

// vsf_mutex_t support priority inherit
// so who claim mutex, he must free the mutex himself
typedef vsf_sync_owner_t    vsf_mutex_t;
typedef vsf_mutex_t         vsf_crit_t;

typedef struct vsf_bmpevt_adapter_sync_t {
    implement(vsf_bmpevt_adapter_eda_t)
    vsf_sync_t *sync;
} vsf_bmpevt_adapter_sync_t;

typedef struct vsf_bmpevt_adapter_bmpevt_t {
    implement(vsf_bmpevt_adapter_eda_t)
    vsf_bmpevt_t *bmpevt;
    vsf_bmpevt_pender_t pender;
} vsf_bmpevt_adapter_bmpevt_t;
#endif

typedef enum vsf_kernel_error_t {
    VSF_KERNEL_ERR_NONE = 0,
    VSF_KERNEL_ERR_NULL_EDA_PTR,
    VSF_KERNEL_ERR_INVALID_USAGE,
    VSF_KERNEL_ERR_EDA_DOES_NOT_SUPPORT_TIMER,
    VSF_KERNEL_ERR_SHOULD_NOT_USE_PRIO_INHERIT_IN_IDLE_OR_ISR
} vsf_kernel_error_t;

typedef struct vsf_kernel_cfg_t {
    vsf_prio_t      highest_prio;
    vsf_arch_prio_t systimer_arch_prio;
} vsf_kernel_cfg_t;


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED

#   if VSF_KERNEL_CFG_TIMER_MODE == VSF_KERNEL_CFG_TIMER_MODE_TICK
SECTION(".text.vsf.kernel.teda")
extern void vsf_timer_on_tick(void);
#   endif

SECTION(".text.vsf.kernel.teda")
extern vsf_timer_tick_t vsf_systimer_get_tick(void);

SECTION(".text.vsf.kernel.vsf_timer_get_duration")
extern uint_fast32_t vsf_timer_get_duration(vsf_timer_tick_t from_time, vsf_timer_tick_t to_time);

SECTION(".text.vsf.kernel.vsf_timer_get_elapsed")
extern uint_fast32_t vsf_timer_get_elapsed(vsf_timer_tick_t from_time);

#endif

#if defined(__VSF_EDA_CLASS_INHERIT__) || defined(__VSF_EDA_CLASS_IMPLEMENT)
SECTION(".text.vsf.kernel.eda")
extern vsf_err_t vsf_eda_init(vsf_eda_t *this_ptr, vsf_prio_t priotiry, bool is_stack_owner);

SECTION(".text.vsf.kernel.vsf_eda_set_evthandler")
extern vsf_err_t vsf_eda_set_evthandler(vsf_eda_t *this_ptr, vsf_eda_evthandler_t evthandler);

SECTION(".text.vsf.kernel.eda")
extern void vsf_kernel_init( const vsf_kernel_cfg_t *cfg_ptr);

#   if VSF_KERNEL_USE_SIMPLE_SHELL == ENABLED

SECTION(".text.vsf.kernel.vsf_eda_polling_state_get")
extern bool vsf_eda_polling_state_get(vsf_eda_t *this_ptr);

SECTION(".text.vsf.kernel.vsf_eda_polling_state_set")
extern void vsf_eda_polling_state_set(vsf_eda_t *this_ptr, bool state);

#   endif
#endif

SECTION(".text.vsf.kernel.vsf_eda_set_evthandler")
extern vsf_err_t vsf_eda_go_to(uintptr_t evthandler);

SECTION(".text.vsf.kernel.vsf_eda_start")
vsf_err_t vsf_eda_start(vsf_eda_t *this_ptr, vsf_eda_cfg_t *cfg);

SECTION(".text.vsf.kernel.eda")
extern vsf_eda_t *vsf_eda_get_cur(void);

SECTION(".text.vsf.kernel.vsf_eda_get_cur_evt")
extern vsf_evt_t vsf_eda_get_cur_evt(void);

SECTION(".text.vsf.kernel.vsf_eda_get_cur_msg")
extern void *vsf_eda_get_cur_msg(void);

#if VSF_KERNEL_USE_SIMPLE_SHELL == ENABLED
SECTION(".text.vsf.kernel.vsf_eda_is_stack_owner")
extern bool vsf_eda_is_stack_owner(vsf_eda_t *this_ptr);
#endif

SECTION(".text.vsf.kernel.vsf_eda_return")
extern bool __vsf_eda_return(uintptr_t return_value);


#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
SECTION(".text.vsf.kernel.vsf_eda_get_return_value")
extern uintptr_t vsf_eda_get_return_value(void);
#endif

SECTION(".text.vsf.kernel.vsf_eda_yield")
extern void vsf_eda_yield(void);

#if defined(__VSF_EDA_CLASS_INHERIT__) || defined(__VSF_EDA_CLASS_IMPLEMENT)
/* vsf_eda_fini() enables you to kill other eda tasks.
   We highly recommend that DO NOT use this api until you 100% sure.
   please make sure that the resources are properly freed when you trying to kill
   an eda other than your own. We highly recommend that please send a semaphore to
   the target eda to ask it killing itself after properly freeing all the resources.
 */
SECTION(".text.vsf.kernel.eda")
extern vsf_err_t vsf_eda_fini(vsf_eda_t *this_ptr);
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
SECTION(".text.vsf.kernel.__vsf_eda_call_eda")
extern
vsf_err_t __vsf_eda_call_eda(   uintptr_t evthandler,
                                uintptr_t param,
                                size_t local_size);

SECTION(".text.vsf.kernel.__vsf_eda_go_to_ex")
extern vsf_err_t __vsf_eda_go_to_ex(uintptr_t evthandler, uintptr_t param);

SECTION(".text.vsf.kernel.eda_nesting")
extern vsf_err_t __vsf_eda_call_eda_ex( uintptr_t func,
                                        uintptr_t param,
                                        __vsf_eda_frame_state_t state,
                                        bool is_sub_call);

SECTION(".text.vsf.kernel.__vsf_eda_get_local")
extern uintptr_t __vsf_eda_get_local(vsf_eda_t* this_ptr);

SECTION(".text.vsf.kernel.vsf_eda_target_set")
extern vsf_err_t vsf_eda_target_set(uintptr_t param);

SECTION(".text.vsf.kernel.vsf_eda_target_get")
extern uintptr_t vsf_eda_target_get(void);

SECTION(".text.vsf.kernel.vsf_eda_frame_user_value_set")
extern
vsf_err_t vsf_eda_frame_user_value_set(__vsf_frame_uint_t value);

SECTION(".text.vsf.kernel.vsf_eda_frame_user_value_get")
extern
vsf_err_t vsf_eda_frame_user_value_get(__vsf_frame_uint_t* pvalue);

#if     VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
SECTION(".text.vsf.kernel.eda_fsm")
extern
fsm_rt_t __vsf_eda_call_fsm(vsf_fsm_entry_t entry,
                            uintptr_t param,
                            size_t local_size);
#   endif      // VSF_KERNEL_CFG_EDA_SUPPORT_FSM

#endif      // VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
SECTION(".text.vsf.kernel.teda")
extern vsf_err_t vsf_teda_init(vsf_teda_t *this_ptr, vsf_prio_t priority, bool is_stack_owner);

SECTION(".text.vsf.kernel.vsf_teda_start")
extern vsf_err_t vsf_teda_start(vsf_teda_t *this_ptr, vsf_eda_cfg_t *cfg);

SECTION(".text.vsf.kernel.vsf_teda_set_timer")
extern vsf_err_t vsf_teda_set_timer(uint_fast32_t tick);

SECTION(".text.vsf.kernel.vsf_teda_set_timer_ex")
vsf_err_t vsf_teda_set_timer_ex(vsf_teda_t *this_ptr, uint_fast32_t tick);

#   if VSF_KERNEL_CFG_TIMER_MODE == VSF_KERNEL_CFG_TIMER_MODE_TICKLESS
SECTION(".text.vsf.kernel.vsf_teda_set_timer_ms")
extern vsf_err_t vsf_teda_set_timer_ms(uint_fast32_t ms);

SECTION(".text.vsf.kernel.vsf_teda_set_timer_us")
extern vsf_err_t vsf_teda_set_timer_us(uint_fast32_t us);
#   endif

SECTION(".text.vsf.kernel.vsf_teda_cancel_timer")
extern vsf_err_t vsf_teda_cancel_timer(void);

#if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
SECTION(".text.vsf.kernel.vsf_callback_timer_init")
void vsf_callback_timer_init(vsf_callback_timer_t *timer);

SECTION(".text.vsf.kernel.vsf_callback_timer_add")
vsf_err_t vsf_callback_timer_add(vsf_callback_timer_t *timer, uint_fast32_t tick);

SECTION(".text.vsf.kernel.vsf_callback_timer_remove")
vsf_err_t vsf_callback_timer_remove(vsf_callback_timer_t *timer);

#   if VSF_KERNEL_CFG_TIMER_MODE == VSF_KERNEL_CFG_TIMER_MODE_TICKLESS
SECTION(".text.vsf.kernel.vsf_callback_timer_add_ms")
vsf_err_t vsf_callback_timer_add_ms(vsf_callback_timer_t *timer, uint_fast32_t ms);

SECTION(".text.vsf.kernel.vsf_callback_timer_add_us")
vsf_err_t vsf_callback_timer_add_us(vsf_callback_timer_t *timer, uint_fast32_t us);
#   endif
#endif
#endif

SECTION(".text.vsf.kernel.eda")
extern vsf_err_t vsf_eda_post_evt(vsf_eda_t *this_ptr, vsf_evt_t evt);

SECTION(".text.vsf.kernel.vsf_eda_post_msg")
extern vsf_err_t vsf_eda_post_msg(vsf_eda_t *this_ptr, void *msg);
#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
SECTION(".text.vsf.kernel.vsf_eda_post_evt_msg")
extern vsf_err_t vsf_eda_post_evt_msg(vsf_eda_t *this_ptr, vsf_evt_t evt, void *msg);
#endif

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
SECTION(".text.vsf.kernel.vsf_sync")
extern vsf_err_t vsf_eda_sync_init(vsf_sync_t *this_ptr, uint_fast16_t cur_value,
        uint_fast16_t max_value);

#if VSF_KERNEL_CFG_SUPPORT_SYNC_IRQ == ENABLED
SECTION(".text.vsf.kernel.vsf_sync")
vsf_err_t vsf_eda_sync_increase_irq(vsf_sync_t *this_ptr);
#endif

SECTION(".text.vsf.kernel.vsf_sync")
extern vsf_err_t vsf_eda_sync_increase(vsf_sync_t *this_ptr);

SECTION(".text.vsf.kernel.vsf_sync")
extern vsf_err_t vsf_eda_sync_increase_ex(vsf_sync_t *this_ptr, vsf_eda_t *eda);

SECTION(".text.vsf.kernel.vsf_sync")
extern void vsf_eda_sync_force_reset(vsf_sync_t *this_ptr);

SECTION(".text.vsf.kernel.vsf_sync")
extern vsf_err_t vsf_eda_sync_decrease(vsf_sync_t *this_ptr, int_fast32_t timeout);

SECTION(".text.vsf.kernel.vsf_sync")
extern vsf_err_t vsf_eda_sync_decrease_ex(vsf_sync_t *this_ptr, int_fast32_t timeout, vsf_eda_t *eda);

SECTION(".text.vsf.kernel.vsf_eda_sync_cancel")
extern void vsf_eda_sync_cancel(vsf_sync_t *this_ptr);

SECTION(".text.vsf.kernel.vsf_eda_sync_get_reason")
extern vsf_sync_reason_t vsf_eda_sync_get_reason(vsf_sync_t *this_ptr, vsf_evt_t evt);

#if VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT == ENABLED
SECTION(".data.vsf.kernel.vsf_eda_bmpevt_adapter_sync_op")
extern const vsf_bmpevt_adapter_op_t vsf_eda_bmpevt_adapter_sync_op;

SECTION(".data.vsf.kernel.vsf_eda_bmpevt_adapter_bmpevt_op")
extern const vsf_bmpevt_adapter_op_t vsf_eda_bmpevt_adapter_bmpevt_op;

SECTION(".text.vsf.kernel.vsf_eda_bmpevt_init")
extern vsf_err_t vsf_eda_bmpevt_init(vsf_bmpevt_t *this_ptr, uint_fast8_t adapter_count);

SECTION(".text.vsf.kernel.vsf_eda_bmpevt_set")
extern vsf_err_t vsf_eda_bmpevt_set(vsf_bmpevt_t *this_ptr, uint_fast32_t mask);

SECTION(".text.vsf.kernel.vsf_eda_bmpevt_reset")
extern vsf_err_t vsf_eda_bmpevt_reset(vsf_bmpevt_t *this_ptr, uint_fast32_t mask);

SECTION(".text.vsf.kernel.vsf_eda_bmpevt_cancel")
extern vsf_err_t vsf_eda_bmpevt_cancel(vsf_bmpevt_t *this_ptr, uint_fast32_t mask);

SECTION(".text.vsf.kernel.vsf_eda_bmpevt_pend")
extern vsf_err_t vsf_eda_bmpevt_pend(vsf_bmpevt_t *this_ptr, vsf_bmpevt_pender_t *pender, int_fast32_t timeout);

SECTION(".text.vsf.kernel.vsf_eda_bmpevt_poll")
extern vsf_sync_reason_t vsf_eda_bmpevt_poll(vsf_bmpevt_t *this_ptr, vsf_bmpevt_pender_t *pender, vsf_evt_t evt);
#endif

#if __VSF_KERNEL_CFG_SUPPORT_GENERIC_QUEUE == ENABLED
SECTION(".text.vsf.kernel.vsf_eda_queue_init")
extern vsf_err_t vsf_eda_queue_init(vsf_eda_queue_t *this_ptr, uint_fast16_t max);

SECTION(".text.vsf.kernel.vsf_eda_queue_send")
extern vsf_err_t vsf_eda_queue_send(vsf_eda_queue_t *this_ptr, void *node, int_fast32_t timeout);

SECTION(".text.vsf.kernel.vsf_eda_queue_send_ex")
extern vsf_err_t vsf_eda_queue_send_ex(vsf_eda_queue_t *this_ptr, void *node, int_fast32_t timeout, vsf_eda_t *eda);

SECTION(".text.vsf.kernel.vsf_eda_queue_send_get_reason")
extern vsf_sync_reason_t vsf_eda_queue_send_get_reason(vsf_eda_queue_t *this_ptr, vsf_evt_t evt, void *node);

SECTION(".text.vsf.kernel.vsf_eda_queue_recv")
extern vsf_err_t vsf_eda_queue_recv(vsf_eda_queue_t *this_ptr, void **node, int_fast32_t timeout);

SECTION(".text.vsf.kernel.vsf_eda_queue_recv_ex")
extern vsf_err_t vsf_eda_queue_recv_ex(vsf_eda_queue_t *this_ptr, void **node, int_fast32_t timeout, vsf_eda_t *eda);

SECTION(".text.vsf.kernel.vsf_eda_queue_recv_get_reason")
extern vsf_sync_reason_t vsf_eda_queue_recv_get_reason(vsf_eda_queue_t *this_ptr, vsf_evt_t evt, void **node);
#endif      // __VSF_KERNEL_CFG_SUPPORT_GENERIC_QUEUE


#endif      // VSF_KERNEL_CFG_SUPPORT_SYNC

#ifdef __cplusplus
}
#endif

#undef __VSF_EDA_CLASS_INHERIT__
#undef __VSF_EDA_CLASS_IMPLEMENT

#endif
#endif      // __VSF_EDA_H__
