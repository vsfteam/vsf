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

#ifndef __VSF_THREAD_H__
#define __VSF_THREAD_H__

/*============================ INCLUDES ======================================*/

#include "kernel/vsf_kernel_cfg.h"

#if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED && VSF_USE_KERNEL == ENABLED
#include "../vsf_eda.h"

#if VSF_KERNEL_CFG_EDA_SUPPORT_TASK == ENABLED
#   include "./vsf_task.h"
#endif

#if     defined(__VSF_THREAD_CLASS_IMPLEMENT)
#   undef __VSF_THREAD_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__VSF_THREAD_CLASS_INHERIT__)
#   undef __VSF_THREAD_CLASS_INHERIT__
#   define __PLOOC_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE
#   define VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE        1
#endif
#ifndef VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE
#   define VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE    0
#endif

#ifndef VSF_KERNEL_CFG_THREAD_STACK_ALIGN_BIT
#   ifdef VSF_ARCH_STACK_ALIGN_BIT
#       define VSF_KERNEL_CFG_THREAD_STACK_ALIGN_BIT    VSF_ARCH_STACK_ALIGN_BIT
#   else
#       define VSF_KERNEL_CFG_THREAD_STACK_ALIGN_BIT    3
#   endif
#endif

#define __VSF_THREAD_STACK_SAFE_SIZE(__stack)                                   \
            (   (   (   ((__stack) + VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE - 1) \
                    /   VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE)                  \
                * VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE)                        \
            + VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE)

#ifdef VSF_ARCH_LIMIT_NO_SET_STACK
// arch does not support set stack, use host_thread mode
#   define VSF_KERNEL_THREAD_USE_HOST                   ENABLED
#   ifdef VSF_ARCH_RTOS_DYNAMIC_STACK
#       define VSF_KERNEL_THREAD_DYNAMIC_STACK          ENABLED
#       if VSF_KERNEL_CFG_THREAD_STACK_CHECK == ENABLED
#           warning VSF_KERNEL_CFG_THREAD_STACK_CHECK is not usable for dynamic stack
#           undef VSF_KERNEL_CFG_THREAD_STACK_CHECK
#           define VSF_KERNEL_CFG_THREAD_STACK_CHECK    DISABLED
#       endif
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __declare_vsf_thread(__name)                                            \
            typedef struct __name __name;                                       \
            typedef struct thread_cb_##__name##_t thread_cb_##__name##_t;
/**
 * \~english
 * @brief Declare a thread type with embedded stack
 * @param[in] __name: name of the thread type to declare
 *
 * @note Declares the thread instance type __name and its thread control block
 *       type thread_cb_##__name##_t; use def_vsf_thread to define them.
 * \~chinese
 * @brief 声明一个内嵌堆栈的线程类型
 * @param[in] __name: 要声明的线程类型名
 *
 * @note 声明线程实例类型 __name 及其线程控制块类型
 *       thread_cb_##__name##_t；使用 def_vsf_thread 进行定义。
 */
#define declare_vsf_thread(__name)      __declare_vsf_thread(__name)
/**
 * \~english
 * @brief Declare a thread type using an external stack, same as declare_vsf_thread
 * @param[in] __name: name of the thread type to declare
 *
 * @note Use def_vsf_thread_ex to define the thread type.
 * \~chinese
 * @brief 声明一个使用外部堆栈的线程类型，与 declare_vsf_thread 相同
 * @param[in] __name: 要声明的线程类型名
 *
 * @note 使用 def_vsf_thread_ex 定义该线程类型。
 */
#define declare_vsf_thread_ex(__name)   __declare_vsf_thread(__name)

/**
 * \~english
 * @brief Alias of declare_vsf_thread
 * @param[in] __name: name of the thread type to declare
 * \~chinese
 * @brief declare_vsf_thread 的别名
 * @param[in] __name: 要声明的线程类型名
 */
#define dcl_vsf_thread(__name)          declare_vsf_thread(__name)
/**
 * \~english
 * @brief Alias of declare_vsf_thread_ex
 * @param[in] __name: name of the thread type to declare
 * \~chinese
 * @brief declare_vsf_thread_ex 的别名
 * @param[in] __name: 要声明的线程类型名
 */
#define dcl_vsf_thread_ex(__name)       declare_vsf_thread(__name)

#if VSF_KERNEL_THREAD_DYNAMIC_STACK == ENABLED
#   define __vsf_thread_set_stack_canery(__thread, __task)                      \
            (__thread)->canary = 0xDEADBEEF;
#else
#   define __vsf_thread_set_stack_canery(__thread, __task)                      \
            (__task)->canary = 0xDEADBEEF;
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   define __vsf_thread_set_stack(__thread, __task, __stack_ptr, __stack_bytesize)\
            (__thread)->use_as__vsf_thread_cb_t.stack = (__stack_ptr);          \
            (__thread)->use_as__vsf_thread_cb_t.stack_size = (__stack_bytesize);\
            __vsf_thread_set_stack_canery(__thread, __task)
#else
#   define __vsf_thread_set_stack(__thread, __task, __stack_ptr, __stack_bytesize)\
            (__thread)->stack = (__stack_ptr);                                  \
            (__thread)->stack_size = (__stack_bytesize);                        \
            __vsf_thread_set_stack_canery(__thread, __task)
#endif

#if VSF_KERNEL_THREAD_DYNAMIC_STACK == ENABLED
#   define __vsf_thread_def_stack(__name, __bytesize)                           \
            typedef enum {                                                      \
                vsf_thread##__name##_stack_bytesize = (__bytesize),             \
            };
#   define __vsf_thread_def_stack_member(__name, __bytesize)
#   define __vsf_thread_imp_stack(__name, __thread, __task)                     \
            __vsf_thread_set_stack((__thread), (__task), NULL, (vsf_thread##__name##_stack_bytesize))
#   define __vsf_eda_call_thread_prepare_stack(__name, __thread)                \
            .stack = NULL,                                                      \
            .stack_size = (vsf_thread##__name##_stack_bytesize),
#else
#   define __vsf_thread_def_stack(__name, __bytesize)
#   define __vsf_thread_def_stack_member(__name, __bytesize)                    \
            uint32_t canary;                                                    \
            uint64_t stack_arr[(__VSF_THREAD_STACK_SAFE_SIZE(__bytesize) + 7) / 8]\
                        VSF_CAL_ALIGN(1 << VSF_KERNEL_CFG_THREAD_STACK_ALIGN_BIT);
#   define __vsf_thread_imp_stack(__name, __thread, __task)                     \
            __vsf_thread_set_stack((__thread), (__task), (__task)->stack_arr, sizeof((__task)->stack_arr))
#   define __vsf_eda_call_thread_prepare_stack(__name, __thread)                \
            .stack = (__thread)->stack_arr,                                     \
            .stack_size = sizeof((__thread)->stack_arr),
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   define __def_vsf_thread(__name, __stack_bytesize, ...)                      \
            __vsf_thread_def_stack(__name, (__stack_bytesize))                  \
            struct thread_cb_##__name##_t {                                     \
                implement(vsf_thread_cb_t)                                      \
                __VA_ARGS__                                                     \
                __vsf_thread_def_stack_member(__name, (__stack_bytesize))       \
            };                                                                  \
            struct __name {                                                     \
                implement(vsf_thread_t)                                         \
                implement_ex(thread_cb_##__name##_t, param)                     \
            } VSF_CAL_ALIGN(8);                                                 \
            extern void vsf_thread_##__name##_start(struct __name *task,        \
                                                    vsf_prio_t priority);       \
            extern void vsf_thread_##__name##_entry(                            \
                            struct thread_cb_##__name##_t *vsf_pthis);

#   define __implement_vsf_thread(__name)                                       \
            void vsf_thread_##__name##_entry(                                   \
                        struct thread_cb_##__name##_t *vsf_pthis);              \
            void vsf_thread_##__name##_start( struct __name *task,              \
                                                vsf_prio_t priority)            \
            {                                                                   \
                VSF_KERNEL_ASSERT(NULL != task);                                \
                thread_cb_##__name##_t *__vsf_pthis = &(task->param);           \
                __vsf_pthis->use_as__vsf_thread_cb_t.entry = (vsf_thread_entry_t *)\
                                    &vsf_thread_##__name##_entry;               \
                __vsf_thread_imp_stack(__name, __vsf_pthis, task)               \
                vsf_thread_start(   &(task->use_as__vsf_thread_t),              \
                                    &(__vsf_pthis->use_as__vsf_thread_cb_t),    \
                                    priority);                                  \
            }                                                                   \
            void vsf_thread_##__name##_entry(                                   \
                        struct thread_cb_##__name##_t *vsf_pthis)

#   define __vsf_eda_call_thread_prepare(__name, __thread_cb)                   \
            do {                                                                \
                thread_cb_##__name##_t *__vsf_pthis = (__thread_cb);            \
                const vsf_thread_prepare_cfg_t cfg = {                          \
                    .entry = (vsf_thread_entry_t *)                             \
                                    &vsf_thread_##__name##_entry,               \
                    __vsf_eda_call_thread_prepare_stack(__name, (__thread_cb))  \
                };                                                              \
                vk_eda_call_thread_prepare(&(__vsf_pthis->use_as__vsf_thread_cb_t),\
                                              (vsf_thread_prepare_cfg_t *)&cfg);\
            } while(0)

/**
 * \~english
 * @brief Prepare a thread control block before the current eda calls a thread
 * @param[in] __name: name of the thread type defined by def_vsf_thread
 * @param[in] __thread_cb: a pointer to the thread control block instance,
 *            of type vsf_thread(__name)
 * @return none
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is ENABLED.
 * @note Call vsf_eda_call_thread after the preparation to run the thread and
 *       wait for its completion.
 * @note Must be called in eda task context (e.g. from an eda, pt or task event handler); the caller waits for the called thread to complete.
 * \~chinese
 * @brief 在当前 eda 调用线程之前准备线程控制块
 * @param[in] __name: 由 def_vsf_thread 定义的线程类型名
 * @param[in] __thread_cb: 指向线程控制块实例的指针，类型为 vsf_thread(__name)
 * @return 无
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL 为 ENABLED 时可用。
 * @note 准备完成后调用 vsf_eda_call_thread 运行线程并等待其完成。
 * @note 必须在 eda 任务上下文（如 eda、pt 或 task 的事件处理函数）中调用；调用者会等待被调用的线程执行完成。
 */
#   define vsf_eda_call_thread_prepare(__name, __thread_cb)                     \
                __vsf_eda_call_thread_prepare(__name, __thread_cb)

/**
 * \~english
 * @brief Call a prepared thread from the current eda and wait for its completion
 * @param[in] __thread_cb: a pointer to the thread control block prepared by
 *            vsf_eda_call_thread_prepare
 * @return vsf_err_t: VSF_ERR_NONE if the call is started successfully; VSF_ERR_NOT_ENOUGH_RESOURCES if no frame is available
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is ENABLED.
 * @note Must be called in eda task context (e.g. from an eda, pt or task event handler); the caller waits for the called thread to complete.
 * \~chinese
 * @brief 从当前 eda 调用一个已准备的线程并等待其完成
 * @param[in] __thread_cb: 指向线程控制块的指针，已由
 *            vsf_eda_call_thread_prepare 准备
 * @return vsf_err_t: 调用启动成功返回 VSF_ERR_NONE；没有可用的帧返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL 为 ENABLED 时可用。
 * @note 必须在 eda 任务上下文（如 eda、pt 或 task 的事件处理函数）中调用；调用者会等待被调用的线程执行完成。
 */
#   define vsf_eda_call_thread(__thread_cb)                                     \
                vk_eda_call_thread(&((__thread_cb)->use_as__vsf_thread_cb_t))


#   define __def_vsf_thread_ex(__name, ...)                                     \
            struct thread_cb_##__name##_t {                                     \
                implement(vsf_thread_cb_t)                                      \
                __VA_ARGS__                                                     \
            };                                                                  \
            struct __name {                                                     \
                implement(vsf_thread_t)                                         \
                implement_ex(thread_cb_##__name##_t, param)                     \
            };                                                                  \
            extern void vsf_thread_##__name##_start( struct __name *task,       \
                                                vsf_prio_t priority,            \
                                                void *stack,                    \
                                                uint_fast32_t size);            \
            extern void vsf_thread_##__name##_entry(                            \
                            struct thread_cb_##__name##_t *vsf_pthis);


#   define __implement_vsf_thread_ex(__name)                                    \
            void vsf_thread_##__name##_entry(                                   \
                        struct thread_cb_##__name##_t *vsf_pthis);              \
            void vsf_thread_##__name##_start(   struct __name *task,            \
                                                vsf_prio_t priority,            \
                                                void *stack,                    \
                                                uint_fast32_t size)             \
            {                                                                   \
                VSF_KERNEL_ASSERT(NULL != task && 0 != size && NULL != stack);  \
                thread_cb_##__name##_t *__vsf_pthis = &(task->param);           \
                __vsf_pthis->use_as__vsf_thread_cb_t.entry = (vsf_thread_entry_t *)\
                                    &vsf_thread_##__name##_entry;               \
                __vsf_pthis->use_as__vsf_thread_cb_t.stack = stack;             \
                __vsf_pthis->use_as__vsf_thread_cb_t.stack_size = size;         \
                vsf_thread_start(   &(task->use_as__vsf_thread_t),              \
                                    &(task->param.use_as__vsf_thread_cb_t),     \
                                    priority);                                  \
            }                                                                   \
            void vsf_thread_##__name##_entry(                                   \
                        struct thread_cb_##__name##_t *vsf_pthis)

#   define __vsf_eda_call_thread_prepare_ex__(  __name,                         \
                                                __thread_cb,                    \
                                                __stack,                        \
                                                __stack_bytesize)               \
            do {                                                                \
                VSF_KERNEL_ASSERT((NULL != (__stack)) && (0 != (__size)))       \
                thread_cb_##__name##_t *__vsf_pthis = (__thread_cb);            \
                const vsf_thread_prepare_cfg_t cfg = {                          \
                    .entry = (vsf_thread_entry_t *)                             \
                                    &vsf_thread_##__name##_entry,               \
                    .stack = (__stack),                                         \
                    .stack_size = (__stack_bytesize),                           \
                };                                                              \
                vk_eda_call_thread_prepare(&(__vsf_pthis->use_as__vsf_thread_cb_t),\
                                              (vsf_thread_prepare_cfg_t *)&cfg);\
            } while(0)


/**
 * \~english
 * @brief Prepare a thread control block with an external stack before the
 *        current eda calls a thread
 * @param[in] __name: name of the thread type defined by def_vsf_thread_ex
 * @param[in] __thread_cb: a pointer to the thread control block instance,
 *            of type vsf_thread(__name)
 * @param[in] __stack: a pointer to the external stack
 * @param[in] __stack_bytesize: size of the external stack in bytes
 * @return none
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is ENABLED.
 * @note Call vsf_eda_call_thread_ex after the preparation to run the thread
 *       and wait for its completion.
 * @note Must be called in eda task context (e.g. from an eda, pt or task event handler); the caller waits for the called thread to complete.
 * \~chinese
 * @brief 在当前 eda 调用线程之前，使用外部堆栈准备线程控制块
 * @param[in] __name: 由 def_vsf_thread_ex 定义的线程类型名
 * @param[in] __thread_cb: 指向线程控制块实例的指针，类型为 vsf_thread(__name)
 * @param[in] __stack: 指向外部堆栈的指针
 * @param[in] __stack_bytesize: 外部堆栈的字节大小
 * @return 无
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL 为 ENABLED 时可用。
 * @note 准备完成后调用 vsf_eda_call_thread_ex 运行线程并等待其完成。
 * @note 必须在 eda 任务上下文（如 eda、pt 或 task 的事件处理函数）中调用；调用者会等待被调用的线程执行完成。
 */
#   define vsf_eda_call_thread_prepare_ex(  __name,                             \
                                            __thread_cb,                        \
                                            __stack,                            \
                                            __stack_bytesize)                   \
                __vsf_eda_call_thread_prepare_ex__( __name,                     \
                                                    (__thread_cb),              \
                                                    (__stack),                  \
                                                    (__stack_bytesize))

/**
 * \~english
 * @brief Call a prepared thread (defined by def_vsf_thread_ex) from the current
 *        eda and wait for its completion
 * @param[in] __thread_cb: a pointer to the thread control block prepared by
 *            vsf_eda_call_thread_prepare_ex
 * @return vsf_err_t: VSF_ERR_NONE if the call is started successfully; VSF_ERR_NOT_ENOUGH_RESOURCES if no frame is available
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is ENABLED.
 * @note Must be called in eda task context (e.g. from an eda, pt or task event handler); the caller waits for the called thread to complete.
 * \~chinese
 * @brief 从当前 eda 调用一个已准备的线程（由 def_vsf_thread_ex 定义）并等待其完成
 * @param[in] __thread_cb: 指向线程控制块的指针，已由
 *            vsf_eda_call_thread_prepare_ex 准备
 * @return vsf_err_t: 调用启动成功返回 VSF_ERR_NONE；没有可用的帧返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL 为 ENABLED 时可用。
 * @note 必须在 eda 任务上下文（如 eda、pt 或 task 的事件处理函数）中调用；调用者会等待被调用的线程执行完成。
 */
#   define vsf_eda_call_thread_ex(__thread_cb)                                  \
                vk_eda_call_thread(&((__thread_cb)->use_as__vsf_thread_cb_t))

#else
#   define __def_vsf_thread(__name, __stack_bytesize, ...)                      \
            struct thread_cb_##__name##_t {                                     \
                implement(vsf_thread_t)                                         \
                __VA_ARGS__                                                     \
            };                                                                  \
            __vsf_thread_def_stack(__name, (__stack_bytesize))                  \
            struct __name {                                                     \
                __vsf_thread_def_stack_member(__name, (__stack_bytesize))       \
                implement_ex(thread_cb_##__name##_t, param);                    \
            } VSF_CAL_ALIGN(8);                                                 \
            extern void vsf_thread_##__name##_start(struct __name *task,        \
                                                    vsf_prio_t priority);       \
            extern void vsf_thread_##__name##_entry(                            \
                        struct thread_cb_##__name##_t *vsf_pthis);

#   define __implement_vsf_thread(__name)                                       \
            void vsf_thread_##__name##_entry(                                   \
                        struct thread_cb_##__name##_t *vsf_pthis);              \
            void vsf_thread_##__name##_start( struct __name *task,              \
                                                vsf_prio_t priority)            \
            {                                                                   \
                VSF_KERNEL_ASSERT(NULL != task);                                \
                vsf_thread_t *__vsf_pthis =                                     \
                    &(task->param.use_as__vsf_thread_t);                        \
                __vsf_pthis->entry = (vsf_thread_entry_t *)                     \
                                    &vsf_thread_##__name##_entry;               \
                __vsf_thread_imp_stack(__name, __vsf_pthis, task)               \
                vsf_thread_start(__vsf_pthis, priority);                        \
            }                                                                   \
            void vsf_thread_##__name##_entry(                                   \
                        struct thread_cb_##__name##_t *vsf_pthis)

#   define __def_vsf_thread_ex(__name, ...)                                     \
            struct thread_cb_##__name##_t {                                     \
                implement(vsf_thread_t)                                         \
                __VA_ARGS__                                                     \
            };                                                                  \
            struct __name {                                                     \
                implement_ex(thread_cb_##__name##_t, param);                    \
            } VSF_CAL_ALIGN(8);                                                 \
            extern void vsf_thread_##__name##_start(struct __name *task,        \
                                                    vsf_prio_t priority);       \
            extern void vsf_thread_##__name##_entry(                            \
                        struct thread_cb_##__name##_t *vsf_pthis);


#   define __implement_vsf_thread_ex(__name)                                    \
            void vsf_thread_##__name##_entry(                                   \
                        struct thread_cb_##__name##_t *vsf_pthis);              \
            void vsf_thread_##__name##_start(   struct __name *task,            \
                                                vsf_prio_t priority,            \
                                                void *stack,                    \
                                                uint_fast32_t size)             \
            {                                                                   \
                VSF_KERNEL_ASSERT(NULL != task && 0 != size && NULL != stack);  \
                vsf_thread_t *__vsf_pthis =                                     \
                    &(task->param.use_as__vsf_thread_t);                        \
                __vsf_pthis->entry = (vsf_thread_entry_t *)                     \
                                    &vsf_thread_##__name##_entry;               \
                __vsf_pthis->stack = stack;                                     \
                __vsf_pthis->stack_size = size;                                 \
                vsf_thread_start(__vsf_pthis, priority);                        \
            }                                                                   \
            void vsf_thread_##__name##_entry(                                   \
                        struct thread_cb_##__name##_t *vsf_pthis)

#endif


/**
 * \~english
 * @brief Define a thread type with embedded stack
 * @param[in] __name: name of the thread type, declared by declare_vsf_thread
 * @param[in] __stack_bytesize: size of the embedded stack in bytes
 * @param[in] ...: additional member variables bound to the thread, accessible
 *            via vsf_this/vsf_pthis in the thread entry
 *
 * @note Generates the thread instance type __name, the thread control block
 *       type thread_cb_##__name##_t, and the start function
 *       vsf_thread_##__name##_start(struct __name *task, vsf_prio_t priority).
 * \~chinese
 * @brief 定义一个内嵌堆栈的线程类型
 * @param[in] __name: 线程类型名，由 declare_vsf_thread 声明
 * @param[in] __stack_bytesize: 内嵌堆栈的字节大小
 * @param[in] ...: 绑定到线程的附加成员变量，在线程入口中可通过
 *            vsf_this/vsf_pthis 访问
 *
 * @note 生成线程实例类型 __name、线程控制块类型 thread_cb_##__name##_t，
 *       以及启动函数
 *       vsf_thread_##__name##_start(struct __name *task, vsf_prio_t priority)。
 */
#define def_vsf_thread(__name, __stack_bytesize, ...)                           \
            __def_vsf_thread(__name, (__stack_bytesize), __VA_ARGS__)

/**
 * \~english
 * @brief Alias of def_vsf_thread
 * @param[in] __name: name of the thread type, declared by declare_vsf_thread
 * @param[in] __stack_bytesize: size of the embedded stack in bytes
 * @param[in] ...: additional member variables bound to the thread
 * \~chinese
 * @brief def_vsf_thread 的别名
 * @param[in] __name: 线程类型名，由 declare_vsf_thread 声明
 * @param[in] __stack_bytesize: 内嵌堆栈的字节大小
 * @param[in] ...: 绑定到线程的附加成员变量
 */
#define define_vsf_thread(__name, __stack_bytesize, ...)                        \
            def_vsf_thread(__name, (__stack_bytesize), __VA_ARGS__)

/**
 * \~english
 * @brief Define a thread type using an external stack
 * @param[in] __name: name of the thread type, declared by declare_vsf_thread_ex
 * @param[in] ...: additional member variables bound to the thread, accessible
 *            via vsf_this/vsf_pthis in the thread entry
 *
 * @note No stack is embedded; the generated start function takes the external
 *       stack: vsf_thread_##__name##_start(struct __name *task,
 *       vsf_prio_t priority, void *stack, uint_fast32_t size).
 * \~chinese
 * @brief 定义一个使用外部堆栈的线程类型
 * @param[in] __name: 线程类型名，由 declare_vsf_thread_ex 声明
 * @param[in] ...: 绑定到线程的附加成员变量，在线程入口中可通过
 *            vsf_this/vsf_pthis 访问
 *
 * @note 不内嵌堆栈；生成的启动函数接收外部堆栈：
 *       vsf_thread_##__name##_start(struct __name *task,
 *       vsf_prio_t priority, void *stack, uint_fast32_t size)。
 */
#define def_vsf_thread_ex(__name, ...)                                          \
            __def_vsf_thread_ex(__name, __VA_ARGS__)

/**
 * \~english
 * @brief Alias of def_vsf_thread_ex
 * @param[in] __name: name of the thread type, declared by declare_vsf_thread_ex
 * @param[in] ...: additional member variables bound to the thread
 * \~chinese
 * @brief def_vsf_thread_ex 的别名
 * @param[in] __name: 线程类型名，由 declare_vsf_thread_ex 声明
 * @param[in] ...: 绑定到线程的附加成员变量
 */
#define define_vsf_thread_ex(__name, ...)                                       \
            def_vsf_thread_ex(__name, __VA_ARGS__)

/**
 * \~english
 * @brief Mark the end of a thread definition, no-op, for readability only
 * \~chinese
 * @brief 标记线程定义的结束，空操作，仅用于提高可读性
 */
#define end_def_vsf_thread(...)
/**
 * \~english
 * @brief Alias of end_def_vsf_thread
 * \~chinese
 * @brief end_def_vsf_thread 的别名
 */
#define end_define_vsf_thread(...)

/**
 * \~english
 * @brief Implement the entry body of a thread type
 * @param[in] __name: name of the thread type defined by def_vsf_thread
 *
 * @note The thread entry body follows this macro in braces; use
 *       vsf_this/vsf_pthis to access the members bound in def_vsf_thread.
 * \~chinese
 * @brief 实现线程类型的入口函数体
 * @param[in] __name: 由 def_vsf_thread 定义的线程类型名
 *
 * @note 线程入口函数体写在此宏之后的大括号中；使用 vsf_this/vsf_pthis
 *       访问 def_vsf_thread 中绑定的成员。
 */
#define implement_vsf_thread(__name)        __implement_vsf_thread(__name)
/**
 * \~english
 * @brief Implement the entry body of a thread type using an external stack
 * @param[in] __name: name of the thread type defined by def_vsf_thread_ex
 *
 * @note The thread entry body follows this macro in braces; use
 *       vsf_this/vsf_pthis to access the members bound in def_vsf_thread_ex.
 * \~chinese
 * @brief 实现使用外部堆栈线程类型的入口函数体
 * @param[in] __name: 由 def_vsf_thread_ex 定义的线程类型名
 *
 * @note 线程入口函数体写在此宏之后的大括号中；使用 vsf_this/vsf_pthis
 *       访问 def_vsf_thread_ex 中绑定的成员。
 */
#define implement_vsf_thread_ex(__name)     __implement_vsf_thread_ex(__name)

/**
 * \~english
 * @brief Alias of implement_vsf_thread
 * @param[in] __name: name of the thread type defined by def_vsf_thread
 * \~chinese
 * @brief implement_vsf_thread 的别名
 * @param[in] __name: 由 def_vsf_thread 定义的线程类型名
 */
#define imp_vsf_thread(__name)              implement_vsf_thread(__name)
/**
 * \~english
 * @brief Alias of implement_vsf_thread_ex
 * @param[in] __name: name of the thread type defined by def_vsf_thread_ex
 * \~chinese
 * @brief implement_vsf_thread_ex 的别名
 * @param[in] __name: 由 def_vsf_thread_ex 定义的线程类型名
 */
#define imp_vsf_thread_ex(__name)           implement_vsf_thread_ex(__name)

#define __init_vsf_thread(__name, __task, __pri)                                \
            vsf_thread_##__name##_start((__task), (__pri))

#define __init_vsf_thread_ex(__name, __task, __pri, __stack, __stack_bytesize)  \
            vsf_thread_##__name##_start((__task), (__pri), (__stack), (__stack_bytesize))

/**
 * \~english
 * @brief Initialize and start an instance of a thread type with embedded stack
 * @param[in] __name: name of the thread type defined by def_vsf_thread
 * @param[in] __task: a pointer to the thread instance of type __name
 * @param[in] __pri: priority of the thread: vsf_prio_0, vsf_prio_1, ... (in increasing priority order), or vsf_prio_inherit (inherit the current event queue's priority)
 * @return none
 *
 * @note Calls the generated vsf_thread_##__name##_start function.
 * @note Must be called in task (or initialization) context, NOT in interrupt context.
 * \~chinese
 * @brief 初始化并启动一个内嵌堆栈线程类型的实例
 * @param[in] __name: 由 def_vsf_thread 定义的线程类型名
 * @param[in] __task: 指向 __name 类型线程实例的指针
 * @param[in] __pri: 线程的优先级，vsf_prio_0、vsf_prio_1 ……优先级递增，或 vsf_prio_inherit（继承当前事件队列的优先级）
 * @return 无
 *
 * @note 调用生成的 vsf_thread_##__name##_start 函数。
 * @note 必须在任务（或初始化）上下文中调用，禁止在中断上下文中调用。
 */
#define init_vsf_thread(__name, __task, __pri)                                  \
            __init_vsf_thread(__name, (__task), (__pri))

/**
 * \~english
 * @brief Initialize and start an instance of a thread type with an external stack
 * @param[in] __name: name of the thread type defined by def_vsf_thread_ex
 * @param[in] __task: a pointer to the thread instance of type __name
 * @param[in] __pri: priority of the thread: vsf_prio_0, vsf_prio_1, ... (in increasing priority order), or vsf_prio_inherit (inherit the current event queue's priority)
 * @param[in] __stack: a pointer to the external stack
 * @param[in] __stack_bytesize: size of the external stack in bytes
 * @return none
 *
 * @note Calls the generated vsf_thread_##__name##_start function.
 * @note Must be called in task (or initialization) context, NOT in interrupt context.
 * \~chinese
 * @brief 初始化并启动一个使用外部堆栈线程类型的实例
 * @param[in] __name: 由 def_vsf_thread_ex 定义的线程类型名
 * @param[in] __task: 指向 __name 类型线程实例的指针
 * @param[in] __pri: 线程的优先级，vsf_prio_0、vsf_prio_1 ……优先级递增，或 vsf_prio_inherit（继承当前事件队列的优先级）
 * @param[in] __stack: 指向外部堆栈的指针
 * @param[in] __stack_bytesize: 外部堆栈的字节大小
 * @return 无
 *
 * @note 调用生成的 vsf_thread_##__name##_start 函数。
 * @note 必须在任务（或初始化）上下文中调用，禁止在中断上下文中调用。
 */
#define init_vsf_thread_ex(__name, __task, __pri, __stack, __stack_bytesize)    \
            __init_vsf_thread_ex(__name, (__task), (__pri), (__stack), (__stack_bytesize))

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   define __vsf_thread(__name)      thread_cb_##__name##_t
/**
 * \~english
 * @brief Get the thread control block type of a thread type
 * @param[in] __name: name of the thread type defined by def_vsf_thread
 *
 * @note Expands to thread_cb_##__name##_t. Only available when
 *       VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is ENABLED.
 * \~chinese
 * @brief 获取线程类型对应的线程控制块类型
 * @param[in] __name: 由 def_vsf_thread 定义的线程类型名
 *
 * @note 展开为 thread_cb_##__name##_t。仅在
 *       VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL 为 ENABLED 时可用。
 */
#   define vsf_thread(__name)        __vsf_thread(__name)

#endif

/**
 * \~english
 * @brief Short alias of vsf_thread_wait_for_msg
 * @note Must be called in thread context; the current thread is blocked until the operation completes.
 * \~chinese
 * @brief vsf_thread_wait_for_msg 的简写别名
 * @note 必须在线程上下文中调用；当前线程会被阻塞直到操作完成。
 */
#define vsf_thread_wfm      vsf_thread_wait_for_msg
/**
 * \~english
 * @brief Short alias of vsf_thread_wait_for_evt
 * @note Must be called in thread context; the current thread is blocked until the operation completes.
 * \~chinese
 * @brief vsf_thread_wait_for_evt 的简写别名
 * @note 必须在线程上下文中调用；当前线程会被阻塞直到操作完成。
 */
#define vsf_thread_wfe      vsf_thread_wait_for_evt

#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
/**
 * \~english
 * @brief Short alias of vsf_thread_wait_for_evt_msg
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE is ENABLED.
 * @note Must be called in thread context; the current thread is blocked until the operation completes.
 * \~chinese
 * @brief vsf_thread_wait_for_evt_msg 的简写别名
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE 为 ENABLED 时可用。
 * @note 必须在线程上下文中调用；当前线程会被阻塞直到操作完成。
 */
#   define vsf_thread_wfem     vsf_thread_wait_for_evt_msg
#endif


#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   define __vsf_thread_call_sub(__name, __target, ...)                         \
            vk_thread_call_eda( (uintptr_t)(__name),                            \
                                (uintptr_t)(__target),                          \
                                (0, ##__VA_ARGS__),                             \
                                0,                                              \
                                0)


/**
 * \~english
 * @brief Call an eda (event-driven task) sub-routine from a thread and wait
 *        for its completion
 * @param[in] __name: entry function of the target eda
 * @param[in] __target: a pointer to the target eda instance
 * @param[in] ...: optional size of the local variables of the target eda
 * @return vsf_err_t: always VSF_ERR_NONE (frame exhaustion triggers an assertion in debug builds)
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is ENABLED.
 * @note Must be called in thread context; the current thread is blocked until the called task completes.
 * \~chinese
 * @brief 从线程调用一个 eda（事件驱动任务）子例程并等待其完成
 * @param[in] __name: 目标 eda 的入口函数
 * @param[in] __target: 指向目标 eda 实例的指针
 * @param[in] ...: 可选的目标 eda 局部变量大小
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE（帧不足时在调试版本中触发断言）
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL 为 ENABLED 时可用。
 * @note 必须在线程上下文中调用；当前线程会被阻塞直到被调用的任务执行完成。
 */
#   define vsf_thread_call_sub(__name, __target, ...)                           \
            __vsf_thread_call_sub(__name, (__target), (0, ##__VA_ARGS__))


/**
 * \~english
 * @brief Call a pt (protothread) from a thread and wait for its completion
 * @param[in] __name: name of the target protothread defined by def_vsf_pt()
 * @param[in] __target: a pointer to the target pt instance
 * @param[in] ...: optional size of the local variables of the target pt
 * @return vsf_err_t: always VSF_ERR_NONE (frame exhaustion triggers an assertion in debug builds)
 *
 * @note The fsm_state of the target pt is reset before the call.
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is ENABLED.
 * @note Must be called in thread context; the current thread is blocked until the called task completes.
 * \~chinese
 * @brief 从线程调用一个 pt（protothread）并等待其完成
 * @param[in] __name: 目标 pt 的名称（由 def_vsf_pt() 定义）
 * @param[in] __target: 指向目标 pt 实例的指针
 * @param[in] ...: 可选的目标 pt 局部变量大小
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE（帧不足时在调试版本中触发断言）
 *
 * @note 调用前会复位目标 pt 的 fsm_state。
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL 为 ENABLED 时可用。
 * @note 必须在线程上下文中调用；当前线程会被阻塞直到被调用的任务执行完成。
 */
#   define vsf_thread_call_pt(__name, __target, ...)                            \
            (__target)->fsm_state = 0;                                          \
            vsf_thread_call_sub(vsf_pt_func(__name), (__target), (0, ##__VA_ARGS__))

#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_TASK == ENABLED && VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
/**
 * \~english
 * @brief Call a task from a thread and wait for its completion
 * @param[in] __name: entry function of the target task
 * @param[in] __target: a pointer to the target task instance
 * @param[in] ...: optional size of the local variables of the target task
 * @return fsm_rt_t: the final state of the called task
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_TASK and
 *       VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE are ENABLED.
 * @note Must be called in thread context; the current thread is blocked until the called task completes.
 * \~chinese
 * @brief 从线程调用一个任务并等待其完成
 * @param[in] __name: 目标任务的入口函数
 * @param[in] __target: 指向目标任务实例的指针
 * @param[in] ...: 可选的目标任务局部变量大小
 * @return fsm_rt_t: 被调用任务的最终状态
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_TASK 和
 *       VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE 为 ENABLED 时可用。
 * @note 必须在线程上下文中调用；当前线程会被阻塞直到被调用的任务执行完成。
 */
#   define vsf_thread_call_task(__name, __target, ...)                          \
                vk_thread_call_task(vsf_task_func(__name), __target, (0, ##__VA_ARGS__))
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
/**
 * \~english
 * @brief Delay (sleep) the current thread for the given milliseconds
 * @param[in] __ms: delay time in milliseconds
 * @return none
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_TIMER is ENABLED.
 * @note Must be called in thread context; the current thread is blocked until the operation completes.
 * \~chinese
 * @brief 将当前线程延时（睡眠）指定的毫秒数
 * @param[in] __ms: 延时时间（单位：毫秒）
 * @return 无
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_TIMER 为 ENABLED 时可用。
 * @note 必须在线程上下文中调用；当前线程会被阻塞直到操作完成。
 */
#   define vsf_thread_delay_ms(__ms)                vsf_thread_delay(vsf_systimer_ms_to_tick(__ms))
/**
 * \~english
 * @brief Delay (sleep) the current thread for the given microseconds
 * @param[in] __us: delay time in microseconds
 * @return none
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_TIMER is ENABLED.
 * @note Must be called in thread context; the current thread is blocked until the operation completes.
 * \~chinese
 * @brief 将当前线程延时（睡眠）指定的微秒数
 * @param[in] __us: 延时时间（单位：微秒）
 * @return 无
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_TIMER 为 ENABLED 时可用。
 * @note 必须在线程上下文中调用；当前线程会被阻塞直到操作完成。
 */
#   define vsf_thread_delay_us(__us)                vsf_thread_delay(vsf_systimer_us_to_tick(__us))
#endif

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
/**
 * \~english
 * @brief Post a semaphore inside a thread, non-blocking alias of vsf_eda_sem_post
 * @param[in] __sem: a pointer to structure @ref vsf_sem_t
 * @return vsf_err_t: VSF_ERR_NONE if successful; VSF_ERR_OVERRUN if the count has reached the maximum
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 在线程中发送信号量，vsf_eda_sem_post 的非阻塞别名
 * @param[in] __sem: 指向结构体 @ref vsf_sem_t 的指针
 * @return vsf_err_t: 成功返回 VSF_ERR_NONE；计数已达到最大值返回 VSF_ERR_OVERRUN
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
#   define vsf_thread_sem_post(__sem)               vsf_eda_sem_post(__sem)
/**
 * \~english
 * @brief Pend on a semaphore inside a thread
 * @param[in] __sem: a pointer to structure @ref vsf_sem_t
 * @param[in] timeout: timeout in ticks; negative to wait forever, 0 to try once, a positive value to wait up to the given ticks
 * @return vsf_sync_reason_t: VSF_SYNC_GET if the semaphore is obtained,
 *         VSF_SYNC_TIMEOUT if timed out, VSF_SYNC_CANCEL if cancelled
 *
 * @note The current thread is blocked while waiting.
 * @note Must be called in thread context; the current thread is blocked until the operation completes.
 * \~chinese
 * @brief 在线程中等待信号量
 * @param[in] __sem: 指向结构体 @ref vsf_sem_t 的指针
 * @param[in] timeout: 超时时间（单位：tick），负数表示永久等待，0 表示只尝试一次，正数表示最多等待对应的 tick 数
 * @return vsf_sync_reason_t: 获取到信号量返回 VSF_SYNC_GET，超时返回
 *         VSF_SYNC_TIMEOUT，被取消返回 VSF_SYNC_CANCEL
 *
 * @note 等待时当前线程会被阻塞。
 * @note 必须在线程上下文中调用；当前线程会被阻塞直到操作完成。
 */
#   define vsf_thread_sem_pend(__sem, timeout)      __vsf_thread_wait_for_sync(__sem, timeout)

/**
 * \~english
 * @brief Set a trigger inside a thread, non-blocking alias of vsf_eda_trig_set
 * @param[in] __trig: a pointer to structure @ref vsf_trig_t
 * @param[in] ...: optional bool, true to set the trigger as manual-reset
 * @return vsf_err_t: VSF_ERR_NONE if successful; VSF_ERR_OVERRUN if the trigger is already set
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 在线程中设置触发器，vsf_eda_trig_set 的非阻塞别名
 * @param[in] __trig: 指向结构体 @ref vsf_trig_t 的指针
 * @param[in] ...: 可选的 bool 值，true 表示将触发器设置为手动复位
 * @return vsf_err_t: 成功返回 VSF_ERR_NONE；触发器已是置位状态返回 VSF_ERR_OVERRUN
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
#   define vsf_thread_trig_set(__trig, ...)         vsf_eda_trig_set(__trig, ##__VA_ARGS__)
/**
 * \~english
 * @brief Reset a trigger inside a thread, alias of vsf_eda_trig_reset
 * @param[in] __trig: a pointer to structure @ref vsf_trig_t
 * @return none
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 在线程中复位触发器，vsf_eda_trig_reset 的别名
 * @param[in] __trig: 指向结构体 @ref vsf_trig_t 的指针
 * @return 无
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
#   define vsf_thread_trig_reset(__trig)            vsf_eda_trig_reset(__trig)
/**
 * \~english
 * @brief Pend on a trigger inside a thread
 * @param[in] __trig: a pointer to structure @ref vsf_trig_t
 * @param[in] timeout: timeout in ticks; negative to wait forever, 0 to try once, a positive value to wait up to the given ticks
 * @return vsf_sync_reason_t: VSF_SYNC_GET if the trigger is set,
 *         VSF_SYNC_TIMEOUT if timed out, VSF_SYNC_CANCEL if cancelled
 *
 * @note The current thread is blocked while waiting.
 * @note Must be called in thread context; the current thread is blocked until the operation completes.
 * \~chinese
 * @brief 在线程中等待触发器
 * @param[in] __trig: 指向结构体 @ref vsf_trig_t 的指针
 * @param[in] timeout: 超时时间（单位：tick），负数表示永久等待，0 表示只尝试一次，正数表示最多等待对应的 tick 数
 * @return vsf_sync_reason_t: 触发器被设置返回 VSF_SYNC_GET，超时返回
 *         VSF_SYNC_TIMEOUT，被取消返回 VSF_SYNC_CANCEL
 *
 * @note 等待时当前线程会被阻塞。
 * @note 必须在线程上下文中调用；当前线程会被阻塞直到操作完成。
 */
#   define vsf_thread_trig_pend(__trig, timeout)    __vsf_thread_wait_for_sync(__trig, timeout)

/**
 * \~english
 * @brief Enter (acquire) a mutex inside a thread
 * @param[in] __mtx: a pointer to structure @ref vsf_mutex_t
 * @param[in] timeout: timeout in ticks; negative to wait forever, 0 to try once, a positive value to wait up to the given ticks
 * @return vsf_sync_reason_t: VSF_SYNC_GET if the mutex is acquired,
 *         VSF_SYNC_TIMEOUT if timed out, VSF_SYNC_CANCEL if cancelled
 *
 * @note The current thread is blocked while waiting. Use vsf_thread_mutex_leave
 *       to release the mutex.
 * @note Must be called in thread context; the current thread is blocked until the operation completes.
 * \~chinese
 * @brief 在线程中进入（获取）互斥量
 * @param[in] __mtx: 指向结构体 @ref vsf_mutex_t 的指针
 * @param[in] timeout: 超时时间（单位：tick），负数表示永久等待，0 表示只尝试一次，正数表示最多等待对应的 tick 数
 * @return vsf_sync_reason_t: 获取到互斥量返回 VSF_SYNC_GET，超时返回
 *         VSF_SYNC_TIMEOUT，被取消返回 VSF_SYNC_CANCEL
 *
 * @note 等待时当前线程会被阻塞。使用 vsf_thread_mutex_leave 释放互斥量。
 * @note 必须在线程上下文中调用；当前线程会被阻塞直到操作完成。
 */
#   define vsf_thread_mutex_enter(__mtx, timeout)   __vsf_thread_wait_for_sync(&(__mtx)->use_as__vsf_sync_t, timeout)
#endif

/*============================ TYPES =========================================*/

declare_class(vsf_thread_t)

#   if VSF_KERNEL_CFG_THREAD_SIGNAL == ENABLED
/**
 * \~english
 * @brief POSIX-like thread signal handler, invoked when a signal is delivered
 *        to the thread
 * @param[in] thread: the thread that receives the signal
 * @param[in] sig: the signal number
 *
 * @note Requires VSF_KERNEL_CFG_THREAD_SIGNAL to be ENABLED.
 * \~chinese
 * @brief 类 POSIX 的线程信号处理函数，当信号递送到线程时被调用
 * @param[in] thread: 接收信号的线程
 * @param[in] sig: 信号编号
 *
 * @note 需要启用 VSF_KERNEL_CFG_THREAD_SIGNAL。
 */
typedef void vsf_thread_sighandler_t(vsf_thread_t *thread, int sig);
#   endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
declare_class(vsf_thread_cb_t)
/**
 * \~english
 * @brief Thread entry function, called on the thread's own stack
 * @param[in] thread: the thread control block @ref vsf_thread_cb_t
 *            (when VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is ENABLED)
 * \~chinese
 * @brief 线程入口函数，在线程自己的栈上被调用
 * @param[in] thread: 线程控制块 @ref vsf_thread_cb_t
 *            （当启用 VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL 时）
 */
typedef void vsf_thread_entry_t(vsf_thread_cb_t *thread);
#else
/**
 * \~english
 * @brief Thread entry function, called on the thread's own stack
 * @param[in] thread: the thread @ref vsf_thread_t
 *            (when VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is DISABLED)
 * \~chinese
 * @brief 线程入口函数，在线程自己的栈上被调用
 * @param[in] thread: 线程 @ref vsf_thread_t
 *            （当禁用 VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL 时）
 */
typedef void vsf_thread_entry_t(vsf_thread_t *thread);
#endif

//! \name thread control block
//! @{
/**
 * \~english
 * @brief Thread control block of an independent-stack thread
 *
 * @note The control block must be initialized before vsf_thread_start() is
 *       called; it is usually filled by the def_vsf_thread() /
 *       implement_vsf_thread() macros.
 * \~chinese
 * @brief 独立栈线程的线程控制块
 *
 * @note 控制块必须在调用 vsf_thread_start() 之前初始化；通常由
 *       def_vsf_thread() / implement_vsf_thread() 宏填充。
 */
def_class(vsf_thread_cb_t,

    public_member(
        //! \~english thread entry function
        //! \~chinese 线程入口函数
        vsf_thread_entry_t      *entry;
        //! \~english stack size in bytes
        //! \~chinese 栈大小（单位：字节）
#if VSF_KERNEL_CFG_THREAD_STACK_LARGE == ENABLED
        uint32_t                stack_size;
#else
        uint16_t                stack_size;
#endif
        //! \~english stack pointer, MUST be 8-byte aligned
        //! \~chinese 栈指针，必须 8 字节对齐
        uint64_t                *stack;                 //!< stack must be 8byte aligned
    )

    private_member(
#if VSF_KERNEL_THREAD_USE_HOST == ENABLED
        vsf_arch_irq_thread_t   host_thread;
        vsf_arch_irq_request_t  req, *rep;
        vsf_evt_t               evt;
#   if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
        bool                    is_inited;
#   endif
#else
        jmp_buf                 *pos;
        jmp_buf                 *ret;
#endif
    )
)
end_def_class(vsf_thread_cb_t)
//! @}

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED

/**
 * \~english
 * @brief An independent-stack thread, derived from @ref vsf_teda_t
 *        (or @ref vsf_eda_t when timer support is disabled); blocking calls
 *        suspend only the calling thread
 *
 * @note This variant is used when VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is
 *       ENABLED; the thread control block is allocated separately as
 *       @ref vsf_thread_cb_t.
 * \~chinese
 * @brief 独立栈线程，派生自 @ref vsf_teda_t（禁用定时器支持时为
 *        @ref vsf_eda_t）；阻塞调用只挂起调用它的线程
 *
 * @note 此变体在启用 VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL 时使用；线程控制块
 *       作为 @ref vsf_thread_cb_t 单独分配。
 */
def_class( vsf_thread_t,
    public_member(
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
        implement(vsf_teda_t)
#else
        implement(vsf_eda_t)
#endif
#if VSF_KERNEL_CFG_THREAD_SIGNAL == ENABLED
        vsf_thread_sighandler_t *sighandler;
#endif
    )
#if VSF_KERNEL_CFG_THREAD_SIGNAL == ENABLED && VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    private_member(
        // set sig_pending if current thread is in subcall,
        //  sighandler will be called if sig_pending is set and subcall returns.
        VSF_KERNEL_CFG_THREAD_SIGNAL_MASK_T sig_pending;
    )
#endif
)
end_def_class(vsf_thread_t)

/**
 * \~english
 * @brief Preparation configuration for calling a thread from an eda
 *        (vk_eda_call_thread_prepare() / vk_thread_call_thread())
 *
 * @note All members must be set; the stack must be 8-byte aligned.
 * \~chinese
 * @brief 从 eda 调用线程时的准备配置
 *        （vk_eda_call_thread_prepare() / vk_thread_call_thread()）
 *
 * @note 所有成员都必须设置；栈必须 8 字节对齐。
 */
typedef struct {
    //! \~english thread entry function
    //! \~chinese 线程入口函数
    vsf_thread_entry_t          *entry;
    //! \~english stack pointer, MUST be 8-byte aligned
    //! \~chinese 栈指针，必须 8 字节对齐
    void                        *stack;
    //! \~english stack size in bytes
    //! \~chinese 栈大小（单位：字节）
    uint_fast32_t               stack_size;
    //vsf_prio_t                  priority;         //!< TODO do we need this??
} vsf_thread_prepare_cfg_t;


#else

//! \name thread
//! @{
/**
 * \~english
 * @brief An independent-stack thread, derived from @ref vsf_teda_t
 *        (or @ref vsf_eda_t when timer support is disabled); blocking calls
 *        suspend only the calling thread
 *
 * @note This variant is used when VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is
 *       DISABLED; the thread control block @ref vsf_thread_cb_t is embedded
 *       in the thread.
 * \~chinese
 * @brief 独立栈线程，派生自 @ref vsf_teda_t（禁用定时器支持时为
 *        @ref vsf_eda_t）；阻塞调用只挂起调用它的线程
 *
 * @note 此变体在禁用 VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL 时使用；线程控制块
 *       @ref vsf_thread_cb_t 内嵌在线程中。
 */
def_class(vsf_thread_t,
    public_member(
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
        implement(vsf_teda_t)
#else
        implement(vsf_eda_t)
#endif
#if VSF_KERNEL_CFG_THREAD_SIGNAL == ENABLED
        vsf_thread_sighandler_t *sighandler;
#endif
        implement(vsf_thread_cb_t)
    )

#if VSF_KERNEL_CFG_THREAD_SIGNAL == ENABLED && VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    private_member(
        // set sig_pending if current thread is in subcall,
        //  sighandler will be called if sig_pending is set and subcall returns.
        VSF_KERNEL_CFG_THREAD_SIGNAL_MASK_T sig_pending;
    )
#endif
)
end_def_class(vsf_thread_t)
//! @}
#endif
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
VSF_CAL_SECTION(".text.vsf.kernel.vk_eda_call_thread_prepare")
/**
 * \~english
 * @brief Prepare a thread control block before an eda calls a thread
 * @param[in] thread_cb: a pointer to the thread control block @ref vsf_thread_cb_t
 * @param[in] cfg: a pointer to the prepare configuration @ref vsf_thread_prepare_cfg_t,
 *            including entry, stack and stack_size
 * @return vsf_err_t: VSF_ERR_NONE if the preparation is successful
 *
 * @note Call vk_eda_call_thread after the preparation to run the thread and
 *       wait for its completion.
 * @note Must be called in eda task context (e.g. from an eda, pt or task event handler); the caller waits for the called thread to complete.
 * \~chinese
 * @brief 在 eda 调用线程之前准备线程控制块
 * @param[in] thread_cb: 指向线程控制块 @ref vsf_thread_cb_t 的指针
 * @param[in] cfg: 指向准备配置 @ref vsf_thread_prepare_cfg_t 的指针，
 *            包括 entry、stack 和 stack_size
 * @return vsf_err_t: 准备成功返回 VSF_ERR_NONE
 *
 * @note 准备完成后调用 vk_eda_call_thread 运行线程并等待其完成。
 * @note 必须在 eda 任务上下文（如 eda、pt 或 task 的事件处理函数）中调用；调用者会等待被调用的线程执行完成。
 */
extern vsf_err_t vk_eda_call_thread_prepare(    vsf_thread_cb_t *thread_cb,
                                                vsf_thread_prepare_cfg_t *cfg);

VSF_CAL_SECTION(".text.vsf.kernel.vk_eda_call_thread")
/**
 * \~english
 * @brief Call a prepared thread from the current eda and wait for its completion
 * @param[in] thread_cb: a pointer to the thread control block @ref vsf_thread_cb_t,
 *            prepared by vk_eda_call_thread_prepare
 * @return vsf_err_t: VSF_ERR_NONE if the call is started successfully; VSF_ERR_NOT_ENOUGH_RESOURCES if no frame is available
 * @note Must be called in eda task context (e.g. from an eda, pt or task event handler); the caller waits for the called thread to complete.
 * \~chinese
 * @brief 从当前 eda 调用一个已准备的线程并等待其完成
 * @param[in] thread_cb: 指向线程控制块 @ref vsf_thread_cb_t 的指针，
 *            已由 vk_eda_call_thread_prepare 准备
 * @return vsf_err_t: 调用启动成功返回 VSF_ERR_NONE；没有可用的帧返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 * @note 必须在 eda 任务上下文（如 eda、pt 或 task 的事件处理函数）中调用；调用者会等待被调用的线程执行完成。
 */
extern vsf_err_t vk_eda_call_thread(vsf_thread_cb_t *thread_cb);

#if VSF_KERNEL_CFG_EDA_SUPPORT_TASK == ENABLED && VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
VSF_CAL_SECTION(".text.vsf.kernel.vk_thread_call_task")
/**
 * \~english
 * @brief Call a task from the current thread and wait for its completion
 * @param[in] task_handler: entry handler of the target task, of type @ref vsf_task_entry_t
 * @param[in] param: parameter passed to the target task
 * @param[in] local_size: size of the local variables of the target task
 * @return fsm_rt_t: the final state of the called task
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_TASK and
 *       VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE are ENABLED.
 * @note Must be called in thread context; the current thread is blocked until the called task completes.
 * \~chinese
 * @brief 从当前线程调用一个任务并等待其完成
 * @param[in] task_handler: 目标任务的入口函数，类型为 @ref vsf_task_entry_t
 * @param[in] param: 传递给目标任务的参数
 * @param[in] local_size: 目标任务局部变量的大小
 * @return fsm_rt_t: 被调用任务的最终状态
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_TASK 和
 *       VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE 为 ENABLED 时可用。
 * @note 必须在线程上下文中调用；当前线程会被阻塞直到被调用的任务执行完成。
 */
extern
fsm_rt_t vk_thread_call_task(vsf_task_entry_t task_handler, uintptr_t param, size_t local_size);
#endif

VSF_CAL_SECTION(".text.vsf.kernel.vk_thread_call_eda")
/**
 * \~english
 * @brief Call an eda (event-driven task) from the current thread and wait for its completion
 * @param[in] eda_handler: entry handler of the target eda
 * @param[in] param: parameter passed to the target eda
 * @param[in] local_size: size of the local variables of the target eda
 * @param[in] local_buff_size: size of the buffer used to initialize the local variables
 * @param[in] local_buff: buffer used to initialize the local variables, NULL if not needed
 * @return vsf_err_t: always VSF_ERR_NONE (frame exhaustion triggers an assertion in debug builds)
 * @note Must be called in thread context; the current thread is blocked until the called task completes.
 * \~chinese
 * @brief 从当前线程调用一个 eda（事件驱动任务）并等待其完成
 * @param[in] eda_handler: 目标 eda 的入口函数
 * @param[in] param: 传递给目标 eda 的参数
 * @param[in] local_size: 目标 eda 局部变量的大小
 * @param[in] local_buff_size: 用于初始化局部变量的缓冲区大小
 * @param[in] local_buff: 用于初始化局部变量的缓冲区，不需要时为 NULL
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE（帧不足时在调试版本中触发断言）
 * @note 必须在线程上下文中调用；当前线程会被阻塞直到被调用的任务执行完成。
 */
extern
vsf_err_t vk_thread_call_eda(   uintptr_t eda_handler,
                                uintptr_t param,
                                size_t local_size,
                                size_t local_buff_size,
                                uintptr_t local_buff);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_thread_call_thread")
/**
 * \~english
 * @brief Call a thread from the current thread and wait for its completion
 * @param[in] thread_cb: a pointer to the thread control block @ref vsf_thread_cb_t
 *            of the target thread
 * @param[in] cfg: a pointer to the prepare configuration @ref vsf_thread_prepare_cfg_t,
 *            including entry, stack and stack_size
 * @return vsf_err_t: always VSF_ERR_NONE (frame exhaustion triggers an assertion in debug builds)
 * @note Must be called in thread context; the current thread is blocked until the called thread completes.
 * \~chinese
 * @brief 从当前线程调用另一个线程并等待其完成
 * @param[in] thread_cb: 指向目标线程的线程控制块 @ref vsf_thread_cb_t 的指针
 * @param[in] cfg: 指向准备配置 @ref vsf_thread_prepare_cfg_t 的指针，
 *            包括 entry、stack 和 stack_size
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE（帧不足时在调试版本中触发断言）
 * @note 必须在线程上下文中调用；当前线程会被阻塞直到被调用的线程执行完成。
 */
extern vsf_err_t vk_thread_call_thread(     vsf_thread_cb_t *thread_cb,
                                            vsf_thread_prepare_cfg_t *cfg);

#endif


VSF_CAL_SECTION(".text.vsf.kernel.vsf_thread")
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
/**
 * \~english
 * @brief Start a thread
 * @param[in] thread: a pointer to structure @ref vsf_thread_t
 * @param[in] thread_cb: a pointer to the thread control block @ref vsf_thread_cb_t,
 *            whose entry, stack and stack_size members must be initialized
 * @param[in] priority: priority of the thread
 * @return vsf_err_t: VSF_ERR_NONE if the thread is started successfully,
 *         VSF_ERR_PROVIDED_RESOURCE_NOT_SUFFICIENT if the stack is too small,
 *         VSF_ERR_PROVIDED_RESOURCE_NOT_ALIGNED if the stack size is not page-aligned,
 *         VSF_ERR_NOT_ENOUGH_RESOURCES if no frame/event node is available to start the task
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 启动一个线程
 * @param[in] thread: 指向结构体 @ref vsf_thread_t 的指针
 * @param[in] thread_cb: 指向线程控制块 @ref vsf_thread_cb_t 的指针，
 *            其 entry、stack 和 stack_size 成员必须已初始化
 * @param[in] priority: 线程的优先级
 * @return vsf_err_t: 线程启动成功返回 VSF_ERR_NONE，堆栈过小返回
 *         VSF_ERR_PROVIDED_RESOURCE_NOT_SUFFICIENT，堆栈大小未按页对齐返回
 *         VSF_ERR_PROVIDED_RESOURCE_NOT_ALIGNED，没有可用于启动任务的帧/事件节点返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
extern vsf_err_t vsf_thread_start(  vsf_thread_t *thread,
                                    vsf_thread_cb_t *thread_cb,
                                    vsf_prio_t priority);
#else
/**
 * \~english
 * @brief Signature variant when VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is DISABLED; shares the documentation of the first variant above.
 * \~chinese
 * @brief VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL 为 DISABLED 时的签名变体；与上方第一个变体共用文档。
 */
extern vsf_err_t vsf_thread_start(vsf_thread_t *this_ptr, vsf_prio_t priority);
#endif

#if VSF_KERNEL_CFG_THREAD_STACK_CHECK == ENABLED
VSF_CAL_SECTION(".text.vsf.kernel.vsf_thread_stack_check")
/**
 * \~english
 * @brief Check whether the current stack pointer is inside the stack range of the current thread
 * @return none
 *
 * @note Only available when VSF_KERNEL_CFG_THREAD_STACK_CHECK is ENABLED;
 *       assertion fails on stack overflow. It is a no-op in host-thread mode
 *       (VSF_KERNEL_THREAD_USE_HOST).
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 检查当前堆栈指针是否在当前线程的堆栈范围内
 * @return 无
 *
 * @note 仅在 VSF_KERNEL_CFG_THREAD_STACK_CHECK 为 ENABLED 时可用；堆栈溢出时
 *       断言失败。在 host 线程模式（VSF_KERNEL_THREAD_USE_HOST）下为空操作。
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
extern void vsf_thread_stack_check(void);
#endif

VSF_CAL_SECTION(".text.vsf.kernel.vsf_thread_exit")
/**
 * \~english
 * @brief Terminate the current thread
 * @return none
 *
 * @note This function does not return. If the current thread was called as a
 *       sub-call, the caller is resumed.
 * @note Must be called in thread context.
 * \~chinese
 * @brief 终止当前线程
 * @return 无
 *
 * @note 此函数不会返回。如果当前线程是作为子调用被调用的，调用者会被恢复执行。
 * @note 必须在线程上下文中调用。
 */
extern VSF_CAL_NO_RETURN void vsf_thread_exit(void);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_thread_get_cur")
/**
 * \~english
 * @brief Get the currently running thread
 * @return vsf_thread_t *: a pointer to the current thread @ref vsf_thread_t
 *
 * @note Must be called in thread context.
 * \~chinese
 * @brief 获取当前正在运行的线程
 * @return vsf_thread_t *: 指向当前线程 @ref vsf_thread_t 的指针
 *
 * @note 必须在线程上下文中调用。
 */
extern vsf_thread_t *vsf_thread_get_cur(void);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_thread_wait")
/**
 * \~english
 * @brief Wait for any event inside a thread
 * @return vsf_evt_t: the event received
 *
 * @note Must be called in thread context; the current thread is blocked until the operation completes.
 * \~chinese
 * @brief 在线程中等待任意事件
 * @return vsf_evt_t: 收到的事件
 *
 * @note 必须在线程上下文中调用；当前线程会被阻塞直到操作完成。
 */
extern vsf_evt_t vsf_thread_wait(void);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_thread_wait_for_evt")
/**
 * \~english
 * @brief Wait for a specific event inside a thread
 * @param[in] evt: the event to wait for, of type @ref vsf_evt_t
 * @return none
 *
 * @note The current thread is blocked until the next event arrives; in debug
 * builds an assertion fires if that event is not evt.
 * @note Must be called in thread context; the current thread is blocked until the operation completes.
 * \~chinese
 * @brief 在线程中等待指定的事件
 * @param[in] evt: 要等待的事件，类型为 @ref vsf_evt_t
 * @return 无
 *
 * @note 当前线程会被阻塞直到下一个事件到达；在调试版本中，如果该事件不是
 * evt 会触发断言。
 * @note 必须在线程上下文中调用；当前线程会被阻塞直到操作完成。
 */
extern void vsf_thread_wait_for_evt(vsf_evt_t evt);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_thread_sendevt")
/**
 * \~english
 * @brief Post an event to a thread
 * @param[in] thread: a pointer to structure @ref vsf_thread_t
 * @param[in] evt: the event to post, of type @ref vsf_evt_t
 * @return none
 * @note Can be called in task or interrupt context (it is a plain wrapper of vsf_eda_post_evt()).
 * \~chinese
 * @brief 向线程发送一个事件
 * @param[in] thread: 指向结构体 @ref vsf_thread_t 的指针
 * @param[in] evt: 要发送的事件，类型为 @ref vsf_evt_t
 * @return 无
 * @note 可以在任务或中断上下文中调用（它是 vsf_eda_post_evt() 的直接封装）。
 */
extern void vsf_thread_sendevt(vsf_thread_t *thread, vsf_evt_t evt);

#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
VSF_CAL_SECTION(".text.vsf.kernel.vsf_thread_wait_for_evt_msg")
/**
 * \~english
 * @brief Wait for an event carrying a message inside a thread
 * @param[in] evt: the event to wait for, of type @ref vsf_evt_t
 * @return uintptr_t: the message carried by the event
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE is ENABLED.
 * @note Must be called in thread context; the current thread is blocked until the operation completes.
 * \~chinese
 * @brief 在线程中等待一个携带消息的事件
 * @param[in] evt: 要等待的事件，类型为 @ref vsf_evt_t
 * @return uintptr_t: 事件携带的消息
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE 为 ENABLED 时可用。
 * @note 必须在线程上下文中调用；当前线程会被阻塞直到操作完成。
 */
extern uintptr_t vsf_thread_wait_for_evt_msg(vsf_evt_t evt);
#endif

VSF_CAL_SECTION(".text.vsf.kernel.vsf_thread_wait_for_evt_msg")
/**
 * \~english
 * @brief Wait for a message inside a thread
 * @return uintptr_t: the message received
 *
 * @note The current thread is blocked until a message arrives.
 * @note Must be called in thread context; the current thread is blocked until the operation completes.
 * \~chinese
 * @brief 在线程中等待一个消息
 * @return uintptr_t: 收到的消息
 *
 * @note 当前线程会被阻塞，直到消息到达。
 * @note 必须在线程上下文中调用；当前线程会被阻塞直到操作完成。
 */
extern uintptr_t vsf_thread_wait_for_msg(void);

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
VSF_CAL_SECTION(".text.vsf.kernel.vsf_thread_delay")
/**
 * \~english
 * @brief Delay (sleep) the current thread for the given ticks
 * @param[in] tick: delay time in system ticks
 * @return none
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUPPORT_TIMER is ENABLED.
 * @note Must be called in thread context; the current thread is blocked until the operation completes.
 * \~chinese
 * @brief 将当前线程延时（睡眠）指定的 tick 数
 * @param[in] tick: 延时时间（单位：系统 tick）
 * @return 无
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUPPORT_TIMER 为 ENABLED 时可用。
 * @note 必须在线程上下文中调用；当前线程会被阻塞直到操作完成。
 */
extern void vsf_thread_delay(vsf_systimer_tick_t tick);
#endif

VSF_CAL_SECTION(".text.vsf.kernel.vsf_thread_yield")
/**
 * \~english
 * @brief Yield the CPU voluntarily to other tasks of the same priority
 * @return none
 *
 * @note The current thread is blocked until it is scheduled again.
 * @note Must be called in thread context.
 * \~chinese
 * @brief 主动让出 CPU 给同优先级的其他任务
 * @return 无
 *
 * @note 当前线程会被阻塞，直到再次被调度。
 * @note 必须在线程上下文中调用。
 */
void vsf_thread_yield(void);

#if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED
VSF_CAL_SECTION(".text.vsf.kernel.vsf_thread_set_priority")
/**
 * \~english
 * @brief Set the priority of the current thread dynamically
 * @param[in] priority: new priority of the current thread
 * @return vsf_prio_t: the original priority before the change
 *
 * @note Only available when VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY is ENABLED.
 * @note If the new priority differs from the original one, the current thread
 *       yields after the change.
 * @note Must be called in thread context.
 * \~chinese
 * @brief 动态设置当前线程的优先级
 * @param[in] priority: 当前线程的新优先级
 * @return vsf_prio_t: 修改前的原优先级
 *
 * @note 仅在 VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY 为 ENABLED 时可用。
 * @note 如果新优先级与原优先级不同，修改后当前线程会让出 CPU。
 * @note 必须在线程上下文中调用。
 */
extern vsf_prio_t vsf_thread_set_priority(vsf_prio_t priority);
#endif

#if VSF_KERNEL_CFG_THREAD_SIGNAL == ENABLED
VSF_CAL_SECTION(".text.vsf.kernel.vsf_thread_signal")
/**
 * \~english
 * @brief Send a POSIX-like signal to a thread
 * @param[in] thread: a pointer to structure @ref vsf_thread_t
 * @param[in] sig: signal number
 * @return none
 *
 * @note Only available when VSF_KERNEL_CFG_THREAD_SIGNAL is ENABLED.
 * @note The sighandler member of the target thread is called when the thread
 *       processes the signal; if the thread is currently running, has no
 *       pending events (blocked in a wait), or is in a sub-call, the signal
 *       is recorded and handled later.
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 向线程发送一个类 POSIX 的信号
 * @param[in] thread: 指向结构体 @ref vsf_thread_t 的指针
 * @param[in] sig: 信号编号
 * @return 无
 *
 * @note 仅在 VSF_KERNEL_CFG_THREAD_SIGNAL 为 ENABLED 时可用。
 * @note 目标线程处理信号时会调用其 sighandler 成员；如果线程正在运行、
 *       没有待处理事件（阻塞在等待中）或处于子调用中，信号会被记录并
 *       延后处理。
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
extern void vsf_thread_signal(vsf_thread_t *thread, int sig);
#endif

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
VSF_CAL_SECTION(".text.vsf.kernel.__vsf_thread_wait_for_sync")
/**
 * \~english
 * @brief Wait for a sync object (semaphore, trigger or mutex) inside a thread
 * @param[in] sync: a pointer to structure @ref vsf_sync_t
 * @param[in] time_out: timeout in ticks, negative to wait forever, 0 to try once
 * @return vsf_sync_reason_t: VSF_SYNC_GET if the sync object is obtained,
 *         VSF_SYNC_TIMEOUT if timed out, VSF_SYNC_CANCEL if cancelled,
 *         VSF_SYNC_FAIL on error
 *
 * @note Internal use only. Use vsf_thread_sem_pend, vsf_thread_trig_pend or
 *       vsf_thread_mutex_enter instead.
 * @note Must be called in thread context; the current thread is blocked until the operation completes.
 * \~chinese
 * @brief 在线程中等待一个同步对象（信号量、触发器或互斥量）
 * @param[in] sync: 指向结构体 @ref vsf_sync_t 的指针
 * @param[in] time_out: 超时时间（单位：tick），负数表示永久等待，0 表示只尝试一次
 * @return vsf_sync_reason_t: 获取到同步对象返回 VSF_SYNC_GET，超时返回
 *         VSF_SYNC_TIMEOUT，被取消返回 VSF_SYNC_CANCEL，出错返回 VSF_SYNC_FAIL
 *
 * @note 仅供内部使用。请使用 vsf_thread_sem_pend、vsf_thread_trig_pend 或
 *       vsf_thread_mutex_enter。
 * @note 必须在线程上下文中调用；当前线程会被阻塞直到操作完成。
 */
vsf_sync_reason_t __vsf_thread_wait_for_sync(vsf_sync_t *sync, vsf_timeout_tick_t time_out);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_thread_mutex")
/**
 * \~english
 * @brief Leave (release) a mutex inside a thread
 * @param[in] mtx: a pointer to structure @ref vsf_mutex_t
 * @return vsf_err_t: VSF_ERR_NONE if successful; VSF_ERR_OVERRUN if the mutex is not owned
 * @note Must be called in task context, NOT in interrupt context (in interrupt context use the corresponding _isr API if available).
 * \~chinese
 * @brief 在线程中离开（释放）一个互斥量
 * @param[in] mtx: 指向结构体 @ref vsf_mutex_t 的指针
 * @return vsf_err_t: 成功返回 VSF_ERR_NONE；互斥量未被持有返回 VSF_ERR_OVERRUN
 * @note 必须在任务上下文中调用，禁止在中断上下文中调用（如有对应的 _isr API，中断里请使用它）。
 */
extern vsf_err_t vsf_thread_mutex_leave(vsf_mutex_t *mtx);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_thread_queue")
/**
 * \~english
 * @brief Send a node to a queue inside a thread
 * @param[in] queue: a pointer to structure @ref vsf_eda_queue_t
 * @param[in] node: a pointer to the node to send
 * @param[in] timeout: timeout in ticks; negative to wait forever, 0 to try once, a positive value to wait up to the given ticks
 * @return vsf_sync_reason_t: VSF_SYNC_GET if the node is sent,
 *         VSF_SYNC_TIMEOUT if timed out, VSF_SYNC_CANCEL if cancelled
 *
 * @note The current thread is blocked while waiting.
 * @note Must be called in thread context; the current thread is blocked until the operation completes.
 * \~chinese
 * @brief 在线程中向队列发送一个节点
 * @param[in] queue: 指向结构体 @ref vsf_eda_queue_t 的指针
 * @param[in] node: 指向要发送节点的指针
 * @param[in] timeout: 超时时间（单位：tick），负数表示永久等待，0 表示只尝试一次，正数表示最多等待对应的 tick 数
 * @return vsf_sync_reason_t: 节点发送成功返回 VSF_SYNC_GET，超时返回
 *         VSF_SYNC_TIMEOUT，被取消返回 VSF_SYNC_CANCEL
 *
 * @note 等待时当前线程会被阻塞。
 * @note 必须在线程上下文中调用；当前线程会被阻塞直到操作完成。
 */
vsf_sync_reason_t vsf_thread_queue_send(vsf_eda_queue_t *queue, void *node, vsf_timeout_tick_t timeout);

VSF_CAL_SECTION(".text.vsf.kernel.vsf_thread_queue")
/**
 * \~english
 * @brief Receive a node from a queue inside a thread
 * @param[in] queue: a pointer to structure @ref vsf_eda_queue_t
 * @param[out] node: a pointer to receive the dequeued node
 * @param[in] timeout: timeout in ticks; negative to wait forever, 0 to try once, a positive value to wait up to the given ticks
 * @return vsf_sync_reason_t: VSF_SYNC_GET if a node is received,
 *         VSF_SYNC_TIMEOUT if timed out, VSF_SYNC_CANCEL if cancelled
 *
 * @note The current thread is blocked while waiting.
 * @note Must be called in thread context; the current thread is blocked until the operation completes.
 * \~chinese
 * @brief 在线程中从队列接收一个节点
 * @param[in] queue: 指向结构体 @ref vsf_eda_queue_t 的指针
 * @param[out] node: 用于接收出队节点的指针
 * @param[in] timeout: 超时时间（单位：tick），负数表示永久等待，0 表示只尝试一次，正数表示最多等待对应的 tick 数
 * @return vsf_sync_reason_t: 接收到节点返回 VSF_SYNC_GET，超时返回
 *         VSF_SYNC_TIMEOUT，被取消返回 VSF_SYNC_CANCEL
 *
 * @note 等待时当前线程会被阻塞。
 * @note 必须在线程上下文中调用；当前线程会被阻塞直到操作完成。
 */
vsf_sync_reason_t vsf_thread_queue_recv(vsf_eda_queue_t *queue, void **node, vsf_timeout_tick_t timeout);

#   if VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT == ENABLED
VSF_CAL_SECTION(".text.vsf.kernel.vsf_thread_bmpevt_pend")
/**
 * \~english
 * @brief Pend on a bitmap event inside a thread
 * @param[in] bmpevt: a pointer to structure @ref vsf_bmpevt_t
 * @param[in] pender: a pointer to structure @ref vsf_bmpevt_pender_t describing the bits to wait for
 * @param[in] timeout: timeout in ticks; negative to wait forever, 0 to try once, a positive value to wait up to the given ticks
 * @return vsf_sync_reason_t: VSF_SYNC_GET if the expected bits are set,
 *         VSF_SYNC_TIMEOUT if timed out, VSF_SYNC_CANCEL if cancelled,
 *         VSF_SYNC_FAIL on error
 *
 * @note The current thread is blocked while waiting. Only available when
 *       VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT is ENABLED.
 * @note Must be called in thread context; the current thread is blocked until the operation completes.
 * \~chinese
 * @brief 在线程中等待一个位图事件
 * @param[in] bmpevt: 指向结构体 @ref vsf_bmpevt_t 的指针
 * @param[in] pender: 指向结构体 @ref vsf_bmpevt_pender_t 的指针，描述要等待的位
 * @param[in] timeout: 超时时间（单位：tick），负数表示永久等待，0 表示只尝试一次，正数表示最多等待对应的 tick 数
 * @return vsf_sync_reason_t: 期望的位被置位返回 VSF_SYNC_GET，超时返回
 *         VSF_SYNC_TIMEOUT，被取消返回 VSF_SYNC_CANCEL，出错返回 VSF_SYNC_FAIL
 *
 * @note 等待时当前线程会被阻塞。仅在 VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT
 *       为 ENABLED 时可用。
 * @note 必须在线程上下文中调用；当前线程会被阻塞直到操作完成。
 */
extern vsf_sync_reason_t vsf_thread_bmpevt_pend(
                    vsf_bmpevt_t *bmpevt,
                    vsf_bmpevt_pender_t *pender,
                    vsf_timeout_tick_t timeout);
#   endif
#endif

#ifdef __cplusplus
}
#endif

#endif
#endif
