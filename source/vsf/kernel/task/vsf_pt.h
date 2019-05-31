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

#if VSF_USE_KERNEL_PT_MODE == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/


#define __vsf_pt_begin()    enum {                                              \
                                count_offset = __COUNTER__ + 1,                 \
                            };                                                  \
                            switch (ptThis->chState) {                          \
                                case __COUNTER__ - count_offset:
                                        
#define __vsf_pt_entry()        ptThis->chState =                               \
                                    (__COUNTER__ - count_offset + 1) >> 1;      \
                                case (__COUNTER__ - count_offset) >> 1:
                                        
#define __vsf_pt_end()      } return fsm_rt_cpl;


#define vsf_pt_begin()              __vsf_pt_begin()
#define vsf_pt_entry()              __vsf_pt_entry()
#define vsf_pt_end()                __vsf_pt_end()

#define __vsf_pt_wait_cond(...)                                                 \
    do {                                                                        \
        evt = VSF_EVT_INVALID;                                                  \
        vsf_pt_entry();                                                         \
        if (__VA_ARGS__){                                                       \
            return fsm_rt_wait_for_evt;                                         \
        }                                                                       \
    } while (0)

#define vsf_pt_yield()          __vsf_pt_wait_cond((VSF_EVT_INVALID == evt))


/*! \note please use if-then clause in the vsf_pt_wait_until()
 *!       E.g. 
 *!       vsf_pt_wait_until(
 *!           if (...) {
 *!               // do something here when if expression is true, when there is 
 *!               // nothing to do here, please use "continue;" here
 *!           }
 *!       );
 *!
 *!       NOTE: wait_for_one(), wait_for_any() and wait_for_all() can be seen as
 *!             an if-then clause. So you can use them directly here. E.g.
 *!
 *!             vsf_pt_wait_until(
 *!                 wait_for_one(&__user_grouped_evts, sem_evt_msk){
 *!                     printf("get sem in pt slave thread\r\n");
 *!                 } 
 *!             );
 *!
 *!       NOTE: when there is nothing to do in the "then" part of an if-then 
 *!             clause, please use "continue;". Using "break" here has the same
 *!             effect but it is not recommended. E.g.
 *!
 *!             vsf_pt_wait_until(
 *!                 if (...) {
 *!                     //! when the if expression is true, continue to execute 
 *!                     //! the code after vsf_pt_wait_until
 *!                     continue; 
 *!                 }
 *!             );
 */

#define vsf_pt_wait_until(...)                                                  \
        do {                                                                    \
            vsf_pt_entry();                                                     \
            vsf_task_wait_until(__VA_ARGS__);                                   \
        }while(0)



#define vsf_pt_wfe(__evt)               __vsf_pt_wait_cond( (evt != __evr))


#define vsf_pt_call(__NAME, __TARGET)                                           \
        (__TARGET)->chState = 0;                                                \
        vsf_pt_entry();                                                         \
        for(fsm_rt_t fsm_result = fsm_rt_on_going;                              \
            fsm_result == fsm_rt_on_going; )                                    \
            if (fsm_rt_on_going ==                                              \
                (fsm_result =                                                   \
                    vsf_task_call_sub(__NAME, (__TARGET)), fsm_result)) {       \
                return fsm_rt_yield;                                            \
            } else if (fsm_rt_cpl == fsm_result)
        
#define vsf_pt_on_call_return(__VALUE)                                          \
            else if ((__VALUE == fsm_result)) 
        
#define implement_vsf_pt(__NAME)        implement_vsf_task(__NAME)
        
#define vsf_pt_func(__NAME)             vsf_task_func(__NAME)
#define vsf_pt(__NAME)                  vsf_task(__NAME)

#define def_vsf_pt(__NAME,...)                                                  \
            def_vsf_task(__NAME,                                                \
                __VA_ARGS__)

#define declare_vsf_pt(__NAME)          declare_vsf_task(__NAME)

#define init_vsf_pt(__NAME, __PT, __PRI, ...)                                   \
            init_vsf_task(__NAME, __PT, __PRI, __VA_ARGS__)
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif
#endif
