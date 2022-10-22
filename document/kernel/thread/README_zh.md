# 《 VSF 独立堆栈任务》

[TOC]

## 1. 简介
&emsp;&emsp;**独立堆栈任务**（后面会简单使用 thread 任务，或者 thread 来表述）和普通 RTOS 中的线程一样，具有独立的堆栈，开发相对简单。 VSF 中，所有任务都是基于**事件驱动任务**（ **eda 任务**）实现的，**独立堆栈任务**也同样如此。任务的堆栈切换是通过libc中的setjmp库实现的，不需要额外的汇编移植。

&emsp;&emsp;避坑指南：
- 有些环境不支持 setjmp 库，或者 setjmp 库支持不完整的，并且无替代实现。这个环境下就无法使用**独立堆栈任务**，不支持的环境如下：
  - 双堆栈构架的51处理器
- 有一些环境，虽然 c 库中的 setjmp 不完整，但是有 2 种情况下是可以使用**独立堆栈任务**
  - 有替代 setjmp 库。比如： x86_64 的 windows 系统， C 库里的 setjmp 切换堆栈后，会使得堆栈检测不通过。目前 VSF 里已经集成了 intel 的 x64 setjmp 库。
  - VSF 运行在其他 OS 中， VSF 的**独立堆栈任务**直接对接了 OS 的线程。比如， linux 、 windows 、 wasm ( web assembly ，双堆栈构架)、各种 RTOS

## 2. 配置

```c
    /* 开启内核中的 thread 模式支持，用户没有配置的话，默认打开 */
    #define VSF_KERNEL_CFG_SUPPORT_THREAD           ENABLED

    /* 用户入口函数（ VSF_USR_ENTRY ，一般为 main ）运行模式配置
       可以配置为 thread 任务形式、事件驱动任务形式、普通函数形式
       默认为 thread 形式
    */
    #define VSF_OS_CFG_MAIN_MODE                    VSF_OS_CFG_MAIN_MODE_THREAD
    /* 如果用户入口为 thread 形式，需要配置堆栈大小（字节单位），默认为 4096 字节 */
    #define VSF_OS_CFG_MAIN_STACK_SIZE              2048
```

## 3. thread任务实现和启动
&emsp;&emsp;VSF 的 thread 任务可以使用声明式的方式开发，用户不需要知道内部的数据结构，直接按照声明、定义、实现、启动的流程，就可以使用 thread 任务了。涉及到的宏 API：

- declare_vsf_thread/dcl_vsf_thread (类型名) -- 声明指定的 thread 任务类型
- define_vsf_thread/def_vsf_thread (类型名, 任务栈字节大小, ...) -- 定义指定的 thread 任务类型，可变参数为和任务绑定的数据成员
- implement_vsf_thread/imp_vsf_thread (类型名) -- 实现指定类型的任务函数
- init_vsf_thread (类型名, 指定类型的任务实例, 任务优先级) -- 初始化并且启动任务，优先级为 vsf_prio_0 , vsf_prio_1 ...，优先级递增


```c
#include "vsf.h"

/* 申明用户 thread 任务类型 user_thread_t */
declare_vsf_thread(user_thread_t)

/* 定义用户任务类型 user_thread_t */
define_vsf_thread(user_thread_t, 1024)

/* 实现用户定义的任务类型的任务函数 */
implement_vsf_thread(user_thread_t)
{
    vsf_trace_info("Hello World!\r\n");
}

static user_thread_t __thread_task;

int main(void)
{
    vsf_start_trace();
    init_vsf_thread(user_thread_t, &__thread_task, vsf_prio_0);
    return 0;
}
```

&emsp;&emsp;如果任务需要绑定一些数据的话，可以在 define_vsf_thread 定义任务的时候扩展，可以扩展各种类型的变量包括结构、联合体变量等等。在 thread 任务函数中，可以通过 vsf_this/vsf_pthis 来引用这些数据。

```c
#include "vsf.h"

/* 申明用户 thread 任务类型 user_thread_t */
declare_vsf_thread(user_thread_t)

/* 定义用户任务类型 user_thread_t */
define_vsf_thread(user_thread_t, 1024,
    int count0;
    int count1;
)

/* 实现用户定义的任务类型的任务函数 */
implement_vsf_thread(user_thread_t)
{
    vsf_trace_info("Hello World! I got %d %d.\r\n", vsf_this.count0, vsf_this.count1);
}

static user_thread_t __thread_task = {
    /* 初始化 count0 */
    .count0 = 6,
};

int main(void)
{
    vsf_start_trace();

    /* 初始化 count1 */
    __thread_task.count1 = 10;
    init_vsf_thread(user_thread_t, &__thread_task, vsf_prio_0);
    return 0;
}
```

## 4. thread任务的IPC

### 4.1 信号量
&emsp;&emsp;相关 API 。具体参数和返回值，可以参考内核文档中的信号量的章节，意义完全一样
- vsf_err_t vsf_eda_sem_init(vsf_sem_t *sem, uint_fast16_t count = 0, uint_fast16_t max_count = VSF_SYNC_MAX);
- vsf_err_t vsf_thread_sem_post(vsf_sem_t *sem); -- 发送信号量，等价于vsf_eda_sem_post
- vsf_sync_reason_t vsf_thread_sem_pend(vsf_sem_t *sem, vsf_timeout_tick_t timeout); -- 等待信号量

```c
#include "vsf.h"

/* 申明用户 thread 任务类型 user_thread_t */
declare_vsf_thread(user_thread_t)

/* 定义用户任务类型 user_thread_t */
define_vsf_thread(user_thread_t, 1024,
    vsf_sem_t *sem;
)

/* 实现用户定义的任务类型的任务函数 */
implement_vsf_thread(user_thread_t)
{
    while (1) {
        /* -1 表示不会超时，永久等待，并且没有 cancel 操作，所以可以忽略 vsf_thread_sem_pend 的返回值 */
        vsf_thread_sem_pend(vsf_this.sem, -1);
        vsf_trace_info("Hello World!\r\n");
    }
}

static vsf_sem_t __sem;
static user_thread_t __thread_task = {
    .sem = &__sem,
};

int main(void)
{
    vsf_start_trace();

    vsf_eda_sem_init(&__sem, 0);
    init_vsf_thread(user_thread_t, &__thread_task, vsf_prio_0);

    while (1) {
        vsf_thread_delay_ms(1000);
        vsf_thread_sem_post(&__sem);
    }
    return 0;
}
```

### 4.2 互斥量
&emsp;&emsp;相关 API 。具体参数和返回值，可以参考内核文档中的互斥量的章节，意义完全一样
- vsf_err_t vsf_eda_mutex_init(vsf_mutex_t *mutex);
- vsf_sync_reason_t vsf_thread_mutex_enter(vsf_mutex_t *mtx, vsf_timeout_tick_t timeout);
- vsf_err_t vsf_thread_mutex_leave(vsf_mutex_t *mtx);

```c
#include "vsf.h"

/* 申明用户 thread 任务类型 user_thread_t */
declare_vsf_thread(user_thread_t)

/* 定义用户任务类型 user_thread_t */
define_vsf_thread(user_thread_t, 1024,
    int id;
    vsf_mutex_t *mutex;
)

/* 实现用户定义的任务类型的任务函数 */
implement_vsf_thread(user_thread_t)
{
    while (1) {
        vsf_thread_mutex_enter(vsf_this.mutex, -1);
            vsf_trace_info("Hello World! from %d\r\n", vsf_this.id);
        vsf_thread_mutex_leave(vsf_this.mutex);
        vsf_thread_delay_ms(100 * (vsf_this.id + 1));
    }
}

static vsf_mutex_t __mutex;
static user_thread_t __thread_tasks[10];

int main(void)
{
    vsf_start_trace();
    vsf_eda_mutex_init(&__mutex);

    for (int i = 0; i < dimof(thread_a); i++) {
        __thread_tasks[i].param.id = i;
        __thread_tasks[i].param.mutex = &__mutex;
        init_vsf_thread(user_thread_t, &__thread_tasks[i], vsf_prio_0);
    }
    return 0;
}
```

### 4.3 临界代码段
&emsp;&emsp;使用互斥量实现。

### 4.4 触发器
&emsp;&emsp;相关 API 。具体参数和返回值，可以参考内核文档中的触发器的章节，意义完全一样
- vsf_err_t vsf_eda_trig_init(*vsf_trig_t *trig, bool is_set, bool is_auto_reset);
- vsf_err_t vsf_thread_trig_set(vsf_trig_t *trig);
- vsf_err_t vsf_thread_trig_set(vsf_trig_t *trig, bool is_manual_reset);
- void vsf_thread_trig_reset(vsf_trig_t *trig);
- vsf_sync_reason_t vsf_thread_trig_pend(vsf_trig_t *trig, vsf_timeout_tick_t timeout);

```c
#include "vsf.h"

/* 申明用户 thread 任务类型 user_thread_t */
declare_vsf_thread(user_thread_t)

/* 定义用户任务类型 user_thread_t */
define_vsf_thread(user_thread_t, 1024,
    int id;
    vsf_trig_t *trig;
)

/* 实现用户定义的任务类型的任务函数 */
implement_vsf_thread(user_thread_t)
{
    while (1) {
        /* -1 表示不会超时，永久等待，并且没有 cancel 操作，所以可以忽略 vsf_thread_trig_pend 的返回值 */
        vsf_thread_trig_pend(vsf_this.trig, -1);
        vsf_trace_info("Hello World! from %d.\r\n", vsf_this.id);
    }
}

static vsf_trig_t __trig;
static user_thread_t __thread_tasks[10];

int main(void)
{
    vsf_start_trace();

    vsf_eda_trig_init(&__trig, false, true);
    for (int i = 0; i < dimof(__thread_tasks); i++) {
        __thread_tasks[i].param.id = i;
        __thread_tasks[i].param.trig = &__trig;
        init_vsf_thread(user_thread_t, &__thread_tasks[i], vsf_prio_0);
    }

    while (1) {
        vsf_thread_delay_ms(1000);
        vsf_thread_trig_set(&__trig);
    }
    return 0;
}
```

### 4.5 队列
&emsp;&emsp;相关 API 。具体参数和返回值，可以参考内核文档中的队列的章节，意义完全一样
- vsf_sync_reason_t vsf_thread_queue_send(vsf_eda_queue_t *queue, void *node, vsf_timeout_tick_t timeout);
- vsf_sync_reason_t vsf_thread_queue_recv(vsf_eda_queue_t *queue, void **node, vsf_timeout_tick_t timeout);

## 5. thread 任务的其他 API
### 5.1 延时相关
- void vsf_thread_delay(vsf_systimer_tick_t tick);  -- 延时指定的 tick 周期
- void vsf_thread_delay_ms(uint_fast32_t ms); -- 延时指定的毫秒
- void vsf_thread_delay_us(uint_fast32_t us); -- 延时指定的微妙

### 5.2 优先级相关
&emsp;&emsp;使能动态优先级的时候（配置 VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY 为 ENABLED ），可以动态设置当前 thread 任务的优先级。
- vsf_prio_t vsf_thread_set_priority(vsf_prio_t priority);