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

/*============================ INCLUDES ======================================*/

#define __VSF_ARCH_LINUX_IMPLEMENT
#include "hal/arch/vsf_arch_abstraction.h"
#include "hal/arch/__vsf_arch_interface.h"

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <setjmp.h>
#include <sys/mman.h>

#ifdef __CPU_WEBASSEMBLY__
#   include <emscripten/emscripten.h>
#   include <emscripten/html5.h>
#   include <emscripten/eventloop.h>
#endif

/*============================ MACROS ========================================*/

#if VSF_ARCH_PRI_NUM != 1 || VSF_ARCH_SWI_NUM != 0
#   error "linux support parameter error!"
#endif

#ifndef VSF_ARCH_CFG_THREAD_NUM
#   define VSF_ARCH_CFG_THREAD_NUM          32
#endif

#ifndef VSF_ARCH_CFG_IRQ_REQUEST_NUM
#   define VSF_ARCH_CFG_IRQ_REQUEST_NUM     (VSF_ARCH_CFG_THREAD_NUM + 32)
#endif
#if VSF_ARCH_CFG_IRQ_REQUEST_NUM <= VSF_ARCH_CFG_THREAD_NUM
#   error VSF_ARCH_CFG_IRQ_REQUEST_NUM MUST be larger than VSF_ARCH_CFG_THREAD_NUM,\
        because every thread has a start_request
#endif

#ifndef VSF_ARCH_CFG_TRACE_FUNC
#   define VSF_ARCH_CFG_TRACE_FUNC          printf
#endif
#ifndef VSF_ARCH_CFG_IRQ_TRACE_EN
#   define VSF_ARCH_CFG_IRQ_TRACE_EN        DISABLED
#endif
#ifndef VSF_ARCH_CFG_REQUEST_TRACE_EN
#   define VSF_ARCH_CFG_REQUEST_TRACE_EN    DISABLED
#endif

#ifndef __CPU_WEBASSEMBLY__
#   define __VSF_ARCH_LINUX_CFG_SYSTIMER_SIGNAL     ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_ARCH_CFG_IRQ_TRACE_EN == ENABLED
#   define vsf_arch_irq_trace(...)          VSF_ARCH_CFG_TRACE_FUNC(__VA_ARGS__)
#else
#   define vsf_arch_irq_trace(...)
#endif
#if VSF_ARCH_CFG_REQUEST_TRACE_EN == ENABLED
#   define vsf_arch_request_trace(__request, __event)                           \
    do {                                                                        \
        int idx = __vsf_arch_get_thread_idx((__request)->arch_thread);          \
        if (idx >= 0) {                                                         \
            VSF_ARCH_CFG_TRACE_FUNC("irq_request%d %p " __event "\n", idx, (__request));\
        } else {                                                                \
            VSF_ARCH_CFG_TRACE_FUNC("irq_request %p " __event "\n", (__request));\
        }                                                                       \
    } while (false)
#else
#   define vsf_arch_request_trace(...)
#endif

#define __vsf_arch_crit_init(__crit)        pthread_mutex_init(&(__crit), NULL)
#define __vsf_arch_crit_enter(__crit)       pthread_mutex_lock(&(__crit))
#define __vsf_arch_crit_leave(__crit)       pthread_mutex_unlock(&(__crit))

/*============================ TYPES =========================================*/

typedef pthread_mutex_t vsf_arch_crit_t;

/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vsf_arch_create_irq_thread(vsf_arch_irq_thread_t *irq_thread, vsf_arch_irq_entry_t entry);

/*============================ INCLUDES ======================================*/

#define __VSF_ARCH_WITHOUT_THREAD_SUSPEND_TEMPLATE__
#include "hal/arch/common/arch_without_thread_suspend/vsf_arch_without_thread_suspend_template.inc"

/*============================ TYPES =========================================*/

typedef struct vsf_arch_systimer_ctx_t {
    implement(vsf_arch_irq_thread_t);
    vsf_arch_irq_request_t due_request;
#ifndef __CPU_WEBASSEMBLY__
    timer_t timer;
#endif
} vsf_arch_systimer_ctx_t;

dcl_vsf_bitmap(vsf_arch_thread_bitmap, VSF_ARCH_CFG_THREAD_NUM)

typedef struct vsf_arch_thread_t {
    pthread_t pthread;
    vsf_arch_irq_request_t start_request;

    void *param;
} vsf_arch_thread_t;

dcl_vsf_bitmap(vsf_arch_irq_request_bitmap, VSF_ARCH_CFG_IRQ_REQUEST_NUM)

typedef struct vsf_arch_irq_request_priv_t {
    pthread_cond_t cond;
    pthread_mutex_t mutex;
} vsf_arch_irq_request_priv_t;

typedef struct vsf_arch_t {
    struct {
        vsf_arch_thread_t pool[VSF_ARCH_CFG_THREAD_NUM];
        vsf_bitmap(vsf_arch_thread_bitmap) bitmap;
    } thread;
    struct {
        vsf_arch_irq_request_priv_t pool[VSF_ARCH_CFG_IRQ_REQUEST_NUM];
        vsf_bitmap(vsf_arch_irq_request_bitmap) bitmap;
    } irq_request;

    vsf_arch_systimer_ctx_t systimer;
} vsf_arch_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static VSF_CAL_NO_INIT vsf_arch_t __vsf_arch;

/*============================ IMPLEMENTATION ================================*/

static int __vsf_arch_get_thread_idx(vsf_arch_thread_t *thread)
{
    int idx = thread - __vsf_arch.thread.pool;
    if ((idx >= 0) && (idx <= dimof(__vsf_arch.thread.pool))) {
        return idx;
    }
    return -1;
}

/*----------------------------------------------------------------------------*
 * infrastructure                                                             *
 *----------------------------------------------------------------------------*/

void __vsf_arch_irq_request_init(vsf_arch_irq_request_t *request)
{
    VSF_HAL_ASSERT(!request->is_inited);

    __vsf_arch_crit_enter(__vsf_arch_common.lock);
        request->id = vsf_bitmap_ffz(&__vsf_arch.irq_request.bitmap, VSF_ARCH_CFG_IRQ_REQUEST_NUM);
        if (request->id >= 0) {
            vsf_bitmap_set(&__vsf_arch.irq_request.bitmap, request->id);
        }
    __vsf_arch_crit_leave(__vsf_arch_common.lock);
    VSF_ARCH_ASSERT(request->id >= 0);

    pthread_cond_t *cond = &__vsf_arch.irq_request.pool[request->id].cond;
    pthread_mutex_t *mutex = &__vsf_arch.irq_request.pool[request->id].mutex;

    request->is_triggered = false;
    pthread_mutex_init(mutex, NULL);
    pthread_cond_init(cond, NULL);
    request->is_inited = true;
}

void __vsf_arch_irq_request_fini(vsf_arch_irq_request_t *request)
{
    VSF_HAL_ASSERT(request->is_inited);
    pthread_cond_t *cond = &__vsf_arch.irq_request.pool[request->id].cond;
    pthread_mutex_t *mutex = &__vsf_arch.irq_request.pool[request->id].mutex;

    pthread_cond_destroy(cond);
    pthread_mutex_destroy(mutex);

    __vsf_arch_crit_enter(__vsf_arch_common.lock);
        vsf_bitmap_clear(&__vsf_arch.irq_request.bitmap, request->id);
    __vsf_arch_crit_leave(__vsf_arch_common.lock);

    request->is_inited = false;
    request->is_triggered = false;
}

void __vsf_arch_irq_request_pend(vsf_arch_irq_request_t *request)
{
    VSF_HAL_ASSERT(request->is_inited);
    pthread_cond_t *cond = &__vsf_arch.irq_request.pool[request->id].cond;
    pthread_mutex_t *mutex = &__vsf_arch.irq_request.pool[request->id].mutex;

    vsf_arch_request_trace(request, "pend");
    pthread_mutex_lock(mutex);
        while (!request->is_triggered) {
            vsf_arch_request_trace(request, "wait");
            pthread_cond_wait(cond, mutex);
        }
        request->is_triggered = false;
    pthread_mutex_unlock(mutex);
    vsf_arch_request_trace(request, "got");

    vsf_arch_irq_thread_t *irq_thread = request->irq_thread_to_exit;
    if (irq_thread != NULL) {
        request->irq_thread_to_exit = NULL;
        __vsf_arch_irq_exit(irq_thread);
    }
}

void __vsf_arch_irq_request_send(vsf_arch_irq_request_t *request)
{
    VSF_HAL_ASSERT(request->is_inited);
    pthread_cond_t *cond = &__vsf_arch.irq_request.pool[request->id].cond;
    pthread_mutex_t *mutex = &__vsf_arch.irq_request.pool[request->id].mutex;

    vsf_arch_request_trace(request, "send");
    pthread_mutex_lock(mutex);
        request->is_triggered = true;
        pthread_cond_signal(cond);
        vsf_arch_request_trace(request, "signal");
    pthread_mutex_unlock(mutex);
    vsf_arch_request_trace(request, "sent");
}

static void * __vsf_arch_irq_entry(void *arg)
{
    vsf_arch_thread_t *thread = arg;
    int idx = __vsf_arch_get_thread_idx(thread);
    jmp_buf exit;

    thread->start_request.arch_thread = thread;

    while (true) {
        __vsf_arch_irq_request_pend(&thread->start_request);

        vsf_arch_irq_thread_t *irq_thread = thread->param;
        vsf_arch_irq_trace("irq_thread_start %s\n", irq_thread->name);

        irq_thread->exit = (void *)&exit;
        if (!setjmp(exit)) {
            if (irq_thread->entry != NULL) {
                irq_thread->entry(irq_thread);
            }
        }
        irq_thread->exit = NULL;

        if (irq_thread->is_to_restart) {
            irq_thread->is_to_restart = false;
        } else {
            __vsf_arch_crit_enter(__vsf_arch_common.lock);
                vsf_bitmap_clear(&__vsf_arch.thread.bitmap, idx);
            __vsf_arch_crit_leave(__vsf_arch_common.lock);
        }
    }

    pthread_detach(pthread_self());
    return NULL;
}

static vsf_err_t __vsf_arch_create_irq_thread(vsf_arch_irq_thread_t *irq_thread, vsf_arch_irq_entry_t entry)
{
    vsf_arch_thread_t *thread;
    int idx;

    __vsf_arch_crit_enter(__vsf_arch_common.lock);
        idx = vsf_bitmap_ffz(&__vsf_arch.thread.bitmap, VSF_ARCH_CFG_THREAD_NUM);
        if (idx >= 0) {
            vsf_bitmap_set(&__vsf_arch.thread.bitmap, idx);
        }
    __vsf_arch_crit_leave(__vsf_arch_common.lock);

    if (idx >= 0) {
        thread = &__vsf_arch.thread.pool[idx];
        thread->param = irq_thread;
        irq_thread->entry = entry;
        irq_thread->arch_thread = thread;
        vsf_arch_irq_trace("irq_thread_init %s\n", irq_thread->name);
        __vsf_arch_irq_request_send(&thread->start_request);
        return VSF_ERR_NONE;
    }
    VSF_ARCH_ASSERT(false);
    return VSF_ERR_NOT_ENOUGH_RESOURCES;
}

void __vsf_arch_irq_exit(vsf_arch_irq_thread_t *irq_thread)
{
    VSF_ARCH_ASSERT((irq_thread != NULL) && (irq_thread->exit != NULL));
    longjmp(*(jmp_buf *)(irq_thread->exit), -1);
}

vsf_err_t __vsf_arch_irq_restart(vsf_arch_irq_thread_t *irq_thread,
                    vsf_arch_irq_request_t *request_pending)
{
    vsf_arch_thread_t *thread = irq_thread->arch_thread;

    irq_thread->is_to_restart = true;
    request_pending->irq_thread_to_exit = irq_thread;
    __vsf_arch_irq_request_send(request_pending);
    __vsf_arch_irq_request_send(&thread->start_request);
    return VSF_ERR_NONE;
}

void __vsf_arch_irq_sleep(uint32_t ms)
{
    usleep(ms * 1000);
}

/*----------------------------------------------------------------------------*
 * Systimer Timer Implementation                                              *
 *----------------------------------------------------------------------------*/

#if VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_REQUEST_RESPONSE

#if __VSF_ARCH_LINUX_CFG_SYSTIMER_SIGNAL == ENABLED
static void __vsf_systimer_on_notify(int signal)
#elif defined(__CPU_WEBASSEMBLY__)
static void __vsf_systimer_on_notify(void *param)
#else
static void __vsf_systimer_on_notify(union sigval s)
#endif
{
    vsf_arch_systimer_ctx_t *ctx = &__vsf_arch.systimer;
    __vsf_arch_irq_request_send(&__vsf_arch.systimer.due_request);
}

static void __vsf_systimer_thread(void *arg)
{
    vsf_arch_systimer_ctx_t *ctx = arg;

    __vsf_arch_irq_set_background(&ctx->use_as__vsf_arch_irq_thread_t);

    while (1) {
        __vsf_arch_irq_request_pend(&__vsf_arch.systimer.due_request);

        __vsf_arch_irq_start(&ctx->use_as__vsf_arch_irq_thread_t);
            vsf_systimer_tick_t tick = vsf_systimer_get();
            vsf_systimer_timeout_evt_handler(tick);
        __vsf_arch_irq_end(&ctx->use_as__vsf_arch_irq_thread_t, false);
    }
    __vsf_arch_irq_fini(&ctx->use_as__vsf_arch_irq_thread_t);
}

/*! \brief initialise SysTick to generate a system timer
 *! \param frequency the target tick frequency in Hz
 *! \return initialization result in vsf_err_t
 */
vsf_err_t vsf_systimer_init(void)
{
    vsf_arch_systimer_ctx_t *ctx = &__vsf_arch.systimer;
#ifndef __CPU_WEBASSEMBLY__
    struct sigevent evp = {
#   if __VSF_ARCH_LINUX_CFG_SYSTIMER_SIGNAL == ENABLED
        .sigev_notify = SIGEV_SIGNAL,
        .sigev_signo = SIGUSR1,
#   else
        .sigev_notify = SIGEV_THREAD,
        .sigev_notify_function = __vsf_systimer_on_notify,
#   endif
    };
#   if __VSF_ARCH_LINUX_CFG_SYSTIMER_SIGNAL == ENABLED
    signal(SIGUSR1, __vsf_systimer_on_notify);
#   endif
    if (timer_create(CLOCK_MONOTONIC, &evp, &ctx->timer)) {
        VSF_HAL_ASSERT(false);
        return VSF_ERR_FAIL;
    }
#endif
    __vsf_arch_irq_request_init(&__vsf_arch.systimer.due_request);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_systimer_start(void)
{
    __vsf_arch_irq_init(&__vsf_arch.systimer.use_as__vsf_arch_irq_thread_t,
                "timer", __vsf_systimer_thread, vsf_arch_prio_0);
    __vsf_arch.systimer.due_request.arch_thread = __vsf_arch.systimer.arch_thread;
    return VSF_ERR_NONE;
}

void vsf_systimer_set_idle(void)
{
}

vsf_systimer_tick_t vsf_systimer_get(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

bool vsf_systimer_set(vsf_systimer_tick_t due)
{
#ifdef __CPU_WEBASSEMBLY__
    vsf_systimer_tick_t cur = vsf_systimer_get();
    if (due > cur) {
        vsf_systimer_tick_t diff = due - cur;
        emscripten_set_timeout(__vsf_systimer_on_notify, diff / 1000.0, NULL);
        return true;
    }
    return false;
#else
    struct itimerspec its = { 0 };
    struct timespec ts;
    ts.tv_sec = due / 1000000;
    ts.tv_nsec = (due % 1000000) * 1000;
    its.it_value = ts;
    timer_settime(__vsf_arch.systimer.timer, TIMER_ABSTIME, &its, NULL);
    return true;
#endif
}

bool vsf_systimer_is_due(vsf_systimer_tick_t due)
{
    return (vsf_systimer_get() >= due);
}

vsf_systimer_tick_t vsf_systimer_us_to_tick(uint_fast32_t time_us)
{
    return (vsf_systimer_tick_t)time_us;
}

vsf_systimer_tick_t vsf_systimer_ms_to_tick(uint_fast32_t time_ms)
{
    return (vsf_systimer_tick_t)(time_ms * 1000);
}

vsf_systimer_tick_t vsf_systimer_tick_to_us(vsf_systimer_tick_t tick)
{
    return tick;
}

vsf_systimer_tick_t vsf_systimer_tick_to_ms(vsf_systimer_tick_t tick)
{
    return tick / 1000;
}

void vsf_systimer_prio_set(vsf_arch_prio_t priority)
{

}

#endif

/*! \note initialize architecture specific service
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
bool vsf_arch_low_level_init(void)
{
    memset(&__vsf_arch, 0, sizeof(__vsf_arch));
    strcpy((char *)__vsf_arch_common.por_thread.name, "por");

    // create thread pool
    vsf_bitmap_reset(&__vsf_arch.irq_request.bitmap, VSF_ARCH_CFG_IRQ_REQUEST_NUM);
    vsf_bitmap_reset(&__vsf_arch.thread.bitmap, VSF_ARCH_CFG_THREAD_NUM);

    // __vsf_arch_low_level_init MUST be called before using __vsf_arch_common.lock
    __vsf_arch_low_level_init();

    for (int i = 0; i < dimof(__vsf_arch.thread.pool); i++) {
        __vsf_arch_irq_request_init(&__vsf_arch.thread.pool[i].start_request);
        if (0 != pthread_create(&__vsf_arch.thread.pool[i].pthread, NULL, __vsf_arch_irq_entry, &__vsf_arch.thread.pool[i])) {
            VSF_HAL_ASSERT(false);
            return false;
        }
    }
    return true;
}

#ifdef __CPU_WEBASSEMBLY__
static EM_BOOL __vsf_arch_one_iter(double time, void* userData)
{
    return EM_TRUE;
}

static void __vsf_arch_main_loop(void)
{

}

void vsf_arch_main(void)
{
    emscripten_set_main_loop(__vsf_arch_main_loop, 0, 0);
    emscripten_exit_with_live_runtime();
}
#endif

/*----------------------------------------------------------------------------*
 * Heap Implementation                                                        *
 *----------------------------------------------------------------------------*/

#if VSF_ARCH_PROVIDE_HEAP == ENABLED
typedef struct vsf_arch_heap_mcb_t {
    void *ptr;
    uint_fast32_t alignment;
    uint_fast32_t size;             // size of memory allocated, smaller than memory_size
    uint_fast32_t memory_size;      // total memory size
} vsf_arch_heap_mcb_t;

void * vsf_arch_heap_malloc(uint_fast32_t size, uint_fast32_t alignment)
{
    void *buffer, *aligned_buffer;
    vsf_arch_heap_mcb_t *mcb;
    uint_fast32_t offset;

    if (!alignment) {
        alignment = 1;
    }
    offset = alignment - 1 + sizeof(vsf_arch_heap_mcb_t);
    if ((buffer = malloc(size + offset)) == NULL) {
        return NULL;
    }

    aligned_buffer = (void *)(((uintptr_t)buffer + offset) & ~(alignment - 1));
    mcb = &((vsf_arch_heap_mcb_t *)aligned_buffer)[-1];
    mcb->ptr = buffer;
    mcb->alignment = alignment;
    mcb->size = size;
    mcb->memory_size = size;
    return aligned_buffer;
}

void * vsf_arch_heap_realloc(void *buffer, uint_fast32_t size)
{
    if (NULL == buffer) {
        return vsf_arch_heap_malloc(size, 0);
    }

    vsf_arch_heap_mcb_t *mcb = &((vsf_arch_heap_mcb_t *)buffer)[-1];

    if (mcb->memory_size >= size) {
        mcb->size = size;
        return buffer;
    }

    uint32_t *new_buffer = vsf_arch_heap_malloc(size, mcb->alignment);
    if (NULL == new_buffer) {
        vsf_arch_heap_free(buffer);
        return NULL;
    }

    size = vsf_min(size, mcb->size);
    memcpy(new_buffer, buffer, size);
    vsf_arch_heap_free(buffer);
    return new_buffer;
}

void vsf_arch_heap_free(void *buffer)
{
    vsf_arch_heap_mcb_t *mcb = &((vsf_arch_heap_mcb_t *)buffer)[-1];
    free(mcb->ptr);
}

unsigned int vsf_arch_heap_alignment(void)
{
    return 16;
}

uint_fast32_t vsf_arch_heap_size(void *buffer)
{
    vsf_arch_heap_mcb_t *mcb = &((vsf_arch_heap_mcb_t *)buffer)[-1];
    return mcb->memory_size;
}
#endif

/*----------------------------------------------------------------------------*
 * Execute                                                                    *
 *----------------------------------------------------------------------------*/

void * vsf_arch_alloc_exe(uint_fast32_t size)
{
    return mmap(0, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

void vsf_arch_free_exe(void *ptr)
{
    munmap(ptr, 0);
}

/*----------------------------------------------------------------------------*
 * Argument                                                                   *
 *----------------------------------------------------------------------------*/

int vsf_arch_argu(char ***argv)
{
    static char *__vsf_arch_argv_str, **__vsf_arch_argv;
    static int __vsf_arch_argc;

    if (__vsf_arch_argv != NULL) {
        if (argv != NULL) {
            *argv = __vsf_arch_argv;
        }
        return __vsf_arch_argc;
    }

    int fd = open("/proc/self/cmdline", 0);
    if (fd < 0) {
        return -1;
    }

    int pos = 0, ret;
    while(true) {
        __vsf_arch_argv_str = realloc(__vsf_arch_argv_str, pos + 1024);
        ret = read(fd, __vsf_arch_argv_str + pos, 1024);
        if (ret < 0) {
            break;
        }
        pos += ret;
        if (ret < 1024) {
            break;
        }
    }

    for (int i = 0; i < pos; i++) {
        if (__vsf_arch_argv_str[i] == '\0') {
            __vsf_arch_argc++;
        }
    }
    VSF_ARCH_ASSERT(__vsf_arch_argc > 0);
    __vsf_arch_argv = malloc(__vsf_arch_argc * sizeof(char *));
    VSF_ARCH_ASSERT(__vsf_arch_argv != NULL);

    for (int i = 0, argv_pos = 0; i < pos; i++) {
        if (    !argv_pos
            ||  (   (__vsf_arch_argv_str[i] != '\0')
                &&  (__vsf_arch_argv_str[i - 1] == '\0'))) {
            __vsf_arch_argv[argv_pos++] = &__vsf_arch_argv_str[i];
        }
    }

    return vsf_arch_argu(argv);
}

void vsf_arch_shutdown(void)
{
    exit(0);
}

void vsf_arch_reset(void)
{
    // dedicated arch should implement this
    VSF_ARCH_ASSERT(false);
}

/* EOF */
