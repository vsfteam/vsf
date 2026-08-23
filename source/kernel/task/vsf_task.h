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

#ifndef __VSF_KERNEL_TASK_H__
#define __VSF_KERNEL_TASK_H__

/*============================ INCLUDES ======================================*/

#include "./kernel/vsf_kernel_cfg.h"

#if VSF_USE_KERNEL == ENABLED && VSF_KERNEL_CFG_EDA_SUPPORT_TASK == ENABLED

#include "service/vsf_service.h"
#include "../vsf_eda.h"
#include "./vsf_pt.h"
#include "./vsf_fsm.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define __vsf_task_func(__name)     vsf_task_func_##__name
/**
 * \~english
 * @brief Expand to the name of the task entry function.
 * @param[in] __name: name of the task
 * \~chinese
 * @brief 展开为任务入口函数的函数名。
 * @param[in] __name: 任务名
 */
#define vsf_task_func(__name)       __vsf_task_func(__name)

#define __vsf_task(__name)          task_cb_##__name
/**
 * \~english
 * @brief Expand to the type name of the task control block.
 * @param[in] __name: name of the task
 * \~chinese
 * @brief 展开为任务控制块的类型名。
 * @param[in] __name: 任务名
 */
#define vsf_task(__name)            __vsf_task(__name)

#if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
#   define __implement_vsf_task(__name)                                         \
        fsm_rt_t vsf_task_func(__name)( uintptr_t local,                        \
                                        vsf_evt_t evt)                          \
        {                                                                       \
            vsf_task(__name) *vsf_pthis =                                       \
                *(vsf_task(__name) **)                                          \
                    ((uintptr_t)local - sizeof(uintptr_t));

/**
 * \~english
 * @brief Begin the task entry function body.
 *
 * @note Used together with implement_vsf_task() and vsf_task_end().
 * \~chinese
 * @brief 开始任务入口函数体。
 *
 * @note 与 implement_vsf_task() 和 vsf_task_end() 配合使用。
 */
#   define vsf_task_begin()

/**
 * \~english
 * @brief End the task entry function body started by implement_vsf_task().
 *
 * @note In this variant (VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE is
 *       ENABLED), it returns @ref fsm_rt_on_going to keep the task alive.
 * \~chinese
 * @brief 结束由 implement_vsf_task() 开始的任务入口函数体。
 *
 * @note 在此变体中（VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE 为 ENABLED），
 *       返回 @ref fsm_rt_on_going 以保持任务存活。
 */
#   define vsf_task_end()                                                       \
            } return fsm_rt_on_going;

/**
 * \~english
 * @brief Expand to the current state of the task control block.
 * \~chinese
 * @brief 展开为任务控制块的当前状态。
 */
#   define vsf_task_state  (vsf_this.fsm_state)

#else
#   define __implement_vsf_task(__name)                                         \
        void vsf_task_func(__name)(__name *obj_ptr, vsf_evt_t evt)              \
        {                                                                       \
            vsf_task(__name) *vsf_pthis = &(obj_ptr->param);

/**
 * \~english
 * @brief Variant when VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE is DISABLED; shares the documentation of the first variant above.
 * \~chinese
 * @brief VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE 为 DISABLED 时的变体；与上方第一个变体共用文档。
 */
#   define vsf_task_begin()

/**
 * \~english
 * @brief Variant when VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE is DISABLED; shares the documentation of the first variant above.
 * \~chinese
 * @brief VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE 为 DISABLED 时的变体；与上方第一个变体共用文档。
 */
#   define vsf_task_end()           } __vsf_eda_yield();

/**
 * \~english
 * @brief Variant when VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE is DISABLED; shares the documentation of the first variant above.
 * \~chinese
 * @brief VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE 为 DISABLED 时的变体；与上方第一个变体共用文档。
 */
#   define vsf_task_state  (vsf_this.fsm_state)
#endif

/**
 * \~english
 * @brief Start the implementation of the task entry function.
 * @param[in] __name: name of the task
 *
 * @note Inside the body, vsf_pthis points to the task control block, and
 *       vsf_this gives direct access to its members. When
 *       VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE is ENABLED, the entry
 *       function returns @ref fsm_rt_t, otherwise it returns void.
 * @note Used together with def_vsf_task() and init_vsf_task().
 * \~chinese
 * @brief 开始任务入口函数的实现。
 * @param[in] __name: 任务名
 *
 * @note 在函数体内，vsf_pthis 指向任务控制块，vsf_this 可直接访问其成员。
 *       当 VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE 为 ENABLED 时，入口
 *       函数返回 @ref fsm_rt_t，否则返回 void。
 * @note 与 def_vsf_task() 和 init_vsf_task() 配合使用。
 */
#define implement_vsf_task(__name)  __implement_vsf_task(__name)

/**
 * \~english
 * @brief Alias of implement_vsf_task().
 * @param[in] __name: name of the task
 * \~chinese
 * @brief implement_vsf_task() 的别名。
 * @param[in] __name: 任务名
 */
#define imp_vsf_task(__name)        implement_vsf_task(__name)

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
/**
 * \~english
 * @brief Start a task with the given configuration. In this variant
 *        (VSF_KERNEL_CFG_EDA_SUPPORT_TIMER is ENABLED) it is mapped to
 *        vsf_teda_start(), so timer services are available to the task.
 * @param[in] __task: pointer to the task, of type @ref vsf_task_t
 * @param[in] __cfg: pointer to the task configuration, of type @ref vsf_eda_cfg_t
 * @return vsf_err_t: VSF_ERR_NONE if the task is started successfully;
 *         VSF_ERR_NOT_ENOUGH_RESOURCES if no frame is available
 *
 * @note A VSF_EVT_INIT event is sent to the task once it is started.
 * \~chinese
 * @brief 以给定配置启动任务。在此变体中（VSF_KERNEL_CFG_EDA_SUPPORT_TIMER 为
 *        ENABLED）映射为 vsf_teda_start()，任务可使用定时器服务。
 * @param[in] __task: 指向任务的指针，类型为 @ref vsf_task_t
 * @param[in] __cfg: 指向任务配置的指针，类型为 @ref vsf_eda_cfg_t
 * @return vsf_err_t: 任务启动成功返回 VSF_ERR_NONE；没有可用的帧返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 任务启动后会收到一个 VSF_EVT_INIT 事件。
 */
#   define vsf_task_start           vsf_teda_start
#else
/**
 * \~english
 * @brief Variant when VSF_KERNEL_CFG_EDA_SUPPORT_TIMER is DISABLED; shares the documentation of the first variant above.
 * \~chinese
 * @brief VSF_KERNEL_CFG_EDA_SUPPORT_TIMER 为 DISABLED 时的变体；与上方第一个变体共用文档。
 */
#   define vsf_task_start           vsf_eda_start
#endif

#if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
#   define __def_vsf_task(__name,...)                                           \
        struct task_cb_##__name {                                               \
            uint8_t fsm_state;                                                  \
            __VA_ARGS__                                                         \
        };                                                                      \
        struct __name {                                                         \
            implement(vsf_task_t);                                              \
            implement_ex(task_cb_##__name, param);                              \
        };                                                                      \
        extern fsm_rt_t vsf_task_func_##__name( uintptr_t local,                \
                                                vsf_evt_t evt);

#else
#   define __def_vsf_task(__name,...)                                           \
        typedef struct task_cb_##__name {                                       \
            uint8_t fsm_state;                                                  \
            __VA_ARGS__                                                         \
        } task_cb_##__name;                                                     \
        typedef struct __name {                                                 \
            implement(vsf_task_t);                                              \
            implement_ex(task_cb_##__name, param);                              \
        } __name;                                                               \
        extern void vsf_task_func_##__name(struct __name *vsf_pthis, vsf_evt_t evt);
#endif

/**
 * \~english
 * @brief Define a task control block and declare its entry function.
 * @param[in] __name: name of the task
 * @param[in] ...: member variables of the task control block
 *
 * @note The first member of the task control block is fsm_state, which is
 *       used internally to hold the task state.
 * @note Used together with implement_vsf_task() and init_vsf_task().
 * \~chinese
 * @brief 定义任务控制块，并声明其入口函数。
 * @param[in] __name: 任务名
 * @param[in] ...: 任务控制块的成员变量
 *
 * @note 任务控制块的第一个成员是 fsm_state，供内部保存任务状态使用。
 * @note 与 implement_vsf_task() 和 init_vsf_task() 配合使用。
 */
#define def_vsf_task(__name,...)            __def_vsf_task(__name,__VA_ARGS__)

/**
 * \~english
 * @brief Alias of def_vsf_task().
 * @param[in] __name: name of the task
 * @param[in] ...: member variables of the task control block
 * \~chinese
 * @brief def_vsf_task() 的别名。
 * @param[in] __name: 任务名
 * @param[in] ...: 任务控制块的成员变量
 */
#define define_vsf_task(__name,...)         def_vsf_task(__name,__VA_ARGS__)

/**
 * \~english
 * @brief End a task definition started by def_vsf_task(). It is empty and
 *        only used for coding style symmetry.
 * \~chinese
 * @brief 结束由 def_vsf_task() 开始的任务定义。该宏为空，仅用于编码风格
 *        上的对称。
 */
#define end_def_vsf_task(...)

/**
 * \~english
 * @brief Alias of end_def_vsf_task().
 * \~chinese
 * @brief end_def_vsf_task() 的别名。
 */
#define end_define_vsf_task(...)

#define __declare_vsf_task(__name)                                              \
            typedef struct __name __name;                                       \
            typedef struct task_cb_##__name  task_cb_##__name;

/**
 * \~english
 * @brief Declare a task (the task class and its control block type) before
 *        it is defined by def_vsf_task().
 * @param[in] __name: name of the task
 * \~chinese
 * @brief 在使用 def_vsf_task() 定义之前，先声明任务（任务类及其控制块
 *        类型）。
 * @param[in] __name: 任务名
 */
#define declare_vsf_task(__name)        __declare_vsf_task(__name)

/**
 * \~english
 * @brief Alias of declare_vsf_task().
 * @param[in] __name: name of the task
 * \~chinese
 * @brief declare_vsf_task() 的别名。
 * @param[in] __name: 任务名
 */
#define dcl_vsf_task(__name)            declare_vsf_task(__name)

/**
 * \~english
 * @brief Prepare a task control block before the task is started, i.e.
 *        reset the task state.
 * @param[in] __name: name of the task
 * @param[in] __task: pointer to the task control block
 * \~chinese
 * @brief 在任务启动前准备任务控制块，即复位任务状态。
 * @param[in] __name: 任务名
 * @param[in] __task: 指向任务控制块的指针
 */
#define prepare_vsf_task(__name, __task)                                        \
            do {(__task)->fsm_state = 0; } while(0)

/**
 * \~english
 * @brief Alias of prepare_vsf_task().
 * @param[in] __name: name of the task
 * @param[in] __task: pointer to the task control block
 * \~chinese
 * @brief prepare_vsf_task() 的别名。
 * @param[in] __name: 任务名
 * @param[in] __task: 指向任务控制块的指针
 */
#define prp_vsf_task(__name, __task)        prepare_vsf_task(__name, __task)

#if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
#   define __init_vsf_task(__name, __task, __pri, ...)                          \
        do {                                                                    \
            vsf_eda_cfg_t VSF_MACRO_SAFE_NAME(cfg) = {                          \
                .fn.func = (uintptr_t)vsf_task_func(__name),                    \
                .priority = (__pri),                                            \
                .target = (uintptr_t)&((__task)->param),                        \
                .feature.is_subcall_has_return_value = true,                    \
                __VA_ARGS__                                                     \
            };                                                                  \
            prepare_vsf_task(__name, &((__task)->param));                       \
            vsf_task_start(&((__task)->use_as__vsf_task_t),                     \
                &VSF_MACRO_SAFE_NAME(cfg));                                     \
        } while(0)
#else
#   define __init_vsf_task(__name, __task, __pri, ...)                          \
        do {                                                                    \
            vsf_eda_cfg_t VSF_MACRO_SAFE_NAME(cfg) = {                          \
                .fn.evthandler = (vsf_task_entry_t)vsf_task_func(__name),       \
                .priority = (__pri),                                            \
                .target = NULL,                                                 \
                __VA_ARGS__                                                     \
            };                                                                  \
            prepare_vsf_task(__name, &((__task)->param));                       \
            vsf_task_start(&((__task)->use_as__vsf_task_t),                     \
                &VSF_MACRO_SAFE_NAME(cfg));                                     \
        } while(0)
#endif

/**
 * \~english
 * @brief Prepare and start a task defined by def_vsf_task().
 * @param[in] __name: name of the task
 * @param[in] __task: pointer to the task instance
 * @param[in] __pri: priority of the task, of type @ref vsf_prio_t
 * @param[in] ...: optional extra initializers for @ref vsf_eda_cfg_t
 *
 * @note The task control block is reset first, then the task is started via
 *       vsf_task_start(); the task entry function will receive a
 *       VSF_EVT_INIT event.
 * \~chinese
 * @brief 准备并启动由 def_vsf_task() 定义的任务。
 * @param[in] __name: 任务名
 * @param[in] __task: 指向任务实例的指针
 * @param[in] __pri: 任务优先级，类型为 @ref vsf_prio_t
 * @param[in] ...: @ref vsf_eda_cfg_t 的可选额外初始化项
 *
 * @note 先复位任务控制块，再通过 vsf_task_start() 启动任务；任务入口函数
 *       将收到 VSF_EVT_INIT 事件。
 */
#define init_vsf_task(__name, __task, __pri, ...)                               \
            __init_vsf_task(__name, (__task), (__pri), __VA_ARGS__)


#if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
/**
 * \~english
 * @brief Call another task as a sub-task from the current task, and wait
 *        until it finishes.
 * @param[in] __name: name of the sub-task
 * @param[in] __target: pointer to the control block of the sub-task
 * @param[in] ...: optional size of the local variables of the sub-task frame
 * @return fsm_rt_t: the return value of the sub-task when it finishes,
 *         or @ref fsm_rt_yield while the sub-task is scheduled to run
 *
 * @note Only available when VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE is
 *       ENABLED.
 * \~chinese
 * @brief 从当前任务调用另一个任务作为子任务，并等待其完成。
 * @param[in] __name: 子任务名
 * @param[in] __target: 指向子任务控制块的指针
 * @param[in] ...: 子任务栈帧局部变量的可选大小
 * @return fsm_rt_t: 子任务完成时的返回值；子任务被调度运行期间返回
 *         @ref fsm_rt_yield
 *
 * @note 仅在 VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE 为 ENABLED 时可用。
 */
#define vsf_task_call_task(__name, __target, ...)                               \
            __vsf_eda_call_task((vsf_task_entry_t)vsf_task_func(__name),        \
                                (uintptr_t)(__target), (0, ##__VA_ARGS__))
#endif

#if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
/**
 * \~english
 * @brief Call an event-driven subroutine (sub-eda) from the current task.
 * @param[in] __name: entry function of the subroutine
 * @param[in] __target: pointer to the target (parameter) of the subroutine
 * @param[in] ...: optional size of the local variables of the sub-frame
 *
 * @note If the subroutine cannot be started, the current task returns
 *       @ref fsm_rt_on_going and retries later.
 * \~chinese
 * @brief 从当前任务调用一个事件驱动子程序（子 eda）。
 * @param[in] __name: 子程序的入口函数
 * @param[in] __target: 指向子程序目标（参数）的指针
 * @param[in] ...: 子栈帧局部变量的可选大小
 *
 * @note 如果子程序无法启动，当前任务返回 @ref fsm_rt_on_going 并稍后重试。
 */
#   define vsf_task_call_sub(__name, __target, ...)                             \
            if (VSF_ERR_NONE != __vsf_eda_call_eda(                             \
                    (vsf_task_entry_t)(__name),                                 \
                    (__target),                                                 \
                    (0, ##__VA_ARGS))) {                                        \
                return fsm_rt_on_going;                                         \
            }

/**
 * \~english
 * @brief Alias of vsf_task_call_task().
 * @param[in] __name: name of the sub-task
 * @param[in] __target: pointer to the control block of the sub-task
 * @param[in] ...: optional size of the local variables of the sub-task frame
 * \~chinese
 * @brief vsf_task_call_task() 的别名。
 * @param[in] __name: 子任务名
 * @param[in] __target: 指向子任务控制块的指针
 * @param[in] ...: 子任务栈帧局部变量的可选大小
 */
#   define vsf_eda_call_task    vsf_task_call_task

#elif VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
/**
 * \~english
 * @brief Variant when VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE is DISABLED; shares the documentation of the first variant above.
 * \~chinese
 * @brief VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE 为 DISABLED 时的变体；与上方第一个变体共用文档。
 */
#   define vsf_task_call_sub(__name, __target, ...)                             \
            if (VSF_ERR_NONE != __vsf_eda_call_eda(                             \
                    (vsf_task_entry_t)(__name),                                 \
                    (__target),                                                 \
                    (0, ##__VA_ARGS))) {                                        \
                return ;                                                        \
            }

/**
 * \~english
 * @brief Variant when VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE is DISABLED; shares the documentation of the first variant above.
 * \~chinese
 * @brief VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE 为 DISABLED 时的变体；与上方第一个变体共用文档。
 */
#   define vsf_eda_call_task(__name, __target)                                  \
        vsf_eda_call_sub((vsf_task_entry_t)vsf_task_func(__name), (__target))
#endif


#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
/**
 * \~english
 * @brief Call a protothread (pt) as a subroutine from the current task.
 * @param[in] __name: name of the protothread
 * @param[in] __target: pointer to the control block of the protothread
 * \~chinese
 * @brief 从当前任务调用一个 protothread（pt）作为子程序。
 * @param[in] __name: protothread 名
 * @param[in] __target: 指向 protothread 控制块的指针
 */
#define vsf_task_call_pt(__name, __target)                                      \
            vsf_task_call_sub(vsf_pt_func(__name), __target)
#endif

/**
 * \~english
 * @brief Condition helper checking whether the current event is
 *        VSF_EVT_INIT, i.e. the task is started.
 *
 * @note Used inside the task entry function, usually together with
 *       vsf_task_wait_until().
 * \~chinese
 * @brief 条件辅助宏，检查当前事件是否为 VSF_EVT_INIT，即任务已启动。
 *
 * @note 在任务入口函数内使用，通常与 vsf_task_wait_until() 配合。
 */
#define on_vsf_task_init()                                                      \
        if (VSF_EVT_INIT == evt)

/**
 * \~english
 * @brief Condition helper checking whether the current event is
 *        VSF_EVT_FINI, i.e. the task is about to terminate.
 *
 * @note Used inside the task entry function.
 * \~chinese
 * @brief 条件辅助宏，检查当前事件是否为 VSF_EVT_FINI，即任务即将终止。
 *
 * @note 在任务入口函数内使用。
 */
#define on_vsf_task_fini()                                                      \
        if (VSF_EVT_FINI == evt)

/**
 * \~english
 * @brief Condition helper checking whether the current event is the given
 *        event.
 * @param[in] __evt: the event to check, of type @ref vsf_evt_t
 *
 * @note Used inside the task entry function, usually together with
 *       vsf_task_wait_until().
 * \~chinese
 * @brief 条件辅助宏，检查当前事件是否为给定事件。
 * @param[in] __evt: 要检查的事件，类型为 @ref vsf_evt_t
 *
 * @note 在任务入口函数内使用，通常与 vsf_task_wait_until() 配合。
 */
#define on_vsf_task_evt(__evt)                                                  \
        if ((__evt) == evt)


/*! \note IMPORTANT
 *        For anything you want to wait, which is coming from vsf
 *        system, you can use vsf_task_wait_until() optionally.
 *        Those includues: VSF_EVT_XXXXX, semaphore, mutex and etc.
 *
 *        For VSF_EVT_XXXX, please use:
 *        vsf_task_wait_until(  on_vsf_task_evt(VSF_EVT_XXXXX) ) ;
 *
 *
 *        For semaphore, please use:
 *        vsf_task_wait_until( vsf_sem_pend(...) ) ;
 *
 *
 *        For time, please use:
 *        vsf_task_wait_until( vsf_delay(...) ) ;
 *
 */
#if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
/**
 * \~english
 * @brief Wait cooperatively until the given condition is satisfied.
 * @param[in] ...: the condition to wait for, e.g. on_vsf_task_evt(XXX),
 *            vsf_sem_pend(...) or vsf_delay(...)
 *
 * @note If the condition is not satisfied, the task entry function returns
 *       @ref fsm_rt_wait_for_evt, so the task sleeps until a new event
 *       arrives; the condition is checked again on the next run.
 * @note Used inside the task entry function.
 * \~chinese
 * @brief 协作式等待，直到给定条件满足。
 * @param[in] ...: 要等待的条件，例如 on_vsf_task_evt(XXX)、
 *            vsf_sem_pend(...) 或 vsf_delay(...)
 *
 * @note 如果条件不满足，任务入口函数返回 @ref fsm_rt_wait_for_evt，任务
 *       休眠直至新事件到达；下次运行时再次检查条件。
 * @note 在任务入口函数内使用。
 */
#   define vsf_task_wait_until(...)                                             \
            __VA_ARGS__ {} else {                                               \
                return fsm_rt_wait_for_evt;                                     \
            }

#else
/**
 * \~english
 * @brief Variant when VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE is DISABLED; shares the documentation of the first variant above.
 * \~chinese
 * @brief VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE 为 DISABLED 时的变体；与上方第一个变体共用文档。
 */
#   define vsf_task_wait_until(...)                                             \
            __VA_ARGS__ {} else {                                               \
                return ;                                                        \
            }
#endif

/*============================ TYPES =========================================*/

#   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
/**
 * \~english
 * @brief Cooperative task type, alias of @ref vsf_teda_t when
 *        VSF_KERNEL_CFG_EDA_SUPPORT_TIMER is ENABLED; the base type produced
 *        by def_vsf_task()
 * \~chinese
 * @brief 协作式任务类型，启用 VSF_KERNEL_CFG_EDA_SUPPORT_TIMER 时是
 *        @ref vsf_teda_t 的别名；是 def_vsf_task() 生成的基类型
 */
typedef vsf_teda_t              vsf_task_t;
#   else
/**
 * \~english
 * @brief Cooperative task type, alias of @ref vsf_eda_t when
 *        VSF_KERNEL_CFG_EDA_SUPPORT_TIMER is DISABLED; the base type produced
 *        by def_vsf_task()
 * \~chinese
 * @brief 协作式任务类型，禁用 VSF_KERNEL_CFG_EDA_SUPPORT_TIMER 时是
 *        @ref vsf_eda_t 的别名；是 def_vsf_task() 生成的基类型
 */
typedef vsf_eda_t               vsf_task_t;
#   endif

#if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
/**
 * \~english
 * @brief Task entry function (the event handler of the task), returning
 *        fsm_rt_t when VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE is ENABLED
 * @param[in] target: target (parameter) passed to the task
 * @param[in] evt: the event to be handled
 * @return fsm_rt_t: the return value of the task
 * \~chinese
 * @brief 任务入口函数（任务的事件处理函数），启用
 *        VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE 时返回 fsm_rt_t
 * @param[in] target: 传给任务的目标（参数）
 * @param[in] evt: 要处理的事件
 * @return fsm_rt_t: 任务的返回值
 */
typedef fsm_rt_t (*vsf_task_entry_t)(uintptr_t target, vsf_evt_t evt);
#else
/**
 * \~english
 * @brief Task entry function (the event handler of the task), alias of
 *        @ref vsf_eda_evthandler_t when
 *        VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE is DISABLED
 * \~chinese
 * @brief 任务入口函数（任务的事件处理函数），禁用
 *        VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE 时是
 *        @ref vsf_eda_evthandler_t 的别名
 */
typedef vsf_eda_evthandler_t    vsf_task_entry_t;
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
/**
 * \~english
 * @brief Call a task as a sub-task and wait for its return value.
 * @param[in] entry: entry function of the sub-task, of type @ref vsf_task_entry_t
 * @param[in] param: pointer to the target (parameter) of the sub-task
 * @param[in] local_size: size of the local variables of the sub-task frame
 * @return fsm_rt_t: the return value of the sub-task when it finishes,
 *         or @ref fsm_rt_yield while the sub-task is scheduled to run
 *
 * @note This is an internal function, please use vsf_task_call_task()
 *       instead.
 * \~chinese
 * @brief 调用一个任务作为子任务，并等待其返回值。
 * @param[in] entry: 子任务的入口函数，类型为 @ref vsf_task_entry_t
 * @param[in] param: 指向子任务目标（参数）的指针
 * @param[in] local_size: 子任务栈帧局部变量的大小
 * @return fsm_rt_t: 子任务完成时的返回值；子任务被调度运行期间返回
 *         @ref fsm_rt_yield
 *
 * @note 这是内部函数，请使用 vsf_task_call_task()。
 */
VSF_CAL_SECTION(".text.vsf.kernel.eda_task")
extern fsm_rt_t __vsf_eda_call_task(vsf_task_entry_t entry,
                                    uintptr_t param,
                                    size_t local_size);
#   endif      // VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE
#endif      // VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL

#ifdef __cplusplus
}
#endif

#endif
#endif
/* EOF */
