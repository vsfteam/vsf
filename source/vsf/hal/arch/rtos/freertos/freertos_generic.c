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

/*============================ MACROS ========================================*/

// skip tskIDLE_PRIORITY(0) and main priority(tskIDLE_PRIORITY + 1)
#define arch_prio_to_rtos_prio(__arch_prio)     ((__arch_prio) + 2)

/*============================ MACROFIED FUNCTIONS ===========================*/

#ifndef VSF_ARCH_ASSERT
#   define VSF_ARCH_ASSERT(...)                 ASSERT(__VA_ARGS__)
#endif

/*============================ TYPES =========================================*/

typedef struct __vsf_freertos_t {
    struct {
        TimerHandle_t               handle;
#if configSUPPORT_STATIC_ALLOCATION
        StaticTimer_t               static_timer;
#endif
        vsf_systimer_cnt_t          due;
        bool                        is_running;
    } timer;
#if configSUPPORT_STATIC_ALLOCATION
    struct {
        StaticTask_t                task;
        StackType_t                 stack[VSF_ARCH_RTOS_CFG_STACK_DEPTH];
    } main;
#endif
} __vsf_freertos_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static NO_INIT __vsf_freertos_t __vsf_freertos;
// use a standalone variable for better optimization if interrupt part is over-written
static vsf_gint_state_t __gint_state = true;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*----------------------------------------------------------------------------*
 * interrupt                                                                  *
 *----------------------------------------------------------------------------*/

WEAK(vsf_get_interrupt)
vsf_gint_state_t vsf_get_interrupt(void)
{
    return __gint_state;
}

WEAK(vsf_set_interrupt)
void vsf_set_interrupt(vsf_gint_state_t level)
{
    if (__gint_state != level) {
        if (level) {
            __gint_state = level;
            portENABLE_INTERRUPTS();
        } else {
            portDISABLE_INTERRUPTS();
            __gint_state = level;
        }
    }
}

WEAK(vsf_disable_interrupt)
vsf_gint_state_t vsf_disable_interrupt(void)
{
    vsf_gint_state_t orig = vsf_get_interrupt();
    vsf_set_interrupt(false);
    return orig;
}

WEAK(vsf_enable_interrupt)
void vsf_enable_interrupt(void)
{
    vsf_set_interrupt(true);
}

/*----------------------------------------------------------------------------*
 * irq_request                                                                *
 *----------------------------------------------------------------------------*/
void __vsf_arch_irq_request_init(vsf_arch_irq_request_t *request)
{
#if configSUPPORT_STATIC_ALLOCATION
    request->event = xEventGroupCreateStatic(&request->static_event);
#else
    request->event = xEventGroupCreate();
#endif
}

void __vsf_arch_irq_request_fini(vsf_arch_irq_request_t *request)
{
    vEventGroupDelete(request->event);
    request->event = NULL;
}

void __vsf_arch_irq_request_pend(vsf_arch_irq_request_t *request)
{
    xEventGroupWaitBits(request->event, 1, pdTRUE, pdFALSE, -1);
}

void __vsf_arch_irq_request_send(vsf_arch_irq_request_t *request)
{
    xEventGroupSetBits(request->event, 1);
}

/*----------------------------------------------------------------------------*
 * irq_thread                                                                 *
 *----------------------------------------------------------------------------*/
void __vsf_arch_irq_thread_start(vsf_arch_irq_thread_t *irq_thread, vsf_arch_irq_thread_entry_t entry, vsf_arch_prio_t priority)
{
#if configSUPPORT_STATIC_ALLOCATION
    irq_thread->thread = xTaskCreateStatic(entry, "irq_thread", dimof(irq_thread->stack), irq_thread,
        arch_prio_to_rtos_prio(priority), irq_thread->stack, &irq_thread->static_task);
#else
    xTaskCreate(entry, "irq_thread", VSF_ARCH_RTOS_CFG_STACK_DEPTH, irq_thread,
        arch_prio_to_rtos_prio(priority), &irq_thread->thread);
#endif
}

void __vsf_arch_irq_thread_set_priority(vsf_arch_irq_thread_t *irq_thread, vsf_arch_prio_t priority)
{
    vTaskPrioritySet(irq_thread->thread, arch_prio_to_rtos_prio(priority));
}

void __vsf_arch_irq_thread_suspend(vsf_arch_irq_thread_t *irq_thread)
{
    vTaskSuspend(irq_thread->thread);
}

void __vsf_arch_irq_thread_resume(vsf_arch_irq_thread_t *irq_thread)
{
    vTaskResume(irq_thread->thread);
}

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
    return true;
}

bool __vsf_arch_model_is_current_task(vsf_arch_irq_thread_t *irq_thread)
{
    return irq_thread->thread == xTaskGetCurrentTaskHandle();
}

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
    __vsf_freertos.timer.is_running = false;
#if configSUPPORT_STATIC_ALLOCATION
    __vsf_freertos.timer.handle = xTimerCreateStatic("vsf_timer", 1, pdFALSE,
            NULL, __vsf_systimer_callback, &__vsf_freertos.timer.static_timer);
#else
    __vsf_freertos.timer.handle = xTimerCreate("vsf_timer", 1, pdFALSE,
            NULL, __vsf_systimer_callback);
#endif
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
    vsf_systimer_cnt_t diff;
    if (due > vsf_systimer_get()) {
        diff = due - vsf_systimer_get();
    } else {
        diff = 1;
    }

    xTimerChangePeriod(__vsf_freertos.timer.handle, diff, 0);
    return true;
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
    return pdMS_TO_TICKS(time_ms);
}

uint_fast32_t vsf_systimer_tick_to_us(vsf_systimer_cnt_t tick)
{
    // not supported by freertos
    VSF_ARCH_ASSERT(false);
    return 0;
}

uint_fast32_t vsf_systimer_tick_to_ms(vsf_systimer_cnt_t tick)
{
    return tick / portTICK_PERIOD_MS;
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
#if configSUPPORT_STATIC_ALLOCATION
    xTaskCreateStatic(  __vsf_main_task,                        /* The function that implements the task being created. */
                        "vsf",                                  /* Text name for the task - not used by the RTOS, its just to assist debugging. */
                        dimof(__vsf_freertos.main.stack),       /* Size of the buffer passed in as the stack - in words, not bytes! */
                        NULL,                                   /* Parameter passed into the task - not used in this case. */
                        tskIDLE_PRIORITY + 1,                   /* Priority of the task. */
                        __vsf_freertos.main.stack,              /* The buffer to use as the task's stack. */
                        &__vsf_freertos.main.task);             /* The variable that will hold the task's TCB. */
#else
    xTaskCreate(        __vsf_main_task,                        /* The function that implements the task being created. */
                        "vsf",                                  /* Text name for the task - not used by the RTOS, its just to assist debugging. */
                        VSF_ARCH_RTOS_CFG_STACK_DEPTH,          /* Size of the buffer passed in as the stack - in words, not bytes! */
                        NULL,                                   /* Parameter passed into the task - not used in this case. */
                        tskIDLE_PRIORITY + 1,                   /* Priority of the task. */
                        NULL);                                  /* The pointer to the task_id. */
#endif
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
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
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
