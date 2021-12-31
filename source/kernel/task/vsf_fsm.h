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

/****************************************************************************
*  Copyright 2017 Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)       *
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

#ifndef __SIMPLE_FSM_H__
#define __SIMPLE_FSM_H__

/*============================ INCLUDES ======================================*/
#include "./kernel/vsf_kernel_cfg.h"

#include "utilities/vsf_utilities.h"
/*============================ MACROS ========================================*/

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   undef VSF_KERNEL_CFG_EDA_SUPPORT_SIMPLE_FSM
#   define VSF_KERNEL_CFG_EDA_SUPPORT_SIMPLE_FSM        DISABLED
#endif

#if     VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED                  \
    &&  VSF_USE_KERNEL == ENABLED                                               \
    &&  VSF_KERNEL_CFG_EDA_SUPPORT_SIMPLE_FSM == ENABLED                        \
    &&  VSF_KERNEL_CFG_EDA_SUPPORT_TASK == ENABLED

#ifdef __cplusplus
extern "C" {
#endif


#ifndef vsf_this
#   define vsf_this    (*vsf_pthis)
#endif


#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   error simple_fsm require at least ANSI-C99 support
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
#define def_states(...)                                                         \
        enum {                                                                  \
            START = 0,                                                          \
            __VA_ARGS__                                                         \
        };

#ifndef def_params
#   define def_params(...)      __VA_ARGS__
#endif

#define vsf_args(...)           ,__VA_ARGS__

#define vsf_fsm(__name)         vsf_task(__name)

#define __def_fsm(__fsm_type, ...)                                              \
        def_vsf_task(__fsm_type,                                                \
            __VA_ARGS__);

#define def_fsm(__name, ...)                                                    \
        __def_fsm(__name, __VA_ARGS__)

#define def_simple_fsm(__name, ...)                                             \
        __declare_fsm(__name);                                                  \
        __def_fsm(__name, __VA_ARGS__)

#define end_def_simple_fsm(...)

/*
#define __extern_simple_fsm(__fsm_type, ...)                                    \
        declare_class(__fsm_type)                                               \
        extern_class(__fsm_type)                                                \
            uint_fast8_t fsm_state;                                             \
            __VA_ARGS__                                                         \
        end_extern_class(__fsm_type)

#define extern_simple_fsm(__name, ...)                                          \
        __extern_simple_fsm(fsm(__name), __VA_ARGS__)
*/

#define __declare_fsm(__name)                                                   \
        declare_vsf_task(__name)

#define declare_vsf_fsm(__name)         __declare_fsm(__name)

/*! \brief extern fsm initialisation function and provide function prototye
           as <__name>_fn, E.g
           extern_vsf_fsm_initialiser( demo_fsm );
           we extern a function called:
           extern fsm_demo_fsm_t *demo_fsm_init( fsm_demo_fsm_t *fsm_ptr );
           and a prototype definition:
           typedef fsm_demo_fsm_t *demo_fsm_init_fn( fsm_demo_fsm_t *fsm_ptr );
           We can then use demo_fsm_init_fn to define function pointer
 */
#define __extern_fsm_initialiser(__name, ...)                                   \
        vsf_fsm(__name) *__name##_init(vsf_fsm(__name) *fsm_ptr __VA_ARGS__);           \
        typedef vsf_fsm(__name) *__name##_init_fn(vsf_fsm(__name) *fsm_ptr __VA_ARGS__);

#define extern_vsf_fsm_initialiser(__name, ...)                                     \
            __extern_fsm_initialiser(__name, __VA_ARGS__)


/*! \brief extern fsm task function and provide function prototye as <__name>_fn, E.g
           extern_vsf_fsm_implementation( demo_fsm );
           we extern a function called:
           fsm_rt_t demo_fsm( fsm_demo_fsm_t *fsm_ptr )
           and a prototype definition:
           typedef fsm_rt_t demo_fsm_fn (fsm_demo_fsm_t *fsm_ptr);
           We can then use demo_fsm_fn to define function pointer
 */
 //! @{
#define __extern_fsm_implementation_ex(__name,__type)                           \
        fsm_rt_t __name(fsm(__type) *vsf_pthis, vsf_evt_t evt);                 \
        typedef fsm_rt_t __name##_fn( vsf_fsm(__type) *vsf_pthis, vsf_evt_t evt );

#define declare_vsf_fsm_implementation_ex(__name, __type)                       \
            __extern_fsm_implementation_ex(__name, __type)

#define extern_vsf_fsm_implementation_ex(__name,__type)                         \
            __extern_fsm_implementation_ex(__name, __type)

#define extern_vsf_fsm_implementation(__name)                                   \
            __extern_fsm_implementation_ex(__name, __name)

#define declare_vsf_fsm_implementation(__name)                                  \
            __extern_fsm_implementation_ex(__name, __name)
//! @}

#define call_vsf_fsm(__name, __fsm )                                            \
            vsf_task_call_task(__name, __fsm)

#define ____state(__state, ...)                                                 \
            case __state:                                                       \
        __state_entry_##__state:{                                               \
                __VA_ARGS__;                                                    \
            };

#define vsf_state(__state, ...)                 break; ____state(__state, __VA_ARGS__)

#define on_start(...)                       {__VA_ARGS__;}


#define reset_vsf_fsm()         do { vsf_this.fsm_state = 0; } while(0);
#define vsf_fsm_cpl()           do {reset_vsf_fsm(); return fsm_rt_cpl;} while(0);
#define vsf_fsm_report(__ERROR) do {reset_vsf_fsm(); return (fsm_rt_t)(__ERROR); } while(0);
#define vsf_fsm_wait_for_obj()  return fsm_rt_wait_for_obj;
#define vsf_fsm_on_going()      return fsm_rt_on_going;

//! fsm_continue is deprecated, should not be used anymore
//#define fsm_continue()      break


#define update_state_to(__state)                                                \
        { vsf_this.fsm_state = (__state); goto __state_entry_##__state;}

#define transfer_to(__state)                                                    \
         { vsf_this.fsm_state = (__state); vsf_fsm_on_going() }


#define __fsm_initialiser(__name, ...)                                          \
        vsf_fsm(__name) *__name##_init(vsf_fsm(__name) *vsf_pthis __VA_ARGS__)  \
        {                                                                       \
            VSF_KERNEL_ASSERT (NULL != vsf_pthis);                              \
            vsf_this.fsm_state = 0;

#define vsf_fsm_initialiser(__name, ...)                                        \
            __fsm_initialiser(__name, __VA_ARGS__)


#define abort_vsf_fsm_init()     return NULL;

#define vsf_fsm_init_body(...)                                                  \
            __VA_ARGS__                                                         \
            return &vsf_this;                                                   \
        }


#define init_vsf_fsm(__name, __fsm, ...)                                        \
        __name##_init(__fsm __VA_ARGS__)

#define init_simple_fsm(__name, __fsm, ...)                                     \
        init_vsf_fsm(__fsm __VA_ARGS__)

#define start_vsf_fsm(__name, __fsm, __pri, ...)                                \
            init_vsf_task(__name, (__fsm), (__pri), __VA_ARGS__)

#define start_simple_fsm(__name, __fsm, __pri, ...)                             \
            start_vsf_fsm(__name, (__fsm), (__pri), __VA_ARGS__)

#define __implement_fsm_ex(__name, __type)                                      \
    implement_vsf_task(__name)                                                  \
    {                                                                           \
        vsf_task_begin();                                                       \
        if (NULL == vsf_pthis) {                                                \
            return fsm_rt_err;                                                  \
        }

#define __body(...)                                                             \
        switch (vsf_this.fsm_state) {                                           \
            case 0:                                                             \
                vsf_this.fsm_state++;                                           \
            __VA_ARGS__                                                         \
            break;                                                              \
            default:                                                            \
            return fsm_rt_err;                                                  \
        }                                                                       \
                                                                                \
        vsf_task_end();                                                         \
    }

#define vsf_fsm_body(...)               __body(__VA_ARGS__)

#define vsf_fsm_begin()                                                         \
            switch (vsf_this.fsm_state) {                                       \
                case 0:                                                         \
                    vsf_this.fsm_state++;

#define vsf_fsm_end()                                                              \
                break;                                                          \
                default:                                                        \
                return fsm_rt_err;                                              \
            }                                                                   \
        }vsf_task_end()


/*! \note Debug Support: You can use body_begin() together with body_end()
 *!       to enable debug, this means you are ale to set break
 *!       points and single-step into those states.
 *!
 *!       Example:

        vsf_fsm_begin()
            vsf_state(xxxxx,
                //the range no debug is allowed
            )

            vsf_state(xxxxx){
                //the range you can debug with
            }

        vsf_fsm_end()
*/


#define implement_fsm_ex(__name, __type)                                        \
            __implement_fsm_ex(__name, __type)

#define __implement_fsm(__name)                                                 \
            implement_fsm_ex(__name, __name)

#define implement_vsf_fsm(__name, ...)      __implement_fsm(__name)

#define __privilege_state(__state, ...)                                         \
            break;do {                                                          \
                do {                                                            \
                    ____state(__state, __VA_ARGS__)                             \
                } while(0); /* add extra while(0) to catch the fsm_continue()*/ \
                if (vsf_this.fsm_state != (__state)) {                          \
                    break;                                                      \
                }                                                               \
            } while(1);

#define privilege_state(__state, ...)                                           \
            __privilege_state(__state, __VA_ARGS__)


#define privilege_group(...)  { __VA_ARGS__;}

#define privilege_body(...)                                                     \
        do {                                                                    \
            switch (vsf_this.fsm_state) {                                       \
                case 0:                                                         \
                    vsf_this.fsm_state++;                                       \
                __VA_ARGS__                                                     \
            }                                                                   \
        while(1);                                                               \
                                                                                \
        return fsm_rt_on_going;                                                 \
    }


/*============================ TYPES =========================================*/

#ifndef __FSM_RT_TYPE__
#define __FSM_RT_TYPE__
//! \name finit vsf_state machine vsf_state
//! @{
typedef enum {
    fsm_rt_err          = -1,    //!< fsm error, error code can be get from other interface
    fsm_rt_cpl          = 0,     //!< fsm complete
    fsm_rt_on_going     = 1,     //!< fsm on-going
    fsm_rt_wait_for_obj = 2,     //!< fsm wait for object
    fsm_rt_asyn         = 3,     //!< fsm asynchronose complete, you can check it later.
} fsm_rt_t;
//! @}

#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
#endif
