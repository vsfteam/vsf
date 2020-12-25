/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

#define __VSF_ARCH_RTOS_MODEL_IMPLEMENT
#include "hal/arch/vsf_arch_abstraction.h"
#include "hal/arch/__vsf_arch_interface.h"

#include "FreeRTOS.h"
#include "event_groups.h"
#include "timers.h"

/*============================ MACROS ========================================*/

#if !configSUPPORT_STATIC_ALLOCATION
// stack is maybe provided from upper layer,
//  so xTaskCreateStatic MUST be used to create task, which accept static stack.
#   error configSUPPORT_STATIC_ALLOCATION MUST be enabled
#endif
#if !configSUPPORT_DYNAMIC_ALLOCATION
#   error configSUPPORT_DYNAMIC_ALLOCATION MUST be enabled
#endif

#ifndef VSF_ARCH_RTOS_CFG_BASE_PRIORITY
#   define VSF_ARCH_RTOS_CFG_BASE_PRIORITY      1
#endif

// skip tskIDLE_PRIORITY(0) and main priority(tskIDLE_PRIORITY + VSF_ARCH_RTOS_CFG_BASE_PRIORITY)
#define arch_prio_to_rtos_prio(__arch_prio)     ((__arch_prio) + VSF_ARCH_RTOS_CFG_BASE_PRIORITY + 1)

#if VSF_ARCH_RTOS_CFG_MODE == VSF_ARCH_RTOS_MODE_SUSPEND_RESUME
#   define rtos_prio_to_arch_prio(__rtos_prio)  ((__rtos_prio) - (VSF_ARCH_RTOS_CFG_BASE_PRIORITY + 1))
#endif

#ifndef VSF_ARCH_RTOS_CFG_MAIN_STACK_DEPTH
#   define VSF_ARCH_RTOS_CFG_MAIN_STACK_DEPTH   VSF_ARCH_RTOS_CFG_STACK_DEPTH
#endif

#ifndef VSF_ARCH_FREERTOS_CFG_IS_IN_ISR
#   error VSF_ARCH_FREERTOS_CFG_IS_IN_ISR MUST be defined
#endif

#ifndef VSF_ARCH_FREERTOS_CFG_IRQ_DEPTH
#   define VSF_ARCH_FREERTOS_CFG_IRQ_DEPTH      8
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct __vsf_freertos_t {
    struct {
        TimerHandle_t               handle;
        vsf_systimer_cnt_t          due;
    } timer;
    struct {
        BaseType_t                  stack[VSF_ARCH_FREERTOS_CFG_IRQ_DEPTH];
        int8_t                      pos;
    } irq;
    struct {
        vsf_gint_state_t            state;
        UBaseType_t                 os_state;
    } gint;
} __vsf_freertos_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static NO_INIT __vsf_freertos_t __vsf_freertos;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*----------------------------------------------------------------------------*
 * interrupt                                                                  *
 *----------------------------------------------------------------------------*/

vsf_gint_state_t vsf_get_interrupt(void)
{
    return __vsf_freertos.gint.state;
}

void vsf_set_interrupt(vsf_gint_state_t level)
{
    if (__vsf_freertos.gint.state != level) {
        if (level) {
            __vsf_freertos.gint.state = level;
            taskEXIT_CRITICAL_FROM_ISR(__vsf_freertos.gint.os_state);
        } else {
            __vsf_freertos.gint.os_state = taskENTER_CRITICAL_FROM_ISR();
            __vsf_freertos.gint.state = level;
        }
    }
}

vsf_gint_state_t vsf_disable_interrupt(void)
{
    vsf_gint_state_t orig = vsf_get_interrupt();
    vsf_set_interrupt(false);
    return orig;
}

void vsf_enable_interrupt(void)
{
    vsf_set_interrupt(true);
}

/*----------------------------------------------------------------------------*
 * irq_ctx                                                                    *
 *----------------------------------------------------------------------------*/

void __vsf_arch_irq_enter(void)
{
    int_fast8_t pos;
    vsf_gint_state_t orig = vsf_disable_interrupt();
        pos = ++__vsf_freertos.irq.pos;
        if (pos >= dimof(__vsf_freertos.irq.stack)) {
            VSF_ARCH_ASSERT(false);
        }
    vsf_set_interrupt(orig);

    __vsf_freertos.irq.stack[pos] = pdFALSE;
}

void __vsf_arch_irq_leave(void)
{
    BaseType_t is_to_switch;

    vsf_gint_state_t orig = vsf_disable_interrupt();
        is_to_switch = __vsf_freertos.irq.stack[__vsf_freertos.irq.pos--];
    vsf_set_interrupt(orig);

    if (is_to_switch) {
        portYIELD_FROM_ISR(is_to_switch);
    }
}

/*----------------------------------------------------------------------------*
 * irq_request                                                                *
 *----------------------------------------------------------------------------*/

void __vsf_arch_irq_request_init(vsf_arch_irq_request_t *request, bool is_auto_reset)
{
    request->is_auto_reset = is_auto_reset;
    request->event = xEventGroupCreate();
}

void __vsf_arch_irq_request_fini(vsf_arch_irq_request_t *request)
{
    vEventGroupDelete(request->event);
    request->event = NULL;
}

void __vsf_arch_irq_request_pend(vsf_arch_irq_request_t *request)
{
    BaseType_t is_to_reset = request->is_auto_reset ? pdTRUE : pdFALSE;
    EventBits_t bits;

    do {
        bits = xEventGroupWaitBits(request->event, 1, is_to_reset, pdFALSE, portMAX_DELAY);
    } while (!(bits & 1));
    request->is_set_pending = false;
}

void __vsf_arch_irq_request_send(vsf_arch_irq_request_t *request)
{
    if (VSF_ARCH_FREERTOS_CFG_IS_IN_ISR()) {
        // avoid xEventGroupSetBitsFromISR to be called too many times in isr
        // because it will take a node in timer queue
        if (!request->is_set_pending) {
            request->is_set_pending = true;

            BaseType_t ret = xEventGroupSetBitsFromISR(request->event, 1, &__vsf_freertos.irq.stack[__vsf_freertos.irq.pos]);
            VSF_ARCH_ASSERT(ret == pdPASS);
        }
    } else {
        xEventGroupSetBits(request->event, 1);
    }
}

void __vsf_arch_irq_request_reset(vsf_arch_irq_request_t *request)
{
    VSF_ARCH_ASSERT(!request->is_auto_reset);
    xEventGroupClearBits(request->event, 1);
}

/*----------------------------------------------------------------------------*
 * irq_thread                                                                 *
 *----------------------------------------------------------------------------*/

void __vsf_arch_irq_thread_start(vsf_arch_irq_thread_t *irq_thread,
        const char * const name, vsf_arch_irq_thread_entry_t entry, vsf_arch_prio_t priority,
        VSF_ARCH_RTOS_STACK_T *stack, uint_fast32_t stack_depth)
{
    UBaseType_t rtos_priority;
    if (priority < 0) {
        rtos_priority = uxTaskPriorityGet(xTaskGetCurrentTaskHandle());
    } else {
        rtos_priority = arch_prio_to_rtos_prio(priority);
    }

    // because dynamic xTaskCreate does not support stack from user, so xTaskCreateStatic is sued
    // and part of the stack is used as StaticTask_t.
#ifdef VSF_ARCH_CFG_STACK_ALIGN_BIT
    VSF_ARCH_ASSERT(!((1 << VSF_ARCH_CFG_STACK_ALIGN_BIT) % sizeof(StackType_t)));
    VSF_ARCH_ASSERT(!((uintptr_t)stack & ((1 << VSF_ARCH_CFG_STACK_ALIGN_BIT) - 1)));
    uint_fast32_t reserved_stack_depth = sizeof(StaticTask_t) + (1 << VSF_ARCH_CFG_STACK_ALIGN_BIT) - 1;
    reserved_stack_depth &= ~((1 << VSF_ARCH_CFG_STACK_ALIGN_BIT) - 1);
#else
    uint_fast32_t reserved_stack_depth = sizeof(StaticTask_t) + sizeof(StackType_t) - 1;
    if (sizeof(StackType_t) < sizeof(VSF_ARCH_RTOS_STACK_T)) {
        reserved_stack_depth += sizeof(VSF_ARCH_RTOS_STACK_T) - 1;
        reserved_stack_depth &= ~(sizeof(VSF_ARCH_RTOS_STACK_T) - 1);
    }
#endif

    // if assert here, modify VSF_ARCH_RTOS_STACK_T in freerots_generic.h to fit StackType_t
    VSF_ARCH_ASSERT(sizeof(VSF_ARCH_RTOS_STACK_T) >= sizeof(StackType_t));
    VSF_ARCH_ASSERT((stack != NULL) && (stack_depth * sizeof(VSF_ARCH_RTOS_STACK_T) > reserved_stack_depth));
    StaticTask_t *static_task = (StaticTask_t *)stack;

    reserved_stack_depth /= sizeof(StackType_t);
    stack_depth = stack_depth * sizeof(VSF_ARCH_RTOS_STACK_T) / sizeof(StackType_t) - reserved_stack_depth;
    stack = (VSF_ARCH_RTOS_STACK_T *)((uintptr_t)stack + reserved_stack_depth);

    // if thread is restarted, delete first
    if (irq_thread->thread_handle != NULL) {
        vTaskDelete(irq_thread->thread_handle);
    }
    irq_thread->thread_handle = xTaskCreateStatic(entry, name, stack_depth, irq_thread,
        rtos_priority, (StackType_t *)stack, static_task);
    VSF_ARCH_ASSERT(irq_thread->thread_handle != NULL);
}

void __vsf_arch_irq_thread_exit(void)
{
    vTaskDelete(NULL);
}

void __vsf_arch_irq_thread_set_priority(vsf_arch_irq_thread_t *irq_thread, vsf_arch_prio_t priority)
{
    vTaskPrioritySet(irq_thread->thread_handle, arch_prio_to_rtos_prio(priority));
}

#if VSF_ARCH_RTOS_CFG_MODE == VSF_ARCH_RTOS_MODE_SUSPEND_RESUME
void __vsf_arch_irq_thread_suspend(vsf_arch_irq_thread_t *irq_thread)
{
    vTaskSuspend(irq_thread->thread_handle);
}

void __vsf_arch_irq_thread_resume(vsf_arch_irq_thread_t *irq_thread)
{
    vTaskResume(irq_thread->thread_handle);
}

vsf_arch_prio_t __vsf_arch_irq_thread_get_priority(vsf_arch_irq_thread_t *irq_thread)
{
    UBaseType_t rtos_priority = uxTaskPriorityGet(irq_thread->thread_handle);
    return (vsf_arch_prio_t)rtos_prio_to_arch_prio(rtos_priority);
}
#endif

/*----------------------------------------------------------------------------*
 * Infrastructure                                                             *
 *----------------------------------------------------------------------------*/
/*! \note initialize architecture specific service
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
bool __vsf_arch_model_low_level_init(void)
{
    __vsf_freertos.irq.pos = -1;
    return true;
}

void __vsf_arch_delay_ms(uint_fast32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

#if VSF_ARCH_RTOS_CFG_MODE == VSF_ARCH_RTOS_MODE_SUSPEND_RESUME
vsf_arch_prio_t __vsf_arch_model_get_current_priority(void)
{
    UBaseType_t rtos_priority = uxTaskPriorityGet(xTaskGetCurrentTaskHandle());
    return (vsf_arch_prio_t)rtos_prio_to_arch_prio(rtos_priority);
}
#endif

/*----------------------------------------------------------------------------*
 * System Timer Implementation                                                *
 *----------------------------------------------------------------------------*/

#if VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_REQUEST_RESPONSE

vsf_systimer_cnt_t vsf_systimer_get(void)
{
    return xTaskGetTickCount();
}

static void __vsf_systimer_callback(TimerHandle_t xTimer)
{
    vsf_systimer_timeout_evt_hanlder(vsf_systimer_get());
}

/*! \brief initialise SysTick to generate a system timer
 *! \param frequency the target tick frequency in Hz
 *! \return initialization result in vsf_err_t
 */
vsf_err_t vsf_systimer_init(void)
{
    // if assert here, modify vsf_systimer_cnt_t in freerots_generic.h to fit TickType_t
    VSF_ARCH_ASSERT(sizeof(vsf_systimer_cnt_t) == sizeof(TickType_t));
    __vsf_freertos.timer.handle = xTimerCreate("vsf_timer", 1, pdFALSE,
            NULL, __vsf_systimer_callback);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_systimer_start(void)
{
    return VSF_ERR_NONE;
}

void vsf_systimer_set_idle(void)
{

}

bool vsf_systimer_set(vsf_systimer_cnt_t due)
{
    vsf_systimer_cnt_t cur = vsf_systimer_get();

    if (due > cur) {
        vsf_systimer_cnt_t diff = due - cur;
        BaseType_t ret = xTimerChangePeriod(__vsf_freertos.timer.handle, diff, 0);
        VSF_ARCH_ASSERT(pdPASS == ret);
        return true;
    }
    return false;
}

bool vsf_systimer_is_due(vsf_systimer_cnt_t due)
{
    return vsf_systimer_get() >= due;
}

vsf_systimer_cnt_t vsf_systimer_us_to_tick(uint_fast32_t time_us)
{
    // not supported by freertos
    VSF_ARCH_ASSERT(false);
    return 0;
}

vsf_systimer_cnt_t vsf_systimer_ms_to_tick(uint_fast32_t time_ms)
{
    vsf_systimer_cnt_t tick = pdMS_TO_TICKS(time_ms);
    // if assert here, try increase configTICK_RATE_HZ in freertos configuration
    VSF_ARCH_ASSERT(tick != 0);
    return tick;
}

uint_fast32_t vsf_systimer_tick_to_us(vsf_systimer_cnt_t tick)
{
    // not supported by freertos
    VSF_ARCH_ASSERT(false);
    return 0;
}

uint_fast32_t vsf_systimer_tick_to_ms(vsf_systimer_cnt_t tick)
{
    return tick * portTICK_PERIOD_MS;
}

void vsf_systimer_prio_set(vsf_arch_prio_t priority)
{
    // DO NOT ASSERT here
}

/*----------------------------------------------------------------------------*
 * FreeRTOS                                                                   *
 *----------------------------------------------------------------------------*/

static void __vsf_main_task(void *param)
{
    extern void __vsf_main_entry(void);
    __vsf_main_entry();
}

void vsf_freertos_start(void)
{
    xTaskCreate(        __vsf_main_task,                        /* The function that implements the task being created. */
                        "vsf_main",                             /* Text name for the task - not used by the RTOS, its just to assist debugging. */
                        VSF_ARCH_RTOS_CFG_MAIN_STACK_DEPTH,     /* Size of the buffer passed in as the stack - in words, not bytes! */
                        NULL,                                   /* Parameter passed into the task - not used in this case. */
                        tskIDLE_PRIORITY + VSF_ARCH_RTOS_CFG_BASE_PRIORITY,
                                                                /* Priority of the task. */
                        NULL);                                  /* The pointer to the task_id. */
}

WEAK(vAssertCalled)
void vAssertCalled(unsigned long ulLine, const char * const pcFileName)
{
    VSF_ARCH_ASSERT(false);
}

WEAK(vApplicationMallocFailedHook)
void vApplicationMallocFailedHook(void)
{
    VSF_ARCH_ASSERT(false);
}

WEAK(vApplicationGetIdleTaskMemory)
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

WEAK(vApplicationGetTimerTaskMemory)
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

WEAK(vApplicationIdleHook)
void vApplicationIdleHook(void)
{
}

WEAK(vApplicationTickHook)
void vApplicationTickHook(void)
{
}

WEAK(vApplicationDaemonTaskStartupHook)
void vApplicationDaemonTaskStartupHook(void)
{
}

#endif

/* EOF */
