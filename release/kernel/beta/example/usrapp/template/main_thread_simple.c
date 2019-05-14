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
declare_vsf_thread(user_thread_t)

def_vsf_thread(user_thread_t, 512,

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

implement_vsf_thread(user_thread_t) 
{
    uint32_t cnt = 0;
    while (1) {
        vsf_sem_pend(this.psem);        //! wait for semaphore forever
        printf("receive semaphore from main...[%08x]\r\n", cnt++);
    }
}


void vsf_kernel_thread_simple_demo(void)
{
    static_task_instance(
        features_used(
            mem_sharable( )
            mem_nonsharable( )
        )
    )
    
    //! initialise semaphore
    vsf_sem_init(&user_sem, 0); 
    
    //! start a user task
    do {
        static NO_INIT user_thread_t __user_task;
#if __IS_COMPILER_ARM_COMPILER_5__
        __user_task.use_as__vsf_thread_user_thread_t_t.psem = &user_sem;
#else
        __user_task.psem = &user_sem;
#endif
        init_vsf_thread(user_thread_t, &__user_task, vsf_priority_0);
    } while(0);

    
    while(1) {
        printf("hello world! \r\n");
        vsf_delay_ms(1000);
        vsf_sem_post(&user_sem);            //!< post a semaphore
    }
    
}

#if VSF_PROJ_CFG_USE_CUBE != ENABLED
int main(void)
{
    vsf_stdio_init();
    
    vsf_kernel_thread_simple_demo();
    
    while(1);
}

#endif