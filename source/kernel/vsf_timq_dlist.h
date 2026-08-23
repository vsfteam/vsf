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

#ifndef __VSF_TIMQ_DLIST_H__
#define __VSF_TIMQ_DLIST_H__

/*============================ INCLUDES ======================================*/
#include "kernel/vsf_kernel_cfg.h"

#if VSF_USE_KERNEL == ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/**
 * \~english
 * @brief Initialize a timer queue
 * @param[in] __queue: a pointer to the timer queue of type @ref vsf_timer_queue_t
 * @note Used internally by the kernel timer implementation; users normally do not call it directly.
 * \~chinese
 * @brief 初始化一个定时器队列
 * @param[in] __queue: 指向 @ref vsf_timer_queue_t 类型定时器队列的指针
 * @note 由内核定时器实现内部使用；用户通常不直接调用。
 */
#define vsf_timq_init(__queue)              vsf_dlist_init(__queue)

/**
 * \~english
 * @brief Insert a timer eda into the timer queue, sorted in ascending order of due
 * @param[in] __queue: a pointer to the timer queue of type @ref vsf_timer_queue_t
 * @param[in] __teda: a pointer to the @ref vsf_teda_t to insert
 * @note Used internally by the kernel timer implementation; users normally do not call it directly.
 * \~chinese
 * @brief 将一个定时器 eda 按 due 升序插入定时器队列
 * @param[in] __queue: 指向 @ref vsf_timer_queue_t 类型定时器队列的指针
 * @param[in] __teda: 指向要插入的 @ref vsf_teda_t 的指针
 * @note 由内核定时器实现内部使用；用户通常不直接调用。
 */
#define vsf_timq_insert(__queue, __teda)                                        \
        vsf_dlist_insert(                                                       \
                vsf_teda_t, timer_node,                                         \
                (__queue),                                                      \
                (__teda),                                                       \
                _->due >= (__teda)->due)

/**
 * \~english
 * @brief Remove a timer eda from the timer queue
 * @param[in] __queue: a pointer to the timer queue of type @ref vsf_timer_queue_t
 * @param[in] __teda: a pointer to the @ref vsf_teda_t to remove
 * @note Used internally by the kernel timer implementation; users normally do not call it directly.
 * \~chinese
 * @brief 从定时器队列中移除一个定时器 eda
 * @param[in] __queue: 指向 @ref vsf_timer_queue_t 类型定时器队列的指针
 * @param[in] __teda: 指向要移除的 @ref vsf_teda_t 的指针
 * @note 由内核定时器实现内部使用；用户通常不直接调用。
 */
#define vsf_timq_remove(__queue, __teda)                                        \
        vsf_dlist_remove(                                                       \
                vsf_teda_t, timer_node,                                         \
                (__queue),                                                      \
                (__teda))

/**
 * \~english
 * @brief Get the head timer eda (the earliest due) of the timer queue without removing it
 * @param[in] __queue: a pointer to the timer queue of type @ref vsf_timer_queue_t
 * @param[out] __teda: a @ref vsf_teda_t pointer variable receiving the head timer eda,
 *        NULL if the queue is empty
 * @note Used internally by the kernel timer implementation; users normally do not call it directly.
 * \~chinese
 * @brief 获取定时器队列的队首定时器 eda（due 最早），但不将其移除
 * @param[in] __queue: 指向 @ref vsf_timer_queue_t 类型定时器队列的指针
 * @param[out] __teda: 用于接收队首定时器 eda 的 @ref vsf_teda_t 指针变量，
 *        队列为空时为 NULL
 * @note 由内核定时器实现内部使用；用户通常不直接调用。
 */
#define vsf_timq_peek(__queue, __teda)                                          \
        vsf_dlist_queue_peek(                                                   \
                vsf_teda_t, timer_node,                                         \
                (__queue),                                                      \
                (__teda))

/**
 * \~english
 * @brief Remove the head timer eda (the earliest due) from the timer queue
 * @param[in] __queue: a pointer to the timer queue of type @ref vsf_timer_queue_t
 * @param[out] __teda: a @ref vsf_teda_t pointer variable receiving the removed head timer eda,
 *        NULL if the queue is empty
 * @note Used internally by the kernel timer implementation; users normally do not call it directly.
 * \~chinese
 * @brief 从定时器队列中移除队首定时器 eda（due 最早）
 * @param[in] __queue: 指向 @ref vsf_timer_queue_t 类型定时器队列的指针
 * @param[out] __teda: 用于接收被移除的队首定时器 eda 的 @ref vsf_teda_t 指针变量，
 *        队列为空时为 NULL
 * @note 由内核定时器实现内部使用；用户通常不直接调用。
 */
#define vsf_timq_dequeue(__queue, __teda)                                       \
        vsf_dlist_queue_dequeue(                                                \
                vsf_teda_t, timer_node,                                         \
                (__queue),                                                      \
                (__teda))



/**
 * \~english
 * @brief Initialize a callback timer queue
 * @param[in] __queue: a pointer to the timer queue of type @ref vsf_timer_queue_t
 * @note Used internally by the kernel timer implementation; users normally do not call it directly.
 * \~chinese
 * @brief 初始化一个回调定时器队列
 * @param[in] __queue: 指向 @ref vsf_timer_queue_t 类型定时器队列的指针
 * @note 由内核定时器实现内部使用；用户通常不直接调用。
 */
#define vsf_callback_timq_init(__queue)     vsf_dlist_init(__queue)

/**
 * \~english
 * @brief Enqueue a callback timer to the tail of the timer queue (FIFO order)
 * @param[in] __queue: a pointer to the timer queue of type @ref vsf_timer_queue_t
 * @param[in] __timer: a pointer to the @ref vsf_callback_timer_t to enqueue
 * @note Used internally by the kernel timer implementation; users normally do not call it directly.
 * \~chinese
 * @brief 将一个回调定时器入队到定时器队列尾部（先进先出顺序）
 * @param[in] __queue: 指向 @ref vsf_timer_queue_t 类型定时器队列的指针
 * @param[in] __timer: 指向要入队的 @ref vsf_callback_timer_t 的指针
 * @note 由内核定时器实现内部使用；用户通常不直接调用。
 */
#define vsf_callback_timq_enqueue(__queue, __timer)                             \
        vsf_dlist_queue_enqueue(                                                \
                vsf_callback_timer_t, timer_node,                               \
                (__queue),                                                      \
                (__timer))

/**
 * \~english
 * @brief Insert a callback timer into the timer queue, sorted in ascending order of due
 * @param[in] __queue: a pointer to the timer queue of type @ref vsf_timer_queue_t
 * @param[in] __timer: a pointer to the @ref vsf_callback_timer_t to insert
 * @note Used internally by the kernel timer implementation; users normally do not call it directly.
 * \~chinese
 * @brief 将一个回调定时器按 due 升序插入定时器队列
 * @param[in] __queue: 指向 @ref vsf_timer_queue_t 类型定时器队列的指针
 * @param[in] __timer: 指向要插入的 @ref vsf_callback_timer_t 的指针
 * @note 由内核定时器实现内部使用；用户通常不直接调用。
 */
#define vsf_callback_timq_insert(__queue, __timer)                              \
        vsf_dlist_insert(                                                       \
                vsf_callback_timer_t, timer_node,                               \
                (__queue),                                                      \
                (__timer),                                                      \
                _->due >= (__timer)->due)

/**
 * \~english
 * @brief Remove a callback timer from the timer queue
 * @param[in] __queue: a pointer to the timer queue of type @ref vsf_timer_queue_t
 * @param[in] __timer: a pointer to the @ref vsf_callback_timer_t to remove
 * @note Used internally by the kernel timer implementation; users normally do not call it directly.
 * \~chinese
 * @brief 从定时器队列中移除一个回调定时器
 * @param[in] __queue: 指向 @ref vsf_timer_queue_t 类型定时器队列的指针
 * @param[in] __timer: 指向要移除的 @ref vsf_callback_timer_t 的指针
 * @note 由内核定时器实现内部使用；用户通常不直接调用。
 */
#define vsf_callback_timq_remove(__queue, __timer)                              \
        vsf_dlist_remove(                                                       \
                vsf_callback_timer_t, timer_node,                               \
                (__queue),                                                      \
                (__timer))

/**
 * \~english
 * @brief Get the head callback timer of the timer queue without removing it
 * @param[in] __queue: a pointer to the timer queue of type @ref vsf_timer_queue_t
 * @param[out] __timer: a @ref vsf_callback_timer_t pointer variable receiving the head callback timer,
 *        NULL if the queue is empty
 * @note Used internally by the kernel timer implementation; users normally do not call it directly.
 * \~chinese
 * @brief 获取定时器队列的队首回调定时器，但不将其移除
 * @param[in] __queue: 指向 @ref vsf_timer_queue_t 类型定时器队列的指针
 * @param[out] __timer: 用于接收队首回调定时器的 @ref vsf_callback_timer_t 指针变量，
 *        队列为空时为 NULL
 * @note 由内核定时器实现内部使用；用户通常不直接调用。
 */
#define vsf_callback_timq_peek(__queue, __timer)                                \
        vsf_dlist_queue_peek(                                                   \
                vsf_callback_timer_t, timer_node,                               \
                (__queue),                                                      \
                (__timer))

/**
 * \~english
 * @brief Remove the head callback timer from the timer queue
 * @param[in] __queue: a pointer to the timer queue of type @ref vsf_timer_queue_t
 * @param[out] __timer: a @ref vsf_callback_timer_t pointer variable receiving the removed head
 *        callback timer, NULL if the queue is empty
 * @note Used internally by the kernel timer implementation; users normally do not call it directly.
 * \~chinese
 * @brief 从定时器队列中移除队首回调定时器
 * @param[in] __queue: 指向 @ref vsf_timer_queue_t 类型定时器队列的指针
 * @param[out] __timer: 用于接收被移除的队首回调定时器的 @ref vsf_callback_timer_t 指针变量，
 *        队列为空时为 NULL
 * @note 由内核定时器实现内部使用；用户通常不直接调用。
 */
#define vsf_callback_timq_dequeue(__queue, __timer)                             \
        vsf_dlist_queue_dequeue(                                                \
                vsf_callback_timer_t, timer_node,                               \
                (__queue),                                                      \
                (__timer))

/*============================ TYPES =========================================*/

typedef vsf_dlist_t vsf_timer_queue_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
#endif
