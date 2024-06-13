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

#ifndef __ST_HAL_CONF_H__
#define __ST_HAL_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define HAL_MODULE_ENABLED
#define HAL_ADC_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_I2C_MODULE_ENABLED
#define HAL_RTC_MODULE_ENABLED
#define HAL_SPI_MODULE_ENABLED
#define HAL_TIM_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED
#define HAL_WWDG_MODULE_ENABLED

#define USE_HAL_ADC_REGISTER_CALLBACKS  1U
#define USE_HAL_DAC_REGISTER_CALLBACKS  1U
#define USE_HAL_I2C_REGISTER_CALLBACKS  1U
#define USE_HAL_RTC_REGISTER_CALLBACKS  1U
#define USE_HAL_SPI_REGISTER_CALLBACKS  1U
#define USE_HAL_TIM_REGISTER_CALLBACKS  1U
#define USE_HAL_UART_REGISTER_CALLBACKS 1U

/*============================ INCLUDES ======================================*/

#include "sthal_devices.h"

#ifdef HAL_GPIO_MODULE_ENABLED
#    include "sthal_gpio.h"
#endif

#ifdef HAL_DMA_MODULE_ENABLED
#    include "sthal_dma.h"
#endif

#ifdef HAL_ADC_MODULE_ENABLED
#    include "sthal_adc.h"
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

#ifdef HAL_WWDG_MODULE_ENABLED
#    include "sthal_wwdg.h"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
