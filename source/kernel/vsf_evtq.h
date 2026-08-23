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

#ifndef __VSF_EVTQ_H__
#define __VSF_EVTQ_H__

/*============================ INCLUDES ======================================*/
#include "kernel/vsf_kernel_cfg.h"
#if VSF_USE_KERNEL == ENABLED

#include "./vsf_eda.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/

typedef struct vsf_evtq_ctx_t {
    vsf_eda_t *eda;
    vsf_evt_t evt;
    uintptr_t msg;
#if VSF_KERNEL_CFG_EDA_CPU_USAGE == ENABLED
    vsf_systimer_tick_t preempted_ticks;
    bool is_timing;
#endif
} vsf_evtq_ctx_t;

#if VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED == ENABLED

typedef struct vsf_evt_node_t vsf_evt_node_t;
typedef struct vsf_evtq_t vsf_evtq_t;

#if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED

struct vsf_evtq_t {
    vsf_dlist_t rdy_list;
    vsf_evtq_ctx_t cur;
#   if VSF_KERNEL_CFG_TRACE == ENABLED
    bool is_isr_info_sent;
#   endif
};

struct vsf_evt_node_t {
    implement(vsf_slist_node_t)

#   if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
    vsf_evt_t evt;
    void *msg;
#   else
    union {
        uintptr_t value;
        void *msg;
    } evt_union;
#   endif
};

#else

struct vsf_evt_node_t {
    vsf_eda_t *eda;

#   if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
    vsf_evt_t evt;
    void *msg;
#   else
    union {
        uintptr_t value;
        void *msg;
    } evt_union;
#   endif
};

struct vsf_evtq_t {
    vsf_evt_node_t *node;
    uint8_t bitsize;

    // private
    uint8_t head;
    uint8_t tail;
#   if VSF_KERNEL_CFG_TRACE == ENABLED
    bool is_isr_info_sent;
#   endif
    vsf_evtq_ctx_t cur;
};

#endif
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED == ENABLED
/**
 * \~english
 * @brief Initialize an event queue
 * @param[in] this_ptr: a pointer to structure @ref vsf_evtq_t
 * @return vsf_err_t: always VSF_ERR_NONE
 * @note Kernel-internal API, called during kernel startup; applications do not call it directly.
 * \~chinese
 * @brief 初始化一个事件队列
 * @param[in] this_ptr: 指向结构体 @ref vsf_evtq_t 的指针
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE
 * @note 内核内部 API，在内核启动阶段调用；应用程序不直接调用。
 */
extern vsf_err_t vsf_evtq_init(vsf_evtq_t *this_ptr);
#   if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
/**
 * \~english
 * @brief Post an event with a message to the event queue of the target eda
 * @param[in] eda: a pointer to the target @ref vsf_eda_t
 * @param[in] evt: the event to post
 * @param[in] msg: the message pointer carried with the event
 * @return vsf_err_t: VSF_ERR_NONE if posted successfully;
 *         VSF_ERR_FAIL if the eda is limited and already has a pending event;
 *         VSF_ERR_NOT_ENOUGH_RESOURCES if the event queue is full
 * @note Can be called in task or interrupt context.
 * \~chinese
 * @brief 向目标 eda 的事件队列发送一个携带消息的事件
 * @param[in] eda: 指向目标 @ref vsf_eda_t 的指针
 * @param[in] evt: 要发送的事件
 * @param[in] msg: 随事件携带的消息指针
 * @return vsf_err_t: 发送成功返回 VSF_ERR_NONE；
 *         如果 eda 是受限的且已有待处理事件则返回 VSF_ERR_FAIL；
 *         如果事件队列已满则返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 * @note 可以在任务或中断上下文中调用。
 */
extern vsf_err_t vsf_evtq_post_evt_msg(vsf_eda_t *eda, vsf_evt_t evt, void *msg);
#   endif
/**
 * \~english
 * @brief Check whether an event queue is empty
 * @param[in] this_ptr: a pointer to structure @ref vsf_evtq_t
 * @return bool: true if the event queue is empty, false otherwise
 * @note Kernel-internal API used by the kernel event queues; applications normally use vsf_eda_post_evt()/vsf_eda_post_msg() instead.
 * \~chinese
 * @brief 检查事件队列是否为空
 * @param[in] this_ptr: 指向结构体 @ref vsf_evtq_t 的指针
 * @return bool: 如果事件队列为空返回 true，否则返回 false
 * @note 内核事件队列使用的内部 API；应用程序一般使用 vsf_eda_post_evt()/vsf_eda_post_msg()。
 */
extern bool vsf_evtq_is_empty(vsf_evtq_t *this_ptr);

/**
 * \~english
 * @brief Poll an event queue and dispatch the pending events to their edas
 * @param[in] this_ptr: a pointer to structure @ref vsf_evtq_t
 * @return vsf_err_t: VSF_ERR_NONE
 * @note Kernel-internal API used in event-queue (SWI) context; applications normally use vsf_eda_post_evt()/vsf_eda_post_msg() instead.
 * \~chinese
 * @brief 轮询一个事件队列，并将待处理的事件分发给对应的 eda
 * @param[in] this_ptr: 指向结构体 @ref vsf_evtq_t 的指针
 * @return vsf_err_t: VSF_ERR_NONE
 * @note 内核内部 API，在事件队列（SWI）上下文中使用；应用程序一般使用 vsf_eda_post_evt()/vsf_eda_post_msg()。
 */
extern vsf_err_t vsf_evtq_poll(vsf_evtq_t *this_ptr);

/**
 * \~english
 * @brief Clean the pending event(s) posted to the current eda
 * @param[in] evt: the event to clean; VSF_EVT_NONE cleans all pending events of the current eda
 * @return none
 * @note Kernel-internal API, called in the current eda task context; applications normally use vsf_eda_post_evt()/vsf_eda_post_msg() instead.
 * \~chinese
 * @brief 清除发送给当前 eda 的待处理事件
 * @param[in] evt: 要清除的事件；VSF_EVT_NONE 表示清除当前 eda 的所有待处理事件
 * @return 无
 * @note 内核内部 API，在当前 eda 任务上下文中调用；应用程序一般使用 vsf_eda_post_evt()/vsf_eda_post_msg()。
 */
extern void vsf_evtq_clean_evt(vsf_evt_t evt);
#endif

/**
 * \~english
 * @brief Event queue hook called when an eda is initialized
 * @param[in] eda: a pointer to the @ref vsf_eda_t being initialized
 * @return none
 * @note Kernel-internal hook, invoked by the kernel in task context when an eda is initialized; applications do not call it directly.
 * \~chinese
 * @brief 事件队列钩子，在 eda 初始化时调用
 * @param[in] eda: 指向正在初始化的 @ref vsf_eda_t 的指针
 * @return 无
 * @note 内核内部钩子，由内核在任务上下文中、eda 初始化时调用；应用程序不直接调用。
 */
extern void vsf_evtq_on_eda_init(vsf_eda_t *eda);

/**
 * \~english
 * @brief Event queue hook called when an eda is finalized
 * @param[in] eda: a pointer to the @ref vsf_eda_t being finalized
 * @return none
 * @note Kernel-internal hook, invoked by the kernel in task context when an eda is terminated; applications do not call it directly.
 * \~chinese
 * @brief 事件队列钩子，在 eda 终止时调用
 * @param[in] eda: 指向正在终止的 @ref vsf_eda_t 的指针
 * @return 无
 * @note 内核内部钩子，由内核在任务上下文中、eda 终止时调用；应用程序不直接调用。
 */
extern void vsf_evtq_on_eda_fini(vsf_eda_t *eda);

/**
 * \~english
 * @brief Post an event to the event queue of the target eda, with force control
 * @param[in] eda: a pointer to the target @ref vsf_eda_t
 * @param[in] evt: the event to post
 * @param[in] force: true to post even if the eda is limited and already has a pending event
 * @return vsf_err_t: VSF_ERR_NONE if posted successfully;
 *         VSF_ERR_FAIL if the eda is limited and already has a pending event while force is false;
 *         VSF_ERR_NOT_ENOUGH_RESOURCES if the event queue is full
 * @note Can be called in task or interrupt context.
 * \~chinese
 * @brief 向目标 eda 的事件队列发送一个事件，可强制发送
 * @param[in] eda: 指向目标 @ref vsf_eda_t 的指针
 * @param[in] evt: 要发送的事件
 * @param[in] force: 为 true 时，即使 eda 是受限的且已有待处理事件也强制发送
 * @return vsf_err_t: 发送成功返回 VSF_ERR_NONE；
 *         如果 eda 是受限的且已有待处理事件且 force 为 false 则返回 VSF_ERR_FAIL；
 *         如果事件队列已满则返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 * @note 可以在任务或中断上下文中调用。
 */
extern vsf_err_t vsf_evtq_post_evt_ex(vsf_eda_t *eda, vsf_evt_t evt, bool force);

/**
 * \~english
 * @brief Post an event to the event queue of the target eda
 * @param[in] eda: a pointer to the target @ref vsf_eda_t
 * @param[in] evt: the event to post
 * @return vsf_err_t: VSF_ERR_NONE if posted successfully;
 *         VSF_ERR_FAIL if the eda is limited and already has a pending event;
 *         VSF_ERR_NOT_ENOUGH_RESOURCES if the event queue is full
 * @note Can be called in task or interrupt context.
 * \~chinese
 * @brief 向目标 eda 的事件队列发送一个事件
 * @param[in] eda: 指向目标 @ref vsf_eda_t 的指针
 * @param[in] evt: 要发送的事件
 * @return vsf_err_t: 发送成功返回 VSF_ERR_NONE；
 *         如果 eda 是受限的且已有待处理事件则返回 VSF_ERR_FAIL；
 *         如果事件队列已满则返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 * @note 可以在任务或中断上下文中调用。
 */
extern vsf_err_t vsf_evtq_post_evt(vsf_eda_t *eda, vsf_evt_t evt);

/**
 * \~english
 * @brief Post a message to the event queue of the target eda (event VSF_EVT_MESSAGE)
 * @param[in] eda: a pointer to the target @ref vsf_eda_t
 * @param[in] msg: the message pointer to post
 * @return vsf_err_t: VSF_ERR_NONE if posted successfully;
 *         VSF_ERR_FAIL if the eda is limited and already has a pending event;
 *         VSF_ERR_NOT_ENOUGH_RESOURCES if the event queue is full
 * @note Can be called in task or interrupt context.
 * \~chinese
 * @brief 向目标 eda 的事件队列发送一个消息（事件 VSF_EVT_MESSAGE）
 * @param[in] eda: 指向目标 @ref vsf_eda_t 的指针
 * @param[in] msg: 要发送的消息指针
 * @return vsf_err_t: 发送成功返回 VSF_ERR_NONE；
 *         如果 eda 是受限的且已有待处理事件则返回 VSF_ERR_FAIL；
 *         如果事件队列已满则返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 * @note 可以在任务或中断上下文中调用。
 */
extern vsf_err_t vsf_evtq_post_msg(vsf_eda_t *eda, void *msg);

#ifdef __cplusplus
}
#endif

#endif
#endif
