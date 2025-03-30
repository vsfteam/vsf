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

/*============================ INCLUDES ======================================*/

#ifndef __ST_HAL_H__
#define __ST_HAL_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal.h"

#ifdef VSF_STHAL_CONF_HEADER
#   include VSF_STHAL_CONF_HEADER
#else
#   include "sthal_conf_template.h"
#endif

#include "sthal_def.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum {
    HAL_TICK_FREQ_10HZ    = 100U,
    HAL_TICK_FREQ_100HZ   = 10U,
    HAL_TICK_FREQ_1KHZ    = 1U,
    HAL_TICK_FREQ_DEFAULT = HAL_TICK_FREQ_1KHZ
} HAL_TickFreqTypeDef;

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

/*============================ GLOBAL VARIABLES ==============================*/

extern volatile uint32_t       uwTick;
extern HAL_TickFreqTypeDef uwTickFreq;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

HAL_StatusTypeDef   HAL_Init(void);
HAL_StatusTypeDef   HAL_DeInit(void);
void                HAL_MspInit(void);
void                HAL_MspDeInit(void);
HAL_StatusTypeDef   HAL_InitTick(uint32_t TickPriority);
void                HAL_IncTick(void);
void                HAL_Delay(uint32_t Delay);
uint32_t            HAL_GetTick(void);
HAL_StatusTypeDef   HAL_SetTickFreq(HAL_TickFreqTypeDef Freq);
HAL_TickFreqTypeDef HAL_GetTickFreq(void);
void                HAL_SuspendTick(void);
void                HAL_ResumeTick(void);
uint32_t            HAL_GetHalVersion(void);
uint32_t            HAL_GetUIDw2(void);

bool __sthal_check_timeout(uint32_t start, uint32_t timeout);

/*============================ INCLUDES ======================================*/

#ifdef HAL_GPIO_MODULE_ENABLED
#    include "sthal_gpio.h"
#endif

#ifdef HAL_DMA_MODULE_ENABLED
#    include "sthal_dma.h"
#endif

#ifdef HAL_ADC_MODULE_ENABLED
#    include "sthal_adc.h"
#endif

#ifdef HAL_EXTI_MODULE_ENABLED
#    include "sthal_exti.h"
#endif

#ifdef HAL_FLASH_MODULE_ENABLED
#    include "sthal_flash.h"
#endif

#ifdef HAL_I2C_MODULE_ENABLED
#    include "sthal_i2c.h"
#endif

#ifdef HAL_RTC_MODULE_ENABLED
#    include "sthal_rtc.h"
#endif

#ifdef HAL_SPI_MODULE_ENABLED
#    include "sthal_spi.h"
#endif

#ifdef HAL_TIM_MODULE_ENABLED
#    include "sthal_tim.h"
#endif

#ifdef HAL_UART_MODULE_ENABLED
#    include "sthal_uart.h"
#endif

#ifdef HAL_USART_MODULE_ENABLED
#    include "sthal_usart.h"
#endif

#ifdef HAL_IRDA_MODULE_ENABLED
#    include "sthal_irda.h"
#endif

#ifdef HAL_SMARTCARD_MODULE_ENABLED
#    include "sthal_smartcard.h"
#endif

#ifdef HAL_WWDG_MODULE_ENABLED
#    include "sthal_wwdg.h"
#endif

#ifdef __cplusplus
}
#endif

#endif
