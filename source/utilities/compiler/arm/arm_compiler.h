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

#ifndef __USE_ARM_COMPILER_H_PART_1__
#define __USE_ARM_COMPILER_H_PART_1__

/*============================ INCLUDES ======================================*/

#include "./arm_compiler_detect.h"

#endif  /* end of __USE_ARM_COMPILER_H_PART_1__ */

/*========================== Multiple-Entry Start ============================*/

#if defined(__IS_COMPILER_IAR__) && __IS_COMPILER_IAR__
#   include <intrinsics.h>
#endif

#ifndef __VSF_HEADER_ONLY_SHOW_COMPILER_INFO__
#   include "./type.h"
#endif
#include "../__common/__compiler.h"

/*----------------------------------------------------------------------------*
 * Overwrite MAX_CONSTRUCTOR_PRIORITY if necessary                            *
 *----------------------------------------------------------------------------*/
#if     __IS_COMPILER_ARM_COMPILER_5__                                          \
    ||  __IS_COMPILER_ARM_COMPILER_6__
// TODO: what is the real MAX_CONSTRUCTOR_PRIORITY for arm-compiler 5/6?
#   ifndef MAX_CONSTRUCTOR_PRIORITY
#       define MAX_CONSTRUCTOR_PRIORITY         65535
#   endif
#elif __IS_COMPILER_GCC__
// TODO: as test, priority of 65535 is lower than 0, 65534 is the max priority
#   undef MAX_CONSTRUCTOR_PRIORITY
#   define MAX_CONSTRUCTOR_PRIORITY             65534
#endif

#ifndef VSF_UTILITIES_REQ___CMSIS_HEADER_FILE__FROM_USR
#include "cmsis_compiler.h"
#else
#include VSF_UTILITIES_REQ___CMSIS_HEADER_FILE__FROM_USR
#endif

/*========================== Multiple-Entry End ==============================*/


#ifndef __USE_ARM_COMPILER_H_PART_2__
#define __USE_ARM_COMPILER_H_PART_2__

#ifdef __cplusplus
extern "C" {
#endif

/* -----------------  Start of section using anonymous unions  -------------- */
#if __IS_COMPILER_ARM_COMPILER_5__
  //#pragma push
  #pragma anon_unions
#elif __IS_COMPILER_ARM_COMPILER_6__
#elif __IS_COMPILER_IAR__
  #pragma language=extended
#elif __IS_COMPILER_GCC__
/* anonymous unions are enabled by default */
  #if __GNUC__ <= 7
    #define __COMPILER_HAS_NO_NEXTTOWARDF_IN_MATH__
  #endif
#elif __IS_COMPILER_LLVM__
/* anonymous unions are enabled by default */
#elif defined(__TMS470__)
/* anonymous unions are enabled by default */
#elif defined(__TASKING__)
  #pragma warning 586
#else
  #warning Not supported compiler type
#endif

/*----------------------------------------------------------------------------*
 * Warning Mitigation                                                         *
 *----------------------------------------------------------------------------*/

#if __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic ignored "-Wdollar-in-identifier-extension"
#endif



/*============================ MACROS ========================================*/

//! \brief 1 cycle nop operation
#ifndef NOP
    #define NOP()                       __asm__ __volatile__ ("nop");
#endif


//! \brief none standard memory types
#if __IS_COMPILER_IAR__
#   define LOW_LEVEL_INIT_RET_T int
#   define VSF_CAL_NO_INIT              __no_init
#   define VSF_CAL_NO_RETURN            __attribute__((noreturn))
#   define VSF_CAL_ROOT                 __root
#   define VSF_CAL_INLINE               inline
#   define VSF_CAL_NO_INLINE            __attribute__((noinline))
#   define VSF_CAL_ALWAYS_INLINE        inline __attribute__((always_inline))
#   define VSF_CAL_WEAK(...)            __weak
#   define VSF_CAL_RAMFUNC              __ramfunc
#   define __asm__                      __asm
#   define __VSF_CAL_ALIGN(__N)         __attribute__((aligned (__N)))
#   define __VSF_CAL_AT_ADDR(__ADDR)    @ __ADDR
#   define __VSF_CAL_SECTION(__SEC)     __attribute__((section (__SEC)))
#   define __VSF_CAL_WEAK_ALIAS(__ORIGIN, __ALIAS) \
                                        _Pragma(__VSF_STR(weak __ORIGIN=__ALIAS))
#   define VSF_CAL_PACKED               __attribute__((packed))
#   define VSF_CAL_UNALIGNED            __packed
#   define VSF_CAL_TRANSPARENT_UNION    __attribute__((transparent_union))
#   define __VSF_CAL_ALIGN_OF(...)      __ALIGNOF__(__VA_ARGS__)

#   define __IAR_STARTUP_DATA_INIT      __iar_data_init3

#   define __VSF_CAL_ISR(__VEC)         void __VEC(void)

#elif __IS_COMPILER_ARM_COMPILER_5__
#   define VSF_CAL_NO_INIT              __attribute__( ( section( ".bss.noinit"),zero_init) )
#   define VSF_CAL_NO_RETURN            __attribute__((noreturn))
#   define VSF_CAL_ROOT                 __attribute__((used))
#   define VSF_CAL_INLINE               __inline
#   define VSF_CAL_NO_INLINE            __attribute__((noinline))
#   define VSF_CAL_ALWAYS_INLINE        __inline __attribute__((always_inline))
#   define VSF_CAL_WEAK(...)            __attribute__((weak))
#   define VSF_CAL_RAMFUNC              __attribute__((section (".textrw")))
#   define __asm__                      __asm
#   define __VSF_CAL_ALIGN(__N)         __attribute__((aligned (__N)))
#   define __VSF_CAL_AT_ADDR(__ADDR)    __attribute__((at(__ADDR)))
#   define __VSF_CAL_SECTION(__SEC)     __attribute__((section (__SEC)))
#   define __VSF_CAL_WEAK_ALIAS(__ORIGIN, __ALIAS) \
                                        __attribute__((weakref(__VSF_STR(__ALIAS))))

#   define VSF_CAL_PACKED               __attribute__((packed))
#   define VSF_CAL_UNALIGNED            __packed
#   define VSF_CAL_TRANSPARENT_UNION    __attribute__((transparent_union))
#   define __VSF_CAL_ALIGN_OF(...)      __alignof__(__VA_ARGS__)

#   define __VSF_CAL_ISR(__VEC)         void __VEC(void)

#elif __IS_COMPILER_ARM_COMPILER_6__
#   define VSF_CAL_NO_INIT              __attribute__( ( section( ".bss.noinit")) )
#   define VSF_CAL_NO_RETURN            __attribute__((noreturn))
#   define VSF_CAL_ROOT                 __attribute__((used))
#   define VSF_CAL_INLINE               __inline
#   define VSF_CAL_NO_INLINE            __attribute__((noinline))
#   define VSF_CAL_ALWAYS_INLINE        __inline __attribute__((always_inline))
#   define VSF_CAL_WEAK(...)            __attribute__((weak))
#   define VSF_CAL_RAMFUNC              __attribute__((section (".textrw")))
#   define __asm__                      __asm
#   define __VSF_CAL_ALIGN(__N)         __attribute__((aligned (__N)))
#   define __VSF_CAL_AT_ADDR(__ADDR)    __attribute__((section (".ARM.__at_" #__ADDR)))
#   define __VSF_CAL_SECTION(__SEC)     __attribute__((section (__SEC)))
#   define __VSF_CAL_WEAK_ALIAS(__ORIGIN, __ALIAS) \
                                        __attribute__((weakref(__VSF_STR(__ALIAS))))

#   define VSF_CAL_PACKED               __attribute__((packed))
#   define VSF_CAL_UNALIGNED            __unaligned
#   define VSF_CAL_TRANSPARENT_UNION    __attribute__((transparent_union))
#   define __VSF_CAL_ALIGN_OF(...)      __alignof__(__VA_ARGS__)

#   define __VSF_CAL_ISR(__VEC)         void __VEC(void)
#endif

/*----------------------------------------------------------------------------*
 * Signal & Interrupt Definition                                              *
 *----------------------------------------------------------------------------*/

  /*!< Macro to enable all interrupts. */
#if __IS_COMPILER_IAR__
#   define ENABLE_GLOBAL_INTERRUPT()            __enable_interrupt()
#else
#   define ENABLE_GLOBAL_INTERRUPT()            __enable_irq()
#endif

  /*!< Macro to disable all interrupts. */
#if __IS_COMPILER_IAR__
#   define DISABLE_GLOBAL_INTERRUPT()           ____disable_irq()

static VSF_CAL_ALWAYS_INLINE uint32_t ____disable_irq(void)
{
    uint32_t wPRIMASK = __get_interrupt_state();
    __disable_interrupt();
    return wPRIMASK;
}

#elif __IS_COMPILER_ARM_COMPILER_5__
#   define DISABLE_GLOBAL_INTERRUPT()           __disable_irq()
#elif __IS_COMPILER_ARM_COMPILER_6__ && !defined(__CMSIS_ARMCC_V6_H)
#   define DISABLE_GLOBAL_INTERRUPT()           __disable_irq()
#elif __IS_COMPILER_GCC_
#   define DISABLE_GLOBAL_INTERRUPT()           __disable_irq()
#else /* for other compilers, using gcc assembly syntax to implement */

#   define DISABLE_GLOBAL_INTERRUPT()           ____disable_irq()

static VSF_CAL_ALWAYS_INLINE uint32_t ____disable_irq(void)
{
    uint32_t cpsr;

    __asm__ __volatile__("mrs %[cpsr], primask\n"
                        "cpsid i\n"
                        : [cpsr] "=r"(cpsr));
    return cpsr & 0x1;
}
#endif

#if __IS_COMPILER_IAR__
#   define GET_GLOBAL_INTERRUPT_STATE()         __get_interrupt_state()
#   define SET_GLOBAL_INTERRUPT_STATE(__STATE)  __set_interrupt_state(__STATE)
typedef __istate_t   vsf_gint_state_t;
#elif __IS_COMPILER_ARM_COMPILER_5__ || __IS_COMPILER_ARM_COMPILER_6__
#   define GET_GLOBAL_INTERRUPT_STATE()         __get_PRIMASK()
#   define SET_GLOBAL_INTERRUPT_STATE(__STATE)  __set_PRIMASK(__STATE)
typedef uint32_t   vsf_gint_state_t;
#elif __IS_COMPILER_GCC__
#   define GET_GLOBAL_INTERRUPT_STATE()         __get_PRIMASK()
#   define SET_GLOBAL_INTERRUPT_STATE(__STATE)  __set_PRIMASK(__STATE)
typedef uint32_t   vsf_gint_state_t;
#else
typedef uint32_t   vsf_gint_state_t;
#   define GET_GLOBAL_INTERRUPT_STATE()         ____get_PRIMASK()

/**
  \brief   Get Priority Mask
  \details Returns the current state of the priority mask bit from the Priority Mask Register.
  \return               Priority Mask value
 */
VSF_CAL_ALWAYS_INLINE static inline uint32_t ____get_PRIMASK(void)
{
    unsigned int result;

    __asm__ volatile ("MRS %0, primask" : "=r" (result) );
    return(result);
}

#   define SET_GLOBAL_INTERRUPT_STATE(__STATE)  ____set_PRIMASK(__STATE)

/**
  \brief   Set Priority Mask
  \details Assigns the given value to the Priority Mask Register.
  \param [in]    priMask  Priority Mask
 */
VSF_CAL_ALWAYS_INLINE static inline void ____set_PRIMASK(uint32_t priMask)
{
    __asm__ volatile ("MSR primask, %0" : : "r" (priMask) : "memory");
}
#endif

/*----------------------------------------------------------------------------*
 * Startup Source Code                                                        *
 *----------------------------------------------------------------------------*/
#if     __IS_COMPILER_IAR__

#ifndef __VECTOR_TABLE
#   define __VECTOR_TABLE               __vector_table
#endif
#ifndef __VECTOR_TABLE_ATTRIBUTE
#   define __VECTOR_TABLE_ATTRIBUTE     @".intvec"
#endif
#ifndef __PROGRAM_START
#   define __PROGRAM_START              __iar_program_start
#endif
#ifndef __INITIAL_SP
#   define __INITIAL_SP                 CSTACK$$Limit
#endif

#elif   __IS_COMPILER_ARM_COMPILER_6__ || __IS_COMPILER_ARM_COMPILER_5__

#ifndef __VECTOR_TABLE
#   define __VECTOR_TABLE               __Vectors
#endif
#ifndef __VECTOR_TABLE_ATTRIBUTE
#   define __VECTOR_TABLE_ATTRIBUTE     ROOT SECTION("RESET")
#endif
#ifndef __PROGRAM_START
#   define __PROGRAM_START              __main
#endif
#ifndef __INITIAL_SP
#   define __INITIAL_SP                 Image$$ARM_LIB_STACK$$ZI$$Limit
#endif

#else   //__IS_COMPILER_GCC__ || __IS_COMPILER_LLVM__

#ifndef __PROGRAM_START
#define __PROGRAM_START           __cmsis_start

#endif

#ifndef __INITIAL_SP
#define __INITIAL_SP              __StackTop
#endif

#ifndef __STACK_LIMIT
#define __STACK_LIMIT             __StackLimit
#endif

#ifndef __VECTOR_TABLE
#define __VECTOR_TABLE            __Vectors
#endif

#ifndef __VECTOR_TABLE_ATTRIBUTE
#define __VECTOR_TABLE_ATTRIBUTE  __attribute((used, section(".vectors")))
#endif
#endif

/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif  /* end of __USE_ARM_COMPILER_H_PART_2__ */



