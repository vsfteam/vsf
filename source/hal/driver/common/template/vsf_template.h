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

#ifndef __HAL_DRIVER_INTERFACE_H__
#define __HAL_DRIVER_INTERFACE_H__

/*============================ INCLUDES ======================================*/

#if (VSF_HAL_USE_GPIO != DISABLED) && !defined(__VSF_TEMPLATE_GPIO_H__)
#   include "./vsf_template_gpio.h"
#endif

#if (VSF_HAL_USE_ADC != DISABLED) && !defined(__VSF_TEMPLATE_ADC_H__)
#   include "./vsf_template_adc.h"
#endif

#if (VSF_HAL_USE_DAC != DISABLED) && !defined(__VSF_TEMPLATE_DAC_H__)
#   include "./vsf_template_dac.h"
#endif

#if (VSF_HAL_USE_DMA != DISABLED) && !defined(__VSF_TEMPLATE_DMA_H__)
#   include "./vsf_template_dma.h"
#endif

#if (VSF_HAL_USE_WDT != DISABLED) && !defined(__VSF_TEMPLATE_WDT_H__)
#   include "./vsf_template_wdt.h"
#endif

#if (VSF_HAL_USE_FLASH != DISABLED) && !defined(__VSF_TEMPLATE_FLASH_H__)
#   include "./vsf_template_flash.h"
#endif

#if (VSF_HAL_USE_I2C != DISABLED) && !defined(__VSF_TEMPLATE_I2C_H__)
#   include "./vsf_template_i2c.h"
#endif

#if (VSF_HAL_USE_I2S != DISABLED) && !defined(__VSF_TEMPLATE_I2S_H__)
#   include "./vsf_template_i2s.h"
#endif

#if (VSF_HAL_USE_SDIO != DISABLED) && !defined(__VSF_TEMPLATE_SDIO_H__)
#   include "./vsf_template_sdio.h"
#endif

#if (VSF_HAL_USE_PM != DISABLED) && !defined(__VSF_TEMPLATE_PM_H__)
#   include "./vsf_template_pm.h"
#endif

#if (VSF_HAL_USE_PWM != DISABLED) && !defined(__VSF_TEMPLATE_PWM_H__)
#   include "./vsf_template_pwm.h"
#endif

#if (VSF_HAL_USE_RNG != DISABLED) && !defined(__VSF_TEMPLATE_RNG_H__)
#   include "./vsf_template_rng.h"
#endif

#if (VSF_HAL_USE_RTC != DISABLED) && !defined(__VSF_TEMPLATE_RTC_H__)
#   include "./vsf_template_rtc.h"
#endif

#if (VSF_HAL_USE_SPI != DISABLED) && !defined(__VSF_TEMPLATE_SPI_H__)
#   include "./vsf_template_spi.h"
#endif

#if (VSF_HAL_USE_TIMER != DISABLED) && !defined(__VSF_TEMPLATE_TIMER_H__)
#   include "./vsf_template_timer.h"
#endif

#if (VSF_HAL_USE_USART != DISABLED) && !defined(__VSF_TEMPLATE_USART_H__)
#   include "./vsf_template_usart.h"
#endif

#if (VSF_HAL_USE_USB != DISABLED) && !defined(__VSF_TEMPLATE_USB_H__)
#   include "./vsf_template_usb.h"
#endif

#if (VSF_HAL_USE_ETH != DISABLED) && !defined(__VSF_TEMPLATE_ETH_H__)
#   include "./vsf_template_eth.h"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif /* __HAL_DRIVER_INTERFACE_H__ */
/* EOF */
