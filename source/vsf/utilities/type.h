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

#ifndef __APP_TYPE_H_INCLUDED__
#define __APP_TYPE_H_INCLUDED__

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
/*============================ MACROS ========================================*/
/*
//! \name boolean value
//! @{
#ifndef false
    # define false          0x00
#endif

#ifdef true
    #undef true
#endif
#define true                (!false)
//! @}  
     
#ifndef NULL
#   define NULL             ((void*)0)
#endif
*/

//! \brief system macros
#ifndef ASSERT
#   define ASSERT(...)      assert(__VA_ARGS__)
#endif

#define __STR(__N)          #__N  
#define STR(__N)            __STR(__N)

#define max(__A, __B)       (((__A) > (__B)) ? (__A) : (__B))
#define min(__A, __B)       (((__A) < (__B)) ? (__A) : (__B))

#ifndef dimof
#   define dimof(arr)       (sizeof(arr) / sizeof((arr)[0]))
#endif
#ifndef UBOUND
#   define UBOUND(__ARR)    dimof(__ARR)
#endif

#ifndef offset_of
#   define offset_of(s, m)  (uint32_t)(&(((s *)0)->m))
#endif
#ifndef container_of
#   define container_of(ptr, type, member)      \
        ((type *)((char *)(ptr) - offset_of(type, member)))
#endif
#ifndef safe_container_of
#   define safe_container_of(ptr, type, member) \
        (ptr ? container_of(ptr, type, member) : NULL)
#endif


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
    VSF_ERR_NOT_READY                   =1,     //!< service not ready yet
    VSF_ERR_NONE                        =0,     //!< none error
    VSF_ERR_UNKNOWN                     =-1,    //!< unknown error
    VSF_ERR_NOT_SUPPORT                 =-2,    //!< function not supported
    VSF_ERR_NOT_AVAILABLE               =-4,    //!< service not available
    VSF_ERR_NOT_ACCESSABLE              =-5,    //!< target not acceesable
    VSF_ERR_NOT_ENOUGH_RESOURCES        =-6,    //!< no enough resources
    VSF_ERR_FAIL                        =-7,    //!< failed
    VSF_ERR_INVALID_PARAMETER           =-8,    //!< invalid parameter
    VSF_ERR_INVALID_RANGE               =-9,    //!< invalid range
    VSF_ERR_INVALID_PTR                 =-10,   //!< invalid pointer
    VSF_ERR_INVALID_KEY                 =-11,   //!< invalid key
    VSF_ERR_IO                          =-12,   //!< IO error
    VSF_ERR_REQ_ALREADY_REGISTERED      =-13,   //!< request all ready exist
    VSF_ERR_BUG                         =-14,   //!< bug
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
} fsm_rt_t;
//! @}

#endif


#define __REG_TYPE__

typedef volatile unsigned char     reg8_t;
typedef volatile unsigned short    reg16_t;
typedef volatile unsigned int      reg32_t;


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif // __APP_TYPE_H_INCLUDED__
