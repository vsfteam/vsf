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

#ifndef __RTOS_GENERIC_H__
#define __RTOS_GENERIC_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef __LITTLE_ENDIAN
#   define __LITTLE_ENDIAN                  1
#endif
#ifndef __BYTE_ORDER
#   define __BYTE_ORDER                     __LITTLE_ENDIAN
#endif

#ifndef VSF_ARCH_RTOS_CFG_STACK_DEPTH
#   define VSF_ARCH_RTOS_CFG_STACK_DEPTH    1024
#endif

#ifndef VSF_ARCH_SWI_NUM
#   define VSF_ARCH_SWI_NUM                 0
#endif

#ifndef VSF_ARCH_PRI_NUM
#   if VSF_ARCH_SWI_NUM > 0
#       define VSF_ARCH_PRI_NUM             VSF_ARCH_SWI_NUM
#   else
#       define VSF_ARCH_PRI_NUM             1
#   endif
#endif

// for rtos support, default vsf entry is int vsf_main(void)
#ifndef VSF_USER_ENTRY
#   define VSF_USER_ENTRY                   vsf_main
#endif

#define VSF_ARCH_RTOS_MODE_SUSPEND_RESUME   1
#define VSF_ARCH_RTOS_MODE_REQUEST          2

// TODO: remove suspend/resume mode later if request mode is tested OK
#define VSF_ARCH_RTOS_CFG_MODE              VSF_ARCH_RTOS_MODE_SUSPEND_RESUME

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __VSF_ARCH_PRI(__N, __UNUSED)                                           \
            VSF_ARCH_PRIO_##__N = (__N),                                        \
            vsf_arch_prio_##__N = (__N),

/*============================ TYPES =========================================*/

typedef enum vsf_arch_prio_t {
    VSF_ARCH_PRIO_INVALID = -1,
    vsf_arch_prio_invalid = -1,
    REPEAT_MACRO(VSF_ARCH_PRI_NUM, __VSF_ARCH_PRI, __UNUSED)
    vsf_arch_prio_highest = VSF_ARCH_PRI_NUM - 1,
} vsf_arch_prio_t;

/*============================ INCLUDES ======================================*/

#if !defined(VSF_ARCH_RTOS_MODEL_HEADER)
#   if   defined(__FREERTOS__)
#       define VSF_ARCH_RTOS_MODEL_HEADER           "./freertos/freertos_generic.h"
#   else
#       error no supported rtos found
# endif
#endif

#ifdef __cplusplus
}
#endif

// rtos_model contains:
//  1. vsf_systimer_cnt_t if sys timer is enabled
//  2. vsf_arch_set_stack if thread is enabled
//  3. irq_request implementation
//  4. irq_thread implementation
//  5. vsf_arch_model_low_level_init
//  6. vsf_arch_prio_t
//  7. VSF_ARCH_RTOS_PRIORITY_NUM
//  8. gint
#include VSF_ARCH_RTOS_MODEL_HEADER

#if     defined(__VSF_ARCH_RTOS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_ARCH_RTOS_CFG_SET_STACK
#   define VSF_ARCH_LIMIT_NO_SET_STACK
#endif

#if VSF_ARCH_SWI_NUM > VSF_ARCH_RTOS_PRIORITY_NUM
#   error too many SWI to support in the current rtos configuration
#endif

/*============================ TYPES =========================================*/

typedef void (*vsf_arch_irq_thread_entry_t)(void*);

dcl_simple_class(vsf_arch_irq_thread_with_stack_t)
def_simple_class(vsf_arch_irq_thread_with_stack_t) {
    private_member(
        implement(vsf_arch_irq_thread_t)

#ifdef VSF_ARCH_CFG_STACK_ALIGN_BIT
        VSF_ARCH_RTOS_STACK_T       stack[VSF_ARCH_RTOS_CFG_STACK_DEPTH] ALIGN(1 << VSF_ARCH_CFG_STACK_ALIGN_BIT);
#else
        VSF_ARCH_RTOS_STACK_T       stack[VSF_ARCH_RTOS_CFG_STACK_DEPTH];
#endif
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern void __vsf_arch_delay_ms(uint_fast32_t ms);

extern void __vsf_arch_irq_request_init(vsf_arch_irq_request_t *request, bool auto_reset);
extern void __vsf_arch_irq_request_fini(vsf_arch_irq_request_t *request);
extern void __vsf_arch_irq_request_pend(vsf_arch_irq_request_t *request);
extern void __vsf_arch_irq_request_send(vsf_arch_irq_request_t *request);
extern void __vsf_arch_irq_request_reset(vsf_arch_irq_request_t *request);

extern void __vsf_arch_irq_enter(void);
extern void __vsf_arch_irq_leave(void);

// if priority < 0, then inherit from current priority
extern void __vsf_arch_irq_thread_start(vsf_arch_irq_thread_t *irq_thread,
        const char * const name, vsf_arch_irq_thread_entry_t entry, vsf_arch_prio_t priority,
        VSF_ARCH_RTOS_STACK_T *stack, uint_fast32_t stack_depth);
extern void __vsf_arch_irq_thread_exit(void);
extern void __vsf_arch_irq_thread_set_priority(vsf_arch_irq_thread_t *irq_thread, vsf_arch_prio_t priority);

#if VSF_ARCH_RTOS_CFG_MODE == VSF_ARCH_RTOS_MODE_SUSPEND_RESUME
extern void __vsf_arch_irq_thread_suspend(vsf_arch_irq_thread_t *irq_thread);
extern void __vsf_arch_irq_thread_resume(vsf_arch_irq_thread_t *irq_thread);
extern vsf_arch_prio_t __vsf_arch_irq_thread_get_priority(vsf_arch_irq_thread_t *irq_thread);
#endif

// for rtos support, vsf_arch_wakeup is used to wakeup vsf thread
extern void vsf_arch_wakeup(void);

#ifndef VSF_ARCH_LIMIT_NO_SET_STACK
static ALWAYS_INLINE void vsf_arch_set_stack(uint32_t stack)
{
    VSF_ARCH_RTOS_CFG_SET_STACK(stack);
}
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */

