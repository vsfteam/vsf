/*****************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
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

#ifndef __ST_DEVICES_H__
#define __ST_DEVICES_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/

#include "sthal_system.h"

/*============================ MACROS ========================================*/

#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))
#define SET_BIT(REG, BIT)          ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)        ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)         ((REG) & (BIT))
#define CLEAR_REG(REG)             ((REG) = (0x0))
#define WRITE_REG(REG, VAL)        ((REG) = (VAL))
#define READ_REG(REG)              ((REG))
#define MODIFY_REG(REG, CLEARMASK, SETMASK)                                    \
    WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))
#define POSITION_VAL(VAL) (__CLZ(__RBIT(VAL)))

#define ATOMIC_SET_BIT(REG, BIT)                                               \
    do {                                                                       \
        uint32_t val;                                                          \
        do {                                                                   \
            val = __LDREXW((__IO uint32_t *)&(REG)) | (BIT);                   \
        } while ((__STREXW(val, (__IO uint32_t *)&(REG))) != 0U);              \
    } while (0)

#define ATOMIC_CLEAR_BIT(REG, BIT)                                             \
    do {                                                                       \
        uint32_t val;                                                          \
        do {                                                                   \
            val = __LDREXW((__IO uint32_t *)&(REG)) & ~(BIT);                  \
        } while ((__STREXW(val, (__IO uint32_t *)&(REG))) != 0U);              \
    } while (0)

#define ATOMIC_MODIFY_REG(REG, CLEARMSK, SETMASK)                              \
    do {                                                                       \
        uint32_t val;                                                          \
        do {                                                                   \
            val =                                                              \
                (__LDREXW((__IO uint32_t *)&(REG)) & ~(CLEARMSK)) | (SETMASK); \
        } while ((__STREXW(val, (__IO uint32_t *)&(REG))) != 0U);              \
    } while (0)

#define ATOMIC_SETH_BIT(REG, BIT)                                              \
    do {                                                                       \
        uint16_t val;                                                          \
        do {                                                                   \
            val = __LDREXH((__IO uint16_t *)&(REG)) | (BIT);                   \
        } while ((__STREXH(val, (__IO uint16_t *)&(REG))) != 0U);              \
    } while (0)

#define ATOMIC_CLEARH_BIT(REG, BIT)                                            \
    do {                                                                       \
        uint16_t val;                                                          \
        do {                                                                   \
            val = __LDREXH((__IO uint16_t *)&(REG)) & ~(BIT);                  \
        } while ((__STREXH(val, (__IO uint16_t *)&(REG))) != 0U);              \
    } while (0)

#define ATOMIC_MODIFYH_REG(REG, CLEARMSK, SETMASK)                             \
    do {                                                                       \
        uint16_t val;                                                          \
        do {                                                                   \
            val =                                                              \
                (__LDREXH((__IO uint16_t *)&(REG)) & ~(CLEARMSK)) | (SETMASK); \
        } while ((__STREXH(val, (__IO uint16_t *)&(REG))) != 0U);              \
    } while (0)

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum {
    RESET = 0U,
    SET   = !RESET
} FlagStatus, ITStatus;

typedef enum {
    DISABLE = 0U,
    ENABLE  = !DISABLE
} FunctionalState;

typedef enum {
    SUCCESS = 0U,
    ERROR   = !SUCCESS
} ErrorStatus;

typedef struct {
    uint32_t todo;
} ADC_TypeDef;

typedef struct {
    uint32_t todo;
} DMA_TypeDef;

typedef struct {
    uint32_t todo;
} GPIO_TypeDef;

typedef struct {
    uint32_t todo;
} I2C_TypeDef;

typedef struct {
    uint32_t todo;
} RTC_TypeDef;

typedef struct {
    uint32_t todo;
} SPI_TypeDef;

typedef struct {
    uint32_t todo;
} TIM_TypeDef;

typedef struct {
    uint32_t todo;
} USART_TypeDef;

typedef struct {
    uint32_t todo;
} WWDG_TypeDef;

typedef struct {
    uint32_t todo;
} DMA_Stream_TypeDef;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
