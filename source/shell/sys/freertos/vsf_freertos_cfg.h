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

/*============================ INCLUDES ======================================*/

//! \note do not move this pre-processor statement to other places
#include "../../../vsf_cfg.h"

#ifndef __VSF_FREERTOS_CFG_H__
#define __VSF_FREERTOS_CFG_H__

#include "shell/vsf_shell_wrapper.h"

/*============================ MACROS ========================================*/

// Master switch. User enables this in vsf_usr_cfg.h / board cfg.
#ifndef VSF_USE_FREERTOS
#   define VSF_USE_FREERTOS                     DISABLED
#endif

#ifndef VSF_FREERTOS_ASSERT
#   define VSF_FREERTOS_ASSERT                  VSF_ASSERT
#endif

#define VSF_FREERTOS_WRAPPER(__api)             VSF_SHELL_WRAPPER(vsf_freertos, __api)

// ---------------------------------------------------------------------------
// FreeRTOS kernel baseline this sub-system targets. Aligned with ESP-IDF 5.x
// bundled FreeRTOS (Amazon smp branch derived from v10.5.x).
// ---------------------------------------------------------------------------
#ifndef VSF_FREERTOS_CFG_VERSION_MAJOR
#   define VSF_FREERTOS_CFG_VERSION_MAJOR       10
#endif
#ifndef VSF_FREERTOS_CFG_VERSION_MINOR
#   define VSF_FREERTOS_CFG_VERSION_MINOR       5
#endif
#ifndef VSF_FREERTOS_CFG_VERSION_BUILD
#   define VSF_FREERTOS_CFG_VERSION_BUILD       0
#endif

// ---------------------------------------------------------------------------
// Per-component enable switches. All OFF by default except the minimal
// "task" primitives that other modules depend on.
// ---------------------------------------------------------------------------
#ifndef VSF_FREERTOS_CFG_USE_TASK
#   define VSF_FREERTOS_CFG_USE_TASK            ENABLED
#endif
#ifndef VSF_FREERTOS_CFG_USE_QUEUE
#   define VSF_FREERTOS_CFG_USE_QUEUE           ENABLED
#endif
#ifndef VSF_FREERTOS_CFG_USE_SEMPHR
#   define VSF_FREERTOS_CFG_USE_SEMPHR          DISABLED
#endif
#ifndef VSF_FREERTOS_CFG_USE_TIMERS
#   define VSF_FREERTOS_CFG_USE_TIMERS          DISABLED
#endif
#ifndef VSF_FREERTOS_CFG_USE_EVENT_GROUPS
#   define VSF_FREERTOS_CFG_USE_EVENT_GROUPS    DISABLED
#endif
// Task notification primitives (xTaskNotify / ulTaskNotifyTake / ...).
// Allocates one vsf_sem_t plus two words per task.
#ifndef VSF_FREERTOS_CFG_USE_NOTIFY
#   define VSF_FREERTOS_CFG_USE_NOTIFY          DISABLED
#endif
// Stream buffers + message buffers (xStreamBufferSend / xMessageBufferSend).
#ifndef VSF_FREERTOS_CFG_USE_STREAM_BUFFER
#   define VSF_FREERTOS_CFG_USE_STREAM_BUFFER   DISABLED
#endif
// QueueSet (xQueueCreateSet / xQueueSelectFromSet / ...).
// Requires VSF_FREERTOS_CFG_USE_QUEUE == ENABLED.
#ifndef VSF_FREERTOS_CFG_USE_QUEUESET
#   define VSF_FREERTOS_CFG_USE_QUEUESET        DISABLED
#endif
// Critical section & scheduler-suspend primitives
// (vTaskEnterCritical / vTaskSuspendAll / portENTER_CRITICAL / ...).
// portmacro.h unconditionally exposes the macros; this switch gates
// the port implementation. Defaults to ENABLED because ESP-IDF code
// heavily relies on taskENTER_CRITICAL() being callable.
#ifndef VSF_FREERTOS_CFG_USE_CRITICAL
#   define VSF_FREERTOS_CFG_USE_CRITICAL        ENABLED
#endif

// Default priority used when FreeRTOS task priorities are mapped to VSF
// priorities. FreeRTOS priorities are numerical (0..configMAX_PRIORITIES-1)
// while VSF uses enum (vsf_prio_0 .. vsf_prio_N). The shim maps any FreeRTOS
// priority to VSF_FREERTOS_CFG_DEFAULT_VSF_PRIO by default; boards can
// override for a more nuanced mapping.
#ifndef VSF_FREERTOS_CFG_DEFAULT_VSF_PRIO
#   define VSF_FREERTOS_CFG_DEFAULT_VSF_PRIO    vsf_prio_0
#endif

// Default stack size (in StackType_t units, which we define as uint8_t to
// match byte-sized budgets) when xTaskCreate is asked to allocate its own
// stack. Used for diagnostics only; the real allocation is byte-sized.
#ifndef VSF_FREERTOS_CFG_MIN_STACK_BYTES
#   define VSF_FREERTOS_CFG_MIN_STACK_BYTES     1024
#endif

#endif      // __VSF_FREERTOS_CFG_H__
