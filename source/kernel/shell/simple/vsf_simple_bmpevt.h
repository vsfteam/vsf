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

#ifndef __VSF_KERNEL_SHELL_SIMPLE_BMEVT_H__
#define __VSF_KERNEL_SHELL_SIMPLE_BMEVT_H__

/*============================ INCLUDES ======================================*/
#include "kernel/vsf_kernel_cfg.h"

#if     VSF_KERNEL_USE_SIMPLE_SHELL == ENABLED                                  \
    &&  VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT == ENABLED                          \
    &&  VSF_USE_KERNEL == ENABLED                                               \
    &&  VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED

#include "../../vsf_eda.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

/**
 * \~english
 * @brief Declare the grouped-event pender member in the task-local control
 *        block; required by the wait_for_* statement macros.
 *
 * @note Expands to a @ref vsf_bmpevt_pender_t member named @c pender, which
 *       the wait_for_one()/wait_for_any()/wait_for_all() macros use through
 *       @c vsf_this. Typically placed inside a features_used()/mem_sharable()
 *       block of the task definition.
 * \~chinese
 * @brief 在任务局部控制块中声明成组事件的等待成员；wait_for_* 语句宏
 *        需要它。
 *
 * @note 展开为一个名为 @c pender 的 @ref vsf_bmpevt_pender_t 成员，
 *       wait_for_one()/wait_for_any()/wait_for_all() 宏通过 @c vsf_this
 *       使用它。通常放在任务定义的 features_used()/mem_sharable() 块中。
 */
#   define using_grouped_evt            vsf_bmpevt_pender_t pender

#   define __declare_grouped_evts(__name)                                       \
            typedef vsf_bmpevt_t __name;                                        \
            typedef enum enum_of_##__name   enum_of_##__name;

/**
 * \~english
 * @brief Declare a bitmap-event group type.
 * @param[in] __name: name of the group; a typedef of @ref vsf_bmpevt_t and a
 *        forward declaration of the enumeration enum_of_<__name> are generated
 *
 * @note Pair with def_grouped_evts()/end_def_grouped_evts(), which define the
 *       event bits of the group.
 * \~chinese
 * @brief 声明一个位图事件组类型。
 * @param[in] __name: 组名；会生成一个 @ref vsf_bmpevt_t 的类型定义以及
 *        枚举 enum_of_<__name> 的前置声明
 *
 * @note 与 def_grouped_evts()/end_def_grouped_evts() 配合使用，后者定义组内
 *       的事件位。
 */
#   define declare_grouped_evts(__name)       __declare_grouped_evts(__name)

#   define __def_grouped_evts(__name)                                           \
        enum enum_of_##__name{                                                  \
            __##__name##_start = __COUNTER__ + 1,


#   define __end_def_grouped_evts(__name)                                       \
            all_evts_msk_of_##__name =                                          \
            ((uint64_t)1<<(__COUNTER__ - __##__name##_start)) - 1,              \
        };
/**
 * \~english
 * @brief End the definition of a bitmap-event group started with
 *        def_grouped_evts().
 * @param[in] __name: name of the group; also generates the enumerator
 *        all_evts_msk_of_<__name> covering all defined event bits
 * \~chinese
 * @brief 结束由 def_grouped_evts() 开始的位图事件组定义。
 * @param[in] __name: 组名；同时生成覆盖所有已定义事件位的枚举值
 *        all_evts_msk_of_<__name>
 */
#   define end_def_grouped_evts(__name)    __end_def_grouped_evts(__name)

#   define __def_evt(__name, __evt)                                             \
            __evt##_idx = __COUNTER__ - __##__name##_start,                     \
            __evt##_msk = (uint32_t)(1<<(__evt##_idx))
/**
 * \~english
 * @brief Define an event bit of a bitmap-event group.
 * @param[in] __name: name of the group
 * @param[in] __evt: name of the event bit; generates the enumerators
 *        <__evt>_idx (bit index) and <__evt>_msk (bit mask)
 *
 * @note Must be used between def_grouped_evts() and end_def_grouped_evts(),
 *       with entries separated by commas.
 * \~chinese
 * @brief 定义位图事件组中的一个事件位。
 * @param[in] __name: 组名
 * @param[in] __evt: 事件位名称；会生成枚举值 <__evt>_idx（位索引）和
 *        <__evt>_msk（位掩码）
 *
 * @note 必须在 def_grouped_evts() 和 end_def_grouped_evts() 之间使用，
 *       各条目之间用逗号分隔。
 */
#   define def_evt(__name, __evt)      __def_evt(__name, __evt)

/**
 * \~english
 * @brief Define an event bit of a bitmap-event group that is driven by an
 *        adapter.
 * @param[in] __name: name of the group
 * @param[in] __evt: name of the event bit; generates the enumerators
 *        <__evt>_idx (bit index) and <__evt>_msk (bit mask)
 *
 * @note Same expansion as def_evt(); the distinct name documents that the bit
 *       is mapped onto an IPC object by an adapter (see add_sync_adapter()).
 * \~chinese
 * @brief 定义位图事件组中由适配器驱动的事件位。
 * @param[in] __name: 组名
 * @param[in] __evt: 事件位名称；会生成枚举值 <__evt>_idx（位索引）和
 *        <__evt>_msk（位掩码）
 *
 * @note 展开形式与 def_evt() 相同；使用不同的名字是为了表明该事件位通过
 *       适配器映射到某个 IPC 对象（参见 add_sync_adapter()）。
 */
#   define def_adapter(__name, __evt)     __def_evt(__name, __evt)

/**
 * \~english
 * @brief Begin the definition of a bitmap-event group.
 * @param[in] __name: name of the group previously declared with
 *        declare_grouped_evts()
 *
 * @note Expands to the opening of the enumeration enum_of_<__name>; list the
 *       event bits with def_evt()/def_adapter() entries and close the
 *       definition with end_def_grouped_evts().
 * \~chinese
 * @brief 开始定义一个位图事件组。
 * @param[in] __name: 先前用 declare_grouped_evts() 声明的组名
 *
 * @note 展开为枚举 enum_of_<__name> 的开头；用 def_evt()/def_adapter()
 *       列出事件位，并用 end_def_grouped_evts() 结束定义。
 */
#   define def_grouped_evts(__name)                                             \
            __def_grouped_evts(__name)


#   define __implement_grouped_evts(__name, ...)                                \
        static const vsf_bmpevt_adapter_t *__adapters_of_##__name[] = {         \
            __VA_ARGS__                                                         \
        };

/**
 * \~english
 * @brief Define the adapter list of a bitmap-event group.
 * @param[in] __name: name of the group
 * @param[in] ...: adapter entries, typically created with add_sync_adapter()
 *
 * @note Adapters map IPC objects (e.g. semaphores) onto event bits of the
 *       group, so that a task can wait for events from different IPC objects
 *       at once. The list is referenced by init_grouped_evts().
 * \~chinese
 * @brief 定义位图事件组的适配器列表。
 * @param[in] __name: 组名
 * @param[in] ...: 适配器条目，通常用 add_sync_adapter() 创建
 *
 * @note 适配器把 IPC 对象（例如信号量）映射到组内的事件位，使任务可以
 *       同时等待来自不同 IPC 对象的事件。该列表由 init_grouped_evts()
 *       引用。
 */
#   define implement_grouped_evts(__name, ...)                                  \
            __implement_grouped_evts(__name, __VA_ARGS__)

/**
 * \~english
 * @brief Create an adapter entry mapping a sync object (e.g. a semaphore)
 *        onto event bits of a bitmap-event group.
 * @param[in] __SYNC: pointer to the sync object @ref vsf_sync_t (e.g.
 *        @ref vsf_sem_t)
 * @param[in] __MSK: mask of the event bits driven by this sync object
 * @param[in] ...: optional additional initializers for
 *        @ref vsf_bmpevt_adapter_sync_t
 * @return pointer to the adapter @ref vsf_bmpevt_adapter_t
 *
 * @note Used inside the adapter list of implement_grouped_evts().
 * \~chinese
 * @brief 创建一个适配器条目，把同步对象（例如信号量）映射到位图事件组的
 *        事件位上。
 * @param[in] __SYNC: 指向同步对象 @ref vsf_sync_t（例如 @ref vsf_sem_t）
 *        的指针
 * @param[in] __MSK: 由该同步对象驱动的事件位掩码
 * @param[in] ...: @ref vsf_bmpevt_adapter_sync_t 的可选附加初始化项
 * @return 指向适配器 @ref vsf_bmpevt_adapter_t 的指针
 *
 * @note 在 implement_grouped_evts() 的适配器列表中使用。
 */
#   define add_sync_adapter(__SYNC, __MSK, ...)                                 \
    &((vsf_bmpevt_adapter_sync_t){                                              \
            .sync = (__SYNC),                                                   \
            .use_as__vsf_bmpevt_adapter_eda_t.use_as__vsf_bmpevt_adapter_t = {  \
                .mask  = (__MSK),                                               \
                .op = &vsf_eda_bmpevt_adapter_sync_op,                          \
            },                                                                  \
            __VA_ARGS__                                                         \
        }.use_as__vsf_bmpevt_adapter_eda_t.use_as__vsf_bmpevt_adapter_t)



#   define __init_grouped_evts(__name, __evt_group, __auto_reset)               \
        __vsf_grouped_evts_init((__evt_group),                                  \
            (vsf_bmpevt_adapter_t **)&(__adapters_of_##__name),                 \
            dimof(__adapters_of_##__name),                                      \
            (__auto_reset))

/**
 * \~english
 * @brief Initialize a bitmap-event group instance with its adapter list.
 * @param[in] __name: name of the group (selects the adapter list defined by
 *        implement_grouped_evts())
 * @param[in] __evt_group: pointer to the group instance @ref vsf_bmpevt_t
 * @param[in] __auto_reset: mask of the event bits that are reset automatically
 *        after a successful wait
 * \~chinese
 * @brief 用适配器列表初始化一个位图事件组实例。
 * @param[in] __name: 组名（用于选择 implement_grouped_evts() 定义的适配器
 *        列表）
 * @param[in] __evt_group: 指向组实例 @ref vsf_bmpevt_t 的指针
 * @param[in] __auto_reset: 等待成功后自动复位的事件位掩码
 */
#   define init_grouped_evts(__name, __evt_group, __auto_reset)                 \
            __init_grouped_evts(__name, __evt_group, (__auto_reset))

#   define __grouped_evts_info(__name)    __grouped_evts_##__name##_info
/**
 * \~english
 * @brief Get the info symbol of a bitmap-event group.
 * @param[in] __name: name of the group
 * @return expands to the symbol __grouped_evts_<__name>_info
 * \~chinese
 * @brief 获取位图事件组的信息符号。
 * @param[in] __name: 组名
 * @return 展开为符号 __grouped_evts_<__name>_info
 */
#   define grouped_evts_info(__name)      __grouped_evts_info(__name)
#   define __grouped_evts_adapter(__name, __INDEX)                              \
        __adapters_of_##__name[(__INDEX)]
/**
 * \~english
 * @brief Get an adapter of a bitmap-event group by index.
 * @param[in] __name: name of the group
 * @param[in] __INDEX: index of the adapter in the list defined by
 *        implement_grouped_evts()
 * @return pointer to the adapter @ref vsf_bmpevt_adapter_t
 * \~chinese
 * @brief 按索引获取位图事件组的一个适配器。
 * @param[in] __name: 组名
 * @param[in] __INDEX: 适配器在 implement_grouped_evts() 定义的列表中的索引
 * @return 指向适配器 @ref vsf_bmpevt_adapter_t 的指针
 */
#   define grouped_evts_adapter(__name, __INDEX)                                \
        __grouped_evts_adapter(__name, (__INDEX))


/**
 * \~english
 * @brief Wait until all event bits of a mask in a bitmap-event group are set
 *        (AND), with a timeout in system timer ticks (cooperative,
 *        non-blocking for other tasks).
 * @param[in] __group: pointer to the group instance @ref vsf_bmpevt_t
 * @param[in] __msk: mask of the event bits to wait for
 * @param[in] __timeout: timeout in system timer ticks; a negative value waits
 *        forever
 *
 * @note Statement macro: the following brace block runs when all bits of the
 *       mask are set (VSF_SYNC_GET) or the wait times out (VSF_SYNC_TIMEOUT);
 *       use on_timeout() to catch the timeout case. Usage:
 *       wait_for_all_timeout(group, msk, timeout) { ... }. Must be used inside
 *       a simple-shell event handler whose task declares using_grouped_evt.
 * \~chinese
 * @brief 等待位图事件组中掩码指定的所有事件位都置位（与），超时时间以系统
 *        定时器 tick 为单位（协作式，不阻塞其他任务）。
 * @param[in] __group: 指向组实例 @ref vsf_bmpevt_t 的指针
 * @param[in] __msk: 要等待的事件位掩码
 * @param[in] __timeout: 以系统定时器 tick 为单位的超时时间；负值表示一直等待
 *
 * @note 语句宏：当掩码的所有事件位置位（VSF_SYNC_GET）或等待超时
 *       （VSF_SYNC_TIMEOUT）时，紧随其后的花括号代码块执行；使用
 *       on_timeout() 捕获超时情况。
 *       用法：wait_for_all_timeout(group, msk, timeout) { ... }。
 *       必须在声明了 using_grouped_evt 的任务的 simple-shell 事件处理
 *       函数中使用。
 */
#   define wait_for_all_timeout(__group, __msk, __timeout)                      \
            vsf_this.pender.mask = (__msk);                                         \
            vsf_this.pender.op  = VSF_BMPEVT_AND;                                   \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason = __vsf_bmpevt_wait_for(                            \
                    (__group),                                                  \
                    (vsf_bmpevt_pender_t *)                                     \
                    &vsf_this.pender, (__timeout)),                                 \
                    (reason == VSF_SYNC_GET || reason == VSF_SYNC_TIMEOUT)))

/**
 * \~english
 * @brief Wait until all event bits of a mask in a bitmap-event group are set
 *        (AND), with a timeout in milliseconds (cooperative, non-blocking for
 *        other tasks).
 * @param[in] __group: pointer to the group instance @ref vsf_bmpevt_t
 * @param[in] __msk: mask of the event bits to wait for
 * @param[in] __timeout: timeout in milliseconds, converted to ticks via
 *        vsf_systimer_ms_to_tick()
 *
 * @note Statement macro: the following brace block runs when all bits of the
 *       mask are set (VSF_SYNC_GET) or the wait times out (VSF_SYNC_TIMEOUT);
 *       use on_timeout() to catch the timeout case. Usage:
 *       wait_for_all_timeout_ms(group, msk, timeout) { ... }. Must be used
 *       inside a simple-shell event handler whose task declares
 *       using_grouped_evt.
 * \~chinese
 * @brief 等待位图事件组中掩码指定的所有事件位都置位（与），超时时间以毫秒
 *        为单位（协作式，不阻塞其他任务）。
 * @param[in] __group: 指向组实例 @ref vsf_bmpevt_t 的指针
 * @param[in] __msk: 要等待的事件位掩码
 * @param[in] __timeout: 以毫秒为单位的超时时间，通过
 *        vsf_systimer_ms_to_tick() 转换为 tick
 *
 * @note 语句宏：当掩码的所有事件位置位（VSF_SYNC_GET）或等待超时
 *       （VSF_SYNC_TIMEOUT）时，紧随其后的花括号代码块执行；使用
 *       on_timeout() 捕获超时情况。
 *       用法：wait_for_all_timeout_ms(group, msk, timeout) { ... }。
 *       必须在声明了 using_grouped_evt 的任务的 simple-shell 事件处理
 *       函数中使用。
 */
#   define wait_for_all_timeout_ms(__group, __msk, __timeout)                   \
            wait_for_all_timeout(   __group,                                    \
                                    (__msk),                                    \
                                    vsf_systimer_ms_to_tick(__timeout))

/**
 * \~english
 * @brief Wait until all event bits of a mask in a bitmap-event group are set
 *        (AND), with a timeout in microseconds (cooperative, non-blocking for
 *        other tasks).
 * @param[in] __group: pointer to the group instance @ref vsf_bmpevt_t
 * @param[in] __msk: mask of the event bits to wait for
 * @param[in] __timeout: timeout in microseconds, converted to ticks via
 *        vsf_systimer_us_to_tick()
 *
 * @note Statement macro: the following brace block runs when all bits of the
 *       mask are set (VSF_SYNC_GET) or the wait times out (VSF_SYNC_TIMEOUT);
 *       use on_timeout() to catch the timeout case. Usage:
 *       wait_for_all_timeout_us(group, msk, timeout) { ... }. Must be used
 *       inside a simple-shell event handler whose task declares
 *       using_grouped_evt.
 * \~chinese
 * @brief 等待位图事件组中掩码指定的所有事件位都置位（与），超时时间以微秒
 *        为单位（协作式，不阻塞其他任务）。
 * @param[in] __group: 指向组实例 @ref vsf_bmpevt_t 的指针
 * @param[in] __msk: 要等待的事件位掩码
 * @param[in] __timeout: 以微秒为单位的超时时间，通过
 *        vsf_systimer_us_to_tick() 转换为 tick
 *
 * @note 语句宏：当掩码的所有事件位置位（VSF_SYNC_GET）或等待超时
 *       （VSF_SYNC_TIMEOUT）时，紧随其后的花括号代码块执行；使用
 *       on_timeout() 捕获超时情况。
 *       用法：wait_for_all_timeout_us(group, msk, timeout) { ... }。
 *       必须在声明了 using_grouped_evt 的任务的 simple-shell 事件处理
 *       函数中使用。
 */
#   define wait_for_all_timeout_us(__group, __msk, __timeout)                   \
            wait_for_all_timeout(   __group,                                    \
                                    (__msk),                                    \
                                    vsf_systimer_us_to_tick(__timeout))

/**
 * \~english
 * @brief Wait until all event bits of a mask in a bitmap-event group are set
 *        (AND), waiting forever (cooperative, non-blocking for other tasks).
 * @param[in] __group: pointer to the group instance @ref vsf_bmpevt_t
 * @param[in] __msk: mask of the event bits to wait for
 *
 * @note Statement macro: the following brace block runs after all bits of the
 *       mask are set. Usage: wait_for_all(group, msk) { ... }. Must be used
 *       inside a simple-shell event handler whose task declares
 *       using_grouped_evt.
 * \~chinese
 * @brief 等待位图事件组中掩码指定的所有事件位都置位（与），一直等待
 *       （协作式，不阻塞其他任务）。
 * @param[in] __group: 指向组实例 @ref vsf_bmpevt_t 的指针
 * @param[in] __msk: 要等待的事件位掩码
 *
 * @note 语句宏：紧随其后的花括号代码块在掩码的所有事件位置位后执行。
 *       用法：wait_for_all(group, msk) { ... }。必须在声明了
 *       using_grouped_evt 的任务的 simple-shell 事件处理函数中使用。
 */
#   define wait_for_all(__group, __msk)                                         \
            wait_for_all_timeout( __group, (__msk), -1)

/**
 * \~english
 * @brief Wait until any event bit of a mask in a bitmap-event group is set
 *        (OR), with a timeout in system timer ticks (cooperative,
 *        non-blocking for other tasks).
 * @param[in] __group: pointer to the group instance @ref vsf_bmpevt_t
 * @param[in] __msk: mask of the event bits to wait for
 * @param[in] __timeout: timeout in system timer ticks; a negative value waits
 *        forever
 *
 * @note Statement macro: the following brace block runs when any bit of the
 *       mask is set (VSF_SYNC_GET) or the wait times out (VSF_SYNC_TIMEOUT);
 *       use on_timeout() to catch the timeout case. Usage:
 *       wait_for_any_timeout(group, msk, timeout) { ... }. Must be used inside
 *       a simple-shell event handler whose task declares using_grouped_evt.
 * \~chinese
 * @brief 等待位图事件组中掩码指定的任一事件位置位（或），超时时间以系统
 *        定时器 tick 为单位（协作式，不阻塞其他任务）。
 * @param[in] __group: 指向组实例 @ref vsf_bmpevt_t 的指针
 * @param[in] __msk: 要等待的事件位掩码
 * @param[in] __timeout: 以系统定时器 tick 为单位的超时时间；负值表示一直等待
 *
 * @note 语句宏：当掩码的任一事件位置位（VSF_SYNC_GET）或等待超时
 *       （VSF_SYNC_TIMEOUT）时，紧随其后的花括号代码块执行；使用
 *       on_timeout() 捕获超时情况。
 *       用法：wait_for_any_timeout(group, msk, timeout) { ... }。
 *       必须在声明了 using_grouped_evt 的任务的 simple-shell 事件处理
 *       函数中使用。
 */
#   define wait_for_any_timeout(__group, __msk, __timeout)                      \
            vsf_this.pender.mask = (__msk);                                         \
            vsf_this.pender.op = VSF_BMPEVT_OR;                                     \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason = __vsf_bmpevt_wait_for(                            \
                    (__group),                                                  \
                    (vsf_bmpevt_pender_t *)                                     \
                    &vsf_this.pender, (__timeout)),                                 \
                    (reason == VSF_SYNC_GET || reason == VSF_SYNC_TIMEOUT)))

/**
 * \~english
 * @brief Wait until any event bit of a mask in a bitmap-event group is set
 *        (OR), with a timeout in milliseconds (cooperative, non-blocking for
 *        other tasks).
 * @param[in] __group: pointer to the group instance @ref vsf_bmpevt_t
 * @param[in] __msk: mask of the event bits to wait for
 * @param[in] __timeout: timeout in milliseconds, converted to ticks via
 *        vsf_systimer_ms_to_tick()
 *
 * @note Statement macro: the following brace block runs when any bit of the
 *       mask is set (VSF_SYNC_GET) or the wait times out (VSF_SYNC_TIMEOUT);
 *       use on_timeout() to catch the timeout case. Usage:
 *       wait_for_any_timeout_ms(group, msk, timeout) { ... }. Must be used
 *       inside a simple-shell event handler whose task declares
 *       using_grouped_evt.
 * \~chinese
 * @brief 等待位图事件组中掩码指定的任一事件位置位（或），超时时间以毫秒
 *        为单位（协作式，不阻塞其他任务）。
 * @param[in] __group: 指向组实例 @ref vsf_bmpevt_t 的指针
 * @param[in] __msk: 要等待的事件位掩码
 * @param[in] __timeout: 以毫秒为单位的超时时间，通过
 *        vsf_systimer_ms_to_tick() 转换为 tick
 *
 * @note 语句宏：当掩码的任一事件位置位（VSF_SYNC_GET）或等待超时
 *       （VSF_SYNC_TIMEOUT）时，紧随其后的花括号代码块执行；使用
 *       on_timeout() 捕获超时情况。
 *       用法：wait_for_any_timeout_ms(group, msk, timeout) { ... }。
 *       必须在声明了 using_grouped_evt 的任务的 simple-shell 事件处理
 *       函数中使用。
 */
#   define wait_for_any_timeout_ms(__group, __msk, __timeout)                   \
            wait_for_any_timeout(   __group,                                    \
                                    (__msk),                                    \
                                    vsf_systimer_ms_to_tick(__timeout))

/**
 * \~english
 * @brief Wait until any event bit of a mask in a bitmap-event group is set
 *        (OR), with a timeout in microseconds (cooperative, non-blocking for
 *        other tasks).
 * @param[in] __group: pointer to the group instance @ref vsf_bmpevt_t
 * @param[in] __msk: mask of the event bits to wait for
 * @param[in] __timeout: timeout in microseconds, converted to ticks via
 *        vsf_systimer_us_to_tick()
 *
 * @note Statement macro: the following brace block runs when any bit of the
 *       mask is set (VSF_SYNC_GET) or the wait times out (VSF_SYNC_TIMEOUT);
 *       use on_timeout() to catch the timeout case. Usage:
 *       wait_for_any_timeout_us(group, msk, timeout) { ... }. Must be used
 *       inside a simple-shell event handler whose task declares
 *       using_grouped_evt.
 * \~chinese
 * @brief 等待位图事件组中掩码指定的任一事件位置位（或），超时时间以微秒
 *        为单位（协作式，不阻塞其他任务）。
 * @param[in] __group: 指向组实例 @ref vsf_bmpevt_t 的指针
 * @param[in] __msk: 要等待的事件位掩码
 * @param[in] __timeout: 以微秒为单位的超时时间，通过
 *        vsf_systimer_us_to_tick() 转换为 tick
 *
 * @note 语句宏：当掩码的任一事件位置位（VSF_SYNC_GET）或等待超时
 *       （VSF_SYNC_TIMEOUT）时，紧随其后的花括号代码块执行；使用
 *       on_timeout() 捕获超时情况。
 *       用法：wait_for_any_timeout_us(group, msk, timeout) { ... }。
 *       必须在声明了 using_grouped_evt 的任务的 simple-shell 事件处理
 *       函数中使用。
 */
#   define wait_for_any_timeout_us(__group, __msk, __timeout)                   \
            wait_for_any_timeout(   __group,                                    \
                                    (__msk),                                    \
                                    vsf_systimer_us_to_tick(__timeout))

/**
 * \~english
 * @brief Wait until any event bit of a mask in a bitmap-event group is set
 *        (OR), waiting forever (cooperative, non-blocking for other tasks).
 * @param[in] __group: pointer to the group instance @ref vsf_bmpevt_t
 * @param[in] __msk: mask of the event bits to wait for
 *
 * @note Statement macro: the following brace block runs after any bit of the
 *       mask is set. Usage: wait_for_any(group, msk) { ... }. Must be used
 *       inside a simple-shell event handler whose task declares
 *       using_grouped_evt.
 * \~chinese
 * @brief 等待位图事件组中掩码指定的任一事件位置位（或），一直等待
 *       （协作式，不阻塞其他任务）。
 * @param[in] __group: 指向组实例 @ref vsf_bmpevt_t 的指针
 * @param[in] __msk: 要等待的事件位掩码
 *
 * @note 语句宏：紧随其后的花括号代码块在掩码的任一事件位置位后执行。
 *       用法：wait_for_any(group, msk) { ... }。必须在声明了
 *       using_grouped_evt 的任务的 simple-shell 事件处理函数中使用。
 */
#   define wait_for_any(__group, __msk)                                         \
            wait_for_any_timeout( __group, (__msk), -1)

/**
 * \~english
 * @brief Wait until any event bit of a mask in a bitmap-event group is set
 *        (alias of wait_for_any(), OR), waiting forever (cooperative,
 *        non-blocking for other tasks).
 * @param[in] __group: pointer to the group instance @ref vsf_bmpevt_t
 * @param[in] __msk: mask of the event bits to wait for
 *
 * @note Statement macro: the following brace block runs after any bit of the
 *       mask is set. Usage: wait_for_one(group, msk) { ... }. Must be used
 *       inside a simple-shell event handler whose task declares
 *       using_grouped_evt.
 * \~chinese
 * @brief 等待位图事件组中掩码指定的任一事件位置位（wait_for_any() 的别名，
 *        或），一直等待（协作式，不阻塞其他任务）。
 * @param[in] __group: 指向组实例 @ref vsf_bmpevt_t 的指针
 * @param[in] __msk: 要等待的事件位掩码
 *
 * @note 语句宏：紧随其后的花括号代码块在掩码的任一事件位置位后执行。
 *       用法：wait_for_one(group, msk) { ... }。必须在声明了
 *       using_grouped_evt 的任务的 simple-shell 事件处理函数中使用。
 */
#   define wait_for_one(__group, __msk)                                         \
            wait_for_any((__group), (__msk))

/**
 * \~english
 * @brief Wait until any event bit of a mask in a bitmap-event group is set
 *        (alias of wait_for_any_timeout(), OR), with a timeout in system timer
 *        ticks (cooperative, non-blocking for other tasks).
 * @param[in] __group: pointer to the group instance @ref vsf_bmpevt_t
 * @param[in] __msk: mask of the event bits to wait for
 * @param[in] __timeout: timeout in system timer ticks; a negative value waits
 *        forever
 *
 * @note Statement macro: the following brace block runs when any bit of the
 *       mask is set (VSF_SYNC_GET) or the wait times out (VSF_SYNC_TIMEOUT);
 *       use on_timeout() to catch the timeout case. Usage:
 *       wait_for_one_timeout(group, msk, timeout) { ... }. Must be used inside
 *       a simple-shell event handler whose task declares using_grouped_evt.
 * \~chinese
 * @brief 等待位图事件组中掩码指定的任一事件位置位（wait_for_any_timeout()
 *        的别名，或），超时时间以系统定时器 tick 为单位（协作式，不阻塞
 *        其他任务）。
 * @param[in] __group: 指向组实例 @ref vsf_bmpevt_t 的指针
 * @param[in] __msk: 要等待的事件位掩码
 * @param[in] __timeout: 以系统定时器 tick 为单位的超时时间；负值表示一直等待
 *
 * @note 语句宏：当掩码的任一事件位置位（VSF_SYNC_GET）或等待超时
 *       （VSF_SYNC_TIMEOUT）时，紧随其后的花括号代码块执行；使用
 *       on_timeout() 捕获超时情况。
 *       用法：wait_for_one_timeout(group, msk, timeout) { ... }。
 *       必须在声明了 using_grouped_evt 的任务的 simple-shell 事件处理
 *       函数中使用。
 */
#   define wait_for_one_timeout(__group, __msk, __timeout)                      \
            wait_for_any_timeout((__group), (__msk), (__timeout))

/**
 * \~english
 * @brief Wait until any event bit of a mask in a bitmap-event group is set
 *        (alias of wait_for_any_timeout_ms(), OR), with a timeout in
 *        milliseconds (cooperative, non-blocking for other tasks).
 * @param[in] __group: pointer to the group instance @ref vsf_bmpevt_t
 * @param[in] __msk: mask of the event bits to wait for
 * @param[in] __timeout: timeout in milliseconds, converted to ticks via
 *        vsf_systimer_ms_to_tick()
 *
 * @note Statement macro: the following brace block runs when any bit of the
 *       mask is set (VSF_SYNC_GET) or the wait times out (VSF_SYNC_TIMEOUT);
 *       use on_timeout() to catch the timeout case. Usage:
 *       wait_for_one_timeout_ms(group, msk, timeout) { ... }. Must be used
 *       inside a simple-shell event handler whose task declares
 *       using_grouped_evt.
 * \~chinese
 * @brief 等待位图事件组中掩码指定的任一事件位置位
 *        （wait_for_any_timeout_ms() 的别名，或），超时时间以毫秒为单位
 *       （协作式，不阻塞其他任务）。
 * @param[in] __group: 指向组实例 @ref vsf_bmpevt_t 的指针
 * @param[in] __msk: 要等待的事件位掩码
 * @param[in] __timeout: 以毫秒为单位的超时时间，通过
 *        vsf_systimer_ms_to_tick() 转换为 tick
 *
 * @note 语句宏：当掩码的任一事件位置位（VSF_SYNC_GET）或等待超时
 *       （VSF_SYNC_TIMEOUT）时，紧随其后的花括号代码块执行；使用
 *       on_timeout() 捕获超时情况。
 *       用法：wait_for_one_timeout_ms(group, msk, timeout) { ... }。
 *       必须在声明了 using_grouped_evt 的任务的 simple-shell 事件处理
 *       函数中使用。
 */
#   define wait_for_one_timeout_ms(__group, __msk, __timeout)                   \
            wait_for_any_timeout_ms((__group), (__msk), (__timeout))

/**
 * \~english
 * @brief Wait until any event bit of a mask in a bitmap-event group is set
 *        (alias of wait_for_any_timeout_us(), OR), with a timeout in
 *        microseconds (cooperative, non-blocking for other tasks).
 * @param[in] __group: pointer to the group instance @ref vsf_bmpevt_t
 * @param[in] __msk: mask of the event bits to wait for
 * @param[in] __timeout: timeout in microseconds, converted to ticks via
 *        vsf_systimer_us_to_tick()
 *
 * @note Statement macro: the following brace block runs when any bit of the
 *       mask is set (VSF_SYNC_GET) or the wait times out (VSF_SYNC_TIMEOUT);
 *       use on_timeout() to catch the timeout case. Usage:
 *       wait_for_one_timeout_us(group, msk, timeout) { ... }. Must be used
 *       inside a simple-shell event handler whose task declares
 *       using_grouped_evt.
 * \~chinese
 * @brief 等待位图事件组中掩码指定的任一事件位置位
 *        （wait_for_any_timeout_us() 的别名，或），超时时间以微秒为单位
 *       （协作式，不阻塞其他任务）。
 * @param[in] __group: 指向组实例 @ref vsf_bmpevt_t 的指针
 * @param[in] __msk: 要等待的事件位掩码
 * @param[in] __timeout: 以微秒为单位的超时时间，通过
 *        vsf_systimer_us_to_tick() 转换为 tick
 *
 * @note 语句宏：当掩码的任一事件位置位（VSF_SYNC_GET）或等待超时
 *       （VSF_SYNC_TIMEOUT）时，紧随其后的花括号代码块执行；使用
 *       on_timeout() 捕获超时情况。
 *       用法：wait_for_one_timeout_us(group, msk, timeout) { ... }。
 *       必须在声明了 using_grouped_evt 的任务的 simple-shell 事件处理
 *       函数中使用。
 */
#   define wait_for_one_timeout_us(__group, __msk, __timeout)                   \
            wait_for_any_timeout_us((__group), (__msk), (__timeout))

/**
 * \~english
 * @brief Reset event bits of a bitmap-event group.
 * @param[in] __group: pointer to the group instance @ref vsf_bmpevt_t
 * @param[in] __msk: mask of the event bits to reset
 * @return vsf_err_t: always VSF_ERR_NONE
 * \~chinese
 * @brief 复位位图事件组的事件位。
 * @param[in] __group: 指向组实例 @ref vsf_bmpevt_t 的指针
 * @param[in] __msk: 要复位的事件位掩码
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE
 */
#   define reset_grouped_evts(__group, __msk)                                   \
            vsf_eda_bmpevt_reset((__group),(__msk))

/**
 * \~english
 * @brief Set event bits of a bitmap-event group, waking up the tasks waiting
 *        for them if any.
 * @param[in] __group: pointer to the group instance @ref vsf_bmpevt_t
 * @param[in] __msk: mask of the event bits to set
 * @return vsf_err_t: always VSF_ERR_NONE
 * \~chinese
 * @brief 置位位图事件组的事件位，如果有任务在等待则唤醒它们。
 * @param[in] __group: 指向组实例 @ref vsf_bmpevt_t 的指针
 * @param[in] __msk: 要置位的事件位掩码
 * @return vsf_err_t: 始终返回 VSF_ERR_NONE
 */
#   define set_grouped_evts(__group, __msk)                                     \
            vsf_eda_bmpevt_set((__group),(__msk))


/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

VSF_CAL_SECTION(".text.vsf.kernel.__vsf_grouped_evts_init")
/**
 * \~english
 * @brief Low-level initialization helper backing the init_grouped_evts()
 *        macro.
 * @param[in] this_ptr: pointer to the group instance @ref vsf_bmpevt_t
 * @param[in] adapters_pptr: pointer to the adapter list defined by
 *        implement_grouped_evts()
 * @param[in] adapter_count: number of adapters in the list
 * @param[in] auto_reset: mask of the event bits that are reset automatically
 *        after a successful wait
 * @return none
 *
 * @note Internal use only.
 * \~chinese
 * @brief 支撑 init_grouped_evts() 宏的底层初始化辅助函数。
 * @param[in] this_ptr: 指向组实例 @ref vsf_bmpevt_t 的指针
 * @param[in] adapters_pptr: 指向 implement_grouped_evts() 定义的适配器列表
 * @param[in] adapter_count: 列表中适配器的数量
 * @param[in] auto_reset: 等待成功后自动复位的事件位掩码
 * @return 无
 *
 * @note 仅供内部使用。
 */
extern void __vsf_grouped_evts_init(  vsf_bmpevt_t *this_ptr,
                                vsf_bmpevt_adapter_t **adapters_pptr,
                                uint_fast8_t adapter_count,
                                uint_fast32_t auto_reset);

VSF_CAL_SECTION(".text.vsf.kernel.__vsf_bmpevt_wait_for")
/**
 * \~english
 * @brief Low-level bitmap-event wait helper backing the wait_for_one()/
 *        wait_for_any()/wait_for_all() statement macros.
 * @param[in] bmpevt_ptr: pointer to the group instance @ref vsf_bmpevt_t
 * @param[in] pender_ptr: pointer to the pender @ref vsf_bmpevt_pender_t
 *        holding the mask and the operation (VSF_BMPEVT_AND/VSF_BMPEVT_OR)
 * @param[in] time_out: timeout in system timer ticks; a negative value waits
 *        forever
 * @return vsf_sync_reason_t: VSF_SYNC_GET when the wait condition is met,
 *         VSF_SYNC_TIMEOUT on timeout, VSF_SYNC_CANCEL when the wait is
 *         cancelled, VSF_SYNC_FAIL on error, otherwise VSF_SYNC_PENDING while
 *         the wait is still in progress
 *
 * @note Internal use only.
 * \~chinese
 * @brief 支撑 wait_for_one()/wait_for_any()/wait_for_all() 语句宏的底层
 *        位图事件等待辅助函数。
 * @param[in] bmpevt_ptr: 指向组实例 @ref vsf_bmpevt_t 的指针
 * @param[in] pender_ptr: 指向等待者 @ref vsf_bmpevt_pender_t 的指针，保存
 *        事件位掩码和操作（VSF_BMPEVT_AND/VSF_BMPEVT_OR）
 * @param[in] time_out: 以系统定时器 tick 为单位的超时时间；负值表示一直等待
 * @return vsf_sync_reason_t: 等待条件满足时返回 VSF_SYNC_GET，超时时返回
 *         VSF_SYNC_TIMEOUT，被取消时返回 VSF_SYNC_CANCEL，出错时返回
 *         VSF_SYNC_FAIL，等待仍在进行时返回 VSF_SYNC_PENDING
 *
 * @note 仅供内部使用。
 */
extern vsf_sync_reason_t __vsf_bmpevt_wait_for(
                                            vsf_bmpevt_t *bmpevt_ptr,
                                            const vsf_bmpevt_pender_t *pender_ptr,
                                            int_fast32_t time_out);

#ifdef __cplusplus
}
#endif

#endif
#endif
/* EOF */
