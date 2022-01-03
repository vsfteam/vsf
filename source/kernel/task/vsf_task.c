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

/*============================ INCLUDES ======================================*/

#include "../vsf_kernel_cfg.h"

#if VSF_KERNEL_CFG_EDA_SUPPORT_TASK == ENABLED && VSF_USE_KERNEL == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#include "./vsf_task.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
SECTION(".text.vsf.kernel.eda_task")
fsm_rt_t __vsf_eda_call_task(   vsf_task_entry_t entry,
                                uintptr_t param,
                                size_t local_size)
{
    vsf_eda_t *pthis = vsf_eda_get_cur();

    fsm_rt_t subcall_return_value = pthis->subcall_return_value;
    __vsf_eda_frame_state_t state   = {
        .feature.is_subcall_has_return_value    = true,
        .local_size                             = local_size,
    };
    switch(subcall_return_value) {
        case fsm_rt_on_going:
        case fsm_rt_wait_for_obj:
        //case fsm_rt_asyn:
            break;
        default:
            pthis->subcall_return_value = fsm_rt_on_going;
            return subcall_return_value;
    }


    __vsf_eda_call_eda_ex((uintptr_t)entry, param, state, true);

    /*! \note
     *  - if VSF_ERR_NOT_ENOUGH_RESOURCES is detected, yield and try it again
     *  (automatically). For tasks sharing the same frame pool, if the pool is
     *  too small, only task performance will be affected, and all sub-task call
     *  will work when frame is allocated.
     *  - if the frame is allocated and pushed to the stack, we should yield to
     *  let the sub-task run.
     *
     *  Since in either way, we will yield, no need to handle the return
     *  value of vsf_eda_call().
     */
    return fsm_rt_yield;
}

SECTION(".text.vsf.kernel.eda_task")
void __vsf_eda_task_evthandler_process_return_value(vsf_eda_t *pthis, vsf_evt_t evt)
{
    fsm_rt_t ret;
    VSF_KERNEL_ASSERT(     pthis != NULL
            &&  NULL != pthis->fn.frame
            &&  (uintptr_t)NULL != pthis->fn.frame->fn.func);

    uintptr_t param = pthis->fn.frame->ptr.target;
    if  (   ((uintptr_t)NULL == param)                             //!< no param
        &&  (0 == pthis->fn.frame->state.local_size)) {      //!< no local
        param = (uintptr_t )pthis;                           //!< it is a pure eda
    }

    vsf_task_entry_t entry = (vsf_task_entry_t)pthis->fn.frame->fn.func;
    ret = entry((uintptr_t)(pthis->fn.frame + 1), evt);
    pthis->subcall_return_value = ret;
    switch(ret) {
        default:            //! return fsm_rt_err
        case fsm_rt_asyn:   //! call sub fsm later
        case fsm_rt_cpl:
            if (vsf_eda_return()) {
                return ;
            }
            break ;
        case fsm_rt_wait_for_evt:
            //! delay, wait_for, mutex_pend, sem_pend and etc...
            return ;
        case fsm_rt_yield:
            __vsf_eda_yield();
            return;
    }
}

#   endif      // VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE
#endif      // VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL

#endif      // VSF_KERNEL_CFG_EDA_SUPPORT_TASK && VSF_USE_KERNEL
