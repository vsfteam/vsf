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

#ifndef __IS_COMPILER_LLVM__
#   define __IS_COMPILER_LLVM__     0
#endif

#ifndef __IS_COMPILER_GCC__
#   define __IS_COMPILER_GCC__      0
#endif

#ifndef __IS_COMPILER_IAR__
#   define __IS_COMPILER_IAR__      0
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
 * Warning Mitigation                                                         *
 *----------------------------------------------------------------------------*/
#if defined(__clang__) //__IS_COMPILER_LLVM__
#pragma clang diagnostic ignored "-Wmissing-declarations"
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic ignored "-Wmicrosoft-anon-tag"
#pragma clang diagnostic ignored "-Wmissing-braces"
#pragma clang diagnostic ignored "-Wconstant-conversion"
#pragma clang diagnostic ignored "-Wmicrosoft-enum-forward-reference"
#pragma clang diagnostic ignored "-Wbuiltin-requires-header"
#pragma clang diagnostic ignored "-Winitializer-overrides" 
#pragma clang diagnostic ignored "-Wbraced-scalar-init" 
#pragma clang diagnostic ignored "-Wempty-body" 
#pragma clang diagnostic ignored "-Wgnu-empty-struct"
#pragma clang diagnostic ignored "-Wint-conversion" 
#pragma clang diagnostic ignored "-Wint-to-pointer-cast" 
#pragma clang diagnostic ignored "-Wmicrosoft-include"
#pragma clang diagnostic ignored "-Wpragma-pack" 
#pragma clang diagnostic ignored "-Wunused-function" 
#pragma clang diagnostic ignored "-Wswitch"
#pragma clang diagnostic ignored "-Wembedded-directive"
#pragma clang diagnostic ignored "-Wundef"
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#pragma clang diagnostic ignored "-Wpadded"
#pragma clang diagnostic ignored "-Wnewline-eof"
#pragma clang diagnostic ignored "-Wduplicate-enum"
#pragma clang diagnostic ignored "-Wextra-semi"
#pragma clang diagnostic ignored "-Wextra-semi-stmt"
#pragma clang diagnostic ignored "-Wzero-length-array"
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#pragma clang diagnostic ignored "-Wnonportable-include-path"
#pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#pragma clang diagnostic ignored "-Wc++-compat"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma clang diagnostic ignored "-Wcast-qual"
#pragma clang diagnostic ignored "-Wbad-function-cast"
#pragma clang diagnostic ignored "-Wswitch-enum"
#pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#pragma clang diagnostic ignored "-Wtautological-pointer-compare"
#pragma clang diagnostic ignored "-Wunused-value" 

/*! \NOTE do not ignore following warning unless you take the risk by yourself */
//#pragma clang diagnostic ignored "-Wbitfield-constant-conversion"
//#pragma clang diagnostic ignored "-Wpointer-integer-compare" 

//#pragma clang diagnostic ignored "-Wno-sometimes-uninitialized" 
//#pragma clang diagnostic ignored "-Wdeprecated-declarations" 
//#pragma clang diagnostic ignored "-Wunused-variable"  

#elif __IS_COMPILER_GCC__
// TODO: validate diagnositc ignored for GCC
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
//#pragma GCC diagnostic ignored "-Wmicrosoft-anon-tag"
#pragma GCC diagnostic ignored "-Wmissing-braces"
//#pragma GCC diagnostic ignored "-Wconstant-conversion"
//#pragma GCC diagnostic ignored "-Wmicrosoft-enum-forward-reference"
//#pragma GCC diagnostic ignored "-Wbuiltin-requires-header"
//#pragma GCC diagnostic ignored "-Winitializer-overrides" 
//#pragma GCC diagnostic ignored "-Wbraced-scalar-init" 
#pragma GCC diagnostic ignored "-Wempty-body" 
//#pragma GCC diagnostic ignored "-Wgnu-empty-struct"
#pragma GCC diagnostic ignored "-Wint-conversion" 
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast" 
//#pragma GCC diagnostic ignored "-Wmicrosoft-include"
//#pragma GCC diagnostic ignored "-Wpragma-pack" 
#pragma GCC diagnostic ignored "-Wunused-function" 
#pragma GCC diagnostic ignored "-Wswitch"
//#pragma GCC diagnostic ignored "-Wembedded-directive"
#pragma GCC diagnostic ignored "-Wundef"
//#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#pragma GCC diagnostic ignored "-Wpadded"
//#pragma GCC diagnostic ignored "-Wnewline-eof"
//#pragma GCC diagnostic ignored "-Wduplicate-enum"
//#pragma GCC diagnostic ignored "-Wextra-semi"
//#pragma GCC diagnostic ignored "-Wextra-semi-stmt"
//#pragma GCC diagnostic ignored "-Wzero-length-array"
#pragma GCC diagnostic ignored "-Wmissing-prototypes"
//#pragma GCC diagnostic ignored "-Wimplicit-int-conversion"
//#pragma GCC diagnostic ignored "-Wnonportable-include-path"
//#pragma GCC diagnostic ignored "-Wmissing-variable-declarations"
#pragma GCC diagnostic ignored "-Wc++-compat"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wbad-function-cast"
#pragma GCC diagnostic ignored "-Wswitch-enum"
//#pragma GCC diagnostic ignored "-Wdisabled-macro-expansion"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
//#pragma GCC diagnostic ignored "-Wtautological-pointer-compare"
#pragma GCC diagnostic ignored "-Wunused-value" 
#pragma GCC diagnostic ignored "-Wcomment"
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#pragma GCC diagnostic ignored "-Wnonnull-compare"

/*! \NOTE do not ignore following warning unless you take the risk by yourself */
//#pragma GCC diagnostic ignored "-Wbitfield-constant-conversion"
//#pragma GCC diagnostic ignored "-Wpointer-integer-compare" 
//
//#pragma GCC diagnostic ignored "-Wno-sometimes-uninitialized" 
//#pragma GCC diagnostic ignored "-Wdeprecated-declarations" 
//#pragma GCC diagnostic ignored "-Wunused-variable"  

#elif __IS_COMPILER_IAR__

//! undefined behavior: the order of volatile accesses is undefined in this statement 
#pragma diag_suppress=Pa082

//! Typedef name has already been declared (with same type)
#pragma diag_suppress=pe301

//! enumerated type mixed with another type
#pragma diag_suppress=pe188

//! extra ";" ignored
#pragma diag_suppress=pe381

//! enumerated type mixed with another enumerated type 
#pragma diag_suppress=pa089

//! use of address of unaligned structure member 
#pragma diag_suppress=pa039

//,Pe186,Pe111,,pe128,,,Pe1866,Pe064,Pa039

#endif

/*----------------------------------------------------------------------------*
 * Warning Emphasize                                                          *
 *----------------------------------------------------------------------------*/
#if defined(__clang__) //__IS_COMPILER_LLVM__
#pragma clang diagnostic warning "-Wcast-align"
#elif __IS_COMPILER_GCC__

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
