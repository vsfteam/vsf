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

#ifndef __VSF_EDA_SLIST_QUEUE_H__
#define __VSF_EDA_SLIST_QUEUE_H__

/*============================ INCLUDES ======================================*/
#include "kernel/vsf_kernel_cfg.h"

#if VSF_USE_KERNEL == ENABLED

#if     defined(__VSF_EDA_SLIST_QUEUE_CLASS_IMPLEMENT)
#   undef __VSF_EDA_SLIST_QUEUE_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_EDA_SLIST_QUEUE_CLASS_INHERIT__)
#   undef __VSF_EDA_SLIST_QUEUE_CLASS_INHERIT__
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_SUPPORT_SLIST_QUEUE == ENABLED

/**
 * \~english
 * @brief Alias of vsf_eda_slist_queue_t for message queue usage.
 *
 * \~chinese
 * @brief vsf_eda_slist_queue_t 的别名，用于消息队列场景。
 */
#   define vsf_msg_queue_t              vsf_eda_slist_queue_t

/**
 * \~english
 * @brief Initialize an EDA singly-linked list message queue
 * @param[in] queue: a pointer to the message queue instance @ref vsf_msg_queue_t
 * @param[in] max: maximum number of nodes the queue can hold
 * @return vsf_err_t: always VSF_ERR_NONE (invalid parameters trigger an assertion)
 *
 * @note Must be called in task (or initialization) context, NOT in interrupt context.
 * \~chinese
 * @brief 初始化一个 EDA 单向链表消息队列
 * @param[in] queue: 指向消息队列实例 @ref vsf_msg_queue_t 的指针
 * @param[in] max: 队列可容纳的最大节点数
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE（参数非法会触发断言）
 *
 * @note 必须在任务（或初始化）上下文中调用，禁止在中断上下文中调用。
 */
#   define vsf_eda_msg_queue_init(__queue, __max)                               \
                vsf_eda_slist_queue_init((__queue), (__max))

/**
 * \~english
 * @brief Send a node to the message queue, pending while the queue is full
 * @param[in] queue: a pointer to the message queue instance @ref vsf_msg_queue_t
 * @param[in] node: the node to send
 * @param[in] timeout: timeout in ticks; negative waits forever, 0 is a non-blocking
 * try, a positive value waits up to the given ticks (requires timer support)
 * @return vsf_err_t: VSF_ERR_NONE if the node is enqueued immediately;
 * VSF_ERR_NOT_READY if the queue is full and the caller pends or the try failed
 *
 * @note If VSF_ERR_NOT_READY is returned with a non-zero timeout, the caller is
 * woken later with VSF_EVT_SYNC, VSF_EVT_TIMER (timeout) or VSF_EVT_SYNC_CANCEL;
 * call vsf_eda_msg_queue_send_get_reason() to retrieve the result.
 * @note Must be called in an eda task context (i.e. inside an event handler); must NOT be called in interrupt context.
 * \~chinese
 * @brief 向消息队列发送一个节点，队列满时进入等待
 * @param[in] queue: 指向消息队列实例 @ref vsf_msg_queue_t 的指针
 * @param[in] node: 要发送的节点
 * @param[in] timeout: 超时 tick 数；负数永远等待，0 为不等待的尝试，正数最多
 * 等待对应的 tick 数（需要定时器支持）
 * @return vsf_err_t: 节点立即入队返回 VSF_ERR_NONE；队列满，调用者进入等待
 * 或者尝试失败返回 VSF_ERR_NOT_READY
 *
 * @note 如果以非零超时返回 VSF_ERR_NOT_READY，调用者之后会收到 VSF_EVT_SYNC、
 * VSF_EVT_TIMER（超时）或 VSF_EVT_SYNC_CANCEL 事件被唤醒；调用
 * vsf_eda_msg_queue_send_get_reason() 获取结果。
 * @note 必须在 eda 任务上下文（事件处理函数）中调用，禁止在中断上下文中调用。
 */
#   define vsf_eda_msg_queue_send(__queue, __node, __timeout)                   \
                vsf_eda_queue_send((vsf_eda_queue_t *)(__queue), (__node), (__timeout))

/**
 * \~english
 * @brief Retrieve the result of a message queue send from the wakeup event
 * @param[in] queue: a pointer to the message queue instance @ref vsf_msg_queue_t
 * @param[in] evt: the wakeup event (VSF_EVT_SYNC, VSF_EVT_TIMER or VSF_EVT_SYNC_CANCEL)
 * @param[in] node: the node that was being sent
 * @return vsf_sync_reason_t: VSF_SYNC_GET if the node is enqueued, VSF_SYNC_TIMEOUT
 * on timeout, VSF_SYNC_CANCEL if cancelled, VSF_SYNC_PENDING if the wait should continue
 *
 * @note Must be called in an eda task context (i.e. inside an event handler); must NOT be called in interrupt context.
 * \~chinese
 * @brief 根据唤醒事件获取消息队列发送的结果
 * @param[in] queue: 指向消息队列实例 @ref vsf_msg_queue_t 的指针
 * @param[in] evt: 唤醒事件（VSF_EVT_SYNC、VSF_EVT_TIMER 或 VSF_EVT_SYNC_CANCEL）
 * @param[in] node: 当时正在发送的节点
 * @return vsf_sync_reason_t: 节点已入队返回 VSF_SYNC_GET，超时返回
 * VSF_SYNC_TIMEOUT，被取消返回 VSF_SYNC_CANCEL，需要继续等待返回 VSF_SYNC_PENDING
 *
 * @note 必须在 eda 任务上下文（事件处理函数）中调用，禁止在中断上下文中调用。
 */
#   define vsf_eda_msg_queue_send_get_reason(__queue, __evt, __node)            \
                vsf_eda_queue_send_get_reason((vsf_eda_queue_t *)(__queue), (__evt), (__node))

/**
 * \~english
 * @brief Receive a node from the message queue, pending while the queue is empty
 * @param[in] queue: a pointer to the message queue instance @ref vsf_msg_queue_t
 * @param[out] node: a pointer to store the received node
 * @param[in] timeout: timeout in ticks; negative waits forever, 0 is a non-blocking
 * try, a positive value waits up to the given ticks (requires timer support)
 * @return vsf_err_t: VSF_ERR_NONE if a node is dequeued immediately;
 * VSF_ERR_NOT_READY if the queue is empty and the caller pends or the try failed
 *
 * @note If VSF_ERR_NOT_READY is returned with a non-zero timeout, the caller is
 * woken later with VSF_EVT_SYNC, VSF_EVT_TIMER (timeout) or VSF_EVT_SYNC_CANCEL;
 * call vsf_eda_msg_queue_recv_get_reason() to retrieve the result.
 * @note Must be called in an eda task context (i.e. inside an event handler); must NOT be called in interrupt context.
 * \~chinese
 * @brief 从消息队列接收一个节点，队列空时进入等待
 * @param[in] queue: 指向消息队列实例 @ref vsf_msg_queue_t 的指针
 * @param[out] node: 用于存储接收到的节点的指针
 * @param[in] timeout: 超时 tick 数；负数永远等待，0 为不等待的尝试，正数最多
 * 等待对应的 tick 数（需要定时器支持）
 * @return vsf_err_t: 节点立即出队返回 VSF_ERR_NONE；队列空，调用者进入等待
 * 或者尝试失败返回 VSF_ERR_NOT_READY
 *
 * @note 如果以非零超时返回 VSF_ERR_NOT_READY，调用者之后会收到 VSF_EVT_SYNC、
 * VSF_EVT_TIMER（超时）或 VSF_EVT_SYNC_CANCEL 事件被唤醒；调用
 * vsf_eda_msg_queue_recv_get_reason() 获取结果。
 * @note 必须在 eda 任务上下文（事件处理函数）中调用，禁止在中断上下文中调用。
 */
#   define vsf_eda_msg_queue_recv(__queue, __node, __timeout)                   \
                vsf_eda_queue_recv((vsf_eda_queue_t *)(__queue), (void **)(__node), (__timeout))

/**
 * \~english
 * @brief Retrieve the result of a message queue receive from the wakeup event
 * @param[in] queue: a pointer to the message queue instance @ref vsf_msg_queue_t
 * @param[in] evt: the wakeup event (VSF_EVT_SYNC, VSF_EVT_TIMER or VSF_EVT_SYNC_CANCEL)
 * @param[out] node: a pointer to store the received node
 * @return vsf_sync_reason_t: VSF_SYNC_GET if a node is dequeued, VSF_SYNC_TIMEOUT
 * on timeout, VSF_SYNC_CANCEL if cancelled, VSF_SYNC_PENDING if the wait should continue
 *
 * @note Must be called in an eda task context (i.e. inside an event handler); must NOT be called in interrupt context.
 * \~chinese
 * @brief 根据唤醒事件获取消息队列接收的结果
 * @param[in] queue: 指向消息队列实例 @ref vsf_msg_queue_t 的指针
 * @param[in] evt: 唤醒事件（VSF_EVT_SYNC、VSF_EVT_TIMER 或 VSF_EVT_SYNC_CANCEL）
 * @param[out] node: 用于存储接收到的节点的指针
 * @return vsf_sync_reason_t: 节点已出队返回 VSF_SYNC_GET，超时返回
 * VSF_SYNC_TIMEOUT，被取消返回 VSF_SYNC_CANCEL，需要继续等待返回 VSF_SYNC_PENDING
 *
 * @note 必须在 eda 任务上下文（事件处理函数）中调用，禁止在中断上下文中调用。
 */
#   define vsf_eda_msg_queue_recv_get_reason(__queue, __evt, __node)            \
                vsf_eda_queue_recv_get_reason((vsf_eda_queue_t *)(__queue), (__evt), (void **)(__node))

#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if VSF_KERNEL_CFG_SUPPORT_SLIST_QUEUE == ENABLED

/**
 * \~english
 * @brief An @ref vsf_eda_queue_t backend based on a singly-linked list
 *
 * @note Initialize with vsf_eda_slist_queue_init(); also aliased as
 *       @ref vsf_msg_queue_t for message-queue usage.
 * \~chinese
 * @brief 基于单向链表的 @ref vsf_eda_queue_t 后端
 *
 * @note 使用 vsf_eda_slist_queue_init() 初始化；消息队列场景下也以
 *       @ref vsf_msg_queue_t 作为别名。
 */
vsf_class(vsf_eda_slist_queue_t) {
    public_member(
        implement(vsf_eda_queue_t)
    )
    private_member(
        vsf_slist_queue_t queue;
    )
};

#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_KERNEL_CFG_SUPPORT_SLIST_QUEUE == ENABLED
/**
 * \~english
 * @brief Initialize an EDA singly-linked list queue
 * @param[in] this_ptr: a pointer to structure @ref vsf_eda_slist_queue_t
 * @param[in] max: maximum number of nodes the queue can hold
 * @return vsf_err_t: always VSF_ERR_NONE (invalid parameters trigger an assertion)
 * @note Must be called in task (or initialization) context, NOT in interrupt context.
 * \~chinese
 * @brief 初始化一个 EDA 单向链表队列
 * @param[in] this_ptr: 指向结构体 @ref vsf_eda_slist_queue_t 的指针
 * @param[in] max: 队列可容纳的最大节点数
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE（参数非法会触发断言）
 * @note 必须在任务（或初始化）上下文中调用，禁止在中断上下文中调用。
 */
extern vsf_err_t vsf_eda_slist_queue_init(vsf_eda_slist_queue_t *this_ptr, uint_fast16_t max);
#endif

#ifdef __cplusplus
}
#endif

#endif
#endif      // __VSF_EDA_SLIST_QUEUE_H__
