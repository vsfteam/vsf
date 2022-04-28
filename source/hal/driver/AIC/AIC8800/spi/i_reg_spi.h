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

#ifndef __I_REG_SPI_H__
#define __I_REG_SPI_H__

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

#ifndef __AIC8800_SPI_USE_BIT_FIELD
#   define __AIC8800_SPI_USE_BIT_FIELD              DISABLED
#endif

#define SPI0_BASE_ADDRESS                           (0X40105000ul)

#define REG_SPI0                                    ((REG_SPI_T *)SPI0_BASE_ADDRESS)

/* Define structure member permissions : read only */
#ifndef __IM
#   define __IM                                     const
#endif

/* Define structure member permissions : write only */
#ifndef __OM
#   define __OM
#endif

/* Define structure member permissions : read or write */
#ifndef __IOM
#   define __IOM
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#if __AIC8800_SPI_USE_BIT_FIELD == ENABLED
#   define DEF_SPI_REG(__NAME, __TOTAL_SIZE, ...)                               \
    union {                                                                     \
        reg##__TOTAL_SIZE##_t VALUE;                                            \
        struct {                                                                \
            __VA_ARGS__                                                         \
        };                                                                      \
    } __NAME
#else
#   define DEF_SPI_REG(__NAME, __TOTAL_SIZE, ...)                               \
        __VA_ARGS__ reg##__TOTAL_SIZE##_t __NAME
#endif

#define SPI_DR_DIV              0
#define SPI_DR_DIV_MASK         ((reg32_t)(0xFFFF << SPI_DR_DIV))

/*============================ TYPES =========================================*/

typedef struct REG_SPI_T {
    union {
        __IM            reg32_t         BASE_ADDR;
        DEF_SPI_REG(IOR,    32, __IOM);
    };
    DEF_SPI_REG(DR,         32, __IOM);
    reg32_t CR[6];
    DEF_SPI_REG(IER,        32, __IOM);
    DEF_SPI_REG(ICLR,       32, __IOM);
    DEF_SPI_REG(IRSR,       32, __IM);
    DEF_SPI_REG(IMSR,       32, __IM);
    REG_RSVD_U32
    DEF_SPI_REG(SR,         32, __IOM);
    REG_RSVD_U32N(5)
    DEF_SPI_REG(MR0,        32, __IOM);
    REG_RSVD_U32
    DEF_SPI_REG(MR1,        32, __IOM);
    DEF_SPI_REG(OCR,        32, __IOM);
    REG_RSVD_U32
    DEF_SPI_REG(ICR,        32, __IOM);
    DEF_SPI_REG(TCR,        32, __IOM);
} REG_SPI_T;

#ifdef __cplusplus
}
#endif
#endif
/* EOF */
