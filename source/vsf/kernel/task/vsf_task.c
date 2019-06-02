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

/*============================ INCLUDES ======================================*/
#include "../vsf_kernel_cfg.h"

#if VSF_USE_KERNEL_TASK_MODE == ENABLED

#define __VSF_TASK_CLASS_IMPLEMENT

#include "../vsf_kernel_common.h"
#include "../vsf_eda.h"
#include "../vsf_evtq.h"
/*! \note implement class vsf_pt */

#include "./vsf_task.h"
#include "../vsf_os.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

implement_vsf_pool(vsf_task_stack_frame_pool, vsf_task_stack_frame_t);

static NO_INIT vsf_pool(vsf_task_stack_frame_pool) __default_frame_pool;

static void __vsf_task_pop(vsf_task_t *ptask)
{
    class_internal(ptask, ptThis, vsf_task_t);
    vsf_task_stack_frame_t *frame_ptr = NULL;
    
    this.call_depth--;
    
    vsf_slist_stack_pop(vsf_task_stack_frame_t, 
                        use_as__vsf_slist_node_t, 
                        &(this.stack.list), 
                        frame_ptr);
                        
    VSF_POOL_FREE(vsf_task_stack_frame_pool, this.pstack_frame_pool, frame_ptr);
}

static void __vsf_task_push(vsf_task_t *ptask, vsf_task_stack_frame_t *frame_ptr)
{
    class_internal(ptask, ptThis, vsf_task_t);
    
    vsf_slist_stack_push(   vsf_task_stack_frame_t, 
                            use_as__vsf_slist_node_t, 
                            &(this.stack.list), 
                            frame_ptr);
    this.call_depth++;
    
    //! calculate the maximum depth
    this.max_call_depth = max(this.call_depth, this.max_call_depth);
}


vsf_err_t __vsf_task_branch(vsf_task_t *ptask,
                            vsf_task_entry_t *fnEntry, 
                            void *pTarget, 
                            bool is_sub_call )
{
    ASSERT(NULL != ptask);
    class_internal(ptask, ptThis, vsf_task_t);   

    if (!is_sub_call) {
        this.stack.frame_ptr->fnEntry = fnEntry;
        if (NULL == pTarget) {
            this.stack.frame_ptr->pTarget = ptask + 1;
        } else {
            this.stack.frame_ptr->pTarget = pTarget;
        }
    } else {
        vsf_task_stack_frame_t *pframe = 
            VSF_POOL_ALLOC(vsf_task_stack_frame_pool, this.pstack_frame_pool);
        if (NULL == pframe) {
            return VSF_ERR_NOT_ENOUGH_RESOURCES;
        }
        pframe->fnEntry = fnEntry;
        pframe->next = NULL;
        if (NULL == pTarget) {
            pframe->pTarget = ptask + 1;
        } else {
            pframe->pTarget = pTarget;
        }
        __vsf_task_push(ptask, pframe);
    }

    return VSF_ERR_NONE;
}

fsm_rt_t vsf_task_branch(  vsf_task_entry_t *fnEntry, 
                            void *pTarget, 
                            bool is_sub_call )
{
    class_internal(vsf_eda_get_cur(), ptThis, vsf_task_t);
    fsm_rt_t task_return_state = this.task_return_state;
    
    if (is_sub_call) {
        switch(task_return_state) {
            case fsm_rt_on_going:
            case fsm_rt_wait_for_obj:
            case fsm_rt_asyn:
                break;
            default:
                this.task_return_state = fsm_rt_on_going;
                return task_return_state;
        }
    }
    
    __vsf_task_branch((vsf_task_t *)ptThis, 
                            fnEntry,
                            pTarget,
                            is_sub_call);
    /*! \note
     *  - if VSF_ERR_NOT_ENOUGH_RESOURCES is detected, yield and try it again 
     *  (automatically). For tasks sharing the same frame pool, if the pool is 
     *  too small, only task performance will be affected, and all sub-task call
     *  will work when frame is allocated. 
     *  - if the frame is allocated and pushed to the stack, we should yield to
     *  let the sub-task run.
     *
     *  Since in either way, we will yield, no need to handle the return 
     *  value of __vsf_task_branch().
     */
    return fsm_rt_yield;
}

static void __vsf_task_evthandler(vsf_eda_t *peda, vsf_evt_t evt)
{
    vsf_task_t *ptask = (vsf_task_t *)peda;
    class_internal(ptask, ptThis, vsf_task_t);
    ASSERT(     ptask != NULL 
            &&  NULL != this.stack.frame_ptr
            &&  NULL != this.stack.frame_ptr->fnEntry);

    do {
        this.task_return_state = 
            (*this.stack.frame_ptr->fnEntry)(this.stack.frame_ptr->pTarget, evt);
        
        switch(this.task_return_state) {
            default:            //! return fsm_rt_err
            case fsm_rt_asyn:   //! call sub fsm later
            case fsm_rt_cpl:
                if (NULL != this.stack.list.head) {
                    __vsf_task_pop(ptask);
                }
                
                if (NULL == this.stack.list.head) {
                    //! stack is empty, i.e. the root of the call stack
                #if VSF_CFG_TIMER_EN == ENABLED
                    vsf_teda_fini((vsf_teda_t *)peda);
                #else
                    vsf_eda_fini(peda);
                #endif
                    return ;
                } 
                break;
            
            case fsm_rt_wait_for_evt:
                //! delay, wait_for, mutex_pend, sem_pend and etc...
                return ;
            case fsm_rt_yield:
                vsf_eda_yield();
                return ;
        }
    } while(true);
}

void vsf_task_init( vsf_pool_block(vsf_task_stack_frame_pool) *frame_buf_ptr,
                    uint_fast16_t count)
{
    do {   
        /*
        static const vsf_pool_cfg_t cfg = {
            NULL, 
            (code_region_t *)&VSF_SCHED_SAFE_CODE_REGION,
        };                             
        vsf_task_stack_frame_pool_pool_init((&__default_frame_pool), &cfg);                         
        */    
        VSF_POOL_PREPARE(vsf_task_stack_frame_pool, (&__default_frame_pool),
            .pTarget = NULL, 
            .ptRegion = (code_region_t *)&VSF_SCHED_SAFE_CODE_REGION,
        );
        if (NULL == frame_buf_ptr || 0 == count) {
            break;
        }
        vsf_pool_add_buffer(  
            (vsf_pool_t *)(&__default_frame_pool),               
            frame_buf_ptr,                                  
            count 
                * sizeof(vsf_pool_block(vsf_task_stack_frame_pool)),                     
            sizeof(vsf_task_stack_frame_pool_pool_item_t));              
    } while(0);       
}

vsf_err_t vsf_task_start(vsf_task_t *ptask, vsf_task_cfg_t *pcfg)
{
    ASSERT(     NULL != ptask 
            &&  NULL != pcfg
            &&  NULL != pcfg->fnEntry);
            
    class_internal(ptask, ptThis, vsf_task_t);
    
    memset(ptask, 0, sizeof(vsf_task_t));

    if (NULL == pcfg->pframe_pool) {
        //! use default frame pool
        this.pstack_frame_pool = &__default_frame_pool; 
    } else {
        //! use specified frame pool
        this.pstack_frame_pool = pcfg->pframe_pool;
    }
    //this.call_depth = 0;
    //this.max_call_depth = 0;
    this.task_return_state = fsm_rt_on_going;
    vsf_err_t err = __vsf_task_branch(ptask, pcfg->fnEntry, pcfg->pTarget, true);
    if (VSF_ERR_NONE != err) {
        return err;
    }
    
    this.evthandler = __vsf_task_evthandler;
    
#if VSF_CFG_TIMER_EN == ENABLED
    return vsf_teda_init(&this.use_as__vsf_teda_t, pcfg->priority, false);
#else
    return vsf_eda_init(&this.use_as__vsf_eda_t, pcfg->priority, false);
#endif
}

#endif
