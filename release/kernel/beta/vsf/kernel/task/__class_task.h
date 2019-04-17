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

//#ifndef __VSF_KERNEL_CLASS_TASK_H__     /* deliberately comment this out! */
//#define __VSF_KERNEL_CLASS_TASK_H__     /* deliberately comment this out! */

/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file 
 *!        included in this file
 */
#define __VSF_CLASS_USE_STRICT_TEMPLATE__
#include "utilities/ooc_class.h"


declare_class(vsf_task_t)

def_class(vsf_task_t, 
    which(
#if VSF_CFG_TIMER_EN == ENABLED
        implement(vsf_teda_t)
#else
        implement(vsf_eda_t)
#endif
    ))
    
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
    
end_def_class(vsf_task_t)



//#endif                                  /* deliberately comment this out! */




