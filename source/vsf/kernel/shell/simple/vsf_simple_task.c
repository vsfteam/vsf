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
#include "kernel/vsf_kernel_cfg.h"


#if VSF_KERNEL_USE_SIMPLE_SHELL == ENABLED && VSF_USE_KERNEL == ENABLED
#define __VSF_EDA_CLASS_INHERIT__
#include "../../vsf_kernel_common.h"
#include "./vsf_simple.h"
#include "../../task/vsf_thread.h"
#include "../../vsf_eda.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED

#if __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#endif

SECTION(".text.vsf.kernel.__vsf_delay")
vsf_evt_t __vsf_delay(uint_fast32_t tick)
{
    vsf_evt_t result = VSF_EVT_INVALID;
    enum {
        VSF_APP_STATE_SET_TIMER = 0,
        VSF_APP_STATE_WAIT_TIMER_EVT,
    };
    if (0 == tick) {
        return VSF_EVT_TIMER;
    }
    vsf_teda_t *pteda = (vsf_teda_t *)vsf_eda_get_cur();
    VSF_KERNEL_ASSERT(NULL != pteda);

#   if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
    if (vsf_eda_is_stack_owner(&(pteda->use_as__vsf_eda_t))) {
        vsf_thread_delay(tick);
        result = vsf_eda_get_cur_evt();
    } else
#   endif
    {
        if (vsf_eda_polling_state_get(&(pteda->use_as__vsf_eda_t))) {
            /* VSF_APP_STATE_WAIT_TIMER_EVT */
            result = vsf_eda_get_cur_evt();
            if (result == VSF_EVT_TIMER) {
                vsf_eda_polling_state_set(  &(pteda->use_as__vsf_eda_t),
                                            (bool)VSF_APP_STATE_SET_TIMER);
            }
        } else  {
            /*VSF_APP_STATE_SET_TIMER*/
            vsf_teda_set_timer(tick);
            vsf_eda_polling_state_set(  &(pteda->use_as__vsf_eda_t),
                                        (bool)VSF_APP_STATE_WAIT_TIMER_EVT);
        }
    }

    return result;
}

#endif

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED

#if __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif



SECTION(".text.vsf.kernel.__vsf_sem_pend")
vsf_sync_reason_t __vsf_sem_pend(vsf_sem_t *sem_ptr, int_fast32_t time_out)
{
    vsf_sync_reason_t result = VSF_SYNC_PENDING;
    vsf_err_t err;
    enum {
        VSF_APP_STATE_PENDING = 0,
        VSF_APP_STATE_WAIT_PENDING,
    };


    vsf_eda_t *eda = vsf_eda_get_cur();
    VSF_KERNEL_ASSERT(NULL != eda);

    do {
#if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
        if (vsf_eda_is_stack_owner(eda)) {
            result = vsf_thread_sem_pend(sem_ptr, time_out);
        } else
#endif
        {
            if (vsf_eda_polling_state_get(eda)) {
                /* VSF_APP_STATE_WAIT_PENDING */
                result = vsf_eda_sync_get_reason(sem_ptr, vsf_eda_get_cur_evt());
                if (result != VSF_SYNC_PENDING) {
                    vsf_eda_polling_state_set(  eda,
                                                (bool)VSF_APP_STATE_PENDING);
                }
            } else {
                /* VSF_APP_STATE_PENDING */
                err = vsf_eda_sem_pend(sem_ptr, time_out);
                if (!err) {
                    result = VSF_SYNC_GET;
                    break;
                } else if (err < 0) {
                    result = VSF_SYNC_FAIL;
                    break;
                }
                vsf_eda_polling_state_set(  eda,
                                            (bool)VSF_APP_STATE_WAIT_PENDING);
            }
        }
    } while(false);

    return result;
}

SECTION(".text.vsf.kernel.vsf_mutex_enter")
vsf_sync_reason_t __vsf_mutex_enter(vsf_mutex_t *mtx_ptr, int_fast32_t time_out)
{
    VSF_KERNEL_ASSERT(NULL != mtx_ptr);
    return __vsf_sem_pend(&(mtx_ptr->use_as__vsf_sync_t), time_out);
}

#endif

SECTION(".text.vsf.kernel.vsf_yield")
vsf_evt_t __vsf_yield(void)
{
    vsf_evt_t result = VSF_EVT_YIELD;
    vsf_eda_t *eda = vsf_eda_get_cur();
    VSF_KERNEL_ASSERT(NULL != eda);

    enum {
        VSF_APP_STATE_YIELD = 0,
        VSF_APP_STATE_WAIT_EVENT,
    };

#if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
    if (vsf_eda_is_stack_owner(eda)) {
        vsf_eda_yield();
        vsf_thread_wait();          //!< wait for any evt
    } else
#endif
    {
        if (vsf_eda_polling_state_get(eda)) {
            /* VSF_APP_STATE_WAIT_EVENT */
            vsf_eda_polling_state_set(eda, VSF_APP_STATE_YIELD);
        } else {
            vsf_eda_yield();
            /* VSF_APP_STATE_YIELD */
            vsf_eda_polling_state_set(eda, VSF_APP_STATE_WAIT_EVENT);
            result = VSF_EVT_NONE;
        }
    }
    return result;
}

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED

SECTION(".text.vsf.kernel.vsf_call_eda_ex")
extern vsf_err_t __vsf_call_eda(uintptr_t evthandler,
                                uintptr_t param,
                                size_t local_size,
                                size_t local_buff_size,
                                uintptr_t local_buff)
{
    vsf_eda_t *eda = vsf_eda_get_cur();
    VSF_KERNEL_ASSERT(NULL != eda);

#if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
    if (vsf_eda_is_stack_owner(eda)) {
        return vk_thread_call_eda(  evthandler,
                                    param,
                                    local_size,
                                    local_buff_size,
                                    local_buff );
    } else
#endif
    {
        vsf_err_t err = __vsf_eda_call_eda(evthandler, param, local_size);
        if ((VSF_ERR_NONE == err) && ((uintptr_t)NULL != local_buff)) {
            size_t size = min(local_buff_size, local_size);
            if (size > 0) {
                uintptr_t local = vsf_eda_get_local();
                memcpy((void *)local, (void *)local_buff, size);
            }
        }
        return err;
    }
}

SECTION(".text.vsf.kernel.__vsf_call_fsm")
fsm_rt_t __vsf_call_fsm(vsf_fsm_entry_t entry, uintptr_t param, size_t local_size)
{
    vsf_eda_t *eda = vsf_eda_get_cur();
    VSF_KERNEL_ASSERT(NULL != eda);

#if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
    if (vsf_eda_is_stack_owner(eda)) {
        return vk_thread_call_fsm(entry, param, local_size);
    } else
#endif
    {
        return __vsf_eda_call_fsm(entry, param, local_size);
    }
}
#endif

#endif

/*EOF*/
