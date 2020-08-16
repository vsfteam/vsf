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
declare_vsf_thread(user_thread_a_t)

def_vsf_thread(user_thread_a_t, 1024,

    features_used(
        mem_sharable( )
        mem_nonsharable( )
    )
    
    def_params(
        vsf_sem_t *sem_ptr;
    ));

declare_vsf_thread(user_thread_b_t)

def_vsf_thread(user_thread_b_t, 1024,

    features_used(
        mem_sharable( )
        mem_nonsharable( )
    )
    
    def_params(
        vsf_sem_t *sem_ptr;
    ));

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static NO_INIT vsf_sem_t user_sem;
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


void vsf_kernel_thread_simple_demo(void)
{    
    //! initialise semaphore
    vsf_sem_init(&user_sem, 0); 
    
    //! start the user task a
    {
        static NO_INIT user_thread_a_t __user_task_a;
        __user_task_a.param.sem_ptr = &user_sem;
        init_vsf_thread(user_thread_a_t, &__user_task_a, vsf_prio_0);
    }
    
    //! start the user task b
    {
        static NO_INIT user_thread_b_t __user_task_b;
        __user_task_b.param.sem_ptr = &user_sem;
        init_vsf_thread(user_thread_b_t, &__user_task_b, vsf_prio_0);
    }
}

implement_vsf_thread(user_thread_a_t) 
{
    uint32_t cnt = 0;
    while (1) {
        vsf_delay_ms(1000);
        printf("post semaphore...     [%08x]\r\n", cnt++);
        vsf_sem_post(this.sem_ptr);            //!< post a semaphore
    }
}

implement_vsf_thread(user_thread_b_t) 
{
    uint32_t cnt = 0;
    while (1) {
        vsf_sem_pend(this.sem_ptr);            //! wait for semaphore forever
        printf("receive semaphore...  [%08x]\r\n", cnt++);
    }
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
    
    vsf_kernel_thread_simple_demo();
    
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
    
    vsf_kernel_thread_simple_demo();
    
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
    
    vsf_kernel_thread_simple_demo();
    
    return 0;
}

#endif
#endif
