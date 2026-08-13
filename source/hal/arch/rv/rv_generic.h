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

// VSF_ARCH_PRI_NUM is required by the kernel config for compile-time
//  clamps(vsf_kernel_cfg.h) when the chip device.h is unreachable
//  (applet builds, see below). it sizes data structures only,
//  it does NOT program any hardware
#ifndef VSF_ARCH_PRI_NUM
#   define VSF_ARCH_PRI_NUM             8
#endif

// default systimer implementation mode, chip device.h can override before
//  this header is parsed. non-NONE default is required so that timer related
//  types(eg: vsf_systimer_tick_t) are available in applet builds
#ifndef VSF_SYSTIMER_CFG_IMPL_MODE
#   define VSF_SYSTIMER_CFG_IMPL_MODE   VSF_SYSTIMER_IMPL_WITH_COMP_TIMER
#endif

// callstack trace is disabled on RiscV by default.
//  To use callstack trace, please add -fno-omit-frame-pointer to compile options
#ifndef VSF_ARCH_CFG_CALLSTACK_TRACE
#   define VSF_ARCH_CFG_CALLSTACK_TRACE DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_arch_wakeup()

/*============================ TYPES =========================================*/

/*! \note vsf_arch_prio_t is provided here ONLY as a type, for builds where
 *!       the chip device.h is intentionally unreachable(chip-independent
 *!       __VSF_APPLET__ builds, see hal/driver/driver.h). The real enum with
 *!       concrete values lives in the chip device.h.
 *!       CONCRETE PRIORITY VALUES ARE DELIBERATELY NOT DEFINED HERE:
 *!       RISC-V has no standard priority scheme - PLIC(more levels,
 *!       larger value = higher priority, 0 = disabled), Qingke
 *!       PFIC(IPRIOR-encoded, smaller value = higher preemption priority)
 *!       and CLIC all differ in both count and direction, so any value
 *!       defined at this level would be silently wrong on some chip
 *!       (eg: 0 means "interrupt disabled" on PLIC but "highest" on PFIC).
 *!       Code needing actual priorities must use the values from the chip
 *!       device.h of the target it runs on - a chip-independent build
 *!       cannot and must not pick one.
 */
#ifndef __VSF_ARCH_PRIO_DEFINED
#   define __VSF_ARCH_PRIO_DEFINED
typedef int vsf_arch_prio_t;
#endif

#if VSF_SYSTIMER_CFG_IMPL_MODE != VSF_SYSTIMER_CFG_IMPL_NONE
#   if VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_TICK_MODE
typedef uint32_t vsf_systimer_tick_t;
typedef int32_t vsf_systimer_tick_signed_t;
#   else
typedef uint64_t vsf_systimer_tick_t;
#   endif
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

static VSF_CAL_ALWAYS_INLINE void vsf_arch_set_stack(uintptr_t stack, uint32_t stack_size)
{
    __asm volatile("mv sp, %0" : : "r"(stack) : );
}

static VSF_CAL_ALWAYS_INLINE uintptr_t vsf_arch_get_stack(void)
{
    uintptr_t result;
    __asm volatile("mv %0, sp" : "=r"(result) : );
    return result;
}

// x3(gp) is used as the thread register(eg: GOT base of applets in vsf linux,
//  the ARM counterpart is r9). enabled by the BOARD config
//  (VSF_ARCH_USE_THREAD_REG, see board/<chip>/vsf_board_cfg.h), and unlike
//  ARM r9 it has toolchain side effects, ALL required:
//  1. firmware built with -mno-relax -msmall-data-limit=0, so gp is never
//     used for small-data addressing and stays owned by the thread register;
//  2. applets with GOT accesses rebound to gp at link time(patched binutils
//     or a post-link rewriter like elfpatch_rv.py), see gnuriscvemb.cmake;
//  3. the gp-aware setjmp/longjmp injected below(standard RV setjmp does
//     not save gp)
#if VSF_ARCH_USE_THREAD_REG == ENABLED && !defined(__cplusplus)
static VSF_CAL_ALWAYS_INLINE uintptr_t vsf_arch_set_thread_reg(uintptr_t value)
{
    uintptr_t result;
    __asm volatile("mv %0, gp" : "=r"(result) : );
    __asm volatile("mv gp, %0" : : "r"(value));
    return result;
}
static VSF_CAL_ALWAYS_INLINE uintptr_t vsf_arch_get_thread_reg(void)
{
    uintptr_t result;
    __asm volatile("mv %0, gp" : "=r"(result) : );
    return result;
}

// thread context switches rely on setjmp/longjmp, and the standard RISC-V
//  setjmp does not save gp(x3) - inject the gp-aware implementation from
//  rv_setjmp.S so that the thread register is preserved across switches.
//  NOTE: prototypes come from simple_libc setjmp.h(jmp_buf typed)
#   ifndef VSF_ARCH_SETJMP
#       define VSF_ARCH_SETJMP      vsf_arch_rv_setjmp
#   endif
#   ifndef VSF_ARCH_LONGJMP
#       define VSF_ARCH_LONGJMP     vsf_arch_rv_longjmp
#   endif
#endif

#if VSF_ARCH_CFG_CALLSTACK_TRACE == ENABLED
extern void vsf_arch_add_text_region(vsf_arch_text_region_t *region);
extern uint_fast16_t vsf_arch_get_callstack(uintptr_t sp, uintptr_t *callstack, uint_fast16_t callstack_num);
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */

