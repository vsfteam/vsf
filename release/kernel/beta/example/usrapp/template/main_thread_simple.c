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
declare_vsf_thread(user_task_t)

def_vsf_thread(user_task_t, 512,

    features_used(
        mem_sharable( using_grouped_evt; )
        mem_nonsharable( )
    )
    
    def_params(
        vsf_sem_t *psem;
    ));

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static NO_INIT vsf_sem_t user_sem;
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_OS_RUN_MAIN_AS_THREAD != ENABLED
#error In order to run this demo, please set VSF_OS_RUN_MAIN_AS_THREAD to ENABLED
#endif

implement_vsf_thread(user_task_t) 
{
    uint32_t cnt = 0;
    while (1) {
        vsf_sem_pend(this.psem, -1);        //! wait for semaphore forever
        printf("receive semaphore from main...[%08x]\r\n", cnt++);
    }
}



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
        init_vsf_thread(user_task_t, &__user_task, vsf_priority_0);
    } while(0);

    
    while(1) {
        printf("hello world! \r\n");
        vsf_delay(10000);
        vsf_sem_post(&user_sem);            //!< post a semaphore
    }
    
}