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

/**
 * \~english
 * @brief User application entry point symbol.
 *        If not defined externally, it defaults to user_main when the kernel
 *        entry is configured as main, otherwise it defaults to main.
 * \~chinese
 * @brief 用户应用程序入口点符号。
 *        如果未在外部定义，当内核入口配置为主函数时默认为 user_main，
 *        否则默认为 main。
 */
#ifndef VSF_USER_ENTRY
#   if VSF_KERNEL_CFG_ENTRY_IS_MAIN == ENABLED
#       define VSF_USER_ENTRY               user_main
#   else
#       define VSF_USER_ENTRY               main
#   endif
#endif

#if VSF_OS_CFG_PRIORITY_NUM > 1

/**
 * \~english
 * @brief Code region object in which the scheduler is locked (scheduler-safe region)
 * \~chinese
 * @brief 调度器被锁定的代码区域对象（调度器安全区域）
 */
#   define VSF_SCHED_SAFE_CODE_REGION       VSF_FORCED_SCHED_SAFE_CODE_REGION

/**
 * \~english
 * @brief Execute the given code with the scheduler locked
 * @param[in] ...: code to be executed while the scheduler is locked
 * @note Internal use only.
 * @note Typically called in task context to protect a short critical section from scheduling.
 * \~chinese
 * @brief 在调度器锁定状态下执行给定的代码
 * @param[in] ...: 在调度器锁定期间要执行的代码
 * @note 仅供内部使用。
 * @note 一般在任务上下文中调用，用于保护一小段不被调度打断的临界区。
 */
#   define __vsf_sched_safe(...)            __vsf_forced_sched_safe(__VA_ARGS__)

/**
 * \~english
 * @brief Lock the scheduler (disable scheduling)
 * @return vsf_sched_lock_status_t: the previous scheduler lock status, to be passed to vsf_sched_unlock()
 * @note Typically called in task context to protect a short critical section from scheduling.
 * \~chinese
 * @brief 锁定调度器（禁止调度）
 * @return vsf_sched_lock_status_t: 之前的调度器锁定状态，需传给 vsf_sched_unlock()
 * @note 一般在任务上下文中调用，用于保护一小段不被调度打断的临界区。
 */
#   define vsf_sched_lock()                 vsf_forced_sched_lock()

/**
 * \~english
 * @brief Unlock the scheduler, restoring a previous lock status
 * @param[in] __level: the lock status returned by vsf_sched_lock()
 * @note Typically called in task context to protect a short critical section from scheduling.
 * \~chinese
 * @brief 解锁调度器，恢复之前的锁定状态
 * @param[in] __level: vsf_sched_lock() 返回的锁定状态
 * @note 一般在任务上下文中调用，用于保护一小段不被调度打断的临界区。
 */
#   define vsf_sched_unlock(__level)        vsf_forced_sched_unlock((vsf_sched_lock_status_t)(__level))

/**
 * \~english
 * @brief Open a scheduler-safe region, used together with the following code block:
 *        vsf_sched_safe() { code; }
 * @note Typically called in task context to protect a short critical section from scheduling.
 * \~chinese
 * @brief 开启一个调度器安全区域，与其后的代码块配合使用：
 *        vsf_sched_safe() { 代码; }
 * @note 一般在任务上下文中调用，用于保护一小段不被调度打断的临界区。
 */
#   define vsf_sched_safe()                 vsf_forced_sched_safe()

/**
 * \~english
 * @brief Exit early from the current scheduler-safe region opened by vsf_sched_safe()
 * @note Must be called inside the region opened by vsf_sched_safe().
 * @note Typically called in task context to protect a short critical section from scheduling.
 * \~chinese
 * @brief 提前退出由 vsf_sched_safe() 开启的当前调度器安全区域
 * @note 必须在 vsf_sched_safe() 开启的区域内调用。
 * @note 一般在任务上下文中调用，用于保护一小段不被调度打断的临界区。
 */
#   define vsf_sched_safe_exit()            vsf_forced_sched_safe_exit()

#else

/**
 * \~english
 * @brief Empty stub variant when VSF_OS_CFG_PRIORITY_NUM <= 1; shares the documentation of the first variant above.
 * \~chinese
 * @brief VSF_OS_CFG_PRIORITY_NUM <= 1 时的空桩变体；与上方第一个变体共用文档。
 */
#   define VSF_SCHED_SAFE_CODE_REGION       DEFAULT_CODE_REGION_NONE

/**
 * \~english
 * @brief No-op stub variant when VSF_OS_CFG_PRIORITY_NUM <= 1; shares the documentation of the first variant above.
 * \~chinese
 * @brief VSF_OS_CFG_PRIORITY_NUM <= 1 时的空操作桩变体；与上方第一个变体共用文档。
 */
#   define vsf_sched_lock()                 0

/**
 * \~english
 * @brief No-op stub variant when VSF_OS_CFG_PRIORITY_NUM <= 1; shares the documentation of the first variant above.
 * \~chinese
 * @brief VSF_OS_CFG_PRIORITY_NUM <= 1 时的空操作桩变体；与上方第一个变体共用文档。
 */
#   define vsf_sched_unlock(__level)        VSF_UNUSED_PARAM(__level)

/**
 * \~english
 * @brief Stub variant when VSF_OS_CFG_PRIORITY_NUM <= 1 (the following block always runs); shares the documentation of the first variant above.
 * \~chinese
 * @brief VSF_OS_CFG_PRIORITY_NUM <= 1 时的桩变体（其后的代码块总是执行）；与上方第一个变体共用文档。
 */
#   define vsf_sched_safe()                 if (1)

#   if (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L) && !defined(__cplusplus)
/**
 * \~english
 * @brief No-op stub variant when VSF_OS_CFG_PRIORITY_NUM <= 1; shares the documentation of the first variant above.
 * \~chinese
 * @brief VSF_OS_CFG_PRIORITY_NUM <= 1 时的空操作桩变体；与上方第一个变体共用文档。
 */
#       define __vsf_sched_safe(__code)     __code
#   else
/**
 * \~english
 * @brief No-op stub variant when VSF_OS_CFG_PRIORITY_NUM <= 1; shares the documentation of the first variant above.
 * \~chinese
 * @brief VSF_OS_CFG_PRIORITY_NUM <= 1 时的空操作桩变体；与上方第一个变体共用文档。
 */
#       define __vsf_sched_safe(...)        __VA_ARGS__
#   endif

/**
 * \~english
 * @brief No-op stub variant when VSF_OS_CFG_PRIORITY_NUM <= 1; shares the documentation of the first variant above.
 * \~chinese
 * @brief VSF_OS_CFG_PRIORITY_NUM <= 1 时的空操作桩变体；与上方第一个变体共用文档。
 */
#   define vsf_sched_safe_exit()

#endif

/**
 * \~english
 * @brief Lock the scheduler, alias of vsf_sched_lock()
 * @return vsf_sched_lock_status_t: the previous scheduler lock status, to be passed to vsf_unprotect_scheduler()
 * @note Typically called in task context to protect a short critical section from scheduling.
 * \~chinese
 * @brief 锁定调度器，vsf_sched_lock() 的别名
 * @return vsf_sched_lock_status_t: 之前的调度器锁定状态，需传给 vsf_unprotect_scheduler()
 * @note 一般在任务上下文中调用，用于保护一小段不被调度打断的临界区。
 */
#define vsf_protect_scheduler()             vsf_sched_lock()

/**
 * \~english
 * @brief Unlock the scheduler, alias of vsf_sched_unlock()
 * @param[in] __state: the lock status returned by vsf_protect_scheduler()
 * @note Typically called in task context to protect a short critical section from scheduling.
 * \~chinese
 * @brief 解锁调度器，vsf_sched_unlock() 的别名
 * @param[in] __state: vsf_protect_scheduler() 返回的锁定状态
 * @note 一般在任务上下文中调用，用于保护一小段不被调度打断的临界区。
 */
#define vsf_unprotect_scheduler(__state)    vsf_sched_unlock((__state))



#if __VSF_OS_SWI_NUM > 0
/**
 * \~english
 * @brief Execute the given code with the scheduler locked
 * @param[in] ...: code to be executed while the scheduler is locked
 * @note Internal use only.
 * @note Typically called in task context to protect a short critical section from scheduling.
 * \~chinese
 * @brief 在调度器锁定状态下执行给定的代码
 * @param[in] ...: 在调度器锁定期间要执行的代码
 * @note 仅供内部使用。
 * @note 一般在任务上下文中调用，用于保护一小段不被调度打断的临界区。
 */
#define __vsf_forced_sched_safe(...)                                            \
        {                                                                       \
            vsf_sched_lock_status_t VSF_MACRO_SAFE_NAME(status) = vsf_forced_sched_lock();\
                __VA_ARGS__;                                                    \
            vsf_forced_sched_unlock(VSF_MACRO_SAFE_NAME(status));               \
        }

/**
 * \~english
 * @brief Enter the forced scheduler-safe protect region, used together with the
 *        following code block: vsf_forced_sched_safe() { code; }
 * @note Typically called in task context to protect a short critical section from scheduling.
 * \~chinese
 * @brief 进入强制调度器安全保护区域，与其后的代码块配合使用：
 *        vsf_forced_sched_safe() { 代码; }
 * @note 一般在任务上下文中调用，用于保护一小段不被调度打断的临界区。
 */
#define vsf_forced_sched_safe()             vsf_protect_region(&vsf_protect_region_sched)

/**
 * \~english
 * @brief Exit early from the current forced scheduler-safe region opened by vsf_forced_sched_safe()
 * @note Must be called inside the region opened by vsf_forced_sched_safe().
 * @note Typically called in task context to protect a short critical section from scheduling.
 * \~chinese
 * @brief 提前退出由 vsf_forced_sched_safe() 开启的当前强制调度器安全区域
 * @note 必须在 vsf_forced_sched_safe() 开启的区域内调用。
 * @note 一般在任务上下文中调用，用于保护一小段不被调度打断的临界区。
 */
#define vsf_forced_sched_safe_exit()        vsf_forced_sched_unlock(VSF_MACRO_SAFE_NAME(status))
#endif

/**
 * \~english
 * @brief Lock the scheduler and return the previous status as @ref vsf_protect_t
 * @return vsf_protect_t: the previous scheduler lock status, to be passed to vsf_unprotect_sched()
 * @note Typically called in task context to protect a short critical section from scheduling.
 * \~chinese
 * @brief 锁定调度器，并以 @ref vsf_protect_t 形式返回之前的状态
 * @return vsf_protect_t: 之前的调度器锁定状态，需传给 vsf_unprotect_sched()
 * @note 一般在任务上下文中调用，用于保护一小段不被调度打断的临界区。
 */
#define vsf_protect_sched()                 (vsf_protect_t)vsf_protect_scheduler()

/**
 * \~english
 * @brief Restore the scheduler lock status saved by vsf_protect_sched()
 * @param[in] __prot: the status returned by vsf_protect_sched()
 * @note Typically called in task context to protect a short critical section from scheduling.
 * \~chinese
 * @brief 恢复由 vsf_protect_sched() 保存的调度器锁定状态
 * @param[in] __prot: vsf_protect_sched() 返回的状态
 * @note 一般在任务上下文中调用，用于保护一小段不被调度打断的临界区。
 */
#define vsf_unprotect_sched(__prot)         vsf_unprotect_scheduler(__prot)

// kernel wrapper for host_request and host_thread
#if VSF_KERNEL_THREAD_USE_HOST == ENABLED
#   ifdef VSF_ARCH_IRQ_REQUEST_SUPPORT_MANUAL_RESET
/**
 * \~english
 * @brief Initialize a host IRQ request
 * @param[in] __req: pointer to the host IRQ request
 * @note Internal use only.
 * \~chinese
 * @brief 初始化一个主机 IRQ 请求
 * @param[in] __req: 指向主机 IRQ 请求的指针
 * @note 仅供内部使用。
 */
#       define __vsf_kernel_host_request_init(__req)    __vsf_arch_irq_request_init((__req), true)
#   else
/**
 * \~english
 * @brief Initialize a host IRQ request
 * @param[in] __req: pointer to the host IRQ request
 * @note Internal use only.
 * \~chinese
 * @brief 初始化一个主机 IRQ 请求
 * @param[in] __req: 指向主机 IRQ 请求的指针
 * @note 仅供内部使用。
 */
#       define __vsf_kernel_host_request_init(__req)    __vsf_arch_irq_request_init(__req)
#   endif

/**
 * \~english
 * @brief Finalize a host IRQ request
 * @param[in] __req: pointer to the host IRQ request
 * @note Internal use only.
 * \~chinese
 * @brief 终止一个主机 IRQ 请求
 * @param[in] __req: 指向主机 IRQ 请求的指针
 * @note 仅供内部使用。
 */
#   define __vsf_kernel_host_request_fini(__req)        __vsf_arch_irq_request_fini(__req)

/**
 * \~english
 * @brief Send (trigger) a host IRQ request
 * @param[in] __req: pointer to the host IRQ request
 * @note Internal use only.
 * \~chinese
 * @brief 发送（触发）一个主机 IRQ 请求
 * @param[in] __req: 指向主机 IRQ 请求的指针
 * @note 仅供内部使用。
 */
#   define __vsf_kernel_host_request_send(__req)        __vsf_arch_irq_request_send(__req)

/**
 * \~english
 * @brief Pend (wait) on a host IRQ request
 * @param[in] __req: pointer to the host IRQ request
 * @note Internal use only.
 * \~chinese
 * @brief 等待一个主机 IRQ 请求
 * @param[in] __req: 指向主机 IRQ 请求的指针
 * @note 仅供内部使用。
 */
#   define __vsf_kernel_host_request_pend(__req)        __vsf_arch_irq_request_pend(__req)

#   ifdef VSF_ARCH_IRQ_SUPPORT_STACK
/**
 * \~english
 * @brief Initialize a host IRQ thread with a user-provided stack
 * @param[in] __thread: pointer to the host thread
 * @param[in] __name: name of the thread
 * @param[in] __entry: entry function of the thread
 * @param[in] __prio: priority of the thread
 * @param[in] __stack: pointer to the thread stack
 * @param[in] __stacksize: size of the thread stack
 * @note Internal use only.
 * \~chinese
 * @brief 使用用户提供的栈初始化一个主机 IRQ 线程
 * @param[in] __thread: 指向主机线程的指针
 * @param[in] __name: 线程名称
 * @param[in] __entry: 线程入口函数
 * @param[in] __prio: 线程优先级
 * @param[in] __stack: 指向线程栈的指针
 * @param[in] __stacksize: 线程栈大小
 * @note 仅供内部使用。
 */
#       define __vsf_kernel_host_thread_init(__thread, __name, __entry, __prio, __stack, __stacksize)\
                __vsf_arch_irq_init(__thread, __name, __entry, __prio, __stack, __stacksize)
#   else
/**
 * \~english
 * @brief Initialize a host IRQ thread
 * @param[in] __thread: pointer to the host thread
 * @param[in] __name: name of the thread
 * @param[in] __entry: entry function of the thread
 * @param[in] __prio: priority of the thread
 * @param[in] __stack: ignored when the arch does not support user-provided stack
 * @param[in] __stacksize: ignored when the arch does not support user-provided stack
 * @note Internal use only.
 * \~chinese
 * @brief 初始化一个主机 IRQ 线程
 * @param[in] __thread: 指向主机线程的指针
 * @param[in] __name: 线程名称
 * @param[in] __entry: 线程入口函数
 * @param[in] __prio: 线程优先级
 * @param[in] __stack: 当架构不支持用户提供的栈时被忽略
 * @param[in] __stacksize: 当架构不支持用户提供的栈时被忽略
 * @note 仅供内部使用。
 */
#       define __vsf_kernel_host_thread_init(__thread, __name, __entry, __prio, __stack, __stacksize)\
                __vsf_arch_irq_init(__thread, __name, __entry, __prio)
#   endif

/**
 * \~english
 * @brief Restart a host IRQ thread
 * @param[in] __thread: pointer to the host thread
 * @param[in] __request_pending: pointer to the thread request
 *        @ref vsf_arch_irq_request_t pended on after restart
 * @note Internal use only.
 * \~chinese
 * @brief 重启一个主机 IRQ 线程
 * @param[in] __thread: 指向主机线程的指针
 * @param[in] __request_pending: 指向重启后要等待的线程请求
 *        @ref vsf_arch_irq_request_t 的指针
 * @note 仅供内部使用。
 */
#   define __vsf_kernel_host_thread_restart(__thread, __request_pending)        \
                __vsf_arch_irq_restart((__thread), (__request_pending))

/**
 * \~english
 * @brief Exit a host IRQ thread
 * @param[in] __thread: pointer to the host thread
 * @note Internal use only.
 * \~chinese
 * @brief 退出一个主机 IRQ 线程
 * @param[in] __thread: 指向主机线程的指针
 * @note 仅供内部使用。
 */
#   define __vsf_kernel_host_thread_exit(__thread)      __vsf_arch_irq_exit(__thread)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

// prototype: void vsf_sleep(int level = 0);
/**
 * \~english
 * @brief Put the CPU into sleep (low-power) mode; an optional sleep level
 *        (architecture-specific, 0 by default) may be passed
 * @note Can only be called inside the vsf_plug_in_on_kernel_idle plug-in.
 * \~chinese
 * @brief 使 CPU 进入睡眠（低功耗）模式；可以传入可选的睡眠级别
 *        （与具体架构相关，默认为 0）
 * @note 只能在 vsf_plug_in_on_kernel_idle 插件中调用。
 */
#define vsf_sleep(...)                      __vsf_sleep((0, ##__VA_ARGS__))

/*============================ TYPES =========================================*/

/**
 * \~english
 * @brief Scheduler lock status, returned by vsf_sched_lock() /
 *        vsf_forced_sched_lock() and passed back to vsf_sched_unlock() /
 *        vsf_forced_sched_unlock()
 * \~chinese
 * @brief 调度器锁定状态，由 vsf_sched_lock() / vsf_forced_sched_lock()
 *        返回，并传回给 vsf_sched_unlock() / vsf_forced_sched_unlock()
 */
typedef vsf_arch_prio_t vsf_sched_lock_status_t;

#ifdef __VSF_OS_CFG_EVTQ_LIST
dcl_vsf_pool(vsf_evt_node_pool)
def_vsf_pool(vsf_evt_node_pool, vsf_evt_node_t)
#endif

#if __VSF_KERNEL_CFG_EDA_FRAME_POOL == ENABLED
typedef struct __vsf_eda_frame_buffer_t {
    __vsf_eda_frame_t frame;
    uint8_t extra[VSF_OS_CFG_EDA_FRAME_POOL_EXTRA_SIZE];
} __vsf_eda_frame_buffer_t;
dcl_vsf_pool(vsf_eda_frame_pool)
def_vsf_pool(vsf_eda_frame_pool, __vsf_eda_frame_buffer_t)
#endif

typedef struct vsf_kernel_resource_t {
    struct {
    #if __VSF_OS_SWI_NUM > 0
        const vsf_arch_prio_t               *os_swi_priorities_ptr;
        uint16_t                            swi_priority_cnt;
        struct {
            vsf_prio_t                      begin;
            vsf_prio_t                      highest;
        } sched_prio;
    #else
        struct {
            vsf_prio_t                      highest;
        } sched_prio;
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

//! __vsf_kernel_os_start and __vsf_kernel_os_run_priority are ONLY used when embedded weak vsf_main_entry is not used
/**
 * \~english
 * @brief Initialize and start the VSF kernel (arch, HAL, service and OS initialization)
 * @note Internal use only. Only used when the embedded weak vsf_main_entry is not used.
 * \~chinese
 * @brief 初始化并启动 VSF 内核（架构、HAL、服务及 OS 初始化）
 * @note 仅供内部使用。仅在不使用内嵌弱符号 vsf_main_entry 时使用。
 */
extern void __vsf_kernel_os_start(void);

/**
 * \~english
 * @brief Run the event queue of the given priority (dispatch pending events)
 * @param[in] priority: the priority whose event queue is handled
 * @note Internal use only. Only used when the embedded weak vsf_main_entry is not used.
 * \~chinese
 * @brief 运行指定优先级的事件队列（分发待处理的事件）
 * @param[in] priority: 要处理的事件队列对应的优先级
 * @note 仅供内部使用。仅在不使用内嵌弱符号 vsf_main_entry 时使用。
 */
extern void __vsf_kernel_os_run_priority(vsf_prio_t priority);
#ifndef WEAK_VSF_KERNEL_ERR_REPORT
/**
 * \~english
 * @brief Report a kernel error
 * @param[in] err: the kernel error of type @ref vsf_kernel_error_t
 * @note Weak function, can be re-implemented by defining WEAK_VSF_KERNEL_ERR_REPORT.
 * \~chinese
 * @brief 报告一个内核错误
 * @param[in] err: @ref vsf_kernel_error_t 类型的内核错误
 * @note 弱函数，可通过定义 WEAK_VSF_KERNEL_ERR_REPORT 重新实现。
 */
extern void vsf_kernel_err_report(enum vsf_kernel_error_t err);
#endif

#if VSF_KERNEL_CFG_CPU_USAGE == ENABLED
/**
 * \~english
 * @brief Start CPU usage measurement
 * @param[in] ctx: a pointer to structure @ref vsf_cpu_usage_ctx_t storing the measurement result
 * @note Only one measurement can run at a time; stop it with vsf_cpu_usage_stop().
 * \~chinese
 * @brief 开始 CPU 使用率测量
 * @param[in] ctx: 指向存储测量结果的结构体 @ref vsf_cpu_usage_ctx_t 的指针
 * @note 同一时间只能进行一个测量；使用 vsf_cpu_usage_stop() 停止。
 */
extern void vsf_cpu_usage_start(vsf_cpu_usage_ctx_t *ctx);

/**
 * \~english
 * @brief Stop CPU usage measurement
 * @note After stopping, ctx->duration holds the total elapsed ticks and ctx->ticks
 *       holds the busy (non-idle) ticks; CPU usage is ticks / duration.
 * \~chinese
 * @brief 停止 CPU 使用率测量
 * @note 停止后，ctx->duration 为总经过的 tick 数，ctx->ticks 为忙（非空闲）
 *       tick 数；CPU 使用率为 ticks / duration。
 */
extern void vsf_cpu_usage_stop(void);
#endif

#if __VSF_OS_SWI_NUM > 0
/**
 * \~english
 * @brief Forcibly lock the scheduler by masking all scheduler SWIs
 * @return vsf_sched_lock_status_t: the previous scheduler lock status, to be passed to vsf_forced_sched_unlock()
 * @note Typically called in task context to protect a short critical section from scheduling.
 * \~chinese
 * @brief 通过屏蔽所有调度器 SWI 强制锁定调度器
 * @return vsf_sched_lock_status_t: 之前的调度器锁定状态，需传给 vsf_forced_sched_unlock()
 * @note 一般在任务上下文中调用，用于保护一小段不被调度打断的临界区。
 */
extern vsf_sched_lock_status_t vsf_forced_sched_lock(void);

/**
 * \~english
 * @brief Restore the scheduler lock status saved by vsf_forced_sched_lock()
 * @param[in] origlevel: the previous lock status returned by vsf_forced_sched_lock()
 * @note Typically called in task context to protect a short critical section from scheduling.
 * \~chinese
 * @brief 恢复由 vsf_forced_sched_lock() 保存的调度器锁定状态
 * @param[in] origlevel: vsf_forced_sched_lock() 返回的之前锁定状态
 * @note 一般在任务上下文中调用，用于保护一小段不被调度打断的临界区。
 */
extern void vsf_forced_sched_unlock(vsf_sched_lock_status_t origlevel);
#endif

// vsf_sleep can only be called in vsf_plug_in_on_kernel_idle
/**
 * \~english
 * @brief Implementation of vsf_sleep(), put the CPU into sleep (low-power) mode
 * @param[in] level: sleep level, architecture-specific
 * @note Internal use only. Can only be called inside the vsf_plug_in_on_kernel_idle plug-in.
 * \~chinese
 * @brief vsf_sleep() 的实现，使 CPU 进入睡眠（低功耗）模式
 * @param[in] level: 睡眠级别，与具体架构相关
 * @note 仅供内部使用。只能在 vsf_plug_in_on_kernel_idle 插件中调用。
 */
extern void __vsf_sleep(int level);

#ifdef __cplusplus
}
#endif

#endif
#endif
