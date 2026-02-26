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

#ifndef __USE_COMMON_COMPILER_H__
#define __USE_COMMON_COMPILER_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/

#include "vsf_cfg.h"

/*============================ MACROS ========================================*/

#ifndef __REG_MACRO__
#define __REG_MACRO__

#define __DEF_REG                                                               \
        union  {                                                                \
            struct {
#define __END_DEF_REG(__NAME, __BIT_LEN)                                        \
            };                                                                  \
            reg##__BIT_LEN##_t VALUE;                                           \
        } __NAME;

#define DEF_REG                 __DEF_REG
#define END_DEF_REG(__NAME, __BIT_LEN)                                          \
        __END_DEF_REG(__NAME, __BIT_LEN)

#define DEF_REG8                DEF_REG
#define END_DEF_REG8(__NAME)    END_DEF_REG(__NAME, 8)
#define DEF_REG16               DEF_REG
#define END_DEF_REG16(__NAME)   END_DEF_REG(__NAME, 16)
#define DEF_REG32               DEF_REG
#define END_DEF_REG32(__NAME)   END_DEF_REG(__NAME, 32)

#endif      // __REG_MACRO__

#ifndef __volatile__
#   define __volatile__         volatile
#endif

#undef __IS_COMPILER_SUPPORT_GNUC_EXTENSION__
#if defined(__GNUC__) || defined(__clang__)
#   define  __IS_COMPILER_SUPPORT_GNUC_EXTENSION__      1
#endif

#ifndef __IS_COMPILER_LLVM__
#   define __IS_COMPILER_LLVM__ 0
#endif

#ifndef __IS_COMPILER_GCC__
#   define __IS_COMPILER_GCC__  0
#endif

#ifndef __IS_COMPILER_IAR__
#   define __IS_COMPILER_IAR__  0
#endif

#ifndef __IS_COMPILER_TCC__
#   define __IS_COMPILER_TCC__  0
#endif

#ifndef MAX_CONSTRUCTOR_PRIORITY
#   if __IS_COMPILER_GCC__
#       define MAX_CONSTRUCTOR_PRIORITY                 65535
#   elif __IS_COMPILER_LLVM__
#       define MAX_CONSTRUCTOR_PRIORITY                 0xFFFFFFFF
#   endif
#endif

//! \brief none standard memory types
#if __IS_COMPILER_LLVM__
#   ifdef __APPLE__
#       define VSF_CAL_NO_INIT
#   else
#       define VSF_CAL_NO_INIT          __attribute__((__section__(".bss.noinit")))
#   endif
#   define VSF_CAL_NO_RETURN            __attribute__((__noreturn__))
#   define VSF_CAL_ROOT                 __attribute__((__used__))
#   define VSF_CAL_INLINE               __inline__
#   define VSF_CAL_NO_INLINE            __attribute__((__noinline__))
#   define VSF_CAL_ALWAYS_INLINE        __inline__ __attribute__((__always_inline__))
#   define VSF_CAL_WEAK(...)            __attribute__((__weak__))
#   ifdef __APPLE__
#       define VSF_CAL_RAMFUNC          Not Supported by Apple LLVM
#   else
#       define VSF_CAL_RAMFUNC          __attribute__((__section__(".ramfunc")))
#   endif
#   define __asm__                      __asm
#   define __VSF_CAL_ALIGN(__N)         __attribute__((__aligned__(__N)))
#   define __VSF_CAL_AT_ADDR(__ADDR)    Not Supported by LLVM
#   ifdef __APPLE__
#       define __VSF_CAL_SECTION(__SEC_STR)
#   else
#       define __VSF_CAL_SECTION(__SEC_STR) __attribute__((__section__(__SEC_STR)))
#   endif
#   define __VSF_CAL_WEAK_ALIAS(__ORIGIN, __ALIAS)                              \
                                        __asm__(".weak " #__ALIAS);             \
                                        __asm__(".equ " #__ALIAS ", " #__ORIGIN)

#   define VSF_CAL_PACKED               __attribute__((__packed__))
//#   define VSF_CAL_UNALIGNED            __attribute__((packed))
#   undef VSF_CAL_UNALIGNED                                             //! llvm doesn't support this
#   define VSF_CAL_NAKED                __attribute__((naked))
#   define VSF_CAL_TRANSPARENT_UNION    __attribute__((__transparent_union__))
#   define __VSF_CAL_ALIGN_OF(...)      __alignof__(__VA_ARGS__)

#   define __VSF_CAL_ISR(__VEC)         void __VEC(void)
#   define __VSF_CAL_WRAPPER(__API)     __wrap_ ## __API

#elif  __IS_COMPILER_GCC__
#   define VSF_CAL_NO_INIT              __attribute__((section(".bss.noinit")))
#   define VSF_CAL_NO_RETURN            __attribute__((noreturn))
#   define VSF_CAL_ROOT                 __attribute__((used))
#   define VSF_CAL_INLINE               inline
#   define VSF_CAL_NO_INLINE            __attribute__((noinline))
#   define VSF_CAL_ALWAYS_INLINE        inline __attribute__((always_inline))
#   define VSF_CAL_WEAK(...)            __attribute__((weak))
#   define VSF_CAL_RAMFUNC              __attribute__((section(".ramfunc")))
#   define __asm__                      __asm
#   define __VSF_CAL_ALIGN(__N)         __attribute__((aligned(__N)))
#   define __VSF_CAL_AT_ADDR(__ADDR)    Not Supported by GCC
#   define __VSF_CAL_SECTION(__SEC_STR) __attribute__((section(__SEC_STR)))
#   define VSF_CAL_SECTION_DECLARE(__SEC)   extern void *__start_ ## __SEC;\
                                            extern void *__stop_ ## __SEC;
#   define VSF_CAL_SECTION_START(__SEC) __start_ ## __SEC
#   define VSF_CAL_SECTION_END(__SEC)   __stop_ ## __SEC
#   define VSF_CAL_SECTION_SIZE(__SEC)    (__VSF_CAL_SECTION_END(__SEC) - __VSF_CAL_SECTION_START(__SEC))
#   define __VSF_CAL_WEAK_ALIAS(__ORIGIN, __ALIAS) \
                                        __attribute__((weakref(__VSF_STR(__ORIGIN))))

#   define VSF_CAL_PACKED               __attribute__((packed))
//#   define VSF_CAL_UNALIGNED            __attribute__((packed))
#   undef VSF_CAL_UNALIGNED                                             //! gcc doesn't support this
#   define VSF_CAL_NAKED                __attribute__((naked))
#   define VSF_CAL_TRANSPARENT_UNION    __attribute__((transparent_union))
#   define __VSF_CAL_ALIGN_OF(...)      __alignof__(__VA_ARGS__)

#   define __VSF_CAL_ISR(__VEC)         void __VEC(void)
#   define __VSF_CAL_WRAPPER(__API)     __wrap_ ## __API

#elif  __IS_COMPILER_TCC__
#   define VSF_CAL_NO_INIT              __attribute__((section( ".bss.noinit")))
#   define VSF_CAL_NO_RETURN            __attribute__((noreturn))
#   define VSF_CAL_ROOT                 __attribute__((used))
#   define VSF_CAL_INLINE               inline
#   define VSF_CAL_NO_INLINE            __attribute__((noinline))
#   define VSF_CAL_ALWAYS_INLINE        inline __attribute__((always_inline))
#   define VSF_CAL_WEAK(...)            __attribute__((weak))
#   define __asm__                      __asm
#   define __VSF_CAL_ALIGN(__N)         __attribute__((aligned (__N)))
#   define __VSF_CAL_AT_ADDR(__ADDR)    Not Supported by TCC
#   define __VSF_CAL_SECTION(__SEC_STR)
#   define __VSF_CAL_WEAK_ALIAS(__ORIGIN, __ALIAS) \
                                        __attribute__((weakref(__VSF_STR(__ORIGIN))))

#   define VSF_CAL_PACKED               __attribute__((packed))
#   define VSF_CAL_UNALIGNED            __attribute__((packed))
#   define VSF_CAL_NAKED                __attribute__((naked))
#   define VSF_CAL_TRANSPARENT_UNION    __attribute__((transparent_union))
#   define __VSF_CAL_ALIGN_OF(...)      __alignof__(__VA_ARGS__)

#   define __VSF_CAL_ISR(__VEC)         void __VEC(void)
#   define __VSF_CAL_WRAPPER(__API)     __wrap_ ## __API
#endif

#define VSF_CAL_WEAK_ALIAS(__ORIGIN, __ALIAS)                                   \
                                        __VSF_CAL_WEAK_ALIAS(__ORIGIN, __ALIAS)
#define VSF_CAL_AT_ADDR(__ADDR)         __VSF_CAL_AT_ADDR(__ADDR)
#define VSF_CAL_ALIGN(__N)              __VSF_CAL_ALIGN(__N)

#define VSF_CAL_SECTION(__SEC_STR)      __VSF_CAL_SECTION(__SEC_STR)

#ifdef __VSF_CAL_WRAPPER
#   define VSF_CAL_WRAPPER(__API)      __VSF_CAL_WRAPPER(__API)
#endif

#if (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L) && !defined(__cplusplus)
#define VSF_CAL_ALIGN_OF(__TYPE)        __VSF_CAL_ALIGN_OF(__TYPE)
#define VSF_CAL_ALIGN_WITH(__TYPE)      VSF_CAL_ALIGN(VSF_CAL_ALIGN_OF(__TYPE))
#define VSF_CAL_ISR(__VECT)             __VSF_CAL_ISR(__VECT)
#else
#define VSF_CAL_ALIGN_OF(...)           __VSF_CAL_ALIGN_OF(__VA_ARGS__)
#define VSF_CAL_ALIGN_WITH(...)         VSF_CAL_ALIGN(VSF_CAL_ALIGN_OF(__VA_ARGS__))
#define VSF_CAL_ISR(...)                __VSF_CAL_ISR(__VA_ARGS__)
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
#pragma clang diagnostic ignored "-Wunknown-warning-option"
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
#pragma clang diagnostic ignored "-Wbuiltin-requires-header"
#pragma clang diagnostic ignored "-Wmain-return-type"
#pragma clang diagnostic ignored "-Wextern-c-compat"
#pragma clang diagnostic ignored "-Wunused-label"
#pragma clang diagnostic ignored "-Wgnu-variable-sized-type-not-at-end"
#pragma clang diagnostic ignored "-Wself-assign"
/*! \NOTE do not ignore following warning unless you take the risk by yourself */
//#pragma clang diagnostic ignored "-Wbitfield-constant-conversion"
//#pragma clang diagnostic ignored "-Wpointer-integer-compare"

//#pragma clang diagnostic ignored "-Wno-sometimes-uninitialized"
//#pragma clang diagnostic ignored "-Wdeprecated-declarations"
//#pragma clang diagnostic ignored "-Wunused-variable"

#elif __IS_COMPILER_GCC__

#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wmissing-braces"
#pragma GCC diagnostic ignored "-Wempty-body"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wswitch"
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wpadded"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wunused-value"
#pragma GCC diagnostic ignored "-Wcomment"
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#pragma GCC diagnostic ignored "-Wnonnull-compare"
#pragma GCC diagnostic ignored "-Woverflow"

#ifndef __cplusplus
#   pragma GCC diagnostic ignored "-Wmissing-prototypes"
#   pragma GCC diagnostic ignored "-Wc++-compat"
#   pragma GCC diagnostic ignored "-Wbad-function-cast"
#   pragma GCC diagnostic ignored "-Wint-conversion"
#   pragma GCC diagnostic ignored "-Woverride-init-side-effects"
#else

#endif

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

//! declaration is not visible outside of function
#pragma diag_suppress=pe231

//! expression has no effect
#pragma diag_suppress=pe174

//! label was declared but never referenced
#pragma diag_suppress=pe177

//!	struct or union declares no named members
#pragma diag_suppress=pe618

//,Pe186,Pe111,,pe128,,,Pe1866,Pe064,Pa039

#endif

/*----------------------------------------------------------------------------*
 * Warning Emphasize                                                          *
 *----------------------------------------------------------------------------*/

#if defined(__VSF_DEBUG__)
#   if defined(__clang__) //__IS_COMPILER_LLVM__
#       pragma clang diagnostic warning "-Wcast-align"
#   elif __IS_COMPILER_GCC__
#       pragma GCC diagnostic warning "-Wcast-align"
#   endif
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

#ifdef __cplusplus
}
#endif

#endif
