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

/****************************************************************************
*  Copyright 2017 Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)       *
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

#ifndef __SIMPLE_FSM_H__
#define __SIMPLE_FSM_H__

/*============================ INCLUDES ======================================*/
#include "./kernel/vsf_kernel_cfg.h"

#include "utilities/vsf_utilities.h"
/*============================ MACROS ========================================*/

#if (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L) && !defined(__cplusplus)
#   undef VSF_KERNEL_CFG_EDA_SUPPORT_SIMPLE_FSM
#   define VSF_KERNEL_CFG_EDA_SUPPORT_SIMPLE_FSM        DISABLED
#endif

#if     VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED                  \
    &&  VSF_USE_KERNEL == ENABLED                                               \
    &&  VSF_KERNEL_CFG_EDA_SUPPORT_SIMPLE_FSM == ENABLED                        \
    &&  VSF_KERNEL_CFG_EDA_SUPPORT_TASK == ENABLED

#ifdef __cplusplus
extern "C" {
#endif


#ifndef vsf_this
#   define vsf_this    (*vsf_pthis)
#endif


#if (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L) && !defined(__cplusplus)
#   error simple_fsm require at least ANSI-C99 support
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/**
 * \~english
 * @brief Define the states of a simple fsm as an anonymous enum.
 * @param[in] ...: the state names
 *
 * @note The first enumerator START is 0, which is the initial state.
 * \~chinese
 * @brief 以匿名枚举定义简单状态机的状态。
 * @param[in] ...: 状态名列表
 *
 * @note 第一个枚举值 START 为 0，即初始状态。
 */
#define def_states(...)                                                         \
        enum {                                                                  \
            START = 0,                                                          \
            __VA_ARGS__                                                         \
        };

#ifndef def_params
/**
 * \~english
 * @brief Expand to the parameter declarations of the fsm initialiser.
 * @param[in] ...: the parameter declarations
 * \~chinese
 * @brief 展开为状态机初始化函数的参数声明。
 * @param[in] ...: 参数声明
 */
#   define def_params(...)      __VA_ARGS__
#endif

/**
 * \~english
 * @brief Expand to extra arguments with a leading comma, used to append
 *        arguments to the fsm initialiser call.
 * @param[in] ...: the extra arguments
 * \~chinese
 * @brief 展开为带前导逗号的额外参数，用于向状态机初始化函数调用追加
 *        参数。
 * @param[in] ...: 额外参数
 */
#define vsf_args(...)           ,__VA_ARGS__

/**
 * \~english
 * @brief Expand to the type name of the fsm control block.
 * @param[in] __name: name of the fsm
 * \~chinese
 * @brief 展开为状态机控制块的类型名。
 * @param[in] __name: 状态机名
 */
#define vsf_fsm(__name)         vsf_task(__name)

#define __def_fsm(__fsm_type, ...)                                              \
        def_vsf_task(__fsm_type,                                                \
            __VA_ARGS__);

/**
 * \~english
 * @brief Define an fsm as a vsf task: define the fsm control block and
 *        declare its entry function.
 * @param[in] __name: name of the fsm
 * @param[in] ...: member variables of the fsm control block
 *
 * @note Based on def_vsf_task(); used together with implement_vsf_fsm()
 *       and start_vsf_fsm().
 * \~chinese
 * @brief 将状态机定义为 vsf 任务：定义状态机控制块并声明其入口函数。
 * @param[in] __name: 状态机名
 * @param[in] ...: 状态机控制块的成员变量
 *
 * @note 基于 def_vsf_task()；与 implement_vsf_fsm() 和 start_vsf_fsm()
 *       配合使用。
 */
#define def_fsm(__name, ...)                                                    \
        __def_fsm(__name, __VA_ARGS__)

/**
 * \~english
 * @brief Declare and define a simple fsm.
 * @param[in] __name: name of the fsm
 * @param[in] ...: member variables of the fsm control block
 * \~chinese
 * @brief 声明并定义一个简单状态机。
 * @param[in] __name: 状态机名
 * @param[in] ...: 状态机控制块的成员变量
 */
#define def_simple_fsm(__name, ...)                                             \
        __declare_fsm(__name);                                                  \
        __def_fsm(__name, __VA_ARGS__)

/**
 * \~english
 * @brief End a simple fsm definition started by def_simple_fsm(). It is
 *        empty and only used for coding style symmetry.
 * \~chinese
 * @brief 结束由 def_simple_fsm() 开始的简单状态机定义。该宏为空，仅用于
 *        编码风格上的对称。
 */
#define end_def_simple_fsm(...)

/*
#define __extern_simple_fsm(__fsm_type, ...)                                    \
        declare_class(__fsm_type)                                               \
        extern_class(__fsm_type)                                                \
            uint_fast8_t fsm_state;                                             \
            __VA_ARGS__                                                         \
        end_extern_class(__fsm_type)

#define extern_simple_fsm(__name, ...)                                          \
        __extern_simple_fsm(fsm(__name), __VA_ARGS__)
*/

#define __declare_fsm(__name)                                                   \
        declare_vsf_task(__name)

/**
 * \~english
 * @brief Declare an fsm (the fsm class and its control block type) before
 *        it is defined by def_simple_fsm().
 * @param[in] __name: name of the fsm
 * \~chinese
 * @brief 在使用 def_simple_fsm() 定义之前，先声明状态机（状态机类及其
 *        控制块类型）。
 * @param[in] __name: 状态机名
 */
#define declare_vsf_fsm(__name)         __declare_fsm(__name)

/*! \brief extern fsm initialisation function and provide function prototype
           as <__name>_fn, E.g
           extern_vsf_fsm_initialiser( demo_fsm );
           we extern a function called:
           extern fsm_demo_fsm_t *demo_fsm_init( fsm_demo_fsm_t *fsm_ptr );
           and a prototype definition:
           typedef fsm_demo_fsm_t *demo_fsm_init_fn( fsm_demo_fsm_t *fsm_ptr );
           We can then use demo_fsm_init_fn to define function pointer
 */
#define __extern_fsm_initialiser(__name, ...)                                   \
        vsf_fsm(__name) *__name##_init(vsf_fsm(__name) *fsm_ptr __VA_ARGS__);           \
        typedef vsf_fsm(__name) *__name##_init_fn(vsf_fsm(__name) *fsm_ptr __VA_ARGS__);

/**
 * \~english
 * @brief Declare the initialiser function of an fsm and provide the
 *        corresponding function prototype.
 * @param[in] __name: name of the fsm
 * @param[in] ...: optional extra parameters of the initialiser, declared
 *            with def_params()
 *
 * @note It declares a function <__name>_init() returning a pointer to the
 *       fsm control block, and a function prototype typedef
 *       <__name>_init_fn, which can be used to define function pointers.
 * \~chinese
 * @brief 声明状态机的初始化函数，并提供对应的函数原型。
 * @param[in] __name: 状态机名
 * @param[in] ...: 初始化函数的可选额外参数，用 def_params() 声明
 *
 * @note 声明一个返回状态机控制块指针的函数 <__name>_init()，以及函数原型
 *       类型定义 <__name>_init_fn，可用于定义函数指针。
 */
#define extern_vsf_fsm_initialiser(__name, ...)                                     \
            __extern_fsm_initialiser(__name, __VA_ARGS__)


/*! \brief extern fsm task function and provide function prototype as <__name>_fn, E.g
           extern_vsf_fsm_implementation( demo_fsm );
           we extern a function called:
           fsm_rt_t demo_fsm( fsm_demo_fsm_t *fsm_ptr )
           and a prototype definition:
           typedef fsm_rt_t demo_fsm_fn (fsm_demo_fsm_t *fsm_ptr);
           We can then use demo_fsm_fn to define function pointer
 */
 //! @{
#define __extern_fsm_implementation_ex(__name,__type)                           \
        fsm_rt_t __name(fsm(__type) *vsf_pthis, vsf_evt_t evt);                 \
        typedef fsm_rt_t __name##_fn( vsf_fsm(__type) *vsf_pthis, vsf_evt_t evt );

/**
 * \~english
 * @brief Declare the entry function of an fsm implementation and provide
 *        the corresponding function prototype, with a dedicated fsm type.
 * @param[in] __name: name of the fsm entry function
 * @param[in] __type: name of the fsm type
 *
 * @note It declares a function <__name>() returning @ref fsm_rt_t, and a
 *       function prototype typedef <__name>_fn, which can be used to define
 *       function pointers.
 * \~chinese
 * @brief 以指定的状态机类型，声明状态机实现的入口函数，并提供对应的
 *        函数原型。
 * @param[in] __name: 状态机入口函数名
 * @param[in] __type: 状态机类型名
 *
 * @note 声明一个返回 @ref fsm_rt_t 的函数 <__name>()，以及函数原型类型
 *       定义 <__name>_fn，可用于定义函数指针。
 */
#define declare_vsf_fsm_implementation_ex(__name, __type)                       \
            __extern_fsm_implementation_ex(__name, __type)

/**
 * \~english
 * @brief Alias of declare_vsf_fsm_implementation_ex().
 * @param[in] __name: name of the fsm entry function
 * @param[in] __type: name of the fsm type
 * \~chinese
 * @brief declare_vsf_fsm_implementation_ex() 的别名。
 * @param[in] __name: 状态机入口函数名
 * @param[in] __type: 状态机类型名
 */
#define extern_vsf_fsm_implementation_ex(__name,__type)                         \
            __extern_fsm_implementation_ex(__name, __type)

/**
 * \~english
 * @brief Declare the entry function of an fsm implementation and provide
 *        the corresponding function prototype.
 * @param[in] __name: name of the fsm
 * \~chinese
 * @brief 声明状态机实现的入口函数，并提供对应的函数原型。
 * @param[in] __name: 状态机名
 */
#define extern_vsf_fsm_implementation(__name)                                   \
            __extern_fsm_implementation_ex(__name, __name)

/**
 * \~english
 * @brief Alias of extern_vsf_fsm_implementation().
 * @param[in] __name: name of the fsm
 * \~chinese
 * @brief extern_vsf_fsm_implementation() 的别名。
 * @param[in] __name: 状态机名
 */
#define declare_vsf_fsm_implementation(__name)                                  \
            __extern_fsm_implementation_ex(__name, __name)
//! @}

/**
 * \~english
 * @brief Call a sub-fsm from the current fsm, and wait until it finishes.
 * @param[in] __name: name of the sub-fsm
 * @param[in] __fsm: pointer to the control block of the sub-fsm
 * @return fsm_rt_t: the return value of the sub-fsm when it finishes
 *
 * @note Based on vsf_task_call_task().
 * \~chinese
 * @brief 从当前状态机调用一个子状态机，并等待其完成。
 * @param[in] __name: 子状态机名
 * @param[in] __fsm: 指向子状态机控制块的指针
 * @return fsm_rt_t: 子状态机完成时的返回值
 *
 * @note 基于 vsf_task_call_task()。
 */
#define call_vsf_fsm(__name, __fsm )                                            \
            vsf_task_call_task(__name, __fsm)

#define ____state(__state, ...)                                                 \
            case __state:                                                       \
        __state_entry_##__state:{                                               \
                __VA_ARGS__;                                                    \
            };

/**
 * \~english
 * @brief Define a state of the fsm inside vsf_fsm_body() (or between
 *        vsf_fsm_begin() and vsf_fsm_end()).
 * @param[in] __state: name of the state, defined by def_states()
 * @param[in] ...: the code executed in this state
 * \~chinese
 * @brief 在 vsf_fsm_body() 内（或 vsf_fsm_begin() 与 vsf_fsm_end() 之间）
 *        定义状态机的一个状态。
 * @param[in] __state: 状态名，由 def_states() 定义
 * @param[in] ...: 在该状态下执行的代码
 */
#define vsf_state(__state, ...)                 break; ____state(__state, __VA_ARGS__)

/**
 * \~english
 * @brief Define the code executed once when the fsm starts (state START).
 * @param[in] ...: the code executed on start
 * \~chinese
 * @brief 定义状态机启动时（START 状态）执行一次的代码。
 * @param[in] ...: 启动时执行的代码
 */
#define on_start(...)                       {__VA_ARGS__;}


/**
 * \~english
 * @brief Reset the fsm to its initial state.
 * \~chinese
 * @brief 将状态机复位到初始状态。
 */
#define reset_vsf_fsm()         do { vsf_this.fsm_state = 0; } while(0);

/**
 * \~english
 * @brief Reset the fsm and return @ref fsm_rt_cpl to report completion.
 * \~chinese
 * @brief 复位状态机并返回 @ref fsm_rt_cpl，报告完成。
 */
#define vsf_fsm_cpl()           do {reset_vsf_fsm(); return fsm_rt_cpl;} while(0);

/**
 * \~english
 * @brief Reset the fsm and return the given value to report an error (or a
 *        user-defined result).
 * @param[in] __ERROR: the value returned as @ref fsm_rt_t
 * \~chinese
 * @brief 复位状态机并返回给定值，报告错误（或用户自定义结果）。
 * @param[in] __ERROR: 作为 @ref fsm_rt_t 返回的值
 */
#define vsf_fsm_report(__ERROR) do {reset_vsf_fsm(); return (fsm_rt_t)(__ERROR); } while(0);

/**
 * \~english
 * @brief Return @ref fsm_rt_wait_for_obj, so the fsm waits for an object
 *        (e.g. a semaphore or a mutex).
 * \~chinese
 * @brief 返回 @ref fsm_rt_wait_for_obj，使状态机等待某个对象（例如信号量
 *        或互斥量）。
 */
#define vsf_fsm_wait_for_obj()  return fsm_rt_wait_for_obj;

/**
 * \~english
 * @brief Return @ref fsm_rt_on_going, so the fsm keeps running and resumes
 *        at the current state next time.
 * \~chinese
 * @brief 返回 @ref fsm_rt_on_going，使状态机继续运行，下次从当前状态恢复
 *        执行。
 */
#define vsf_fsm_on_going()      return fsm_rt_on_going;

//! fsm_continue is deprecated, should not be used anymore
//#define fsm_continue()      break


/**
 * \~english
 * @brief Set the fsm to the given state and jump to its entry within the
 *        current call.
 * @param[in] __state: name of the target state, defined by def_states()
 *
 * @note The code of the target state is executed immediately in the current
 *       call, without returning from the entry function.
 * \~chinese
 * @brief 将状态机设置为给定状态，并在当前调用内跳转到该状态入口。
 * @param[in] __state: 目标状态名，由 def_states() 定义
 *
 * @note 目标状态的代码立即在当前调用内执行，无需从入口函数返回。
 */
#define update_state_to(__state)                                                \
        { vsf_this.fsm_state = (__state); goto __state_entry_##__state;}

/**
 * \~english
 * @brief Set the fsm to the given state and return @ref fsm_rt_on_going,
 *        so the target state is executed the next time the fsm runs.
 * @param[in] __state: name of the target state, defined by def_states()
 * \~chinese
 * @brief 将状态机设置为给定状态并返回 @ref fsm_rt_on_going，目标状态将在
 *        状态机下次运行时执行。
 * @param[in] __state: 目标状态名，由 def_states() 定义
 */
#define transfer_to(__state)                                                    \
         { vsf_this.fsm_state = (__state); vsf_fsm_on_going() }


#define __fsm_initialiser(__name, ...)                                          \
        vsf_fsm(__name) *__name##_init(vsf_fsm(__name) *vsf_pthis __VA_ARGS__)  \
        {                                                                       \
            VSF_KERNEL_ASSERT (NULL != vsf_pthis);                              \
            vsf_this.fsm_state = 0;

/**
 * \~english
 * @brief Start the implementation of the fsm initialiser function
 *        <__name>_init().
 * @param[in] __name: name of the fsm
 * @param[in] ...: optional extra parameters of the initialiser, declared
 *            with def_params()
 *
 * @note The fsm control block is checked against NULL and reset first. The
 *       body must be closed by vsf_fsm_init_body().
 * \~chinese
 * @brief 开始状态机初始化函数 <__name>_init() 的实现。
 * @param[in] __name: 状态机名
 * @param[in] ...: 初始化函数的可选额外参数，用 def_params() 声明
 *
 * @note 先对状态机控制块进行 NULL 检查并复位。函数体必须由
 *       vsf_fsm_init_body() 结束。
 */
#define vsf_fsm_initialiser(__name, ...)                                        \
            __fsm_initialiser(__name, __VA_ARGS__)


/**
 * \~english
 * @brief Abort the fsm initialiser by returning NULL.
 *
 * @note Used inside the fsm initialiser when the initialisation fails.
 * \~chinese
 * @brief 通过返回 NULL 中止状态机初始化。
 *
 * @note 在状态机初始化函数内、初始化失败时使用。
 */
#define abort_vsf_fsm_init()     return NULL;

/**
 * \~english
 * @brief The body of the fsm initialiser, closed with returning the fsm
 *        control block pointer.
 * @param[in] ...: the initialisation code
 * \~chinese
 * @brief 状态机初始化函数的函数体，结束时返回状态机控制块指针。
 * @param[in] ...: 初始化代码
 */
#define vsf_fsm_init_body(...)                                                  \
            __VA_ARGS__                                                         \
            return &vsf_this;                                                   \
        }


/**
 * \~english
 * @brief Initialize an fsm by calling its initialiser <__name>_init().
 * @param[in] __name: name of the fsm
 * @param[in] __fsm: pointer to the fsm control block
 * @param[in] ...: optional extra arguments of the initialiser
 * @return pointer to the fsm control block, or NULL if aborted
 * \~chinese
 * @brief 通过调用初始化函数 <__name>_init() 初始化状态机。
 * @param[in] __name: 状态机名
 * @param[in] __fsm: 指向状态机控制块的指针
 * @param[in] ...: 初始化函数的可选额外参数
 * @return 指向状态机控制块的指针；若初始化被中止则返回 NULL
 */
#define init_vsf_fsm(__name, __fsm, ...)                                        \
        __name##_init(__fsm __VA_ARGS__)

/**
 * \~english
 * @brief Initialize a simple fsm, mapped to init_vsf_fsm().
 * @param[in] __name: name of the fsm
 * @param[in] __fsm: pointer to the fsm control block
 * @param[in] ...: optional extra arguments of the initialiser
 * \~chinese
 * @brief 初始化一个简单状态机，映射为 init_vsf_fsm()。
 * @param[in] __name: 状态机名
 * @param[in] __fsm: 指向状态机控制块的指针
 * @param[in] ...: 初始化函数的可选额外参数
 */
#define init_simple_fsm(__name, __fsm, ...)                                     \
        init_vsf_fsm(__name, __fsm, __VA_ARGS__)

/**
 * \~english
 * @brief Prepare and start an fsm as a vsf task.
 * @param[in] __name: name of the fsm
 * @param[in] __fsm: pointer to the fsm instance
 * @param[in] __pri: priority of the fsm task, of type @ref vsf_prio_t
 * @param[in] ...: optional extra initializers for @ref vsf_eda_cfg_t
 *
 * @note Mapped to init_vsf_task(); the fsm entry function will receive a
 *       VSF_EVT_INIT event.
 * \~chinese
 * @brief 准备并将状态机作为 vsf 任务启动。
 * @param[in] __name: 状态机名
 * @param[in] __fsm: 指向状态机实例的指针
 * @param[in] __pri: 状态机任务优先级，类型为 @ref vsf_prio_t
 * @param[in] ...: @ref vsf_eda_cfg_t 的可选额外初始化项
 *
 * @note 映射为 init_vsf_task()；状态机入口函数将收到 VSF_EVT_INIT 事件。
 */
#define start_vsf_fsm(__name, __fsm, __pri, ...)                                \
            init_vsf_task(__name, (__fsm), (__pri), __VA_ARGS__)

/**
 * \~english
 * @brief Alias of start_vsf_fsm().
 * @param[in] __name: name of the fsm
 * @param[in] __fsm: pointer to the fsm instance
 * @param[in] __pri: priority of the fsm task, of type @ref vsf_prio_t
 * @param[in] ...: optional extra initializers for @ref vsf_eda_cfg_t
 * \~chinese
 * @brief start_vsf_fsm() 的别名。
 * @param[in] __name: 状态机名
 * @param[in] __fsm: 指向状态机实例的指针
 * @param[in] __pri: 状态机任务优先级，类型为 @ref vsf_prio_t
 * @param[in] ...: @ref vsf_eda_cfg_t 的可选额外初始化项
 */
#define start_simple_fsm(__name, __fsm, __pri, ...)                             \
            start_vsf_fsm(__name, (__fsm), (__pri), __VA_ARGS__)

#define __implement_fsm_ex(__name, __type)                                      \
    implement_vsf_task(__name)                                                  \
    {                                                                           \
        vsf_task_begin();                                                       \
        if (NULL == vsf_pthis) {                                                \
            return fsm_rt_err;                                                  \
        }

#define __body(...)                                                             \
        switch (vsf_this.fsm_state) {                                           \
            case 0:                                                             \
                vsf_this.fsm_state++;                                           \
            __VA_ARGS__                                                         \
            break;                                                              \
            default:                                                            \
            return fsm_rt_err;                                                  \
        }                                                                       \
                                                                                \
        vsf_task_end();                                                         \
    }

/**
 * \~english
 * @brief The state-switch body of the fsm, containing all the states.
 * @param[in] ...: the states defined by vsf_state()
 *
 * @note It dispatches on the current fsm state; an unknown state returns
 *       @ref fsm_rt_err. Used inside implement_vsf_fsm().
 * \~chinese
 * @brief 状态机的状态分支体，包含所有状态。
 * @param[in] ...: 由 vsf_state() 定义的状态
 *
 * @note 根据当前状态机状态进行分支；未知状态返回 @ref fsm_rt_err。在
 *       implement_vsf_fsm() 内使用。
 */
#define vsf_fsm_body(...)               __body(__VA_ARGS__)

/**
 * \~english
 * @brief Begin the state-switch of the fsm, used together with
 *        vsf_fsm_end() instead of vsf_fsm_body().
 *
 * @note Compared with vsf_fsm_body(), the code between vsf_fsm_begin() and
 *       vsf_fsm_end() can be debugged (break points and single-step).
 * \~chinese
 * @brief 开始状态机的状态分支，与 vsf_fsm_end() 配合使用，代替
 *        vsf_fsm_body()。
 *
 * @note 与 vsf_fsm_body() 相比，vsf_fsm_begin() 和 vsf_fsm_end() 之间的
 *       代码可以调试（设置断点和单步执行）。
 */
#define vsf_fsm_begin()                                                         \
            switch (vsf_this.fsm_state) {                                       \
                case 0:                                                         \
                    vsf_this.fsm_state++;

/**
 * \~english
 * @brief End the state-switch started by vsf_fsm_begin().
 *
 * @note An unknown state returns @ref fsm_rt_err.
 * \~chinese
 * @brief 结束由 vsf_fsm_begin() 开始的状态分支。
 *
 * @note 未知状态返回 @ref fsm_rt_err。
 */
#define vsf_fsm_end()                                                              \
                break;                                                          \
                default:                                                        \
                return fsm_rt_err;                                              \
            }                                                                   \
        }vsf_task_end()


/*! \note Debug Support: You can use body_begin() together with body_end()
 *!       to enable debug, this means you are ale to set break
 *!       points and single-step into those states.
 *!
 *!       Example:

        vsf_fsm_begin()
            vsf_state(xxxxx,
                //the range no debug is allowed
            )

            vsf_state(xxxxx){
                //the range you can debug with
            }

        vsf_fsm_end()
*/


/**
 * \~english
 * @brief Start the implementation of the fsm entry function, with a
 *        dedicated fsm type.
 * @param[in] __name: name of the fsm entry function
 * @param[in] __type: name of the fsm type
 *
 * @note Based on implement_vsf_task(); the entry returns @ref fsm_rt_err if
 *       the fsm control block pointer is NULL. The body is formed by
 *       vsf_fsm_body() or vsf_fsm_begin()/vsf_fsm_end().
 * \~chinese
 * @brief 以指定的状态机类型，开始状态机入口函数的实现。
 * @param[in] __name: 状态机入口函数名
 * @param[in] __type: 状态机类型名
 *
 * @note 基于 implement_vsf_task()；如果状态机控制块指针为 NULL，入口返回
 *       @ref fsm_rt_err。函数体由 vsf_fsm_body() 或
 *       vsf_fsm_begin()/vsf_fsm_end() 构成。
 */
#define implement_fsm_ex(__name, __type)                                        \
            __implement_fsm_ex(__name, __type)

#define __implement_fsm(__name)                                                 \
            implement_fsm_ex(__name, __name)

/**
 * \~english
 * @brief Start the implementation of the fsm entry function.
 * @param[in] __name: name of the fsm
 * @param[in] ...: unused, kept for symmetry
 *
 * @note Based on implement_vsf_task(); the entry returns @ref fsm_rt_err if
 *       the fsm control block pointer is NULL. The body is formed by
 *       vsf_fsm_body() or vsf_fsm_begin()/vsf_fsm_end().
 * \~chinese
 * @brief 开始状态机入口函数的实现。
 * @param[in] __name: 状态机名
 * @param[in] ...: 未使用，仅为保持对称
 *
 * @note 基于 implement_vsf_task()；如果状态机控制块指针为 NULL，入口返回
 *       @ref fsm_rt_err。函数体由 vsf_fsm_body() 或
 *       vsf_fsm_begin()/vsf_fsm_end() 构成。
 */
#define implement_vsf_fsm(__name, ...)      __implement_fsm(__name)

#define __privilege_state(__state, ...)                                         \
            break;do {                                                          \
                do {                                                            \
                    ____state(__state, __VA_ARGS__)                             \
                } while(0); /* add extra while(0) to catch the fsm_continue()*/ \
                if (vsf_this.fsm_state != (__state)) {                          \
                    break;                                                      \
                }                                                               \
            } while(1);

/**
 * \~english
 * @brief Define a privilege state of the fsm: the state code is executed
 *        repeatedly within the current call until the state is changed
 *        (e.g. by update_state_to() or transfer_to()).
 * @param[in] __state: name of the state, defined by def_states()
 * @param[in] ...: the code executed in this state
 * \~chinese
 * @brief 定义状态机的一个特权状态：在当前调用内重复执行该状态代码，直到
 *        状态被改变（例如通过 update_state_to() 或 transfer_to()）。
 * @param[in] __state: 状态名，由 def_states() 定义
 * @param[in] ...: 在该状态下执行的代码
 */
#define privilege_state(__state, ...)                                           \
            __privilege_state(__state, __VA_ARGS__)


/**
 * \~english
 * @brief Define a group of privilege states.
 * @param[in] ...: the privilege states defined by privilege_state()
 * \~chinese
 * @brief 定义一组特权状态。
 * @param[in] ...: 由 privilege_state() 定义的特权状态
 */
#define privilege_group(...)  { __VA_ARGS__;}

/**
 * \~english
 * @brief The privilege state-switch body of the fsm: the states are
 *        executed in a loop within the current call, until a state returns
 *        (e.g. by transfer_to() or vsf_fsm_cpl()).
 * @param[in] ...: the states defined by vsf_state() or privilege_state()
 *
 * @note If the loop ever breaks, @ref fsm_rt_on_going is returned.
 * \~chinese
 * @brief 状态机的特权状态分支体：在当前调用内循环执行各状态，直到某个
 *        状态返回（例如通过 transfer_to() 或 vsf_fsm_cpl()）。
 * @param[in] ...: 由 vsf_state() 或 privilege_state() 定义的状态
 *
 * @note 如果循环退出，返回 @ref fsm_rt_on_going。
 */
#define privilege_body(...)                                                     \
        do {                                                                    \
            switch (vsf_this.fsm_state) {                                       \
                case 0:                                                         \
                    vsf_this.fsm_state++;                                       \
                __VA_ARGS__                                                     \
            }                                                                   \
        while(1);                                                               \
                                                                                \
        return fsm_rt_on_going;                                                 \
    }


/*============================ TYPES =========================================*/

#ifndef __FSM_RT_TYPE__
#define __FSM_RT_TYPE__
//! \name finit vsf_state machine vsf_state
//! @{
typedef enum {
    fsm_rt_err          = -1,    //!< fsm error, error code can be get from other interface
    fsm_rt_cpl          = 0,     //!< fsm complete
    fsm_rt_on_going     = 1,     //!< fsm on-going
    fsm_rt_wait_for_obj = 2,     //!< fsm wait for object
    fsm_rt_asyn         = 3,     //!< fsm asynchronose complete, you can check it later.
} fsm_rt_t;
//! @}

#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
#endif
