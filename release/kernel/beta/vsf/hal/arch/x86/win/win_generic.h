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

#ifndef __WIN_GENERIC_H__
#define __WIN_GENERIC_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"

#include <Windows.h>

// IMPORTANT for using COM: interface from combaseapi is a macro defined to struct
//  this will conflicts with some 3rd-party code, eg: libusb, freetype
//  if windows.h is included in global header file, remove interface define
//  if user c file need to use com, define interface to struct.
#ifdef interface
#   undef interface
#endif

#if     defined(VSF_ARCH_WIN_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(VSF_ARCH_WIN_IMPLEMENT)
#   define __PLOOC_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#define __LITTLE_ENDIAN                 1
#define __BYTE_ORDER                    __LITTLE_ENDIAN

#define VSF_ARCH_PRI_NUM                64

#ifndef VSF_SYSTIMER_CFG_IMPL_MODE
#   define VSF_SYSTIMER_CFG_IMPL_MODE   VSF_SYSTIMER_IMPL_REQUEST_RESPONSE
#endif

// software interrupt provided by arch
#define VSF_ARCH_SWI_NUM                32

#define VSF_ARCH_STACK_PAGE_SIZE        4096
#define VSF_ARCH_STACK_GUARDIAN_SIZE    4096

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef uint64_t vsf_systimer_cnt_t;

#define __VSF_ARCH_PRI(__N, __BIT)                                              \
            VSF_ARCH_PRIO_##__N = (__N),                                        \
            vsf_arch_prio_##__N = (__N),

enum vsf_arch_prio_t {
    VSF_ARCH_PRIO_IVALID = -1,
    vsf_arch_prio_ivalid = -1,
    REPEAT_MACRO(VSF_ARCH_PRI_NUM, __VSF_ARCH_PRI, VSF_ARCH_PRI_BIT)
    vsf_arch_prio_highest = VSF_ARCH_PRI_NUM - 1,
};
typedef enum vsf_arch_prio_t vsf_arch_prio_t;

declare_simple_class(vsf_arch_irq_thread_t)
declare_simple_class(vsf_arch_irq_request_t)

enum vsf_arch_irq_state_t {
    VSF_ARCH_IRQ_STATE_IDLE,
    VSF_ARCH_IRQ_STATE_ACTIVE,
    VSF_ARCH_IRQ_STATE_FOREGROUND,
    VSF_ARCH_IRQ_STATE_BACKGROUND,
};
typedef enum vsf_arch_irq_state_t vsf_arch_irq_state_t;

def_simple_class(vsf_arch_irq_request_t) {
    private_member(
        HANDLE event;
    )
};

def_simple_class(vsf_arch_irq_thread_t) {
    public_member(
        const char *name;
    )
    private_member(
        vsf_dlist_node_t irq_node;
        vsf_dlist_node_t rdy_node;
        HANDLE thread;
        DWORD thread_id;
        vsf_arch_prio_t priority;
        vsf_arch_irq_thread_t *prev;     // call stack
        vsf_arch_irq_state_t state;
        vsf_arch_irq_request_t *reply;

        vsf_irq_handler_t *handler;
        void *param;
    )
};

typedef void (*vsf_arch_irq_entry_t)(void*);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern void __vsf_arch_lock(void);
extern void __vsf_arch_unlock(void);

extern void __vsf_arch_irq_request_init(vsf_arch_irq_request_t *request);
extern void __vsf_arch_irq_request_fini(vsf_arch_irq_request_t *request);
extern void __vsf_arch_irq_request_pend(vsf_arch_irq_request_t *request);
extern void __vsf_arch_irq_request_send(vsf_arch_irq_request_t *request);

extern void __vsf_arch_irq_init(vsf_arch_irq_thread_t *irq_thread,
    vsf_arch_irq_entry_t entry, vsf_arch_prio_t priority, bool is_to_start);
extern void __vsf_arch_irq_fini(vsf_arch_irq_thread_t *irq_thread);
extern void __vsf_arch_irq_set_background(vsf_arch_irq_thread_t *irq_thread);
extern void __vsf_arch_irq_start(vsf_arch_irq_thread_t *irq_thread);
extern void __vsf_arch_irq_end(vsf_arch_irq_thread_t *irq_thread, bool is_terminate);

static ALWAYS_INLINE void vsf_arch_set_stack(uintptr_t stack)
{
#if     defined(__CPU_X86__)
    __asm__("movl %0, %%esp" : : "r"(stack));
#elif   defined(__CPU_X64__)
    __asm__("movq %0, %%rsp" : : "r"(stack));
#endif
}

static ALWAYS_INLINE void vsf_arch_set_pc(uintptr_t pc)
{
    VSF_HAL_ASSERT(false);
}

static ALWAYS_INLINE uintptr_t vsf_arch_get_lr(void)
{
    VSF_HAL_ASSERT(false);
    return 0;
}

#endif
/* EOF */
