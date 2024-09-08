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

#ifndef __ST_HAL_EXTI_H__
#define __ST_HAL_EXTI_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/

#include "sthal_def.h"

/*============================ MACROS ========================================*/

#define EXTI_LINE_0  (EXTI_GPIO | 0x00u)
#define EXTI_LINE_1  (EXTI_GPIO | 0x01u)
#define EXTI_LINE_2  (EXTI_GPIO | 0x02u)
#define EXTI_LINE_3  (EXTI_GPIO | 0x03u)
#define EXTI_LINE_4  (EXTI_GPIO | 0x04u)
#define EXTI_LINE_5  (EXTI_GPIO | 0x05u)
#define EXTI_LINE_6  (EXTI_GPIO | 0x06u)
#define EXTI_LINE_7  (EXTI_GPIO | 0x07u)
#define EXTI_LINE_8  (EXTI_GPIO | 0x08u)
#define EXTI_LINE_9  (EXTI_GPIO | 0x09u)
#define EXTI_LINE_10 (EXTI_GPIO | 0x0Au)
#define EXTI_LINE_11 (EXTI_GPIO | 0x0Bu)
#define EXTI_LINE_12 (EXTI_GPIO | 0x0Cu)
#define EXTI_LINE_13 (EXTI_GPIO | 0x0Du)
#define EXTI_LINE_14 (EXTI_GPIO | 0x0Eu)
#define EXTI_LINE_15 (EXTI_GPIO | 0x0Fu)
#define EXTI_LINE_16 (EXTI_CONFIG | 0x10u)
#define EXTI_LINE_17 (EXTI_CONFIG | 0x11u)
#if defined(EXTI_IMR_IM18)
#    define EXTI_LINE_18 (EXTI_CONFIG | 0x12u)
#else
#    define EXTI_LINE_18 (EXTI_RESERVED | 0x12u)
#endif
#if defined(EXTI_IMR_IM19)
#    define EXTI_LINE_19 (EXTI_CONFIG | 0x13u)
#else
#    define EXTI_LINE_19 (EXTI_RESERVED | 0x13u)
#endif
#if defined(EXTI_IMR_IM20)
#    define EXTI_LINE_20 (EXTI_CONFIG | 0x14u)
#else
#    define EXTI_LINE_20 (EXTI_RESERVED | 0x14u)
#endif
#define EXTI_LINE_21 (EXTI_CONFIG | 0x15u)
#define EXTI_LINE_22 (EXTI_CONFIG | 0x16u)
#if defined(EXTI_IMR_IM23)
#    define EXTI_LINE_23 (EXTI_CONFIG | 0x17u)
#endif

#define EXTI_MODE_NONE      0x00000000u
#define EXTI_MODE_INTERRUPT 0x00000001u
#define EXTI_MODE_EVENT     0x00000002u

#define EXTI_TRIGGER_NONE           0x00000000u
#define EXTI_TRIGGER_RISING         0x00000001u
#define EXTI_TRIGGER_FALLING        0x00000002u
#define EXTI_TRIGGER_RISING_FALLING (EXTI_TRIGGER_RISING | EXTI_TRIGGER_FALLING)

#define EXTI_GPIOA 0x00000000u
#define EXTI_GPIOB 0x00000001u
#define EXTI_GPIOC 0x00000002u
#if defined(GPIOD)
#    define EXTI_GPIOD 0x00000003u
#endif
#if defined(GPIOE)
#    define EXTI_GPIOE 0x00000004u
#endif
#if defined(GPIOF)
#    define EXTI_GPIOF 0x00000005u
#endif
#if defined(GPIOG)
#    define EXTI_GPIOG 0x00000006u
#endif
#if defined(GPIOH)
#    define EXTI_GPIOH 0x00000007u
#endif
#if defined(GPIOI)
#    define EXTI_GPIOI 0x00000008u
#endif
#if defined(GPIOJ)
#    define EXTI_GPIOJ 0x00000009u
#endif
#if defined(GPIOK)
#    define EXTI_GPIOK 0x0000000Au
#endif

#define EXTI_PROPERTY_SHIFT 24u
#define EXTI_CONFIG         (0x02uL << EXTI_PROPERTY_SHIFT)
#define EXTI_GPIO           ((0x04uL << EXTI_PROPERTY_SHIFT) | EXTI_CONFIG)
#define EXTI_RESERVED       (0x08uL << EXTI_PROPERTY_SHIFT)
#define EXTI_PROPERTY_MASK  (EXTI_CONFIG | EXTI_GPIO)

#define EXTI_PIN_MASK 0x0000001Fu

#define EXTI_MODE_MASK (EXTI_MODE_EVENT | EXTI_MODE_INTERRUPT)

#define EXTI_TRIGGER_MASK (EXTI_TRIGGER_RISING | EXTI_TRIGGER_FALLING)

#if defined(EXTI_IMR_IM23)
#    define EXTI_LINE_NB 24UL
#else
#    define EXTI_LINE_NB 23UL
#endif

#define IS_EXTI_LINE(__EXTI_LINE__)                                            \
    ((((__EXTI_LINE__) & ~(EXTI_PROPERTY_MASK | EXTI_PIN_MASK)) == 0x00u) &&   \
     ((((__EXTI_LINE__) & EXTI_PROPERTY_MASK) == EXTI_CONFIG) ||               \
      (((__EXTI_LINE__) & EXTI_PROPERTY_MASK) == EXTI_GPIO)) &&                \
     (((__EXTI_LINE__) & EXTI_PIN_MASK) < EXTI_LINE_NB))

#define IS_EXTI_MODE(__EXTI_LINE__)                                            \
    ((((__EXTI_LINE__) & EXTI_MODE_MASK) != 0x00u) &&                          \
     (((__EXTI_LINE__) & ~EXTI_MODE_MASK) == 0x00u))

#define IS_EXTI_TRIGGER(__EXTI_LINE__)                                         \
    (((__EXTI_LINE__) & ~EXTI_TRIGGER_MASK) == 0x00u)

#define IS_EXTI_PENDING_EDGE(__EXTI_LINE__)                                    \
    ((__EXTI_LINE__) == EXTI_TRIGGER_RISING_FALLING)

#define IS_EXTI_CONFIG_LINE(__EXTI_LINE__)                                     \
    (((__EXTI_LINE__) & EXTI_CONFIG) != 0x00u)

#if !defined(GPIOD)
#    define IS_EXTI_GPIO_PORT(__PORT__)                                        \
        (((__PORT__) == EXTI_GPIOA) || ((__PORT__) == EXTI_GPIOB) ||           \
         ((__PORT__) == EXTI_GPIOC) || ((__PORT__) == EXTI_GPIOH))
#elif !defined(GPIOE)
#    define IS_EXTI_GPIO_PORT(__PORT__)                                        \
        (((__PORT__) == EXTI_GPIOA) || ((__PORT__) == EXTI_GPIOB) ||           \
         ((__PORT__) == EXTI_GPIOC) || ((__PORT__) == EXTI_GPIOD) ||           \
         ((__PORT__) == EXTI_GPIOH))
#elif !defined(GPIOF)
#    define IS_EXTI_GPIO_PORT(__PORT__)                                        \
        (((__PORT__) == EXTI_GPIOA) || ((__PORT__) == EXTI_GPIOB) ||           \
         ((__PORT__) == EXTI_GPIOC) || ((__PORT__) == EXTI_GPIOD) ||           \
         ((__PORT__) == EXTI_GPIOE) || ((__PORT__) == EXTI_GPIOH))
#elif !defined(GPIOI)
#    define IS_EXTI_GPIO_PORT(__PORT__)                                        \
        (((__PORT__) == EXTI_GPIOA) || ((__PORT__) == EXTI_GPIOB) ||           \
         ((__PORT__) == EXTI_GPIOC) || ((__PORT__) == EXTI_GPIOD) ||           \
         ((__PORT__) == EXTI_GPIOE) || ((__PORT__) == EXTI_GPIOF) ||           \
         ((__PORT__) == EXTI_GPIOG) || ((__PORT__) == EXTI_GPIOH))
#elif !defined(GPIOJ)
#    define IS_EXTI_GPIO_PORT(__PORT__)                                        \
        (((__PORT__) == EXTI_GPIOA) || ((__PORT__) == EXTI_GPIOB) ||           \
         ((__PORT__) == EXTI_GPIOC) || ((__PORT__) == EXTI_GPIOD) ||           \
         ((__PORT__) == EXTI_GPIOE) || ((__PORT__) == EXTI_GPIOF) ||           \
         ((__PORT__) == EXTI_GPIOG) || ((__PORT__) == EXTI_GPIOH) ||           \
         ((__PORT__) == EXTI_GPIOI))
#else
#    define IS_EXTI_GPIO_PORT(__PORT__)                                        \
        (((__PORT__) == EXTI_GPIOA) || ((__PORT__) == EXTI_GPIOB) ||           \
         ((__PORT__) == EXTI_GPIOC) || ((__PORT__) == EXTI_GPIOD) ||           \
         ((__PORT__) == EXTI_GPIOE) || ((__PORT__) == EXTI_GPIOF) ||           \
         ((__PORT__) == EXTI_GPIOG) || ((__PORT__) == EXTI_GPIOH) ||           \
         ((__PORT__) == EXTI_GPIOI) || ((__PORT__) == EXTI_GPIOJ) ||           \
         ((__PORT__) == EXTI_GPIOK))
#endif

#define IS_EXTI_GPIO_PIN(__PIN__) ((__PIN__) < 16U)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum {
    HAL_EXTI_COMMON_CB_ID = 0x00U
} EXTI_CallbackIDTypeDef;

typedef struct {
    uint32_t Line;
    void     (*PendingCallback)(void);
} EXTI_HandleTypeDef;

typedef struct {
    uint32_t Line;
    uint32_t Mode;
    uint32_t Trigger;
    uint32_t GPIOSel;
} EXTI_ConfigTypeDef;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

HAL_StatusTypeDef HAL_EXTI_SetConfigLine(EXTI_HandleTypeDef *hexti,
                                         EXTI_ConfigTypeDef *pExtiConfig);
HAL_StatusTypeDef HAL_EXTI_GetConfigLine(EXTI_HandleTypeDef *hexti,
                                         EXTI_ConfigTypeDef *pExtiConfig);
HAL_StatusTypeDef HAL_EXTI_ClearConfigLine(EXTI_HandleTypeDef *hexti);
HAL_StatusTypeDef HAL_EXTI_RegisterCallback(EXTI_HandleTypeDef    *hexti,
                                            EXTI_CallbackIDTypeDef CallbackID,
                                            void (*pPendingCbfn)(void));
HAL_StatusTypeDef HAL_EXTI_GetHandle(EXTI_HandleTypeDef *hexti,
                                     uint32_t            ExtiLine);

void     HAL_EXTI_IRQHandler(EXTI_HandleTypeDef *hexti);
uint32_t HAL_EXTI_GetPending(EXTI_HandleTypeDef *hexti, uint32_t Edge);
void     HAL_EXTI_ClearPending(EXTI_HandleTypeDef *hexti, uint32_t Edge);
void     HAL_EXTI_GenerateSWI(EXTI_HandleTypeDef *hexti);

#ifdef __cplusplus
}
#endif

#endif
