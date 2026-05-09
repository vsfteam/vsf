/*****************************************************************************
 *   Copyright(C)2009-2026 by VSF Team                                       *
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

// Clean-room FreeRTOS portmacro.h shim for VSF.
//
// Only the primitive types FreeRTOS applications rely on are exposed here.
// Architecture-specific primitives (portYIELD_FROM_ISR, portENTER_CRITICAL,
// etc.) are mapped to VSF kernel operations where a sensible equivalent
// exists; the rest become no-ops so that code written against FreeRTOS
// compiles but the semantics are explicit in this header.

#ifndef __VSF_FREERTOS_PORTMACRO_H__
#define __VSF_FREERTOS_PORTMACRO_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// for vsf_systimer_tick_t
#include "kernel/vsf_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ TYPES =========================================*/

typedef long                    BaseType_t;
typedef unsigned long           UBaseType_t;

typedef vsf_systimer_tick_t     TickType_t;
#ifndef portMAX_DELAY
#   define portMAX_DELAY        ((TickType_t)-1)
#endif

// StackType_t is documented as "the type used by the stack". FreeRTOS
// applications pass stack budgets in units of StackType_t. We define it as
// a byte so budgets are expressed in bytes, which matches how the VSF
// thread API actually consumes them.
typedef uint8_t                 StackType_t;

/*============================ MACROS ========================================*/

#ifndef pdFALSE
#   define pdFALSE              ((BaseType_t)0)
#endif
#ifndef pdTRUE
#   define pdTRUE               ((BaseType_t)1)
#endif
#ifndef pdPASS
#   define pdPASS               pdTRUE
#endif
#ifndef pdFAIL
#   define pdFAIL               pdFALSE
#endif

// Scheduler-level critical sections. Under the hood these call
// vTaskEnterCritical / vTaskExitCritical (see task.h), which wrap
// vsf_sched_lock with a nested-counter so pairs can safely nest.
// The FromISR variants return/accept the saved scheduler state.
//
// ESP-IDF's FreeRTOS variant passes a portMUX_TYPE spinlock to the
// macro -- we accept but ignore the argument on single-core targets.
// Users that genuinely need IRQ-off sections should still call
// vsf_protect_interrupt() directly.
#define portENTER_CRITICAL(...)         vTaskEnterCritical()
#define portEXIT_CRITICAL(...)          vTaskExitCritical()
#define portENTER_CRITICAL_ISR(...)     vTaskEnterCritical()
#define portEXIT_CRITICAL_ISR(...)      vTaskExitCritical()
#define portDISABLE_INTERRUPTS()        ((void)0)
#define portENABLE_INTERRUPTS()         ((void)0)

// ESP-IDF portMUX_TYPE spinlock compatibility shim. On single-core VSF
// targets there is no real spinlock to acquire; the type and its
// initialiser are provided purely so code written for ESP-IDF compiles.
typedef struct {
    uint32_t _placeholder;
} portMUX_TYPE;
#define portMUX_INITIALIZER_UNLOCKED    { 0 }
#define portMUX_FREE_VAL                0
#define vPortCPUInitializeMutex(mux)    do { (void)(mux); } while(0)

// Entry points (defined in port/freertos_critical_port.c). Exposed
// through portmacro.h so anything that includes FreeRTOS.h picks them up.
extern void          vTaskEnterCritical(void);
extern void          vTaskExitCritical(void);
extern UBaseType_t   vTaskEnterCriticalFromISR(void);
extern void          vTaskExitCriticalFromISR(UBaseType_t uxSavedInterruptState);

#define taskENTER_CRITICAL(...)             vTaskEnterCritical()
#define taskEXIT_CRITICAL(...)              vTaskExitCritical()
#define taskENTER_CRITICAL_ISR(...)         vTaskEnterCriticalFromISR()
#define taskEXIT_CRITICAL_ISR(x)            vTaskExitCriticalFromISR((UBaseType_t)(x))
#define taskENTER_CRITICAL_FROM_ISR(...)    vTaskEnterCriticalFromISR()
#define taskEXIT_CRITICAL_FROM_ISR(x)       vTaskExitCriticalFromISR((UBaseType_t)(x))

// Stack budget unit. FreeRTOS defines configMINIMAL_STACK_SIZE in units of
// StackType_t. In this shim one unit == one byte.
#define portSTACK_TYPE                  StackType_t
#define portBASE_TYPE                   BaseType_t

// portTICK_PERIOD_MS is the number of milliseconds per kernel tick. The VSF
// shim maps FreeRTOS ticks 1:1 to milliseconds so that pdMS_TO_TICKS is the
// identity. The underlying sleep still uses systimer ticks internally.
#ifndef portTICK_PERIOD_MS
#   define portTICK_PERIOD_MS           ((TickType_t)1)
#endif
#ifndef portTICK_RATE_MS
#   define portTICK_RATE_MS             portTICK_PERIOD_MS
#endif

// Placeholder: real ISR yield must be deferred to the scheduler on the
// caller side. Documented as a best-effort no-op.
#define portYIELD_FROM_ISR(x)           ((void)(x))

#ifdef __cplusplus
}
#endif

#endif      // __VSF_FREERTOS_PORTMACRO_H__
