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

#include "service/vsf_service.h"
#include "../vsf_eda.h"
#include "./vsf_pt.h"
#include "./vsf_thread.h"
#include "./vsf_fsm.h"

#if VSF_USE_KERNEL_TASK_MODE == ENABLED

/*============================ MACROS ========================================*/
/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file 
 *!        included in this file
 */
#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__VSF_TASK_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#   undef __VSF_TASK_CLASS_IMPLEMENT
#elif   defined(__VSF_TASK_CLASS_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#   undef __VSF_TASK_CLASS_INHERIT
#endif   

#include "utilities/ooc_class.h"



/*============================ MACROFIED FUNCTIONS ===========================*/

#define __implement_vsf_task(__NAME)                                            \
        fsm_rt_t vsf_task_func_##__NAME(task_cb_##__NAME *ptThis, vsf_evt_t evt) 
        
#define implement_vsf_task(__NAME)    __implement_vsf_task(__NAME)
        
        
#define __vsf_task_func(__NAME)     vsf_task_func_##__NAME
#define vsf_task_func(__NAME)       __vsf_task_func(__NAME)

#define __vsf_task(__NAME)          task_cb_##__NAME
#define vsf_task(__NAME)            __vsf_task(__NAME)


#define __def_vsf_task(__NAME,...)                                          \
        struct task_cb_##__NAME {                                           \
            uint8_t chState;                                                \
            __VA_ARGS__                                                     \
        };                                                                  \
        struct __NAME {                                                     \
            implement(vsf_task_t);                                          \
            implement(task_cb_##__NAME);                                    \
        };                                                                  \
            


#define def_vsf_task(__NAME,...)      __def_vsf_task(__NAME,__VA_ARGS__)

#define __declare_vsf_task(__NAME)                                              \
            typedef struct __NAME __NAME;                                       \
            typedef struct task_cb_##__NAME  task_cb_##__NAME;
#define declare_vsf_task(__NAME)      __declare_vsf_task(__NAME)

#define __init_vsf_task(__NAME, __TASK, __PRI, ...)                             \
        do {                                                                    \
            vsf_task_cfg_t cfg = {                                              \
                .fnEntry = (vsf_task_entry_t *)vsf_task_func(__NAME),           \
                .priority = (__PRI),                                            \
                __VA_ARGS__                                                     \
            };                                                                  \
            vsf_task_start(&((__TASK)->use_as__vsf_task_t), &cfg);              \
        } while(0)
#define init_vsf_task(__NAME, __TASK, __PRI, ...)                               \
            __init_vsf_task(__NAME, (__TASK), (__PRI), __VA_ARGS__)

#define vsf_task_call_sub(__NAME, __TARGET)                                     \
            vsf_task_branch((vsf_task_entry_t *)vsf_task_func(__NAME),          \
                            (__TARGET),                                         \
                            true)

/*! \note IMPORTANT!!!!!
 *        For anything you want to wait, which is coming from vsf 
 *        system, please use vsf_task_wait_until().
 *        Those includues: VSF_EVT_XXXXX, sempahore, mutex and etc.
 *        
 *        For VSF_EVT_XXXX, please use:
 *        vsf_task_wait_until(
 *            on_vsf_task_evt(VSF_EVT_XXXXX) {
 *                //! when the target VSF_EVT_XXXX arrived
 *                ...
 *            }
 *        )
 *
 *        For sempahore, please use:
 *        vsf_task_wait_until(
 *            vsf_sem_pend(...) {
 *                //! when the semaphore is acquired
 *                ...
 *            }on_sem_timeout() {
 *                //! when timeout
 *            }
 *        )
 *
 *        For time, please use:
 *        vsf_task_wait_until(
 *            vsf_delay(...) {
 *                //! when timeout
 *                ...
 *            )
 *        )
 */                              
#define vsf_task_wait_until(...)                                                \
        do {                                                                    \
            __VA_ARGS__ else {                                                  \
                return fsm_rt_wait_for_evt;                                     \
            }                                                                   \
        } while(0)
        
#define on_vsf_task_init()                                                      \
        if (VSF_EVT_INIT == evt)
        
#define on_vsf_task_fini()                                                      \
        if (VSF_EVT_FINI == evt)
        
#define on_vsf_task_evt(__EVT)                                                  \
        if ((__EVT) == evt)
        
/*============================ TYPES =========================================*/


typedef fsm_rt_t vsf_task_entry_t(void *pthis, vsf_evt_t evt);

typedef struct vsf_task_stack_frame_t vsf_task_stack_frame_t;

struct vsf_task_stack_frame_t {
    implement(vsf_slist_node_t)
    vsf_task_entry_t    *fnEntry;
    void                *pTarget;
};

declare_vsf_pool(vsf_task_stack_frame_pool)

def_vsf_pool(vsf_task_stack_frame_pool, vsf_task_stack_frame_t)

declare_class(vsf_task_t)

def_class(vsf_task_t, 
    which(
#if VSF_CFG_TIMER_EN == ENABLED
        implement(vsf_teda_t)
#else
        implement(vsf_eda_t)
#endif
    ),
    
    private_member(
        union {
            vsf_task_stack_frame_t *frame_ptr;
            vsf_slist_t             list;
        }stack;
        
        vsf_pool(vsf_task_stack_frame_pool) *pstack_frame_pool;
        int8_t task_return_state;           /* for holding fsm_rt_t value */
        uint8_t call_depth;
        uint8_t max_call_depth;             /* for debug purpose */
    )
)

end_def_class(vsf_task_t)


typedef struct {
    vsf_task_entry_t                        *fnEntry;
    vsf_priority_t                          priority;
    void                                    *pTarget;
    vsf_pool(vsf_task_stack_frame_pool)     *pframe_pool;
} vsf_task_cfg_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

SECTION("text.vsf.kernel.vsf_task_init")
extern void vsf_task_init( 
                vsf_pool_block(vsf_task_stack_frame_pool) *frame_buf_ptr,
                uint_fast16_t count);
SECTION("text.vsf.kernel.vsf_task_start")
extern vsf_err_t vsf_task_start( vsf_task_t *, vsf_task_cfg_t *);
SECTION("text.vsf.kernel.vsf_task_branch")
extern fsm_rt_t vsf_task_branch(    vsf_task_entry_t *fnEntry, 
                                    void *pTarget, 
                                    bool is_sub_call );



#endif

#endif
/* EOF */
