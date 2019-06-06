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
#include "vsf.h"
#include <stdio.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
declare_vsf_task(user_task_t)
declare_vsf_task(user_sub_task_t)

def_vsf_task(user_sub_task_t,
    def_params(
        uint32_t cnt;
    ));

def_vsf_task(user_task_t,
    def_params(
        vsf_sem_t *psem;
        uint32_t cnt;
        
        vsf_task(user_sub_task_t) print_task;
        
    ));
                                                       


#if VSF_OS_RUN_MAIN_AS_THREAD != ENABLED
declare_vsf_task(user_task_b_t)
def_vsf_task(user_task_b_t,
    def_params(
        vsf_sem_t *psem;
        uint32_t cnt;
    ));
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static NO_INIT vsf_sem_t user_sem;
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

implement_vsf_task(user_sub_task_t) 
{
    vsf_task_begin();
    printf("receive semaphore...[%08x]\r\n", this.cnt++);
    return fsm_rt_cpl;                  //!< return to caller
    vsf_task_end();
}


#define USER_TASK_RESET_FSM()   do { vsf_task_state = 0;} while(0)

implement_vsf_task(user_task_t) 
{
    vsf_task_begin();
    enum {
        WAIT_FOR_SEM = 0,
        CALL_SUB_TO_PRINT,
    };
    
    on_vsf_task_init() {
        this.cnt = 0;
    }

    switch (vsf_task_state) {
        case WAIT_FOR_SEM:    
            vsf_task_wait_until(vsf_sem_pend(this.psem));                       //!< wait for semaphore forever                                                                                  
            this.print_task.cnt = this.cnt;                                     //!< passing parameter
            vsf_task_state = CALL_SUB_TO_PRINT;                                 //!< tranfer to next state
            
            break;
        case CALL_SUB_TO_PRINT:
            if (fsm_rt_cpl == vsf_call_task(user_sub_task_t, 
                                            &this.print_task)) {
                //! task complete
                this.cnt = this.print_task.cnt;                                 //!< read param value
                USER_TASK_RESET_FSM();                                          //!< reset fsm
            }
            break;
    }
    vsf_task_end();
}

#if VSF_OS_RUN_MAIN_AS_THREAD != ENABLED
implement_vsf_task(user_task_b_t) 
{
    enum {
        PRINT = 0,
        DELAY,
    };
    
    switch(this.chState) {
        case PRINT:
            printf("post semaphore...   [%08x]\r\n", this.cnt++);
            this.chState = DELAY;
            //break;
        case DELAY:
            vsf_task_wait_until(
                vsf_delay_ms(10000){                                               //!< wait 10s
                    vsf_sem_post(this.psem);                                    //!< post a semaphore
                    USER_TASK_RESET_FSM();                                      //!< reset fsm
                }
            );
            break;
    }
    
    return fsm_rt_on_going;
}
#endif

void vsf_kernel_task_simple_demo(void)
{   
    //! initialise semaphore
    vsf_sem_init(&user_sem, 0); 
    
    //! start a user task
    {
        static NO_INIT user_task_t __user_task;
        __user_task.param.psem = &user_sem;
        init_vsf_task(user_task_t, &__user_task, vsf_priority_0);
    }

#if VSF_OS_RUN_MAIN_AS_THREAD == ENABLED
    uint32_t cnt = 0;
    while(1) {
        printf("post semaphore...   [%08x]\r\n", cnt++);
        vsf_delay_ms(10000);
        vsf_sem_post(&user_sem);            //!< post a semaphore
    }
#else
    //! in this case, we only use main to initialise vsf_tasks

    //! start a user task b
    {
        static NO_INIT user_task_b_t __user_task_b;
        __user_task_b.param.psem = &user_sem;
        __user_task_b.param.cnt = 0;
        init_vsf_task(user_task_b_t, &__user_task_b, vsf_priority_0);
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
    
    vsf_kernel_task_simple_demo();
    
#if VSF_OS_RUN_MAIN_AS_THREAD == ENABLED
    while(1) {
        printf("hello world! \r\n");
        vsf_delay_ms(1000);
    }
#else
    return 0;
#endif
}

#endif
