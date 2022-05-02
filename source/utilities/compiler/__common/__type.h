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

#ifndef __COMMON_TYPE_H__
#define __COMMON_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/

#ifndef ENABLED
#   define ENABLED                              1
#endif

#ifndef DISABLED
#   define DISABLED                             0
#endif

/* do not modify this */
#include "vsf_usr_cfg.h"

/*============================ TYPES =========================================*/

//! \name standard error code
//! @{
typedef enum {
    VSF_ERR_NOT_READY                       =1,     //!< service not ready yet
    VSF_ERR_NONE                            =0,     //!< none error
    VSF_ERR_UNKNOWN                         =-1,    //!< unknown error
    VSF_ERR_NOT_SUPPORT                     =-2,    //!< function not supported
    VSF_ERR_NOT_AVAILABLE                   =-4,    //!< service not available
    VSF_ERR_NOT_ACCESSABLE                  =-5,    //!< target not acceesable
    VSF_ERR_NOT_ENOUGH_RESOURCES            =-6,    //!< no enough resources
    VSF_ERR_FAIL                            =-7,    //!< failed
    VSF_ERR_INVALID_PARAMETER               =-8,    //!< invalid parameter
    VSF_ERR_INVALID_RANGE                   =-9,    //!< invalid range
    VSF_ERR_INVALID_PTR                     =-10,   //!< invalid pointer
    VSF_ERR_INVALID_KEY                     =-11,   //!< invalid key
    VSF_ERR_IO                              =-12,   //!< IO error
    VSF_ERR_ALREADY_EXISTS                  =-13,   //!< already exists
    VSF_ERR_REQ_ALREADY_REGISTERED          =-13,   //!< request all ready exist
    VSF_ERR_BUG                             =-14,   //!< bug
    VSF_ERR_OVERRUN                         =-15,   //!< overrun
    VSF_ERR_PROVIDED_RESOURCE_NOT_SUFFICIENT
                                            =-17,   //!< the resource provided by user is not sufficient
    VSF_ERR_PROVIDED_RESOURCE_NOT_ALIGNED
                                            =-18,   //!< the provided resource is not aligned to certain size (2^N)
    VSF_ERR_TIMEOUT                         =-19,
} vsf_err_t;
//! @}

#ifndef __FSM_RT_TYPE__
#define __FSM_RT_TYPE__
//! \name finit state machine state
//! @{
typedef enum {
    fsm_rt_err                              = -1,    //!< fsm error, error code can be get from other interface
    fsm_rt_cpl                              = 0,     //!< fsm complete
    fsm_rt_on_going                         = 1,     //!< fsm on-going
    fsm_rt_yield                            = 1,
    fsm_rt_wait_for_obj                     = 2,     //!< fsm wait for object
    fsm_rt_wait_for_evt                     = 2,
    fsm_rt_wfe                              = 2,
    fsm_rt_asyn                             = 3,     //!< fsm asynchronose complete, you can check it later.
    fsm_rt_user                             = 4
} fsm_rt_t;
//! @}

#endif

/*============================ INCLUDES ======================================*/

/*! \note IMPORTANT
 *!       - Everytime when you try to add some 'common' header file including
 *!         into __type.h, please take more time to think your decision, and
 *!         make sure that the header file is available in all the compiler
 *!         environment and platforms.
 *!
 *!       - If you know what you are doing, and there is still some concerns
 *!         which others need to know, please DO add necessary information
 *!         besides the "#include <xxxx>"
 *!
 *!       - If you want to introduce a system header file (as part of c
 *!         standard) and you haven't encounter any known case where the header
 *!         file is absent, we can assume that it is safe to put it in __type.h.
 *!         If you do encounter scenarios where the header files is missing,
 *!         please move it to the plaform specific type.h
 */

#if !defined(__USE_LOCAL_LIBC__)
#   include <ctype.h>
#   include <stdlib.h>
#   include <string.h>
#   include <stddef.h>
#   include <assert.h>
#else
#   undef __USE_LOCAL_LIBC__
#endif

/*============================ MACROS ========================================*/

//! \brief system macros
#ifndef VSF_ASSERT
#   define VSF_ASSERT                       assert
#endif

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   ifndef __VSF_STR
#       define __VSF_STR(__STRING)          #__STRING
#   endif
#   ifndef VSF_STR
#       define VSF_STR(__STRING)            __VSF_STR(__STRING)
#   endif
#else
#   ifndef __VSF_STR
#       define __VSF_STR(...)               #__VA_ARGS__
#   endif
#   ifndef VSF_STR
#       define VSF_STR(...)                 __VSF_STR(__VA_ARGS__)
#   endif
#endif

#define vsf_max(__a, __b)                   (((__a) > (__b)) ? (__a) : (__b))
#define vsf_min(__a, __b)                   (((__a) < (__b)) ? (__a) : (__b))

#ifndef dimof
#   define dimof(__arr)                     (sizeof(__arr) / sizeof((__arr)[0]))
#endif

#ifndef offset_of
#   define offset_of(__s, __m)              (uintptr_t)(&(((__s *)0)->__m))
#endif
#ifndef container_of
#   define container_of(__ptr, __type, __member)                                \
        ((__type *)((uintptr_t)(__ptr) - offset_of(__type, __member)))
#endif
#ifndef safe_container_of
#   define safe_container_of(__ptr, __type, __member)                           \
        (__ptr ? container_of(__ptr, __type, __member) : NULL)
#endif

static inline int vsf_sign(int x)
{
    return ((int)((int)((int)(x) > 0) - (int)((int)(x) < 0)));
}

#define vsf_abs(__num)                      (((__num) < 0) ? (-(__num)) : (__num))

#define VSF_BIT(__N)                        (1UL << (__N))
#define VSF_BITMASK(__N)                    (VSF_BIT(__N) - 1)
#define VSF_UNUSED_PARAM(__VAL)             (__VAL) = (__VAL)

#define VSF_STATIC_ASSERT(__COND)                                               \
            typedef int VSF_MACRO_SAFE_NAME(STATIC_ASSERT)[(__COND) * 2 - 1]

// will be removed later
#ifdef __VSF_COMPAT__
#   define min                              vsf_min
#   define max                              vsf_max
#   define __STR                            __VSF_STR
#   define STR                              VSF_STR
#   define sign                             vsf_sign
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if __IS_COMPILER_IAR__
//! start of typedef name has already been declared
#   pragma diag_suppress=pe301
#endif


#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 201112L && !defined(__cplusplus)
/*! \note if your compiler raises warning about "redefine" type,
 *!       please ignore and suppress the warning
 *!
 *! \note char32_t and char16_t should only be used when C11 is enabled.
 */
typedef uint_least32_t char32_t;
typedef uint_least16_t char16_t;
#endif


#if __IS_COMPILER_IAR__
//! end of typedef name has already been declared
//#   pragma diag_suppress=pe301
#endif


#ifndef __REG_TYPE__
#define __REG_TYPE__

typedef volatile uint8_t            reg8_t;
typedef volatile uint16_t           reg16_t;
typedef volatile uint32_t           reg32_t;

#define __REG_CONNECT(__A, __B)     __A##__B
#define __REG_RSVD_NAME(__NAME)     __REG_CONNECT(__unused_, __NAME)

#define ____REG_RSVD(__NAME, __BIT)                                             \
        reg##__BIT##_t              __NAME : __BIT;
#define ____REG_RSVD_N(__NAME, __BIT, __N)                                      \
        reg##__BIT##_t              __NAME[__N];
#define __REG_RSVD(__BIT)           ____REG_RSVD(REG_RSVD_NAME, __BIT)
#define __REG_RSVD_N(__BIT, __N)    ____REG_RSVD_N(REG_RSVD_NAME, __BIT, (__N))

#define REG_RSVD_NAME               __REG_RSVD_NAME(__LINE__)
#define REG_RSVD(__BIT)             __REG_RSVD(__BIT)
#define REG_RSVD_N(__BIT, __N)      __REG_RSVD_N(__BIT, (__N))

#define REG_RSVD_U8                 REG_RSVD(8)
#define REG_RSVD_U16                REG_RSVD(16)
#define REG_RSVD_U32                REG_RSVD(32)

#define REG_RSVD_U8N(__N)           REG_RSVD_N(8, (__N))
#define REG_RSVD_U16N(__N)          REG_RSVD_N(16, (__N))
#define REG_RSVD_U32N(__N)          REG_RSVD_N(32, (__N))

#define REG8_RSVD_N(__N)            REG_RSVD_U8N(__N)
#define REG8_RSVD_B(__BYTE_CNT)     REG8_RSVD_N(__BYTE_CNT)
#define REG8_RSVD_8B                REG8_RSVD_B(8)
#define REG8_RSVD_16B               REG8_RSVD_B(16)
#define REG8_RSVD_32B               REG8_RSVD_B(32)
#define REG8_RSVD_64B               REG8_RSVD_B(64)
#define REG8_RSVD_128B              REG8_RSVD_B(128)
#define REG8_RSVD_256B              REG8_RSVD_B(256)
#define REG8_RSVD_512B              REG8_RSVD_B(512)
#define REG8_RSVD_1K                REG8_RSVD_B(1024)
#define REG8_RSVD_2K                REG8_RSVD_B(2048)
#define REG8_RSVD_4K                REG8_RSVD_B(4096)
#define REG8_RSVD_8K                REG8_RSVD_B(8192)
#define REG8_RSVD_16K               REG8_RSVD_B(16 * 1024)
#define REG8_RSVD_32K               REG8_RSVD_B(32 * 1024)
#define REG8_RSVD_64K               REG8_RSVD_B(64 * 1024)
#define REG8_RSVD_128K              REG8_RSVD_B(128 * 1024)
#define REG8_RSVD_256K              REG8_RSVD_B(256 * 1024)
#define REG8_RSVD_512K              REG8_RSVD_B(512 * 1024)
#define REG8_RSVD_1M                REG8_RSVD_B(1024 * 1024)

#define REG16_RSVD_N(__N)           REG_RSVD_U16N(__N)
// __BYTE_CNT MUST be mutiple of 2
#define REG16_RSVD_B(__BYTE_CNT)    REG16_RSVD_N(__BYTE_CNT >> 1)
#define REG16_RSVD_8B               REG16_RSVD_B(8)
#define REG16_RSVD_16B              REG16_RSVD_B(16)
#define REG16_RSVD_32B              REG16_RSVD_B(32)
#define REG16_RSVD_64B              REG16_RSVD_B(64)
#define REG16_RSVD_128B             REG16_RSVD_B(128)
#define REG16_RSVD_256B             REG16_RSVD_B(256)
#define REG16_RSVD_512B             REG16_RSVD_B(512)
#define REG16_RSVD_1K               REG16_RSVD_B(1024)
#define REG16_RSVD_2K               REG16_RSVD_B(2048)
#define REG16_RSVD_4K               REG16_RSVD_B(4096)
#define REG16_RSVD_8K               REG16_RSVD_B(8192)
#define REG16_RSVD_16K              REG16_RSVD_B(16 * 1024)
#define REG16_RSVD_32K              REG16_RSVD_B(32 * 1024)
#define REG16_RSVD_64K              REG16_RSVD_B(64 * 1024)
#define REG16_RSVD_128K             REG16_RSVD_B(128 * 1024)
#define REG16_RSVD_256K             REG16_RSVD_B(256 * 1024)
#define REG16_RSVD_512K             REG16_RSVD_B(512 * 1024)
#define REG16_RSVD_1M               REG16_RSVD_B(1024 * 1024)

#define REG32_RSVD_N(__N)           REG_RSVD_U32N(__N)
// __BYTE_CNT MUST be mutiple of 4
#define REG32_RSVD_B(__BYTE_CNT)    REG_RSVD_U32N(__BYTE_CNT >> 2)
#define REG32_RSVD_8B               REG32_RSVD_B(8)
#define REG32_RSVD_16B              REG32_RSVD_B(16)
#define REG32_RSVD_32B              REG32_RSVD_B(32)
#define REG32_RSVD_64B              REG32_RSVD_B(64)
#define REG32_RSVD_128B             REG32_RSVD_B(128)
#define REG32_RSVD_256B             REG32_RSVD_B(256)
#define REG32_RSVD_512B             REG32_RSVD_B(512)
#define REG32_RSVD_1K               REG32_RSVD_B(1024)
#define REG32_RSVD_2K               REG32_RSVD_B(2048)
#define REG32_RSVD_4K               REG32_RSVD_B(4096)
#define REG32_RSVD_8K               REG32_RSVD_B(8192)
#define REG32_RSVD_16K              REG32_RSVD_B(16 * 1024)
#define REG32_RSVD_32K              REG32_RSVD_B(32 * 1024)
#define REG32_RSVD_64K              REG32_RSVD_B(64 * 1024)
#define REG32_RSVD_128K             REG32_RSVD_B(128 * 1024)
#define REG32_RSVD_256K             REG32_RSVD_B(256 * 1024)
#define REG32_RSVD_512K             REG32_RSVD_B(512 * 1024)
#define REG32_RSVD_1M               REG32_RSVD_B(1024 * 1024)

#endif      // __REG_TYPE__




/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif


/*============================ Multiple-Entry ================================*/

/*! \note it is forseeable that in certain platform or compiler, people might
 *!       have their own version of those system header files listed below
 */

#if !defined(__USE_LOCAL_STDIO__)
//#warning including stdio.h                                                    //! uncomment this for debugging purpose only
#   include <stdio.h>
#else
//#   warning user ignored standard stdio.h                                     //! uncomment this for debugging purpose only
#   undef __USE_LOCAL_STDIO__
#endif

#if !defined(__USE_LOCAL_SETJMP__)
//#warning including setjmp.h                                                    //! uncomment this for debugging purpose only
#   include <setjmp.h>
#else
//#   warning user ignored standard setjmp.h                                     //! uncomment this for debugging purpose only
#   undef __USE_LOCAL_SETJMP__
#endif

#if !defined(__USE_LOCAL_STDARG__)
//#warning including stdarg.h                                                    //! uncomment this for debugging purpose only
#   include <stdarg.h>
#else
//#   warning user ignored standard stdarg.h                                     //! uncomment this for debugging purpose only
#   undef __USE_LOCAL_STDARG__
#endif

#endif // __COMMON_TYPE_H__
