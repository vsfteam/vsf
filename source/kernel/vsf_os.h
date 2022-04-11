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

#ifndef __VSF_OS_H__
#define __VSF_OS_H__

/*============================ INCLUDES ======================================*/
#include "kernel/vsf_kernel_cfg.h"

#if VSF_USE_KERNEL == ENABLED
//#include "./vsf_kernel_common.h"
#include "./vsf_evtq.h"
#include "./task/vsf_task.h"

// for VSF_USER_ENTRY macro if exists in arch
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_USER_ENTRY
#   ifdef VSF_ARCH_ENTRY_NO_PENDING
#       define VSF_USER_ENTRY               user_main
#   else
#       define VSF_USER_ENTRY               main
#   endif
#endif

#if VSF_OS_CFG_PRIORITY_NUM > 1

#   define VSF_SCHED_SAFE_CODE_REGION       VSF_FORCED_SCHED_SAFE_CODE_REGION
#   define __vsf_sched_safe(...)            __vsf_forced_sched_safe(__VA_ARGS__)

#   define vsf_sched_lock()                 vsf_forced_sched_lock()
#   define vsf_sched_unlock(__level)        vsf_forced_sched_unlock((vsf_sched_lock_status_t)(__level))
#   define vsf_sched_safe()                 vsf_forced_sched_safe()
#   define vsf_sched_safe_exit()            vsf_forced_sched_safe_exit()

#else

#   define VSF_SCHED_SAFE_CODE_REGION       DEFAULT_CODE_REGION_NONE
#   define vsf_sched_lock()                 0
#   define vsf_sched_unlock(__level)        VSF_UNUSED_PARAM(__level)
#   define vsf_sched_safe()                 if (1)
#   if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#       define __vsf_sched_safe(__code)     __code
#   else
#       define __vsf_sched_safe(...)        __VA_ARGS__
#   endif
#   define vsf_sched_safe_exit()

#endif

#define vsf_protect_scheduler()             vsf_sched_lock()
#define vsf_unprotect_scheduler(__state)    vsf_sched_unlock((__state))



#if __VSF_OS_SWI_NUM > 0
#define __vsf_forced_sched_safe(...)                                            \
        {                                                                       \
            vsf_sched_lock_status_t VSF_MACRO_SAFE_NAME(status) = vsf_forced_sched_lock();\
                __VA_ARGS__;                                                    \
            vsf_forced_sched_unlock(VSF_MACRO_SAFE_NAME(status));               \
        }

#define vsf_forced_sched_safe()             vsf_protect_region(&vsf_protect_region_sched)
#define vsf_forced_sched_safe_exit()        vsf_forced_sched_unlock(VSF_MACRO_SAFE_NAME(status))
#endif

#define vsf_protect_sched()                 (vsf_protect_t)vsf_protect_scheduler()
#define vsf_unprotect_sched(__prot)         vsf_unprotect_scheduler(__prot)

// kernel wrapper for host_request and host_thread
#ifdef VSF_ARCH_LIMIT_NO_SET_STACK
#   ifdef VSF_ARCH_IRQ_REQUEST_SUPPORT_MANUAL_RESET
#       define __vsf_kernel_host_request_init(__req)    __vsf_arch_irq_request_init((__req), true)
#   else
#       define __vsf_kernel_host_request_init(__req)    __vsf_arch_irq_request_init(__req)
#   endif
#   define __vsf_kernel_host_request_fini(__req)        __vsf_arch_irq_request_fini(__req)
#   define __vsf_kernel_host_request_send(__req)        __vsf_arch_irq_request_send(__req)
#   define __vsf_kernel_host_request_pend(__req)        __vsf_arch_irq_request_pend(__req)

#   ifdef VSF_ARCH_IRQ_SUPPORT_STACK
#       define __vsf_kernel_host_thread_init(__thread, __name, __entry, __prio, __stack, __stacksize)\
                __vsf_arch_irq_init(__thread, __name, __entry, __prio, __stack, __stacksize)
#   else
#       define __vsf_kernel_host_thread_init(__thread, __name, __entry, __prio, __stack, __stacksize)\
                __vsf_arch_irq_init(__thread, __name, __entry, __prio)
#   endif

#   define __vsf_kernel_host_thread_restart(__thread, __request_pending)        \
                __vsf_arch_irq_restart((__thread), (__request_pending))
#   define __vsf_kernel_host_thread_exit(__thread)      __vsf_arch_irq_exit(__thread)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef vsf_arch_prio_t vsf_sched_lock_status_t;

#ifdef __VSF_OS_CFG_EVTQ_LIST
declare_vsf_pool(vsf_evt_node_pool)
def_vsf_pool(vsf_evt_node_pool, vsf_evt_node_t)
#endif

#if __VSF_KERNEL_CFG_EDA_FRAME_POOL == ENABLED
declare_vsf_pool(vsf_eda_frame_pool)
def_vsf_pool(vsf_eda_frame_pool, __vsf_eda_frame_t)
#endif

typedef struct vsf_kernel_resource_t {
    struct {
    #if __VSF_OS_SWI_NUM > 0
        const vsf_arch_prio_t               *os_swi_priorities_ptr;
        uint16_t                            swi_priority_cnt;
        struct {
            vsf_prio_t                      begin;
            vsf_prio_t                      highest;
        }sched_prio;
    #else
        struct {
            vsf_prio_t                      highest;
        }sched_prio;
    #endif
    } arch;


#if __VSF_KERNEL_CFG_EVTQ_EN == ENABLED
    struct {
        vsf_evtq_t                          *queue_array;
#   if defined(__VSF_OS_CFG_EVTQ_ARRAY)
        vsf_evt_node_t                      **nodes;
        uint8_t                             node_bit_sz;
#endif
#   if defined(__VSF_OS_CFG_EVTQ_LIST)
        vsf_pool_item(vsf_evt_node_pool)    *nodes_buf_ptr;
        uint16_t                            node_cnt;
#   endif

        uint16_t                            queue_cnt;
    } evt_queue;

#endif
#if __VSF_KERNEL_CFG_EDA_FRAME_POOL == ENABLED
    struct {
        vsf_pool_item(vsf_eda_frame_pool)   *frame_buf_ptr;
        uint16_t                            frame_cnt;
    } frame_stack;
#endif
} vsf_kernel_resource_t;

/*============================ GLOBAL VARIABLES ==============================*/

#if __VSF_OS_SWI_NUM > 0
extern const code_region_t VSF_FORCED_SCHED_SAFE_CODE_REGION;
extern const vsf_protect_region_t vsf_protect_region_sched;
#else
#   define vsf_protect_region_sched         vsf_protect_region_none
#endif

/*============================ PROTOTYPES ====================================*/

//! __vsf_kernel_os_start and __vsf_kernel_os_run_priority are ONLY used when __vsf_main_entry is not used
extern void __vsf_kernel_os_start(void);
extern void __vsf_kernel_os_run_priority(vsf_prio_t priority);
#ifndef WEAK_VSF_KERNEL_ERR_REPORT
extern void vsf_kernel_err_report(enum vsf_kernel_error_t err);
#endif

#if VSF_KERNEL_CFG_CPU_USAGE == ENABLED
extern void vsf_cpu_usage_start(vsf_cpu_usage_ctx_t *ctx);
extern void vsf_cpu_usage_stop(void);
#endif

#if __VSF_OS_SWI_NUM > 0
extern vsf_sched_lock_status_t vsf_forced_sched_lock(void);
extern void vsf_forced_sched_unlock(vsf_sched_lock_status_t origlevel);
#endif

// vsf_sleep can only be called in vsf_plug_in_on_kernel_idle
extern void vsf_sleep(void);

#ifdef __cplusplus
}
#endif

#endif
#endif
