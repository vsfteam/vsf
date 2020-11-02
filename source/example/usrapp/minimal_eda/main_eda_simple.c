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

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
dcl_vsf_peda_ctx(example_peda)

def_vsf_peda_ctx(example_peda,

    def_members(
        uint8_t StartLevel;
        uint32_t cnt;
        vsf_sem_t *sem_ptr;
    )
    end_def_members()

    def_args(
        uint8_t level;
    )
    end_def_params()

    def_locals(
        vsf_systimer_cnt_t time;
    )
    end_def_locals()
)
end_def_vsf_peda_ctx(example_peda)


#   if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED

declare_vsf_thread(caller_thread_t)

def_vsf_thread(caller_thread_t, 1024,

    features_used(
        mem_sharable( )
        mem_nonsharable( )
    )

    def_members(
        vsf_peda_param(example_peda) callee_param;
    )
    end_def_members()
);

declare_vsf_thread(caller_thread_t)
#   endif
#endif


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static NO_INIT vsf_sem_t __user_sem;
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
imp_vsf_peda(example_peda)
{
    vsf_peda_begin();

    switch (evt) {
        case VSF_EVT_INIT:

            vsf_local.time = vsf_systimer_get_tick();
            printf("\t\t\t\tEnter Level[%2d]\r\n", vsf_local.level);


            if (0 == vsf_local.level) {
                if (VSF_ERR_NONE != vsf_eda_sem_pend(vsf_this.sem_ptr, -1)) {
                    break;
                }
                goto label_get_sem;
            } else if (0 == vsf_this.StartLevel) {
                vsf_this.StartLevel = vsf_local.level;
            }

            //! fall through

        case VSF_EVT_YIELD: {
                vsf_peda_arg(example_peda) local = {.level = vsf_local.level - 1};
                if (VSF_ERR_NONE != vsf_call_peda(example_peda, &vsf_this, &local)) {
                    //! try again
                    vsf_eda_yield();
                }
            }

            break;

        case VSF_EVT_RETURN:
            printf("Use time [%2d]", vsf_systimer_tick_to_ms(vsf_systimer_get_tick() - vsf_local.time));
            printf("\t\tReturn To Level[%2d]\r\n", vsf_local.level);
            vsf_eda_return();

            break;

        case VSF_EVT_SYNC_CANCEL:
            //! cancelled (not used in this case)
        case VSF_EVT_TIMER:
            //! timeout   (not used in this case)
        case VSF_EVT_SYNC:
label_get_sem:
            //! we get the sem
            printf("receive semaphore...[%08x]\r\n", vsf_this.cnt++);
            vsf_eda_return();
            break;
    }

    vsf_peda_end();
}


#   if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED

implement_vsf_thread(caller_thread_t)
{
    while(1) {
        printf("call peda ------------------\r\n");
        const vsf_peda_arg(example_peda) local = {.level = 5};
        vsf_call_peda(example_peda, &vsf_this.callee_param, &local);

        printf("cpl-------------------------\r\n\r\n");
    }
}

#   endif

#else
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
#endif



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

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
    {
        static caller_thread_t __caller_thread;
        __caller_thread.callee_param.sem_ptr = &__user_sem;
        init_vsf_thread(caller_thread_t, &__caller_thread, vsf_prio_0);
    }
#   else
    {
        static example_peda __user_task;
        __user_task.param.sem_ptr = &__user_sem;
        __user_task.param.cnt = 0;

        vsf_sched_safe(){
            init_vsf_peda(example_peda, &__user_task, vsf_prio_0);

            vsf_peda_local(example_peda) *plocal =
                (vsf_peda_local(example_peda) *)vsf_eda_get_local(&__user_task);
            plocal->level = 5;
        }
    }
#   endif
#else
    //! start a user task
    {
        static NO_INIT user_task_t __user_task;
        const vsf_eda_cfg_t cfg = {
            .fn.evthandler = user_task_evthandler,
            .priority = vsf_prio_0,
        };
        __user_task.sem_ptr = &__user_sem;
        vsf_eda_start(&__user_task.use_as__vsf_eda_t, (vsf_eda_cfg_t *)&cfg);
    }
#endif

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
        vsf_teda_start(&__user_task_b, (vsf_eda_cfg_t *)&cfg);
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
