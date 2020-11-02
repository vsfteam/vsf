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

#define __VSF_ARCH_WIN_IMPLEMENT
#include "hal/arch/vsf_arch_abstraction.h"

#if VSF_ARCH_PRI_NUM == 1 && VSF_ARCH_SWI_NUM == 0

#include "hal/arch/__vsf_arch_interface.h"
#include <Windows.h>

/*============================ MACROS ========================================*/

#define VSF_ARCH_BG_TRACE_EN                ENABLED
#define VSF_ARCH_SYSTIMER_FREQ              (10 * 1000 * 1000)

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __vsf_arch_crit_init(__crit)        InitializeCriticalSection(&(__crit))
#define __vsf_arch_crit_enter(__crit)       EnterCriticalSection(&(__crit))
#define __vsf_arch_crit_leave(__crit)       LeaveCriticalSection(&(__crit))

/*============================ TYPES =========================================*/

typedef CRITICAL_SECTION vsf_arch_crit_t;

/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vsf_arch_create_irq_thread(vsf_arch_irq_thread_t *irq_thread, vsf_arch_irq_entry_t entry);

/*============================ INCLUDES ======================================*/

#define __VSF_ARCH_WITHOUT_THREAD_SUSPEND_TEMPLATE__
#include "hal/arch/common/arch_without_thread_suspend/vsf_arch_without_thread_suspend_template.inc"

/*============================ TYPES =========================================*/

typedef struct vsf_arch_systimer_ctx_t {
    implement(vsf_arch_irq_thread_t);
    vsf_arch_irq_request_t timer_request;
    HANDLE timer;
    vsf_systimer_cnt_t start_tick;
} vsf_arch_systimer_ctx_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static NO_INIT vsf_arch_systimer_ctx_t __vsf_arch_systimer;

/*============================ IMPLEMENTATION ================================*/

/*----------------------------------------------------------------------------*
 * infrastructure                                                             *
 *----------------------------------------------------------------------------*/

void __vsf_arch_irq_request_init(vsf_arch_irq_request_t *request)
{
    VSF_HAL_ASSERT(!request->is_inited);
    request->event = CreateEvent(NULL, false, false, NULL);
    request->is_inited = true;
}

void __vsf_arch_irq_request_fini(vsf_arch_irq_request_t *request)
{
    VSF_HAL_ASSERT(request->is_inited);
    CloseHandle(request->event);
    request->is_inited = false;
}

void __vsf_arch_irq_request_pend(vsf_arch_irq_request_t *request)
{
    VSF_HAL_ASSERT(request->is_inited);
    WaitForSingleObject(request->event, INFINITE);
}

void __vsf_arch_irq_request_send(vsf_arch_irq_request_t *request)
{
    VSF_HAL_ASSERT(request->is_inited);
    SetEvent(request->event);
}

static DWORD __stdcall __vsf_arch_irq_entry(LPVOID lpThreadParameter)
{
    vsf_arch_irq_thread_t *irq_thread = lpThreadParameter;
    if (irq_thread->entry != NULL) {
        irq_thread->entry(irq_thread);
    }
    return 0;
}

static vsf_err_t __vsf_arch_create_irq_thread(vsf_arch_irq_thread_t *irq_thread, vsf_arch_irq_entry_t entry)
{
    irq_thread->entry = entry;
    irq_thread->thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)__vsf_arch_irq_entry,
                irq_thread, 0, NULL);
    if (NULL == irq_thread->thread) {
        VSF_HAL_ASSERT(false);
        return VSF_ERR_FAIL;
    }
    irq_thread->thread_id = GetThreadId(irq_thread->thread);
    return VSF_ERR_NONE;
}

void __vsf_arch_irq_sleep(uint32_t ms)
{
    Sleep(ms);
}

/*----------------------------------------------------------------------------*
 * Systimer Timer Implementation                                              *
 *----------------------------------------------------------------------------*/

#if VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_REQUEST_RESPONSE

static void __vsf_systimer_thread(void *arg)
{
    vsf_arch_systimer_ctx_t *ctx = arg;
    __vsf_arch_irq_set_background(&ctx->use_as__vsf_arch_irq_thread_t);
    while (1) {
        __vsf_arch_irq_request_pend(&__vsf_arch_systimer.timer_request);
        WaitForSingleObject(ctx->timer, INFINITE);

        __vsf_arch_irq_start(&ctx->use_as__vsf_arch_irq_thread_t);

            vsf_systimer_cnt_t tick = vsf_systimer_get();
            vsf_systimer_timeout_evt_hanlder(tick);

        __vsf_arch_irq_end(&ctx->use_as__vsf_arch_irq_thread_t, false);
    }
}

/*! \brief initialise SysTick to generate a system timer
 *! \param frequency the target tick frequency in Hz
 *! \return initialization result in vsf_err_t
 */
vsf_err_t vsf_systimer_init(void)
{
    __vsf_arch_systimer.start_tick = vsf_systimer_get();

    __vsf_arch_systimer.timer = CreateWaitableTimer(NULL, true, NULL);
    VSF_HAL_ASSERT(NULL != __vsf_arch_systimer.timer);

    __vsf_arch_irq_request_init(&__vsf_arch_systimer.timer_request);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_systimer_start(void)
{
    __vsf_arch_irq_init(&__vsf_arch_systimer.use_as__vsf_arch_irq_thread_t,
                "timer", __vsf_systimer_thread, vsf_arch_prio_0);
    return VSF_ERR_NONE;
}

void vsf_systimer_set_idle(void)
{
}

vsf_systimer_cnt_t vsf_systimer_get(void)
{
    LARGE_INTEGER li;
    GetSystemTimeAsFileTime((LPFILETIME)&li);
    return (vsf_systimer_cnt_t)li.QuadPart - __vsf_arch_systimer.start_tick;
}

bool vsf_systimer_set(vsf_systimer_cnt_t due)
{
    LARGE_INTEGER li = {
        .QuadPart = __vsf_arch_systimer.start_tick + due,
    };
    if (!SetWaitableTimer(__vsf_arch_systimer.timer, &li, 0, NULL, NULL, false)) {
        VSF_HAL_ASSERT(false);
        return false;
    }
    __vsf_arch_irq_request_send(&__vsf_arch_systimer.timer_request);
    return true;
}

bool vsf_systimer_is_due(vsf_systimer_cnt_t due)
{
    return (vsf_systimer_get() >= due);
}

vsf_systimer_cnt_t vsf_systimer_us_to_tick(uint_fast32_t time_us)
{
    return (vsf_systimer_cnt_t)(((uint64_t)time_us * VSF_ARCH_SYSTIMER_FREQ) / 1000000UL);
}

vsf_systimer_cnt_t vsf_systimer_ms_to_tick(uint_fast32_t time_ms)
{
    return (vsf_systimer_cnt_t)(((uint64_t)time_ms * VSF_ARCH_SYSTIMER_FREQ) / 1000UL);
}

uint_fast32_t vsf_systimer_tick_to_us(vsf_systimer_cnt_t tick)
{
    return tick * 1000000ul / VSF_ARCH_SYSTIMER_FREQ;
}

uint_fast32_t vsf_systimer_tick_to_ms(vsf_systimer_cnt_t tick)
{
    return tick * 1000ul / VSF_ARCH_SYSTIMER_FREQ;
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
    memset(&__vsf_arch_systimer, 0, sizeof(__vsf_arch_systimer));
    strcpy((char *)__vsf_arch_common.por_thread.name, "por");
    __vsf_arch_common.por_thread.thread_id = GetCurrentThreadId();
    __vsf_arch_common.por_thread.thread = OpenThread(THREAD_ALL_ACCESS, false, __vsf_arch_common.por_thread.thread_id);
    return __vsf_arch_low_level_init();
}

#endif
/* EOF */
