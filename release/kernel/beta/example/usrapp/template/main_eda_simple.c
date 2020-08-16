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
#include "app_cfg.h"
#include <stdio.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef struct user_task_t user_task_t;
struct user_task_t {
    implement(vsf_eda_t)
    
    vsf_sem_t *sem_ptr;
    uint32_t cnt;
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static NO_INIT vsf_sem_t __user_sem;
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void user_task_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    user_task_t *pthis = (user_task_t *)eda;
    ASSERT(NULL != eda);
    
    switch (evt) {
        case VSF_EVT_INIT:
            pthis->cnt = 0;
label_loop_start:
            if (VSF_ERR_NONE != vsf_eda_sem_pend(pthis->sem_ptr, -1)) {
                break;
            }
            //! fall through
        
        case VSF_EVT_SYNC_CANCEL:
            //! cancelled (not used in this case)
        case VSF_EVT_TIMER:
            //! timeout   (not used in this case)
        case VSF_EVT_SYNC:
            //! we get the sem
            printf("receive semaphore...[%08x]\r\n", pthis->cnt++);
            goto label_loop_start;
    }
}

#if   VSF_OS_CFG_MAIN_MODE != VSF_OS_CFG_MAIN_MODE_EDA                        \
    &&  !(  VSF_OS_CFG_MAIN_MODE == VSF_OS_CFG_MAIN_MODE_IDLE                   \
        &&  VSF_OS_CFG_ADD_EVTQ_TO_IDLE == ENABLED)
static void user_task_b_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    static uint32_t cnt = 0;
    
    switch(evt) {
        case VSF_EVT_INIT:
label_loop_start:
            vsf_teda_set_timer_ms(3000);
            break;
        case VSF_EVT_TIMER:
            printf("post semaphore...   [%08x]\r\n", cnt++);
            vsf_eda_sem_post(&__user_sem);                                      //!< post a semaphore
            goto label_loop_start;
    }
}
#endif

void vsf_kernel_eda_simple_demo(void)
{   
    //! initialise semaphore
    vsf_eda_sem_init(&__user_sem, 0);
    
    //! start a user task b
    {
        static NO_INIT user_task_t __user_task_t;
        const vsf_eda_cfg_t cfg = {
            .fn.evthandler = user_task_evthandler,
            .priority = vsf_prio_0,
        };
        __user_task_t.sem_ptr = &__user_sem;
        vsf_eda_init_ex(&__user_task_t.use_as__vsf_eda_t, (vsf_eda_cfg_t *)&cfg);
    }

#if VSF_OS_CFG_MAIN_MODE == VSF_OS_CFG_MAIN_MODE_THREAD
    uint32_t cnt = 0;
    while(1) {
        vsf_delay_ms(3000);
        printf("post semaphore...   [%08x]\r\n", cnt++);
        vsf_eda_sem_post(&__user_sem);                                          //!< post a semaphore
    }
#elif   VSF_OS_CFG_MAIN_MODE != VSF_OS_CFG_MAIN_MODE_EDA                        \
    &&  !(  VSF_OS_CFG_MAIN_MODE == VSF_OS_CFG_MAIN_MODE_IDLE                   \
        &&  VSF_OS_CFG_ADD_EVTQ_TO_IDLE == ENABLED)
    //! in this case, we only use main to initialise vsf_tasks

    //! start a user task b
    {
        static NO_INIT vsf_teda_t __user_task_b;
        const vsf_eda_cfg_t cfg = {
            .fn.evthandler = user_task_b_evthandler,
            .priority = vsf_prio_0,
        };
        vsf_teda_init_ex(&__user_task_b, (vsf_eda_cfg_t *)&cfg);
    }
#endif
}

#if VSF_PROJ_CFG_USE_CUBE != ENABLED
#if VSF_OS_CFG_MAIN_MODE == VSF_OS_CFG_MAIN_MODE_THREAD
int main(void)
{
    static_task_instance(
        features_used(
            mem_sharable( )
            mem_nonsharable( )
        )
    )

    vsf_stdio_init();
    
    vsf_kernel_eda_simple_demo();
    
    while(1) {
        printf("hello world! \r\n");
        vsf_delay_ms(1000);
    }
}
#elif   VSF_OS_CFG_MAIN_MODE == VSF_OS_CFG_MAIN_MODE_EDA                        \
    ||  (   VSF_OS_CFG_MAIN_MODE == VSF_OS_CFG_MAIN_MODE_IDLE                   \
        &&  VSF_OS_CFG_ADD_EVTQ_TO_IDLE == ENABLED)
void main(void)
{
    static_task_instance(
        features_used(
            mem_sharable( )
            mem_nonsharable( )
        )
        def_params(
            uint32_t cnt;
        )
    )

    vsf_pt_begin()

    vsf_stdio_init();
    
    vsf_kernel_eda_simple_demo();
    
    this.cnt = 0;
    while(1) {
        printf("hello world! \r\n");
        vsf_pt_wait_until(vsf_delay_ms(1000));
    }
    vsf_pt_end()
}
#else 
int main(void)
{
    vsf_stdio_init();
    
    vsf_kernel_eda_simple_demo();
    
    return 0;
}

#endif
#endif
