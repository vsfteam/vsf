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

#ifndef __USE_COMMON_COMPILER_H__
#define __USE_COMMON_COMPILER_H__

/*============================ INCLUDES ======================================*/
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

#undef __IS_COMPILER_SUPPORT_GNUC_EXTENSION__
#if defined(__GNUC__) || defined(__clang__)
#   define  __IS_COMPILER_SUPPORT_GNUC_EXTENSION__      1
#endif

//! \brief none standard memory types
#if __IS_COMPILER_LLVM__
#   define ROM_FLASH            __attribute__(( __section__( ".rom.flash"))) const
#   define ROM_EEPROM           __attribute__(( __section__( ".rom.eeprom"))) const
#   define NO_INIT              __attribute__(( __section__( ".bss.noinit")))
#   define ROOT                 __attribute__((__used__))    
#   define INLINE               __inline__
#   define NO_INLINE            __attribute__ ((__noinline__))
#   define ALWAYS_INLINE        __inline__ __attribute__((__always_inline__))
#   define WEAK(...)            __attribute__((__weak__))
#   define RAMFUNC              __attribute__((__section__ (".textrw")))
#   define __asm__              __asm
#   define __ALIGN(__N)         __attribute__((__aligned__(__N)))
#   define __AT_ADDR(__ADDR)    __attribute__((__section__(".ARM.__at_" #__ADDR)))
#   define __SECTION(__SEC)     __attribute__((__section__(__SEC)))
#   define __WEAK_ALIAS(__ORIGIN, __ALIAS)                                      \
                                __asm__(".weak " #__ALIAS);                     \
                                __asm__(".equ " #__ALIAS ", " #__ORIGIN)

#   define PACKED               __attribute__((__packed__))
#   define UNALIGNED            __attribute__((__packed__))
#   define TRANSPARENT_UNION    __attribute__((__transparent_union__))
#   define __ALIGN_OF(...)      __alignof__(__VA_ARGS__)

#   define __ISR(__VEC)       void __VEC(void)

#elif  __IS_COMPILER_GCC__ 
#   define ROM_FLASH            __attribute__(( section( ".rom.flash"))) const
#   define ROM_EEPROM           __attribute__(( section( ".rom.eeprom"))) const
#   define NO_INIT              __attribute__(( section( ".bss.noinit")))
#   define ROOT                 __attribute__((used))    
#   define INLINE               inline
#   define NO_INLINE            __attribute__((noinline))
#   define ALWAYS_INLINE        inline __attribute__((always_inline))
#   define WEAK(...)            __attribute__((weak))
#   define RAMFUNC              __attribute__((section (".textrw")))
#   define __asm__              __asm
#   define __ALIGN(__N)         __attribute__((aligned (__N)))
#   define __AT_ADDR(__ADDR)    __section(".ARM.__at_" #__ADDR) 
#   define __SECTION(__SEC)     __attribute__((section (__SEC)))
#   define __WEAK_ALIAS(__ORIGIN, __ALIAS) \
                                __attribute__((weakref(__STR(__ALIAS))))

#   define PACKED               __attribute__((packed))
#   define UNALIGNED            __attribute__((packed))
#   define TRANSPARENT_UNION    __attribute__((transparent_union))
#   define __ALIGN_OF(...)    __alignof__(__VA_ARGS__)

#   define __ISR(__VEC)       void __VEC(void)
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

/*----------------------------------------------------------------------------*
 * Startup Source Code                                                        *
 *----------------------------------------------------------------------------*/

/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/
extern void vsf_stdio_init(void);

#endif
