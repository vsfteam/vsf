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
#include "vsf.h"
#include <stdio.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
declare_vsf_pt(user_pt_task_t)
declare_vsf_pt(user_pt_sub_t)

#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
declare_vsf_task(user_sub_task_t)
#endif

def_vsf_pt(user_pt_sub_t,
    def_params(
        uint32_t cnt;
    ));

#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
def_vsf_task(user_sub_task_t,
    def_params(
        /*! \note when you want to use fsm_rt_asyn, you need a dedicated chState
         *        rather than using vsf_task_state by default.
         */
        uint8_t chState;        
        uint32_t cnt;
    ));
#endif

#if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
declare_vsf_thread(user_thread_a_t)
def_vsf_thread(user_thread_a_t, 1024);


#endif

def_vsf_pt(user_pt_task_t,
    def_params(
        vsf_sem_t *psem;
        uint32_t cnt;
        
        vsf_pt(user_pt_sub_t)       print_task;
    #if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
        vsf_task(user_sub_task_t)   progress_task;
    #endif
    #if     VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED                            \
        &&  VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
        vsf_thread(user_thread_a_t) thread_task;
    #endif
    ));
    


    
#if VSF_OS_CFG_RUN_MAIN_AS_THREAD != ENABLED
declare_vsf_pt(user_pt_task_b_t)
def_vsf_pt(user_pt_task_b_t,
    def_params(
        uint32_t cnt;
        vsf_sem_t *psem;
    ));
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static NO_INIT vsf_sem_t user_sem;
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
implement_vsf_thread(user_thread_a_t)
{
    printf("run thread...delay 100ms...");
    vsf_delay_ms(100);
    printf("cpl\r\n");
}
#endif


private implement_vsf_pt(user_pt_sub_t) 
{
    vsf_pt_begin();
   
    printf("receive semaphore...[%08x]\r\n", this.cnt++);
     
    vsf_pt_end();
}

#define RESET_FSM()     do {this.chState = 0;} while(0)

#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
private implement_vsf_task(user_sub_task_t)
{
    vsf_task_begin();
    enum {
        START,
        PRINT_PROGRESS,
    };
    
    /*! \note when you want to use fsm_rt_asyn, you need a dedicated chState
     *        rather than using vsf_task_state by default.
     */
    switch (this.chState) {
        case START:
            this.cnt = 0;
            this.chState = PRINT_PROGRESS;
            printf("\r\n[");
            //break;
        case PRINT_PROGRESS:
            if (this.cnt >= 100) {
                printf("]\r\n");
                RESET_FSM();
                return fsm_rt_cpl;
            }
            printf(".");
            this.cnt += 5;
            if (0 == (this.cnt % 25)) {
                return fsm_rt_asyn;
            }
            break;
    }
    
    vsf_task_end();
}
#endif

private implement_vsf_pt(user_pt_task_t) 
{
    vsf_pt_begin();

    this.cnt = 0;
    while(1) {
        vsf_pt_wait_until(vsf_sem_pend(this.psem));

    #if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
        this.print_task.cnt = this.cnt;                                         //!< Pass parameter
        vsf_pt_call_pt(user_pt_sub_t, &this.print_task);
        //! pt call complete
        this.cnt = this.print_task.cnt;                                         //!< read parameter
    #else
        printf("receive semaphore...[%08x]\r\n", this.cnt++);
    #endif
    
    #if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
        this.progress_task.chState = 0;
        do {
            fsm_rt_t ret;
            vsf_pt_call_task(user_sub_task_t, &this.progress_task, &ret);
            if (fsm_rt_cpl == ret) {
                break;
            } /* else if (fsm_rt_asyn == ret ) */
            printf("%2d%%", this.progress_task.cnt);
        } while(true);
    #endif
    
    #if     VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED                            \
        &&  VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
        vsf_pt_call_thread(user_thread_a_t, &(this.thread_task));
    #endif
        
    }

    vsf_pt_end();
}

#if VSF_OS_CFG_RUN_MAIN_AS_THREAD != ENABLED
private implement_vsf_pt(user_pt_task_b_t) 
{
    vsf_pt_begin();
    
    while(1) {
        vsf_pt_wait_until( vsf_delay_ms(10000) );                               //!< wait 10s
        printf("post semaphore...   [%08x]\r\n", this.cnt++);
        vsf_sem_post(this.psem);                                                //!< post a semaphore
    }
    
    vsf_pt_end();
}
#endif

void vsf_kernel_task_cross_call_demo(void)
{  
    //! initialise semaphore
    vsf_sem_init(&user_sem, 0); 
    
    //! start a user task
    {
        static NO_INIT user_pt_task_t __user_pt;
        __user_pt.param.psem = &user_sem;
        init_vsf_pt(user_pt_task_t, &__user_pt, vsf_priority_inherit);
    };

#if VSF_OS_CFG_RUN_MAIN_AS_THREAD == ENABLED
    uint32_t cnt = 0;
    while(1) {
        vsf_delay_ms(10000);
        printf("post semaphore...   [%08x]\r\n", cnt++);
        vsf_sem_post(&user_sem);            //!< post a semaphore
    }
#else
    //! in this case, we only use main to initialise vsf_tasks

    //! start a user task b
    {
        static NO_INIT user_pt_task_b_t __user_pt_task_b;
        __user_pt_task_b.param.psem = &user_sem;
        __user_pt_task_b.param.cnt = 0;
        init_vsf_pt(user_pt_task_b_t, &__user_pt_task_b, vsf_priority_0);
    }
#endif
}

#if VSF_PROJ_CFG_USE_CUBE != ENABLED
int main(void)
{
    static_task_instance(
        features_used(
            mem_sharable( )
            mem_nonsharable( )
        )
    )

    vsf_stdio_init();
    
    vsf_kernel_task_cross_call_demo();
    
#if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
    while(1) {
        printf("hello world! \r\n");
        vsf_delay_ms(1000);
    }
#else
    return 0;
#endif
}

#endif

