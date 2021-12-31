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
#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
dcl_vsf_pt(user_pt_task_t)
dcl_vsf_pt(user_pt_sub_t)
dcl_vsf_pt(user_pt_called_by_thread_t)

#if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
dcl_vsf_task(user_sub_task_t)
#endif

def_vsf_pt(user_pt_sub_t,
    def_params(
        uint32_t cnt;
    ));

def_vsf_pt(user_pt_called_by_thread_t,
    def_params(
        uint32_t cnt;
    ));

#if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
def_vsf_task(user_sub_task_t,
    def_params(
        uint32_t cnt;
    ));
#endif

#if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
dcl_vsf_thread(user_thread_b_t)
def_vsf_thread(user_thread_b_t, 1024,
    def_params(
        vsf_pt(user_pt_called_by_thread_t) pt_task;
    ));


#endif

def_vsf_pt(user_pt_task_t,
    def_params(
        vsf_sem_t *sem_ptr;
        uint32_t cnt;

        vsf_pt(user_pt_sub_t)       print_task;
    #if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
        vsf_task(user_sub_task_t)   progress_task;
    #endif
    #if     VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED                            \
        &&  VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
        vsf_thread(user_thread_b_t) thread_task;
    #endif
    ));




#if     VSF_OS_CFG_MAIN_MODE != VSF_OS_CFG_MAIN_MODE_THREAD                     \
    ||  VSF_KERNEL_CFG_SUPPORT_THREAD != ENABLED
dcl_vsf_pt(user_pt_task_b_t)
def_vsf_pt(user_pt_task_b_t,
    def_params(
        uint32_t cnt;
        vsf_sem_t *sem_ptr;
    ));
#elif VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
dcl_vsf_thread(user_thread_a0_t)

def_vsf_thread(user_thread_a0_t, 1024,

    features_used(
        mem_sharable( )
        mem_nonsharable( )
    )

    def_params(
        vsf_sem_t *sem_ptr;
    ));
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static NO_INIT vsf_sem_t __user_sem;
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

imp_vsf_pt(user_pt_called_by_thread_t)
{
    vsf_pt_begin();

    vsf_trace_info("\t\t\trun pt task: delay 1000ms\r\n");
    vsf_pt_wait_until(vsf_delay_ms(100));
    vsf_this.cnt++;
    vsf_trace_info("\t\t\tdelay complete [0x%08x]\r\n", vsf_this.cnt);

    vsf_pt_end();
}

#if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
imp_vsf_thread(user_thread_b_t)
{
    vsf_trace_info("\trun thread...delay 100ms...");
    vsf_delay_ms(100);
    vsf_trace_info("\tcpl\r\n");

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    vsf_trace_info("\t\tcall pt task\r\n");
    vsf_thread_call_pt( user_pt_called_by_thread_t, &vsf_this.pt_task);
    vsf_trace_info("\t\treturn from pt task\r\n");
#endif
}
#endif


imp_vsf_pt(user_pt_sub_t)
{
    vsf_pt_begin();

    vsf_trace_info("receive semaphore...[%08x]\r\n", vsf_this.cnt++);

    vsf_pt_end();
}

#define RESET_FSM()     do {vsf_task_state = 0;} while(0)

#if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
imp_vsf_task(user_sub_task_t)
{
    vsf_task_begin();
    enum {
        START,
        PRINT_PROGRESS,
    };

    /*! \note when you want to use fsm_rt_asyn, you need a dedicated chState
     *        rather than using vsf_task_state by default.
     */
    switch (vsf_task_state) {
        case START:
            vsf_this.cnt = 0;
            vsf_task_state = PRINT_PROGRESS;
            vsf_trace_info("\r\n[");
            //break;
        case PRINT_PROGRESS:
            if (vsf_this.cnt >= 100) {
                vsf_trace_info("]\r\n");
                RESET_FSM();
                return fsm_rt_cpl;
            }
            vsf_trace_info(".");
            vsf_this.cnt += 5;
            if (0 == (vsf_this.cnt % 25)) {
                return fsm_rt_asyn;
            }
            break;
    }

    vsf_task_end();
}
#endif

imp_vsf_pt(user_pt_task_t)
{
    vsf_pt_begin();

    vsf_this.cnt = 0;
    while(1) {
        vsf_pt_wait_until(vsf_sem_pend(vsf_this.sem_ptr));

    #if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
        vsf_this.print_task.cnt = vsf_this.cnt;                                         //!< Pass parameter
        vsf_pt_call_pt(user_pt_sub_t, &vsf_this.print_task);
        //! pt call complete
        vsf_this.cnt = vsf_this.print_task.cnt;                                         //!< read parameter
    #else
        vsf_trace_info("receive semaphore...[%08x]\r\n", vsf_this.cnt++);
    #endif

    #if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
        prepare_vsf_task(user_sub_task_t,&vsf_this.progress_task);
        do {
            fsm_rt_t ret;
            vsf_pt_call_task(user_sub_task_t, &vsf_this.progress_task, &ret);
            if (fsm_rt_cpl == ret) {
                break;
            } /* else if (fsm_rt_asyn == ret ) */
            vsf_trace_info("%2d%%", vsf_this.progress_task.cnt);
        } while(true);
    #endif

    #if     VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED                            \
        &&  VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
        vsf_pt_call_thread(user_thread_b_t, &(vsf_this.thread_task));
    #endif

    }

    vsf_pt_end();
}

#if     VSF_OS_CFG_MAIN_MODE != VSF_OS_CFG_MAIN_MODE_THREAD                     \
    ||  VSF_KERNEL_CFG_SUPPORT_THREAD != ENABLED
imp_vsf_pt(user_pt_task_b_t)
{
    vsf_pt_begin();

    while(1) {
        vsf_pt_wait_until( vsf_delay_ms(3000) );                               //!< wait 10s
        vsf_trace_info("post semaphore...   [%08x]\r\n", vsf_this.cnt++);
        vsf_sem_post(vsf_this.sem_ptr);                                                //!< post a semaphore
    }

    vsf_pt_end();
}
#else
imp_vsf_thread(user_thread_a0_t)
{
    uint32_t cnt = 0;
    while (1) {
        vsf_delay_ms(3000);
        vsf_trace_info("post semaphore...   [%08x]\r\n", cnt++);
        vsf_sem_post(&__user_sem);            //!< post a semaphore
    }
}

#endif

void vsf_kernel_cross_call_demo(void)
{
    //! initialise semaphore
    vsf_sem_init(&__user_sem, 0);

    //! start a user task
    {
        static NO_INIT user_pt_task_t __user_pt;
        memset(&__user_pt, 0, sizeof(user_pt_task_t));
        __user_pt.param.sem_ptr = &__user_sem;
        init_vsf_pt(user_pt_task_t, &__user_pt, vsf_prio_0);
    };

#if     VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED                                \
    &&  VSF_OS_CFG_MAIN_MODE == VSF_OS_CFG_MAIN_MODE_THREAD

    //! start the user task a
    {
        static NO_INIT user_thread_a0_t __user_task_a;
        __user_task_a.param.sem_ptr = &__user_sem;
        init_vsf_thread(user_thread_a0_t, &__user_task_a, vsf_prio_0);
    }
#else
    //! in this case, we only use main to initialise vsf_tasks

    //! start a user task b
    {
        static NO_INIT user_pt_task_b_t __user_pt_task_b;
        __user_pt_task_b.param.sem_ptr = &__user_sem;
        __user_pt_task_b.param.cnt = 0;
        init_vsf_pt(user_pt_task_b_t, &__user_pt_task_b, vsf_prio_0);
    }
#endif
}

#if APP_USE_LINUX_DEMO == ENABLED
int kernel_cross_task_test_main(int argc, char *argv[])
{
#else
int VSF_USER_ENTRY(void)
{
#   if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#       if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#       endif
#   endif
#endif

    static_task_instance(
        features_used(
            mem_sharable( )
            mem_nonsharable( )
        )
    )

    vsf_stdio_init();

    vsf_kernel_cross_call_demo();

#if APP_USE_LINUX_DEMO == ENABLED
    while (true) {
        vsf_trace_info("hello world! \r\n");
        vsf_delay_ms(1000);
    }
#endif
    return 0;
}
