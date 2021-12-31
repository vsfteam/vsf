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

#ifndef __I_REG_GPIO_H__
#define __I_REG_GPIO_H__

/*============================ INCLUDES ======================================*/

/*! \note i_reg_xxxx.h header files must be standalong and assume following
 *!       conditions:
 *!       a. stdint.h exists
 *!       b. anonymouse structures and unions are supported
 */
#include "./utilities/compiler/__common/__type.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef __AIC8800_GPIO_USE_BIT_FIELD
#   define __AIC8800_GPIO_USE_BIT_FIELD             DISABLED
#endif

#define GPIO0_BASE_ADDRESS                          (0X40504000UL)
#define REG_GPIO0                                   ((GPIO_REG_T *)GPIO0_BASE_ADDRESS)

#define GPIO1_BASE_ADDRESS                          (0x50011000UL)
#define REG_GPIO1                                   ((GPIO_REG_T *)GPIO1_BASE_ADDRESS)

/* Define structure member permissions : 'read only' */
#ifndef __IM
#   define __IM                                     const
#endif

/* Define structure member permissions : 'write only' */
#ifndef __OM
#   define __OM
#endif

/* Define structure member permissions : 'read or write' */
#ifndef __IOM
#   define __IOM
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#if __AIC8800_GPIO_USE_BIT_FIELD == ENABLED
#   define DEF_GPIO_REG(__NAME, __TOTAL_SIZE, ...)                               \
    union {                                                                     \
        reg##__TOTAL_SIZE##_t VALUE;                                            \
        struct {                                                                \
            __VA_ARGS__                                                         \
        };                                                                      \
    } __NAME
#else
#   define DEF_GPIO_REG(__NAME, __TOTAL_SIZE, ...)                               \
        __VA_ARGS__ reg##__TOTAL_SIZE##_t __NAME
#endif

/*============================ TYPES =========================================*/

typedef struct GPIO_REG_T {
    DEF_GPIO_REG(VR     , 32, __IOM);
    DEF_GPIO_REG(MR     , 32, __IOM);
    DEF_GPIO_REG(DR     , 32, __IOM);
    DEF_GPIO_REG(TELR   , 32, __IOM);
    DEF_GPIO_REG(TER    , 32, __IOM);
    DEF_GPIO_REG(TLR    , 32, __IOM);
    DEF_GPIO_REG(ICR    , 32, __IOM);
    REG_RSVD_U32
    DEF_GPIO_REG(ISR    , 32, __OM);
    DEF_GPIO_REG(IRR    , 32, __IOM);
    DEF_GPIO_REG(TIR    , 32, __IOM);
    DEF_GPIO_REG(FR     , 32, __IOM);
} GPIO_REG_T;

#ifdef __cplusplus
}
#endif
#endif
/* EOF */
