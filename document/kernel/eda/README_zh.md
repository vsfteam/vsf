# 《 VSF 事件驱动》

[TOC]

## 1. 简介
&emsp;&emsp;VSF 中的所有任务底层都是事件驱动（ Event-driven Architecture ，以下简称 **eda** 或者 **eda 任务**。具备定时器功能的 **eda 任务**称为 **teda 任务**）。 VSF 的事件驱动框架下，事件驱动任务仅仅是处理任务收到的各个事件。这个从任务的接口形式上，就可以很容易理解：
```c
void user_task(vsf_eda_t *eda, vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_INIT:
        break;
    case VSF_EVT_TIMER:
        break;
    }
}
```
&emsp;&emsp;每个事件的处理都必须是非阻塞的，一般都是使用状态机的方式来开发，可以称为**事件驱动状态机**的开发方式。 **eda 任务**本身都是共享堆栈的（但是可以在 **eda 任务**的事件处理函数里，切换到专用堆栈，实现[**独立堆栈任务**](../thread/README_zh.md)），只有在处理事件的时候，才会占用堆栈。并且可以把 **eda 任务**封装成其他的任务形式，比如层次状态机任务、[**独立堆栈任务**](../thread/README_zh.md)等等。由于 **eda 任务**一般需要使用状态机，所以开发难度较高，资源占用较少（仅需要 20-40 字节的任务控制块）；并且由于代码一般都不是顺序实行，所以逆向的难度相对较大。

&emsp;&emsp;**eda 任务**是有优先级，vsf系统可以支持抢占式和协作式的调度方式。抢占式是指**事件队列**之简是抢占的，**事件队列**运行在 swi 中断里（可调优先级、可屏蔽的软件中断），或者复位中断里（不可设置优先级、不可屏蔽、可以被所有其他中断抢占）。协作式是指同一**事件队列**里的事件的处理是协作式的，按照事件发生的先后顺序调用对应任务的事件处理函数。根据**事件队列**运行在哪个中断里，决定了**事件队列**的优先级，**事件队列**的优先级抢占也是纯硬件的中断抢占。高优先级的 swi 中断里运行的**事件队列**，其优先级大于低中断优先级的swi中断里运行的**事件队列**，并且，复位中断里的**事件队列**优先级最低。每个**eda任务**同一时间内只能隶属于一个**事件队列**，但是，可以通过切换任务优先级来切换到其他**事件队列**。

&emsp;&emsp;常用系统事件：
- VSF_EVT_INIT：初始化事件，任务启动的时候，会收到一次。
- VSF_EVT_FINI：终止化事件，终止启动的时候，会收到一次。
- VSF_EVT_RETURN：子任务返回事件。
- VSF_EVT_TIMER：定时器事件， **teda 任务**里有效。
- VSF_EVT_SYNC：同步事件，表示获得同步IPC。
- VSF_EVT_SYNC_CANCEL：同步IPC被取消事件。
- VSF_EVT_SYNC_POLL：同步IPC轮询事件。
- VSF_EVT_MESSAGE：消息事件，带一个指针参数，通过 void * vsf_eda_get_cur_msg(void); 获得指针。
- VSF_EVT_USER：从 VSF_EVT_USER 开始，之后的都是用户事件。

## 2. 配置
&emsp;&emsp;由于其他形式的任务底层都是 **eda 任务**，所以 eda 的配置也会影响所有其他形式的任务。一些 **IPC （任务间通信）**相关的内容，请首先阅读[ **IPC （任务间融信）**](../ipc/README_zh.md)。
```c
/********************************************************************************
 * 内核相关配置
 *******************************************************************************/

// 内核任务优先级，如果不定义的话，会使用最高任务优先级。
// 内核任务用来处理定时器以及一些中断中调用的带 _isr 后缀 API 的内核跳板（中断里调用的后缀为 _isr 的 API ，实际是发送事件给内核任务，由内核任务调用对应的 API ）
// 默认不定义
 #define VSF_KERNEL_CFG_KERNEL_TASK_PRIORITY            vsf_prio_0

// 定时器支持，使能之后，可以使用 teda 任务。并且，其他的任务形式，也都会从 teda 任务派生（定时器没使能的时候，从 eda 任务派生）。
// 默认使能
#define VSF_KERNEL_CFG_EDA_SUPPORT_TIMER                ENABLED

// 内核定时器模式，可以设置为 VSF_KERNEL_CFG_TIMER_MODE_TICK 或者 VSF_KERNEL_CFG_TIMER_MODE_TICKLESS 模式。
// VSF_KERNEL_CFG_TIMER_MODE_TICK 模式下，定时器基于固定的周期。除了一些特殊情况外（比如资源极其有限）， vsf 一般不会使用 tick 模式。
//      还需定义 VSF_SYSTIMER_CFG_IMPL_MODE 为 VSF_SYSTIMER_IMPL_TICK_MODE ，来设置 systimer 的实现模式为 tick 模式
//      vsf_systimer_tick_t 以及 vsf_timeout_tick_t 的单位都是系统定时器的溢出周期
// VSF_KERNEL_CFG_TIMER_MODE_TICKLESS 模式下，系统不需要被周期性的中断唤醒，并且高层 API 完全兼容，支持极小延迟的定时器。
//      vsf_systimer_tick_t 以及 vsf_timeout_tick_t 的单位都是这个系统定时器的计数周期
// 不同模式下，用户 API 完全兼容，只是行为上会有略微差异。在 TICK 模式下，小于 TICK 周期的延迟，会被拉长到 TICK 周期。
// 默认为 VSF_KERNEL_CFG_TIMER_MODE_TICKLESS
#define VSF_KERNEL_CFG_TIMER_MODE                       VSF_KERNEL_CFG_TIMER_MODE_TICKLESS

// callback_timer 是 VSF 中的一种不依赖任务的定时器（实际内核中使用了内核任务来处理）。用户设置的定时事件到了之后，只是简单调用用户回调。
// 默认值根据 VSF_KERNEL_CFG_EDA_SUPPORT_TIMER 设置的不同会有差异。
// VSF_KERNEL_CFG_EDA_SUPPORT_TIMER 使能的时候， callback_timer 也默认使能；反之禁能。
#define VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER           VSF_KERNEL_CFG_TIMER_MODE

// 内核 IPC 。 vsf_sync_t 可以被封装成 mutex 互斥量、 semaphore 信号量、 trigger 触发器、 critical_section 临界代码段等等
// 默认使能
#define VSF_KERNEL_CFG_SUPPORT_SYNC                     ENABLED
// 配置是否使能中断里可以调用的一些 IPC 函数，这类函数都带 _isr 后缀。
// 默认使能
#   define VSF_SYNC_CFG_SUPPORT_ISR                     ENABLED

// 不依赖特定内存数据结构的内核队列。
// 默认使能
#define VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE                ENABLED

// 动态优先级支持，使能之后 mutex 具备防止优先级反转的能力。并且可以使用 __vsf_eda_set_priority 动态设置 eda 任务优先级。
// 默认使能
#define VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY         ENABLED

// 任务结束的回调配置
// 默认使能
#define VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE         ENABLED

// 使能的时候，每个事件都可以带一个指针消息，通过 vsf_err_t vsf_eda_post_evt_msg(vsf_eda_t *pthis, vsf_evt_t evt, void *msg); 发送。
// 处理事件的时候，通过 void * vsf_eda_get_cur_msg(void); 获得指针。
// 默认使能
#define VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE              ENABLED

// 各种不同任务形式的配置，请参考对应任务形式的说明文档
// 默认使能
#define VSF_KERNEL_CFG_SUPPORT_THREAD                   ENABLED
#define VSF_KERNEL_CFG_EDA_SUPPORT_TASK                 ENABLED
#define VSF_KERNEL_CFG_EDA_SUPPORT_PT                   ENABLED
#define VSF_KERNEL_CFG_EDA_SUPPORT_SIMPLE_FSM           ENABLED

// thread 模式下的堆栈检测
// 默认禁能
#define VSF_KERNEL_CFG_THREAD_STACK_CHECK               DISABLED

/********************************************************************************
 * 系统相关配置
 *******************************************************************************/

// systimer 的中断优先级设置，如果没有定义的话，会使用最高优中断先级。
// 默认不定义
#define VSF_OS_CFG_TIMER_ARCH_PRIORITY                  vsf_arch_prio_0

// 是否在复位中断中，也增加一个事件队列，该事件队列不可调优先级，可以被其他所有事件队列抢占。
// 默认关闭
#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                     DISABLED

// 优先级个数配置
// 如果 VSF_OS_CFG_ADD_EVTQ_TO_IDLE 使能，那么最低优先级的事件队列运行在复位中断里，其他优先级的事件队列，运行在 swi 中断里。并且硬件的 swi 中断数量必须足够。
// 如果 VSF_OS_CFG_ADD_EVTQ_TO_IDLE 禁能，那么所有事件队列都运行在 swi 中断里。同样，硬件的 swi 中断数量必须足够。
// 默认配置为最大可用优先级数量（受硬件 swi 中断数量影响）
#define VSF_OS_CFG_PRIORITY_NUM                         __VSF_OS_SWI_NUM

// 用户入口函数 VSF_USER_ENTRY 的任务形式， VSF_OS_CFG_MAIN_MODE_THREAD 独立堆栈任务 / VSF_OS_CFG_MAIN_MODE_EDA eda 任务 / VSF_OS_CFG_MAIN_MODE_IDLE 只是函数，不是任务
// 独立堆栈任务使能的时候，默认为独立堆栈任务；否则默认为普通函数
#define VSF_OS_CFG_MAIN_MODE                            VSF_OS_CFG_MAIN_MODE_THREAD

// VSF_OS_CFG_MAIN_MODE 配置为 VSF_OS_CFG_MAIN_MODE_THREAD 时候，设置堆栈大小，单位字节
// 默认为4K字节
#define VSF_OS_CFG_MAIN_STACK_SIZE                      4096

// 几种推荐的内核配置：
// 1. 协作式内核，不使用 swi ，只有复位中断里运行事件队列，单一优先级：
#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                     ENABLED
#define VSF_OS_CFG_PRIORITY_NUM                         1
#define VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY         DISABLED
// 2. 协作式内核，使用 swi ，只有一个 swi 中断里运行事件队列，单一优先级：
#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                     DISABLED
#define VSF_OS_CFG_PRIORITY_NUM                         1
#define VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY         DISABLED
// 3. 抢占式内核，支持动态优先级
#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                     DISABLED
#define VSF_OS_CFG_PRIORITY_NUM                         ACTUAL_NUM
#define VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY         ENABLED

```

### 3. **eda 任务** / **teda 任务**的实现和启动
- **eda 任务** / **teda 任务**类型
**eda 任务**：vsf_eda_t
**teda 任务**：vsf_teda_t ，继承自 vsf_eda_t

- 任务配置数据结构 vsf_eda_cfg_t
```c
typedef union __vsf_eda_fn_t {
    uintptr_t                   func;
    vsf_eda_evthandler_t        evthandler;             // eda/teda 任务的事件处理函数
    vsf_param_eda_evthandler_t  param_evthandler;       // peda任务的事件处理函数
} __vsf_eda_fn_t;

typedef struct vsf_eda_cfg_t {
    __vsf_eda_fn_t              fn;                     // 任务入口，具备多种任务形式，其中的 evthandler 对应 eda/teda 任务
    vsf_eda_on_terminate_t      on_terminate;           // 任务结束的通知回调
    vsf_prio_t                  priority;               // 任务优先级，vsf_prio_0 < vsf_prio_1 < vsf_prio_2 ......
    vsf_eda_feature_t           feature;                // 任务特性，eda 任务不使用
    uint16_t                    local_size;             // 特定任务使用，eda 任务不使用
    uintptr_t                   target;                 // 特定任务使用，eda 任务不使用
} vsf_eda_cfg_t;
```
- vsf_eda_start(vsf_eda_t *pthis, vsf_eda_cfg_t *cfg);
```c
#include "vsf.h"

/* VSF_CAL_NO_INIT 表示对应的内存不需要初始化 */
static VSF_CAL_NO_INIT vsf_eda_t __eda_task;

static void __user_eda_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_INIT:
        vsf_trace_info("user eda started...\r\n");
        break;
    }
}

int VSF_USER_ENTRY(void)
{
    vsf_start_trace();

    /* 启动 __eda_task 任务 */
    vsf_eda_start(&__eda_task, *(vsf_eda_cfg_t){
        .fn.evthandler  = __user_eda_evthandler,
        .priority       = vsf_prio_0,
    });
    return 0;
}
```
- vsf_teda_start(vsf_teda_t *pthis, vsf_eda_cfg_t *cfg);
和 vsf_eda_start 类似，只是第一个参数是 vsf_teda_t 类型。
- vsf_err_t vsf_teda_set_timer(vsf_systimer_tick_t tick);
- vsf_err_t vsf_teda_set_timer_ms(uint_fast32_t ms);
- vsf_err_t vsf_teda_set_timer_us(uint_fast32_t us);
注册定时器，在超时后，任务会收到 VSF_EVT_TIMER 事件。
```c
#include "vsf.h"

/* VSF_CAL_NO_INIT 表示对应的内存不需要初始化 */
static VSF_CAL_NO_INIT vsf_teda_t __teda_task;

static void __user_teda_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_INIT:
        vsf_trace_info("user teda started...\r\n");
        // fall through
    case VSF_EVT_TIMER:
        vsf_trace_info("tick...\r\n");
        vsf_teda_set_timer_ms(1000);
        break;
    }
}

int VSF_USER_ENTRY(void)
{
    vsf_start_trace();

    /* 启动 __teda_task 任务 */
    vsf_teda_start(&__teda_task, &(vsf_eda_cfg_t){
        .fn.evthandler  = __user_teda_evthandler,
        .priority       = vsf_prio_0,
    });
    return 0;
}
```

## 4. **eda 任务**的IPC
&emsp;&emsp;VSF 的信号量、互斥量、临界代码段、触发器等等，都是通过 vsf_sync_t 实现。 VSF 的内核队列，都是通过 vsf_eda_queue_t 实现。

- IPC 事件
  - VSF_EVT_SYNC：同步事件，表示获得同步 IPC。
  - VSF_EVT_SYNC_CANCEL：同步 IPC 被取消事件。
  - VSF_EVT_SYNC_POLL：同步 IPC 轮询事件，继续等待下一个 IPC 事件。
  - VSF_EVT_TIMER：超时

- vsf_sync_reason_t
收到 IPC 事件后，调用 vsf_eda_sync_get_reason 得到 IPC 执行结果。
```c
typedef enum vsf_sync_reason_t {
    VSF_SYNC_FAIL,              // IPC 同步出错
    VSF_SYNC_TIMEOUT,           // IPC 超时
    VSF_SYNC_PENDING,           // IPC 还在等待，还需继续等待 IPC 事件
    VSF_SYNC_GET,               // 已经获得 IPC
    VSF_SYNC_CANCEL,            // IPC 已经被取消
} vsf_sync_reason_t;
```
- vsf_sync_reason_t vsf_eda_sync_get_reason(vsf_sync_t *pthis, vsf_evt_t evt);
在 IPC 事件中，调用 vsf_eda_sync_get_reason ，得到 IPC 执行结果。

- vsf_timeout_tick_t -- 超时 tick ， tick 模式下，为系统定时器的溢出周期； tickless 模式下，为系统定时器的计数周期
  - 负数，一般使用 -1 -- 永远等待，所有任务都可以使用
  - 0 -- 不等待，所有任务都可以使用
  - 大于 0 -- 等待超时时间为对应的 tick 数， teda 任务或者继承自 teda 的任务可以使用

### 4.1 信号量
- vsf_err_t vsf_eda_sem_init(vsf_sem_t *sem, uint_fast16_t count = 0, uint_fast16_t max_count = VSF_SYNC_MAX);
- vsf_err_t vsf_eda_sem_post(vsf_sem_t *sem);
- vsf_err_t vsf_eda_sem_pend(vsf_sem_t *sem, vsf_timeout_tick_t timeout = -1);
- vsf_err_t vsf_eda_sem_post_isr(vsf_sem_t *sem);

```c
#include "vsf.h"

static VSF_CAL_NO_INIT vsf_sem_t __user_sem;
static VSF_CAL_NO_INIT vsf_teda_t __teda_a;
static VSF_CAL_NO_INIT vsf_eda_t __eda_b;

static void __user_teda_a_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_INIT:
        vsf_trace_info("user teda_a started...\r\n");
        // fallthrough
    case VSF_EVT_TIMER:
        vsf_eda_sem_post(&__user_sem);
        vsf_teda_set_timer_ms(1000);
        break;
    }
}

static void __user_eda_b_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_INIT:
        vsf_trace_info("user eda_b started...\r\n");
    __pend_next:
        if (VSF_ERR_NONE != vsf_eda_sem_pend(&__user_sem)) {
            // 没有得到信号量，退出并且等待 VSF_EVT_SYNC 事件
            break;
        }
        evt = VSF_EVT_SYNC;
        // fallthrough
        // 实际不会取消信号量，也不会超时，只可能发生 VSF_EVT_SYNC 事件
    case VSF_EVT_TIMER:
    case VSF_EVT_SYNC_CANCEL:
    case VSF_EVT_SYNC: {
            // 实际不会取消信号量，也不会超时，可以默认就是 reason 是 VSF_SYNC_GET
            vsf_sync_reason_t reason = vsf_eda_sync_get_reason((vsf_sync_t *)&__user_sem, evt);
            if (VSF_SYNC_GET == reason) {
                vsf_trace_info("eda_b got sem\r\n");
                goto __pend_next;
            }
        }
    }
}

int VSF_USER_ENTRY(void)
{
    vsf_start_trace();

    vsf_eda_sem_init(&__user_sem);
    vsf_teda_start(&__teda_a, &(vsf_eda_cfg_t){
        .fn.evthandler  = __user_teda_a_evthandler,
        .priority       = vsf_prio_0,
    });
    vsf_eda_start(&__eda_b, &(vsf_eda_cfg_t){
        .fn.evthandler  = __user_eda_b_evthandler,
        .priority       = vsf_prio_0,
    });
    return 0;
}
```

### 4.2 互斥量
- vsf_err_t vsf_eda_mutex_init(vsf_mutex_t *mutex);
- vsf_err_t vsf_eda_mutex_enter(vsf_mutex_t *mutex, vsf_timeout_tick_t timeout = -1);
- vsf_err_t vsf_eda_mutex_leave(vsf_mutex_t *mutex);
- vsf_err_t vsf_eda_mutex_leave_isr(vsf_mutex_t *mutex);

```c
#include "vsf.h"

static VSF_CAL_NO_INIT vsf_mutex_t __user_mutex;
static VSF_CAL_NO_INIT vsf_teda_t __teda_a;
static VSF_CAL_NO_INIT vsf_teda_t __teda_b;

static void __user_teda_a_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_INIT:
        vsf_trace_info("user teda_a started...\r\n");
    __try_get_mutex:
        if (VSF_ERR_NONE != vsf_eda_mutex_enter(&__user_mutex)) {
            // 没有得到互斥量，退出并且等待 VSF_EVT_SYNC 事件
            break;
        }
        evt = VSF_EVT_SYNC;
        // fall through
        // 实际不会取消互斥量，也不会超时，只可能发生 VSF_EVT_SYNC 事件
    case VSF_EVT_SYNC_CANCEL:
    case VSF_EVT_SYNC: {
            // 实际不会取消互斥量，也不会超时，可以默认就是 VSF_SYNC_GET
            vsf_sync_reason_t reason = vsf_eda_sync_get_reason((vsf_sync_t *)&__user_mutex, evt);
            if (VSF_SYNC_GET == reason) {
                vsf_trace_info("teda_a got mutex\r\n");
                vsf_teda_set_timer_ms(1000);
            }
            break;
        }
    case VSF_EVT_TIMER:
        vsf_eda_mutex_leave(&__user_mutex);
        vsf_trace_info("teda_a released mutex\r\n");
        goto __try_get_mutex;
    }
}

static void __user_teda_b_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_INIT:
        vsf_trace_info("user teda_b started...\r\n");
    __try_get_mutex:
        if (VSF_ERR_NONE != vsf_eda_mutex_enter(&__user_mutex)) {
            // 没有得到互斥量，退出并且等待 VSF_EVT_SYNC 事件
            break;
        }
        evt = VSF_EVT_SYNC;
        // fall through
    case VSF_EVT_SYNC_CANCEL:
    case VSF_EVT_SYNC: {
            vsf_sync_reason_t reason = vsf_eda_sync_get_reason((vsf_sync_t *)&__user_mutex, evt);
            if (VSF_SYNC_GET == reason) {
                vsf_trace_info("teda_b got mutex\r\n");
                vsf_teda_set_timer_ms(700);
            }
            break;
        }
    case VSF_EVT_TIMER:
        vsf_eda_mutex_leave(&__user_mutex);
        vsf_trace_info("teda_b released mutex\r\n");
        goto __try_get_mutex;
    }
}

int VSF_USER_ENTRY(void)
{
    vsf_start_trace();

    vsf_eda_mutex_init(&__user_mutex);

    vsf_teda_start(&__teda_a, &(vsf_eda_cfg_t){
        .fn.evthandler  = __user_teda_a_evthandler,
        .priority       = vsf_prio_0,
    });
    vsf_teda_start(&__teda_b, &(vsf_eda_cfg_t){
        .fn.evthandler  = __user_teda_b_evthandler,
        .priority       = vsf_prio_0,
    });
    return 0;
}
```

### 4.3 临界代码段
&emsp;&emsp;等价于互斥量。

- vsf_err_t vsf_eda_crit_init(vsf_crit_t *crit);
- vsf_err_t vsf_eda_crit_enter(vsf_crit_t *crit, vsf_timeout_tick_t timeout = -1);
- vsf_err_t vsf_eda_crit_leave(vsf_crit_t *crit);

### 4.4 触发器
- vsf_err_t vsf_eda_trig_init(vsf_trig_t *trig, bool is_set, bool is_auto_reset);
- vsf_err_t vsf_eda_trig_set(vsf_trig_t *trig, bool is_manual_reset = false); -- if is_manual_reset is false, use reset setting in vsf_eda_trig_init
- vsf_err_t vsf_eda_trig_reset(vsf_trig_t *trig);
- vsf_err_t vsf_eda_trig_wait(vsf_trig_t *trig, vsf_timeout_tick_t timeout = -1);
- vsf_err_t vsf_eda_trig_set_isr(vsf_trig_t *trig);

```c
#include "vsf.h"

static VSF_CAL_NO_INIT vsf_trig_t __user_trig;
static VSF_CAL_NO_INIT vsf_teda_t __teda_a;
static VSF_CAL_NO_INIT vsf_eda_t __eda_b;

static void __user_teda_a_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_INIT:
        vsf_trace_info("user teda_a started...\r\n");
        // fall through
    case VSF_EVT_TIMER:
        vsf_eda_trig_set(&__user_trig);
        vsf_teda_set_timer_ms(1000);
        break;
    }
}

static void __user_eda_b_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_INIT:
        vsf_trace_info("user eda_b started...\r\n");
    __pend_next:
        if (VSF_ERR_NONE != vsf_eda_trig_wait(&__user_trig)) {
            // 没有得到触发的事件，退出并且等待 VSF_EVT_SYNC 事件
            break;
        }
        evt = VSF_EVT_SYNC;
        // fall through
        // 实际不会取消触发器，也不会超时，只可能发生 VSF_EVT_SYNC 事件
    case VSF_EVT_TIMER:
    case VSF_EVT_SYNC_CANCEL:
    case VSF_EVT_SYNC: {
            // 实际不会取消触发器，也不会超时，可以默认就是 VSF_SYNC_GET
            vsf_sync_reason_t reason = vsf_eda_sync_get_reason(&__user_trig, evt);
            if (VSF_SYNC_GET == reason) {
                vsf_trace_info("eda_b triggered\r\n");
                goto __pend_next;
            }
        }
    }
}

int VSF_USER_ENTRY(void)
{
    vsf_start_trace();

    vsf_eda_trig_init(&__user_trig, false, true);

    vsf_teda_start(&__teda_a, &(vsf_eda_cfg_t){
        .fn.evthandler  = __user_teda_a_evthandler,
        .priority       = vsf_prio_0,
    });
    vsf_eda_start(&__eda_b, &(vsf_eda_cfg_t){
        .fn.evthandler  = __user_eda_b_evthandler,
            .priority       = vsf_prio_0,
    });
    return 0;
}
```

### 4.5 队列
- vsf_err_t vsf_eda_queue_init(vsf_eda_queue_t *pthis, uint_fast16_t max);
- vsf_err_t vsf_eda_queue_send(vsf_eda_queue_t *pthis, void *node, vsf_timeout_tick_t timeout);
- vsf_sync_reason_t vsf_eda_queue_send_get_reason(vsf_eda_queue_t *pthis, vsf_evt_t evt, void *node);
- vsf_err_t vsf_eda_queue_recv(vsf_eda_queue_t *pthis, void **node, vsf_timeout_tick_t timeout);
- vsf_sync_reason_t vsf_eda_queue_recv_get_reason(vsf_eda_queue_t *pthis, vsf_evt_t evt, void **node);
- uint_fast16_t vsf_eda_queue_get_cnt(vsf_eda_queue_t *pthis);
- void vsf_eda_queue_cancel(vsf_eda_queue_t *pthis);
- vsf_err_t vsf_eda_queue_send_isr(vsf_eda_queue_t *pthis, void *node);
- vsf_err_t vsf_eda_queue_recv_isr(vsf_eda_queue_t *pthis, void **node);

## 5. eda 任务的其他 API
