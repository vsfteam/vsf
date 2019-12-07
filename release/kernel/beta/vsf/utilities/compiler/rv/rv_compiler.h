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

#ifndef __USE_RV_COMPILER_H__
#define __USE_RV_COMPILER_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#ifndef VSF_UTILITIES_REQ___RV_HEADER_FILE__FROM_USR
#include "./RISCV/riscv_compiler.h"
#else
#include VSF_UTILITIES_REQ___RV_HEADER_FILE__FROM_USR
#endif


//! \name The macros to identify the compiler
//! @{

//! \note for IAR
#ifdef __IS_COMPILER_IAR__
#   undef __IS_COMPILER_IAR__
#endif
#if defined(__IAR_SYSTEMS_ICC__)
#   define __IS_COMPILER_IAR__                 1
#endif

#ifdef __IS_COMPILER_LLVM__
#   undef  __IS_COMPILER_LLVM__
#endif
#if defined(__clang__)
#   define __IS_COMPILER_LLVM__                1
#else
//! \note for gcc
#ifdef __IS_COMPILER_GCC__
#   undef __IS_COMPILER_GCC__
#endif
#if defined(__GNUC__) && !__IS_COMPILER_LLVM__
#   define __IS_COMPILER_GCC__                 1
#endif
//! @}
#endif
//! @}


#if __IS_COMPILER_IAR__
#   include <intrinsics.h>
#endif

#include "./type.h"
#include "../__common/__compiler.h"


/* -----------------  Start of section using anonymous unions  -------------- */
#if __IS_COMPILER_IAR__
  #pragma language=extended
#elif __IS_COMPILER_GCC__
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif

/*============================ MACROS ========================================*/

//! \brief 1 cycle nop operation
#ifndef NOP
    #define NOP()                       __asm__ __volatile__ ("nop");
#endif


//! \brief none standard memory types
#if __IS_COMPILER_IAR__
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
#   define UNALIGNED            __attribute__((packed))
#   define TRANSPARENT_UNION    __attribute__((transparent_union))
#   define __ALIGN_OF(...)      __ALIGNOF__(__VA_ARGS__)

#   define __IAR_STARTUP_DATA_INIT  __iar_data_init3

#   define __ISR(__VEC)       void __VEC(void)

#endif

/*----------------------------------------------------------------------------*
 * Signal & Interrupt Definition                                              *
 *----------------------------------------------------------------------------*/

  /*!< Macro to enable/disable all interrupts. */
#if __IS_COMPILER_IAR__
#   define ENABLE_GLOBAL_INTERRUPT()            __enable_interrupt()
#   define DISABLE_GLOBAL_INTERRUPT()           ____disable_irq()

static ALWAYS_INLINE uint32_t ____disable_irq(void) 
{
    uint32_t wPRIMASK = __get_interrupt_state();
    __disable_irq();
    return wPRIMASK & 0x1;
}
#else
#   define ENABLE_GLOBAL_INTERRUPT()            __enable_interrupt()
#   define DISABLE_GLOBAL_INTERRUPT()           __disable_interrupt()
#endif

#if __IS_COMPILER_IAR__
#   define GET_GLOBAL_INTERRUPT_STATE()         __get_interrupt_state()
#   define SET_GLOBAL_INTERRUPT_STATE(__STATE)  __set_interrupt_state(__STATE)
typedef __istate_t   vsf_gint_state_t;
#elif __IS_COMPILER_GCC__
#   define GET_GLOBAL_INTERRUPT_STATE()         __get_interrupt_state()
#   define SET_GLOBAL_INTERRUPT_STATE(__STATE)  __set_interrupt_state(__STATE)
typedef uint32_t   vsf_gint_state_t;
#endif

/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/

//! \brief for interrupt 
#include "./signal.h"

/*============================ PROTOTYPES ====================================*/
extern void vsf_stdio_init(void);

#endif
