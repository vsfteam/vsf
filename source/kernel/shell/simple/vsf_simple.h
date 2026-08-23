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

#ifndef __VSF_KERNEL_SHELL_SIMPLE_H__
#define __VSF_KERNEL_SHELL_SIMPLE_H__

/*============================ INCLUDES ======================================*/
#include "kernel/vsf_kernel_cfg.h"

#if VSF_KERNEL_USE_SIMPLE_SHELL == ENABLED
#include "../../vsf_eda.h"
#include "./vsf_simple_bmpevt.h"
#if VSF_KERNEL_CFG_EDA_SUPPORT_TASK == ENABLED
#   include "../../task/vsf_task.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

/*----------------------------------------------------------------------------*
 * Common                                                                     *
 *----------------------------------------------------------------------------*/

/**
 * \~english
 * @brief Declare the per-invocation static task-local control block and fetch
 *        the current event.
 * @param[in] ...: user local variable declarations appended to the control block
 *
 * @note Expands to an anonymous struct with a static instance and a pointer
 *       @c this_ptr to it; the first member @c fsm_state is reserved for the
 *       simple-shell state machine. Also defines a local variable @c evt
 *       holding the current event (vsf_eda_get_cur_evt()).
 * @note Must be placed at the beginning of a simple-shell event handler.
 * \~chinese
 * @brief 声明每次调用都有效的静态任务局部控制块，并获取当前事件。
 * @param[in] ...: 追加到控制块中的用户局部变量声明
 *
 * @note 展开为一个匿名结构体，包含一个静态实例以及指向它的指针
 *       @c this_ptr；第一个成员 @c fsm_state 保留给 simple-shell 状态机使用。
 *       同时定义局部变量 @c evt 保存当前事件（vsf_eda_get_cur_evt()）。
 * @note 必须放在 simple-shell 事件处理函数的开头。
 */
#define static_task_instance(...)                                               \
        struct {                                                                \
            uint_fast8_t fsm_state;                                             \
            __VA_ARGS__                                                         \
        } static VSF_MACRO_SAFE_NAME(local_cb),                                 \
            *this_ptr = &VSF_MACRO_SAFE_NAME(local_cb);                         \
             vsf_evt_t evt = vsf_eda_get_cur_evt();                             \
             VSF_UNUSED_PARAM(evt);                                             \
             VSF_UNUSED_PARAM(this_ptr);

#if __IS_COMPILER_IAR__
/**
 * \~english
 * @brief List the task features (memory blocks) used by a simple-shell task.
 * @param[in] ...: feature items, typically mem_sharable() and
 *        mem_nonsharable() blocks
 * \~chinese
 * @brief 列出 simple-shell 任务使用的任务特性（内存块）。
 * @param[in] ...: 特性项，通常是 mem_sharable() 和 mem_nonsharable() 块
 */
#       define features_used(...)              __VA_ARGS__;
/**
 * \~english
 * @brief Declare variables that may be overlaid with other features to save
 *        RAM; they must be re-initialized on every entry.
 * @param[in] ...: variable declarations placed in a union
 * \~chinese
 * @brief 声明可与其他特性复用以节省 RAM 的变量；每次进入时必须重新初始化。
 * @param[in] ...: 放入联合体中的变量声明
 */
#       define mem_sharable(...)                                                \
            union {uint_fast8_t __zzzz_do_not_use; __VA_ARGS__;};
/**
 * \~english
 * @brief Declare variables that occupy their own memory and are never
 *        overlaid with other features.
 * @param[in] ...: variable declarations kept as-is
 * \~chinese
 * @brief 声明独占内存、不与其他特性复用的变量。
 * @param[in] ...: 原样保留的变量声明
 */
#       define mem_nonsharable(...)            __VA_ARGS__;
#else
/**
 * \~english
 * @brief Non-IAR variant; shares the documentation of the first variant above.
 * \~chinese
 * @brief 非 IAR 变体；与上方第一个变体共用文档。
 */
#       define features_used(...)              __VA_ARGS__
/**
 * \~english
 * @brief Non-IAR variant; shares the documentation of the first variant above.
 * \~chinese
 * @brief 非 IAR 变体；与上方第一个变体共用文档。
 */
#       define mem_sharable(...)               union {__VA_ARGS__};
/**
 * \~english
 * @brief Non-IAR variant; shares the documentation of the first variant above.
 * \~chinese
 * @brief 非 IAR 变体；与上方第一个变体共用文档。
 */
#       define mem_nonsharable(...)            __VA_ARGS__
#endif

/**
 * \~english
 * @brief Yield the CPU cooperatively so that other tasks of the same event
 *        queue can run before this task continues.
 *
 * @note Statement macro: the following brace block runs after the task is
 *       resumed. Usage: vsf_yield() { ... }. Must be used inside a
 *       simple-shell event handler.
 * \~chinese
 * @brief 协作式让出 CPU，让同一事件队列中的其他任务先运行，然后本任务再继续。
 *
 * @note 语句宏：紧随其后的花括号代码块在任务恢复运行后执行。
 *       用法：vsf_yield() { ... }。必须在 simple-shell 事件处理函数中使用。
 */
#define vsf_yield()                                                             \
            for (   vsf_evt_t result = VSF_EVT_INVALID;                         \
                    result == VSF_EVT_INVALID;)                                 \
                if ((result =__vsf_yield(), result == VSF_EVT_YIELD))

/*----------------------------------------------------------------------------*
 * Delay                                                                      *
 *----------------------------------------------------------------------------*/
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
/**
 * \~english
 * @brief Delay for a given number of system timer ticks (cooperative,
 *        non-blocking for other tasks).
 * @param[in] __tick: delay time in system timer ticks
 *
 * @note Statement macro: the following brace block runs after the delay
 *       expires. Usage: vsf_delay(tick) { ... }. Must be used inside a
 *       simple-shell event handler.
 * \~chinese
 * @brief 延迟指定的系统定时器 tick 数（协作式，不阻塞其他任务）。
 * @param[in] __tick: 以系统定时器 tick 为单位的延迟时间
 *
 * @note 语句宏：紧随其后的花括号代码块在延迟到期后执行。
 *       用法：vsf_delay(tick) { ... }。必须在 simple-shell 事件处理函数中使用。
 */
#   define vsf_delay(__tick)                                                    \
    if (VSF_EVT_TIMER == __vsf_delay((vsf_systimer_tick_t)__tick))
/**
 * \~english
 * @brief Delay for a given number of milliseconds (cooperative, non-blocking
 *        for other tasks).
 * @param[in] __ms: delay time in milliseconds, converted to ticks via
 *        vsf_systimer_ms_to_tick()
 *
 * @note Statement macro: the following brace block runs after the delay
 *       expires. Usage: vsf_delay_ms(ms) { ... }. Must be used inside a
 *       simple-shell event handler.
 * \~chinese
 * @brief 延迟指定的毫秒数（协作式，不阻塞其他任务）。
 * @param[in] __ms: 以毫秒为单位的延迟时间，通过 vsf_systimer_ms_to_tick()
 *        转换为 tick
 *
 * @note 语句宏：紧随其后的花括号代码块在延迟到期后执行。
 *       用法：vsf_delay_ms(ms) { ... }。必须在 simple-shell 事件处理函数中使用。
 */
#   define vsf_delay_ms(__ms)                                                   \
    if (VSF_EVT_TIMER == __vsf_delay((vsf_systimer_tick_t)vsf_systimer_ms_to_tick(__ms)))
/**
 * \~english
 * @brief Delay for a given number of microseconds (cooperative, non-blocking
 *        for other tasks).
 * @param[in] __us: delay time in microseconds, converted to ticks via
 *        vsf_systimer_us_to_tick()
 *
 * @note Statement macro: the following brace block runs after the delay
 *       expires. Usage: vsf_delay_us(us) { ... }. Must be used inside a
 *       simple-shell event handler.
 * \~chinese
 * @brief 延迟指定的微秒数（协作式，不阻塞其他任务）。
 * @param[in] __us: 以微秒为单位的延迟时间，通过 vsf_systimer_us_to_tick()
 *        转换为 tick
 *
 * @note 语句宏：紧随其后的花括号代码块在延迟到期后执行。
 *       用法：vsf_delay_us(us) { ... }。必须在 simple-shell 事件处理函数中使用。
 */
#   define vsf_delay_us(__us)                                                   \
    if (VSF_EVT_TIMER == __vsf_delay((vsf_systimer_tick_t)vsf_systimer_us_to_tick(__us)))
#endif



#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
/*----------------------------------------------------------------------------*
 * IPC Common                                                                 *
 *----------------------------------------------------------------------------*/
/**
 * \~english
 * @brief Catch the timeout case of a preceding *_timeout wait macro.
 *
 * @note Statement macro: the following brace block runs when the preceding
 *       wait timed out, i.e. the current event is VSF_EVT_TIMER. Usage:
 *       on_timeout() { ... }. Must be used inside a simple-shell event
 *       handler, right after a *_timeout() statement macro.
 * \~chinese
 * @brief 捕获前一个 *_timeout 等待宏的超时情况。
 *
 * @note 语句宏：当前面的等待超时（即当前事件为 VSF_EVT_TIMER）时，紧随
 *       其后的花括号代码块执行。用法：on_timeout() { ... }。必须在
 *       simple-shell 事件处理函数中、紧跟一个 *_timeout() 语句宏之后使用。
 */
#   define on_timeout()                                                         \
                if (VSF_EVT_TIMER == vsf_eda_get_cur_evt())

/*----------------------------------------------------------------------------*
 * Mutex                                                                      *
 *----------------------------------------------------------------------------*/
/**
 * \~english
 * @brief Initialize a mutex.
 * @param[in] __mutex_addr: pointer to the mutex @ref vsf_mutex_t
 * @return vsf_err_t: always VSF_ERR_NONE (invalid parameters trigger an assertion)
 * @note Must be called in task (or initialization) context, NOT in interrupt context.
 * \~chinese
 * @brief 初始化一个互斥量。
 * @param[in] __mutex_addr: 指向互斥量 @ref vsf_mutex_t 的指针
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE（参数非法会触发断言）
 * @note 必须在任务（或初始化）上下文中调用，禁止在中断上下文中调用。
 */
#   define vsf_mutex_init(__mutex_addr)     vsf_eda_mutex_init(__mutex_addr)
/**
 * \~english
 * @brief Leave (release) a mutex previously entered by the current task.
 * @param[in] __mutex_addr: pointer to the mutex @ref vsf_mutex_t
 * @return vsf_err_t: VSF_ERR_NONE if successful; VSF_ERR_OVERRUN if the mutex is not owned
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 离开（释放）当前任务先前进入的互斥量。
 * @param[in] __mutex_addr: 指向互斥量 @ref vsf_mutex_t 的指针
 * @return vsf_err_t: 成功返回 VSF_ERR_NONE；互斥量未被持有返回 VSF_ERR_OVERRUN
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
#   define vsf_mutex_leave(__mutex_addr)    vsf_eda_mutex_leave(__mutex_addr)

/**
 * \~english
 * @brief Enter (lock) a mutex, waiting forever until it is obtained
 *        (cooperative, non-blocking for other tasks).
 * @param[in] __mutex_addr: pointer to the mutex @ref vsf_mutex_t
 *
 * @note Statement macro: the following brace block runs after the mutex is
 *       obtained. Usage: vsf_mutex_enter(pmtx) { ... }. Must be used inside a
 *       simple-shell event handler.
 * \~chinese
 * @brief 进入（加锁）互斥量，一直等待直到获得（协作式，不阻塞其他任务）。
 * @param[in] __mutex_addr: 指向互斥量 @ref vsf_mutex_t 的指针
 *
 * @note 语句宏：紧随其后的花括号代码块在获得互斥量后执行。
 *       用法：vsf_mutex_enter(pmtx) { ... }。必须在 simple-shell 事件处理
 *       函数中使用。
 */
#   define vsf_mutex_enter(__mutex_addr)                                        \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_mutex_enter((__mutex_addr),                  \
                    (-1)), reason == VSF_SYNC_GET))

/**
 * \~english
 * @brief Try to enter (lock) a mutex with a timeout in system timer ticks
 *        (cooperative, non-blocking for other tasks).
 * @param[in] __mutex_addr: pointer to the mutex @ref vsf_mutex_t
 * @param[in] __timeout: timeout in system timer ticks; a negative value waits
 *        forever
 *
 * @note Statement macro: the following brace block runs when the mutex is
 *       obtained (VSF_SYNC_GET) or the wait times out (VSF_SYNC_TIMEOUT); use
 *       on_timeout() to catch the timeout case. Usage:
 *       vsf_mutex_try_enter_timeout(pmtx, timeout) { ... }. Must be used
 *       inside a simple-shell event handler.
 * \~chinese
 * @brief 尝试在指定的系统定时器 tick 数内进入（加锁）互斥量（协作式，
 *        不阻塞其他任务）。
 * @param[in] __mutex_addr: 指向互斥量 @ref vsf_mutex_t 的指针
 * @param[in] __timeout: 以系统定时器 tick 为单位的超时时间；负值表示一直等待
 *
 * @note 语句宏：当获得互斥量（VSF_SYNC_GET）或等待超时（VSF_SYNC_TIMEOUT）
 *       时，紧随其后的花括号代码块执行；使用 on_timeout() 捕获超时情况。
 *       用法：vsf_mutex_try_enter_timeout(pmtx, timeout) { ... }。
 *       必须在 simple-shell 事件处理函数中使用。
 */
#   define vsf_mutex_try_enter_timeout(__mutex_addr, __timeout)                 \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_mutex_enter((__mutex_addr),                  \
                    (__timeout)),                                               \
                    (reason == VSF_SYNC_GET || reason == VSF_SYNC_TIMEOUT)))

/**
 * \~english
 * @brief Try to enter (lock) a mutex with a timeout in milliseconds
 *        (cooperative, non-blocking for other tasks).
 * @param[in] __mutex_addr: pointer to the mutex @ref vsf_mutex_t
 * @param[in] __timeout: timeout in milliseconds, converted to ticks via
 *        vsf_systimer_ms_to_tick()
 *
 * @note Statement macro: the following brace block runs when the mutex is
 *       obtained (VSF_SYNC_GET) or the wait times out (VSF_SYNC_TIMEOUT); use
 *       on_timeout() to catch the timeout case. Usage:
 *       vsf_mutex_try_enter_timeout_ms(pmtx, timeout) { ... }. Must be used
 *       inside a simple-shell event handler.
 * \~chinese
 * @brief 尝试在指定的毫秒数内进入（加锁）互斥量（协作式，不阻塞其他任务）。
 * @param[in] __mutex_addr: 指向互斥量 @ref vsf_mutex_t 的指针
 * @param[in] __timeout: 以毫秒为单位的超时时间，通过 vsf_systimer_ms_to_tick()
 *        转换为 tick
 *
 * @note 语句宏：当获得互斥量（VSF_SYNC_GET）或等待超时（VSF_SYNC_TIMEOUT）
 *       时，紧随其后的花括号代码块执行；使用 on_timeout() 捕获超时情况。
 *       用法：vsf_mutex_try_enter_timeout_ms(pmtx, timeout) { ... }。
 *       必须在 simple-shell 事件处理函数中使用。
 */
#   define vsf_mutex_try_enter_timeout_ms(__mutex_addr, __timeout)              \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_mutex_enter((__mutex_addr),                  \
                    vsf_systimer_ms_to_tick(__timeout)),                        \
                    (reason == VSF_SYNC_GET || reason == VSF_SYNC_TIMEOUT)))

/**
 * \~english
 * @brief Try to enter (lock) a mutex with a timeout in microseconds
 *        (cooperative, non-blocking for other tasks).
 * @param[in] __mutex_addr: pointer to the mutex @ref vsf_mutex_t
 * @param[in] __timeout: timeout in microseconds, converted to ticks via
 *        vsf_systimer_us_to_tick()
 *
 * @note Statement macro: the following brace block runs when the mutex is
 *       obtained (VSF_SYNC_GET) or the wait times out (VSF_SYNC_TIMEOUT); use
 *       on_timeout() to catch the timeout case. Usage:
 *       vsf_mutex_try_enter_timeout_us(pmtx, timeout) { ... }. Must be used
 *       inside a simple-shell event handler.
 * \~chinese
 * @brief 尝试在指定的微秒数内进入（加锁）互斥量（协作式，不阻塞其他任务）。
 * @param[in] __mutex_addr: 指向互斥量 @ref vsf_mutex_t 的指针
 * @param[in] __timeout: 以微秒为单位的超时时间，通过 vsf_systimer_us_to_tick()
 *        转换为 tick
 *
 * @note 语句宏：当获得互斥量（VSF_SYNC_GET）或等待超时（VSF_SYNC_TIMEOUT）
 *       时，紧随其后的花括号代码块执行；使用 on_timeout() 捕获超时情况。
 *       用法：vsf_mutex_try_enter_timeout_us(pmtx, timeout) { ... }。
 *       必须在 simple-shell 事件处理函数中使用。
 */
#   define vsf_mutex_try_enter_timeout_us(__mutex_addr, __timeout)              \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_mutex_enter((__mutex_addr),                  \
                    vsf_systimer_us_to_tick(__timeout)),                        \
                    (reason == VSF_SYNC_GET || reason == VSF_SYNC_TIMEOUT)))

/*----------------------------------------------------------------------------*
 * Critical Section                                                           *
 *----------------------------------------------------------------------------*/
/**
 * \~english
 * @brief Initialize a critical section object.
 * @param[in] __crit_addr: pointer to the critical section object @ref vsf_crit_t
 * @return vsf_err_t: always VSF_ERR_NONE (invalid parameters trigger an assertion)
 * @note Must be called in task (or initialization) context, NOT in interrupt context.
 * \~chinese
 * @brief 初始化一个临界代码段对象。
 * @param[in] __crit_addr: 指向临界代码段对象 @ref vsf_crit_t 的指针
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE（参数非法会触发断言）
 * @note 必须在任务（或初始化）上下文中调用，禁止在中断上下文中调用。
 */
#   define vsf_crit_init(__crit_addr)     vsf_eda_crit_init(__crit_addr)
/**
 * \~english
 * @brief Leave a critical section previously entered by the current task.
 * @param[in] __crit_addr: pointer to the critical section object @ref vsf_crit_t
 * @return vsf_err_t: VSF_ERR_NONE if successful; VSF_ERR_OVERRUN if the critical section is not owned
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 离开当前任务先前进入的临界代码段。
 * @param[in] __crit_addr: 指向临界代码段对象 @ref vsf_crit_t 的指针
 * @return vsf_err_t: 成功返回 VSF_ERR_NONE；临界代码段未被持有返回 VSF_ERR_OVERRUN
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
#   define vsf_crit_leave(__crit_addr)    vsf_eda_crit_leave(__crit_addr)

/**
 * \~english
 * @brief Enter a critical section, waiting forever until it is obtained
 *        (cooperative, non-blocking for other tasks).
 * @param[in] __crit_addr: pointer to the critical section object @ref vsf_crit_t
 *
 * @note Statement macro: the following brace block runs after the critical
 *       section is entered. Usage: vsf_crit_enter(pcrit) { ... }. Must be used
 *       inside a simple-shell event handler.
 * \~chinese
 * @brief 进入临界代码段，一直等待直到获得（协作式，不阻塞其他任务）。
 * @param[in] __crit_addr: 指向临界代码段对象 @ref vsf_crit_t 的指针
 *
 * @note 语句宏：紧随其后的花括号代码块在进入临界代码段后执行。
 *       用法：vsf_crit_enter(pcrit) { ... }。必须在 simple-shell 事件处理
 *       函数中使用。
 */
#   define vsf_crit_enter(__crit_addr)                                          \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_mutex_enter((__crit_addr),                   \
                    (-1)), reason == VSF_SYNC_GET))

/**
 * \~english
 * @brief Try to enter a critical section with a timeout in system timer ticks
 *        (cooperative, non-blocking for other tasks).
 * @param[in] __crit_addr: pointer to the critical section object @ref vsf_crit_t
 * @param[in] __timeout: timeout in system timer ticks; a negative value waits
 *        forever
 *
 * @note Statement macro: the following brace block runs when the critical
 *       section is entered (VSF_SYNC_GET) or the wait times out
 *       (VSF_SYNC_TIMEOUT); use on_timeout() to catch the timeout case.
 *       Usage: vsf_crit_try_enter_timeout(pcrit, timeout) { ... }. Must be
 *       used inside a simple-shell event handler.
 * \~chinese
 * @brief 尝试在指定的系统定时器 tick 数内进入临界代码段（协作式，
 *        不阻塞其他任务）。
 * @param[in] __crit_addr: 指向临界代码段对象 @ref vsf_crit_t 的指针
 * @param[in] __timeout: 以系统定时器 tick 为单位的超时时间；负值表示一直等待
 *
 * @note 语句宏：当进入临界代码段（VSF_SYNC_GET）或等待超时
 *       （VSF_SYNC_TIMEOUT）时，紧随其后的花括号代码块执行；使用
 *       on_timeout() 捕获超时情况。
 *       用法：vsf_crit_try_enter_timeout(pcrit, timeout) { ... }。
 *       必须在 simple-shell 事件处理函数中使用。
 */
#   define vsf_crit_try_enter_timeout(__crit_addr, __timeout)                   \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_mutex_enter((__crit_addr),                   \
                    (__timeout)),                                               \
                    (reason == VSF_SYNC_GET || reason == VSF_SYNC_TIMEOUT)))

/**
 * \~english
 * @brief Try to enter a critical section with a timeout in milliseconds
 *        (cooperative, non-blocking for other tasks).
 * @param[in] __crit_addr: pointer to the critical section object @ref vsf_crit_t
 * @param[in] __timeout: timeout in milliseconds, converted to ticks via
 *        vsf_systimer_ms_to_tick()
 *
 * @note Statement macro: the following brace block runs when the critical
 *       section is entered (VSF_SYNC_GET) or the wait times out
 *       (VSF_SYNC_TIMEOUT); use on_timeout() to catch the timeout case.
 *       Usage: vsf_crit_try_enter_timeout_ms(pcrit, timeout) { ... }. Must be
 *       used inside a simple-shell event handler.
 * \~chinese
 * @brief 尝试在指定的毫秒数内进入临界代码段（协作式，不阻塞其他任务）。
 * @param[in] __crit_addr: 指向临界代码段对象 @ref vsf_crit_t 的指针
 * @param[in] __timeout: 以毫秒为单位的超时时间，通过 vsf_systimer_ms_to_tick()
 *        转换为 tick
 *
 * @note 语句宏：当进入临界代码段（VSF_SYNC_GET）或等待超时
 *       （VSF_SYNC_TIMEOUT）时，紧随其后的花括号代码块执行；使用
 *       on_timeout() 捕获超时情况。
 *       用法：vsf_crit_try_enter_timeout_ms(pcrit, timeout) { ... }。
 *       必须在 simple-shell 事件处理函数中使用。
 */
#   define vsf_crit_try_enter_timeout_ms(__crit_addr, __timeout)                \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_mutex_enter((__crit_addr),                   \
                    vsf_systimer_ms_to_tick(__timeout)),                        \
                    (reason == VSF_SYNC_GET || reason == VSF_SYNC_TIMEOUT)))

/**
 * \~english
 * @brief Try to enter a critical section with a timeout in microseconds
 *        (cooperative, non-blocking for other tasks).
 * @param[in] __crit_addr: pointer to the critical section object @ref vsf_crit_t
 * @param[in] __timeout: timeout in microseconds, converted to ticks via
 *        vsf_systimer_us_to_tick()
 *
 * @note Statement macro: the following brace block runs when the critical
 *       section is entered (VSF_SYNC_GET) or the wait times out
 *       (VSF_SYNC_TIMEOUT); use on_timeout() to catch the timeout case.
 *       Usage: vsf_crit_try_enter_timeout_us(pcrit, timeout) { ... }. Must be
 *       used inside a simple-shell event handler.
 * \~chinese
 * @brief 尝试在指定的微秒数内进入临界代码段（协作式，不阻塞其他任务）。
 * @param[in] __crit_addr: 指向临界代码段对象 @ref vsf_crit_t 的指针
 * @param[in] __timeout: 以微秒为单位的超时时间，通过 vsf_systimer_us_to_tick()
 *        转换为 tick
 *
 * @note 语句宏：当进入临界代码段（VSF_SYNC_GET）或等待超时
 *       （VSF_SYNC_TIMEOUT）时，紧随其后的花括号代码块执行；使用
 *       on_timeout() 捕获超时情况。
 *       用法：vsf_crit_try_enter_timeout_us(pcrit, timeout) { ... }。
 *       必须在 simple-shell 事件处理函数中使用。
 */
#   define vsf_crit_try_enter_timeout_us(__crit_addr, __timeout)                \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_mutex_enter((__crit_addr),                   \
                    vsf_systimer_us_to_tick(__timeout)),                        \
                    (reason == VSF_SYNC_GET || reason == VSF_SYNC_TIMEOUT)))

/*----------------------------------------------------------------------------*
 * Semaphore                                                                  *
 *----------------------------------------------------------------------------*/
/**
 * \~english
 * @brief Initialize a semaphore with an initial count; the count is
 *        auto-reset and limited by VSF_SYNC_MAX.
 * @param[in] __psem: pointer to the semaphore @ref vsf_sem_t
 * @param[in] __cnt: initial count of the semaphore
 * @return vsf_err_t: always VSF_ERR_NONE (invalid parameters trigger an assertion)
 * @note Must be called in task (or initialization) context, NOT in interrupt context.
 * \~chinese
 * @brief 以指定的初始计数值初始化一个信号量；计数值自动复位，上限为
 *        VSF_SYNC_MAX。
 * @param[in] __psem: 指向信号量 @ref vsf_sem_t 的指针
 * @param[in] __cnt: 信号量的初始计数值
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE（参数非法会触发断言）
 * @note 必须在任务（或初始化）上下文中调用，禁止在中断上下文中调用。
 */
#   define vsf_sem_init(__psem, __cnt)                                          \
            vsf_eda_sync_init((__psem), (__cnt), VSF_SYNC_MAX | VSF_SYNC_AUTO_RST)
/**
 * \~english
 * @brief Post (increase) a semaphore, waking up one pending task if any.
 * @param[in] __psem: pointer to the semaphore @ref vsf_sem_t
 * @return vsf_err_t: VSF_ERR_NONE if successful; VSF_ERR_OVERRUN if the count has reached the maximum
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 发布（增加）一个信号量，如果有任务在等待则唤醒其中一个。
 * @param[in] __psem: 指向信号量 @ref vsf_sem_t 的指针
 * @return vsf_err_t: 成功返回 VSF_ERR_NONE；计数已达到最大值返回 VSF_ERR_OVERRUN
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
#   define vsf_sem_post(__psem)             vsf_eda_sem_post((__psem))

/**
 * \~english
 * @brief Wait for (pend on) a semaphore, waiting forever until it is obtained
 *        (cooperative, non-blocking for other tasks).
 * @param[in] __psem: pointer to the semaphore @ref vsf_sem_t
 *
 * @note Statement macro: the following brace block runs after the semaphore
 *       is obtained. Usage: vsf_sem_pend(psem) { ... }. Must be used inside a
 *       simple-shell event handler.
 * \~chinese
 * @brief 等待信号量，一直等待直到获得（协作式，不阻塞其他任务）。
 * @param[in] __psem: 指向信号量 @ref vsf_sem_t 的指针
 *
 * @note 语句宏：紧随其后的花括号代码块在获得信号量后执行。
 *       用法：vsf_sem_pend(psem) { ... }。必须在 simple-shell 事件处理
 *       函数中使用。
 */
#   define vsf_sem_pend(__psem)                                                 \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_sem_pend((__psem),                           \
                    (-1)), reason == VSF_SYNC_GET))

/**
 * \~english
 * @brief Wait for (pend on) a semaphore with a timeout in system timer ticks
 *        (cooperative, non-blocking for other tasks).
 * @param[in] __psem: pointer to the semaphore @ref vsf_sem_t
 * @param[in] __timeout: timeout in system timer ticks; a negative value waits
 *        forever
 *
 * @note Statement macro: the following brace block runs when the semaphore is
 *       obtained (VSF_SYNC_GET) or the wait times out (VSF_SYNC_TIMEOUT); use
 *       on_timeout() to catch the timeout case. Usage:
 *       vsf_sem_pend_timeout(psem, timeout) { ... }. Must be used inside a
 *       simple-shell event handler.
 * \~chinese
 * @brief 在指定的系统定时器 tick 数内等待信号量（协作式，不阻塞其他任务）。
 * @param[in] __psem: 指向信号量 @ref vsf_sem_t 的指针
 * @param[in] __timeout: 以系统定时器 tick 为单位的超时时间；负值表示一直等待
 *
 * @note 语句宏：当获得信号量（VSF_SYNC_GET）或等待超时（VSF_SYNC_TIMEOUT）
 *       时，紧随其后的花括号代码块执行；使用 on_timeout() 捕获超时情况。
 *       用法：vsf_sem_pend_timeout(psem, timeout) { ... }。
 *       必须在 simple-shell 事件处理函数中使用。
 */
#   define vsf_sem_pend_timeout(__psem, __timeout)                              \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_sem_pend((__psem),                           \
                    (__timeout)),                                               \
                    (reason == VSF_SYNC_GET || reason == VSF_SYNC_TIMEOUT)))


/**
 * \~english
 * @brief Wait for (pend on) a semaphore with a timeout in milliseconds
 *        (cooperative, non-blocking for other tasks).
 * @param[in] __psem: pointer to the semaphore @ref vsf_sem_t
 * @param[in] __timeout: timeout in milliseconds, converted to ticks via
 *        vsf_systimer_ms_to_tick()
 *
 * @note Statement macro: the following brace block runs when the semaphore is
 *       obtained (VSF_SYNC_GET) or the wait times out (VSF_SYNC_TIMEOUT); use
 *       on_timeout() to catch the timeout case. Usage:
 *       vsf_sem_pend_timeout_ms(psem, timeout) { ... }. Must be used inside a
 *       simple-shell event handler.
 * \~chinese
 * @brief 在指定的毫秒数内等待信号量（协作式，不阻塞其他任务）。
 * @param[in] __psem: 指向信号量 @ref vsf_sem_t 的指针
 * @param[in] __timeout: 以毫秒为单位的超时时间，通过 vsf_systimer_ms_to_tick()
 *        转换为 tick
 *
 * @note 语句宏：当获得信号量（VSF_SYNC_GET）或等待超时（VSF_SYNC_TIMEOUT）
 *       时，紧随其后的花括号代码块执行；使用 on_timeout() 捕获超时情况。
 *       用法：vsf_sem_pend_timeout_ms(psem, timeout) { ... }。
 *       必须在 simple-shell 事件处理函数中使用。
 */
#   define vsf_sem_pend_timeout_ms(__psem, __timeout)                           \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_sem_pend((__psem),                           \
                    vsf_systimer_ms_to_tick(__timeout)),                        \
                    (reason == VSF_SYNC_GET || reason == VSF_SYNC_TIMEOUT)))

/**
 * \~english
 * @brief Wait for (pend on) a semaphore with a timeout in microseconds
 *        (cooperative, non-blocking for other tasks).
 * @param[in] __psem: pointer to the semaphore @ref vsf_sem_t
 * @param[in] __timeout: timeout in microseconds, converted to ticks via
 *        vsf_systimer_us_to_tick()
 *
 * @note Statement macro: the following brace block runs when the semaphore is
 *       obtained (VSF_SYNC_GET) or the wait times out (VSF_SYNC_TIMEOUT); use
 *       on_timeout() to catch the timeout case. Usage:
 *       vsf_sem_pend_timeout_us(psem, timeout) { ... }. Must be used inside a
 *       simple-shell event handler.
 * \~chinese
 * @brief 在指定的微秒数内等待信号量（协作式，不阻塞其他任务）。
 * @param[in] __psem: 指向信号量 @ref vsf_sem_t 的指针
 * @param[in] __timeout: 以微秒为单位的超时时间，通过 vsf_systimer_us_to_tick()
 *        转换为 tick
 *
 * @note 语句宏：当获得信号量（VSF_SYNC_GET）或等待超时（VSF_SYNC_TIMEOUT）
 *       时，紧随其后的花括号代码块执行；使用 on_timeout() 捕获超时情况。
 *       用法：vsf_sem_pend_timeout_us(psem, timeout) { ... }。
 *       必须在 simple-shell 事件处理函数中使用。
 */
#   define vsf_sem_pend_timeout_us(__psem, __timeout)                           \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_sem_pend((__psem),                           \
                    vsf_systimer_us_to_tick(__timeout)),                        \
                    (reason == VSF_SYNC_GET || reason == VSF_SYNC_TIMEOUT)))

/*----------------------------------------------------------------------------*
 * Event                                                                      *
 *----------------------------------------------------------------------------*/
/**
 * \~english
 * @brief Initialize a trigger.
 * @param[in] __ptrig: pointer to the trigger @ref vsf_trig_t
 * @param[in] __set: initial state, non-zero for set, zero for reset
 * @param[in] __auto_rst: non-zero to reset the trigger automatically after it
 *        is obtained, zero to reset it manually with vsf_trig_reset()
 * @return vsf_err_t: always VSF_ERR_NONE (invalid parameters trigger an assertion)
 * @note Must be called in task (or initialization) context, NOT in interrupt context.
 * \~chinese
 * @brief 初始化一个触发器。
 * @param[in] __ptrig: 指向触发器 @ref vsf_trig_t 的指针
 * @param[in] __set: 初始状态，非零表示置位，零表示复位
 * @param[in] __auto_rst: 非零表示触发器被获得后自动复位，零表示需要用
 *        vsf_trig_reset() 手动复位
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE（参数非法会触发断言）
 * @note 必须在任务（或初始化）上下文中调用，禁止在中断上下文中调用。
 */
#   define vsf_trig_init(__ptrig, __set, __auto_rst)                            \
            vsf_eda_trig_init(__ptrig, __set, __auto_rst)
/**
 * \~english
 * @brief Set a trigger, waking up the tasks waiting for it (all waiters for a
 *        manual-reset trigger, one waiter otherwise).
 * @param[in] __ptrig: pointer to the trigger @ref vsf_trig_t
 * @return vsf_err_t: VSF_ERR_NONE if successful; VSF_ERR_OVERRUN if the trigger is already set
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 置位一个触发器，唤醒等待它的任务（手动复位的触发器唤醒全部等待者，
 *        否则唤醒其中一个）。
 * @param[in] __ptrig: 指向触发器 @ref vsf_trig_t 的指针
 * @return vsf_err_t: 成功返回 VSF_ERR_NONE；触发器已是置位状态返回 VSF_ERR_OVERRUN
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
#   define vsf_trig_set(__ptrig)            vsf_eda_trig_set((__ptrig))
/**
 * \~english
 * @brief Reset a trigger.
 * @param[in] __ptrig: pointer to the trigger @ref vsf_trig_t
 * @return none
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 复位一个触发器。
 * @param[in] __ptrig: 指向触发器 @ref vsf_trig_t 的指针
 * @return 无
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
#   define vsf_trig_reset(__ptrig)          vsf_eda_trig_reset((__ptrig))

/**
 * \~english
 * @brief Wait for a trigger to be set, waiting forever (cooperative,
 *        non-blocking for other tasks).
 * @param[in] __ptrig: pointer to the trigger @ref vsf_trig_t
 *
 * @note Statement macro: the following brace block runs after the trigger is
 *       set. Usage: vsf_trig_wait(ptrig) { ... }. Must be used inside a
 *       simple-shell event handler.
 * \~chinese
 * @brief 等待触发器被置位，一直等待（协作式，不阻塞其他任务）。
 * @param[in] __ptrig: 指向触发器 @ref vsf_trig_t 的指针
 *
 * @note 语句宏：紧随其后的花括号代码块在触发器被置位后执行。
 *       用法：vsf_trig_wait(ptrig) { ... }。必须在 simple-shell 事件处理
 *       函数中使用。
 */
#   define vsf_trig_wait(__ptrig)                                               \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_sem_pend((__ptrig),                          \
                    (-1)), reason == VSF_SYNC_GET))

/**
 * \~english
 * @brief Wait for a trigger to be set with a timeout in system timer ticks
 *        (cooperative, non-blocking for other tasks).
 * @param[in] __ptrig: pointer to the trigger @ref vsf_trig_t
 * @param[in] __timeout: timeout in system timer ticks; a negative value waits
 *        forever
 *
 * @note Statement macro: the following brace block runs when the trigger is
 *       set (VSF_SYNC_GET) or the wait times out (VSF_SYNC_TIMEOUT); use
 *       on_timeout() to catch the timeout case. Usage:
 *       vsf_trig_wait_timeout(ptrig, timeout) { ... }. Must be used inside a
 *       simple-shell event handler.
 * \~chinese
 * @brief 在指定的系统定时器 tick 数内等待触发器被置位（协作式，
 *        不阻塞其他任务）。
 * @param[in] __ptrig: 指向触发器 @ref vsf_trig_t 的指针
 * @param[in] __timeout: 以系统定时器 tick 为单位的超时时间；负值表示一直等待
 *
 * @note 语句宏：当触发器被置位（VSF_SYNC_GET）或等待超时
 *       （VSF_SYNC_TIMEOUT）时，紧随其后的花括号代码块执行；使用
 *       on_timeout() 捕获超时情况。
 *       用法：vsf_trig_wait_timeout(ptrig, timeout) { ... }。
 *       必须在 simple-shell 事件处理函数中使用。
 */
#   define vsf_trig_wait_timeout(__ptrig, __timeout)                            \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_sem_pend((__ptrig),                          \
                    (__timeout)),                                               \
                    (reason == VSF_SYNC_GET || reason == VSF_SYNC_TIMEOUT)))


/**
 * \~english
 * @brief Wait for a trigger to be set with a timeout in milliseconds
 *        (cooperative, non-blocking for other tasks).
 * @param[in] __ptrig: pointer to the trigger @ref vsf_trig_t
 * @param[in] __timeout: timeout in milliseconds, converted to ticks via
 *        vsf_systimer_ms_to_tick()
 *
 * @note Statement macro: the following brace block runs when the trigger is
 *       set (VSF_SYNC_GET) or the wait times out (VSF_SYNC_TIMEOUT); use
 *       on_timeout() to catch the timeout case. Usage:
 *       vsf_trig_wait_timeout_ms(ptrig, timeout) { ... }. Must be used inside
 *       a simple-shell event handler.
 * \~chinese
 * @brief 在指定的毫秒数内等待触发器被置位（协作式，不阻塞其他任务）。
 * @param[in] __ptrig: 指向触发器 @ref vsf_trig_t 的指针
 * @param[in] __timeout: 以毫秒为单位的超时时间，通过 vsf_systimer_ms_to_tick()
 *        转换为 tick
 *
 * @note 语句宏：当触发器被置位（VSF_SYNC_GET）或等待超时
 *       （VSF_SYNC_TIMEOUT）时，紧随其后的花括号代码块执行；使用
 *       on_timeout() 捕获超时情况。
 *       用法：vsf_trig_wait_timeout_ms(ptrig, timeout) { ... }。
 *       必须在 simple-shell 事件处理函数中使用。
 */
#   define vsf_trig_wait_timeout_ms(__ptrig, __timeout)                         \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_sem_pend((__ptrig),                          \
                    vsf_systimer_ms_to_tick(__timeout)),                        \
                    (reason == VSF_SYNC_GET || reason == VSF_SYNC_TIMEOUT)))

/**
 * \~english
 * @brief Wait for a trigger to be set with a timeout in microseconds
 *        (cooperative, non-blocking for other tasks).
 * @param[in] __ptrig: pointer to the trigger @ref vsf_trig_t
 * @param[in] __timeout: timeout in microseconds, converted to ticks via
 *        vsf_systimer_us_to_tick()
 *
 * @note Statement macro: the following brace block runs when the trigger is
 *       set (VSF_SYNC_GET) or the wait times out (VSF_SYNC_TIMEOUT); use
 *       on_timeout() to catch the timeout case. Usage:
 *       vsf_trig_wait_timeout_us(ptrig, timeout) { ... }. Must be used inside
 *       a simple-shell event handler.
 * \~chinese
 * @brief 在指定的微秒数内等待触发器被置位（协作式，不阻塞其他任务）。
 * @param[in] __ptrig: 指向触发器 @ref vsf_trig_t 的指针
 * @param[in] __timeout: 以微秒为单位的超时时间，通过 vsf_systimer_us_to_tick()
 *        转换为 tick
 *
 * @note 语句宏：当触发器被置位（VSF_SYNC_GET）或等待超时
 *       （VSF_SYNC_TIMEOUT）时，紧随其后的花括号代码块执行；使用
 *       on_timeout() 捕获超时情况。
 *       用法：vsf_trig_wait_timeout_us(ptrig, timeout) { ... }。
 *       必须在 simple-shell 事件处理函数中使用。
 */
#   define vsf_trig_wait_timeout_us(__ptrig, __timeout)                         \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_sem_pend((__ptrig),                          \
                    vsf_systimer_us_to_tick(__timeout)),                        \
                    (reason == VSF_SYNC_GET || reason == VSF_SYNC_TIMEOUT)))

#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED

/*----------------------------------------------------------------------------*
 * sub call                                                                   *
 *----------------------------------------------------------------------------*/

/**
 * \~english
 * @brief Call a sub eda task (event handler) from the current task.
 * @param[in] __entry: entry function of the sub task
 * @param[in] __param_addr: parameter passed to the sub task
 * @return vsf_err_t: VSF_ERR_NONE if the call is started successfully; VSF_ERR_NOT_ENOUGH_RESOURCES if no frame is available
 *
 * @note Must be used inside a simple-shell event handler. After the sub task
 *       returns, the caller resumes with the VSF_EVT_RETURN event.
 * \~chinese
 * @brief 从当前任务调用一个子 eda 任务（事件处理函数）。
 * @param[in] __entry: 子任务的入口函数
 * @param[in] __param_addr: 传递给子任务的参数
 * @return vsf_err_t: 调用启动成功返回 VSF_ERR_NONE；没有可用的帧返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 必须在 simple-shell 事件处理函数中使用。子任务返回后，调用者以
 *       VSF_EVT_RETURN 事件恢复运行。
 */
#   define vsf_call_eda(__entry, __param_addr)                                  \
            __vsf_call_eda( (uintptr_t)(__entry),                               \
                            (__param_addr),                                     \
                            0, 0, 0)

/**
 * \~english
 * @brief Call a sub fsm task from the current task, and wait until it
 *        finishes. Internally mapped to __vsf_call_task(); an fsm is a vsf
 *        task in this codebase.
 * @param[in] __entry: entry function of the sub task
 * @param[in] __param_addr: parameter passed to the sub task
 * @param[in] ...: optional size of the local variables of the sub-task frame
 * @return fsm_rt_t: the return value of the sub fsm task when it finishes,
 *         or @ref fsm_rt_yield while the sub task is scheduled to run
 *
 * @note Must be used inside a simple-shell event handler. Only available when
 *       VSF_KERNEL_CFG_EDA_SUPPORT_TASK and
 *       VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE are ENABLED (the same
 *       requirements as the simple-fsm module).
 * \~chinese
 * @brief 从当前任务调用一个子 fsm 任务，并等待其完成。内部映射为
 *        __vsf_call_task()；在本代码库中 fsm 就是 vsf 任务。
 * @param[in] __entry: 子任务的入口函数
 * @param[in] __param_addr: 传递给子任务的参数
 * @param[in] ...: 子任务栈帧局部变量的可选大小
 * @return fsm_rt_t: 子 fsm 任务完成时返回其最终返回值；子任务被调度运行
 *         期间返回 @ref fsm_rt_yield
 *
 * @note 必须在 simple-shell 事件处理函数中使用。仅在
 *       VSF_KERNEL_CFG_EDA_SUPPORT_TASK 和
 *       VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE 为 ENABLED 时可用
 *       （与 simple-fsm 模块的要求相同）。
 */
#   define vsf_call_fsm(__entry, __param_addr, ...)                             \
            __vsf_call_task( (vsf_task_entry_t)(__entry),                       \
                             (uintptr_t)(__param_addr),                         \
                             (0, ##__VA_ARGS__))

/**
 * \~english
 * @brief Call a sub peda task (eda task with private local variables and
 *        argument buffer) from the current task.
 * @param[in] __peda_name: name of the peda task declared with the peda macros
 * @param[in] __entry: (vsf_call_peda4/3 only) entry function of the sub task
 * @param[in] __private_local_size: (vsf_call_peda4/3 only) extra private local
 *        size appended to the peda local variables
 * @param[in] __peda_param_addr: parameter passed to the sub task
 * @param[in] __buff: (vsf_call_peda4/2 only) buffer used to initialize the
 *        argument area of the sub task
 * @return vsf_err_t: VSF_ERR_NONE if the call is started successfully; VSF_ERR_NOT_ENOUGH_RESOURCES if no frame is available
 *
 * @note vsf_call_peda() dispatches to vsf_call_peda1/2/3/4 according to the
 *       argument count; the four helpers share this documentation.
 * @note Must be used inside a simple-shell event handler. After the sub task
 *       returns, the caller resumes with the VSF_EVT_RETURN event.
 * \~chinese
 * @brief 从当前任务调用一个子 peda 任务（带私有局部变量和参数缓冲区的
 *        eda 任务）。
 * @param[in] __peda_name: 用 peda 宏声明的 peda 任务名
 * @param[in] __entry: （仅 vsf_call_peda4/3）子任务的入口函数
 * @param[in] __private_local_size: （仅 vsf_call_peda4/3）追加到 peda 局部
 *        变量之后的私有局部空间大小
 * @param[in] __peda_param_addr: 传递给子任务的参数
 * @param[in] __buff: （仅 vsf_call_peda4/2）用于初始化子任务参数区的缓冲区
 * @return vsf_err_t: 调用启动成功返回 VSF_ERR_NONE；没有可用的帧返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note vsf_call_peda() 根据参数个数分发到 vsf_call_peda1/2/3/4；
 *       这四个辅助宏共用本说明。
 * @note 必须在 simple-shell 事件处理函数中使用。子任务返回后，调用者以
 *       VSF_EVT_RETURN 事件恢复运行。
 */
#   define vsf_call_peda4( __peda_name,                                         \
                        __entry,                                                \
                        __private_local_size,                                   \
                        __peda_param_addr,                                      \
                        __buff)                                                 \
            __vsf_call_eda((uintptr_t)__entry,                                  \
                (uintptr_t)(__peda_param_addr),                                 \
                ((sizeof(vsf_peda_local(__peda_name)) + 7) & ~0x07) + (__private_local_size),\
                sizeof(vsf_peda_arg(__peda_name)),                              \
                (uintptr_t)(__buff))

/**
 * \~english
 * @brief Helper of vsf_call_peda() with explicit entry and private local size,
 *        without the argument buffer; shares the documentation of vsf_call_peda4.
 * \~chinese
 * @brief vsf_call_peda() 的辅助宏，显式指定入口函数和私有局部空间，
 *        不带参数缓冲区；与 vsf_call_peda4 共用文档。
 */
#   define vsf_call_peda3( __peda_name,                                         \
                        __entry,                                                \
                        __private_local_size,                                   \
                        __peda_param_addr)                                      \
            __vsf_call_eda((uintptr_t)__entry,                                  \
                (uintptr_t)(__peda_param_addr),                                 \
                ((sizeof(vsf_peda_local(__peda_name)) + 7) & ~0x07) + (__private_local_size),\
                sizeof(vsf_peda_arg(__peda_name)),                              \
                0)

/**
 * \~english
 * @brief Helper of vsf_call_peda() with an explicit argument buffer, using the
 *        peda's own entry; shares the documentation of vsf_call_peda4.
 * \~chinese
 * @brief vsf_call_peda() 的辅助宏，显式指定参数缓冲区，使用 peda 自身的
 *        入口函数；与 vsf_call_peda4 共用文档。
 */
#   define vsf_call_peda2( __peda_name,                                         \
                        __peda_param_addr,                                      \
                        __buff)                                                 \
            __vsf_call_eda((uintptr_t)vsf_peda_func(__peda_name),               \
                        (uintptr_t)(__peda_param_addr),                         \
                        sizeof(vsf_peda_local(__peda_name)),                    \
                        sizeof(vsf_peda_arg(__peda_name)),                      \
                        (uintptr_t)(__buff))

/**
 * \~english
 * @brief Helper of vsf_call_peda() using the peda's own entry without the
 *        argument buffer; shares the documentation of vsf_call_peda4.
 * \~chinese
 * @brief vsf_call_peda() 的辅助宏，使用 peda 自身的入口函数，不带参数
 *        缓冲区；与 vsf_call_peda4 共用文档。
 */
#   define vsf_call_peda1( __peda_name,                                         \
                        __peda_param_addr)                                      \
            __vsf_call_eda((uintptr_t)vsf_peda_func(__peda_name),               \
                        (uintptr_t)(__peda_param_addr),                         \
                        sizeof(vsf_peda_local(__peda_name)),                    \
                        sizeof(vsf_peda_arg(__peda_name)),                      \
                        0)

#   define vsf_call_peda(__peda_name, ...)                                      \
            __PLOOC_EVAL(vsf_call_peda, __VA_ARGS__) (__peda_name, __VA_ARGS__)

#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
VSF_CAL_SECTION(".text.vsf.kernel.__vsf_delay")
/**
 * \~english
 * @brief Low-level delay helper backing the vsf_delay() statement macros.
 * @param[in] tick: delay time in system timer ticks
 * @return vsf_evt_t: VSF_EVT_TIMER when the delay expires, otherwise
 *         VSF_EVT_INVALID while the delay is still pending
 *
 * @note Internal use only.
 * \~chinese
 * @brief 支撑 vsf_delay() 语句宏的底层延迟辅助函数。
 * @param[in] tick: 以系统定时器 tick 为单位的延迟时间
 * @return vsf_evt_t: 延迟到期时返回 VSF_EVT_TIMER，延迟未到期时返回
 *         VSF_EVT_INVALID
 *
 * @note 仅供内部使用。
 */
extern vsf_evt_t __vsf_delay(vsf_systimer_tick_t tick);
#endif

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
VSF_CAL_SECTION(".text.vsf.kernel.__vsf_sem_pend")
/**
 * \~english
 * @brief Low-level semaphore-pend helper backing the vsf_sem_pend() and
 *        vsf_trig_wait() statement macros.
 * @param[in] sem_ptr: pointer to the semaphore @ref vsf_sem_t
 * @param[in] time_out: timeout in system timer ticks; a negative value waits
 *        forever
 * @return vsf_sync_reason_t: VSF_SYNC_GET when the semaphore is obtained,
 *         VSF_SYNC_TIMEOUT on timeout, VSF_SYNC_CANCEL when the wait is
 *         cancelled, VSF_SYNC_FAIL on error, otherwise VSF_SYNC_PENDING while
 *         the wait is still in progress
 *
 * @note Internal use only.
 * \~chinese
 * @brief 支撑 vsf_sem_pend() 和 vsf_trig_wait() 语句宏的底层信号量等待
 *        辅助函数。
 * @param[in] sem_ptr: 指向信号量 @ref vsf_sem_t 的指针
 * @param[in] time_out: 以系统定时器 tick 为单位的超时时间；负值表示一直等待
 * @return vsf_sync_reason_t: 获得信号量时返回 VSF_SYNC_GET，超时时返回
 *         VSF_SYNC_TIMEOUT，被取消时返回 VSF_SYNC_CANCEL，出错时返回
 *         VSF_SYNC_FAIL，等待仍在进行时返回 VSF_SYNC_PENDING
 *
 * @note 仅供内部使用。
 */
extern
vsf_sync_reason_t __vsf_sem_pend(vsf_sem_t *sem_ptr, vsf_timeout_tick_t time_out);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_mutex_enter")
/**
 * \~english
 * @brief Low-level mutex-enter helper backing the vsf_mutex_enter() and
 *        vsf_crit_enter() statement macros.
 * @param[in] mtx_ptr: pointer to the mutex @ref vsf_mutex_t
 * @param[in] time_out: timeout in system timer ticks; a negative value waits
 *        forever
 * @return vsf_sync_reason_t: VSF_SYNC_GET when the mutex is obtained,
 *         VSF_SYNC_TIMEOUT on timeout, VSF_SYNC_CANCEL when the wait is
 *         cancelled, VSF_SYNC_FAIL on error, otherwise VSF_SYNC_PENDING while
 *         the wait is still in progress
 *
 * @note Internal use only.
 * \~chinese
 * @brief 支撑 vsf_mutex_enter() 和 vsf_crit_enter() 语句宏的底层互斥量进入
 *        辅助函数。
 * @param[in] mtx_ptr: 指向互斥量 @ref vsf_mutex_t 的指针
 * @param[in] time_out: 以系统定时器 tick 为单位的超时时间；负值表示一直等待
 * @return vsf_sync_reason_t: 获得互斥量时返回 VSF_SYNC_GET，超时时返回
 *         VSF_SYNC_TIMEOUT，被取消时返回 VSF_SYNC_CANCEL，出错时返回
 *         VSF_SYNC_FAIL，等待仍在进行时返回 VSF_SYNC_PENDING
 *
 * @note 仅供内部使用。
 */
extern
vsf_sync_reason_t __vsf_mutex_enter(vsf_mutex_t *mtx_ptr, vsf_timeout_tick_t time_out);
#endif

#endif

VSF_CAL_SECTION(".text.vsf.kernel.vsf_yield")
/**
 * \~english
 * @brief Low-level yield helper backing the vsf_yield() statement macro.
 * @return vsf_evt_t: VSF_EVT_YIELD when the task is resumed after the yield,
 *         otherwise VSF_EVT_NONE while the yield is still pending
 *
 * @note Internal use only.
 * \~chinese
 * @brief 支撑 vsf_yield() 语句宏的底层让出辅助函数。
 * @return vsf_evt_t: 让出后任务恢复运行时返回 VSF_EVT_YIELD，让出仍未完成时
 *         返回 VSF_EVT_NONE
 *
 * @note 仅供内部使用。
 */
extern vsf_evt_t __vsf_yield(void);

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
VSF_CAL_SECTION(".text.vsf.kernel.vsf_call_eda_ex")
/**
 * \~english
 * @brief Low-level sub-task-call helper backing the vsf_call_eda() and
 *        vsf_call_peda() macros.
 * @param[in] evthandler: entry function of the sub task
 * @param[in] param: parameter passed to the sub task
 * @param[in] local_size: size of the local variable area of the sub task
 * @param[in] local_buff_size: size of the buffer pointed by local_buff
 * @param[in] local_buff: buffer used to initialize the local variable area of
 *        the sub task, NULL if not used
 * @return vsf_err_t: VSF_ERR_NONE if the call is started successfully; VSF_ERR_NOT_ENOUGH_RESOURCES if no frame is available
 *
 * @note Internal use only.
 * \~chinese
 * @brief 支撑 vsf_call_eda() 和 vsf_call_peda() 宏的底层子任务调用辅助函数。
 * @param[in] evthandler: 子任务的入口函数
 * @param[in] param: 传递给子任务的参数
 * @param[in] local_size: 子任务局部变量区的大小
 * @param[in] local_buff_size: local_buff 指向的缓冲区大小
 * @param[in] local_buff: 用于初始化子任务局部变量区的缓冲区，不使用时为 NULL
 * @return vsf_err_t: 调用启动成功返回 VSF_ERR_NONE；没有可用的帧返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 仅供内部使用。
 */
extern vsf_err_t __vsf_call_eda(uintptr_t evthandler,
                                uintptr_t param,
                                size_t local_size,
                                size_t local_buff_size,
                                uintptr_t local_buff);

#   if VSF_KERNEL_CFG_EDA_SUPPORT_TASK == ENABLED && VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
VSF_CAL_SECTION(".text.vsf.kernel.__vsf_call_task")
/**
 * \~english
 * @brief Low-level helper for calling a sub task with a return value.
 * @param[in] entry: entry function of the sub task @ref vsf_task_entry_t
 * @param[in] param: parameter passed to the sub task
 * @param[in] local_size: size of the local variable area of the sub task
 * @return fsm_rt_t: return status of the sub task
 *
 * @note Internal use only.
 * \~chinese
 * @brief 带返回值的子任务调用的底层辅助函数。
 * @param[in] entry: 子任务的入口函数 @ref vsf_task_entry_t
 * @param[in] param: 传递给子任务的参数
 * @param[in] local_size: 子任务局部变量区的大小
 * @return fsm_rt_t: 子任务的返回状态
 *
 * @note 仅供内部使用。
 */
extern fsm_rt_t __vsf_call_task(vsf_task_entry_t entry,
                                uintptr_t param,
                                size_t local_size);
#   endif

#ifdef __cplusplus
}
#endif

#endif
#endif
/* EOF */
