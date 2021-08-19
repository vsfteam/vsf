# 《VSF 独立堆栈任务》

[TOC]

## 1. 简介
&emsp;&emsp;**独立堆栈任务**（后面会简单使用thread任务，或者thread来表述）和普通RTOS中的线程一样，具有独立的堆栈，开发相对简单，一般代码占用也相对较小。VSF中，**独立堆栈任务**是通过libc中的setjmp库，基于VSF的**事件驱动任务**实现的，可以和VSF中的各种其他任务形式同时运行。

&emsp;&emsp;避坑指南：
- 有些环境不支持setjmp库，或者setjmp库支持不完整的，无法使用**独立堆栈任务**。
  - 双堆栈结构的51处理器
  - windows下的x86_64，setjmp切换堆栈后，会使得堆栈检测不通过（因为切换到具体任务的堆栈了）

## 2. 配置

```c
    /* 开启内核中的thread模式支持，用户没有配置的话，默认打开 */
    #define VSF_KERNEL_CFG_SUPPORT_THREAD           ENABLED

    /* 用户入口函数（VSF_USR_ENTRY，一般为main）运行模式配置
       可以配置为thread任务形式、事件驱动任务形式、普通函数形式
       默认为thread形式
    */
    #define VSF_OS_CFG_MAIN_MODE                    VSF_OS_CFG_MAIN_MODE_THREAD
    /* 如果用户入口为thread形式，需要配置堆栈大小（字节单位），默认为4096字节 */
    #define VSF_OS_CFG_MAIN_STACK_SIZE              2048
```



## 3. thread任务实现和启动
&emsp;&emsp;VSF的thread任务可以使用声明式的方式开发，用户不需要知道内部的数据结构，直接按照声明、定义、实现、启动的流程，就可以使用thread任务了。涉及到的API：

- declare_vsf_thread/dcl_vsf_thread(类型名) -- 声明指定的thread任务类型
- define_vsf_thread/def_vsf_thread(类型名, 任务栈字节大小, ...) -- 定义指定的thread任务类型，可变参数为和任务绑定的数据成员
- implement_vsf_thread/imp_vsf_thread(类型名) -- 实现指定类型的任务函数
- init_vsf_thread(类型名, 指定类型的任务实例, 任务优先级) -- 初始化并且启动任务，优先级为vsf_prio_0, vsf_prio_1...，优先级递增


```c
#include "vsf.h"

/* 申明用户thread任务类型 user_thread_a_t */
declare_vsf_thread(user_thread_a_t)

/* 定义用户任务类型 user_thread_a_t */
define_vsf_thread(user_thread_a_t, 1024)

/* 实现用户定义的任务类型的任务函数 */
implement_vsf_thread(user_thread_a_t)
{
    vsf_trace_info("Hello World!\r\n");
}

int main(void)
{
    vsf_start_trace();

    /* 使用大括号是为了限制实例化的thread_a的作用域 */
    {
        /* 实例化 user_thread_a_t，用前面的类型，定义一个变量，注意这里是static的变量 */
        static user_thread_a_t thread_a;

        /* 启动thread_a任务 */
        init_vsf_thread(user_thread_a_t, &thread_a, vsf_prio_0);
    }
    return 0;
}
```

&emsp;&emsp;如果任务需要绑定一些数据的话，可以在define_vsf_thread定义任务的时候扩展，可以扩展各种类型的变量包括结构、联合体变量等等。在thread任务函数中，可以通过vsf_this来引用这些数据。实例化之后，可以通过实例的param成员，访问这些数据。

```c
#include "vsf.h"

/* 申明用户thread任务类型 user_thread_a_t */
declare_vsf_thread(user_thread_a_t)

/* 定义用户任务类型 user_thread_a_t */
define_vsf_thread(user_thread_a_t, 1024,
    int count0;
    int count1;
)

/* 实现用户定义的任务类型的任务函数 */
implement_vsf_thread(user_thread_a_t)
{
    vsf_trace_info("Hello World! I got %d %d.\r\n", vsf_this.count0, vsf_this.count1);
}

int main(void)
{
    vsf_start_trace();

    /* 使用大括号是为了限制实例化的thread_a的作用域 */
    {
        /* 实例化 user_thread_a_t，用前面的类型，定义一个变量，注意这里是static的变量 */
        static user_thread_a_t thread_a;

        /* 启动thread_a任务 */
        thread_a.param.count0 = 6;
        thread_a.param.count1 = 10;
        init_vsf_thread(user_thread_a_t, &thread_a, vsf_prio_0);
    }
    return 0;
}
```

## 4. thread任务的IPC

### 4.1 信号量
&emsp;&emsp;相关API。具体参数和返回值，可以参考内核文档中的信号量的章节，意义完全一样
- vsf_err_t vsf_eda_sem_init(vsf_sem_t *sem, uint_fast16_t count = 0, uint_fast16_t max_count = VSF_SYNC_MAX);
- vsf_err_t vsf_thread_sem_post(vsf_sem_t *sem); -- 发送信号量，等价于vsf_eda_sem_post
- vsf_sync_reason_t vsf_thread_sem_pend(vsf_sem_t *sem, vsf_timeout_tick_t timeout); -- 等待信号量

```c
#include "vsf.h"

/* 申明用户thread任务类型 user_thread_a_t */
declare_vsf_thread(user_thread_a_t)

/* 定义用户任务类型 user_thread_a_t */
define_vsf_thread(user_thread_a_t, 1024,
    vsf_sem_t *sem;
)

/* 实现用户定义的任务类型的任务函数 */
implement_vsf_thread(user_thread_a_t)
{
    while (1) {
        vsf_thread_sem_pend(vsf_this.sem, -1);
        vsf_trace_info("Hello World!\r\n");
    }
}

int main(void)
{
    vsf_start_trace();

    static vsf_sem_t sem;
    vsf_eda_sem_init(&sem, 0);

    /* 使用大括号是为了限制实例化的thread_a的作用域 */
    {
        /* 实例化 user_thread_a_t，用前面的类型，定义一个变量，注意这里是static的变量 */
        static user_thread_a_t thread_a;
        thread_a.param.sem = &sem;

        /* 启动thread_a任务 */
        init_vsf_thread(user_thread_a_t, &thread_a, vsf_prio_0);
    }

    while (1) {
        vsf_thread_delay_ms(1000);
        vsf_thread_sem_post(&sem);
    }
    return 0;
}
```

### 4.2 互斥量
&emsp;&emsp;相关API。具体参数和返回值，可以参考内核文档中的互斥量的章节，意义完全一样
- vsf_err_t vsf_eda_mutex_init(vsf_mutex_t *mutex);
- vsf_sync_reason_t vsf_thread_mutex_enter(vsf_mutex_t *mtx, vsf_timeout_tick_t timeout);
- vsf_err_t vsf_thread_mutex_leave(vsf_mutex_t *mtx);

```c
#include "vsf.h"

/* 申明用户thread任务类型 user_thread_a_t */
declare_vsf_thread(user_thread_a_t)

/* 定义用户任务类型 user_thread_a_t */
define_vsf_thread(user_thread_a_t, 1024,
    int id;
    vsf_mutex_t *mutex;
)

/* 实现用户定义的任务类型的任务函数 */
implement_vsf_thread(user_thread_a_t)
{
    while (1) {
        vsf_thread_mutex_enter(vsf_this.mutex, -1);
            vsf_trace_info("Hello World! from %d\r\n", vsf_this.id);
        vsf_thread_mutex_leave(vsf_this.mutex);
        vsf_thread_delay_ms(100 * (vsf_this.id + 1));
    }
}

int main(void)
{
    vsf_start_trace();

    static vsf_mutex_t mutex;
    vsf_eda_mutex_init(&mutex);

    /* 使用大括号是为了限制实例化的thread_a的作用域 */
    {
        /* 实例化 user_thread_a_t，用前面的类型，定义一个变量，注意这里是static的变量 */
        static user_thread_a_t thread_a[10];
        for (int i = 0; i < dimof(thread_a); i++) {
            thread_a[i].param.id = i;
            thread_a[i].param.mutex = &mutex;

            /* 启动thread_a任务 */
            init_vsf_thread(user_thread_a_t, &thread_a[i], vsf_prio_0);
        }
    }
    return 0;
}
```

### 4.3 临界代码段
&emsp;&emsp;使用互斥量实现。

### 4.4 触发器
&emsp;&emsp;相关API。具体参数和返回值，可以参考内核文档中的触发器的章节，意义完全一样
- vsf_err_t vsf_eda_trig_init(*vsf_trig_t *trig, bool is_set, bool is_auto_reset);
- vsf_err_t vsf_thread_trig_set(vsf_trig_t *trig);
- vsf_err_t vsf_thread_trig_set(vsf_trig_t *trig, bool is_manual_reset);
- void vsf_thread_trig_reset(vsf_trig_t *trig);
- vsf_sync_reason_t vsf_thread_trig_pend(vsf_trig_t *trig, vsf_timeout_tick_t timeout);

```c
#include "vsf.h"

/* 申明用户thread任务类型 user_thread_a_t */
declare_vsf_thread(user_thread_a_t)

/* 定义用户任务类型 user_thread_a_t */
define_vsf_thread(user_thread_a_t, 1024,
    int id;
    vsf_trig_t *trig;
)

/* 实现用户定义的任务类型的任务函数 */
implement_vsf_thread(user_thread_a_t)
{
    while (1) {
        vsf_thread_trig_pend(vsf_this.trig, -1);
        vsf_trace_info("Hello World! from %d.\r\n", vsf_this.id);
    }
}

int main(void)
{
    vsf_start_trace();

    static vsf_trig_t trig;
    vsf_eda_trig_init(&trig, false, true);

    /* 使用大括号是为了限制实例化的thread_a的作用域 */
    {
        /* 实例化 user_thread_a_t，用前面的类型，定义一个变量，注意这里是static的变量 */
        static user_thread_a_t thread_a[10];
        for (int i = 0; i < dimof(thread_a); i++) {
            thread_a[i].param.id = i;
            thread_a[i].param.trig = &trig;

            /* 启动thread_a任务 */
            init_vsf_thread(user_thread_a_t, &thread_a[i], vsf_prio_0);
        }
    }

    while (1) {
        vsf_thread_delay_ms(1000);
        vsf_thread_trig_set(&trig);
    }
    return 0;
}
```

### 4.5 队列
&emsp;&emsp;相关API。具体参数和返回值，可以参考内核文档中的队列的章节，意义完全一样
- vsf_sync_reason_t vsf_thread_queue_send(vsf_eda_queue_t *queue, void *node, vsf_timeout_tick_t timeout);
- vsf_sync_reason_t vsf_thread_queue_recv(vsf_eda_queue_t *queue, void **node, vsf_timeout_tick_t timeout);

## 5. thread任务调用其他形式的任务
TBD

## 6. thread任务的其他API
### 6.1 延时相关
- void vsf_thread_delay(vsf_systimer_tick_t tick);  -- 延时指定的tick周期
- void vsf_thread_delay_ms(uint_fast32_t ms); -- 延时指定的毫秒
- void vsf_thread_delay_us(uint_fast32_t us); -- 延时指定的微妙

### 6.2 优先级相关
&emsp;&emsp;使能动态优先级的时候（配置VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY为ENABLED），可以设置当前thread任务的优先级。
- vsf_prio_t vsf_thread_set_priority(vsf_prio_t priority);