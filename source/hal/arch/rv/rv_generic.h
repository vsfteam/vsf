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

#ifndef __RV_GENERIC_H__
#define __RV_GENERIC_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#define __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
#include "hal/driver/driver.h"
#undef  __VSF_HEADER_ONLY_SHOW_ARCH_INFO__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef __LITTLE_ENDIAN
#   define __LITTLE_ENDIAN              1
#endif
#ifndef __BYTE_ORDER
#   define __BYTE_ORDER                 __LITTLE_ENDIAN
#endif

#ifndef __VSF_ARCH_SYSTIMER_BITS
#   define __VSF_ARCH_SYSTIMER_BITS     64
#endif

// software interrupt provided by arch
#ifndef VSF_ARCH_SWI_NUM
#   define VSF_ARCH_SWI_NUM             0
#endif

// callstack trace is disabled on RiscV by default.
//  To use callstack trace, please add -fno-omit-frame-pointer to compile options
#ifndef VSF_ARCH_CFG_CALLSTACK_TRACE
#   define VSF_ARCH_CFG_CALLSTACK_TRACE DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_arch_wakeup()

/*============================ TYPES =========================================*/

#if defined(VSF_SYSTIMER_CFG_IMPL_MODE) && (VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_TICK_MODE)
typedef uint32_t vsf_systimer_tick_t;
typedef int32_t vsf_systimer_tick_signed_t;
#else
typedef uint64_t vsf_systimer_tick_t;
#endif
typedef uint32_t vsf_gint_state_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static VSF_CAL_ALWAYS_INLINE vsf_gint_state_t vsf_get_interrupt(void)
{
    vsf_gint_state_t result;
    __asm volatile("csrr %0, mstatus" : "=r"(result) : );
    return result;
}

static VSF_CAL_ALWAYS_INLINE vsf_gint_state_t vsf_set_interrupt(vsf_gint_state_t level)
{
    vsf_gint_state_t result;
    __asm volatile("csrrs %0, mstatus, %1" : "=r"(result) : "r"(level));
    return result & 8;
}

static VSF_CAL_ALWAYS_INLINE vsf_gint_state_t vsf_disable_interrupt(void)
{
    vsf_gint_state_t result;
    __asm volatile("csrrci %0, mstatus, 8" : "=r"(result) :);
    return result & 8;
}

static VSF_CAL_ALWAYS_INLINE vsf_gint_state_t vsf_enable_interrupt(void)
{
    vsf_gint_state_t result;
    __asm volatile("csrrsi %0, mstatus, 8" : "=r"(result) :);
    return result & 8;
}

static VSF_CAL_ALWAYS_INLINE void vsf_arch_sleep(uint_fast32_t mode)
{
    __asm volatile("wfi" : :);
}

static VSF_CAL_ALWAYS_INLINE void vsf_arch_set_stack(uintptr_t stack)
{
    __asm volatile("mv sp, %0" : : "r"(stack) : );
}

static VSF_CAL_ALWAYS_INLINE uintptr_t vsf_arch_get_stack(void)
{
    uintptr_t result;
    __asm volatile("mv %0, sp" : "=r"(result) : );
    return result;
}

#if VSF_ARCH_CFG_CALLSTACK_TRACE == ENABLED
extern void vsf_arch_add_text_region(vsf_arch_text_region_t *region);
extern uint_fast16_t vsf_arch_get_callstack(uintptr_t sp, uintptr_t *callstack, uint_fast16_t callstack_num);
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */

