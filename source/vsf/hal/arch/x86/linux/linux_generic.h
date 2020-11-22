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

#ifndef __LINUX_GENERIC_H__
#define __LINUX_GENERIC_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"
#include "utilities/vsf_utilities.h"

#if     defined(__VSF_ARCH_LINUX_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef __LITTLE_ENDIAN
#   define __LITTLE_ENDIAN                 1
#endif
#ifndef __BYTE_ORDER
#   define __BYTE_ORDER                    __LITTLE_ENDIAN
#endif

#ifndef VSF_ARCH_PRI_NUM
#   define VSF_ARCH_PRI_NUM             1
#endif
#if VSF_ARCH_PRI_NUM != 1
#   error VSF_ARCH_PRI_NUM MUST be 1 for linux
#endif

#ifndef VSF_SYSTIMER_CFG_IMPL_MODE
#   define VSF_SYSTIMER_CFG_IMPL_MODE   VSF_SYSTIMER_IMPL_REQUEST_RESPONSE
#endif

// software interrupt provided by arch
#ifndef VSF_ARCH_SWI_NUM
#   define VSF_ARCH_SWI_NUM             0
#endif
#if VSF_ARCH_SWI_NUM != 0
#   error VSF_ARCH_SWI_NUM MUST be 0 for linux
#endif

#define VSF_ARCH_STACK_PAGE_SIZE        4096
#define VSF_ARCH_STACK_GUARDIAN_SIZE    4096

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __VSF_ARCH_PRI(__N, __BIT)                                              \
            VSF_ARCH_PRIO_##__N = (__N),                                        \
            vsf_arch_prio_##__N = (__N),

#define vsf_arch_wakeup()

/*============================ TYPES =========================================*/

// unit is us
typedef uint64_t vsf_systimer_cnt_t;

typedef enum vsf_arch_prio_t {
    VSF_ARCH_PRIO_INVALID = -1,
    vsf_arch_prio_invalid = -1,
    REPEAT_MACRO(VSF_ARCH_PRI_NUM, __VSF_ARCH_PRI, VSF_ARCH_PRI_BIT)
    vsf_arch_prio_highest = VSF_ARCH_PRI_NUM - 1,
} vsf_arch_prio_t;

declare_simple_class(vsf_arch_irq_thread_t)
declare_simple_class(vsf_arch_irq_request_t)

typedef void (*vsf_arch_irq_entry_t)(void*);

def_simple_class(vsf_arch_irq_request_t) {
    private_member(
        int id;
        void *arch_thread;
        bool is_triggered;
        bool is_inited;
    )
};

typedef volatile bool vsf_gint_state_t;

/*============================ INCLUDES ======================================*/

#include "hal/arch/common/arch_without_thread_suspend/vsf_arch_without_thread_suspend_template.h"

/*============================ TYPES =========================================*/

def_simple_class(vsf_arch_irq_thread_t) {
    private_member(
        implement(vsf_arch_irq_thread_common_t)
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern void __vsf_arch_irq_sleep(uint32_t ms);

extern void __vsf_arch_irq_request_init(vsf_arch_irq_request_t *request);
extern void __vsf_arch_irq_request_fini(vsf_arch_irq_request_t *request);
extern void __vsf_arch_irq_request_pend(vsf_arch_irq_request_t *request);
extern void __vsf_arch_irq_request_send(vsf_arch_irq_request_t *request);

extern void __vsf_arch_irq_init(vsf_arch_irq_thread_t *irq_thread, char *name,
    vsf_arch_irq_entry_t entry, vsf_arch_prio_t priority);
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

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
