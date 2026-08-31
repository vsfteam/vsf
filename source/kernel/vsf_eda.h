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

#ifndef __VSF_EDA_H__
#define __VSF_EDA_H__

/*============================ INCLUDES ======================================*/

#include "kernel/vsf_kernel_cfg.h"

#if VSF_USE_KERNEL == ENABLED
#include "hal/arch/vsf_arch.h"
#include "service/vsf_service.h"

// for vsf_prio_t
#include "./vsf_kernel_common.h"

/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file
 *!        included in this file
 */

#if     defined(__VSF_EDA_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_EDA_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/**
 * \~english
 * @brief Auto-reset flag, OR-ed into max_value of vsf_eda_sync_init(): the count
 * is decreased automatically each time a pending task gets the sync object.
 * \~chinese
 * @brief 自动复位标志，与 vsf_eda_sync_init() 的 max_value 参数相或：每当一个
 * 等待中的任务获得同步对象时，计数自动减一。
 */
#define VSF_SYNC_AUTO_RST                   0x0000
/**
 * \~english
 * @brief Manual-reset flag, OR-ed into max_value of vsf_eda_sync_init(): the count
 * stays set until it is cleared by vsf_eda_sync_force_reset().
 * \~chinese
 * @brief 手动复位标志，与 vsf_eda_sync_init() 的 max_value 参数相或：计数保持
 * 置位状态，直到调用 vsf_eda_sync_force_reset() 清除。
 */
#define VSF_SYNC_MANUAL_RST                 0x8000
/**
 * \~english
 * @brief Has-owner flag, OR-ed into cur_value of vsf_eda_sync_init(): the sync
 * object has an owner (e.g. a mutex) and supports priority inheritance.
 * \~chinese
 * @brief 带持有者标志，与 vsf_eda_sync_init() 的 cur_value 参数相或：同步对象
 * 拥有持有者（例如互斥量），并支持优先级继承。
 */
#define VSF_SYNC_HAS_OWNER                  0x8000

/**
 * \~english
 * @brief Maximum count value of a sync object @ref vsf_sync_t
 * \~chinese
 * @brief 同步对象 @ref vsf_sync_t 的最大计数值
 */
#define VSF_SYNC_MAX                        0x7FFF

#ifndef VSF_KERNEL_CFG_EDA_USER_BITLEN
#   define VSF_KERNEL_CFG_EDA_USER_BITLEN   5
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

// SEMAPHORE
#define __vsf_eda_sem_init2(__psem, __init_cnt, __max_cnt)                      \
            vsf_eda_sync_init((__psem), (__init_cnt), (__max_cnt) | VSF_SYNC_AUTO_RST)
#define __vsf_eda_sem_init1(__psem, __init_cnt)                                 \
            __vsf_eda_sem_init2((__psem), (__init_cnt), VSF_SYNC_MAX)
#define __vsf_eda_sem_init0(__psem)                                             \
            __vsf_eda_sem_init1((__psem), 0)
// prototype: vsf_err_t vsf_eda_sem_init(vsf_sem_t *sem, uint_fast16_t init_cnt = 0, uint_fast16_t max_cnt = VSF_SYNC_MAX);
/**
 * \~english
 * @brief Initialize a semaphore
 * @param[in] sem: a pointer to structure @ref vsf_sem_t
 * @param[in] init_cnt: initial count, 0 by default
 * @param[in] max_cnt: maximum count, @ref VSF_SYNC_MAX by default
 * @return vsf_err_t: always VSF_ERR_NONE (invalid parameters trigger an assertion)
 * @note Must be called in task (or initialization) context, NOT in interrupt context.
 * \~chinese
 * @brief 初始化一个信号量
 * @param[in] sem: 指向结构体 @ref vsf_sem_t 的指针
 * @param[in] init_cnt: 初始计数，默认为 0
 * @param[in] max_cnt: 最大计数，默认为 @ref VSF_SYNC_MAX
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE（参数非法会触发断言）
 * @note 必须在任务（或初始化）上下文中调用，禁止在中断上下文中调用。
 */
#define vsf_eda_sem_init(__psem, ...)                                           \
            __PLOOC_EVAL(__vsf_eda_sem_init, __VA_ARGS__)((__psem), ##__VA_ARGS__)

/**
 * \~english
 * @brief Post (increase) a semaphore, waking up one pending task if any
 * @param[in] sem: a pointer to structure @ref vsf_sem_t
 * @return vsf_err_t: VSF_ERR_NONE if successful, VSF_ERR_OVERRUN if the count has reached the maximum
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 释放（增加）一个信号量，如果有任务在等待则唤醒其中一个
 * @param[in] sem: 指向结构体 @ref vsf_sem_t 的指针
 * @return vsf_err_t: 成功返回 VSF_ERR_NONE，如果计数已达到最大值返回 VSF_ERR_OVERRUN
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
#define vsf_eda_sem_post(__psem)            vsf_eda_sync_increase((__psem))

#define __vsf_eda_sem_pend1(__psem, __timeout)  vsf_eda_sync_decrease((__psem), (__timeout))
#define __vsf_eda_sem_pend0(__psem)         __vsf_eda_sem_pend1((__psem), -1)
// prototype: vsf_err_t vsf_eda_sem_pend(vsf_sem_t *sem, vsf_timeout_tick_t timeout = -1);
/**
 * \~english
 * @brief Pend (decrease) on a semaphore
 * @param[in] sem: a pointer to structure @ref vsf_sem_t
 * @param[in] timeout: timeout in ticks, -1 (wait forever) by default; 0 means a
 * non-blocking try; a positive value waits up to the given ticks and requires
 * timer support (i.e. the caller must be a teda task)
 * @return vsf_err_t: VSF_ERR_NONE if the semaphore is obtained immediately;
 * VSF_ERR_NOT_READY if the caller pends or the try failed
 *
 * @note If VSF_ERR_NOT_READY is returned with a non-zero timeout, the caller is
 * woken later with VSF_EVT_SYNC, VSF_EVT_TIMER (timeout) or VSF_EVT_SYNC_CANCEL;
 * call vsf_eda_sync_get_reason() to retrieve the result.
 * @note Must be called in an eda task context (i.e. inside an event handler); must NOT be called in interrupt context.
 * \~chinese
 * @brief 等待（减少）一个信号量
 * @param[in] sem: 指向结构体 @ref vsf_sem_t 的指针
 * @param[in] timeout: 超时 tick 数，默认为 -1（永远等待）；0 表示不等待的尝试；
 * 大于 0 表示最多等待对应的 tick 数，需要定时器支持（即调用者必须是 teda 任务）
 * @return vsf_err_t: 如果立即获得信号量返回 VSF_ERR_NONE；如果调用者进入等待
 * 或者尝试失败返回 VSF_ERR_NOT_READY
 *
 * @note 如果以非零超时返回 VSF_ERR_NOT_READY，调用者之后会收到 VSF_EVT_SYNC、
 * VSF_EVT_TIMER（超时）或 VSF_EVT_SYNC_CANCEL 事件被唤醒；调用
 * vsf_eda_sync_get_reason() 获取结果。
 * @note 必须在 eda 任务上下文（事件处理函数）中调用，禁止在中断上下文中调用。
 */
#define vsf_eda_sem_pend(__psem, ...)                                           \
            __PLOOC_EVAL(__vsf_eda_sem_pend, __VA_ARGS__)((__psem), ##__VA_ARGS__)

#if VSF_SYNC_CFG_SUPPORT_ISR == ENABLED
/**
 * \~english
 * @brief Post (increase) a semaphore from interrupt context
 * @param[in] sem: a pointer to structure @ref vsf_sem_t
 * @return vsf_err_t: VSF_ERR_NONE if the request is posted to the kernel task; VSF_ERR_NOT_ENOUGH_RESOURCES if no event node is available
 *
 * @note Only available when VSF_SYNC_CFG_SUPPORT_ISR is ENABLED. The actual
 * increase is done by the kernel task.
 * @note May also be called in task context, but that is less efficient (the
 * request is bounced to the kernel task instead of being executed directly) —
 * prefer vsf_eda_sem_post() there.
 * \~chinese
 * @brief 在中断上下文里释放（增加）一个信号量
 * @param[in] sem: 指向结构体 @ref vsf_sem_t 的指针
 * @return vsf_err_t: 请求已发送给内核任务返回 VSF_ERR_NONE；没有可用的事件节点返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 仅在 VSF_SYNC_CFG_SUPPORT_ISR 使能时可用。实际的增加操作由内核任务完成。
 * @note 也可以在任务上下文中调用，但效率较低（请求经内核任务跳板，而不是
 * 直接执行）——任务上下文里建议使用 vsf_eda_sem_post()。
 */
#   define vsf_eda_sem_post_isr(__psem)     vsf_eda_sync_increase_isr((__psem))
#endif

// MUTEX
/**
 * \~english
 * @brief Initialize a mutex
 * @param[in] mutex: a pointer to structure @ref vsf_mutex_t
 * @return vsf_err_t: always VSF_ERR_NONE (invalid parameters trigger an assertion)
 *
 * @note vsf_mutex_t supports priority inheritance; the task which enters a mutex
 * must leave it by itself.
 * @note Must be called in task (or initialization) context, NOT in interrupt context.
 * \~chinese
 * @brief 初始化一个互斥量
 * @param[in] mutex: 指向结构体 @ref vsf_mutex_t 的指针
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE（参数非法会触发断言）
 *
 * @note vsf_mutex_t 支持优先级继承；进入互斥量的任务必须由它自己离开互斥量。
 * @note 必须在任务（或初始化）上下文中调用，禁止在中断上下文中调用。
 */
#define vsf_eda_mutex_init(__pmtx)                                              \
            vsf_eda_sync_init(  &((__pmtx)->use_as__vsf_sync_t),                \
                                1 | VSF_SYNC_HAS_OWNER,                         \
                                1 | VSF_SYNC_AUTO_RST)

#define __vsf_eda_mutex_enter1(__pmtx, __timeout)                               \
            vsf_eda_sync_decrease(&((__pmtx)->use_as__vsf_sync_t), (__timeout))
#define __vsf_eda_mutex_enter0(__pmtx)      __vsf_eda_mutex_enter1((__pmtx), -1)
// prototype: vsf_err_t vsf_eda_mutex_enter(vsf_mutex_t *mutex, vsf_timeout_tick_t timeout = -1);
/**
 * \~english
 * @brief Enter (lock) a mutex
 * @param[in] mutex: a pointer to structure @ref vsf_mutex_t
 * @param[in] timeout: timeout in ticks, -1 (wait forever) by default; 0 means a
 * non-blocking try; a positive value waits up to the given ticks and requires
 * timer support (i.e. the caller must be a teda task)
 * @return vsf_err_t: VSF_ERR_NONE if the mutex is obtained immediately;
 * VSF_ERR_NOT_READY if the caller pends or the try failed
 *
 * @note If VSF_ERR_NOT_READY is returned with a non-zero timeout, the caller is
 * woken later with VSF_EVT_SYNC, VSF_EVT_TIMER (timeout) or VSF_EVT_SYNC_CANCEL;
 * call vsf_eda_sync_get_reason() to retrieve the result.
 * @note Must be called in an eda task context (i.e. inside an event handler); must NOT be called in interrupt context.
 * \~chinese
 * @brief 进入（加锁）一个互斥量
 * @param[in] mutex: 指向结构体 @ref vsf_mutex_t 的指针
 * @param[in] timeout: 超时 tick 数，默认为 -1（永远等待）；0 表示不等待的尝试；
 * 大于 0 表示最多等待对应的 tick 数，需要定时器支持（即调用者必须是 teda 任务）
 * @return vsf_err_t: 如果立即获得互斥量返回 VSF_ERR_NONE；如果调用者进入等待
 * 或者尝试失败返回 VSF_ERR_NOT_READY
 *
 * @note 如果以非零超时返回 VSF_ERR_NOT_READY，调用者之后会收到 VSF_EVT_SYNC、
 * VSF_EVT_TIMER（超时）或 VSF_EVT_SYNC_CANCEL 事件被唤醒；调用
 * vsf_eda_sync_get_reason() 获取结果。
 * @note 必须在 eda 任务上下文（事件处理函数）中调用，禁止在中断上下文中调用。
 */
#define vsf_eda_mutex_enter(__pmtx, ...)                                        \
            __PLOOC_EVAL(__vsf_eda_mutex_enter, __VA_ARGS__)((__pmtx), ##__VA_ARGS__)

/**
 * \~english
 * @brief Leave (unlock) a mutex, waking up one pending task if any
 * @param[in] mutex: a pointer to structure @ref vsf_mutex_t
 * @return vsf_err_t: VSF_ERR_NONE if successful, VSF_ERR_OVERRUN if the mutex is not owned
 *
 * @note Must be called by the task which entered the mutex.
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 离开（解锁）一个互斥量，如果有任务在等待则唤醒其中一个
 * @param[in] mutex: 指向结构体 @ref vsf_mutex_t 的指针
 * @return vsf_err_t: 成功返回 VSF_ERR_NONE，如果互斥量未被持有返回 VSF_ERR_OVERRUN
 *
 * @note 必须由进入该互斥量的任务调用。
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
#define vsf_eda_mutex_leave(__pmtx)                                             \
            vsf_eda_sync_increase(&((__pmtx)->use_as__vsf_sync_t))
#if VSF_SYNC_CFG_SUPPORT_ISR == ENABLED
/**
 * \~english
 * @brief Leave (unlock) a mutex from interrupt context
 * @param[in] mutex: a pointer to structure @ref vsf_mutex_t
 * @return vsf_err_t: VSF_ERR_NONE if the request is posted to the kernel task; VSF_ERR_NOT_ENOUGH_RESOURCES if no event node is available
 *
 * @note Only available when VSF_SYNC_CFG_SUPPORT_ISR is ENABLED. The actual
 * leave is done by the kernel task.
 * @note May also be called in task context, but that is less efficient (the
 * request is bounced to the kernel task instead of being executed directly) —
 * prefer vsf_eda_mutex_leave() there.
 * \~chinese
 * @brief 在中断上下文里离开（解锁）一个互斥量
 * @param[in] mutex: 指向结构体 @ref vsf_mutex_t 的指针
 * @return vsf_err_t: 请求已发送给内核任务返回 VSF_ERR_NONE；没有可用的事件节点返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 仅在 VSF_SYNC_CFG_SUPPORT_ISR 使能时可用。实际的离开操作由内核任务完成。
 * @note 也可以在任务上下文中调用，但效率较低（请求经内核任务跳板，而不是
 * 直接执行）——任务上下文里建议使用 vsf_eda_mutex_leave()。
 */
#   define vsf_eda_mutex_leave_isr(__pmtx)  vsf_eda_sync_increase_isr(&(__pmtx)->use_as__vsf_sync_t)
#endif

// CRIT
/**
 * \~english
 * @brief Initialize a critical section
 * @param[in] crit: a pointer to structure @ref vsf_crit_t
 * @return vsf_err_t: always VSF_ERR_NONE (invalid parameters trigger an assertion)
 *
 * @note vsf_crit_t has the same semantics as @ref vsf_mutex_t.
 * @note Must be called in task (or initialization) context, NOT in interrupt context.
 * \~chinese
 * @brief 初始化一个临界代码段
 * @param[in] crit: 指向结构体 @ref vsf_crit_t 的指针
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE（参数非法会触发断言）
 *
 * @note vsf_crit_t 与 @ref vsf_mutex_t 语义相同。
 * @note 必须在任务（或初始化）上下文中调用，禁止在中断上下文中调用。
 */
#define vsf_eda_crit_init(__pcrit)                                              \
            vsf_eda_mutex_init((__pcrit))

#define __vsf_eda_crit_enter1(__pcrit, __timeout)                               \
            vsf_eda_mutex_enter((__pcrit), (__timeout))
#define __vsf_eda_crit_enter0(__pcrit)      __vsf_eda_crit_enter1((__pcrit), -1)
// prototype: vsf_err_t vsf_eda_crit_enter(vsf_crit_t *crit, vsf_timeout_tick_t timeout = -1);
/**
 * \~english
 * @brief Enter a critical section, same semantics as vsf_eda_mutex_enter()
 * @param[in] crit: a pointer to structure @ref vsf_crit_t
 * @param[in] timeout: timeout in ticks, -1 (wait forever) by default; 0 means a
 * non-blocking try; a positive value waits up to the given ticks and requires
 * timer support (i.e. the caller must be a teda task)
 * @return vsf_err_t: VSF_ERR_NONE if the critical section is obtained immediately;
 * VSF_ERR_NOT_READY if the caller pends or the try failed
 *
 * @note If VSF_ERR_NOT_READY is returned with a non-zero timeout, the caller is
 * woken later with VSF_EVT_SYNC, VSF_EVT_TIMER (timeout) or VSF_EVT_SYNC_CANCEL;
 * call vsf_eda_sync_get_reason() to retrieve the result.
 * @note Must be called in an eda task context (i.e. inside an event handler); must NOT be called in interrupt context.
 * \~chinese
 * @brief 进入一个临界代码段，与 vsf_eda_mutex_enter() 语义相同
 * @param[in] crit: 指向结构体 @ref vsf_crit_t 的指针
 * @param[in] timeout: 超时 tick 数，默认为 -1（永远等待）；0 表示不等待的尝试；
 * 大于 0 表示最多等待对应的 tick 数，需要定时器支持（即调用者必须是 teda 任务）
 * @return vsf_err_t: 如果立即获得临界代码段返回 VSF_ERR_NONE；如果调用者进入
 * 等待或者尝试失败返回 VSF_ERR_NOT_READY
 *
 * @note 如果以非零超时返回 VSF_ERR_NOT_READY，调用者之后会收到 VSF_EVT_SYNC、
 * VSF_EVT_TIMER（超时）或 VSF_EVT_SYNC_CANCEL 事件被唤醒；调用
 * vsf_eda_sync_get_reason() 获取结果。
 * @note 必须在 eda 任务上下文（事件处理函数）中调用，禁止在中断上下文中调用。
 */
#define vsf_eda_crit_enter(__pcrit, ...)                                        \
            __PLOOC_EVAL(__vsf_eda_crit_enter, __VA_ARGS__)((__pcrit), ##__VA_ARGS__)

/**
 * \~english
 * @brief Leave a critical section, same semantics as vsf_eda_mutex_leave()
 * @param[in] crit: a pointer to structure @ref vsf_crit_t
 * @return vsf_err_t: VSF_ERR_NONE if successful, VSF_ERR_OVERRUN if the critical section is not owned
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 离开一个临界代码段，与 vsf_eda_mutex_leave() 语义相同
 * @param[in] crit: 指向结构体 @ref vsf_crit_t 的指针
 * @return vsf_err_t: 成功返回 VSF_ERR_NONE，如果临界代码段未被持有返回 VSF_ERR_OVERRUN
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
#define vsf_eda_crit_leave(__pcrit)                                             \
            vsf_eda_mutex_leave((__pcrit))

// EVENT
/**
 * \~english
 * @brief Initialize a trigger (a sync object with maximum count 1)
 * @param[in] trig: a pointer to structure @ref vsf_trig_t
 * @param[in] is_set: initial state, non-zero for set(triggered), 0 for reset
 * @param[in] is_auto_rst: non-zero for auto-reset mode, 0 for manual-reset mode
 * @return vsf_err_t: always VSF_ERR_NONE (invalid parameters trigger an assertion)
 * @note Must be called in task (or initialization) context, NOT in interrupt context.
 * \~chinese
 * @brief 初始化一个触发器（最大计数为 1 的同步对象）
 * @param[in] trig: 指向结构体 @ref vsf_trig_t 的指针
 * @param[in] is_set: 初始状态，非 0 表示已触发，0 表示未触发
 * @param[in] is_auto_rst: 非 0 表示自动复位模式，0 表示手动复位模式
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE（参数非法会触发断言）
 * @note 必须在任务（或初始化）上下文中调用，禁止在中断上下文中调用。
 */
#define vsf_eda_trig_init(__pevt, __set, __auto_rst)                            \
            vsf_eda_sync_init((__pevt), (__set),                                \
                    1 | ((__auto_rst) ? VSF_SYNC_AUTO_RST : VSF_SYNC_MANUAL_RST))

#define vsf_eda_trig_set0(__pevt)           vsf_eda_sync_increase((__pevt))
#define vsf_eda_trig_set1(__pevt, __manual)                                     \
            __vsf_eda_sync_increase_ex((__pevt), NULL, (__manual))
// prototype: vsf_err_t vsf_eda_trig_set(vsf_trig_t *trig, bool manual);
// prototype: vsf_err_t vsf_eda_trig_set(vsf_trig_t *trig);
/**
 * \~english
 * @brief Set (trigger) a trigger, waking up the pending tasks
 * @param[in] trig: a pointer to structure @ref vsf_trig_t
 * @param[in] manual: optional; non-zero to force manual-reset behavior for this
 * set (the trigger stays set until vsf_eda_trig_reset()); if omitted, the reset
 * mode configured by vsf_eda_trig_init() is used
 * @return vsf_err_t: VSF_ERR_NONE if successful, VSF_ERR_OVERRUN if the trigger is already set
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 设置（触发）一个触发器，唤醒等待中的任务
 * @param[in] trig: 指向结构体 @ref vsf_trig_t 的指针
 * @param[in] manual: 可选参数；非 0 表示本次设置强制使用手动复位行为（触发器
 * 保持触发状态，直到调用 vsf_eda_trig_reset()）；如果省略，则使用
 * vsf_eda_trig_init() 配置的复位模式
 * @return vsf_err_t: 成功返回 VSF_ERR_NONE，如果触发器已经处于触发状态返回 VSF_ERR_OVERRUN
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
#define vsf_eda_trig_set(__pevt, ...)                                           \
            __PLOOC_EVAL(vsf_eda_trig_set, __VA_ARGS__)((__pevt), ##__VA_ARGS__)

/**
 * \~english
 * @brief Reset a trigger to the untriggered state
 * @param[in] trig: a pointer to structure @ref vsf_trig_t
 * @return none
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 复位一个触发器到未触发状态
 * @param[in] trig: 指向结构体 @ref vsf_trig_t 的指针
 * @return 无
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
#define vsf_eda_trig_reset(__pevt)          vsf_eda_sync_force_reset((__pevt))

#define __vsf_eda_trig_wait1(__pevt, __timeout)                                 \
            vsf_eda_sync_decrease((__pevt), (__timeout))
#define __vsf_eda_trig_wait0(__pevt)        __vsf_eda_trig_wait1((__pevt), -1)
/**
 * \~english
 * @brief Wait for a trigger to be set
 * @param[in] trig: a pointer to structure @ref vsf_trig_t
 * @param[in] timeout: timeout in ticks, -1 (wait forever) by default; 0 means a
 * non-blocking try; a positive value waits up to the given ticks and requires
 * timer support (i.e. the caller must be a teda task)
 * @return vsf_err_t: VSF_ERR_NONE if the trigger is set; VSF_ERR_NOT_READY if
 * the caller pends or the try failed
 *
 * @note If VSF_ERR_NOT_READY is returned with a non-zero timeout, the caller is
 * woken later with VSF_EVT_SYNC, VSF_EVT_TIMER (timeout) or VSF_EVT_SYNC_CANCEL;
 * call vsf_eda_sync_get_reason() to retrieve the result.
 * @note Must be called in an eda task context (i.e. inside an event handler); must NOT be called in interrupt context.
 * \~chinese
 * @brief 等待一个触发器被触发
 * @param[in] trig: 指向结构体 @ref vsf_trig_t 的指针
 * @param[in] timeout: 超时 tick 数，默认为 -1（永远等待）；0 表示不等待的尝试；
 * 大于 0 表示最多等待对应的 tick 数，需要定时器支持（即调用者必须是 teda 任务）
 * @return vsf_err_t: 如果触发器已触发返回 VSF_ERR_NONE；如果调用者进入等待
 * 或者尝试失败返回 VSF_ERR_NOT_READY
 *
 * @note 如果以非零超时返回 VSF_ERR_NOT_READY，调用者之后会收到 VSF_EVT_SYNC、
 * VSF_EVT_TIMER（超时）或 VSF_EVT_SYNC_CANCEL 事件被唤醒；调用
 * vsf_eda_sync_get_reason() 获取结果。
 * @note 必须在 eda 任务上下文（事件处理函数）中调用，禁止在中断上下文中调用。
 */
#define vsf_eda_trig_wait(__pevt, ...)                                          \
            __PLOOC_EVAL(__vsf_eda_trig_wait, __VA_ARGS__)((__pevt), ##__VA_ARGS__)

#if VSF_SYNC_CFG_SUPPORT_ISR == ENABLED
/**
 * \~english
 * @brief Set (trigger) a trigger from interrupt context
 * @param[in] trig: a pointer to structure @ref vsf_trig_t
 * @return vsf_err_t: VSF_ERR_NONE if the request is posted to the kernel task; VSF_ERR_NOT_ENOUGH_RESOURCES if no event node is available
 *
 * @note Only available when VSF_SYNC_CFG_SUPPORT_ISR is ENABLED. The actual
 * set is done by the kernel task.
 * @note May also be called in task context, but that is less efficient (the
 * request is bounced to the kernel task instead of being executed directly) —
 * prefer vsf_eda_trig_set() there.
 * \~chinese
 * @brief 在中断上下文里设置（触发）一个触发器
 * @param[in] trig: 指向结构体 @ref vsf_trig_t 的指针
 * @return vsf_err_t: 请求已发送给内核任务返回 VSF_ERR_NONE；没有可用的事件节点返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 仅在 VSF_SYNC_CFG_SUPPORT_ISR 使能时可用。实际的设置操作由内核任务完成。
 * @note 也可以在任务上下文中调用，但效率较低（请求经内核任务跳板，而不是
 * 直接执行）——任务上下文里建议使用 vsf_eda_trig_set()。
 */
#   define vsf_eda_trig_set_isr(__pevt)     vsf_eda_sync_increase_isr((__pevt))
#endif

// CRIT without priority boost, internal use only
// only used for edas with same priority
/**
 * \~english
 * @brief Initialize a critical section without priority boost
 * @param[in] crit_npb: a pointer to structure @ref __vsf_crit_npb_t
 * @return vsf_err_t: always VSF_ERR_NONE
 *
 * @note Internal use only. Only used for edas with the same priority.
 * \~chinese
 * @brief 初始化一个不带优先级提升的临界代码段
 * @param[in] crit_npb: 指向结构体 @ref __vsf_crit_npb_t 的指针
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE
 *
 * @note 仅供内部使用。仅用于相同优先级的 eda 任务之间。
 */
#define __vsf_eda_crit_npb_init(__pcrit)                                        \
            vsf_eda_sync_init((__pcrit), 1, 1 | VSF_SYNC_AUTO_RST)

#define __vsf_eda_crit_npb_enter1(__pcrit, __timeout)                           \
            vsf_eda_sync_decrease((__pcrit), (__timeout))
#define __vsf_eda_crit_npb_enter0(__pcrit)  __vsf_eda_crit_npb_enter1((__pcrit), -1)
// prototype: vsf_err_t __vsf_eda_crit_npb_enter(__vsf_crit_npb_t *crit_npb, vsf_timeout_tick_t timeout = -1);
/**
 * \~english
 * @brief Enter a critical section without priority boost
 * @param[in] crit_npb: a pointer to structure @ref __vsf_crit_npb_t
 * @param[in] timeout: timeout in ticks, -1 (wait forever) by default; 0 means a
 * non-blocking try; a positive value waits up to the given ticks and requires
 * timer support (i.e. the caller must be a teda task)
 * @return vsf_err_t: VSF_ERR_NONE if the critical section is obtained immediately;
 * VSF_ERR_NOT_READY if the caller pends or the try failed
 *
 * @note Internal use only.
 * @note Must be called in an eda task context (i.e. inside an event handler); must NOT be called in interrupt context.
 * \~chinese
 * @brief 进入一个不带优先级提升的临界代码段
 * @param[in] crit_npb: 指向结构体 @ref __vsf_crit_npb_t 的指针
 * @param[in] timeout: 超时 tick 数，默认为 -1（永远等待）；0 表示不等待的尝试；
 * 大于 0 表示最多等待对应的 tick 数，需要定时器支持（即调用者必须是 teda 任务）
 * @return vsf_err_t: 如果立即获得临界代码段返回 VSF_ERR_NONE；如果调用者进入
 * 等待或者尝试失败返回 VSF_ERR_NOT_READY
 *
 * @note 仅供内部使用。
 * @note 必须在 eda 任务上下文（事件处理函数）中调用，禁止在中断上下文中调用。
 */
#define __vsf_eda_crit_npb_enter(__pcrit, ...)                                  \
            __PLOOC_EVAL(__vsf_eda_crit_npb_enter, __VA_ARGS__)((__pcrit), ##__VA_ARGS__)

/**
 * \~english
 * @brief Leave a critical section without priority boost
 * @param[in] crit_npb: a pointer to structure @ref __vsf_crit_npb_t
 * @return vsf_err_t: VSF_ERR_NONE if successful, VSF_ERR_OVERRUN if the critical section is not owned
 *
 * @note Internal use only.
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 离开一个不带优先级提升的临界代码段
 * @param[in] crit_npb: 指向结构体 @ref __vsf_crit_npb_t 的指针
 * @return vsf_err_t: 成功返回 VSF_ERR_NONE，如果临界代码段未被持有返回 VSF_ERR_OVERRUN
 *
 * @note 仅供内部使用。
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
#define __vsf_eda_crit_npb_leave(__pcrit)                                       \
            vsf_eda_sync_increase((__pcrit))

#define __vsf_eda_init2(__eda, __priority, __feature)                           \
            __vsf_eda_init((__eda), (__priority), (__feature))
#define __vsf_eda_init1(__eda, __priority)                                      \
            __vsf_eda_init2((__eda), (__priority), (vsf_eda_feature_t){.value = 0})
#define __vsf_eda_init0(__eda)                                                  \
            __vsf_eda_init1((__eda), vsf_prio_inherit)
// prototype: vsf_err_t vsf_eda_init(vsf_eda_t *eda, vsf_prio_t prio = vsf_prio_inherit, vsf_eda_feature feature = 0);
/**
 * \~english
 * @brief Initialize an eda (event-driven task) and post VSF_EVT_INIT to it
 * @param[in] eda: a pointer to structure @ref vsf_eda_t
 * @param[in] prio: priority of the task, vsf_prio_inherit (inherit the current
 * priority) by default
 * @param[in] feature: task feature @ref vsf_eda_feature_t, 0 by default
 * @return vsf_err_t: VSF_ERR_NONE if the INIT event is posted; VSF_ERR_NOT_ENOUGH_RESOURCES if no event node is available
 *
 * @note The task receives VSF_EVT_INIT once when it is started.
 * @note Must be called in task (or initialization) context, NOT in interrupt context.
 * \~chinese
 * @brief 初始化一个 eda（事件驱动任务），并向其发送 VSF_EVT_INIT 事件
 * @param[in] eda: 指向结构体 @ref vsf_eda_t 的指针
 * @param[in] prio: 任务优先级，默认为 vsf_prio_inherit（继承当前优先级）
 * @param[in] feature: 任务特性 @ref vsf_eda_feature_t，默认为 0
 * @return vsf_err_t: INIT 事件发送成功返回 VSF_ERR_NONE；没有可用的事件节点返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 任务启动时会收到一次 VSF_EVT_INIT 事件。
 * @note 必须在任务（或初始化）上下文中调用，禁止在中断上下文中调用。
 */
#define vsf_eda_init(__eda, ...)                                                \
            __PLOOC_EVAL(__vsf_eda_init, __VA_ARGS__)((__eda), ##__VA_ARGS__)

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
/**
 * \~english
 * @brief Initialize a teda (eda with timer support), same arguments as vsf_eda_init()
 * @param[in] teda: a pointer to structure @ref vsf_teda_t
 * @param[in] prio: priority of the task, vsf_prio_inherit by default
 * @param[in] feature: task feature @ref vsf_eda_feature_t, 0 by default
 * @return vsf_err_t: VSF_ERR_NONE if the INIT event is posted; VSF_ERR_NOT_ENOUGH_RESOURCES if no event node is available
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_TIMER is ENABLED.
 * @note Must be called in task (or initialization) context, NOT in interrupt context.
 * \~chinese
 * @brief 初始化一个 teda（带定时器支持的 eda），参数与 vsf_eda_init() 相同
 * @param[in] teda: 指向结构体 @ref vsf_teda_t 的指针
 * @param[in] prio: 任务优先级，默认为 vsf_prio_inherit
 * @param[in] feature: 任务特性 @ref vsf_eda_feature_t，默认为 0
 * @return vsf_err_t: INIT 事件发送成功返回 VSF_ERR_NONE；没有可用的事件节点返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_TIMER 使能时可用。
 * @note 必须在任务（或初始化）上下文中调用，禁止在中断上下文中调用。
 */
#   define vsf_teda_init(__teda, ...)       vsf_eda_init(&(__teda)->use_as__vsf_eda_t, ##__VA_ARGS__)
#endif

/**
 * \~english
 * @brief Return from the current (sub-called) eda with an optional return value
 * @param[in] return_value: optional return value passed back to the caller, 0 by default
 * @return bool: true if the current task is terminated (no caller), false if
 * control is returned to the caller
 *
 * @note The caller is woken with VSF_EVT_RETURN and can retrieve the value with
 * vsf_eda_get_return_value().
 * @note Operates on the current task; must be called in the current eda/teda task's event handler context.
 * \~chinese
 * @brief 从当前（被子调用的）eda 返回，可带一个可选的返回值
 * @param[in] return_value: 可选的返回值，传递回调用者，默认为 0
 * @return bool: 如果当前任务被终止（没有调用者）返回 true，如果返回到调用者
 * 返回 false
 *
 * @note 调用者会收到 VSF_EVT_RETURN 事件被唤醒，并可通过
 * vsf_eda_get_return_value() 获取返回值。
 * @note 作用于当前任务，必须在当前 eda/teda 任务的事件处理函数上下文中调用。
 */
#define vsf_eda_return(...)                 __vsf_eda_return((uintptr_t)(0, ##__VA_ARGS__))

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
/**
 * \~english
 * @brief Get the current system tick converted to milliseconds
 * @return uint_fast32_t: current tick in milliseconds
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_TIMER is ENABLED.
 * @note Can be called in any context (task or interrupt).
 * \~chinese
 * @brief 获取当前系统 tick 转换成的毫秒数
 * @return uint_fast32_t: 当前 tick 对应的毫秒数
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_TIMER 使能时可用。
 * @note 可以在任意上下文（任务或中断）中调用。
 */
#   define vsf_systimer_get_ms()            vsf_systimer_tick_to_ms(vsf_systimer_get_tick())
/**
 * \~english
 * @brief Get the current system tick converted to microseconds
 * @return uint_fast32_t: current tick in microseconds
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_TIMER is ENABLED.
 * @note Can be called in any context (task or interrupt).
 * \~chinese
 * @brief 获取当前系统 tick 转换成的微秒数
 * @return uint_fast32_t: 当前 tick 对应的微秒数
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_TIMER 使能时可用。
 * @note 可以在任意上下文（任务或中断）中调用。
 */
#   define vsf_systimer_get_us()            vsf_systimer_tick_to_us(vsf_systimer_get_tick())
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
/**
 * \~english
 * @brief Sub-call an eda event handler from the current task
 * @param[in] evthandler: event handler @ref vsf_eda_evthandler_t of the sub-called task
 * @param[in] local_size: optional size of the local variable storage, 0 by default
 * @return vsf_err_t: VSF_ERR_NONE if the sub-call is started, VSF_ERR_NOT_ENOUGH_RESOURCES
 * if no frame is available
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is ENABLED. The
 * sub-called handler receives VSF_EVT_INIT; when it calls vsf_eda_return(), the
 * caller is woken with VSF_EVT_RETURN.
 * @note Operates on the current task; must be called in the current eda/teda task's event handler context.
 * \~chinese
 * @brief 从当前任务子调用一个 eda 事件处理函数
 * @param[in] evthandler: 被子调用任务的事件处理函数 @ref vsf_eda_evthandler_t
 * @param[in] local_size: 可选的局部变量存储大小，默认为 0
 * @return vsf_err_t: 如果子调用启动返回 VSF_ERR_NONE，如果没有可用的帧返回
 * VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL 使能时可用。被子调用的处理
 * 函数会收到 VSF_EVT_INIT 事件；当它调用 vsf_eda_return() 时，调用者会收到
 * VSF_EVT_RETURN 事件被唤醒。
 * @note 作用于当前任务，必须在当前 eda/teda 任务的事件处理函数上下文中调用。
 */
#   define vsf_eda_call_eda(__evthandler, ...)                                  \
            __vsf_eda_call_eda((uintptr_t)__evthandler, NULL, (0, ##__VA_ARGS))
/**
 * \~english
 * @brief Sub-call a parameterized eda event handler from the current task
 * @param[in] param_evthandler: parameterized event handler @ref vsf_param_eda_evthandler_t
 * @param[in] param: target parameter passed to the handler
 * @param[in] local_size: optional size of the local variable storage, 0 by default
 * @return vsf_err_t: VSF_ERR_NONE if the sub-call is started, VSF_ERR_NOT_ENOUGH_RESOURCES
 * if no frame is available
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is ENABLED.
 * @note Operates on the current task; must be called in the current eda/teda task's event handler context.
 * \~chinese
 * @brief 从当前任务子调用一个带参数的 eda 事件处理函数
 * @param[in] param_evthandler: 带参数的事件处理函数 @ref vsf_param_eda_evthandler_t
 * @param[in] param: 传递给处理函数的目标参数
 * @param[in] local_size: 可选的局部变量存储大小，默认为 0
 * @return vsf_err_t: 如果子调用启动返回 VSF_ERR_NONE，如果没有可用的帧返回
 * VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL 使能时可用。
 * @note 作用于当前任务，必须在当前 eda/teda 任务的事件处理函数上下文中调用。
 */
#   define vsf_eda_call_param_eda(__param_evthandler, __param, ...)             \
            __vsf_eda_call_eda( (uintptr_t)__param_evthandler,                  \
                                (uintptr_t)__param,                             \
                                (0, ##__VA_ARGS__))

/**
 * \~english
 * @brief Get the local variable storage of a frame-based (sub-called) eda
 * @param[in] eda: optional pointer to structure @ref vsf_eda_t, the current
 * task by default
 * @return uintptr_t: address of the local storage, NULL if the task has no local storage
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is ENABLED.
 * @note Operates on the current task; must be called in the current eda/teda task's event handler context.
 * \~chinese
 * @brief 获取基于帧的（被子调用的）eda 的局部变量存储
 * @param[in] eda: 可选的指向结构体 @ref vsf_eda_t 的指针，默认为当前任务
 * @return uintptr_t: 局部变量存储的地址，如果任务没有局部变量存储返回 NULL
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL 使能时可用。
 * @note 作用于当前任务，必须在当前 eda/teda 任务的事件处理函数上下文中调用。
 */
#   define vsf_eda_get_local(...)                                               \
            __vsf_eda_get_local((vsf_eda_t *)(vsf_eda_get_cur(), ##__VA_ARGS__))

#   define __vsf_peda_local(__name)     peda_local_##__name
/**
 * \~english
 * @brief Generate the local type name of a peda task
 * @param[in] name: name of the peda task
 * \~chinese
 * @brief 生成 peda 任务的局部变量类型名
 * @param[in] name: peda 任务名
 */
#   define vsf_peda_local(__name)       __vsf_peda_local(__name)

#   define __vsf_peda_arg(__name)       peda_arg_##__name
/**
 * \~english
 * @brief Generate the argument type name of a peda task
 * @param[in] name: name of the peda task
 * \~chinese
 * @brief 生成 peda 任务的参数类型名
 * @param[in] name: peda 任务名
 */
#   define vsf_peda_arg(__name)         __vsf_peda_arg(__name)

#   define __vsf_peda_func(__name)      vsf_peda_func_##__name
/**
 * \~english
 * @brief Generate the event handler function name of a peda task
 * @param[in] name: name of the peda task
 * \~chinese
 * @brief 生成 peda 任务的事件处理函数名
 * @param[in] name: peda 任务名
 */
#   define vsf_peda_func(__name)        __vsf_peda_func(__name)


#   define __vsf_peda_param(__name)     peda_cb_##__name
/**
 * \~english
 * @brief Generate the parameter type name of a peda task
 * @param[in] name: name of the peda task
 * \~chinese
 * @brief 生成 peda 任务的参数（控制块）类型名
 * @param[in] name: peda 任务名
 */
#   define vsf_peda_param(__name)       __vsf_peda_param(__name)


#   define __declare_vsf_peda_ctx(__name)                                       \
            typedef struct vsf_peda_param(__name)   vsf_peda_param(__name);     \
            typedef struct vsf_peda_arg(__name)     vsf_peda_arg(__name);       \
            typedef struct vsf_peda_local(__name)   vsf_peda_local(__name);
/**
 * \~english
 * @brief Declare the context types (param, arg, local) of a peda task
 * @param[in] name: name of the peda task
 * \~chinese
 * @brief 声明 peda 任务的上下文类型（param、arg、local）
 * @param[in] name: peda 任务名
 */
#   define declare_vsf_peda_ctx(__name)     __declare_vsf_peda_ctx(__name)

/**
 * \~english
 * @brief Alias of declare_vsf_peda_ctx()
 * @param[in] name: name of the peda task
 * \~chinese
 * @brief declare_vsf_peda_ctx() 的别名
 * @param[in] name: peda 任务名
 */
#   define dcl_vsf_peda_ctx(__name)                                             \
            declare_vsf_peda_ctx(__name)

#   define __declare_vsf_peda(__name)                                           \
            typedef struct __name __name;                                       \
            __declare_vsf_peda_ctx(__name)
/**
 * \~english
 * @brief Declare a peda task type and its context types
 * @param[in] name: name of the peda task
 * \~chinese
 * @brief 声明 peda 任务类型及其上下文类型
 * @param[in] name: peda 任务名
 */
#   define declare_vsf_peda(__name)     __declare_vsf_peda(__name)

/**
 * \~english
 * @brief Alias of declare_vsf_peda()
 * @param[in] name: name of the peda task
 * \~chinese
 * @brief declare_vsf_peda() 的别名
 * @param[in] name: peda 任务名
 */
#   define dcl_vsf_peda(__name)                                                 \
            declare_vsf_peda(__name)

#   define declare_vsf_peda_methods1(__decoration, __name)                      \
            declare_vsf_peda_ctx(__name)                                        \
            __decoration                                                        \
                    void vsf_peda_func(__name)(                                 \
                                        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);

#   define declare_vsf_peda_methods2(__decoration, __name,  __func1)            \
            declare_vsf_peda_ctx(__name)                                        \
            __decoration                                                        \
                   void vsf_peda_func(__name)(                                  \
                                        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func1(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);

#   define declare_vsf_peda_methods3(__decoration, __name, __func1, __func2)    \
            declare_vsf_peda_ctx(__name)                                        \
            __decoration                                                        \
                   void vsf_peda_func(__name)(                                  \
                                        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func1(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func2(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);

#   define declare_vsf_peda_methods4(__name, __func1, __func2, __func3)         \
            declare_vsf_peda_ctx(__name)                                        \
            __decoration                                                        \
                   void vsf_peda_func(__name)(                                  \
                                        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func1(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func2(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func3(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);

#   define declare_vsf_peda_methods5(   __decoration, __name, __func1, __func2, \
                                        __func3, __func4)                       \
            declare_vsf_peda_ctx(__name)                                        \
            __decoration                                                        \
                   void vsf_peda_func(__name)(                                  \
                                        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func1(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func2(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func3(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func4(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);


#   define declare_vsf_peda_methods6(   __decoration, __name, __func1, __func2, \
                                        __func3, __func4, __func5)              \
            declare_vsf_peda_ctx(__name)                                        \
            __decoration                                                        \
                   void vsf_peda_func(__name)(                                  \
                                        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func1(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func2(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func3(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func4(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func5(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);

#   define declare_vsf_peda_methods7(   __decoration, __name, __func1, __func2, \
                                        __func3, __func4, __func5, __func6)     \
            declare_vsf_peda_ctx(__name)                                        \
            __decoration                                                        \
                   void vsf_peda_func(__name)(                                  \
                                        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func1(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func2(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func3(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func4(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func5(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func6(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);

#   define declare_vsf_peda_methods8(   __decoration, __name, __func1, __func2, \
                                        __func3, __func4, __func5, __func6,     \
                                        __func7)                                \
            declare_vsf_peda_ctx(__name)                                        \
            __decoration                                                        \
                   void vsf_peda_func(__name)(                                  \
                                        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func1(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func2(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func3(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func4(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func5(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func6(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);                         \
            __decoration                                                        \
                   void __func7(        struct vsf_peda_local(__name) *vsf_pthis,\
                                        vsf_evt_t evt);

/**
 * \~english
 * @brief Declare the context types and the event handler prototypes of a peda task
 * @param[in] decoration: declaration decoration (e.g. extern, static)
 * @param[in] name: name of the peda task, followed by up to 7 additional handler names
 *
 * @note Dispatches to the numbered declare_vsf_peda_methods1..8 variants
 * according to the argument count; the numbered variants are internal helpers.
 * \~chinese
 * @brief 声明 peda 任务的上下文类型和事件处理函数原型
 * @param[in] decoration: 声明修饰符（例如 extern、static）
 * @param[in] name: peda 任务名，后面可跟最多 7 个额外的处理函数名
 *
 * @note 根据参数个数分发到带编号的 declare_vsf_peda_methods1..8 变体；
 * 带编号的变体是内部辅助宏。
 */
#define declare_vsf_peda_methods(__decoration, ...)                             \
            __PLOOC_EVAL(declare_vsf_peda_methods, __VA_ARGS__)                 \
                (__decoration, __VA_ARGS__)

/**
 * \~english
 * @brief Alias of declare_vsf_peda_methods()
 * @param[in] decoration: declaration decoration (e.g. extern, static)
 * @param[in] name: name of the peda task, followed by up to 7 additional handler names
 * \~chinese
 * @brief declare_vsf_peda_methods() 的别名
 * @param[in] decoration: 声明修饰符（例如 extern、static）
 * @param[in] name: peda 任务名，后面可跟最多 7 个额外的处理函数名
 */
#define dcl_vsf_peda_methods(__decoration, ...)                                 \
            declare_vsf_peda_methods(__decoration, __VA_ARGS__)

#   if __IS_COMPILER_IAR__
#       define __def_vsf_peda_ctx4(__name, __param, __arg, __local)             \
        struct vsf_peda_param(__name) {                                         \
            __param                                                             \
            uint8_t VSF_MCONNECT4(_,__LINE__,__COUNTER__,_canary);              \
        };                                                                      \
        struct vsf_peda_arg(__name) {                                           \
            __arg                                                               \
            uint8_t VSF_MCONNECT4(_,__LINE__,__COUNTER__,_canary);              \
        };                                                                      \
        struct vsf_peda_local(__name) {                                         \
            implement(vsf_peda_arg(__name))                                     \
            __local                                                             \
        };
#   else
#       define __def_vsf_peda_ctx4(__name, __param, __arg, __local)             \
        struct vsf_peda_param(__name) {                                         \
            __param                                                             \
        };                                                                      \
        struct vsf_peda_arg(__name) {                                           \
            __arg                                                               \
        };                                                                      \
        struct vsf_peda_local(__name) {                                         \
            implement(vsf_peda_arg(__name))                                     \
            __local                                                             \
        };
#endif

#   define __def_vsf_peda4(__name, __param, __arg, __local)                     \
        __def_vsf_peda_ctx4(__name, __param, __arg, __local)                    \
        struct __name {                                                         \
            implement(vsf_peda_t)                                               \
            implement_ex(vsf_peda_param(__name), param)                         \
        };

#   if __IS_COMPILER_IAR__
#       define __def_vsf_peda_ctx3(__name, __param, __arg)                      \
        struct vsf_peda_param(__name) {                                         \
            __param                                                             \
            uint8_t VSF_MCONNECT4(_,__LINE__,__COUNTER__,_canary);              \
        };                                                                      \
        struct vsf_peda_arg(__name) {                                           \
            __arg                                                               \
            uint8_t VSF_MCONNECT4(_,__LINE__,__COUNTER__,_canary);              \
        };                                                                      \
        struct vsf_peda_local(__name) {                                         \
            implement(vsf_peda_arg(__name))                                     \
        };
#   else
#       define __def_vsf_peda_ctx3(__name, __param, __arg)                      \
        struct vsf_peda_param(__name) {                                         \
            __param                                                             \
        };                                                                      \
        struct vsf_peda_arg(__name) {                                           \
            __arg                                                               \
        };                                                                      \
        struct vsf_peda_local(__name) {                                         \
            implement(vsf_peda_arg(__name))                                     \
        };
#endif

#   define __def_vsf_peda3(__name, __param, __arg)                              \
        __def_vsf_peda_ctx3(__name, __param, __arg)                             \
        struct __name {                                                         \
            implement(vsf_peda_t)                                               \
            implement_ex(vsf_peda_param(__name), param)                         \
        };


#   if __IS_COMPILER_IAR__
#       define __def_vsf_peda_ctx2(__name, __param)                             \
        struct vsf_peda_param(__name) {                                         \
            __param                                                             \
            uint8_t VSF_MCONNECT4(_,__LINE__,__COUNTER__,_canary);              \
        };                                                                      \
        struct vsf_peda_arg(__name) {                                           \
            uint8_t VSF_MCONNECT4(_,__LINE__,__COUNTER__,_canary);              \
        };                                                                      \
        struct vsf_peda_local(__name) {                                         \
            implement(vsf_peda_arg(__name))                                     \
        };
#   else
#       define __def_vsf_peda_ctx2(__name, __param)                             \
        struct vsf_peda_param(__name) {                                         \
            __param                                                             \
        };                                                                      \
        struct vsf_peda_arg(__name) {                                           \
        };                                                                      \
        struct vsf_peda_local(__name) {                                         \
            implement(vsf_peda_arg(__name))                                     \
        };
#endif

#   define __def_vsf_peda_ctx1(__name)                                          \
        __def_vsf_peda_ctx2(__name, )

#   define __def_vsf_peda2(__name, __param)                                     \
        __def_vsf_peda_ctx2(__name, __param)                                    \
        struct __name {                                                         \
            implement(vsf_peda_t)                                               \
            implement_ex(vsf_peda_param(__name), param)                         \
        };

#   define __def_vsf_peda1(__name)                                              \
        __def_vsf_peda_ctx1(__name)                                             \
        struct __name {                                                         \
            implement(vsf_peda_t)                                               \
            implement_ex(vsf_peda_param(__name), param)                         \
        };

/**
 * \~english
 * @brief Define a peda task structure and its context structures
 * @param[in] name: name of the peda task, optionally followed by param, arg and
 * local member definitions (use def_args(), def_locals() etc. as the sections)
 *
 * @note Dispatches to the internal numbered __def_vsf_peda1..4 variants
 * according to the argument count. Pair it with end_def_vsf_peda().
 * \~chinese
 * @brief 定义 peda 任务结构体及其上下文结构体
 * @param[in] name: peda 任务名，后面可选地跟 param、arg 和 local 成员定义
 * （使用 def_args()、def_locals() 等作为分段）
 *
 * @note 根据参数个数分发到内部带编号的 __def_vsf_peda1..4 变体。
 * 与 end_def_vsf_peda() 配对使用。
 */
#   define def_vsf_peda(...)                                                    \
                __PLOOC_EVAL(__def_vsf_peda, __VA_ARGS__) (__VA_ARGS__)

/**
 * \~english
 * @brief Terminator of def_vsf_peda(), expands to nothing
 * \~chinese
 * @brief def_vsf_peda() 的结束符，展开为空
 */
#   define end_def_vsf_peda(...)

/**
 * \~english
 * @brief Define only the context structures (param, arg, local) of a peda task
 * @param[in] name: name of the peda task, optionally followed by param, arg and
 * local member definitions
 *
 * @note Dispatches to the internal numbered __def_vsf_peda_ctx1..4 variants
 * according to the argument count. Pair it with end_def_vsf_peda_ctx().
 * \~chinese
 * @brief 仅定义 peda 任务的上下文结构体（param、arg、local）
 * @param[in] name: peda 任务名，后面可选地跟 param、arg 和 local 成员定义
 *
 * @note 根据参数个数分发到内部带编号的 __def_vsf_peda_ctx1..4 变体。
 * 与 end_def_vsf_peda_ctx() 配对使用。
 */
#   define def_vsf_peda_ctx(...)                                                \
                __PLOOC_EVAL(__def_vsf_peda_ctx, __VA_ARGS__)(__VA_ARGS__)

/**
 * \~english
 * @brief Terminator of def_vsf_peda_ctx(), expands to nothing
 * \~chinese
 * @brief def_vsf_peda_ctx() 的结束符，展开为空
 */
#   define end_def_vsf_peda_ctx(...)

/**
 * \~english
 * @brief Alias of def_vsf_peda_ctx()
 * @param[in] name: name of the peda task, optionally followed by param, arg and
 * local member definitions
 * \~chinese
 * @brief def_vsf_peda_ctx() 的别名
 * @param[in] name: peda 任务名，后面可选地跟 param、arg 和 local 成员定义
 */
#   define define_vsf_peda_ctx(__name, ...)                                     \
                def_vsf_peda_ctx(__name, __VA_ARGS__)

/**
 * \~english
 * @brief Terminator of define_vsf_peda_ctx(), expands to nothing
 * \~chinese
 * @brief define_vsf_peda_ctx() 的结束符，展开为空
 */
#   define end_define_vsf_peda_ctx(...)

/**
 * \~english
 * @brief Local member section used inside def_vsf_peda()/def_vsf_peda_ctx()
 * \~chinese
 * @brief 在 def_vsf_peda()/def_vsf_peda_ctx() 内使用的局部成员分段
 */
#   define def_locals(...)              ,##__VA_ARGS__
/**
 * \~english
 * @brief Terminator of def_locals(), expands to nothing
 * \~chinese
 * @brief def_locals() 的结束符，展开为空
 */
#   define end_def_locals(...)

/**
 * \~english
 * @brief Alias of def_locals()
 * \~chinese
 * @brief def_locals() 的别名
 */
#   define define_locals(...)           ,##__VA_ARGS__
/**
 * \~english
 * @brief Terminator of define_locals(), expands to nothing
 * \~chinese
 * @brief define_locals() 的结束符，展开为空
 */
#   define end_define_locals(...)

/**
 * \~english
 * @brief Argument member section used inside def_vsf_peda()/def_vsf_peda_ctx()
 * \~chinese
 * @brief 在 def_vsf_peda()/def_vsf_peda_ctx() 内使用的参数成员分段
 */
#   define def_args(...)                ,__VA_ARGS__
/**
 * \~english
 * @brief Terminator of def_args(), expands to nothing
 * \~chinese
 * @brief def_args() 的结束符，展开为空
 */
#   define end_def_args(...)

/**
 * \~english
 * @brief Alias of def_args()
 * \~chinese
 * @brief def_args() 的别名
 */
#   define define_args(...)             ,__VA_ARGS__
/**
 * \~english
 * @brief Terminator of define_args(), expands to nothing
 * \~chinese
 * @brief define_args() 的结束符，展开为空
 */
#   define end_define_args(...)

/**
 * \~english
 * @brief Alias of def_args()
 * \~chinese
 * @brief def_args() 的别名
 */
#   define define_arguments(...)        ,__VA_ARGS__
/**
 * \~english
 * @brief Terminator of define_arguments(), expands to nothing
 * \~chinese
 * @brief define_arguments() 的结束符，展开为空
 */
#   define end_define_arguments(...)

/**
 * \~english
 * @brief Parameter member section used inside def_vsf_peda()/def_vsf_peda_ctx()
 * \~chinese
 * @brief 在 def_vsf_peda()/def_vsf_peda_ctx() 内使用的参数（控制块）成员分段
 */
#   define define_parameters(...)       __VA_ARGS__
/**
 * \~english
 * @brief Terminator of define_parameters(), expands to nothing
 * \~chinese
 * @brief define_parameters() 的结束符，展开为空
 */
#   define end_define_parameters(...)

/**
 * \~english
 * @brief Start a peda task, alias of vsf_teda_start() when
 * VSF_KERNEL_CFG_EDA_SUPPORT_TIMER is ENABLED, otherwise alias of vsf_eda_start()
 * @param[in] peda: a pointer to the peda task structure
 * @param[in] cfg: a pointer to structure @ref vsf_eda_cfg_t
 * @return vsf_err_t: VSF_ERR_NONE if started successfully; VSF_ERR_NOT_ENOUGH_RESOURCES if no frame is available
 * \~chinese
 * @brief 启动一个 peda 任务，当 VSF_KERNEL_CFG_EDA_SUPPORT_TIMER 使能时是
 * vsf_teda_start() 的别名，否则是 vsf_eda_start() 的别名
 * @param[in] peda: 指向 peda 任务结构体的指针
 * @param[in] cfg: 指向结构体 @ref vsf_eda_cfg_t 的指针
 * @return vsf_err_t: 启动成功返回 VSF_ERR_NONE；没有可用的帧返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 */
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
#   define vsf_peda_start               vsf_teda_start
#else
#   define vsf_peda_start               vsf_eda_start
#endif

#   define __init_vsf_peda(__name, __param_eda, __pri, ...)                     \
        do {                                                                    \
            vsf_eda_cfg_t VSF_MACRO_SAFE_NAME(cfg) = {                          \
                .fn.param_evthandler =                                          \
                    (vsf_param_eda_evthandler_t)vsf_peda_func(__name),          \
                .priority = (__pri),                                            \
                .target = (uintptr_t)&((__param_eda)->param),                   \
                .local_size = sizeof(vsf_peda_local(__name)),                   \
                __VA_ARGS__                                                     \
            };                                                                  \
            vsf_peda_start((vsf_peda_t *)(__param_eda),                         \
                &VSF_MACRO_SAFE_NAME(cfg));                                     \
        } while(0)

/**
 * \~english
 * @brief Initialize and start a peda (parameterized eda) task
 * @param[in] name: name of the peda task
 * @param[in] param_eda: a pointer to the peda task instance
 * @param[in] pri: priority of the task
 * @param[in] ...: optional extra initializers of @ref vsf_eda_cfg_t
 * @return none
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is ENABLED.
 * @note Operates on the current task; must be called in the current eda/teda task's event handler context.
 * \~chinese
 * @brief 初始化并启动一个 peda（带参数的 eda）任务
 * @param[in] name: peda 任务名
 * @param[in] param_eda: 指向 peda 任务实例的指针
 * @param[in] pri: 任务优先级
 * @param[in] ...: 可选的 @ref vsf_eda_cfg_t 额外初始化成员
 * @return 无
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL 使能时可用。
 * @note 作用于当前任务，必须在当前 eda/teda 任务的事件处理函数上下文中调用。
 */
#   define init_vsf_peda(__name, __param_eda, __pri, ...)                       \
            __init_vsf_peda(__name, (__param_eda), (__pri), __VA_ARGS__)


#   define __implement_vsf_peda2(__name, __func_name)                           \
        void __func_name(   struct vsf_peda_local(__name) *vsf_plocal,          \
                            vsf_evt_t evt)                                      \
        {                                                                       \
            vsf_peda_param(__name) *vsf_pthis =                                 \
                *(vsf_peda_param(__name) **)                                    \
                    ((uintptr_t)vsf_plocal - sizeof(uintptr_t));                \
            VSF_UNUSED_PARAM(vsf_pthis);                                        \
            VSF_KERNEL_ASSERT(NULL != vsf_pthis || NULL != vsf_plocal);

#   define __implement_vsf_peda1(__name)                                        \
        void vsf_peda_func(__name)( struct vsf_peda_local(__name) *vsf_plocal,  \
                                    vsf_evt_t evt)                              \
        {                                                                       \
            vsf_peda_param(__name) *vsf_pthis =                                 \
                *(vsf_peda_param(__name) **)                                    \
                    ((uintptr_t)vsf_plocal - sizeof(uintptr_t));                \
            VSF_UNUSED_PARAM(vsf_pthis);                                        \
            VSF_KERNEL_ASSERT(NULL != vsf_pthis || NULL != vsf_plocal);

/**
 * \~english
 * @brief Beginning helper of a peda implementation, expands to nothing; pair
 * with vsf_peda_end() to keep brackets balanced for editors
 * \~chinese
 * @brief peda 实现的开始辅助宏，展开为空；与 vsf_peda_end() 配对使用，
 * 为编辑器保持括号配对
 */
#   define vsf_peda_begin()

/**
 * \~english
 * @brief Ending helper of a peda implementation, expands to the closing brace
 * of the event handler; pair with implement_vsf_peda()/vsf_peda_begin()
 * \~chinese
 * @brief peda 实现的结束辅助宏，展开为事件处理函数的右花括号；
 * 与 implement_vsf_peda()/vsf_peda_begin() 配对使用
 */
#   define vsf_peda_end()                                                       \
            }

/**
 * \~english
 * @brief Start implementing the event handler of a peda task
 * @param[in] name: name of the peda task, optionally followed by a custom
 * function name
 *
 * @note Inside the implementation, use vsf_local to access the local variables
 * and vsf_this to access the parameters. Pair it with vsf_peda_end() or a
 * closing brace.
 * \~chinese
 * @brief 开始实现一个 peda 任务的事件处理函数
 * @param[in] name: peda 任务名，后面可选地跟一个自定义函数名
 *
 * @note 在实现内部，使用 vsf_local 访问局部变量，使用 vsf_this 访问参数。
 * 与 vsf_peda_end() 或右花括号配对使用。
 */
#   define implement_vsf_peda(...)                                              \
                __PLOOC_EVAL(__implement_vsf_peda, __VA_ARGS__)(__VA_ARGS__)

/**
 * \~english
 * @brief Alias of implement_vsf_peda()
 * @param[in] name: name of the peda task, optionally followed by a custom
 * function name
 * \~chinese
 * @brief implement_vsf_peda() 的别名
 * @param[in] name: peda 任务名，后面可选地跟一个自定义函数名
 */
#   define imp_vsf_peda(...)                                                    \
                implement_vsf_peda(__VA_ARGS__)

/**
 * \~english
 * @brief Sub-call a peda task with its parameter from the current task
 * @param[in] name: name of the peda task
 * @param[in] param: a pointer to the parameter (control block) of the peda task
 * @return vsf_err_t: VSF_ERR_NONE if the sub-call is started, VSF_ERR_NOT_ENOUGH_RESOURCES
 * if no frame is available
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is ENABLED. The
 * caller is woken with VSF_EVT_RETURN after the peda task calls vsf_eda_return().
 * @note Operates on the current task; must be called in the current eda/teda task's event handler context.
 * \~chinese
 * @brief 从当前任务子调用一个带参数的 peda 任务
 * @param[in] name: peda 任务名
 * @param[in] param: 指向 peda 任务参数（控制块）的指针
 * @return vsf_err_t: 如果子调用启动返回 VSF_ERR_NONE，如果没有可用的帧返回
 * VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL 使能时可用。peda 任务调用
 * vsf_eda_return() 后，调用者会收到 VSF_EVT_RETURN 事件被唤醒。
 * @note 作用于当前任务，必须在当前 eda/teda 任务的事件处理函数上下文中调用。
 */
#   define vsf_eda_call_peda(__name, __param)                                   \
                vsf_eda_call_param_eda( vsf_peda_func(__name),                  \
                                        (__param),                              \
                                        sizeof(vsf_peda_local(__name)))

/**
 * \~english
 * @brief Access the local variables inside a peda implementation
 * \~chinese
 * @brief 在 peda 实现内部访问局部变量
 */
#   define vsf_local                    (*vsf_plocal)

#endif
/**
 * \~english
 * @brief Access the parameters (control block) inside a peda implementation
 * \~chinese
 * @brief 在 peda 实现内部访问参数（控制块）
 */
#   define vsf_this                     (*vsf_pthis)

// backward compatibility, do not use in new design
/**
 * \~english
 * @brief Alias of vsf_eda_mutex_enter(), kept for backward compatibility; do
 * not use in new designs (use vsf_eda_mutex_enter() with timeout 0 instead)
 * \~chinese
 * @brief vsf_eda_mutex_enter() 的别名，仅为兼容旧设计保留；新设计请勿使用
 * （请使用超时为 0 的 vsf_eda_mutex_enter()）
 */
#define vsf_eda_mutex_try_enter         vsf_eda_mutex_enter
/**
 * \~english
 * @brief Alias of vsf_eda_crit_enter(), kept for backward compatibility; do
 * not use in new designs (use vsf_eda_crit_enter() with timeout 0 instead)
 * \~chinese
 * @brief vsf_eda_crit_enter() 的别名，仅为兼容旧设计保留；新设计请勿使用
 * （请使用超时为 0 的 vsf_eda_crit_enter()）
 */
#define vsf_eda_crit_try_enter          vsf_eda_crit_enter

/*============================ TYPES =========================================*/

/**
 * \~english
 * @brief Underlying integer type of vsf_timeout_tick_t; redefine it before
 * including this header to change the timeout tick width
 * \~chinese
 * @brief vsf_timeout_tick_t 的底层整数类型；在包含本头文件前重新定义它可改变超时 tick 的位宽
 */
#ifndef VSF_KERNEL_TIMEOUT_TICK_T
#   define VSF_KERNEL_TIMEOUT_TICK_T    int_fast64_t
#endif
/**
 * \~english
 * @brief Timeout type in system timer ticks: negative means wait forever,
 * 0 means non-blocking try, positive means the maximum ticks to wait
 * \~chinese
 * @brief 以系统定时器 tick 为单位的超时类型：负数表示永久等待，0 表示非阻塞尝试，正数表示最多等待的 tick 数
 */
typedef VSF_KERNEL_TIMEOUT_TICK_T       vsf_timeout_tick_t;

/**
 * \~english
 * @brief VSF kernel event types, carried by vsf_evt_t; every eda event
 * handler is driven by these events
 * \~chinese
 * @brief VSF 内核事件类型，由 vsf_evt_t 承载；每个 eda 事件处理器都由这些事件驱动
 */
enum {
    /*!\ note wait for invalid also means wait for any evt */
    //! \~english
    //! @brief Invalid event; waiting for it means waiting for any event;
    //! also compatible with fsm_rt_err
    //! \~chinese
    //! @brief 无效事件；等待它表示等待任意事件；同时兼容 fsm_rt_err
    VSF_EVT_INVALID             = -1,       //!< compatible with fsm_rt_err
    //! \~english
    //! @brief No event; compatible with fsm_rt_cpl
    //! \~chinese
    //! @brief 无事件；兼容 fsm_rt_cpl
    VSF_EVT_NONE                = 0,        //!< compatible with fsm_rt_cpl
    //! \~english
    //! @brief Cooperative yield event; compatible with fsm_rt_on_going
    //! \~chinese
    //! @brief 协作式让出事件；兼容 fsm_rt_on_going
    VSF_EVT_YIELD               = 1,        //!< compatible with fsm_rt_on_going
#if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED && VSF_KERNEL_CFG_THREAD_SIGNAL == ENABLED
    //! \~english
    //! @brief POSIX-like thread signal event (VSF_KERNEL_CFG_THREAD_SIGNAL)
    //! \~chinese
    //! @brief POSIX 风格的线程信号事件（VSF_KERNEL_CFG_THREAD_SIGNAL）
    VSF_EVT_SIGNAL              = 2,
#endif

    //! \~english
    //! @brief Base of system events; system events start here, user events
    //! start at VSF_EVT_USER
    //! \~chinese
    //! @brief 系统事件的基值；系统事件从此开始，用户事件从 VSF_EVT_USER 开始
    VSF_EVT_SYSTEM              = 0x100,
    //! \~english
    //! @brief Dummy system event
    //! \~chinese
    //! @brief 哑（占位）系统事件
    VSF_EVT_DUMMY               = VSF_EVT_SYSTEM + 0,
    //! \~english
    //! @brief Received once when a task starts
    //! \~chinese
    //! @brief 任务启动时收到一次
    VSF_EVT_INIT                = VSF_EVT_SYSTEM + 1,
    //! \~english
    //! @brief Received once when a task terminates
    //! \~chinese
    //! @brief 任务终止时收到一次
    VSF_EVT_FINI                = VSF_EVT_SYSTEM + 2,
    //! \~english
    //! @brief Received when entering a sub-call frame
    //! \~chinese
    //! @brief 进入子调用帧时收到
    VSF_EVT_ENTER               = VSF_EVT_SYSTEM + 3,
    //! \~english
    //! @brief Received when a called sub task returns
    //! \~chinese
    //! @brief 被调用的子任务返回时收到
    VSF_EVT_EXIT                = VSF_EVT_SYSTEM + 4,
    //! \~english
    //! @brief Same value as VSF_EVT_EXIT; received when a called sub task
    //! returns
    //! \~chinese
    //! @brief 与 VSF_EVT_EXIT 相同的值；被调用的子任务返回时收到
    VSF_EVT_RETURN              = VSF_EVT_EXIT,

    // events for time
    //! \~english
    //! @brief teda timer expired (only for teda tasks)
    //! \~chinese
    //! @brief teda 定时器到期（仅用于 teda 任务）
    VSF_EVT_TIMER               = VSF_EVT_SYSTEM + 5,

    // events for sync
    //! \~english
    //! @brief A pending sync IPC (semaphore/mutex/trigger/queue/bmpevt) was
    //! obtained
    //! \~chinese
    //! @brief 挂起的同步 IPC（信号量/互斥锁/触发器/队列/位图事件）已获取
    VSF_EVT_SYNC                = VSF_EVT_SYSTEM + 6,
    //! \~english
    //! @brief The pending sync IPC was cancelled (e.g. by
    //! vsf_eda_sync_cancel())
    //! \~chinese
    //! @brief 挂起的同步 IPC 被取消（例如被 vsf_eda_sync_cancel() 取消）
    VSF_EVT_SYNC_CANCEL         = VSF_EVT_SYSTEM + 7,
    //! \~english
    //! @brief Sync poll event (used by bmpevt polling)
    //! \~chinese
    //! @brief 同步轮询事件（用于 bmpevt 轮询）
    VSF_EVT_SYNC_POLL           = VSF_EVT_SYSTEM + 8,

    // events for message
    //! \~english
    //! @brief A message event carrying a pointer, retrieved with
    //! vsf_eda_get_cur_msg()
    //! \~chinese
    //! @brief 携带指针的消息事件，使用 vsf_eda_get_cur_msg() 获取
    VSF_EVT_MESSAGE             = VSF_EVT_SYSTEM + 9,
    //! \~english
    //! @brief Base of user-defined events; values from here on are user
    //! events
    //! \~chinese
    //! @brief 用户自定义事件的基值；从此开始的值都是用户事件
    VSF_EVT_USER                = VSF_EVT_SYSTEM + 10,
};

// events for kernel task
enum {
    VSF_KERNEL_EVT_CALLBACK_TIMER       = VSF_EVT_USER + 0,
    VSF_KERNEL_EVT_CALLBACK_TIMER_ADD   = VSF_EVT_USER + 1,
    VSF_KERNEL_EVT_QUEUE_SEND_NOTIFY    = VSF_EVT_USER + 2,
    VSF_KERNEL_EVT_QUEUE_RECV_NOTIFY    = VSF_EVT_USER + 3,
};

#if VSF_KERNEL_CFG_CPU_USAGE == ENABLED || VSF_KERNEL_CFG_EDA_CPU_USAGE == ENABLED
/**
 * \~english
 * @brief Context for per-task CPU usage sampling; the user provides the ctx
 * storage and must keep it valid until vsf_eda_cpu_usage_stop() is called
 * @note The measured ticks include higher-priority tasks and interrupts.
 * \~chinese
 * @brief 单任务 CPU 使用率采样上下文；用户提供 ctx 存储，并且必须在调用
 * vsf_eda_cpu_usage_stop() 之前保持其有效
 * @note 测量的 tick 包含更高优先级任务和中断的执行时间。
 */
typedef struct vsf_cpu_usage_ctx_t {
    vsf_systimer_tick_t         ticks;
    vsf_systimer_tick_t         duration;
} vsf_cpu_usage_ctx_t;
/**
 * \~english
 * @brief Per-task CPU usage sampling data; points to the user-provided
 * sampling context @ref vsf_cpu_usage_ctx_t
 * \~chinese
 * @brief 单任务 CPU 使用率采样数据；指向用户提供的采样上下文 @ref vsf_cpu_usage_ctx_t
 */
typedef struct vsf_cpu_usage_t {
    vsf_systimer_tick_t         ticks;
    vsf_cpu_usage_ctx_t         *ctx;
} vsf_cpu_usage_t;
#endif

vsf_dcl_class(vsf_eda_t)
vsf_dcl_class(vsf_teda_t)
vsf_dcl_class(vsf_sync_t)
vsf_dcl_class(vsf_sync_owner_t)
vsf_dcl_class(vsf_bmpevt_t)
vsf_dcl_class(vsf_bmpevt_pender_t)
vsf_dcl_class(vsf_bmpevt_adapter_t)
vsf_dcl_class(vsf_bmpevt_adapter_eda_t)
vsf_dcl_class(vsf_eda_queue_t)
vsf_dcl_class(vsf_callback_timer_t)

/**
 * \~english
 * @brief Kernel event type
 * \~chinese
 * @brief 内核事件类型
 */
typedef int16_t vsf_evt_t;

/**
 * \~english
 * @brief eda event handler; invoked with the task (eda) and the event (evt)
 * \~chinese
 * @brief eda 事件处理器；以任务（eda）和事件（evt）为参数被调用
 */
typedef void (*vsf_eda_evthandler_t)(vsf_eda_t *eda, vsf_evt_t evt);
/**
 * \~english
 * @brief On-terminate callback; invoked with the terminating task (eda)
 * \~chinese
 * @brief 终止回调；以正在终止的任务（eda）为参数被调用
 */
typedef void (*vsf_eda_on_terminate_t)(vsf_eda_t *eda);
/**
 * \~english
 * @brief Parameterized event handler; invoked with the user parameter
 * (target) and the event (evt)
 * \~chinese
 * @brief 带参数的事件处理器；以用户参数（target）和事件（evt）为参数被调用
 */
typedef void (*vsf_param_eda_evthandler_t)(uintptr_t target, vsf_evt_t evt);

#if VSF_KERNEL_CFG_EDA_USER_BITLEN <= 8 - 3
typedef uint8_t __vsf_eda_feature_word;
typedef uint16_t __vsf_eda_flag_word;
#elif VSF_KERNEL_CFG_EDA_USER_BITLEN <= 16 - 3
typedef uint16_t __vsf_eda_feature_word;
typedef uint32_t __vsf_eda_flag_word;
#elif VSF_KERNEL_CFG_EDA_USER_BITLEN <= 32 - 3
typedef uint32_t __vsf_eda_feature_word;
typedef uint64_t __vsf_eda_flag_word;
#else
#    error VSF_KERNEL_CFG_EDA_USER_BITLEN not supported yet
#endif

/**
 * \~english
 * @brief Task feature flags passed in vsf_eda_cfg_t
 * \~chinese
 * @brief 在 vsf_eda_cfg_t 中传递的任务特性标志
 */
typedef union vsf_eda_feature_t {
    struct {
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    /*! \note  is_use_frame is used by vsf_peda and sub-call
     *!        since both vsf_peda and sub-call will share most of the functions
     *!        used for frame push and pop operation, it has little gain to
     *!        only disable sub-call feature but keep vsf_peda. Hence, to reduce
     *!        complexity, we only use VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL to
     *!        enable or disable the is_use_frame bit and frame related
     *!        functions.
     */
        //! \~english task uses a stack frame (for peda/sub-call tasks)
        //! \~chinese 任务使用栈帧（用于 peda/子调用任务）
        __vsf_eda_feature_word  is_use_frame : 1;
#endif
#if VSF_KERNEL_USE_SIMPLE_SHELL == ENABLED
        //! \~english task owns a dedicated stack, i.e. a thread
        //! \~chinese 任务拥有专用栈，即线程
        __vsf_eda_feature_word  is_stack_owner : 1;
#endif
#if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
        //! \~english the sub-call returns a value (fsm_rt_t)
        //! \~chinese 子调用返回一个值（fsm_rt_t）
        __vsf_eda_feature_word  is_subcall_has_return_value : 1;
#endif
        //! \~english application-defined bits (e.g. the simple-shell polling
        //! state)
        //! \~chinese 应用自定义位（例如 simple-shell 轮询状态）
        __vsf_eda_feature_word  user_bits : VSF_KERNEL_CFG_EDA_USER_BITLEN;
    };
    //! \~english raw value, 0 = default
    //! \~chinese 原始值，0 表示默认
    __vsf_eda_feature_word      value;
} vsf_eda_feature_t;

typedef union __vsf_eda_state_t {
    struct {
#if VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED == ENABLED
#   if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED
        uint8_t                 is_new_prio : 1;
#   endif
        uint8_t                 is_to_exit : 1;
#else
        uint8_t                 is_processing : 1;
#endif

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
        /* if is_limitted, eda can only receive 1 event */
        uint8_t                 is_limitted : 1;
        uint8_t                 is_sync_got : 1;
#   if (VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED) && defined(__VSF_OS_CFG_EVTQ_LIST)
        uint8_t                 is_to_set_due : 1;
#   endif
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
        uint8_t                 is_timed : 1;
#endif
    };
    uint8_t                     value;
} __vsf_eda_state_t;

typedef union __vsf_eda_flag_t {
    struct {
        __vsf_eda_state_t       state;
        vsf_eda_feature_t       feature;
    };
    __vsf_eda_flag_word         value;
} __vsf_eda_flag_t;

typedef union __vsf_eda_fn_t {
    uintptr_t                   func;
    vsf_eda_evthandler_t        evthandler;
    vsf_param_eda_evthandler_t  param_evthandler;
} __vsf_eda_fn_t;

typedef struct __vsf_eda_frame_state_t {
    vsf_eda_feature_t           feature;
    uint16_t                    local_size;
} __vsf_eda_frame_state_t;

vsf_dcl_class(__vsf_eda_frame_t)
vsf_class(__vsf_eda_frame_t) {
    protected_member (
        implement(vsf_slist_node_t)
        __vsf_eda_fn_t          fn;
        __vsf_eda_frame_state_t state;

        union {
            uintptr_t           param;
            uintptr_t           target;
        } ptr;
    )
};

/**
 * \~english
 * @brief Configuration for starting an eda/teda task (vsf_eda_start(),
 * vsf_teda_start(), init_vsf_peda(), etc.)
 * \~chinese
 * @brief 启动 eda/teda 任务的配置（vsf_eda_start()、vsf_teda_start()、init_vsf_peda() 等）
 */
typedef struct vsf_eda_cfg_t {
    //! \~english task entry: evthandler / param_evthandler / raw function
    //! \~chinese 任务入口：evthandler / param_evthandler / 原始函数
    __vsf_eda_fn_t              fn;
    //! \~english optional termination callback
    //! (VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE)
    //! \~chinese 可选的终止回调（VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE）
    vsf_eda_on_terminate_t      on_terminate;
    //! \~english task priority (vsf_prio_t); vsf_prio_inherit inherits the
    //! current event queue's priority (not usable in ISR/idle)
    //! \~chinese 任务优先级（vsf_prio_t）；vsf_prio_inherit 继承当前事件队列的优先级（不可在中断/idle 中使用）
    vsf_prio_t                  priority;
    //! \~english task feature flags (@ref vsf_eda_feature_t)
    //! \~chinese 任务特性标志（@ref vsf_eda_feature_t）
    vsf_eda_feature_t           feature;
    //! \~english frame/local area size for frame-based tasks, 0 otherwise
    //! \~chinese 基于帧的任务的帧/局部区域大小，其他任务为 0
    uint16_t                    local_size;
    //! \~english user parameter passed to a param_evthandler / peda
    //! parameter block
    //! \~chinese 传递给 param_evthandler / peda 参数块的用户参数
    uintptr_t                   target;
} vsf_eda_cfg_t;

//! \name eda
//! @{
/**
 * \~english
 * @brief The event-driven task (TCB); every VSF task is an eda at the bottom.
 * Tasks share stacks and only occupy a stack while handling events; driven by
 * events from its event queue
 * \~chinese
 * @brief 事件驱动任务（TCB）；每个 VSF 任务底层都是 eda。任务共享栈，仅在处理事件时占用栈；由其事件队列中的事件驱动
 */
vsf_class(vsf_eda_t) {
#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    protected_member(
        vsf_eda_on_terminate_t  on_terminate;
    )
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    protected_member(
        union {
            vsf_eda_evthandler_t    evthandler;
            vsf_slist_t         frame_list;
            __vsf_eda_frame_t   *frame;
        } fn;
        uintptr_t               return_value;
    )
#else
    protected_member(
        union {
            vsf_eda_evthandler_t    evthandler;
        } fn;
    )
#endif

    protected_member(
#   if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
        union {
            vsf_dlist_node_t    pending_node;
            vsf_slist_node_t    pending_snode;
        };
#   endif

#   if VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED == ENABLED
#       if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED
        vsf_dlist_node_t        rdy_node;
        vsf_slist_queue_t       evt_list;
        uint8_t                 cur_priority;
        uint8_t                 new_priority;
        uint8_t                 priority;
        bool                    is_ready;
#       else
        uint8_t                 evt_cnt;
        uint8_t                 priority;
#       endif
#   else
        uintptr_t               evt_pending;
#   endif

#   if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
        /* value holder for enum fsm_rt_t */
        int8_t                  subcall_return_value;
#   endif
#   if VSF_KERNEL_OPT_AVOID_UNNECESSARY_YIELD_EVT == ENABLED
        bool                    is_evt_incoming;
#   endif
        __vsf_eda_flag_t        flag;
    )

#if VSF_KERNEL_CFG_EDA_CPU_USAGE == ENABLED
    private_member(
        vsf_cpu_usage_t         usage;
    )
#endif
};
//! @}

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
//! \name timed eda
//! @{
/**
 * \~english
 * @brief eda with timer support (can use vsf_teda_set_timer() etc.); derived
 * classes (task/pt/thread) inherit from it when
 * VSF_KERNEL_CFG_EDA_SUPPORT_TIMER is ENABLED
 * \~chinese
 * @brief 带定时器支持的 eda（可使用 vsf_teda_set_timer() 等）；当
 * VSF_KERNEL_CFG_EDA_SUPPORT_TIMER 为 ENABLED 时，派生类（task/pt/thread）从它继承
 */
vsf_class(vsf_teda_t)  {
    public_member(
        implement(vsf_eda_t)
    )
    private_member(
        vsf_dlist_node_t        timer_node;
        vsf_systimer_tick_t     due;
    )
};
//! @}

#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
//! \name callback_timer
//! @{
/**
 * \~english
 * @brief Task-independent timer; when it expires the kernel task invokes the
 * user callback
 * \~chinese
 * @brief 独立于任务的定时器；到期时由内核任务调用用户回调
 */
vsf_class(vsf_callback_timer_t) {
    public_member(
        //! \~english
        //! @brief User callback invoked at due time (in kernel-task context);
        //! must be set before adding the timer
        //! \~chinese
        //! @brief 到期时被调用的用户回调（在内核任务上下文中）；必须在添加定时器之前设置
        void (*on_timer)(vsf_callback_timer_t *timer);
    )
    private_member(
        vsf_dlist_node_t        timer_node;
        vsf_systimer_tick_t     due;
    )
};
//! @}
#endif
#endif

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
//! \name sync
//! @{
/**
 * \~english
 * @brief Base sync IPC object; sem/mutex/trig/crit are built on it
 * \~chinese
 * @brief 基础同步 IPC 对象；sem/mutex/trig/crit 都基于它构建
 */
vsf_class(vsf_sync_t) {
    // it's not good to make cur_union & max_union public
    //  but some APIs in shell will require these to be visible
    public_member(
        //! \~english
        //! @brief Current count: bits.cur = current count, bits.has_owner =
        //! the object has an owner (e.g. mutex)
        //! \~chinese
        //! @brief 当前计数：bits.cur = 当前计数，bits.has_owner = 对象拥有所有者（例如互斥锁）
        union {
            struct {
                uint16_t        cur         : 15;
                uint16_t        has_owner   : 1;
            } bits;
            uint16_t            cur_value;
        } cur_union;
        //! \~english
        //! @brief Maximum count: bits.max = maximum count, bits.manual_rst =
        //! manual-reset
        //! \~chinese
        //! @brief 最大计数：bits.max = 最大计数，bits.manual_rst = 手动复位
        union {
            struct {
                uint16_t        max         : 15;
                uint16_t        manual_rst  : 1;
            } bits;
            uint16_t            max_value;
        } max_union;
    )

    protected_member(
        vsf_dlist_t             pending_list;
    )
};
//! @}

//! \name sync_with_owner
//! @{
/**
 * \~english
 * @brief Sync object with an owner (mutex base); supports priority
 * inheritance; the owner must leave it itself
 * \~chinese
 * @brief 带所有者的同步对象（互斥锁基类）；支持优先级继承；所有者必须自己离开它
 */
vsf_class(vsf_sync_owner_t) {
    public_member(
        implement(vsf_sync_t)
    )
    protected_member(
        vsf_eda_t               *eda_owner;
    )
};
//! @}

#ifndef __VSF_BITMAP_EVT_DEFINED__
#define __VSF_BITMAP_EVT_DEFINED__

/**
 * \~english
 * @brief bmpevt pender op value: the wait completes when any bit of the mask
 * is set
 * \~chinese
 * @brief bmpevt 等待条件的操作值：掩码中任意一位被置位时等待完成
 */
#define VSF_BMPEVT_OR                   0
/**
 * \~english
 * @brief bmpevt pender op value: the wait completes when all bits of the
 * mask are set
 * \~chinese
 * @brief bmpevt 等待条件的操作值：掩码中所有位都被置位时等待完成
 */
#define VSF_BMPEVT_AND                  1

/**
 * \~english
 * @brief Adapter init/reset operations, mapping an IPC object (e.g. a
 * semaphore) onto bitmap-event bits so one task can wait on multiple IPC
 * objects at once
 * \~chinese
 * @brief 适配器的 init/reset 操作，将 IPC 对象（例如信号量）映射到位图事件位上，使一个任务可以同时等待多个 IPC 对象
 */
typedef struct vsf_bmpevt_adapter_op_t {
    vsf_err_t (*init)(vsf_bmpevt_adapter_t *pthis);
    vsf_err_t (*reset)(vsf_bmpevt_adapter_t *pthis);
} vsf_bmpevt_adapter_op_t;
#endif

//! \name bmpevt_adapter
//! @{
/**
 * \~english
 * @brief Adapter mapping an IPC object (e.g. a semaphore) onto bitmap-event
 * bits, so one task can wait on multiple IPC objects at once
 * \~chinese
 * @brief 将 IPC 对象（例如信号量）映射到位图事件位上的适配器，使一个任务可以同时等待多个 IPC 对象
 */
vsf_class(vsf_bmpevt_adapter_t) {

    public_member (
        //! \~english adapter init/reset operations
        //! \~chinese 适配器的 init/reset 操作
        const vsf_bmpevt_adapter_op_t   *op;
        //! \~english bits this adapter drives
        //! \~chinese 该适配器驱动的位
        const uint32_t                  mask;
    )
    private_member(
        vsf_bmpevt_t                    *bmpevt_host;
    )
};
//! @}

//! \name bmpevt_adapter_eda
//! @{
/**
 * \~english
 * @brief bmpevt adapter with an internal eda
 * \~chinese
 * @brief 带内部 eda 的 bmpevt 适配器
 */
vsf_class(vsf_bmpevt_adapter_eda_t) {
    public_member(
        implement(vsf_bmpevt_adapter_t)
    )
    private_member(
        vsf_eda_t               eda;
    )
};
//! @}

//! \name bmpevt_pender
//! @{
/**
 * \~english
 * @brief Wait condition of a bitmap-event wait
 * \~chinese
 * @brief 位图事件等待的等待条件
 */
vsf_class(vsf_bmpevt_pender_t) {

    public_member (
        //! \~english bits to wait for
        //! \~chinese 要等待的位
        uint32_t                mask;
        //! \~english match mode: @ref VSF_BMPEVT_OR / @ref VSF_BMPEVT_AND
        //! \~chinese 匹配模式：@ref VSF_BMPEVT_OR / @ref VSF_BMPEVT_AND
        uint8_t                 op  : 1;
    )

    private_member(
        vsf_eda_t               *eda_pending;
    )
};
//! @}

//! \name bmpevt
//! @{
/**
 * \~english
 * @brief Bitmap-event group
 * \~chinese
 * @brief 位图事件组
 */
vsf_class(vsf_bmpevt_t) {

    public_member (
        //! \~english bits automatically cleared after a pender gets them
        //! \~chinese 等待者获取后自动清除的位
        uint32_t                auto_reset;
        //! \~english array of adapters driving the bits
        //! \~chinese 驱动各个位的适配器数组
        vsf_bmpevt_adapter_t    **adapters;
    )

    private_member(
        vsf_dlist_t             pending_list;
        uint32_t                value;
        uint32_t                cancelled_value;
    )

    private_member(
        union {
            struct {
                uint8_t         adapter_count   : 5;
                uint8_t         is_cancelling   : 1;
                uint8_t         is_polling      : 1;
                uint8_t         is_to_repoll    : 1;
            } bits;
            uint8_t             flag;
        } state;
    )
};
//! @}

#if VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE == ENABLED
/**
 * \~english
 * @brief User-provided queue backend operations for vsf_eda_queue_t;
 * enqueue/dequeue return bool success, dequeue outputs the node pointer
 * \~chinese
 * @brief 用户为 vsf_eda_queue_t 提供的队列后端操作；enqueue/dequeue 返回 bool 表示成功与否，dequeue 输出节点指针
 */
typedef struct vsf_eda_queue_op_t {
    bool (*enqueue)(vsf_eda_queue_t *pthis, void *node);
    bool (*dequeue)(vsf_eda_queue_t *pthis, void **node);
} vsf_eda_queue_op_t;

/**
 * \~english
 * @brief Aliases of vsf_eda_queue_t / vsf_eda_queue_op_t; osa means os-aware
 * \~chinese
 * @brief vsf_eda_queue_t / vsf_eda_queue_op_t 的别名；osa 表示 os-aware（OS 感知）
 */
//! \brief define alias for vsf_eda_queue_t. osa means os-aware
typedef struct vsf_eda_queue_t vsf_osa_queue_t;
typedef struct vsf_eda_queue_op_t vsf_osa_queue_op_t;

//! \name queue
//! @{
/**
 * \~english
 * @brief OS-aware queue; send pends while full, recv pends while empty;
 * backend ops are supplied by the user (e.g. the slist queue @ref
 * vsf_eda_slist_queue_t)
 * \~chinese
 * @brief OS 感知队列；满时发送挂起，空时接收挂起；后端操作由用户提供（例如 slist 队列 @ref vsf_eda_slist_queue_t）
 */
vsf_class(vsf_eda_queue_t) {
    union {
        implement(vsf_sync_t)
#if VSF_KERNEL_CFG_QUEUE_MULTI_TX_EN == ENABLED

        protected_member(
            union {
                uint16_t        __cur_value;
            };
            union {
                struct {
                    uint16_t    __max         : 15;
                    uint16_t    tx_processing : 1;
                };
                uint16_t        __max_value;
            };
        )
#else
        protected_member(
            struct {
                uint16_t        __cur_value;
                uint16_t        __max_value;
                vsf_eda_t       *eda_tx;
            };
        )
#endif
    };

    public_member(
#if VSF_EDA_QUEUE_CFG_REGION == ENABLED
        //! \~english protection region of the queue
        //! \~chinese 队列的保护区域
        vsf_protect_region_t    *region;
#endif
        //! \~english user-provided queue backend operations (@ref
        //! vsf_eda_queue_op_t)
        //! \~chinese 用户提供的队列后端操作（@ref vsf_eda_queue_op_t）
        vsf_eda_queue_op_t      op;
    )

    protected_member(
        vsf_eda_t               *eda_rx;
#if VSF_EDA_QUEUE_CFG_SUPPORT_ISR == ENABLED
        uint16_t                readable_cnt;
#endif
#if VSF_KERNEL_CFG_QUEUE_HAS_RX_NOTIFIED == ENABLED
        bool                    rx_notified;
#endif
    )
};
//! @}
#endif



#   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
typedef vsf_teda_t  vsf_peda_t;
#   else
typedef vsf_eda_t  vsf_peda_t;
#   endif

// IPC
/**
 * \~english
 * @brief Result of a sync wait, returned by the *_get_reason() APIs and
 * thread IPC
 * \~chinese
 * @brief 同步等待的结果，由 *_get_reason() API 和线程 IPC 返回
 */
typedef enum vsf_sync_reason_t {
    //! \~english
    //! @brief Failed/error
    //! \~chinese
    //! @brief 失败/错误
    VSF_SYNC_FAIL,
    //! \~english
    //! @brief Timed out
    //! \~chinese
    //! @brief 超时
    VSF_SYNC_TIMEOUT,
    //! \~english
    //! @brief Still waiting, retry
    //! \~chinese
    //! @brief 仍在等待，请重试
    VSF_SYNC_PENDING,
    //! \~english
    //! @brief Obtained
    //! \~chinese
    //! @brief 已获取
    VSF_SYNC_GET,
    //! \~english
    //! @brief Cancelled
    //! \~chinese
    //! @brief 已取消
    VSF_SYNC_CANCEL
} vsf_sync_reason_t;

/**
 * \~english
 * @brief Counting semaphore (vsf_sync_t with auto-reset)
 * \~chinese
 * @brief 计数信号量（自动复位的 vsf_sync_t）
 */
typedef vsf_sync_t              vsf_sem_t;
/**
 * \~english
 * @brief Trigger/event flag (sync with max 1, auto or manual reset)
 * \~chinese
 * @brief 触发器/事件标志（最大计数为 1 的同步对象，自动或手动复位）
 */
typedef vsf_sync_t              vsf_trig_t;
// internal use only
typedef vsf_sync_t              __vsf_crit_npb_t;

/**
 * \~english
 * @brief Mutex (vsf_sync_owner_t with priority inheritance); the owner must
 * leave it itself
 * \~chinese
 * @brief 互斥锁（带优先级继承的 vsf_sync_owner_t）；所有者必须自己离开它
 */
// vsf_mutex_t support priority inherit
// so who claim mutex, he must free the mutex himself
typedef vsf_sync_owner_t        vsf_mutex_t;
/**
 * \~english
 * @brief Critical section (alias of mutex semantics)
 * \~chinese
 * @brief 临界区（互斥锁语义的别名）
 */
typedef vsf_mutex_t             vsf_crit_t;

/**
 * \~english
 * @brief Adapter mapping a @ref vsf_sync_t onto bitmap-event bits
 * \~chinese
 * @brief 将 @ref vsf_sync_t 映射到位图事件位上的适配器
 */
typedef struct vsf_bmpevt_adapter_sync_t {
    implement(vsf_bmpevt_adapter_eda_t)
    vsf_sync_t *sync;
} vsf_bmpevt_adapter_sync_t;

/**
 * \~english
 * @brief Adapter mapping a nested @ref vsf_bmpevt_t onto bitmap-event bits
 * \~chinese
 * @brief 将嵌套的 @ref vsf_bmpevt_t 映射到位图事件位上的适配器
 */
typedef struct vsf_bmpevt_adapter_bmpevt_t {
    implement(vsf_bmpevt_adapter_eda_t)
    vsf_bmpevt_t *bmpevt;
    vsf_bmpevt_pender_t pender;
} vsf_bmpevt_adapter_bmpevt_t;
#endif

/**
 * \~english
 * @brief Kernel error codes reported to vsf_kernel_err_report()
 * \~chinese
 * @brief 报告给 vsf_kernel_err_report() 的内核错误码
 */
typedef enum vsf_kernel_error_t {
    //! \~english
    //! @brief No error
    //! \~chinese
    //! @brief 无错误
    VSF_KERNEL_ERR_NONE = 0,
    //! \~english
    //! @brief API called from an invalid context
    //! \~chinese
    //! @brief 在无效的上下文中调用了 API
    VSF_KERNEL_ERR_INVALID_CONTEXT,
    //! \~english
    //! @brief API used incorrectly
    //! \~chinese
    //! @brief API 使用方式错误
    VSF_KERNEL_ERR_INVALID_USAGE,
    //! \~english
    //! @brief Timer service used on an eda without timer support
    //! \~chinese
    //! @brief 在不支持定时器的 eda 上使用了定时器服务
    VSF_KERNEL_ERR_EDA_DOES_NOT_SUPPORT_TIMER,
    //! \~english
    //! @brief vsf_prio_inherit used where there is no current event queue
    //! (idle or ISR)
    //! \~chinese
    //! @brief 在没有当前事件队列的地方（idle 或中断）使用了 vsf_prio_inherit
    VSF_KERNEL_ERR_SHOULD_NOT_USE_PRIO_INHERIT_IN_IDLE_OR_ISR
} vsf_kernel_error_t;

typedef struct vsf_kernel_cfg_t {
    vsf_prio_t                  highest_prio;
    vsf_arch_prio_t             systimer_arch_prio;
} vsf_kernel_cfg_t;


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED

#   if VSF_KERNEL_CFG_TIMER_MODE == VSF_KERNEL_CFG_TIMER_MODE_TICK
VSF_CAL_SECTION(".text.vsf.kernel.teda")
/**
 * \~english
 * @brief System timer tick handler; called by the user on every system tick to
 * drive the kernel timers
 * @return none
 *
 * @note Only used when VSF_KERNEL_CFG_TIMER_MODE is VSF_KERNEL_CFG_TIMER_MODE_TICK,
 * called from the system timer interrupt.
 * \~chinese
 * @brief 系统定时器 tick 处理函数；由用户在每个系统 tick 调用，用于驱动内核定时器
 * @return 无
 *
 * @note 仅在 VSF_KERNEL_CFG_TIMER_MODE 为 VSF_KERNEL_CFG_TIMER_MODE_TICK 时使用，
 * 在系统定时器中断中调用。
 */
extern void vsf_systimer_on_tick(void);
#   endif

VSF_CAL_SECTION(".text.vsf.kernel.teda")
/**
 * \~english
 * @brief Get the current system timer tick count
 * @return vsf_systimer_tick_t: current tick count
 * @note Can be called in any context (task or interrupt).
 * \~chinese
 * @brief 获取当前系统定时器的 tick 计数
 * @return vsf_systimer_tick_t: 当前 tick 计数
 * @note 可以在任意上下文（任务或中断）中调用。
 */
extern vsf_systimer_tick_t vsf_systimer_get_tick(void);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_systimer_get_duration")
/**
 * \~english
 * @brief Get the tick duration between two tick counts, handling wrap-around
 * @param[in] from_time: the start tick count
 * @param[in] to_time: the end tick count
 * @return vsf_systimer_tick_t: tick duration from from_time to to_time
 * @note Can be called in any context (task or interrupt).
 * \~chinese
 * @brief 获取两个 tick 计数之间的 tick 间隔，已处理回绕
 * @param[in] from_time: 起始 tick 计数
 * @param[in] to_time: 结束 tick 计数
 * @return vsf_systimer_tick_t: 从 from_time 到 to_time 的 tick 间隔
 * @note 可以在任意上下文（任务或中断）中调用。
 */
extern vsf_systimer_tick_t vsf_systimer_get_duration(vsf_systimer_tick_t from_time, vsf_systimer_tick_t to_time);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_systimer_get_elapsed")
/**
 * \~english
 * @brief Get the ticks elapsed from the given tick count to now
 * @param[in] from_time: the start tick count
 * @return vsf_systimer_tick_t: ticks elapsed since from_time
 * @note Can be called in any context (task or interrupt).
 * \~chinese
 * @brief 获取从给定 tick 计数到现在经过的 tick 数
 * @param[in] from_time: 起始 tick 计数
 * @return vsf_systimer_tick_t: 从 from_time 到现在经过的 tick 数
 * @note 可以在任意上下文（任务或中断）中调用。
 */
extern vsf_systimer_tick_t vsf_systimer_get_elapsed(vsf_systimer_tick_t from_time);

#endif

#if defined(__VSF_EDA_CLASS_INHERIT__) || defined(__VSF_EDA_CLASS_IMPLEMENT)
VSF_CAL_SECTION(".text.vsf.kernel.eda")
/**
 * \~english
 * @brief Initialize an eda and post VSF_EVT_INIT to it
 * @param[in] pthis: a pointer to structure @ref vsf_eda_t
 * @param[in] priority: priority of the task
 * @param[in] feature: task feature @ref vsf_eda_feature_t
 * @return vsf_err_t: VSF_ERR_NONE if the INIT event is posted; VSF_ERR_NOT_ENOUGH_RESOURCES if no event node is available
 *
 * @note Internal use only.
 * @note Must be called in task (or initialization) context, NOT in interrupt context.
 * \~chinese
 * @brief 初始化一个 eda，并向其发送 VSF_EVT_INIT 事件
 * @param[in] pthis: 指向结构体 @ref vsf_eda_t 的指针
 * @param[in] priority: 任务优先级
 * @param[in] feature: 任务特性 @ref vsf_eda_feature_t
 * @return vsf_err_t: INIT 事件发送成功返回 VSF_ERR_NONE；没有可用的事件节点返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 仅供内部使用。
 * @note 必须在任务（或初始化）上下文中调用，禁止在中断上下文中调用。
 */
extern vsf_err_t __vsf_eda_init(vsf_eda_t *pthis, vsf_prio_t priority, vsf_eda_feature_t feature);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_set_evthandler")
/**
 * \~english
 * @brief Set the event handler of an eda
 * @param[in] pthis: a pointer to structure @ref vsf_eda_t
 * @param[in] evthandler: the new event handler @ref vsf_eda_evthandler_t
 * @return vsf_err_t: always VSF_ERR_NONE
 *
 * @note Internal use only.
 * \~chinese
 * @brief 设置 eda 的事件处理函数
 * @param[in] pthis: 指向结构体 @ref vsf_eda_t 的指针
 * @param[in] evthandler: 新的事件处理函数 @ref vsf_eda_evthandler_t
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE
 *
 * @note 仅供内部使用。
 */
extern vsf_err_t vsf_eda_set_evthandler(vsf_eda_t *pthis, vsf_eda_evthandler_t evthandler);

VSF_CAL_SECTION(".text.vsf.kernel.eda")
/**
 * \~english
 * @brief Initialize the eda kernel module
 * @param[in] cfg_ptr: a pointer to structure @ref vsf_kernel_cfg_t
 * @return none
 *
 * @note Internal use only.
 * @note Must be called in task (or initialization) context, NOT in interrupt context.
 * \~chinese
 * @brief 初始化 eda 内核模块
 * @param[in] cfg_ptr: 指向结构体 @ref vsf_kernel_cfg_t 的指针
 * @return 无
 *
 * @note 仅供内部使用。
 * @note 必须在任务（或初始化）上下文中调用，禁止在中断上下文中调用。
 */
extern void vsf_kernel_init( const vsf_kernel_cfg_t *cfg_ptr);

#   if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
VSF_CAL_SECTION(".text.vsf.kernel.eda")
/**
 * \~english
 * @brief Post an event to an eda, with an option to force the post
 * @param[in] pthis: a pointer to structure @ref vsf_eda_t
 * @param[in] evt: the event to post
 * @param[in] force: true to force posting even if the task only accepts one event
 * @return vsf_err_t: VSF_ERR_NONE if posted; VSF_ERR_FAIL if the eda is limited and already has a pending event (and force is not set); VSF_ERR_NOT_ENOUGH_RESOURCES if no event node is available
 *
 * @note Internal use only. Only available when VSF_KERNEL_CFG_SUPPORT_SYNC is ENABLED.
 * \~chinese
 * @brief 向 eda 发送一个事件，可选择强制发送
 * @param[in] pthis: 指向结构体 @ref vsf_eda_t 的指针
 * @param[in] evt: 要发送的事件
 * @param[in] force: 为 true 时即使任务只接受一个事件也强制发送
 * @return vsf_err_t: 发送成功返回 VSF_ERR_NONE；eda 受限且已有待处理事件（且未设置 force）返回 VSF_ERR_FAIL；没有可用的事件节点返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 仅供内部使用。仅在 VSF_KERNEL_CFG_SUPPORT_SYNC 使能时可用。
 */
vsf_err_t __vsf_eda_post_evt_ex(vsf_eda_t *pthis, vsf_evt_t evt, bool force);
#   endif

#   if VSF_KERNEL_USE_SIMPLE_SHELL == ENABLED

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_polling_state_get")
/**
 * \~english
 * @brief Get the polling state of an eda
 * @param[in] pthis: a pointer to structure @ref vsf_eda_t
 * @return bool: current polling state
 *
 * @note Internal use only. Only available when VSF_KERNEL_USE_SIMPLE_SHELL is ENABLED.
 * \~chinese
 * @brief 获取 eda 的轮询状态
 * @param[in] pthis: 指向结构体 @ref vsf_eda_t 的指针
 * @return bool: 当前轮询状态
 *
 * @note 仅供内部使用。仅在 VSF_KERNEL_USE_SIMPLE_SHELL 使能时可用。
 */
extern bool vsf_eda_polling_state_get(vsf_eda_t *pthis);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_polling_state_set")
/**
 * \~english
 * @brief Set the polling state of an eda
 * @param[in] pthis: a pointer to structure @ref vsf_eda_t
 * @param[in] state: the polling state to set
 * @return none
 *
 * @note Internal use only. Only available when VSF_KERNEL_USE_SIMPLE_SHELL is ENABLED.
 * \~chinese
 * @brief 设置 eda 的轮询状态
 * @param[in] pthis: 指向结构体 @ref vsf_eda_t 的指针
 * @param[in] state: 要设置的轮询状态
 * @return 无
 *
 * @note 仅供内部使用。仅在 VSF_KERNEL_USE_SIMPLE_SHELL 使能时可用。
 */
extern void vsf_eda_polling_state_set(vsf_eda_t *pthis, bool state);

#   endif
#endif

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_set_evthandler")
/**
 * \~english
 * @brief Switch the current task to a new event handler and post VSF_EVT_INIT to it
 * @param[in] evthandler: address of the new event handler @ref vsf_eda_evthandler_t
 * @return vsf_err_t: always VSF_ERR_NONE
 * @note Operates on the current task; must be called in the current eda/teda task's event handler context.
 * \~chinese
 * @brief 切换当前任务到新的事件处理函数，并向其发送 VSF_EVT_INIT 事件
 * @param[in] evthandler: 新的事件处理函数 @ref vsf_eda_evthandler_t 的地址
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE
 * @note 作用于当前任务，必须在当前 eda/teda 任务的事件处理函数上下文中调用。
 */
extern vsf_err_t vsf_eda_go_to(uintptr_t evthandler);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_start")
/**
 * \~english
 * @brief Start an eda task with the given configuration and post VSF_EVT_INIT to it
 * @param[in] pthis: a pointer to structure @ref vsf_eda_t
 * @param[in] cfg: a pointer to structure @ref vsf_eda_cfg_t holding the task configuration
 * @return vsf_err_t: VSF_ERR_NONE if successful, VSF_ERR_NOT_ENOUGH_RESOURCES if
 * no frame is available for a frame-based task
 * @note Must be called in task (or initialization) context, NOT in interrupt context.
 * \~chinese
 * @brief 以给定的配置启动一个 eda 任务，并向其发送 VSF_EVT_INIT 事件
 * @param[in] pthis: 指向结构体 @ref vsf_eda_t 的指针
 * @param[in] cfg: 指向保存任务配置的结构体 @ref vsf_eda_cfg_t 的指针
 * @return vsf_err_t: 成功返回 VSF_ERR_NONE，如果基于帧的任务没有可用的帧返回
 * VSF_ERR_NOT_ENOUGH_RESOURCES
 * @note 必须在任务（或初始化）上下文中调用，禁止在中断上下文中调用。
 */
vsf_err_t vsf_eda_start(vsf_eda_t *pthis, vsf_eda_cfg_t *cfg);

// if vsf_eda_get_cur return NULL, means not in task context
VSF_CAL_SECTION(".text.vsf.kernel.eda")
/**
 * \~english
 * @brief Get the eda task currently being dispatched
 * @return vsf_eda_t *: pointer to the current @ref vsf_eda_t, NULL if not in task context
 * @note Can be called in any context; returns NULL when not in eda task context.
 * \~chinese
 * @brief 获取当前正在处理的 eda 任务
 * @return vsf_eda_t *: 指向当前 @ref vsf_eda_t 的指针，如果不在任务上下文中返回 NULL
 * @note 可以在任意上下文中调用；不在 eda 任务上下文时返回 NULL。
 */
extern vsf_eda_t *vsf_eda_get_cur(void);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_get_cur_evt")
/**
 * \~english
 * @brief Get the event currently being processed by the current task
 * @return vsf_evt_t: the current event
 * @note Operates on the current task; must be called in the current eda/teda task's event handler context.
 * \~chinese
 * @brief 获取当前任务正在处理的事件
 * @return vsf_evt_t: 当前事件
 * @note 作用于当前任务，必须在当前 eda/teda 任务的事件处理函数上下文中调用。
 */
extern vsf_evt_t vsf_eda_get_cur_evt(void);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_get_cur_msg")
/**
 * \~english
 * @brief Get the pointer message carried by the current event (e.g. VSF_EVT_MESSAGE)
 * @return void *: the message pointer posted with the current event
 * @note Operates on the current task; must be called in the current eda/teda task's event handler context.
 * \~chinese
 * @brief 获取当前事件携带的指针消息（例如 VSF_EVT_MESSAGE）
 * @return void *: 随当前事件发送的消息指针
 * @note 作用于当前任务，必须在当前 eda/teda 任务的事件处理函数上下文中调用。
 */
extern void *vsf_eda_get_cur_msg(void);

#if VSF_KERNEL_USE_SIMPLE_SHELL == ENABLED
VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_is_stack_owner")
/**
 * \~english
 * @brief Check whether an eda owns a dedicated stack
 * @param[in] pthis: a pointer to structure @ref vsf_eda_t
 * @return bool: true if the eda owns a dedicated stack
 *
 * @note Only available when VSF_KERNEL_USE_SIMPLE_SHELL is ENABLED.
 * \~chinese
 * @brief 检查 eda 是否拥有独立堆栈
 * @param[in] pthis: 指向结构体 @ref vsf_eda_t 的指针
 * @return bool: 如果 eda 拥有独立堆栈返回 true
 *
 * @note 仅在 VSF_KERNEL_USE_SIMPLE_SHELL 使能时可用。
 */
extern bool vsf_eda_is_stack_owner(vsf_eda_t *pthis);
#endif

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_return")
/**
 * \~english
 * @brief Return from the current (sub-called) eda with the given return value
 * @param[in] return_value: return value passed back to the caller
 * @return bool: true if the current task is terminated (no caller), false if
 * control is returned to the caller
 *
 * @note Internal use only. Use the macro vsf_eda_return() instead.
 * @note Operates on the current task; must be called in the current eda/teda task's event handler context.
 * \~chinese
 * @brief 以给定的返回值从当前（被子调用的）eda 返回
 * @param[in] return_value: 传递回调用者的返回值
 * @return bool: 如果当前任务被终止（没有调用者）返回 true，如果返回到调用者
 * 返回 false
 *
 * @note 仅供内部使用。请使用宏 vsf_eda_return()。
 * @note 作用于当前任务，必须在当前 eda/teda 任务的事件处理函数上下文中调用。
 */
extern bool __vsf_eda_return(uintptr_t return_value);


#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_get_return_value")
/**
 * \~english
 * @brief Get the return value passed back by the sub-called task
 * @return uintptr_t: the return value set by vsf_eda_return()
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is ENABLED.
 * @note Operates on the current task; must be called in the current eda/teda task's event handler context.
 * \~chinese
 * @brief 获取被子调用任务传回的返回值
 * @return uintptr_t: 由 vsf_eda_return() 设置的返回值
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL 使能时可用。
 * @note 作用于当前任务，必须在当前 eda/teda 任务的事件处理函数上下文中调用。
 */
extern uintptr_t vsf_eda_get_return_value(void);
#endif

VSF_CAL_SECTION(".text.vsf.kernel.__vsf_eda_yield")
/**
 * \~english
 * @brief Yield the current task by posting VSF_EVT_YIELD to itself, so that it
 * is dispatched again later
 * @return none
 *
 * @note Internal use only.
 * @note Operates on the current task; must be called in the current eda/teda task's event handler context.
 * \~chinese
 * @brief 通过向自身发送 VSF_EVT_YIELD 事件让出当前任务，使其稍后被再次处理
 * @return 无
 *
 * @note 仅供内部使用。
 * @note 作用于当前任务，必须在当前 eda/teda 任务的事件处理函数上下文中调用。
 */
extern void __vsf_eda_yield(void);

#if VSF_KERNEL_CFG_EDA_CPU_USAGE == ENABLED
// user should provide vsf_cpu_usage_ctx_t memory, and maintain this memory until stop
//  the ticks used returned is actually including ticks from all higher priority tasks and interrupt
/**
 * \~english
 * @brief Start measuring the CPU usage of an eda task
 * @param[in] pthis: a pointer to structure @ref vsf_eda_t
 * @param[in] ctx: a pointer to user-provided structure @ref vsf_cpu_usage_ctx_t,
 * which must be kept valid until vsf_eda_cpu_usage_stop() is called
 * @return none
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_CPU_USAGE is ENABLED. The
 * measured ticks actually include ticks from all higher priority tasks and interrupts.
 * \~chinese
 * @brief 开始测量一个 eda 任务的 CPU 占用
 * @param[in] pthis: 指向结构体 @ref vsf_eda_t 的指针
 * @param[in] ctx: 指向用户提供的结构体 @ref vsf_cpu_usage_ctx_t 的指针，
 * 在调用 vsf_eda_cpu_usage_stop() 之前必须保持有效
 * @return 无
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_CPU_USAGE 使能时可用。测量的 tick 数实际包含
 * 所有更高优先级任务和中断占用的 tick。
 */
extern void vsf_eda_cpu_usage_start(vsf_eda_t *pthis, vsf_cpu_usage_ctx_t *ctx);
/**
 * \~english
 * @brief Stop measuring the CPU usage of an eda task
 * @param[in] pthis: a pointer to structure @ref vsf_eda_t
 * @return none
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_CPU_USAGE is ENABLED.
 * \~chinese
 * @brief 停止测量一个 eda 任务的 CPU 占用
 * @param[in] pthis: 指向结构体 @ref vsf_eda_t 的指针
 * @return 无
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_CPU_USAGE 使能时可用。
 */
extern void vsf_eda_cpu_usage_stop(vsf_eda_t *pthis);
#endif

#if defined(__VSF_EDA_CLASS_INHERIT__) || defined(__VSF_EDA_CLASS_IMPLEMENT)
/* vsf_eda_fini() enables you to kill other eda tasks.
   We highly recommend that DO NOT use this api until you 100% sure.
   please make sure that the resources are properly freed when you trying to kill
   an eda other than your own. We highly recommend that please send a semaphore to
   the target eda to ask it killing itself after properly freeing all the resources.
 */
VSF_CAL_SECTION(".text.vsf.kernel.eda")
/**
 * \~english
 * @brief Finalize (terminate) an eda task
 * @param[in] pthis: a pointer to structure @ref vsf_eda_t, the current task if NULL
 * @return vsf_err_t: always VSF_ERR_NONE
 *
 * @note Internal use only. Read the warning above carefully before killing an
 * eda task other than the current one.
 * \~chinese
 * @brief 终止一个 eda 任务
 * @param[in] pthis: 指向结构体 @ref vsf_eda_t 的指针，为 NULL 时表示当前任务
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE
 *
 * @note 仅供内部使用。在终止非当前任务之前，请仔细阅读上面的警告。
 */
extern vsf_err_t vsf_eda_fini(vsf_eda_t *pthis);

VSF_CAL_SECTION(".text.vsf.kernel.eda")
/**
 * \~english
 * @brief Dispatch an event to the event handler of an eda
 * @param[in] pthis: a pointer to structure @ref vsf_eda_t
 * @param[in] evt: the event to dispatch
 * @return none
 *
 * @note Internal use only.
 * \~chinese
 * @brief 将事件分发给 eda 的事件处理函数
 * @param[in] pthis: 指向结构体 @ref vsf_eda_t 的指针
 * @param[in] evt: 要分发的事件
 * @return 无
 *
 * @note 仅供内部使用。
 */
extern void __vsf_dispatch_evt(vsf_eda_t *pthis, vsf_evt_t evt);

VSF_CAL_SECTION(".text.vsf.kernel.eda")
/**
 * \~english
 * @brief Invoke the on_terminate callback of an eda when it is terminated
 * @param[in] pthis: a pointer to structure @ref vsf_eda_t
 * @return none
 *
 * @note Internal use only.
 * \~chinese
 * @brief 在 eda 终止时调用其 on_terminate 回调
 * @param[in] pthis: 指向结构体 @ref vsf_eda_t 的指针
 * @return 无
 *
 * @note 仅供内部使用。
 */
void __vsf_eda_on_terminate(vsf_eda_t *pthis);

#   if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED
VSF_CAL_SECTION(".text.vsf.kernel.__vsf_eda_get_cur_priority")
/**
 * \~english
 * @brief Get the current (effective, possibly boosted) priority of an eda
 * @param[in] pthis: a pointer to structure @ref vsf_eda_t
 * @return vsf_prio_t: the current effective priority
 *
 * @note Internal use only. Only available when VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY
 * is ENABLED.
 * \~chinese
 * @brief 获取 eda 的当前（实际生效的，可能被提升过的）优先级
 * @param[in] pthis: 指向结构体 @ref vsf_eda_t 的指针
 * @return vsf_prio_t: 当前实际生效的优先级
 *
 * @note 仅供内部使用。仅在 VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY 使能时可用。
 */
extern vsf_prio_t __vsf_eda_get_cur_priority(vsf_eda_t *pthis);

VSF_CAL_SECTION(".text.vsf.kernel.__vsf_eda_set_priority")
/**
 * \~english
 * @brief Set the priority of an eda dynamically
 * @param[in] pthis: a pointer to structure @ref vsf_eda_t
 * @param[in] prio: the new priority
 * @return vsf_err_t: always VSF_ERR_NONE
 *
 * @note Internal use only. Only available when VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY
 * is ENABLED.
 * \~chinese
 * @brief 动态设置 eda 的优先级
 * @param[in] pthis: 指向结构体 @ref vsf_eda_t 的指针
 * @param[in] prio: 新的优先级
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE
 *
 * @note 仅供内部使用。仅在 VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY 使能时可用。
 */
extern vsf_err_t __vsf_eda_set_priority(vsf_eda_t *pthis, vsf_prio_t prio);
#   endif

#endif

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_set_user_value")
/**
 * \~english
 * @brief Set the user-defined bits in the feature of the current eda
 * @param[in] value: the user value to set, up to VSF_KERNEL_CFG_EDA_USER_BITLEN bits
 * @return none
 * @note Operates on the current task; must be called in the current eda/teda task's event handler context.
 * \~chinese
 * @brief 设置当前 eda 特性中的用户自定义位
 * @param[in] value: 要设置的用户值，最多 VSF_KERNEL_CFG_EDA_USER_BITLEN 位
 * @return 无
 * @note 作用于当前任务，必须在当前 eda/teda 任务的事件处理函数上下文中调用。
 */
extern void vsf_eda_set_user_value(uint8_t value);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_get_user_value")
/**
 * \~english
 * @brief Get the user-defined bits in the feature of the current eda
 * @return uint8_t: the user value previously set by vsf_eda_set_user_value()
 * @note Operates on the current task; must be called in the current eda/teda task's event handler context.
 * \~chinese
 * @brief 获取当前 eda 特性中的用户自定义位
 * @return uint8_t: 之前由 vsf_eda_set_user_value() 设置的用户值
 * @note 作用于当前任务，必须在当前 eda/teda 任务的事件处理函数上下文中调用。
 */
extern uint8_t vsf_eda_get_user_value(void);

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
VSF_CAL_SECTION(".text.vsf.kernel.__vsf_eda_call_eda")
/**
 * \~english
 * @brief Prepare a sub-call frame without dispatching VSF_EVT_INIT
 * @param[in] evthandler: address of the event handler to call
 * @param[in] param: target parameter of the new frame
 * @param[in] local_size: size of the local variable storage
 * @return vsf_err_t: VSF_ERR_NONE if the frame is prepared, VSF_ERR_NOT_ENOUGH_RESOURCES
 * if no frame is available
 *
 * @note Internal use only. Only available when VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL
 * is ENABLED.
 * @note Operates on the current task; must be called in the current eda/teda task's event handler context.
 * \~chinese
 * @brief 准备子调用帧，但不分发 VSF_EVT_INIT 事件
 * @param[in] evthandler: 要调用的事件处理函数地址
 * @param[in] param: 新帧的目标参数
 * @param[in] local_size: 局部变量存储的大小
 * @return vsf_err_t: 如果帧已准备好返回 VSF_ERR_NONE，如果没有可用的帧返回
 * VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 仅供内部使用。仅在 VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL 使能时可用。
 * @note 作用于当前任务，必须在当前 eda/teda 任务的事件处理函数上下文中调用。
 */
vsf_err_t __vsf_eda_call_eda_prepare(   uintptr_t evthandler,
                                        uintptr_t param,
                                        size_t local_size);

VSF_CAL_SECTION(".text.vsf.kernel.__vsf_eda_call_eda")
/**
 * \~english
 * @brief Sub-call an event handler: prepare a frame and dispatch VSF_EVT_INIT
 * @param[in] evthandler: address of the event handler to call
 * @param[in] param: target parameter of the new frame
 * @param[in] local_size: size of the local variable storage
 * @return vsf_err_t: VSF_ERR_NONE if the sub-call is started, VSF_ERR_NOT_ENOUGH_RESOURCES
 * if no frame is available
 *
 * @note Internal use only. Use the macros vsf_eda_call_eda() or
 * vsf_eda_call_param_eda() instead. Only available when
 * VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is ENABLED.
 * @note Operates on the current task; must be called in the current eda/teda task's event handler context.
 * \~chinese
 * @brief 子调用一个事件处理函数：准备帧并分发 VSF_EVT_INIT 事件
 * @param[in] evthandler: 要调用的事件处理函数地址
 * @param[in] param: 新帧的目标参数
 * @param[in] local_size: 局部变量存储的大小
 * @return vsf_err_t: 如果子调用启动返回 VSF_ERR_NONE，如果没有可用的帧返回
 * VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 仅供内部使用。请使用宏 vsf_eda_call_eda() 或
 * vsf_eda_call_param_eda()。仅在 VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL
 * 使能时可用。
 * @note 作用于当前任务，必须在当前 eda/teda 任务的事件处理函数上下文中调用。
 */
extern
vsf_err_t __vsf_eda_call_eda(           uintptr_t evthandler,
                                        uintptr_t param,
                                        size_t local_size);

VSF_CAL_SECTION(".text.vsf.kernel.__vsf_eda_go_to_ex")
/**
 * \~english
 * @brief Switch the current frame to a new event handler with a target parameter
 * and dispatch VSF_EVT_INIT
 * @param[in] evthandler: address of the new event handler
 * @param[in] param: target parameter
 * @return vsf_err_t: VSF_ERR_NONE if started successfully; VSF_ERR_NOT_ENOUGH_RESOURCES if no frame is available
 *
 * @note Internal use only. Only available when VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL
 * is ENABLED.
 * @note Operates on the current task; must be called in the current eda/teda task's event handler context.
 * \~chinese
 * @brief 切换当前帧到带目标参数的新事件处理函数，并分发 VSF_EVT_INIT 事件
 * @param[in] evthandler: 新的事件处理函数地址
 * @param[in] param: 目标参数
 * @return vsf_err_t: 启动成功返回 VSF_ERR_NONE；没有可用的帧返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 仅供内部使用。仅在 VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL 使能时可用。
 * @note 作用于当前任务，必须在当前 eda/teda 任务的事件处理函数上下文中调用。
 */
extern vsf_err_t __vsf_eda_go_to_ex(uintptr_t evthandler, uintptr_t param);

VSF_CAL_SECTION(".text.vsf.kernel.eda_nesting")
/**
 * \~english
 * @brief Prepare an extended sub-call frame without dispatching VSF_EVT_INIT
 * @param[in] func: address of the handler function to call
 * @param[in] param: target parameter of the new frame
 * @param[in] state: frame state @ref __vsf_eda_frame_state_t (feature and local size)
 * @param[in] is_sub_call: true for a sub-call (new frame), false to reuse the current frame
 * @return vsf_err_t: VSF_ERR_NONE if the frame is prepared, VSF_ERR_NOT_ENOUGH_RESOURCES
 * if no frame is available
 *
 * @note Internal use only. Only available when VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL
 * is ENABLED.
 * @note Operates on the current task; must be called in the current eda/teda task's event handler context.
 * \~chinese
 * @brief 准备扩展子调用帧，但不分发 VSF_EVT_INIT 事件
 * @param[in] func: 要调用的处理函数地址
 * @param[in] param: 新帧的目标参数
 * @param[in] state: 帧状态 @ref __vsf_eda_frame_state_t（特性和局部变量大小）
 * @param[in] is_sub_call: 为 true 表示子调用（新帧），为 false 表示复用当前帧
 * @return vsf_err_t: 如果帧已准备好返回 VSF_ERR_NONE，如果没有可用的帧返回
 * VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 仅供内部使用。仅在 VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL 使能时可用。
 * @note 作用于当前任务，必须在当前 eda/teda 任务的事件处理函数上下文中调用。
 */
extern vsf_err_t __vsf_eda_call_eda_ex_prepare(
                                        uintptr_t func,
                                        uintptr_t param,
                                        __vsf_eda_frame_state_t state,
                                        bool is_sub_call);

VSF_CAL_SECTION(".text.vsf.kernel.eda_nesting")
/**
 * \~english
 * @brief Extended sub-call: prepare a frame and dispatch VSF_EVT_INIT
 * @param[in] func: address of the handler function to call
 * @param[in] param: target parameter of the new frame
 * @param[in] state: frame state @ref __vsf_eda_frame_state_t (feature and local size)
 * @param[in] is_sub_call: true for a sub-call (new frame), false to reuse the current frame
 * @return vsf_err_t: VSF_ERR_NONE if the call is started, VSF_ERR_NOT_ENOUGH_RESOURCES
 * if no frame is available
 *
 * @note Internal use only. Only available when VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL
 * is ENABLED.
 * @note Operates on the current task; must be called in the current eda/teda task's event handler context.
 * \~chinese
 * @brief 扩展子调用：准备帧并分发 VSF_EVT_INIT 事件
 * @param[in] func: 要调用的处理函数地址
 * @param[in] param: 新帧的目标参数
 * @param[in] state: 帧状态 @ref __vsf_eda_frame_state_t（特性和局部变量大小）
 * @param[in] is_sub_call: 为 true 表示子调用（新帧），为 false 表示复用当前帧
 * @return vsf_err_t: 如果调用启动返回 VSF_ERR_NONE，如果没有可用的帧返回
 * VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 仅供内部使用。仅在 VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL 使能时可用。
 * @note 作用于当前任务，必须在当前 eda/teda 任务的事件处理函数上下文中调用。
 */
extern vsf_err_t __vsf_eda_call_eda_ex( uintptr_t func,
                                        uintptr_t param,
                                        __vsf_eda_frame_state_t state,
                                        bool is_sub_call);

VSF_CAL_SECTION(".text.vsf.kernel.__vsf_eda_get_local")
/**
 * \~english
 * @brief Get the local variable storage of a frame-based eda
 * @param[in] pthis: a pointer to structure @ref vsf_eda_t
 * @return uintptr_t: address of the local storage, NULL if the task has no local storage
 *
 * @note Internal use only. Use the macro vsf_eda_get_local() instead. Only
 * available when VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is ENABLED.
 * @note Operates on the current task; must be called in the current eda/teda task's event handler context.
 * \~chinese
 * @brief 获取基于帧的 eda 的局部变量存储
 * @param[in] pthis: 指向结构体 @ref vsf_eda_t 的指针
 * @return uintptr_t: 局部变量存储的地址，如果任务没有局部变量存储返回 NULL
 *
 * @note 仅供内部使用。请使用宏 vsf_eda_get_local()。仅在
 * VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL 使能时可用。
 * @note 作用于当前任务，必须在当前 eda/teda 任务的事件处理函数上下文中调用。
 */
extern uintptr_t __vsf_eda_get_local(vsf_eda_t* pthis);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_target_set")
/**
 * \~english
 * @brief Set the target parameter of the current frame of the current task
 * @param[in] param: the target parameter to set
 * @return vsf_err_t: VSF_ERR_NONE if successful, VSF_ERR_NOT_ENOUGH_RESOURCES if
 * no frame is available
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is ENABLED.
 * @note Operates on the current task; must be called in the current eda/teda task's event handler context.
 * \~chinese
 * @brief 设置当前任务当前帧的目标参数
 * @param[in] param: 要设置的目标参数
 * @return vsf_err_t: 成功返回 VSF_ERR_NONE，如果没有可用的帧返回
 * VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL 使能时可用。
 * @note 作用于当前任务，必须在当前 eda/teda 任务的事件处理函数上下文中调用。
 */
extern vsf_err_t vsf_eda_target_set(uintptr_t param);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_target_get")
/**
 * \~english
 * @brief Get the target parameter of the current frame of the current task
 * @return uintptr_t: the target parameter, NULL if the current task uses no frame
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is ENABLED.
 * @note Operates on the current task; must be called in the current eda/teda task's event handler context.
 * \~chinese
 * @brief 获取当前任务当前帧的目标参数
 * @return uintptr_t: 目标参数，如果当前任务不使用帧返回 NULL
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL 使能时可用。
 * @note 作用于当前任务，必须在当前 eda/teda 任务的事件处理函数上下文中调用。
 */
extern uintptr_t vsf_eda_target_get(void);

#endif      // VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED

VSF_CAL_SECTION(".text.vsf.kernel.vsf_teda_start")
/**
 * \~english
 * @brief Start a teda (eda with timer support) task with the given configuration
 * and post VSF_EVT_INIT to it
 * @param[in] pthis: a pointer to structure @ref vsf_teda_t
 * @param[in] cfg: a pointer to structure @ref vsf_eda_cfg_t holding the task configuration
 * @return vsf_err_t: VSF_ERR_NONE if started successfully; VSF_ERR_NOT_ENOUGH_RESOURCES if no frame is available
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_TIMER is ENABLED.
 * @note Must be called in task (or initialization) context, NOT in interrupt context.
 * \~chinese
 * @brief 以给定的配置启动一个 teda（带定时器支持的 eda）任务，并向其发送
 * VSF_EVT_INIT 事件
 * @param[in] pthis: 指向结构体 @ref vsf_teda_t 的指针
 * @param[in] cfg: 指向保存任务配置的结构体 @ref vsf_eda_cfg_t 的指针
 * @return vsf_err_t: 启动成功返回 VSF_ERR_NONE；没有可用的帧返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_TIMER 使能时可用。
 * @note 必须在任务（或初始化）上下文中调用，禁止在中断上下文中调用。
 */
extern vsf_err_t vsf_teda_start(vsf_teda_t *pthis, vsf_eda_cfg_t *cfg);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_teda_set_timer")
/**
 * \~english
 * @brief Set a one-shot timer of the given ticks for the current teda task; the
 * task receives VSF_EVT_TIMER when the timer expires
 * @param[in] tick: timer interval in ticks, must not be 0
 * @return vsf_err_t: VSF_ERR_NONE if successful; VSF_ERR_NOT_AVAILABLE if tick
 * is 0 (tick == 0 also triggers an assertion in debug builds)
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_TIMER is ENABLED.
 * @note Operates on the current task; must be called in the current eda/teda task's event handler context.
 * @note A teda can have only ONE pending one-shot timer: setting a new timer
 * while the previous one is still pending triggers a kernel assertion. To
 * restart the timer, cancel the pending one with vsf_teda_cancel_timer() first.
 * \~chinese
 * @brief 为当前 teda 任务设置一个给定 tick 数的单次定时器；定时器到期后任务
 * 会收到 VSF_EVT_TIMER 事件
 * @param[in] tick: 定时器间隔 tick 数，不能为 0
 * @return vsf_err_t: 成功返回 VSF_ERR_NONE；tick 为 0 返回 VSF_ERR_NOT_AVAILABLE
 * （tick 为 0 时在调试版本中还会触发断言）
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_TIMER 使能时可用。
 * @note 作用于当前任务，必须在当前 eda/teda 任务的事件处理函数上下文中调用。
 * @note 一个 teda 只允许有一个挂起的单次定时器：前一个定时器未到期时再次设置
 * 会触发内核断言。若需重新设置（重启定时器），请先调用 vsf_teda_cancel_timer()
 * 取消挂起的定时器。
 */
extern vsf_err_t vsf_teda_set_timer(vsf_systimer_tick_t tick);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_teda_set_due_ex")
/**
 * \~english
 * @brief Set a one-shot timer with an absolute due tick for the given teda task
 * @param[in] this_ptr: a pointer to structure @ref vsf_teda_t
 * @param[in] due: absolute due tick count
 * @return vsf_err_t: always VSF_ERR_NONE
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_TIMER is ENABLED. The
 * task receives VSF_EVT_TIMER when the timer expires.
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * @note A teda can have only ONE pending one-shot timer: setting a new timer
 * while the previous one is still pending triggers a kernel assertion. To
 * restart the timer, cancel the pending one with vsf_teda_cancel_timer() first.
 * \~chinese
 * @brief 为给定的 teda 任务设置一个绝对到期 tick 的单次定时器
 * @param[in] this_ptr: 指向结构体 @ref vsf_teda_t 的指针
 * @param[in] due: 绝对到期 tick 计数
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_TIMER 使能时可用。定时器到期后任务
 * 会收到 VSF_EVT_TIMER 事件。
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 * @note 一个 teda 只允许有一个挂起的单次定时器：前一个定时器未到期时再次设置
 * 会触发内核断言。若需重新设置（重启定时器），请先调用 vsf_teda_cancel_timer()
 * 取消挂起的定时器。
 */
extern vsf_err_t vsf_teda_set_due_ex(vsf_teda_t *this_ptr, vsf_systimer_tick_t due);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_teda_set_timer_ex")
/**
 * \~english
 * @brief Set a one-shot timer of the given ticks for the given teda task; the
 * task receives VSF_EVT_TIMER when the timer expires
 * @param[in] pthis: a pointer to structure @ref vsf_teda_t
 * @param[in] tick: timer interval in ticks, must not be 0
 * @return vsf_err_t: VSF_ERR_NONE if successful; VSF_ERR_NOT_AVAILABLE if tick
 * is 0 (tick == 0 also triggers an assertion in debug builds)
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_TIMER is ENABLED.
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * @note A teda can have only ONE pending one-shot timer: setting a new timer
 * while the previous one is still pending triggers a kernel assertion. To
 * restart the timer, cancel the pending one with vsf_teda_cancel_timer() first.
 * \~chinese
 * @brief 为给定的 teda 任务设置一个给定 tick 数的单次定时器；定时器到期后任务
 * 会收到 VSF_EVT_TIMER 事件
 * @param[in] pthis: 指向结构体 @ref vsf_teda_t 的指针
 * @param[in] tick: 定时器间隔 tick 数，不能为 0
 * @return vsf_err_t: 成功返回 VSF_ERR_NONE；tick 为 0 返回 VSF_ERR_NOT_AVAILABLE
 * （tick 为 0 时在调试版本中还会触发断言）
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_TIMER 使能时可用。
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 * @note 一个 teda 只允许有一个挂起的单次定时器：前一个定时器未到期时再次设置
 * 会触发内核断言。若需重新设置（重启定时器），请先调用 vsf_teda_cancel_timer()
 * 取消挂起的定时器。
 */
extern vsf_err_t vsf_teda_set_timer_ex(vsf_teda_t *pthis, vsf_systimer_tick_t tick);

/**
 * \~english
 * @brief Set a one-shot timer of the given milliseconds for the current teda task
 * @param[in] ms: timer interval in milliseconds
 * @return vsf_err_t: VSF_ERR_NONE if the timer is set; VSF_ERR_NOT_AVAILABLE if the interval converts to 0 ticks
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_TIMER is ENABLED. The
 * task receives VSF_EVT_TIMER when the timer expires.
 * @note Operates on the current task; must be called in the current eda/teda task's event handler context.
 * \~chinese
 * @brief 为当前 teda 任务设置一个给定毫秒数的单次定时器
 * @param[in] ms: 定时器间隔毫秒数
 * @return vsf_err_t: 定时器设置成功返回 VSF_ERR_NONE；间隔换算为 0 tick 返回 VSF_ERR_NOT_AVAILABLE
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_TIMER 使能时可用。定时器到期后任务
 * 会收到 VSF_EVT_TIMER 事件。
 * @note 作用于当前任务，必须在当前 eda/teda 任务的事件处理函数上下文中调用。
 */
static inline vsf_err_t vsf_teda_set_timer_ms(uint_fast32_t ms)
{
    return vsf_teda_set_timer(vsf_systimer_ms_to_tick(ms));
}

/**
 * \~english
 * @brief Set a one-shot timer of the given microseconds for the current teda task
 * @param[in] us: timer interval in microseconds
 * @return vsf_err_t: VSF_ERR_NONE if the timer is set; VSF_ERR_NOT_AVAILABLE if the interval converts to 0 ticks
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_TIMER is ENABLED. The
 * task receives VSF_EVT_TIMER when the timer expires.
 * @note Operates on the current task; must be called in the current eda/teda task's event handler context.
 * \~chinese
 * @brief 为当前 teda 任务设置一个给定微秒数的单次定时器
 * @param[in] us: 定时器间隔微秒数
 * @return vsf_err_t: 定时器设置成功返回 VSF_ERR_NONE；间隔换算为 0 tick 返回 VSF_ERR_NOT_AVAILABLE
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_TIMER 使能时可用。定时器到期后任务
 * 会收到 VSF_EVT_TIMER 事件。
 * @note 作用于当前任务，必须在当前 eda/teda 任务的事件处理函数上下文中调用。
 */
static inline vsf_err_t vsf_teda_set_timer_us(uint_fast32_t us)
{
    return vsf_teda_set_timer(vsf_systimer_us_to_tick(us));
}

VSF_CAL_SECTION(".text.vsf.kernel.vsf_teda_cancel_timer")
/**
 * \~english
 * @brief Cancel the pending timer of the current teda task
 * @return vsf_err_t: always VSF_ERR_NONE
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_TIMER is ENABLED.
 * @note Operates on the current task; must be called in the current eda/teda task's event handler context.
 * \~chinese
 * @brief 取消当前 teda 任务未到期的定时器
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_TIMER 使能时可用。
 * @note 作用于当前任务，必须在当前 eda/teda 任务的事件处理函数上下文中调用。
 */
extern vsf_err_t vsf_teda_cancel_timer(void);

VSF_CAL_SECTION(".text.vsf.kernel.__vsf_teda_cancel_timer")
/**
 * \~english
 * @brief Cancel the pending timer of the given teda task
 * @param[in] pthis: a pointer to structure @ref vsf_teda_t, the current task if NULL
 * @return vsf_err_t: always VSF_ERR_NONE
 *
 * @note Internal use only. Only available when VSF_KERNEL_CFG_EDA_SUPPORT_TIMER
 * is ENABLED.
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 取消给定 teda 任务未到期的定时器
 * @param[in] pthis: 指向结构体 @ref vsf_teda_t 的指针，为 NULL 时表示当前任务
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE
 *
 * @note 仅供内部使用。仅在 VSF_KERNEL_CFG_EDA_SUPPORT_TIMER 使能时可用。
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
extern vsf_err_t __vsf_teda_cancel_timer(vsf_teda_t *pthis);

#   if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
VSF_CAL_SECTION(".text.vsf.kernel.vsf_callback_timer_init")
/**
 * \~english
 * @brief Initialize a task-independent callback timer; set the on_timer callback
 * before adding the timer
 * @param[in] timer: a pointer to structure @ref vsf_callback_timer_t
 * @return none
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER is ENABLED.
 * When the timer expires, the on_timer callback of @ref vsf_callback_timer_t is
 * invoked by the kernel task.
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 初始化一个不依赖任务的回调定时器；在添加定时器之前设置 on_timer 回调
 * @param[in] timer: 指向结构体 @ref vsf_callback_timer_t 的指针
 * @return 无
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER 使能时可用。定时器到期后，
 * 由内核任务调用 @ref vsf_callback_timer_t 的 on_timer 回调。
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
extern void vsf_callback_timer_init(vsf_callback_timer_t *timer);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_callback_timer_add_due")
/**
 * \~english
 * @brief Add a callback timer with an absolute due tick
 * @param[in] timer: a pointer to structure @ref vsf_callback_timer_t
 * @param[in] due: absolute due tick count, must not be 0
 * @return vsf_err_t: VSF_ERR_NONE if successful, VSF_ERR_FAIL if the timer is already added
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER is ENABLED.
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 以绝对到期 tick 添加一个回调定时器
 * @param[in] timer: 指向结构体 @ref vsf_callback_timer_t 的指针
 * @param[in] due: 绝对到期 tick 计数，不能为 0
 * @return vsf_err_t: 成功返回 VSF_ERR_NONE，如果定时器已被添加返回 VSF_ERR_FAIL
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER 使能时可用。
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
extern vsf_err_t vsf_callback_timer_add_due(vsf_callback_timer_t *timer, vsf_systimer_tick_t due);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_callback_timer_add")
/**
 * \~english
 * @brief Add a callback timer with a relative interval in ticks
 * @param[in] timer: a pointer to structure @ref vsf_callback_timer_t
 * @param[in] tick: timer interval in ticks
 * @return vsf_err_t: VSF_ERR_NONE if successful, VSF_ERR_FAIL if the timer is already added
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER is ENABLED.
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 以相对 tick 间隔添加一个回调定时器
 * @param[in] timer: 指向结构体 @ref vsf_callback_timer_t 的指针
 * @param[in] tick: 定时器间隔 tick 数
 * @return vsf_err_t: 成功返回 VSF_ERR_NONE，如果定时器已被添加返回 VSF_ERR_FAIL
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER 使能时可用。
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
extern vsf_err_t vsf_callback_timer_add(vsf_callback_timer_t *timer, vsf_systimer_tick_t tick);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_callback_timer_remove")
/**
 * \~english
 * @brief Remove a callback timer before it expires
 * @param[in] timer: a pointer to structure @ref vsf_callback_timer_t
 * @return vsf_err_t: always VSF_ERR_NONE
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER is ENABLED.
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 在回调定时器到期前将其移除
 * @param[in] timer: 指向结构体 @ref vsf_callback_timer_t 的指针
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER 使能时可用。
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
extern vsf_err_t vsf_callback_timer_remove(vsf_callback_timer_t *timer);

#       if VSF_CALLBACK_TIMER_CFG_SUPPORT_ISR == ENABLED
VSF_CAL_SECTION(".text.vsf.kernel.vsf_callback_timer_add_due_isr")
/**
 * \~english
 * @brief Add a callback timer with an absolute due tick from interrupt context
 * @param[in] timer: a pointer to structure @ref vsf_callback_timer_t
 * @param[in] due: absolute due tick count, must not be 0
 * @return vsf_err_t: VSF_ERR_NONE if the request is posted to the kernel task,
 * VSF_ERR_FAIL if the timer is already added, VSF_ERR_NOT_ENOUGH_RESOURCES if
 * no event node is available for the request
 *
 * @note Only available when VSF_CALLBACK_TIMER_CFG_SUPPORT_ISR is ENABLED. The
 * actual add is done by the kernel task.
 * @note May also be called in task context, but that is less efficient (the
 * request is bounced to the kernel task instead of being executed directly) —
 * prefer vsf_callback_timer_add_due() there.
 * \~chinese
 * @brief 在中断上下文里以绝对到期 tick 添加一个回调定时器
 * @param[in] timer: 指向结构体 @ref vsf_callback_timer_t 的指针
 * @param[in] due: 绝对到期 tick 计数，不能为 0
 * @return vsf_err_t: 如果请求已发送给内核任务返回 VSF_ERR_NONE，如果定时器
 * 已被添加返回 VSF_ERR_FAIL，如果没有可用的事件节点返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 仅在 VSF_CALLBACK_TIMER_CFG_SUPPORT_ISR 使能时可用。实际的添加操作
 * 由内核任务完成。
 * @note 也可以在任务上下文中调用，但效率较低（请求经内核任务跳板，而不是
 * 直接执行）——任务上下文里建议使用 vsf_callback_timer_add_due()。
 */
extern vsf_err_t vsf_callback_timer_add_due_isr(vsf_callback_timer_t *timer, vsf_systimer_tick_t due);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_callback_timer_add_isr")
/**
 * \~english
 * @brief Add a callback timer with a relative interval in ticks from interrupt context
 * @param[in] timer: a pointer to structure @ref vsf_callback_timer_t
 * @param[in] tick: timer interval in ticks
 * @return vsf_err_t: VSF_ERR_NONE if the request is posted to the kernel task,
 * VSF_ERR_FAIL if the timer is already added, VSF_ERR_NOT_ENOUGH_RESOURCES if
 * no event node is available for the request
 *
 * @note Only available when VSF_CALLBACK_TIMER_CFG_SUPPORT_ISR is ENABLED. The
 * actual add is done by the kernel task.
 * @note May also be called in task context, but that is less efficient (the
 * request is bounced to the kernel task instead of being executed directly) —
 * prefer vsf_callback_timer_add() there.
 * \~chinese
 * @brief 在中断上下文里以相对 tick 间隔添加一个回调定时器
 * @param[in] timer: 指向结构体 @ref vsf_callback_timer_t 的指针
 * @param[in] tick: 定时器间隔 tick 数
 * @return vsf_err_t: 如果请求已发送给内核任务返回 VSF_ERR_NONE，如果定时器
 * 已被添加返回 VSF_ERR_FAIL，如果没有可用的事件节点返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 仅在 VSF_CALLBACK_TIMER_CFG_SUPPORT_ISR 使能时可用。实际的添加操作
 * 由内核任务完成。
 * @note 也可以在任务上下文中调用，但效率较低（请求经内核任务跳板，而不是
 * 直接执行）——任务上下文里建议使用 vsf_callback_timer_add()。
 */
extern vsf_err_t vsf_callback_timer_add_isr(vsf_callback_timer_t *timer, vsf_systimer_tick_t tick);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_callback_timer_remove_isr")
/**
 * \~english
 * @brief Remove a callback timer from interrupt context
 * @param[in] timer: a pointer to structure @ref vsf_callback_timer_t
 * @return vsf_err_t: always VSF_ERR_NONE
 *
 * @note Only available when VSF_CALLBACK_TIMER_CFG_SUPPORT_ISR is ENABLED.
 * @note Can be called in interrupt context; unlike the other _isr APIs, it is
 * executed directly (same as vsf_callback_timer_remove()), not bounced to the
 * kernel task.
 * \~chinese
 * @brief 在中断上下文里移除一个回调定时器
 * @param[in] timer: 指向结构体 @ref vsf_callback_timer_t 的指针
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE
 *
 * @note 仅在 VSF_CALLBACK_TIMER_CFG_SUPPORT_ISR 使能时可用。
 * @note 可以在中断上下文中调用；与其他 _isr API 不同，它直接执行
 * （等同于 vsf_callback_timer_remove()），不经内核任务跳板。
 */
extern vsf_err_t vsf_callback_timer_remove_isr(vsf_callback_timer_t *timer);
#       endif

/**
 * \~english
 * @brief Add a callback timer with a relative interval in milliseconds
 * @param[in] timer: a pointer to structure @ref vsf_callback_timer_t
 * @param[in] ms: timer interval in milliseconds
 * @return vsf_err_t: VSF_ERR_NONE if successful, VSF_ERR_FAIL if the timer is already added
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER is ENABLED.
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 以相对毫秒间隔添加一个回调定时器
 * @param[in] timer: 指向结构体 @ref vsf_callback_timer_t 的指针
 * @param[in] ms: 定时器间隔毫秒数
 * @return vsf_err_t: 成功返回 VSF_ERR_NONE，如果定时器已被添加返回 VSF_ERR_FAIL
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER 使能时可用。
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
static inline vsf_err_t vsf_callback_timer_add_ms(vsf_callback_timer_t *timer, uint_fast32_t ms)
{
    return vsf_callback_timer_add(timer, vsf_systimer_ms_to_tick(ms));
}

/**
 * \~english
 * @brief Add a callback timer with a relative interval in microseconds
 * @param[in] timer: a pointer to structure @ref vsf_callback_timer_t
 * @param[in] us: timer interval in microseconds
 * @return vsf_err_t: VSF_ERR_NONE if successful, VSF_ERR_FAIL if the timer is already added
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER is ENABLED.
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 以相对微秒间隔添加一个回调定时器
 * @param[in] timer: 指向结构体 @ref vsf_callback_timer_t 的指针
 * @param[in] us: 定时器间隔微秒数
 * @return vsf_err_t: 成功返回 VSF_ERR_NONE，如果定时器已被添加返回 VSF_ERR_FAIL
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER 使能时可用。
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
static inline vsf_err_t vsf_callback_timer_add_us(vsf_callback_timer_t *timer, uint_fast32_t us)
{
    return vsf_callback_timer_add(timer, vsf_systimer_us_to_tick(us));
}
#   endif
#endif

VSF_CAL_SECTION(".text.vsf.kernel.vsf_irq_enter")
/**
 * \~english
 * @brief Notify the kernel of entering interrupt context
 * @return uintptr_t: a context value which must be passed back to vsf_irq_leave()
 * \~chinese
 * @brief 通知内核进入中断上下文
 * @return uintptr_t: 上下文值，必须传回给 vsf_irq_leave()
 */
extern uintptr_t vsf_irq_enter(void);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_irq_leave")
/**
 * \~english
 * @brief Notify the kernel of leaving interrupt context
 * @param[in] ctx: the context value returned by vsf_irq_enter()
 * @return none
 * \~chinese
 * @brief 通知内核离开中断上下文
 * @param[in] ctx: 由 vsf_irq_enter() 返回的上下文值
 * @return 无
 */
extern void vsf_irq_leave(uintptr_t ctx);

VSF_CAL_SECTION(".text.vsf.kernel.eda")
/**
 * \~english
 * @brief Post an event to an eda task
 * @param[in] pthis: a pointer to structure @ref vsf_eda_t
 * @param[in] evt: the event to post
 * @return vsf_err_t: VSF_ERR_NONE if posted; VSF_ERR_FAIL if the eda is limited and already has a pending event; VSF_ERR_NOT_ENOUGH_RESOURCES if no event node is available
 * @note Can be called in task or interrupt context.
 * \~chinese
 * @brief 向 eda 任务发送一个事件
 * @param[in] pthis: 指向结构体 @ref vsf_eda_t 的指针
 * @param[in] evt: 要发送的事件
 * @return vsf_err_t: 发送成功返回 VSF_ERR_NONE；eda 受限且已有待处理事件返回 VSF_ERR_FAIL；没有可用的事件节点返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 * @note 可以在任务或中断上下文中调用。
 */
extern vsf_err_t vsf_eda_post_evt(vsf_eda_t *pthis, vsf_evt_t evt);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_post_msg")
/**
 * \~english
 * @brief Post a pointer message to an eda task; the task receives VSF_EVT_MESSAGE
 * and retrieves the pointer with vsf_eda_get_cur_msg()
 * @param[in] pthis: a pointer to structure @ref vsf_eda_t
 * @param[in] msg: the message pointer to post, must be aligned (LSB must be 0)
 * @return vsf_err_t: VSF_ERR_NONE if posted; VSF_ERR_FAIL if the eda is limited and already has a pending event; VSF_ERR_NOT_ENOUGH_RESOURCES if no event node is available
 * @note Can be called in task or interrupt context.
 * \~chinese
 * @brief 向 eda 任务发送一个指针消息；任务会收到 VSF_EVT_MESSAGE 事件，并通过
 * vsf_eda_get_cur_msg() 获取指针
 * @param[in] pthis: 指向结构体 @ref vsf_eda_t 的指针
 * @param[in] msg: 要发送的消息指针，必须对齐（最低位必须为 0）
 * @return vsf_err_t: 发送成功返回 VSF_ERR_NONE；eda 受限且已有待处理事件返回 VSF_ERR_FAIL；没有可用的事件节点返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 * @note 可以在任务或中断上下文中调用。
 */
extern vsf_err_t vsf_eda_post_msg(vsf_eda_t *pthis, void *msg);
#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_post_evt_msg")
/**
 * \~english
 * @brief Post an event carrying a pointer message to an eda task; the pointer
 * is retrieved with vsf_eda_get_cur_msg()
 * @param[in] pthis: a pointer to structure @ref vsf_eda_t
 * @param[in] evt: the event to post
 * @param[in] msg: the message pointer carried with the event
 * @return vsf_err_t: VSF_ERR_NONE if posted; VSF_ERR_FAIL if the eda is limited and already has a pending event; VSF_ERR_NOT_ENOUGH_RESOURCES if no event node is available
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE is ENABLED.
 * @note Can be called in task or interrupt context.
 * \~chinese
 * @brief 向 eda 任务发送一个携带指针消息的事件；指针通过 vsf_eda_get_cur_msg()
 * 获取
 * @param[in] pthis: 指向结构体 @ref vsf_eda_t 的指针
 * @param[in] evt: 要发送的事件
 * @param[in] msg: 随事件携带的消息指针
 * @return vsf_err_t: 发送成功返回 VSF_ERR_NONE；eda 受限且已有待处理事件返回 VSF_ERR_FAIL；没有可用的事件节点返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE 使能时可用。
 * @note 可以在任务或中断上下文中调用。
 */
extern vsf_err_t vsf_eda_post_evt_msg(vsf_eda_t *pthis, vsf_evt_t evt, void *msg);
#endif

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
VSF_CAL_SECTION(".text.vsf.kernel.vsf_sync")
/**
 * \~english
 * @brief Initialize a sync object, the base of semaphore, mutex, trigger etc.
 * @param[in] pthis: a pointer to structure @ref vsf_sync_t
 * @param[in] cur_value: initial count, optionally OR-ed with @ref VSF_SYNC_HAS_OWNER
 * @param[in] max_value: maximum count (up to @ref VSF_SYNC_MAX), optionally OR-ed
 * with @ref VSF_SYNC_AUTO_RST or @ref VSF_SYNC_MANUAL_RST
 * @return vsf_err_t: always VSF_ERR_NONE (invalid parameters trigger an assertion)
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_SYNC is ENABLED.
 * @note Must be called in task (or initialization) context, NOT in interrupt context.
 * \~chinese
 * @brief 初始化一个同步对象，它是信号量、互斥量、触发器等的基础
 * @param[in] pthis: 指向结构体 @ref vsf_sync_t 的指针
 * @param[in] cur_value: 初始计数，可选择与 @ref VSF_SYNC_HAS_OWNER 相或
 * @param[in] max_value: 最大计数（最大为 @ref VSF_SYNC_MAX），可选择与
 * @ref VSF_SYNC_AUTO_RST 或 @ref VSF_SYNC_MANUAL_RST 相或
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE（参数非法会触发断言）
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_SYNC 使能时可用。
 * @note 必须在任务（或初始化）上下文中调用，禁止在中断上下文中调用。
 */
extern vsf_err_t vsf_eda_sync_init(vsf_sync_t *pthis, uint_fast16_t cur_value,
        uint_fast16_t max_value);

#if VSF_SYNC_CFG_SUPPORT_ISR == ENABLED
VSF_CAL_SECTION(".text.vsf.kernel.vsf_sync")
/**
 * \~english
 * @brief Increase a sync object from interrupt context
 * @param[in] pthis: a pointer to structure @ref vsf_sync_t
 * @return vsf_err_t: VSF_ERR_NONE if the request is posted to the kernel task; VSF_ERR_NOT_ENOUGH_RESOURCES if no event node is available
 *
 * @note Only available when VSF_SYNC_CFG_SUPPORT_ISR is ENABLED. The actual
 * increase is done by the kernel task.
 * @note May also be called in task context, but that is less efficient (the
 * request is bounced to the kernel task instead of being executed directly) —
 * prefer vsf_eda_sync_increase() there.
 * \~chinese
 * @brief 在中断上下文里增加一个同步对象
 * @param[in] pthis: 指向结构体 @ref vsf_sync_t 的指针
 * @return vsf_err_t: 请求已发送给内核任务返回 VSF_ERR_NONE；没有可用的事件节点返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 仅在 VSF_SYNC_CFG_SUPPORT_ISR 使能时可用。实际的增加操作由内核任务完成。
 * @note 也可以在任务上下文中调用，但效率较低（请求经内核任务跳板，而不是
 * 直接执行）——任务上下文里建议使用 vsf_eda_sync_increase()。
 */
extern vsf_err_t vsf_eda_sync_increase_isr(vsf_sync_t *pthis);
#endif

VSF_CAL_SECTION(".text.vsf.kernel.vsf_sync")
/**
 * \~english
 * @brief Increase a sync object, waking up the pending tasks if any
 * @param[in] pthis: a pointer to structure @ref vsf_sync_t
 * @return vsf_err_t: VSF_ERR_NONE if successful, VSF_ERR_OVERRUN if the count has
 * reached the maximum
 *
 * @note A pending task is woken with VSF_EVT_SYNC.
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 增加一个同步对象，如果有任务在等待则唤醒等待中的任务
 * @param[in] pthis: 指向结构体 @ref vsf_sync_t 的指针
 * @return vsf_err_t: 成功返回 VSF_ERR_NONE，如果计数已达到最大值返回 VSF_ERR_OVERRUN
 *
 * @note 等待中的任务会收到 VSF_EVT_SYNC 事件被唤醒。
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
extern vsf_err_t vsf_eda_sync_increase(vsf_sync_t *pthis);
VSF_CAL_SECTION(".text.vsf.kernel.vsf_sync")
/**
 * \~english
 * @brief Increase a sync object on behalf of the given eda (e.g. the owner of a mutex)
 * @param[in] pthis: a pointer to structure @ref vsf_sync_t
 * @param[in] eda: a pointer to structure @ref vsf_eda_t, the current task if NULL
 * @return vsf_err_t: VSF_ERR_NONE if successful, VSF_ERR_OVERRUN if the count has
 * reached the maximum
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 代表给定的 eda 增加一个同步对象（例如互斥量的持有者）
 * @param[in] pthis: 指向结构体 @ref vsf_sync_t 的指针
 * @param[in] eda: 指向结构体 @ref vsf_eda_t 的指针，为 NULL 时表示当前任务
 * @return vsf_err_t: 成功返回 VSF_ERR_NONE，如果计数已达到最大值返回 VSF_ERR_OVERRUN
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
extern vsf_err_t vsf_eda_sync_increase_ex(vsf_sync_t *pthis, vsf_eda_t *eda);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_sync")
/**
 * \~english
 * @brief Increase a sync object with an explicit manual-reset flag
 * @param[in] pthis: a pointer to structure @ref vsf_sync_t
 * @param[in] eda: a pointer to structure @ref vsf_eda_t, the current task if NULL
 * @param[in] manual: true to keep the count set (manual-reset behavior for this increase)
 * @return vsf_err_t: VSF_ERR_NONE if successful, VSF_ERR_OVERRUN if the count has
 * reached the maximum
 *
 * @note Internal use only.
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 以显式手动复位标志增加一个同步对象
 * @param[in] pthis: 指向结构体 @ref vsf_sync_t 的指针
 * @param[in] eda: 指向结构体 @ref vsf_eda_t 的指针，为 NULL 时表示当前任务
 * @param[in] manual: 为 true 时保持计数置位（本次增加使用手动复位行为）
 * @return vsf_err_t: 成功返回 VSF_ERR_NONE，如果计数已达到最大值返回 VSF_ERR_OVERRUN
 *
 * @note 仅供内部使用。
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
extern vsf_err_t __vsf_eda_sync_increase_ex(vsf_sync_t *pthis, vsf_eda_t *eda, bool manual);
VSF_CAL_SECTION(".text.vsf.kernel.vsf_sync")
/**
 * \~english
 * @brief Decrease a sync object with an explicit manual-reset flag
 * @param[in] pthis: a pointer to structure @ref vsf_sync_t
 * @param[in] timeout: timeout in ticks; negative waits forever, 0 is a non-blocking
 * try, a positive value waits up to the given ticks (requires timer support)
 * @param[in] eda: a pointer to structure @ref vsf_eda_t, the current task if NULL
 * @param[in] manual: true to keep the count set when the resource is obtained
 * @return vsf_err_t: VSF_ERR_NONE if the resource is obtained immediately;
 * VSF_ERR_NOT_READY if the caller pends or the try failed
 *
 * @note Internal use only.
 * @note Must be called in an eda task context (i.e. inside an event handler); must NOT be called in interrupt context.
 * \~chinese
 * @brief 以显式手动复位标志减少一个同步对象
 * @param[in] pthis: 指向结构体 @ref vsf_sync_t 的指针
 * @param[in] timeout: 超时 tick 数；负数永远等待，0 为不等待的尝试，正数最多
 * 等待对应的 tick 数（需要定时器支持）
 * @param[in] eda: 指向结构体 @ref vsf_eda_t 的指针，为 NULL 时表示当前任务
 * @param[in] manual: 为 true 时获得资源后保持计数置位
 * @return vsf_err_t: 如果立即获得资源返回 VSF_ERR_NONE；如果调用者进入等待
 * 或者尝试失败返回 VSF_ERR_NOT_READY
 *
 * @note 仅供内部使用。
 * @note 必须在 eda 任务上下文（事件处理函数）中调用，禁止在中断上下文中调用。
 */
extern vsf_err_t __vsf_eda_sync_decrease_ex(vsf_sync_t *pthis, vsf_timeout_tick_t timeout, vsf_eda_t *eda, bool manual);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_sync")
/**
 * \~english
 * @brief Force the count of a sync object to 0
 * @param[in] pthis: a pointer to structure @ref vsf_sync_t
 * @return none
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 强制将同步对象的计数清零
 * @param[in] pthis: 指向结构体 @ref vsf_sync_t 的指针
 * @return 无
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
extern void vsf_eda_sync_force_reset(vsf_sync_t *pthis);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_sync")
/**
 * \~english
 * @brief Decrease a sync object, pending if the resource is not available
 * @param[in] pthis: a pointer to structure @ref vsf_sync_t
 * @param[in] timeout: timeout in ticks; negative waits forever, 0 is a non-blocking
 * try, a positive value waits up to the given ticks (requires timer support, i.e.
 * the caller must be a teda task)
 * @return vsf_err_t: VSF_ERR_NONE if the resource is obtained immediately;
 * VSF_ERR_NOT_READY if the caller pends or the try failed
 *
 * @note If VSF_ERR_NOT_READY is returned with a non-zero timeout, the caller is
 * woken later with VSF_EVT_SYNC, VSF_EVT_TIMER (timeout) or VSF_EVT_SYNC_CANCEL;
 * call vsf_eda_sync_get_reason() to retrieve the result.
 * @note Must be called in an eda task context (i.e. inside an event handler); must NOT be called in interrupt context.
 * \~chinese
 * @brief 减少一个同步对象，如果资源不可用则进入等待
 * @param[in] pthis: 指向结构体 @ref vsf_sync_t 的指针
 * @param[in] timeout: 超时 tick 数；负数永远等待，0 为不等待的尝试，正数最多
 * 等待对应的 tick 数（需要定时器支持，即调用者必须是 teda 任务）
 * @return vsf_err_t: 如果立即获得资源返回 VSF_ERR_NONE；如果调用者进入等待
 * 或者尝试失败返回 VSF_ERR_NOT_READY
 *
 * @note 如果以非零超时返回 VSF_ERR_NOT_READY，调用者之后会收到 VSF_EVT_SYNC、
 * VSF_EVT_TIMER（超时）或 VSF_EVT_SYNC_CANCEL 事件被唤醒；调用
 * vsf_eda_sync_get_reason() 获取结果。
 * @note 必须在 eda 任务上下文（事件处理函数）中调用，禁止在中断上下文中调用。
 */
extern vsf_err_t vsf_eda_sync_decrease(vsf_sync_t *pthis, vsf_timeout_tick_t timeout);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_sync")
/**
 * \~english
 * @brief Decrease a sync object on behalf of the given eda
 * @param[in] pthis: a pointer to structure @ref vsf_sync_t
 * @param[in] timeout: timeout in ticks; negative waits forever, 0 is a non-blocking
 * try, a positive value waits up to the given ticks (requires timer support)
 * @param[in] eda: a pointer to structure @ref vsf_eda_t, the current task if NULL
 * @return vsf_err_t: VSF_ERR_NONE if the resource is obtained immediately;
 * VSF_ERR_NOT_READY if the caller pends or the try failed
 * @note Must be called in an eda task context (i.e. inside an event handler); must NOT be called in interrupt context.
 * \~chinese
 * @brief 代表给定的 eda 减少一个同步对象
 * @param[in] pthis: 指向结构体 @ref vsf_sync_t 的指针
 * @param[in] timeout: 超时 tick 数；负数永远等待，0 为不等待的尝试，正数最多
 * 等待对应的 tick 数（需要定时器支持）
 * @param[in] eda: 指向结构体 @ref vsf_eda_t 的指针，为 NULL 时表示当前任务
 * @return vsf_err_t: 如果立即获得资源返回 VSF_ERR_NONE；如果调用者进入等待
 * 或者尝试失败返回 VSF_ERR_NOT_READY
 * @note 必须在 eda 任务上下文（事件处理函数）中调用，禁止在中断上下文中调用。
 */
extern vsf_err_t vsf_eda_sync_decrease_ex(vsf_sync_t *pthis, vsf_timeout_tick_t timeout, vsf_eda_t *eda);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_sync_cancel")
/**
 * \~english
 * @brief Cancel all pending tasks on a sync object; they are woken with
 * VSF_EVT_SYNC_CANCEL
 * @param[in] pthis: a pointer to structure @ref vsf_sync_t
 * @return none
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 取消同步对象上所有等待中的任务；它们会收到 VSF_EVT_SYNC_CANCEL 事件
 * 被唤醒
 * @param[in] pthis: 指向结构体 @ref vsf_sync_t 的指针
 * @return 无
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
extern void vsf_eda_sync_cancel(vsf_sync_t *pthis);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_sync_get_reason")
/**
 * \~english
 * @brief Retrieve the result of a sync operation from the wakeup event
 * @param[in] pthis: a pointer to structure @ref vsf_sync_t
 * @param[in] evt: the wakeup event (VSF_EVT_SYNC, VSF_EVT_TIMER or VSF_EVT_SYNC_CANCEL)
 * @return vsf_sync_reason_t: VSF_SYNC_GET if the resource is obtained,
 * VSF_SYNC_TIMEOUT on timeout, VSF_SYNC_CANCEL if cancelled, VSF_SYNC_PENDING if
 * the sync was obtained before the timeout event and the wait should continue
 * @note Must be called in an eda task context (i.e. inside an event handler); must NOT be called in interrupt context.
 * \~chinese
 * @brief 根据唤醒事件获取同步操作的结果
 * @param[in] pthis: 指向结构体 @ref vsf_sync_t 的指针
 * @param[in] evt: 唤醒事件（VSF_EVT_SYNC、VSF_EVT_TIMER 或 VSF_EVT_SYNC_CANCEL）
 * @return vsf_sync_reason_t: 已获得资源返回 VSF_SYNC_GET，超时返回
 * VSF_SYNC_TIMEOUT，被取消返回 VSF_SYNC_CANCEL，如果超时事件到达前已获得同步
 * 且需要继续等待返回 VSF_SYNC_PENDING
 * @note 必须在 eda 任务上下文（事件处理函数）中调用，禁止在中断上下文中调用。
 */
extern vsf_sync_reason_t vsf_eda_sync_get_reason(vsf_sync_t *pthis, vsf_evt_t evt);

#if VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT == ENABLED
VSF_CAL_SECTION(".data.vsf.kernel.vsf_eda_bmpevt_adapter_sync_op")
/**
 * \~english
 * @brief Adapter operation table @ref vsf_bmpevt_adapter_op_t used to bind a
 * sync object to a bitmap event
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT is ENABLED.
 * \~chinese
 * @brief 用于将同步对象绑定到位图事件的适配器操作表 @ref vsf_bmpevt_adapter_op_t
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT 使能时可用。
 */
extern const vsf_bmpevt_adapter_op_t vsf_eda_bmpevt_adapter_sync_op;

VSF_CAL_SECTION(".data.vsf.kernel.vsf_eda_bmpevt_adapter_bmpevt_op")
/**
 * \~english
 * @brief Adapter operation table @ref vsf_bmpevt_adapter_op_t used to bind
 * another bitmap event to a bitmap event
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT is ENABLED.
 * \~chinese
 * @brief 用于将另一个位图事件绑定到位图事件的适配器操作表
 * @ref vsf_bmpevt_adapter_op_t
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT 使能时可用。
 */
extern const vsf_bmpevt_adapter_op_t vsf_eda_bmpevt_adapter_bmpevt_op;

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_bmpevt_init")
/**
 * \~english
 * @brief Initialize a bitmap event, initializing the bound adapters as well
 * @param[in] pthis: a pointer to structure @ref vsf_bmpevt_t
 * @param[in] adapter_count: number of adapters bound in the adapters member, up to 32
 * @return vsf_err_t: always VSF_ERR_NONE
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT is ENABLED.
 * @note Must be called in task (or initialization) context, NOT in interrupt context.
 * \~chinese
 * @brief 初始化一个位图事件，同时初始化绑定的适配器
 * @param[in] pthis: 指向结构体 @ref vsf_bmpevt_t 的指针
 * @param[in] adapter_count: adapters 成员中绑定的适配器数量，最多 32 个
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT 使能时可用。
 * @note 必须在任务（或初始化）上下文中调用，禁止在中断上下文中调用。
 */
extern vsf_err_t vsf_eda_bmpevt_init(vsf_bmpevt_t *pthis, uint_fast8_t adapter_count);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_bmpevt_set")
/**
 * \~english
 * @brief Set event bits of a bitmap event, polling the pending tasks
 * @param[in] pthis: a pointer to structure @ref vsf_bmpevt_t
 * @param[in] mask: bitmask of the event bits to set
 * @return vsf_err_t: always VSF_ERR_NONE
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT is ENABLED. A
 * matching pending task is woken with VSF_EVT_SYNC_POLL.
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 设置位图事件的事件位，并轮询等待中的任务
 * @param[in] pthis: 指向结构体 @ref vsf_bmpevt_t 的指针
 * @param[in] mask: 要设置的事件位掩码
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT 使能时可用。匹配的等待任务
 * 会收到 VSF_EVT_SYNC_POLL 事件被唤醒。
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
extern vsf_err_t vsf_eda_bmpevt_set(vsf_bmpevt_t *pthis, uint_fast32_t mask);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_bmpevt_reset")
/**
 * \~english
 * @brief Reset (clear) event bits of a bitmap event, resetting the bound adapters
 * whose bits are cleared
 * @param[in] pthis: a pointer to structure @ref vsf_bmpevt_t
 * @param[in] mask: bitmask of the event bits to clear
 * @return vsf_err_t: always VSF_ERR_NONE
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT is ENABLED.
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 复位（清除）位图事件的事件位，并复位被清除位所绑定的适配器
 * @param[in] pthis: 指向结构体 @ref vsf_bmpevt_t 的指针
 * @param[in] mask: 要清除的事件位掩码
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT 使能时可用。
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
extern vsf_err_t vsf_eda_bmpevt_reset(vsf_bmpevt_t *pthis, uint_fast32_t mask);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_bmpevt_cancel")
/**
 * \~english
 * @brief Cancel the pending tasks on a bitmap event whose mask intersects the
 * given mask; they are woken with VSF_EVT_SYNC_CANCEL
 * @param[in] pthis: a pointer to structure @ref vsf_bmpevt_t
 * @param[in] mask: bitmask of the event bits to cancel
 * @return vsf_err_t: always VSF_ERR_NONE
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT is ENABLED.
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 取消位图事件上掩码与给定掩码相交的等待任务；它们会收到
 * VSF_EVT_SYNC_CANCEL 事件被唤醒
 * @param[in] pthis: 指向结构体 @ref vsf_bmpevt_t 的指针
 * @param[in] mask: 要取消的事件位掩码
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT 使能时可用。
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
extern vsf_err_t vsf_eda_bmpevt_cancel(vsf_bmpevt_t *pthis, uint_fast32_t mask);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_bmpevt_pend")
/**
 * \~english
 * @brief Pend on a bitmap event until the masked bits are set (in OR or AND mode
 * according to the pender configuration)
 * @param[in] pthis: a pointer to structure @ref vsf_bmpevt_t
 * @param[in] pender: a pointer to structure @ref vsf_bmpevt_pender_t holding the
 * mask and the match mode (@ref VSF_BMPEVT_OR or @ref VSF_BMPEVT_AND)
 * @param[in] timeout: timeout in ticks; negative waits forever, 0 polls once
 * without pending, a positive value waits up to the given ticks (requires timer
 * support, i.e. the caller must be a teda task)
 * @return vsf_err_t: VSF_ERR_NONE if the event bits match immediately;
 * VSF_ERR_NOT_READY if the caller pends or the poll failed; VSF_ERR_FAIL if the
 * masked bits are cancelled
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT is ENABLED. A
 * pending caller is woken with VSF_EVT_SYNC_POLL, VSF_EVT_TIMER (timeout) or
 * VSF_EVT_SYNC_CANCEL, and must call vsf_eda_bmpevt_poll() to retrieve the result.
 * @note Must be called in an eda task context (i.e. inside an event handler); must NOT be called in interrupt context.
 * \~chinese
 * @brief 等待位图事件，直到掩码指定的位被设置（根据等待者配置按或/与模式匹配）
 * @param[in] pthis: 指向结构体 @ref vsf_bmpevt_t 的指针
 * @param[in] pender: 指向结构体 @ref vsf_bmpevt_pender_t 的指针，保存掩码和
 * 匹配模式（@ref VSF_BMPEVT_OR 或 @ref VSF_BMPEVT_AND）
 * @param[in] timeout: 超时 tick 数；负数永远等待，0 为只轮询一次不进入等待，
 * 正数最多等待对应的 tick 数（需要定时器支持，即调用者必须是 teda 任务）
 * @return vsf_err_t: 如果事件位立即匹配返回 VSF_ERR_NONE；如果调用者进入等待
 * 或者轮询失败返回 VSF_ERR_NOT_READY；如果掩码位被取消返回 VSF_ERR_FAIL
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT 使能时可用。等待中的调用者
 * 会收到 VSF_EVT_SYNC_POLL、VSF_EVT_TIMER（超时）或 VSF_EVT_SYNC_CANCEL 事件
 * 被唤醒，并必须调用 vsf_eda_bmpevt_poll() 获取结果。
 * @note 必须在 eda 任务上下文（事件处理函数）中调用，禁止在中断上下文中调用。
 */
extern vsf_err_t vsf_eda_bmpevt_pend(vsf_bmpevt_t *pthis, vsf_bmpevt_pender_t *pender, vsf_timeout_tick_t timeout);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_bmpevt_poll")
/**
 * \~english
 * @brief Retrieve the result of a bitmap event pend from the wakeup event
 * @param[in] pthis: a pointer to structure @ref vsf_bmpevt_t
 * @param[in] pender: a pointer to structure @ref vsf_bmpevt_pender_t used in
 * vsf_eda_bmpevt_pend()
 * @param[in] evt: the wakeup event (VSF_EVT_SYNC_POLL, VSF_EVT_TIMER or
 * VSF_EVT_SYNC_CANCEL)
 * @return vsf_sync_reason_t: VSF_SYNC_GET if the event bits match, VSF_SYNC_TIMEOUT
 * on timeout, VSF_SYNC_CANCEL if cancelled, VSF_SYNC_PENDING if the wait should continue
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT is ENABLED.
 * @note Must be called in an eda task context (i.e. inside an event handler); must NOT be called in interrupt context.
 * \~chinese
 * @brief 根据唤醒事件获取位图事件等待的结果
 * @param[in] pthis: 指向结构体 @ref vsf_bmpevt_t 的指针
 * @param[in] pender: 指向在 vsf_eda_bmpevt_pend() 中使用的结构体
 * @ref vsf_bmpevt_pender_t 的指针
 * @param[in] evt: 唤醒事件（VSF_EVT_SYNC_POLL、VSF_EVT_TIMER 或
 * VSF_EVT_SYNC_CANCEL）
 * @return vsf_sync_reason_t: 事件位匹配返回 VSF_SYNC_GET，超时返回
 * VSF_SYNC_TIMEOUT，被取消返回 VSF_SYNC_CANCEL，需要继续等待返回 VSF_SYNC_PENDING
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT 使能时可用。
 * @note 必须在 eda 任务上下文（事件处理函数）中调用，禁止在中断上下文中调用。
 */
extern vsf_sync_reason_t vsf_eda_bmpevt_poll(vsf_bmpevt_t *pthis, vsf_bmpevt_pender_t *pender, vsf_evt_t evt);
#endif

#if VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE == ENABLED
VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_queue_init")
/**
 * \~english
 * @brief Initialize an OS-aware queue whose enqueue/dequeue operations are
 * provided by the user in the op member (@ref vsf_eda_queue_op_t)
 * @param[in] pthis: a pointer to structure @ref vsf_eda_queue_t
 * @param[in] max: maximum number of nodes the queue can hold, must be greater than 0
 * @return vsf_err_t: always VSF_ERR_NONE (invalid parameters trigger an assertion)
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE is ENABLED. The op
 * member must be set before initialization (e.g. with the slist queue operations
 * in vsf_eda_slist_queue.h).
 * @note Must be called in task (or initialization) context, NOT in interrupt context.
 * \~chinese
 * @brief 初始化一个操作系统感知的队列，其入队/出队操作由用户在 op 成员
 * （@ref vsf_eda_queue_op_t）中提供
 * @param[in] pthis: 指向结构体 @ref vsf_eda_queue_t 的指针
 * @param[in] max: 队列可容纳的最大节点数，必须大于 0
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE（参数非法会触发断言）
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE 使能时可用。op 成员必须在
 * 初始化前设置（例如使用 vsf_eda_slist_queue.h 中的 slist 队列操作）。
 * @note 必须在任务（或初始化）上下文中调用，禁止在中断上下文中调用。
 */
extern vsf_err_t vsf_eda_queue_init(vsf_eda_queue_t *pthis, uint_fast16_t max);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_queue_send")
/**
 * \~english
 * @brief Send a node to a queue, pending while the queue is full
 * @param[in] pthis: a pointer to structure @ref vsf_eda_queue_t
 * @param[in] node: the node to enqueue
 * @param[in] timeout: timeout in ticks; negative waits forever, 0 is a non-blocking
 * try, a positive value waits up to the given ticks (requires timer support, i.e.
 * the caller must be a teda task)
 * @return vsf_err_t: VSF_ERR_NONE if the node is enqueued immediately;
 * VSF_ERR_NOT_READY if the queue is full and the caller pends or the try failed
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE is ENABLED. If
 * VSF_ERR_NOT_READY is returned with a non-zero timeout, the caller is woken later
 * with VSF_EVT_SYNC, VSF_EVT_TIMER (timeout) or VSF_EVT_SYNC_CANCEL, and must call
 * vsf_eda_queue_send_get_reason() to retry and retrieve the result.
 * @note Must be called in an eda task context (i.e. inside an event handler); must NOT be called in interrupt context.
 * \~chinese
 * @brief 向队列发送一个节点，队列满时进入等待
 * @param[in] pthis: 指向结构体 @ref vsf_eda_queue_t 的指针
 * @param[in] node: 要入队的节点
 * @param[in] timeout: 超时 tick 数；负数永远等待，0 为不等待的尝试，正数最多
 * 等待对应的 tick 数（需要定时器支持，即调用者必须是 teda 任务）
 * @return vsf_err_t: 如果节点立即入队返回 VSF_ERR_NONE；如果队列满，调用者
 * 进入等待或者尝试失败返回 VSF_ERR_NOT_READY
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE 使能时可用。如果以非零超时返回
 * VSF_ERR_NOT_READY，调用者之后会收到 VSF_EVT_SYNC、VSF_EVT_TIMER（超时）或
 * VSF_EVT_SYNC_CANCEL 事件被唤醒，并必须调用 vsf_eda_queue_send_get_reason()
 * 重试并获取结果。
 * @note 必须在 eda 任务上下文（事件处理函数）中调用，禁止在中断上下文中调用。
 */
extern vsf_err_t vsf_eda_queue_send(vsf_eda_queue_t *pthis, void *node, vsf_timeout_tick_t timeout);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_queue_send_ex")
/**
 * \~english
 * @brief Send a node to a queue on behalf of the given eda
 * @param[in] pthis: a pointer to structure @ref vsf_eda_queue_t
 * @param[in] node: the node to enqueue
 * @param[in] timeout: timeout in ticks; negative waits forever, 0 is a non-blocking
 * try, a positive value waits up to the given ticks (requires timer support)
 * @param[in] eda: a pointer to structure @ref vsf_eda_t, the current task if NULL
 * @return vsf_err_t: VSF_ERR_NONE if the node is enqueued immediately;
 * VSF_ERR_NOT_READY if the queue is full and the caller pends or the try failed
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE is ENABLED.
 * @note Must be called in an eda task context (i.e. inside an event handler); must NOT be called in interrupt context.
 * \~chinese
 * @brief 代表给定的 eda 向队列发送一个节点
 * @param[in] pthis: 指向结构体 @ref vsf_eda_queue_t 的指针
 * @param[in] node: 要入队的节点
 * @param[in] timeout: 超时 tick 数；负数永远等待，0 为不等待的尝试，正数最多
 * 等待对应的 tick 数（需要定时器支持）
 * @param[in] eda: 指向结构体 @ref vsf_eda_t 的指针，为 NULL 时表示当前任务
 * @return vsf_err_t: 如果节点立即入队返回 VSF_ERR_NONE；如果队列满，调用者
 * 进入等待或者尝试失败返回 VSF_ERR_NOT_READY
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE 使能时可用。
 * @note 必须在 eda 任务上下文（事件处理函数）中调用，禁止在中断上下文中调用。
 */
extern vsf_err_t vsf_eda_queue_send_ex(vsf_eda_queue_t *pthis, void *node, vsf_timeout_tick_t timeout, vsf_eda_t *eda);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_queue_send_get_reason")
/**
 * \~english
 * @brief Retrieve the result of a queue send from the wakeup event, retrying the
 * enqueue if the resource is obtained
 * @param[in] pthis: a pointer to structure @ref vsf_eda_queue_t
 * @param[in] evt: the wakeup event (VSF_EVT_SYNC, VSF_EVT_TIMER or VSF_EVT_SYNC_CANCEL)
 * @param[in] node: the node passed to vsf_eda_queue_send()
 * @return vsf_sync_reason_t: VSF_SYNC_GET if the node is enqueued, VSF_SYNC_TIMEOUT
 * on timeout, VSF_SYNC_CANCEL if cancelled, VSF_SYNC_PENDING if the wait should continue
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE is ENABLED.
 * @note Must be called in an eda task context (i.e. inside an event handler); must NOT be called in interrupt context.
 * \~chinese
 * @brief 根据唤醒事件获取队列发送的结果，如果获得资源则重试入队
 * @param[in] pthis: 指向结构体 @ref vsf_eda_queue_t 的指针
 * @param[in] evt: 唤醒事件（VSF_EVT_SYNC、VSF_EVT_TIMER 或 VSF_EVT_SYNC_CANCEL）
 * @param[in] node: 传给 vsf_eda_queue_send() 的节点
 * @return vsf_sync_reason_t: 节点已入队返回 VSF_SYNC_GET，超时返回
 * VSF_SYNC_TIMEOUT，被取消返回 VSF_SYNC_CANCEL，需要继续等待返回 VSF_SYNC_PENDING
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE 使能时可用。
 * @note 必须在 eda 任务上下文（事件处理函数）中调用，禁止在中断上下文中调用。
 */
extern vsf_sync_reason_t vsf_eda_queue_send_get_reason(vsf_eda_queue_t *pthis, vsf_evt_t evt, void *node);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_queue_recv")
/**
 * \~english
 * @brief Receive a node from a queue, pending while the queue is empty
 * @param[in] pthis: a pointer to structure @ref vsf_eda_queue_t
 * @param[out] node: a pointer to receive the dequeued node
 * @param[in] timeout: timeout in ticks; negative waits forever, 0 is a non-blocking
 * try, a positive value waits up to the given ticks (requires timer support, i.e.
 * the caller must be a teda task)
 * @return vsf_err_t: VSF_ERR_NONE if a node is dequeued immediately;
 * VSF_ERR_NOT_READY if the queue is empty and the caller pends or the try failed
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE is ENABLED. If
 * VSF_ERR_NOT_READY is returned with a non-zero timeout, the caller is woken later
 * with VSF_EVT_SYNC, VSF_EVT_TIMER (timeout) or VSF_EVT_SYNC_CANCEL, and must call
 * vsf_eda_queue_recv_get_reason() to retry and retrieve the result.
 * @note Must be called in an eda task context (i.e. inside an event handler); must NOT be called in interrupt context.
 * \~chinese
 * @brief 从队列接收一个节点，队列空时进入等待
 * @param[in] pthis: 指向结构体 @ref vsf_eda_queue_t 的指针
 * @param[out] node: 用于接收出队节点的指针
 * @param[in] timeout: 超时 tick 数；负数永远等待，0 为不等待的尝试，正数最多
 * 等待对应的 tick 数（需要定时器支持，即调用者必须是 teda 任务）
 * @return vsf_err_t: 如果节点立即出队返回 VSF_ERR_NONE；如果队列空，调用者
 * 进入等待或者尝试失败返回 VSF_ERR_NOT_READY
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE 使能时可用。如果以非零超时返回
 * VSF_ERR_NOT_READY，调用者之后会收到 VSF_EVT_SYNC、VSF_EVT_TIMER（超时）或
 * VSF_EVT_SYNC_CANCEL 事件被唤醒，并必须调用 vsf_eda_queue_recv_get_reason()
 * 重试并获取结果。
 * @note 必须在 eda 任务上下文（事件处理函数）中调用，禁止在中断上下文中调用。
 */
extern vsf_err_t vsf_eda_queue_recv(vsf_eda_queue_t *pthis, void **node, vsf_timeout_tick_t timeout);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_queue_recv_ex")
/**
 * \~english
 * @brief Receive a node from a queue on behalf of the given eda
 * @param[in] pthis: a pointer to structure @ref vsf_eda_queue_t
 * @param[out] node: a pointer to receive the dequeued node
 * @param[in] timeout: timeout in ticks; negative waits forever, 0 is a non-blocking
 * try, a positive value waits up to the given ticks (requires timer support)
 * @param[in] eda: a pointer to structure @ref vsf_eda_t, the current task if NULL
 * @return vsf_err_t: VSF_ERR_NONE if a node is dequeued immediately;
 * VSF_ERR_NOT_READY if the queue is empty and the caller pends or the try failed
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE is ENABLED.
 * @note Must be called in an eda task context (i.e. inside an event handler); must NOT be called in interrupt context.
 * \~chinese
 * @brief 代表给定的 eda 从队列接收一个节点
 * @param[in] pthis: 指向结构体 @ref vsf_eda_queue_t 的指针
 * @param[out] node: 用于接收出队节点的指针
 * @param[in] timeout: 超时 tick 数；负数永远等待，0 为不等待的尝试，正数最多
 * 等待对应的 tick 数（需要定时器支持）
 * @param[in] eda: 指向结构体 @ref vsf_eda_t 的指针，为 NULL 时表示当前任务
 * @return vsf_err_t: 如果节点立即出队返回 VSF_ERR_NONE；如果队列空，调用者
 * 进入等待或者尝试失败返回 VSF_ERR_NOT_READY
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE 使能时可用。
 * @note 必须在 eda 任务上下文（事件处理函数）中调用，禁止在中断上下文中调用。
 */
extern vsf_err_t vsf_eda_queue_recv_ex(vsf_eda_queue_t *pthis, void **node, vsf_timeout_tick_t timeout, vsf_eda_t *eda);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_queue_recv_get_reason")
/**
 * \~english
 * @brief Retrieve the result of a queue receive from the wakeup event, retrying
 * the dequeue if the resource is obtained
 * @param[in] pthis: a pointer to structure @ref vsf_eda_queue_t
 * @param[in] evt: the wakeup event (VSF_EVT_SYNC, VSF_EVT_TIMER or VSF_EVT_SYNC_CANCEL)
 * @param[out] node: a pointer to receive the dequeued node
 * @return vsf_sync_reason_t: VSF_SYNC_GET if a node is dequeued, VSF_SYNC_TIMEOUT
 * on timeout, VSF_SYNC_CANCEL if cancelled, VSF_SYNC_PENDING if the wait should continue
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE is ENABLED.
 * @note Must be called in an eda task context (i.e. inside an event handler); must NOT be called in interrupt context.
 * \~chinese
 * @brief 根据唤醒事件获取队列接收的结果，如果获得资源则重试出队
 * @param[in] pthis: 指向结构体 @ref vsf_eda_queue_t 的指针
 * @param[in] evt: 唤醒事件（VSF_EVT_SYNC、VSF_EVT_TIMER 或 VSF_EVT_SYNC_CANCEL）
 * @param[out] node: 用于接收出队节点的指针
 * @return vsf_sync_reason_t: 节点已出队返回 VSF_SYNC_GET，超时返回
 * VSF_SYNC_TIMEOUT，被取消返回 VSF_SYNC_CANCEL，需要继续等待返回 VSF_SYNC_PENDING
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE 使能时可用。
 * @note 必须在 eda 任务上下文（事件处理函数）中调用，禁止在中断上下文中调用。
 */
extern vsf_sync_reason_t vsf_eda_queue_recv_get_reason(vsf_eda_queue_t *pthis, vsf_evt_t evt, void **node);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_queue_get_cnt")
/**
 * \~english
 * @brief Get the number of nodes currently held in a queue
 * @param[in] pthis: a pointer to structure @ref vsf_eda_queue_t
 * @return uint_fast16_t: number of nodes in the queue
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE is ENABLED.
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 获取队列中当前的节点数量
 * @param[in] pthis: 指向结构体 @ref vsf_eda_queue_t 的指针
 * @return uint_fast16_t: 队列中的节点数量
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE 使能时可用。
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
extern uint_fast16_t vsf_eda_queue_get_cnt(vsf_eda_queue_t *pthis);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_queue_cancel")
/**
 * \~english
 * @brief Cancel all pending senders and the pending receiver on a queue; they
 * are woken with VSF_EVT_SYNC_CANCEL
 * @param[in] pthis: a pointer to structure @ref vsf_eda_queue_t
 * @return none
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE is ENABLED.
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 取消队列上所有等待中的发送者和等待中的接收者；它们会收到
 * VSF_EVT_SYNC_CANCEL 事件被唤醒
 * @param[in] pthis: 指向结构体 @ref vsf_eda_queue_t 的指针
 * @return 无
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE 使能时可用。
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
extern void vsf_eda_queue_cancel(vsf_eda_queue_t *pthis);

#   if VSF_EDA_QUEUE_CFG_SUPPORT_ISR == ENABLED
VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_queue_send_isr")
/**
 * \~english
 * @brief Send a node to a queue from interrupt context
 * @param[in] pthis: a pointer to structure @ref vsf_eda_queue_t
 * @param[in] node: the node to enqueue
 * @return vsf_err_t: VSF_ERR_NONE if the node is enqueued, VSF_ERR_NOT_READY if
 * the queue is full
 *
 * @note Only available when VSF_EDA_QUEUE_CFG_SUPPORT_ISR is ENABLED. The
 * receiver notification is done by the kernel task.
 * @note Intended for interrupt context: the node is enqueued directly, and the
 * receiver wakeup is bounced to the kernel task. May also be called in task
 * context, but vsf_eda_queue_send() is more efficient there.
 * \~chinese
 * @brief 在中断上下文里向队列发送一个节点
 * @param[in] pthis: 指向结构体 @ref vsf_eda_queue_t 的指针
 * @param[in] node: 要入队的节点
 * @return vsf_err_t: 如果节点已入队返回 VSF_ERR_NONE，如果队列满返回
 * VSF_ERR_NOT_READY
 *
 * @note 仅在 VSF_EDA_QUEUE_CFG_SUPPORT_ISR 使能时可用。接收者通知由内核任务完成。
 * @note 设计用于中断上下文：节点直接入队，接收者的唤醒通知经内核任务跳板
 * 完成。也可以在任务上下文中调用，但任务里使用 vsf_eda_queue_send()
 * 效率更高。
 */
extern vsf_err_t vsf_eda_queue_send_isr(vsf_eda_queue_t *pthis, void *node);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_eda_queue_recv_isr")
/**
 * \~english
 * @brief Receive a node from a queue from interrupt context
 * @param[in] pthis: a pointer to structure @ref vsf_eda_queue_t
 * @param[out] node: a pointer to receive the dequeued node
 * @return vsf_err_t: VSF_ERR_NONE if a node is dequeued, VSF_ERR_NOT_READY if
 * the queue is empty
 *
 * @note Only available when VSF_EDA_QUEUE_CFG_SUPPORT_ISR is ENABLED. The sender
 * notification is done by the kernel task.
 * @note Intended for interrupt context: the node is dequeued directly, and the
 * sender wakeup is bounced to the kernel task. May also be called in task
 * context, but vsf_eda_queue_recv() is more efficient there.
 * \~chinese
 * @brief 在中断上下文里从队列接收一个节点
 * @param[in] pthis: 指向结构体 @ref vsf_eda_queue_t 的指针
 * @param[out] node: 用于接收出队节点的指针
 * @return vsf_err_t: 如果节点已出队返回 VSF_ERR_NONE，如果队列空返回
 * VSF_ERR_NOT_READY
 *
 * @note 仅在 VSF_EDA_QUEUE_CFG_SUPPORT_ISR 使能时可用。发送者通知由内核任务完成。
 * @note 设计用于中断上下文：节点直接出队，发送者的唤醒通知经内核任务跳板
 * 完成。也可以在任务上下文中调用，但任务里使用 vsf_eda_queue_recv()
 * 效率更高。
 */
extern vsf_err_t vsf_eda_queue_recv_isr(vsf_eda_queue_t *pthis, void **node);
#   endif

#endif      // VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE


#endif      // VSF_KERNEL_CFG_SUPPORT_SYNC

#ifdef __cplusplus
}
#endif

#undef __VSF_EDA_CLASS_INHERIT__
#undef __VSF_EDA_CLASS_IMPLEMENT

/*============================ INCLUDES ======================================*/

#if VSF_KERNEL_CFG_TRACE == ENABLED
#   ifdef VSF_KERNEL_CFG_TRACE_HEADER
#       include VSF_KERNEL_CFG_TRACE_HEADER
#   endif
#endif

#endif
#endif      // __VSF_EDA_H__
