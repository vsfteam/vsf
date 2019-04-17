/***************************************************************************
 *   Copyright(C)2009-2018 by Gorgon Meducer<Embedded_zhuoran@hotmail.com> *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "hal/arch/vsf_arch.h"

#include "utilities/compiler.h"
#include "utilities/ooc.h"
#include "utilities/template/template.h"
#include "windows.h"

/*============================ MACROS ========================================*/
#define __VSF_WIN32_SWI_NUM                 16

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
struct __vsf_win32_thread_t {
	vsf_slist_t threadlist;

	uint_fast8_t priority;
	HANDLE hThread;
};
typedef struct __vsf_win32_thread_t __vsf_win32_thread_t;

struct __vsf_win32_tickclk_thread_t {
	implement(__vsf_win32_thread_t)
	bool running, to_exit;
};
typedef struct __vsf_win32_tickclk_thread_t __vsf_win32_tickclk_thread_t;

struct __vsf_win32_swi_t {
    vsf_swi_hanler_t *handler;
    void *p;
    uint_fast8_t priority;
};
typedef struct __vsf_win32_swi_t __vsf_win32_swi_t;

struct __vsf_win32_t {
	bool inited;
	bool locked;
    CRITICAL_SECTION hGlobalLock;
    HANDLE hWakeupSem;

	uint_fast8_t priority_enabled;
	vsf_slist_t threadlist;

    CRITICAL_SECTION hSleepPend;
	__vsf_win32_tickclk_thread_t tickclk;
    __vsf_win32_swi_t swi[__VSF_WIN32_SWI_NUM];
};
typedef struct __vsf_win32_t __vsf_win32_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static __vsf_win32_t __vsf_win32;

/*============================ PROTOTYPES ====================================*/
extern void vsf_systimer_evthandler(void);

/*============================ IMPLEMENTATION ================================*/

static bool __vsf_win32_thread_start(__vsf_win32_thread_t *pthread,
        LPTHREAD_START_ROUTINE entry, uint_fast8_t priority)
{
    pthread->priority = priority;
    pthread->hThread = CreateThread(NULL, 0, entry, NULL, CREATE_SUSPENDED, NULL);
    if (NULL == pthread->hThread)
        return false;

    vsf_slist_insert(&__vsf_win32.threadlist, pthread, pnxt, __vsf_win32_thread_t, threadlist,
            pnxt->priority >= pthread->priority
        );
    ResumeThread(pthread->hThread);
    return true;
}

static void __vsf_win32_init(void)
{
    if (!__vsf_win32.inited) {
        __vsf_win32.inited = true;
        __vsf_win32.locked = false;
        InitializeCriticalSection(&__vsf_win32.hGlobalLock);

        __vsf_win32.hWakeupSem = CreateSemaphore(0, 0, 1, 0);

        InitializeCriticalSection(&__vsf_win32.hSleepPend);
        EnterCriticalSection(&__vsf_win32.hSleepPend);
    }
}

static void __vsf_win32_irq_start(uint_fast8_t priority)
{
    EnterCriticalSection(&__vsf_win32.hSleepPend);
}

static void __vsf_win32_irq_end(uint_fast8_t priority)
{
    ReleaseSemaphore(__vsf_win32.hWakeupSem, 1, NULL);
}

void ENABLE_GLOBAL_INTERRUPT(void)
{
    __vsf_win32_init();
    __vsf_win32.locked = true;
    LeaveCriticalSection(&__vsf_win32.hGlobalLock);
}

void DISABLE_GLOBAL_INTERRUPT(void)
{
    __vsf_win32_init();
    __vsf_win32.locked = false;
    EnterCriticalSection(&__vsf_win32.hGlobalLock);
}

bool GET_GLOBAL_INTERRUPT_STATE(void)
{
    __vsf_win32_init();
    return __vsf_win32.locked;
}

static void CALLBACK TimeProc(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
    if (__vsf_win32.tickclk.running) {
        __vsf_win32_irq_start(__vsf_win32.tickclk.priority);
        vsf_systimer_evthandler();
        __vsf_win32_irq_end(__vsf_win32.tickclk.priority);
    }
}

static DWORD CALLBACK TickclkThreadProc(PVOID pvoid)
{
    MSG msg;
    PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
    SetTimer(NULL, 10, 1, TimeProc);
    __vsf_win32.tickclk.running = true;
    while (!__vsf_win32.tickclk.to_exit && GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_TIMER) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    KillTimer(NULL, 10);
    __vsf_win32.tickclk.to_exit = false;
    return 0;
}

/*! \brief initialise tickclk thread to generate a system timer
 *! \param frequency the target frequency in Hz
 *! \return initialization result in vsf_err_t 
 */
vsf_err_t vsf_systimer_init(uint32_t frequency)
{
    __vsf_win32_init();
    __vsf_win32_thread_start(&__vsf_win32.tickclk.use_as____vsf_win32_thread_t,
            TickclkThreadProc, 1);
    __vsf_win32.tickclk.to_exit = false;
    return VSF_ERR_NONE;
}

/*! \brief initialise a software interrupt
 *! \param idx the index of the software interrupt
 *! \return initialization result in vsf_err_t
 */
vsf_err_t vsf_swi_init(uint_fast8_t idx, uint_fast8_t priority, vsf_swi_hanler_t *handler, void *p)
{
    __vsf_win32_init();

    if (idx >= dimof(__vsf_win32.swi))
        return VSF_ERR_NOT_ENOUGH_RESOURCES;

    __vsf_win32.swi[idx].handler = handler;
    __vsf_win32.swi[idx].handler = p;
    __vsf_win32.swi[idx].priority = priority;
    return VSF_ERR_NONE;
}

/*! \brief trigger a software interrupt
 *! \param idx the index of the software interrupt
 */
void vsf_swi_trigger(uint_fast8_t idx)
{
    __vsf_win32_init();

    if ((idx < dimof(__vsf_win32.swi)) && (__vsf_win32.swi[idx].handler != NULL)) {
        __vsf_win32_irq_start(__vsf_win32.swi[idx].priority);
        __vsf_win32.swi[idx].handler(__vsf_win32.swi[idx].p);
        __vsf_win32_irq_end(__vsf_win32.swi[idx].priority);
    }
}

uint_fast8_t vsf_set_base_priority(uint_fast8_t priority)
{
    uint_fast8_t orig = __vsf_win32.priority_enabled;
    __vsf_win32.priority_enabled = priority;
    return orig;
}

INLINE void vsf_arch_set_stack(uint32_t stack)
{

}

int vsf_disable_interrupt(void)
{
    return 0;
}

void vsf_enable_interrupt(int orig)
{
}

void vsf_arch_sleep(uint32_t mode)
{
    __vsf_win32_init();
    LeaveCriticalSection(&__vsf_win32.hSleepPend);
    WaitForSingleObject(__vsf_win32.hWakeupSem, INFINITE);
}

/* EOF */
