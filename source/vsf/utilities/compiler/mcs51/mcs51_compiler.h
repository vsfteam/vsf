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

#ifndef __USE_ARM_COMPILER_H__
#define __USE_ARM_COMPILER_H__

/*============================ INCLUDES ======================================*/
#include "vsf_usr_cfg.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

//! \name The macros to identify the compiler
//! @{

//! \note for IAR
#ifdef __IS_COMPILER_IAR__
#   undef __IS_COMPILER_IAR__
#endif
#if defined(__IAR_SYSTEMS_ICC__)
#   define __IS_COMPILER_IAR__          1
#endif


//! \note for arm compiler 5
#ifdef __IS_COMPILER_51_KEIL__
#   undef __IS_COMPILER_51_KEIL__
#endif
#if defined(__C51__) || defined(__CX51__)
#   define __IS_COMPILER_51_KEIL__      1
#endif
//! @}

#undef __IS_COMPILER_SUPPORT_GNUC_EXTENSION__
#if defined(__GNUC__)
#   define  __IS_COMPILER_SUPPORT_GNUC_EXTENSION__      1
#endif

#if __IS_COMPILER_IAR__
#   include <intrinsics.h>
#endif


#include "./type.h"

/* -----------------  Start of section using anonymous unions  -------------- */
#if __IS_COMPILER_51_KEIL__
  //#pragma push
  //#pragma anon_unions
#elif __IS_COMPILER_IAR__
  #pragma language=extended
#else
  #warning Not supported compiler type
#endif

/*============================ MACROS ========================================*/

#ifndef DEF_REG
#define DEF_REG                     \
        union  {                    \
            struct {
#endif
    
#ifndef END_DEF_REG
#define END_DEF_REG(__NAME)         \
            };                      \
            reg32_t Value;          \
        }__NAME;
#endif

#ifndef __REG_MACRO__
#define __REG_MACRO__
#endif


#ifndef REG_RSVD_0x10
#define REG_RSVD_0x10                   \
    reg32_t                     : 32;   \
    reg32_t                     : 32;   \
    reg32_t                     : 32;   \
    reg32_t                     : 32;   
#endif
#ifndef REG_RSVD_0x80       
#define REG_RSVD_0x80                   \
            REG_RSVD_0x10               \
            REG_RSVD_0x10               \
            REG_RSVD_0x10               \
            REG_RSVD_0x10               \
            REG_RSVD_0x10               \
            REG_RSVD_0x10               \
            REG_RSVD_0x10               \
            REG_RSVD_0x10
#endif

#ifndef REG_RSVD_0x100                 
#define REG_RSVD_0x100                  \
            REG_RSVD_0x80               \
            REG_RSVD_0x80
#endif

#ifndef REG_RSVD_0x800
#define REG_RSVD_0x800                  \
            REG_RSVD_0x100              \
            REG_RSVD_0x100              \
            REG_RSVD_0x100              \
            REG_RSVD_0x100              \
            REG_RSVD_0x100              \
            REG_RSVD_0x100              \
            REG_RSVD_0x100              \
            REG_RSVD_0x100
#endif


//! \brief The mcu memory align mode
# define MCU_MEM_ALIGN_SIZE             sizeof(int)

#ifndef __volatile__
#define __volatile__                    volatile
#endif

//! \brief 1 cycle nop operation
#ifndef NOP
    #define NOP()                       __asm__ __volatile__ ("nop");
#endif

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   ifndef __STR
#       define __STR(__STRING)      #__STRING  
#   endif
#   ifndef STR
#       define STR(__STRING)        __STR(__STRING)
#   endif
#else
#   ifndef __STR
#       define __STR(...)           #__VA_ARGS__  
#   endif
#   ifndef STR
#       define STR(...)             __STR(__VA_ARGS__)
#   endif
#endif

//! \brief none standard memory types
#if __IS_COMPILER_IAR__
#   define ROM_FLASH            _Pragma(__STR(location=".rom.flash")) const
#   define ROM_EEPROM           _Pragma(__STR(location=".rom.eeprom")) const
#   define NO_INIT              __no_init
#   define ROOT                 __root
#   define INLINE               inline
#   define NO_INLINE            _Pragma("optimisze=no_inline")
#   define ALWAYS_INLINE        _Pragma("inline=forced")
#   define WEAK(...)            _Pragma(__STR(weak __VA_ARGS__))
#   define RAMFUNC              __ramfunc
#   define __asm__              __asm
#   define __ALIGN(__N)         //_Pragma("data_alignment=" __STR(__N))         //!< 8051 does not have any alignment restrictions
#   define __AT_ADDR(__ADDR)    @ __ADDR
#   define __SECTION(__SEC)     _Pragma(__STR(section=__SEC))
#   define __WEAK_ALIAS(__ORIGIN, __ALIAS) \
                                _Pragma(__STR(weak __ORIGIN=__ALIAS))
#   define PACKED               
#   define UNALIGNED            
#   define TRANSPARENT_UNION    
#   define __ALIGN_OF(...)      __ALIGNOF__(__VA_ARGS__)

#define __ISR(__VEC)                                                            \
        _Pragma(__STR(vector=__VEC))                                            \
        __interrupt void vect_##__VEC##_handler(void)  

//! TODO: Need further investigation about the negative effect
#   define __stackless

#   define __IAR_STARTUP_DATA_INIT  __INIT_XDATA_I
#elif __IS_COMPILER_51_KEIL__
#   define ROM_FLASH            const
#   define ROM_EEPROM           const
#   define NO_INIT              
#   define ROOT                 
#   define INLINE               
#   define NO_INLINE            
#   define ALWAYS_INLINE        
#   define WEAK(__ANYTHING)            
#   define RAMFUNC              
#   define __asm__              __asm
#   define __ALIGN(__N)         //_Pragma("data_alignment=" __STR(__N))         //!< 8051 does not have any alignment restrictions
#   define __AT_ADDR(__ADDR)    
#   define __SECTION(__SEC)     
#   define __WEAK_ALIAS(__ORIGIN, __ALIAS) 
#   define PACKED               
#   define UNALIGNED            
#   define TRANSPARENT_UNION    
#   define __ALIGN_OF(__ANYTHING)      

#define __ISR(__VEC)                                                            \
        void vect_##__VEC##_handler(void) interrupt __VEC

//! TODO: Need further investigation about the negative effect
#   define __stackless

#   define __IAR_STARTUP_DATA_INIT  __INIT_XDATA_I
#endif

#define WEAK_ALIAS(__ORIGIN, __ALIAS)                                           \
                            __WEAK_ALIAS(__ORIGIN, __ALIAS)
#define AT_ADDR(__ADDR)     __AT_ADDR(__ADDR)
#define ALIGN(__N)          __ALIGN(__N)
#define SECTION(__SEC)      __SECTION(__SEC)
        
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#define ALIGN_OF(__TYPE)       __ALIGN_OF(__TYPE)
#define ALIGN_WITH(__TYPE)     ALIGN(ALIGN_OF(__TYPE))
#define ISR(__VECT)            __ISR(__VECT)    
#else
#define ALIGN_OF(...)       __ALIGN_OF(__VA_ARGS__)
#define ALIGN_WITH(...)     ALIGN(ALIGN_OF(__VA_ARGS__))
#define ISR(...)            __ISR(__VA_ARGS__)
#endif
        
/*----------------------------------------------------------------------------*
 * Signal & Interrupt Definition                                              *
 *----------------------------------------------------------------------------*/

  /*!< Macro to enable all interrupts. */
#if __IS_COMPILER_IAR__
#   define ENABLE_GLOBAL_INTERRUPT()            __enable_interrupt()
#elif __IS_COMPILER_51_KEIL__
#   define ENABLE_GLOBAL_INTERRUPT()            __enable_interrupt()
#endif

  /*!< Macro to disable all interrupts. */
#if __IS_COMPILER_IAR__
#   define DISABLE_GLOBAL_INTERRUPT()           ____disable_irq()

INLINE
static uint32_t ____disable_irq(void) 
{
    __istate_t s = __get_interrupt_state();
    __disable_interrupt();
    return s;
}
#elif __IS_COMPILER_51_KEIL__
#   define DISABLE_GLOBAL_INTERRUPT()           __disable_interrupt()
#endif

#if __IS_COMPILER_IAR__
#   define GET_GLOBAL_INTERRUPT_STATE()         __get_interrupt_state()
#   define SET_GLOBAL_INTERRUPT_STATE(__STATE)  __set_interrupt_state(__STATE)
typedef __istate_t   vsf_gint_state_t;
#elif __IS_COMPILER_51_KEIL__
#   define GET_GLOBAL_INTERRUPT_STATE()         __get_interrupt_state()
#   define SET_GLOBAL_INTERRUPT_STATE(__STATE)  __set_interrupt_state(__STATE)
typedef uint8_t   vsf_gint_state_t;
#endif


/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/

//! \brief for interrupt 
#include "./signal.h"

/*============================ PROTOTYPES ====================================*/
extern void vsf_stdio_init(void);

#endif
