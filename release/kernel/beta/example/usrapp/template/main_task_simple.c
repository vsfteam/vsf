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
        uint8_t state;
        
        vsf_task(user_sub_task_t) print_task;
        
    ));
    
#if VSF_OS_RUN_MAIN_AS_THREAD != ENABLED
declare_vsf_task(user_task_b_t)
def_vsf_task(user_task_b_t,
    def_params(
        vsf_sem_t *psem;
        uint8_t state;
    ));
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static NO_INIT vsf_sem_t user_sem;
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

implement_vsf_task(user_sub_task_t) 
{
    printf("receive semaphore from main...[%08x]\r\n", this.cnt++);
    return fsm_rt_cpl;                  //!< return to caller
}


#define USER_TASK_RESET_FSM()   do { this.state = 0;} while(0)

implement_vsf_task(user_task_t) 
{
    enum {
        WAIT_FOR_SEM = 0,
        CALL_SUB_TO_PRINT,
    };
    on_vsf_task_init() {
        this.state = 0;
        this.cnt = 0;
    }

    switch (this.state) {
        case WAIT_FOR_SEM:
            /*! \note IMPORTANT!!!!!
             *        For anything you want to wait, which is coming from vsf 
             *        system, please use vsf_task_wait_until().
             *        Those includue: VSF_EVT_XXXXX, sempahore, mutex and etc.
             *        
             *        - For VSF_EVT_XXXX, please use:
             *        vsf_task_wait_until(
             *            on_vsf_task_evt(VSF_EVT_XXXXX) {
             *                //! when the target VSF_EVT_XXXX arrived
             *                ...
             *            }
             *        )
             *        - For sempahore, please use:
             *        vsf_task_wait_until(
             *            vsf_sem_pend(...) {
             *                //! when the semaphore is acquired
             *                ...
             *            }on_sem_timeout() {
             *                //! when timeout
             *            }
             *        )
             *        - For time, please use:
             *        vsf_task_wait_until(
             *            vsf_delay(...) {
             *                //! when timeout
             *                ...
             *            )
             *        )
             */
            vsf_task_wait_until(
                vsf_sem_pend(this.psem, -1){                                    //!< wait for semaphore forever
                    this.print_task.cnt = this.cnt;                             //!< passing parameter
                    this.state = CALL_SUB_TO_PRINT;                             //!< tranfer to next state
                }
            );
            break;
        case CALL_SUB_TO_PRINT:
            if (fsm_rt_cpl == 
                vsf_task_call_sub(user_sub_task_t, &this.print_task)) {
                //! task complete
                this.cnt = this.print_task.cnt;                                 //!< read param value
                USER_TASK_RESET_FSM();                                          //!< reset fsm
            }
            break;
    }
    return fsm_rt_on_going;
}

#if VSF_OS_RUN_MAIN_AS_THREAD != ENABLED
implement_vsf_task(user_task_b_t) 
{
    enum {
        PRINT = 0,
        DELAY,
    };
    
    switch(this.state) {
        case PRINT:
            printf("hello world! \r\n");
            this.state = DELAY;
            //break;
        case DELAY:
            vsf_task_wait_until(
                vsf_delay(10000){                                               //!< wait 10s
                    vsf_sem_post(this.psem);                                    //!< post a semaphore
                    USER_TASK_RESET_FSM();                                      //!< reset fsm
                }
            );
            break;
    }
    
    return fsm_rt_on_going;
}
#endif

static void system_init(void)
{
    extern void uart_config(void);
    uart_config();
}

int main(void)
{
    static_task_instance(
        features_used(
            mem_sharable( )
            mem_nonsharable( )
        )
    )
    
    system_init();
    
    //! initialise semaphore
    vsf_sem_init(&user_sem, 0); 
    
    //! start a user task
    do {
        static NO_INIT user_task_t __user_task;
        __user_task.psem = &user_sem;
        init_vsf_task(user_task_t, &__user_task, vsf_priority_0);
    } while(0);

#if VSF_OS_RUN_MAIN_AS_THREAD == ENABLED
    while(1) {
        printf("hello world! \r\n");
        vsf_delay(10000);
        vsf_sem_post(&user_sem);            //!< post a semaphore
    }
#else
    //! in this case, we only use main to initialise vsf_tasks

    //! start a user task b
    do {
        static NO_INIT user_task_b_t __user_task_b;
        __user_task_b.psem = &user_sem;
        __user_task_b.state = 0;
        init_vsf_task(user_task_b_t, &__user_task_b, vsf_priority_0);
    } while(0);
    
    return 0;
#endif
}