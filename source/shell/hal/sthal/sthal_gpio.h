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

#define GPIO_MODE_INPUT     VSF_GPIO_INPUT
#define GPIO_MODE_OUTPUT_PP VSF_GPIO_OUTPUT_PUSH_PULL
#define GPIO_MODE_OUTPUT_OD VSF_GPIO_OUTPUT_OPEN_DRAIN

#define GPIO_MODE_AF_PP VSF_GPIO_AF_PUSH_PULL
#define GPIO_MODE_AF_OD VSF_GPIO_AF_OPEN_DRAIN

#define GPIO_MODE_ANALOG VSF_GPIO_ANALOG

#define GPIO_MODE_IT_RISING  (VSF_GPIO_EXTI | VSF_GPIO_EXTI_MODE_RISING)
#define GPIO_MODE_IT_FALLING (VSF_GPIO_EXTI | VSF_GPIO_EXTI_MODE_FALLING)
#define GPIO_MODE_IT_RISING_FALLING                                            \
    (VSF_GPIO_EXTI | VSF_GPIO_EXTI_MODE_RISING_FALLING)

#define GPIO_NOPULL   VSF_GPIO_NO_PULL_UP_DOWN
#define GPIO_PULLUP   VSF_GPIO_PULL_UP
#define GPIO_PULLDOWN VSF_GPIO_PULL_DOWN

#if defined(VSF_GPIO_SPEED_MASK)
#    if define VSF_GPIO_SPEED_LOW
#        define GPIO_SPEED_FREQ_LOW VSF_GPIO_SPEED_LOW
#    endif
#    if define VSF_GPIO_SPEED_MEDIUM
#        define GPIO_SPEED_FREQ_MEDIUM VSF_GPIO_SPEED_MEDIUM
#    endif
#    if define VSF_GPIO_SPEED_HIGH
#        define GPIO_SPEED_FREQ_HIGH VSF_GPIO_SPEED_HIGH
#    endif
#    if define VSF_GPIO_SPEED_VERY_HIGH
#        define GPIO_SPEED_FREQ_VERY_HIGH VSF_GPIO_SPEED_VERY_HIGH
#    endif
#endif

#ifndef GPIO_SPEED_FREQ_LOW
#    define GPIO_SPEED_FREQ_LOW 0
#endif
#ifndef GPIO_SPEED_FREQ_MEDIUM
#    define GPIO_SPEED_FREQ_MEDIUM 0
#endif
#ifndef GPIO_SPEED_FREQ_HIGH
#    define GPIO_SPEED_FREQ_HIGH 0
#endif
#ifndef GPIO_SPEED_FREQ_VERY_HIGH
#    define GPIO_SPEED_FGPIO_SPEED_FREQ_VERY_HIGHREQ_LOW 0
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef vsf_gpio_t GPIO_TypeDef;

typedef struct GPIO_InitTypeDef {
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
