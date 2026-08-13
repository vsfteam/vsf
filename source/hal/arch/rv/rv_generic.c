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

/*============================ INCLUDES ======================================*/

#include "hal/arch/vsf_arch_abstraction.h"
#include "hal/driver/driver.h"
#include "hal/arch/__vsf_arch_interface.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if (__IS_COMPILER_GCC__ || __IS_COMPILER_LLVM__) && !defined(__VSF_ARCH_CLZ)
// __vsf_arch_clz(0) = sizeof(uintalu_t) * 8
uint_fast8_t __vsf_arch_clz(uintalu_t a)
{
    // __builtin_clz(0) for riscv is -1
    int result = __builtin_clz(a);
    if (result < 0) {
        return sizeof(a) << 3;
    } else {
        return result;
    }
}
#endif

/*----------------------------------------------------------------------------*
 * Infrastructure                                                             *
 *----------------------------------------------------------------------------*/
/*! \note initialize architecture specific service
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
bool vsf_arch_low_level_init(void)
{
    return true;
}

/*----------------------------------------------------------------------------*
 * callstack trace                                                            *
 *----------------------------------------------------------------------------*/

#if VSF_ARCH_CFG_CALLSTACK_TRACE == ENABLED

#if VSF_USE_KERNEL == ENABLED
#   define __VSF_EDA_CLASS_INHERIT__
#   include "kernel/vsf_kernel.h"
#endif

void vsf_arch_add_text_region(vsf_arch_text_region_t *region)
{
    // TODO
}

uint_fast16_t vsf_arch_get_callstack(uintptr_t sp, uintptr_t *callstack, uint_fast16_t callstack_num)
{
    uint32_t pc;
    uint_fast16_t realnum = 0;
    uintptr_t stack_bottom;

    extern uintptr_t _sp;
#if VSF_USE_KERNEL == ENABLED && VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
    vsf_eda_t *eda = vsf_eda_get_cur();
    if ((NULL == eda) || !vsf_eda_is_stack_owner(eda)) {
        stack_bottom = &_sp;
    } else {
#   if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
        __vsf_eda_frame_t *frame = eda->fn.frame;
        vsf_thread_cb_t *thread_cb = (vsf_thread_cb_t *)frame->ptr.target;
        stack_bottom = (uintptr_t)thread_cb->stack + thread_cb->stack_size;
#   else
        vsf_thread_t *thread = (vsf_thread_t *)eda;
        stack_bottom = (uintptr_t)thread->stack + thread->stack_size;
#   endif
    }
#else
    stack_bottom = &_sp;
#endif

    uintptr_t frame_pointer;
    __asm volatile("mv %0, s0" : "=r"(frame_pointer) : );

    for (; (frame_pointer < stack_bottom) && (realnum < callstack_num); frame_pointer = ((uintptr_t *)frame_pointer)[-2]) {
        *callstack++ = ((uintptr_t *)frame_pointer)[-1];
        realnum++;
    }

    return realnum;
}

#endif

/* EOF */
