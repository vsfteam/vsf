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

#ifndef __COMMON_TYPE_H__
#define __COMMON_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/

/* do not modify this */
#include "vsf_usr_cfg.h"

/*============================ MACROS ========================================*/
#ifndef ENABLED
#   define ENABLED                              1
#endif

#ifndef DISABLED
#   define DISABLED                             0
#endif


//! \brief system macros
#ifndef ASSERT
#   define ASSERT           assert
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

#ifndef __cplusplus
//  conflict with std::max, std::min
#   define max(__A, __B)    (((__A) > (__B)) ? (__A) : (__B))
#   define min(__A, __B)    (((__A) < (__B)) ? (__A) : (__B))
#endif

#ifndef dimof
#   define dimof(arr)       (sizeof(arr) / sizeof((arr)[0]))
#endif
#ifndef UBOUND
#   define UBOUND(__ARR)    dimof(__ARR)
#endif

#ifndef offset_of
#   define offset_of(s, m)  (uintptr_t)(&(((s *)0)->m))
#endif
#ifndef container_of
#   define container_of(ptr, type, member)      \
        ((type *)((uintptr_t)(ptr) - offset_of(type, member)))
#endif
#ifndef safe_container_of
#   define safe_container_of(ptr, type, member) \
        (ptr ? container_of(ptr, type, member) : NULL)
#endif

#ifndef SIGN
#   define SIGN(__N)        ((int)((int)((int)(__N) > 0) - (int)((int)(__N) < 0)))
#endif
#undef sign
#define sign(__n)           SIGN(__n)

#define ABS(__NUM)          (((__NUM) < 0) ? (-(__NUM)) : (__NUM))

#ifndef BIT
#define BIT(__N)            ((uint32_t)1 << (__N))
#endif
#ifndef _BV
#define _BV(__N)            ((uint32_t)1 << (__N))
#endif
#ifndef _BM
#define __MASK(__N)         (_BV(__N) - 1)
#define _BM(__FROM,__TO)    (__MASK((__TO)+1)-__MASK(__FROM))
#endif

#ifndef UNUSED_PARAM
# define UNUSED_PARAM(__VAL)    (__VAL) = (__VAL)
#endif

//! \brief This macro convert variable types between different datatypes.
#define __TYPE_CONVERT(__ADDR,__TYPE)       (*((__TYPE *)(__ADDR)))
#define TYPE_CONVERT(__ADDR, __TYPE)        __TYPE_CONVERT((__ADDR), __TYPE)
#define type_convert(__ADDR, __TYPE)        TYPE_CONVERT(__ADDR, __TYPE)

/*============================ MACROFIED FUNCTIONS ===========================*/
#define IS_FSM_ERR(__FSM_RT)        ((__FSM_RT) < fsm_rt_cpl)
#define is_fsm_err(__FSM_RT)        IS_FSM_ERR(__FSM_RT)
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
                                            =-18    //!< the provided resource is not aligned to certain size (2^N)
} vsf_err_t;
//! @}

#ifndef __FSM_RT_TYPE__
#define __FSM_RT_TYPE__
//! \name finit state machine state
//! @{
typedef enum {
    fsm_rt_err          = -1,    //!< fsm error, error code can be get from other interface
    fsm_rt_cpl          = 0,     //!< fsm complete
    fsm_rt_on_going     = 1,     //!< fsm on-going
    fsm_rt_yield        = 1,
    fsm_rt_wait_for_obj = 2,     //!< fsm wait for object
    fsm_rt_wait_for_evt = 2,    
    fsm_rt_wfe          = 2,
    fsm_rt_asyn         = 3,     //!< fsm asynchronose complete, you can check it later.
    fsm_rt_user         = 4
} fsm_rt_t;
//! @}

#endif


#define __REG_TYPE__

typedef volatile uint8_t     reg8_t;
typedef volatile uint16_t    reg16_t;
typedef volatile uint32_t    reg32_t;

#if (defined(__IS_COMPILER_IAR__) && __IS_COMPILER_IAR__)                       \
    ||  (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L)

#undef ____RESERVED
#undef __RESERVED
#define ____RESERVED(__BIT, __NAME)                                             \
        uint##__BIT##_t __unused_##__NAME : __BIT;
#define __RESERVED(__BIT, __NAME)                                               \
            ____RESERVED(__BIT, __NAME)

#   ifndef RESERVED_U8           
#       define RESERVED_U8          __RESERVED( 8, __LINE__ )
#   endif

#   ifndef RESERVED_U16            
#       define RESERVED_U16         __RESERVED( 16, __LINE__ )
#   endif

#   ifndef RESERVED_U32             
#       define RESERVED_U32         __RESERVED( 32, __LINE__ )
#   endif

#   ifndef RESERVED_16B             
#       define RESERVED_16B         uint32_t __unused_##__LINE__[4];
#   endif

#   ifndef RESERVED_64B             
#       define RESERVED_64B         uint32_t __unused_##__LINE__[16];
#   endif

#   ifndef RESERVED_256B             
#       define RESERVED_256B        uint32_t __unused_##__LINE__[64];
#   endif

#   ifndef RESERVED_1K             
#       define RESERVED_1K          uint32_t __unused_##__LINE__[256];
#   endif

#   ifndef RESERVED_4K             
#       define RESERVED_4K          uint32_t __unused_##__LINE__[1024];
#   endif

#   ifndef RESERVED_16K             
#       define RESERVED_16K         uint32_t __unused_##__LINE__[4 * 1024];
#   endif

#   ifndef RESERVED_64K             
#       define RESERVED_64K         uint32_t __unused_##__LINE__[16 * 1024];
#   endif

#   ifndef RESERVED_256K             
#       define RESERVED_256K        uint32_t __unused_##__LINE__[64 * 1024];
#   endif


#   ifndef RESERVED_1M             
#       define RESERVED_1M          uint32_t __unused_##__LINE__[256 * 1024];
#   endif

#else
#   ifndef RESERVED_U8           
#       define RESERVED_U8          uint8_t  : 8;
#   endif

#   ifndef RESERVED_U16            
#       define RESERVED_U16         uint16_t : 16;
#   endif

#   ifndef RESERVED_U32             
#       define RESERVED_U32         uint32_t : 32;
#   endif

#   ifndef RESERVED_16B             
#       define RESERVED_16B         RESERVED_U32                                \
                                RESERVED_U32                                    \
                                RESERVED_U32                                    \
                                RESERVED_U32
#   endif

#   ifndef RESERVED_64B             
#       define RESERVED_64B         RESERVED_16B                                \
                                RESERVED_16B                                    \
                                RESERVED_16B                                    \
                                RESERVED_16B
#   endif

#   ifndef RESERVED_256B             
#       define RESERVED_256B        RESERVED_64B                                \
                                RESERVED_64B                                    \
                                RESERVED_64B                                    \
                                RESERVED_64B
#   endif

#   ifndef RESERVED_1K             
#       define RESERVED_1K          RESERVED_256B                               \
                                RESERVED_256B                                   \
                                RESERVED_256B                                   \
                                RESERVED_256B
#   endif

#   ifndef RESERVED_4K             
#       define RESERVED_4K          RESERVED_1K                                 \
                                RESERVED_1K                                     \
                                RESERVED_1K                                     \
                                RESERVED_1K
#   endif

#   ifndef RESERVED_16K             
#       define RESERVED_16K         RESERVED_4K                                 \
                                RESERVED_4K                                     \
                                RESERVED_4K                                     \
                                RESERVED_4K
#   endif

#   ifndef RESERVED_64K             
#       define RESERVED_64K         RESERVED_16K                                \
                                RESERVED_16K                                    \
                                RESERVED_16K                                    \
                                RESERVED_16K
#   endif

#   ifndef RESERVED_256K             
#       define RESERVED_256K        RESERVED_64K                                \
                                RESERVED_64K                                    \
                                RESERVED_64K                                    \
                                RESERVED_64K
#   endif


#   ifndef RESERVED_1M             
#       define RESERVED_1M          RESERVED_256K                               \
                                RESERVED_256K                                   \
                                RESERVED_256K                                   \
                                RESERVED_256K
#   endif

#endif




/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif


#endif // __APP_TYPE_H_INCLUDED__
