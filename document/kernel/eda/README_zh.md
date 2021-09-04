# 《VSF 事件驱动》

## 1. 简介
&emsp;&emsp;VSF中的所有任务底层都是事件驱动（Event-driven Architecture，以下简称**eda**或者**eda任务**。具备定时器功能的**eda任务**称为**teda任务**）。VSF的事件驱动框架下，事件驱动任务仅仅是处理任务收到的各个事件。从任务的接口形式上，就可以很容易理解：
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
&emsp;&emsp;这里，每个事件的处理，都必须是非阻塞的。所以，一般都是使用状态机的方式开发，所以可以称为**事件驱动状态机**的开发方式。存粹的eda任务都是共享堆栈的，只在处理事件的时候，才会占用堆栈，处理完后任务推出，不再占用堆栈。并且，可以通过vsf里的封装，把**eda任务**封装成其他的任务形式，甚至包括[**独立堆栈任务**](../thread/README_zh.md)。由于**eda任务**一般需要使用状态机，所以开发难度较高，资源占用较少（仅需要20-40字节ram）；并且由于代码一般都不是顺序实行，所以逆向的难度会增大。

&emsp;&emsp;**eda任务**是有优先级，vsf系统可以支持抢占式或者协作式的调度方式（第二章中，推荐的内核配置里，有对应的设置方式）。事件驱动下，会有**事件队列**，**事件队列**运行在swi中断里（软件中断，可设置优先级，可屏蔽），或者复位中断里（不可设置优先级、不可屏蔽、可以被所有其他中断抢占）。根据**事件队列**运行的位置，决定了**事件队列**的优先级，**事件队列**的优先级抢占也是纯硬件的中断抢占。高优先级的swi中断里的**事件队列**，其优先级大于运行在低中断优先级的swi中断里的**事件队列**，并且，复位中断里的**事件队列**优先级最低。由于swi中断的优先级可以设置，所以swi中断里运行的**事件队列**的优先级也是可调的。由于硬件特性的不同，协作式内核的时候，只使用复位中断里运行**事件队列**；而抢占式内核的时候，只使用swi中断里运行**事件队列**。每个**eda任务**都只隶属于一个**事件队列**，也就是只有一个任务优先级。

&emsp;&emsp;事件的定义：
- VSF_EVT_INIT：初始化事件，任务启动的时候，会收到一次。
- VSF_EVT_TIMER：定时器事件，**teda任务**里有效。
- VSF_EVT_SYNC：同步事件，表示或者同步IPC。
- VSF_EVT_SYNC_CANCEL：同步IPC被取消事件。
- VSF_EVT_MESSAGE：消息事件，带一个指针参数，通过void * vsf_eda_get_cur_msg(void);获得指针。
- VSF_EVT_USER：从VSF_EVT_USER开始，之后的都是用户事件。

## 2. 配置
&emsp;&emsp;由于其他形式的任务底层都是**eda任务**，所以eda的配置也会影响所有其他形式的任务。一些**IPC（任务间通信）**相关的内容，请首先阅读[**IPC（任务间融信）**](../ipc/README_zh.md)。
```c
/********************************************************************************
 * 内核相关配置
 *******************************************************************************/

// 定时器支持，使能之后，可以通过vsf_teda_t来定义集成定时器功能的eda任务。并且，其他的任务形式，也都会从vsf_teda_t派生（定时器没使能的时候，从vsf_eda_t派生）。
// 默认使能
#define VSF_KERNEL_CFG_EDA_SUPPORT_TIMER                ENABLED

// 内核定时器模式，可以设置为VSF_KERNEL_CFG_TIMER_MODE_TICK或者VSF_KERNEL_CFG_TIMER_MODE_TICKLESS模式。
// VSF_KERNEL_CFG_TIMER_MODE_TICK模式下，定时器基于固定的周期。除了一些特殊情况外（比如资源极其有限），vsf一般不会使用tick模式。
//      vsf_systimer_tick_t以及vsf_timeout_tick_t的单位都是系统定时器的溢出周期
// VSF_KERNEL_CFG_TIMER_MODE_TICKLESS模式下，系统不需要被周期性的中断唤醒，并且高层API完全兼容，支持极小延迟的定时器。
//      vsf_systimer_tick_t以及vsf_timeout_tick_t的单位都是这个系统定时器的计数周期
// 不同模式下，用户API完全兼容，只是行为上会有略微差异。在TICK模式下，小于TICK周期的延迟，会被拉长到TICK周期。
// 默认为VSF_KERNEL_CFG_TIMER_MODE_TICKLESS
#define VSF_KERNEL_CFG_TIMER_MODE                       VSF_KERNEL_CFG_TIMER_MODE_TICKLESS

// callback_timer是VSF中的一种不依赖任务的定时器（实际内核中使用了内核任务来处理）。用户设置的定时事件到了之后，只是简单调用用户回调。
// 默认值根据VSF_KERNEL_CFG_EDA_SUPPORT_TIMER设置的不同会有差异。
// VSF_KERNEL_CFG_EDA_SUPPORT_TIMER使能的时候，callback_timer也默认使能；反之禁能。
#define VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER           VSF_KERNEL_CFG_TIMER_MODE

// 内核IPC。vsf_sync_t可以被封装成mutex（互斥量）、semaphore（信号量）、trigger（触发器）、critical_section（临界代码段）等等
// 默认使能
#define VSF_KERNEL_CFG_SUPPORT_SYNC                     ENABLED
// 配置是否可以在中断里，调用vsf_err_t vsf_eda_sem_post_isr(vsf_sem_t *sem)来发送信号
// 默认使能
#   define VSF_SYNC_CFG_SUPPORT_ISR                     ENABLED

// 不依赖特定内存数据结构的内核队列。
// 默认使能
#define VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE                ENABLED

// 动态优先级支持，使能的话，mutex具备防止优先级反转的能力。并且可以使用__vsf_eda_set_priority设置eda任务优先级，不过不会马上生效，需要在下一次事件处理的时候才生效。
// 默认使能
#define VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY         ENABLED

// 任务结束的回调配置
// 默认使能
#define VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE         ENABLED

// 使能的时候，每个事件都可以带一个指针消息。消息指针通过void * vsf_eda_get_cur_msg(void);获得。
// 默认使能
#define VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE              ENABLED

// 各种不同任务形式的配置，请参考对应任务形式的说明文档
// 默认使能
#define VSF_KERNEL_CFG_SUPPORT_THREAD                   ENABLED
#define VSF_KERNEL_CFG_EDA_SUPPORT_TASK                 ENABLED
#define VSF_KERNEL_CFG_EDA_SUPPORT_PT                   ENABLED
#define VSF_KERNEL_CFG_EDA_SUPPORT_SIMPLE_FSM           ENABLED

// thread模式下的堆栈检测
// 默认禁能
#define VSF_KERNEL_CFG_THREAD_STACK_CHECK               DISABLED

/********************************************************************************
 * 系统相关配置
 *******************************************************************************/

// 是否在IDLE中，也增加一个事件队列。
// 默认关闭
#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                     DISABLED

// 优先级个数配置
// 如果VSF_OS_CFG_ADD_EVTQ_TO_IDLE使能，那么最低优先级的事件队列运行在复位中断里，其他优先级的事件队列，运行在swi中断里。并且硬件的swi中断数量必须足够。
// 如果VSF_OS_CFG_ADD_EVTQ_TO_IDLE禁能，那么所有事件队列都运行在swi中断里。同样，硬件的swi中断数量必须足够。
// 默认配置为最大可用优先级数量（受硬件swi中断数量影响）
#define VSF_OS_CFG_PRIORITY_NUM                         __VSF_OS_SWI_NUM

// main函数的任务形式，VSF_OS_CFG_MAIN_MODE_THREAD（独立堆栈任务）/VSF_OS_CFG_MAIN_MODE_EDA（eda任务）/VSF_OS_CFG_MAIN_MODE_IDLE（只是函数，不是任务）
// 独立堆栈任务使能的时候，默认为独立堆栈任务；否则默认为普通函数
#define VSF_OS_CFG_MAIN_MODE                            VSF_OS_CFG_MAIN_MODE_THREAD

// VSF_OS_CFG_MAIN_MODE配置为VSF_OS_CFG_MAIN_MODE_THREAD时候，设置堆栈大小，单位字节
// 默认为4K字节
#define VSF_OS_CFG_MAIN_STACK_SIZE                      4096

// 几种推荐的内核配置：
// 1. 协作式内核，不使用swi，只有复位中断里运行事件队列，单一优先级：
#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                     ENABLED
#define VSF_OS_CFG_PRIORITY_NUM                         1
#define VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY         DISABLED
// 2. 协作式内核，使用swi，只有一个swi中断里运行事件队列，单一优先级：
#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                     DISABLED
#define VSF_OS_CFG_PRIORITY_NUM                         1
#define VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY         DISABLED
// 3. 抢占式内核，支持动态优先级
#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                     DISABLED
#define VSF_OS_CFG_PRIORITY_NUM                         ACTUAL_NUM
#define VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY         ENABLED

```

### 3. **eda任务**/**teda任务**的实现和启动
- 任务配置数据结构vsf_eda_cfg_t
```c
typedef struct vsf_eda_cfg_t {
    __vsf_eda_fn_t              fn;                     // 任务入口，具备多种任务形式，其中的evthandler对应eda任务
    vsf_eda_on_terminate_t      on_terminate;           // 任务结束的通知回调
    vsf_prio_t                  priority;               // 任务优先级，vsf_prio_0 < vsf_prio_1 < vsf_prio_2 ......
    vsf_eda_feature_t           feature;                // 任务特性，eda任务不使用
    uint16_t                    local_size;             // 特定任务使用，eda任务不使用
    uintptr_t                   target;                 // 特定任务使用，eda任务不使用
} vsf_eda_cfg_t;
```
- vsf_eda_start(vsf_eda_t *pthis, vsf_eda_cfg_t *cfg);
```c
#include "vsf.h"

static void __user_eda_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_INIT:
        vsf_trace_info("user eda started...\r\n");
        break;
    }
}

int main(void)
{
    vsf_start_trace();

    /* 使用大括号是为了限制eda任务实例的作用域，NO_INIT表示对应的内存不需要初始化 */
    {
        static NO_INIT vsf_eda_t __eda_a;
        const vsf_eda_cfg_t cfg = {
            .fn.evthandler  = __user_eda_evthandler,
            .priority       = vsf_prio_0,
        };

        /* 启动__eda_a任务，强制类型转换用于避免编译警告 */
        vsf_eda_start(&__eda_a, (vsf_eda_cfg_t *)&cfg);
    }
    return 0;
}
```
- vsf_teda_start(vsf_teda_t *pthis, vsf_eda_cfg_t *cfg);
- vsf_err_t vsf_teda_set_timer(vsf_systimer_tick_t tick);
- vsf_err_t vsf_teda_set_timer_ms(uint_fast32_t ms);
- vsf_err_t vsf_teda_set_timer_us(uint_fast32_t us);
```c
#include "vsf.h"

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

int main(void)
{
    vsf_start_trace();

    /* 使用大括号是为了限制eda任务实例的作用域，NO_INIT表示对应的内存不需要初始化 */
    {
        static NO_INIT vsf_teda_t __teda_a;
        const vsf_eda_cfg_t cfg = {
            .fn.evthandler  = __user_teda_evthandler,
            .priority       = vsf_prio_0,
        };

        /* 启动__teda_a任务，强制类型转换用于避免编译警告 */
        vsf_teda_start(&__teda_a, (vsf_eda_cfg_t *)&cfg);
    }
    return 0;
}
```

## 4. **eda任务**的IPC
&emsp;&emsp;VSF的信号量、互斥量、临界代码段、触发器等等，都是通过vsf_sync_t实现。VSF的内核队列，都是通过vsf_eda_queue_t实现。

- vsf_sync_reason_t
```c
typedef enum vsf_sync_reason_t {
    VSF_SYNC_FAIL,              // IPC同步出错
    VSF_SYNC_TIMEOUT,           // IPC超时
    VSF_SYNC_PENDING,           // IPC还在等待
    VSF_SYNC_GET,               // 已经获得IPC
    VSF_SYNC_CANCEL,            // IPC已经被取消
} vsf_sync_reason_t;
```
- vsf_sync_reason_t vsf_eda_sync_get_reason(vsf_sync_t *pthis, vsf_evt_t evt); -- 根据事件得到sync的事件
- vsf_timeout_tick_t -- 超时tick，tick模式下，为系统定时器的溢出周期；tickless模式下，为系统定时器的计数周期
  - 负数，一般使用-1 -- 不超时（永远等待）
  - 0 -- 不使用超时（不等待）
  - 大于0 -- 使用超时，超时时间为对应的tick数

### 4.1 信号量
- vsf_err_t vsf_eda_sem_init(vsf_sem_t *sem, uint_fast16_t count = 0, uint_fast16_t max_count = VSF_SYNC_MAX);
- vsf_err_t vsf_eda_sem_post(vsf_sem_t *sem);
- vsf_err_t vsf_eda_sem_pend(vsf_sem_t *sem, vsf_timeout_tick_t timeout = -1);
- vsf_err_t vsf_eda_sem_post_isr(vsf_sem_t *sem);

```c
#include "vsf.h"

static NO_INIT vsf_sem_t __user_sem;

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
            // 没有得到信号量，退出并且等待VSF_EVT_SYNC事件
            break;
        }
        evt = VSF_EVT_SYNC;
        // fallthrough
        // 实际不会取消信号量，也不会超时，只可能发生VSF_EVT_SYNC事件
    case VSF_EVT_TIMEOUT:
    case VSF_EVT_SYNC_CANCEL:
    case VSF_EVT_SYNC: {
            // 实际不会取消信号量，也不会超时，可以默认就是VSF_SYNC_GET
            vsf_sync_reason_t reason = vsf_eda_sync_get_reason((vsf_sync_t *)&__user_sem, evt);
            if (VSF_SYNC_GET == reason) {
                vsf_trace_info("eda_b got sem\r\n");
                goto __pend_next;
            }
        }
    }
}

int main(void)
{
    vsf_start_trace();

    vsf_eda_sem_init(&__user_sem);

    {
        static NO_INIT vsf_teda_t __teda_a;
        const vsf_eda_cfg_t cfg = {
            .fn.evthandler  = __user_teda_a_evthandler,
            .priority       = vsf_prio_0,
        };
        vsf_teda_start(&__teda_a, (vsf_eda_cfg_t *)&cfg);
    }
    {
        static NO_INIT vsf_eda_t __eda_b;
        const vsf_eda_cfg_t cfg = {
            .fn.evthandler  = __user_eda_b_evthandler,
            .priority       = vsf_prio_0,
        };

        vsf_eda_start(&__eda_b, (vsf_eda_cfg_t *)&cfg);
    }
    return 0;
}
```

### 4.2 互斥量
- vsf_err_t vsf_eda_mutex_init(vsf_mutex_t *mutex);
- vsf_err_t vsf_eda_mutex_enter(vsf_mutex_t *mutex, vsf_timeout_tick_t timeout = -1);
- vsf_err_t vsf_eda_mutex_leave(vsf_mutex_t *mutex);

```c
#include "vsf.h"

static NO_INIT vsf_mutex_t __user_mutex;

static void __user_teda_a_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_INIT:
        vsf_trace_info("user teda_a started...\r\n");
    __try_get_mutex:
        if (VSF_ERR_NONE != vsf_eda_mutex_enter(&__user_mutex)) {
            // 没有得到互斥量，退出并且等待VSF_EVT_SYNC事件
            break;
        }
        evt = VSF_EVT_SYNC;
        // fallthrough
        // 实际不会取消互斥量，也不会超时，只可能发生VSF_EVT_SYNC事件
    case VSF_EVT_SYNC_CANCEL:
    case VSF_EVT_SYNC: {
            // 实际不会取消互斥量，也不会超时，可以默认就是VSF_SYNC_GET
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
            // 没有得到互斥量，退出并且等待VSF_EVT_SYNC事件
            break;
        }
        evt = VSF_EVT_SYNC;
        // fallthrough
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

int main(void)
{
    vsf_start_trace();

    vsf_eda_mutex_init(&__user_mutex);

    {
        static NO_INIT vsf_teda_t __teda_a;
        const vsf_eda_cfg_t cfg = {
            .fn.evthandler  = __user_teda_a_evthandler,
            .priority       = vsf_prio_0,
        };
        vsf_teda_start(&__teda_a, (vsf_eda_cfg_t *)&cfg);
    }
    {
        static NO_INIT vsf_teda_t __teda_b;
        const vsf_eda_cfg_t cfg = {
            .fn.evthandler  = __user_teda_b_evthandler,
            .priority       = vsf_prio_0,
        };

        vsf_teda_start(&__teda_b, (vsf_eda_cfg_t *)&cfg);
    }
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

static NO_INIT vsf_trig_t __user_trig;

static void __user_teda_a_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_INIT:
        vsf_trace_info("user teda_a started...\r\n");
        // fallthrough
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
            // 没有得到触发的事件，退出并且等待VSF_EVT_SYNC事件
            break;
        }
        evt = VSF_EVT_SYNC;
        // fallthrough
        // 实际不会取消触发器，也不会超时，只可能发生VSF_EVT_SYNC事件
    case VSF_EVT_TIMEOUT:
    case VSF_EVT_SYNC_CANCEL:
    case VSF_EVT_SYNC: {
            // 实际不会取消触发器，也不会超时，可以默认就是VSF_SYNC_GET
            vsf_sync_reason_t reason = vsf_eda_sync_get_reason(&__user_trig, evt);
            if (VSF_SYNC_GET == reason) {
                vsf_trace_info("eda_b triggered\r\n");
                goto __pend_next;
            }
        }
    }
}

int main(void)
{
    vsf_start_trace();

    vsf_eda_trig_init(&__user_trig, false, true);

    {
        static NO_INIT vsf_teda_t __teda_a;
        const vsf_eda_cfg_t cfg = {
            .fn.evthandler  = __user_teda_a_evthandler,
            .priority       = vsf_prio_0,
        };
        vsf_teda_start(&__teda_a, (vsf_eda_cfg_t *)&cfg);
    }
    {
        static NO_INIT vsf_eda_t __eda_b;
        const vsf_eda_cfg_t cfg = {
            .fn.evthandler  = __user_eda_b_evthandler,
            .priority       = vsf_prio_0,
        };

        vsf_eda_start(&__eda_b, (vsf_eda_cfg_t *)&cfg);
    }
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

## 5. eda任务调用其他形式的任务
TBD

## 6. eda任务的其他API
