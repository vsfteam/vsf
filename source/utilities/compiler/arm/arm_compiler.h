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

#ifndef __USE_ARM_COMPILER_H_PART_1__
#define __USE_ARM_COMPILER_H_PART_1__

/*============================ INCLUDES ======================================*/

//! \name The macros to identify the compiler
//! @{

//! \note for IAR
#ifdef __IS_COMPILER_IAR__
#   undef __IS_COMPILER_IAR__
#endif
#if defined(__IAR_SYSTEMS_ICC__)
#   define __IS_COMPILER_IAR__                 1
#endif

//! \note for arm compiler 5
#ifdef __IS_COMPILER_ARM_COMPILER_5__
#   undef __IS_COMPILER_ARM_COMPILER_5__
#endif
#if ((__ARMCC_VERSION >= 5000000) && (__ARMCC_VERSION < 6000000))
#   define __IS_COMPILER_ARM_COMPILER_5__      1
#endif
//! @}

//! \note for arm compiler 6
#ifdef __IS_COMPILER_ARM_COMPILER_6__
#   undef __IS_COMPILER_ARM_COMPILER_6__
#endif
#if ((__ARMCC_VERSION >= 6000000) && (__ARMCC_VERSION < 7000000))
#   define __IS_COMPILER_ARM_COMPILER_6__      1
#endif

#ifdef __IS_COMPILER_LLVM__
#   undef  __IS_COMPILER_LLVM__
#endif
#if defined(__clang__) && !__IS_COMPILER_ARM_COMPILER_6__
#   define __IS_COMPILER_LLVM__                1
#else
//! \note for gcc
#ifdef __IS_COMPILER_GCC__
#   undef __IS_COMPILER_GCC__
#endif
#if defined(__GNUC__) && !(     __IS_COMPILER_ARM_COMPILER_6__                  \
                            ||  __IS_COMPILER_LLVM__                            \
                            ||  __IS_COMPILER_ARM_COMPILER_5__)
#   define __IS_COMPILER_GCC__                 1
#endif
//! @}
#endif
//! @}

#endif  /* end of __USE_ARM_COMPILER_H_PART_1__ */

/*========================== Multiple-Entry Start ============================*/

#if defined(__IS_COMPILER_IAR__) && __IS_COMPILER_IAR__
#   include <intrinsics.h>
#endif

#include "./type.h"
#include "../__common/__compiler.h"

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
#   define ROM_FLASH            _Pragma(__STR(location=".rom.flash")) const
#   define ROM_EEPROM           _Pragma(__STR(location=".rom.eeprom")) const
#   define NO_INIT              __no_init
#   define ROOT                 __root
#   define INLINE               inline
#   define NO_INLINE            __attribute__((noinline))
#   define ALWAYS_INLINE        inline __attribute__((always_inline))
#   define WEAK(...)            __weak
#   define RAMFUNC              __ramfunc
#   define __asm__              __asm
#   define __ALIGN(__N)         __attribute__((aligned (__N)))
#   define __AT_ADDR(__ADDR)    @ __ADDR
#   define __SECTION(__SEC)     __attribute__((section (__SEC)))
#   define __WEAK_ALIAS(__ORIGIN, __ALIAS) \
                                _Pragma(__STR(weak __ORIGIN=__ALIAS))
#   define PACKED               __attribute__((packed))
#   define UNALIGNED            __packed
#   define TRANSPARENT_UNION    __attribute__((transparent_union))
#   define __ALIGN_OF(...)      __ALIGNOF__(__VA_ARGS__)

#   define __IAR_STARTUP_DATA_INIT  __iar_data_init3

#   define __ISR(__VEC)       void __VEC(void)

#elif __IS_COMPILER_ARM_COMPILER_5__
#   define ROM_FLASH            __attribute__(( section( ".rom.flash"))) const
#   define ROM_EEPROM           __attribute__(( section( ".rom.eeprom"))) const
#   define NO_INIT              __attribute__( ( section( ".bss.noinit"),zero_init) )
#   define ROOT                 __attribute__((used))    
#   define INLINE               __inline
#   define NO_INLINE            __attribute__((noinline))
#   define ALWAYS_INLINE        __inline __attribute__((always_inline))
#   define WEAK(...)            __attribute__((weak))
#   define RAMFUNC              __attribute__((section (".textrw")))
#   define __asm__              __asm
#   define __ALIGN(__N)         __attribute__((aligned (__N))) 
#   define __AT_ADDR(__ADDR)    __attribute__((at(__ADDR)))
#   define __SECTION(__SEC)     __attribute__((section (__SEC)))
#   define __WEAK_ALIAS(__ORIGIN, __ALIAS) \
                                __attribute__((weakref(__STR(__ALIAS))))
                                
#   define PACKED               __attribute__((packed))
#   define UNALIGNED            __packed
#   define TRANSPARENT_UNION    __attribute__((transparent_union))
#   define __ALIGN_OF(...)      __alignof__(__VA_ARGS__)

#   define __ISR(__VEC)       void __VEC(void)

#elif __IS_COMPILER_ARM_COMPILER_6__
#   define ROM_FLASH            __attribute__(( section( ".rom.flash"))) const
#   define ROM_EEPROM           __attribute__(( section( ".rom.eeprom"))) const
#   define NO_INIT              __attribute__( ( section( ".bss.noinit")) )
#   define ROOT                 __attribute__((used))    
#   define INLINE               __inline
#   define NO_INLINE            __attribute__((noinline))
#   define ALWAYS_INLINE        __inline __attribute__((always_inline))
#   define WEAK(...)            __attribute__((weak))
#   define RAMFUNC              __attribute__((section (".textrw")))
#   define __asm__              __asm
#   define __ALIGN(__N)         __attribute__((aligned (__N))) 
#   define __AT_ADDR(__ADDR)    __attribute__((section (".ARM.__at_" #__ADDR)))
#   define __SECTION(__SEC)     __attribute__((section (__SEC)))
#   define __WEAK_ALIAS(__ORIGIN, __ALIAS) \
                                __attribute__((weakref(__STR(__ALIAS))))

#   define PACKED               __attribute__((packed))
#   define UNALIGNED            __unaligned
#   define TRANSPARENT_UNION    __attribute__((transparent_union))
#   define __ALIGN_OF(...)      __alignof__(__VA_ARGS__)

#   define __ISR(__VEC)       void __VEC(void)
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

static ALWAYS_INLINE uint32_t ____disable_irq(void) 
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

static ALWAYS_INLINE uint32_t ____disable_irq(void) 
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
__attribute__((always_inline)) static inline uint32_t ____get_PRIMASK(void)
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
__attribute__((always_inline)) static inline void ____set_PRIMASK(uint32_t priMask)
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

/**
  \brief   Initializes data and bss sections
  \details This default implementations initialized all data and additional bss
           sections relying on .copy.table and .zero.table specified properly
           in the used linker script.
  
 */
__attribute__((always_inline, __noreturn__)) static inline void __cmsis_start(void)
{
  extern void _start(void) __attribute__((__noreturn__));
  
  typedef struct {
    uint32_t const* src;
    uint32_t* dest;
    uint32_t  wlen;
  } __copy_table_t;
  
  typedef struct {
    uint32_t* dest;
    uint32_t  wlen;
  } __zero_table_t;
  
  extern const __copy_table_t __copy_table_start__;
  extern const __copy_table_t __copy_table_end__;
  extern const __zero_table_t __zero_table_start__;
  extern const __zero_table_t __zero_table_end__;

  for (__copy_table_t const* pTable = &__copy_table_start__; pTable < &__copy_table_end__; ++pTable) {
    for(uint32_t i=0u; i<pTable->wlen; ++i) {
      pTable->dest[i] = pTable->src[i];
    }
  }
 
  for (__zero_table_t const* pTable = &__zero_table_start__; pTable < &__zero_table_end__; ++pTable) {
    for(uint32_t i=0u; i<pTable->wlen; ++i) {
      pTable->dest[i] = 0u;
    }
  }
 
  _start();
}
  
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



