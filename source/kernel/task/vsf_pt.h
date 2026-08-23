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

#ifndef __VSF_PT_H__
#define __VSF_PT_H__

/*============================ INCLUDES ======================================*/
#include "./kernel/vsf_kernel_cfg.h"

#if VSF_KERNEL_CFG_EDA_SUPPORT_PT == ENABLED && VSF_USE_KERNEL == ENABLED
#include "../vsf_eda.h"
#include "./__vsf_task_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/


#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   define __implement_vsf_pt(__name)                                           \
            __implement_vsf_pt_common(__name, uintptr_t local)                  \
            {                                                                   \
                __vsf_pt_common(__name) *vsf_pthis =                            \
                    *(__vsf_pt_common(__name) **)                               \
                        ((uintptr_t)local - sizeof(uintptr_t));

#   define __implement_vsf_pt_ex(__name, __func_name)                           \
            __implement_vsf_pt_common(  __func_name,  uintptr_t local)          \
            {                                                                   \
                __vsf_pt_common(__name) *vsf_pthis =                            \
                    *(__vsf_pt_common(__name) **)                               \
                        ((uintptr_t)local - sizeof(uintptr_t));

#else

#   define __implement_vsf_pt(__name)                                           \
            __implement_vsf_pt_common(__name, __name *obj_ptr) {                \
                __vsf_pt_common(__name) *vsf_pthis = &(obj_ptr->param);

#   define __implement_vsf_pt_ex(__name, __func_name)                           \
            __implement_vsf_pt_common(__func_name, __name *obj_ptr) {           \
                __vsf_pt_common(__name) *vsf_pthis = &(obj_ptr->param);
#endif

#define __vsf_pt_state()         (vsf_this.fsm_state)
#define __vsf_pt_end()           __vsf_pt_end_common()


#define __vsf_pt_entry(__state, ...)                                            \
            __vsf_pt_entry_common(__state, __VA_ARGS__)
/**
 * \~english
 * @brief Mark a re-entry point inside the protothread body.
 * @param[in] ...: optional code executed right after the re-entry point is
 *            set
 *
 * @note When the protothread is entered again later, execution resumes at
 *       the re-entry point. It does not return from the entry function by
 *       itself; it is usually used by other blocking macros such as
 *       vsf_pt_wait_until().
 * \~chinese
 * @brief 在 protothread 体内标记一个重入点。
 * @param[in] ...: 设置重入点后立即执行的可选代码
 *
 * @note 之后再次进入该 protothread 时，从该重入点恢复执行。它本身不会从
 *       入口函数返回，通常由 vsf_pt_wait_until() 等阻塞宏使用。
 */
#define vsf_pt_entry(...)                                                       \
            __vsf_pt_entry(__vsf_pt_state(), __VA_ARGS__)

#define __vsf_pt_begin(__state)     __vsf_pt_begin_common(__state)


/**
 * \~english
 * @brief Begin the protothread body, opened right after
 *        implement_vsf_pt().
 *
 * @note Used together with vsf_pt_end(); the blocking macros can only be
 *       used between them.
 * \~chinese
 * @brief 开始 protothread 体，紧跟在 implement_vsf_pt() 之后。
 *
 * @note 与 vsf_pt_end() 配合使用；阻塞类宏只能在它们之间使用。
 */
#define vsf_pt_begin()              VSF_UNUSED_PARAM(evt);                      \
                                    __vsf_pt_begin(__vsf_pt_state())

/**
 * \~english
 * @brief End the protothread body started by vsf_pt_begin(), and terminate
 *        the protothread.
 * \~chinese
 * @brief 结束由 vsf_pt_begin() 开始的 protothread 体，并终止该
 *        protothread。
 */
#define vsf_pt_end()                __vsf_pt_end() }

/**
 * \~english
 * @brief Wait cooperatively until the given event arrives.
 * @param[in] __evt: the event to wait for, of type @ref vsf_evt_t
 *
 * @note If the current event is not the expected one, the protothread
 *       returns and resumes here when a new event arrives.
 * \~chinese
 * @brief 协作式等待，直到给定事件到达。
 * @param[in] __evt: 要等待的事件，类型为 @ref vsf_evt_t
 *
 * @note 如果当前事件不是期望的事件，protothread 返回，并在新事件到达时
 *       从此处恢复执行。
 */
#define vsf_pt_wait_for_evt(__evt)  __vsf_pt_wfe_common(__vsf_pt_state(), __evt)

/**
 * \~english
 * @brief Alias of vsf_pt_wait_for_evt().
 * @param[in] __evt: the event to wait for, of type @ref vsf_evt_t
 * \~chinese
 * @brief vsf_pt_wait_for_evt() 的别名。
 * @param[in] __evt: 要等待的事件，类型为 @ref vsf_evt_t
 */
#define vsf_pt_wfe(__evt)           vsf_pt_wait_for_evt(__evt)

/**
 * \~english
 * @brief Yield the cpu cooperatively: mark a re-entry point and post a
 *        VSF_EVT_YIELD event to the current protothread, so that it will be
 *        rescheduled.
 * \~chinese
 * @brief 协作式让出 CPU：标记一个重入点，并向当前 protothread 发送
 *        VSF_EVT_YIELD 事件，使其将被重新调度。
 */
#define vsf_pt_yield()              vsf_pt_entry(__vsf_eda_yield();)

#if VSF_KERNEL_CFG_EDA_FAST_SUB_CALL == ENABLED
/**
 * \~english
 * @brief Set the protothread state to the next re-entry point, without
 *        emitting the re-entry label.
 *
 * @note Used in pairs with vsf_pt_raw_entry() to implement fast sub-calls
 *       (VSF_KERNEL_CFG_EDA_FAST_SUB_CALL is ENABLED).
 * \~chinese
 * @brief 将 protothread 状态设置为下一个重入点，但不生成重入标签。
 *
 * @note 与 vsf_pt_raw_entry() 成对使用，用于实现快速子调用
 *       （VSF_KERNEL_CFG_EDA_FAST_SUB_CALL 为 ENABLED）。
 */
#   define vsf_pt_raw_next()        __vsf_pt_raw_next_common(__vsf_pt_state())

/**
 * \~english
 * @brief Emit a re-entry label, optionally executing the given code first.
 * @param[in] ...: optional code executed before the re-entry label
 *
 * @note Used in pairs with vsf_pt_raw_next() to implement fast sub-calls
 *       (VSF_KERNEL_CFG_EDA_FAST_SUB_CALL is ENABLED).
 * \~chinese
 * @brief 生成一个重入标签，可选地先执行给定代码。
 * @param[in] ...: 在重入标签之前执行的可选代码
 *
 * @note 与 vsf_pt_raw_next() 成对使用，用于实现快速子调用
 *       （VSF_KERNEL_CFG_EDA_FAST_SUB_CALL 为 ENABLED）。
 */
#   define vsf_pt_raw_entry(...)    __vsf_pt_raw_entry_common(__vsf_pt_state(), __VA_ARGS__)
#endif

/*! \note please use if-then clause in the vsf_pt_wait_until()
 *!       E.g.
 *!       vsf_pt_wait_until( if (...) );
 *!
 *!
 *!       NOTE: wait_for_one(), wait_for_any() and wait_for_all() can be seen as
 *!             an if-then clause. So you can use them directly here. E.g.
 *!
 *!       vsf_pt_wait_until( wait_for_one(&__user_grouped_evts, sem_evt_msk) );
 *!
 */
/**
 * \~english
 * @brief Wait cooperatively until the given condition is satisfied.
 * @param[in] ...: the condition to wait for, must be an if-then clause,
 *            e.g. vsf_pt_wait_until( if (...) );
 *
 * @note If the condition is not satisfied, the protothread returns and
 *       resumes here when it is entered again.
 * \~chinese
 * @brief 协作式等待，直到给定条件满足。
 * @param[in] ...: 要等待的条件，必须是 if-then 语句，例如
 *            vsf_pt_wait_until( if (...) );
 *
 * @note 如果条件不满足，protothread 返回，并在再次进入时从此处恢复执行。
 */
#define vsf_pt_wait_until(...)                                                  \
            vsf_pt_entry();                                                     \
            __VA_ARGS__ {} else {                                               \
                return ;                                                        \
            }

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   define __vsf_pt_call_sub(__name, __target, ...)                             \
            __vsf_eda_call_eda( (uintptr_t)(__name),                            \
                                (uintptr_t)(__target),                          \
                                (0, ##__VA_ARGS__))

#   if VSF_KERNEL_CFG_EDA_FAST_SUB_CALL == ENABLED
/**
 * \~english
 * @brief Call an event-driven subroutine (sub-eda) from the current
 *        protothread, and wait until it finishes.
 * @param[in] __name: entry function of the subroutine
 * @param[in] __target: pointer to the target (parameter) of the subroutine
 *
 * @note In this variant (VSF_KERNEL_CFG_EDA_FAST_SUB_CALL is ENABLED), the
 *       subroutine is dispatched directly. The protothread returns while
 *       the subroutine is running, and resumes after it finishes.
 * \~chinese
 * @brief 从当前 protothread 调用一个事件驱动子程序（子 eda），并等待其
 *        完成。
 * @param[in] __name: 子程序的入口函数
 * @param[in] __target: 指向子程序目标（参数）的指针
 *
 * @note 在此变体中（VSF_KERNEL_CFG_EDA_FAST_SUB_CALL 为 ENABLED），子程序
 *       被直接派发。子程序运行期间 protothread 返回，并在其完成后恢复
 *       执行。
 */
#       define vsf_pt_call_sub(__name, __target)                                \
            vsf_pt_raw_next();                                                  \
            vsf_err_t VSF_MACRO_SAFE_NAME(ret) =                                \
                __vsf_pt_call_sub(__name, (__target));                          \
            VSF_KERNEL_ASSERT(VSF_ERR_NONE == VSF_MACRO_SAFE_NAME(ret));        \
            return ;                                                            \
            vsf_pt_raw_entry();
#   else
/**
 * \~english
 * @brief Variant when VSF_KERNEL_CFG_EDA_FAST_SUB_CALL is DISABLED; shares the documentation of the first variant above.
 * \~chinese
 * @brief VSF_KERNEL_CFG_EDA_FAST_SUB_CALL 为 DISABLED 时的变体；与上方第一个变体共用文档。
 */
#       define vsf_pt_call_sub(__name, __target)                                \
            vsf_pt_entry();                                                     \
            if (VSF_ERR_NONE != __vsf_pt_call_sub(__name, (__target))) {        \
                return ;                                                        \
            }                                                                   \
            vsf_pt_entry(return;);
#endif


/**
 * \~english
 * @brief Call another protothread as a subroutine from the current
 *        protothread, and wait until it finishes.
 * @param[in] __name: name of the sub protothread
 * @param[in] __target: pointer to the control block of the sub protothread
 *
 * @note The control block of the sub protothread is reset first.
 * \~chinese
 * @brief 从当前 protothread 调用另一个 protothread 作为子程序，并等待其
 *        完成。
 * @param[in] __name: 子 protothread 名
 * @param[in] __target: 指向子 protothread 控制块的指针
 *
 * @note 先复位子 protothread 的控制块。
 */
#   define vsf_pt_call_pt(__name, __target)                                     \
            (__target)->fsm_state = 0;                                          \
            vsf_pt_call_sub(vsf_pt_func(__name), (__target))

#endif

/**
 * \~english
 * @brief Call a protothread as a subroutine from an event-driven task
 *        (eda), without blocking.
 * @param[in] __name: name of the protothread
 * @param[in] __target: pointer to the control block of the protothread
 * @return vsf_err_t: VSF_ERR_NONE if the subroutine call is started
 *         successfully; VSF_ERR_NOT_ENOUGH_RESOURCES if no frame is available
 *
 * @note Used inside a plain eda event handler; inside a protothread please
 *       use vsf_pt_call_pt() instead.
 * \~chinese
 * @brief 从事件驱动任务（eda）中调用一个 protothread 作为子程序，不阻塞。
 * @param[in] __name: protothread 名
 * @param[in] __target: 指向 protothread 控制块的指针
 * @return vsf_err_t: 子程序调用启动成功返回 VSF_ERR_NONE；没有可用的帧返回
 * VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note 在普通 eda 事件处理函数中使用；在 protothread 内请使用
 *       vsf_pt_call_pt()。
 */
#define vsf_eda_call_pt(__name, __target)                                       \
            __vsf_pt_call_sub(vsf_pt_func(__name), (__target))

#if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
#   define __vsf_pt_call_task(__name, __target, ...)                            \
            __vsf_eda_call_task((vsf_task_entry_t)(__name),                     \
                                (uintptr_t)(__target),                          \
                                (0, ##__VA_ARGS__))


/**
 * \~english
 * @brief Call a task as a sub-task from the current protothread, and wait
 *        until it finishes.
 * @param[in] __name: name of the sub-task
 * @param[in] __target: pointer to the control block of the sub-task
 * @param[out] __ret_addr: pointer receiving the @ref fsm_rt_t return value
 *            of the sub-task, can be NULL
 * @param[in] ...: optional size of the local variables of the sub-task frame
 *
 * @note The protothread returns while the sub-task is running, and resumes
 *       after it finishes. Only available when
 *       VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE is ENABLED.
 * \~chinese
 * @brief 从当前 protothread 调用一个任务作为子任务，并等待其完成。
 * @param[in] __name: 子任务名
 * @param[in] __target: 指向子任务控制块的指针
 * @param[out] __ret_addr: 用于接收子任务 @ref fsm_rt_t 返回值的指针，
 *            可以为 NULL
 * @param[in] ...: 子任务栈帧局部变量的可选大小
 *
 * @note 子任务运行期间 protothread 返回，并在其完成后恢复执行。仅在
 *       VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE 为 ENABLED 时可用。
 */
#   define vsf_pt_call_task(__name, __target, __ret_addr, ...)                  \
        do {                                                                    \
            fsm_rt_t VSF_MCONNECT3(__vsf_pt_call_task,__LINE__,tReturn);        \
            vsf_pt_entry();                                                     \
            VSF_MCONNECT3(__vsf_pt_call_task,__LINE__,tReturn) =                \
                __vsf_pt_call_task(vsf_task_func(__name), (__target), (0, ##__VA_ARGS__));\
            if (fsm_rt_on_going ==                                              \
                VSF_MCONNECT3(__vsf_pt_call_task,__LINE__,tReturn)) {           \
                return ;                                                        \
            }                                                                   \
            if (NULL != (__ret_addr)) {                                         \
                *(__ret_addr) =                                                 \
                    VSF_MCONNECT3(__vsf_pt_call_task,__LINE__,tReturn);         \
            }                                                                   \
        } while(0)
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
/**
 * \~english
 * @brief Start a protothread with the given configuration. In this variant
 *        (VSF_KERNEL_CFG_EDA_SUPPORT_TIMER is ENABLED) it is mapped to
 *        vsf_teda_start(), so timer services are available.
 * @param[in] __pt: pointer to the protothread, of type @ref vsf_pt_t
 * @param[in] __cfg: pointer to the configuration, of type @ref vsf_eda_cfg_t
 * @return vsf_err_t: VSF_ERR_NONE if the protothread is started
 *         successfully; VSF_ERR_NOT_ENOUGH_RESOURCES if no frame is available
 *
 * @note A VSF_EVT_INIT event is sent to the protothread once it is started.
 * \~chinese
 * @brief 以给定配置启动 protothread。在此变体中
 *        （VSF_KERNEL_CFG_EDA_SUPPORT_TIMER 为 ENABLED）映射为
 *        vsf_teda_start()，可使用定时器服务。
 * @param[in] __pt: 指向 protothread 的指针，类型为 @ref vsf_pt_t
 * @param[in] __cfg: 指向配置的指针，类型为 @ref vsf_eda_cfg_t
 * @return vsf_err_t: protothread 启动成功返回 VSF_ERR_NONE；没有可用的帧返回 VSF_ERR_NOT_ENOUGH_RESOURCES
 *
 * @note protothread 启动后会收到一个 VSF_EVT_INIT 事件。
 */
#   define vsf_pt_start                 vsf_teda_start
#else
/**
 * \~english
 * @brief Variant when VSF_KERNEL_CFG_EDA_SUPPORT_TIMER is DISABLED; shares the documentation of the first variant above.
 * \~chinese
 * @brief VSF_KERNEL_CFG_EDA_SUPPORT_TIMER 为 DISABLED 时的变体；与上方第一个变体共用文档。
 */
#   define vsf_pt_start                 vsf_eda_start
#endif



/**
 * \~english
 * @brief Start the implementation of the protothread entry function.
 * @param[in] __name: name of the protothread
 *
 * @note Inside the body, vsf_pthis points to the protothread control block,
 *       and vsf_this gives direct access to its members. The body must be
 *       bracketed by vsf_pt_begin() and vsf_pt_end().
 * @note Used together with def_vsf_pt() and init_vsf_pt().
 * \~chinese
 * @brief 开始 protothread 入口函数的实现。
 * @param[in] __name: protothread 名
 *
 * @note 在函数体内，vsf_pthis 指向 protothread 控制块，vsf_this 可直接
 *       访问其成员。函数体必须由 vsf_pt_begin() 和 vsf_pt_end() 包围。
 * @note 与 def_vsf_pt() 和 init_vsf_pt() 配合使用。
 */
#define implement_vsf_pt(__name)        __implement_vsf_pt(__name)

/**
 * \~english
 * @brief Start the implementation of the protothread entry function with a
 *        dedicated function name.
 * @param[in] __name: name of the protothread
 * @param[in] __FUNC_NAME: function name of the entry function
 * \~chinese
 * @brief 以指定的函数名开始 protothread 入口函数的实现。
 * @param[in] __name: protothread 名
 * @param[in] __FUNC_NAME: 入口函数的函数名
 */
#define implement_vsf_pt_ex(__name, __FUNC_NAME)                                \
            __implement_vsf_pt_ex(__name, __FUNC_NAME)

/**
 * \~english
 * @brief Alias of implement_vsf_pt().
 * @param[in] __name: name of the protothread
 * \~chinese
 * @brief implement_vsf_pt() 的别名。
 * @param[in] __name: protothread 名
 */
#define imp_vsf_pt(__name)              implement_vsf_pt(__name)

/**
 * \~english
 * @brief Alias of implement_vsf_pt_ex().
 * @param[in] __name: name of the protothread
 * @param[in] __FUNC_NAME: function name of the entry function
 * \~chinese
 * @brief implement_vsf_pt_ex() 的别名。
 * @param[in] __name: protothread 名
 * @param[in] __FUNC_NAME: 入口函数的函数名
 */
#define imp_vsf_pt_ex(__name, __FUNC_NAME)                                      \
            implement_vsf_pt_ex(__name, __FUNC_NAME)

/**
 * \~english
 * @brief Expand to the name of the protothread entry function.
 * @param[in] __name: name of the protothread
 * \~chinese
 * @brief 展开为 protothread 入口函数的函数名。
 * @param[in] __name: protothread 名
 */
#define __vsf_pt_func(__name)           __vsf_pt_func_common(__name)
#define vsf_pt_func(__name)             __vsf_pt_func(__name)

/**
 * \~english
 * @brief Expand to the type name of the protothread control block.
 * @param[in] __name: name of the protothread
 * \~chinese
 * @brief 展开为 protothread 控制块的类型名。
 * @param[in] __name: protothread 名
 */
#define __vsf_pt(__name)                __vsf_pt_common(__name)
#define vsf_pt(__name)                  __vsf_pt(__name)

#define __def_vsf_pt(__name,...)                                                \
            __def_vsf_pt_common(__name,                                         \
                                uint8_t fsm_state;                              \
                                __VA_ARGS__)

/**
 * \~english
 * @brief Define a protothread control block.
 * @param[in] __name: name of the protothread
 * @param[in] ...: member variables of the protothread control block
 *
 * @note The first member of the protothread control block is fsm_state,
 *       which is used internally to hold the protothread state.
 * @note Used together with implement_vsf_pt() and init_vsf_pt().
 * \~chinese
 * @brief 定义 protothread 控制块。
 * @param[in] __name: protothread 名
 * @param[in] ...: protothread 控制块的成员变量
 *
 * @note protothread 控制块的第一个成员是 fsm_state，供内部保存
 *       protothread 状态使用。
 * @note 与 implement_vsf_pt() 和 init_vsf_pt() 配合使用。
 */
#define def_vsf_pt(__name,...)          __def_vsf_pt(__name,__VA_ARGS__)

/**
 * \~english
 * @brief End a protothread definition started by def_vsf_pt(). It is empty
 *        and only used for coding style symmetry.
 * \~chinese
 * @brief 结束由 def_vsf_pt() 开始的 protothread 定义。该宏为空，仅用于
 *        编码风格上的对称。
 */
#define end_def_vsf_pt(...)

/**
 * \~english
 * @brief Alias of def_vsf_pt().
 * @param[in] __name: name of the protothread
 * @param[in] ...: member variables of the protothread control block
 * \~chinese
 * @brief def_vsf_pt() 的别名。
 * @param[in] __name: protothread 名
 * @param[in] ...: protothread 控制块的成员变量
 */
#define define_vsf_pt(__name,...)       def_vsf_pt(__name,__VA_ARGS__)

/**
 * \~english
 * @brief Alias of end_def_vsf_pt().
 * \~chinese
 * @brief end_def_vsf_pt() 的别名。
 */
#define end_define_vsf_pt(...)



#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   define __declare_vsf_pt(__name)                                             \
            __declare_vsf_pt_common(__name)                                     \
            __extern_vsf_pt_common(__name, uintptr_t local)
#else
#   define __declare_vsf_pt(__name)                                             \
            __declare_vsf_pt_common(__name)                                     \
            __extern_vsf_pt_common(__name, __name *vsf_pthis)
#endif

/**
 * \~english
 * @brief Declare a protothread (the protothread class, its control block
 *        type and its entry function prototype) before it is defined by
 *        def_vsf_pt().
 * @param[in] __name: name of the protothread
 * \~chinese
 * @brief 在使用 def_vsf_pt() 定义之前，先声明 protothread（protothread
 *        类、其控制块类型及其入口函数原型）。
 * @param[in] __name: protothread 名
 */
#define declare_vsf_pt(__name)          __declare_vsf_pt(__name)

/**
 * \~english
 * @brief Alias of declare_vsf_pt().
 * @param[in] __name: name of the protothread
 * \~chinese
 * @brief declare_vsf_pt() 的别名。
 * @param[in] __name: protothread 名
 */
#define dcl_vsf_pt(__name)              declare_vsf_pt(__name)

#define __init_vsf_pt(__name, __pt, __pri, ...)                                 \
        do {                                                                    \
            vsf_eda_cfg_t VSF_MACRO_SAFE_NAME(cfg) = {                          \
                .fn.evthandler = (vsf_pt_entry_t)__vsf_pt_func(__name),         \
                .priority = (__pri),                                            \
                .target = (uintptr_t)&((__pt)->param),                          \
                __VA_ARGS__                                                     \
            };                                                                  \
            (__pt)->param.fsm_state = 0;                                        \
            vsf_pt_start(&((__pt)->use_as__vsf_pt_t),                           \
                &VSF_MACRO_SAFE_NAME(cfg));                                     \
        } while(0)



#if     VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED                                \
    &&  VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED

#   if VSF_KERNEL_CFG_EDA_FAST_SUB_CALL == ENABLED
/**
 * \~english
 * @brief Call a thread from the current protothread, and wait until the
 *        thread terminates.
 * @param[in] __name: name of the thread type defined by def_vsf_thread()
 * @param[in] __target: pointer to the thread control block
 *
 * @note In this variant (VSF_KERNEL_CFG_EDA_FAST_SUB_CALL is ENABLED), the
 *       thread is started directly. The protothread returns while the
 *       thread is running, and resumes after the thread terminates.
 * \~chinese
 * @brief 从当前 protothread 调用一个线程，并等待线程结束。
 * @param[in] __name: 由 def_vsf_thread() 定义的线程类型名
 * @param[in] __target: 指向线程控制块的指针
 *
 * @note 在此变体中（VSF_KERNEL_CFG_EDA_FAST_SUB_CALL 为 ENABLED），线程被
 *       直接启动。线程运行期间 protothread 返回，并在线程结束后恢复
 *       执行。
 */
#       define vsf_pt_call_thread(__name, __target)                             \
            vsf_eda_call_thread_prepare(__name, __target);                      \
            vsf_pt_raw_next();                                                  \
            vsf_err_t VSF_MACRO_SAFE_NAME(ret) =                                \
                                vsf_eda_call_thread(__target);                  \
            VSF_KERNEL_ASSERT(VSF_ERR_NONE == VSF_MACRO_SAFE_NAME(ret));        \
            return ;                                                            \
            vsf_pt_raw_entry();
#   else
/**
 * \~english
 * @brief Variant when VSF_KERNEL_CFG_EDA_FAST_SUB_CALL is DISABLED; shares the documentation of the first variant above.
 * \~chinese
 * @brief VSF_KERNEL_CFG_EDA_FAST_SUB_CALL 为 DISABLED 时的变体；与上方第一个变体共用文档。
 */
#       define vsf_pt_call_thread(__name, __target)                             \
            vsf_eda_call_thread_prepare(__name, __target);                      \
            vsf_pt_entry();                                                     \
            if (VSF_ERR_NONE != vsf_eda_call_thread(__target)) {                \
                return ;                                                        \
            }                                                                   \
            vsf_pt_entry(return;);
#   endif
#endif

/**
 * \~english
 * @brief Prepare and start a protothread defined by def_vsf_pt().
 * @param[in] __name: name of the protothread
 * @param[in] __pt: pointer to the protothread instance
 * @param[in] __pri: priority of the protothread, of type @ref vsf_prio_t
 * @param[in] ...: optional extra initializers for @ref vsf_eda_cfg_t
 *
 * @note The protothread control block is reset first, then the protothread
 *       is started via vsf_pt_start(); the protothread entry function will
 *       receive a VSF_EVT_INIT event.
 * \~chinese
 * @brief 准备并启动由 def_vsf_pt() 定义的 protothread。
 * @param[in] __name: protothread 名
 * @param[in] __pt: 指向 protothread 实例的指针
 * @param[in] __pri: protothread 优先级，类型为 @ref vsf_prio_t
 * @param[in] ...: @ref vsf_eda_cfg_t 的可选额外初始化项
 *
 * @note 先复位 protothread 控制块，再通过 vsf_pt_start() 启动
 *       protothread；protothread 入口函数将收到 VSF_EVT_INIT 事件。
 */
#define init_vsf_pt(__name, __pt, __pri, ...)                                   \
            __init_vsf_pt(__name, __pt, __pri, __VA_ARGS__)

/*============================ TYPES =========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
/**
 * \~english
 * @brief Protothread type, alias of @ref vsf_teda_t when
 *        VSF_KERNEL_CFG_EDA_SUPPORT_TIMER is ENABLED
 * \~chinese
 * @brief protothread 类型，启用 VSF_KERNEL_CFG_EDA_SUPPORT_TIMER 时是
 *        @ref vsf_teda_t 的别名
 */
typedef vsf_teda_t vsf_pt_t;
#else
/**
 * \~english
 * @brief Protothread type, alias of @ref vsf_eda_t when
 *        VSF_KERNEL_CFG_EDA_SUPPORT_TIMER is DISABLED
 * \~chinese
 * @brief protothread 类型，禁用 VSF_KERNEL_CFG_EDA_SUPPORT_TIMER 时是
 *        @ref vsf_eda_t 的别名
 */
typedef vsf_eda_t vsf_pt_t;
#endif

/**
 * \~english
 * @brief Protothread entry function, with the eda event handler signature
 *        (alias of @ref vsf_eda_evthandler_t)
 * \~chinese
 * @brief protothread 入口函数，采用 eda 事件处理函数签名
 *        （@ref vsf_eda_evthandler_t 的别名）
 */
typedef vsf_eda_evthandler_t    vsf_pt_entry_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
#endif
