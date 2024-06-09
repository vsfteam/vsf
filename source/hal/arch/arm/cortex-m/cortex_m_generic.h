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

#ifndef __CORTEX_M_GENERIC_H__
#define __CORTEX_M_GENERIC_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#define __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
#include "hal/driver/driver.h"
#undef  __VSF_HEADER_ONLY_SHOW_ARCH_INFO__

#include "./SysTick/systick.h"

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

#if __ARM_ARCH == 6 || __TARGET_ARCH_6_M == 1 || __TARGET_ARCH_6S_M == 1
#   ifndef VSF_ARCH_PRI_NUM
#       define VSF_ARCH_PRI_NUM         4
#       undef  VSF_ARCH_PRI_BIT
#       define VSF_ARCH_PRI_BIT         2
#   endif

#   ifndef VSF_ARCH_PRI_BIT
#       define VSF_ARCH_PRI_BIT         2
#   endif
#elif __ARM_ARCH >= 7 || __TARGET_ARCH_7_M == 1 || __TARGET_ARCH_7E_M == 1
#   ifndef VSF_ARCH_PRI_NUM
#       define VSF_ARCH_PRI_NUM         16
#       undef  VSF_ARCH_PRI_BIT
#       define VSF_ARCH_PRI_BIT         4
#   endif

#   ifndef VSF_ARCH_PRI_BIT
#       define VSF_ARCH_PRI_BIT         4
#   endif
#endif

#ifndef VSF_ARCH_CFG_CALLSTACK_TRACE
#   define VSF_ARCH_CFG_CALLSTACK_TRACE ENABLED
#endif

// software interrupt provided by arch
#define VSF_ARCH_SWI_NUM                1

#ifndef VSF_SYSTIMER_CFG_IMPL_MODE
//! \note by default, use tickless mode
#   define VSF_SYSTIMER_CFG_IMPL_MODE      VSF_SYSTIMER_IMPL_WITH_NORMAL_TIMER
#endif
#define __VSF_ARCH_SYSTIMER_BITS        24

/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_arch_wakeup()

#if (__ARM_ARCH >= 7) || (__TARGET_ARCH_7_M == 1) || (__TARGET_ARCH_7E_M == 1)

#   define vsf_atom32_op(__ptr, ...)                                            \
        ({                                                                      \
            int32_t _;                                                          \
            do {                                                                \
                _ = __LDREXW((volatile int32_t *)(__ptr));                      \
            } while ((__STREXW((__VA_ARGS__), (volatile int32_t *)(__ptr))) != 0U);\
            _;                                                                  \
        })

#   define vsf_atom16_op(__ptr, ...)                                            \
        ({                                                                      \
            int16_t _;                                                          \
            do {                                                                \
                _ = __LDREXH((volatile int16_t *)(__ptr));                      \
            } while ((__STREXH((__VA_ARGS__), (volatile int16_t *)(__ptr))) != 0U);\
            _;                                                                  \
        })

#   define vsf_atom8_op(__ptr, ...)                                             \
        ({                                                                      \
            int8_t _;                                                           \
            do {                                                                \
                _ = __LDREXB((volatile int8_t *)(__ptr));                       \
            } while ((__STREXB((__VA_ARGS__), (volatile int8_t *)(__ptr))) != 0U);\
            _;                                                                  \
        })

#   define vsf_spinlock_t               uint32_t
#   define vsf_spinlock_init(__plock)                                           \
        do {                                                                    \
            *(__plock) = 0;                                                     \
        } while (0)
#   define vsf_spin_lock(__plock)                                               \
        do {                                                                    \
            vsf_spinlock_t VSF_MACRO_SAFE_NAME(value) = __LDREXW((volatile vsf_spinlock_t *)(__plock));\
            if (VSF_MACRO_SAFE_NAME(value) != 0) {                              \
                __WFE();                                                        \
            } else if ((__STREXW(1, (volatile vsf_spinlock_t *)(__plock))) == 0U) {\
                break;                                                          \
            }                                                                   \
        } while (1)
#   define vsf_spin_unlock(__plock)                                             \
        do {                                                                    \
            *(__plock) = 0;                                                     \
            __SEV();                                                            \
        } while (0)
#   define vsf_spin_trylock(__plock)    (vsf_spin_lock(__plock), 1)
#endif

/*============================ TYPES =========================================*/

typedef uint64_t vsf_systimer_tick_t;

#define __VSF_ARCH_PRI_INDEX(__N, __UNUSED)                                     \
            __vsf_arch_prio_index_##__N = (__N),

enum {
    VSF_MREPEAT(VSF_ARCH_PRI_NUM,__VSF_ARCH_PRI_INDEX, VSF_ARCH_PRI_BIT)
    __vsf_arch_prio_index_number,
};

#define __VSF_ARCH_PRI(__N, __BIT)                                              \
            VSF_ARCH_PRIO_##__N =                                               \
                ((VSF_ARCH_PRI_NUM - 1 - __vsf_arch_prio_index_##__N)) & 0xFF,  \
            vsf_arch_prio_##__N =                                               \
                ((VSF_ARCH_PRI_NUM - 1 - __vsf_arch_prio_index_##__N)) & 0xFF,


typedef enum vsf_arch_prio_t {
    // avoid vsf_arch_prio_t to be optimized to 8bit
    __VSF_ARCH_PRIO_LEAST_MAX       = INT16_MAX,
    __VSF_ARCH_PRIO_LEAST_MIN       = INT16_MIN,
    VSF_ARCH_PRIO_INVALID           = -1,
    vsf_arch_prio_invalid           = -1,

    VSF_ARCH_PRIO_ENABLE_ALL        = 0x100,
    vsf_arch_prio_enable_all        = 0x100,
    VSF_ARCH_PRIO_DISABLE_ALL       = 0,
    vsf_arch_prio_disable_all       = 0,

    VSF_MREPEAT(VSF_ARCH_PRI_NUM,__VSF_ARCH_PRI,VSF_ARCH_PRI_BIT)

    vsf_arch_prio_highest           = ((VSF_ARCH_PRI_NUM - 1 - (__vsf_arch_prio_index_number - 1))) & 0xFF,
    VSF_ARCH_PRIO_HIGHEST           = vsf_arch_prio_highest,
} vsf_arch_prio_t;

#ifndef VSF_APPLET_USE_ARCH_ABI
#   define VSF_APPLET_USE_ARCH_ABI                  ENABLED
#endif
#if VSF_APPLET_USE_ARCH_ABI == ENABLED
// functions in section 4 of "Run-Time ABI for the ARM Architecture"
//  refer to: https://github.com/ARM-software/abi-aa/releases/download/2023Q1/rtabi32.pdf
typedef struct vsf_arch_abi_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_cdcmpeq);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_cdcmple);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_cdrcmple);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_cfcmpeq);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_cfcmple);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_cfrcmple);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_ldivmod);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_uldivmod);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_idivmod);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_uidivmod);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_d2f);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_f2d);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_lmul);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_llsl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_llsr);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_lasr);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_lcmp);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_ulcmp);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_idiv);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_uidiv);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_uread4);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_uwrite4);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_uread8);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_uwrite8);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_dadd);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_ddiv);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_dmul);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_drsub);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_dsub);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_dcmpeq);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_dcmplt);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_dcmple);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_dcmpge);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_dcmpgt);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_dcmpun);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_fadd);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_fdiv);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_fmul);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_frsub);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_fsub);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_fcmpeq);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_fcmplt);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_fcmple);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_fcmpge);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_fcmpgt);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_fcmpun);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_d2iz);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_d2uiz);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_d2lz);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_d2ulz);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_f2iz);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_f2uiz);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_f2lz);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_f2ulz);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_i2d);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_ui2d);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_l2d);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_ul2d);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_i2f);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_ui2f);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_l2f);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__aeabi_ul2f);
} vsf_arch_abi_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_arch_abi_vplt_t vsf_arch_abi_vplt;
#   endif
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static VSF_CAL_ALWAYS_INLINE void vsf_arch_set_stack(uintptr_t stack)
{
    __set_MSP(stack);
}

static VSF_CAL_ALWAYS_INLINE uintptr_t vsf_arch_get_stack(void)
{
    return __get_MSP();
}

#if VSF_ARCH_USE_THREAD_REG == ENABLED
static VSF_CAL_ALWAYS_INLINE uintptr_t vsf_arch_set_thread_reg(uintptr_t value)
{
    register uintptr_t result;
    __asm__ __volatile__ ("MOV %0, r9" : "=r"(result));
    __asm__ __volatile__ ("MOV r9, %0" : : "r"(value));
    return result;
}
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

