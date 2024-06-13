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

#ifndef __ST_HAL_GPIO_H__
#define __ST_HAL_GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/

#include "sthal_def.h"

/*============================ MACROS ========================================*/

#define GPIO_PIN_0   ((uint16_t)0x0001)
#define GPIO_PIN_1   ((uint16_t)0x0002)
#define GPIO_PIN_2   ((uint16_t)0x0004)
#define GPIO_PIN_3   ((uint16_t)0x0008)
#define GPIO_PIN_4   ((uint16_t)0x0010)
#define GPIO_PIN_5   ((uint16_t)0x0020)
#define GPIO_PIN_6   ((uint16_t)0x0040)
#define GPIO_PIN_7   ((uint16_t)0x0080)
#define GPIO_PIN_8   ((uint16_t)0x0100)
#define GPIO_PIN_9   ((uint16_t)0x0200)
#define GPIO_PIN_10  ((uint16_t)0x0400)
#define GPIO_PIN_11  ((uint16_t)0x0800)
#define GPIO_PIN_12  ((uint16_t)0x1000)
#define GPIO_PIN_13  ((uint16_t)0x2000)
#define GPIO_PIN_14  ((uint16_t)0x4000)
#define GPIO_PIN_15  ((uint16_t)0x8000)
#define GPIO_PIN_All ((uint16_t)0xFFFF)

#define GPIO_PIN_MASK 0x0000FFFFU

#define GPIO_MODE_INPUT     MODE_INPUT
#define GPIO_MODE_OUTPUT_PP (MODE_OUTPUT | OUTPUT_PP)
#define GPIO_MODE_OUTPUT_OD (MODE_OUTPUT | OUTPUT_OD)
#define GPIO_MODE_AF_PP     (MODE_AF | OUTPUT_PP)
#define GPIO_MODE_AF_OD     (MODE_AF | OUTPUT_OD)

#define GPIO_MODE_ANALOG MODE_ANALOG

#define GPIO_MODE_IT_RISING  (MODE_INPUT | EXTI_IT | TRIGGER_RISING)
#define GPIO_MODE_IT_FALLING (MODE_INPUT | EXTI_IT | TRIGGER_FALLING)
#define GPIO_MODE_IT_RISING_FALLING                                            \
    (MODE_INPUT | EXTI_IT | TRIGGER_RISING | TRIGGER_FALLING)

#define GPIO_MODE_EVT_RISING  (MODE_INPUT | EXTI_EVT | TRIGGER_RISING)
#define GPIO_MODE_EVT_FALLING (MODE_INPUT | EXTI_EVT | TRIGGER_FALLING)
#define GPIO_MODE_EVT_RISING_FALLING                                           \
    (MODE_INPUT | EXTI_EVT | TRIGGER_RISING | TRIGGER_FALLING)

#define GPIO_SPEED_FREQ_LOW       0x00000000U
#define GPIO_SPEED_FREQ_MEDIUM    0x00000001U
#define GPIO_SPEED_FREQ_HIGH      0x00000002U
#define GPIO_SPEED_FREQ_VERY_HIGH 0x00000003U
#define GPIO_NOPULL               0x00000000U
#define GPIO_PULLUP               0x00000001U
#define GPIO_PULLDOWN             0x00000002U

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct {
    uint32_t Pin;
    uint32_t Mode;
    uint32_t Pull;
    uint32_t Speed;
    uint32_t Alternate;
} GPIO_InitTypeDef;

typedef enum {
    GPIO_PIN_RESET = 0,
    GPIO_PIN_SET
} GPIO_PinState;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

void          HAL_GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_Init);
void          HAL_GPIO_DeInit(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin);
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void          HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,
                                GPIO_PinState PinState);
void          HAL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
HAL_StatusTypeDef HAL_GPIO_LockPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void              HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin);
void              HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#ifdef __cplusplus
}
#endif

#endif
